#include <errno.h>
#include <stdio.h>
// #include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "lobj.h"

lobj*
lobj_num(double x)
{
  lobj* v = (lobj*)malloc(sizeof(lobj));
  v->type = LOBJ_NUM;
  v->num = x;
  v->count = 0;
  return v;
}

lobj*
lobj_err(char* perr)
{
  lobj* v = (lobj*)malloc(sizeof(lobj));
  v->type = LOBJ_ERR;
  v->err = malloc(strlen(perr) + 1);
  strcpy(v->err, perr);
  v->count = 0;
  return v;
}

lobj*
lobj_sym(char* psym)
{
  lobj* v = (lobj*)malloc(sizeof(lobj));
  v->type = LOBJ_SYM;
  v->sym = malloc(strlen(psym) + 1);
  strcpy(v->sym, psym);
  v->count = 0;
  return v;
}

lobj*
lobj_sexpr(void)
{
  lobj* v = malloc(sizeof(lobj));
  v->type = LOBJ_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lobj*
lobj_qexpr(void)
{
  lobj* v = malloc(sizeof(lobj));
  v->type = LOBJ_QEXPR;
  v->cell = NULL;
  v->count = 0;
  return v;
}

lobj*
lobj_func(lbuiltinFunc func)
{
  lobj* v = malloc(sizeof(lobj));
  v->type = LOBJ_FUNC;
  v->func = func;
  return v;
}

void
lobj_del(lobj* v)
{
  // printf("deleted %d\n", v->type);
  switch (v->type) {
    case LOBJ_NUM:
      break;
    case LOBJ_ERR:
      free(v->err);
      break;
    case LOBJ_SYM:
      free(v->sym);
      break;
    case LOBJ_SEXPR:
    case LOBJ_QEXPR:
      for (int i = 0; i < v->count; ++i)
        lobj_del(v->cell[i]);
      free(v->cell);
      break;
    case LOBJ_FUNC:
      break;
    default:
      break;
  }
  free(v);
}

lobj*
lobj_copy(lobj* v)
{
  lobj* x = malloc(sizeof(lobj));
  x->type = v->type;

  switch (v->type) {
    case LOBJ_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err);
      break;
    case LOBJ_NUM:
      x->num = v->num;
      break;
    case LOBJ_SYM:
      x->sym = malloc(strlen(v->sym) + 1);
      strcpy(x->sym, v->sym);
      break;
    case LOBJ_SEXPR:
    case LOBJ_QEXPR:
      x->count = v->count;
      x->cell = malloc(sizeof(lobj*) * v->count);
      for (int i = 0; i < x->count; ++i)
        x->cell[i] = lobj_copy(v->cell[i]);
      break;
    case LOBJ_FUNC:
      x->func = v->func;
      break;
    default:
      break;
  }
  return x;
}

// pop and delete original
lobj*
lobj_take(lobj* sexpr, int i)
{
  // TODO no need to shift if cells are disgarded very soon
  lobj* pop = lobj_pop(sexpr, i);
  lobj_del(sexpr);
  return pop;
}

lobj*
lobj_pop(lobj* sexpr, int i)
{ // TODO should throw exception if i >= count
  lobj* pop = sexpr->cell[i];
  memmove(&sexpr->cell[i],     // popped item: sexpr->cell+i
          &sexpr->cell[i + 1], // next item: sexpr->cell+i+1
          sizeof(lobj*) *
            (sexpr->count - i - 1) // number of item left starting from i
  );
  sexpr->count--;
  sexpr->cell = realloc(sexpr->cell, sizeof(lobj*) * sexpr->count);
  return pop;
}

lobj*
lobj_read_num(const mpc_ast_t* ast)
{
  errno = 0;
  double x = strtod(ast->contents, NULL);
  if (errno == 0)
    return lobj_num(x);
  else
    return lobj_err("invalid number");
}

// regex paring is handled by mpc TODO
lobj*
lobj_read(const mpc_ast_t* ast)
{
  if (strstr(ast->tag, "number"))
    return lobj_read_num(ast);
  if (strstr(ast->tag, "symbol"))
    return lobj_sym(ast->contents);

  lobj* x = NULL;
  if (strcmp(ast->tag, ">") == 0 || // root node (everything is expr first)
      strstr(ast->tag, "sexpr"))    // sexpr
    x = lobj_sexpr();
  if (strstr(ast->tag, "qexpr"))
    x = lobj_qexpr();

  for (int i = 0; i < ast->children_num; ++i) {
    if (strcmp(ast->children[i]->contents, "(") == 0 || // sexpr branch
        strcmp(ast->children[i]->contents, ")") == 0 ||
        strcmp(ast->children[i]->contents, "{") == 0 || // qexpr branch
        strcmp(ast->children[i]->contents, "}") == 0 ||
        strcmp(ast->children[i]->tag, "regex") == 0)
      continue;
    x = lobj_append(x, lobj_read(ast->children[i]));
  }
  return x;
}

lobj*
lobj_append(lobj* dest, lobj* x)
{
  dest->count++;
  dest->cell = realloc(dest->cell, sizeof(lobj) * dest->count);
  dest->cell[dest->count - 1] = x;
  return dest;
}

void
lobj_print_expr(lobj* v, char open, char close)
{
  putchar(open);
  for (int i = 0; i < v->count; ++i) {
    lobj_print(v->cell[i]);
    if (i != v->count - 1)
      putchar(' ');
  }
  putchar(close);
}

void
lobj_print(lobj* v)
{
  switch (v->type) {
    case LOBJ_ERR:
      printf("Error: %s", v->err);
      break;
    case LOBJ_NUM:
      printf("%f", v->num);
      break;
    case LOBJ_SYM:
      printf("%s", v->sym);
      break;
    case LOBJ_SEXPR:
      lobj_print_expr(v, '(', ')');
      break;
    case LOBJ_QEXPR:
      lobj_print_expr(v, '{', '}');
      break;
    case LOBJ_FUNC:
      printf("<function>");
      break;
    default:
      break;
  }
}


void
lobj_println(lobj* v)
{
  lobj_print(v);
  putchar('\n');
}
