/*
  MzScheme
  Copyright (c) 1995-2001 Matthew Flatt
 
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

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/

/* This file contains the MzScheme reader, including the normal reader
   and the one for .zo files. The normal reader is a recursive-descent
   parser. The really messy part is number parsing, which is in a
   different file, numstr.c. */

#include "schpriv.h"
#include "schmach.h"
#include "schcpt.h"
#include "schvers.h"
#include <stdlib.h>
#include <ctype.h>
#ifdef USE_STACKAVAIL
# include <malloc.h>
#endif

#define MAX_QUICK_SYMBOL_SIZE 255

/* Init options for embedding: */
int scheme_square_brackets_are_parens = 1;
int scheme_curly_braces_are_parens = 1;

/* local function prototypes */

static Scheme_Object *read_case_sensitive(int, Scheme_Object *[]);
static Scheme_Object *read_bracket_as_paren(int, Scheme_Object *[]);
static Scheme_Object *read_brace_as_paren(int, Scheme_Object *[]);
static Scheme_Object *read_accept_graph(int, Scheme_Object *[]);
static Scheme_Object *read_accept_compiled(int, Scheme_Object *[]);
static Scheme_Object *read_accept_box(int, Scheme_Object *[]);
static Scheme_Object *read_accept_pipe_quote(int, Scheme_Object *[]);
static Scheme_Object *read_decimal_as_inexact(int, Scheme_Object *[]);
static Scheme_Object *read_dot_as_symbol(int, Scheme_Object *[]);
static Scheme_Object *print_graph(int, Scheme_Object *[]);
static Scheme_Object *print_struct(int, Scheme_Object *[]);
static Scheme_Object *print_box(int, Scheme_Object *[]);
static Scheme_Object *print_vec_shorthand(int, Scheme_Object *[]);

/* To make reading more efficient with OS threads, pass the
   process (thread) pointer along. */
#ifndef MZ_REAL_THREADS
# define CURRENTPROCPRM /* empty */
# define CURRENTPROCARG /* empty */
#else
# define CURRENTPROCPRM , Scheme_Thread *p
# define CURRENTPROCARG , p
#endif

#define SRCLOC_TMPL " in %q[%L%ld]"

static Scheme_Object *read_list(Scheme_Object *port, Scheme_Object *stxsrc,
				long line, long col,
				char closer,
				int vec, int use_stack,
				Scheme_Hash_Table **ht 
				CURRENTPROCPRM);
static Scheme_Object *read_string(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col
				  CURRENTPROCPRM);
