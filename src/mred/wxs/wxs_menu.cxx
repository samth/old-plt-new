/* DO NOT EDIT THIS FILE. */
/* This file was generated by xctocc from "wxs_menu.xc". */


#if defined(_MSC_VER)
# include "wx.h"
#endif

#include "wx_menu.h"

#include "wxscheme.h"
#include "wxs_menu.h"

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif




#ifdef wx_x
# define BM_SELECTED(map) ((map)->selectedTo)
#endif
#if defined(wx_mac) || defined(wx_msw)
# define BM_SELECTED(map) ((map)->selectedInto)
#endif
# define BM_IN_USE(map) ((map)->selectedIntoDC)





#ifdef wx_msw
# define XTMAC_UNUSED(x) x
#else
# define XTMAC_UNUSED(x) /**/
#endif

static void menuSelect(wxMenu *XTMAC_UNUSED(m))
{
#ifdef wx_msw
  m->SelectMenu();
#endif
}

// @CLASSBASE wxMenuItem "menu-item" : "object"
// @END

// wxMenu is really derived from wxItem
//  but that makes no sense. Enforce different hierarchy here


#define CB_FUNCTYPE wxFunction 


#undef CALLBACKCLASS
#undef CB_REALCLASS
#undef CB_UNBUNDLE
#undef CB_USER

#define CALLBACKCLASS os_wxMenu
#define CB_REALCLASS wxMenu
#define CB_UNBUNDLE objscheme_unbundle_wxMenu
#define CB_USER METHODNAME("menu%","initialization")

#undef CB_TOSCHEME
#undef CB_TOC
#define CB_TOSCHEME wxMenuCallbackToScheme
#define CB_TOC wxMenuCallbackToC


class CALLBACKCLASS;





extern wxCommandEvent *objscheme_unbundle_wxCommandEvent(Scheme_Object *,const char *,int);
extern Scheme_Object *objscheme_bundle_wxCommandEvent(wxCommandEvent *);

static void CB_TOSCHEME(CB_REALCLASS *obj, wxCommandEvent *event);






class os_wxMenu : public wxMenu {
 public:
  Scheme_Object *callback_closure;

