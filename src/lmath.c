#include "lmath.h"
#include "lobj.h"
#include <math.h>


// local func declarations

// recursive
double
sub_(double a, double b)
{
  return a - b;
}

lobj*
lisp_sub(lenv*e, lobj* v)
{
  if (v->count == 1) {
    v->num = -v->num;
  } else {
    v = lisp_rec_op(e, v, sub_);
  }
  return v;
}

double
add_(double a, double b)
{
  return a + b;
}
lobj*
lisp_add(lenv* e, lobj* v)
{
  if (v->count > 1) {
    v = lisp_rec_op(e, v, add_);
  }
  return v;
}

lobj*
lisp_div(lenv* e, lobj* v)
{
  lobj* x;
  if (v->count >= 2) {
    x = lobj_pop(v, 0);
    while (v->count > 0) {
      lobj* y = lobj_pop(v, 0);
      if (y->num == 0) {
        lobj_del(x);
        lobj_del(y);
        x = lobj_err("Division by zero");
        break;
      }
      x->num = x->num / y->num;
      lobj_del(y);
    }
  } else {
    x = lobj_err("Wrong number of arguments");
  }
  lobj_del(v);
  return x;
}

double
mul_(double a, double b)
{
  return a * b;
}
lobj*
lisp_mul(lenv* e, lobj* v)
{
  return lisp_rec_op(e, v, mul_);
}

double
min_(double a, double b)
{
  return a > b ? b : a;
}
lobj*
lisp_min(lenv* e, lobj* v)
{
  return lisp_rec_op(e, v, min_);
}

double
max_(double a, double b)
{
  return a < b ? b : a;
}
lobj*
lisp_max(lenv* e, lobj* v)
{
  return lisp_rec_op(e, v, max_);
}

lobj*
lisp_rec_op(lenv* e, lobj* v, double (*op)(double, double))
{
  lobj* x;
  if (v->count >= 2) {
    x = lobj_pop(v, 0);
    while (v->count > 0) {
      lobj* y = lobj_pop(v, 0);
      x->num = op(x->num, y->num);
      lobj_del(y);
    }
  } else {
    x = lobj_err("Wrong number of arguments");
  }
  lobj_del(v);
  return x;
}

// binary
lobj*
lisp_binary_op(lenv* e, lobj* v, double (*op)(double, double))
{
  lobj* ret = NULL;
  if (v->count == 2) {
    ret = lobj_num(op(v->cell[0]->num, v->cell[1]->num));
  } else {
    ret = lobj_err("Operator expects two arguments");
  }
  lobj_del(v);
  return ret;
}
lobj*
lisp_mod(lenv* e, lobj* v)
{
  return lisp_binary_op(e, v, fmod);
}
lobj*
lisp_pow(lenv* e, lobj* v)
{
  return lisp_binary_op(e, v, pow);
}

// unary
lobj*
lisp_unary_op(lenv* e, lobj* v, double (*op)(double))
{
  lobj* ret = NULL;
  if (v->count == 1) {
    ret = lobj_num(op(v->cell[0]->num));
  } else {
    ret = lobj_err("Operator expects two arguments");
  }
  lobj_del(v);
  return ret;
}
lobj*
lisp_exp(lenv* e, lobj* v)
{
  return lisp_unary_op(e, v, exp);
}
lobj*
lisp_log(lenv* e, lobj* v)
{
  return lisp_unary_op(e, v, log);
}

lobj*
math_op(lenv*e, lobj* v, const char* sym)
{
  for (int i = 0; i < v->count; ++i) {
    if (v->cell[i]->type != LOBJ_NUM) {
      lobj_del(v);
      return lobj_err("Cannot operate on non-number");
    }
  }
  if (strcmp(sym, "sub") == 0 || strcmp(sym, "-") == 0) {
    return lisp_sub(e, v);
  } else if (strcmp(sym, "add") == 0 || strcmp(sym, "+") == 0) {
    return lisp_add(e, v);
  } else if (strcmp(sym, "div") == 0 || strcmp(sym, "/") == 0) {
    return lisp_div(e, v);
  } else if (strcmp(sym, "mul") == 0 || strcmp(sym, "*") == 0) {
    return lisp_mul(e, v);
  } else if (strcmp(sym, "mod") == 0 || strcmp(sym, "%") == 0) {
    return lisp_mod(e, v);
  } else if (strcmp(sym, "pow") == 0 || strcmp(sym, "^") == 0) {
    return lisp_pow(e, v);
  } else if (strcmp(sym, "exp") == 0) {
    return lisp_exp(e, v);
  } else if (strcmp(sym, "log") == 0) {
    return lisp_log(e, v);
  } else if (strcmp(sym, "min") == 0) {
    return lisp_min(e, v);
  } else if (strcmp(sym, "max") == 0) {
    return lisp_max(e, v);
  } else {
    lobj_del(v);
    return lobj_err("Unknown symbol");
  }
}
