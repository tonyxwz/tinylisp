#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <readline/history.h>
#include <readline/readline.h>
#include "tl.h"
#include "lisp_func.h"

lval*
lval_num(double x)
{
  lval* v = (lval*)malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  v->count = 0;
  return v;
}

lval*
lval_err(char* perr)
{
  lval* v = (lval*)malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(perr) + 1);
  strcpy(v->err, perr);
  v->count = 0;
  return v;
}

lval*
lval_sym(char* psym)
{
  lval* v = (lval*)malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(psym) + 1);
  strcpy(v->sym, psym);
  v->count = 0;
  return v;
}

lval*
lval_sexpr(void)
{
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

void
lval_del(lval* v)
{
  // printf("deleted %d\n", v->type);
  switch (v->type) {
    case LVAL_NUM:
      break;
    case LVAL_ERR:
      free(v->err);
      break;
    case LVAL_SYM:
      free(v->sym);
      break;
    case LVAL_SEXPR:
      for (int i = 0; i < v->count; ++i)
        lval_del(v->cell[i]);
      free(v->cell);
      break;
  }
  free(v);
}

// pop and delete original
lval*
lval_take(lval* sexpr, int i)
{
  // TODO no need to shift if cells are disgarded very soon
  lval* pop = lval_pop(sexpr, i);
  lval_del(sexpr);
  return pop;
}

lval*
lval_pop(lval* sexpr, int i)
{ // TODO should throw exception if i >= count
  lval* pop = sexpr->cell[i];
  memmove(&sexpr->cell[i],     // popped item: sexpr->cell+i
          &sexpr->cell[i + 1], // next item: sexpr->cell+i+1
          sizeof(lval*) *
            (sexpr->count - i - 1) // number of item left starting from i
  );
  sexpr->count--;
  sexpr->cell = realloc(sexpr->cell, sizeof(lval*) * sexpr->count);
  return pop;
}

lval*
lval_read_num(const mpc_ast_t* ast)
{
  errno = 0;
  double x = strtod(ast->contents, NULL);
  if (errno == 0)
    return lval_num(x);
  else
    return lval_err("invalid number");
}

lval*
lval_read(const mpc_ast_t* ast)
{
  if (strstr(ast->tag, "number"))
    return lval_read_num(ast);
  if (strstr(ast->tag, "symbol"))
    return lval_sym(ast->contents);

  lval* x = NULL;
  if (strcmp(ast->tag, ">") == 0 || // root node (everything is expr first)
      strstr(ast->tag, "sexpr"))    // sexpr
    x = lval_sexpr();

  for (int i = 0; i < ast->children_num; ++i) {
    if (strcmp(ast->children[i]->contents, "(") == 0 ||
        strcmp(ast->children[i]->contents, ")") == 0 ||
        strcmp(ast->children[i]->tag, "regex") == 0)
      continue;
    x = lval_add(x, lval_read(ast->children[i]));
  }
  return x;
}

lval*
lval_add(lval* dest, lval* x)
{
  dest->count++;
  dest->cell = realloc(dest->cell, sizeof(lval) * dest->count);
  dest->cell[dest->count - 1] = x;
  return dest;
}

void
lval_print_expr(lval* v, char open, char close)
{
  putchar(open);
  for (int i = 0; i < v->count; ++i) {
    lval_print(v->cell[i]);
    if (i != v->count - 1)
      putchar(' ');
  }
  putchar(close);
}

void
lval_print(lval* v)
{
  switch (v->type) {
    case LVAL_ERR:
      printf("Error: %s", v->err);
      break;
    case LVAL_NUM:
      printf("%f", v->num);
      break;
    case LVAL_SYM:
      printf("%s", v->sym);
      break;
    case LVAL_SEXPR:
      lval_print_expr(v, '(', ')');
      break;
    default:
      break;
  }
}

void
lval_println(lval* v)
{
  lval_print(v);
  putchar('\n');
}

int
repl()
{
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* TL = mpc_new("tl");

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    number   : /(\\b|\\B)-?([0-9]*\\.)?[0-9]+(\\B|\\b)/ ; \
    symbol: '+' | '-' | '*' | '/' | '%' | '^' | \
        \"mod\" | \"sub\" | \"mul\" | \"div\" | \"mod\" | \"exp\" | \"log\" | \"pow\" | \
        \"min\" | \"max\" ; \
    sexpr: '(' <expr>* ')' ;\
    expr     : <number> | <symbol> | <sexpr> ; \
    tl       : /^/ <expr>* /$/; \
  ",
            Number,
            Symbol,
            Sexpr,
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
      // mpc_ast_print(r.output);
      lval* tmp = eval(lval_read(r.output));
      lval_println(tmp);
      lval_del(tmp);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    /* printf("You've entered %s\n", input); */
    free(input);
  }

  mpc_cleanup(5, Number, Symbol, Sexpr, Expr, TL);
  return 0;
}

lval*
eval(lval* v)
{
  if (v->type == LVAL_SEXPR)
    return eval_sexpr(v);
  else
    return v;
}

/* Think of func `eval` as a transformer, taking in one lval* transform it and
 * return back.
 * lval(sexpr) -> lval(num/err)
 * since lval_read always read a expression as a sexpr
 */
lval*
eval_sexpr(lval* v)
{
  // evaluate all the childrens
  for (int i = 0; i < v->count; ++i) {
    v->cell[i] = eval(v->cell[i]);
  }
  // error handling
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type == LVAL_ERR)
      return lval_take(v, i);
  }
  if (v->count == 0) // empty expression ()
    return v;
  if (v->count == 1) // empty expression (5)
    return lval_take(v, 0);

  // operator/symbol is the first cell of sexpr
  lval* op = lval_pop(v, 0);
  if (op->type != LVAL_SYM) {
    lval_del(v);
    lval_del(op);
    return lval_err("sexpr must start with operator");
  }
  // v contains only nums
  lval* result = buildin_op(v, op->sym); // sexpr -> num
  lval_del(op);
  return result;
}

lval*
buildin_op(lval* v, const char* sym)
{
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type != LVAL_NUM) {
      lval_del(v);
      return lval_err("Cannot operate on non-number");
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
    lval_del(v);
    return lval_err("Unknown symbol");
  }
}