  os_wxMenu CONSTRUCTOR_ARGS((nstring x0 = NULL, wxFunction x1 = NULL));
  ~os_wxMenu();
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxMenu::gcMark() {
  wxMenu::gcMark();
  gcMARK_TYPED(Scheme_Object *, callback_closure);
}
void os_wxMenu::gcFixup() {
  wxMenu::gcFixup();
  gcFIXUP_TYPED(Scheme_Object *, callback_closure);
}
#endif

static Scheme_Object *os_wxMenu_class;

os_wxMenu::os_wxMenu CONSTRUCTOR_ARGS((nstring x0, wxFunction x1))
CONSTRUCTOR_INIT(: wxMenu(x0, x1))
{
}

os_wxMenu::~os_wxMenu()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

static Scheme_Object *os_wxMenumenuSelect(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  WITH_VAR_STACK(menuSelect(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuSetTitle(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  string x0;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[0], "set-title in menu%"));

  
  WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->SetTitle(x0));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuSetLabel(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  ExactLong x0;
  string x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "set-label in menu%"));
  x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "set-label in menu%"));

  
  WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->SetLabel(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuSetHelpString(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  ExactLong x0;
  nstring x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "set-help-string in menu%"));
  x1 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[1], "set-help-string in menu%"));

  
  WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->SetHelpString(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuNumber(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Number());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxMenuEnable(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  ExactLong x0;
  Bool x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "enable in menu%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_bool(p[1], "enable in menu%"));

  
  WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Enable(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuCheck(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  ExactLong x0;
  Bool x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "check in menu%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_bool(p[1], "check in menu%"));

  
  WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Check(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuChecked(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  ExactLong x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "checked? in menu%"));

  
  r = WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Checked(x0));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxMenuAppendSeparator(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->AppendSeparator());

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuDeleteByPosition(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  int x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "delete-by-position in menu%"));

  
  r = WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->DeleteByPosition(x0));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxMenuDelete(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  ExactLong x0;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "delete in menu%"));

  
  r = WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Delete(x0));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxMenuAppend(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  SETUP_PRE_VAR_STACK(2);
  PRE_VAR_STACK_PUSH(0, p);
  PRE_VAR_STACK_PUSH(1, obj);
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  if ((n >= 3) && WITH_REMEMBERED_STACK(objscheme_istype_ExactLong(p[0], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_string(p[1], NULL)) && WITH_REMEMBERED_STACK(objscheme_istype_wxMenu(p[2], NULL, 0))) {
    ExactLong x0;
    string x1;
    class wxMenu* x2;
    nstring x3;

    SETUP_VAR_STACK_PRE_REMEMBERED(5);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x1);
    VAR_STACK_PUSH(3, x2);
    VAR_STACK_PUSH(4, x3);

    
    if ((n < 3) ||(n > 4)) 
      WITH_VAR_STACK(scheme_wrong_count("append in menu% (submenu case)", 3, 4, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "append in menu% (submenu case)"));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "append in menu% (submenu case)"));
    x2 = WITH_VAR_STACK(objscheme_unbundle_wxMenu(p[2], "append in menu% (submenu case)", 0));
    if (n > 3) {
      x3 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[3], "append in menu% (submenu case)"));
    } else
      x3 = NULL;

    
    WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Append(x0, x1, x2, x3));

    
    
  } else  {
    ExactLong x0;
    string x1;
    nstring x2;
    Bool x3;

    SETUP_VAR_STACK_PRE_REMEMBERED(4);
    VAR_STACK_PUSH(0, p);
    VAR_STACK_PUSH(1, obj);
    VAR_STACK_PUSH(2, x1);
    VAR_STACK_PUSH(3, x2);

    
    if ((n < 2) ||(n > 4)) 
      WITH_VAR_STACK(scheme_wrong_count("append in menu% (string item case)", 2, 4, n, p));
    x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "append in menu% (string item case)"));
    x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "append in menu% (string item case)"));
    if (n > 2) {
      x2 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[2], "append in menu% (string item case)"));
    } else
      x2 = NULL;
    if (n > 3) {
      x3 = WITH_VAR_STACK(objscheme_unbundle_bool(p[3], "append in menu% (string item case)"));
    } else
      x3 = FALSE;

    
    WITH_VAR_STACK(((wxMenu *)((Scheme_Class_Object *)obj)->primdata)->Append(x0, x1, x2, x3));

    
    
  }

  return scheme_void;
}

static Scheme_Object *os_wxMenu_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, obj);
  os_wxMenu *realobj;
  REMEMBER_VAR_STACK();
  nstring x0;
  wxFunction x1;

  SETUP_VAR_STACK_PRE_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, realobj);
  VAR_STACK_PUSH(3, x0);

  int cb_pos = 0;
  if ((n > 2)) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in menu%", 0, 2, n, p));
  if (n > 0) {
    x0 = (nstring)WITH_VAR_STACK(objscheme_unbundle_nullable_string(p[0], "initialization in menu%"));
  } else
    x0 = NULL;
  if (n > 1) {
    x1 = (SCHEME_NULLP(p[1]) ? NULL : (WITH_REMEMBERED_STACK(objscheme_istype_proc2(p[1], CB_USER)), cb_pos = 1, (CB_FUNCTYPE)CB_TOSCHEME));
  } else
    x1 = NULL;

  
  realobj = WITH_VAR_STACK(new os_wxMenu CONSTRUCTOR_ARGS((x0, x1)));
#ifdef MZ_PRECISE_GC
  WITH_VAR_STACK(realobj->gcInit_wxMenu(x0, x1));
#endif
  realobj->__gc_external = (void *)obj;
  objscheme_note_creation(obj);
  
  realobj->callback_closure = p[cb_pos];
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxMenu(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxMenu_class);

  os_wxMenu_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "menu%", "object%", os_wxMenu_ConstructScheme, 12));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "select", os_wxMenumenuSelect, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "set-title", os_wxMenuSetTitle, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "set-label", os_wxMenuSetLabel, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "set-help-string", os_wxMenuSetHelpString, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "number", os_wxMenuNumber, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "enable", os_wxMenuEnable, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "check", os_wxMenuCheck, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "checked?", os_wxMenuChecked, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "append-separator", os_wxMenuAppendSeparator, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "delete-by-position", os_wxMenuDeleteByPosition, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "delete", os_wxMenuDelete, 1, 1));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenu_class, "append", os_wxMenuAppend, 2, 4));


  WITH_VAR_STACK(scheme_made_class(os_wxMenu_class));


}

