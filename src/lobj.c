#include "lobj.h"
#include "lenv.h"
#include "qexpr.h"
#include <stdio.h>
#include <stdlib.h>

#define ERR_STRING_LEN 512

lobj*
lobj_common_init()
{
  lobj* v = (lobj*)malloc(sizeof(lobj));
  v->i = 0;
  v->d = 0;
  v->constant = false;
  v->err = NULL;
  v->sym = NULL;
  v->env = NULL;
  v->builtin = NULL;
  v->formals = NULL;
  v->body = NULL;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lobj*
lobj_double(double x)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_DOUBLE;
  v->d = x;
  return v;
}

lobj*
lobj_int(int x)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_INT;
  v->i = x;
  return v;
}
lobj*
lobj_err(char* fmt, ...)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_ERR;

  va_list va;
  va_start(va, fmt);

  v->err = malloc(ERR_STRING_LEN);
  vsnprintf(v->err, ERR_STRING_LEN - 1, fmt, va);

  v->err = realloc(v->err, strlen(v->err) + 1);
  va_end(va);

  return v;
}

lobj*
lobj_sym(char* psym)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_SYM;
  v->sym = malloc(strlen(psym) + 1);
  strcpy(v->sym, psym);
  return v;
}

lobj*
lobj_sexpr(void)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_SEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lobj*
lobj_qexpr(void)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_QEXPR;
  v->count = 0;
  v->cell = NULL;
  return v;
}

lobj*
lobj_func(lbuiltinFunc func)
{
  lobj* v = lobj_common_init();
  v->type = LOBJ_FUNC;
  v->builtin = func;
  return v;
}

lobj*
lobj_lambda(lobj* formals, lobj* body)
{
  // qexpr, qexpr
  lobj* v = lobj_common_init();
  v->type = LOBJ_FUNC;
  v->builtin = NULL;
  v->env = lenv_new();
  v->formals = formals;
  v->body = body;
  return v;
}

void
lobj_del(lobj* v)
{
  // printf("deleted %d\n", v->type);
  switch (v->type) {
    case LOBJ_DOUBLE:
    case LOBJ_INT:
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
      // user defined
      if (!v->builtin) {
        lobj_del(v->formals);
        lobj_del(v->body);
        lenv_del(v->env);
      }
      break;
    default:
      break;
  }
  free(v);
}

lobj*
lobj_copy(lobj* v)
{
  lobj* x = lobj_common_init();
  x->type = v->type;

  switch (v->type) {
    case LOBJ_ERR:
      x->err = malloc(strlen(v->err) + 1);
      strcpy(x->err, v->err);
      break;
    case LOBJ_DOUBLE:
      x->d = v->d;
      break;
    case LOBJ_INT:
      x->i = v->i;
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
      if (v->builtin) {
        x->builtin = v->builtin;
      } else {
        x->formals = lobj_copy(v->formals);
        x->body = lobj_copy(v->body);
        x->env = lenv_copy(v->env);
      }
      break;
    default:
      break;
  }
  return x;
}

lobj*
lobj_move(lobj* v)
{
  lobj* x = lobj_common_init();
  x->type = v->type;

  switch (v->type) {
    case LOBJ_ERR:
      x->err = v->err;
      v->err = NULL;
      break;
    case LOBJ_DOUBLE:
      x->d = v->d;
      break;
    case LOBJ_INT:
      x->i = v->i;
      break;
    case LOBJ_SYM:
      x->sym = v->sym;
      v->sym = NULL;
      break;
    case LOBJ_SEXPR:
    case LOBJ_QEXPR:
      x->count = v->count;
      x->cell = v->cell;
      v->count = 0;
      v->cell = NULL;
      break;
    case LOBJ_FUNC:
      x->builtin = v->builtin;
      break;
    default:
      break;
  }
  lobj_del(v);
  return x;
}

// pop and delete original
lobj*
lobj_take(lobj* expr, int i)
{
  // TODO no need to shift if cells are disgarded very soon
  lobj* pop = lobj_pop(expr, i);
  lobj_del(expr);
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
    return lobj_double(x);
  else
    return lobj_err("invalid number");
}

// regex paring is handled by mpc TODO
lobj*
lobj_read(const mpc_ast_t* ast)
{
  if (strstr(ast->tag, "double"))
    return lobj_read_num(ast);
  if (strstr(ast->tag, "integer"))
    return lobj_int(atoi(ast->contents));
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
    if (i != v->count - 1) // last member
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
    case LOBJ_DOUBLE:
      printf("%f", v->d);
      break;
    case LOBJ_INT:
      printf("%d", v->i);
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
      if (v->builtin) {
        printf("<builtin>");
      } else {
        printf("(\\ ");
        lobj_print(v->formals);
        putchar(' ');
        lobj_print(v->body);
        putchar(')');
      }
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

char*
lobj_typename(lobj_type t)
{
  switch (t) {
    case LOBJ_DOUBLE:
      return "Double";
    case LOBJ_INT:
      return "Integer";
    case LOBJ_ERR:
      return "Error";
    case LOBJ_SYM:
      return "Symbol";
    case LOBJ_SEXPR:
      return "S-Expression";
    case LOBJ_QEXPR:
      return "Q-Expression";
    case LOBJ_FUNC:
      return "Function";
    default:
      return "Unknown";
  }
}

lobj*
lobj_call(lobj* f, lobj* args, lenv* env)
{
  if (f->builtin) {
    return f->builtin(env, args);
  }
  int asked = f->formals->count;
  int given = args->count;
  while (args->count) {
    if (f->formals->count == 0) {
      lobj_del(args);
      return lobj_err(
        "too many args given, expecting %d, got %d", asked, given);
    }
    lobj* sym = lobj_pop(f->formals, 0);

    // allow variable argument number
    if (strcmp(sym->sym, "&") == 0) {
      if (f->formals->count != 1) {
        lobj_del(args); // TODO: move this checking to defining lambda
        lobj_del(sym);
        return lobj_err(
          "Invalid function, '&' should be followed by one single symbol");
      } else {
        lobj* nsym = lobj_pop(f->formals, 0);
        lenv_create(f->env, nsym, builtin_list(env, args));
        lobj_del(sym);
        lobj_del(nsym);
        break;
      }
    }
    lobj* val = lobj_pop(args, 0);
    lenv_create(f->env, sym, val);
    lobj_del(sym);
    lobj_del(val);
  }
  lobj_del(args);
  // since args after & is optional,
  if (f->formals->count && strcmp(f->formals->cell[0]->sym, "&") == 0) {
    if (f->formals->count != 2) {
      return lobj_err(
        "Invalid function, '&' should be followed by one single symbol");
    }
    lobj_del(lobj_pop(f->formals, 0)); // delete '&' symbol
    lobj* va_sym = lobj_pop(f->formals, 0);
    lobj* val = lobj_qexpr();
    lenv_create(f->env, va_sym, val);
    lobj_del(va_sym);
    lobj_del(val);
  }
  if (f->formals->count) {
    // still remaining args
    return lobj_copy(f);
  } else {
    f->env->par = env;
    return builtin_eval( // `builtin_eval` evals q-expr
      f->env,
      lobj_append(lobj_sexpr(), lobj_copy(f->body)));
  }
}
