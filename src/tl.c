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

lval_t
lval_num(double x)
{
  lval_t v;
  v.type = OK;
  v.val = x;
  return v;
}

lval_t
lval_err(lval_error err)
{
  lval_t v;
  v.type = ERR;
  v.err = err;
  return v;
}

void
lval_print(const lval_t* const v)
{
  switch (v->type) {
    case OK:
      printf("%f", v->val);
      break;
    case ERR:
      printf("Error: %s", lval_what_error(v->err));
      break;
    default:
      break;
  }
}

void
lval_println(const lval_t* const v)
{
  lval_print(v);
  putchar('\n');
}

const char*
lval_what_error(lval_error e)
{
  switch (e) {
    case DIV_ZERO:
      return "DIV ZERO";
      break;
    case BAD_OP:
      return "BAD OPERATOR";
      break;
    case BAD_NUM:
      return "BAD NUMBER";
      break;
    default:
      return "UNKNOWN ERROR";
      break;
  }
}

void
repl()
{
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* TL = mpc_new("tl");

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    number   : /-?([0-9]*\\.)?[0-9]+/ ; \
    operator : '+' | '-' | '*' | '/' | '%' | '^' | \
        \"mod\" | \"sub\" | \"mul\" | \"div\" | \"mod\" | \"exp\" | \"log\" | \"pow\" | \
        \"min\" | \"max\" ; \
    expr     : <number> | '(' <operator> <expr>+ ')'; \
    tl       : /^/ <operator> <expr>+ /$/; \
  ",
            Number,
            Operator,
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
      lval_t tmp = eval(r.output);
      lval_println(&tmp);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    /* printf("You've entered %s\n", input); */
    free(input);
  }

  mpc_cleanup(4, Number, Operator, Expr, TL);
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

