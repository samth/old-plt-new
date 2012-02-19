#lang racket/base
(require racket/bool rackunit)

(check-true true)
(check-false false)

(check-true (boolean=? #t #t))
(check-false (boolean=? #t #f))
(check-exn #rx"^boolean=?" (λ () (boolean=? #f 11)))
(check-exn #rx"^boolean=?" (λ () (boolean=? 11 #f)))

(check-true (symbol=? 'x 'x))
(check-false (symbol=? 'x 'y))
(check-exn #rx"^symbol=?" (λ () (symbol=? 'z 11)))
(check-exn #rx"^symbol=?" (λ () (symbol=? 11 'z)))

(check-true (false? #f))
(check-false (false? #t))
(check-false (false? "11"))

(for ([x (in-list '(#f #t))])
  (for ([y (in-list '(#f #t))])
    (for ([z (in-list '(#f #t))])
      (check-equal? (implies x y z)
                    (or (not x) (or (not y) z))))))
(check-equal? (implies #f (car 'x)) #t)


(check-equal? (nand #f #f) #t)
(check-equal? (nand #f #t) #t)
(check-equal? (nand #t #f) #t)
(check-equal? (nand #t #t) #f)
(check-equal? (nand #f (car 'x)) #t)

(check-equal? (nor #f #f) #t)
(check-equal? (nor #t #f) #f)
(check-equal? (nor #f #t) #f)
(check-equal? (nor #t #t) #f)
(check-equal? (nor #t (car 'x)) #f)
