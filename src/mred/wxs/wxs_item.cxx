/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_item.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_item.h"
#include "wx_messg.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_item.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif




// @INCLUDE wxs_icol.xci


// @ "set-background-colour" : void SetBackgroundColour(wxColour!);
// @ "set-label-colour" : void SetLabelColour(wxColour!);
// @ "set-button-colour" : void SetButtonColour(wxColour!);

class os_wxItem : public wxItem {
 public:

  ~os_wxItem();
#ifdef MZ_PRECISE_GC
  void gcMark(Mark_Proc mark);
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxItem::gcMark(Mark_Proc mark) {
  wxItem::gcMark(mark);
  if (mark) {
  }
}
#endif

static Scheme_Object *os_wxItem_class;

os_wxItem::~os_wxItem()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

#pragma argsused
static Scheme_Object *os_wxItemSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  string x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "set-label in item%"));

  
  WITH_VAR_STACK(((wxItem *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxItemGetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  nstring r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxItem *)((Scheme_Class_Object *)obj)->primdata)->GetLabel());

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_string((char *)r));
}

#pragma argsused
static Scheme_Object *os_wxItemCommand(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  class wxCommandEvent* x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxCommandEvent(p[0], "command in item%", 0));

  
  WITH_VAR_STACK(((wxItem *)((Scheme_Class_Object *)obj)->primdata)->Command(x0));

  
  
  return scheme_void;
}

void objscheme_setup_wxItem(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxItem_class);

  os_wxItem_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "item%", "window%", NULL, 3));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxItem_class, "set-label", os_wxItemSetLabel, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxItem_class, "get-label", os_wxItemGetLabel, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxItem_class, "command", os_wxItemCommand, 1, 1));


  WITH_VAR_STACK(scheme_made_class(os_wxItem_class));

  WITH_VAR_STACK(objscheme_install_bundler((Objscheme_Bundler)objscheme_bundle_wxItem, wxTYPE_ITEM));

}

int objscheme_istype_wxItem(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxItem_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "item% object or " XC_NULL_STR: "item% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxItem(class wxItem *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((realobj->__type != wxTYPE_ITEM) && (sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxItem_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxItem *objscheme_unbundle_wxItem(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxItem(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxItem *)o->primdata;
  else
    return (wxItem *)o->primdata;
}


static int istype_symset_messageStyle(Scheme_Object *v, const char *where) {
  if SCHEME_NULLP(v) return 1;
  if (where) scheme_wrong_type(where, "messageStyle symbol list", -1, 0, &v);
  return 0;
}

static int unbundle_symset_messageStyle(Scheme_Object *v, const char *where) {
  istype_symset_messageStyle(v, where);
  return 0;
}










class os_wxMessage : public wxMessage {
 public:

  os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, string x1, int x2 = -1, int x3 = -1, int x4 = 0, string x5 = "message"));
#ifndef MZ_PRECISE_GC
  os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, class wxBitmap* x1, int x2 = -1, int x3 = -1, int x4 = 0, string x5 = "message"));
#endif
  ~os_wxMessage();
  void OnDropFile(pathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
#ifdef MZ_PRECISE_GC
  void gcMark(Mark_Proc mark);
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxMessage::gcMark(Mark_Proc mark) {
  wxMessage::gcMark(mark);
  if (mark) {
  }
}
#endif

static Scheme_Object *os_wxMessage_class;

os_wxMessage::os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, string x1, int x2, int x3, int x4, string x5))
CONSTRUCTOR_INIT(: wxMessage(x0, x1, x2, x3, x4, x5))
{
}

#ifndef MZ_PRECISE_GC
os_wxMessage::os_wxMessage CONSTRUCTOR_ARGS((class wxPanel* x0, class wxBitmap* x1, int x2, int x3, int x4, string x5))
CONSTRUCTOR_INIT(: wxMessage(x0, x1, x2, x3, x4, x5))
{
}
#endif

os_wxMessage::~os_wxMessage()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxMessage::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxMessage::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in message%"", extracting return value"));
  }
}

Bool os_wxMessage::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return 1; }

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in message%"", extracting return value"));
  }
}

void os_wxMessage::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  
  
  }
}

