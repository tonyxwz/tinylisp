#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "lenv.h"
#include "lobj.h"
#include "qexpr.h"
#include "tl.h"

int
main(int argc, char** argv)
{
  return repl();
}

int
repl()
{
  mpc_parser_t* Integer = mpc_new("integer");
  mpc_parser_t* Double = mpc_new("double");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Qexpr = mpc_new("qexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* TL = mpc_new("tl");

  // symbol: '+' | '-' | '*' | '/' | '%' | '^' | \
    //     \"add\" | \"sub\" | \"mul\" | \"div\" | \"mod\" | \
    //     \"exp\" | \"log\" | \"pow\" | \
    //     \"min\" | \"max\" | \
    //     \"list\" | \"head\" | \"tail\" | \"join\" | \"eval\"; \
    //

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    integer  : /[0-9]+/ ; \
    double   : /(\\b|\\B)-?([0-9]*)?\\.[0-9]+(\\B|\\b)/ ; \
    symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
    sexpr: '(' <expr>* ')' ;\
    qexpr: '{' <expr>* '}'; \
    expr     : <double> | <integer> | <symbol> | <sexpr> | <qexpr> ; \
    tl       : /^/ <expr>* /$/; \
  ",
            Double,
            Integer,
            Symbol,
            Sexpr,
            Qexpr,
            Expr,
            TL);

  puts("Tony's Lisp (tl) version 0.1-dev");
  puts("Press Ctrl+C to exit");

  lenv* env = lenv_new();
  init_env(env);

  while (1) {
    char* input = readline(">>> ");
    if (input == NULL) {
      free(input);
      break;
    }
    add_history(input);
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, TL, &r)) {
      // mpc_ast_print(r.output);
      lobj* tmp = eval(env, lobj_read(r.output));
      lobj_println(tmp);
      lobj_del(tmp);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    /* printf("You've entered %s\n", input); */
    free(input);
  }

  lenv_del(env);
  mpc_cleanup(7, Double, Integer, Symbol, Sexpr, Qexpr, Expr, TL);
  return 0;
}

/* The design of the parser guarantees that any input is read as sexpr at the
 * ast root node.
 * >>> mod
 * ast: (mod)
 *  eval "(mod)"
 *    eval_sexpr "(mod)"
 *      eval "mod" -> read from environment "mod" which is a function
 */
lobj*
eval(lenv* env, lobj* v)
{
  if (v->type == LOBJ_SYM) {
    lobj* x = lenv_read(env, v);
    lobj_del(v);
    return x;
  } else if (v->type == LOBJ_SEXPR)
    return eval_sexpr(env, v);
  else // qexpr, num, err: no eval
    return v;
}

/* Think of func `eval` as a transformer, taking in one lobj* transform it and
 * return back.
 * lobj(sexpr) -> lobj(num/err)
 * since lobj_read always read a expression as a sexpr
 */
lobj*
eval_sexpr(lenv* env, lobj* v)
{
  for (int i = 0; i < v->count; ++i) {
    v->cell[i] = eval(env, v->cell[i]);
  }
  // error handling, TODO move to above for loop?
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type == LOBJ_ERR)
      return lobj_take(v, i);
  }
  if (v->count == 0) // empty expression ()
    return v;
  if (v->count == 1) // echo literals (5) TODO allow or not?
  {
    lobj* x = lobj_pop(v, 0);
    if (x->type == LOBJ_FUNC) {
      lobj* x2 = lobj_call(x, v, env);
      lobj_del(x);
      return x2;
    } else {
      lobj_del(v);
      return x;
    }
  }

  // operator/symbol is the first cell of sexpr
  lobj* f = lobj_pop(v, 0);
  if (f->type != LOBJ_FUNC) {
    lobj* err =
      lobj_err("S-expression starts with incorrect type, expecting %s, got %s",
               lobj_typename(LOBJ_FUNC),
               lobj_typename(f->type));
    lobj_del(v);
    lobj_del(f);
    return err;
  }
  // v contains argument to f, after poping f out
  lobj* result = lobj_call(f, v, env);
  lobj_del(f);
  return result;
}

