# tl, Tony's Lispformatting

A simple lisp implementation ([Build you own Lisp](http://www.buildyourownlisp.com))
## Try it
```bash
make BUILD=RELEASE run
```

create a short hand function for defining global functions using lambda (`\`)
```lisp
(def {fn} 
  (\ {fargs body} 
    {def (head fargs) (\ (tail fargs) body) }))
; then
(fn {fname args...} {body})
```
otherwise
```
def {fname} (\ {arg1 arg2 ...} body)
```

fibonacci
```lisp
;; with recursion
(fn {fib n}
  {if (|| (== n 1) (== n 2))
    {1}
    {+ (fib (- n 1)) (fib (- n 2))}})
;; with closure (closures must be defined using lambda)
(fn {fib2 n} { do
  (= {helper}
    (\ {n2 l} {
      if (== n n2) 
        {l}
        {helper (+ n2 1)
          (list (second l) (+ (first l) (second l)))}}
  ))
  (first (helper 0 {0 1}))
})
```

`tl` only wraps `log` from `<math.h>` as `ln` which is natural logarithm.
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
Lexing is being kept as a black box using library from [libmpc](https://github.com/orangeduck/mpc).

The abstract symtax tree

## TODO

- [x] hash map for symbol lookup [map.c](./src/map.c)
- [x] pool allocation [mpool.c](./src/mpool.c)
- [ ] gc
- [ ] wrap os function
