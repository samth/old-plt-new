#lang racket/base

(require racket/class racket/match racket/list racket/math racket/contract racket/vector
         "../common/math.rkt"
         "../common/plot-device.rkt"
         "../common/ticks.rkt"
         "../common/draw.rkt"
         "../common/contract.rkt"
         "../common/axis-transform.rkt"
         "../common/parameters.rkt"
         "../common/sample.rkt"
         "matrix.rkt"
         "shape.rkt"
         "clip.rkt")

(provide (all-defined-out))

(define plot3d-subdivisions (make-parameter 0))

(define 3d-plot-area%
  (class object%
    (init-field bounds-rect rx-ticks rx-far-ticks ry-ticks ry-far-ticks rz-ticks rz-far-ticks)
    (init dc dc-x-min dc-y-min dc-x-size dc-y-size)
    (super-new)
    
    (define pd (make-object plot-device% dc dc-x-min dc-y-min dc-x-size dc-y-size))
    (send pd reset-drawing-params)
    
    (define char-height (send pd get-char-height))
    (define half-char-height (* 1/2 char-height))
    
    (define dc-x-max (+ dc-x-min dc-x-size))
    (define dc-y-max (+ dc-y-min dc-y-size))
    
    (match-define (vector (ivl x-min x-max) (ivl y-min y-max) (ivl z-min z-max)) bounds-rect)
    (define x-size (- x-max x-min))
    (define y-size (- y-max y-min))
    (define z-size (- z-max z-min))
    (define x-mid (* 1/2 (+ x-min x-max)))
    (define y-mid (* 1/2 (+ y-min y-max)))
    (define z-mid (* 1/2 (+ z-min z-max)))
    
    (define clipping? #f)
    (define clip-x-min x-min)
    (define clip-x-max x-max)
    (define clip-y-min y-min)
    (define clip-y-max y-max)
    (define clip-z-min z-min)
    (define clip-z-max z-max)
    
    (define/public (put-clip-rect rect)
      (match-define (vector (ivl rx-min rx-max) (ivl ry-min ry-max) (ivl rz-min rz-max)) rect)
      (define cx-min (if rx-min (max* x-min rx-min) x-min))
      (define cx-max (if rx-max (min* x-max rx-max) x-max))
      (define cy-min (if ry-min (max* y-min ry-min) y-min))
      (define cy-max (if ry-max (min* y-max ry-max) y-max))
      (define cz-min (if rz-min (max* z-min rz-min) z-min))
      (define cz-max (if rz-max (min* z-max rz-max) z-max))
      (let ([cx-min  (min* cx-min cx-max)]
            [cx-max  (max* cx-min cx-max)]
            [cy-min  (min* cy-min cy-max)]
            [cy-max  (max* cy-min cy-max)]
            [cz-min  (min* cz-min cz-max)]
            [cz-max  (max* cz-min cz-max)])
        (set! clip-x-min cx-min)
        (set! clip-x-max cx-max)
        (set! clip-y-min cy-min)
        (set! clip-y-max cy-max)
        (set! clip-z-min cz-min)
        (set! clip-z-max cz-max))
      (set! clipping? #t))
    
    (define/public (clear-clip-rect) (set! clipping? #f))
    
    (define (in-bounds? v)
      (or (not clipping?) (point-in-bounds? v clip-x-min clip-x-max
                                            clip-y-min clip-y-max
                                            clip-z-min clip-z-max)))
    
    (define/public (get-x-ticks) x-ticks)
    (define/public (get-x-far-ticks) x-far-ticks)
    (define/public (get-y-ticks) y-ticks)
    (define/public (get-y-far-ticks) y-far-ticks)
    (define/public (get-z-ticks) z-ticks)
    (define/public (get-z-far-ticks) z-far-ticks)
    
    (define/public (get-bounds-rect) bounds-rect)
    
    (define/public (get-clip-rect)
      (if clipping?
          (vector (ivl clip-x-min clip-x-max) (ivl clip-y-min clip-y-max) (ivl clip-z-min clip-z-max))
          bounds-rect))
    
    (define angle (plot3d-angle))
    (define altitude (plot3d-altitude))
    ;; FLOATING-POINT HACK: Adding an epsilon to the angle ensures that, when it is 90, 180
    ;; or 270, the x/y/z tick labels are drawn on the left side.
    (define theta (+ (degrees->radians angle) 0.00001))
    (define rho (degrees->radians altitude))
    
    (define identity-transforms?
      (and (equal? (plot-x-transform) id-transform)
           (equal? (plot-y-transform) id-transform)
           (equal? (plot-z-transform) id-transform)))
    
    (match-define (invertible-function fx _) (apply-axis-transform (plot-x-transform) x-min x-max))
    (match-define (invertible-function fy _) (apply-axis-transform (plot-y-transform) y-min y-max))
    (match-define (invertible-function fz _) (apply-axis-transform (plot-z-transform) z-min z-max))
    
    (define axis-transform
      (cond
        [identity-transforms?  (λ (v) v)]
        [else  (λ (v)
                 (match-define (vector x y z) v)
                 (vector (fx x) (fy y) (fz z)))]))
    
    (define (center v)
      (match-define (vector x y z) v)
      (vector (- x x-mid) (- y y-mid) (- z z-mid)))
    
    ;; There are four coordinate systems:
    ;;  1. Plot coordinates (original, user-facing coordinate system)
    ;;  2. Normalized coordinates (axis-transformed, centered, and scaled to [-1,1] on each axis)
    ;;  3. View coordinates (normalized coordinates, rotated)
    ;;  4. DC coordinates (view coordinates, projected to 2D)
    
    ;; View coordinates show up mostly in tick decorations. Normalized coordinates are only used for
    ;; surface normals. Most user vertexes get transformed from plot coordinates directly to DC
    ;; coordinates.
    
    (define scale-matrix (m3-scale (/ x-size) (/ y-size) (/ z-size)))
    (define rotate-theta-matrix (m3-rotate-z theta))
    (define rotate-rho-matrix (m3-rotate-x rho))
    (define rotation-matrix (m3* rotate-rho-matrix rotate-theta-matrix))
    (define transform-matrix (m3* rotation-matrix scale-matrix))
    
    (define (plot->norm v) (m3-apply scale-matrix (center (axis-transform v))))
    (define (norm->view v) (m3-apply rotation-matrix v))
    (define (plot->view v) (m3-apply transform-matrix (center (axis-transform v))))
    
    (define transform-matrix/no-rho (m3* (m3-rotate-z theta) scale-matrix))
    (define (plot->view/no-rho v) (m3-apply transform-matrix/no-rho (center (axis-transform v))))
    (define (rotate/rho v) (m3-apply rotate-rho-matrix v))
    
    (define view->dc #f)
    (define (plot->dc/no-axis-trans v) (view->dc (m3-apply transform-matrix (center v))))
    (define (plot->dc v) (view->dc (plot->view v)))
    
    (define-values (view-x-size view-y-size view-z-size)
      (match-let ([(vector view-x-ivl view-y-ivl view-z-ivl)
                   (bounding-rect
                    (map plot->view (list (vector x-min y-min z-min) (vector x-min y-min z-max)
                                          (vector x-min y-max z-min) (vector x-min y-max z-max)
                                          (vector x-max y-min z-min) (vector x-max y-min z-max)
                                          (vector x-max y-max z-min) (vector x-max y-max z-max))))])
        (values (ivl-length view-x-ivl) (ivl-length view-y-ivl) (ivl-length view-z-ivl))))
    
    (define (make-view->dc left right top bottom)
      (define area-x-min (+ dc-x-min left))
      (define area-x-max (- dc-x-max right))
      (define area-y-min (+ dc-y-min top))
      (define area-y-max (- dc-y-max bottom))
      (define area-x-mid (* 1/2 (+ area-x-min area-x-max)))
      (define area-y-mid (* 1/2 (+ area-y-min area-y-max)))
      (define area-per-view-x (/ (- area-x-max area-x-min) view-x-size))
      (define area-per-view-z (/ (- area-y-max area-y-min) view-z-size))
      (λ (v)
        (match-define (vector x _ z) v)
        (vector (+ area-x-mid (* x area-per-view-x))
                (- area-y-mid (* z area-per-view-z)))))
    
    ;; Initial view->dc
    (define init-top-margin (if (and (plot-decorations?) (plot-title)) (* 3/2 char-height) 0))
    (set! view->dc (make-view->dc 0 0 init-top-margin 0))
    
    (define (plot-dir->dc-angle v)
      (match-define (vector dx dy)
        (v- (plot->dc/no-axis-trans (v+ v (vector x-mid y-mid z-mid)))
            (plot->dc/no-axis-trans (vector x-mid y-mid z-mid))))
      (- (atan2 (- dy) dx)))
    
    (define (x-axis-angle) (plot-dir->dc-angle #(1 0 0)))
    (define (y-axis-angle) (plot-dir->dc-angle #(0 1 0)))
    
    (define/public (plot-dir->dc-dir v)
      (vnormalize (v- (plot->dc/no-axis-trans (v+ v (vector x-mid y-mid z-mid)))
                      (plot->dc/no-axis-trans (vector x-mid y-mid z-mid)))))
    
    (define (x-axis-dir) (plot-dir->dc-dir #(1 0 0)))
    (define (y-axis-dir) (plot-dir->dc-dir #(0 1 0)))
    
    ;; ===============================================================================================
    ;; Tick and label constants
    
    (define tick-radius (* 1/2 (plot-tick-size)))
    (define half-tick-radius (* 1/2 tick-radius))
    
    (define x-axis-y-min? ((cos theta) . >= . 0))  ; #t iff x near labels should be drawn at y-min
    (define y-axis-x-min? ((sin theta) . >= . 0))  ; #t iff y near labels should be drawn at x-min
    
    (define x-axis-y (if x-axis-y-min? y-min y-max))
    (define y-axis-x (if y-axis-x-min? x-min x-max))
    (define z-axis-x (if x-axis-y-min? x-min x-max))
    (define z-axis-y (if y-axis-x-min? y-max y-min))
    
    (define x-far-axis-y (if x-axis-y-min? y-max y-min))
    (define y-far-axis-x (if y-axis-x-min? x-max x-min))
    (define z-far-axis-x (if x-axis-y-min? x-max x-min))
    (define z-far-axis-y (if y-axis-x-min? y-min y-max))
    
    (define near-dist^2 (sqr (* 3 (plot-line-width))))
    (define (vnear? v1 v2)
      ((vmag^2 (v- (plot->dc v1) (plot->dc v2))) . <= . near-dist^2))
    
    (define ((x-ticks-near? y) t1 t2)
      (vnear? (vector (pre-tick-value t1) y z-min)
              (vector (pre-tick-value t2) y z-min)))
    
    (define ((y-ticks-near? x) t1 t2)
      (vnear? (vector x (pre-tick-value t1) z-min)
              (vector x (pre-tick-value t2) z-min)))
    
    (define ((z-ticks-near? x y) t1 t2)
      (vnear? (vector x y (pre-tick-value t1))
              (vector x y (pre-tick-value t2))))
    
    (define x-ticks
      (collapse-ticks (filter (λ (t) (<= x-min (pre-tick-value t) x-max)) rx-ticks)
                      (x-ticks-near? x-axis-y)))
    (define y-ticks
      (collapse-ticks (filter (λ (t) (<= y-min (pre-tick-value t) y-max)) ry-ticks)
                      (y-ticks-near? y-axis-x)))
    (define z-ticks
      (collapse-ticks (filter (λ (t) (<= z-min (pre-tick-value t) z-max)) rz-ticks)
                      (z-ticks-near? z-axis-x z-axis-y)))
    
    (define x-far-ticks
      (collapse-ticks (filter (λ (t) (<= x-min (pre-tick-value t) x-max)) rx-far-ticks)
                      (x-ticks-near? x-far-axis-y)))
    (define y-far-ticks
      (collapse-ticks (filter (λ (t) (<= y-min (pre-tick-value t) y-max)) ry-far-ticks)
                      (y-ticks-near? y-far-axis-x)))
    (define z-far-ticks
      (collapse-ticks (filter (λ (t) (<= z-min (pre-tick-value t) z-max)) rz-far-ticks)
                      (z-ticks-near? z-far-axis-x z-far-axis-y)))
    
    ;; ===============================================================================================
    ;; Tick and label parameters, and fixpoint margin computation
    
    ;; From here through "All parameters" are functions that compute *just the parameters* of ticks
    ;; and labels that will be drawn on the plot. We have to separate computing parameters from
    ;; actually drawing the ticks and labels so we can solve for the plot margins using a fixpoint
    ;; computation. See ../common/draw.rkt for more explanation. (Search for 'margin-fixpoint'.)
    
    ;; -----------------------------------------------------------------------------------------------
    ;; Tick parameters
    
    (define (x-tick-value->view x) (plot->view (vector x x-axis-y z-min)))
    (define (y-tick-value->view y) (plot->view (vector y-axis-x y z-min)))
    (define (x-tick-value->dc x) (view->dc (x-tick-value->view x)))
    (define (y-tick-value->dc y) (view->dc (y-tick-value->view y)))
    (define (z-tick-value->dc z) (plot->dc (vector z-axis-x z-axis-y z)))
    
    (define (x-far-tick-value->view x) (plot->view (vector x x-far-axis-y z-min)))
    (define (y-far-tick-value->view y) (plot->view (vector y-far-axis-x y z-min)))
    (define (x-far-tick-value->dc x) (view->dc (x-far-tick-value->view x)))
    (define (y-far-tick-value->dc y) (view->dc (y-far-tick-value->view y)))
    (define (z-far-tick-value->dc z) (plot->dc (vector z-far-axis-x z-far-axis-y z)))
    
    (define (get-tick-params ticks tick-value->dc angle)
      (for/list ([t  (in-list ticks)])
        (match-define (tick p major? _) t)
        (list major? (tick-value->dc p) (if major? tick-radius half-tick-radius) angle)))
    
    (define (get-x-tick-params)
      (if (plot-x-axis?) (get-tick-params x-ticks x-tick-value->dc (y-axis-angle)) empty))
    
    (define (get-y-tick-params)
      (if (plot-y-axis?) (get-tick-params y-ticks y-tick-value->dc (x-axis-angle)) empty))
    
    (define (get-z-tick-params)
      (if (plot-z-axis?) (get-tick-params z-ticks z-tick-value->dc 0) empty))
    
    (define (get-x-far-tick-params)
      (if (plot-x-far-axis?) (get-tick-params x-far-ticks x-far-tick-value->dc (y-axis-angle)) empty))
    
    (define (get-y-far-tick-params)
      (if (plot-y-far-axis?) (get-tick-params y-far-ticks y-far-tick-value->dc (x-axis-angle)) empty))
    
    (define (get-z-far-tick-params)
      (if (plot-z-far-axis?) (get-tick-params z-far-ticks z-far-tick-value->dc 0) empty))
    
    ;; -----------------------------------------------------------------------------------------------
    ;; Tick label parameters
    
    (define draw-x-far-tick-labels? (not (and (plot-x-axis?) (equal? x-ticks x-far-ticks))))
    (define draw-y-far-tick-labels? (not (and (plot-y-axis?) (equal? y-ticks y-far-ticks))))
    (define draw-z-far-tick-labels? (not (and (plot-z-axis?) (equal? z-ticks z-far-ticks))))
    
    (define (sort-ticks ticks tick-value->view)
      (sort ticks > #:key (λ (t) (vector-ref (tick-value->view (pre-tick-value t)) 2))
            #:cache-keys? #t))
    
    (define (opposite-anchor a)
      (case a
        [(top-left)  'bottom-right] [(top)  'bottom] [(top-right)  'bottom-left] [(right)  'left]
        [(bottom-right)  'top-left] [(bottom)  'top] [(bottom-left)  'top-right] [(left)  'right]))
    
    (define x-tick-label-anchor
      (let ([s  (sin theta)])
        (cond [(s . < . (sin (degrees->radians -67.5)))  (if x-axis-y-min? 'top-right 'top-left)]
              [(s . < . (sin (degrees->radians -22.5)))  (if x-axis-y-min? 'top-right 'top-left)]
              [(s . < . (sin (degrees->radians 22.5)))   'top]
              [(s . < . (sin (degrees->radians 67.5)))   (if x-axis-y-min? 'top-left 'top-right)]
              [else                                      (if x-axis-y-min? 'top-left 'top-right)])))
    
    (define y-tick-label-anchor
      (let ([c  (cos theta)])
        (cond [(c . > . (cos (degrees->radians 22.5)))   (if y-axis-x-min? 'top-right 'top-left)]
              [(c . > . (cos (degrees->radians 67.5)))   (if y-axis-x-min? 'top-right 'top-left)]
              [(c . > . (cos (degrees->radians 112.5)))  'top]
              [(c . > . (cos (degrees->radians 157.5)))  (if y-axis-x-min? 'top-left 'top-right)]
              [else                                      (if y-axis-x-min? 'top-left 'top-right)])))
    
    (define z-tick-label-anchor 'right)
    
    (define x-far-tick-label-anchor (opposite-anchor x-tick-label-anchor))
    (define y-far-tick-label-anchor (opposite-anchor y-tick-label-anchor))
    (define z-far-tick-label-anchor 'left)
    
    (define (get-tick-label-params ticks tick-value->dc offset-dir anchor)
      (define dist (+ (pen-gap) tick-radius))
      (for/list ([t  (in-list ticks)] #:when (pre-tick-major? t))
        (match-define (tick x _ label) t)
        (list label (v+ (tick-value->dc x) (v* offset-dir dist)) anchor)))
    
    (define (get-x-tick-label-params)
      (if (plot-x-axis?)
          (let ([offset  (if x-axis-y-min? (vneg (y-axis-dir)) (y-axis-dir))])
            (get-tick-label-params (sort-ticks x-ticks x-tick-value->view)
                                   x-tick-value->dc offset x-tick-label-anchor))
          empty))
    
    (define (get-y-tick-label-params)
      (if (plot-y-axis?)
          (let ([offset  (if y-axis-x-min? (vneg (x-axis-dir)) (x-axis-dir))])
            (get-tick-label-params (sort-ticks y-ticks y-tick-value->view)
                                   y-tick-value->dc offset y-tick-label-anchor))
          empty))
    
    (define (get-z-tick-label-params)
      (if (plot-z-axis?)
          (get-tick-label-params z-ticks z-tick-value->dc #(-1 0) z-tick-label-anchor)
          empty))
    
    (define (get-x-far-tick-label-params)
      (if (and (plot-x-far-axis?) draw-x-far-tick-labels?)
          (let ([offset  (if x-axis-y-min? (y-axis-dir) (vneg (y-axis-dir)))])
            (get-tick-label-params (sort-ticks x-far-ticks x-far-tick-value->view)
                                   x-far-tick-value->dc offset x-far-tick-label-anchor))
          empty))
    
    (define (get-y-far-tick-label-params)
      (if (and (plot-y-far-axis?) draw-y-far-tick-labels?)
          (let ([offset  (if y-axis-x-min? (x-axis-dir) (vneg (x-axis-dir)))])
            (get-tick-label-params (sort-ticks y-far-ticks y-far-tick-value->view)
                                   y-far-tick-value->dc offset y-far-tick-label-anchor))
          empty))
    
    (define (get-z-far-tick-label-params)
      (if (and (plot-z-far-axis?) draw-z-far-tick-labels?)
          (get-tick-label-params z-far-ticks z-far-tick-value->dc #(1 0) z-far-tick-label-anchor)
          empty))
    
    ;; -----------------------------------------------------------------------------------------------
    ;; Axis label parameters
    
    (define (max-tick-offset ts)
      (cond [(empty? ts)  0]
            [(ormap pre-tick-major? ts)  (+ (pen-gap) tick-radius)]
            [else  (+ (pen-gap) (* 1/4 (plot-tick-size)))]))
    
    (define max-x-tick-offset (if (plot-x-axis?) (max-tick-offset x-ticks) 0))
    (define max-y-tick-offset (if (plot-y-axis?) (max-tick-offset y-ticks) 0))
    
    (define max-x-far-tick-offset (if (plot-x-far-axis?) (max-tick-offset x-far-ticks) 0))
    (define max-y-far-tick-offset (if (plot-y-far-axis?) (max-tick-offset y-far-ticks) 0))
    
    (define (max-tick-label-height ts)
      (if (ormap pre-tick-major? ts) char-height 0))
    
    (define (max-tick-label-width ts)
      (apply max 0 (for/list ([t  (in-list ts)] #:when (pre-tick-major? t))
                     (send pd get-text-width (tick-label t)))))
    
    (define max-x-tick-label-width (max-tick-label-width x-ticks))
    (define max-y-tick-label-width (max-tick-label-width y-ticks))
    (define max-z-tick-label-width (max-tick-label-width z-ticks))
    (define max-x-tick-label-height (max-tick-label-height x-ticks))
    (define max-y-tick-label-height (max-tick-label-height y-ticks))
    (define max-z-tick-label-height (max-tick-label-height z-ticks))
    
    (define max-x-far-tick-label-width (max-tick-label-width x-far-ticks))
    (define max-y-far-tick-label-width (max-tick-label-width y-far-ticks))
    (define max-z-far-tick-label-width (max-tick-label-width z-far-ticks))
    (define max-x-far-tick-label-height (max-tick-label-height x-far-ticks))
    (define max-y-far-tick-label-height (max-tick-label-height y-far-ticks))
    (define max-z-far-tick-label-height (max-tick-label-height z-far-ticks))
    
    (define (max-tick-label-diag axis-dc-dir max-tick-label-width max-tick-label-height)
      (match-define (vector dx dy) axis-dc-dir)
      (+ (* (abs dx) max-tick-label-width) (* (abs dy) max-tick-label-height)))
    
    (define (max-x-tick-label-diag)
      (if (plot-x-axis?)
          (max-tick-label-diag (y-axis-dir) max-x-tick-label-width max-x-tick-label-height)
          0))
    
    (define (max-y-tick-label-diag)
      (if (plot-y-axis?)
          (max-tick-label-diag (x-axis-dir) max-y-tick-label-width max-y-tick-label-height)
          0))
    
    (define (max-x-far-tick-label-diag)
      (if (and (plot-x-far-axis?) draw-x-far-tick-labels?)
          (max-tick-label-diag (y-axis-dir) max-x-far-tick-label-width max-x-far-tick-label-height)
          0))
    
    (define (max-y-far-tick-label-diag)
      (if (and (plot-y-far-axis?) draw-y-far-tick-labels?)
          (max-tick-label-diag (x-axis-dir) max-y-far-tick-label-width max-y-far-tick-label-height)
          0))
    
    (define (get-x-label-params)
      (define v0 (plot->dc/no-axis-trans (vector x-mid x-axis-y z-min)))
      (define dist (+ max-x-tick-offset (max-x-tick-label-diag) half-char-height))
      (list (plot-x-label) (v+ v0 (v* (y-axis-dir) (if x-axis-y-min? (- dist) dist)))
            'top (- (if x-axis-y-min? 0 pi) (x-axis-angle))))
    
    (define (get-y-label-params)
      (define v0 (plot->dc/no-axis-trans (vector y-axis-x y-mid z-min)))
      (define dist (+ max-y-tick-offset (max-y-tick-label-diag) half-char-height))
      (list (plot-y-label) (v+ v0 (v* (x-axis-dir) (if y-axis-x-min? (- dist) dist)))
            'top (- (if y-axis-x-min? pi 0) (y-axis-angle))))
    
    (define (get-z-label-params)
      (list (plot-z-label) (v+ (plot->dc (vector z-axis-x z-axis-y z-max))
                               (vector 0 (- half-char-height)))
            'bottom-left 0))
    
    (define (get-x-far-label-params)
      (define v0 (plot->dc/no-axis-trans (vector x-mid x-far-axis-y z-min)))
      (define dist (+ max-x-far-tick-offset (max-x-far-tick-label-diag) half-char-height))
      (list (plot-x-far-label) (v+ v0 (v* (y-axis-dir) (if x-axis-y-min? dist (- dist))))
            'bottom (- (if x-axis-y-min? 0 pi) (x-axis-angle))))
    
    (define (get-y-far-label-params)
      (define v0 (plot->dc/no-axis-trans (vector y-far-axis-x y-mid z-min)))
      (define dist (+ max-y-far-tick-offset (max-y-far-tick-label-diag) half-char-height))
      (list (plot-y-far-label) (v+ v0 (v* (x-axis-dir) (if y-axis-x-min? dist (- dist))))
            'bottom (- (if y-axis-x-min? pi 0) (y-axis-angle))))
    
    (define (get-z-far-label-params)
      (list (plot-z-far-label) (v+ (plot->dc (vector z-far-axis-x z-far-axis-y z-max))
                                   (vector 0 (- half-char-height)))
            'bottom-right 0))
    
    ;; -----------------------------------------------------------------------------------------------
    ;; All parameters
    
    ;; Within each get-back-* or get-front-*, the parameters are ordered (roughly) back-to-front
    
    (define (get-back-label-params)
      (if (plot-decorations?)
          (append (if (plot-x-far-label) (list (get-x-far-label-params)) empty)
                  (if (plot-y-far-label) (list (get-y-far-label-params)) empty)
                  (get-x-far-tick-label-params)
                  (get-y-far-tick-label-params))
          empty))
    
    (define (get-front-label-params)
      (if (plot-decorations?)
          (append (get-z-tick-label-params)
                  (get-z-far-tick-label-params)
                  (get-x-tick-label-params)
                  (get-y-tick-label-params)
                  (if (plot-x-label) (list (get-x-label-params)) empty)
                  (if (plot-y-label) (list (get-y-label-params)) empty)
                  (if (plot-z-label) (list (get-z-label-params)) empty)
                  (if (plot-z-far-label) (list (get-z-far-label-params)) empty))
          empty))
    
    (define (get-back-tick-params)
      (if (plot-decorations?)
          (append (if (plot-x-far-axis?) (get-x-far-tick-params) empty)
                  (if (plot-y-far-axis?) (get-y-far-tick-params) empty)
                  (if (plot-x-axis?) (get-x-tick-params) empty)
                  (if (plot-y-axis?) (get-y-tick-params) empty))
          empty))
    
    
    (define (get-front-tick-params)
      (if (plot-decorations?)
          (append (if (plot-z-axis?) (get-z-tick-params) empty)
                  (if (plot-z-far-axis?) (get-z-far-tick-params) empty))
          empty))
    
    (define (get-all-tick-params)
      (append (get-back-tick-params) (get-front-tick-params)))
    
    (define (get-all-label-params)
      (append (get-back-label-params) (get-front-label-params)))
    
    ;; -----------------------------------------------------------------------------------------------
    ;; Fixpoint margin computation
    
    (define (get-param-vs/set-view->dc! left right top bottom)
      ;(printf "margins: ~v ~v ~v ~v~n" left right top bottom)
      ;(printf "label params = ~v~n" (get-all-label-params))
      ;(printf "tick params = ~v~n" (get-all-tick-params))
      (set! view->dc (make-view->dc left right top bottom))
      (append (append* (map (λ (params) (send/apply pd get-text-corners params))
                            (get-all-label-params)))
              (append* (map (λ (params) (send/apply pd get-tick-endpoints (rest params)))
                            (get-all-tick-params)))))
    
    (define-values (left right top bottom)
      (margin-fixpoint dc-x-min dc-x-max dc-y-min dc-y-max 0 0 init-top-margin 0
                       get-param-vs/set-view->dc!))
    
    (define area-x-min (+ dc-x-min left))
    (define area-x-max (- dc-x-max right))
    (define area-y-min (+ dc-y-min top))
    (define area-y-max (- dc-y-max bottom))
    
    ;; ===============================================================================================
    ;; Plot decoration
    
    (define (draw-title)
      (when (and (plot-decorations?) (plot-title))
        (send pd draw-text (plot-title) (vector (* 1/2 (+ dc-x-min dc-x-max)) dc-y-min) 'top)))
    
    (define (draw-back-axes)
      (when (plot-decorations?)
        (send pd set-minor-pen)
        (when (plot-x-axis?)
          (send pd draw-line
                (plot->dc/no-axis-trans (vector x-min x-axis-y z-min))
                (plot->dc/no-axis-trans (vector x-max x-axis-y z-min))))
        (when (plot-x-far-axis?)
          (send pd draw-line
                (plot->dc/no-axis-trans (vector x-min x-far-axis-y z-min))
                (plot->dc/no-axis-trans (vector x-max x-far-axis-y z-min))))
        (when (plot-y-axis?)
          (send pd draw-line
                (plot->dc/no-axis-trans (vector y-axis-x y-min z-min))
                (plot->dc/no-axis-trans (vector y-axis-x y-max z-min))))
        (when (plot-y-far-axis?)
          (send pd draw-line
                (plot->dc/no-axis-trans (vector y-far-axis-x y-min z-min))
                (plot->dc/no-axis-trans (vector y-far-axis-x y-max z-min))))))
    
    (define (draw-front-axes)
      (when (plot-decorations?)
        (send pd set-minor-pen)
        (when (plot-z-axis?)
          (send pd draw-line
                (plot->dc/no-axis-trans (vector z-axis-x z-axis-y z-min))
                (plot->dc/no-axis-trans (vector z-axis-x z-axis-y z-max))))
        (when (plot-z-far-axis?)
          (send pd draw-line
                (plot->dc/no-axis-trans (vector z-far-axis-x z-far-axis-y z-min))
                (plot->dc/no-axis-trans (vector z-far-axis-x z-far-axis-y z-max))))))
    
    (define (draw-ticks tick-params)
      (for ([params  (in-list tick-params)])
        (match-define (list major? v r angle) params)
        (if major? (send pd set-major-pen) (send pd set-minor-pen))
        (send pd draw-tick v r angle)))
    
    (define (draw-labels label-params)
      (for ([params  (in-list label-params)])
        (send/apply pd draw-text params #:outline? #t)))
    
    ;; ===============================================================================================
    ;; Delayed drawing
    
    (define render-list empty)
    (define (add-shape! shape) (set! render-list (cons shape render-list)))
    (define (add-shapes! shapes) (set! render-list (append shapes render-list)))
    
    (define (draw-shapes ss)
      (define s+cs (map (λ (s) (cons s (plot->view/no-rho (shape-center s)))) ss))
      (for ([s+c  (in-list (depth-sort (reverse s+cs)))])
        (match-define (cons s c) s+c)
        (draw-shape s (rotate/rho c))))
    
    (define (draw-polygon alpha center vs norm pen-color pen-width pen-style brush-color brush-style)
      (define-values (diff spec) (get-light-values center (norm->view norm)))
      (let ([pen-color  (map (λ (v) (+ (* v diff) spec)) pen-color)]
            [brush-color  (map (λ (v) (+ (* v diff) spec)) brush-color)])
        (send pd set-pen pen-color pen-width pen-style)
        (send pd set-brush brush-color brush-style)
        (send pd draw-polygon (map (λ (v) (plot->dc v)) vs))))
    
    (define (draw-shape s center)
      (send pd set-alpha (shape-alpha s))
      (match s
        ;; shapes
        [(shapes alpha _ ss)  (draw-shapes ss)]
        ;; polygon
        [(polygon alpha _ vs norm pen-color pen-width pen-style brush-color brush-style)
         (draw-polygon alpha center vs norm pen-color pen-width pen-style brush-color brush-style)]
        ;; rectangle
        [(rectangle alpha _ r pen-color pen-width pen-style brush-color brush-style)
         (for ([face  (in-list (rect-visible-faces r theta))])
           (match face
             [(list norm vs ...)  (draw-polygon alpha center vs norm pen-color pen-width pen-style
                                                brush-color brush-style)]
             [_  (void)]))]
        ;; line
        [(line alpha _ v1 v2 pen-color pen-width pen-style)
         (send pd set-pen pen-color pen-width pen-style)
         (send pd draw-line (plot->dc v1) (plot->dc v2))]
        ;; text
        [(text alpha _ anchor angle str font-size font-family color)
         (send pd set-font font-size font-family)
         (send pd set-text-foreground color)
         (send pd draw-text str (view->dc center) anchor angle)]
        ;; glyph
        [(glyph alpha _ symbol size pen-color pen-width pen-style brush-color brush-style)
         (send pd set-pen pen-color pen-width pen-style)
         (send pd set-brush brush-color brush-style)
         (send pd draw-glyphs (list (view->dc center)) symbol size)]
        ;; tick glyph
        [(tick-glyph alpha _ radius angle pen-color pen-width pen-style)
         (send pd set-pen pen-color pen-width pen-style)
         (send pd draw-tick (view->dc center) radius angle)]
        ;; arrow glyph
        [(arrow-glyph alpha _ v1 v2 pen-color pen-width pen-style)
         (send pd set-pen pen-color pen-width pen-style)
         (send pd draw-arrow (plot->dc v1) (plot->dc v2))]
        [_  (error 'draw-shapes "shape not implemented: ~e" s)]))
    
    ;; Use a special view transform for the light so that the light angle is always the same
    ;; regardless of theta (but rotates rho). This also doesn't do any axis transforms, which could
    ;; fail; e.g. log transform when the light is at a negative position.
    (define light-transform-matrix (m3* (m3-rotate-x rho) scale-matrix))
    (define (plot->light-view v) (m3-apply light-transform-matrix (center v)))
    
    ;; Light position, in normalized view coordinates: 5 units up, ~3 units back and to the left
    ;; (simulates non-noon daylight conditions)
    (define light (vector-map exact->inexact
                              (plot->light-view (vector (- x-min (* 2 x-size))
                                                        (- y-min (* 2 y-size))
                                                        (+ z-max (* 5 z-size))))))
    ;; View direction, in normalized view coordinates: many graph widths backward
    (define view-dir (vector 0.0 -50.0 0.0))
    
    (define diffuse-light? (plot3d-diffuse-light?))
    (define specular-light? (plot3d-specular-light?))
    (define ambient-light (exact->inexact (plot3d-ambient-light)))
    
    (define get-light-values
      (cond
        [(not (or diffuse-light? specular-light?))  (λ (v norm) (values 1.0 0.0))]
        [else
         (λ (v norm)
           ; common lighting values
           (define light-dir (vnormalize (v- light v)))
           ; diffuse lighting: typical Lambertian surface model
           (define diff (if diffuse-light? (abs (vdot norm light-dir)) 1.0))
           ; specular highlighting: Blinn-Phong model
           (define spec (cond [specular-light?
                               (define lv (v* (v+ light-dir view-dir) 0.5))
                               (define cos-angle (/ (abs (vdot norm lv)) (vmag lv)))
                               (* 32.0 (expt cos-angle 10.0))]
                              [else  0.0]))
           ; put it all together
           (values (+ ambient-light (* (- 1.0 ambient-light) diff)) spec))]))
    
    ;; ===============================================================================================
    ;; Public drawing control (used by plot3d/dc)
    
    (define/public (start-plot)
      (send pd reset-drawing-params)
      (send pd clear)
      (draw-title)
      (draw-labels (get-back-label-params))
      (draw-ticks (get-back-tick-params))
      (draw-back-axes)
      (send pd set-clipping-rect
            (vector (ivl (+ 1/2 (- area-x-min (plot-line-width))) (+ area-x-max (plot-line-width)))
                    (ivl (+ 1/2 (- area-y-min (plot-line-width))) (+ area-y-max (plot-line-width)))))
      (set! render-list empty))
    
    (define/public (start-renderer rend-bounds-rect)
      (reset-drawing-params)
      (put-clip-rect rend-bounds-rect))
    
    (define/public (end-renderers)
      (clear-clip-rect)
      (draw-shapes render-list)
      (send pd reset-drawing-params)
      (draw-front-axes)
      (draw-ticks (get-front-tick-params))
      (draw-labels (get-front-label-params)))
    
    (define (draw-legend* legend-entries)
      (define gap-size (+ (pen-gap) tick-radius))
      (send pd draw-legend legend-entries
            (vector (ivl (+ area-x-min gap-size) (- area-x-max gap-size))
                    (ivl (+ area-y-min gap-size) (- area-y-max gap-size)))))
    
    (define/public (draw-legend legend-entries) (draw-legend* legend-entries))
    
    (define/public (end-plot)
      (send pd restore-drawing-params))
    
    ;; ===============================================================================================
    ;; Public drawing interface (used by renderers)
    
    (define/public (get-plot-device) pd)
    
    (define/public (get-render-list) render-list)
    (define/public (put-render-list shapes) (add-shapes! shapes))
    
    ;; Drawing parameters
    
    (define alpha 1)
    
    (define pen-color '(0 0 0))
    (define pen-width 1)
    (define pen-style 'solid)
    
    (define brush-color '(255 255 255))
    (define brush-style 'solid)
    
    (define background-color '(255 255 255))
    
    (define font-size 11)
    (define font-family 'roman)
    (define text-foreground '(0 0 0))
    
    ;; Drawing parameter accessors
    
    (define/public (put-alpha a) (set! alpha a))
    
    (define/public (put-pen color width style)
      (set! pen-color (->pen-color color))
      (set! pen-width width)
      (set! pen-style (->pen-style style)))
    
    (define/public (put-major-pen [style 'solid])
      (put-pen (plot-foreground) (plot-line-width) style))
    (define/public (put-minor-pen [style 'solid])
      (put-pen (plot-foreground) (* 1/2 (plot-line-width)) style))
    
    (define/public (put-brush color style)
      (set! brush-color (->brush-color color))
      (set! brush-style (->brush-style style)))
    
    (define/public (put-background color)
      (set! background-color (->brush-color color)))
    
    (define/public (put-font-size size) (set! font-size size))
    (define/public (put-font-family family) (set! font-family family))
    
    (define/public (put-font size family)
      (put-font-size size)
      (put-font-family family))
    
    (define/public (put-text-foreground c)
      (set! text-foreground (->pen-color c)))
    
    (define/public (reset-drawing-params)
      (put-alpha (plot-foreground-alpha))
      (put-pen (plot-foreground) (plot-line-width) 'solid)
      (put-brush (plot-background) 'solid)
      (put-background (plot-background))
      (put-font (plot-font-size) (plot-font-family))
      (put-text-foreground (plot-foreground)))
    
    ;; Drawing shapes
    
    (define/public (put-line v1 v2 [c (v* (v+ v1 v2) 1/2)])
      (let/ec return
        (unless (and (vregular? v1) (vregular? v2)) (return (void)))
        (let-values ([(v1 v2)  (if clipping?
                                   (clip-line v1 v2 clip-x-min clip-x-max
                                              clip-y-min clip-y-max
                                              clip-z-min clip-z-max)
                                   (values v1 v2))])
          (unless (and v1 v2) (return (void)))
          (cond [identity-transforms?
                 (add-shape! (line alpha c v1 v2
                                   pen-color pen-width pen-style))]
                [else
                 (define vs (subdivide-line plot->dc v1 v2))
                 (for ([v1  (in-list vs)] [v2  (in-list (rest vs))])
                   (add-shape! (line alpha c v1 v2 pen-color pen-width pen-style)))]))))
    
    (define/public (put-lines vs)
      (for ([vs  (vregular-sublists vs)])
        (when (not (empty? vs))
          (for ([v1  (in-list vs)] [v2  (in-list (rest vs))])
            (put-line v1 v2)))))
    
    (define (add-polygon lst vs c)
      (let/ec return
        (when (or (empty? vs) (not (and (andmap vregular? vs) (vregular? c))))
          (return lst))
        
        (define norm (vnormal (map plot->norm vs)))
        (let* ([vs  (if clipping?
                        (clip-polygon vs clip-x-min clip-x-max
                                      clip-y-min clip-y-max
                                      clip-z-min clip-z-max)
                        vs)]
               [vs  (if identity-transforms? vs (subdivide-polygon plot->dc vs))])
          (when (empty? vs) (return lst))
          (cons (polygon alpha c vs norm pen-color pen-width pen-style brush-color brush-style)
                lst))))
    
    (define/public (put-polygon vs [c (vcenter vs)])
      (set! render-list (add-polygon render-list vs c)))
    
    (define/public (put-polygons vss [c (vcenter (flatten vss))])
      (define lst (for/fold ([lst empty]) ([vs  (in-list vss)]
                                           #:when (not (empty? vs)))
                    (add-polygon lst vs (vcenter vs))))
      (when (not (empty? lst))
        (add-shape! (shapes alpha c lst))))
    
    (define/public (put-rect r [c (rect-center r)])
      (when (rect-regular? r)
        (let ([r  (rect-meet r bounds-rect)])
          (add-shape! (rectangle alpha c r pen-color pen-width pen-style brush-color brush-style)))))
    
    (define/public (put-text str v [anchor 'center] [angle 0])
      (when (and (vregular? v) (in-bounds? v))
        (add-shape! (text alpha v anchor angle str font-size font-family text-foreground))))
    
    (define/public (put-glyphs vs symbol size)
      (for ([v  (in-list vs)])
        (when (and (vregular? v) (in-bounds? v))
          (add-shape!
           (glyph alpha v symbol size pen-color pen-width pen-style brush-color brush-style)))))
    
    (define/public (put-arrow v1 v2 [c (v* (v+ v1 v2) 1/2)])
      (when (and (vregular? v1) (vregular? v2) (in-bounds? v1))
        (cond [(in-bounds? v2)
               (add-shape!
                (arrow-glyph alpha c v1 v2 (->brush-color (plot-background)) (+ 2 pen-width) 'solid))
               (add-shape!
                (arrow-glyph alpha c v1 v2 pen-color pen-width pen-style))]
              [else  (put-line v1 v2)])))
    
    (define/public (put-tick v radius angle)
      (when (and (vregular? v) (in-bounds? v))
        (add-shape! (tick-glyph alpha v radius angle pen-color pen-width pen-style))))
    )) ; end class
