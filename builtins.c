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
        return find_atom("NIL");
    if (a->atom == b->atom && (a->numval == b->numval))
        return find_atom("T");
    else
        return find_atom("F");
}

expr *add(expr *a, expr *b)
{
    if (a->atom != ATOM_NUMERIC || b->atom != ATOM_NUMERIC)
        return find_atom("NIL");
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = a->numval + b->numval;
    result->left = result->right = find_atom("NIL");
    return result;
}

expr *sub(expr *a, expr *b)
{
    if (a->atom != ATOM_NUMERIC || b->atom != ATOM_NUMERIC)
        return find_atom("NIL");
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = a->numval - b->numval;
    result->left = result->right = find_atom("NIL");
    return result;
}

static expr *def_bind(expr *args, expr *func);

expr *def(expr *e)
{
    expr *name = car(e);
    expr *args = car(cdr(e));
    expr *func = cdr(cdr(e));
    int i = 0;
    for (i = 0; i < MAX_DEFS && def_atoms[i]; i++)
        ;
    if (i >= MAX_DEFS)
        die("More than %d defs!\n", MAX_DEFS);
    def_atoms[i] = name->atom;
    def_exprs[i] = def_bind(args, func);
    return cons(find_atom("DEFINED"), cons(name, find_atom("NIL")));
}

static expr *def_bind(expr *args, expr *func)
{
    if (func->atom) {
        int index = 0;
        for (expr *l = args; l->atom == 0; l = l->right, index++) {
            if (l->left->atom == func->atom) {
                func->atom = func->numval = 0;
                func->bound = index;
            }
        }
    } else {
        def_bind(args, car(func));
        def_bind(args, cdr(func));
    }
    return func;
}
