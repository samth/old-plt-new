/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_rado.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif
#if defined(OS_X)
# include "common.h"
#endif

#include "wx_rbox.h"




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#include "wxscheme.h"
#include "wxs_rado.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

static Scheme_Object *radioboxStyle_wxVERTICAL_sym = NULL;
static Scheme_Object *radioboxStyle_wxHORIZONTAL_sym = NULL;
static Scheme_Object *radioboxStyle_wxINVISIBLE_sym = NULL;
static Scheme_Object *radioboxStyle_wxVERTICAL_LABEL_sym = NULL;
static Scheme_Object *radioboxStyle_wxHORIZONTAL_LABEL_sym = NULL;

static void init_symset_radioboxStyle(void) {
  REMEMBER_VAR_STACK();
  wxREGGLOB(radioboxStyle_wxVERTICAL_sym);
  radioboxStyle_wxVERTICAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical"));
  wxREGGLOB(radioboxStyle_wxHORIZONTAL_sym);
  radioboxStyle_wxHORIZONTAL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal"));
  wxREGGLOB(radioboxStyle_wxINVISIBLE_sym);
  radioboxStyle_wxINVISIBLE_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("deleted"));
  wxREGGLOB(radioboxStyle_wxVERTICAL_LABEL_sym);
  radioboxStyle_wxVERTICAL_LABEL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("vertical-label"));
  wxREGGLOB(radioboxStyle_wxHORIZONTAL_LABEL_sym);
  radioboxStyle_wxHORIZONTAL_LABEL_sym = WITH_REMEMBERED_STACK(scheme_intern_symbol("horizontal-label"));
}

static int unbundle_symset_radioboxStyle(Scheme_Object *v, const char *where) {
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, v);
  if (!radioboxStyle_wxHORIZONTAL_LABEL_sym) WITH_VAR_STACK(init_symset_radioboxStyle());
  Scheme_Object *i INIT_NULLED_OUT, *l = v;
  long result = 0;
  while (SCHEME_PAIRP(l)) {
  i = SCHEME_CAR(l);
  if (0) { }
  else if (i == radioboxStyle_wxVERTICAL_sym) { result = result | wxVERTICAL; }
  else if (i == radioboxStyle_wxHORIZONTAL_sym) { result = result | wxHORIZONTAL; }
  else if (i == radioboxStyle_wxINVISIBLE_sym) { result = result | wxINVISIBLE; }
  else if (i == radioboxStyle_wxVERTICAL_LABEL_sym) { result = result | wxVERTICAL_LABEL; }
  else if (i == radioboxStyle_wxHORIZONTAL_LABEL_sym) { result = result | wxHORIZONTAL_LABEL; }
  else { break; } 
  l = SCHEME_CDR(l);
  }
  if (SCHEME_NULLP(l)) { READY_TO_RETURN; return result; }
  if (where) WITH_VAR_STACK(scheme_wrong_type(where, "radioboxStyle symbol list", -1, 0, &v));
  READY_TO_RETURN;
  return 0;
}




#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxRadioBox
#define CB_REALCLASS wxRadioBox
#define CB_UNBUNDLE objscheme_unbundle_wxRadioBox
#define CB_USER METHODNAME("radio-box%", "initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxRadioBoxCallbackToScheme
#define CB_TOC wxRadioBoxCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);

#include "wxs_bmap.h"

#undef l_ADDRESS
#undef l_DEREF
#undef l_TEST
#undef l_POINT
#undef l_TYPE
#undef l_LIST_ITEM_BUNDLE
#undef l_LIST_ITEM_UNBUNDLE
#undef l_MAKE_LIST
#undef l_MAKE_ARRAY
#undef l_EXTRA
#undef l_TERMINATE
#undef l_COPY
#undef l_OKTEST
#undef l_INTTYPE

#define l_ADDRESS 
#define l_DEREF 
#define l_NEWATOMIC (UseGC)
#define l_NULLOK 0
#define l_TEST 
#define l_POINT 
#define l_EXTRA 0
#define l_TERMINATE 
#define l_COPY l_COPYDEST=l_COPYSRC;
#define l_OKTEST 
#define l_INTTYPE int
#define l_DIRECTMALLOC 0