static Scheme_Object *read_quote(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col,
				 Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_syntax_quote(Scheme_Object *port, Scheme_Object *stxsrc,
					long line, long col,
					Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_vector(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col,
				  char closer, 
				  long reqLen, const char *reqBuffer,
				  Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_number(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col,
				  int, int, int, int 
				  CURRENTPROCPRM);
static Scheme_Object *read_symbol(Scheme_Object *port, Scheme_Object *stxsrc,
				  long line, long col
				  CURRENTPROCPRM);
static Scheme_Object *read_character(Scheme_Object *port, long line, long col CURRENTPROCPRM);
static Scheme_Object *read_quasiquote(Scheme_Object *port, Scheme_Object *stxsrc,
				      long line, long col,
				      Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_unquote(Scheme_Object *port, Scheme_Object *stxsrc,
				   long line, long col,
				   Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_unquote_splicing(Scheme_Object *port, Scheme_Object *stxsrc,
					    long line, long col,
					    Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_box(Scheme_Object *port, Scheme_Object *stxsrc,
			       long line, long col,
			       Scheme_Hash_Table **ht CURRENTPROCPRM);
static Scheme_Object *read_compiled(Scheme_Object *port,
				    Scheme_Hash_Table **ht CURRENTPROCPRM);

static int skip_whitespace_comments(Scheme_Object *port);

/* local_... is copy of parameter values, made before read starts: */

#define local_can_read_compiled (THREAD_FOR_LOCALS->quick_can_read_compiled)
#define local_can_read_pipe_quote (THREAD_FOR_LOCALS->quick_can_read_pipe_quote)
#define local_can_read_box (THREAD_FOR_LOCALS->quick_can_read_box)
#define local_can_read_graph (THREAD_FOR_LOCALS->quick_can_read_graph)
#define local_case_sensitive (THREAD_FOR_LOCALS->quick_case_sens)
#define local_square_brackets_are_parens (THREAD_FOR_LOCALS->quick_square_brackets_are_parens)
#define local_curly_braces_are_parens (THREAD_FOR_LOCALS->quick_curly_braces_are_parens)
#define local_read_decimal_inexact (THREAD_FOR_LOCALS->quick_read_decimal_inexact)
#define local_can_read_dot (THREAD_FOR_LOCALS->quick_can_read_dot)

#define local_list_stack (THREAD_FOR_LOCALS->list_stack)
#define local_list_stack_pos (THREAD_FOR_LOCALS->list_stack_pos)

#define local_rename_memory (THREAD_FOR_LOCALS->rn_memory)

#ifndef MZ_REAL_THREADS
# define THREAD_FOR_LOCALS scheme_current_thread
#else
# define THREAD_FOR_LOCALS p
#endif

/* A list stack is used to speed up the creation of intermediate lists
   during .zo reading. */

#define NUM_CELLS_PER_STACK 500

typedef struct {
  int pos;
  Scheme_Object *stack;
} ListStackRec;

#define STACK_START(r) (r.pos = local_list_stack_pos, r.stack = local_list_stack)
#define STACK_END(r) (local_list_stack_pos = r.pos, local_list_stack = r.stack)

#ifdef MZ_PRECISE_GC
# define USE_LISTSTACK(x) 0
#else
# define USE_LISTSTACK(x) x
#endif

static Scheme_Object *quote_symbol;
static Scheme_Object *quasiquote_symbol;
static Scheme_Object *unquote_symbol;
static Scheme_Object *unquote_splicing_symbol;
static Scheme_Object *syntax_symbol;

/* Table of built-in variable refs for .zo loading: */
static Scheme_Object **variable_references;

/*========================================================================*/
/*                             initialization                             */
/*========================================================================*/

void scheme_init_read(Scheme_Env *env)
{
  REGISTER_SO(variable_references);

  REGISTER_SO(quote_symbol);
  REGISTER_SO(quasiquote_symbol);
  REGISTER_SO(unquote_symbol);
  REGISTER_SO(unquote_splicing_symbol);
  REGISTER_SO(syntax_symbol);
    
  quote_symbol = scheme_intern_symbol("quote");
  quasiquote_symbol = scheme_intern_symbol("quasiquote");
  unquote_symbol = scheme_intern_symbol("unquote");
  unquote_splicing_symbol = scheme_intern_symbol("unquote-splicing");
  syntax_symbol = scheme_intern_symbol("syntax");

  scheme_add_global_constant("read-case-sensitive", 
			     scheme_register_parameter(read_case_sensitive, 
						       "read-case-sensitive",
						       MZCONFIG_CASE_SENS), 
			     env);
  scheme_add_global_constant("read-square-bracket-as-paren", 
			     scheme_register_parameter(read_bracket_as_paren, 
						       "read-square-bracket-as-paren",
						       MZCONFIG_SQUARE_BRACKETS_ARE_PARENS), 
			     env);
  scheme_add_global_constant("read-curly-brace-as-paren", 
			     scheme_register_parameter(read_brace_as_paren, 
						       "read-curly-brace-as-paren",
						       MZCONFIG_CURLY_BRACES_ARE_PARENS), 
			     env);
  scheme_add_global_constant("read-accept-graph", 
			     scheme_register_parameter(read_accept_graph, 
						       "read-accept-graph",
						       MZCONFIG_CAN_READ_GRAPH), 
			     env);
  scheme_add_global_constant("read-accept-compiled", 
			     scheme_register_parameter(read_accept_compiled, 
						       "read-accept-compiled",
						       MZCONFIG_CAN_READ_COMPILED), 
			     env);
  scheme_add_global_constant("read-accept-box", 
			     scheme_register_parameter(read_accept_box, 
						       "read-accept-box",
						       MZCONFIG_CAN_READ_BOX), 
			     env);
  scheme_add_global_constant("read-accept-bar-quote",
			     scheme_register_parameter(read_accept_pipe_quote,
						       "read-accept-bar-quote",
						       MZCONFIG_CAN_READ_PIPE_QUOTE), 
			     env);
  scheme_add_global_constant("read-decimal-as-inexact",
			     scheme_register_parameter(read_decimal_as_inexact,
						       "read-decimal-as-inexact",
						       MZCONFIG_READ_DECIMAL_INEXACT), 
			     env);
  scheme_add_global_constant("read-dot-as-symbol",
			     scheme_register_parameter(read_dot_as_symbol,
						       "read-dot-as-symbol",
						       MZCONFIG_CAN_READ_DOT), 
			     env);
  scheme_add_global_constant("print-graph", 
			     scheme_register_parameter(print_graph, 
						       "print-graph",
						       MZCONFIG_PRINT_GRAPH), 
			     env);
  scheme_add_global_constant("print-struct", 
			     scheme_register_parameter(print_struct, 
						       "print-struct",
						       MZCONFIG_PRINT_STRUCT), 
			     env);
  scheme_add_global_constant("print-box", 
			     scheme_register_parameter(print_box, 
						       "print-box",
						       MZCONFIG_PRINT_BOX), 
			     env);
  scheme_add_global_constant("print-vector-length", 
			     scheme_register_parameter(print_vec_shorthand, 
						       "print-vector-length",
						       MZCONFIG_PRINT_VEC_SHORTHAND), 
			     env);
}

void scheme_alloc_list_stack(Scheme_Thread *process)
{
  Scheme_Object *sa;
  process->list_stack_pos = 0;
  sa = MALLOC_N_RT(Scheme_Object, NUM_CELLS_PER_STACK);
  process->list_stack = sa;
#ifdef MZ_PRECISE_GC
  /* Must set the tag on the first element: */
  process->list_stack[0].type = scheme_pair_type;
#endif
}

/*========================================================================*/
/*                             parameters                                 */
/*========================================================================*/

#define DO_CHAR_PARAM(name, pos) \
  return scheme_param_config(name, scheme_make_integer(pos), argc, argv, -1, NULL, NULL, 1)

static Scheme_Object *
read_case_sensitive(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-case-sensitive", MZCONFIG_CASE_SENS);
}

static Scheme_Object *
read_bracket_as_paren(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-square-bracket-as-paren", MZCONFIG_SQUARE_BRACKETS_ARE_PARENS);
}

static Scheme_Object *
read_brace_as_paren(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-curly-brace-as-paren", MZCONFIG_CURLY_BRACES_ARE_PARENS);
}

static Scheme_Object *
read_accept_graph(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-graph", MZCONFIG_CAN_READ_GRAPH);
}

static Scheme_Object *
read_accept_compiled(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-compiled", MZCONFIG_CAN_READ_COMPILED);
}

static Scheme_Object *
read_accept_box(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-box", MZCONFIG_CAN_READ_BOX);
}

static Scheme_Object *
read_accept_pipe_quote(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-accept-pipe-quote", MZCONFIG_CAN_READ_PIPE_QUOTE);
}

static Scheme_Object *
read_decimal_as_inexact(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-decimal-as-inexact", MZCONFIG_READ_DECIMAL_INEXACT);
}

static Scheme_Object *
read_dot_as_symbol(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("read-dot-as-symbol", MZCONFIG_CAN_READ_DOT);
}

static Scheme_Object *
print_graph(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-graph", MZCONFIG_PRINT_GRAPH);
}

static Scheme_Object *
print_struct(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-struct", MZCONFIG_PRINT_STRUCT);
}

static Scheme_Object *
print_box(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-box", MZCONFIG_PRINT_BOX);
}

static Scheme_Object *
print_vec_shorthand(int argc, Scheme_Object *argv[])
{
  DO_CHAR_PARAM("print-vector-length", MZCONFIG_PRINT_VEC_SHORTHAND);
}

/*========================================================================*/
/*                             main read loop                             */
/*========================================================================*/

#ifdef DO_STACK_CHECK

static Scheme_Object *read_inner(Scheme_Object *port, 
				 Scheme_Object *stxsrc,
				 Scheme_Hash_Table **ht 
				 CURRENTPROCPRM);

static Scheme_Object *read_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *o = (Scheme_Object *)p->ku.k.p1;
  Scheme_Hash_Table **ht = (Scheme_Hash_Table **)p->ku.k.p2;
  Scheme_Object *stxsrc = (Scheme_Object *)p->ku.k.p3;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;
  p->ku.k.p3 = NULL;

  return read_inner(o, stxsrc, ht
#ifdef MZ_REAL_THREADS
		    , p
#endif
		    );
}
#endif

#define MAX_GRAPH_ID_DIGITS 8

static Scheme_Object *
read_inner(Scheme_Object *port, Scheme_Object *stxsrc, Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  int ch, ch2, depth;
  long line = 0, col = 0;

#ifdef DO_STACK_CHECK
  {
# include "mzstkchk.h"
    {
# ifndef MZ_REAL_THREADS
      Scheme_Thread *p = scheme_current_thread;
# endif
      p->ku.k.p1 = (void *)port;
      p->ku.k.p2 = (void *)ht;
      p->ku.k.p3 = (void *)stxsrc;
      return scheme_handle_stack_overflow(read_k);
    }
  }
#endif

 start_over:

  SCHEME_USE_FUEL(1);

  do {
    ch = scheme_getc(port);
  } while (isspace(ch));

  line = scheme_tell_line(port);
  col = scheme_tell_column(port);

  switch ( ch )
    {
    case EOF: return (scheme_eof);
    case ']': 
      if (!local_square_brackets_are_parens) {
	scheme_ungetc(ch, port);
	return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
      } else
        scheme_read_err(port, line, col, 0, "read: unexpected '%c'", ch);
    case '}': 
      if (!local_curly_braces_are_parens) {
	scheme_ungetc(ch, port);
	return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
      } else
        scheme_read_err(port, line, col, 0, "read: unexpected '%c'", ch);
    case ')': 
      scheme_read_err(port, line, col, 0, "read: unexpected '%c'", ch);
    case '(': 
      return read_list(port, stxsrc, line, col, ')', 0, 0, ht CURRENTPROCARG);
    case '[': 
      if (!local_square_brackets_are_parens) {
	scheme_ungetc(ch, port);
	return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
      } else
	return read_list(port, stxsrc, line, col, ']', 0, 0, ht CURRENTPROCARG);
    case '{': 
      if (!local_curly_braces_are_parens) {
	scheme_ungetc(ch, port);
	return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
      } else
	return read_list(port, stxsrc, line, col, '}', 0, 0, ht CURRENTPROCARG);
    case '"': return read_string(port, stxsrc, line, col CURRENTPROCARG);
    case '\'': return read_quote(port, stxsrc, line, col, ht CURRENTPROCARG);
    case '`': 
      if (local_can_read_dot) {
	scheme_read_err(port, line, col, 0, "read: illegal use of backquote");
	return NULL;
      } else
	return read_quasiquote(port, stxsrc, line, col, ht CURRENTPROCARG);
    case ',':
      if (local_can_read_dot) {
	scheme_read_err(port, line, col, 0, "read: illegal use of `,'");
	return NULL;
      } else {
	if (scheme_peekc(port) == '@') {
	  ch = scheme_getc(port);
	  return read_unquote_splicing(port, stxsrc, line, col, ht CURRENTPROCARG);
	} else
	  return read_unquote(port, stxsrc, line, col, ht CURRENTPROCARG);
      }
    case ';':
      while (((ch = scheme_getc(port)) != '\n') && (ch != '\r')) {
	if (ch == EOF)
	  return scheme_eof;
      }
      goto start_over;
    case '+':
    case '-':
    case '.':
      ch2 = scheme_peekc(port);
      if (isdigit(ch2) || (ch2 == '.') 
	  || (ch2 == 'i') || (ch2 == 'I') /* Maybe inf */
	  || (ch2 == 'n') || (ch2 == 'N') /* Maybe nan*/ ) {
	scheme_ungetc(ch, port);
	return read_number(port, stxsrc, line, col, 0, 0, 10, 0 CURRENTPROCARG);
      } else {
	scheme_ungetc(ch, port);
	return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
      }
    case '#':
      ch = scheme_getc(port);
      switch ( ch )
	{
	case '%':
	  scheme_ungetc('%', port);
	  scheme_ungetc('#', port);
	  return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
	case '(': return read_vector(port, stxsrc, line, col, ')', -1, NULL, ht CURRENTPROCARG);
	case '[': 
	  if (!local_square_brackets_are_parens) {
	    scheme_read_err(port, line, col, 0, "read: bad syntax `#['");
	    return NULL;
	  } else
	    return read_vector(port, stxsrc, line, col, ']', -1, NULL, ht CURRENTPROCARG);
	case '{': 
	  if (!local_curly_braces_are_parens) {
	    scheme_read_err(port, line, col, 0, "read: bad syntax `#{'");
	    return NULL;
	  } else
	    return read_vector(port, stxsrc, line, col, '}', -1, NULL, ht CURRENTPROCARG);
	case '\\': 
	  {
	    Scheme_Object *chr;
	    chr = read_character(port, line, col CURRENTPROCARG);
	    if (stxsrc)
	      chr = scheme_make_stx(chr, line, col, stxsrc, STX_SRCTAG);
	    return chr;
	  }
	case 'T':
	case 't': return (stxsrc 
			  ? scheme_make_stx(scheme_true, line, col, stxsrc, STX_SRCTAG) 
			  : scheme_true);
	case 'F':
	case 'f': return (stxsrc 
			  ? scheme_make_stx(scheme_false, line, col, stxsrc, STX_SRCTAG) 
			  : scheme_false);
	case 'X':
	case 'x': return read_number(port, stxsrc, line, col, 0, 0, 16, 1 CURRENTPROCARG);
	case 'B':
	case 'b': return read_number(port, stxsrc, line, col, 0, 0, 2, 1 CURRENTPROCARG);
	case 'O':
	case 'o': return read_number(port, stxsrc, line, col, 0, 0, 8, 1 CURRENTPROCARG);
	case 'D':
	case 'd': return read_number(port, stxsrc, line, col, 0, 0, 10, 1 CURRENTPROCARG);
	case 'E':
	case 'e': return read_number(port, stxsrc, line, col, 0, 1, 10, 0 CURRENTPROCARG);
	case 'I':
	case 'i': return read_number(port, stxsrc, line, col, 1, 0, 10, 0 CURRENTPROCARG);
	case '\'': return read_syntax_quote(port, stxsrc, line, col, ht CURRENTPROCARG);
	case '~': 
	  if (local_can_read_compiled) {
	    Scheme_Object *cpld;
	    cpld = read_compiled(port, ht CURRENTPROCARG);
	    if (stxsrc) 
	      cpld = scheme_make_stx(cpld, line, col, stxsrc, STX_SRCTAG);
	    return cpld;
	  } else {
	    scheme_read_err(port, line, col, 0, 
			    "read: #~ compiled expressions not currently enabled");
	    return NULL;
	  }
	case '|':
	  depth = 0;
	  ch2 = 0;
	  do {
	    ch = scheme_getc(port);
	    if (ch == EOF)
	      scheme_read_err(port, line, col, 1, 
			      "read: end of file in #| comment");
	    if ((ch2 == '|') && (ch == '#')) {
	      if (!(depth--))
		goto start_over;
	    } else if ((ch2 == '#') && (ch == '|'))
	      depth++;
	    ch2 = ch;
	  } while (1);
	  break;
	case '&':
	  if (local_can_read_box)
	    return read_box(port, stxsrc, line, col, ht CURRENTPROCARG);
	  else {
	    scheme_read_err(port, line, col, 0, 
			    "read: #& expressions not currently enabled");
	    return scheme_void;
	  }
	case '$':
	  if (((Scheme_Input_Port *)port)->get_special_fun) {
	    Scheme_Object *v;
	    v = scheme_get_special(port);
	    if (SCHEME_STXP(v)) {
	      if (!stxsrc)
		v = scheme_syntax_to_datum(v, 0, NULL);
	    } else if (stxsrc) {
	      Scheme_Object *s;
	      s = scheme_make_stx(scheme_false, line, col, stxsrc, STX_SRCTAG);
	      v = scheme_datum_to_syntax(v, s, scheme_false, 1, 0);
	    }
	    return v;
	  } else {
	    scheme_read_err(port, line, col, 0, 
			    "read: port does not support `#$' input");
	    return NULL;
	  }
	default:
	  {
	    int vector_length = -1;
	    int i = 0, j = 0, overflow = 0, digits = 0;
	    char tagbuf[64], vecbuf[64]; /* just for errors */
	    
	    while (isdigit(ch)) {
	      if (digits <= MAX_GRAPH_ID_DIGITS)
		digits++;

	      /* For vector error msgs, want to drop leading zeros: */
	      if (j || (ch != '0')) {
		if (j < 60) {
		  vecbuf[j++] = ch;
		} else if (j == 60) {
		  vecbuf[j++] = '.';
		  vecbuf[j++] = '.';
		  vecbuf[j++] = '.';
		  vecbuf[j] = 0;
		}
	      }

	      /* For tag error msgs, want to keep zeros: */
	      if (i < 60) {
		tagbuf[i++] = ch;
	      } else if (i == 60) {
		tagbuf[i++] = '.';
		tagbuf[i++] = '.';
		tagbuf[i++] = '.';
		tagbuf[i] = 0;
	      }
		
	      if (!overflow) {
		long old_len;

		if (vector_length < 0)
		  vector_length = 0;
	      
		old_len = vector_length;
		vector_length = (vector_length * 10) + (ch - 48);
		if ((vector_length < 0)|| ((vector_length / 10) != old_len)) {
		  overflow = 1;
		}
	      }
	      ch = scheme_getc(port);
	    }

	    if (overflow)
	      vector_length = -2;
	    vecbuf[j] = 0;
	    tagbuf[i] = 0;

	    if (ch == '(')
	      return read_vector(port, stxsrc, line, col, ')', vector_length, vecbuf, ht CURRENTPROCARG);
	    if (ch == '[' && local_square_brackets_are_parens)
	      return read_vector(port, stxsrc, line, col, ']', vector_length, vecbuf, ht CURRENTPROCARG);
	    if (ch == '{' && local_curly_braces_are_parens)
	      return read_vector(port, stxsrc, line, col, '}', vector_length, vecbuf, ht CURRENTPROCARG);
	    
	    if (ch == '#' && (vector_length != -1)) {
	      /* Not a vector after all: a graph reference */
	      Scheme_Object *ph;
	      
	      if (!local_can_read_graph)
		scheme_read_err(port, line, col, 0,
				"read: #..# expressions not currently enabled");
	      
	      if (digits > MAX_GRAPH_ID_DIGITS)
		scheme_read_err(port, line, col, 0,
				"read: graph id too long in #%s#",
				tagbuf);
	    
	      if (*ht)
		ph = (Scheme_Object *)scheme_hash_get(*ht, scheme_make_integer(vector_length));
	      else
		ph = NULL;
	      
	      if (!ph) {
		scheme_read_err(port, line, col, 0,
				"read: no #%ld= for #%ld#",
				vector_length, vector_length);
		return scheme_void;
	      }
	      return ph;
	    }
	    if (ch == '=' && (vector_length != -1)) {
	      /* Not a vector after all: a graph definition */
	      Scheme_Object *v, *ph;
	      
	      if (!local_can_read_graph)
		scheme_read_err(port, line, col, 0,
				 "read: #..= expressions not currently enabled");
	      
	      if (digits > MAX_GRAPH_ID_DIGITS)
		scheme_read_err(port, line, col, 0,
				 "read: graph id too long in #%s=",
				 tagbuf);
	      
	      if (*ht) {
		if (scheme_hash_get(*ht, scheme_make_integer(vector_length))) {
		  scheme_read_err(port, line, col, 0,
				  "read: multiple #%ld= tags",
				  vector_length);
		  return NULL;
		}
	      } else {
		Scheme_Hash_Table *tht;
		tht = scheme_make_hash_table(SCHEME_hash_ptr);
		*ht = tht;
	      }
	      ph = scheme_alloc_small_object();
	      ph->type = scheme_placeholder_type;
	      
	      scheme_hash_set(*ht, scheme_make_integer(vector_length), (void *)ph);
	      
	      v = read_inner(port, stxsrc, ht CURRENTPROCARG);
	      if (stxsrc)
		v = scheme_make_graph_stx(v, line, col);
	      SCHEME_PTR_VAL(ph) = v;
	      
	      return v;
	    }
	  
	    {
	      char lbuffer[200];
	      
	      if (ch == EOF)
		ch = 0;
	      
	      sprintf(lbuffer, "%s%c", tagbuf, ch);
	      
	      scheme_read_err(port, line, col, 0,
			      "read: bad syntax `#%s'",
			       lbuffer);

	      return NULL;
	    }
	  }
	}
    default:
      scheme_ungetc(ch, port);
      if (isdigit (ch))
	return read_number(port, stxsrc, line, col, 0, 0, 10, 0 CURRENTPROCARG);
      else
	return read_symbol(port, stxsrc, line, col CURRENTPROCARG);
    }
}

#ifdef DO_STACK_CHECK
static Scheme_Object *resolve_references(Scheme_Object *obj, 
					 Scheme_Object *port,
					 int mkstx
					 CURRENTPROCPRM);

static Scheme_Object *resolve_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *o = (Scheme_Object *)p->ku.k.p1;
  Scheme_Object *port = (Scheme_Object *)p->ku.k.p2;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;

  return resolve_references(o, port, p->ku.k.i1 CURRENTPROCARG);
}
#endif

static Scheme_Object *resolve_references(Scheme_Object *obj, 
					 Scheme_Object *port,
					 int mkstx
					 CURRENTPROCPRM)
{
  Scheme_Object *start = obj, *result;

#ifdef DO_STACK_CHECK
  {
# include "mzstkchk.h"
    {
# ifndef MZ_REAL_THREADS
      Scheme_Thread *p = scheme_current_thread;
# endif
      p->ku.k.p1 = (void *)obj;
      p->ku.k.p2 = (void *)port;
      p->ku.k.i1 = mkstx;
      return scheme_handle_stack_overflow(resolve_k);
    }
  }
#endif

  SCHEME_USE_FUEL(1);

  if (SAME_TYPE(SCHEME_TYPE(obj), scheme_placeholder_type)) {
    obj = (Scheme_Object *)SCHEME_PTR_VAL(obj);
    while (SAME_TYPE(SCHEME_TYPE(obj), scheme_placeholder_type)) {
      if (SAME_OBJ(start, obj)) {
	scheme_read_err(port, -1, -1, 0,
			"read: illegal cycle");
	return NULL;
      }
      obj = (Scheme_Object *)SCHEME_PTR_VAL(obj);
    }
    return obj;
  }

  result = obj;
  if (mkstx && SCHEME_STXP(obj))
    obj = SCHEME_STX_VAL(obj);

  if (SCHEME_PAIRP(obj)) {
    Scheme_Object *rr;
    rr = resolve_references(SCHEME_CAR(obj), port, mkstx CURRENTPROCARG);
    SCHEME_CAR(obj) = rr;
    rr = resolve_references(SCHEME_CDR(obj), port, mkstx CURRENTPROCARG);
    SCHEME_CDR(obj) = rr;
  } else if (SCHEME_BOXP(obj)) {
    Scheme_Object *rr;
    rr = resolve_references(SCHEME_BOX_VAL(obj), port, mkstx CURRENTPROCARG);
    SCHEME_BOX_VAL(obj) = rr;
  } else if (SCHEME_VECTORP(obj)) {
    int i, len;
    Scheme_Object *prev_rr, *prev_v;

    prev_v = prev_rr = NULL;
    len = SCHEME_VEC_SIZE(obj);
    for (i = 0; i < len; i++) {
      Scheme_Object *rr;
      if (SCHEME_VEC_ELS(obj)[i] == prev_v) {
	rr = prev_rr;
      } else {
	prev_v = SCHEME_VEC_ELS(obj)[i];
	rr = resolve_references(prev_v, port, mkstx CURRENTPROCARG);
	prev_rr = rr;
      }
      SCHEME_VEC_ELS(obj)[i] = rr;
    }
  }

  return result;
}

Scheme_Object *
scheme_internal_read(Scheme_Object *port, Scheme_Object *stxsrc, int crc, 
		     Scheme_Config *config CURRENTPROCPRM)
{
#ifndef MZ_REAL_THREADS
  Scheme_Thread *p = scheme_current_thread;
#endif
  Scheme_Object *v;
  Scheme_Hash_Table *ht = NULL;

  local_can_read_compiled = crc;
  local_can_read_pipe_quote = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_CAN_READ_PIPE_QUOTE));
  local_can_read_box = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_CAN_READ_BOX));
  local_can_read_graph = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_CAN_READ_GRAPH));
  local_case_sensitive = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_CASE_SENS));
  local_square_brackets_are_parens = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_SQUARE_BRACKETS_ARE_PARENS));
  local_curly_braces_are_parens = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_CURLY_BRACES_ARE_PARENS));
  local_read_decimal_inexact = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_READ_DECIMAL_INEXACT));
  local_can_read_dot = SCHEME_TRUEP(scheme_get_param(config, MZCONFIG_CAN_READ_DOT));

  if (USE_LISTSTACK(!p->list_stack))
    scheme_alloc_list_stack(p);

  v = read_inner(port, stxsrc, &ht CURRENTPROCARG);

  if (ht) {
    /* Resolve placeholders: */
    v = resolve_references(v, port, !!stxsrc CURRENTPROCARG);
  }

  return v;
}

