#include "lenv.h"
#include "lobj.h"
#include <stdlib.h>
#include <string.h>

// local private c functions
int
find_(lenv* env, lobj* k);

// ctor
lenv*
lenv_new()
{
  lenv* env = malloc(sizeof(lenv));
  env->count = 0;
  env->syms = NULL;
  env->objs = NULL;
  env->par = NULL;
  return env;
}

// dtor
void
lenv_del(lenv* env)
{
  for (int i = 0; i < env->count; ++i) {
    free(env->syms[i]);
    lobj_del(env->objs[i]);
  }
  free(env->objs);
  free(env->syms);
  free(env);
  // no need to free parent
}

// copy ctor
lenv*
lenv_copy(lenv* e)
{
  lenv* x = lenv_new();

  x->count = e->count;
  x->par = e->par; // deepcopy not needed
  x->syms = malloc(sizeof(char*) * x->count);
  x->objs = malloc(sizeof(lobj*) * x->count);
  for (int i = 0; i < x->count; ++i) {
    x->syms[i] = malloc(strlen(e->syms[i]) + 1);
    strcpy(x->syms[i], e->syms[i]);
    x->objs[i] = lobj_copy(e->objs[i]);
  }
  return x;
}

// CRUD
void
lenv_create(lenv* env, lobj* k, lobj* v)
{
  for (int i = 0; i < env->count; ++i) {
    if (strcmp(env->syms[i], k->sym) == 0) {
      // update
      lobj_del(env->objs[i]);
      env->objs[i] = lobj_copy(v);
      return;
    }
  }
  // create
  env->count++;
  env->syms = realloc(env->syms, sizeof(char*) * env->count);
  env->objs = realloc(env->objs, sizeof(lobj*) * env->count);

  env->objs[env->count - 1] = lobj_copy(v);
  env->syms[env->count - 1] = malloc(strlen(k->sym) + 1);
  strcpy(env->syms[env->count - 1], k->sym);
}

void
lenv_global_create(lenv* e, lobj* k, lobj* v)
{
  while (e->par)
    e = e->par;
  lenv_create(e, k, v);
}

void
lenv_swallow(lenv* env, lobj* k, lobj* v)
{
  // TODO use lobj_move
  lenv_create(env, k, v);
  lobj_del(k);
  lobj_del(v);
}

lobj*
lenv_read(lenv* env, lobj* k)
{
  for (int i = 0; i < env->count; ++i) {
    if (strcmp(env->syms[i], k->sym) == 0)
      return lobj_copy(env->objs[i]);
  }
  if (env->par) {
    return lenv_read(env->par, k);
  } else {
    return lobj_err("unbound symbol '%s'", k->sym);
  }
}

lobj*
lenv_pop(lenv* env, lobj* k)
{
  int i = find_(env, k);
  if (i < 0)
    return lobj_err("unbound symbol '%s'", k->sym);
  lobj* obj = lobj_move(env->objs[i]);
  free(env->syms[i]);
  // lobj_del(env->objs[i]);
  // memmove: address of the pointers
  memmove(
    env->syms + 1, env->syms + i + 1, (env->count - i - 1) * sizeof(char*));
  memmove(
    &env->objs[i], &env->objs[i + 1], (env->count - i - 1) * sizeof(lobj*));
  env->count--;
  return obj;
}

lobj*
lenv_remove(lenv* env, lobj* k)
{
  lobj* obj = lenv_pop(env, k);
  if (obj->type == LOBJ_ERR)
    return obj;
  else {
    lobj_del(obj);
    return lobj_qexpr();
  }
}

int
find_(lenv* env, lobj* k)
{
  if (k->type != LOBJ_SYM)
    return -1;
  for (int i = 0; i < env->count; ++i) {
    if (strcmp(env->syms[i], k->sym) == 0)
      return i;
  }
  return -1;
}