#define l_TYPE string
#define l_LIST_ITEM_BUNDLE objscheme_bundle_string
#define l_LIST_ITEM_UNBUNDLE objscheme_unbundle_string
#define l_MAKE_LIST __MakestringList
#define l_MAKE_ARRAY __MakestringArray





static Scheme_Object *l_MAKE_LIST(l_TYPE l_POINT *f, l_INTTYPE c)
{
  Scheme_Object *cdr = scheme_null, *obj = NULL;
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, cdr);

  while (c--) {
    obj = WITH_VAR_STACK(l_LIST_ITEM_BUNDLE(l_ADDRESS f[c]));
    cdr = WITH_VAR_STACK(scheme_make_pair(obj, cdr));
  }
  
  READY_TO_RETURN;

  return cdr;
}

static l_TYPE l_POINT *l_MAKE_ARRAY(Scheme_Object *l, l_INTTYPE *c, char *who)
{
  Scheme_Object *orig_l = l;
  int i = 0;
  long len;
  l_TYPE l_POINT *f = NULL;

  SETUP_VAR_STACK(3);
  VAR_STACK_PUSH(0, l);
  VAR_STACK_PUSH(1, orig_l);
  VAR_STACK_PUSH(2, f);

  len = WITH_VAR_STACK(scheme_proper_list_length(l));
  if (len < 0) WITH_VAR_STACK(scheme_wrong_type(who, "proper-list", -1, 0, &l));
  if (c) *c = len;

  if (!(len + l_EXTRA)) {
    READY_TO_RETURN;
    return NULL;
  }

#if l_DIRECTMALLOC
  f = (l_TYPE l_POINT *)WITH_VAR_STACK(GC_malloc_atomic(sizeof(l_TYPE l_POINT) * (len + l_EXTRA)));
#else
  f = WITH_VAR_STACK(new l_NEWATOMIC l_TYPE l_POINT[len + l_EXTRA]);
#endif

  while (!SCHEME_NULLP(l)) {
    if (!SCHEME_LISTP(l)) {
      WITH_VAR_STACK(scheme_arg_mismatch(who, "expected a proper list: ", orig_l));
      READY_TO_RETURN;
      return NULL;
    }

#define l_COPYDEST f[i]
#define l_COPYSRC (l_DEREF WITH_VAR_STACK(l_LIST_ITEM_UNBUNDLE(SCHEME_CAR(l), who l_TEST)))

    l_COPY

    l_OKTEST

    i++;

    l = SCHEME_CDR(l);
  }
  l_TERMINATE

  READY_TO_RETURN;

  return f;
}


#define OKTESTWHERE METHODNAME("radio-box%", "initialization") 
#undef l_ADDRESS
#undef l_DEREF
#undef l_TEST
#undef l_POINT
#undef l_TYPE
#undef l_LIST_ITEM_BUNDLE
#undef l_LIST_ITEM_UNBUNDLE
#undef l_MAKE_LIST
#undef l_MAKE_ARRAY
#undef l_EXTRA
#undef l_TERMINATE
#undef l_COPY
#undef l_OKTEST
#undef l_INTTYPE

#define l_ADDRESS 
#define l_DEREF 
#define l_NEWATOMIC (UseGC)
#define l_NULLOK 0
#define l_TEST , l_NULLOK
#define l_POINT *
#define l_EXTRA 0
#define l_TERMINATE 
#define l_COPY l_COPYDEST=l_COPYSRC;
#define l_OKTEST { if (!((l_COPYDEST)->Ok())) WITH_VAR_STACK(scheme_arg_mismatch(OKTESTWHERE, "bad bitmap: ", SCHEME_CAR(l))); if (BM_SELECTED(l_COPYDEST)) WITH_VAR_STACK(scheme_arg_mismatch(OKTESTWHERE, "bitmap is currently installed into a bitmap-dc%: ", SCHEME_CAR(l))); }
#define l_INTTYPE int
#define l_DIRECTMALLOC 0

#define l_TYPE wxBitmap
#define l_LIST_ITEM_BUNDLE objscheme_bundle_wxBitmap
#define l_LIST_ITEM_UNBUNDLE objscheme_unbundle_wxBitmap
#define l_MAKE_LIST __MakewxBitmapList
#define l_MAKE_ARRAY __MakewxBitmapArray





