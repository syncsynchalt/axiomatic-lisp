#include "axiom.h"
#include <string.h>
#include <ctype.h>

expr *base_registers[NUM_BASE_REGISTERS];

char *atom_names[MAX_ATOMS] = { "__not_an_atom", "__number", "NIL", };
expr **atom_exprs = base_registers + ATOMS_OFFSET;
expr *freelist = NULL;
expr *arena = NULL;
expr *env = NULL;
expr *NIL;
expr *T;

void init_cells()
{
    arena = calloc(sizeof *arena, MAX_CELLS);
    env = NIL = &arena[0];
    NIL->right = NIL->left = NIL;
    NIL->atom = 2;
    atom_exprs[2] = NIL;
    freelist = &arena[1];
    for (int i = 0; i < MAX_CELLS; i++)
        arena[i].right = &arena[i+1];
    arena[MAX_CELLS-1].right = NIL;
    T = find_atom("T");
}

static void mark(expr *e, int m)
{
    if (m && e->atom < MAX_ATOMS)
        e->atom += MAX_ATOMS;
    if (!m && e->atom >= MAX_ATOMS)
        e->atom -= MAX_ATOMS;
    if (!isNIL(car(e)))
        mark(car(e), m);
    if (!isNIL(cdr(e)))
        mark(cdr(e), m);
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
        if (base_registers[i])
            mark(base_registers[i], 1);
    sweep();
    for (size_t i = 0; i < sizeof(base_registers)/sizeof(*base_registers); i++)
        if (base_registers[i])
            mark(base_registers[i], 0);
}

expr *get_free_cell()
{
    if (isNIL(freelist))
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
