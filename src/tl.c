#define _XOPEN_SOURCE 500
#include "map.h"
#include "mpc.h"
#include "lenv.h"
#include "lobj.h"
#define __USE_MISC
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <readline/history.h>
#include <readline/readline.h>

#include "tl.h"

// global variable
mpc_parser_t* TL;

int
main(int argc, char** argv)
{
  mpc_parser_t* Integer = mpc_new("integer");
  mpc_parser_t* Double = mpc_new("double");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* String = mpc_new("string");
  mpc_parser_t* Comment = mpc_new("comment");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Qexpr = mpc_new("qexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  TL = mpc_new("tl");

  mpca_lang(MPCA_LANG_DEFAULT,
            " \
    integer:  /-?[0-9]+/ ;                                   \
    double:   /\\b-?([0-9]*)?\\.[0-9]+\\b/ ;   \
    symbol:   /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&%\\^\\|]+/ ;    \
    string:   /\"(\\\\.|[^\"])*\"/ ;                       \
    comment:  /;[^\\r\\n]*/ ;                              \
    sexpr:    '(' <expr>* ')' ;                            \
    qexpr:    '{' <expr>* '}';                             \
    expr:     <double> | <integer> | <symbol> | <string> | \
              <comment> | <sexpr> | <qexpr> ;              \
    tl:       /^/ <expr>* /$/;                             \
  ",
            Double,
            Integer,
            Symbol,
            String,
            Comment,
            Sexpr,
            Qexpr,
            Expr,
            TL);
  int return_code = 0;

  lobj_pool_init();
  lenv_pool_init();
  lenv* env = lenv_new(GLOBAL_ENV_INIT_SIZE);
  init_env(env);

  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      lobj* filename = lobj_append(lobj_sexpr(), lobj_str(argv[i]));
      lobj* x = builtin_load(env, filename);
      if (x->type == LOBJ_ERR) {
        return_code = 1;
        lobj_println(x);
      }
      lobj_del(x);
    }
  } else {
    return_code = repl(env);
  }

  lenv_del(env);

  lobj_pool_del();
  lenv_pool_del();
  mpc_cleanup(
    9, Double, Integer, Symbol, String, Comment, Sexpr, Qexpr, Expr, TL);
  return return_code;
}

int
repl(lenv* env)
{
  puts("Tony's Lisp (tl) version 0.1-dev");
  puts("Press Ctrl+C to exit");

  while (1) {
    char* input = readline(">>> ");
    if (input == NULL) {
      free(input);
      break;
    }
    add_history(input);
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, TL, &r)) {
#ifdef DEBUG
      mpc_ast_print(r.output);
#endif
      lobj* tmp = eval(env, lobj_read(r.output));
      lobj_println(tmp);
      lobj_del(tmp);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    /* printf("You've entered %s\n", input); */
    free(input);
  }

  return 0;
}

