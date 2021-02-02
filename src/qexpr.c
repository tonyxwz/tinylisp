#include "qexpr.h"
#include "tl.h"

lobj*
qhead(lenv* e, lobj* v)
{
  printf("%d\n", v->type);
  LASSERT(v, v->count == 1, "head requires 1 argument");
  LASSERT(v, v->cell[0]->type == LOBJ_QEXPR, "head takes 1 qexpr argument");
  LASSERT(v, v->cell[0]->count != 0, "head requires non-empty argument");
  lobj* head = lobj_take(v, 0);
  while (head->count > 1)
    lobj_del(lobj_pop(head, 1));
  return head;
}

lobj*
qtail(lenv* e, lobj* v)
{
  printf("%d\n", v->type);
  LASSERT(v, v->count == 1, "tail requires 1 argument");
  LASSERT(v, v->cell[0]->type == LOBJ_QEXPR, "tail takes 1 qexpr argument");
  LASSERT(v, v->cell[0]->count != 0, "tail requires non-empty argument");
  lobj* tail = lobj_take(v, 0);
  lobj_del(lobj_pop(tail, 0));
  return tail;
}

lobj*
qlist(lenv* e, lobj* v)
{
  v->type = LOBJ_QEXPR;
  return v;
}

lobj*
qeval(lenv* e, lobj* v)
{
  LASSERT(v, v->count == 1, "eval requires 1 argument");
  LASSERT(v, v->cell[0]->type == LOBJ_QEXPR, "eval requires qexpr");
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
qjoin(lenv* e, lobj* v)
{
  for (int i = 0; i < v->count; ++i) {
    LASSERT(v, v->cell[i]->type == LOBJ_QEXPR, "join requires qexpr arguments");
  }
  lobj* x = lobj_pop(v, 0);
  while (v->count) {
    x = lobj_join(x, lobj_pop(v, 0));
  }
  lobj_del(v);
  return x;
}
