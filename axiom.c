#include "axiom.h"
#include "parse.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

char *def_names[MAX_DEFS] = {};
expr *def_exprs[MAX_DEFS] = {};
char *atom_names[MAX_ATOMS] = { "__not_an_atom", "__number", };
expr *atom_exprs[MAX_ATOMS] = { NULL, NULL, };
#define ATOM_NUMERIC 1

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--") == 0)
            break;
        FILE *f = fopen(argv[i], "r");
        if (!f)
            die("Unable to open %s: %s", argv[i], strerror(errno));
        parse(f, 0);
    }
    parse(stdin, 0);
    return 0;
}

expr *find_atom(const char *label)
{
    if (label[strspn(label, "0123456789")] == '\0') {
        expr *e = calloc(sizeof *e, 1);
        e->atom = ATOM_NUMERIC;
        e->numval = atoi(label);
        return e;
    }
    int i = 0;
    for (i = 0; i < MAX_ATOMS; i++) {
        if (!atom_names[i])
            break;
        if (strcasecmp(label, atom_names[i]) == 0)
            return atom_exprs[i];
    }
    if (i >= MAX_ATOMS)
        die("More than %d atoms!\n", MAX_ATOMS);
    atom_names[i] = strdup(label);
    for (int ii = 0; atom_names[i][ii]; ii++)
        atom_names[i][ii] = toupper(atom_names[i][ii]);
    atom_exprs[i] = calloc(sizeof(expr), 1);
    atom_exprs[i]->atom = i;
    return atom_exprs[i++];
}
