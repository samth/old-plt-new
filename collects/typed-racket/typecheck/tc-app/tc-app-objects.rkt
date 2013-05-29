#lang racket/unit

(require "../../utils/utils.rkt"
         "signatures.rkt"
         "utils.rkt"
         syntax/parse syntax/stx racket/match unstable/sequence unstable/syntax
         syntax/parse/experimental/reflect racket/dict
         (typecheck signatures tc-funapp)
         (types abbrev union utils)
         (rep type-rep)
         (utils tc-utils)

         (for-template racket/base))


(import tc-expr^)
(export tc-app-objects^)


(define-tc/app-syntax-class (tc/app-objects expected)
  #:literals (#%plain-app list cons quote)
  (pattern (dmo b cl
            (#%plain-app list . pos-args)
            (#%plain-app list (#%plain-app cons (quote names) named-args) ...))
     #:declare dmo (id-from 'do-make-object 'racket/private/class-internal)
     (check-do-make-object #'b #'cl #'pos-args #'(names ...) #'(named-args ...)))
  (pattern (dmo . args)
     #:declare dmo (id-from 'do-make-object 'racket/private/class-internal)
     (int-err "unexpected arguments to do-make-object")))

;; do-make-object now takes blame as its first argument, which isn't checked
;; (it's just an s-expression)
(define (check-do-make-object b cl pos-args names named-args)
  (let* ([names (stx-map syntax-e names)]
         [name-assoc (stx-map cons names named-args)])
    (let loop ([t (tc-expr cl)])
      (match t
        [(tc-result1: (? Mu? t*)) (loop (ret (unfold t*)))]
        [(tc-result1: (Union: '())) (ret (Un))]
        [(tc-result1: (and c (Class: pos-tys (list (and tnflds (list tnames _ _)) ...) _)))
         (unless (= (length pos-tys)
                    (syntax-length pos-args))
           (tc-error/delayed "expected ~a positional arguments, but got ~a"
                             (length pos-tys) (syntax-length pos-args)))
         ;; use for, since they might be different lengths in error case
         (for ([pa (in-syntax pos-args)]
               [pt (in-list pos-tys)])
           (tc-expr/check pa (ret pt)))
         (for ([n (in-list names)]
               #:unless (memq n tnames))
           (tc-error/delayed
            "unknown named argument ~a for class\nlegal named arguments are ~a"
            n (stringify tnames)))
         (for-each (match-lambda
                     [(list tname tfty opt?)
                      (let ([s (dict-ref name-assoc tname
                                 (lambda ()
                                   (unless opt?
                                     (tc-error/delayed "value not provided for named init arg ~a"
                                                       tname))
                                   #f))])
                        (if s
                            ;; this argument was present
                            (tc-expr/check s (ret tfty))
                            ;; this argument wasn't provided, and was optional
                            #f))])
                   tnflds)
         (ret (make-Instance c))]
        [(tc-result1: t)
         (tc-error/expr #:return (ret (Un))
                        "expected a class value for object creation, got: ~a" t)]))))
