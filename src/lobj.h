#ifndef _LOBJ_H_
#define _LOBJ_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "mpc.h"

#define LASSERT(args, cond, emsg)                                              \
  if (!(cond)) {                                                               \
    lobj_del(args);                                                            \
    return lobj_err(emsg);                                                     \
  }

  typedef enum
  {
    LOBJ_ERR,
    LOBJ_NUM,
    LOBJ_SYM,
    LOBJ_SEXPR,
    LOBJ_QEXPR,
    LOBJ_FUNC
  } lobj_type;


  // lobj->lbuiltinFunc
  // lbuiltinFunc -> lobj, lenv
  // lenv -> lobj
  typedef struct lobj lobj;
  typedef struct lenv lenv;
  typedef lobj* (*lbuiltinFunc)(lenv*, lobj*);

  struct lobj
  {
    lobj_type type;
    double num;        // number
    char* err;         // error message
    char* sym;         // symbol
    lbuiltinFunc func; // func
    int count;
    struct lobj** cell; // list of other lobj's contained in sexpr
  };

  // ctor dtor
  lobj*
  lobj_num(double x);
  lobj* lobj_err(char* perr);
  lobj* lobj_sym(char* psym);
  lobj* lobj_sexpr(void);
  lobj* lobj_qexpr(void);
  lobj* lobj_func(lbuiltinFunc func);

  void lobj_del(lobj* v);
  lobj* lobj_take(lobj* sexpr, int i);
  lobj* lobj_pop(lobj* sexpr, int i);
  lobj* lobj_copy(lobj* v);

  // lexing and parsing
  lobj* lobj_read_num(const mpc_ast_t* ast);
  lobj* lobj_read(const mpc_ast_t* ast);
  lobj* lobj_append(lobj*, lobj*);
  void lobj_print_expr(lobj* v, char open, char close);
  void lobj_print(lobj* v);
  void lobj_println(lobj* v);


#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _LOBJ_H_