static void *scheme_internal_read_k(void)
{
  Scheme_Thread *p = scheme_current_thread;
  Scheme_Object *port = (Scheme_Object *)p->ku.k.p1;
  Scheme_Object *stxsrc = (Scheme_Object *)p->ku.k.p2;

  p->ku.k.p1 = NULL;
  p->ku.k.p2 = NULL;

  return (void *)scheme_internal_read(port, stxsrc, local_can_read_compiled,
				      scheme_current_thread->config
				      CURRENTPROCARG);
}

Scheme_Object *
scheme_read(Scheme_Object *port)
{
  Scheme_Thread *p = scheme_current_thread;

  local_can_read_compiled = SCHEME_TRUEP(scheme_get_param(scheme_config, 
							  MZCONFIG_CAN_READ_COMPILED));

  p->ku.k.p1 = (void *)port;
  p->ku.k.p2 = NULL;
  return (Scheme_Object *)scheme_top_level_do(scheme_internal_read_k, 0);
}

Scheme_Object *
scheme_read_syntax(Scheme_Object *port, Scheme_Object *stxsrc)
{
  Scheme_Thread *p = scheme_current_thread;

  local_can_read_compiled = SCHEME_TRUEP(scheme_get_param(scheme_config, 
							  MZCONFIG_CAN_READ_COMPILED));

  p->ku.k.p1 = (void *)port;
  p->ku.k.p2 = (void *)stxsrc;
  return (Scheme_Object *)scheme_top_level_do(scheme_internal_read_k, 0);
}

