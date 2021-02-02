#ifndef _TL_H_
#define _TL_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "mpc.h"
#include "lobj.h"
#include "lenv.h"
#include "lmath.h"
#include "sexpr.h"
#include "qexpr.h"

  // evaluate
  lobj* eval(lenv* e, lobj* v);
  lobj* eval_sexpr(lenv* env, lobj* v);
  // lobj* builtin(lobj* v, const char* sym);

  int repl();
  void init_env(lenv* e);
  lobj* builtin_def(lenv* env, lobj* a);
  lobj* builtin_dir(lenv* env, lobj* a);
  lobj* builtin_echo(lenv* env, lobj* a);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _TL_H_
