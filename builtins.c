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
    base_registers[0] = a; base_registers[1] = b;
    expr *e = get_free_cell();
    base_registers[0] = base_registers[1] = 0;
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
    base_registers[2] = a; base_registers[3] = b;
    expr *result = get_free_cell();
    base_registers[2] = base_registers[3] = 0;
    result->atom = ATOM_NUMERIC;
    result->numval = a->numval + b->numval;
    return result;
}

expr *sub(expr *a, expr *b)
{
    if (a->atom != ATOM_NUMERIC || b->atom != ATOM_NUMERIC)
        return NIL;
    base_registers[4] = a; base_registers[5] = b;
    expr *result = get_free_cell();
    base_registers[4] = base_registers[5] = 0;
    result->atom = ATOM_NUMERIC;
    result->numval = a->numval - b->numval;
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
