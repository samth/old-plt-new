/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_ckbx.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_check.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_ckbx.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static int istype_symset_checkboxStyle(Scheme_Object *v, const char *where) {
  if SCHEME_NULLP(v) return 1;
  if (where) scheme_wrong_type(where, "checkboxStyle symbol list", -1, 0, &v);
  return 0;
}

static int unbundle_symset_checkboxStyle(Scheme_Object *v, const char *where) {
  istype_symset_checkboxStyle(v, where);
  return 0;
}



#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxCheckBox
#define CB_REALCLASS wxCheckBox
#define CB_UNBUNDLE objscheme_unbundle_wxCheckBox
#define CB_USER METHODNAME("check-box%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxCheckBoxCallbackToScheme
#define CB_TOC wxCheckBoxCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);









class os_wxCheckBox : public wxCheckBox {
 public:
  Scheme_Object *callback_closure;

  os_wxCheckBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, string x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "checkBox"));
#ifndef MZ_PRECISE_GC
  os_wxCheckBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string x8 = "checkBox"));
#endif
  ~os_wxCheckBox();
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
void os_wxCheckBox::gcMark(Mark_Proc mark) {
  wxCheckBox::gcMark(mark);
  if (mark) {
    gcMARK_TYPED(Scheme_Object *, callback_closure);
  }
}
#endif

static Scheme_Object *os_wxCheckBox_class;

os_wxCheckBox::os_wxCheckBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, string x2, int x3, int x4, int x5, int x6, int x7, string x8))
CONSTRUCTOR_INIT(: wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8))
{
}

#ifndef MZ_PRECISE_GC
os_wxCheckBox::os_wxCheckBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, class wxBitmap* x2, int x3, int x4, int x5, int x6, int x7, string x8))
CONSTRUCTOR_INIT(: wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8))
{
}
#endif

os_wxCheckBox::~os_wxCheckBox()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

void os_wxCheckBox::OnDropFile(pathname x0)
{
  Scheme_Object *p[1];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxCheckBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxCheckBox::OnDropFile(x0);
  } else {
  mz_jmp_buf savebuf;
  p[0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 1, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

Bool os_wxCheckBox::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxCheckBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "pre-on-event", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in check-box%"", extracting return value"));
  }
}

Bool os_wxCheckBox::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxCheckBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "pre-on-char", &mcache);
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
  
  return WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in check-box%"", extracting return value"));
  }
}

void os_wxCheckBox::OnSize(int x0, int x1)
{
  Scheme_Object *p[2];
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxCheckBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, 2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxCheckBox::OnSize(x0, x1);
  } else {
  
  p[0] = scheme_make_integer(x0);
  p[1] = scheme_make_integer(x1);
  

  v = WITH_VAR_STACK(scheme_apply(method, 2, p));
  
  
  }
}

void os_wxCheckBox::OnSetFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxCheckBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxCheckBox::OnSetFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

