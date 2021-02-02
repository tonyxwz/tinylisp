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
    lobj* x = lenv_get(env, v);
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
    return lobj_take(v, 0);

  // operator/symbol is the first cell of sexpr
  lobj* f = lobj_pop(v, 0);
  if (f->type != LOBJ_FUNC) {
    lobj_del(v);
    lobj_del(f);
    return lobj_err("first element of qexpr must be a function");
  }
  // v contains argument to f
  lobj* result = f->func(env, v); // sexpr -> num
  lobj_del(f);
  return result;
}

void
init_env(lenv* e)
{
  lenv_move(e,  lobj_sym("def"),   lobj_func(builtin_def));

  lenv_move(e,  lobj_sym("list"),  lobj_func(qlist));
  lenv_move(e,  lobj_sym("head"),  lobj_func(qhead));
  lenv_move(e,  lobj_sym("tail"),  lobj_func(qtail));
  lenv_move(e,  lobj_sym("join"),  lobj_func(qjoin));
  lenv_move(e,  lobj_sym("eval"),  lobj_func(qeval));

  lenv_move(e,  lobj_sym("add"),   lobj_func(lisp_add));
  lenv_move(e,  lobj_sym("+"),     lobj_func(lisp_add));
  lenv_move(e,  lobj_sym("sub"),   lobj_func(lisp_sub));
  lenv_move(e,  lobj_sym("-"),     lobj_func(lisp_sub));
  lenv_move(e,  lobj_sym("mul"),   lobj_func(lisp_mul));
  lenv_move(e,  lobj_sym("*"),     lobj_func(lisp_mul));
  lenv_move(e,  lobj_sym("div"),   lobj_func(lisp_div));
  lenv_move(e,  lobj_sym("/"),     lobj_func(lisp_div));
  lenv_move(e,  lobj_sym("min"),   lobj_func(lisp_min));
  lenv_move(e,  lobj_sym("max"),   lobj_func(lisp_max));
  lenv_move(e,  lobj_sym("mod"),   lobj_func(lisp_mod));
  lenv_move(e,  lobj_sym("exp"),   lobj_func(lisp_exp));
  lenv_move(e,  lobj_sym("log"),   lobj_func(lisp_log));
  lenv_move(e,  lobj_sym("log"),   lobj_func(lisp_log));

}

lobj* builtin_def(lenv* env, lobj* a) {
  LASSERT(a, a->cell[0]->type == LOBJ_QEXPR,
    "symbol must be qexpr");
  lobj * syms = a->cell[0];
  for(int i = 1; i < syms->count; ++i) {
    LASSERT(a, syms->cell[i]->type == LOBJ_SYM,
      "function def can only bind to symbols");
  }
  LASSERT(a, syms->count==a->count-1,
    "unmatching numbers of symbols as values");
  for(int i = 0; i < syms->count; ++i) {
    lenv_put(env, syms->cell[i], a->cell[i+1]);
  }
  lobj_del(a);
  return lobj_sexpr();
}


int
main(int argc, char** argv)
{
  return repl();
}
