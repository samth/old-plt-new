#lang meta/web

(require "resources.rkt" "people.rkt" "irc.rkt"
         "../minis/lists.rkt"
         "../stubs/blog.rkt" "../stubs/git.rkt"
         (prefix-in pre: "../stubs/pre.rkt"))

(provide community)
(define community
  @page[#:part-of 'community]{
    @mailing-lists-quick
    @irc-quick
    @parlist[@strong{PLT Scheme Inc.}
      @text{@blog — announcements, helpful hints, and thoughtful rants.}
      @text{@a[href: "http://twitter.com/#!/racketlang"]{Twitter}
            — random racket bits.}
      @text{@people — the people behind Racket.}]
    @parlist[@strong{Development}
      @text{@git (also available on
            @a[href: "http://github.com/plt/racket/"]{GitHub})}
      @text{@pre:installers and @pre:index{more}.}]
    @parlist[@strong{Support}
      @text{
        Thanks to @a[href: "http://www.nsf.gov/"]{the NSF},
        @a[href: "http://www.darpa.mil/"]{DARPA},
        the Fund for the Improvement of Postsecondary Education (FIPSE)
          at the US Department of Education,
        the Exxon Foundation,
        CORD,
        partners of the Academy of Information Technology,
        Microsoft,
        and Google for their generous support over the years.}]})
