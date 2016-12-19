#include "execute.h"
#include "axiom.h"
#include "builtins.h"
#include <strings.h>

expr *execute(expr *e)
{
    if (!e->left || !e->left->atom)
        return e;
    expr *cmd = e->left;
    expr *args = e->right;
    const char *label = atom_names[cmd->atom];
    if (strcasecmp(label, "car") == 0)
        return axiom_car(args->left);
    if (strcasecmp(label, "cdr") == 0)
        return axiom_cdr(args->left);
    if (strcasecmp(label, "atom") == 0)
        return axiom_atom(args->left);
    if (strcasecmp(label, "eq") == 0)
        return axiom_eq(args);
    if (strcasecmp(label, "cons") == 0)
        return axiom_cons(args->left, args->right->left);
    if (strcasecmp(label, "add") == 0)
        return builtin_add(args);
    if (strcasecmp(label, "sub") == 0)
        return builtin_sub(args);
    if (strcasecmp(label, "def") == 0)
        return builtin_def(args);
    return e;
}
