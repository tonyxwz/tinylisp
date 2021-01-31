#ifndef _TL_H_
#define _TL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mpc.h"


typedef enum
{
  OK,
  ERR
} lval_type;
typedef enum
{
  DIV_ZERO,
  BAD_OP,
  BAD_NUM,
  BAD_LOG_BASE
} lval_error;

typedef struct
{
  lval_type type;
  lval_error err;
  double val;
} lval_t;

lval_t        lval_num(double x);
lval_t        lval_err(lval_error err);
const char*   lval_what_error(lval_error e);
void          lval_print(const lval_t* v);
void          repl();
void          print_ast(const mpc_ast_t* ast);
lval_t        eval(const mpc_ast_t* ast);
lval_t        eval_op(const char* op, lval_t a, lval_t b);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _TL_H_
