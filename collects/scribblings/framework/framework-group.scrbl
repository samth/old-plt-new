#lang scribble/doc
@(require scribble/manual)
@(require (for-label framework/framework))
@(require (for-label scheme/gui))
@title{Group}

@defclass[group:% object% ()]{
  This class manages a group of frames matching the
  @scheme[frame:basic<%>]
  interface. There is one instance created by
  the framework, returned by the function
  @scheme[group:get-the-frame-group]
  and every frame that was constructed with
  @scheme[frame:basic-mixin]
  adds itself to the result of 
  @scheme[group:get-the-frame-group].
  @defmethod*[(((set-open-here-frame (frame (is-a?/c frame:editor%))) void))]{
    Sets the frame to load new files into.
    See also
    @scheme[frame:open-here<%>].


  }
  @defmethod*[(((get-open-here-frame) (union |#f| (is-a?/c frame:editor<%>))))]{
    Returns the currently saved frame
    to load new files into.

  }
  @defmethod*[(((get-mdi-parent) (union |#f| (instance frame%))))]{
    The result of this method must be used as the parent frame for each
    frame in the group.

  }
  @defmethod*[(((get-frames) (list-of (instance frame:basic<%>))))]{
    Returns the frames in the group.

  }
  @defmethod*[(((frame-label-changed (frame (is-a?/c frame:basic<%>))) void))]{
    This method is called by frames constructed with
    @scheme[frame:basic-mixin]
    when their titles change.


    Updates the windows menu of each frame in the group.
  }
  @defmethod*[(((frame-shown/hidden) void))]{
    This method is called by instances of
    @scheme[frame:basic%]
    to notify the frame group that
    a frame's visibility is changed.


    Updates the Windows menus of all of the frames in the frame group.
  }
  @defmethod*[(((for-each-frame (f ((instance frame:basic<%>) -> void))) void))]{
    This method applies a function to each frame in the group. It also
    remembers the function and applies it to any new frames that are added
    to the group when they are added.

    See also
    @method[group:% get-frames].


    Applies \var{f} to each frame in the group
  }
  @defmethod*[(((get-active-frame) (is-a?/c frame:basic<%>)))]{
    Returns the frame with the keyboard focus or the first frame in the
    group.

  }
  @defmethod*[(((set-active-frame (frame (is-a?/c frame:basic<%>))) void))]{
    Sets the active frame in the group.
    This method is called by
    @method[frame:register-group-mixin on-focus].

  }
  @defmethod*[(((insert-frame (frame (is-a?/c frame:basic<%>))) void))]{
    Inserts a frame into the group.

  }
  @defmethod*[(((remove-frame (frame (is-a?/c frame:basic<%>))) void))]{
    Removes a frame from the group.


  }
  @defmethod*[(((clear) boolean))]{
    This removes all of the frames in the group. It does not close the frames. See also
    @method[group:% on-close-all]and 
    @method[group:% can-close-all?].

  }
  @defmethod*[(((on-close-all) void))]{
    Call this method to close all of the frames in the group. 
    The function
    @method[group:% can-close-all?]
    must have been called just before this function and it must have returned 
    \rawscm{\#t}.


    Calls the
    @method[top-level-window<%> on-close]
    method and the
    @method[top-level-window<%> show]
    method (with \rawscm{\#f} as argument)
    on each frame in the group.
  }
  @defmethod*[(((can-close-all?) void))]{
    Call this method to make sure that closing all of the frames in the frame groups is
    permitted by the user. The function
    @method[group:% on-close-all]
    is expected to be called just after this method is called.


    Calls the
    @method[top-level-window<%> can-close?]
    method of each frame in the group.
  }
  @defmethod*[(((locate-file) (union |#f| (is-a?/c frame:basic<%>))))]{
    Returns the frame that is editing or viewing a particular file.

  }
}
@(require framework/framework-docs)
@(def-fw-procs group)
