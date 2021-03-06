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
  LASSERT_ARGC("head", v, 1);

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
  LASSERT_ARGC("eval", v, 1);
  LASSERT_TYPE_I("eval", v, 0, LOBJ_QEXPR);

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
    LASSERT_TYPE_I("join", v, i, LOBJ_QEXPR);
  }
  lobj* x = lobj_pop(v, 0);
  while (v->count) {
    x = lobj_join(x, lobj_pop(v, 0));
  }
  lobj_del(v);
  return x;
}

lobj*
builtin_len(lenv* env, lobj* a)
{
  LASSERT_ARGC("len", a, 1);
  LASSERT_TYPE_I("len", a, 0, LOBJ_QEXPR);
  lobj* len = lobj_int(a->cell[0]->count);
  lobj_del(a);
  return len;
}
