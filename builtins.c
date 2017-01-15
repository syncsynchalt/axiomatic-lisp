#include "axiom.h"

expr *atom(expr *e)
{
    return e->atom ? T : NIL;
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
    expr *e = get_free_cell(a, b);
    base_registers[0] = base_registers[1] = 0;
    return e;
}

expr *eq(expr *a, expr *b)
{
    return (a->atom && a->atom == b->atom && (a->numval == b->numval)) ? T : NIL;
}

expr *add(expr *a, expr *b, int subtract)
{
    if (a->atom != ATOM_NUMERIC || b->atom != ATOM_NUMERIC)
        return NIL;
    base_registers[2] = a; base_registers[3] = b;
    expr *result = get_free_cell(NIL, NIL);
    base_registers[2] = base_registers[3] = 0;
    result->atom = ATOM_NUMERIC;
    if (subtract)
        result->numval = a->numval - b->numval;
    else
        result->numval = a->numval + b->numval;
    return result;
}
