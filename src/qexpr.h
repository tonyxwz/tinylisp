#ifndef _QEXPR_H_
#define _QEXPR_H_


#include <string.h>
#include "lobj.h"

#ifdef __cplusplus
extern "C"
{
#endif
  // {1 2 3} -> {1}
  lobj* builtin_head(lenv* e, lobj* v);

  // {1 2 3} -> {2 3}
  lobj* builtin_tail(lenv* e, lobj* v);

  // 1 2 3 -> {1 2 3}
  lobj* builtin_list(lenv* e, lobj* v);

  // {1}{2}{3} -> {1 2 3}
  lobj* builtin_join(lenv* e, lobj* v);

  // qexpr -> num
  lobj* builtin_eval(lenv* e, lobj* v);

  // qexpr -> length
  lobj* builtin_len(lenv* env, lobj* a);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _QEXPR_H_
