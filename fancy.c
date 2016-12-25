#include "fancy.h"
#include "axiom.h"
#include "builtins.h"
#include <stdio.h>

static void fancy_print_indented(expr *e, int indent);

void fancy_print(expr *e)
{
    fancy_print_indented(e, 0);
}

static void fancy_print_indented(expr *e, int indent)
{
    printf("%*s", indent, "");
    if (e->atom) {
        printf("%s\n", atom_names[e->atom]);
        return;
    }
    expr *l;
    for (l = e; !l->atom; l = cdr(l)) {
        printf("+---");
    }
    printf("NIL\n");
    printf("%*s", indent, "");
    for (l = e; !l->atom; l = cdr(l)) {
        if (car(l)->atom) {
            if (l->numval) {
                printf("%-3.3d ", l->numval);
            } else {
                printf("%-3.3s ", atom_names[car(l)->atom]);
            }
        } else {
            printf("%3s ", "");
        }
    }
    printf("\n");
    int i;
    for (l = e, i = indent; !l->atom; l = cdr(l), i += 4) {
        if (!car(l)->atom) {
            fancy_print_indented(car(l), i);
        }
    }
}
