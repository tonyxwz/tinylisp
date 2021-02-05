# tl, Tony's Lisp

A simple lisp implementation ([Build you own Lisp](http://www.buildyourownlisp.com))

## Language

primitives:
- number: any forms of numbers stored using double in C
- symbol: string like identifier of a piece of resource
- s-expr: ( expression*... )
- q-expr: { expression*... }
- expression: s-expr |  q-expr | number | symbol
- tl (lisp): expression*

## Builtin

- integer math operation: + - * /
- double operator: add sub mul div mod pow exp ln
- compare: max min
- Q-Expression: list head tail join eval
- Environment: def, =, dir, echo, lambda

## Implementation

### Parser
Lexing is being kept as a black bos using library from [libmpc](https://github.com/orangeduck/mpc). When language feature completes, I will start rewriting a compatible library.

The abstract symtax tree

