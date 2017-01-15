# axiomatic-lisp

Inspired by [this article by Eric Normand](https://dev.to/ericnormand/the-idea-of-lisp),
an implementation of LISP from [the original McCarthy paper]
(https://www.brinckerhoff.org/clements/csc530-sp09/Readings/mccarthy-1960.pdf).

The goal is to implement a working LISP with the fewest lines of C.

S-Expressions are implemented, with the following "axioms" from the McCarthy paper:

* ATOM: atom x -> T if x is an atom, F if not
* CAR: car (a b ...) -> (a)
* CDR: cdr (a b ...) -> (b ...)
* CONS: cons a (b) -> (a b)
* EQ: eq a b -> T if a and b are the same atom, F if a and b are different atoms, else NIL

The following to build math capabilities:

* ADD: add 1 2 -> 3
* SUB: sub 3 2 -> 1

The following intermediate functions made from the above axioms:

* QUOTE: (quote (car (a))) -> (car (a)) [ie. not executed]

The following (non-executable) intermediates from the McCarthy paper:

* ASSOC: `[X; ((W, (A, B)), (X, (C, D)), (Y, (E, F)))] => (C, D)`
* PAIR: `[(A, B, C); (X, (Y, Z), U); (TAIL)] => ((A, X), (B, (Y, Z)), (C, U), TAIL)`
   * the "tail" addition removes the need for "append" in the kernel
* EVLIS: `[((QUOTE T), (X), (ADD, 1, 2))] => (T, NIL, 3)`
* EVCON: `[(((EQ, A, B), 1), ((EQ, A, A), 2), ((T), 3))] => 2`
* EVAL, APPLY, APPQ: see paper

And, the ability to define functions (puts a label/lambda in global environment):

* DEFUN: defun (x, y, ...) (S-Expression using x, y, ...)

### Build and Run

    make
    make test #(optional)
    ./axiom {source1.lisp, source2.lisp, ...}

Atoms are instantiated on first use.  Case is ignored.