void os_wxMessage::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxMessage::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxMessage *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxMessage_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxMessage::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxMessageSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(2);
  PRE_VAR_STACK_PUSH(0, p);
  PRE_VAR_STACK_PUSH(1, obj);
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  if ((n >= 1) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[0], NULL, 0))) {
    class wxBitmap* x0;

    SETUP_VAR_STACK_REMEMBERED(3);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x0);

    
    if (n != 1) 
      WITH_VAR_STACK(scheme_wrong_count("set-label in message% (bitmap label case)", 1, 1, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[0], "set-label in message% (bitmap label case)", 0));

    { if (x0 && !x0->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","set-label"), "bad bitmap: ", p[0])); if (x0 && BM_SELECTED(x0)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","set-label"), "bitmap is currently installed into a bitmap-dc%: ", p[0])); }
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0));

    
    
  } else  {
    string x0;

    SETUP_VAR_STACK_REMEMBERED(3);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x0);

    
    if (n != 1) 
      WITH_VAR_STACK(scheme_wrong_count("set-label in message% (string label case)", 1, 1, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "set-label in message% (string label case)"));

    
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0));

    
    
  }

  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessageOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  pathname x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "on-drop-file in message%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessagePreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxMouseEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-event in message%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in message%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxMessagePreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxWindow* x0;
  class wxKeyEvent* x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-char in message%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in message%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxMessageOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "on-size in message%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "on-size in message%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessageOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnSetFocus());
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessageOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxMessage *)((Scheme_Class_Object *)obj)->primdata)->wxMessage::OnKillFocus());
  else
    WITH_VAR_STACK(((wxMessage *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxMessage_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(2);
  PRE_VAR_STACK_PUSH(0, obj);
  PRE_VAR_STACK_PUSH(1, p);
  os_wxMessage *realobj;
  REMEMBER_VAR_STACK();
  if ((n >= 2) && WITH_REMEMBERED_STACK(objscheme_istype_wxPanel(p[0], NULL, 0)) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[1], NULL, 0))) {
    class wxPanel* x0;
    class wxBitmap* x1;
    int x2;
    int x3;
    int x4;
    string x5;

    SETUP_VAR_STACK_REMEMBERED(6);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, realobj);
    VAR_STACK_PUSH(3, x0);
    VAR_STACK_PUSH(4, x1);
    VAR_STACK_PUSH(5, x5);

    
    if ((n < 2) ||(n > 6)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in message% (bitmap label case)", 2, 6, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in message% (bitmap label case)", 0));
    x1 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[1], "initialization in message% (bitmap label case)", 0));
    if (n > 2) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[2], "initialization in message% (bitmap label case)"));
    } else
      x2 = -1;
    if (n > 3) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in message% (bitmap label case)"));
    } else
      x3 = -1;
    if (n > 4) {
      x4 = WITH_VAR_STACK(unbundle_symset_messageStyle(p[4], "initialization in message% (bitmap label case)"));
    } else
      x4 = 0;
    if (n > 5) {
      x5 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[5], "initialization in message% (bitmap label case)"));
    } else
      x5 = "message";

    { if (x1 && !x1->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","initialization"), "bad bitmap: ", p[1])); if (x1 && BM_SELECTED(x1)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("message%","initialization"), "bitmap is currently installed into a bitmap-dc%: ", p[1])); }
    realobj = WITH_VAR_STACK(new os_wxMessage CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxMessage(x0, x1, x2, x3, x4, x5));
#endif
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    
    
  } else  {
    class wxPanel* x0;
    string x1;
    int x2;
    int x3;
    int x4;
    string x5;

    SETUP_VAR_STACK_REMEMBERED(6);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, realobj);
    VAR_STACK_PUSH(3, x0);
    VAR_STACK_PUSH(4, x1);
    VAR_STACK_PUSH(5, x5);

    
    if ((n < 2) ||(n > 6)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in message% (string label case)", 2, 6, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in message% (string label case)", 0));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "initialization in message% (string label case)"));
    if (n > 2) {
      x2 = WITH_VAR_STACK(objscheme_unbundle_integer(p[2], "initialization in message% (string label case)"));
    } else
      x2 = -1;
    if (n > 3) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in message% (string label case)"));
    } else
      x3 = -1;
    if (n > 4) {
      x4 = WITH_VAR_STACK(unbundle_symset_messageStyle(p[4], "initialization in message% (string label case)"));
    } else
      x4 = 0;
    if (n > 5) {
      x5 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[5], "initialization in message% (string label case)"));
    } else
      x5 = "message";

    
    realobj = WITH_VAR_STACK(new os_wxMessage CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxMessage(x0, x1, x2, x3, x4, x5));
#endif
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    
    
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxMessage(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxMessage_class);

  os_wxMessage_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "message%", "item%", os_wxMessage_ConstructScheme, 7));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "set-label", os_wxMessageSetLabel, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-drop-file", os_wxMessageOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "pre-on-event", os_wxMessagePreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "pre-on-char", os_wxMessagePreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-size", os_wxMessageOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-set-focus", os_wxMessageOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMessage_class, "on-kill-focus", os_wxMessageOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxMessage_class));


}

int objscheme_istype_wxMessage(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxMessage_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "message% object or " XC_NULL_STR: "message% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxMessage(class wxMessage *realobj)
{
  Scheme_Class_Object *obj;
  Scheme_Object *sobj;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    return sobj;
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxMessage_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxMessage *objscheme_unbundle_wxMessage(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxMessage(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxMessage *)o->primdata;
  else
    return (wxMessage *)o->primdata;
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
