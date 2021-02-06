#ifndef _LMATH_H_
#define _LMATH_H_

#include "lobj.h"
#include "lenv.h"

#ifdef __cplusplus
extern "C"
{
#endif
  // variadic
  lobj* builtin_fsub(lenv* e, lobj* v);
  lobj* builtin_fadd(lenv* e, lobj* v);
  lobj* builtin_fdiv(lenv* e, lobj* v);
  lobj* builtin_fmul(lenv* e, lobj* v);
  lobj* builtin_fmin(lenv* e, lobj* v);
  lobj* builtin_fmax(lenv* e, lobj* v);

  // binary
  lobj* builtin_fmod(lenv* e, lobj* v);
  lobj* builtin_pow(lenv* e, lobj* v);

  // unary
  lobj* builtin_exp(lenv* e, lobj* v);
  lobj* builtin_ln(lenv* e, lobj* v);

  // cmp
  lobj* builtin_feq(lenv* e, lobj* v);
  lobj* builtin_fgt(lenv* e, lobj* v);
  lobj* builtin_flt(lenv* e, lobj* v);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LMATH_H_