static Scheme_Object *l_MAKE_LIST(l_TYPE l_POINT *f, l_INTTYPE c)
{
  Scheme_Object *cdr = scheme_null, *obj = NULL;
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, cdr);

  while (c--) {
    obj = WITH_VAR_STACK(l_LIST_ITEM_BUNDLE(l_ADDRESS f[c]));
    cdr = WITH_VAR_STACK(scheme_make_pair(obj, cdr));
  }
  
  READY_TO_RETURN;

  return cdr;
}

static l_TYPE l_POINT *l_MAKE_ARRAY(Scheme_Object *l, l_INTTYPE *c, char *who)
{
  Scheme_Object *orig_l = l;
  int i = 0;
  long len;
  l_TYPE l_POINT *f = NULL;

  SETUP_VAR_STACK(3);
  VAR_STACK_PUSH(0, l);
  VAR_STACK_PUSH(1, orig_l);
  VAR_STACK_PUSH(2, f);

  len = WITH_VAR_STACK(scheme_proper_list_length(l));
  if (len < 0) WITH_VAR_STACK(scheme_wrong_type(who, "proper-list", -1, 0, &l));
  if (c) *c = len;

  if (!(len + l_EXTRA)) {
    READY_TO_RETURN;
    return NULL;
  }

#if l_DIRECTMALLOC
  f = (l_TYPE l_POINT *)WITH_VAR_STACK(GC_malloc_atomic(sizeof(l_TYPE l_POINT) * (len + l_EXTRA)));
#else
  f = WITH_VAR_STACK(new l_NEWATOMIC l_TYPE l_POINT[len + l_EXTRA]);
#endif

  while (!SCHEME_NULLP(l)) {
    if (!SCHEME_LISTP(l)) {
      WITH_VAR_STACK(scheme_arg_mismatch(who, "expected a proper list: ", orig_l));
      READY_TO_RETURN;
      return NULL;
    }

#define l_COPYDEST f[i]
#define l_COPYSRC (l_DEREF WITH_VAR_STACK(l_LIST_ITEM_UNBUNDLE(SCHEME_CAR(l), who l_TEST)))

    l_COPY

    l_OKTEST

    i++;

    l = SCHEME_CDR(l);
  }
  l_TERMINATE

  READY_TO_RETURN;

  return f;
}











#define RANGECLASS wxRadioBox

#define THISOBJECT ((RANGECLASS *)((Scheme_Class_Object *)THEOBJ)->primdata)





class os_wxRadioBox : public wxRadioBox {
 public:
  Scheme_Object *callback_closure;

  os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3 = -1, int x4 = -1, int x5 = -1, int x6 = -1, int x7 = 0, string* x8 = NULL, int x9 = 0, int x10 = wxVERTICAL, class wxFont* x11 = NULL, string x12 = "radioBox"));
#ifndef MZ_PRECISE_GC
  os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, wxBitmap** x8, int x9 = 0, int x10 = wxVERTICAL, class wxFont* x11 = NULL, string x12 = "radioBox"));
#endif
  ~os_wxRadioBox();
  void OnDropFile(epathname x0);
  Bool PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1);
  Bool PreOnChar(class wxWindow* x0, class wxKeyEvent* x1);
  void OnSize(int x0, int x1);
  void OnSetFocus();
  void OnKillFocus();
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxRadioBox::gcMark() {
  wxRadioBox::gcMark();
  gcMARK_TYPED(Scheme_Object *, callback_closure);
}
void os_wxRadioBox::gcFixup() {
  wxRadioBox::gcFixup();
  gcFIXUP_TYPED(Scheme_Object *, callback_closure);
}
#endif

static Scheme_Object *os_wxRadioBox_class;

os_wxRadioBox::os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, string* x8, int x9, int x10, class wxFont* x11, string x12))
CONSTRUCTOR_INIT(: wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12))
{
}

#ifndef MZ_PRECISE_GC
os_wxRadioBox::os_wxRadioBox CONSTRUCTOR_ARGS((class wxPanel* x0, wxFunction x1, nstring x2, int x3, int x4, int x5, int x6, int x7, wxBitmap** x8, int x9, int x10, class wxFont* x11, string x12))
CONSTRUCTOR_INIT(: wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12))
{
}
#endif

