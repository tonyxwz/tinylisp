#include <errno.h>
#include <stdio.h>
// #include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "tl.h"
#include "lobj.h"
#include "lmath.h"
#include "qexpr.h"


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
      lobj* tmp = eval(lobj_read(r.output));
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

  mpc_cleanup(6, Number, Symbol, Sexpr, Qexpr, Expr, TL);
  return 0;
}

lobj*
eval(lobj* v)
{
  if (v->type == LOBJ_SEXPR)
    return eval_sexpr(v);
  else
    return v;
}

/* Think of func `eval` as a transformer, taking in one lobj* transform it and
 * return back.
 * lobj(sexpr) -> lobj(num/err)
 * since lobj_read always read a expression as a sexpr
 */
lobj*
eval_sexpr(lobj* v)
{
  // evaluate all the childrens
  for (int i = 0; i < v->count; ++i) {
    v->cell[i] = eval(v->cell[i]);
  }
  // error handling
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type == LOBJ_ERR)
      return lobj_take(v, i);
  }
  if (v->count == 0) // empty expression ()
    return v;
  if (v->count == 1) // empty expression (5)
    return lobj_take(v, 0);

  // operator/symbol is the first cell of sexpr
  lobj* op = lobj_pop(v, 0);
  if (op->type != LOBJ_SYM) {
    lobj_del(v);
    lobj_del(op);
    return lobj_err("sexpr must start with operator");
  }
  // v contains only nums
  lobj* result = buildin(v, op->sym); // sexpr -> num
  lobj_del(op);
  return result;
}

lobj* buildin(lobj* v, const char* sym) {
  if (strcmp(sym, "head") == 0) {
    return qhead(v);
  } else if (strcmp(sym, "tail") == 0) {
    return qtail(v);
  } else if (strcmp(sym, "list") == 0) {
    return qlist(v);
  } else if (strcmp(sym, "join") == 0) {
    return qjoin(v);
  } else if (strcmp(sym, "eval") == 0) {
    return qeval(v);
  } else if (strstr("+ - * / % ^ add sub mul div mod pow exp log min max", sym)) {
    return math_op(v, sym);
  } else {
    lobj_del(v);
    return lobj_err("Unknown symbol");
  }
}

lobj* 
math_op(lobj* v, const char* sym)
{
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type != LOBJ_NUM) {
      lobj_del(v);
      return lobj_err("Cannot operate on non-number");
    }
  }
  if (strcmp(sym, "sub") == 0 || strcmp(sym, "-") == 0) {
    return lisp_sub(v);
  } else if (strcmp(sym, "add") == 0 || strcmp(sym, "+") == 0) {
    return lisp_add(v);
  } else if (strcmp(sym, "div") == 0 || strcmp(sym, "/") == 0) {
    return lisp_div(v);
  } else if (strcmp(sym, "mul") == 0 || strcmp(sym, "*") == 0) {
    return lisp_mul(v);
  } else if (strcmp(sym, "mod") == 0 || strcmp(sym, "%") == 0) {
    return lisp_mod(v);
  } else if (strcmp(sym, "pow") == 0 || strcmp(sym, "^") == 0) {
    return lisp_pow(v);
  } else if (strcmp(sym, "exp") == 0) {
    return lisp_exp(v);
  } else if (strcmp(sym, "log") == 0) {
    return lisp_log(v);
  } else if (strcmp(sym, "min") == 0) {
    return lisp_min(v);
  } else if (strcmp(sym, "max") == 0) {
    return lisp_max(v);
  } else {
    lobj_del(v);
    return lobj_err("Unknown symbol");
  }
}

int
main(int argc, char** argv)
{
  return repl();
}
