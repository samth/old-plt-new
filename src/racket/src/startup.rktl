;; This file is converted to [c]startup.inc and evaluated by
;; Racket's scheme_basic_env().

;; It implements, in a non-bootstrapping way, some functions
;; needed to start up Racket --- especially to install the
;; default module-name resolver.

;; Racket runs ((dynamic-require ''#%boot boot)) on startup. Then,
;; after configuring all startup parameters, Racket may run
;; ((dynamic-require ''#%boot seal)), and it may seal multiple
;; times. So, replace the content of this file to get a different set
;; of initial module definitions and parameter values.

;; When using makefiles, `make startup' in [the build directory for]
;; "plt/src/racket" creates "plt/src/racket/src/cstartup.inc", and
;; `make cstartup' creates plt/src/racket/src/cstartup.inc. Both
;; require a working Racket executable.

;; The recommend build strategy for cstartup.inc is
;;   * Set USE_COMPILED_STARTUP in schminc.h to 0
;;   * Modify startup.rkt to taste
;;   * Run make startup in <builddir>/racket
;;   * Run make in <builddir>/racket
;;   * Set USE_COMPILED_STARTUP in schminc.h to 1
;;   * Run make in <builddir>/racket

;; Do not use block comments (with #| and |#) in this file. The
;; script to build startup.inc can't handle them.

;; ------------------------------------------------------
;; Minimal syntax (no error checks!) needed for the rest

(module #%min-stx '#%kernel
  (#%require '#%paramz
             (for-syntax '#%kernel))

  (#%provide unless when
             and or
             cond
             let let* letrec
             parameterize
             define)

  (begin-for-syntax 
   (define-values (here-stx) (quote-syntax here)))

  (define-syntaxes (unless)
    (lambda (stx)
      (let-values ([(s) (syntax->list stx)])
        (datum->syntax here-stx
                       (list 'if (cadr s)
                             (void)
                             (cons 'begin (cddr s)))))))

  (define-syntaxes (when)
    (lambda (stx)
      (let-values ([(s) (syntax->list stx)])
        (datum->syntax here-stx
                       (list 'if (cadr s)
                             (cons 'begin (cddr s))
                             (void))))))

  (define-syntaxes (and)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (if (null? s)
            (quote-syntax #t)
            (if (null? (cdr s))
                (car s)
                (datum->syntax here-stx
                               (list 'if (car s) (cons 'and (cdr s)) #f)))))))

  (define-syntaxes (or)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (if (null? s)
            (quote-syntax #f)
            (if (null? (cdr s))
                (car s)
                (datum->syntax here-stx
                               (list 'let-values (list (list (list 'x)
                                                             (car s)))
                                     (list 'if 'x 'x (cons 'or (cdr s))))))))))

  (define-syntaxes (let)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (datum->syntax 
         here-stx
         (if (symbol? (syntax-e (car s)))
             (let-values ([(clauses)
                           (map (lambda (c)
                                  (syntax->list c))
                                (syntax->list (cadr s)))])
               (list 'letrec-values (list (list (list (car s))
                                                (list* 'lambda
                                                       (map car clauses)
                                                       (cddr s))))
                     (cons (car s) (map cadr clauses))))
             (list* 'let-values (map (lambda (c)
                                       (let-values ([(c) (syntax->list c)])
                                         (cons (list (car c))
                                               (cdr c))))
                                     (syntax->list (car s)))
                    (cdr s)))))))

  (define-syntaxes (letrec)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (datum->syntax 
         here-stx
         (list* 'letrec-values (map (lambda (c)
                                      (let-values ([(c) (syntax->list c)])
                                        (cons (list (car c))
                                              (cdr c))))
                                    (syntax->list (car s)))
                (cdr s))))))

  (define-syntaxes (let*)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (let-values ([(fst) (syntax->list (car s))])
          (datum->syntax 
           here-stx
           (if (null? fst)
               (list* 'let-values () (cdr s))
               (list 'let (list (car fst))
                     (list* 'let* (cdr fst) (cdr s)))))))))

  (define-syntaxes (parameterize)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (let-values ([(bindings) (apply append
                                        (map syntax->list (syntax->list (car s))))])
          (syntax-arm
           (datum->syntax 
            here-stx
            (list 'with-continuation-mark
                  'parameterization-key
                  (list* 'extend-parameterization
                         '(continuation-mark-set-first #f parameterization-key)
                         bindings)
                  (list* 'let-values ()
                         (cdr s)))))))))

  (define-syntaxes (cond)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (if (null? s)
            (quote-syntax (void))
            (datum->syntax 
             here-stx
             (let-values ([(a) (syntax->list (car s))])
               (if (eq? '=> (syntax-e (cadr a)))
                   (list 'let-values (list (list '(v) (car a)))
                         (list* 'cond
                                (list 'v (list (caddr a) 'v))
                                (cdr s)))
                   (list 'if (if (eq? (syntax-e (car a)) 'else)
                                 #t
                                 (car a))
                         (list* 'let-values '() (cdr a))
                         (cons 'cond (cdr s))))))))))
  
  (define-syntaxes (define)
    (lambda (stx)
      (let-values ([(s) (cdr (syntax->list stx))])
        (datum->syntax 
         here-stx
         (if (symbol? (syntax-e (car s)))
             (list 'define-values (list (car s)) (cadr s))
             (let-values ([(a) (syntax-e (car s))])
               (list 'define-values (list (car a))
                     (list* 'lambda (cdr a)
                            (cdr s))))))))))

;; ----------------------------------------
;; Utilities that are eventually exported to the rest of the world
;; (along with much of '#%kernel)

(module #%utils '#%kernel
  (#%require '#%min-stx '#%paramz)

  (#%provide path-string?
             normal-case-path
             path-replace-suffix
             path-add-suffix
             reroot-path
             find-col-file
             collection-path
             collection-file-path
             find-library-collection-paths
             path-list-string->path-list
             find-executable-path
             load/use-compiled
             embedded-load)

  (define-values (path-string?)
    (lambda (s)
      (or (path? s) 
          (and (string? s)
               (or (relative-path? s)
                   (absolute-path? s))))))

  (define-values (bsbs) (string #\u5C #\u5C))

  (define-values (normal-case-path)
    (lambda (s)
      (unless (or (path-for-some-system? s)
                  (path-string? s))
        (raise-argument-error 'normal-path-case "(or/c path-for-some-system? path-string?)" s))
      (cond
       [(if (path-for-some-system? s)
            (eq? (path-convention-type s) 'windows)
            (eq? (system-type) 'windows))
        (let ([str (if (string? s) s (bytes->string/locale (path->bytes s)))])
          (if (regexp-match? #rx"^[\u5C][\u5C][?][\u5C]" str)
              (if (string? s)
                  (string->path s)
                  s)
              (let ([s (string-locale-downcase str)])
                (bytes->path 
                 (string->bytes/locale
                  (regexp-replace* #rx"/" 
                                   (if (regexp-match? #rx"[/\u5C][. ]+[/\u5C]*$" s)
                                       ;; Just "." or ".." in last path element - don't remove
                                       s
                                       (regexp-replace* #rx"\u5B .\u5D+([/\u5C]*)$" s "\u005C1"))
                                   bsbs))
                 'windows))))]
       [(string? s) (string->path s)]
       [else s])))

  (define-values (reroot-path)
    (lambda (p root)
      (unless (or (path-string? p) (path-for-some-system? p))
        (raise-argument-error 'reroot-path "(or/c path-string? path-for-some-system?)" 0 p root))
      (unless (or (path-string? root) (path-for-some-system? root))
        (raise-argument-error 'reroot-path "(or/c path-string? path-for-some-system?)" 1 p root))
      (define conv (if (path-for-some-system? p)
                       (path-convention-type p)
                       (system-path-convention-type)))
      (unless (or (complete-path? p)
                  (eq? (system-path-convention-type) conv))
        (raise-arguments-error 'reroot-path
                               "path is not complete and not the platform's convention"
                               "path" p
                               "platform convention type" (system-path-convention-type)))
      (unless (eq? (if (path-for-some-system? root)
                       (path-convention-type root)
                       (system-path-convention-type))
                   conv)
        (raise-arguments-error 'reroot-path
                               "given paths use different conventions"
                               "path" p
                               "root path" root))
      (define c-p (normal-case-path (cleanse-path (if (complete-path? p)
                                                      p
                                                      (path->complete-path p)))))
      (define bstr (path->bytes c-p))
      (cond 
       [(eq? conv 'unix) 
        (if (bytes=? bstr #"/")
            (if (path-for-some-system? root)
                root
                (string->path root))
            (build-path root (bytes->path (subbytes (path->bytes c-p) 1) conv)))]
       [(eq? conv 'windows)
        (build-path
         root
         (bytes->path
          (cond
           ((regexp-match? #rx"^\\\\\\\\[?]\\\\[a-z]:" bstr)
            (bytes-append #"\\\\?\\REL\\" (subbytes bstr 4 5) #"\\" (subbytes bstr 6)))
           ((regexp-match? #rx"^\\\\\\\\[?]\\\\UNC\\\\" bstr)
            (bytes-append #"\\\\?\\REL\\" (subbytes bstr 4)))
           ((regexp-match? #rx"^\\\\\\\\[?]\\\\UNC\\\\" bstr)
            (bytes-append #"\\\\?\\REL\\" (subbytes bstr 4)))
           ((regexp-match? #rx"^\\\\\\\\" bstr)
            (bytes-append #"UNC\\" (subbytes bstr 2)))
           ((regexp-match? #rx"^[a-z]:" bstr)
            (bytes-append (subbytes bstr 0 1) (subbytes bstr 2))))
          conv))])))

  ;; ------------------------------ executable path ------------------------------

  (define-values (find-executable-path)
    (case-lambda 
     [(program libpath reverse?)
      (unless (path-string? program) 
	(raise-argument-error 'find-executable-path "path-string?" program))
      (unless (or (not libpath) (and (path-string? libpath) 
				     (relative-path? libpath)))
	(raise-argument-error 'find-executable-path "(or/c #f (and/c path-string? relative-path?))" libpath))
      (letrec ([found-exec
		(lambda (exec-name)
                  (if libpath
		      (let-values ([(base name isdir?) (split-path exec-name)])
			(let ([next
			       (lambda ()
				 (let ([resolved (resolve-path exec-name)])
				   (cond
				    [(equal? resolved exec-name) #f]
				    [(relative-path? resolved)
				     (found-exec (build-path base resolved))]
				    [else (found-exec resolved)])))])
			  (or (and reverse? (next))
			      (if (path? base)
				  (let ([lib (build-path base libpath)])
				    (and (or (directory-exists? lib) 
					     (file-exists? lib))
					 lib))
				  #f)
			      (and (not reverse?) (next)))))
		      exec-name))])
	(if (and (relative-path? program)
		 (let-values ([(base name dir?) (split-path program)])
		   (eq? base 'relative)))
	    (let ([paths-str (environment-variables-get #"PATH")]
		  [win-add (lambda (s) (if (eq? (system-type) 'windows) 
					   (cons (bytes->path #".") s) 
					   s))])
	      (let loop ([paths (win-add 
                                 (if paths-str
                                     (path-list-string->path-list (bytes->string/locale paths-str #\?)
                                                                  null)
                                     null))])
		(if (null? paths)
		    #f
		    (let* ([base (path->complete-path (car paths))]
			   [name (build-path base program)])
		      (if (file-exists? name)
			  (found-exec name)
			  (loop (cdr paths)))))))
	    (let ([p (path->complete-path program)])
	      (and (file-exists? p) (found-exec p)))))]
     [(program libpath) (find-executable-path program libpath #f)]
     [(program) (find-executable-path program #f #f)]))

  (define-values (path-list-string->path-list)
    (let ((r (byte-regexp (string->bytes/utf-8
			   (let ((sep (if (eq? (system-type) 'windows)
                                          ";"
                                          ":")))
			     (format "([^~a]*)~a(.*)" sep sep)))))
	  (cons-path (lambda (default s l) 
		       (if (bytes=? s #"")
			   (append default l)
                           (cons (bytes->path (if (eq? (system-type) 'windows)
                                                  (regexp-replace* #rx#"\"" s #"")
                                                  s))
                                 l)))))
      (lambda (s default)
	(unless (or (bytes? s)
		    (string? s))
	  (raise-argument-error 'path-list-string->path-list "(or/c bytes? string?)" s))
	(unless (and (list? default)
		     (andmap path? default))
	  (raise-argument-error 'path-list-string->path-list "(listof path?)" default))
	(let loop ([s (if (string? s)
			  (string->bytes/utf-8 s)
			  s)])
	  (let ([m (regexp-match r s)])
	    (if m
		(cons-path default (cadr m) (loop (caddr m)))
		(cons-path default s null)))))))

  ;; ------------------------------ Collections ------------------------------

  (define-values (-check-relpath)
    (lambda (who s)
      (unless (path-string? s)
        (raise-argument-error who "path-string?" s))
      (unless (relative-path? s)
        (raise-arguments-error who
                               "invalid relative path"
                               "path" s))))

  (define-values (-check-collection)
    (lambda (who collection collection-path)
      (-check-relpath who collection) 
      (for-each (lambda (p) (-check-relpath who p)) collection-path)))

  (define-values (-check-fail)
    (lambda (who fail)
      (unless (and (procedure? fail)
                   (procedure-arity-includes? fail 1))
        (raise-argument-error who "(any/c . -> . any)" fail))))

  (define-values (collection-path)
    (lambda (fail collection . collection-path) 
      (-check-collection 'collection-path collection collection-path)
      (-check-fail 'collection-path fail)
      (find-col-file fail
                     collection collection-path
                     #f)))

  (define-values (collection-file-path)
    (lambda (fail file-name collection . collection-path) 
      (-check-relpath 'collection-file-path file-name)
      (-check-collection 'collection-file-path collection collection-path)
      (-check-fail 'collection-file-path fail)
      (find-col-file fail
                     collection collection-path
                     file-name)))

  (define-values (user-links-path) (find-system-path 'links-file))
  (define-values (user-links-cache) (make-hasheq))
  (define-values (user-links-timestamp) -inf.0)

  (define-values (links-path) (find-links-path!
                               ;; This thunk is called once per place, and the result
                               ;; is remembered for later invocations. Otherwise, the
                               ;; search for the config file can trip over filesystem
                               ;; restrictions imposed by security guards.
                               (lambda ()
                                 (let ([d (let ([c (find-system-path 'collects-dir)])
                                            (if (absolute-path? c)
                                                c
                                                (parameterize ([current-directory (find-system-path 'orig-dir)])
                                                  (find-executable-path (find-system-path 'exec-file) c))))])
                                   (and d
                                        (build-path d "config" "links.rktd"))))))
  (define-values (links-cache) (make-hasheq))
  (define-values (links-timestamp) -inf.0)

  (define-values (get-linked-collections)
    (lambda (user?)
      (call/ec (lambda (esc)
                 (define-values (make-handler)
                   (lambda (ts)
                     (lambda (exn)
                       (if (exn:fail? exn)
                           (let ([l (current-logger)])
                             (when (log-level? l 'error)
                               (log-message l 'error 
                                            (format
                                             "error reading collection links file ~s: ~a"
                                             (if user? user-links-path links-path)
                                             (exn-message exn))
                                            (current-continuation-marks))))
                           (void))
                       (when ts
                         (if user?
                             (begin
                               (set! user-links-cache (make-hasheq))
                               (set! user-links-timestamp ts))
                             (begin
                               (set! links-cache (make-hasheq))
                               (set! links-timestamp ts))))
                       (if (exn:fail? exn)
                           (esc (make-hasheq))
                           ;; re-raise the exception (which is probably a break)
                           exn))))
                 (with-continuation-mark
                     exception-handler-key
                     (make-handler #f)
                   (let ([ts (file-or-directory-modify-seconds (if user?
                                                                   user-links-path
                                                                   links-path)
                                                               #f 
                                                               (lambda () -inf.0))])
                     (if (ts . > . (if user? user-links-timestamp links-timestamp))
                         (with-continuation-mark
                             exception-handler-key
                             (make-handler ts)
                           (parameterize ([read-case-sensitive #t]
                                          [read-square-bracket-as-paren #t]
                                          [read-curly-brace-as-paren #t]
                                          [read-accept-box #t]
                                          [read-accept-compiled #t]
                                          [read-accept-bar-quote #t]
                                          [read-accept-graph #t]
                                          [read-decimal-as-inexact #t]
                                          [read-accept-dot #t]
                                          [read-accept-infix-dot #t]
                                          [read-accept-quasiquote #t]
                                          [read-accept-reader #t]
                                          [read-accept-lang #f]
                                          [current-readtable #f])
                             (let ([v (let ([p (open-input-file (if user? user-links-path links-path)
                                                                'binary)])
                                        (dynamic-wind
                                            void
                                            (lambda () 
                                              (begin0
                                               (read p)
                                               (unless (eof-object? (read p))
                                                 (error "expected a single S-expression"))))
                                            (lambda () (close-input-port p))))])
                               (unless (and (list? v)
                                            (andmap (lambda (p)
                                                      (and (list? p)
                                                           (or (= 2 (length p))
                                                               (= 3 (length p)))
                                                           (or (string? (car p))
                                                               (eq? 'root (car p)))
                                                           (path-string? (cadr p))
                                                           (or (null? (cddr p))
                                                               (regexp? (caddr p)))))
                                                    v))
                                 (error "ill-formed content"))
                               (let ([ht (make-hasheq)]
                                     [dir (let-values ([(base name dir?) (split-path (if user?
                                                                                         user-links-path
                                                                                         links-path))])
                                            base)])
                                 (for-each
                                  (lambda (p)
                                    (when (or (null? (cddr p))
                                              (regexp-match? (caddr p) (version)))
                                      (let ([dir (simplify-path
                                                  (path->complete-path (cadr p) dir))])
                                        (if (symbol? (car p))
                                            ;; add to every table element (to keep
                                            ;; the choices in order); need a better
                                            ;; data structure
                                            (begin
                                              (unless (hash-ref ht #f #f)
                                                (hash-set! ht #f null))
                                              (hash-for-each
                                               ht
                                               (lambda (k v)
                                                 (hash-set! ht k (cons dir v)))))
                                            ;; single collection:
                                            (let ([s (string->symbol (car p))])
                                              (hash-set! ht s (cons (box dir)
                                                                    (hash-ref ht s null))))))))
                                  v)
                                 ;; reverse all lists:
                                 (hash-for-each
                                  ht
                                  (lambda (k v) (hash-set! ht k (reverse v))))
                                 ;; save table & timestamp:
                                 (if user?
                                     (begin
                                       (set! user-links-cache ht)
                                       (set! user-links-timestamp ts))
                                     (begin
                                       (set! links-cache ht)
                                       (set! links-timestamp ts)))
                                 ht))))
                         (if user?
                             user-links-cache
                             links-cache))))))))

  (define-values (normalize-collection-reference)
    (lambda (collection collection-path)
      ;; make sure that `collection' is a top-level collection name,
      (cond
       [(string? collection)
        (let ([m (regexp-match-positions #rx"/+" collection)])
          (if m
              (cond
               [(= (caar m) (sub1 (string-length collection)))
                (values (substring collection 0 (caar m)) collection-path)]
               [else
                (values (substring collection 0 (caar m))
                        (cons (substring collection (cdar m))
                              collection-path))])
              (values collection collection-path)))]
       [else
        (let-values ([(base name dir?) (split-path collection)])
          (if (eq? base 'relative)
              (values name collection-path)
              (normalize-collection-reference base (cons name collection-path))))])))

  (define-values (find-col-file)
    (lambda (fail collection collection-path file-name)
      (let-values ([(collection collection-path)
                    (normalize-collection-reference collection collection-path)])
        (let ([all-paths (let ([sym (string->symbol (if (path? collection)
                                                        (path->string collection)
                                                        collection))]
                               [links? (use-collection-link-paths)])
                           (append
                            ;; list of paths and (box path)s:
                            (if (and links? (use-user-specific-search-paths))
                                (let ([ht (get-linked-collections #t)])
                                  (or (hash-ref ht sym #f)
                                      (hash-ref ht #f null)))
                                null)
                            ;; list of paths and (box path)s:
                            (if (and links? links-path)
                                (let ([ht (get-linked-collections #f)])
                                  (or (hash-ref ht sym #f)
                                      (hash-ref ht #f null)))
                                null)
                            ;; list of paths:
                            (current-library-collection-paths)))])
          (define-values (done)
            (lambda (p)
              (if file-name (build-path p file-name) p)))
          (define-values (*build-path-rep)
            (lambda (p c)
              (if (path? p)
                  (build-path p c)
                  ;; box => from links table for c
                  (unbox p))))
          (define-values (*directory-exists?)
            (lambda (orig p)
              (if (path? orig)
                  (directory-exists? p)
                  ;; orig is box => from links table
                  #t)))
          (define-values (to-string) (lambda (p) (if (path? p) (path->string p) p)))
          (let cloop ([paths all-paths] [found-col #f])
            (if (null? paths)
                (if found-col
                    (done found-col)
                    (let ([rest-coll
                           (if (null? collection-path)
                               ""
                               (apply
                                string-append
                                (let loop ([cp collection-path])
                                  (if (null? (cdr cp))
                                      (list (to-string (car cp)))
                                      (list* (to-string (car cp)) "/" (loop (cdr cp)))))))])
                      (define-values (filter)
                        (lambda (f l)
                          (if (null? l)
                              null
                              (if (f (car l))
                                  (cons (car l) (filter f (cdr l)))
                                  (filter f (cdr l))))))
                      (fail
                       (format "collection not found\n  collection: ~s\n  in collection directories:~a~a" 
                               (if (null? collection-path)
                                   (to-string collection)
                                   (string-append (to-string collection) "/" rest-coll))
                               (apply
                                string-append
                                (map (lambda (p)
                                       (format "\n   ~a" p))
                                     (filter path? all-paths)))
                               (if (ormap box? all-paths)
                                   (format "\n   sub-collection: ~s\n  in parent directories:~a"
                                           rest-coll 
                                           (apply
                                            string-append
                                            (map (lambda (p)
                                                   (format "\n   ~a" (unbox p)))
                                                 (filter box? all-paths))))
                                   "")))))
                (let ([dir (*build-path-rep (car paths) collection)])
                  (if (*directory-exists? (car paths) dir)
                      (let ([cpath (apply build-path dir collection-path)])
                        (if (if (null? collection-path)
                                #t
                                (directory-exists? cpath))
                            (if file-name
                                (if (or (file-exists? (build-path cpath file-name))
                                        (let ([alt-file-name
                                               (let* ([file-name (if (path? file-name)
                                                                     (path->string file-name)
                                                                     file-name)]
                                                      [len (string-length file-name)])
                                                 (and (len . >= . 4)
                                                      (string=? ".rkt" (substring file-name (- len 4)))
                                                      (string-append (substring file-name 0 (- len 4)) ".ss")))])
                                          (and alt-file-name
                                               (file-exists? (build-path cpath alt-file-name)))))
                                    (done cpath)
                                    ;; Look further for specific file, but remember
                                    ;; first found directory
                                    (cloop (cdr paths) (or found-col cpath)))
                                ;; Just looking for dir; found it:
                                (done cpath))
                            ;; sub-collection not here; try next instance
                            ;; of the top-level collection
                            (cloop (cdr paths) found-col)))
                      (cloop (cdr paths) found-col)))))))))

  (define-values (check-suffix-call)
    (lambda (s sfx who)
      (unless (or (path-for-some-system? s)
                  (path-string? s))
        (raise-argument-error who "(or/c path-for-some-system? path-string?)" 0 s sfx))
      (unless (or (string? sfx) (bytes? sfx))
        (raise-argument-error who "(or/c string? bytes?)" 1 s sfx))
      (let-values ([(base name dir?) (split-path s)])
        (when (not base)
          (raise-mismatch-error who "cannot add a suffix to a root path: " s))
        (values base name))))
        

  (define-values (path-replace-suffix)
    (lambda (s sfx)
      (let-values ([(base name) (check-suffix-call s sfx 'path-replace-suffix)])
        (let ([new-name (bytes->path-element
                         (regexp-replace #rx#"(?:[.][^.]*|)$"
                                         (path-element->bytes name)
                                         (if (string? sfx)
                                             (string->bytes/locale sfx (char->integer #\?))
                                             sfx))
                         (if (path-for-some-system? s)
                             (path-convention-type s)
                             (system-path-convention-type)))])
          (if (path? base)
              (build-path base new-name)
              new-name)))))

  (define-values (path-add-suffix)
    (lambda (s sfx)
      (let-values ([(base name) (check-suffix-call s sfx 'path-add-suffix)])
        (let ([new-name (bytes->path-element
                         (bytes-append
                          (regexp-replace* #rx#"[.]"
                                           (path-element->bytes name)
                                           "_")
                          (if (string? sfx)
                              (string->bytes/locale sfx (char->integer #\?))
                              sfx))
                         (if (path-for-some-system? s)
                             (path-convention-type s)
                             (system-path-convention-type)))])
          (if (path? base)
              (build-path base new-name)
              new-name)))))

  (define-values (load/use-compiled)
    (lambda (f) ((current-load/use-compiled) f #f)))
    
  (define-values (find-library-collection-paths)
    (case-lambda
     [() (find-library-collection-paths null null)]
     [(extra-collects-dirs) (find-library-collection-paths extra-collects-dirs null)]
     [(extra-collects-dirs post-collects-dirs)
      (let ([user-too? (use-user-specific-search-paths)]
	    [cons-if (lambda (f r) (if f (cons f r) r))])
	(path-list-string->path-list
	 (if user-too?
             (let ([c (environment-variables-get #"PLTCOLLECTS")])
               (if c
                   (bytes->string/locale c #\?)
                   ""))
	     "")
	 (cons-if
	  (and user-too?
	       (build-path (find-system-path 'addon-dir)
			   (version)
			   "collects"))
	  (let loop ([l (append
			 extra-collects-dirs
			 (list (find-system-path 'collects-dir))
                         post-collects-dirs)])
	    (if (null? l)
		null
		(let* ([collects-path (car l)]
		       [v
			(cond
			 [(complete-path? collects-path) collects-path]
			 [(absolute-path? collects-path)
			  (path->complete-path collects-path
					       (find-executable-path (find-system-path 'exec-file) #f #t))]
			 [else
			  (find-executable-path (find-system-path 'exec-file) collects-path #t)])])
		  (if v
		      (cons (simplify-path (path->complete-path v (current-directory)))
			    (loop (cdr l)))
		      (loop (cdr l)))))))))]))

  ;; used for the -k command-line argument:
  (define (embedded-load start end str)
    (let* ([s (if str
                  str
                  (let* ([sp (find-system-path 'exec-file)] 
                         [exe (find-executable-path sp #f)]
                         [start (or (string->number start) 0)]
                         [end (or (string->number end) 0)])
                    (with-input-from-file exe 
                      (lambda ()
                        (file-position (current-input-port) start)
                        (read-bytes (max 0 (- end start)))))))]
           [p (open-input-bytes s)])
      (let loop ()
        (let ([e (parameterize ([read-accept-compiled #t]
                                [read-accept-reader #t]
                                [read-accept-lang #t])
                   (read p))])
          (unless (eof-object? e)
            (eval e)
            (loop)))))))


;; ----------------------------------------
;; When places are implemented by plain old threads,
;; place channels need to be shared across namespaces,
;; so `#%place-struct' is included in builtins

(module #%place-struct '#%kernel
  
  (define-values (struct:TH-place-channel TH-place-channel TH-place-channel? 
                  TH-place-channel-ref TH-place-channel-set!)
    (make-struct-type 'TH-place-channel #f 2 0 #f (list (cons prop:evt (lambda (x) (TH-place-channel-ref x 0))))))

  (define-values (TH-place-channel-in TH-place-channel-out) 
    (values
      (lambda (x) (TH-place-channel-ref x 0))
      (lambda (x) (TH-place-channel-ref x 1))))

 (#%provide 
    struct:TH-place-channel
    TH-place-channel 
    TH-place-channel? 
    TH-place-channel-in
    TH-place-channel-out))

;; ----------------------------------------
;; Handlers to install on startup

(module #%boot '#%kernel
  (#%require '#%min-stx '#%utils '#%paramz)

  (#%provide boot seal orig-paramz)

  (define-values (dll-suffix)
    (system-type 'so-suffix))
       
  (define-values (default-load/use-compiled)
    (let* ([resolve (lambda (s)
                      (if (complete-path? s)
                          s
                          (let ([d (current-load-relative-directory)])
                            (if d (path->complete-path s d) s))))]
           [date-of-1 (lambda (a)
                        (let ([v (file-or-directory-modify-seconds a #f (lambda () #f))])
                          (and v (cons a v))))]
           [date-of (lambda (a modes roots)
                      (ormap (lambda (root-dir)
                               (ormap
                                (lambda (compiled-dir)
                                  (let ([a (a root-dir compiled-dir)])
                                    (date-of-1 a)))
                                modes))
                             roots))]
           [date>=?
            (lambda (modes roots a bm)
              (and a
                   (let ([am (date-of a modes roots)])
                     (or (and (not bm) am) 
                         (and am bm (>= (cdr am) (cdr bm)) am)))))]
           [with-dir* (lambda (base t) 
                        (parameterize ([current-load-relative-directory 
                                        (if (path? base) 
                                            base 
                                            (current-directory))])
                          (t)))])
      (lambda (path expect-module)
        (unless (path-string? path)
          (raise-argument-error 'load/use-compiled "path-string?" path))
        (unless (or (not expect-module)
                    (symbol? expect-module)
                    (and (list? expect-module)
                         ((length expect-module) . > . 1)
                         (or (symbol? (car expect-module))
                             (not (car expect-module)))
                         (andmap symbol? (cdr expect-module))))
          (raise-argument-error 'load/use-compiled "(or/c #f symbol? (cons/c (or/c #f symbol?) (non-empty-listof symbol?)))" path))
        (define name (and expect-module (current-module-declare-name)))
        (define ns-hts (and name
                            (hash-ref -module-hash-table-table
                                      (namespace-module-registry (current-namespace))
                                      #f)))
        (define use-path/src (and ns-hts (hash-ref (cdr ns-hts) name #f)))
        (if use-path/src
            ;; Use previous decision of .zo vs. source:
            (parameterize ([current-module-declare-source (cadr use-path/src)])
              (with-dir* (caddr use-path/src)
                         (lambda () ((current-load) (car use-path/src) expect-module))))
            ;; Check .zo vs. src dates, etc.:
            (let*-values ([(orig-path) (resolve path)]
                          [(base orig-file dir?) (split-path path)]
                          [(file alt-file) (if expect-module
                                               (let* ([b (path->bytes orig-file)]
                                                      [len (bytes-length b)])
                                                 (cond
                                                  [(and (len . >= . 4)
                                                        (bytes=? #".rkt" (subbytes b (- len 4))))
                                                   ;; .rkt => try .rkt then .ss
                                                   (values orig-file
                                                           (bytes->path (bytes-append (subbytes b 0 (- len 4)) #".ss")))]
                                                  [else
                                                   ;; No search path
                                                   (values orig-file #f)]))
                                               (values orig-file #f))]
                          [(path) (if (eq? file orig-file)
                                      orig-path
                                      (build-path base file))]
                          [(alt-path) (and alt-file
                                           (if (eq? alt-file orig-file)
                                               orig-path
                                               (build-path base alt-file)))]
                          [(base) (if (eq? base 'relative) 'same base)]
                          [(modes) (use-compiled-file-paths)]
                          [(roots) (current-compiled-file-roots)]
                          [(reroot) (lambda (p d)
                                      (cond
                                       [(eq? d 'same) p]
                                       [(relative-path? d) (build-path p d)]
                                       [else (reroot-path p d)]))])
              (let* ([main-path-d (date-of-1 path)]
                     [alt-path-d (and alt-path 
                                      (not main-path-d)
                                      (date-of-1 alt-path))]
                     [path-d (or main-path-d alt-path-d)]
                     [get-so (lambda (file rep-sfx?)
                               (lambda (root-dir compiled-dir)
                                 (build-path (reroot base root-dir)
                                             compiled-dir
                                             "native"
                                             (system-library-subpath)
                                             (if rep-sfx?
                                                 (path-add-suffix
                                                  file
                                                  dll-suffix)
                                                 file))))]
                     [zo (lambda (root-dir compiled-dir)
                           (build-path (reroot base root-dir)
                                       compiled-dir
                                       (path-add-suffix file #".zo")))]
                     [alt-zo (lambda (root-dir compiled-dir)
                               (build-path (reroot base root-dir)
                                           compiled-dir
                                           (path-add-suffix alt-file #".zo")))]
                     [so (get-so file #t)]
                     [alt-so (get-so alt-file #t)]
                     [try-main? (or main-path-d (not alt-path-d))]
                     [try-alt? (and alt-file (or alt-path-d (not main-path-d)))]
                     [with-dir (lambda (t) (with-dir* base t))])
                (cond
                 [(and try-main?
                       (date>=? modes roots so path-d))
                  => (lambda (so-d)
                       (parameterize ([current-module-declare-source #f])
                         (with-dir (lambda () ((current-load-extension) (car so-d) expect-module)))))]
                 [(and try-alt?
                       (date>=? modes roots alt-so alt-path-d))
                  => (lambda (so-d)
                       (parameterize ([current-module-declare-source alt-path])
                         (with-dir (lambda () ((current-load-extension) (car so-d) expect-module)))))]
                 [(and try-main?
                       (date>=? modes roots zo path-d))
                  => (lambda (zo-d)
                       (register-zo-path name ns-hts (car zo-d) #f base)
                       (parameterize ([current-module-declare-source #f])
                         (with-dir (lambda () ((current-load) (car zo-d) expect-module)))))]
                 [(and try-alt?
                       (date>=? modes roots alt-zo path-d))
                  => (lambda (zo-d)
                       (register-zo-path name ns-hts (car zo-d) alt-path base)
                       (parameterize ([current-module-declare-source alt-path])
                         (with-dir (lambda () ((current-load) (car zo-d) expect-module)))))]
                 [(or (not (pair? expect-module))
                      (car expect-module))
                  (let ([p (if try-main? path alt-path)])
                    ;; "quiet" failure when asking for a submodule:
                    (unless (and (pair? expect-module)
                                 (not (file-exists? p)))
                      (parameterize ([current-module-declare-source (and expect-module 
                                                                         (not try-main?)
                                                                         p)])
                        (with-dir (lambda () ((current-load) p expect-module))))))])))))))

  (define (register-zo-path name ns-hts path src-path base)
    (when ns-hts
      (hash-set! (cdr ns-hts) name (list path src-path base))))

  (define-values (default-reader-guard)
    (lambda (path) path))

  (define-values (-module-hash-table-table) (make-weak-hasheq)) ; weak map from namespace to pair of module-name hts

  ;; weak map from `lib' path + corrent-library-paths to symbols:
  ;;  We'd like to use a weak `equal?'-based hash table here,
  ;;  but that's not kill-safe. Instead, we use a non-thread-safe
  ;;  custom hash table; a race could lose cache entries, but
  ;;  that's ok.
  (define CACHE-N 512)
  (define-values (-path-cache) (make-vector CACHE-N #f)) 
  (define (path-cache-get p)
    (let* ([i (modulo (abs (equal-hash-code p)) CACHE-N)]
           [w (vector-ref -path-cache i)]
           [l (and w (weak-box-value w))])
      (and l
           (let ([a (assoc p l)])
             (and a (cdr a))))))
  (define (path-cache-set! p v)
    (let* ([i (modulo (abs (equal-hash-code p)) CACHE-N)]
           [w (vector-ref -path-cache i)]
           [l (and w (weak-box-value w))])
      (vector-set! -path-cache i (make-weak-box (cons (cons p v) (or l null))))))
  
  (define-values (-loading-filename) (gensym))
  (define-values (-loading-prompt-tag) (make-continuation-prompt-tag 'module-loading))
  (define-values (-prev-relto) #f)
  (define-values (-prev-relto-dir) #f)

  (define (split-relative-string s coll-mode?)
    (let ([l (let loop ([s s])
               (cond
                [(regexp-match #rx"^(.*?)/(.*)$" s)
                 => (lambda (m)
                      (cons (cadr m)
                            (loop (caddr m))))]
                [else (list s)]))])
      (if coll-mode?
          l
          (let loop ([l l])
            (if (null? (cdr l))
                (values null (car l))
                (let-values ([(c f) (loop (cdr l))])
                  (values (cons (car l) c) f)))))))

  (define-values (orig-paramz) #f)

  (define-values (standard-module-name-resolver)
    (let-values ()
      (define-values (planet-resolver) #f)
      (define-values (prep-planet-resolver!)
        (lambda ()
          (unless planet-resolver
            (with-continuation-mark
                parameterization-key
                orig-paramz
              (set! planet-resolver (dynamic-require '(lib "planet/resolver.rkt") 'planet-module-name-resolver))))))
      (define-values (standard-module-name-resolver)
        (case-lambda 
         [(s from-namespace) 
          (unless (resolved-module-path? s)
            (raise-argument-error 'standard-module-name-resolver
                                  "resolved-module-path?"
                                  s))
          (unless (or (not from-namespace) (namespace? from-namespace))
            (raise-argument-error 'standard-module-name-resolver
                                  "(or/c #f namespace?)"
                                  from-namespace))
          (when planet-resolver
            ;; Let planet resolver register, too:
            (planet-resolver s))
          ;; Register s as loaded:
          (let ([hts (or (hash-ref -module-hash-table-table
                                   (namespace-module-registry (current-namespace))
                                   #f)
                         (let ([hts (cons (make-hasheq) (make-hasheq))])
                           (hash-set! -module-hash-table-table
                                      (namespace-module-registry (current-namespace))
                                      hts)
                           hts))])
            (hash-set! (car hts) s 'declared)
            ;; If attach from another namespace, copy over source-file path, if any:
            (when from-namespace
              (let ([root-name (if (pair? (resolved-module-path-name s))
                                   (make-resolved-module-path (car (resolved-module-path-name s)))
                                   s)]
                    [from-hts (hash-ref -module-hash-table-table
                                        (namespace-module-registry from-namespace)
                                        #f)])
                (when from-hts
                  (let ([use-path/src (hash-ref (cdr from-hts) root-name #f)])
                    (when use-path/src
                      (hash-set! (cdr hts) root-name use-path/src)))))))]
         [(s relto stx) ; for backward-compatibility
          (standard-module-name-resolver s relto stx #t)]          
         [(s relto stx load?)
          ;; If stx is not #f, raise syntax error for ill-formed paths
          (unless (module-path? s)
            (if stx
                (raise-syntax-error #f
                                    "bad module path"
                                    stx)
                (raise-argument-error 'standard-module-name-resolver
                                      "module-path?"
                                      s)))
          (define (flatten-sub-path base orig-l)
            (let loop ([a null] [l orig-l])
              (cond
               [(null? l) (if (null? a)
                              base
                              (cons base (reverse a)))]
               [(equal? (car l) "..")
                (if (null? a)
                    (error
                     'standard-module-name-resolver
                     "too many \"..\"s in submodule path: ~.s"
                     (list* 'submod
                            (if (equal? base ".") 
                                base 
                                (if (path? base)
                                    base
                                    (list (if (symbol? base) 'quote 'file) base)))
                            orig-l))
                    (loop (cdr a) (cdr l)))]
               [else (loop (cons (car l) a) (cdr l))])))
          (cond
           [(and (pair? s) (eq? (car s) 'quote))
            (make-resolved-module-path (cadr s))]
           [(and (pair? s) (eq? (car s) 'submod)
                 (pair? (cadr s)) (eq? (caadr s) 'quote))
            (make-resolved-module-path (flatten-sub-path (cadadr s) (cddr s)))]
           [(and (pair? s) (eq? (car s) 'submod)
                 (or (equal? (cadr s) ".")
                     (equal? (cadr s) ".."))
                 (and relto
                      (let ([p (resolved-module-path-name relto)])
                        (or (symbol? p)
                            (and (pair? p) (symbol? (car p)))))))
            (define rp (resolved-module-path-name relto))
            (make-resolved-module-path (flatten-sub-path (if (pair? rp) (car rp) rp)
                                                         (let ([r (if (equal? (cadr s) "..")
                                                                      (cdr s)
                                                                      (cddr s))])
                                                           (if (pair? rp)
                                                               (append (cdr rp) r)
                                                               r))))]
           [(and (pair? s) (eq? (car s) 'planet))
            (prep-planet-resolver!)
            (planet-resolver s relto stx load? #f orig-paramz)]
           [(and (pair? s)
                 (eq? (car s) 'submod)
                 (pair? (cadr s))
                 (eq? (caadr s) 'planet))
            (prep-planet-resolver!)
            (planet-resolver (cadr s) relto stx load? (cddr s) orig-paramz)]
           [else
            (let ([get-dir (lambda ()
                             (or (and relto
                                      (if (eq? relto -prev-relto)
                                          -prev-relto-dir
                                          (let ([p (resolved-module-path-name relto)])
                                            (let ([p (if (pair? p) (car p) p)])
                                              (and (path? p)
                                                   (let-values ([(base n d?) (split-path p)])
                                                     (set! -prev-relto relto)
                                                     (set! -prev-relto-dir base)
                                                     base))))))
                                 (current-load-relative-directory)
                                 (current-directory)))]
                  [show-collection-err (lambda (s)
                                         (let ([s (string-append "standard-module-name-resolver: " s)])
                                           (if stx
                                               (raise-syntax-error
                                                #f
                                                s
                                                stx)
                                               (error s))))]
                  [ss->rkt (lambda (s)
                             (let ([len (string-length s)])
                               (if (and (len . >= . 3)
                                        (string=? ".ss" (substring s (- len 3))))
                                   (string-append (substring s 0 (- len 3)) ".rkt")
                                   s)))]
                  [path-ss->rkt (lambda (p)
                                  (let-values ([(base name dir?) (split-path p)])
                                    (if (regexp-match #rx"[.]ss$" (path->bytes name))
                                        (path-replace-suffix p #".rkt")
                                        p)))]
                  [s (if (and (pair? s) (eq? 'submod (car s)))
                         (let ([v (cadr s)])
                           (if (or (equal? v ".")
                                   (equal? v ".."))
                               (if relto
                                   ;; must have a path inside, or we wouldn't get here
                                   (let ([p (resolved-module-path-name relto)])
                                     (if (pair? p)
                                         (car p)
                                         p))
                                   (error 'standard-module-name-resolver
                                          "no base path for relative submodule path: ~.s"
                                          s))
                               v))
                         s)]
                  [subm-path (if (and (pair? s) (eq? 'submod (car s)))
                                 (let ([p (if (and (or (equal? (cadr s) ".")
                                                       (equal? (cadr s) ".."))
                                                   relto)
                                              (let ([p (resolved-module-path-name relto)]
                                                    [r (if (equal? (cadr s) "..")
                                                           (cdr s)
                                                           (cddr s))])
                                                (if (pair? p)
                                                    (flatten-sub-path (car p) (append (cdr p) r))
                                                    (flatten-sub-path p r)))
                                              (flatten-sub-path "." 
                                                                (if (equal? (cadr s) "..")
                                                                    (cdr s)
                                                                    (cddr s))))])
                                   ;; flattening may erase the submodule path:
                                   (if (pair? p)
                                       (cdr p)
                                       #f))
                                 #f)])
              (let ([s-parsed
                     ;; Non-string result represents an error
                     (cond
                      [(symbol? s)
                       (or (path-cache-get (cons s (current-library-collection-paths)))
                           (let-values ([(cols file) (split-relative-string (symbol->string s) #f)])
                             (let* ([f-file (if (null? cols)
                                                "main.rkt"
                                                (string-append file ".rkt"))])
                               (find-col-file show-collection-err
                                              (if (null? cols) file (car cols))
                                              (if (null? cols) null (cdr cols))
                                              f-file))))]
                      [(string? s)
                       (let* ([dir (get-dir)])
                         (or (path-cache-get (cons s dir))
                             (let-values ([(cols file) (split-relative-string s #f)])
                               (apply build-path 
                                      dir
                                      (append
                                       (map (lambda (s)
                                              (cond
                                               [(string=? s ".") 'same]
                                               [(string=? s "..") 'up]
                                               [else s]))
                                            cols)
                                       (list (ss->rkt file)))))))]
                      [(path? s) 
                       ;; Use filesystem-sensitive `simplify-path' here:
                       (path-ss->rkt (simplify-path (if (complete-path? s)
                                                        s
                                                        (path->complete-path s (get-dir)))))]
                      [(eq? (car s) 'lib)
                       (or (path-cache-get (cons s (current-library-collection-paths)))
                           (let*-values ([(cols file) (split-relative-string (cadr s) #f)]
                                         [(old-style?) (if (null? (cddr s))
                                                           (and (null? cols)
                                                                (regexp-match? #rx"[.]" file))
                                                           #t)])
                             (let* ([f-file (if old-style?
                                                (ss->rkt file)
                                                (if (null? cols)
                                                    "main.rkt"
                                                    (if (regexp-match? #rx"[.]" file)
                                                        (ss->rkt file)
                                                        (string-append file ".rkt"))))])
                               (let-values ([(cols)
                                             (if old-style?
                                                 (append (if (null? (cddr s))
                                                             '("mzlib")
                                                             (apply append
                                                                    (map (lambda (p)
                                                                           (split-relative-string p #t))
                                                                         (cddr s))))
                                                         cols)
                                                 (if (null? cols)
                                                     (list file)
                                                     cols))])
                                 (find-col-file show-collection-err
                                                (car cols)
                                                (cdr cols)
                                                f-file)))))]
                      [(eq? (car s) 'file)
                       ;; Use filesystem-sensitive `simplify-path' here:
                       (path-ss->rkt 
                        (simplify-path (path->complete-path (expand-user-path (cadr s)) (get-dir))))])])
                (unless (or (path? s-parsed)
                            (vector? s-parsed))
                  (if stx
                      (raise-syntax-error
                       'require
                       (format "bad module path~a" (if s-parsed
                                                       (car s-parsed)
                                                       ""))
                       stx)
                      (raise-argument-error 
                       'standard-module-name-resolver
                       "module-path?"
                       s)))
                ;; At this point, s-parsed is a complete path (or a cached vector)
                (let* ([filename (if (vector? s-parsed)
                                     (vector-ref s-parsed 0)
                                     (simplify-path (cleanse-path s-parsed) #f))]
                       [normal-filename (if (vector? s-parsed)
                                            (vector-ref s-parsed 1)
                                            (normal-case-path filename))])
                  (let-values ([(base name dir?) (if (vector? s-parsed)
                                                     (values 'ignored (vector-ref s-parsed 2) 'ignored)
                                                     (split-path filename))])
                    (let* ([no-sfx (if (vector? s-parsed)
                                       (vector-ref s-parsed 3)
                                       (path-replace-suffix name #""))])
                      (let* ([root-modname (if (vector? s-parsed)
                                               (vector-ref s-parsed 4)
                                               (make-resolved-module-path filename))]
                             [hts (or (hash-ref -module-hash-table-table
                                                (namespace-module-registry (current-namespace))
                                                #f)
                                      (let ([hts (cons (make-hasheq) (make-hasheq))])
                                        (hash-set! -module-hash-table-table
                                                   (namespace-module-registry (current-namespace))
                                                   hts)
                                        hts))]
                             [modname (if subm-path
                                          (make-resolved-module-path 
                                           (cons (resolved-module-path-name root-modname)
                                                 subm-path))
                                          root-modname)])
                        ;; Loaded already?
                        (when load?
                          (let ([got (hash-ref (car hts) modname #f)])
                            (unless got
                              ;; Currently loading?
                              (let ([loading
                                     (let ([tag (if (continuation-prompt-available? -loading-prompt-tag)
                                                    -loading-prompt-tag
                                                    (default-continuation-prompt-tag))])
                                       (continuation-mark-set-first
                                        #f
                                        -loading-filename
                                        null
                                        tag))]
                                    [nsr (namespace-module-registry (current-namespace))])
                                (for-each
                                 (lambda (s)
                                   (when (and (equal? (cdr s) normal-filename)
                                              (eq? (car s) nsr))
                                     (error
                                      'standard-module-name-resolver
                                      "cycle in loading\n  at path: ~a\n  paths:~a"
                                      filename
                                      (apply string-append
                                             (let loop ([l (reverse loading)])
                                               (if (null? l)
                                                   '()
                                                   (list* "\n   " (path->string (cdar l)) (loop (cdr l)))))))))
                                 loading)
                                ((if (continuation-prompt-available? -loading-prompt-tag)
                                     (lambda (f) (f))
                                     (lambda (f) (call-with-continuation-prompt f -loading-prompt-tag)))
                                 (lambda ()
                                   (with-continuation-mark -loading-filename (cons (cons 
                                                                                    (namespace-module-registry (current-namespace))
                                                                                    normal-filename)
                                                                                   loading)
                                     (parameterize ([current-module-declare-name root-modname])
                                       ((current-load/use-compiled) 
                                        filename 
                                        (let ([sym (string->symbol (path->string no-sfx))])
                                          (if subm-path
                                              (if (hash-ref (car hts) root-modname #f)
                                                  ;; Root is already loaded, so only use .zo
                                                  (cons #f subm-path)
                                                  ;; Root isn't loaded, so it's ok to load form source:
                                                  (cons sym subm-path))
                                              sym)))))))))))
                        ;; If a `lib' path, cache pathname manipulations
                        (when (and (not (vector? s-parsed))
                                   (or (string? s)
                                       (symbol? s)
                                       (and (pair? s)
                                            (eq? (car s) 'lib))))
                          (path-cache-set! (if (string? s)
                                               (cons s (get-dir))
                                               (cons s (current-library-collection-paths)))
                                           (vector filename
                                                   normal-filename
                                                   name
                                                   no-sfx
                                                   root-modname)))
                        ;; Result is the module name:
                        modname))))))])]))
      standard-module-name-resolver))

  (define-values (boot)
    (lambda ()
      (seal)
      (current-module-name-resolver standard-module-name-resolver)
      (current-load/use-compiled default-load/use-compiled)
      (current-reader-guard default-reader-guard)))

  (define-values (seal)
    (lambda ()
      (set! orig-paramz
            (reparameterize 
             (continuation-mark-set-first #f parameterization-key))))))

;; ----------------------------------------
;; A module that collects all the built-in modules,
;;  so that it's easier to keep them attached in new
;;  namespaces.

(module #%builtin '#%kernel
  (#%require '#%expobs
             (only '#%foreign)  ; so it's attached, but doesn't depend on any exports
             (only '#%unsafe)   ; ditto
             (only '#%flfxnum)  ; ditto
             '#%boot
             '#%place-struct
             '#%paramz
             '#%network
             '#%utils
             (only '#%place)
             (only '#%futures)))

