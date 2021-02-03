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
#include "tl.h"

int
repl()
{
  mpc_parser_t* Number = mpc_new("number");
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
    number   : /(\\b|\\B)-?([0-9]*\\.)?[0-9]+(\\B|\\b)/ ; \
    symbol   : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ; \
    sexpr: '(' <expr>* ')' ;\
    qexpr: '{' <expr>* '}'; \
    expr     : <number> | <symbol> | <sexpr> | <qexpr> ; \
    tl       : /^/ <expr>* /$/; \
  ",
            Number,
            Symbol,
            Sexpr,
            Qexpr,
            Expr,
            TL);

  puts("Tony's Lisp (tl) version 0.1-dev");
  puts("Press Ctrl+C to exit");

  // TODO
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
      mpc_ast_print(r.output);
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
  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, TL);
  return 0;
}

lobj*
eval(lenv* env, lobj* v)
{
  if (v->type == LOBJ_SYM) {
    lobj* x = lenv_read(env, v);
    lobj_del(v);
    // func with 0 args
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
  // evaluate all the childrens, call eval here recusively
  for (int i = 0; i < v->count; ++i) {
    v->cell[i] = eval(env, v->cell[i]);
  }
  // error handling, TODO move to above for loop
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type == LOBJ_ERR)
      return lobj_take(v, i);
  }
  if (v->count == 0) // empty expression ()
    return v;
  if (v->count == 1) // empty expression (5)
  {
    lobj* x = lobj_take(v, 0);
    if (x->type == LOBJ_FUNC) {
      lobj* x2 = lobj_call(x, lobj_sexpr(), env);
      // x->func(env, lobj_sexpr());
      lobj_del(x);
      return x2;
    } else {
      return x;
    }
  }

  // operator/symbol is the first cell of sexpr
  lobj* f = lobj_pop(v, 0);
  if (f->type != LOBJ_FUNC) {
    lobj_del(v);
    lobj_del(f);
    return lobj_err("first element of qexpr must be a function");
  }
  // v contains argument to f
  lobj* result = lobj_call(f, v, env);
  lobj_del(f);
  return result;
}

void
init_env(lenv* e)
{
  lenv_swallow(e, lobj_sym("def"), lobj_func(builtin_def));
  lenv_swallow(e, lobj_sym("="), lobj_func(builtin_assign));
  lenv_swallow(e, lobj_sym("dir"), lobj_func(builtin_dir));
  lenv_swallow(e, lobj_sym("echo"), lobj_func(builtin_echo));
  lenv_swallow(e, lobj_sym("del"), lobj_func(builtin_del));
  lenv_swallow(e, lobj_sym("\\"), lobj_func(builtin_lambda));

  lenv_swallow(e, lobj_sym("list"), lobj_func(builtin_list));
  lenv_swallow(e, lobj_sym("head"), lobj_func(builtin_head));
  lenv_swallow(e, lobj_sym("tail"), lobj_func(builtin_tail));
  lenv_swallow(e, lobj_sym("join"), lobj_func(builtin_join));
  lenv_swallow(e, lobj_sym("eval"), lobj_func(builtin_eval));

  lenv_swallow(e, lobj_sym("add"), lobj_func(builtin_add));
  lenv_swallow(e, lobj_sym("+"), lobj_func(builtin_add));
  lenv_swallow(e, lobj_sym("sub"), lobj_func(builtin_sub));
  lenv_swallow(e, lobj_sym("-"), lobj_func(builtin_sub));
  lenv_swallow(e, lobj_sym("mul"), lobj_func(builtin_mul));
  lenv_swallow(e, lobj_sym("*"), lobj_func(builtin_mul));
  lenv_swallow(e, lobj_sym("div"), lobj_func(builtin_div));
  lenv_swallow(e, lobj_sym("/"), lobj_func(builtin_div));
  lenv_swallow(e, lobj_sym("min"), lobj_func(builtin_min));
  lenv_swallow(e, lobj_sym("max"), lobj_func(builtin_max));
  lenv_swallow(e, lobj_sym("mod"), lobj_func(builtin_mod));
  lenv_swallow(e, lobj_sym("exp"), lobj_func(builtin_exp));
  lenv_swallow(e, lobj_sym("pow"), lobj_func(builtin_pow));
  lenv_swallow(e, lobj_sym("log"), lobj_func(builtin_log));
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
            "<function %s>can only bind to %s, got %s",
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

int
main(int argc, char** argv)
{
  return repl();
}
