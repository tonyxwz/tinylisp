#include "lisp_func.h"
#include "tl.h"
#include <math.h>

// local func declarations

// recursive
double
sub_(double a, double b)
{
  return a - b;
}

lval*
lisp_sub(lval* v)
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
lval*
lisp_add(lval* v)
{
  if (v->count > 1) {
    v = lisp_rec_op(v, add_);
  }
  return v;
}

lval*
lisp_div(lval* v)
{
  lval* x;
  if (v->count >= 2) {
    x = lval_pop(v, 0);
    while (v->count > 0) {
      lval* y = lval_pop(v, 0);
      if (y->num == 0) {
        lval_del(x);
        lval_del(y);
        x = lval_err("Division by zero");
        break;
      }
      x->num = x->num / y->num;
      lval_del(y);
    }
  } else {
    x = lval_err("Wrong number of arguments");
  }
  lval_del(v);
  return x;
}

double
mul_(double a, double b)
{
  return a * b;
}
lval*
lisp_mul(lval* v)
{
  return lisp_rec_op(v, mul_);
}

double
min_(double a, double b)
{
  return a > b ? b : a;
}
lval*
lisp_min(lval* v)
{
  return lisp_rec_op(v, min_);
}

double
max_(double a, double b)
{
  return a < b ? b : a;
}
lval*
lisp_max(lval* v)
{
  return lisp_rec_op(v, max_);
}

lval*
lisp_rec_op(lval* v, double (*op)(double, double))
{
  lval* x;
  if (v->count >= 2) {
    x = lval_pop(v, 0);
    while (v->count > 0) {
      lval* y = lval_pop(v, 0);
      x->num = op(x->num, y->num);
      lval_del(y);
    }
  } else {
    x = lval_err("Wrong number of arguments");
  }
  lval_del(v);
  return x;
}

// binary
lval*
lisp_binary_op(lval* v, double (*op)(double, double))
{
  lval* ret = NULL;
  if (v->count == 2) {
    ret = lval_num(op(v->cell[0]->num, v->cell[1]->num));
  } else {
    ret = lval_err("Operator expects two arguments");
  }
  lval_del(v);
  return ret;
}
lval*
lisp_mod(lval* v)
{
  return lisp_binary_op(v, fmod);
}
lval*
lisp_pow(lval* v)
{
  return lisp_binary_op(v, pow);
}

// unary
lval*
lisp_unary_op(lval* v, double (*op)(double))
{
  lval* ret = NULL;
  if (v->count == 1) {
    ret = lval_num(op(v->cell[0]->num));
  } else {
    ret = lval_err("Operator expects two arguments");
  }
  lval_del(v);
  return ret;
}
lval*
lisp_exp(lval* v)
{
  return lisp_unary_op(v, exp);
}
lval*
lisp_log(lval* v)
{
  return lisp_unary_op(v, log);
}