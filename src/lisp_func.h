#ifndef _LISP_FUNC_H_
#define _LISP_FUNC_H_
#include "tl.h"
#ifdef __cplusplus
extern "C"
{
#endif

  // recursive
  lval* lisp_sub(lval* v);
  lval* lisp_add(lval* v);
  lval* lisp_div(lval* v);
  lval* lisp_mul(lval* v);
  lval* lisp_min(lval* v);
  lval* lisp_max(lval* v);

  lval* lisp_rec_op(lval* v, double (*op)(double, double));

  // binary
  lval* lisp_mod(lval* v);
  lval* lisp_pow(lval* v);

  // unary
  lval* lisp_exp(lval* v);
  lval* lisp_log(lval* v);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LISP_FUNC_H_