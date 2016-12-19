#include "execute.h"
#include "axiom.h"
#include "builtins.h"
#include <strings.h>

expr *execute(expr *e)
{
    if (!e->left || !e->left->atom)
        return e;
    expr *cmd = e->left;
    const char *label = atom_names[cmd->atom];
    if (strcasecmp(label, "car") == 0)
        return axiom_car(e->right);
    if (strcasecmp(label, "cdr") == 0)
        return axiom_cdr(e->right);
    if (strcasecmp(label, "atom") == 0)
        return axiom_atom(e->right);
    if (strcasecmp(label, "eq") == 0)
        return axiom_atom(e->right);
    if (strcasecmp(label, "cons") == 0)
        return axiom_cons(e->right->left, e->right->right->left);
    return e;
}
