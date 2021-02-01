#ifndef _QEXPR_H_
#define _QEXPR_H_

#include <string.h>
#include "tl.h"

// {1 2 3} -> {1}
lval*
qhead(lval* v);

// {1 2 3} -> {2 3}
lval*
qtail(lval* v);

// 1 2 3 -> {1 2 3}
lval*
qlist(lval* v);

// {1}{2}{3} -> {1 2 3}
lval*
qjoin(lval* v);

// qexpr -> num
lval*
qeval(lval* v);


#endif