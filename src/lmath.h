#ifndef _LISP_FUNC_H_
#define _LISP_FUNC_H_
#include "tl.h"
#ifdef __cplusplus
extern "C"
{
#endif

  // recursive
  lobj* lisp_sub(lobj* v);
  lobj* lisp_add(lobj* v);
  lobj* lisp_div(lobj* v);
  lobj* lisp_mul(lobj* v);
  lobj* lisp_min(lobj* v);
  lobj* lisp_max(lobj* v);

  lobj* lisp_rec_op(lobj* v, double (*op)(double, double));

  // binary
  lobj* lisp_mod(lobj* v);
  lobj* lisp_pow(lobj* v);

  // unary
  lobj* lisp_exp(lobj* v);
  lobj* lisp_log(lobj* v);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LISP_FUNC_H_