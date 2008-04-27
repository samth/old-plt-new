(module debug-tool mzscheme
  (require mzlib/etc
           mzlib/list
           mzlib/string
           ;mzlib/math
           mzlib/class
           mzlib/unit
           mzlib/contract
           mred
           mzlib/match
           (prefix drscheme:arrow: (lib "arrow.ss" "drscheme"))
           (lib "tool.ss" "drscheme")
           "marks.ss"
           syntax/boundmap
           (lib "bitmap-label.ss" "mrlib")
           "annotator.ss"
           "load-sandbox.ss"
           framework
           string-constants
           (lib "debugger-language-interface.ss" "lang"))
  
  (provide tool@)
  
  ; QUESTIONS/IDEAS
  ; what is the right way to deal with macros?
  ; how can the three tool classes communicate with each other safely
  
  (define tool@
    (unit 
      (import drscheme:tool^)
      (export drscheme:tool-exports^) 

      (define (phase1)
	(drscheme:language:extend-language-interface
         debugger-language<%>
         (lambda (superclass)
           (class* superclass (debugger-language<%>)
	           (public debugger:supported?)
		   (define (debugger:supported?) #t)
                   (super-instantiate ())))))
      (define phase2 void)
      
      (define (extract-language-level settings)
	(drscheme:language-configuration:language-settings-language settings))
      
      (define (debugger-does-not-work-for? lang)
	(not (send lang debugger:supported?)))
      
      (define (robust-syntax-source stx)
        (and (syntax? stx) (syntax-source stx)))
      
      (define (robust-vector-ref vec idx)
        (if (< idx (vector-length vec))
            (vector-ref vec idx)
            #f))
      
      (define (safe-vector-set! vec idx val)
        (when (< idx (vector-length vec))
          (vector-set! vec idx val))
        (void))
      
      (define (truncate str n)
        (if (< (string-length str) n)
            str
            (if (>= n 3)
                (string-append
                 (substring str 0 (- n 3))
                 "...")
                (substring str 0 (min n (string-length str))))))
      
      (define (clean-status s)
	(truncate (regexp-replace* #rx"\n" s " ") 200))
      
      (define (index-of chr str)
        (let loop ([i 0])
          (if (< i (string-length str))
              (if (char=? chr (string-ref str i))
                  i
                  (loop (add1 i)))
              #f)))
      
      ;; trim-expr-str: string -> string
      ;; examples:
      ;; short-id                   => short-id
      ;; really-long-identifier     => really-lon...
      ;; (<form>)                   => (<form>)
      ;; (<form> <arg1> ... <argn>) => (<form> ...)
      (define (trim-expr-str str)
        (let ([starts-with-paren (and (> (string-length str) 0))])
          (cond
            [(and starts-with-paren
                  (or (index-of #\space str)
                      (index-of #\newline str)
                      (and (> (string-length str) 14) 10)))
             ;; non-atomic expr: truncate to (form-name ...)
             => (lambda (i)
                  (string-append
                   (substring str 0 i)
                   (if starts-with-paren
                       " ...)"
                       " ...")))]
            [str])))
      
      (define (average . values)
        (/ (apply + values) (length values)))
      
      (define (truncate-value v size depth)
        (cond
          [(zero? depth) "..."]
          [(and (string? v)
                (> (string-length v) size))
           (string-append (substring v 0 size) "...")]
          [(list? v)
           (let* ([len (length v)]
                  [res (build-list (min size len)
                                   (lambda (i) (truncate-value (list-ref v i) size (sub1 depth))))])
             (if (> len size) (append res (list '...)) res))]
          [(vector? v)
           (build-vector (min size (vector-length v))
                         (lambda (i)
                           (if (and (= i (sub1 size))
                                    (> size (vector-length v)))
                               "..."
                               (truncate-value (vector-ref v i) size (sub1 depth)))))]
          [else v]))
      
      (define filename->defs
        (opt-lambda (source [default #f])
          (let/cc k
            (cond
              [(is-a? source editor<%>) source]
              [else
               (send (group:get-the-frame-group) for-each-frame
                     (lambda (frame)
                       (let* ([defss (map (lambda (t) (send t get-defs)) (send frame get-tabs))]
                              [defs (findf (lambda (d) (send d port-name-matches? source)) defss)])
                         (and defs (k defs)))))
               default]))))
      
      (define (debug-definitions-text-mixin super%)
        (class super%
          
          (inherit dc-location-to-editor-location
                   editor-location-to-dc-location
                   invalidate-bitmap-cache
                   begin-edit-sequence
                   end-edit-sequence
                   get-canvas
                   get-top-level-window
                   get-filename
                   get-tab)
          
          (define mouse-over-pos #f)
          ;; pen and brush for drawing a breakpoint
          (define bp-pen (send the-pen-list find-or-create-pen "black" 1 'solid))
          (define bp-brush (send the-brush-list find-or-create-brush "red" 'solid))
          ;; pen and brush for marking a location that could have a breakpoint installed
          (define bp-mo-pen (send the-pen-list find-or-create-pen "darkgray" 1 'solid))
          (define bp-mo-brush (send the-brush-list find-or-create-brush "tomato"
                                    'solid))
          ;; pen and brush for marking a conditional breakpoint
          (define bp-tmp-pen (send the-pen-list find-or-create-pen "black" 1 'solid))
          (define bp-tmp-brush (send the-brush-list find-or-create-brush "yellow"
                                     'solid))
          ;; pen and brush for drawing the normal execution location
          (define pc-pen (send the-pen-list find-or-create-pen "black" 1 'solid))
          (define pc-brush (send the-brush-list find-or-create-brush "forestgreen" 'solid))
          ;; pen and brush for marking the expression when not at the top of the stack
          (define pc-up-stack-pen (send the-pen-list find-or-create-pen "black" 1 'solid))
          (define pc-up-stack-brush (send the-brush-list find-or-create-brush "lightgreen" 'solid))
          ;; pen and brush for marking the location when there's an an error
          (define pc-err-pen (send the-pen-list find-or-create-pen "black" 1 'solid))
          (define pc-err-brush (send the-brush-list find-or-create-brush "red" 'solid))
          ;; pen and brush for marking the location following a break
          (define pc-brk-pen (send the-pen-list find-or-create-pen "black" 1 'solid))
          (define pc-brk-brush (send the-brush-list find-or-create-brush "gray" 'solid))
          
          (super-instantiate ())
          
          (define/augment (on-delete start len)
            (begin-edit-sequence)
            (let ([breakpoints (send (get-tab) get-breakpoints)]
                  [shifts empty])
              (hash-table-for-each
               breakpoints
               (lambda (pos status)
                 (cond
                   ; deletion after breakpoint: no effect
                   [(<= pos start)]
                   ; deletion of breakpoint: remove from table
                   [(and (< start pos)
                         (<= pos (+ start len)))
                    (hash-table-remove! breakpoints pos)]
                   ; deletion before breakpoint: shift breakpoint
                   [(> pos (+ start len))
                    (hash-table-remove! breakpoints pos)
                    (set! shifts (cons (cons (- pos len) status) shifts))])))
              (for-each (lambda (p) (hash-table-put! breakpoints (car p) (cdr p)))
                        shifts))
            (inner (void) on-delete start len))
          
          (define/augment (after-delete start len)
            (inner (void) after-delete start len)
            (when (send (get-tab) debug?)
              (send (get-tab) hide-debug))
            (end-edit-sequence))
          
          (define/augment (on-insert start len)
            (begin-edit-sequence)
            (let ([breakpoints (send (get-tab) get-breakpoints)]
                  [shifts empty])
              (hash-table-for-each
               breakpoints
               (lambda (pos status)
                 (when (< start pos)
                   ;; text inserted before this breakpoint, so shift
                   ;; the breakpoint forward by <len> positions
                   (hash-table-remove! breakpoints pos)
                   (set! shifts (cons (cons (+ pos len) status) shifts)))))
              ;; update the breakpoint locations
              (for-each (lambda (p) (hash-table-put! breakpoints (car p) (cdr p)))
                        shifts))
            (inner (void) on-insert start len))
          
          (define/augment (after-insert start len)
            (inner (void) after-insert start len)
            (when (send (get-tab) debug?)
              (send (get-tab) hide-debug))
            (end-edit-sequence))
          
          ;; lookup id in the given set of stack frames;
          ;; if that fails, try the top-level environment
          ;; invokes sk on success, fk on failure
          (define (lookup-var id frames sk fk)
            (cond
              [(and id frames (lookup-first-binding
                               (lambda (id2) (module-identifier=? id id2))
                               frames
                               (lambda () #f)))
               =>
               (lambda (binding)
                 (sk (mark-binding-value binding)
                     (lambda (v) (mark-binding-set! binding v))))]
              [(and id (send (get-tab) lookup-top-level-var
                             id
                             (lambda () #f)))
               =>
               (lambda (tlb)
                 (sk (tlb) tlb))]
              [else (fk)]))
          
          ;; mouse-event -> (or (values #f #f) (values pos editor))
          (define/private (get-pos/text event)
            (let ([event-x (send event get-x)]
                  [event-y (send event get-y)]
                  [on-it? (box #f)])
              (let loop ([editor this])
                (let-values ([(x y) (send editor dc-location-to-editor-location
                                          event-x event-y)])
                  (cond
                    [(is-a? editor text%)
                     (let ([pos (send editor find-position x y #f on-it?)])
                       (cond
                         [(not (unbox on-it?)) (values #f #f)]
                         [else
                          (let ([snip (send editor find-snip pos 'after-or-none)])
                            (if (and snip
                                     (is-a? snip editor-snip%))
                                (loop (send snip get-editor))
                                (values pos editor)))]))]
                    [(is-a? editor pasteboard%)
                     (let ([snip (send editor find-snip x y)])
                       (if (and snip
                                (is-a? snip editor-snip%))
                           (loop (send snip get-editor))
                           (values #f #f)))]
                    [else (values #f #f)])))))
          
          ;; text% start end -> (values left top right bottom)
          ;; (four numbers that indicate the locations in pixels of the
          ;;  box bounding the text between start and end
          (define/private (find-char-box text left-pos right-pos)
            (let ([xlb (box 0)]
                  [ylb (box 0)]
                  [xrb (box 0)]
                  [yrb (box 0)])
              (send text position-location left-pos xlb ylb #t)
              (send text position-location right-pos xrb yrb #f)
              (let*-values ([(xl-off yl-off) (send text editor-location-to-dc-location
                                                   (unbox xlb) (unbox ylb))]
                            [(xl yl) (dc-location-to-editor-location xl-off yl-off)]
                            [(xr-off yr-off) (send text editor-location-to-dc-location
                                                   (unbox xrb) (unbox yrb))]
                            [(xr yr) (dc-location-to-editor-location xr-off yr-off)])
                (values xl yl xr yr))))
          
          (define/private (render v)
            (send (get-tab) render v))
          
          (define (debugger-handle-right-click-non-breakable event pos)
            (let* ([frames (send (get-tab) get-stack-frames)]
                   [pos-vec (send (get-tab) get-pos-vec)]
                   [id (robust-vector-ref pos-vec pos)])
              (unless (lookup-var
                       id
                       frames
                       (lambda (val wr)
                         (let ([id-sym (syntax-e id)]
                               [menu (make-object popup-menu% #f)])
                           (make-object menu-item%
                             (clean-status
                              (format "Print value of ~a to console" id-sym))
                             menu
                             (lambda (item evt)
                               (send (get-tab) print-to-console (format "~a = ~a" id-sym val))))
                           (make-object menu-item%
                             (format "(set! ~a ...)" id-sym)
                             menu
                             (lambda (item evt)
                               (let* ([tmp
                                       (get-text-from-user
                                        (format "New value for ~a" id-sym) #f #f
                                        (format "~a" val))])
                                 (when tmp
                                   (let/ec k
                                     (wr (with-handlers
                                             ([exn:fail?
                                               (lambda (exn)
                                                 (message-box
                                                  "Debugger Error"
                                                  (format "The following error occurred: ~a"
                                                          (exn-message exn)))
                                                 (k))])
                                           (read (open-input-string tmp)))))))))
                           (send (get-canvas) popup-menu menu
                                 (+ 1 (inexact->exact (floor (send event get-x))))
                                 (+ 1 (inexact->exact (floor (send event get-y)))))
                           #t))
                       (lambda () #f))
                (super on-event event))))
          
          (define (debugger-handle-right-click-breakable event breakpoints pos break-status)
            (let ([menu (make-object popup-menu% #f)])
              (make-object menu-item%
                (if break-status
                    "Remove pause at this point"
                    "Pause at this point")
                menu
                (lambda (item evt)
                  (hash-table-put! breakpoints pos (not break-status))
                  (invalidate-bitmap-cache)))
              (let ([pc (send (get-tab) get-pc)])
                (if (and pc (= pos pc))
                    (let* ([stat (send (get-tab) get-break-status)]
                           [f (get-top-level-window)]
                           [rendered-value
                            (if (cons? stat)
                                (if (= 2 (length stat))
                                    (render (cadr stat))
                                    (format "~a" (cons 'values 
                                                       (map (lambda (v) (render v)) (rest stat)))))
                                "")])
                      (when (cons? stat)
                        (make-object menu-item%
                          "Print return value to console"
                          menu
                          (lambda _ (send (get-tab) print-to-console (format "return val = ~a"
                                                                             rendered-value)))))
                      (when (not (eq? stat 'break))
                        (make-object menu-item%
                          (if (cons? stat)
                              "Change return value..."
                              "Skip expression...")
                          menu
                          (lambda (item evt)
                            (let ([tmp (get-text-from-user "Return value" #f)])
                              (when tmp
                                (let/ec k
                                  (send (get-tab) set-break-status
                                        (cons 'exit-break
                                              (call-with-values
                                               (lambda ()
                                                 (with-handlers
                                                     ([exn:fail?
                                                       (lambda (exn)
                                                         (message-box
                                                          "Debugger Error"
                                                          (format "An error occurred: ~a" (exn-message exn))
                                                          #f
                                                          '(ok))
                                                         (k))])
                                                   (read (open-input-string tmp))))
                                               list)))
                                  (invalidate-bitmap-cache))))))))
                    (make-object menu-item%
                      "Continue to this point"
                      menu
                      (lambda (item evt)
                        (hash-table-put!
                         breakpoints pos
                         (lambda () (hash-table-put! breakpoints pos break-status) #t))
                        (invalidate-bitmap-cache)
                        (when (send (get-tab) get-stack-frames)
                          (send (get-tab) resume))))))
              (send (get-canvas) popup-menu menu
                    (+ 1 (inexact->exact (floor (send event get-x))))
                    (+ 1 (inexact->exact (floor (send event get-y)))))))
          
          (define (debugger-handle-right-click event breakpoints)
            (let-values ([(pos text) (get-pos/text event)])
              (if (and pos text)
                  (let* ([pos (add1 pos)]
                         [break-status (hash-table-get breakpoints pos (lambda () 'invalid))])
                    (match break-status
                      [(or #t #f (? procedure?))
                       (debugger-handle-right-click-breakable event breakpoints pos break-status)]
                      ['invalid
                       (debugger-handle-right-click-non-breakable event pos)]))
                  (super on-event event))))
          
          (define (debugger-handle-event event)
            (let ([breakpoints (send (get-tab) get-breakpoints)])
              (cond
                [(send event leaving?)
                 (when mouse-over-pos
                   (set! mouse-over-pos #f)
                   (invalidate-bitmap-cache))
                 (super on-event event)]
                [(or (send event moving?)
                     (send event entering?))
                 (let-values ([(pos text) (get-pos/text event)])
                   (when (and pos text)
                     (let ([pos (add1 pos)])
                       (cond
                         ;; mouse on breakable pos and hasn't moved significantly
                         [(eq? pos mouse-over-pos)]
                         ;; mouse on new breakable pos
                         [(not (eq? (hash-table-get
                                     breakpoints
                                     pos (lambda () 'invalid)) 'invalid))
                          (set! mouse-over-pos pos)
                          (invalidate-bitmap-cache)]
                         ;; moved off breakable pos
                         [mouse-over-pos
                          (set! mouse-over-pos #f)
                          (invalidate-bitmap-cache)])
                       (let* ([frames (send (get-tab) get-stack-frames)]
                              [pos-vec (send (get-tab) get-pos-vec)]
                              [id (robust-vector-ref pos-vec pos)])
                         (send (get-tab)
                               set-mouse-over-msg
                               (clean-status
                                (lookup-var id (list-tail frames (send (get-tab) get-frame-num))
                                            ;; id found
                                            (lambda (val _)
                                              (format "~a = ~a" (syntax-e id) (render val)))
                                            ;; id not found
                                            (lambda () ""))))))))
                 (super on-event event)]
                [(send event button-down? 'right)
                 (debugger-handle-right-click event breakpoints)]
                [else (super on-event event)])))
            
          (define/override (on-event event)
            (if (send (get-tab) debug?)
                (debugger-handle-event event)
                (super on-event event)))
          
          (define/override (on-paint before dc left top right bottom dx dy draw-caret)
            (super on-paint before dc left top right bottom dx dy draw-caret)
            (when (and (send (get-tab) debug?) (not before))
              ;; render breakpoints
              (let ([breakpoints (send (get-tab) get-breakpoints)])
                (hash-table-for-each
                 breakpoints
                 (lambda (pos enabled?)
                   (when (and (>= pos 0) (or enabled? (and mouse-over-pos (= mouse-over-pos pos))))
                     (let*-values ([(xl yl xr yr) (find-char-box this (sub1 pos) pos)]
                                   [(diameter) (- xr xl)]
                                   [(yoff) (/ (- yr yl diameter) 2)])
                       (let ([op (send dc get-pen)]
                             [ob (send dc get-brush)])
                         (case enabled?
                           [(#t) (send dc set-pen bp-pen)
                                 (send dc set-brush bp-brush)]
                           [(#f) (send dc set-pen bp-mo-pen)
                                 (send dc set-brush bp-mo-brush)]
                           [else (send dc set-pen bp-tmp-pen)
                                 (send dc set-brush bp-tmp-brush)])
                         (send dc draw-ellipse (+ xl dx) (+ yl dy yoff) diameter diameter)
                         (send dc set-pen op)
                         (send dc set-brush ob)))))))
              ;; mark the boundaries of the current stack frame
              ;; unless we're at the end of the expression and looking at the top frame,
              ;; in which case just mark the current location
              (let* ([frame-defs (send (get-tab) defs-containing-current-frame)]
                     [pos (send (get-tab) get-current-frame-endpoints)]
                     [start (and pos (first pos))]
                     [end (and pos (second pos))]
                     [frame-num (send (get-tab) get-frame-num)]
                     [break-status (send (get-tab) get-break-status)])
                (when (and (eq? frame-defs this) start end)
                  (let*-values ([(xl yl xr yr) (find-char-box this (sub1 start) start)]
                                [(ym) (average yl yr)]
                                [(xa ya xb yb) (find-char-box this (sub1 end) end)]
                                [(diameter) (- xb xa)]
                                [(yoff) (/ (- yb ya diameter) 2)]
                                [(ym2) (average ya yb)])
                    (let ([op (send dc get-pen)]
                          [ob (send dc get-brush)])
                      (cond
                        [(and (zero? frame-num)
                              (eq? break-status 'error))
                         (send dc set-pen pc-err-pen)
                         (send dc set-brush pc-err-brush)]
                        [(and (zero? frame-num)
                              (eq? break-status 'break))
                         (send dc set-pen pc-brk-pen)
                         (send dc set-brush pc-brk-brush)]
                        [(zero? frame-num)
                         (send dc set-pen pc-pen)
                         (send dc set-brush pc-brush)]
                        [else
                         (send dc set-pen pc-up-stack-pen)
                         (send dc set-brush pc-up-stack-brush)])
                      (unless (and (zero? frame-num) (cons? break-status))
                        ;; mark the beginning of the expression with a triangle
                        (send dc draw-polygon (list (make-object point% xl yl)
                                                    (make-object point% xl yr)
                                                    (make-object point% xr ym)) dx dy))
                      (if (and (zero? frame-num) (cons? break-status))
                          ;; top frame, end: mark the end of the expression with a triangle
                          (send dc draw-polygon (list (make-object point% xa ya)
                                                      (make-object point% xa yb)
                                                      (make-object point% xb ym2)) dx dy)
                          ;; otherwise: make the end of the expression with a circle
                          (send dc draw-ellipse (+ xa dx) (+ ya dy yoff) diameter diameter))
                      (send dc set-pen op)
                      (send dc set-brush ob)))))))
          
          (define/augment (after-set-next-settings s)
            (let ([tlw (get-top-level-window)])
              (when tlw
                (send tlw check-current-language-for-debugger)))
            (inner (void) after-set-next-settings s))))
      
      (define (debug-interactions-text-mixin super%)
        (class super%
          
          (inherit run-in-evaluation-thread
                   display-results)
          
          (super-instantiate ())
          
          (define debugged-thread #f)
          (define tab #f)
          (define/public (get-tab) tab)
          (define/public (set-tab t) (set! tab t))
          
          (define/private (stx-source->breakpoints src)
            (send (send (if src (filename->defs src) this) get-tab) get-breakpoints))
          
          (define/private (stx-source->pos-vec src)
            (send (send (if src (filename->defs src) this) get-tab) get-pos-vec))
          
          ;; make-debug-eval-handler : (sexp -> value) -> sexp -> value
          ;; adds debugging information to `sexp' and calls `oe'
          (define/private (make-debug-eval-handler oe break? break-before break-after)
            (lambda (orig-exp)
              (if (compiled-expression? (if (syntax? orig-exp)
                                            (syntax-e orig-exp)
                                            orig-exp))
                  (oe orig-exp)
                  (let* ([exp (if (syntax? orig-exp)
                                  orig-exp
                                  (namespace-syntax-introduce
                                   (datum->syntax-object #f orig-exp)))]
                         [top-e (expand-syntax-to-top-form exp)])
                    (parameterize ([current-eval oe])
                      (eval/annotations
                       top-e
                       ; annotate-module?
                       (lambda (fn m)
                         (cond
                           [(filename->defs fn)
                            =>
                            ; fn is loaded into defs
                            (lambda (defs)
                              (let ([extern-tab (send defs get-tab)]
                                    [this-tab (get-tab)])
                                (case (if (or (not (send extern-tab debug?))
                                              (eq? this-tab (send extern-tab get-master)))
                                          (message-box
                                           "Debugging Multi-File Program"
                                           (format "Debug ~a?" fn)
                                           #f
                                           '(yes-no))
                                          (message-box
                                           "Debugging Multi-File Program"
                                           (format "~a is already involved in a debugging session." fn)
                                           #f
                                           '(ok)))
                                  [(yes)
                                   ; set tab up with shared data from the master tab
                                   (send extern-tab prepare-execution #t)
                                   (send this-tab add-slave extern-tab)
                                   (call-with-values
                                    (lambda () (send this-tab get-shared-data))
                                    (lambda vals (send extern-tab set-shared-data . vals)))
                                   #t]
                                  [(no ok)
                                   (send extern-tab prepare-execution #f)
                                   #f])))]
                           ; fn is not open, so don't try to debug it
                           [else #f]))
                       ; annotator
                       (lambda (stx)
                         (let*-values ([(source) (syntax-source stx)]
                                       [(breakpoints) (stx-source->breakpoints source)]
                                       [(pos-vec) (stx-source->pos-vec source)]
                                       [(annotated break-posns)
                                        (annotate-for-single-stepping
                                         (expand-syntax stx)
                                         break?
                                         break-before
                                         break-after
                                         ; record-bound-identifier
                                         (lambda (type bound binding)
                                           ;(display-results (list bound))
                                           (cond
                                             [(filename->defs (robust-syntax-source bound))
                                              =>
                                              (lambda (defs)
                                                (let ([pos-vec (send (send defs get-tab) get-pos-vec)])
                                                  (let loop ([i 0])
                                                    (when (< i (syntax-span bound))
                                                      (safe-vector-set! pos-vec (+ i (syntax-position bound))
                                                                        binding)
                                                   (loop (add1 i))))))]
                                             [else (void)]))
                                         ; record-top-level-identifier
                                         (lambda (mod var rd/wr)
                                           ; filename->defs should succeed unless a slave tab gets closed
                                           (cond
                                             [(filename->defs (robust-syntax-source var))
                                              =>
                                              (lambda (defs)
                                                (send (send defs get-tab)
                                                      add-top-level-binding var rd/wr))]
                                             [else #;(printf "record-top-level failed for ~a~n" var) (void)])
                                           #;
                                           (printf "top-level binding: ~a ~a ~a~n" mod var rd/wr))
                                         source)])
                           (hash-table-for-each
                            breakpoints
                            (lambda (pos status)
                              ; possible efficiency problem for large files with many breakpoints
                              (when (and (syntax-position stx)
                                         (>= pos (syntax-position stx))
                                         (< pos (+ (syntax-position stx) (syntax-span stx)))
                                         (not (memq pos break-posns)))
                                (hash-table-remove! breakpoints pos))))
                           (for-each (lambda (posn)
                                       (hash-table-put!
                                        breakpoints posn
                                        (hash-table-get breakpoints posn (lambda () #f)))) break-posns)
                           annotated))))))))
          
          (define/override (reset-console)
            (super reset-console)
            (let ([tab (get-tab)])
              (when (and tab (send tab debug?))
                (let ([breakpoints (send tab get-breakpoints)])
                  (run-in-evaluation-thread
                   (lambda ()
                     ;(print-struct #t)
                     (let ([self (current-thread)]
                           [oeh (uncaught-exception-handler)]
                           [err-hndlr (error-display-handler)])
                       (set! debugged-thread self)
                       (error-display-handler
                        (lambda (msg exn)
                          (err-hndlr msg exn)
                          (if (and (eq? self (current-thread)) (exn:fail? exn))
                              (send (get-tab) suspend oeh
                                    (continuation-mark-set->list (exn-continuation-marks exn) debug-key)
                                    'error)))) ; this breaks the buttons because it looks like we can resume
                       (current-eval
                        (make-debug-eval-handler
                         (current-eval)
                         ; break? -- curried to avoid looking up defs from source each time
                         (lambda (src)
                           (let* ([defs (filename->defs src)]
                                  [src-tab (if defs (send defs get-tab) (get-tab))]
                                  [breakpoints
                                   (if src
                                       (send src-tab get-breakpoints)
                                       breakpoints)]
                                  [single-step? (send tab get-single-step-box)]
                                  [closed? (send src-tab get-closed-box)])
                             (lambda (pos)
                               (and (not (unbox closed?))
                                    (or (unbox single-step?)
                                        (let ([bp (hash-table-get breakpoints pos #f)])
                                          (if (procedure? bp)
                                              (bp)
                                              bp)))))))
                         ; break-before
                         (lambda (top-mark ccm)
                           (let* ([debug-marks (continuation-mark-set->list ccm debug-key)])
                             (send (get-tab) suspend oeh (cons top-mark debug-marks) 'entry-break)))
                         ; break-after
                         (case-lambda
                           [(top-mark ccm val)
                            (let* ([debug-marks (continuation-mark-set->list ccm debug-key)])
                              (car (send (get-tab) suspend oeh (cons top-mark debug-marks) (list 'exit-break val))))]
                           [(top-mark ccm . vals) 
                            (let* ([debug-marks (continuation-mark-set->list ccm debug-key)])
                              (apply values
                                     (send (get-tab) suspend oeh (cons top-mark debug-marks) (cons 'exit-break vals))))])))
                       (uncaught-exception-handler
                        (lambda (exn)
                          (if (and (exn:break? exn) (send (get-tab) suspend-on-break?))
                              (let ([marks (exn-continuation-marks exn)]
                                    [cont (exn:break-continuation exn)])
                                (send (get-tab) suspend oeh (continuation-mark-set->list marks debug-key) 'break)
                                (cont))
                              (oeh exn)))))))))))))
      
      (define (debug-tab-mixin super%)
        (class super%
          
          (inherit get-defs
                   get-ints
                   get-frame)
          
          (field [breakpoints (make-hash-table)]
                 [suspend-sema (make-semaphore 1)]
                 [resume-ch (make-channel)]
                 [in-user-ch (make-channel)]
                 [want-suspend-on-break? #f]
                 [want-debug? #f]
                 [master this]
                 [slaves empty]
                 [closed? (box #f)]
                 [stack-frames (box #f)]
                 [frame-num (box 0)]
                 [break-status (box #f)]
                 [current-language-settings #f]
                 [pos-vec (vector #f)]
                 [single-step? (box #t)]
                 [top-level-bindings empty]
                 [control-panel #f])
          
          (define/public (debug?) want-debug?)
          (define/public (get-master) master)
          (define/public (add-slave s)
            (set! slaves (cons s slaves)))
          (define/public (get-closed-box) closed?)
          (define/public suspend-on-break?
            (case-lambda
              [() want-suspend-on-break?]
              [(v) (set! want-suspend-on-break? v)]))
          (define/public (get-stack-frames)
            (unbox stack-frames))
          (define/public (get-pos-vec) pos-vec)
          (define/public (get-breakpoints) breakpoints)
          (define/public (get-break-status) (unbox break-status))
          (define/public (get-frame-num) (unbox frame-num))
          
          (define/public (set-shared-data bs sf sema res-ch usr-ch step? frame m)
            (set! break-status bs)
            (set! stack-frames sf)
            (set! suspend-sema sema)
            (set! resume-ch res-ch)
            (set! in-user-ch usr-ch)
            (set! single-step? step?)
            (set! frame-num frame)
            (set! master m))
          
          (define/public (get-shared-data)
            (values break-status stack-frames suspend-sema resume-ch in-user-ch single-step? frame-num master))
          
          (define/public (get-single-step-box) single-step?)
          (define/public (set-single-step?! v) (set-box! single-step? v))
          (define/public (set-break-status stat) (set-box! break-status stat))
          (define/public (add-top-level-binding var rd/wr)
            (set! top-level-bindings (cons (cons var rd/wr) top-level-bindings)))
          (define/public (lookup-top-level-var var failure-thunk)
            #;
            (printf "looking for ~a in ~a~n" var top-level-bindings)
            (let loop ([bindings top-level-bindings])
              (cond
                [(empty? bindings) (failure-thunk)]
                [(let ([res (or (bound-identifier=? var (caar bindings))
                                (free-identifier=? var (caar bindings)))])
                   #;
                   (printf "~a = ~a -> ~a~n" var (caar bindings) res)
                   res) (cdar bindings)]
                [else (loop (rest bindings))])))
          
          (define/public (move-to-frame the-frame-num)
            (set-box! frame-num the-frame-num)
            (suspend-gui (get-stack-frames) (get-break-status) #t))
          
          (define/public (resume)
            (let ([v (get-break-status)])
              (resume-gui)
              (channel-put resume-ch (and (pair? v) (cdr v)))))
          
          (define/public (set-mouse-over-msg msg)
            (send (get-frame) set-mouse-over-msg msg))
          
          (define/public (defs-containing-pc)
            (let ([stack-frames (get-stack-frames)])
              (and (cons? stack-frames)
                   (let* ([src-stx (mark-source (first stack-frames))]
                          [source (syntax-source src-stx)])
                     (if source
                         (filename->defs source)
                         (get-defs))))))
          
          (define/public (defs-containing-current-frame)
            (let ([stack-frames (get-stack-frames)])
              (and (cons? stack-frames)
                   (let* ([src-stx (mark-source (list-ref stack-frames (get-frame-num)))]
                          [source (syntax-source src-stx)])
                     (if source
                         (filename->defs source)
                         (get-defs))))))

          (define/public (get-pc)
            (let ([stack-frames (get-stack-frames)])
              (and (cons? stack-frames)
                   (let* ([src-stx (mark-source (first stack-frames))]
                          [start (syntax-position src-stx)]
                          [end (and start (+ start (syntax-span src-stx) -1))])
                     (if (cons? (get-break-status))
                         end
                         start)))))
          
          (define/public (get-frame-endpoints frame-num)
            (let ([stack-frames (get-stack-frames)])
              (and (cons? stack-frames)
                   (let* ([src-stx (mark-source (list-ref stack-frames frame-num))]
                          [start (syntax-position src-stx)]
                          [end (and start (+ start (syntax-span src-stx) -1))])
                     (list start end)))))
          
          (define/public (get-current-frame-endpoints)
            (get-frame-endpoints (get-frame-num)))
          
          (define (do-in-user-thread thunk)
            (if (get-break-status)
                (channel-put in-user-ch thunk)
                (send (get-ints) run-in-evaluation-thread thunk)))
          
          (define/public (render v)
            ;; ==drscheme eventspace thread==
            ;; only when a user thread is suspended
            (let ([result-ch (make-channel)]
                  [v (truncate-value v 100 5)])
              (do-in-user-thread
               (lambda ()
                 (let ([s (open-output-string)])
                   (send (drscheme:language-configuration:language-settings-language 
                          current-language-settings)
                         render-value
                         v
                         (drscheme:language-configuration:language-settings-settings
                          current-language-settings)
                         s)
                   (channel-put result-ch (get-output-string s)))))
              (channel-get result-ch)))
          
          (define/public (print-to-console v)
            ;; ==drscheme eventspace thread==
            ;; only when a user thread is suspended
            (do-in-user-thread (lambda () (fprintf (current-error-port) " ### DEBUGGER: ~a~n" v))))
          
          (define (frame->end-breakpoint-status frame)
            (let/ec k
              (let* ([stx (mark-source frame)]
                     [src (syntax-source stx)]
                     [pos (if (not (syntax-position stx))
                              (k 'invalid)
                              (+ (syntax-position stx) (syntax-span stx) -1))]
                     [defs (filename->defs src)]
                     [tab (if defs (send defs get-tab) (k (begin #;(printf "no defs for ~a~n" src) 'invalid)))]
                     [bps (send tab get-breakpoints)])
                (hash-table-get bps pos 'invalid))))
          
          (define (can-step-over? frames status)
            (and (or (not (zero? (get-frame-num))) (eq? status 'entry-break))
                 (not (eq? (frame->end-breakpoint-status (list-ref frames (get-frame-num))) 'invalid))))
          
          (define (can-step-out? frames status)
            (let ([frames (list-tail frames (get-frame-num))])
              (and (not (empty? frames))
                   (ormap (lambda (f) (not (eq? (frame->end-breakpoint-status f) 'invalid)))
                          (rest frames)))))

          (define (can-move-up-frame? frames)
            (< (get-frame-num) (sub1 (length frames))))
          
          (define (can-move-down-frame? frames)
            (> (get-frame-num) 0))

          (define/public suspend-gui
            (opt-lambda (frames status [switch-tabs? #f])
              (let ([top-of-stack? (zero? (get-frame-num))]
                    [status-message (send (get-frame) get-status-message)])
                (set! want-suspend-on-break? #f)
                (set-single-step?! #f)
                (set-box! stack-frames frames)
                (set-box! break-status status)
                (send (send (get-frame) get-pause-button) enable #f)
                (send (send (get-frame) get-step-button) enable top-of-stack?)
                (send (send (get-frame) get-step-over-button) enable (can-step-over? frames status))
                (send (send (get-frame) get-step-out-button) enable (can-step-out? frames status))
                (send (send (get-frame) get-resume-button) enable #t)
                (send (get-frame) register-stack-frames frames)
                ;;(fprintf (current-error-port) "break: ~a~n" (map expose-mark frames))
                ;;(printf "status = ~a~n" status)
                (send status-message set-label
                      (if (and (cons? status) top-of-stack?)
                          (let ([expr (mark-source (first frames))])
                            (cond
                              ; should succeed unless the user closes a slave tab during debugging
                              [(filename->defs (syntax-source expr))
                               =>
                               (lambda (defs)
                                 (clean-status
                                  (format "~a => ~a"
                                          (if (syntax-position expr)
                                              (trim-expr-str
                                               (send defs get-text
                                                     (sub1 (syntax-position expr))
                                                     (+ -1 (syntax-position expr) (syntax-span expr))))
                                              "??")
                                          (if (= 2 (length status))
                                              (render (cadr status))
                                              (cons 'values (map (lambda (v) (render v)) (rest status)))))))]
                              [""]))
                          ""))
                (cond [(get-current-frame-endpoints)
                       => (lambda (start/end)
                            (cond [(and (first start/end) (defs-containing-current-frame))
                                   => (lambda (defs)
                                        (cond
                                          [(and switch-tabs? (send defs get-filename))
                                           =>
                                           (lambda (fn)
                                             (handler:edit-file fn))])
                                        (send defs scroll-to-position (first start/end)))]))])
                (send (get-defs) invalidate-bitmap-cache))))
          
          (define/public (resume-gui)
            (set-box! stack-frames #f)
            (set-box! break-status #f)
            (set-box! frame-num 0)
            (send (send (get-frame) get-pause-button) enable #t)
            (send (send (get-frame) get-step-button) enable #f)
            (send (send (get-frame) get-step-over-button) enable #f)
            (send (send (get-frame) get-step-out-button) enable #f)
            (send (send (get-frame) get-resume-button) enable #f)
            (send (send (get-frame) get-status-message) set-label "")
            (send (get-frame) clear-stack-frames)
            (send (get-defs) invalidate-bitmap-cache))
          
          (define/public suspend
            ;; ==called from user thread==
            (opt-lambda (break-handler frames [status #f])
              ;; suspend-sema ensures that we allow only one suspended thread
              ;;  at a time
              (if (semaphore-try-wait? suspend-sema)
                  (begin
                    (parameterize ([current-eventspace (send (get-frame) get-eventspace)])
                      (queue-callback (lambda () (suspend-gui frames status #t))))
                    (with-handlers ([exn:break?
                                     (lambda (exn)
                                       (let ([wait-sema (make-semaphore)])
                                         (parameterize ([current-eventspace (send (get-frame) get-eventspace)])
                                           (queue-callback (lambda () 
                                                             (resume-gui)
                                                             (semaphore-post wait-sema))))
                                         (semaphore-wait wait-sema))
                                       (semaphore-post suspend-sema)
                                       (break-handler exn))])
                      (begin0
                        (let loop ()
                          (sync/enable-break resume-ch
                                             (handle-evt
                                              in-user-ch
                                              (lambda (thunk)
                                                (thunk)
                                                (loop)))))
                        (semaphore-post suspend-sema))))
                  (if (pair? status)
                      (cdr status)
                      #f))))
          
          (define/public (prepare-execution debug?)
            (set! want-debug? debug?)
            (if debug?
                (send (get-frame) show-debug)
                (begin
                  (send (get-frame) hide-debug)
                  (set! master this)
                  (for-each
                   (lambda (t) (send t prepare-execution #f))
                   slaves)
                  (set! slaves empty)))
            (set! current-language-settings
                  (and debug? (send (get-defs) get-next-settings)))
            (set! single-step? (box #t))
            (set! pos-vec (make-vector (add1 (send (get-defs) last-position)) #f))
            (set! top-level-bindings empty)
            (set! resume-ch (make-channel))
            (set! suspend-sema (make-semaphore 1))
            (set! in-user-ch (make-channel))
            (set! break-status (box #f))
            (set! want-suspend-on-break? #f)
            (set! stack-frames (box #f))
            (send (get-ints) set-tab this))
          
          (define/augment (on-close)
            (inner (void) on-close)
            (set-box! closed? #t)
            (for-each
             (lambda (t) (send t prepare-execution #f))
             slaves))
          
          (define/public (hide-debug)
            (send (get-frame) hide-debug))
          
          (define/override (enable-evaluation)
            (send (send (get-frame) get-debug-button) enable #t)
            (super enable-evaluation))
          
          (define/override (disable-evaluation)
            (send (send (get-frame) get-debug-button) enable #f)
            (super disable-evaluation))
          
          (super-new)))
      
      (define (debug-unit-frame-mixin super%)
        (class super%
          
          (inherit get-button-panel
                   get-definitions-text
                   get-interactions-text
                   get-menu-bar
                   get-current-tab
                   get-top-level-window
                   get-eventspace)
          
          (define current-language-settings #f)
          (define control-panel #f)
          (define debug? #f)
          (define/public (set-mouse-over-msg msg)
            (when (not (string=? msg (send mouse-over-message get-label)))
              (send mouse-over-message set-label msg)))
          
          (define/override execute-callback
            (lambda ()
              (let* ([tab (get-current-tab)])
                (if (eq? tab (send tab get-master))
                    (begin
                      (send (get-current-tab) prepare-execution debug?)
                      (set! debug? #f)
                      (super execute-callback))
                    (message-box
                     "Message from Debugger"
                     (format "This file is involved in a debugging session.  To run/debug this file, finish the session for ~a and close or re-run it."
                             (send (send (send tab get-master) get-defs) get-filename/untitled-name))
                     this
                     '(ok))))))
          
          (define max-stack-frames 40)
          
          (define/public (register-stack-frames frames)
            (let loop ([i 0] [frames frames])
              (when (< i max-stack-frames)
                (send (list-ref stack-frames i) set-expr
                      (and (cons? frames) (mark-source (first frames))))
                (loop (add1 i) (and (cons? frames) (rest frames))))))
          
          (define/public (clear-stack-frames)
            (let loop ([i 0])
              (when (< i max-stack-frames)
                (send (list-ref stack-frames i) set-expr #f)
                (loop (add1 i)))))
          
          (define debug-grandparent-panel 'uninitialized-debug-grandparent-panel)
          (define debug-parent-panel 'uninitialized-debug-parent-panel)
          (define debug-panel 'uninitialized-debug-panel)
          (define stack-view-panel 'uninitialized-stack-view-panel)
          (define stack-frames 'uninitialized-stack-frames)
          (define highlight-color (make-object color% 207 255 207))
          (define mouse-over-frame #f)
          (define refresh-timer (new timer% [notify-callback
                                             (lambda ()
                                               (unless mouse-over-frame
                                                 (send (get-current-tab) move-to-frame
                                                       (send (get-current-tab) get-frame-num))))]))
          (define/override (get-definitions/interactions-panel-parent)
            (set! debug-grandparent-panel
                  (make-object horizontal-panel%
                    (super get-definitions/interactions-panel-parent)))
            (set! stack-view-panel
                  (new vertical-panel%
                       [parent debug-grandparent-panel]
                       [stretchable-width #f]))
            (new message% [parent stack-view-panel] [label "Context"]
                 [font (make-object font% 18 'default 'normal 'bold #t)])
            (set! stack-frames
                  (build-list max-stack-frames
                              (lambda (i)
                                (new (class message%
                                       (super-new)
                                       (inherit set-label get-label)
                                       (define expr #f)
                                       (define/public (set-expr expr1)
                                         (set! expr expr1)
                                         (set-label
                                          (cond
                                            ; should succeed unless the user closes a slave tab during debugging
                                            [(and expr (filename->defs (syntax-source expr)))
                                             =>
                                             (lambda (defs)
                                               (clean-status
                                                (if (syntax-position expr)
                                                    (send defs get-text
                                                          (sub1 (syntax-position expr))
                                                          (+ -1 (syntax-position expr) (syntax-span expr)))
                                                    "??")))]
                                            [""])))
                                       (define/override (on-subwindow-event msg evt)
                                         (case (send evt get-event-type)
                                           [(enter) (cond
                                                      [(and expr (filename->defs (syntax-source expr)))
                                                       => (lambda (defs)
                                                            (send defs highlight-range
                                                                  (sub1 (syntax-position expr))
                                                                  (+ -1 (syntax-position expr)
                                                                     (syntax-span expr))
                                                                  highlight-color)
                                                            (cond
                                                              [(send defs get-filename)
                                                               =>
                                                               (lambda (fn)
                                                                 (handler:edit-file fn))])
                                                            (send defs scroll-to-position
                                                                  (syntax-position expr)))])
                                                    (when expr
                                                      (set! mouse-over-frame i))]
                                           [(leave) (cond
                                                      [(and expr (filename->defs (syntax-source expr)))
                                                       => (lambda (defs)
                                                            (send defs unhighlight-range
                                                                  (sub1 (syntax-position expr))
                                                                  (+ -1 (syntax-position expr)
                                                                     (syntax-span expr))
                                                                  highlight-color))])
                                                    (set! mouse-over-frame #f)
                                                    (when expr
                                                      (send refresh-timer start 250 #t))]
                                           [(left-down) (when expr
                                                          (send (get-current-tab) move-to-frame i))])))
                                     [parent stack-view-panel]
                                     [stretchable-width #f]
                                     [label (make-string 30 #\ )]))))
            ;; parent of panel with debug buttons
            (set! debug-parent-panel
                  (make-object vertical-panel% debug-grandparent-panel))
            ;; horizontal panel with debug buttons; not vertically stretchable
            (set! debug-panel (instantiate horizontal-panel% ()
                                (parent debug-parent-panel)
                                (stretchable-height #f)
                                (alignment '(center center))
                                (style '(border))))
            ;; hide the debug panel and stack view initially
            (send debug-parent-panel change-children (lambda (l) null))
            (send debug-grandparent-panel change-children (lambda (l) (remq stack-view-panel l)))
            (make-object vertical-panel% debug-parent-panel))
          
          (define/public (hide-debug)
            (when (member debug-panel (send debug-parent-panel get-children))
              (send debug-grandparent-panel change-children
                    (lambda (l) (remq stack-view-panel l)))
              (send debug-parent-panel change-children
                    (lambda (l) (remq debug-panel l)))))
          
          (define/public (show-debug)
            (unless (member debug-panel (send debug-parent-panel get-children))
              (send debug-grandparent-panel change-children
                    (lambda (l) (append l (list stack-view-panel))))
              (send debug-parent-panel change-children
                    (lambda (l) (cons debug-panel l)))))
          
          (super-new)
          
          (define status-message
            (instantiate message% ()
              [label " "]
              [parent debug-panel]
              [stretchable-width #t]))
          
          (define debug-button
            (make-object button%
              ((bitmap-label-maker
                (string-constant debug-tool-button-name)
                (build-path (collection-path "gui-debugger" "icons") "icon-small.png")) this)
              (make-object vertical-pane% (get-button-panel))
              (lambda (button evt) (set! debug? #t) (execute-callback))))
          
          (define pause-button
            (instantiate button% ()
              [label ((bitmap-label-maker
                       "Pause"
                       (build-path (collection-path "gui-debugger" "icons") "pause.png")) this)]
              [parent debug-panel]
              [callback (lambda (button evt)
                          (if (send (get-current-tab) get-stack-frames)
                              (bell)
                              (begin
                                (send (get-current-tab) suspend-on-break? #t)
                                (send (get-current-tab) break-callback)
                                (send (get-current-tab) reset-offer-kill))))]
              [enabled #t]))
          
          (define resume-button
            (instantiate button% ()
              [label ((bitmap-label-maker
                       "Go"
                       (build-path (collection-path "gui-debugger" "icons") "resume.png")) this)]
              [parent debug-panel]
              [callback (lambda (button evt)
                          (if (send (get-current-tab) get-stack-frames)
                              (send (get-current-tab) resume)
                              (bell)))]
              [enabled #f]))
          
          (define step-button
            (instantiate button% ()
              [label ((bitmap-label-maker
                       "Step"
                       (build-path (collection-path "gui-debugger" "icons") "step.png")) this)]
              [parent debug-panel]
              [callback (lambda (btn evt)
                          (if (send (get-current-tab) get-stack-frames)
                              (begin
                                (send (get-current-tab) set-single-step?! #t)
                                (send (get-current-tab) resume))
                              (bell)))]
              [enabled #f]))
          
          (define (make-big-step-callback out?)
            (lambda (btn evt)
              ; go through stack frames until it's possible to set a breakpoint at the end
              (let* ([frames (list-tail (send (get-current-tab) get-stack-frames)
                                        (send (get-current-tab) get-frame-num))]
                     [frames (case (send (get-current-tab) get-break-status)
                               [(entry-break) (if out? (rest frames) frames)]
                               [else (if out? (rest frames) empty)])]
                     [frame (ormap (lambda (f depth)
                                     (let/ec k
                                       (let* ([stx (mark-source f)]
                                              [src (syntax-source stx)]
                                              [lpos (or (syntax-position stx) (k #f))]
                                              [pos (+ lpos (syntax-span stx) -1)]
                                              [defs (filename->defs src)]
                                              [tab (if defs (send defs get-tab) (k (begin #;(printf "no defs for ~a~n" src) #f)))]
                                              [bps (send tab get-breakpoints)]
                                              [cur-stat (hash-table-get bps pos 'invalid)])
                                         ;(printf "stat for ~a in ~a is ~a~n" pos src cur-stat)
                                         (case cur-stat
                                           [(invalid) #f]
                                           [else
                                            (hash-table-put!
                                             bps pos
                                             (lambda ()
                                               (and (< (length (continuation-mark-set->list (current-continuation-marks) debug-key)) depth)
                                                    (begin
                                                      (hash-table-put! bps pos cur-stat)
                                                      #t))))
                                            f]))))
                                   frames
                                   (let ([len (length frames)])
                                     (build-list len (lambda (i) (- len i)))))])
                (if frames
                    (begin
                      (send (get-current-tab) set-single-step?! (not frame))
                      (send (get-current-tab) resume))
                    (bell)))))
          
          (define step-over-button
            (new button%
                 [label ((bitmap-label-maker
                          "Over"
                          (build-path (collection-path "gui-debugger" "icons") "step-over2.png")) this)]
                 [parent debug-panel]
                 [callback (make-big-step-callback #f)]
                 [enabled #f]))
          
          (define step-out-button
            (new button%
                 [label ((bitmap-label-maker
                          "Out"
                          (build-path (collection-path "gui-debugger" "icons") "step-out2.png")) this)]
                 [parent debug-panel]
                 [callback (make-big-step-callback #t)]
                 [enabled #f]))
          
          (define/public (get-debug-button) debug-button)
          (define/public (get-pause-button) pause-button)
          (define/public (get-resume-button) resume-button)
          (define/public (get-step-button) step-button)
          (define/public (get-step-over-button) step-over-button)
          (define/public (get-step-out-button) step-out-button)
          (define/public (get-status-message) status-message)
          
          (define mouse-over-message
            (instantiate message% ()
              [label " "]
              [parent debug-panel]
              [stretchable-width #t]))
          
          (define/augment (on-tab-change old new)
            (check-current-language-for-debugger)
            (if (send new debug?)
                (let ([status (send new get-break-status)])
                  (if status
                      (send new suspend-gui (send new get-stack-frames) status)
                      (send new resume-gui))
                  (show-debug))
                (hide-debug))
            (inner (void) on-tab-change old new))
          
          (define/public (check-current-language-for-debugger)
            (let* ([settings (send (get-definitions-text) get-next-settings)]
                   [lang (drscheme:language-configuration:language-settings-language settings)]
                   [visible? (and (send lang capability-value 'gui-debugger:debug-button)
                                  (not (debugger-does-not-work-for?
                                        (extract-language-level settings))))])
              (if visible?
                  (unless (send debug-button is-shown?)
                    (send (send debug-button get-parent) add-child debug-button))
                  (when (send debug-button is-shown?)
                    (send (send debug-button get-parent) delete-child debug-button)))))
          
          (send (get-button-panel) change-children
                (lambda (_)
                  (cons (send debug-button get-parent)
                        (remq (send debug-button get-parent) _))))
          
          ; hide debug button if it's not supported for the initial language:
          (check-current-language-for-debugger)))
      (drscheme:language:register-capability 'gui-debugger:debug-button (flat-contract boolean?) #t)
      (drscheme:get/extend:extend-definitions-text debug-definitions-text-mixin)
      (drscheme:get/extend:extend-interactions-text debug-interactions-text-mixin)
      (drscheme:get/extend:extend-unit-frame debug-unit-frame-mixin)
      (drscheme:get/extend:extend-tab debug-tab-mixin))))
