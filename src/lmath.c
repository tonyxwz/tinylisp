#define _XOPEN_SOURCE 500
#include "mpc.h"
#include <float.h>
#include <math.h>

#include "lmath.h"
#include "lobj.h"

lobj*
check_args(lobj* v, const char* fname)
{
  for (int i = 0; i < v->count; ++i) {
    LASSERT_TYPE_I(fname, v, i, LOBJ_DOUBLE)
  }
  return NULL; // OK
}

// recursive TODO use macros, add integer
lobj*
recursive_op_(lenv* e, lobj* v, double (*op)(double, double), const char* fname)
{
  lobj* err = check_args(v, fname);
  if (err)
    return err;
  lobj* x;
  if (v->count >= 2) {
    x = lobj_pop(v, 0);
    while (v->count > 0) {
      lobj* y = lobj_pop(v, 0);
      x->d = op(x->d, y->d);
      lobj_del(y);
    }
  } else {
    x = lobj_err("<function %s> expecting %s, got %d", fname, "> 2", v->count);
  }
  lobj_del(v);
  return x;
}

double
sub_(double a, double b)
{
  return a - b;
}

lobj*
builtin_fsub(lenv* e, lobj* v)
{
  lobj* err = check_args(v, "sub");
  if (err)
    return err;
  if (v->count == 1) {
    v->d = -v->d;
  } else {
    v = recursive_op_(e, v, sub_, "sub");
  }
  return v;
}

double
add_(double a, double b)
{
  return a + b;
}
lobj*
builtin_fadd(lenv* e, lobj* v)
{
  lobj* err = check_args(v, "add");
  if (err)
    return err;
  if (v->count != 1) {
    v = recursive_op_(e, v, add_, "add");
  }
  return v;
}

lobj*
builtin_fdiv(lenv* e, lobj* v)
{
  lobj* err = check_args(v, "div");
  if (err)
    return err;
  lobj* x;
  if (v->count >= 2) {
    x = lobj_pop(v, 0);
    while (v->count > 0) {
      lobj* y = lobj_pop(v, 0);
      if (y->d == 0) {
        lobj_del(x);
        lobj_del(y);
        x = lobj_err("<function %s> %s", "div", "division by zero");
        break;
      }
      x->d = x->d / y->d;
      lobj_del(y);
    }
  } else {
    x = lobj_err(
      "<function %s>, expecting %s arguments, got %d", "div", "> 2", v->count);
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
builtin_fmul(lenv* e, lobj* v)
{
  return recursive_op_(e, v, mul_, "mul");
}

double
min_(double a, double b)
{
  return a > b ? b : a;
}
lobj*
builtin_fmin(lenv* e, lobj* v)
{
  return recursive_op_(e, v, min_, "min");
}

double
max_(double a, double b)
{
  return a < b ? b : a;
}
lobj*
builtin_fmax(lenv* e, lobj* v)
{
  return recursive_op_(e, v, max_, "max");
}

// binary
lobj*
binary_op_(lenv* e, lobj* v, double (*op)(double, double), const char* fname)
{
  lobj* err = check_args(v, fname);
  if (err)
    return err;
  lobj* ret = NULL;
  if (v->count == 2) {
    ret = lobj_double(op(v->cell[0]->d, v->cell[1]->d));
  } else {
    ret =
      lobj_err("<function %s> expects two arguments, got %d", fname, v->count);
  }
  lobj_del(v);
  return ret;
}

lobj*
builtin_fmod(lenv* e, lobj* v)
{
  return binary_op_(e, v, fmod, "mod");
}

lobj*
builtin_pow(lenv* e, lobj* v)
{
  return binary_op_(e, v, pow, "pow");
}

// unary
lobj*
unary_op_(lenv* e, lobj* v, double (*op)(double), const char* fname)
{
  lobj* err = check_args(v, "div");
  if (err)
    return err;

  lobj* ret = NULL;
  if (v->count == 1) {
    ret = lobj_double(op(v->cell[0]->d));
  } else {
    ret =
      lobj_err("<function %s> expects one arguments, got %d", fname, v->count);
  }
  lobj_del(v);
  return ret;
}
lobj*
builtin_exp(lenv* e, lobj* v)
{
  return unary_op_(e, v, exp, "exp");
}
lobj*
builtin_ln(lenv* e, lobj* v)
{
  return unary_op_(e, v, log, "ln");
}

// cmp
lobj*
builtin_feq(lenv* e, lobj* v)
{
  LASSERT_ARGC("float ==", v, 2);
  LASSERT_TYPE_I("float ==", v, 0, LOBJ_DOUBLE);
  LASSERT_TYPE_I("float ==", v, 1, LOBJ_DOUBLE);
  lobj* ans = lobj_int(fabs(v->cell[0]->d - v->cell[0]->d) < DBL_EPSILON);
  lobj_del(v);
  return ans;
}
lobj*
builtin_fgt(lenv* e, lobj* v)
{
  LASSERT_ARGC("float >", v, 2);
  LASSERT_TYPE_I("float >", v, 0, LOBJ_DOUBLE);
  LASSERT_TYPE_I("float >", v, 1, LOBJ_DOUBLE);
  lobj* ans = lobj_int(v->cell[0]->d > v->cell[1]->d);
  lobj_del(v);
  return ans;
}
lobj*
builtin_flt(lenv* e, lobj* v)
{
  LASSERT_ARGC("float <", v, 2);
  LASSERT_TYPE_I("float <", v, 0, LOBJ_DOUBLE);
  LASSERT_TYPE_I("float <", v, 1, LOBJ_DOUBLE);
  lobj* ans = lobj_int(v->cell[0]->d < v->cell[1]->d);
  lobj_del(v);
  return ans;
}
