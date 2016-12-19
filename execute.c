#include "execute.h"
#include "axiom.h"
#include "builtins.h"
#include <strings.h>

expr *execute(expr *e)
{
    if (!e->left || !e->left->atom)
        return e;
    expr *cmd = car(e);
    expr *args = cdr(e);
    const char *label = atom_names[cmd->atom];
    if (strcasecmp(label, "car") == 0)
        return car(args->left);
    if (strcasecmp(label, "cdr") == 0)
        return cdr(args->left);
    if (strcasecmp(label, "atom") == 0)
        return atom(args->left);
    if (strcasecmp(label, "eq") == 0)
        return eq(args);
    if (strcasecmp(label, "cons") == 0)
        return cons(car(args), car(cdr(args)));
    if (strcasecmp(label, "add") == 0)
        return add(args);
    if (strcasecmp(label, "sub") == 0)
        return sub(args);
    if (strcasecmp(label, "def") == 0)
        return def(args);
    return e;
}