os_wxRadioBox::~os_wxRadioBox()
{
    objscheme_destroy(this, (Scheme_Object *) __gc_external);
}

static Scheme_Object *os_wxRadioBoxOnDropFile(int n, Scheme_Object *p[]);

void os_wxRadioBox::OnDropFile(epathname x0)
{
  Scheme_Object *p[POFFSET+1] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(6);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+1);
  VAR_STACK_PUSH(5, x0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxRadioBox_class, "on-drop-file", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method, os_wxRadioBoxOnDropFile)) {
    SET_VAR_STACK();
    READY_TO_RETURN; ASSELF wxRadioBox::OnDropFile(x0);
  } else {
  mz_jmp_buf *savebuf, newbuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_pathname((char *)x0));
  savebuf = scheme_current_thread->error_buf; scheme_current_thread->error_buf = &newbuf; if (scheme_setjmp(newbuf)) { scheme_current_thread->error_buf = savebuf; scheme_clear_escape(); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+1, p));
  scheme_current_thread->error_buf = savebuf;
  
     READY_TO_RETURN;
  }
}

static Scheme_Object *os_wxRadioBoxPreOnEvent(int n, Scheme_Object *p[]);

Bool os_wxRadioBox::PreOnEvent(class wxWindow* x0, class wxMouseEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxRadioBox_class, "pre-on-event", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method, os_wxRadioBoxPreOnEvent)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf *savebuf, newbuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxMouseEvent(x1));
  savebuf = scheme_current_thread->error_buf; scheme_current_thread->error_buf = &newbuf; if (scheme_setjmp(newbuf)) { scheme_current_thread->error_buf = savebuf; scheme_clear_escape(); return 1; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  scheme_current_thread->error_buf = savebuf;
  
  {
     Bool resval;
     resval = WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-event in radio-box%"", extracting return value"));
     READY_TO_RETURN;
     return resval;
  }
  }
}

static Scheme_Object *os_wxRadioBoxPreOnChar(int n, Scheme_Object *p[]);

Bool os_wxRadioBox::PreOnChar(class wxWindow* x0, class wxKeyEvent* x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(7);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  VAR_STACK_PUSH(5, x0);
  VAR_STACK_PUSH(6, x1);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxRadioBox_class, "pre-on-char", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method, os_wxRadioBoxPreOnChar)) {
    SET_VAR_STACK();
    return FALSE;
  } else {
  mz_jmp_buf *savebuf, newbuf;
  p[POFFSET+0] = WITH_VAR_STACK(objscheme_bundle_wxWindow(x0));
  p[POFFSET+1] = WITH_VAR_STACK(objscheme_bundle_wxKeyEvent(x1));
  savebuf = scheme_current_thread->error_buf; scheme_current_thread->error_buf = &newbuf; if (scheme_setjmp(newbuf)) { scheme_current_thread->error_buf = savebuf; scheme_clear_escape(); return 1; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  scheme_current_thread->error_buf = savebuf;
  
  {
     Bool resval;
     resval = WITH_VAR_STACK(objscheme_unbundle_bool(v, "pre-on-char in radio-box%"", extracting return value"));
     READY_TO_RETURN;
     return resval;
  }
  }
}

static Scheme_Object *os_wxRadioBoxOnSize(int n, Scheme_Object *p[]);

void os_wxRadioBox::OnSize(int x0, int x1)
{
  Scheme_Object *p[POFFSET+2] INIT_NULLED_ARRAY({ NULLED_OUT INA_comma NULLED_OUT INA_comma NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+2);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxRadioBox_class, "on-size", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method, os_wxRadioBoxOnSize)) {
    SET_VAR_STACK();
    READY_TO_RETURN; ASSELF wxRadioBox::OnSize(x0, x1);
  } else {
  
  p[POFFSET+0] = scheme_make_integer(x0);
  p[POFFSET+1] = scheme_make_integer(x1);
  
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+2, p));
  
  
     READY_TO_RETURN;
  }
}

static Scheme_Object *os_wxRadioBoxOnSetFocus(int n, Scheme_Object *p[]);

