
(module eval mzscheme
  (require "manual.ss"
           "struct.ss"
           "scheme.ss"
           "decode.ss"
           (lib "class.ss")
           (lib "file.ss")
           (lib "string.ss"))

  (provide interaction
           interaction-eval
           interaction-eval-show
           schemeblock+eval
           schememod+eval
           def+int
           defs+int
           examples
           defexamples
           as-examples

           current-int-namespace
           eval-example-string

           scribble-eval-handler)

  (define current-int-namespace (make-parameter (make-namespace)))
  (define scribble-eval-handler (make-parameter (lambda (c? x) (eval x))))

  (define image-counter 0)

  (define maxlen 60)

  (define (interleave title expr-paras val-list+outputs)
    (make-table
     #f
     (append
      (if title (list (list title)) null)
      (let loop ([expr-paras expr-paras]
                 [val-list+outputs val-list+outputs]
                 [first? #t])
        (if (null? expr-paras)
            null
            (append
             (list (list (let ([p (car expr-paras)])
                           (if (flow? p)
                               p
                               (make-flow (list p))))))
             (append
              (if (string? (car val-list+outputs))
                  (map
                   (lambda (s)
                     (list (make-flow (list (make-paragraph
                                             (list
                                              (hspace 2)
                                              (span-class "schemeerror"
                                                          (italic s))))))))
                   (let sloop ([s (car val-list+outputs)])
                     (if ((string-length s) . > . maxlen)
                         ;; break the error message into multiple lines:
                         (let loop ([pos (sub1 maxlen)])
                           (cond
                            [(zero? pos) (cons (substring s 0 maxlen)
                                               (sloop (substring s maxlen)))]
                            [(char-whitespace? (string-ref s pos))
                             (cons (substring s 0 pos)
                                   (sloop (substring s (add1 pos))))]
                            [else (loop (sub1 pos))]))
                         (list s))))
                  (append
                   (if (string=? "" (cdar val-list+outputs))
                       null
                       (list
                        (list
                         (make-flow 
                          (list
                           (let ([s (regexp-split #rx"\n"
                                                  (regexp-replace #rx"\n$"
                                                                  (cdar val-list+outputs)
                                                                  ""))])
                             (if (= 1 (length s))
                                 (make-paragraph
                                  (list
                                   (hspace 2)
                                   (span-class "schemestdout" (car s))))
                                 (make-table
                                  #f
                                  (map (lambda (s)
                                         (list (make-flow (list (make-paragraph
                                                                 (list
                                                                  (hspace 2)
                                                                  (span-class "schemestdout" s)))))))
                                       s)))))))))
                   (let ([val-list (caar val-list+outputs)])
                     (if (equal? val-list (list (void)))
                         null
                         (map (lambda (v)
                                (list (make-flow (list (make-paragraph 
                                                        (list
                                                         (hspace 2)
                                                         (span-class "schemeresult"
                                                                     (to-element/no-color v))))))))
                              val-list)))))
              (loop (cdr expr-paras)
                    (cdr val-list+outputs)
                    #f))))))))

  (define (do-eval s)
    (cond
     [(and (list? s)
           (eq? 'code:line (car s))
           (= (length s) 3)
           (list? (caddr s))
           (eq? 'code:comment (caaddr s)))
      (do-eval (cadr s))]
     [(and (list? s)
           (eq? 'eval:alts (car s))
           (= (length s) 3))
      (do-eval (caddr s))]
     [else
      (let ([o (open-output-string)])
        (parameterize ([current-output-port o])
          (with-handlers ([exn? (lambda (e)
                                  (exn-message e))])
            (cons (let ([v (do-plain-eval s #t)])
                    (copy-value v (make-hash-table)))
                  (get-output-string o)))))]))

  (define (install ht v v2)
    (hash-table-put! ht v v2)
    v2)

  ;; Since we evaluate everything in an interaction before we typeset,
  ;;  copy each value to avoid side-effects.
  (define (copy-value v ht)
    (cond
     [(and v (hash-table-get ht v #f))
      => (lambda (v) v)]
     [(string? v) (install ht v (string-copy v))]
     [(bytes? v) (install ht v (bytes-copy v))]
     [(pair? v) (let ([p (cons #f #f)])
                  (hash-table-put! ht v p)
                  (set-car! p (copy-value (car v) ht))
                  (set-cdr! p (copy-value (cdr v) ht))
                  p)]
     [(vector? v) (let ([v2 (make-vector (vector-length v))])
                    (hash-table-put! ht v v2)
                    (let loop ([i (vector-length v2)])
                      (unless (zero? i)
                        (let ([i (sub1 i)])
                          (vector-set! v2 i (copy-value (vector-ref v i) ht))
                          (loop i))))
                    v2)]
     [(box? v) (let ([v2 (box #f)])
                 (hash-table-put! ht v v2)
                 (set-box! v2 (copy-value (unbox v) ht))
                 v2)]
     [else v]))
            
  (define (strip-comments s)
    (cond
     [(and (pair? s)
           (pair? (car s))
           (eq? (caar s) 'code:comment))
      (strip-comments (cdr s))]
     [(pair? s)
      (cons (strip-comments (car s))
            (strip-comments (cdr s)))]
     [(eq? s 'code:blank) (void)]
     [else s]))
      

  (define (do-plain-eval s catching-exns?)
    (parameterize ([current-namespace (current-int-namespace)])
        (call-with-values (lambda () ((scribble-eval-handler) catching-exns? (strip-comments s))) list)))

  (define-syntax interaction-eval
    (syntax-rules ()
      [(_ e) (#%expression
              (begin (parameterize ([current-command-line-arguments #()])
                       (do-plain-eval (quote e) #f))
                     ""))]))


  (define (show-val v)
    (span-class "schemeresult"
                (to-element/no-color v)))

  (define-syntax interaction-eval-show
    (syntax-rules ()
      [(_ e) (#%expression
              (parameterize ([current-command-line-arguments #()])
                (show-val (car (do-plain-eval (quote e) #f)))))]))

  (define (eval-example-string s)
    (eval (read (open-input-string s))))

  (parameterize ([current-namespace (current-int-namespace)])
    (eval `(define eval-example-string ,eval-example-string)))

  (define-syntax schemeinput*
    (syntax-rules (eval-example-string eval:alts)
      [(_ (eval-example-string s))
       (make-paragraph
        (list
         (hspace 2)
         (tt "> ")
         (span-class "schemevalue" (schemefont s))))]
      [(_ (eval:alts a b)) (schemeinput* a)]
      [(_ e) (schemeinput e)]))

  (define (defspace p)
    (make-flow (list p
                     (make-paragraph null))))

  (define-syntax (schemedefinput* stx)
    (syntax-case stx (eval-example-string define define-struct)
      [(_ (eval-example-string s))
       #'(schemeinput* (eval-example-string s))]
      [(_ (define . rest))
       (syntax-case stx ()
         [(_ e) #'(defspace (schemeblock e))])]
      [(_ (define-struct . rest))
       (syntax-case stx ()
         [(_ e) #'(defspace (schemeblock e))])]
      [(_ (code:line (define . rest) . rest2))
       (syntax-case stx ()
         [(_ e) #'(defspace (schemeblock e))])]
      [(_ e) #'(schemeinput e)]))

  (define-syntax titled-interaction
    (syntax-rules ()
      [(_ t schemeinput* e ...)
       (interleave t
                   (list (schemeinput* e) ...)
                   (map do-eval (list (quote e) ...)))]))

    (define-syntax interaction
      (syntax-rules ()
        [(_ e ...) (titled-interaction #f schemeinput* e ...)]))

  (define-syntax schemeblock+eval
    (syntax-rules ()
      [(_ e ...)
       (#%expression
        (begin (interaction-eval e) ...
               (schemeblock e ...)))]))

  (define-syntax schememod+eval
    (syntax-rules ()
      [(_ name e ...)
       (#%expression
        (begin (interaction-eval e) ...
               (schememod name e ...)))]))

  (define-syntax def+int
    (syntax-rules ()
      [(_ def e ...)
       (make-splice (list (schemeblock+eval def)
                          (interaction e ...)))]))

  (define-syntax defs+int
    (syntax-rules ()
      [(_ [def ...] e ...)
       (make-splice (list (schemeblock+eval def ...)
                          (interaction e ...)))]))

  (define example-title
    (make-flow (list (make-paragraph (list "Examples:")))))
  (define-syntax examples
    (syntax-rules ()
      [(_ e ...)
       (titled-interaction example-title schemeinput* e ...)]))
  (define-syntax defexamples
    (syntax-rules ()
      [(_ e ...)
       (titled-interaction example-title schemedefinput* e ...)]))

  (define (as-examples t)
    (make-table #f
                (list
                 (list example-title)
                 (list (make-flow (list t)))))))

