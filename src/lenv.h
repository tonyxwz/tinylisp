#ifndef _LENV_H_
#define _LENV_H_

#include "lobj.h"
#include "map.h"
#ifdef __cplusplus
extern "C"
{
#endif
#ifdef DEBUG
#define GLOBAL_ENV_INIT_SIZE 200
#else
#define GLOBAL_ENV_INIT_SIZE 20000
#endif
  typedef struct lenv lenv;

  // TODO use a hash table for symbol lookup
  struct lenv
  {
    // int count;
    // char** syms;
    // lobj** objs;
    map_t* map;
    lenv* par;
  };

  lenv* lenv_new(int size);
  lenv* lenv_copy(lenv* e);
  void lenv_del(lenv* e);

  // create + update (by copy / by move)
  void lenv_create(lenv* env, lobj* k, lobj* v);
  void lenv_global_create(lenv* e, lobj* k, lobj* v);
  void lenv_add_builtin(lenv* env, char* sym, lbuiltinFunc f);
  void lenv_add_symbol(lenv* env, char* sym, lobj* val);

  // read
  lobj* lenv_read(lenv* env, lobj* k);
  // delete
  lobj* lenv_pop(lenv* env, lobj* k);
  int lenv_remove(lenv* env, lobj* k);

  void lenv_pool_init();
  void lenv_pool_del();

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _LENV_H_
