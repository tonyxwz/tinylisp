#ifndef _TL_H_
#define _TL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mpc.h"


typedef enum
{
  LVAL_ERR,
  LVAL_NUM,
  LVAL_SYM,
  LVAL_SEXPR
} lval_type;


typedef struct lval
{
  lval_type type;
  double num; // number
  char* err; // error message
  char* sym; // symbol
  // list of other lval's contained in sexpr
  int count;
  struct lval** cell;
} lval;

lval* lval_num(double x);
lval* lval_err(char* perr);
lval* lval_sym(char* psym);
lval* lval_sexpr(void);
void lval_del(lval* v);

lval* lval_read_num(const mpc_ast_t* ast);
lval* lval_read(const mpc_ast_t* ast);
lval* lval_add(lval*, lval*);
void lval_print_expr(lval* v, char open, char close);
void lval_print(lval* v);

void repl();
lval* eval(const mpc_ast_t* ast);
lval* eval_op(const char* op, lval* a, lval* b);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _TL_H_
