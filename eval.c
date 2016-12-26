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
    // todo COND
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
            expr *a2 = pair(def_argsl[defnum], cdr(e));
            return eval(def_exprs[defnum], a2);
        }
    }
    return e;
}

// todo apply
