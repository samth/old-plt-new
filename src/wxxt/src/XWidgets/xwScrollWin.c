/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#line 568 "XWidgets/xwScrollWin.w"
#include <stdio.h>
#line 569 "XWidgets/xwScrollWin.w"
#include <xwEnforcer.h>
#line 570 "XWidgets/xwScrollWin.w"
#include <xwBoard.h>
#line 571 "XWidgets/xwScrollWin.w"
#include <xwVScrollb.h>
#line 572 "XWidgets/xwScrollWin.w"
#include <xwHScrollb.h>
#line 573 "XWidgets/xwScrollWin.w"
#include <xwscroll.h>
#include <./xwScrollWinP.h>
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
#line 169 "XWidgets/xwScrollWin.w"
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);
#line 219 "XWidgets/xwScrollWin.w"
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
#line 260 "XWidgets/xwScrollWin.w"
static void resize(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 273 "XWidgets/xwScrollWin.w"
static void insert_child(
#if NeedFunctionPrototypes
Widget 
#endif
);
#line 311 "XWidgets/xwScrollWin.w"
static void compute_inside(
#if NeedFunctionPrototypes
Widget,Position *,Position *,int *,int *
#endif
);
#line 319 "XWidgets/xwScrollWin.w"
static void destroy(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 332 "XWidgets/xwScrollWin.w"
static void scroll_response(
#if NeedFunctionPrototypes
Widget ,XtPointer ,XtPointer 
#endif
);
#line 368 "XWidgets/xwScrollWin.w"
static void compute_sizes(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 418 "XWidgets/xwScrollWin.w"
static void copy_vScrollAmount(
#if NeedFunctionPrototypes
Widget,int ,XrmValue *
#endif
);
#line 427 "XWidgets/xwScrollWin.w"
static void scroll_callback(
#if NeedFunctionPrototypes
Widget ,XtPointer ,XtPointer 
#endif
);
#line 490 "XWidgets/xwScrollWin.w"
static void CW_killed(
#if NeedFunctionPrototypes
Widget ,XtPointer ,XtPointer 
#endif
);
#line 510 "XWidgets/xwScrollWin.w"
static void configure(
#if NeedFunctionPrototypes
Widget ,XtPointer ,XEvent *,Boolean *
#endif
);
#line 368 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 368 "XWidgets/xwScrollWin.w"
static void compute_sizes(Widget self)
#else
#line 368 "XWidgets/xwScrollWin.w"
static void compute_sizes(self)Widget self;
#endif
#line 369 "XWidgets/xwScrollWin.w"
{
    int selfw, selfh, framew, frameh, vsheight, hswidth;
    Dimension help;
    Position selfx, selfy;

    xfwfBoardClassRec.xfwfCommon_class.compute_inside(self, &selfx, &selfy, &selfw, &selfh);
    if (! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideHScrollbar)
	vsheight = (int)selfh - 3 * ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth;
    else
	vsheight = (int)selfh - 2 * ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing;
    if (! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideVScrollbar)
	hswidth = (int)selfw - 3 * ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth;
    else
	hswidth = (int)selfw - 2 * ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing;
    XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, XtNhighlightThickness, &help, NULL);
    if (help > ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing) help = 0;
    XtConfigureWidget(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll,
		      selfx + selfw - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth,
		      selfy + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - help,
		      ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth,
		      max(1, vsheight + 2*help),
		      0);
    XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, XtNhighlightThickness, &help, NULL);
    if (help > ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing) help = 0;
    XtConfigureWidget(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll,
		      ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - help,
		      selfy + selfh - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth,
		      max(1, hswidth + 2*help),
		      ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth,
		      0);
    XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNhighlightThickness, &help, NULL);
    if (help > ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing) help = 0;
    framew = selfw - 2 * ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing + 2 * help;
    frameh = selfh - 2 * ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing + 2 * help;
    if (! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideVScrollbar) framew -= ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing;
    if (! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideHScrollbar) frameh -= ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing;
    XtConfigureWidget(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame,
		      selfx + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - help,
		      selfy + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing - help,
		      max(1, framew),
		      max(1, frameh),
		      0);
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW != NULL) XtMoveWidget(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, 0, 0);
}
#line 418 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 418 "XWidgets/xwScrollWin.w"
static void copy_vScrollAmount(Widget self,int  offset,XrmValue * value)
#else
#line 418 "XWidgets/xwScrollWin.w"
static void copy_vScrollAmount(self,offset,value)Widget self;int  offset;XrmValue * value;
#endif
#line 419 "XWidgets/xwScrollWin.w"
{
    value->addr = (XtPointer) &((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vScrollAmount;
}
#line 427 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 427 "XWidgets/xwScrollWin.w"
static void scroll_callback(Widget  w,XtPointer  client_data,XtPointer  call_data)
#else
#line 427 "XWidgets/xwScrollWin.w"
static void scroll_callback(w,client_data,call_data)Widget  w;XtPointer  client_data;XtPointer  call_data;
#endif
#line 428 "XWidgets/xwScrollWin.w"
{
    Widget self = (Widget) client_data;
    XfwfScrollInfo *info = (XfwfScrollInfo *) call_data;
    XfwfScrollInfo new;
    Position boardx, boardy, gx, gy, minx = 0, miny = 0;
    int boardwd, boardht;
    Dimension gwd, ght;

    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.autoAdjustScrollbars) {
      xfwfBoardClassRec.xfwfCommon_class.compute_inside(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, &boardx, &boardy, &boardwd, &boardht);
      boardwd = max(0, boardwd);
      boardht = max(0, boardht);
      XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, XtNx, &gx, XtNy, &gy, XtNwidth, &gwd,
		    XtNheight, &ght, NULL);
      minx = gwd <= boardwd ? 0 : boardwd - gwd;
      miny = ght <= boardht ? 0 : boardht - ght;
      switch (info->reason) {
      case XfwfSUp: gy = min(gy + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vScrollAmount, 0); break;
      case XfwfSDown: gy = max(gy - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vScrollAmount, miny); break;
      case XfwfSLeft: gx = min(gx + ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hScrollAmount, 0); break;
      case XfwfSRight: gx = max(gx - ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hScrollAmount, minx); break;
      case XfwfSPageUp: gy = min(gy + boardht, 0); break;
      case XfwfSPageDown: gy = max(gy - boardht, miny); break;
      case XfwfSPageLeft: gx = min(gx + boardwd, 0); break;
      case XfwfSPageRight: gx = max(gx - boardwd, minx); break;
      case XfwfSDrag:
	if (w == ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll) gy = info->vpos * miny;
	else gx = info->hpos * minx;
	break;
      case XfwfSMove: break;
      case XfwfSTop: gy = 0; break;
      case XfwfSBottom: gy = miny; break;
      case XfwfSLeftSide: gx = 0; break;
      case XfwfSRightSide: gx = minx; break;
      default: break;
      }
    
      if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.doScroll)
	XtMoveWidget(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, gx, gy);
    }

    if (info->reason != XfwfSNotify) {
	new.reason = info->reason;
	if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.autoAdjustScrollbars) {
	  new.flags  = XFWF_VPOS | XFWF_HPOS | XFWF_HSIZE | XFWF_VSIZE ;
	  new.hpos   = minx == 0 ? 0.0 : gx/minx;
	  new.vpos   = miny == 0 ? 0.0 : gy/miny;
	  new.hsize  = gwd <= boardwd ? 1.0 : (float)boardwd/(float)gwd;
	  new.vsize  = ght <= boardht ? 1.0 : (float)boardht/(float)ght;
	  new.gx     = gx;
	  new.gy     = gy;
	} else
	  new.flags  = 0;
	XtCallCallbackList(self, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollCallback, &new);
    }
}
#line 490 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 490 "XWidgets/xwScrollWin.w"
static void CW_killed(Widget  w,XtPointer  client_data,XtPointer  call_data)
#else
#line 490 "XWidgets/xwScrollWin.w"
static void CW_killed(w,client_data,call_data)Widget  w;XtPointer  client_data;XtPointer  call_data;
#endif
#line 491 "XWidgets/xwScrollWin.w"
{
    Widget self = (Widget) client_data;

    /* don't propagate key events from frame to child widget anymore */
    XtVaSetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNpropagateTarget, NULL, NULL);
    XtRemoveEventHandler(w, StructureNotifyMask, False, configure, self);
    XtRemoveEventHandler(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, StructureNotifyMask, False, configure, self);
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW = NULL;
    XfwfSetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, 0.0, 1.0);
    XfwfSetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, 0.0, 1.0);
}
#line 510 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 510 "XWidgets/xwScrollWin.w"
static void configure(Widget  w,XtPointer  client_data,XEvent * event,Boolean * cont)
#else
#line 510 "XWidgets/xwScrollWin.w"
static void configure(w,client_data,event,cont)Widget  w;XtPointer  client_data;XEvent * event;Boolean * cont;
#endif
#line 511 "XWidgets/xwScrollWin.w"
{
    Widget self = (Widget) client_data;
    int boardwd, boardht;
    Dimension gwd, ght;
    Position boardx, boardy, gx, gy;
    float wd, ht, x, y;

    if (((XfwfScrolledWindowWidget)self)->core.being_destroyed) {
	/*
	 * it may be possible that a configure event
	 * occures during destruction
	 */
	return;
    }

    /* -- There is nothing wrong, when the event is something else */
    /* -- than a ConfigureNotify. The if prevents correct setting of */
    /* -- scrollbars during the insert_child method */
    /* if (event != NULL && event->type != ConfigureNotify) return; */
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.autoAdjustScrollbars) {
      xfwfBoardClassRec.xfwfCommon_class.compute_inside(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, &boardx, &boardy, &boardwd, &boardht);
      boardwd = max(0, boardwd);
      boardht = max(0, boardht);
      XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, XtNx, &gx, XtNy, &gy, XtNwidth, &gwd,
	  	    XtNheight, &ght, NULL);
      wd = gwd <= boardwd ? 1.0 : (float) boardwd/gwd;
      ht = ght <= boardht ? 1.0 : (float) boardht/ght;
      x = gwd <= boardwd ? 0.0 : gx/(((float) boardwd) - gwd);
      y = ght <= boardht ? 0.0 : gy/(((float) boardht) - ght);
      if (x > 1.0) x = 1.0;
      if (y > 1.0) y = 1.0;
      XfwfSetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, x, wd);
      XfwfSetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, y, ht);
   }
}

