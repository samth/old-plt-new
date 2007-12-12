#lang scribble/doc
@require["mz.ss"]

@title[#:tag-prefix '(lib "scribblings/reference/reference.scrbl")
       #:tag "top"]{@bold{Reference}: PLT Scheme}

This manual defines the core PLT Scheme language and describes its
most prominent libraries. The companion manual @|Guide| provides a
friendlier (though less precise and less complete) overview of the
language.

@defmodulelang*[(scheme/base scheme)]{Unless otherwise noted, the
bindings defined in this manual are exported by the
@schememodname[scheme/base] and @schememodname[scheme] languages,
where @schememodname[scheme] includes all of
@schememodname[scheme/base].}

@table-of-contents[]

@include-section["model.scrbl"]
@include-section["syntax-model.scrbl"]
@include-section["syntax.scrbl"]
@include-section["data.scrbl"]
@include-section["struct.scrbl"]
@include-section["class.scrbl"]
@include-section["units.scrbl"]
@include-section["contracts.scrbl"]
@include-section["match.scrbl"]
@include-section["control.scrbl"]
@include-section["concurrency.scrbl"]
@include-section["macros.scrbl"]
@include-section["io.scrbl"]
@include-section["security.scrbl"]
@include-section["os.scrbl"]
@include-section["memory.scrbl"]
@include-section["running.scrbl"]

@;------------------------------------------------------------------------

@section{To Do}

This chapter provides some temporary hyper-link targets.

@subsection[#:tag "running-sa"]{Running MzScheme}
@subsection[#:tag "async-channel"]{Asynchronous Channels}
@subsection[#:tag "honu"]{Honu}

@;------------------------------------------------------------------------

@(bibliography

  (bib-entry #:key "Danvy90"
             #:author "Olivier Danvy and Andre Filinski"
             #:title "Abstracting Control"
             #:location "LISP and Functional Programming"
             #:date "1990")

  (bib-entry #:key "Felleisen88"
             #:author "Matthias Felleisen, Mitch Wand, Dan Friedman, and Bruce Duba"
             #:title "Abstract Continuations: A Mathematical Semantics for Handling Full Functional Jumps"
             #:location "LISP and Functional Programming"
             #:date "1988")

  (bib-entry #:key "Friedman95" 
             #:title "Exception system proposal" 
             #:author "Daniel P. Friedman, C. T. Haynes, and R. Kent Dybvig" 
             #:location "web page"
             #:url "http://www.cs.indiana.edu/scheme-repository/doc.proposals.exceptions.html"
             #:date "1995")

  (bib-entry #:key "Gasbichler02" 
             #:title "Processes vs. User-Level Threads in Scsh" 
             #:author "Martin Gasbichler and Michael Sperber" 
             #:date 2002 
             #:location "Workshop on Scheme and Functional Programming")

 (bib-entry #:key "Gunter95"
            #:author "Carl Gunter, Didier Remy, and Jon Rieke"
            #:title "A Generalization of Exceptions and Control in ML-like Languages"
            #:location "Functional Programming Languages and Computer Architecture"
            #:date 1995)

  (bib-entry #:key "Hieb90"
             #:author "Robert Hieb and R. Kent Dybvig"
             #:title "Continuations and Concurrency"
             #:location "Principles and Practice of Parallel Programming"
             #:date "1990")

  (bib-entry #:key "Queinnec91"
             #:author "Queinnec and Serpette"
             #:title "A Dynamic Extent Control Operator for Partial Continuations"
             #:location "Principles of Programming Languages"
             #:date "1991")

  (bib-entry #:key "Shan04"
             #:author "Ken Shan"
             #:title "Shift to Control"
             #:location "Workshop on Scheme and Functional Programming"
             #:date 2004)

  (bib-entry #:key "Sitaram90"
             #:author "Dorai Sitaram"
             #:title "Control Delimiters and Their Hierarchies"
             #:location @italic{Lisp and Symbolic Computation}
             #:date "1990")

  (bib-entry #:key "Sitaram93" 
             #:title "Handling Control" 
             #:author "Dorai Sitaram"
             #:location "Programming Language Design and Implementation" 
             #:date "1993")

  (bib-entry #:key "SRFI-42"
             #:title "SRFI-42: Eager Comprehensions"
             #:author "Sebastian Egner"
             #:location "SRFI"
             #:url "http://srfi.schemers.org/srfi-42/"
             #:date "2003")

  )

@;------------------------------------------------------------------------

@index-section[]
