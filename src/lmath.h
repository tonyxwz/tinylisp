#ifndef _LISP_FUNC_H_
#define _LISP_FUNC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "lobj.h"
#include "lenv.h"

  // recursive
  lobj* builtin_sub(lenv* e, lobj* v);
  lobj* builtin_add(lenv* e, lobj* v);
  lobj* builtin_div(lenv* e, lobj* v);
  lobj* builtin_mul(lenv* e, lobj* v);
  lobj* builtin_min(lenv* e, lobj* v);
  lobj* builtin_max(lenv* e, lobj* v);

  // binary
  lobj* builtin_mod(lenv* e, lobj* v);
  lobj* builtin_pow(lenv* e, lobj* v);

  // unary
  lobj* builtin_exp(lenv* e, lobj* v);
  lobj* builtin_log(lenv* e, lobj* v);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LISP_FUNC_H_