int objscheme_istype_wxMenu(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxMenu_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "menu% object or " XC_NULL_STR: "menu% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxMenu(class wxMenu *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxMenu_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxMenu *objscheme_unbundle_wxMenu(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxMenu(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxMenu *)o->primdata;
  else
    return (wxMenu *)o->primdata;
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

// wxMenuBar is really derived from wxItem
//  but that makes no sense. Enforce different hierarchy here





class os_wxMenuBar : public wxMenuBar {
 public:

  os_wxMenuBar CONSTRUCTOR_ARGS(());
  ~os_wxMenuBar();
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxMenuBar::gcMark() {
  wxMenuBar::gcMark();
}
void os_wxMenuBar::gcFixup() {
  wxMenuBar::gcFixup();
}
#endif

static Scheme_Object *os_wxMenuBar_class;

os_wxMenuBar::os_wxMenuBar CONSTRUCTOR_ARGS(())
CONSTRUCTOR_INIT(: wxMenuBar())
{
}

os_wxMenuBar::~os_wxMenuBar()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

static Scheme_Object *os_wxMenuBarSetLabelTop(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  string x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "set-label-top in menu-bar%"));
  x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "set-label-top in menu-bar%"));

  if (x0 < 0) return scheme_void;
  WITH_VAR_STACK(((wxMenuBar *)((Scheme_Class_Object *)obj)->primdata)->SetLabelTop(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuBarNumber(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  int r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxMenuBar *)((Scheme_Class_Object *)obj)->primdata)->Number());

  
  
  return scheme_make_integer(r);
}

static Scheme_Object *os_wxMenuBarEnableTop(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  int x0;
  Bool x1;

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_integer(p[0], "enable-top in menu-bar%"));
  x1 = WITH_VAR_STACK(objscheme_unbundle_bool(p[1], "enable-top in menu-bar%"));

  if (x0 < 0) return scheme_void;
  WITH_VAR_STACK(((wxMenuBar *)((Scheme_Class_Object *)obj)->primdata)->EnableTop(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuBarDelete(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  Bool r;
  objscheme_check_valid(obj);
  class wxMenu* x0;
  int x1;

  SETUP_VAR_STACK_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxMenu(p[0], "delete in menu-bar%", 1));
  if (n > 1) {
    x1 = WITH_VAR_STACK(objscheme_unbundle_integer(p[1], "delete in menu-bar%"));
  } else
    x1 = 0;

  
  r = WITH_VAR_STACK(((wxMenuBar *)((Scheme_Class_Object *)obj)->primdata)->Delete(x0, x1));

  
  
  return (r ? scheme_true : scheme_false);
}

static Scheme_Object *os_wxMenuBarAppend(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  objscheme_check_valid(obj);
  class wxMenu* x0;
  string x1;

  SETUP_VAR_STACK_REMEMBERED(4);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, x0);
  VAR_STACK_PUSH(3, x1);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_wxMenu(p[0], "append in menu-bar%", 0));
  x1 = (string)WITH_VAR_STACK(objscheme_unbundle_string(p[1], "append in menu-bar%"));

  
  WITH_VAR_STACK(((wxMenuBar *)((Scheme_Class_Object *)obj)->primdata)->Append(x0, x1));

  
  
  return scheme_void;
}

static Scheme_Object *os_wxMenuBar_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, obj);
  os_wxMenuBar *realobj;
  REMEMBER_VAR_STACK();

  SETUP_VAR_STACK_PRE_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, realobj);

  
  if (n != 0) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in menu-bar%", 0, 0, n, p));

  
  realobj = WITH_VAR_STACK(new os_wxMenuBar CONSTRUCTOR_ARGS(()));
#ifdef MZ_PRECISE_GC
  WITH_VAR_STACK(realobj->gcInit_wxMenuBar());
