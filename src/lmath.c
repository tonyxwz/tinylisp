#include "lmath.h"
#include "tl.h"
#include <math.h>


// local func declarations

// recursive
double
sub_(double a, double b)
{
  return a - b;
}

lobj*
lisp_sub(lobj* v)
{
  if (v->count == 1) {
    v->num = -v->num;
  } else {
    v = lisp_rec_op(v, sub_);
  }
  return v;
}

double
add_(double a, double b)
{
  return a + b;
}
lobj*
lisp_add(lobj* v)
{
  if (v->count > 1) {
    v = lisp_rec_op(v, add_);
  }
  return v;
}

lobj*
lisp_div(lobj* v)
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
lisp_mul(lobj* v)
{
  return lisp_rec_op(v, mul_);
}

double
min_(double a, double b)
{
  return a > b ? b : a;
}
lobj*
lisp_min(lobj* v)
{
  return lisp_rec_op(v, min_);
}

double
max_(double a, double b)
{
  return a < b ? b : a;
}
lobj*
lisp_max(lobj* v)
{
  return lisp_rec_op(v, max_);
}

lobj*
lisp_rec_op(lobj* v, double (*op)(double, double))
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
lisp_binary_op(lobj* v, double (*op)(double, double))
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
lisp_mod(lobj* v)
{
  return lisp_binary_op(v, fmod);
}
lobj*
lisp_pow(lobj* v)
{
  return lisp_binary_op(v, pow);
}

// unary
lobj*
lisp_unary_op(lobj* v, double (*op)(double))
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
lisp_exp(lobj* v)
{
  return lisp_unary_op(v, exp);
}
lobj*
lisp_log(lobj* v)
{
  return lisp_unary_op(v, log);
}