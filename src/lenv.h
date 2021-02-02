#ifndef _LENV_H_
#define _LENV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lobj.h"

typedef struct lenv lenv;

// TODO use a hash table for symbol lookup
struct lenv {
  int count;
  char** syms;
  lobj** objs;
};

lenv* lenv_new(void);
void lenv_del(lenv* e);

void lenv_put(lenv* env, lobj* k, lobj* v);
lobj* lenv_get(lenv* env, lobj* k);

void lenv_move(lenv* env, lobj* k ,lobj* v);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _LENV_H_