Scheme_Object *scheme_resolve_placeholders(Scheme_Object *obj, int mkstx)
{
#ifdef MZ_REAL_THREADS
  Scheme_Thread *p = scheme_current_thread;
#endif
  return resolve_references(obj, NULL, mkstx CURRENTPROCARG);
}

/*========================================================================*/
/*                             list reader                                */
/*========================================================================*/

/* "(" has already been read */
static Scheme_Object *
read_list(Scheme_Object *port, 
	  Scheme_Object *stxsrc, long line, long col,
	  char closer, int vec, int use_stack,
	  Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  Scheme_Object *list = NULL, *last = NULL, *car, *cdr, *pair, *infixed = NULL;
  int ch, next;
  int brackets = local_square_brackets_are_parens;
  int braces = local_curly_braces_are_parens;
  long start, startline, dotpos, dotline;

  start = scheme_tell_column(port);
  startline = scheme_tell_line(port);

  while (1) {
    ch = skip_whitespace_comments(port);
    if (ch == EOF) {
      char s[2];
      s[0] = closer;
      s[1] = 0;
      scheme_read_err(port, startline, start, 1, 
		      "read: expected a '%c'; started",
		      closer);
    }

    if (ch == closer) {
      if (!list)
	list = scheme_null;
      return (stxsrc
	      ? scheme_make_stx(list, line, col, stxsrc, STX_SRCTAG)
	      : list);
    }

    scheme_ungetc(ch, port);
    car = read_inner(port, stxsrc, ht CURRENTPROCARG);

    if (USE_LISTSTACK(use_stack)) {
      if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
	/* Overflow */
	Scheme_Object *sa;
	local_list_stack_pos = 0;
	sa = MALLOC_N_RT(Scheme_Object, NUM_CELLS_PER_STACK);
	local_list_stack = sa;
      }

      pair = local_list_stack + (local_list_stack_pos++);
      pair->type = scheme_pair_type;
      pair->u.pair_val.car = car;
      pair->u.pair_val.cdr = scheme_null;
    } else {
      pair = scheme_make_pair(car, scheme_null);
      if (stxsrc)
	SCHEME_SET_PAIR_IMMUTABLE(pair);
    }

    ch = skip_whitespace_comments(port);
    if (ch == closer) {
      cdr = pair;
      if (!list)
	list = cdr;
      else
	SCHEME_CDR(last) = cdr;
      
      if (infixed) {
	/* Assert: we're not using the list stack */
	list = scheme_make_pair(infixed, list);
	if (stxsrc)
	  SCHEME_SET_PAIR_IMMUTABLE(list);
      }

      return (stxsrc
	      ? scheme_make_stx(list, line, col, stxsrc, STX_SRCTAG)
	      : list);
    } else if (!local_can_read_dot
	       && (ch == '.')
	       && (next = scheme_peekc(port),
		   ((next == EOF)
		    || isspace(next)
		    || (next == '(')
		    || (next == ')')
		    || (next == '"')
		    || (next == ';')
		    || (next == '\'')
		    || (next == '`')
		    || (next == ',')
		    || ((next == '[') && brackets)
		    || ((next == '{') && braces)
		    || ((next == ']') && brackets)
		    || ((next == '}') && braces)))) {
      dotpos = scheme_tell_column(port);
      dotline = scheme_tell_line(port);
      
      if (vec || infixed) {
	scheme_read_err(port, dotline, dotpos, 0,
			"read: illegal use of \".\"");
	return NULL;
      }
      cdr = read_inner(port, stxsrc, ht CURRENTPROCARG);
      ch = skip_whitespace_comments(port);
      if (ch != closer) {
	if (ch == '.') {
	  /* Parse as infix: */
	  infixed = cdr;

	  if (!list)
	    list = pair;
	  else
	    SCHEME_CDR(last) = pair;
	  last = pair;
	} else {
	  scheme_read_err(port, dotline, dotpos, 0,
			  "read: illegal use of \".\"");
	  return NULL;
	}
      } else {
	SCHEME_CDR(pair) = cdr;
	cdr = pair;
	if (!list)
	  list = cdr;
	else
	  SCHEME_CDR(last) = cdr;

	if (infixed) {
	  /* Assert: we're not using the list statck */
	  list = scheme_make_pair(infixed, list);
	  if (stxsrc)
	    SCHEME_SET_PAIR_IMMUTABLE(list);
	}

	return (stxsrc
		? scheme_make_stx(list, line, col, stxsrc, STX_SRCTAG)
		: list);
      }
    } else {
      scheme_ungetc(ch, port);
      cdr = pair;
      if (!list)
	list = cdr;
      else
	SCHEME_CDR(last) = cdr;
      last = cdr;
    }
  }
}

/*========================================================================*/
/*                            string reader                               */
/*========================================================================*/

