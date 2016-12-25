#include "axiom.h"
#include <stdio.h>

static void fancy_print_indented(expr *e, int indent);

void fancy_print(expr *e)
{
    fancy_print_indented(e, 0);
}

static void fancy_print_indented(expr *e, int indent)
{
    fprintf(stderr, "%*s", indent, "");
    if (e->atom) {
        fprintf(stderr, "%s\n", atom_names[e->atom]);
        return;
    }
    expr *l;
    for (l = e; !l->atom; l = cdr(l)) {
        fprintf(stderr, "+---");
    }
    fprintf(stderr, "NIL\n");
    fprintf(stderr, "%*s", indent, "");
    for (l = e; !l->atom; l = cdr(l)) {
        if (car(l)->atom == ATOM_NUMERIC) {
            fprintf(stderr, "%-3.3d ", l->numval);
        } else if (car(l)->atom) {
            fprintf(stderr, "%-3.3s ", atom_names[car(l)->atom]);
        } else {
            fprintf(stderr, "%3s ", "");
        }
    }
    fprintf(stderr, "\n");
    int i;
    for (l = e, i = indent; !l->atom; l = cdr(l), i += 4) {
        if (!car(l)->atom) {
            fancy_print_indented(car(l), i);
        }
    }
}