void
init_env(lenv* e)
{
  lenv_add_builtin(e, "def", builtin_def);
  lenv_add_builtin(e, "=", builtin_assign);
  lenv_add_builtin(e, "dir", builtin_dir);
  lenv_add_builtin(e, "echo", builtin_echo);
  lenv_add_builtin(e, "error", builtin_error);
  lenv_add_builtin(e, "del", builtin_del);
  lenv_add_builtin(e, "load", builtin_load);

  lenv_add_builtin(e, "\\", builtin_lambda);
  lenv_add_builtin(e, "lambda", builtin_lambda);
  // lenv_add_builtin(e, "fn", builtin_fn);
  lenv_add_builtin(e, "if", builtin_if);

  lenv_add_builtin(e, "==", builtin_eq);
  lenv_add_builtin(e, "!=", builtin_ne);

  lenv_add_builtin(e, "<", builtin_lt);
  lenv_add_builtin(e, ">", builtin_gt);
  lenv_add_builtin(e, "<=", builtin_le);
  lenv_add_builtin(e, ">=", builtin_ge);

  lenv_add_builtin(e, "+", builtin_add);
  lenv_add_builtin(e, "-", builtin_sub);
  lenv_add_builtin(e, "*", builtin_mul);
  lenv_add_builtin(e, "/", builtin_div);
  lenv_add_builtin(e, "%", builtin_mod);

  // logical
  lenv_add_builtin(e, "&&", builtin_and);
  lenv_add_builtin(e, "!", builtin_not);
  lenv_add_builtin(e, "||", builtin_or);

  // list
  lenv_add_builtin(e, "list", builtin_list);
  lenv_add_builtin(e, "head", builtin_head);
  lenv_add_builtin(e, "tail", builtin_tail);
  lenv_add_builtin(e, "join", builtin_join);
  lenv_add_builtin(e, "eval", builtin_eval);
  // lenv_add_builtin(e, "len", builtin_len);

  lenv_add_builtin(e, "add", builtin_fadd);
  lenv_add_builtin(e, "sub", builtin_fsub);
  lenv_add_builtin(e, "mul", builtin_fmul);
  lenv_add_builtin(e, "div", builtin_fdiv);

  // lenv_add_builtin(e, "min", builtin_fmin);
  // lenv_add_builtin(e, "max", builtin_fmax);

  lenv_add_builtin(e, "mod", builtin_fmod);
  lenv_add_builtin(e, "exp", builtin_exp);
  lenv_add_builtin(e, "pow", builtin_pow);
  lenv_add_builtin(e, "ln", builtin_ln);

  lenv_add_builtin(e, "eq", builtin_feq);
  lenv_add_builtin(e, "lt", builtin_flt);
  lenv_add_builtin(e, "gt", builtin_fgt);

  // const symbols
  lenv_add_symbol(e, "PI", lobj_double(M_PI));
  lenv_add_symbol(e, "E", lobj_double(M_E));
  // lenv_add_symbol(e, "true", lobj_int(1));
  // lenv_add_symbol(e, "false", lobj_int(0));

  // for (int i = 0; i < e->count; ++i) {
  //   e->objs[i]->constant = true;
  // }
}

lobj*
builtin_var(lenv* env, lobj* a, char* operator)
{
  // sexpr (symbol args...)
  // def {a b c} 1 2 3
  LASSERT_ARGC(operator, a, 2);
  LASSERT_TYPE_I(operator, a, 0, LOBJ_QEXPR);

  lobj* syms = a->cell[0];
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(a,
            syms->cell[i]->type == LOBJ_SYM,
            "<function %s> can only bind to %s, got %s",
            operator,
            lobj_typename(LOBJ_SYM),
            lobj_typename(syms->cell[i]->type));
  }
  LASSERT(a,
          syms->count == a->count - 1,
          "<function %s> got %d symbols and %d values",
          operator,
          syms->count,
          a->count - 1);

  // TODO protect builtin symbols
  for (int i = 0; i < syms->count; ++i) {
    if (strcmp(operator, "=") == 0) {
      // TODO
      lenv_create(env, syms->cell[i], a->cell[i + 1]);
    }
    if (strcmp(operator, "def") == 0) {
      lenv_global_create(env, syms->cell[i], a->cell[i + 1]);
    }
    // else undefined
  }
  lobj_del(a);
  return lobj_sexpr();
}

lobj*
builtin_def(lenv* env, lobj* a)
{
  return builtin_var(env, a, "def");
}

lobj*
builtin_assign(lenv* env, lobj* a)
{
  return builtin_var(env, a, "=");
}

lobj*
builtin_dir(lenv* env, lobj* a)
{

  LASSERT_ARGC("dir", a, 0);
  for (int i = 0; i < env->map->max_size; ++i) {
    if (env->map->entries[i]) {
      printf("slot[%d]:", i);
      entry_t* e = env->map->entries[i];
      while (e) {
        printf("%s = ", e->key);
        lobj_print(e->val);
        putchar('\t');
        e = e->next;
      }
      putchar('\n');
    }
  }
  lobj_del(a);
  // TODO return a qexpr of symbols instead
  // return ans;
  return lobj_sexpr();
}

lobj*
builtin_del(lenv* env, lobj* a)
{
  // del { 1 2 3}
  LASSERT(
    a, a->count == 1, "<function del> expecting 1 argument, got %d", a->count);
  LASSERT(a, a->cell[0]->type == LOBJ_QEXPR, "symbol must be qexpr");
  lobj* syms = a->cell[0]; // qexpr
  LASSERT(syms,
          syms->count == 1,
          "<function del> deletes 1 symbol at a time, got %d",
          syms->count);
  for (int i = 0; i < syms->count; ++i) {
    LASSERT(a,
            syms->cell[i]->type == LOBJ_SYM,
            "<function del> can only delete symbols");
  }
  lobj* result;
  if (lenv_remove(env, syms->cell[0]) == 0) {
    result = lobj_sexpr();
  } else {
    result = lobj_err("undefined symbol %s", syms->str);
  }
  lobj_del(a);
  return result;
}