void os_wxRadioBox::OnSetFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxRadioBox_class, "on-set-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method, os_wxRadioBoxOnSetFocus)) {
    SET_VAR_STACK();
    READY_TO_RETURN; ASSELF wxRadioBox::OnSetFocus();
  } else {
  mz_jmp_buf *savebuf, newbuf;
  savebuf = scheme_current_thread->error_buf; scheme_current_thread->error_buf = &newbuf; if (scheme_setjmp(newbuf)) { scheme_current_thread->error_buf = savebuf; scheme_clear_escape(); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  scheme_current_thread->error_buf = savebuf;
  
     READY_TO_RETURN;
  }
}

static Scheme_Object *os_wxRadioBoxOnKillFocus(int n, Scheme_Object *p[]);

void os_wxRadioBox::OnKillFocus()
{
  Scheme_Object *p[POFFSET+0] INIT_NULLED_ARRAY({ NULLED_OUT });
  Scheme_Object *v;
  Scheme_Object *method INIT_NULLED_OUT;
#ifdef MZ_PRECISE_GC
  os_wxRadioBox *sElF = this;
#endif
  static void *mcache = 0;

  SETUP_VAR_STACK(5);
  VAR_STACK_PUSH(0, method);
  VAR_STACK_PUSH(1, sElF);
  VAR_STACK_PUSH_ARRAY(2, p, POFFSET+0);
  SET_VAR_STACK();

  method = objscheme_find_method((Scheme_Object *) ASSELF __gc_external, os_wxRadioBox_class, "on-kill-focus", &mcache);
  if (!method || OBJSCHEME_PRIM_METHOD(method, os_wxRadioBoxOnKillFocus)) {
    SET_VAR_STACK();
    READY_TO_RETURN; ASSELF wxRadioBox::OnKillFocus();
  } else {
  mz_jmp_buf *savebuf, newbuf;
  savebuf = scheme_current_thread->error_buf; scheme_current_thread->error_buf = &newbuf; if (scheme_setjmp(newbuf)) { scheme_current_thread->error_buf = savebuf; scheme_clear_escape(); return; }
  p[0] = (Scheme_Object *) ASSELF __gc_external;

  v = WITH_VAR_STACK(scheme_apply(method, POFFSET+0, p));
  scheme_current_thread->error_buf = savebuf;
  
     READY_TO_RETURN;
  }
}

