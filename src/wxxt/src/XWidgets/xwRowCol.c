/* Generated by wbuild
 * (generator version 3.2)
 */
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <./xwRowColP.h>
static void _resolve_inheritance(
#if NeedFunctionPrototypes
WidgetClass
#endif
);
#line 88 "XWidgets/xwRowCol.w"
static void change_managed(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 95 "XWidgets/xwRowCol.w"
static void insert_child(
#if NeedFunctionPrototypes
Widget 
#endif
);
#line 119 "XWidgets/xwRowCol.w"
static void layout(
#if NeedFunctionPrototypes
Widget,int ,Boolean 
#endif
);
#line 200 "XWidgets/xwRowCol.w"
static XtGeometryResult  geometry_manager(
#if NeedFunctionPrototypes
Widget ,XtWidgetGeometry *,XtWidgetGeometry *
#endif
);
#line 225 "XWidgets/xwRowCol.w"
static void resize(
#if NeedFunctionPrototypes
Widget
#endif
);
#line 234 "XWidgets/xwRowCol.w"
static void initialize(
#if NeedFunctionPrototypes
Widget ,Widget,ArgList ,Cardinal *
#endif
);
#line 244 "XWidgets/xwRowCol.w"
static Boolean  set_values(
#if NeedFunctionPrototypes
Widget ,Widget ,Widget,ArgList ,Cardinal *
#endif
);
#line 266 "XWidgets/xwRowCol.w"
static void align_child(
#if NeedFunctionPrototypes
Widget,int ,int ,int ,int ,Alignment 
#endif
);
#line 266 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 266 "XWidgets/xwRowCol.w"
static void align_child(Widget self,int  cx,int  cy,int  width,int  height,Alignment  alignment)
#else
#line 266 "XWidgets/xwRowCol.w"
static void align_child(self,cx,cy,width,height,alignment)Widget self;int  cx;int  cy;int  width;int  height;Alignment  alignment;
#endif
#line 267 "XWidgets/xwRowCol.w"
{
    Position x, y;

    if (alignment & XfwfLeft) x = cx;
    else if (alignment & XfwfRight) x = cx + width - ((XfwfRowColWidget)self)->core.width;
    else x = cx + (width - ((XfwfRowColWidget)self)->core.width) / 2;
    if (alignment & XfwfTop) y = cy;
    else if (alignment & XfwfBottom) y = cy + height - ((XfwfRowColWidget)self)->core.height;
    else y = cy + (height - ((XfwfRowColWidget)self)->core.height) / 2;
    XtMoveWidget(self, x, y);
}

static XtResource resources[] = {
#line 32 "XWidgets/xwRowCol.w"
{XtNstoreByRow,XtCStoreByRow,XtRBoolean,sizeof(((XfwfRowColRec*)NULL)->xfwfRowCol.storeByRow),XtOffsetOf(XfwfRowColRec,xfwfRowCol.storeByRow),XtRImmediate,(XtPointer)True },
#line 42 "XWidgets/xwRowCol.w"
{XtNrows,XtCRows,XtRInt,sizeof(((XfwfRowColRec*)NULL)->xfwfRowCol.rows),XtOffsetOf(XfwfRowColRec,xfwfRowCol.rows),XtRImmediate,(XtPointer)0 },
#line 43 "XWidgets/xwRowCol.w"
{XtNcolumns,XtCColumns,XtRInt,sizeof(((XfwfRowColRec*)NULL)->xfwfRowCol.columns),XtOffsetOf(XfwfRowColRec,xfwfRowCol.columns),XtRImmediate,(XtPointer)0 },
#line 52 "XWidgets/xwRowCol.w"
{XtNalignment,XtCAlignment,XtRAlignment,sizeof(((XfwfRowColRec*)NULL)->xfwfRowCol.alignment),XtOffsetOf(XfwfRowColRec,xfwfRowCol.alignment),XtRImmediate,(XtPointer)XfwfTopLeft },
#line 64 "XWidgets/xwRowCol.w"
{XtNshrinkToFit,XtCShrinkToFit,XtRBoolean,sizeof(((XfwfRowColRec*)NULL)->xfwfRowCol.shrinkToFit),XtOffsetOf(XfwfRowColRec,xfwfRowCol.shrinkToFit),XtRImmediate,(XtPointer)False },
#line 70 "XWidgets/xwRowCol.w"
{XtNframeType,XtCFrameType,XtRFrameType,sizeof(((XfwfRowColRec*)NULL)->xfwfFrame.frameType),XtOffsetOf(XfwfRowColRec,xfwfFrame.frameType),XtRImmediate,(XtPointer)XfwfSunken },
#line 71 "XWidgets/xwRowCol.w"
{XtNframeWidth,XtCFrameWidth,XtRDimension,sizeof(((XfwfRowColRec*)NULL)->xfwfFrame.frameWidth),XtOffsetOf(XfwfRowColRec,xfwfFrame.frameWidth),XtRImmediate,(XtPointer)2 },
};

XfwfRowColClassRec xfwfRowColClassRec = {
{ /* core_class part */
/* superclass   	*/  (WidgetClass) &xfwfBoardClassRec,
/* class_name   	*/  "XfwfRowCol",
/* widget_size  	*/  sizeof(XfwfRowColRec),
/* class_initialize 	*/  NULL,
/* class_part_initialize*/  _resolve_inheritance,
/* class_inited 	*/  FALSE,
/* initialize   	*/  initialize,
/* initialize_hook 	*/  NULL,
/* realize      	*/  XtInheritRealize,
/* actions      	*/  NULL,
/* num_actions  	*/  0,
/* resources    	*/  resources,
/* num_resources 	*/  7,
/* xrm_class    	*/  NULLQUARK,
/* compres_motion 	*/  True ,
/* compress_exposure 	*/  XtExposeCompressMultiple ,
/* compress_enterleave 	*/  True ,
/* visible_interest 	*/  False ,
/* destroy      	*/  NULL,
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
geometry_manager,
change_managed,
insert_child,
XtInheritDeleteChild,
NULL
},
{ /* XfwfCommon_class part */
XtInherit_compute_inside,
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
{ /* XfwfRowCol_class part */
layout,
},
};
WidgetClass xfwfRowColWidgetClass = (WidgetClass) &xfwfRowColClassRec;
static void _resolve_inheritance(class)
WidgetClass class;
{
  XfwfRowColWidgetClass c = (XfwfRowColWidgetClass) class;
  XfwfRowColWidgetClass super;
  static CompositeClassExtensionRec extension_rec = {
    NULL, NULLQUARK, XtCompositeExtensionVersion,
    sizeof(CompositeClassExtensionRec), True};
  CompositeClassExtensionRec *ext;
  ext = (XtPointer)XtMalloc(sizeof(*ext));
  *ext = extension_rec;
  ext->next_extension = c->composite_class.extension;
  c->composite_class.extension = ext;
  if (class == xfwfRowColWidgetClass) return;
  super = (XfwfRowColWidgetClass)class->core_class.superclass;
  if (c->xfwfRowCol_class.layout == XtInherit_layout)
    c->xfwfRowCol_class.layout = super->xfwfRowCol_class.layout;
}
#line 88 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 88 "XWidgets/xwRowCol.w"
static void change_managed(Widget self)
#else
#line 88 "XWidgets/xwRowCol.w"
static void change_managed(self)Widget self;
#endif
#line 89 "XWidgets/xwRowCol.w"
{
    ((XfwfRowColWidgetClass)self->core.widget_class)->xfwfRowCol_class.layout(self, ((XfwfRowColWidget)self)->xfwfRowCol.shrinkToFit, True);
}
#line 95 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 174 "XWidgets/xwRowCol.w"
static void insert_child(Widget  child)
#else
#line 174 "XWidgets/xwRowCol.w"
static void insert_child(child)Widget  child;
#endif
{ Widget self = XtParent(child); {
    xfwfBoardClassRec.composite_class.insert_child(child);
    if (((XfwfRowColWidget)self)->xfwfRowCol.shrinkToFit)
	((XfwfRowColWidgetClass)self->core.widget_class)->xfwfRowCol_class.layout(self, ((XfwfRowColWidget)self)->xfwfRowCol.shrinkToFit, False);
}
}
#line 119 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 119 "XWidgets/xwRowCol.w"
static void layout(Widget self,int  shrink,Boolean  align)
#else
#line 119 "XWidgets/xwRowCol.w"
static void layout(self,shrink,align)Widget self;int  shrink;Boolean  align;
#endif
#line 120 "XWidgets/xwRowCol.w"
{
    int nrows, ncols, i, nchild, n;
    Position left, top, x, y;
    Dimension width, height, w, h;
    Widget child;

    nchild = 0;
    ((XfwfRowColWidget)self)->xfwfRowCol.max_width = 0;
    ((XfwfRowColWidget)self)->xfwfRowCol.max_height = 0;
    for (i = 0; i < ((XfwfRowColWidget)self)->composite.num_children; i++) {
        child = ((XfwfRowColWidget)self)->composite.children[i];
        if (align && ! XtIsManaged(child)) continue;
        nchild++;
        ((XfwfRowColWidget)self)->xfwfRowCol.max_width = max(((XfwfRowColWidget)self)->xfwfRowCol.max_width, ((XfwfRowColWidget)child)->core.width + 2*((XfwfRowColWidget)child)->core.border_width);
        ((XfwfRowColWidget)self)->xfwfRowCol.max_height = max(((XfwfRowColWidget)self)->xfwfRowCol.max_height, ((XfwfRowColWidget)child)->core.height + 2*((XfwfRowColWidget)child)->core.border_width);
    }

    ((XfwfRowColWidgetClass)self->core.widget_class)->xfwfCommon_class.compute_inside(self, &left, &top, &width, &height);

    if (((XfwfRowColWidget)self)->xfwfRowCol.columns != 0) {
        ncols = ((XfwfRowColWidget)self)->xfwfRowCol.columns;
        nrows = (nchild + ncols - 1)/ncols;
    } else if (((XfwfRowColWidget)self)->xfwfRowCol.rows != 0) {
        nrows = ((XfwfRowColWidget)self)->xfwfRowCol.rows;
        ncols = (nchild + nrows - 1)/nrows;
    } else {
        ncols = ((XfwfRowColWidget)self)->xfwfRowCol.max_width != 0 ? width/((XfwfRowColWidget)self)->xfwfRowCol.max_width : 1;
        if (ncols == 0) ncols = 1;
        nrows = (nchild + ncols - 1)/ncols;
    }

    x = left;
    y = top;
    n = 0;
    if (((XfwfRowColWidget)self)->xfwfRowCol.storeByRow) {
        for (i = 0; i < ((XfwfRowColWidget)self)->composite.num_children; i++) {
            child = ((XfwfRowColWidget)self)->composite.children[i];
            if (align && ! XtIsManaged(child)) continue;
	    if (align)
		align_child(child, x, y, ((XfwfRowColWidget)self)->xfwfRowCol.max_width, ((XfwfRowColWidget)self)->xfwfRowCol.max_height, ((XfwfRowColWidget)self)->xfwfRowCol.alignment);
            n++;
            if (n == ncols) {
                n = 0;
                x = left;
                y += ((XfwfRowColWidget)self)->xfwfRowCol.max_height;
            } else
                x += ((XfwfRowColWidget)self)->xfwfRowCol.max_width;
        }
    } else {
        for (i = 0; i < ((XfwfRowColWidget)self)->composite.num_children; i++) {
            child = ((XfwfRowColWidget)self)->composite.children[i];
            if (align && ! XtIsManaged(child)) continue;
	    if (align)
		align_child(child, x, y, ((XfwfRowColWidget)self)->xfwfRowCol.max_width, ((XfwfRowColWidget)self)->xfwfRowCol.max_height, ((XfwfRowColWidget)self)->xfwfRowCol.alignment);
            n++;
            if (n == nrows) {
                n = 0;
                y = top;
                x += ((XfwfRowColWidget)self)->xfwfRowCol.max_width;
            } else
                y += ((XfwfRowColWidget)self)->xfwfRowCol.max_height;
        }
    }

    if (shrink) {
      w = 2*left + ncols * ((XfwfRowColWidget)self)->xfwfRowCol.max_width;
      h = 2*top + nrows * ((XfwfRowColWidget)self)->xfwfRowCol.max_height;
      XtVaSetValues(self, XtNwidth, w, XtNheight, h, NULL);
      /* printf("RowCol width/height : %d/%d\n", w, h); */
    }
}
#line 200 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 167 "XWidgets/xwRowCol.w"
static XtGeometryResult  geometry_manager(Widget  child,XtWidgetGeometry * request,XtWidgetGeometry * reply)
#else
#line 167 "XWidgets/xwRowCol.w"
static XtGeometryResult  geometry_manager(child,request,reply)Widget  child;XtWidgetGeometry * request;XtWidgetGeometry * reply;
#endif
{ Widget self = XtParent(child); {
    Dimension newwd, newht, newbd;

    if (request->request_mode & (CWX | CWY)) return XtGeometryNo;
    if (request->request_mode & XtCWQueryOnly) return XtGeometryYes;

    newwd = request->request_mode & CWWidth ? request->width : ((XfwfRowColWidget)child)->core.width;
    newht = request->request_mode & CWHeight ? request->height : ((XfwfRowColWidget)child)->core.height;
    newbd = request->request_mode & CWBorderWidth
        ? request->border_width : ((XfwfRowColWidget)child)->core.border_width;

    if (newwd == ((XfwfRowColWidget)child)->core.width && newht == ((XfwfRowColWidget)child)->core.height
        && newbd == ((XfwfRowColWidget)child)->core.border_width) return XtGeometryNo;

    XtResizeWidget(child, newwd, newht, newbd);
    ((XfwfRowColWidgetClass)self->core.widget_class)->xfwfRowCol_class.layout(self, ((XfwfRowColWidget)self)->xfwfRowCol.shrinkToFit, True);
    return XtGeometryDone;
}
}
#line 225 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 225 "XWidgets/xwRowCol.w"
static void resize(Widget self)
#else
#line 225 "XWidgets/xwRowCol.w"
static void resize(self)Widget self;
#endif
#line 226 "XWidgets/xwRowCol.w"
{
    if (((XfwfRowColWidget)self)->xfwfRowCol.rows == 0 && ((XfwfRowColWidget)self)->xfwfRowCol.columns == 0) ((XfwfRowColWidgetClass)self->core.widget_class)->xfwfRowCol_class.layout(self, False, True);
}
#line 234 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 234 "XWidgets/xwRowCol.w"
static void initialize(Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 234 "XWidgets/xwRowCol.w"
static void initialize(request,self,args,num_args)Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 235 "XWidgets/xwRowCol.w"
{
    ((XfwfRowColWidget)self)->xfwfRowCol.max_width = ((XfwfRowColWidget)self)->xfwfRowCol.max_height = 0;
}
#line 244 "XWidgets/xwRowCol.w"
/*ARGSUSED*/
#if NeedFunctionPrototypes
#line 244 "XWidgets/xwRowCol.w"
static Boolean  set_values(Widget  old,Widget  request,Widget self,ArgList  args,Cardinal * num_args)
#else
#line 244 "XWidgets/xwRowCol.w"
static Boolean  set_values(old,request,self,args,num_args)Widget  old;Widget  request;Widget self;ArgList  args;Cardinal * num_args;
#endif
#line 245 "XWidgets/xwRowCol.w"
{
    Boolean need_layout = False;
    Boolean need_redisplay = False;

    if (((XfwfRowColWidget)old)->xfwfRowCol.storeByRow != ((XfwfRowColWidget)self)->xfwfRowCol.storeByRow) need_layout = True;
    if (((XfwfRowColWidget)old)->xfwfRowCol.rows != ((XfwfRowColWidget)self)->xfwfRowCol.rows) need_layout = True;
    if (((XfwfRowColWidget)old)->xfwfRowCol.columns != ((XfwfRowColWidget)self)->xfwfRowCol.columns) need_layout = True;
    if (((XfwfRowColWidget)old)->xfwfRowCol.alignment != ((XfwfRowColWidget)self)->xfwfRowCol.alignment) need_layout = True;
    if (((XfwfRowColWidget)old)->xfwfRowCol.shrinkToFit != ((XfwfRowColWidget)self)->xfwfRowCol.shrinkToFit) need_layout = True;
    if (need_layout) {
        ((XfwfRowColWidgetClass)self->core.widget_class)->xfwfRowCol_class.layout(self, ((XfwfRowColWidget)self)->xfwfRowCol.shrinkToFit, True);
        need_redisplay = True;
    }
    return need_redisplay;
}
