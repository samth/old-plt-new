#lang racket/base

(require racket/class racket/match racket/list racket/flonum racket/contract
         plot/utils
         "../common/contract-doc.rkt")

(provide (all-defined-out))

;; ===================================================================================================
;; Surface plots of R R -> R functions

(define ((surface3d-render-proc f samples color style line-color line-width line-style alpha label)
         area)
  (match-define (vector (ivl x-min x-max) (ivl y-min y-max) z-ivl) (send area get-bounds-rect))
  (define sample (f x-min x-max (animated-samples samples)
                    y-min y-max (animated-samples samples)))
  
  (send area put-alpha alpha)
  (send area put-brush color style)
  (send area put-pen line-color line-width line-style)
  (for-2d-sample
   (xa xb ya yb z1 z2 z3 z4) sample
   (send area put-polygon
         (list (vector xa ya z1) (vector xb ya z2) (vector xb yb z3) (vector xa yb z4))))
  
  (cond [label  (rectangle-legend-entry label color style line-color line-width line-style)]
        [else   empty]))

(defproc (surface3d
          [f (real? real? . -> . real?)]
          [x-min (or/c regular-real? #f) #f]
          [x-max (or/c regular-real? #f) #f]
          [y-min (or/c regular-real? #f) #f]
          [y-max (or/c regular-real? #f) #f]
          [#:z-min z-min (or/c regular-real? #f) #f]
          [#:z-max z-max (or/c regular-real? #f) #f]
          [#:samples samples (and/c exact-integer? (>=/c 2)) (plot3d-samples)]
          [#:color color plot-color/c (surface-color)]
          [#:style style plot-brush-style/c (surface-style)]
          [#:line-color line-color plot-color/c (surface-line-color)]
          [#:line-width line-width (>=/c 0) (surface-line-width)]
          [#:line-style line-style plot-pen-style/c (surface-line-style)]
          [#:alpha alpha (real-in 0 1) (surface-alpha)]
          [#:label label (or/c string? #f) #f]
          ) renderer3d?
  (define g (2d-function->sampler f))
  (renderer3d (vector (ivl x-min x-max) (ivl y-min y-max) (ivl z-min z-max))
              (surface3d-bounds-fun g samples)
              default-ticks-fun
              (surface3d-render-proc g samples color style
                                     line-color line-width line-style alpha label)))