lobj*
builtin_echo(lenv* env, lobj* a)
{
  for (int i = 0; i < a->count; ++i) {
    lobj_print(a->cell[i]);
    putchar(' ');
  }
  putchar('\n');
  lobj_del(a);
  return lobj_sexpr();
}

lobj*
builtin_error(lenv* env, lobj* a)
{
  LASSERT_ARGC("error", a, 1);
  LASSERT_TYPE_I("error", a, 0, LOBJ_STR);

  lobj* err = lobj_err(a->cell[0]->str);
  lobj_del(a);
  return err;
}

lobj*
builtin_load(lenv* env, lobj* a)
{
  LASSERT_ARGC("load", a, 1);
  LASSERT_TYPE_I("load", a, 0, LOBJ_STR);

  mpc_result_t r;
  if (mpc_parse_contents(a->cell[0]->str, TL, &r)) {
    lobj* expr = lobj_read(r.output);
    mpc_ast_delete(r.output);
    while (expr->count) {
      lobj* x = eval(env, lobj_pop(expr, 0));
      if (x->type == LOBJ_ERR)
        lobj_println(x);
      lobj_del(x);
    }
    lobj_del(expr);
    lobj_del(a);
    return lobj_sexpr();
  } else {
    char* err_msg = mpc_err_string(r.error);
    mpc_err_delete(r.error);
    lobj* err = lobj_err("Cannot find library, %s", err_msg);
    lobj_del(a);
    free(err_msg);
    return err;
  }
}

lobj*
builtin_lambda(lenv* env, lobj* a)
{
  LASSERT_ARGC("\\", a, 2);
  LASSERT_TYPE_I("\\", a, 0, LOBJ_QEXPR);
  LASSERT_TYPE_I("\\", a, 1, LOBJ_QEXPR);

  for (int i = 0; i < a->cell[0]->count; ++i) {
    LASSERT(a,
            (a->cell[0]->cell[i]->type == LOBJ_SYM),
            "<function \\> lambda argument at %d must be %s, got %s",
            i + 1,
            lobj_typename(LOBJ_SYM),
            lobj_typename(a->cell[0]->cell[i]->type));
  }
  lobj* formals = lobj_pop(a, 0);
  lobj* body = lobj_pop(a, 0);
  lobj* lambda = lobj_lambda(formals, body);
  lobj_del(a);
  return lambda;
}

lobj*
builtin_if(lenv* env, lobj* a)
{
  // TODO make else part optional
  LASSERT_ARGC("if", a, 3);
  LASSERT_TYPE_I("if", a, 0, LOBJ_INT);
  LASSERT_TYPE_I("if", a, 1, LOBJ_QEXPR);
  LASSERT_TYPE_I("if", a, 2, LOBJ_QEXPR);

  lobj* x;
  if (a->cell[0]->i) {
    a->cell[1]->type = LOBJ_SEXPR;
    x = eval(env, lobj_pop(a, 1));
  } else {
    a->cell[2]->type = LOBJ_SEXPR;
    x = eval(env, lobj_pop(a, 2));
  }
  lobj_del(a);
  return x;
}

lobj*
builtin_int_arith(lenv* env, lobj* a, tl_operator op)
{
  // LASSERT_ARGC("arithmatic", a, 2);
  LASSERT(a,
          a->count > 1,
          "<function int arith> got %d args, expecting >= 2",
          a->count);
  for (int i = 0; i < a->count; ++i)
    LASSERT_TYPE_I("arithmatic", a, i, LOBJ_INT)
  lobj* ans = lobj_pop(a, 0);
  while (a->count) {
    lobj* tmp = lobj_pop(a, 0);
    switch (op) {
      case OP_ADD:
        ans->i += tmp->i;
        break;
      case OP_SUB:
        ans->i -= tmp->i;
        break;
      case OP_MUL:
        ans->i = ans->i * tmp->i;
        break;
      case OP_DIV:
        if (tmp->i == 0) {
          lobj_del(tmp);
          lobj_del(ans);
          lobj_del(a);
          return lobj_err("divide by zero");
        }
        ans->i += tmp->i;
        break;
      default:
        lobj_del(tmp);
        lobj_del(ans);
        lobj_del(a);
        return lobj_err("internal error");
    }
    lobj_del(tmp);
  }
  lobj_del(a);
  return ans;
}
lobj*
builtin_add(lenv* env, lobj* a)
{
  return builtin_int_arith(env, a, OP_ADD);
}
lobj*
builtin_sub(lenv* env, lobj* a)
{
  return builtin_int_arith(env, a, OP_SUB);
}
lobj*
builtin_mul(lenv* env, lobj* a)
{
  return builtin_int_arith(env, a, OP_MUL);
}
lobj*
builtin_div(lenv* env, lobj* a)
{
  return builtin_int_arith(env, a, OP_DIV);
}

