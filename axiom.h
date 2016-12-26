#ifndef INC_AXIOM_H__
#define INC_AXIOM_H__

#define MAX_ATOMS 1000
#define MAX_DEFS 1000
#define MAX_CELLS 2000

typedef struct _expr {
    int atom;  // 0 if non-atom
    int numval; // only valid if atom == 1 (__number)
    struct _expr *left;
    struct _expr *right;
} expr; 

extern int   def_atoms[MAX_DEFS];
extern expr *def_argsl[MAX_DEFS];
extern expr *def_exprs[MAX_DEFS];
extern char *atom_names[MAX_ATOMS];
extern expr *atom_exprs[MAX_ATOMS];
#define ATOM_NUMERIC 1

extern expr *NIL;

#include <stdio.h>
#define die(...) do { fprintf (stderr, __VA_ARGS__); exit(1); } while (0)
// remove for golfing
#define deb(f, ...) do { fprintf (stderr, f "\n", ##__VA_ARGS__); } while (0)

#include "builtins.h"
#include "eval.h"
#include "parse.h"
#include "mem.h"

#endif
