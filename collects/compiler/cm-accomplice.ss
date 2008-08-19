#lang scheme/base

(provide register-external-file)
(define (register-external-file f)
  (unless (and (path? f) (complete-path? f))
    (raise-type-error 'register-external-file "complete path" f))
  (let ([param (lambda () void)])
    ;; Load the code in a separate thread, so that the dynamic
    ;; extent of this one (likely a phase-sensitive macro expansion)
    ;; doesn't pollute the load:
    (thread-wait
     (thread (lambda ()
               (set! param
                     (dynamic-require 'compiler/private/cm-ctime
                                      'current-external-file-registrar)))))
    ((param) f)))
