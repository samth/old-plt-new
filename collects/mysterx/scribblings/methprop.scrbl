#lang scribble/doc
@(require "common.ss")

@title[#:tag "methprop"]{COM Methods and Properties}

  MysterX allows scripting of most COM components from Scheme.  A COM
  component can be scripted in MysterX if it supports OLE Automation
  via the @tt{IDispatch} interface, and if it publishes type
  information using the @tt{ITypeInfo} interface.

@defproc[(com-all-coclasses) (listof string?)]{

  Returns a list of strings for all COM classes
  registered on a system.}

@defproc[(com-all-controls) (listof string?)]{

  Returns a list of strings for all COM classes
  registered on a system that have the @scheme["Control"]
  subkey.}

@deftogether[(
@defproc[(cocreate-instance-from-coclass [coclass string?]
                                         [where (or/c (one-of/c 'local 'remote) string?) 'local])
         com-object?]
@defproc[(cci/coclass [coclass string?]
                      [where (or/c (one-of/c 'local 'remote) string?) 'local])
         com-object?]
)]{

  Returns an instance of @scheme[coclass]. This is useful for COM
  classes without a visual representation, or when a visual
  representation is not needed.

  The optional argument @scheme[where] indicates a for running the
  instance, and may be @scheme['local], @scheme['remote], or a string
  indicating a machine name.  See @secref["remote"] for more
  information.}

@deftogether[(
@defproc[(cocreate-instance-from-progid [progid string?]
                                         [where (or/c (one-of/c 'local 'remote) string?) 'local])
         com-object?]
@defproc[(cci/progid [progid string?]
                      [where (or/c (one-of/c 'local 'remote) string?) 'local])
         com-object?]
)]{

Like @scheme[cocreate-instance-from-coclass], but using a ProgID.}


@defproc[(coclass [obj com-object?]) string?]{

  Returns a string that is the name of the COM class instantiated by
  @scheme[obj], or raises an error if the COM class is not known.}

@defproc[(progid [obj com-object?]) string?]{

  Returns a string that is the name of the ProgID instantiated by
  @scheme[obj], or raises an error if the COM class is not known.}

@defproc[(set-coclass! [obj com-object?] [coclass string?]) void?]{

  Sets the COM class for @scheme[obj] to @scheme[coclass]. This is
  useful when MysterX COM event-handling procedures can obtain only
  ambiguous information about the object's COM class.}

@defproc[(set-coclass-from-progid! [obj com-object?] [progid string?]) void?]{

  Like @scheme[set-coclass!], but using a ProgID.}

@defproc[(com-methods [obj/type (or/c com-object? com-type?)]) 
         (listof string?)]{

   Returns a list of strings indicating the names of methods on
   @scheme[obj/type].}

@defproc[(com-method-type [obj/type (or/c com-object? com-type?)]
                          [method-name string?])
         (listof symbol?)]{

  Returns a list of symbols indicating the type of the specified
  method in @scheme[obj/type]. See @secref["com-types"] for
  information on the symbols.}

@defproc[(com-invoke [obj com-object?] [method-name string?] [v any/c])
         any/c]{

  Invokes @scheme[method-name] on @scheme[obj] with @scheme[v]s as the
  arguments. The special value @scheme[com-omit] may be used for
  optional arguments, which useful when values are supplied for
  arguments after the omitted argument(s).}

@defproc[(com-get-properties [obj/type (or/c com-object? com-type?)])
         (listof string?)]{

  Returns a list of strings indicating the names of readable
  properties in @scheme[obj/type].}

@defproc[(com-get-property-type [obj/type (or/c com-object? com-type?)]
                                [property-name string?])
         (listof symbol?)]{

  Returns a list of symbols indicating the type of the specified
  property in @scheme[obj/type]. See @secref["com-types"] for
  information on the symbols.}

@defproc[(com-get-property [obj com-object?] [property string?] ...+)
         any/c]{

  Returns the value of the final property by following the indicated
  path of @scheme[property]s, where each intermediate property is a
  COM object.}

@defproc[(com-set-properties [obj/type (or/c com-object? com-type?)]) 
         (listof string?)]{

  Returns a list of strings indicating the names of writeable
  properties in @scheme[obj/type].}

@defproc[(com-set-property-type [obj/type (or/c com-object? com-type?)] 
                                [property-name strig?])
          (listof symbol?)]{

  Returns a list of symbols indicating the type of the specified
  property in @scheme[obj/type]. See @secref["com-types"] for
  information on the symbols.}

@defproc[(com-set-property! [obj com-object?] 
                            [string? property] ...+
                            [v any/c])
         void?]{

   Sets the value of the final property in @scheme[obj] to @scheme[v]
   by following the @scheme[property]s, where the value of each
   intermediate property is a COM object.}

@defproc[(com-help [obj/type (or/c com-object? com-type?)] 
                   [topic string? ""])
         void?]{

  Starts the Window Help system with help about the COM object or COM
  type. The optional @scheme[topic] is typically a method or property
  name.}

@defproc[(com-object-eq? [obj1 com-object?] [obj2 com-object?])
         boolean?]{

  Returns @scheme[#t] if the two COM objects are the same,
  @scheme[#f] otherwise.}

@defproc[(com-object? [obj com-object?]) boolean?]{

  Returns @scheme[#t] if the argument is a COM object, @scheme[#f]
  otherwise.}

@defproc[(com-add-ref [obj com-object?]) void?]{
 
  Increments the reference count for @scheme[obj].
  This procedure should only be called when system-level 
  errors occur due to a mismanaged COM object.  Ordinarily,
  MysterX handles all COM reference-counting automatically.}

@defproc[(com-ref-count [obj com-object?]) exact-nonnegative-integer?]{

  Returns a number indicating the current reference count
  for a COM object.}
