#lang scribble/manual
@(require (for-label scribble/struct
                     scriblib/autobib
                     scheme/base
                     scheme/contract))

@title[#:tag "autobib"]{Bibliographies}

@defmodule[scriblib/autobib]

This library provides support for bibliography management in a Scribble
document. The @racket[define-cite] form is used to bind procedures
that create in-line citations and generate the bibilography in the
document.

Individual bibliography entries are created with the @racket[make-bib]
function. See below for an example.

@codeblock|{
  #lang scribble/base

  @(require scriblib/autobib)

  @(define-cite ~cite citet generate-bibliography)

  @(define plt-tr1
     (make-bib
      #:title    "Reference: Racket"
      #:author   (authors "Matthew Flatt" "PLT")
      #:date     "2010"
      #:location (techrpt-location #:institution "PLT Inc." #:number "PLT-TR-2010-1")
      #:url      "http://racket-lang.org/tr1/"))

  Racket is fun@~cite[plt-tr1].

  @(generate-bibliography)
}|

@defform[(define-cite ~cite-id citet-id generate-bibliography-id
                      [#:disambiguate disambiguator]
                      [#:render-date-bib render-date]
                      [#:render-date-cite render-date]
                      [#:date<? date-compare]
                      [#:date=? date-compare])]{

Binds @racket[~cite-id], @racket[citet-id], and
@racket[generate-bibliography-id], which share state to accumulate and render
citations. If two citations' references would render the same (as judged by equal authors and dates are @racket[date=?]) but are
different, the optionally provided disambiguation function is used to add an
extra element after the date. The default disambiguator will add "a", "b", etc
until "z". Anything more ambiguous will throw an error. It has the contract

@racketblock[(-> exact-nonnegative-integer? element?)]

Dates in citations and dates in the bibliography may be rendered differently,
as specified by the optionally given @racket[render-date] functions, which have the contract

@racketblock[(-> date? element?)]

The dates of citations are stored as @racket[date] values, and the granularity in which they are compared and rendered are, by default, by year. The comparison functions have contract

@racketblock[(-> date? date? boolean?)]

The function bound to @racket[~cite-id] produces a citation referring
to one or more bibliography entries with a preceding non-breaking
space, by default sorting the entries to match the bibliography order.
It has the contract

@racketblock[
(->* (bib?) (#:sort? any/c) #:rest (listof bib?) element?)
]

The function bound to @racket[citet-id] generates an element suitable
for use as a noun---referring to a document or its author---for one
or more bibliography entries which share an author. It has the contract

@racketblock[
(->* (bib?) () #:rest (listof bib?) element?)
]

The function bound to @racket[generate-bibliography-id] generates the
section for the bibliography. It has the contract

@racketblock[
(->* () (#:tag string? #:sec-title string?) part?)
]

The default value for the @racket[#:tag] argument is @racket["doc-bibliography"]
and for @racket[#:sec-title] is @racket["Bibliography"].
}


@defproc[(bib? [v any/c]) boolean?]{

Returns @racket[#t] if @racket[v] is a value produced by
@racket[make-bib] or @racket[in-bib], @racket[#f] otherwise.}


@defproc[(make-bib [#:title title any/c]
                   [#:author author any/c #f]
                   [#:is-book? is-book? any/c #f]
                   [#:location location any/c #f]
                   [#:date date (or/c #f date? exact-nonnegative-integer? string?) #f]
                   [#:url url string? #f])
         bib?]{

Produces a value that represents a document to cite. Except for
@racket[is-book?] and @racket[url], the arguments are used as
elements, except that @racket[#f] means that the information is not
supplied. Functions like @racket[proceedings-location],
@racket[author-name], and @racket[authors] help produce elements in a
standard format.

Dates are internally represented as @racket[date] values, so a @racket[date]
may be given, or a number or string that represent the year.

An element produced by a function like @racket[author-name] tracks
first, last names, and name suffixes separately, so that names can be
ordered and rendered correctly. When a string is provided as an author
name, the last non-empty sequence of alphabetic characters or
@litchar["-"] after a space is treated as the author name, and the
rest is treated as the first name.}

@defproc[(in-bib [orig bib?] [where string?]) bib?]{

Extends a bib value so that the rendered citation is suffixed with
@racket[where], which might be a page or chapter number.}

@defproc[(proceedings-location [location any/c]
                               [#:pages pages (or (list/c any/c any/c) #f) #f]
                               [#:series series any/c #f]
                               [#:volume volume any/c #f])
         element?]{

Combines elements to generate an element that is suitable for
describing a paper's location within a conference or workshop
proceedings.}

@defproc[(journal-location [title any/c]
                           [#:pages pages (or (list/c any/c any/c) #f) #f]
                           [#:number number any/c #f]
                           [#:volume volume any/c #f])
         element?]{

Combines elements to generate an element that is suitable for
describing a paper's location within a journal.}


@defproc[(book-location [#:edition edition any/c #f]
                        [#:publisher publisher any/c #f])
         element?]{

Combines elements to generate an element that is suitable for
describing a book's location.}

@defproc[(techrpt-location [#:institution institution edition any/c]
                           [#:number number any/c])
         element?]{

Combines elements to generate an element that is suitable for
describing a technical report's location.}

@defproc[(dissertation-location [#:institution institution edition any/c]
                                [#:degree degree any/c "PhD"])
         element?]{

Combines elements to generate an element that is suitable for
describing a dissertation.}


@defproc[(author-name [first any/c]
                      [last any/c]
                      [#:suffix suffix any/c #f])
         element?]{

Combines elements to generate an element that is suitable for
describing an author's name, especially where the last name is not
merely a sequence of ASCII alphabet letters or where the name has a
suffix (such as ``Jr.'').}

@defproc[(authors [name any/c] ...) element?]{

Combines multiple author elements into one, so that it is rendered and
alphabetized appropriately. If a @racket[name] is a string, it is
parsed in the same way as by @racket[make-bib].}

@defproc[(org-author-name [name any/c]) element?]{

Converts an element for an organization name to one suitable for use
as a bib-value author.}

@defproc[(other-authors) element?]{

Generates an element that is suitable for use as a ``others'' author.
When combined with another author element via @racket[authors], the
one created by @racket[other-authors] renders as ``et al.''}

@defproc[(editor [name name/c]) element?]{

Takes an author-name element and create one that represents the editor
of a collection. If a @racket[name] is a string, it is parsed in the
same way as by @racket[make-bib].}