void os_wxCheckBox::OnKillFocus()
{
  Scheme_Object **p = NULL;
  Scheme_Object *v;
  Scheme_Object *method;
#ifdef MZ_PRECISE_GC
  os_wxCheckBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *)__gc_external, os_wxCheckBox_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method)) {
    SET_VAR_STACK();
    ASSELF wxCheckBox::OnKillFocus();
  } else {
  mz_jmp_buf savebuf;
  COPY_JMPBUF(savebuf, scheme_error_buf); if (scheme_setjmp(scheme_error_buf)) { COPY_JMPBUF(scheme_error_buf, savebuf); return; }

  v = WITH_VAR_STACK(scheme_apply(method, 0, p));
  COPY_JMPBUF(scheme_error_buf, savebuf);
  
  }
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
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
      WITH_VAR_STACK(scheme_wrong_count("set-label in check-box% (bitmap label case)", 1, 1, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[0], "set-label in check-box% (bitmap label case)", 0));

    { if (x0 && !x0->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("check-box%","set-label"), "bad bitmap: ", p[0])); if (x0 && BM_SELECTED(x0)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("check-box%","set-label"), "bitmap is currently installed into a bitmap-dc%: ", p[0])); }
    WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0));

    
    
  } else  {
    string x0;

    SETUP_VAR_STACK_REMEMBERED(3);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x0);

    
    if (n != 1) 
      WITH_VAR_STACK(scheme_wrong_count("set-label in check-box% (string label case)", 1, 1, n, p));
    x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "set-label in check-box% (string label case)"));

    
    WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0));

    
    
  }

  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxSetValue(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  Bool x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[0], "set-value in check-box%"));

  
  WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->SetValue(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxGetValue(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->GetValue());

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnDropFile(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  pathname x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (pathname)WITH_VAR_STACK(objscheme_unbundle_pathname(p[0], "on-drop-file in check-box%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnDropFile(x0));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxPreOnEvent(Scheme_Object *obj, int n,  Scheme_Object *p[])
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

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-event in check-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[1], "pre-on-event in check-box%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnEvent(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxPreOnChar(Scheme_Object *obj, int n,  Scheme_Object *p[])
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

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[0], "pre-on-char in check-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[1], "pre-on-char in check-box%", 0));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    r = WITH_VAR_STACK(((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->PreOnChar(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnSize(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "on-size in check-box%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "on-size in check-box%"));

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnSize(x0, x1));

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnSetFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnSetFocus());
  else
    WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnSetFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBoxOnKillFocus(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  if (((Scheme_Class_Object *)obj)->primflag)
    WITH_VAR_STACK(((os_wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->wxCheckBox::OnKillFocus());
  else
    WITH_VAR_STACK(((wxCheckBox *)((Scheme_Class_Object *)obj)->primdata)->OnKillFocus());

  
  
  return scheme_void;
}

#pragma argsused
static Scheme_Object *os_wxCheckBox_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(2);
  PRE_VAR_STACK_PUSH(0, obj);
  PRE_VAR_STACK_PUSH(1, p);
  os_wxCheckBox *realobj;
  REMEMBER_VAR_STACK();
  if ((n >= 3) && WITH_REMEMBERED_STACK(objscheme_istype_wxPanel(p[0], NULL, 0)) && (SCHEME_NULLP(p[1]) || WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], NULL))) && WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap(p[2], NULL, 0))) {
    class wxPanel* x0;
    wxFunction x1;
    class wxBitmap* x2;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string x8;

    SETUP_VAR_STACK_REMEMBERED(6);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, realobj);
    VAR_STACK_PUSH(3, x0);
    VAR_STACK_PUSH(4, x2);
    VAR_STACK_PUSH(5, x8);

    int cb_pos = 0;
    if ((n < 3) ||(n > 9)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in check-box% (bitmap label case)", 3, 9, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in check-box% (bitmap label case)", 0));
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = WITH_VAR_STACK(objscheme_unbundle_wxBitmap(p[2], "initialization in check-box% (bitmap label case)", 0));
    if (n > 3) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in check-box% (bitmap label case)"));
    } else
      x3 = -1;
    if (n > 4) {
      x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[4], "initialization in check-box% (bitmap label case)"));
    } else
      x4 = -1;
    if (n > 5) {
      x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[5], "initialization in check-box% (bitmap label case)"));
    } else
      x5 = -1;
    if (n > 6) {
      x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[6], "initialization in check-box% (bitmap label case)"));
    } else
      x6 = -1;
    if (n > 7) {
      x7 = WITH_VAR_STACK(unbundle_symset_checkboxStyle(p[7], "initialization in check-box% (bitmap label case)"));
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[8], "initialization in check-box% (bitmap label case)"));
    } else
      x8 = "checkBox";

    { if (x2 && !x2->Ok()) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("check-box%","initialization"), "bad bitmap: ", p[2])); if (x2 && BM_SELECTED(x2)) WITH_VAR_STACK(scheme_arg_mismatch(METHODNAME("check-box%","initialization"), "bitmap is currently installed into a bitmap-dc%: ", p[2])); }if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = WITH_VAR_STACK(new os_wxCheckBox CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8));
