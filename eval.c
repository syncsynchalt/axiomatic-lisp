#include "axiom.h"
#include <strings.h>

expr *eval(expr *e, expr *a)
{
    // todo ASSOC atom
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
            return eval(def_exprs[defnum], a);
        }
    }
    return e;
}
