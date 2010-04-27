#lang scribble/manual
@(require (for-syntax racket)
          (for-label (only-in scheme/foreign unsafe! provide* define-unsafer)
                     (only-in scheme/base make-base-namespace make-base-empty-namespace)
                     scheme/gui/base
                     scheme/sandbox))

@(define-syntax-rule (def-extras unit-struct
                                 make-base-namespace-id
                                 make-base-empty-namespace-id
                                 sandbox-namespace-specs-id
                                 make-evaluator-id
                                 make-module-evaluator-id)
  (begin
    (require (for-label (only-in scheme struct)
                        (only-in racket/base make-base-namespace
                                             make-base-empty-namespace)
                        racket/sandbox))
    (define unit-struct (racket struct))
    (define make-base-namespace-id (racket make-base-namespace))
    (define make-base-empty-namespace-id (racket make-base-empty-namespace))
    (define sandbox-namespace-specs-id (racket sandbox-namespace-specs))
    (define make-evaluator-id (racket make-evaluator))
    (define make-module-evaluator-id (racket make-module-evaluator))))
@(def-extras unit-struct
             make-base-namespace-id
             make-base-empty-namespace-id
             sandbox-namespace-specs-id
             make-evaluator-id
             make-module-evaluator-id)

@(define-syntax-rule (compat-except sid rid . rest)
   (begin
     @section[@schememodname[sid]]
     @defmodule[sid]
     "The " @schememodname[sid] " library re-exports " @racketmodname[rid] (begin . rest) "."))
@(define-syntax-rule (compat sid rid)
   (compat-except sid rid))

@title{@bold{Scheme}: Compatibility Libraries}

Racket was once called ``PLT Scheme,'' and a number of libraries with
names starting @schemeidfont{scheme} provide compatibility with the
old name.

@table-of-contents[]

@compat-except[scheme racket]{, except that @schememodname[racket]'s
@scheme[struct] is not exported, the @|unit-struct| from
@schememodname[scheme/unit] is exported, @schememodname[scheme/set]
is not re-exported, and @schememodname[scheme/nest] is re-exported}

@compat-except[scheme/base racket/base]{, except that
@schememodname[racket]'s @scheme[struct] is not exported, and
@scheme[make-base-namespace] and @scheme[make-base-empty-namespace]
are different}

@defproc[(make-base-empty-namespace) namespace?]{

Like @|make-base-empty-namespace-id| from @schememodname[racket/base],
but with @schememodname[scheme/base] attached.}

@defproc[(make-base-namespace) namespace?]{

Like @|make-base-namespace-id| from @schememodname[racket/base], but
with @schememodname[scheme/base] attached.}


@compat[scheme/async-channel racket/async-channel]
@compat[scheme/bool racket/bool]
@compat[scheme/class racket/class]
@compat[scheme/cmdline racket/cmdline]
@compat[scheme/contract racket/contract]
@compat[scheme/control racket/control]
@compat[scheme/date racket/date]
@compat[scheme/dict racket/dict]
@; @compat[scheme/fasl racket/fasl]
@compat[scheme/file racket/file]
@compat[scheme/fixnum racket/fixnum]
@compat[scheme/flonum racket/flonum]

@; ----------------------------------------------------------------------

@compat-except[scheme/foreign racket/unsafe/ffi]{, except that @scheme[unsafe!]
must be used to import the unsafe bindings of @scheme[racket/unsafe/ffi]}

@defform[(unsafe!)]{

Makes unsafe bindings available.}


@defform/subs[#:literals (unsafe rename-out)
              (provide* provide-star-spec ...)
              ([provide-star-spec (unsafe id)
                                  (unsafe (rename-out [id external-id]))
                                  provide-spec])]{

Like @scheme[provide], but @scheme[id]s under @scheme[unsafe] are not
actually provided. Instead, they are collected for introduction into
an importing module via a macro created by @scheme[define-unsafer].}

@defform[(define-unsafer id)]{

Cooperates with @scheme[provide*] to define @scheme[id] as a
@scheme[unsafe!]-like form that introduces definitions for each
binding provided as @scheme[unsafe].  The @scheme[define-unsafer] form
must occur after all the @scheme[provide*] forms to which it refers.}

@; ----------------------------------------------------------------------

@compat[scheme/function racket/function]
@compat[scheme/future racket/future]
@compat[scheme/generator racket/generator]
@compat-except[scheme/gui racket/gui]{, except that it builds on
@schememodname[scheme] instead of @schememodname[racket]}
@compat[scheme/gui/base racket/gui/base]
@compat[scheme/gui/dynamic racket/gui/dynamic]
@compat[scheme/help racket/help]
@compat[scheme/include racket/include]
@compat[scheme/init racket/init]
@compat[scheme/list racket/list]
@compat[scheme/load racket/load]
@compat[scheme/local racket/local]
@compat[scheme/match racket/match]
@compat[scheme/math racket/math]
@compat[scheme/mpair racket/mpair]