static Scheme_Object *os_wxRadioBoxButtonFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxRadioBox_class, "button-focus in radio-box%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "button-focus in radio-box%"));

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->ButtonFocus(x0));

  
  
  READY_TO_RETURN;
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxEnable(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxRadioBox_class, "enable in radio-box%", n, p);
  if ((n >= (POFFSET+1)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[POFFSET+0], NULL))) {
    int x0;
    Bool x1;

    SETUP_VAR_STACK_PRE_REMEMBERED(1);
    VAR_STACK_PUSH(0, p);

    
    if (n != (POFFSET+2)) 
      WITH_VAR_STACK(scheme_wrong_count_m("enable in radio-box% (single-button case)", POFFSET+2, POFFSET+2, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "enable in radio-box% (single-button case)"));
    x1 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+1], "enable in radio-box% (single-button case)"));

    if ((x0 < 0) || (x0 >= THISOBJECT->Number())) { READY_TO_RETURN; return scheme_void; }
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->Enable(x0, x1));

    
    
    READY_TO_PRE_RETURN;
  } else  {
    Bool x0;

    SETUP_VAR_STACK_PRE_REMEMBERED(1);
    VAR_STACK_PUSH(0, p);

    
    if (n != (POFFSET+1)) 
      WITH_VAR_STACK(scheme_wrong_count_m("enable in radio-box% (all-buttons case)", POFFSET+1, POFFSET+1, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_bool(p[POFFSET+0], "enable in radio-box% (all-buttons case)"));

    
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->Enable(x0));

    
    
    READY_TO_PRE_RETURN;
  }

  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxSetSelection(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxRadioBox_class, "set-selection in radio-box%", n, p);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "set-selection in radio-box%"));

  if ((x0 < 0) || (x0 >= THISOBJECT->Number())) { READY_TO_RETURN; return scheme_void; }
  WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->SetSelection(x0));

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxNumber(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxRadioBox_class, "number in radio-box%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->Number());

  
  
  READY_TO_RETURN;
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxGetSelection(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(os_wxRadioBox_class, "get-selection in radio-box%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->GetSelection());

  
  
  READY_TO_RETURN;
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxRadioBoxOnDropFile(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxRadioBox_class, "on-drop-file in radio-box%", n, p);
  epathname x0 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);

  
  x0 = (epathname)WITH_VAR_STACK(objscheme_unbundle_epathname(p[POFFSET+0], "on-drop-file in radio-box%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->wxRadioBox::OnDropFile(x0));
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->OnDropFile(x0));

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxPreOnEvent(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxRadioBox_class, "pre-on-event in radio-box%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxMouseEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-event in radio-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxMouseEvent(p[POFFSET+1], "pre-on-event in radio-box%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnEvent(x0, x1));
  else
    r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->PreOnEvent(x0, x1));

  
  
  READY_TO_RETURN;
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxRadioBoxPreOnChar(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(os_wxRadioBox_class, "pre-on-char in radio-box%", n, p);
  class wxWindow* x0 INIT_NULLED_OUT;
  class wxKeyEvent* x1 INIT_NULLED_OUT;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, x0);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxWindow(p[POFFSET+0], "pre-on-char in radio-box%", 0));
  x1 = WITH_VAR_STACK(objscheme_unbundle_wxKeyEvent(p[POFFSET+1], "pre-on-char in radio-box%", 0));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    r = WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)-> wxWindow::PreOnChar(x0, x1));
  else
    r = WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->PreOnChar(x0, x1));

  
  
  READY_TO_RETURN;
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxRadioBoxOnSize(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxRadioBox_class, "on-size in radio-box%", n, p);
  int x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+0], "on-size in radio-box%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+1], "on-size in radio-box%"));

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->wxRadioBox::OnSize(x0, x1));
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->OnSize(x0, x1));

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxOnSetFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxRadioBox_class, "on-set-focus in radio-box%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->wxRadioBox::OnSetFocus());
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->OnSetFocus());

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxRadioBoxOnKillFocus(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(os_wxRadioBox_class, "on-kill-focus in radio-box%", n, p);

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  

  
  if (((Scheme_Class_Object *)p[0])->primflag)
    WITH_VAR_STACK(((os_wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->wxRadioBox::OnKillFocus());
  else
    WITH_VAR_STACK(((wxRadioBox *)((Scheme_Class_Object *)p[0])->primdata)->OnKillFocus());

  
  
  READY_TO_RETURN;
  return scheme_void;
}

static Scheme_Object *os_wxRadioBox_ConstructScheme(int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, p);
  os_wxRadioBox *realobj INIT_NULLED_OUT;
  REMEMBER_VAR_STACK();
  if ((n >= (POFFSET+8)) && WITH_REMEMBERED_STACK(objscheme_istype_wxPanel(p[POFFSET+0], NULL, 0)) && (SCHEME_NULLP(p[POFFSET+1]) || WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], NULL))) && (XC_SCHEME_NULLP(p[POFFSET+2]) || WITH_REMEMBERED_STACK(objscheme_istype_string(p[POFFSET+2], NULL))) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[POFFSET+3], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[POFFSET+4], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[POFFSET+5], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_number(p[POFFSET+6], NULL)) && (SCHEME_LISTP(p[POFFSET+7]) && (XC_SCHEME_NULLP(p[POFFSET+7]) || WITH_REMEMBERED_STACK(objscheme_istype_wxBitmap((SCHEME_CAR(p[POFFSET+7])), NULL, 0))))) {
    class wxPanel* x0 INIT_NULLED_OUT;
    wxFunction x1;
    nstring x2 INIT_NULLED_OUT;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    wxBitmap** x8 INIT_NULLED_OUT;
    int x9;
    int x10;
    class wxFont* x11 INIT_NULLED_OUT;
    string x12 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(7);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x2);
    VAR_STACK_PUSH(4, x8);
    VAR_STACK_PUSH(5, x11);
    VAR_STACK_PUSH(6, x12);

    int cb_pos = 0;
    if ((n < (POFFSET+8)) || (n > (POFFSET+12))) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in radio-box% (bitmap list case)", POFFSET+8, POFFSET+12, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in radio-box% (bitmap list case)", 0));
    x1 = (SCHEME_NULLP(p[POFFSET+1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[POFFSET+2], "initialization in radio-box% (bitmap list case)"));
    x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in radio-box% (bitmap list case)"));
    x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+4], "initialization in radio-box% (bitmap list case)"));
    x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+5], "initialization in radio-box% (bitmap list case)"));
    x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+6], "initialization in radio-box% (bitmap list case)"));
    x8 = NULL;
    if (n > (POFFSET+8)) {
      x9 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+8], "initialization in radio-box% (bitmap list case)"));
    } else
      x9 = 0;
    if (n > (POFFSET+9)) {
      x10 = WITH_VAR_STACK(unbundle_symset_radioboxStyle(p[POFFSET+9], "initialization in radio-box% (bitmap list case)"));
    } else
      x10 = wxVERTICAL;
    if (n > (POFFSET+10)) {
      x11 = WITH_VAR_STACK(objscheme_unbundle_wxFont(p[POFFSET+10], "initialization in radio-box% (bitmap list case)", 1));
    } else
      x11 = NULL;
    if (n > (POFFSET+11)) {
      x12 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+11], "initialization in radio-box% (bitmap list case)"));
    } else
      x12 = "radioBox";

    if (!x5) x5 = -1;if (!x6) x6 = -1;x8 = WITH_VAR_STACK(__MakewxBitmapArray((7+POFFSET < n) ? p[POFFSET+7] : scheme_null, &x7, METHODNAME("radio-box%","initialization")));
    realobj = WITH_VAR_STACK(new os_wxRadioBox CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12));