void
init_env(lenv* e)
{
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "=", builtin_assign);
  lenv_add_builtin(e, "dir", builtin_dir);
  lenv_add_builtin(e, "echo", builtin_echo);
  lenv_add_builtin(e, "del", builtin_del);
  lenv_add_builtin(e, "\\", builtin_lambda);
  lenv_add_builtin(e, "lambda", builtin_lambda);
  lenv_add_builtin(e, "fn", builtin_fn);
  lenv_add_builtin(e, "if", builtin_if);

  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "!=", builtin_ne);

  lenv_add_builtin(e, "<", builtin_lt);
  lenv_add_builtin(e, ">", builtin_gt);
  lenv_add_builtin(e, "<=", builtin_le);
  lenv_add_builtin(e, ">=", builtin_ge);

  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "eval", builtin_eval);
  lenv_add_builtin(e, "len", builtin_len);

  lenv_add_builtin(e, "add", builtin_add);
  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "sub", builtin_sub);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "mul", builtin_mul);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "div", builtin_div);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "min", builtin_min);
  lenv_add_builtin(e, "max", builtin_max);
  lenv_add_builtin(e, "mod", builtin_mod);
  lenv_add_builtin(e, "exp", builtin_exp);
  lenv_add_builtin(e, "pow", builtin_pow);
  lenv_add_builtin(e, "ln", builtin_ln);
}

lobj*
builtin_var(lenv* env, lobj* a, char* operator)
{
  // sexpr (symbol args...)
  // def {a b c} 1 2 3
  LASSERT_ARGC(operator, a, 2);
  LASSERT_TYPE_I(operator, a, 0, LOBJ_QEXPR);

  lobj* syms = a->cell[0];
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(a,
            syms->cell[i]->type == LOBJ_SYM,
            "<function %s> can only bind to %s, got %s",
            operator,
            lobj_typename(LOBJ_SYM),
            lobj_typename(syms->cell[i]->type));
  }
  LASSERT(a,
          syms->count == a->count - 1,
          "<function %s> got %d symbols and %d values",
          operator,
          syms->count,
          a->count - 1);
  for (int i = 0; i < syms->count; ++i) {
    // TODO protect builtin symbols
    if (strcmp(operator, "=") == 0) {
      lenv_create(env, syms->cell[i], a->cell[i + 1]);
    }
    if (strcmp(operator, "def") == 0) {
      lenv_global_create(env, syms->cell[i], a->cell[i + 1]);
    }
    // else undefined
  }
  lobj_del(a);
  return lobj_sexpr();
}

lobj*
builtin_def(lenv* env, lobj* a)
{
  return builtin_var(env, a, "def");
}

lobj*
builtin_assign(lenv* env, lobj* a)
{
  return builtin_var(env, a, "=");
}

lobj*
builtin_dir(lenv* env, lobj* a)
{
  LASSERT(
    a, a->count == 0, "<function dir> expecting 0 argument, got %d", a->count);

  for (int i = 0; i < env->count; ++i) {
    printf("%s:\t", env->syms[i]);
    lobj_println(env->objs[i]);
  }
  lobj_del(a);
  return lobj_sexpr();
}

lobj*
builtin_del(lenv* env, lobj* a)
{
  // del { 1 2 3}
  LASSERT(
    a, a->count == 1, "<function del> expecting 1 argument, got %d", a->count);
  LASSERT(a, a->cell[0]->type == LOBJ_QEXPR, "symbol must be qexpr");
  lobj* syms = a->cell[0]; // qexpr
  LASSERT(syms,
          syms->count == 1,
          "<function del> deletes 1 symbol at a time, got %d",
          syms->count);
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(a,
            syms->cell[i]->type == LOBJ_SYM,
            "<function del> can only delete symbols");
  }
  lobj* result = lenv_remove(env, syms->cell[0]);
  lobj_del(a);
  return result;
}

lobj*
builtin_echo(lenv* env, lobj* a)
{
  // lobj_println(a);
  // lobj_del(a);
  // return lobj_sexpr();
  return a;
}

