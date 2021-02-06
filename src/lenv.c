#include "lenv.h"
#include "lobj.h"
#include "map.h"
#include <stdlib.h>
#include <string.h>

// local private c functions
int
find_(lenv* env, lobj* k);

// ctor
lenv*
lenv_new(int size)
{
  lenv* env = malloc(sizeof(lenv));
  env->map = map_new(size);
  env->par = NULL;
  return env;
}

// dtor
void
lenv_del(lenv* env)
{
  map_free(env->map);
  free(env);
  // no need to free parent
}

// copy ctor
lenv*
lenv_copy(lenv* e)
{
  lenv* x = malloc(sizeof(lenv));
  x->map = map_copy(e->map);
  x->par = e->par;
  return x;
}

// CRUD
void
lenv_create(lenv* env, lobj* k, lobj* v)
{
  map_insert(env->map, k->str, v);
}

void
lenv_global_create(lenv* e, lobj* k, lobj* v)
{
  while (e->par)
    e = e->par;
  lenv_create(e, k, v);
}

void
lenv_add_builtin(lenv* env, char* sym, lbuiltinFunc f)
{
  lobj* key = lobj_sym(sym);
  lobj* val = lobj_func(f);
  val->constant = true;
  lenv_create(env, key, val);
  lobj_del(key);
  lobj_del(val);
}
void
lenv_add_symbol(lenv* env, char* sym, lobj* val)
{
  lobj* key = lobj_sym(sym);
  val->constant = true;
  lenv_create(env, key, val);
  lobj_del(key);
  lobj_del(val);
}

lobj*
lenv_read(lenv* env, lobj* k)
{
  lobj* obj = map_get(env->map, k->str);
  if (obj)
    return lobj_copy(obj);
  if (env->par) {
    return lenv_read(env->par, k);
  } else {
    return lobj_err("unbound symbol '%s'", k->str);
  }
}

int
lenv_remove(lenv* env, lobj* k)
{
  return map_erase(env->map, k->str);
}
