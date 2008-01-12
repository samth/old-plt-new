#lang scribble/doc
@(require scribble/manual
          scribble/struct
          "manuals.ss")

@title{PLT Scheme Documentation}

@margin-note{This is an installation-specific listing. Running
             @exec{plt-help} may open a different
             page with local and user-specific documentation,
             including documentation for installed @|PLaneT| packages.}

@(build-contents #f)

@(make-toc-element
  #f
  null
  (list @link["master-index/index.html" 
              #:underline? #f
              (make-element "tocsubseclink"
                            (list "Master Index"))]))
