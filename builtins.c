#include "axiom.h"
#include "builtins.h"
#include <stdlib.h>

expr *axiom_atom(expr *e)
{
    if (e->atom)
        return find_atom("T");
    else
        return find_atom("F");
}

expr *axiom_car(expr *e)
{
    if (e->atom || !e->left)
        return find_atom("NIL");
    return e->left;
}

expr *axiom_cdr(expr *e)
{
    if (e->atom)
        return find_atom("NIL");
    return e->right;
}

expr *axiom_cons(expr *a, expr *b)
{
    expr *e = calloc(sizeof *e, 1);
    e->left = a;
    e->right = b;
    return e;
}

expr *axiom_eq(expr *e)
{
    if (!e->left->atom || !e->right->atom)
        return find_atom("NIL");
    if (e->left->atom == e->right->atom && (e->left->numval == e->right->numval))
        return find_atom("T");
    else
        return find_atom("F");
}

expr *builtin_add(expr *e)
{
    if (e->atom || e->left->atom != ATOM_NUMERIC || e->right->atom != ATOM_NUMERIC)
        return find_atom("NIL");
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = e->left->numval + e->right->numval;
    result->left = result->right = find_atom("NIL");
    return result;
}

expr *builtin_sub(expr *e)
{
    if (e->atom || e->left->atom != ATOM_NUMERIC || e->right->atom != ATOM_NUMERIC)
        return find_atom("NIL");
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = e->left->numval - e->right->numval;
    result->left = result->right = find_atom("NIL");
    return result;
}

static expr *builtin_def_bind(expr *args, expr *func);

expr *builtin_def(expr *e)
{
    expr *name = axiom_car(e);
    expr *args = axiom_car(axiom_cdr(e));
    expr *func = axiom_cdr(axiom_cdr(e));
    int i = 0;
    for (i = 0; i < MAX_DEFS && def_names[i]; i++)
        ;
    if (i >= MAX_DEFS)
        die("More than %d defs!\n", MAX_DEFS);
    def_names[i] = atom_names[name->atom];
    return def_exprs[i] = builtin_def_bind(args, func);
}

static expr *builtin_def_bind(expr *args, expr *func)
{
    if (func->left) {
        builtin_def_bind(args, func->left);
    } else if (func->right) {
        builtin_def_bind(args, func->right);
    } else if (func->atom) {
        int index = 0;
        for (expr *l = args; l->atom == 0; l = args->right, index++) {
            if (l->left->atom == func->atom) {
                func->atom = func->numval = 0;
                func->bound = index;
            }
        }
    }
    return func;
}
