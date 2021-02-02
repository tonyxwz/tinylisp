#ifndef _QEXPR_H_
#define _QEXPR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include "tl.h"
#include "lobj.h"

  // {1 2 3} -> {1}
  lobj* qhead(lenv* e, lobj* v);

  // {1 2 3} -> {2 3}
  lobj* qtail(lenv* e, lobj* v);

  // 1 2 3 -> {1 2 3}
  lobj* qlist(lenv*e, lobj* v);

  // {1}{2}{3} -> {1 2 3}
  lobj* qjoin(lenv*e, lobj* v);

  // qexpr -> num
  lobj* qeval(lenv* e, lobj* v);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _QEXPR_H_