#endif
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    
    realobj->callback_closure = p[cb_pos];
  } else  {
    class wxPanel* x0;
    wxFunction x1;
    string x2;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string x8;

    SETUP_VAR_STACK_REMEMBERED(6);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, realobj);
    VAR_STACK_PUSH(3, x0);
    VAR_STACK_PUSH(4, x2);
    VAR_STACK_PUSH(5, x8);

    int cb_pos = 0;
    if ((n < 3) ||(n > 9)) 
      WITH_VAR_STACK(scheme_wrong_count("initialization in check-box% (string label case)", 3, 9, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[0], "initialization in check-box% (string label case)", 0));
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[2], "initialization in check-box% (string label case)"));
    if (n > 3) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[3], "initialization in check-box% (string label case)"));
    } else
      x3 = -1;
    if (n > 4) {
      x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[4], "initialization in check-box% (string label case)"));
    } else
      x4 = -1;
    if (n > 5) {
      x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[5], "initialization in check-box% (string label case)"));
    } else
      x5 = -1;
    if (n > 6) {
      x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[6], "initialization in check-box% (string label case)"));
    } else
      x6 = -1;
    if (n > 7) {
      x7 = WITH_VAR_STACK(unbundle_symset_checkboxStyle(p[7], "initialization in check-box% (string label case)"));
    } else
      x7 = 0;
    if (n > 8) {
      x8 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[8], "initialization in check-box% (string label case)"));
    } else
      x8 = "checkBox";

    if (!x5) x5 = -1;if (!x6) x6 = -1;
    realobj = WITH_VAR_STACK(new os_wxCheckBox CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxCheckBox(x0, x1, x2, x3, x4, x5, x6, x7, x8));
#endif
    realobj->__gc_external = (void *)obj;
    objscheme_note_creation(obj);
    
    realobj->callback_closure = p[cb_pos];
  }

  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxCheckBox(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxCheckBox_class);

  os_wxCheckBox_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "check-box%", "item%", os_wxCheckBox_ConstructScheme, 9));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "set-label", os_wxCheckBoxSetLabel, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "set-value", os_wxCheckBoxSetValue, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "get-value", os_wxCheckBoxGetValue, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "on-drop-file", os_wxCheckBoxOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "pre-on-event", os_wxCheckBoxPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "pre-on-char", os_wxCheckBoxPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "on-size", os_wxCheckBoxOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "on-set-focus", os_wxCheckBoxOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxCheckBox_class, "on-kill-focus", os_wxCheckBoxOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxCheckBox_class));


}

int objscheme_istype_wxCheckBox(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxCheckBox_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "check-box% object or " XC_NULL_STR: "check-box% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxCheckBox(class wxCheckBox *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxCheckBox_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxCheckBox *objscheme_unbundle_wxCheckBox(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxCheckBox(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxCheckBox *)o->primdata;
  else
    return (wxCheckBox *)o->primdata;
}



static void CB_TOSCHEME(CB_REALCLASS *realobj, wxCommandEvent *event)
{
  Scheme_Object *p[2];
  Scheme_Class_Object *obj;
  mz_jmp_buf savebuf;
  SETUP_VAR_STACK(4);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, event);
  VAR_STACK_PUSH(2, p[0]);
  VAR_STACK_PUSH(3, p[1]);

  obj = (Scheme_Class_Object *)realobj->__gc_external;

  if (!obj) {
    // scheme_signal_error("bad callback object");
    return;
  }

  p[0] = (Scheme_Object *)obj;
  p[1] = WITH_VAR_STACK(objscheme_bundle_wxCommandEvent(event));

  COPY_JMPBUF(savebuf, scheme_error_buf);

  if (!scheme_setjmp(scheme_error_buf))
    WITH_VAR_STACK(scheme_apply_multi(((CALLBACKCLASS *)obj->primdata)->callback_closure, 2, p));

  COPY_JMPBUF(scheme_error_buf, savebuf);
}
#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
