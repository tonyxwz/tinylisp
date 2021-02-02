#include "lenv.h"
#include "lobj.h"
#include <stdlib.h>
#include <string.h>

lenv*
lenv_new()
{
  lenv* env = malloc(sizeof(lenv));
  env->count = 0;
  env->syms = NULL;
  env->objs = NULL;
  return env;
}

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
}

void
lenv_put(lenv* env, lobj* k, lobj* v)
{
  for (int i = 0; i < env->count; ++i) {
    if (strcmp(env->syms[i], k->sym) == 0) {
      lobj_del(env->objs[i]);
      env->objs[i] = lobj_copy(v);
      return;
    }
  }
  env->count++;
  env->syms = realloc(env->syms, sizeof(char*) * env->count);
  env->objs = realloc(env->objs, sizeof(lobj*) * env->count);
  
  env->objs[env->count - 1] = lobj_copy(v);
  env->syms[env->count - 1] = malloc(strlen(k->sym) + 1);
  strcpy(env->syms[env->count - 1], k->sym);
}