#endif
    realobj->__gc_external = (void *)p[0];
    /* delete[] x8; */
    realobj->callback_closure = p[POFFSET+cb_pos];
    READY_TO_PRE_RETURN;
  } else  {
    class wxPanel* x0 INIT_NULLED_OUT;
    wxFunction x1;
    nstring x2 INIT_NULLED_OUT;
    int x3;
    int x4;
    int x5;
    int x6;
    int x7;
    string* x8 INIT_NULLED_OUT;
    int x9;
    int x10;
    class wxFont* x11 INIT_NULLED_OUT;
    string x12 INIT_NULLED_OUT;

    SETUP_VAR_STACK_PRE_REMEMBERED(7);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, realobj);
    VAR_STACK_PUSH(2, x0);
    VAR_STACK_PUSH(3, x2);
    VAR_STACK_PUSH(4, x8);
    VAR_STACK_PUSH(5, x11);
    VAR_STACK_PUSH(6, x12);

    int cb_pos = 0;
    if ((n < (POFFSET+3)) || (n > (POFFSET+12))) 
      WITH_VAR_STACK(scheme_wrong_count_m("initialization in radio-box% (string list case)", POFFSET+3, POFFSET+12, n, p, 1));
    x0 = WITH_VAR_STACK(objscheme_unbundle_wxPanel(p[POFFSET+0], "initialization in radio-box% (string list case)", 0));
    x1 = (SCHEME_NULLP(p[POFFSET+1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[POFFSET+1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
    x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[POFFSET+2], "initialization in radio-box% (string list case)"));
    if (n > (POFFSET+3)) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+3], "initialization in radio-box% (string list case)"));
    } else
      x3 = -1;
    if (n > (POFFSET+4)) {
      x4 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+4], "initialization in radio-box% (string list case)"));
    } else
      x4 = -1;
    if (n > (POFFSET+5)) {
      x5 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+5], "initialization in radio-box% (string list case)"));
    } else
      x5 = -1;
    if (n > (POFFSET+6)) {
      x6 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+6], "initialization in radio-box% (string list case)"));
    } else
      x6 = -1;
    if (n > (POFFSET+7)) {
      x8 = NULL;
    } else
      x8 = NULL;
    if (n > (POFFSET+8)) {
      x9 = WITH_VAR_STACK(objscheme_unbundle_integer(p[POFFSET+8], "initialization in radio-box% (string list case)"));
    } else
      x9 = 0;
    if (n > (POFFSET+9)) {
      x10 = WITH_VAR_STACK(unbundle_symset_radioboxStyle(p[POFFSET+9], "initialization in radio-box% (string list case)"));
    } else
      x10 = wxVERTICAL;
    if (n > (POFFSET+10)) {
      x11 = WITH_VAR_STACK(objscheme_unbundle_wxFont(p[POFFSET+10], "initialization in radio-box% (string list case)", 1));
    } else
      x11 = NULL;
    if (n > (POFFSET+11)) {
      x12 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[POFFSET+11], "initialization in radio-box% (string list case)"));
    } else
      x12 = "radioBox";

    if (!x5) x5 = -1;if (!x6) x6 = -1;x8 = WITH_VAR_STACK(__MakestringArray((7+POFFSET < n) ? p[POFFSET+7] : scheme_null, &x7, METHODNAME("radio-box%","initialization")));
    realobj = WITH_VAR_STACK(new os_wxRadioBox CONSTRUCTOR_ARGS((x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12)));
