#lang scribble/doc
@(require "mz.ss"
          racket/math
          scribble/extract
          (for-label racket/math
                     racket/flonum
                     racket/fixnum
                     racket/unsafe/ops
                     racket/require))

@(define math-eval (make-base-eval))
@(interaction-eval #:eval math-eval (require racket/math))

@title[#:tag "numbers"]{Numbers}

@guideintro["numbers"]{numbers}

All @deftech{numbers} are @deftech{complex numbers}. Some of them are
@deftech{real numbers}, and all of the real numbers that can be
represented are also @deftech{rational numbers}, except for
@as-index{@racket[+inf.0]} (positive @as-index{infinity}),
@as-index{@racket[-inf.0]} (negative infinity), and
@as-index{@racket[+nan.0]} (@as-index{not-a-number}). Among the
rational numbers, some are @deftech{integers}, because @racket[round]
applied to the number produces the same number.

Orthogonal to those categories, each number is also either an
@deftech{exact number} or an @deftech{inexact number}. Unless
otherwise specified, computations that involve an inexact number
produce inexact results. Certain operations on inexact numbers,
however, produce an exact number, such as multiplying an inexact
number with an exact @racket[0]. Some operations, which can produce an
irrational number for rational arguments (e.g., @racket[sqrt]), may
produce inexact results even for exact arguments.

In the case of complex numbers, either the real and imaginary parts
are both exact or inexact, or the number has an exact zero real part
and an inexact imaginary part; a complex number with an exact zero
imaginary part is a real number.

Inexact real numbers are implemented as either single- or
double-precision @as-index{IEEE floating-point numbers}---the latter
by default, and the former only when support for 32-bit inexact
numbers is specifically enabled when the run-time system is built, and
when computation starts with numerical constants specified as
single-precision numbers.

The precision and size of exact numbers is limited only by available
memory (and the precision of operations that can produce irrational
numbers). In particular, adding, multiplying, subtracting, and
dividing exact numbers always produces an exact result.

Inexact numbers can be coerced to exact form, except for the inexact
numbers @racket[+inf.0], @racket[-inf.0], and @racket[+nan.0], which
have no exact form. @index["division by inexact zero"]{Dividing} a
number by exact zero raises an exception; dividing a non-zero number
other than @racket[+nan.0] by an inexact zero returns @racket[+inf.0]
or @racket[-inf.0], depending on the sign of the dividend. The
@racket[+nan.0] value is not @racket[=] to itself, but @racket[+nan.0]
is @racket[eqv?] to itself. Conversely, @racket[(= 0.0 -0.0)] is
@racket[#t], but @racket[(eqv? 0.0 -0.0)] is @racket[#f]. The datum
@racketvalfont{-nan.0} refers to the same constant as @racket[+nan.0].

Calculations with infinites produce results consistent with IEEE
double-precision floating point where IEEE specifies the result; in
cases where IEEE provides no specification (e.g., @racket[(angle
+inf.0+inf.0i)]), the result corresponds to the limit approaching
infinity, or @racket[+nan.0] if no such limit exists.

A @deftech{fixnum} is an exact integer whose two's complement
representation fit into 31 bits on a 32-bit platform or 63 bits on a
64-bit platform; furthermore, no allocation is required when computing
with fixnums. See also the @racketmodname[racket/fixnum] module, below.

Two fixnums that are @racket[=] are also the same
according to @racket[eq?]. Otherwise, the result of @racket[eq?]
applied to two numbers is undefined.

Two numbers are @racket[eqv?] when they are both inexact or both
exact, and when they are @racket[=] (except for @racket[+nan.0],
@racket[+0.0], and @racket[-0.0], as noted above). Two numbers are
@racket[equal?] when they are @racket[eqv?].

@; ----------------------------------------
@section{Number Types}

@defproc[(number? [v any/c]) boolean?]{Returns @racket[#t] if @racket[v]
 is a number, @racket[#f] otherwise.

@mz-examples[(number? 1) (number? 2+3i) (number? "hello")]}


@defproc[(complex? [v any/c]) boolean?]{ Returns @racket[(number? v)],
because all numbers are @tech{complex numbers}.}


@defproc[(real? [v any/c]) boolean?]{ Returns @racket[#t] if @racket[v] is
 a @techlink{real number}, @racket[#f] otherwise.

@mz-examples[(real? 1) (real? +inf.0) (real? 2+3i) 
             (real? 2+0.0i) (real? "hello")]}


@defproc[(rational? [v any/c]) boolean?]{ Returns @racket[#t] if
 @racket[v] is a @techlink{rational number}, @racket[#f] otherwise.

@mz-examples[(rational? 1) (rational? +inf.0) (real? "hello")]}


@defproc[(integer? [v any/c]) boolean?]{ Returns @racket[#t] if @racket[v]
 is a number that is an @techlink{integer}, @racket[#f] otherwise.

@mz-examples[(integer? 1) (integer? 2.3) (integer? 4.0) (integer? +inf.0) 
             (integer? 2+3i) (integer? "hello")]}


@defproc[(exact-integer? [v any/c]) boolean?]{

Returns @racket[(and (integer? v) (exact? v))].

@mz-examples[(exact-integer? 1) (exact-integer? 4.0)]}


@defproc[(exact-nonnegative-integer? [v any/c]) boolean?]{

Returns @racket[(and (exact-integer? v) (not (negative? v)))].

@mz-examples[(exact-nonnegative-integer? 0) (exact-nonnegative-integer? -1)]}


@defproc[(exact-positive-integer? [v any/c]) boolean?]{

Returns @racket[(and (exact-integer? v) (positive? v))].

@mz-examples[(exact-positive-integer? 1) (exact-positive-integer? 0)]}


@defproc[(inexact-real? [v any/c]) boolean?]{

Returns @racket[(and (real? v) (inexact? v))].}


@defproc[(fixnum? [v any/c]) boolean?]{

Return @racket[#t] if @racket[v] is a @techlink{fixnum}, @racket[#f]
otherwise.}


@defproc[(zero? [z number?]) boolean?]{ Returns @racket[(= 0 z)].

@mz-examples[(zero? 0) (zero? -0.0)]}


@defproc[(positive? [x real?]) boolean?]{ Returns @racket[(> x 0)].

@mz-examples[(positive? 10) (positive? -10) (positive? 0.0)]}


@defproc[(negative? [x real?]) boolean?]{ Returns @racket[(< x 0)].

@mz-examples[(negative? 10) (negative? -10) (negative? -0.0)]}


@defproc[(even? [n integer?]) boolean?]{ Returns @racket[(zero? (modulo
 n 2))].

@mz-examples[(even? 10.0) (even? 11) (even? +inf.0)]}


@defproc[(odd? [n integer?]) boolean?]{ Returns @racket[(not (even? n))].

@mz-examples[(odd? 10.0) (odd? 11) (odd? +inf.0)]}


@defproc[(exact? [z number?]) boolean?]{ Returns @racket[#t] if @racket[z]
 is an exact number, @racket[#f] otherwise.

@mz-examples[(exact? 1) (exact? 1.0)]}


@defproc[(inexact? [z number?]) boolean?]{ Returns @racket[#t] if @racket[z]
 is an inexact number, @racket[#f] otherwise.

@mz-examples[(inexact? 1) (inexact? 1.0)]}


@defproc[(inexact->exact [z number?]) exact?]{ Coerces @racket[z] to an
 exact number. If @racket[z] is already exact, it is returned. If @racket[z]
 is @racket[+inf.0], @racket[-inf.0], or @racket[+nan.0], then the
 @exnraise[exn:fail:contract].

@mz-examples[(inexact->exact 1) (inexact->exact 1.0)]}


@defproc[(exact->inexact [z number?]) inexact?]{ Coerces @racket[z] to an
 inexact number. If @racket[z] is already inexact, it is returned.

@mz-examples[(exact->inexact 1) (exact->inexact 1.0)]}


@; ----------------------------------------
@section{Arithmetic}

@defproc[(+ [z number?] ...) number?]{

Returns the sum of the @racket[z]s, adding pairwise from left to
 right. If no arguments are provided, the result is @racket[0].

@mz-examples[(+ 1 2) (+ 1.0 2+3i 5) (+)]}


@defproc*[([(- [z number?]) number?]
           [(- [z number?] [w number?] ...+) number?])]{

When no @racket[w]s are supplied, returns @racket[(- 0 z)].
 Otherwise, returns the subtraction of the @racket[w]s from @racket[z]
 working pairwise from left to right.}

@mz-examples[(- 5 3.0) (- 1) (- 2+7i 1 3)]


@defproc[(* [z number?] ...) number?]{

Returns the product of the @racket[z]s, multiplying pairwise from left
 to right. If no arguments are provided, the result is
 @racket[1]. Multiplying any number by exact @racket[0] produces exact
 @racket[0].

@mz-examples[(* 2 3) (* 8.0 9) (* 1+2i 3+4i)]}


@defproc*[([(/ [z number?]) number?]
           [(/ [z number?] [w number?] ...+) number?])]{

When no @racket[w]s are supplied, returns @racket[(/ 1 z)].
 Otherwise, returns the division @racket[z] by the @racket[w]s working
 pairwise from left to right.

If @racket[z] is exact @racket[0] and no @racket[w] is exact
 @racket[0], then the result is exact @racket[0]. If any @racket[w] is
 exact @racket[0], the @exnraise[exn:fail:contract:divide-by-zero].

@mz-examples[(/ 3 4) (/ 81 3 3) (/ 10.0) (/ 1+2i 3+4i)]}


@defproc[(quotient [n integer?] [m integer?]) integer?]{

Returns @racket[(truncate (/ n m))].

@mz-examples[(quotient 10 3) (quotient -10.0 3) (quotient +inf.0 3)]}


@defproc[(remainder [n integer?] [m integer?]) integer?]{

Returns @racket[_q] with the same sign as @racket[n] such that

@itemize[

 @item{@racket[(abs _q)] is between @racket[0] (inclusive) and @racket[(abs m)] (exclusive), and}

 @item{@racket[(+ _q (* m (quotient n m)))] equals @racket[n].}

]

If @racket[m] is exact @racket[0], the
 @exnraise[exn:fail:contract:divide-by-zero].

@mz-examples[(remainder 10 3) (remainder -10.0 3) (remainder 10.0 -3) (remainder -10 -3) (remainder +inf.0 3)]}


@defproc[(quotient/remainder [n integer?] [m integer?]) (values integer? integer?)]{

Returns @racket[(values (quotient n m) (remainder n m))], but the
 combination may be computed more efficiently than separate calls to
 @racket[quotient] and @racket[remainder].

@mz-examples[
(quotient/remainder 10 3)
]}


@defproc[(modulo [n integer?] [m integer?]) integer?]{ 

Returns @racket[_q] with the same sign as @racket[m] where

@itemize[

 @item{@racket[(abs _q)] is between @racket[0] (inclusive) and @racket[(abs m)] (exclusive), and}

 @item{the difference between @racket[_q] and @racket[(- n (* m (quotient n m)))] is a multiple of @racket[m].}

]

If @racket[m] is exact @racket[0], the
 @exnraise[exn:fail:contract:divide-by-zero].

@mz-examples[(modulo 10 3) (modulo -10.0 3)  (modulo 10.0 -3) (modulo -10 -3) (modulo +inf.0 3)]}


@defproc[(add1 [z number?]) number?]{ Returns @racket[(+ z 1)].}

@defproc[(sub1 [z number?]) number?]{ Returns @racket[(- z 1)].}

@defproc[(abs [x real?]) number?]{ Returns the absolute value of
 @racket[x].

@mz-examples[(abs 1.0) (abs -1)]}

@defproc[(max [x real?] ...+) real?]{

Returns the largest of the @racket[x]s, or @racket[+nan.0] if any
 @racket[x] is @racket[+nan.0].  If any @racket[x] is inexact, the
 result is coerced to inexact.

@mz-examples[(max 1 3 2) (max 1 3 2.0)]}


@defproc[(min [x real?] ...+) real?]{

Returns the smallest of the @racket[x]s, or @racket[+nan.0] if any
 @racket[x] is @racket[+nan.0].  If any @racket[x] is inexact, the
 result is coerced to inexact.

@mz-examples[(min 1 3 2) (min 1 3 2.0)]}


@defproc[(gcd [n integer?] ...) integer?]{

Returns the @as-index{greatest common divisor} (a non-negative
 number) of the @racket[n]s. If no arguments are provided, the result
 is @racket[0]. If all arguments are zero, the result is zero.

@mz-examples[(gcd 10) (gcd 12 81.0)]}


@defproc[(lcm [n integer?] ...) integer?]{

Returns the @as-index{least common multiple} (a non-negative number)
 of the @racket[n]s. If no arguments are provided, the result is
 @racket[1]. If any argument is zero, the result is zero; furthermore,
 if any argument is exact @racket[0], the result is exact @racket[0].

@mz-examples[(lcm 10) (lcm 3 4.0)]}


@defproc[(round [x real?]) integer?]{

Returns the integer closest to @racket[x], resolving ties in favor of
 an even number.

@mz-examples[(round 17/4) (round -17/4) (round 2.5) (round -2.5)]}


@defproc[(floor [x real?]) integer?]{

Returns the largest integer that is no more than @racket[x].

@mz-examples[(floor 17/4) (floor -17/4) (floor 2.5) (floor -2.5)]}


@defproc[(ceiling [x real?]) integer?]{

Returns the smallest integer that is at least as large as
 @racket[x].

@mz-examples[(ceiling 17/4) (ceiling -17/4) (ceiling 2.5) (ceiling -2.5)]}


@defproc[(truncate [x real?]) integer?]{

Returns the integer farthest from @racket[0] that is no closer to
 @racket[0] than @racket[x].

@mz-examples[(truncate 17/4) (truncate -17/4) (truncate 2.5) (truncate -2.5)]}


@defproc[(numerator [q rational?]) integer?]{

Coreces @racket[q] to an exact number, finds the numerator of the
 number expressed in its simplest fractional form, and returns this
 number coerced to the exactness of @racket[q].

@mz-examples[(numerator 5) (numerator 34/8) (numerator 2.3)]}


@defproc[(denominator [q rational?]) integer?]{

Coreces @racket[q] to an exact number, finds the numerator of the
 number expressed in its simplest fractional form, and returns this
 number coerced to the exactness of @racket[q].

@mz-examples[(denominator 5) (denominator 34/8) (denominator 2.3)]}


@defproc[(rationalize [x real?] [tolerance real?]) real?]{

Among the real numbers within @racket[(abs tolerance)] of @racket[x],
 returns the one corresponding to an exact number whose
 @racket[denominator] is smallest.  If multiple integers are within
 @racket[tolerance] of @racket[x], the one closest to @racket[0] is
 used.

@mz-examples[
(rationalize 1/4 1/10)
(rationalize -1/4 1/10)
(rationalize 1/4 1/4)
(rationalize 11/40 1/4)
]}

@; ----------------------------------------
@section{Number Comparison}

@defproc[(= [z number?] [w number?] ...+) boolean?]{ Returns
 @racket[#t] if all of the arguments are numerically equal,
 @racket[#f] otherwise.  An inexact number is numerically equal to an
 exact number when the exact coercion of the inexact number is the
 exact number. Also, @racket[0.0] and @racket[-0.0] are numerically
 equal, but @racket[+nan.0] is not numerically equal to itself.

@mz-examples[(= 1 1.0) (= 1 2) (= 2+3i 2+3i 2+3i)]}


@defproc[(< [x real?] [y real?] ...+) boolean?]{ Returns @racket[#t] if
 the arguments in the given order are in strictly increasing,
 @racket[#f] otherwise.

@mz-examples[(< 1 1) (< 1 2 3) (< 1 +inf.0) (< 1 +nan.0)]}


@defproc[(<= [x real?] [y real?] ...+) boolean?]{ Returns @racket[#t]
 if the arguments in the given order are in non-decreasing,
 @racket[#f] otherwise.

@mz-examples[(<= 1 1) (<= 1 2 1)]}


@defproc[(> [x real?] [y real?] ...+) boolean?]{ Returns @racket[#t] if
 the arguments in the given order are in strictly decreasing,
 @racket[#f] otherwise.

@mz-examples[(> 1 1) (> 3 2 1) (> +inf.0 1) (< +nan.0 1)]}


@defproc[(>= [x real?] [y real?] ...+) boolean?]{ Returns @racket[#t]
 if the arguments in the given order are in non-increasing,
 @racket[#f] otherwise.

@mz-examples[(>= 1 1) (>= 1 2 1)]}


@; ------------------------------------------------------------------------
@section{Powers and Roots}

@defproc[(sqrt [z number?]) number?]{

Returns the principal @as-index{square root} of @racket[z].  The
 result is exact if @racket[z] is exact and @racket[z]'s square root
 is rational. See also @racket[integer-sqrt].

@mz-examples[(sqrt 4/9) (sqrt 2) (sqrt -1)]}


@defproc[(integer-sqrt [n integer?]) complex?]{

Returns @racket[(floor (sqrt n))] for positive @racket[n]. For
 negative @racket[n], the result is @racket[(* (integer-sqrt (- n))
 0+i)].

@mz-examples[(integer-sqrt 4.0) (integer-sqrt 5)]}


@defproc[(integer-sqrt/remainder [n integer?])
         (values integer? integer?)]{

Returns @racket[(integer-sqrt n)] and @racket[(- n (expt
 (integer-sqrt n) 2))].

@mz-examples[(integer-sqrt/remainder 4.0) (integer-sqrt/remainder 5)]}


@defproc[(expt [z number?] [w number?]) number?]{

Returns @racket[z] raised to the power of @racket[w]. If @racket[w] is
 exact @racket[0], the result is exact @racket[1]. If @racket[z] is
 exact @racket[0] and @racket[w] is negative, the
 @exnraise[exn:fail:contract:divide-by-zero].

@mz-examples[(expt 2 3) (expt 4 0.5) (expt +inf.0 0)]}


@defproc[(exp [z number?]) number?]{

Returns Euler's number raised to the power of @racket[z]. The result
 is normally inexact, but it is exact @racket[1] when @racket[z] is an
 exact @racket[0].

@mz-examples[(exp 1) (exp 2+3i) (exp 0)]}


@defproc[(log [z number?]) number?]{

Returns the natural logarithm of @racket[z].  The result is normally
 inexact, but it is exact @racket[0] when @racket[z] is an exact
 @racket[1]. When @racket[z] is exact @racket[0],
 @exnraise[exn:fail:contract:divide-by-zero].

@mz-examples[(log (exp 1)) (log 2+3i) (log 1)]}


@; ------------------------------------------------------------------------
@section{Trignometric Functions}

@defproc[(sin [z number?]) number?]{

Returns the sine of @racket[z], where @racket[z] is in radians. The
 result is normally inexact, but it is exact @racket[0] if @racket[z]
 is exact @scheme[0].

@mz-examples[(sin 3.14159) (sin 1+05.i)]}


@defproc[(cos [z number?]) number?]{

Returns the cosine of @racket[z], where @racket[z] is in radians.

@mz-examples[(cos 3.14159) (cos 1+05.i)]}


@defproc[(tan [z number?]) number?]{

Returns the tangent of @racket[z], where @racket[z] is in radians. The
 result is normally inexact, but it is exact @racket[0] if @racket[z]
 is exact @scheme[0].

@mz-examples[(tan 0.7854) (tan 1+05.i)]}


@defproc[(asin [z number?]) number?]{

Returns the arcsin in radians of @racket[z]. The result is normally
 inexact, but it is exact @racket[0] if @racket[z] is exact @scheme[0].

@mz-examples[(asin 0.25) (asin 1+05.i)]}


@defproc[(acos [z number?]) number?]{

Returns the arccosine in radians of @racket[z].

@mz-examples[(acos 0.25) (acos 1+05.i)]}


@defproc*[([(atan [z number?]) number?]
           [(atan [y real?] [x real?]) number?])]{

In the one-argument case, returns the arctangent of the inexact
 approximation of @racket[z], except that the result is an exact
 @racket[0] for an exact @racket[0] argument.

In the two-argument case, the result is roughly the same as @racket[(/
 (exact->inexact y) (exact->inexact x))], but the signs of @racket[y]
 and @racket[x] determine the quadrant of the result. Moreover, a
 suitable angle is returned when @racket[y] divided by @racket[x]
 produces @racket[+nan.0] in the case that neither @racket[y] nor
 @racket[x] is @racket[+nan.0]. Finally, if @racket[x] is exact
 @racket[0] and @racket[y] is an exact positive number, the result is
 exact @racket[0]. If both @racket[x] and @racket[y] are exact
 @racket[0], the @exnraise[exn:fail:contract:divide-by-zero].

@mz-examples[(atan 0.5) (atan 2 1) (atan -2 -1) (atan 1+05.i) (atan +inf.0 -inf.0)]}

@; ------------------------------------------------------------------------
@section{Complex Numbers}

@defproc[(make-rectangular [x real?] [y real?]) number?]{

Returns @racket[(+ x (* y 0+1i))].

@mz-examples[(make-rectangular 3 4.0)]}


@defproc[(make-polar [magnitude real?] [angle real?]) number?]{

Returns @racket[(+ (* magnitude (cos angle)) (* magnitude (sin angle)
 0+1i))].

@mz-examples[#:eval math-eval
                    (make-polar 10 (* pi 1/2))
                    (make-polar 10 (* pi 1/4))]}


@defproc[(real-part [z number?]) real?]{

Returns the real part of the complex number @racket[z] in rectangle
 coordinates.

@mz-examples[(real-part 3+4i) (real-part 5.0)]}


@defproc[(imag-part [z number?]) real?]{

Returns the imaginary part of the complex number @racket[z] in
 rectangle coordinates.

@mz-examples[(imag-part 3+4i) (imag-part 5.0) (imag-part 5.0+0.0i)]}


@defproc[(magnitude [z number?]) (and/c real? (not/c negative?))]{

 Returns the magnitude of the complex number @racket[z] in polar
 coordinates.

@mz-examples[(magnitude -3) (magnitude 3.0) (magnitude 3+4i)]}


@defproc[(angle [z number?]) real?]{ Returns the angle of
 the complex number @racket[z] in polar coordinates.

@mz-examples[(angle -3) (angle 3.0) (angle 3+4i) (angle +inf.0+inf.0i)]}

@; ------------------------------------------------------------------------
@section{Bitwise Operations}

@section-index{logical operators}

@defproc[(bitwise-ior [n exact-integer?] ...) exact-integer?]{ Returns
 the bitwise ``inclusive or'' of the @racket[n]s in their (semi-infinite)
 two's complement representation. If no arguments are provided, the
 result is @racket[0].

@mz-examples[(bitwise-ior 1 2) (bitwise-ior -32 1)]}


@defproc[(bitwise-and [n exact-integer?] ...) exact-integer?]{ Returns
 the bitwise ``and'' of the @racket[n]s in their (semi-infinite) two's
 complement representation. If no arguments are provided, the result
 is @racket[-1].

@mz-examples[(bitwise-and 1 2) (bitwise-and -32 -1)]}


@defproc[(bitwise-xor [n exact-integer?] ...) exact-integer?]{ Returns
 the bitwise ``exclusive or'' of the @racket[n]s in their (semi-infinite)
 two's complement representation. If no arguments are provided, the
 result is @racket[0].

@mz-examples[(bitwise-xor 1 5) (bitwise-xor -32 -1)]}


@defproc[(bitwise-not [n exact-integer?])  exact-integer?]{ Returns the
 bitwise ``not'' of @racket[n] in its (semi-infinite) two's complement
 representation.

@mz-examples[(bitwise-not 5) (bitwise-not -1)]}


@defproc[(bitwise-bit-set? [n exact-integer?] [m exact-nonnegative-integer?])
         boolean?]{

Returns @racket[#t] when the @racket[m]th bit of @racket[n] is set in @racket[n]'s
        (semi-infinite) two's complement representation.
                   
This operation is equivalent to
@racket[(not (zero? (bitwise-and n (arithmetic-shift 1 m))))],
but it is faster and runs in constant time when @racket[n] is positive.

@mz-examples[(bitwise-bit-set? 5 0) (bitwise-bit-set? 5 2) (bitwise-bit-set? -5 (expt 2 700))]}


@defproc[(bitwise-bit-field [n exact-integer?] 
                            [start exact-nonnegative-integer?] 
                            [end (and/c exact-nonnegative-integer?
                                        (start . <= . end))])
         exact-integer?]{

Extracts the bits between position @racket[start] and @racket[(- end 1)] (inclusive)
from @racket[n] and shifts them down to the least significant portion of the number.

This operation is equivalent to the computation

@racketblock[
(bitwise-and (sub1 (arithmetic-shift 1 (- end start)))
             (arithmetic-shift n (- start)))
]

but it runs in constant time when @racket[n] is positive, @racket[start] and
@racket[end] are fixnums, and @racket[(- end start)] is no more than
the maximum width of a fixnum.

Each pair of examples below uses the same numbers, showing the result
both in binary and as integers.

@mz-examples[(format "~b" (bitwise-bit-field (string->number "1101" 2) 1 1))
             (bitwise-bit-field 13 1 1)
             (format "~b" (bitwise-bit-field (string->number "1101" 2) 1 3))
             (bitwise-bit-field 13 1 3)
             (format "~b" (bitwise-bit-field (string->number "1101" 2) 1 4))
             (bitwise-bit-field 13 1 4)]
}


@defproc[(arithmetic-shift [n exact-integer?] [m exact-integer?])
 exact-integer?]{ Returns the bitwise ``shift'' of @racket[n] in its
 (semi-infinite) two's complement representation.  If @racket[m] is
 non-negative, the integer @racket[n] is shifted left by @racket[m] bits;
 i.e., @racket[m] new zeros are introduced as rightmost digits. If
 @racket[m] is negative, @racket[n] is shifted right by @racket[(- m)]
 bits; i.e., the rightmost @racket[m] digits are dropped.

@mz-examples[(arithmetic-shift 1 10) (arithmetic-shift 255 -3)]}

@defproc[(integer-length [n exact-integer?]) exact-integer?]{ Returns
 the number of bits in the (semi-infinite) two's complement
 representation of @racket[n] after removing all leading zeros (for
 non-negative @racket[n]) or ones (for negative @racket[n]).

@mz-examples[(integer-length 8) (integer-length -8)]}

@; ------------------------------------------------------------------------
@section{Random Numbers}

@defproc*[([(random [k (integer-in 1 4294967087)]
                    [generator pseudo-random-generator?
                               (current-pseudo-random-generator)])
            exact-nonnegative-integer?]
           [(random [generator pseudo-random-generator?
                               (current-pseudo-random-generator)]) 
            (and/c real? inexact? (>/c 0) (</c 1))])]{  

When called with and integer argument @racket[k], returns a random
exact integer in the range @racket[0] to @math{@racket[k]-1}. When
called with zero arguments, returns a random inexact number between
@racket[0] and @racket[1], exclusive.

In each case, the number is provided by the given pseudo-random number
generator (which defaults to the current one, as produced by
@racket[current-pseudo-random-generator]). The generator maintains an
internal state for generating numbers. The random number generator
uses a 54-bit version of L'Ecuyer's MRG32k3a algorithm
@cite["L'Ecuyer02"].}


@defproc[(random-seed [k (integer-in 1 (sub1 (expt 2 31)))])
          void?]{

Seeds the current pseudo-random number generator with
@racket[k]. Seeding a generator sets its internal state
deterministically; that is, seeding a generator with a particular
number forces it to produce a sequence of pseudo-random numbers that
is the same across runs and across platforms.}


@defproc[(make-pseudo-random-generator) pseudo-random-generator?]{

Returns a new pseudo-random number generator. The new generator is
seeded with a number derived from @racket[(current-milliseconds)].}


@defproc[(pseudo-random-generator? [v any/c]) boolean?]{

Returns @racket[#t] if @racket[v] is a pseudo-random number generator,
@racket[#f] otherwise.}


@defparam[current-pseudo-random-generator generator pseudo-random-generator?]{

A parameter that determines the pseudo-random number generator
used by @racket[random].}


@defproc[(pseudo-random-generator->vector [generator pseudo-random-generator?])
         vector?]{

Produces a vector that represents the complete internal state of
@racket[generator]. The vector is suitable as an argument to
@racket[vector->pseudo-random-generator] to recreate the generator in
its current state (across runs and across platforms).}


@defproc[(vector->pseudo-random-generator [vec vector?])
         pseudo-random-generator?]{

Produces a pseudo-random number generator whose internal state
corresponds to @racket[vec]. The vector @racket[vec] must contain six
exact integers; the first three integers must be in the range
@racket[0] to @racket[4294967086], inclusive; the last three integers
must be in the range @racket[0] to @racket[4294944442], inclusive; at
least one of the first three integers must be non-zero; and at least
one of the last three integers must be non-zero.}

@defproc[(vector->pseudo-random-generator! [generator pseudo-random-generator?]
                                           [vec vector?])
         void?]{

Like @racket[vector->pseudo-random-generator], but changes
@racket[generator] to the given state, instead of creating a new
generator.}

@; ------------------------------------------------------------------------
@section{Number--String Conversions}

@section-index["numbers" "machine representations"]
@section-index["numbers" "floating-point"]
@section-index["numbers" "big-endian"]
@section-index["numbers" "little-endian"]
@section-index["numbers" "converting"]

@defproc[(number->string [z number?] [radix (or/c 2 8 10 16) 10])
         string?]{
 Returns a string that is the printed form of @racket[z]
 in the base specific by @racket[radix]. If @racket[z] is inexact,
 @racket[radix] must be @racket[10], otherwise the
 @exnraise[exn:fail:contract].

@mz-examples[(number->string 3.0) (number->string 255 8)]}


@defproc[(string->number [s string?] [radix (integer-in 2 16) 10])
         (or/c number? #f)]{

Reads and returns a number datum from @racket[s] (see
@secref["parse-number"]), returning @racket[#f] if @racket[s] does not
parse exactly as a number datum (with no whitespace). The optional
@racket[radix] argument specifies the default base for the number,
which can be overriden by @litchar{#b}, @litchar{#o}, @litchar{#d}, or
@litchar{#x} in the string.

@mz-examples[(string->number "3.0+2.5i") (string->number "hello")
          (string->number "111" 7)  (string->number "#b111" 7)]
}

@defproc[(real->decimal-string [n real?] [decimal-digits exact-nonnegative-integer? 2])
         string?]{

Prints @racket[n] into a string and returns the string. The printed
form of @racket[n] shows exactly @racket[decimal-digits] digits after
the decimal point. The printed for uses a minus sign if @racket[n] is
negative, and it does not use a plus sign if @racket[n] is positive.

Before printing, @racket[n] is converted to an exact number,
multiplied by @racket[(expt 10 decimal-digits)], rounded, and then
divided again by @racket[(expt 10 decimal-digits)].  The result of ths
process is an exact number whose decimal representation has no more
than @racket[decimal-digits] digits after the decimal (and it is
padded with trailing zeros if necessary).

@mz-examples[
#:eval math-eval
(real->decimal-string pi)
(real->decimal-string pi 5)
]}

@defproc[(integer-bytes->integer [bstr bytes?]
                                 [signed? any/c]
                                 [big-endian? any/c (system-big-endian?)]
                                 [start exact-nonnegative-integer? 0]
                                 [end exact-nonnegative-integer? (bytes-length bstr)])
         exact-integer?]{

Converts the machine-format number encoded in @racket[bstr] to an
exact integer. The @racket[start] and @racket[end] arguments specify
the substring to decode, where @racket[(- end start)] must be
@racket[2], @racket[4], or @racket[8]. If @racket[signed?] is true,
then the bytes are decoded as a two's-complement number, otherwise it
is decoded as an unsigned integer. If @racket[big-endian?] is true,
then the first character's ASCII value provides the most significant
eight bits of the number, otherwise the first character provides the
least-significant eight bits, and so on.}


@defproc[(integer->integer-bytes [n exact-integer?]
                                 [size-n (or/c 2 4 8)]
                                 [signed? any/c]
                                 [big-endian? any/c (system-big-endian?)]
                                 [dest-bstr (and/c bytes? (not/c immutable?))
                                            (make-bytes size-n)]
                                 [start exact-nonnegative-integer? 0])
          bytes?]{

Converts the exact integer @racket[n] to a machine-format number
encoded in a byte string of length @racket[size-n], which must be
@racket[2], @racket[4], or @racket[8]. If @racket[signed?] is true,
then the number is encoded as two's complement, otherwise it is
encoded as an unsigned bit stream. If @racket[big-endian?] is true,
then the most significant eight bits of the number are encoded in the
first character of the resulting byte string, otherwise the
least-significant bits are encoded in the first byte, and so on.

The @racket[dest-bstr] argument must be a mutable byte string of
length @racket[size-n]. The encoding of @racket[n] is written into
@racket[dest-bstr] starting at offset @racket[start], and
@racket[dest-bstr] is returned as the result.

If @racket[n] cannot be encoded in a string of the requested size and
format, the @exnraise[exn:fail:contract]. If @racket[dest-bstr] is not
of length @racket[size-n], the @exnraise[exn:fail:contract].}


@defproc[(floating-point-bytes->real [bstr bytes?]
                                     [big-endian? any/c (system-big-endian?)]
                                     [start exact-nonnegative-integer? 0]
                                     [end exact-nonnegative-integer? (bytes-length bstr)])
         (and/c real? inexact?)]{

Converts the IEEE floating-point number encoded in @racket[bstr] from
position @racket[start] (inclusive) to @racket[end] (exclusive) to an
inexact real number. The difference between @racket[start] an
@racket[end] must be either 4 or 8 bytes. If @racket[big-endian?] is
true, then the first byte's ASCII value provides the most significant
eight bits of the IEEE representation, otherwise the first byte
provides the least-significant eight bits, and so on.}


@defproc[(real->floating-point-bytes [x real?]
                                     [size-n (or/c 4 8)]
                                     [big-endian? any/c (system-big-endian?)]
                                     [dest-bstr (and/c bytes? (not/c immutable?))
                                                (make-bytes size-n)]
                                     [start exact-nonnegative-integer? 0])
          bytes?]{

Converts the real number @racket[x] to its IEEE representation in a
byte string of length @racket[size-n], which must be @racket[4] or
@racket[8]. If @racket[big-endian?] is true, then the most significant
eight bits of the number are encoded in the first byte of the
resulting byte string, otherwise the least-significant bits are
encoded in the first character, and so on.

The @racket[dest-bstr] argument must be a mutable byte string of
length @racket[size-n]. The encoding of @racket[n] is written into
@racket[dest-bstr] starting with byte @racket[start], and
@racket[dest-bstr] is returned as the result.

If @racket[dest-bstr] is provided and it has less than @racket[start]
plus @racket[size-n] bytes, the @exnraise[exn:fail:contract].}


@defproc[(system-big-endian?) boolean?]{

Returns @racket[#t] if the native encoding of numbers is big-endian
for the machine running Racket, @racket[#f] if the native encoding
is little-endian.}

@; ------------------------------------------------------------------------
@section{Inexact-Real (Flonum) Operations}

@defmodule[racket/flonum]

The @racketmodname[racket/flonum] library provides operations like
@racket[fl+] that consume and produce only real @tech{inexact
numbers}, which are also known as @deftech{flonums}. Flonum-specific
operations provide can better performance when used consistently, and
they are as safe as generic operations like @racket[+].

@guidealso["fixnums+flonums"]

@subsection{Flonum Arithmetic}

@deftogether[(
@defproc[(fl+ [a inexact-real?] [b inexact-real?]) inexact-real?]
@defproc[(fl- [a inexact-real?] [b inexact-real?]) inexact-real?]
@defproc[(fl* [a inexact-real?] [b inexact-real?]) inexact-real?]
@defproc[(fl/ [a inexact-real?] [b inexact-real?]) inexact-real?]
@defproc[(flabs [a inexact-real?]) inexact-real?]
)]{

Like @racket[+], @racket[-], @racket[*], @racket[/], and @racket[abs],
but constrained to consume @tech{flonums}. The result is always a
@tech{flonum}.}

@deftogether[(
@defproc[(fl=   [a inexact-real?] [b inexact-real?]) boolean?]
@defproc[(fl<   [a inexact-real?] [b inexact-real?]) boolean?]
@defproc[(fl>   [a inexact-real?] [b inexact-real?]) boolean?]
@defproc[(fl<=  [a inexact-real?] [b inexact-real?]) boolean?]
@defproc[(fl>=  [a inexact-real?] [b inexact-real?]) boolean?]
@defproc[(flmin [a inexact-real?] [b inexact-real?]) inexact-real?]
@defproc[(flmax [a inexact-real?] [b inexact-real?]) inexact-real?]
)]{

Like @racket[=], @racket[<], @racket[>], @racket[<=], @racket[>=],
@racket[min], and @racket[max], but constrained to consume
@tech{flonums}.}

@deftogether[(
@defproc[(flround    [a inexact-real?]) inexact-real?]
@defproc[(flfloor    [a inexact-real?]) inexact-real?]
@defproc[(flceiling  [a inexact-real?]) inexact-real?]
@defproc[(fltruncate [a inexact-real?]) inexact-real?]
)]{

Like @racket[round], @racket[floor], @racket[ceiling], and
@racket[truncate], but constrained to consume @tech{flonums}.}

@deftogether[(
@defproc[(flsin  [a inexact-real?]) inexact-real?]
@defproc[(flcos  [a inexact-real?]) inexact-real?]
@defproc[(fltan  [a inexact-real?]) inexact-real?]
@defproc[(flasin [a inexact-real?]) inexact-real?]
@defproc[(flacos [a inexact-real?]) inexact-real?]
@defproc[(flatan [a inexact-real?]) inexact-real?]
@defproc[(fllog  [a inexact-real?]) inexact-real?]
@defproc[(flexp  [a inexact-real?]) inexact-real?]
@defproc[(flsqrt [a inexact-real?]) inexact-real?]
)]{

Like @racket[sin], @racket[cos], @racket[tan], @racket[asin],
@racket[acos], @racket[atan], @racket[log], @racket[exp], and
@racket[flsqrt], but constrained to consume and produce
@tech{flonums}. The result is @racket[+nan.0] when a number outside
the range @racket[-1.0] to @racket[1.0] is given to @racket[flasin] or
@racket[flacos], or when a negative number is given to @racket[fllog]
or @racket[flsqrt].}


@defproc[(->fl [a exact-integer?]) inexact-real?]{

Like @racket[exact->inexact], but constrained to consume exact
integers, so the result is always a @tech{flonum}.}


@defproc[(fl->exact-integer [a inexact-real?]) exact-integer?]{

Like @racket[inexact->exact], but constrained to consume an
@tech{integer} @tech{flonum}, so the result is always an exact
integer.}


@deftogether[(
@defproc[(make-flrectangular [a inexact-real?] [b inexact-real?]) 
         (and/c complex? inexact? (not/c real?))]
@defproc[(flreal-part [a (and/c complex? inexact? (not/c real?))]) inexact-real?]
@defproc[(flimag-part [a (and/c complex? inexact? (not/c real?))]) inexact-real?]
)]{

Like @racket[make-rectangular], @racket[real-part], and
@racket[imag-part], but both parts of the complex number must be
inexact.}


@subsection{Flonum Vectors}

A @deftech{flvector} is like a @tech{vector}, but it holds only
inexact real numbers. This representation can be more compact, and
unsafe operations on @tech{flvector}s (see
@racketmodname[racket/unsafe/ops]) can execute more efficiently than
unsafe operations on @tech{vectors} of inexact reals.

An f64vector as provided by @racketmodname[ffi/vector] stores the
same kinds of values as an @tech{flvector}, but with extra
indirections that make f64vectors more convenient for working with
foreign libraries. The lack of indirections make unsafe
@tech{flvector} access more efficient.

Two @tech{flvectors} are @racket[equal?] if they have the same length,
and if the values in corresponding slots of the @tech{flvectors} are
@racket[equal?].

@defproc[(flvector? [v any/c]) boolean?]{

Returns @racket[#t] if @racket[v] is a @tech{flvector}, @racket[#f] otherwise.}

@defproc[(flvector [x inexact-real?] ...) flvector?]{

Creates a @tech{flvector} containing the given inexact real numbers.}

@defproc[(make-flvector [size exact-nonnegative-integer?]
                        [x inexact-real? 0.0]) 
         flvector?]{

Creates a @tech{flvector} with @racket[size] elements, where every
slot in the @tech{flvector} is filled with @racket[x].}

@defproc[(flvector-length [vec flvector?]) exact-nonnegative-integer?]{

Returns the length of @racket[vec] (i.e., the number of slots in the
@tech{flvector}).}


@defproc[(flvector-ref [vec flvector?] [pos exact-nonnegative-integer?])
         inexact-real?]{

Returns the inexact real number in slot @racket[pos] of
@racket[vec]. The first slot is position @racket[0], and the last slot
is one less than @racket[(flvector-length vec)].}

@defproc[(flvector-set! [vec flvector?] [pos exact-nonnegative-integer?]
                        [x inexact-real?])
         inexact-real?]{

Sets the inexact real number in slot @racket[pos] of @racket[vec]. The
first slot is position @racket[0], and the last slot is one less than
@racket[(flvector-length vec)].}

@defproc[(in-flvector (v flvector?)) sequence?]{

Produces a sequence that gives the elements of @scheme[v] in order.
Inside a @scheme[for] form, this can be optimized to step through the
elements of @scheme[v] efficiently as in @scheme[in-list],
@scheme[in-vector], etc.}

@deftogether[(
@defform*[((for/flvector (for-clause ...) body)
           (for/flvector length-expr (for-clause ...) body))]
@defform*[((for*/flvector (for-clause ...) body)
           (for*/flvector length-expr (for-clause ...) body))])]{

Like @scheme[for/vector] or @scheme[for*/vector], but for
@tech{flvector}s.}

                       
@section{Fixnum Operations}

@defmodule[racket/fixnum]

The @racketmodname[racket/fixnum] library provides operations like
@racket[fx+] that consume and produce only fixnums. The operations in
this library are meant to be safe versions of unsafe operations like
@racket[unsafe-fx+]. These safe operations are generally no faster
than using generic primitives like @racket[+].

The expected use of the @racketmodname[racket/fixnum] library is for
code where the @racket[require] of @racketmodname[racket/fixnum] is
replaced with

@racketblock[(require (filtered-in
                       (λ (name) (regexp-replace #rx"unsafe-" name ""))
                       racket/unsafe/ops))]

to drop in unsafe versions of the library. Alternately, when
encountering crashes with code that uses unsafe fixnum operations, use
the @racketmodname[racket/fixnum] library to help debug the problems.

@deftogether[(
@defproc[(fx+ [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fx- [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fx* [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxquotient  [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxremainder [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxmodulo    [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxabs       [a fixnum?]) fixnum?]
)]{

Safe versions of @racket[unsafe-fx+], @racket[unsafe-fx-],
@racket[unsafe-fx*], @racket[unsafe-fxquotient],
@racket[unsafe-fxremainder], @racket[unsafe-fxmodulo], and
@racket[unsafe-fxabs]. The
@exnraise[exn:fail:contract:non-fixnum-result] if the arithmetic
result would not be a fixnum.}


@deftogether[(
@defproc[(fxand [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxior [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxxor [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxnot [a fixnum?]) fixnum?]
@defproc[(fxlshift [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxrshift [a fixnum?] [b fixnum?]) fixnum?]
)]{

Safe versions of @racket[unsafe-fxand], @racket[unsafe-fxior],
@racket[unsafe-fxxor], @racket[unsafe-fxnot],
@racket[unsafe-fxlshift], and @racket[unsafe-fxrshift].  The
@exnraise[exn:fail:contract:non-fixnum-result] if the arithmetic
result would not be a fixnum.}


@deftogether[(
@defproc[(fx=   [a fixnum?] [b fixnum?]) boolean?]
@defproc[(fx<   [a fixnum?] [b fixnum?]) boolean?]
@defproc[(fx>   [a fixnum?] [b fixnum?]) boolean?]
@defproc[(fx<=  [a fixnum?] [b fixnum?]) boolean?]
@defproc[(fx>=  [a fixnum?] [b fixnum?]) boolean?]
@defproc[(fxmin [a fixnum?] [b fixnum?]) fixnum?]
@defproc[(fxmax [a fixnum?] [b fixnum?]) fixnum?]
)]{

Safe versions of @racket[unsafe-fx=], @racket[unsafe-fx<],
 @racket[unsafe-fx>], @racket[unsafe-fx<=], @racket[unsafe-fx>=],
 @racket[unsafe-fxmin], and @racket[unsafe-fxmax].}

@deftogether[(
@defproc[(fx->fl [a fixnum?]) inexact-real?]
@defproc[(fl->fx [a inexact-real?]) fixnum?]
)]{

Safe versions of @racket[unsafe-fx->fl] and @racket[unsafe-fl->fx].}


@; ------------------------------------------------------------------------
@section{Extra Constants and Functions}

@note-lib[racket/math]

@defthing[pi real?]{

An approximation to the ratio of a circle's circumference to its
diameter: @number->string[pi].}

@defproc[(sqr [z number?]) number?]{

Returns @racket[(* z z)].}

@defproc[(sgn [x real?]) (or/c 1 0 -1 1.0 0.0 -1.0)]{

Returns the sign of @racket[x] as either @math{-1}, @math{0}, or
@math{1}.

@mz-examples[
#:eval math-eval
(sgn 10)
(sgn -10.0)
(sgn 0)
]}

@defproc[(conjugate [z number?]) number?]{

Returns the complex conjugate of @racket[z].

@mz-examples[
#:eval math-eval
(conjugate 1)
(conjugate 3+4i)
]}

@defproc[(sinh [z number?]) number?]{

Returns the hyperbolic sine of @racket[z].}

@defproc[(cosh [z number?]) number?]{

Returns the hyperbolic cosine of @racket[z].}

@defproc[(tanh [z number?]) number?]{

Returns the hyperbolic tangent of @racket[z].}

@defproc[(order-of-magnitude [r (and/c real? positive?)]) (and/c exact? integer?)]{
Computes the greatest exact integer @racket[m] such that:
@racketblock[(<= (expt 10 m)
                 (inexact->exact r))]
Hence also:
@racketblock[(< (inexact->exact r)
                (expt 10 (add1 m)))]

@mz-examples[#:eval math-eval 
                    (order-of-magnitude 999)
                    (order-of-magnitude 1000)
                    (order-of-magnitude 1/100)
                    (order-of-magnitude 1/101)]
}

@; ----------------------------------------------------------------------

@close-eval[math-eval]