@;------------------------------------------------------------------------
@section[#:tag "nest"]{@schememodname[scheme/nest]}

@defmodule[scheme/nest]

@defform[(nest ([datum ...+] ...) body ...+)]{

Combines nested expressions that syntactically drift to the right into
a more linear textual format, much in the same way that @racket[let*]
linearizes a sequence of nested @racket[let] expressions.

For example,

@racketblock[
(nest ([let ([x 10]
             [y 6])]
       [with-handlers ([exn:fail? (lambda (x) 15)])]
       [parameterize ([current-output-port (current-error-port)])]
       [let-values ([(d r) (quotient/remainder x y)])])
  (display (+ d r)))
]

is equivalent to

@racketblock[
(let ([x 10]
      [y 6])
  (with-handlers ([exn:fail? (lambda (x) 15)])
    (parameterize ([current-output-port (current-error-port)])
      (let-values ([(d r) (quotient/remainder x y)])
        (display (+ d r))))))
]

The @racket[nest] form is unusual in that it has no semantics apart
from its expansion, and its implementation is easier to understand
than a precise prose description:

@racketblock[
(define-syntax nest
  (syntax-rules ()
    [(nest () body0 body ...)
     (let () body0 body ...)]
    [(nest ([form forms ...]) body0 body ...)
     (form forms ... (let () body0 body ...))]
    [(nest ([form forms ...] . more) body0 body ...)
     (form forms ... (nest more body0 body ...))]))
]}

@; ----------------------------------------

@compat[scheme/package racket/package]
@compat[scheme/path racket/path]
@compat[scheme/port racket/port]
@compat[scheme/pretty racket/pretty]
@compat[scheme/promise racket/promise]
@compat[scheme/provide racket/provide]
@compat[scheme/provide-syntax racket/provide-syntax]
@compat[scheme/provide-transform racket/provide-transform]
@compat[scheme/require racket/require]
@compat[scheme/require-syntax racket/require-syntax]
@compat[scheme/require-transform racket/require-transform]
@compat[scheme/runtime-path racket/runtime-path]

@; ----------------------------------------

@compat-except[scheme/sandbox racket/sandbox]{, except that
@|sandbox-namespace-specs-id|, @|make-evaluator-id|, and
@|make-module-evaluator-id| are replaced}

@defparam[sandbox-namespace-specs spec (cons/c (-> namespace?) 
                                               (listof module-path?))]{

Like @|sandbox-namespace-specs-id| from
@racketmodname[racket/sandbox], but the default is @racket[(list
make-base-namespace)] if @racket[gui?] is @racket[#f], @racket[(list
make-gui-namespace)] if @racket[gui?] is @racket[#t].}

@defproc*[([(make-evaluator [language (or/c module-path?
                                            (list/c 'special symbol?)
                                            (cons/c 'begin list?))]
                            [input-program any/c] ...
                            [#:requires requires (listof (or/c module-path? path?))]
                            [#:allow-read allow (listof (or/c module-path? path?))])
            (any/c . -> . any)]
           [(make-module-evaluator [module-decl (or/c syntax? pair?)]
                                   [#:language   lang  (or/c #f module-path?)]
                                   [#:allow-read allow (listof (or/c module-path? path?))])
            (any/c . -> . any)])]{

Like @|make-evaluator-id| and @|make-module-evaluator-id| from
@racketmodname[racket/sandbox], but the value of the
@racket[sandbox-namespace-specs] parameter is installed as the
value of @|sandbox-namespace-specs-id| from
@racketmodname[racket/sandbox] before chaining to @|make-evaluator-id|
and @|make-module-evaluator-id| from @racketmodname[racket/sandbox].}

@; ----------------------------------------

@compat[scheme/serialize racket/serialize]
@compat[scheme/set racket/set]
@compat[scheme/signature racket/signature]
@compat[scheme/shared racket/shared]
@compat[scheme/splicing racket/splicing]
@compat[scheme/string racket/string]
@compat[scheme/struct-info racket/struct-info]
@compat[scheme/stxparam racket/stxparam]
@compat[scheme/stxparam-exptime racket/stxparam-exptime]
@compat[scheme/surrogate racket/surrogate]
@compat[scheme/system racket/system]
@compat[scheme/tcp racket/tcp]
@; @compat[scheme/trace racket/trace]
@compat[scheme/trait racket/trait]
@compat[scheme/udp racket/udp]
@compat[scheme/unit racket/unit]
@compat[scheme/unit-exptime racket/unit-exptime]
@compat[scheme/unsafe/ops racket/unsafe/ops]
@compat[scheme/vector racket/vector]