/* '"' has already been read */
static Scheme_Object *
read_string(Scheme_Object *port,
	    Scheme_Object *stxsrc, long line, long col
	    CURRENTPROCPRM)
{
  char *buf, *oldbuf, onstack[32];
  int i, j, n, n1, ch;
  long start, startline;
  long size = 31, oldsize;
  Scheme_Object *result;

  i = 0;
  start = scheme_tell_column(port);
  startline = scheme_tell_line(port);
  buf = onstack;
  while ((ch = scheme_getc (port)) != '"') {
    if (ch == EOF)
      scheme_read_err(port, startline, start, 1,
		      "read: expected a '\"'; started");
    /* Note: errors will leave junk on the port, with an open \". */
    if (ch == '\\') {
      ch = scheme_getc(port);
      switch ( ch ) {
      case '\\': case '\"': break;
      case 'a': ch = '\a'; break;
      case 'b': ch = '\b'; break;
      case 'e': ch = 27; break; /* escape */
      case 'f': ch = '\f'; break;
      case 'n': ch = '\n'; break;
      case 'r': ch = '\r'; break;
      case 't': ch = '\t'; break;
      case 'v': ch = '\v'; break;
      case 'x':
	ch = scheme_getc(port);
	if (isxdigit(ch)) {
	  n = ch<='9' ? ch-'0' : toupper(ch)-'A';
	  ch = scheme_getc(port);
	  if (isxdigit(ch)) {
	    n = n*16 + (ch<='9' ? ch-'0' : toupper(ch)-'A');
	  } else {
	    scheme_ungetc(ch, port);
	  }
	  ch = n;
	} else {
#if 1
	  scheme_read_err(port, startline, start, 1,
			  "read: no hex digits following \\x in string");
#else
	  scheme_ungetc(ch, port);
	  ch = 'x';
#endif
	}
	break;
      default:
	if ((ch >= '0') && (ch <= '7')) {
	  for (n = j = 0; j < 3; j++) {
	    n1 = 8*n + ch - '0';
	    if (n1 > 255) {
#if 1
	      scheme_read_err(port, startline, start, 1,
			      "read: escape sequence \\%o out of range in string", n1);
#else
	      scheme_ungetc(ch, port);
	      break;
#endif
	    }
	    n = n1;
	    if (j < 2) {
	      ch = scheme_getc(port);
	      if (!((ch >= '0') && (ch <= '7'))) {
		scheme_ungetc(ch, port);
		break;
	      }
	    }
	  }
	  ch = n;
	} else {
	  scheme_read_err(port, startline, start, 1,
			  "read: unknown escape sequence \\%c in string", ch);
	}
	break;
      }
    }

    if (i >= size) { 
      oldsize = size;
      oldbuf = buf;
      
      size *= 2;
      buf = (char *)scheme_malloc_atomic(size + 1);
      memcpy(buf, oldbuf, oldsize);
    }
    buf[i++] = ch;
  }
  buf[i] = '\0';

  result = scheme_make_immutable_sized_string(buf, i, i <= 31);
  if (stxsrc)
    result =  scheme_make_stx(result, line, col, stxsrc, STX_SRCTAG);
  return result;
}

/*========================================================================*/
/*                            vector reader                               */
/*========================================================================*/

/* "#(" has been read */
static Scheme_Object *
read_vector (Scheme_Object *port,
	     Scheme_Object *stxsrc, long line, long col,
	     char closer, 
	     long requestLength, const char *reqBuffer,
	     Scheme_Hash_Table **ht CURRENTPROCPRM)
/* requestLength == -1 => no request
   requestLength == -2 => overflow */
{
  Scheme_Object *lresult, *obj, *vec, **els;
  int len, i;
  ListStackRec r;

  STACK_START(r);
  lresult = read_list(port, stxsrc, line, col, closer, 1, 1, ht CURRENTPROCARG);
  STACK_END(r);

  if (requestLength == -2) {
    scheme_raise_out_of_memory("read", "making vector of size %s", reqBuffer);
    return NULL;
  }

  if (stxsrc)
    obj = ((Scheme_Stx *)lresult)->val;
  else
    obj = lresult;

  len = scheme_list_length(obj);
  if (requestLength >= 0 && len > requestLength) {
    char buffer[20];
    sprintf(buffer, "%ld", requestLength);
    scheme_read_err(port, line, col, 0,
		    "read: vector length %ld is too small, "
		    "%d values provided",
		    requestLength, len);
  }
  if (requestLength < 0)
    requestLength = len;
  vec = scheme_make_vector(requestLength, NULL);
  els = SCHEME_VEC_ELS(vec);
  for (i = 0; i < len ; i++) {
    els[i] = SCHEME_CAR(obj);
    obj = SCHEME_CDR(obj);
  }
  if (i < requestLength) {
    if (len) {
      obj = els[len - 1];
      if (stxsrc) {
	/* Set the graph flag if obj sharing is visible: */
	Scheme_Object *v;
	v = SCHEME_STX_VAL(obj);
	if (SCHEME_PAIRP(v) || SCHEME_VECTORP(v) || SCHEME_BOXP(v))
	  obj = scheme_make_graph_stx(obj, -1, -1);
      }
    } else
      obj = scheme_make_integer(0);
    for (; i < requestLength; i++) {
      els[i] = obj;
    }
  }

  if (stxsrc) {
    ((Scheme_Stx *)lresult)->val = vec;
    return lresult;
  } else
    return vec;
}

/*========================================================================*/
/*                            symbol reader                               */
/*========================================================================*/

/* Also dispatches to number reader, since things not-a-number are
   symbols. */

typedef int (*Getc_Fun_r)(Scheme_Object *port);

/* nothing has been read, except maybe some flags */
static Scheme_Object  *
read_number_or_symbol(Scheme_Object *port,
		      Scheme_Object *stxsrc, long line, long col,
		      int is_float, int is_not_float, 
		      int radix, int radix_set, 
		      int is_symbol, int pipe_quote CURRENTPROCPRM)
{
  char *buf, *oldbuf, onstack[MAX_QUICK_SYMBOL_SIZE];
  int size, oldsize;
  int i, ch, quoted, quoted_ever = 0, running_quote = 0;
  int rq_pos = 0, rq_line = 0;
  int case_sens = local_case_sensitive;
  int brackets = local_square_brackets_are_parens;
  int braces = local_curly_braces_are_parens;
  int decimal_inexact = local_read_decimal_inexact;
  Scheme_Object *o;
  int ungetc_ok;
  Getc_Fun_r getc_fun;

  ungetc_ok = scheme_peekc_is_ungetc(port);

  if (ungetc_ok)
    getc_fun = scheme_getc;
  else
    getc_fun = scheme_peekc;

  i = 0;
  size = MAX_QUICK_SYMBOL_SIZE - 1;
  buf = onstack;

  while (((ch = getc_fun(port)) != EOF)
	 && (running_quote
	     || (!isspace(ch)
		 && (ch != '(')
		 && (ch != ')')
		 && (ch != '"')
		 && (ch != ';')
		 && (ch != '\'')
		 && (ch != '`')
		 && (ch != ',')
		 && ((ch != '[') || !brackets)
		 && ((ch != '{') || !braces)
		 && ((ch != ']') || !brackets)
		 && ((ch != '}') || !braces)))) {
    if (!ungetc_ok)
      scheme_getc(port);
    if (ch == '\\' && !running_quote) {
      ch = scheme_getc(port);
      if (ch == EOF) {
	scheme_read_err(port, -1, -1, 1, 
			"read: EOF following \"\\\"");
	return NULL;
      }
      quoted = 1;
      quoted_ever = 1;
    } else if (ch == '|' && pipe_quote) {
      quoted_ever = 1;
      running_quote = !running_quote;
      quoted = 0;

      rq_pos = scheme_tell_column(port);
      rq_line = scheme_tell_line(port);

      continue;
    } else 
      quoted = 0;

    if (i >= size) {
      oldsize = size;
      oldbuf = buf;
      
      size *= 2;
      buf = (char *)scheme_malloc_atomic(size + 1);
      memcpy(buf, oldbuf, oldsize);
    }

    if (!case_sens)
      ch = tolower(ch);

    buf[i++] = ch;
  }

  if (ungetc_ok)
    scheme_ungetc(ch, port);

  if (running_quote) {
    scheme_read_err(port, rq_line, rq_pos, 1,
		    "read: unbalanced `|'");
    return NULL;
  }

  buf[i] = '\0';

  if (!quoted_ever && (i == 1) && (buf[0] == '.') && !local_can_read_dot) {
    scheme_read_err(port, scheme_tell_line(port), scheme_tell_column(port), 0,
		    "read: illegal use of \".\"");
    return NULL;
  }

  if ((is_symbol || quoted_ever) && !is_float && !is_not_float && !radix_set)
    o = scheme_false;
  else
    o = scheme_read_number(buf, i, 
			   is_float, is_not_float, decimal_inexact, 
			   radix, radix_set,
			   port, NULL, 0,
			   line, col);

  if (SAME_OBJ(o, scheme_false))
    o = scheme_intern_exact_symbol(buf, i);

  if (stxsrc)
    o = scheme_make_stx(o, line, col, stxsrc, STX_SRCTAG);

  return o;
}

static Scheme_Object  *
read_number(Scheme_Object *port,
	    Scheme_Object *stxsrc, long line, long col,
	    int is_float, int is_not_float, 
	    int radix, int radix_set CURRENTPROCPRM)
{
  return read_number_or_symbol(port, stxsrc, line, col,
			       is_float, is_not_float,
			       radix, radix_set, 0,
			       local_can_read_pipe_quote
			       CURRENTPROCARG);
}

