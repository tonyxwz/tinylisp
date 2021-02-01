#include "tl.h"

lval*
qhead(lval* v)
{
  printf("%d\n", v->type);
  LASSERT(v, v->count == 1, "head requires 1 argument");
  LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "head takes 1 qexpr argument");
  LASSERT(v, v->cell[0]->count != 0, "head requires non-empty argument");
  lval* head = lval_take(v, 0);
  while (head->count > 1)
    lval_del(lval_pop(head, 1));
  return head;
}

lval*
qtail(lval* v)
{
  printf("%d\n", v->type);
  LASSERT(v, v->count == 1, "tail requires 1 argument");
  LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "tail takes 1 qexpr argument");
  LASSERT(v, v->cell[0]->count != 0, "tail requires non-empty argument");
  lval* tail = lval_take(v, 0);
  lval_del(lval_pop(tail, 0));
  return tail;
}

lval*
qlist(lval* v)
{
  v->type = LVAL_QEXPR;
  return v;
}

lval*
qeval(lval* v)
{
  LASSERT(v, v->count == 1, "eval requires 1 argument");
  LASSERT(v, v->cell[0]->type == LVAL_QEXPR, "eval requires qexpr");
  lval* sexpr = lval_take(v, 0);
  sexpr->type = LVAL_SEXPR;
  lval* r = eval(sexpr);
  return r;
}

lval*
lval_join(lval* a, lval* b)
{
  while (b->count) {
    lval_add(a, lval_pop(b, 0));
  }
  lval_del(b);
  return a;
}

lval*
qjoin(lval* v)
{
  for (int i = 0; i < v->count; ++i) {
    LASSERT(v, v->cell[i]->type == LVAL_QEXPR, "join requires qexpr arguments");
  }
  lval* x = lval_pop(v, 0);
  while (v->count) {
    x = lval_join(x, lval_pop(v, 0));
  }
  lval_del(v);
  return x;
}
