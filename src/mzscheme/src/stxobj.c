/*
  MzScheme
  Copyright (c) 2000 Matthew Flatt
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "schpriv.h"

/* Dummy pointers: */
static int graph[1], graphref[1];

static Scheme_Object *syntax_p(int argc, Scheme_Object **argv);
static Scheme_Object *syntax_e(int argc, Scheme_Object **argv);
static Scheme_Object *syntax_line(int argc, Scheme_Object **argv);
static Scheme_Object *syntax_col(int argc, Scheme_Object **argv);
static Scheme_Object *syntax_src(int argc, Scheme_Object **argv);

void scheme_init_stx(Scheme_Env *env)
{
  scheme_add_global_constant("syntax?", 
			     scheme_make_folding_prim(syntax_p,
						      "syntax?",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("syntax-e", 
			     scheme_make_folding_prim(syntax_e,
						      "syntax-e",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("syntax-line", 
			     scheme_make_folding_prim(syntax_line,
						      "syntax-line",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("syntax-column", 
			     scheme_make_folding_prim(syntax_col,
						      "syntax-column",
						      1, 1, 1),
			     env);
  scheme_add_global_constant("syntax-source", 
			     scheme_make_folding_prim(syntax_src,
						      "syntax-source",
						      1, 1, 1),
			     env);  
}

Scheme_Object *scheme_make_stx(Scheme_Object *val, 
			       long line, long col, 
			       Scheme_Object *src)
{
  Scheme_Stx *stx;

  stx = MALLOC_ONE_TAGGED(Scheme_Stx);
  stx->type = scheme_stx_type;
  stx->val = val;
  stx->line = line;
  stx->col = col;
  stx->src = src;
  stx->marks = scheme_null;

  return (Scheme_Object *)stx;
}

Scheme_Object *scheme_make_graph_stx(Scheme_Object *stx, long line, long col)
{
  return scheme_make_stx(stx, line, col, (Scheme_Object *)graph);
}

Scheme_Object *scheme_make_graphref_stx(Scheme_Object *stx, long line, long col)
{
  return scheme_make_stx(stx, line, col, (Scheme_Object *)graphref);
}

static unsigned short mark_id;

Scheme_Object *scheme_new_mark()
{
  mark_id = mark_id + 1;
  return scheme_make_pair(scheme_make_integer(mark_id), scheme_null);
}

Scheme_Object *scheme_add_mark(Scheme_Object *o, Scheme_Object *m)
{
  Scheme_Stx *stx = (Scheme_Stx *)o;
  Scheme_Object *marks, *prev;

  prev = NULL;
  for (marks = stx->marks; !SCHEME_NULLP(marks); marks = SCHEME_CDR(marks)) {
    if (SAME_OBJ(SCHEME_CAR(marks), m)) {
      if (prev) {
	SCHEME_CDR(prev) = SCHEME_CDR(marks);
      } else {
	stx->marks = SCHEME_CDR(marks);
	return;
      }
    }
  }

  stx->marks = scheme_make_pair(m, stx->marks);
}

static Scheme_Object *add_marks(Scheme_Object *o, Scheme_Object *ml)
{
  while (!SCHEME_NULLP(ml)) {
    scheme_add_mark(o, SCHEME_CAR(ml));
    ml = SCHEME_CDR(ml);
  }
}

Scheme_Object *scheme_stx_content(Scheme_Object *o)
{
  Scheme_Stx *stx = (Scheme_Stx *)o;
  Scheme_Object *marks = stx->marks;

  if (SCHEME_NULLP(marks)) {
    Scheme_Object *v = stx->val;
    Scheme_Object *ml = scheme_null;

    /* Reverse the list of marks, to preserve order: */
    while (!SCHEME_NULLP(marks)) {
      Scheme_Object *p = marks;
      marks = SCHEME_CDR(marks);
      SCHEME_CDR(p) = ml;
      ml = p;
    }

    if (SCHEME_PAIRP(v)) {
      while (SCHEME_PAIRP(v)) {
	add_marks(SCHEME_CAR(v), ml);
	v = SCHEME_CDR(v);
      }
      if (!SCHEME_NULLP(v))
	add_marks(v, ml);
    } else if (SCHEME_BOXP(v)) {
      add_marks(SCHEME_BOX_VAL(v), ml);
    } else if (SCHEME_VECTORP(v)) {
      int size = SCHEME_VEC_SIZE(v), i;

      for (i = 0; i < size; i++) {
	add_marks(SCHEME_VEC_ELS(v)[i], ml);
      }
    }

    stx->marks = scheme_null;
  }

  return stx->val;
}

/*========================================================================*/
/*                    Scheme functions and helpers                        */
/*========================================================================*/

static Scheme_Object *syntax_p(int argc, Scheme_Object **argv)
{
  return SCHEME_STXP(argv[0]) ? scheme_true : scheme_false;
}

static Scheme_Object *syntax_e(int argc, Scheme_Object **argv)
{
  return scheme_stx_content(argv[0]);
}

static Scheme_Object *syntax_line(int argc, Scheme_Object **argv)
{
  Scheme_Stx *stx = (Scheme_Stx *)argv[0];

  return scheme_make_integer(stx->line);
}

static Scheme_Object *syntax_col(int argc, Scheme_Object **argv)
{
  Scheme_Stx *stx = (Scheme_Stx *)argv[0];

  return scheme_make_integer(stx->col);
}

static Scheme_Object *syntax_src(int argc, Scheme_Object **argv)
{
  Scheme_Stx *stx = (Scheme_Stx *)argv[0];

  while ((stx->src == (Scheme_Object *)graph)
	 || (stx->src == (Scheme_Object *)graphref)) {
    stx = (Scheme_Stx *)stx->val;
  }

  return stx->src;
}
