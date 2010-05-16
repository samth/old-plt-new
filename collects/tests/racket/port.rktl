
(load-relative "loadtest.rktl")

(Section 'port)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Tests for progress events and commits

(define (test-hello-port p commit-eof?)
  (let ([progress-evt1 (port-progress-evt p)])
    (test #t evt? progress-evt1)
    (test #f sync/timeout 0 progress-evt1)
    (test #f sync/timeout 0 progress-evt1)
    (test #\h read-char p)
    (test progress-evt1 sync/timeout 0 progress-evt1)
    (let ([progress-evt2 (port-progress-evt p)])
      (test #f sync/timeout 0 progress-evt2)
      (test #\e peek-char p)
      (test #f sync/timeout 0 progress-evt2)
      (test #f port-commit-peeked 1 progress-evt1 always-evt p)
      (test #t port-commit-peeked 1 progress-evt2 always-evt p)
      (test progress-evt1 sync/timeout 0 progress-evt1)
      (test progress-evt2 sync/timeout 0 progress-evt2)
      (let ([progress-evt3 (port-progress-evt p)])
	(test #\l peek-char p)
	(test #\l peek-char p 1)
	(test #\o peek-char p 2)
	(test eof peek-char p 3)
	(test eof peek-char p 4)
	(test #t port-commit-peeked 2 progress-evt3 always-evt p)
	(test #\o peek-char p)
	(test eof peek-char p 1)
	(let ([progress-evt4 (port-progress-evt p)])
	  (test #f sync/timeout 0 progress-evt4)
	  (test #t port-commit-peeked (if commit-eof? 2 1) progress-evt4 always-evt p)
	  (test progress-evt4 sync/timeout 0 progress-evt4)
	  (let ([progress-evt5 (port-progress-evt p)])
	    (test #f sync/timeout 0 progress-evt5)
	    (test eof peek-char p)
	    (test eof read-char p)
	    (test #f sync/timeout 0 progress-evt5)
	    (test #f port-commit-peeked 1 progress-evt5 always-evt p)))))))

(test-hello-port (open-input-string "hello") #f)
(test-hello-port (open-input-string "hello") #t)
(let ([test-pipe
       (lambda (commit-eof?)
	 (let-values ([(r w) (make-pipe)])
	   (write-string "hello" w)
	   (close-output-port w)
	   (test-hello-port r commit-eof?)))])
  (test-pipe #f)
  (test-pipe #t))
(let ([test-file
       (lambda (commit-eof?)
	 (let ([p (begin
		    (with-output-to-file "tmp8"
		      (lambda ()
			(write-string "hello"))
                      #:exists 'truncate/replace)
		    (open-input-file "tmp8"))])
	   (test-hello-port p commit-eof?)
	   (close-input-port p)))])
  (test-file #f)
  (test-file #t))

(let-values ([(r w) (make-pipe)])
  (write-byte 200 w)
  (test #t byte-ready? r)
  (test #f char-ready? r))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Based on the Racket manual...

;; A port with no input...
;; Easy: \scheme{(open-input-bytes #"")}
;; Hard:
(define /dev/null-in 
  (make-input-port 'null
		   (lambda (s) eof)
		   (lambda (skip s progress-evt) eof)
		   void
		   (lambda () never-evt)
		   (lambda (k progress-evt done-evt)
		     (error "no successful peeks!"))))
(test eof read-char /dev/null-in)
(test eof peek-char /dev/null-in)
(test eof read-byte-or-special /dev/null-in)
(test eof peek-byte-or-special /dev/null-in 100)
(test #t evt? (port-progress-evt /dev/null-in))
(test #f sync/timeout 0 (port-progress-evt /dev/null-in))
(err/rt-test (port-commit-peeked 100 (port-progress-evt /dev/null-in) always-evt /dev/null-in)
	     exn:fail?)
(err/rt-test (port-commit-peeked 100 never-evt always-evt /dev/null-in))

;; A port that produces a stream of 1s:
(define infinite-ones 
  (make-input-port
   'ones
   (lambda (s) 
     (bytes-set! s 0 (char->integer #\1)) 1)
   #f
   void))
(test "11111" read-string 5 infinite-ones)

;; An infinite stream of 1s with a specific peek procedure:
(define infinite-ones 
  (let ([one! (lambda (s) 
	        (bytes-set! s 0 (char->integer #\1)) 1)])
    (make-input-port
     'ones
     one!
     (lambda (s skip progress-evt) (one! s))
     void)))
(test "11111" read-string 5 infinite-ones)

;; Now we can peek ahead arbitrarily far:
(test "11111" peek-string 5 (expt 2 5000) infinite-ones)

;; The port doesn't supply procedures to implement progress events:
(test #f port-provides-progress-evts? infinite-ones)
(err/rt-test (port-progress-evt infinite-ones) exn:application:mismatch?)

;; This port produces 0, 1, 2, 0, 1, 2, etc,
;;  but it is not thread-safe, because multiple
;;  threads might read and change n
(define mod3-cycle/one-thread
  (let* ([n 2]
	 [mod! (lambda (s delta)
		 (bytes-set! s 0 (+ 48 (modulo (+ n delta) 3)))
                 1)])
    (make-input-port
     'mod3-cycle/not-thread-safe
     (lambda (s) 
       (set! n (modulo (add1 n) 3))
       (mod! s 0))
     (lambda (s skip progress-evt) 
       (mod! s skip))
     void)))
(test "01201" read-string 5 mod3-cycle/one-thread)
(test "20120" peek-string 5 (expt 2 5000) mod3-cycle/one-thread)

;; Same thing, but thread-safe and kill-safe, and with progress
;; events. Only the server thread touches the stateful part
;; directly. (See the output port examples for a simpler thread-safe
;; example, but this one is more general.)
(define (make-mod3-cycle)
  (define read-req-ch (make-channel))
  (define peek-req-ch (make-channel))
  (define progress-req-ch (make-channel))
  (define commit-req-ch (make-channel))
  (define close-req-ch (make-channel))
  (define closed? #f)
  (define n 0)
  (define progress-sema #f)
  (define (mod! s delta)
    (bytes-set! s 0 (+ 48 (modulo (+ n delta) 3)))
    1)
  (define (remq v l)
    (if (eq? (car l) v)
	(cdr l)
	(cons (car l) (remq v (cdr l)))))
  ;; ----------------------------------------
  ;; The server has a list of outstanding commit requests,
  ;;  and it also must service each port operation (read, 
  ;;  progress-evt, etc.)
  (define (serve commit-reqs response-evts)
    (apply
     sync
     (handle-evt read-req-ch (handle-read commit-reqs response-evts))
     (handle-evt progress-req-ch (handle-progress commit-reqs response-evts))
     (handle-evt commit-req-ch (add-commit commit-reqs response-evts))
     (handle-evt close-req-ch (handle-close commit-reqs response-evts))
     (append
      (map (make-handle-response commit-reqs response-evts) response-evts)
      (map (make-handle-commit commit-reqs response-evts) commit-reqs))))
  ;; Read/peek request: fill in the string and commit
  (define ((handle-read commit-reqs response-evts) r)
    (let ([s (car r)]
	  [skip (cadr r)]
	  [ch (caddr r)]
	  [nack (cadddr r)]
	  [peek? (car (cddddr r))]
	  [progress-evt (cdr (cddddr r))])
      (let ([nothing? (or closed?
			  (and progress-evt
			       (sync/timeout 0 progress-evt)))])
	(unless nothing?
	  (mod! s skip)
	  (unless peek?
	    (commit! 1)))
	;; Add an event to respond:
	(serve commit-reqs
	       (cons (choice-evt nack
				 (channel-put-evt ch (if nothing? 
							 #f
							 (if closed? 0 1))))
		     response-evts)))))
  ;; Progress request: send a peek evt for the current 
  ;;  progress-sema
  (define ((handle-progress commit-reqs response-evts) r)
    (let ([ch (car r)]
	  [nack (cdr r)])
      (unless progress-sema
	(set! progress-sema (make-semaphore (if closed? 1 0))))
      ;; Add an event to respond:
      (serve commit-reqs
	     (cons (choice-evt nack
			       (channel-put-evt
				ch
				(semaphore-peek-evt progress-sema)))
		   response-evts))))
  ;; Commit request: add the request to the list
  (define ((add-commit commit-reqs response-evts) r)
    (serve (cons r commit-reqs) response-evts))
  ;; Commit handling: watch out for progress, in which case
  ;;  the response is a commit failure; otherwise, try
  ;;  to sync for a commit. In either event, remove the
  ;;  request from the list
  (define ((make-handle-commit commit-reqs response-evts) r)
    (let ([k (car r)]
	  [progress-evt (cadr r)]
	  [done-evt (caddr r)]
	  [ch (cadddr r)]
	  [nack (cddddr r)])
      ;; Note: we don't check that k is $\leq$ the sum of
      ;;  previous peeks, because the entire stream is actually
      ;;  known, but we could send an exception in that case.
      (choice-evt
       (handle-evt progress-evt
		   (lambda (x) 
		     (sync nack (channel-put-evt ch #f))
		     (serve (remq r commit-reqs) response-evts)))
       ;; Only create an event to satisfy done-evt if progress-evt
       ;;  isn't already ready.
       ;; Afterward, if progress-evt becomes ready, then this
       ;;  event-making function will be called again.) We know this
       ;;  only because the server control all posts to progress-evt.
       (if (sync/timeout 0 progress-evt)
	   never-evt
	   (handle-evt done-evt
		       (lambda (v)
			 (commit! k)
			 (sync nack (channel-put-evt ch #t))
			 (serve (remq r commit-reqs) response-evts)))))))
  ;; Response handling: as soon as the respondee listerns,
  ;;  remove the response
  (define ((make-handle-response commit-reqs response-evts) evt)
    (handle-evt evt
		(lambda (x)
		  (serve commit-reqs
			 (remq evt response-evts)))))
  ;; Close handling: post the progress sema, if any, and set
  ;;   the \scheme{closed?} flag
  (define ((handle-close commit-reqs response-evts) r)
    (let ([ch (car r)]
	  [nack (cdr r)])
      (set! closed? #t)
      (when progress-sema
	(semaphore-post progress-sema))
      (serve commit-reqs
	     (cons (choice-evt nack
			       (channel-put-evt ch (void)))
		   response-evts))))
  ;; Helper for reads and post-peek commits:
  (define (commit! k)
    (when progress-sema
      (semaphore-post progress-sema)
      (set! progress-sema #f))
    (set! n (+ n k)))
  ;; Start the server thread:
  (define server-thread (thread (lambda () (serve null null))))
  ;; ----------------------------------------
  ;; Client-side helpers:
  (define (req-evt f)
    (nack-guard-evt
     (lambda (nack)
       ;; Be sure that the server thread is running:
       (thread-resume server-thread (current-thread))
       ;; Create a channel to hold the reply:
       (let ([ch (make-channel)])
	 (f ch nack)
	 ch))))
  (define (read-or-peek-evt s skip peek? progress-evt)
    (req-evt (lambda (ch nack)
	       (channel-put read-req-ch (list* s skip ch nack peek? progress-evt)))))
  ;; Make the port:
  (make-input-port 'mod3-cycle
		   ;; Each handler for the port just sends
		   ;;  a request to the server
		   (lambda (s) (read-or-peek-evt s 0 #f #f))
		   (lambda (s skip progress-evt) (read-or-peek-evt s skip #t progress-evt))
		   (lambda () ; close
		     (sync (req-evt
			    (lambda (ch nack)
			      (channel-put progress-req-ch (list* ch nack))))))
		   (lambda () ; progress-evt
		     (sync (req-evt
			    (lambda (ch nack)
			      (channel-put progress-req-ch (list* ch nack))))))
		   (lambda (k progress-evt done-evt)  ; commit
		     (sync (req-evt
			    (lambda (ch nack)
			      (channel-put commit-req-ch
					   (list* k progress-evt done-evt ch nack))))))))

(let ([mod3-cycle (make-mod3-cycle)])
  (port-progress-evt mod3-cycle)
  (let ([result1 #f]
	[result2 #f])
    (let ([t1 (thread (lambda ()
			(set! result1 (read-string 5 mod3-cycle))))]
	  [t2 (thread (lambda ()
			(set! result2 (read-string 5 mod3-cycle))))])
      (thread-wait t1)
      (thread-wait t2)
      (test 11 string-length (string-append result1 "," result2))))
  (let ([s (make-bytes 1)]
	[progress-evt (port-progress-evt mod3-cycle)])
    (test 1 peek-bytes-avail! s 0 progress-evt mod3-cycle)
    (test #"1" values s)
    (test #t 
	  port-commit-peeked 1 progress-evt (make-semaphore 1)
	  mod3-cycle)
    (test #t evt? (sync/timeout 0 progress-evt))
    (test 0 peek-bytes-avail! s 0 progress-evt mod3-cycle)
    (test #f 
	  port-commit-peeked 1 progress-evt (make-semaphore 1) 
	  mod3-cycle))
  (close-input-port mod3-cycle))

;; Non-byte port results:
(define infinite-voids
  (make-input-port
   'voids
   (lambda (s) (lambda args 'void))
   (lambda (skip s progress-evt) (lambda args 'void))
   void))
(err/rt-test (read-char infinite-voids) exn:application:mismatch?)
(test 'void read-byte-or-special infinite-voids)
(test 'void read-char-or-special infinite-voids)
(let ([go
       (lambda (get-avail!)
	 (define (get)
	   (if (procedure-arity-includes? get-avail! 1)
	       (get-avail! (make-bytes 10) infinite-voids)
	       (get-avail! (make-bytes 10) 0 #f infinite-voids)))
	 (let ([p (get)])
	   (test #t procedure? p)
	   (test 4 procedure-arity p)
	   (test 'void p 'apple 1 0 1)
	   (err/rt-test (p 'apple 1 0 1) exn:fail:contract?))
	 (let ([p (get)])
	   (err/rt-test (p 'apple 0 0 1))
	   (err/rt-test (p 'apple 1 0 0))
	   (err/rt-test (p 'apple 1 -1 1))))])
  (go read-bytes-avail!)
  (go read-bytes-avail!*)
  (go read-bytes-avail!/enable-break)
  (go peek-bytes-avail!)
  (go peek-bytes-avail!*)
  (go peek-bytes-avail!/enable-break))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Also based on the Racket manual...

(define should-be-breakable? #t)

(define /dev/null-out
  (make-output-port 
   'null
   always-evt
   (lambda (s start end non-block? breakable?)
     (test should-be-breakable? 'breakable breakable?)
     (- end start))
   void
   (lambda (special non-block? breakable?) 
     (test should-be-breakable? 'spec-breakable breakable?)
     #t)
   (lambda (s start end) (wrap-evt
			  always-evt
			  (lambda (x)
			    (- end start))))
   (lambda (special) always-evt)))
(test (void) display "hello" /dev/null-out)
(test 5 write-bytes-avail #"hello" /dev/null-out)
(test #t write-special 'hello /dev/null-out)
(test 5 sync (write-bytes-avail-evt #"hello" /dev/null-out))
(test 5 write-bytes-avail/enable-break #"hello" /dev/null-out)
(test #t write-special-avail* 'hello /dev/null-out)
(parameterize-break #f
  (test 5 write-bytes-avail/enable-break #"hello" /dev/null-out)
  (set! should-be-breakable? #f)
  (test #t write-special-avail* 'hello /dev/null-out)
  (test 5 write-bytes-avail #"hello" /dev/null-out))

;; A part that accumulates bytes as characters in a list,
;;  but not in a thread-safe way:
(define accum-list null)
(define accumulator/not-thread-safe
  (make-output-port 
   'accum/not-thread-safe
   always-evt
   (lambda (s start end non-block? breakable?)
     (set! accum-list
           (append accum-list
                   (map integer->char
                        (bytes->list (subbytes s start end)))))
     (- end start))
   void))
(display "hello" accumulator/not-thread-safe)
(test '(#\h #\e #\l #\l #\o) values accum-list)

;; Same as before, but with simple thread-safety:
(define accum-list null)
(define accumulator 
  (let* ([lock (make-semaphore 1)]
	 [lock-peek-evt (semaphore-peek-evt lock)])
    (make-output-port
     'accum
     lock-peek-evt
     (lambda (s start end non-block? breakable?)
       (let loop ()
	 (if (semaphore-try-wait? lock)
	     (begin
	       (set! accum-list
		     (append accum-list
			     (map integer->char
				  (bytes->list (subbytes s start end)))))
	       (semaphore-post lock)
	       (- end start))
	     ;; Cheap strategy: block until the list is unlocked,
	     ;;   then try again
	     (wrap-evt
	      lock-peek
	      (lambda (x) (loop))))))
     void)))
(display "hello" accumulator)
(test '(#\h #\e #\l #\l #\o) values accum-list)

;; A port that transforms data before sending it on
;;  to another port. Atomic writes exploit the
;;  underlying port's ability for atomic writes.
(define (make-latin-1-capitalize port)
  (define (byte-upcase s start end)
    (list->bytes
     (map (lambda (b) (char->integer
		       (char-upcase
			(integer->char b))))
	  (bytes->list (subbytes s start end)))))
  (make-output-port
   'byte-upcase
   ;; This port is ready when the original is ready:
   port
   ;; Writing procedure:
   (lambda (s start end non-block? breakable?)
     (let ([s (byte-upcase s start end)])
       (if non-block?
           (write-bytes-avail* s port)
           (begin
             (display s port)
             (bytes-length s)))))
   ;; Close procedure --- close original port:
   (lambda () (close-output-port port))
   #f
   ;; Write event:
   (and (port-writes-atomic? port)
	(lambda (s start end)
	  (write-bytes-avail-evt (byte-upcase s start end) port)))))
(define orig-port (open-output-string))
(define cap-port (make-latin-1-capitalize orig-port))
(display "Hello" cap-port)
(test "HELLO" get-output-string orig-port)
(test 3 sync (write-bytes-avail-evt #"Bye" cap-port))
(test "HELLOBYE" get-output-string orig-port)

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Peeking in a limited pipe extends the limit:

(let-values ([(in out) (make-pipe 3)])
  (test 3 write-bytes-avail #"12345" out)
  (test #f sync/timeout 0 out)
  (test #\1 peek-char in)
  (test out sync/timeout 0 out)
  (test 1 write-bytes-avail #"12345" out)
  (test #f sync/timeout 0 out)
  (test #\1 peek-char in)
  (test 0 write-bytes-avail* #"12345" out)
  (test #\2 peek-char in 1)
  (test 1 write-bytes-avail* #"12345" out)
  (let ([s (make-bytes 6 (char->integer #\-))])
    (test 5 read-bytes-avail! s in)
    (test #"12311-" values s))
  (test 3 write-bytes-avail #"1234" out))


;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Provide a location proc:

(let ([mk
       (lambda (adjust-locs)
	 (let ([p (open-input-string "Hello\n\n world")])
	   (port-count-lines! p)
	   (let ([p2 (make-input-port 'with-loc
				      (lambda (s) (read-bytes-avail! s p))
				      (lambda (s skip progress-evt)
					(peek-bytes-avail! s skip progress-evt p))
				      void
				      (lambda () (prot-progress-evt p))
				      (lambda (k progress-evt done-evt)
					(port-commit-peeked k progress-evt done-evt p))
				      (lambda ()
					(let-values ([(line col pos) (port-next-location p)])
					  (adjust-locs line col pos))))])
	     (port-count-lines! p2)
	     p2)))])
  (let ([plain (let ([p (open-input-string "Hello\n\n world")])
		 (port-count-lines! p)
		 p)]
	[double (mk (lambda (l c p)
		      (values (* 2 l) (* 2 c) (* 2 p))))]
	[none (mk (lambda (l c p) (values #f #f #f)))]
	[bad (mk (lambda (l c p) #f))])
    (test-values '(1 0 1) (lambda () (port-next-location plain)))
    (test-values '(2 0 2) (lambda () (port-next-location double)))
    (test-values '(#f #f #f) (lambda () (port-next-location none)))
    (err/rt-test (port-next-location bad) exn:fail:contract:arity?)
    
    (test 'Hello read plain)
    (test 'Hello read double)
    (test 'Hello read none)
    (err/rt-test (read bad) exn:fail:contract:arity?)

    (test-values '(1 5 6) (lambda () (port-next-location plain)))
    (test-values '(2 10 12) (lambda () (port-next-location double)))
    (test-values '(#f #f #f) (lambda () (port-next-location none)))
    (err/rt-test (port-next-location bad))

    (let ([stx (read-syntax #f plain)])
      (test 3 syntax-line stx)
      (test 1 syntax-column stx)
      (test 9 syntax-position stx))
    (let ([stx (read-syntax #f double)])
      (test 6 syntax-line stx)
      ;; The next two should be 2 and 18, but the reader
      ;;  actually reads the character first, the subtracts
      ;;  1 from the column and position.
      (test 3 syntax-column stx)
      (test 19 syntax-position stx))
    (let ([stx (read-syntax #f none)])
      (test #f syntax-line stx)
      (test #f syntax-column stx)
      (test #f syntax-position stx))
    (err/rt-test (read-syntax #f bad) exn:fail:contract:arity?)
    
    (test-values '(3 6 14) (lambda () (port-next-location plain)))
    (test-values '(6 12 28) (lambda () (port-next-location double)))
    (test-values '(#f #f #f) (lambda () (port-next-location none)))))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Check that if the initial commit thread is killed, then
;;  another commit thread is broken, that the second doesn't
;;  assume that the initial commit thread is still there:

(let ()
  (define-values (r w) (make-pipe))
  (define ch (make-channel))
  (display "hi" w)
  (peek-byte r)
  (let ([t (thread (lambda ()
		     (port-commit-peeked 1 (port-progress-evt r) ch r)))])
    (sleep 0.01)
    (let ([t2
	   (thread (lambda ()
		     (port-commit-peeked 1 (port-progress-evt r) ch r)))])
      (sleep 0.01)
      (thread-suspend t2)
      (break-thread t2)
      (kill-thread t)
      (thread-resume t2)
      (sleep)))
  (test (char->integer #\h) peek-byte r))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Check that breaks are enabled properly:

(let ([try
       (lambda (read-char)
	 (let ([p (make-input-port
		   'test
		   (lambda (bstr) never-evt)
		   (lambda (bstr skip-count progress-evt) never-evt)
		   void)])
	   (let ([t (thread (lambda () (with-handlers ([exn:break? void])
					 (read-char p))))])
	     (sleep 0.1)
	     (break-thread t)
	     (sleep 0.1)
	     (test #f thread-running? t))))])
  (try sync)
  (try sync/enable-break)
  (parameterize-break #f (try sync/enable-break))
  (try read-char)
  (try peek-char)
  (try (lambda (x) (read-bytes-avail! (make-bytes 10) x)))
  (try (lambda (x) (read-bytes-avail!/enable-break (make-bytes 10) x)))
  (parameterize-break 
   #f 
   (try (lambda (x) (read-bytes-avail!/enable-break (make-bytes 10) x)))))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Check that an uncooperative output port doesn't keep breaks
;; disabled too long:

(test 'ok
      'stuck-port
      (let ([p (make-output-port 'stumper
                                 always-evt
                                 (lambda args
                                   never-evt)
                                 void)]
            [t (current-thread)])
        (thread (lambda () 
                  (sync (system-idle-evt))
                  (break-thread t)))
        (with-handlers ([exn:break? (lambda (exn) 'ok)])
          (write-byte 0 p))))

;; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(report-errs)
