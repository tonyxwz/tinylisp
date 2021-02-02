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
  lobj* qhead(lobj* v);

  // {1 2 3} -> {2 3}
  lobj* qtail(lobj* v);

  // 1 2 3 -> {1 2 3}
  lobj* qlist(lobj* v);

  // {1}{2}{3} -> {1 2 3}
  lobj* qjoin(lobj* v);

  // qexpr -> num
  lobj* qeval(lobj* v);




#ifdef __cplusplus
} // extern "C"
#endif

#endif // _QEXPR_H_