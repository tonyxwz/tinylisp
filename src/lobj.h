#ifndef _LOBJ_H_
#define _LOBJ_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "mpc.h"
// TODO Improve macros
// ASSERT_OR_CLEAN ...->##__VA_ARGS__
#define LASSERT(args, cond, emsg, ...)                                         \
  if (!(cond)) {                                                               \
    lobj* err = lobj_err(emsg, ##__VA_ARGS__);                                 \
    lobj_del(args);                                                            \
    return err;                                                                \
  }
#define LASSERT_ARGC(fname, args, expect)                                      \
  LASSERT(args,                                                                \
          args->count == expect,                                               \
          "<function %s> wrong number of arguments, expecting %d, got %d",     \
          fname,                                                               \
          expect,                                                              \
          args->count);

#define LASSERT_TYPE(fname, obj, expect)                                       \
  LASSERT(obj,                                                                 \
          obj->type == expect,                                                 \
          "<function %s> wrong argument types, expecting %s, got %s",          \
          fname,                                                               \
          lobj_typename(expect),                                               \
          lobj_typename(obj->type))

// warning, use after ASSERT_COUNT
#define LASSERT_TYPE_I(fname, args, index, expect)                             \
  LASSERT(args,                                                                \
          args->cell[index]->type == expect,                                   \
          "<function %s> got %s at argument %d, expecting %s",                 \
          fname,                                                               \
          lobj_typename(args->cell[index]->type),                              \
          index + 1,                                                           \
          lobj_typename(expect))

  typedef enum
  {
    LOBJ_ERR,
    LOBJ_DOUBLE,
    LOBJ_INT,
    LOBJ_SYM,
    LOBJ_STR,
    LOBJ_SEXPR,
    LOBJ_QEXPR,
    LOBJ_FUNC
  } lobj_type;

  typedef struct lobj lobj;
  typedef struct lenv lenv;
  typedef lobj* (*lbuiltinFunc)(lenv*, lobj*);

  struct lobj
  {
    lobj_type type;
    // number
    double d;
    int i;
    bool constant;
    // error message
    char* err;
    // symbol
    char* sym;
    char* str;
    // func
    lbuiltinFunc builtin;
    lenv* env;
    lobj* formals; // func args: qexpr
    lobj* body;    // func body: qexpr

    // expressions
    int count;
    // list of other lobj's contained in expr
    struct lobj** cell;
  };

  // ctor dtor
  lobj* lobj_double(double x);
  lobj* lobj_int(int x);
  lobj* lobj_err(char* fmt, ...);
  lobj* lobj_sym(char* psym);
  lobj* lobj_str(char* str);
  lobj* lobj_sexpr(void);
  lobj* lobj_qexpr(void);
  lobj* lobj_func(lbuiltinFunc func);
  lobj* lobj_lambda(lobj* formals, lobj* body);

  void lobj_del(lobj* v);
  lobj* lobj_take(lobj* sexpr, int i);
  lobj* lobj_pop(lobj* sexpr, int i);
  lobj* lobj_copy(lobj* v);
  lobj* lobj_move(lobj* v);

  // lexing and parsing
  lobj* lobj_read_num(mpc_ast_t* ast);
  lobj* lobj_read_str(mpc_ast_t* ast);
  lobj* lobj_read(mpc_ast_t* ast);
  lobj* lobj_append(lobj*, lobj*);
  void lobj_print_expr(lobj* v, char open, char close);
  void lobj_print(lobj* v);
  void lobj_println(lobj* v);
  char* lobj_typename(lobj_type t);

  lobj* lobj_call(lobj* f, lobj* args, lenv* env);
  int lobj_eq(lobj* a, lobj* b);

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LOBJ_H_
