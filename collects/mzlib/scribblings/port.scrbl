#lang scribble/doc
@(require "common.rkt"
          (for-label mzlib/port))

@mzlib[#:mode title port]

The @schememodname[mzlib/port] library mostly re-provides
@schememodname[scheme/port].

@defproc[(strip-shell-command-start [in input-port?]) void?]{

Reads and discards a leading @litchar{#!} in @scheme[in] (plus
continuing lines if the line ends with a backslash). Since
@litchar{#!}  followed by a forward slash or space is a comment, this
procedure is not needed before reading Scheme expressions.}