static Scheme_Object  *
read_symbol(Scheme_Object *port,
	    Scheme_Object *stxsrc, long line, long col
	    CURRENTPROCPRM)
{
  return read_number_or_symbol(port, stxsrc, line, col, 
			       0, 0, 10, 0, 1, 
			       local_can_read_pipe_quote
			       CURRENTPROCARG);
}

/*========================================================================*/
/*                              char reader                               */
/*========================================================================*/

/* "#\" has been read */
static Scheme_Object *
read_character(Scheme_Object *port, long line, long col CURRENTPROCPRM)
{
  int ch, next;

  ch = scheme_getc(port);
  next = scheme_peekc(port);
  
  if ((ch >= '0' && ch <= '7') && (next >= '0' && next <= '7')) {
    /* a is the same as next */
    int last;

    last = (scheme_getc(port), scheme_getc(port));

    if (last < '0' || last > '7' || ch > '3') {
      scheme_read_err(port, line, col, 0,
		      "read: bad character constant #\\%c%c%c",
		      ch, next, (last == EOF ? ' ' : last));
      return NULL;
    }

    ch = ((ch - '0') << 6) + ((next - '0') << 3) + (last - '0');

    return scheme_make_char(ch);
  }

  if (isalpha(next)) {
    char *buf, *oldbuf, onstack[32];
    int i;
    long size = 31, oldsize;

    i = 1;
    buf = onstack;
    buf[0] = tolower(ch);
    while ((ch = scheme_peekc(port), (ch != EOF) && isalpha(ch))) {
      scheme_getc(port);
      if (i >= size) {
	oldsize = size;
	oldbuf = buf;
	
	size *= 2;
	buf = (char *)scheme_malloc_atomic(size + 1);
	memcpy(buf, oldbuf, oldsize);
      }
      buf[i++] = tolower(ch);
    }
    buf[i] = '\0';
    
    switch (buf[0]) {
    case 'n': /* maybe `newline' or 'null' or 'nul' */
      if (!strcmp(buf, "newline"))
	return scheme_make_char('\n');
      if (!strcmp(buf, "null") || !strcmp(buf, "nul"))
	return scheme_make_char('\0');
      break;
    case 's': /* maybe `space' */
      if (!strcmp(buf, "space"))
	return scheme_make_char(' ');
      break;
    case 'r': /* maybe `rubout' or `return' */
      if (!strcmp(buf, "rubout"))
	return scheme_make_char(0x7f);
      if (!strcmp(buf, "return"))
	return scheme_make_char('\r');
      break;
    case 'p': /* maybe `page' */
      if (!strcmp(buf, "page"))
	return scheme_make_char('\f');
      break;
    case 't': /* maybe `tab' */
      if (!strcmp(buf, "tab"))
	return scheme_make_char('\t');
      break;
    case 'v': /* maybe `vtab' */
      if (!strcmp(buf, "vtab"))
	return scheme_make_char(0xb);
      break;
    case 'b': /* maybe `backspace' */
      if (!strcmp(buf, "backspace"))
	return scheme_make_char('\b');
      break;
    case 'l': /* maybe `linefeed' */
      if (!strcmp(buf, "linefeed"))
	return scheme_make_char('\n');
      break;
    default:
      break;
    }

    scheme_read_err(port, line, col, 0,
		    "read: bad character constant: #\\%s",
		    buf);
  }

  if (ch == EOF) {
    long pos, l;

    pos = scheme_tell_column(port);
    l = scheme_tell_line(port);

    scheme_read_err(port, l, pos, 1,
		    "read: expected a character after #\\");
  }

  return scheme_make_char(ch);
}

/*========================================================================*/
/*                            quote readers                               */
/*========================================================================*/

/* "'" has been read */
static Scheme_Object *
read_quote(Scheme_Object *port,
	   Scheme_Object *stxsrc, long line, long col,
	   Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  Scheme_Object *obj, *ret;

  obj = read_inner(port, stxsrc, ht CURRENTPROCARG);
  ret = (stxsrc
	 ? scheme_make_stx(quote_symbol, line, col, stxsrc, STX_SRCTAG)
	 : quote_symbol);
  ret = scheme_make_pair(ret, scheme_make_pair(obj, scheme_null));
  if (stxsrc)
    ret = scheme_make_stx(ret, line, col, stxsrc, STX_SRCTAG);
  return ret;
}

/* "`" has been read */
static Scheme_Object *
read_quasiquote(Scheme_Object *port,
		Scheme_Object *stxsrc, long line, long col,
		Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  Scheme_Object *quoted_obj, *ret;
  
  quoted_obj = read_inner(port, stxsrc, ht CURRENTPROCARG);
  ret = (stxsrc
	 ? scheme_make_stx(quasiquote_symbol, line, col, stxsrc, STX_SRCTAG)
	 : quasiquote_symbol);
  ret = scheme_make_pair(ret, scheme_make_pair(quoted_obj, scheme_null));
  
  if (stxsrc)
    ret = scheme_make_stx(ret, line, col, stxsrc, STX_SRCTAG);
  return ret;
}

/* "," has been read */
static Scheme_Object *
read_unquote(Scheme_Object *port,
	     Scheme_Object *stxsrc, long line, long col,
	     Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  Scheme_Object *obj, *ret;

  obj = read_inner(port, stxsrc, ht CURRENTPROCARG);
  ret = (stxsrc
	 ? scheme_make_stx(unquote_symbol, line, col, stxsrc, STX_SRCTAG)
	 : unquote_symbol);
  ret = scheme_make_pair(ret, scheme_make_pair (obj, scheme_null));

  if (stxsrc)
    ret = scheme_make_stx(ret, line, col, stxsrc, STX_SRCTAG);
  return ret;
}

/* ",@" has been read */
static Scheme_Object *
read_unquote_splicing(Scheme_Object *port,
		      Scheme_Object *stxsrc, long line, long col,
		      Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  Scheme_Object *obj, *ret;

  obj = read_inner(port, stxsrc, ht CURRENTPROCARG);
  ret = (stxsrc
	 ? scheme_make_stx(unquote_splicing_symbol, line, col, stxsrc, STX_SRCTAG)
	 : unquote_splicing_symbol);
  ret = scheme_make_pair(ret, scheme_make_pair (obj, scheme_null));
  if (stxsrc)
    ret = scheme_make_stx(ret, line, col, stxsrc, STX_SRCTAG);
  return ret;
}

/* "#&" has been read */
static Scheme_Object *read_box(Scheme_Object *port,
			       Scheme_Object *stxsrc, long line, long col,
			       Scheme_Hash_Table **ht
			       CURRENTPROCPRM)
{
  Scheme_Object *o, *bx;

  o = read_inner(port, stxsrc, ht CURRENTPROCARG);
  
  bx = scheme_box(o);

  if (stxsrc)
    bx = scheme_make_stx(bx, line, col, stxsrc, STX_SRCTAG);

  return bx;
}

/* "#'" has been read */
static Scheme_Object *
read_syntax_quote(Scheme_Object *port,
		  Scheme_Object *stxsrc, long line, long col,
		  Scheme_Hash_Table **ht CURRENTPROCPRM)
{
  Scheme_Object *obj, *ret;

  obj = read_inner(port, stxsrc, ht CURRENTPROCARG);
  ret = (stxsrc
	 ? scheme_make_stx(syntax_symbol, line, col, stxsrc, STX_SRCTAG)
	 : syntax_symbol);
  ret = scheme_make_pair(ret, scheme_make_pair(obj, scheme_null));
  if (stxsrc)
    ret = scheme_make_stx(ret, line, col, stxsrc, STX_SRCTAG);
  return ret;
}

/*========================================================================*/
/*                               utilities                                */
/*========================================================================*/

static int
skip_whitespace_comments(Scheme_Object *port)
{
  int ch;

 start_over:

  while ((ch = scheme_getc(port), isspace(ch))) {}

  if (ch == ';') {
    do {
      ch = scheme_getc(port);
    } while (ch != '\n' && ch != '\r' && ch != EOF);
    goto start_over;
  }
  if (ch == '#' && (scheme_peekc(port) == '|')) {
    int depth = 0;
    int ch2 = 0;
    long col, line;

    col = scheme_tell_column(port);
    line = scheme_tell_line(port);
    
    (void)scheme_getc(port); /* re-read '|' */
    do {
      ch = scheme_getc(port);
      if (ch == EOF)
	scheme_read_err(port, line, col, 1,
			"read: end of file in #| comment; started");
      if ((ch2 == '|') && (ch == '#')) {
	if (!(depth--))
	  goto start_over;
      } else if ((ch2 == '#') && (ch == '|'))
	depth++;
      ch2 = ch;
    } while (1);

    goto start_over;
  }

  return ch;
}

/*========================================================================*/
/*                               .zo reader                               */
/*========================================================================*/

typedef struct CPort {
  long pos;
  unsigned char *start;
  long base;
  Scheme_Object *orig_port;
} CPort;
#define CP_GETC(cp) ((int)(cp->start[cp->pos++]))
#define CP_TELL(port) (port->pos + port->base)

