
(load-relative "loadtest.ss")

(Section 'string)

(require scheme/string)

;; ---------- real->decimal-string ----------
(test "0." real->decimal-string 0 0)
(test "0." real->decimal-string 0.0 0)
(test "1." real->decimal-string 0.6 0)
(test "1." real->decimal-string 3/4 0)
(test "1." real->decimal-string 1.2 0)
(test "-0." real->decimal-string -0.0 0) ; note this!
(test "-0.00" real->decimal-string -0.0) ; same here...
(test "-1." real->decimal-string -0.6 0)
(test "-1." real->decimal-string -3/4 0)
(test "-1." real->decimal-string -1.2 0)
(test "1.20" real->decimal-string 1.2)
(test "-1.20" real->decimal-string -1.2)
(test "1.00" real->decimal-string 0.99999999999)
(test "-1.00" real->decimal-string -0.99999999999)
(test "1999999999999999859514578049071102439861518336.00"
      real->decimal-string 2e45)

;; ---------- regexp-quote ----------
(let ([s (list->string
	  (let loop ([i 0])
	    (if (= i 256)
		null
		(cons (integer->char i) (loop (add1 i))))))])
  (test (list s) regexp-match (regexp-quote s) s)
  (test (string-append "!" s "!")
	regexp-replace
        (regexp-quote s) s (regexp-replace-quote (string-append "!" s "!"))))

;; ---------- regexp-match* ----------
(test '("a" "b" "c") regexp-match* "[abc]" "here's a buck")
(test '("b" "c") regexp-match* "[abc]" "here's a buck" 8)
(test '("a") regexp-match* "[abc]" "here's a buck" 0 8)
(test '("a" "b" "c") regexp-match* "[abc]" "here's a buck" 0 #f)

(test '(#"a" #"b" #"c") regexp-match* "[abc]" #"here's a buck")
(test '(#"b" #"c") regexp-match* "[abc]" #"here's a buck" 8)
(test '(#"a") regexp-match* "[abc]" #"here's a buck" 0 8)
(test '(#"a" #"b" #"c") regexp-match* "[abc]" #"here's a buck" 0 #f)

(test '(#"a" #"b" #"c") regexp-match* #"[abc]" "here's a buck")
(test '(#"b" #"c") regexp-match* #"[abc]" "here's a buck" 8)
(test '(#"a") regexp-match* #"[abc]" "here's a buck" 0 8)
(test '(#"a" #"b" #"c") regexp-match* #"[abc]" "here's a buck" 0 #f)

(test '(#"a" #"b" #"c")
      regexp-match* "[abc]" (open-input-string "here's a buck"))
(test '(#"b" #"c") regexp-match* "[abc]" (open-input-string "here's a buck") 8)
(let ([s (open-input-string "here's a buck")])
  (test '(#"a") regexp-match* "[abc]" s 0 8)
  (test " buck" read-string 50 s))
(let ([s (open-input-string "here's a buck")])
  (test '(#"a" #"b" #"c") regexp-match* "[abc]" s 0 #f)
  (test eof read-char s))

;; ---------- regexp-match-positions* ----------
(test '((7 . 8) (9 . 10) (11 . 12))
      regexp-match-positions* "[abc]" "here's a buck")
(test '((9 . 10) (11 . 12))
      regexp-match-positions* "[abc]" "here's a buck" 8)
(test '((7 . 8))
      regexp-match-positions* "[abc]" "here's a buck" 0 8)
(test '((7 . 8) (9 . 10) (11 . 12))
      regexp-match-positions* "[abc]" "here's a buck" 0 #f)

(test '((7 . 8) (9 . 10) (11 . 12))
      regexp-match-positions* "[abc]" (open-input-string "here's a buck"))
(test '((9 . 10) (11 . 12))
      regexp-match-positions* "[abc]" (open-input-string "here's a buck") 8)
(let ([s (open-input-string "here's a buck")])
  (test '((7 . 8)) regexp-match-positions* "[abc]" s 0 8)
  (test " buck" read-string 50 s))
(let ([s (open-input-string "here's a buck")])
  (test '((7 . 8) (9 . 10) (11 . 12)) regexp-match-positions* "[abc]" s 0 #f)
  (test eof read-char s))

;; ---------- regexp-match-peek-positions* ----------
(let ([s (open-input-string "here's a buck")])
  (test '((7 . 8) (9 . 10) (11 . 12))
        regexp-match-peek-positions*
        "[abc]" (open-input-string "here's a buck"))
  (test '((9 . 10) (11 . 12))
        regexp-match-peek-positions*
        "[abc]" (open-input-string "here's a buck") 8)
  (test '((7 . 8))
        regexp-match-peek-positions* "[abc]" s 0 8)
  (test '((7 . 8) (9 . 10) (11 . 12))
        regexp-match-peek-positions* "[abc]" s 0 #f)
  (test "here's a buck"
        read-string 50 s))

;; ---------- regexp-split ----------
(test '("here's " " " "u" "k") regexp-split "[abc]" "here's a buck")
(test '(" " "u" "k")           regexp-split "[abc]" "here's a buck" 8)
(test '("" "u" "k")            regexp-split "[abc]" "here's a buck" 9)
(test '("" "u" "")             regexp-split "[abc]" "here's a buck" 9 12)
(test '("here's " "")          regexp-split "[abc]" "here's a buck" 0 8)
(test '("here's " " ")         regexp-split "[abc]" "here's a buck" 0 9)
(test '("here's " " " "u" "k") regexp-split "[abc]" "here's a buck" 0 #f)

(test '(#"here's " #" " #"u" #"k")
      regexp-split "[abc]" (open-input-string "here's a buck"))
(test '(#" " #"u" #"k")
      regexp-split "[abc]" (open-input-string "here's a buck") 8)
(test '(#"" #"u" #"k")
      regexp-split "[abc]" (open-input-string "here's a buck") 9)
(let ([s (open-input-string "here's a buck")])
  (test '(#"" #"u" #"") regexp-split "[abc]" s 9 12)
  (test "k" read-string 50 s))
(let ([s (open-input-string "here's a buck")])
  (test '(#"here's " #"") regexp-split "[abc]" s 0 8)
  (test " buck" read-string 50 s))
(let ([s (open-input-string "here's a buck")])
  (test '(#"here's " #" ") regexp-split "[abc]" s 0 9)
  (test "buck" read-string 50 s))
(let ([s (open-input-string "here's a buck")])
  (test '(#"here's " #" " #"u" #"k") regexp-split "[abc]" s 0 #f)
  (test eof read-char s))

;; test with zero-length matches
(test '("" "f" "o" "o" "") regexp-split #rx"" "foo")
(test '("" "f" "o" "o" " " "b" "a" "r" "") regexp-split #rx"" "foo bar")
(test '("" "f" "o" "o" "" "b" "a" "r" "") regexp-split #rx" *" "foo bar")
(test '("f" "" "ar") regexp-split #rx"oo| b" "foo bar")
(test '("foo bar" "") regexp-split #rx"$" "foo bar")
;; this doesn't work (like in Emacs) because ^ matches the start pos
;; (test '("" "foo bar") regexp-split #rx"^" "foo bar")

;; ---------- string-append* ----------
(let ()
  (test ""           string-append* '())
  (test ""           string-append* '(""))
  (test ""           string-append* '("" ""))
  (test "0123456789" string-append* '("0123456789"))
  (test "0123456789" string-append* "0123456789" '())
  (test "0123456789" string-append* "0123456789" '(""))
  (test "0123456789" string-append* "0123456789" '("" ""))
  (test "0123456789" string-append* "01234567" '("8" "9")))

;; ---------- string-join ----------
(let ()
  (test ""    string-join '() " ")
  (test ""    string-join '("") " ")
  (test " "   string-join '("" "") " ")
  (test "x y" string-join '("x" "y") " ")
  (test "x"   string-join '("x") " "))

(report-errs)
