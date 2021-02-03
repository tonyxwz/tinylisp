#ifndef _LENV_H_
#define _LENV_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "lobj.h"

  typedef struct lenv lenv;

  // TODO use a hash table for symbol lookup
  struct lenv
  {
    int count;
    char** syms;
    lobj** objs;
    lenv* par;
  };

  lenv* lenv_new(void);
  lenv* lenv_copy(lenv* e);
  void lenv_del(lenv* e);

  // create + update (by copy / by move)
  void lenv_create(lenv* env, lobj* k, lobj* v);
  void lenv_global_create(lenv* e, lobj* k, lobj* v);
  void lenv_swallow(lenv* env, lobj* k, lobj* v);

  // read
  lobj* lenv_read(lenv* env, lobj* k);
  // delete
  lobj* lenv_pop(lenv* env, lobj* k);
  lobj* lenv_remove(lenv* env, lobj* k);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _LENV_H_
