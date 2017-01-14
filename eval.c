#include "axiom.h"
#include <strings.h>
#include <stdarg.h>

expr **eval_gclinks = base_registers + EVALS_OFFSET;
static int cur_eval_gclink = 0;

// push onto GC stack
expr *_(expr *e)
{
    return eval_gclinks[cur_eval_gclink++] = e;
}

expr *pop_gcstack(int level, expr *ret)
{
    while (cur_eval_gclink > level)
        eval_gclinks[--cur_eval_gclink] = 0;
    return ret;
}

// assoc[X; ((W, (A, B)), (X, (C, D)), (Y, (E, F)))] = (C, D)
static expr *assoc(expr *key, expr *keyvals)
{
    // bug in original paper?  it doesn't have the second condition to avoid loop
    if (isNIL(key) || isNIL(keyvals))
        return NIL;
    if (isT(eq(car(car(keyvals)), key)))
        return car(cdr(car(keyvals)));
    return assoc(key, cdr(keyvals));
}

// pair[(A, B, C); (X, (Y, Z), U); ...tail...] = ((A, X), (B, (Y, Z)), (C, U), ...tail...)
static expr *pair(expr *x, expr *y, expr *tail)
{
    if (isNIL(x))
        return tail;
    return cons(cons(car(x), cons(car(y), NIL)), pair(cdr(x), cdr(y), tail));
}

// evlis[(T, F, (ADD, 1, 2))] = (T, F, 3)
static expr *evlis(expr *l, expr *a)
{
    if (isNIL(l))
        return NIL;
    return cons(_(eval(car(l), a)), _(evlis(cdr(l), a)));
}

// implementation of evcon
// evcon[(((EQ, A, B), 1), ((EQ, A, A), 2), ((T), 3))] = 2
static expr *evcon(expr *conditions, expr *a)
{
    if (isNIL(conditions))
        return NIL;
    if (isT(eval(car(car(conditions)), a)))
        return eval(car(cdr(car(conditions))), a);
    return evcon(cdr(conditions), a);
}

// needed to hold gc links
static expr *eval2_and_call(expr *arg1, expr *arg2, expr *a, expr*(*func)(expr*,expr*))
{
    return func(_(eval(arg1, a)), _(eval(arg2, a)));
}

expr *eval(expr *e, expr *a)
{
    if (isT(atom(e))) {
        return assoc(e, a);
    }
    int ll = cur_eval_gclink;
    _(e);
    _(a);
    if (isT(atom(car(e)))) {
        expr *cmd = car(e);
        expr *arg1 = car(cdr(e));
        expr *arg2 = car(cdr(cdr(e)));
        const char *label = atom_names[cmd->atom];
        if (isNIL(cmd))
            { e = NIL; goto done; }
        if (strcasecmp(label, "quote") == 0)
            { e = arg1; goto done; }
        if (strcasecmp(label, "atom") == 0)
            { e = atom(eval(arg1, a)); goto done; }
        if (strcasecmp(label, "eq") == 0)
            { e = eval2_and_call(arg1, arg2, a, eq); goto done; }
        if (strcasecmp(label, "cond") == 0)
            { e = evcon(cdr(e), a); goto done; }
        if (strcasecmp(label, "car") == 0)
            { e = car(eval(arg1, a)); goto done; }
        if (strcasecmp(label, "cdr") == 0)
            { e = cdr(eval(arg1, a)); goto done; }
        if (strcasecmp(label, "cons") == 0)
            { e = eval2_and_call(arg1, arg2, a, cons); goto done; }
        if (strcasecmp(label, "add") == 0)
            { e = eval2_and_call(arg1, arg2, a, add); goto done; }
        if (strcasecmp(label, "sub") == 0)
            { e = eval2_and_call(arg1, arg2, a, sub); goto done; }
        // todo re-implement all users in original paper's form of label/lambda
        if (strcasecmp(label, "def") == 0) {
            expr *name = _(eval(car(cdr(e)), a));
            expr *args = _(eval(car(cdr(cdr(e))), a));
            expr *func = _(eval(car(cdr(cdr(cdr(e)))), a));
            e = def(name, args, func);
            goto done;
        }
        // todo re-implement def in original paper's form of label/lambda
        for (int defnum = 0; def_atoms[defnum]; defnum++) {
            if (isT(eq(cmd, def_atoms[defnum]))) {
                expr *a2 = pair(def_argsl[defnum], evlis(cdr(e), a), NIL);
                //deb("{{{ calling %s with arglist:", atom_names[def_atoms[defnum]->atom]);
                //dprint(a2);
                e = eval(def_exprs[defnum], a2);
                //deb("}}} result:");
                //dprint(e);
                goto done;
            }
        }
        e = eval(cons(assoc(car(e), a), cdr(e)), a);
        goto done;
    }
    // eval [cons [caddar [e]; cdr [e]]; cons [list [cadar [e]; car [e]; a]];
    if (isT(eq(car(car(e)), find_atom("label")))) {
        expr *label = car(cdr(car(e)));
        expr *to_eval = car(cdr(cdr(car(e))));
        expr *args = cdr(e);
        expr *neweval = _(cons(to_eval, args));
        expr *newpair = _(cons(label, cons(car(e), NIL)));
        e = eval(neweval, cons(newpair, a));
        goto done;
    }
    // eval [caddar [e]; append [pair [cadar [e]; evlis [cdr [e]; a]; a]]]
    if (isT(eq(car(car(e)), find_atom("lambda")))) {
        expr *to_eval = _(car(cdr(cdr(car(e)))));
        expr *argslist = _(car(cdr(car(e))));
        expr *vallist = _(evlis(cdr(e), a));
        expr *lambda_args = _(pair(argslist, vallist, a));
        e = eval(to_eval, lambda_args);
        goto done;
    }
done:
    return pop_gcstack(ll, e);
}

// appq[m] = [null[m] → NIL;T → cons[list[QUOTE;car[m]];appq[cdr[m]]]]
static expr *appq(expr *m)
{
    if (isNIL(m))
        return NIL;
    _(m);
    return cons(cons(find_atom("QUOTE"), _(cons(car(m), NIL))), _(appq(cdr(m))));
}

// apply[f; args] = eval[cons[f; appq[args]]; NIL],
expr *apply(expr *f, expr *args)
{
    expr *q = _(appq(args));
    return pop_gcstack(0, eval(cons(f, q), NIL));
}
