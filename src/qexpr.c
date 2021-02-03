#include "qexpr.h"
#include "lobj.h"

lobj*
check_args_(lobj* v, const char* fname)
{
  LASSERT(v,
          v->count == 1,
          "<function %s> expects %d argument, got %d",
          fname,
          1,
          v->count);
  LASSERT(v,
          v->cell[0]->count != 0,
          "<function %s> requires non-empty argument",
          fname);
  return NULL;
}

lobj*
builtin_head(lenv* e, lobj* v)
{
  lobj* err = check_args_(v, "head");
  if (err)
    return err;

  lobj* head = lobj_take(v, 0);
  while (head->count > 1)
    lobj_del(lobj_pop(head, 1));
  return head;
}

lobj*
builtin_tail(lenv* e, lobj* v)
{
  lobj* err = check_args_(v, "tail");
  if (err)
    return err;

  lobj* tail = lobj_take(v, 0);
  lobj_del(lobj_pop(tail, 0));
  return tail;
}

lobj*
builtin_list(lenv* e, lobj* v)
{
  v->type = LOBJ_QEXPR;
  return v;
}

lobj*
builtin_eval(lenv* e, lobj* v)
{
  LASSERT(
    v, v->count == 1, "<function eval> requires 1 argument, got %d", v->count);
  LASSERT(v,
          v->cell[0]->type == LOBJ_QEXPR,
          "<function %s> got wrong type of argument, expecting %s, got %s",
          "eval",
          lobj_typename(LOBJ_QEXPR),
          lobj_typename(v->cell[0]->type));
  lobj* sexpr = lobj_take(v, 0);
  sexpr->type = LOBJ_SEXPR;
  lobj* r = eval(e, sexpr);
  return r;
}

lobj*
lobj_join(lobj* a, lobj* b)
{
  while (b->count) {
    lobj_append(a, lobj_pop(b, 0));
  }
  lobj_del(b);
  return a;
}

lobj*
builtin_join(lenv* e, lobj* v)
{
  for (int i = 0; i < v->count; ++i) {
    LASSERT(
      v,
      v->cell[0]->type == LOBJ_QEXPR,
      "<function %s> got wrong type for argument %d, expecting %s, got %s",
      "join",
      i + 1,
      lobj_typename(LOBJ_QEXPR),
      lobj_typename(v->cell[0]->type));
  }
  lobj* x = lobj_pop(v, 0);
  while (v->count) {
    x = lobj_join(x, lobj_pop(v, 0));
  }
  lobj_del(v);
  return x;
}