static Scheme_Object *read_marshalled(int type,
				      CPort *port,
				      Scheme_Hash_Table **ht,
				      Scheme_Object **symtab
				      CURRENTPROCPRM);
static Scheme_Object *read_compact_list(int c, int proper, int use_stack,
					CPort *port, 
					Scheme_Hash_Table **ht,
					Scheme_Object **symtab
					CURRENTPROCPRM);
static Scheme_Object *read_compact_quote(CPort *port,
					 Scheme_Object **symtab,
					 int embedded
					 CURRENTPROCPRM);

static long read_compact_number(CPort *port)
{
  long flag, v, a, b, c, d;

  flag = CP_GETC(port);

  if (flag < 252)
    return flag;
  else if (flag == 252) {
    a = CP_GETC(port);
    b = CP_GETC(port);
    
    v = a
      + (b << 8);
    return v;
  } else if (flag == 254)
    return -CP_GETC(port);
  
  a = CP_GETC(port);
  b = CP_GETC(port);
  c = CP_GETC(port);
  d = CP_GETC(port);

  v = a
    + (b << 8)
    + (c << 16)
    + (d << 24);
  
  if (flag == 253)
    return v;
  else
    return -v;
}

static char *read_compact_chars(CPort *port, 
				char *buffer, 
				int bsize, int l)
{
  char *s;
  char *src;
  int i;

  if (l < bsize)
    s = buffer;
  else
    s = (char *)scheme_malloc_atomic(l + 1);

  src = (char *)port->start + port->pos;
  for (i = 0; i < l; i++) {
    s[i] = src[i];
  }
  port->pos += l;

  s[l] = 0;

  return s;
}

static Scheme_Object *read_compact_svector(CPort *port, int l)
{
  Scheme_Object *o;
  short *v;

  o = scheme_alloc_object();
  o->type = scheme_svector_type;

  SCHEME_SVEC_LEN(o) = l;
  if (l)
    v = MALLOC_N_ATOMIC(short, l);
  else
    v = NULL;
  SCHEME_SVEC_VEC(o) = v;

  while (l--) {
    short cn;
    cn = read_compact_number(port);
    v[l] = cn;
  }

  return o;
}

static int cpt_branch[256];

static Scheme_Object *read_compact(CPort *port, 
				   Scheme_Hash_Table **ht,
				   Scheme_Object **symtab,
				   int use_stack
				   CURRENTPROCPRM)
{
#define BLK_BUF_SIZE 32
  int ch;
  int l;
  char *s, buffer[BLK_BUF_SIZE];
  int need_car = 0, proper = 0;
  Scheme_Object *v, *first = NULL, *last = NULL;

  while (1) {
    ch = CP_GETC(port);
    switch(cpt_branch[ch]) {
    case CPT_ESCAPE:
      {
	int len;
	Scheme_Object *ep;
	char *s;

	len = read_compact_number(port);

#if defined(MZ_PRECISE_GC)
	s = read_compact_chars(port, buffer, BLK_BUF_SIZE, len);
#else
	s = (char *)port->start + port->pos;
	port->pos += len;
#endif

	ep = scheme_make_sized_string_input_port(s, len);
	
	v = read_inner(ep, NULL, ht CURRENTPROCARG);
      }
      break;
    case CPT_SYMBOL:
      l = read_compact_number(port);
      s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);
      v = scheme_intern_exact_symbol(s, l);

      l = read_compact_number(port);
      symtab[l] = v;
      break;
    case CPT_SYMREF:
      l = read_compact_number(port);
      v = symtab[l];
      break;
    case CPT_STRING:
      l = read_compact_number(port);
      s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);
      v = scheme_make_immutable_sized_string(s, l, l < BLK_BUF_SIZE);
      break;
    case CPT_CHAR:
      v = scheme_make_character(CP_GETC(port));
      break;
    case CPT_INT:
      v = scheme_make_integer(read_compact_number(port));
      break;
    case CPT_NULL:
      v = scheme_null;
      break;
    case CPT_TRUE:
      v = scheme_true;
      break;
    case CPT_FALSE:
      v = scheme_false;
      break;
    case CPT_VOID:
      v = scheme_void;
      break;
    case CPT_BOX:
      v = scheme_box(read_compact(port, ht, symtab, 0 CURRENTPROCARG));
      break;
    case CPT_PAIR:
      if (need_car) {
	Scheme_Object *car, *cdr;
	car = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	cdr = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	v = scheme_make_pair(car, cdr);
      } else {
	need_car = 1;
	continue;
      }
      break;
    case CPT_LIST:
      l = read_compact_number(port);
      if (need_car) {
	if (l == 1) {
	  Scheme_Object *car, *cdr;
	  car = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	  cdr = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	  v = scheme_make_pair(car, cdr);
	} else
	  v = read_compact_list(l, 0, 0, port, ht, symtab CURRENTPROCARG);
      } else {
	need_car = l;
	continue;
      }
      break;
    case CPT_VECTOR: 
      {
	Scheme_Object *vec;
	int i;

	l = read_compact_number(port);
	vec = scheme_make_vector(l, NULL);
      
	for (i = 0; i < l; i++) {
	  Scheme_Object *cv;
	  cv = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	  SCHEME_VEC_ELS(vec)[i] = cv;
	}

	v = vec;
      }
      break;
    case CPT_STX:
    case CPT_GSTX:
      {
	if (!local_rename_memory) {
	  Scheme_Hash_Table *rht;
	  rht = scheme_make_hash_table(SCHEME_hash_ptr);
	  local_rename_memory = rht;
	}

	v = read_compact(port, ht, symtab, 1 CURRENTPROCARG);
	v = scheme_datum_to_syntax(v, scheme_false, (Scheme_Object *)local_rename_memory,
				   ch == CPT_GSTX, 0);
      }
      break;
    case CPT_MARSHALLED:
      v = read_marshalled(read_compact_number(port), port, ht, symtab 
			  CURRENTPROCARG);
      break;
    case CPT_QUOTE:
      v = read_compact_quote(port, symtab, 1 CURRENTPROCARG);
      break;
    case CPT_REFERENCE:
      l = read_compact_number(port);
      v = variable_references[l];
      break;
    case CPT_LOCAL:
      {
	int p;
	p = read_compact_number(port);
	v = scheme_make_local(scheme_local_type, p);
      }
      break;
    case CPT_LOCAL_UNBOX:
      {
	int p;
	p = read_compact_number(port);
	v = scheme_make_local(scheme_local_unbox_type, p);
      }
      break;
    case CPT_SVECTOR: 
      {
	int l;
	l = read_compact_number(port);
	v = read_compact_svector(port, l);
      }
      break;
    case CPT_APPLICATION:
      {
	int c;
	Scheme_Object *l;
	ListStackRec r;

	c = read_compact_number(port) + 1;
	STACK_START(r);
	l = read_compact_list(c, 1, 1, port, ht, symtab CURRENTPROCARG);
	STACK_END(r);
  
	{
	  Scheme_Type_Reader reader;
	  reader = scheme_type_readers[scheme_application_type];
	  v = reader(l);
	}
      }
      break;
    case CPT_MODULE_INDEX:
	{
	  Scheme_Object *path, *base;

	  l = read_compact_number(port); /* symtab index */
	  path = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	  base = read_compact(port, ht, symtab, 0 CURRENTPROCARG);

	  v = scheme_make_modidx(path, base, scheme_false);

	  symtab[l] = v;
	}
	break;
    case CPT_MODULE_VAR:
      {
	Scheme_Object *mod, *var;
	
	l = read_compact_number(port); /* symtab index */
	mod = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	var = read_compact(port, ht, symtab, 0 CURRENTPROCARG);

	v = scheme_alloc_object();
	v->type = scheme_module_variable_type;
	SCHEME_PTR1_VAL(v) = mod;
	SCHEME_PTR2_VAL(v) = var;
	
	symtab[l] = v;
      }
      break;
    case CPT_SMALL_LOCAL_START:
    case CPT_SMALL_LOCAL_UNBOX_START:
      {
	Scheme_Type type;
	int k;
	
	if (CPT_BETWEEN(ch, SMALL_LOCAL_UNBOX)) {
	  k = 1;
	  type = scheme_local_unbox_type;
	  ch -= CPT_SMALL_LOCAL_UNBOX_START;
	} else {
	  k = 0;
	  type = scheme_local_type;
	  ch -= CPT_SMALL_LOCAL_START;
	}
	if (ch < MAX_CONST_LOCAL_POS)
	  v = scheme_local[ch][k];
	else
	  v = scheme_make_local(type, ch);
      }
      break;
    case CPT_SMALL_MARSHALLED_START:
      {
	l = ch - CPT_SMALL_MARSHALLED_START;
	v = read_marshalled(l, port, ht, symtab CURRENTPROCARG);
      }
      break;
    case CPT_SMALL_SYMBOL_START:
      {
	l = ch - CPT_SMALL_SYMBOL_START;
	s = read_compact_chars(port, buffer, BLK_BUF_SIZE, l);
	v = scheme_intern_exact_symbol(s, l);

	l = read_compact_number(port);
	symtab[l] = v;
      }
      break;
    case CPT_SMALL_NUMBER_START:
      {
	l = ch - CPT_SMALL_NUMBER_START;
	v = scheme_make_integer(l);
      }
      break;
    case CPT_SMALL_SVECTOR_START:
      {
	l = ch - CPT_SMALL_SVECTOR_START;
	v = read_compact_svector(port, l);
      }
      break;
    case CPT_SMALL_PROPER_LIST_START:
    case CPT_SMALL_LIST_START:
      {
	int ppr = CPT_BETWEEN(ch, SMALL_PROPER_LIST);
	l = ch - (ppr ? CPT_SMALL_PROPER_LIST_START : CPT_SMALL_LIST_START);
      	if (need_car) {
	  if (l == 1) {
	    Scheme_Object *car, *cdr;
	    car = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
	    cdr = (ppr 
		   ? scheme_null
		   : read_compact(port, ht, symtab, 0 CURRENTPROCARG));
	    v = scheme_make_pair(car, cdr);
	  } else
	    v = read_compact_list(l, ppr, /* use_stack */ 0, port, ht, symtab CURRENTPROCARG);
	} else {
	  proper = ppr;
	  need_car = l;
	  continue;
	}  
      }
      break;
    case CPT_SMALL_APPLICATION_START:
      {
	int c;
	Scheme_Object *l;
	ListStackRec r;

	c = (ch - CPT_SMALL_APPLICATION_START) + 1;
	STACK_START(r);
	l = read_compact_list(c, 1, 1, port, ht, symtab CURRENTPROCARG);
	STACK_END(r);

	{
	  Scheme_Type_Reader reader;
	  reader = scheme_type_readers[scheme_application_type];
	  v = reader(l);
	}
      }
      break;
    default:
      {
	v = NULL;
	scheme_read_err(port->orig_port, -1, CP_TELL(port), 0,
			"read (compiled): bad #~ contents: %d", ch);
      }
    }

    if (need_car) {
      Scheme_Object *pair;

      if (USE_LISTSTACK(use_stack)) {
	if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
	  /* Overflow */
	  Scheme_Object *sa;
	  local_list_stack_pos = 0;
	  sa = MALLOC_N_RT(Scheme_Object, NUM_CELLS_PER_STACK);
	  local_list_stack = sa;
	}
	
	pair = local_list_stack + (local_list_stack_pos++);
	pair->type = scheme_pair_type;
	pair->u.pair_val.car = v;
	pair->u.pair_val.cdr = scheme_null;
      } else
	pair = scheme_make_pair(v, scheme_null);

      if (last)
	SCHEME_CDR(last) = pair;
      else
	first = pair;
      last = pair;
      --need_car;
      if (!need_car && proper)
	break;
    } else {
      if (last)
	SCHEME_CDR(last) = v;
      break;
    }
  }

  return first ? first : v;
}

