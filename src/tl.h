#ifndef _TL_H_
#define _TL_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mpc.h"
#include "lobj.h"


// evaluate
lobj* eval_sexpr(lobj* v);
lobj* eval(lobj* v);
lobj* buildin(lobj* v, const char* sym);
lobj* math_op(lobj* v, const char* sym);

int repl();

#ifdef __cplusplus
} // extern "C"
#endif
#endif // define _TL_H_
