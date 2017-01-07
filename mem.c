#include "axiom.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

expr *base_registers[NUM_BASE_REGISTERS];

int   def_atoms[MAX_DEFS] = {};
expr **def_argsl = base_registers + ARGSL_OFFSET;
expr **def_exprs = base_registers + EXPRS_OFFSET;
char *atom_names[MAX_ATOMS] = { "__not_an_atom", "__number", "NIL", };
expr **atom_exprs = base_registers + ATOMS_OFFSET;
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
    if (e->atom >= MAX_ATOMS)
        e->atom -= MAX_ATOMS;
    if (e->left->atom != NIL->atom)
        unmark(e->left);
    if (e->right->atom != NIL->atom)
        unmark(e->right);
}

static int sweep()
{
    int count = 0;
    for (size_t i = 1; i < MAX_CELLS; i++) {
        if (arena[i].atom < MAX_ATOMS) {
            count++;
            arena[i].left = NIL;
            arena[i].atom = arena[i].numval = 0;
            arena[i].right = freelist;
            freelist = &arena[i];
        }
    }
    if (!count)
        die("Unable to find free memory");
    return count;
}

static void gc() 
{
    for (size_t i = 0; i < sizeof(base_registers)/sizeof(*base_registers); i++)
        if (base_registers[i]) mark(base_registers[i]);
    sweep();
    for (size_t i = 0; i < sizeof(base_registers)/sizeof(*base_registers); i++)
        if (base_registers[i]) unmark(base_registers[i]);
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
