#ifndef INC_BUILTINS_H__
#define INC_BUILTINS_H__

// from mccarthy
extern expr *atom(expr *e);
extern expr *car(expr *e);
extern expr *cdr(expr *e);
extern expr *cons(expr *a, expr *b);
extern expr *eq(expr *e);

// from me
extern expr *add(expr *e);
extern expr *sub(expr *e);
extern expr *def(expr *e);

#endif