#endif
  realobj->__gc_external = (void *)obj;
  objscheme_note_creation(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxMenuBar(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxMenuBar_class);

  os_wxMenuBar_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "menu-bar%", "object%", os_wxMenuBar_ConstructScheme, 5));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenuBar_class, "set-label-top", os_wxMenuBarSetLabelTop, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenuBar_class, "number", os_wxMenuBarNumber, 0, 0));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenuBar_class, "enable-top", os_wxMenuBarEnableTop, 2, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenuBar_class, "delete", os_wxMenuBarDelete, 1, 2));
  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxMenuBar_class, "append", os_wxMenuBarAppend, 2, 2));


  WITH_VAR_STACK(scheme_made_class(os_wxMenuBar_class));


}

int objscheme_istype_wxMenuBar(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxMenuBar_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "menu-bar% object or " XC_NULL_STR: "menu-bar% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxMenuBar(class wxMenuBar *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxMenuBar_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxMenuBar *objscheme_unbundle_wxMenuBar(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxMenuBar(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxMenuBar *)o->primdata;
  else
    return (wxMenuBar *)o->primdata;
}


#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif

class wxsMenuItem : public wxObject
{
#ifdef MZ_PRECISE_GC
  void *my_id;
#endif
public:
  wxsMenuItem(void);
  ~wxsMenuItem();

  ExactLong Id(void) {
#ifdef MZ_PRECISE_GC
    return (ExactLong)my_id;
#else
    return (ExactLong)this;
#endif
  }
};

wxsMenuItem::wxsMenuItem(void)
{
#ifdef MZ_PRECISE_GC
  void *mid;
  mid = GC_malloc_immobile_box(GC_malloc_weak_box(gcOBJ_TO_PTR(this), NULL, 0));
  my_id = mid;
#endif
}

wxsMenuItem::~wxsMenuItem()
{
#ifdef MZ_PRECISE_GC
  GC_free_immobile_box(my_id);
#endif
}

#ifdef MZ_PRECISE_GC
START_XFORM_SKIP;
#endif

wxsMenuItem* wxsIdToMenuItem(ExactLong id)
{
#ifdef MZ_PRECISE_GC
  return (wxsMenuItem *)gcPTR_TO_OBJ(GC_weak_box_val(*(void **)id));
#else
  return (wxsMenuItem *)id;
#endif
}




class os_wxsMenuItem : public wxsMenuItem {
 public:

  os_wxsMenuItem CONSTRUCTOR_ARGS(());
  ~os_wxsMenuItem();
#ifdef MZ_PRECISE_GC
  void gcMark();
  void gcFixup();
#endif
};

#ifdef MZ_PRECISE_GC
void os_wxsMenuItem::gcMark() {
  wxsMenuItem::gcMark();
}
void os_wxsMenuItem::gcFixup() {
  wxsMenuItem::gcFixup();
}
#endif

static Scheme_Object *os_wxsMenuItem_class;

os_wxsMenuItem::os_wxsMenuItem CONSTRUCTOR_ARGS(())
CONSTRUCTOR_INIT(: wxsMenuItem())
{
}

os_wxsMenuItem::~os_wxsMenuItem()
{
    objscheme_destroy(this, (Scheme_Object *)__gc_external);
}

static Scheme_Object *os_wxsMenuItemId(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  ExactLong r;
  objscheme_check_valid(obj);

  SETUP_VAR_STACK_REMEMBERED(2);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);

  

  
  r = WITH_VAR_STACK(((wxsMenuItem *)((Scheme_Class_Object *)obj)->primdata)->Id());

  
  
  return WITH_REMEMBERED_STACK(scheme_make_integer_value(r));
}

static Scheme_Object *os_wxsMenuItem_ConstructScheme(Scheme_Object *obj, int n,  Scheme_Object *p[])
{
  SETUP_PRE_VAR_STACK(1);
  PRE_VAR_STACK_PUSH(0, obj);
  os_wxsMenuItem *realobj;
  REMEMBER_VAR_STACK();

  SETUP_VAR_STACK_PRE_REMEMBERED(3);
  VAR_STACK_PUSH(0, p);
  VAR_STACK_PUSH(1, obj);
  VAR_STACK_PUSH(2, realobj);

  
  if (n != 0) 
    WITH_VAR_STACK(scheme_wrong_count("initialization in menu-item%", 0, 0, n, p));

  
  realobj = WITH_VAR_STACK(new os_wxsMenuItem CONSTRUCTOR_ARGS(()));
#ifdef MZ_PRECISE_GC
  WITH_VAR_STACK(realobj->gcInit_wxsMenuItem());
#endif
  realobj->__gc_external = (void *)obj;
  objscheme_note_creation(obj);
  
  
  ((Scheme_Class_Object *)obj)->primdata = realobj;
  WITH_REMEMBERED_STACK(objscheme_register_primpointer(&((Scheme_Class_Object *)obj)->primdata));
  ((Scheme_Class_Object *)obj)->primflag = 1;
  return obj;
}