lobj*
builtin_mod(lenv* env, lobj* a)
{
  LASSERT_ARGC("%", a, 2);
  for (int i = 0; i < a->count; ++i)
    LASSERT_TYPE_I("arithmatic", a, i, LOBJ_INT)
  lobj* ans = lobj_pop(a, 0);
  ans->i = ans->i % a->cell[0]->i;
  lobj_del(a);
  return ans;
}

lobj*
builtin_cmp(lenv* env, lobj* a, char* op)
{
  LASSERT_ARGC(op, a, 2);
  int r = 0;
  if (strcmp(op, "==") == 0) {
    r = lobj_eq(a->cell[0], a->cell[1]);
  }
  if (strcmp(op, "!=") == 0) {
    r = !lobj_eq(a->cell[0], a->cell[1]);
  } // unsafe
  lobj_del(a);
  return lobj_int(r);
}

lobj*
builtin_eq(lenv* env, lobj* a)
{
  return builtin_cmp(env, a, "==");
}
lobj*
builtin_ne(lenv* env, lobj* a)
{
  return builtin_cmp(env, a, "!=");
}

lobj*
builtin_ord(lenv* env, lobj* a, char* op)
{
  LASSERT_ARGC(op, a, 2);
  LASSERT_TYPE_I(op, a, 0, LOBJ_INT);
  LASSERT_TYPE_I(op, a, 1, LOBJ_INT);
  int ans = 0;
  if (strcmp(op, ">") == 0) {
    ans = a->cell[0]->i > a->cell[1]->i;
  } else if (strcmp(op, "<") == 0) {
    ans = a->cell[0]->i < a->cell[1]->i;
  } else if (strcmp(op, ">=") == 0) {
    ans = a->cell[0]->i >= a->cell[1]->i;
  } else if (strcmp(op, "<=") == 0) {
    ans = a->cell[0]->i <= a->cell[1]->i;
  }
  lobj_del(a);
  return lobj_int(ans);
}
lobj*
builtin_lt(lenv* env, lobj* a)
{
  return builtin_ord(env, a, "<");
}
lobj*
builtin_le(lenv* env, lobj* a)
{
  return builtin_ord(env, a, "<=");
}

lobj*
builtin_gt(lenv* env, lobj* a)
{
  return builtin_ord(env, a, ">");
}
lobj*
builtin_ge(lenv* env, lobj* a)
{
  return builtin_ord(env, a, ">=");
}

lobj*
builtin_and(lenv* env, lobj* a)
{
  LASSERT_ARGC("and", a, 2);
  for (int i = 0; i < a->count; ++i)
    LASSERT_TYPE_I("and", a, i, LOBJ_INT);
  lobj* ans = lobj_pop(a, 0);
  ans->i = (ans->i && a->cell[0]->i);
  lobj_del(a);
  return ans;
}
lobj*
builtin_or(lenv* env, lobj* a)
{
  LASSERT_ARGC("or", a, 2);
  for (int i = 0; i < a->count; ++i)
    LASSERT_TYPE_I("or", a, i, LOBJ_INT);
  lobj* ans = lobj_pop(a, 0);
  ans->i = (ans->i || a->cell[0]->i);
  lobj_del(a);
  return ans;
}
lobj*
builtin_not(lenv* env, lobj* a)
{
  LASSERT_ARGC("not", a, 1);
  LASSERT_TYPE_I("not", a, 0, LOBJ_INT);
  lobj* ans = lobj_take(a, 0);
  ans->i = !(ans->i);
  return ans;
}
