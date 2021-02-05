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
  typedef enum
  {
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_LE,
    OP_GT,
    OP_GE
  } order_op;

  int repl();
  // evaluate
  lobj* eval(lenv* e, lobj* v);
  lobj* eval_sexpr(lenv* env, lobj* v);
  void print_ast();
  // lobj* builtin(lobj* v, const char* sym);

  void init_env(lenv* e);
  lobj* builtin_def(lenv* env, lobj* a);
  lobj* builtin_assign(lenv* env, lobj* a);
  lobj* builtin_dir(lenv* env, lobj* a);
  lobj* builtin_del(lenv* env, lobj* a);
  lobj* builtin_echo(lenv* env, lobj* a);
  lobj* builtin_lambda(lenv* env, lobj* a);
  lobj* builtin_fn(lenv* env, lobj* a);
  lobj* builtin_if(lenv* env, lobj* a);

  lobj* builtin_cmp(lenv* env, lobj* a, char* op);
  lobj* builtin_eq(lenv* env, lobj* a);
  lobj* builtin_ne(lenv* env, lobj* a);

  lobj* builtin_ord(lenv* env, lobj* a, char* op);
  lobj* builtin_lt(lenv* env, lobj* a);
  lobj* builtin_le(lenv* env, lobj* a);
  lobj* builtin_gt(lenv* env, lobj* a);
  lobj* builtin_ge(lenv* env, lobj* a);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _TL_H_
