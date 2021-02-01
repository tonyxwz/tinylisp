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

lval*
lval_num(double x)
{
  lval* v = (lval*)malloc(sizeof(lval));
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval*
lval_err(char* perr)
{
  lval* v = (lval*)malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(perr) + 1);
  strcpy(v->err, perr);
  return v;
}

lval*
lval_sym(char* psym)
{
  lval* v = (lval*)malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(psym) + 1);
  strcpy(v->sym, psym);
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
  if (strcmp(ast->tag, ">") == 0) // root node
    x = lval_sexpr();
  if (strstr(ast->tag, "sexpr")) // sexpr
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

void
repl()
{
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* TL = mpc_new("tl");

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    number   : /-?([0-9]*\\.)?[0-9]+/ ; \
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

  // repl
  while (1) {
    char* input = readline("tl>>> ");
    if (input == NULL) {
      free(input);
      break;
    }
    add_history(input);
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, TL, &r)) {
      mpc_ast_print(r.output);
      lval* tmp = lval_read(r.output);
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
  /* return 0; */
}

void
print_ast(const mpc_ast_t* ast)
{
  if (!ast->children_num)
    return;
  for (int i = 0; i < ast->children_num; ++i)
    printf("%s ", ast->children[i]->contents);
  printf("\n");
}

lval_t
eval(const mpc_ast_t* ast)
{
  if (strstr(ast->tag, "number")) {
    errno = 0;
    double n = strtod(ast->contents, NULL);
    return errno != 0 ? lval_err(BAD_NUM) : lval_num(n);
  }
  // expression
  // children[0]: '(', children[-1]: ')'
  const char* op = ast->children[1]->contents; // operator
  lval_t x = eval(ast->children[2]); // first operand
  if (ast->children_num == 4) {  // single operand operator

  }

  int i = 3;
  while (strstr(ast->children[i]->tag, "expr")) {
    printf("%s\n", ast->children[i]->contents);
    x = eval_op(op, x, eval(ast->children[i]));
    ++i;
  }

  if (strcmp(op, "-") == 0 && ast->children_num == 4)
    return lval_num(-x.val);
  else
    return x;
}

lval_t
eval_op(const char* op, lval_t a, lval_t b)
{
  if(a.type == ERR) return a;
  if(b.type == ERR) return b;
  if (strcmp(op, "+") == 0 || strcmp(op, "add") == 0)
    return lval_num(a.val + b.val);
  if (strcmp(op, "-") == 0 || strcmp(op, "sub") == 0)
    return lval_num(a.val - b.val);
  if (strcmp(op, "*") == 0 || strcmp(op, "mul") == 0)
    return lval_num(a.val * b.val);
  if (strcmp(op, "/") == 0 || strcmp(op, "div") == 0) {
    if (b.val == 0)
      return lval_err(DIV_ZERO);
    else
      return lval_num(a.val / b.val);
  }
  if (strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) {
    if (b.val == 0)
      return lval_err(DIV_ZERO);
    else
      return lval_num(fmod(a.val, b.val));
  }
  if (strcmp(op, "log") == 0) {
    if (a.val > 0)
      return lval_num(log(a.val));
    else
      return lval_err(BAD_LOG_BASE);
  }
  if (strcmp(op, "exp") == 0)
    return lval_num(exp(a.val));
  if (strcmp(op, "^") == 0 || strcmp(op, "pow") == 0)
    return lval_num(pow(a.val, b.val));
  if (strcmp(op, "min") == 0)
    return lval_num(a.val > b.val ? b.val : a.val);
  if (strcmp(op, "max") == 0)
    return lval_num(a.val > b.val ? a.val : b.val);
  return lval_err(BAD_OP);
}

