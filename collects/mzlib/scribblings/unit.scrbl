#lang scribble/doc
@(require "common.ss"
          (for-label mzlib/unit))

@(begin
  (define-syntax-rule (bind id)
    (begin
     (require (for-label scheme/unit))
     (define id (scheme struct))))
  (bind scheme-struct)
  (define-syntax-rule (bindc id)
    (begin
     (require (for-label scheme/unit))
     (define id (scheme cstruct))))
  (bindc scheme-cstruct))

@mzlib[#:mode title unit]

The @schememodname[mzlib/unit] library mostly re-provides
@schememodname[scheme/unit], except for @scheme-struct and
@scheme-cstruct from @schememodname[scheme/unit].

@defform/subs[(struct id (field-id ...) omit-decl ...)
              ([omit-decl -type
                          -selectors
                          -setters
                          -constructor])]{

A signature form like @scheme-struct from @schememodname[scheme/unit],
but with a different syntax for the options that limit exports.}

@defform/subs[(cstruct id ([field-id contract-expr] ...) omit-decl ...)
              ([omit-decl -type
                          -selectors
                          -setters
                          -constructor])]{

A signature form like @scheme-cstruct from @schememodname[scheme/unit],
but with a different syntax for the options that limit exports.}