lobj*
builtin_lambda(lenv* env, lobj* a)
{
  LASSERT_ARGC("\\", a, 2);
  LASSERT_TYPE_I("\\", a, 0, LOBJ_QEXPR);
  LASSERT_TYPE_I("\\", a, 1, LOBJ_QEXPR);

  for (int i = 0; i < a->cell[0]->count; ++i) {
    LASSERT(a,
            (a->cell[0]->cell[i]->type == LOBJ_SYM),
            "<function \\> lambda argument at %d must be %s, got %s",
            i + 1,
            lobj_typename(LOBJ_SYM),
            lobj_typename(a->cell[0]->cell[i]->type));
  }
  lobj* formals = lobj_pop(a, 0);
  lobj* body = lobj_pop(a, 0);
  lobj* lambda = lobj_lambda(formals, body);
  lobj_del(a);
  return lambda;
}

lobj*
builtin_fn(lenv* env, lobj* a)
{
  LASSERT_ARGC("fn", a, 2);
  LASSERT_TYPE_I("fn", a, 0, LOBJ_QEXPR);
  LASSERT_TYPE_I("fn", a, 1, LOBJ_QEXPR);
  LASSERT(a,
          a->cell[0]->count > 0,
          "<function fn> requires at least one arg as function name");
  lobj* fname = lobj_append(lobj_qexpr(), lobj_pop(a->cell[0], 0));
  lobj* fun = builtin_lambda(env, a); // a is freed

  lobj* tmp = lobj_qexpr();
  lobj_append(tmp, fname);
  lobj_append(tmp, fun);

  return builtin_def(env, tmp);
}

lobj*
builtin_if(lenv* env, lobj* a)
{
  // TODO make else part optional
  LASSERT_ARGC("if", a, 3);
  LASSERT_TYPE_I("if", a, 0, LOBJ_INT);
  LASSERT_TYPE_I("if", a, 1, LOBJ_QEXPR);
  LASSERT_TYPE_I("if", a, 2, LOBJ_QEXPR);

  lobj* x;
  if (a->cell[0]->i) {
    a->cell[1]->type = LOBJ_SEXPR;
    x = eval(env, lobj_pop(a, 1));
  } else {
    a->cell[2]->type = LOBJ_SEXPR;
    x = eval(env, lobj_pop(a, 2));
  }
  lobj_del(a);
  return x;
}

lobj*
builtin_cmp(lenv* env, lobj* a, char* op)
{
  LASSERT_ARGC(op, a, 2);
  int r = 0;
  if (strcmp(op, "==") == 0) {
    r = lobj_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lobj_eq(a->cell[0], a->cell[1]);
  } // unsafe
  lobj_del(a);
  return lobj_int(r);
}

lobj*
builtin_eq(lenv* env, lobj* a)
{
  return builtin_cmp(env, a, "==");
}
lobj*
builtin_ne(lenv* env, lobj* a)
{
  return builtin_cmp(env, a, "!=");
}

// TODO compare double
lobj*
builtin_ord(lenv* env, lobj* a, char* op)
{
  LASSERT_ARGC(op, a, 2);
  LASSERT_TYPE_I(op, a, 0, LOBJ_INT);
  LASSERT_TYPE_I(op, a, 1, LOBJ_INT);
  int ans = 0;
  if (strcmp(op, ">") == 0) {
    ans = a->cell[0]->i > a->cell[1]->i;
  } else if (strcmp(op, "<") == 0) {
    ans = a->cell[0]->i < a->cell[1]->i;
  } else if (strcmp(op, ">=") == 0) {
    ans = a->cell[0]->i >= a->cell[1]->i;
  } else if (strcmp(op, "<=") == 0) {
    ans = a->cell[0]->i <= a->cell[1]->i;
  }
  lobj_del(a);
  return lobj_int(ans);
}
lobj*
builtin_lt(lenv* env, lobj* a)
{
  return builtin_ord(env, a, "<");
}
lobj*
builtin_le(lenv* env, lobj* a)
{
  return builtin_ord(env, a, "<=");
}

lobj*
builtin_gt(lenv* env, lobj* a)
{
  return builtin_ord(env, a, ">");
}
lobj*
builtin_ge(lenv* env, lobj* a)
{
  return builtin_ord(env, a, ">=");
}
