#include "axiom.h"
#include <strings.h>

expr **eval_gclinks = base_registers + EVALS_OFFSET;
static int cur_eval_gclink = 0;

void push_gclink(expr *e)
{
    eval_gclinks[cur_eval_gclink++] = e;
}

expr *pop_gclink()
{
    expr *e = eval_gclinks[--cur_eval_gclink];
    eval_gclinks[cur_eval_gclink] = 0;
    return e;
}

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
    push_gclink(eval(car(list), a));
    push_gclink(evalmap(cdr(list), a));
    expr *e2 = pop_gclink();
    expr *e1 = pop_gclink();
    return cons(e1, e2);
}

// cond[(((EQ, A, B), 1), ((EQ, A, A), 2), ((T), 3))] = 2
static expr *cond(expr *conditions, expr *a)
{
    if (conditions->atom == NIL->atom)
        return NIL;
    if (eval(car(car(conditions)), a)->atom == find_atom("T")->atom)
        return eval(car(cdr(car(conditions))), a);
    return cond(cdr(conditions), a);
}

static expr *eval2_and_call(expr *arg1, expr *arg2, expr *pairs, expr*(*func)(expr*,expr*))
{
    push_gclink(eval(arg1, pairs));
    push_gclink(eval(arg2, pairs));
    expr *e2 = pop_gclink();
    expr *e1 = pop_gclink();
    return func(e1, e2);
}

expr *eval(expr *e, expr *a)
{
    if (e->atom) {
        return assoc(e, a);
    }
    push_gclink(e);
    push_gclink(a);
    expr *cmd = car(e);
    expr *arg1 = car(cdr(e));
    expr *arg2 = car(cdr(cdr(e)));
    const char *label = atom_names[cmd->atom];
    if (strcasecmp(label, "quote") == 0)
        { e = arg1; goto done; }
    if (strcasecmp(label, "atom") == 0)
        { e = atom(eval(arg1, a)); goto done; }
    if (strcasecmp(label, "eq") == 0)
        { e = eval2_and_call(arg1, arg2, a, eq); goto done; }
    if (strcasecmp(label, "cond") == 0)
        { e = cond(arg1, a); goto done; }
    if (strcasecmp(label, "car") == 0)
        { e = car(eval(arg1, a)); goto done; }
    if (strcasecmp(label, "cdr") == 0)
        { e = cdr(eval(arg1, a)); goto done; }
    if (strcasecmp(label, "cons") == 0)
        { e = eval2_and_call(arg1, arg2, a, cons); goto done; }
    // todo more CONS?
    // todo LABEL?
    // todo LAMBDA?
    if (strcasecmp(label, "add") == 0)
        { e = eval2_and_call(arg1, arg2, a, add); goto done; }
    if (strcasecmp(label, "sub") == 0)
        { e = eval2_and_call(arg1, arg2, a, sub); goto done; }
    if (strcasecmp(label, "def") == 0)
        { e = def(cdr(e)); goto done; }
    for (int defnum = 0; def_atoms[defnum]; defnum++) {
        if (cmd->atom == def_atoms[defnum]) {
            expr *a2 = pair(def_argsl[defnum], evalmap(cdr(e), a));
            //deb("{{{ calling %s with arglist:", atom_names[def_atoms[defnum]]);
            //dprint(a2);
            e = eval(def_exprs[defnum], a2);
            //deb("}}} result:");
            //dprint(e);
            goto done;
        }
    }
done:
    pop_gclink();
    pop_gclink();
    return e;
}

// todo apply
