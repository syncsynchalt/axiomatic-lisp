#include "axiom.h"
#include <stdlib.h>

expr *atom(expr *e)
{
    if (e->atom)
        return find_atom("T");
    else
        return find_atom("F");
}

expr *car(expr *e)
{
    return e->left;
}

expr *cdr(expr *e)
{
    return e->right;
}

expr *cons(expr *a, expr *b)
{
    expr *e = calloc(sizeof *e, 1);
    e->left = a;
    e->right = b;
    return e;
}

expr *eq(expr *a, expr *b)
{
    if (!a->atom || !b->atom)
        return NIL;
    if (a->atom == b->atom && (a->numval == b->numval))
        return find_atom("T");
    else
        return find_atom("F");
}

expr *add(expr *a, expr *b)
{
    if (a->atom != ATOM_NUMERIC || b->atom != ATOM_NUMERIC)
        return NIL;
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = a->numval + b->numval;
    result->left = result->right = NIL;
    return result;
}

expr *sub(expr *a, expr *b)
{
    if (a->atom != ATOM_NUMERIC || b->atom != ATOM_NUMERIC)
        return NIL;
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = a->numval - b->numval;
    result->left = result->right = NIL;
    return result;
}

expr *def(expr *e)
{
    expr *name = car(e);
    expr *args = car(cdr(e));
    expr *func = car(cdr(cdr(e)));
    int i = 0;
    for (i = 0; i < MAX_DEFS && def_atoms[i]; i++)
        ;
    if (i >= MAX_DEFS)
        die("More than %d defs!\n", MAX_DEFS);
    def_atoms[i] = name->atom;
    def_argsl[i] = args;
    def_exprs[i] = func;
    return cons(find_atom("DEFINED"), cons(name, NIL));
}
