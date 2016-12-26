#include "axiom.h"
#include <strings.h>

// assoc[X; ((W, (A, B)), (X, (C, D)), (Y, (E, F)))] = (C, D)
static expr *assoc(expr *key, expr *keyvals)
{
    if (keyvals->atom == NIL->atom)
        // not defined in paper, I went with identity
        return key;
    if (car(car(keyvals))->atom == key->atom)
        return car(cdr(car(keyvals)));
    return assoc(key, cdr(keyvals));
}

// pair[(A, B, C); (X, (Y, Z), U)] = ((A, X), (B, (Y, Z)), (C, U))
static expr *pair(expr *x, expr *y)
{
    if (x->atom == NIL->atom)
        return NIL;
    return cons(cons(car(x), cons(car(y), NIL)), pair(cdr(x), cdr(y)));
}

// evalmap[(T, F, (ADD, 1, 2))] = (T, F, 3)
static expr *evalmap(expr *list, expr *a)
{
    if (list->atom == NIL->atom)
        return NIL;
    return cons(eval(car(list), a), evalmap(cdr(list), a));
}

// cond[(((EQ, A, B), 1), ((EQ, A, A), 2), ((T), 3))] = 2
static expr *cond(expr *conditions, expr *a)
{
    if (conditions->atom == NIL->atom)
        return NIL;
    if (eval(car(car(conditions)), a)->atom == find_atom("T")->atom) {
        // todo - get around this deref
        expr *r = eval(cdr(car(conditions)), a);
        return r->atom ? r : car(r);
    }
    return cond(cdr(conditions), a);
}

expr *eval(expr *e, expr *a)
{
    if (e->atom) {
        return assoc(e, a);
    }
    expr *cmd = car(e);
    expr *arg1 = car(cdr(e));
    expr *arg2 = car(cdr(cdr(e)));
    const char *label = atom_names[cmd->atom];
    if (strcasecmp(label, "quote") == 0)
        return arg1;
    if (strcasecmp(label, "atom") == 0)
        return atom(eval(arg1, a));
    if (strcasecmp(label, "eq") == 0)
        return eq(eval(arg1, a), eval(arg2, a));
    if (strcasecmp(label, "cond") == 0)
        return cond(arg1, a);
    if (strcasecmp(label, "car") == 0)
        return car(eval(arg1, a));
    if (strcasecmp(label, "cdr") == 0)
        return cdr(eval(arg1, a));
    if (strcasecmp(label, "cons") == 0)
        return cons(eval(arg1, a), eval(arg2, a));
    // todo more CONS?
    // todo LABEL
    // todo LAMBDA
    if (strcasecmp(label, "add") == 0)
        return add(eval(arg1, a), eval(arg2, a));
    if (strcasecmp(label, "sub") == 0)
        return sub(eval(arg1, a), eval(arg2, a));
    if (strcasecmp(label, "def") == 0)
        return def(cdr(e));
    for (int defnum = 0; def_atoms[defnum]; defnum++) {
        if (cmd->atom == def_atoms[defnum]) {
            expr *a2 = pair(def_argsl[defnum], evalmap(cdr(e), a));
            //deb("{{{ calling %s with arglist:", atom_names[def_atoms[defnum]]);
            //dprint(a2);
            expr *e = eval(def_exprs[defnum], a2);
            //deb("}}} result:");
            //dprint(e);
            return e;
        }
    }
    return e;
}

// todo apply
