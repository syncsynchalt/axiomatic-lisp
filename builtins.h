#ifndef INC_BUILTINS_H__
#define INC_BUILTINS_H__

extern expr *axiom_atom(expr *e);
extern expr *axiom_car(expr *e);
extern expr *axiom_cdr(expr *e);
extern expr *axiom_cons(expr *a, expr *b);
extern expr *axiom_eq(expr *e);

extern expr *builtin_add(expr *e);
extern expr *builtin_sub(expr *e);
extern expr *builtin_def(expr *e);

#endif