static XtResource resources[] = {
#line 26 "XWidgets/xwScrollWin.w"
{XtNautoAdjustScrollbars,XtCAutoAdjustScrollbars,XtRBoolean,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.autoAdjustScrollbars),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.autoAdjustScrollbars),XtRImmediate,(XtPointer)TRUE },
#line 30 "XWidgets/xwScrollWin.w"
{XtNhighlightThickness,XtCHighlightThickness,XtRDimension,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfCommon.highlightThickness),XtOffsetOf(XfwfScrolledWindowRec,xfwfCommon.highlightThickness),XtRImmediate,(XtPointer)0 },
#line 35 "XWidgets/xwScrollWin.w"
{XtNtraverseToChild,XtCTraverseToChild,XtRBoolean,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.traverseToChild),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.traverseToChild),XtRImmediate,(XtPointer)TRUE },
#line 40 "XWidgets/xwScrollWin.w"
{XtNspacing,XtCSpacing,XtRDimension,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.spacing),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.spacing),XtRImmediate,(XtPointer)0 },
#line 45 "XWidgets/xwScrollWin.w"
{XtNscrollbarWidth,XtCScrollbarWidth,XtRDimension,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.scrollbarWidth),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.scrollbarWidth),XtRImmediate,(XtPointer)16 },
#line 52 "XWidgets/xwScrollWin.w"
{XtNshadowWidth,XtCShadowWidth,XtRDimension,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.shadowWidth),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.shadowWidth),XtRImmediate,(XtPointer)2 },
#line 57 "XWidgets/xwScrollWin.w"
{XtNhideHScrollbar,XtCHideHScrollbar,XtRBoolean,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.hideHScrollbar),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.hideHScrollbar),XtRImmediate,(XtPointer)False },
#line 61 "XWidgets/xwScrollWin.w"
{XtNhideVScrollbar,XtCHideVScrollbar,XtRBoolean,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.hideVScrollbar),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.hideVScrollbar),XtRImmediate,(XtPointer)False },
#line 69 "XWidgets/xwScrollWin.w"
{XtNvScrollAmount,XtCVScrollAmount,XtRInt,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.vScrollAmount),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.vScrollAmount),XtRImmediate,(XtPointer)20 },
#line 73 "XWidgets/xwScrollWin.w"
{XtNhScrollAmount,XtCHScrollAmount,XtRInt,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.hScrollAmount),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.hScrollAmount),XtRCallProc,(XtPointer)copy_vScrollAmount },
#line 79 "XWidgets/xwScrollWin.w"
{XtNinitialX,XtCInitialX,XtRPosition,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.initialX),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.initialX),XtRImmediate,(XtPointer)0 },
#line 83 "XWidgets/xwScrollWin.w"
{XtNinitialY,XtCInitialY,XtRPosition,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.initialY),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.initialY),XtRImmediate,(XtPointer)0 },
#line 122 "XWidgets/xwScrollWin.w"
{XtNdoScroll,XtCDoScroll,XtRBoolean,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.doScroll),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.doScroll),XtRImmediate,(XtPointer)TRUE },
#line 127 "XWidgets/xwScrollWin.w"
{XtNscrollCallback,XtCScrollCallback,XtRCallback,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.scrollCallback),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.scrollCallback),XtRImmediate,(XtPointer)NULL },
#line 134 "XWidgets/xwScrollWin.w"
{XtNscrollResponse,XtCScrollResponse,XtRXTCallbackProc,sizeof(((XfwfScrolledWindowRec*)NULL)->xfwfScrolledWindow.scrollResponse),XtOffsetOf(XfwfScrolledWindowRec,xfwfScrolledWindow.scrollResponse),XtRImmediate,(XtPointer)scroll_response },
};

