# tl, Tony's Lisp

A simple lisp implementation ([Build you own Lisp](http://www.buildyourownlisp.com))
## Try it
```bash
make BUILD=RELEASE run
```

create a short hand function for defining functions using lambda (`\`)
```lisp
(def {fn} 
  (\ {fargs body} 
    {def (head fargs) (\ (tail fargs) body) }))
```
otherwise
```
def {fname} (\ {arg1 arg2 ...} body)
```
fibonacci
```lisp
(fn {fib n}
  {if (|| (== n 1) (== n 2))
    {1}
    {+ (fib (- n 1)) (fib (- n 2))}})
```

`tl` only wraps `log` from `<math.h>` which is natural logarithm.
```lisp
;; log_x (y)
(fn {log x y}
  {div (ln y) (ln x)})
(log 4.0 16.0)
2.000000
```

## Language

- double: any forms of numbers stored using double in C
- integer: as int, and boolean type
- string: "..."
- symbol: unique identifier of a piece of resource
- comment: ;...
- s-expr: ( expression*... )
- q-expr: { expression*... }
- expression: s-expr |  q-expr | double | integer | symbol | comment
- tl (lisp): expression*

## Builtin

- integer math operation: + - * /
- double operator: add sub mul div mod pow exp ln
- Q-Expression (list): list head tail join eval
- Environment: def, =, dir, echo, lambda

## Implementation

### Parser
Lexing is being kept as a black bos using library from [libmpc](https://github.com/orangeduck/mpc).

The abstract symtax tree

## TODO

- hash map for symbol lookup
- wrap os function
- pool allocation
- gc