static Scheme_Object *read_compact_list(int c, int proper, int use_stack,
					CPort *port, 
					Scheme_Hash_Table **ht,
					Scheme_Object **symtab
					CURRENTPROCPRM)
{
  Scheme_Object *v, *first, *last, *pair;

  v = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
  if (USE_LISTSTACK(use_stack)) {
    if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
      /* Overflow */
      Scheme_Object *sa;
      local_list_stack_pos = 0;
      sa = MALLOC_N_RT(Scheme_Object, NUM_CELLS_PER_STACK);
      local_list_stack = sa;
    }
    
    last = local_list_stack + (local_list_stack_pos++);
    last->type = scheme_pair_type;
    last->u.pair_val.car = v;
    last->u.pair_val.cdr = scheme_null;
  } else
    last = scheme_make_pair(v, scheme_null);

  first = last;

  while (--c) {
    v = read_compact(port, ht, symtab, 0 CURRENTPROCARG);

    if (USE_LISTSTACK(use_stack)) {
      if (local_list_stack_pos >= NUM_CELLS_PER_STACK) {
	/* Overflow */
	Scheme_Object *sa;
	local_list_stack_pos = 0;
	sa = MALLOC_N_RT(Scheme_Object, NUM_CELLS_PER_STACK);
	local_list_stack = sa;
      }
      
      pair = local_list_stack + (local_list_stack_pos++);
      pair->type = scheme_pair_type;
      pair->u.pair_val.car = v;
      pair->u.pair_val.cdr = scheme_null;
    } else
      pair = scheme_make_pair(v, scheme_null);

    SCHEME_CDR(last) = pair;
    last = pair;
  }

  if (!proper) {
    v = read_compact(port, ht, symtab, 0 CURRENTPROCARG);
    SCHEME_CDR(last) = v;
  }

  return first;
}

static Scheme_Object *read_compact_quote(CPort *port,
					 Scheme_Object **symtab,
					 int embedded
					 CURRENTPROCPRM)
{
  Scheme_Hash_Table *q_ht;
  Scheme_Object *v;

  /* Use a new hash table. A compiled quoted form may have graph
     structure, but only local graph structure is allowed. */
  q_ht = NULL;
  v = read_compact(port, &q_ht, symtab, 0 CURRENTPROCARG);

  if (q_ht)
    resolve_references(v, NULL, 0 CURRENTPROCARG);
  
  return v;
}

static Scheme_Object *read_marshalled(int type,
				      CPort *port,
				      Scheme_Hash_Table **ht,
				      Scheme_Object **symtab
				      CURRENTPROCPRM)
{
  Scheme_Object *l;
  ListStackRec r;
  Scheme_Type_Reader reader;

  STACK_START(r);
  l = read_compact(port, ht, symtab, 1 CURRENTPROCARG);
  STACK_END(r);

  reader = scheme_type_readers[type];

  if (!reader)
    scheme_read_err(port->orig_port, -1, CP_TELL(port), 0,
		    "read (compiled): bad #~ type number: %d");
  
  return reader(l);
}

static long read_compact_number_from_port(Scheme_Object *port)
{
  long flag, v, a, b, c, d;

  flag = scheme_getc(port);

  if (flag < 252)
    return flag;
  if (flag == 254)
    return -scheme_getc(port);
  
  a = scheme_getc(port);
  b = scheme_getc(port);

  if (flag == 252) {
    v = a
      + (b << 8);
    return v;
  }

  c = scheme_getc(port);
  d = scheme_getc(port);

  v = a
    + (b << 8)
    + (c << 16)
    + (d << 24);
  
  if (flag == 253)
    return v;
  else
    return -v;
}

/* "#~" has been read */
static Scheme_Object *read_compiled(Scheme_Object *port, 
				    Scheme_Hash_Table **ht
				    CURRENTPROCPRM)
{
#ifndef MZ_REAL_THREADS
  Scheme_Thread *p = scheme_current_thread;
#endif
  Scheme_Object *result;
  CPort cp;
  long size, got;
  CPort *rp;
  long symtabsize;
  Scheme_Object **symtab;

  if (USE_LISTSTACK(!p->list_stack))
    scheme_alloc_list_stack(p);

  if (!cpt_branch[1]) {
    int i;

    for (i = 0; i < 256; i++) {
      cpt_branch[i] = i;
    }

#define FILL_IN(v) \
    for (i = CPT_ ## v ## _START; i < CPT_ ## v ## _END; i++) { \
      cpt_branch[i] = CPT_ ## v ## _START; \
    }
    FILL_IN(SMALL_NUMBER);
    FILL_IN(SMALL_SYMBOL);
    FILL_IN(SMALL_MARSHALLED);
    FILL_IN(SMALL_LIST);
    FILL_IN(SMALL_PROPER_LIST);
    FILL_IN(SMALL_LOCAL);
    FILL_IN(SMALL_LOCAL_UNBOX);
    FILL_IN(SMALL_SVECTOR);
    FILL_IN(SMALL_APPLICATION);
  }

  if (!variable_references)
    variable_references = scheme_make_builtin_references_table();

  /* Check version: */
  size = read_compact_number_from_port(port);
  {
    char buf[64];
    
    if (size < 0) size = 0;
    if (size > 63) size = 63;

    got = scheme_get_chars(port, size, buf, 0);
    buf[got] = 0;

    if (strcmp(buf, VERSION))
      scheme_read_err(port, -1, -1, 0,
		      "read (compiled): code compiled for version %s, not %s",
		      (buf[0] ? buf : "???"), VERSION);
  }

  symtabsize = read_compact_number_from_port(port);

  size = read_compact_number_from_port(port);
  cp.start = (unsigned char *)scheme_malloc_atomic(size);
  cp.pos = 0;
  cp.base = scheme_tell(port);
  cp.orig_port = port;
  if ((got = scheme_get_chars(port, size, (char *)cp.start, 0)) != size)
    scheme_read_err(port, -1, -1, 0,
		    "read (compiled): bad count: %ld != %ld");
  rp = &cp;

  local_rename_memory = NULL;

  symtab = MALLOC_N(Scheme_Object *, symtabsize);

  result = read_marshalled(scheme_compilation_top_type, rp, ht, symtab CURRENTPROCARG);

  local_rename_memory = NULL;

# ifdef MZ_PRECISE_GC
  rp = &cp; /* Ensures that cp is known to be alive. */
# endif

  return result;
}