void objscheme_setup_wxsMenuItem(void *env)
{
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);

  wxREGGLOB(os_wxsMenuItem_class);

  os_wxsMenuItem_class = WITH_VAR_STACK(objscheme_def_prim_class(env, "menu-item%", "object%", os_wxsMenuItem_ConstructScheme, 1));

  WITH_VAR_STACK(scheme_add_method_w_arity(os_wxsMenuItem_class, "id", os_wxsMenuItemId, 0, 0));


  WITH_VAR_STACK(scheme_made_class(os_wxsMenuItem_class));


}

int objscheme_istype_wxsMenuItem(Scheme_Object *obj, const char *stop, int nullOK)
{
  REMEMBER_VAR_STACK();
  if (nullOK && XC_SCHEME_NULLP(obj)) return 1;
  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_object_type)
      && scheme_is_subclass(((Scheme_Class_Object *)obj)->sclass,          os_wxsMenuItem_class))
    return 1;
  else {
    if (!stop)
       return 0;
    WITH_REMEMBERED_STACK(scheme_wrong_type(stop, nullOK ? "menu-item% object or " XC_NULL_STR: "menu-item% object", -1, 0, &obj));
    return 0;
  }
}

Scheme_Object *objscheme_bundle_wxsMenuItem(class wxsMenuItem *realobj)
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
  obj = (Scheme_Class_Object *)WITH_VAR_STACK(scheme_make_uninited_object(os_wxsMenuItem_class));

  obj->primdata = realobj;
  WITH_VAR_STACK(objscheme_register_primpointer(&obj->primdata));
  obj->primflag = 0;

  realobj->__gc_external = (void *)obj;
  return (Scheme_Object *)obj;
}

class wxsMenuItem *objscheme_unbundle_wxsMenuItem(Scheme_Object *obj, const char *where, int nullOK)
{
  if (nullOK && XC_SCHEME_NULLP(obj)) return NULL;

  REMEMBER_VAR_STACK();

  (void)objscheme_istype_wxsMenuItem(obj, where, nullOK);
  Scheme_Class_Object *o = (Scheme_Class_Object *)obj;
  WITH_REMEMBERED_STACK(objscheme_check_valid(obj));
  if (o->primflag)
    return (os_wxsMenuItem *)o->primdata;
  else
    return (wxsMenuItem *)o->primdata;
}





static Scheme_Object *wxsMenuItemGlobalwxsIdToMenuItem(int n,  Scheme_Object *p[])
{
  WXS_USE_ARGUMENT(n) WXS_USE_ARGUMENT(p)
  REMEMBER_VAR_STACK();
  class wxsMenuItem* r;
  ExactLong x0;

  SETUP_VAR_STACK_REMEMBERED(1);
  VAR_STACK_PUSH(0, p);

  
  x0 = WITH_VAR_STACK(objscheme_unbundle_ExactLong(p[0], "id-to-menu-item in menu-item%"));

  
  r = WITH_VAR_STACK(wxsIdToMenuItem(x0));

  
  
  return WITH_REMEMBERED_STACK(objscheme_bundle_wxsMenuItem(r));
}

void objscheme_setup_wxsMenuItemGlobal(void *env)
{
  Scheme_Object *functmp;
  SETUP_VAR_STACK(1);
  VAR_STACK_PUSH(0, env);
  functmp = WITH_VAR_STACK(scheme_make_prim_w_arity(wxsMenuItemGlobalwxsIdToMenuItem, "id-to-menu-item", 1, 1));
  WITH_VAR_STACK(scheme_install_xc_global("id-to-menu-item", functmp, env));
}

#ifdef MZ_PRECISE_GC
END_XFORM_SKIP;
#endif
