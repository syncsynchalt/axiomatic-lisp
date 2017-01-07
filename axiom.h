#ifndef INC_AXIOM_H__
#define INC_AXIOM_H__

#define MAX_ATOMS 1000
#define MAX_DEFS 1000
#define MAX_CELLS 1000
#define MAX_PARSE 1000
#define MAX_EVAL 1000

typedef struct _expr {
    int atom;  // 0 if non-atom
    int numval; // only valid if atom == ATOM_NUMERIC
    struct _expr *left;
    struct _expr *right;
} expr; 

extern char *atom_names[MAX_ATOMS];

extern expr  *def_atoms[MAX_DEFS];
extern expr **def_argsl;
extern expr **def_exprs;
extern expr **atom_exprs;
extern expr **parse_chain;
#define NUM_BASE_REGISTERS (6+MAX_DEFS+MAX_DEFS+MAX_ATOMS+MAX_PARSE+MAX_EVAL)
extern expr *base_registers[NUM_BASE_REGISTERS];
#define ARGSL_OFFSET (6)
#define EXPRS_OFFSET (6+MAX_DEFS)
#define ATOMS_OFFSET (6+MAX_DEFS+MAX_DEFS)
#define PARSE_OFFSET (6+MAX_DEFS+MAX_DEFS+MAX_ATOMS)
#define EVALS_OFFSET (6+MAX_DEFS+MAX_DEFS+MAX_ATOMS+MAX_PARSE)

#define ATOM_NUMERIC 1

extern expr *NIL;
extern expr *T;
#define isNIL(e) ((e)->atom == NIL->atom ? 1 : 0)
#define isT(e)   ((e)->atom ==   T->atom ? 1 : 0)

#include <stdio.h>
#define die(...) do { fprintf (stderr, __VA_ARGS__); exit(1); } while (0)
// remove for golfing
#define deb(f, ...) do { fprintf (stderr, f "\n", ##__VA_ARGS__); } while (0)

#include "builtins.h"
#include "eval.h"
#include "parse.h"
#include "mem.h"

#endif
