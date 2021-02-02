#ifndef _LISP_FUNC_H_
#define _LISP_FUNC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "lobj.h"
#include "lenv.h"

  // recursive
  lobj* lisp_sub(lenv* e, lobj* v);
  lobj* lisp_add(lenv* e, lobj* v);
  lobj* lisp_div(lenv* e, lobj* v);
  lobj* lisp_mul(lenv* e, lobj* v);
  lobj* lisp_min(lenv* e, lobj* v);
  lobj* lisp_max(lenv* e, lobj* v);

  lobj* lisp_rec_op(lenv* e, lobj* v, double (*op)(double, double));

  // binary
  lobj* lisp_mod(lenv* e, lobj* v);
  lobj* lisp_pow(lenv* e, lobj* v);

  // unary
  lobj* lisp_exp(lenv* e, lobj* v);
  lobj* lisp_log(lenv* e, lobj* v);

  lobj* math_op(lenv* e, lobj* v, const char* sym);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LISP_FUNC_H_