#lang scribble/manual
@(require (for-label (except-in racket/base
                                remove)
                     racket/contract/base
                     pkg
                     pkg/lib
                     (only-in pkg/pnr-db current-pkg-index-file)
                     net/url
                     syntax/modcollapse
                     setup/getinfo))

@title[#:tag "lib"]{Package Management Functions}

@defmodule[pkg/lib]{The @racketmodname[pkg/lib] library provides
building blocks on which the @racket[pkg] library and @exec{raco pkg}
commands are built.}


@deftogether[(
@defform[(with-pkg-lock body ...+)]
@defform[(with-pkg-lock/read-only body ...+)]
)]{

Evaluates the @racket[body]s while holding a lock to prevent
concurrent modification to the package database for the current
@tech{package scope}. Use the @racket[with-pkg-lock/read-only] form
for read-only access.  The lock is reentrant but not upgradable from
read-only.

Use these form to wrap uses of functions from @racketmodname[pkg/lib]
that are documented to require the lock. Other functions from
@racketmodname[pkg/lib] take the lock as needed.}

@deftogether[(
@defparam[current-pkg-scope scope (or/c 'installation 'user 'shared)]
@defparam[current-pkg-scope-version s string?]
)]{

Parameters that together determine @tech{package scope} for management
operations and the version for version-specific scope.}


@defparam[current-pkg-error err procedure?]{

A parameter whose value is used to report errors that are normally
intended for an end uses. The arguments to the procedure are the same
as for @racket[error], except that an initial symbol argument is
omitted.

The default value uses @racket[error] with @racket['pkg] as the first
argument. The @exec{raco pkg} command sets this parameter to use
@racket[raise-user-error] with the sub-command name as its first
argument.}


@defparam[current-pkg-indexes indexes (or/c #f (listof url?))]{

A parameter that determines the @tech{package name resolvers} that are
consulted to resolve a @tech{package name}. If the parameter's value
is @racket[#f], then the result of @racket[pkg-config-indexes] is
used.}


@defproc[(pkg-config-indexes) (listof string?)]{

Returns a list of URL strings for the user's configured @tech{package
name resolvers}.}


@defstruct[pkg-info ([orig-pkg (or/c (list/c 'pnr string?)
                                     (list/c 'url string?)
                                     (list/c 'link string?))]
                     [checksum (or/c #f string?)]
                     [auto? boolean?])
                     #:prefab]{

A structure type that is used to report installed-package information.}


@defproc[(pkg-directory [name string?]) path-string?]{

Returns the directory that holds the installation of the installed
(in any scope) package @racket[name].}


@defproc[(path->pkg [path path-string?]) (or/c string? #f)]{

Returns the installed package containing @racket[path], if any.}


@defproc[(default-pkg-scope) (or/c 'installation 'user 'shared)]{

Returns the user's configured default @tech{package scope}.}


@defproc[(installed-pkg-names [#:scope scope (or/c #f 'installation 'user 'shared)])
         (listof string?)]{

Returns a list of installed package names for the given @tech{package
scope}, where @racket[#f] indicates the user's default @tech{package
scope}.}


@defproc[(installed-pkg-table [#:scope scope (or/c #f 'installation 'user 'shared)])
         (hash/c string? pkg-info?)]{

Returns a hash table of installed packages for the given @tech{package
scope}, where @racket[#f] indicates the user's default @tech{package
scope}.}


@deftogether[(
@defproc[(pkg-desc? [v any/c]) boolean?]
@defproc[(pkg-desc [name string?]
                   [type (or/c #f 'file 'dir 'link 'file-url 'dir-url 'github 'name)]
                   [checksum (or/c string? #f)]
                   [auto? boolean?])
         pkg-desc?]
)]{

A @racket[pkg-desc] value describes a package source plus details of its
intended interpretation, where the @racket[auto?] field indicates that
the package is should be treated as installed automatically for a
dependency.}


@defproc[(pkg-stage [desc pkg-desc?]
                        [#:checksum checksum (or/c #f string?) #f])
         (values path? (or/c #f string?) boolean?)]{

Locates the implementation of the package specified by @racket[desc] and
downloads and unpacks it to a temporary directory (as needed).

The result is the directory containing the unpacked package content,
the checksum (if any) for the unpacked package, and whether the
directory should be removed after the package content is no longer
needed.}


@defproc[(pkg-config [set? boolean?] [keys/vals list?])
         void?]{

Implements the @racket[config] command.

The package lock must be held (allowing writes if @racket[set?] is true); see
@racket[with-pkg-lock].}


@defproc[(pkg-create [format (or/c 'zip 'tgz 'plt 'MANIFEST)]
                     [dir path-string?]
                     [#:quiet? quiet? boolean? #f])
        void?]{

Implements the @racket[create] command.

Unless @racket[quiet?] is true, information about the output is repotred to the current output port.}


@defproc[(pkg-install      [names (listof string?)]
                           [#:dep-behavior dep-behavior
                                           (or/c #f 'fail 'force 'search-ask 'search-auto)
                                           #f]
                           [#:force? force? boolean? #f]
                           [#:ignore-checksums? ignore-checksums? boolean? #f]
                           [#:quiet? boolean? quiet? #f])
         (or/c #f (listof (or/c path-string?
                                (non-empty-listof path-string?))))]{

Implements the @racket[install] command. The result indicates which
collections should be setup via @exec{raco setup}: @racket[#f] means
all, and a list means only the indicated collections.

Status information and debugging details are mostly reported to a logger
named @racket['pkg], but information that is especially relevant to a
user (such as a download action) is reported to the current output
port, unless @racket[quiet?] is true.

The package lock must be held; see @racket[with-pkg-lock].}


@defproc[(pkg-update      [names (listof string?)]
                          [#:dep-behavior dep-behavior
                                          (or/c #f 'fail 'force 'search-ask 'search-auto)
                                          #f]
                          [#:all? all? boolean? #f]
                          [#:deps? deps? boolean? #f]
                          [#:quiet? boolean? quiet? #f])
        (or/c #f (listof (or/c path-string?
                               (non-empty-listof path-string?))))]{

Implements the @racket[update] command. The result is the same as for
@racket[install-pkgs].

The package lock must be held; see @racket[with-pkg-lock].}


@defproc[(pkg-remove      [names (listof string?)]
                          [#:auto? auto? boolean? #f]
                          [#:force? force? boolean? #f]
                          [#:quiet? boolean? quiet? #f])
         (or/c #f (listof (or/c path-string? 
                                (non-empty-listof path-string?))))]{

Implements the @racket[remove] command. The result is the same as for
@racket[install-pkgs], indicating collects that should be setup
via @exec{raco setup}.

The package lock must be held; see @racket[with-pkg-lock].}


@defproc[(pkg-show [indent string?]
                   [#:directory show-dir? boolean? #f])
         void?]{

Implements the @racket[show] command for a single package scope,
printing to the current output port. See also
@racket[installed-pkg-names] and @racket[installed-pkg-table].

The package lock must be held to allow reads; see
@racket[with-pkg-lock/read-only].}


@defproc[(pkg-index-show [names (listof string?)]
                         [#:all? all? boolean? #f]
                         [#:only-names? only-names? boolean? #f])
         void?]{

Implements the @racket[index-show] command. If @racket[all?] is true,
then @racket[names] should be empty.}


@defproc[(pkg-index-copy [sources (listof path-string?)]
                         [dest path-string?]
                         [#:from-config? from-config? boolean? #f]
                         [#:merge? merge? boolean? #f]
                         [#:force? force? boolean? #f]
                         [#:override? override? boolean? #f])
         void?]{

Implements the @racket[index-copy] command.}


@defproc[(pkg-index-update-local [#:index-file index-file path-string? (current-pkg-index-file)]
                                 [#:quiet? quiet? boolean? #f])
         void?]{

Consults the user's configured @tech{package name resolvers} (like
@racket[pkg-index-copy]) and package servers to populate the database
@racket[index-file] with information about available packages and the
modules that they implement.}


@defproc[(pkg-index-suggestions-for-module 
          [module-path module-path?]
          [#:index-file index-file path-string? ....])
         (listof string?)]{

Consults @racket[index-file] and returns a list of available packages
that provide the module specified by @racket[module-path].

The default @racket[index-file] is @racket[(current-pkg-index-file)]
if that file exists, otherwise a file in the racket installation is
tried.}


@defproc[(get-all-pkg-names-from-indexes) (listof string?)]{

Consults @tech{package name resolvers} to obtain a list of available
@tech{package names}.}


@defproc[(get-all-pkg-details-from-indexes)
         (hash/c string? (hash/c symbol? any/c))]{

Consults @tech{package name resolvers} to obtain a hash table of available
@tech{package names} mapped to details about the package. Details for
a particular package are provided by a hash table that maps symbols
such as @racket['source], @racket['checksum], and @racket['author].}


@defproc[(get-pkg-details-from-indexes [name string?])
         (or/c #f (hash/c symbol? any/c))]{

Consults @tech{package name resolvers} to obtain information for a
single @tech{package name}, returning @racket[#f] if the @tech{package
name} has no resolution. Details for the package are provided in the
same form as from @racket[get-all-pkg-details-from-indexes].}


@defproc[(get-pkg-content [desc pkg-desc?]
                          [#:extract-info 
                           extract-proc
                           ((or/c #f
                                  ((symbol?) ((-> any)) . ->* . any))
                            . -> . any)
                           (lambda (get-pkg-info) ...)])
         (values (or/c #f string?) 
                 (listof module-path?)
                 any/c)]{

Gets information about the content of the package specified by
@racket[desc]. The information is determined inspecting the
package---resolving a @tech{package name}, downloading, and unpacking
into a temporary directory as necessary.

The results are as follows:

@itemize[

 @item{The checksum, if any, for the downloaded package.}

 @item{A list of module paths that are provided by the package.
       Each module path is normalized in the sense of
       @racket[collapse-module-path].}

 @item{Information extracted from the package's metadata.  By default,
       this information is the package's dependencies, but in general
       it is the result of @racket[extract-proc], which receives an
       information-getting function (or @racket[#f]) as returned by
       @racket[get-info].}

]}
