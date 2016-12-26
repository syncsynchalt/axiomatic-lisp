#include "axiom.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int   def_atoms[MAX_DEFS] = {};
expr *def_argsl[MAX_DEFS] = {};
expr *def_exprs[MAX_DEFS] = {};
char *atom_names[MAX_ATOMS] = { "__not_an_atom", "__number", "NIL", };
expr *atom_exprs[MAX_ATOMS] = { NULL, NULL, NULL, };
expr *freelist = NULL;
expr *arena = NULL;
expr *NIL;

void init_cells()
{
    arena = calloc(sizeof *arena, MAX_CELLS);
    NIL = &arena[0];
    NIL->right = NIL->left = NIL;
    NIL->atom = 2;
    atom_exprs[2] = NIL;
    freelist = &arena[1];
    for (int i = 0; i < MAX_CELLS; i++)
        arena[i].right = &arena[i+1];
    arena[MAX_CELLS-1].right = NIL;
}

static void mark(expr *e)
{
    if (e->atom < MAX_ATOMS)
        e->atom += MAX_ATOMS;
    if (e->left->atom != NIL->atom)
        mark(e->left);
    if (e->right->atom != NIL->atom)
        mark(e->right);
}

static void unmark(expr *e)
{
    if (e->atom > MAX_ATOMS)
        e->atom -= MAX_ATOMS;
    if (e->left->atom != NIL->atom)
        unmark(e->left);
    if (e->right->atom != NIL->atom)
        unmark(e->right);
}

static void sweep()
{
    for (size_t i = 1; i < MAX_CELLS; i++) {
        if (arena[i].atom < MAX_ATOMS) {
            arena[i].left = NIL;
            arena[i].atom = arena[i].numval = 0;
            arena[i].right = freelist;
            freelist = &arena[i];
        }
    }
}

static void gc() 
{
    fprintf(stderr, "\n\n--- garbage collect ---\n\n");
    for (size_t i = 0; i < MAX_DEFS; i++) if (def_argsl[i]) mark(def_argsl[i]);
    for (size_t i = 0; i < MAX_DEFS; i++) if (def_exprs[i]) mark(def_exprs[i]);
    for (size_t i = 0; i < MAX_ATOMS; i++) if (atom_exprs[i]) mark(atom_exprs[i]);
    sweep();
    for (size_t i = 0; i < MAX_DEFS; i++) if (def_argsl[i]) unmark(def_argsl[i]);
    for (size_t i = 0; i < MAX_DEFS; i++) if (def_exprs[i]) unmark(def_exprs[i]);
    for (size_t i = 0; i < MAX_ATOMS; i++) if (atom_exprs[i]) unmark(atom_exprs[i]);
}

expr *get_free_cell()
{
    if (freelist->atom == NIL->atom)
        gc();
    expr *e = freelist;
    freelist = freelist->right;
    e->left = e->right = NIL;
    e->atom = e->numval = 0;
    return e;
}

expr *find_atom(const char *label)
{
    if (label[strspn(label, "0123456789")] == '\0') {
        expr *e = get_free_cell();
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
    atom_exprs[i] = get_free_cell();
    atom_exprs[i]->atom = i;
    return atom_exprs[i];
}