XfwfScrolledWindowClassRec xfwfScrolledWindowClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfBoardClassRec,
/* class_name   	*/  "XfwfScrolledWindow",
/* widget_size  	*/  sizeof(XfwfScrolledWindowRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  XtInheritRealize,
/* actions      	*/  NULL,
/* num_actions  	*/  0,
/* resources    	*/  resources,
/* num_resources 	*/  15,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  destroy,
/* resize       	*/  resize,
/* expose       	*/  XtInheritExpose,
/* set_values   	*/  set_values,
/* set_values_hook 	*/  NULL,
/* set_values_almost 	*/  XtInheritSetValuesAlmost,
/* get_values+hook 	*/  NULL,
/* accept_focus 	*/  XtInheritAcceptFocus,
/* version      	*/  XtVersion,
/* callback_private 	*/  NULL,
/* tm_table      	*/  NULL,
/* query_geometry 	*/  XtInheritQueryGeometry,
/* display_acceleator 	*/  XtInheritDisplayAccelerator,
/* extension    	*/  NULL 
},
{ /* composite_class part */
XtInheritGeometryManager,
XtInheritChangeManaged,
insert_child,
XtInheritDeleteChild,
NULL
},
{ /* XfwfCommon_class part */
compute_inside,
XtInherit_total_frame_width,
XtInherit__expose,
XtInherit_highlight_border,
XtInherit_unhighlight_border,
XtInherit_hilite_callbacks,
XtInherit_would_accept_focus,
XtInherit_traverse,
XtInherit_lighter_color,
XtInherit_darker_color,
XtInherit_set_color,
/* traversal_trans */  NULL ,
/* traversal_trans_small */  NULL ,
/* travMode */  1 ,
},
{ /* XfwfFrame_class part */
 /* dummy */  0
},
{ /* XfwfBoard_class part */
XtInherit_set_abs_location,
},
{ /* XfwfScrolledWindow_class part */
scroll_response,
},
};
WidgetClass xfwfScrolledWindowWidgetClass = (WidgetClass) &xfwfScrolledWindowClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfScrolledWindowWidgetClass c = (XfwfScrolledWindowWidgetClass) class;
  XfwfScrolledWindowWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfScrolledWindowWidgetClass) return;
  super = (XfwfScrolledWindowWidgetClass)class->core_class.superclass;
  if (c->xfwfScrolledWindow_class.scroll_response == XtInherit_scroll_response)
    c->xfwfScrolledWindow_class.scroll_response = super->xfwfScrolledWindow_class.scroll_response;
}
#line 169 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 169 "XWidgets/xwScrollWin.w"
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 169 "XWidgets/xwScrollWin.w"
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 170 "XWidgets/xwScrollWin.w"
{
    XtPointer bg;

    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initializing = True;
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initialX > 0) ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initialX = 0;
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initialY > 0) ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initialY = 0;
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW = NULL; ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board = NULL;
    XtVaGetValues(self, XtNbackground, &bg, NULL),
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame = XtVaCreateManagedWidget
	("_frame", xfwfEnforcerWidgetClass, self,
	 XtNframeType, XfwfSunken, XtNframeWidth, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.shadowWidth,
	 XtNborderWidth, 0, XtNhighlightThickness, ((XfwfScrolledWindowWidget)self)->xfwfCommon.highlightThickness,
	 XtNtraversalOn, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.traverseToChild, 
	 XtNbackground, bg,
         XtNtraversalTranslationDone, ((XfwfScrolledWindowWidget)self)->xfwfCommon.traversalTranslationDone, 
	 NULL);
    ((XfwfScrolledWindowWidget)self)->xfwfCommon.highlightThickness = 0;
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board = XtVaCreateManagedWidget
	("_board", xfwfBoardWidgetClass, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNframeWidth, 0,
	 XtNborderWidth, 0, XtNhighlightThickness, 0,
	 XtNbackground, bg,
	 XtNtraversalOn, False, NULL);
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll = XtVaCreateWidget
	("_vscroll", xfwfVScrollbarWidgetClass, self, XtNframeWidth,
	 2, XtNframeType, XfwfSunken, XtNborderWidth, 0, 
	 XtNhighlightThickness, 0, XtNbackground, bg, NULL);
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll = XtVaCreateWidget
	("_hscroll", xfwfHScrollbarWidgetClass, self, XtNframeWidth,
	 2, XtNframeType, XfwfSunken, XtNborderWidth, 0, 
	 XtNhighlightThickness, 0, XtNbackground, bg, NULL);
    if (! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideVScrollbar) XtManageChild(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll);
    if (! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideHScrollbar) XtManageChild(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll);
    compute_sizes(self);
    XtAddCallback(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, XtNscrollCallback, scroll_callback, self);
    XtAddCallback(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, XtNscrollCallback, scroll_callback, self);
    XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, XtNscrollResponse, &((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll_resp, NULL);
    XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, XtNscrollResponse, &((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll_resp, NULL);
    ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initializing = False;
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollResponse != scroll_response) {
	((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollResponse = scroll_response;
	XtWarning("scrollResponse resource may only be queried, not set");
    }
}
#line 219 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 219 "XWidgets/xwScrollWin.w"
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 219 "XWidgets/xwScrollWin.w"
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 220 "XWidgets/xwScrollWin.w"
{
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.traverseToChild && !((XfwfScrolledWindowWidget)old)->xfwfCommon.traversalTranslationDone && ((XfwfScrolledWindowWidget)self)->xfwfCommon.traversalTranslationDone) {
	XtVaSetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNtraversalTranslationDone, ((XfwfScrolledWindowWidget)self)->xfwfCommon.traversalTranslationDone, NULL);
    }

    if (((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.hideVScrollbar && ! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideVScrollbar) {
	XtManageChild(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll);
    } else if (! ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.hideVScrollbar && ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideVScrollbar) {
	XtUnmanageChild(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll);
    }
    if (((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.hideHScrollbar && ! ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideHScrollbar) {
	XtManageChild(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll);
    } else if (! ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.hideHScrollbar && ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideHScrollbar) {
	XtUnmanageChild(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll);
    }
    if (((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.traverseToChild != ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.traverseToChild) {
	XtVaSetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNtraversalOn, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.traverseToChild, NULL);
	XtVaSetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNpropagateTarget, (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.traverseToChild ? ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW : NULL), NULL);
    }
    if (((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.spacing != ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.spacing
	|| ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.scrollbarWidth != ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollbarWidth
	|| ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.shadowWidth != ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.shadowWidth
	|| ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.hideHScrollbar != ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideHScrollbar
	|| ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.hideVScrollbar != ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hideVScrollbar)
	compute_sizes(self);
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollResponse != ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.scrollResponse) {
	((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollResponse = ((XfwfScrolledWindowWidget)old)->xfwfScrolledWindow.scrollResponse;
	XtWarning("scrollResponse resource may only be queried, not set");
    }
    return False;
}
#line 260 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 260 "XWidgets/xwScrollWin.w"
static void resize(Widget self)
#else
#line 260 "XWidgets/xwScrollWin.w"
static void resize(self)Widget self;
#endif
#line 261 "XWidgets/xwScrollWin.w"
{
    compute_sizes(self);
}
#line 273 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 174 "XWidgets/xwScrollWin.w"
static void insert_child(Widget  child)
#else
#line 174 "XWidgets/xwScrollWin.w"
static void insert_child(child)Widget  child;
#endif
{ Widget self = XtParent(child); {
    Position boardx, boardy, gx, gy;
    int boardwd, boardht;
    Dimension gwd, ght;
    Boolean dummy;

    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initializing) {
	xfwfBoardClassRec.composite_class.insert_child(child);
    } else if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW == NULL) {
	((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW = child;
	((XfwfScrolledWindowWidget)child)->core.parent = ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board;
	XtAddCallback(child, XtNdestroyCallback, CW_killed, self);
	xfwfBoardClassRec.composite_class.insert_child(child);
	XtAddEventHandler(child, StructureNotifyMask, False, configure, self);
	XtAddEventHandler(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, StructureNotifyMask, False, configure, self);
	xfwfBoardClassRec.xfwfCommon_class.compute_inside(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, &boardx, &boardy, &boardwd, &boardht);
        boardwd = max(0, boardwd);
        boardht = max(0, boardht);
	XtVaGetValues(child, XtNwidth, &gwd, XtNheight, &ght, NULL);
	gx = gwd <= boardwd ? 0 : max(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initialX, boardwd - gwd);
	gy = ght <= boardht ? 0 : max(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.initialY, boardht - ght);
	XtMoveWidget(child, gx, gy);
	/* propagate key events from frame to child widget */
	if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.traverseToChild)
	    XtVaSetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.frame, XtNpropagateTarget, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, NULL);
	configure(self, self, NULL, &dummy);
    } else {
	char s[500];
	(void) sprintf(s, "Cannot add <%s>, %s <%s> already has a child\n",
		       XtName(child), "ScrolledWindow", XtName(self));
	XtWarning(s);
    }
}
}
#line 311 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 311 "XWidgets/xwScrollWin.w"
static void compute_inside(Widget self,Position * x,Position * y,int * w,int * h)
#else
#line 311 "XWidgets/xwScrollWin.w"
static void compute_inside(self,x,y,w,h)Widget self;Position * x;Position * y;int * w;int * h;
#endif
#line 312 "XWidgets/xwScrollWin.w"
{
    xfwfBoardClassRec.xfwfCommon_class.compute_inside((((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board ? ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board : self), x, y, w, h);
}
#line 319 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 319 "XWidgets/xwScrollWin.w"
static void destroy(Widget self)
#else
#line 319 "XWidgets/xwScrollWin.w"
static void destroy(self)Widget self;
#endif
#line 320 "XWidgets/xwScrollWin.w"
{
    if (((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW != NULL) {
	XtRemoveEventHandler(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, StructureNotifyMask, False,
			     configure, self);
	XtRemoveEventHandler(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, StructureNotifyMask, False,
			     configure, self);
    }
}
#line 332 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 332 "XWidgets/xwScrollWin.w"
static void scroll_response(Widget  w,XtPointer  client_data,XtPointer  call_data)
#else
#line 332 "XWidgets/xwScrollWin.w"
static void scroll_response(w,client_data,call_data)Widget  w;XtPointer  client_data;XtPointer  call_data;
#endif
#line 333 "XWidgets/xwScrollWin.w"
{
    Widget self = (Widget) client_data;
    XfwfScrollInfo *info = (XfwfScrollInfo *) call_data;
    XfwfScrollInfo new;
    Position boardx, boardy, gx, gy, minx, miny;
    int boardwd, boardht;
    Dimension gwd, ght;

    xfwfBoardClassRec.xfwfCommon_class.compute_inside(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, &boardx, &boardy, &boardwd, &boardht);
    boardwd = max(0, boardwd);
    boardht = max(0, boardht);
    XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, XtNx, &gx, XtNy, &gy, XtNwidth, &gwd,
		  XtNheight, &ght, NULL);
    minx = gwd <= boardwd ? 0 : boardwd - gwd;
    miny = ght <= boardht ? 0 : boardht - ght;

    if (info->flags & XFWF_VPOS) gy = info->vpos * miny;
    if (info->flags & XFWF_HPOS) gx = info->hpos * minx;

    XtMoveWidget(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.CW, gx, gy);

    if (info->reason != XfwfSNotify && ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.doScroll) {
	new.reason = XfwfSNotify;
	new.flags = info->flags & (XFWF_VPOS | XFWF_HPOS);
	new.hpos = info->hpos;
	new.vpos = info->vpos;
	XtCallCallbackList(self, ((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.scrollCallback, info);
    }
}
#line 549 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 549 "XWidgets/xwScrollWin.w"
void xws_set_scroll_direct(Widget self,long  hlen,long  hpage,long  hpos,long  vlen,long  vpage,long  vpos)
#else
#line 549 "XWidgets/xwScrollWin.w"
void xws_set_scroll_direct(self,hlen,hpage,hpos,vlen,vpage,vpos)Widget self;long  hlen;long  hpage;long  hpos;long  vlen;long  vpage;long  vpos;
#endif
#line 550 "XWidgets/xwScrollWin.w"
{
  XfwfSetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, (float)hpos / hlen, (float)hpage / (hlen + hpage));
  XfwfSetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, (float)vpos / vlen, (float)vpage / (vlen + vpage));
}
#line 555 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 555 "XWidgets/xwScrollWin.w"
void xws_get_scroll_pos(Widget self,double * x,double * y)
#else
#line 555 "XWidgets/xwScrollWin.w"
void xws_get_scroll_pos(self,x,y)Widget self;double * x;double * y;
#endif
#line 556 "XWidgets/xwScrollWin.w"
{
  XfwfGetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.hscroll, x, NULL);
  XfwfGetScrollbar(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.vscroll, NULL, y);
}
#line 561 "XWidgets/xwScrollWin.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 561 "XWidgets/xwScrollWin.w"
void xws_get_scroll_area(Widget self,Dimension * x,Dimension * y)
#else
#line 561 "XWidgets/xwScrollWin.w"
void xws_get_scroll_area(self,x,y)Widget self;Dimension * x;Dimension * y;
#endif
#line 562 "XWidgets/xwScrollWin.w"
{
  XtVaGetValues(((XfwfScrolledWindowWidget)self)->xfwfScrolledWindow.board, XtNwidth, x, XtNheight, y, NULL);
}
