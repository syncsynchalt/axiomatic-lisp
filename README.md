# axiomatic-lisp

Implementing LISP from [the original McCarthy paper]
(https://www.brinckerhoff.org/clements/csc530-sp09/Readings/mccarthy-1960.pdf).

The goal is to implement a working LISP with the fewest built-in defs.

S-Expressions are implemented, with the following "axioms" from the McCarthy paper:

* ATOM: atom x -> T if x is an atom, F if not
* CAR: car (a b ...) -> (a)
* CDR: cdr (a b ...) -> (b ...)
* CONS: cons a (b) -> (a b)
* EQ: eq a b -> T if a and b are the same atom, F if a and b are different atoms, else NIL

In addition, the following to make a useful environment:

* DEF: def (x, y, ...) (S-Expression using x, y, ...)

And the following to build math capabilities:

* ADD: add 1 2 -> 3
* SUB: sub 3 2 -> 1

To run,

    ./axiom {source1.lisp, source2.lisp, ...} {-- arg1, arg2, ...}

The command line arguments following "--" are in ARGS.

Atoms are instantiated on first use.  Case is ignored.

The following are implemented using the axioms, using the definition from McCarthy where applicable:

* ... TBD

Garbage collection is not implemented.
