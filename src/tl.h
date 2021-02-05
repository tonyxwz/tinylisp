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
    OP_GE,
    OP_ADD,
    OP_SUB,
    OP_DIV,
    OP_MUL,
    OP_AND,
    OP_OR,
    OP_NOT
  } tl_operator;

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
  lobj* builtin_error(lenv* env, lobj* a);
  lobj* builtin_load(lenv* env, lobj* a);

  lobj* builtin_lambda(lenv* env, lobj* a);
  lobj* builtin_fn(lenv* env, lobj* a);
  lobj* builtin_if(lenv* env, lobj* a);

  lobj* builtin_add(lenv* env, lobj* a);
  lobj* builtin_sub(lenv* env, lobj* a);
  lobj* builtin_mul(lenv* env, lobj* a);
  lobj* builtin_div(lenv* env, lobj* a);
  lobj* builtin_mod(lenv* env, lobj* a);

  lobj* builtin_cmp(lenv* env, lobj* a, char* op);
  lobj* builtin_eq(lenv* env, lobj* a);
  lobj* builtin_ne(lenv* env, lobj* a);

  lobj* builtin_ord(lenv* env, lobj* a, char* op);
  lobj* builtin_lt(lenv* env, lobj* a);
  lobj* builtin_le(lenv* env, lobj* a);
  lobj* builtin_gt(lenv* env, lobj* a);
  lobj* builtin_ge(lenv* env, lobj* a);

  lobj* builtin_and(lenv* env, lobj* a);
  lobj* builtin_or(lenv* env, lobj* a);
  lobj* builtin_not(lenv* env, lobj* a);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _TL_H_
