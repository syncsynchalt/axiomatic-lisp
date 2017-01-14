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

// makepair[A; B] = (A, B)
static expr *makepair(expr *a, expr *b)
{
    return cons(a, cons(b, NIL));
}

// pair[(A, B, C); (X, (Y, Z), U); ...tail...] = ((A, X), (B, (Y, Z)), (C, U), ...tail...)
static expr *pair(expr *x, expr *y, expr *tail)
{
    if (isNIL(x))
        return tail;
    return cons(makepair(car(x), car(y)), pair(cdr(x), cdr(y), tail));
}

// evlis[(T, F, (ADD, 1, 2))] = (T, F, 3)
static expr *evlis(expr *l, expr *a)
{
    if (isNIL(l))
        return NIL;
    return cons(_(eval(car(l), a)), _(evlis(cdr(l), a)));
}

// evcon[(((EQ, A, B), 1), ((EQ, A, A), 2), ((T), 3))] = 2
static expr *evcon(expr *conditions, expr *a)
{
    if (isNIL(conditions))
        return NIL;
    if (isT(eval(car(car(conditions)), a)))
        return eval(car(cdr(car(conditions))), a);
    return evcon(cdr(conditions), a);
}

expr *defun(expr *ee, expr *a)
{
    expr *name = _(eval(car(ee), a));
    expr *args = _(eval(car(cdr(ee)), a));
    expr *func = _(eval(car(cdr(cdr(ee))), a));
    expr *lambda = cons(find_atom("lambda"), makepair(args, func));
    expr *label = cons(find_atom("label"), makepair(args, lambda));
    env = base_registers[6] = cons(makepair(name, label), env);
    return makepair(find_atom("defun"), name);
}

expr *eval(expr *e, expr *a)
{
    if (isT(atom(e))) {
        return assoc(e, a);
    }
    int ll = cur_eval_gclink;
    _(e); _(a);
    if (isT(atom(car(e)))) {
        expr *cmd = car(e);
        expr *arg1 = car(cdr(e));
        expr *arg2 = car(cdr(cdr(e)));
        const char *label = atom_names[cmd->atom];
        if (isNIL(cmd))                            e = NIL;
        else if (strcasecmp(label, "quote") == 0)  e = arg1;
        else if (strcasecmp(label, "atom") == 0)   e = atom(eval(arg1, a));
        else if (strcasecmp(label, "eq") == 0)     e = eq(_(eval(arg1, a)), _(eval(arg2, a)));
        else if (strcasecmp(label, "cond") == 0)   e = evcon(cdr(e), a);
        else if (strcasecmp(label, "car") == 0)    e = car(eval(arg1, a));
        else if (strcasecmp(label, "cdr") == 0)    e = cdr(eval(arg1, a));
        else if (strcasecmp(label, "cons") == 0)   e = cons(_(eval(arg1, a)), _(eval(arg2, a)));
        else if (strcasecmp(label, "add") == 0)    e = add(_(eval(arg1, a)), _(eval(arg2, a)));
        else if (strcasecmp(label, "sub") == 0)    e = sub(_(eval(arg1, a)), _(eval(arg2, a)));
        else if (strcasecmp(label, "defun") == 0)  e = defun(cdr(e), a);
        else                                       e = eval(cons(assoc(car(e), a), cdr(e)), a);
        goto done;
    }
    // eval [cons [caddar [e]; cdr [e]]; cons [list [cadar [e]; car [e]; a]];
    if (isT(eq(car(car(e)), find_atom("label")))) {
        expr *label = car(cdr(car(e)));
        expr *to_eval = car(cdr(cdr(car(e))));
        expr *args = cdr(e);
        expr *neweval = _(cons(to_eval, args));
        expr *newpair = _(makepair(label, car(e)));
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
    return cons(makepair(find_atom("QUOTE"), car(m)), _(appq(cdr(m))));
}

// apply[f; args] = eval[cons[f; appq[args]]; NIL],
expr *apply(expr *f, expr *args)
{
    expr *q = _(appq(_(args)));
    return pop_gcstack(0, eval(cons(f, q), env));
}