#ifdef MZ_PRECISE_GC
    WITH_VAR_STACK(realobj->gcInit_wxRadioBox(x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12));
#endif
    realobj->__gc_external = (void *)p[0];
    /* delete[] x8; */
    realobj->callback_closure = p[POFFSET+cb_pos];
    READY_TO_PRE_RETURN;
  }

  ((Scheme_Class_Object *)p[0])->primdata = realobj;
  ((Scheme_Class_Object *)p[0])->primflag = 1;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(p[0], &((Scheme_Class_Object *)p[0])->primdata));
  return scheme_void;
}

void objscheme_setup_wxRadioBox(Scheme_Env *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxRadioBox_class);

  os_wxRadioBox_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "radio-box%", "item%", (Scheme_Method_Prim *)os_wxRadioBox_ConstructScheme, 11));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "button-focus" " method", (Scheme_Method_Prim *)os_wxRadioBoxButtonFocus, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "enable" " method", (Scheme_Method_Prim *)os_wxRadioBoxEnable, 1, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "set-selection" " method", (Scheme_Method_Prim *)os_wxRadioBoxSetSelection, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "number" " method", (Scheme_Method_Prim *)os_wxRadioBoxNumber, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "get-selection" " method", (Scheme_Method_Prim *)os_wxRadioBoxGetSelection, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-drop-file" " method", (Scheme_Method_Prim *)os_wxRadioBoxOnDropFile, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "pre-on-event" " method", (Scheme_Method_Prim *)os_wxRadioBoxPreOnEvent, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "pre-on-char" " method", (Scheme_Method_Prim *)os_wxRadioBoxPreOnChar, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-size" " method", (Scheme_Method_Prim *)os_wxRadioBoxOnSize, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-set-focus" " method", (Scheme_Method_Prim *)os_wxRadioBoxOnSetFocus, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxRadioBox_class, "on-kill-focus" " method", (Scheme_Method_Prim *)os_wxRadioBoxOnKillFocus, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxRadioBox_class));


  READY_TO_RETURN;
}

int objscheme_istype_wxRadioBox(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (objscheme_is_a(obj,  os_wxRadioBox_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "radio-box% object or " XC_NULL_STR: "radio-box% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxRadioBox(class wxRadioBox *realobj)
{
  Scheme_Class_Object *obj INIT_NULLED_OUT;
  Scheme_Object *sobj INIT_NULLED_OUT;

  if (!realobj) return XC_SCHEME_NULL;

  if (realobj->__gc_external)
    return (Scheme_Object *)realobj->__gc_external;

  SETUP_VAR_STACK(2);
  VAR_STACK_PUSH(0, obj);
  VAR_STACK_PUSH(1, realobj);

  if ((sobj = WITH_VAR_STACK(objscheme_bundle_by_type(realobj, realobj->__type))))
    { READY_TO_RETURN; return sobj; }
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxRadioBox_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(obj, &obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  READY_TO_RETURN;
  return (Scheme_Object *)obj;
}

class wxRadioBox *objscheme_unbundle_wxRadioBox(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxRadioBox(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(NULL, NULL, 0, &obj));
  if (o->primflag)
    return (os_wxRadioBox *)o->primdata;
  else
    return (wxRadioBox *)o->primdata;
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

  p[0] = NULL;
  p[1] = NULL;

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

  READY_TO_RETURN;
}
