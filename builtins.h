#ifndef INC_BUILTINS_H__
#define INC_BUILTINS_H__

// from mccarthy
extern expr *atom(expr *e);
extern expr *car(expr *e);
extern expr *cdr(expr *e);
extern expr *cons(expr *a, expr *b);
extern expr *eq(expr *a, expr *b);

// from me
extern expr *add(expr *a, expr *b);
extern expr *sub(expr *a, expr *b);
extern expr *def(expr *name, expr *args, expr *func);

#endif
