#lang racket/base

(require racket/class racket/port racket/list racket/match
         racket/gui/base racket/unit drracket/tool
         images/compile-time
         (for-syntax racket/base images/icons/misc images/icons/style))

(require "report.rkt" "display.rkt")

(provide performance-report-drracket-button
         tool@)

;; DrRacket tool for reporting missed optimizations in the editor.

(define performance-report-bitmap
  (compiled-bitmap (stopwatch-icon (toolbar-icon-height))))

;; performance-report-callback : drracket:unit:frame<%> -> void
(define (performance-report-callback drr-frame)
  (with-handlers
      ([exn?
        ;; typechecking failed, report in the interactions window
        (lambda (e)
          (define interactions (send drr-frame get-interactions-text))
          (send interactions reset-console)
          (send interactions run-in-evaluation-thread (lambda () (raise e))))])
    (send (send drr-frame get-definitions-text) add-highlights)))

(define highlights-mixin
  (mixin ((class->interface text%)) ()
    (inherit begin-edit-sequence
             end-edit-sequence
             insert
             get-text)

    (define highlights  '())
    (define color-table #f)

    (define/private (highlight-entry l)
      (match l
        [(report-entry subs start end badness)
         (let ([color (if (= badness 0)
                          "lightgreen"
                          (vector-ref color-table badness))])
           (send this highlight-range start end color #f 'high)
           (send this set-clickback start end (popup-callback l))
           ;; record highlight to undo it later
           (list start end color))]))

    (define/public (add-highlights)
      (define report (generate-report this))
      (define max-badness
        (apply max (cons 0 (map report-entry-badness report))))
      (unless (= max-badness 0) ; no missed opts, color table code would error
        (set! color-table (make-color-table max-badness)))
      (define new-highlights 
        (let loop ([report report])
          (cond
            [(null? report) highlights]
            [else (cons (highlight-entry (car report))
                        (loop (cdr reports)))])))
      (set! highlights new-highlights))

    (define/private (clear-highlights)
      (for ([h (in-list highlights)])
        (match h
          [`(,start ,end . ,rest )
           (send this unhighlight-range . h)
           (send this remove-clickback start end)]))
      (set! highlights '()))

    (define/augment (on-insert start len)
      (clear-highlights))
    (define/augment (on-delete start len)
      (clear-highlights))

    (super-new)))

(define-unit tool@
  (import drracket:tool^)
  (export drracket:tool-exports^)
  (define (phase1) (void))
  (define (phase2) (void))
  (drracket:get/extend:extend-definitions-text highlights-mixin))

(define performance-report-drracket-button
  (list
   "Performance Report"
   performance-report-bitmap
   performance-report-callback))
