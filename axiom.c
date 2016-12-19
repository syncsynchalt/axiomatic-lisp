#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>

#define MAX_ATOMS 1000
#define MAX_DEFS 1000

typedef struct _expr {
    int atom;  // 0 if non-atom
    int numval; // only valid if atom == 1 (__number)
    int bound; // indicates binding to a runtime arg
    struct _expr *left;
    struct _expr *right;
} expr; 

char *def_names[MAX_DEFS] = {};
expr *def_exprs[MAX_DEFS] = {};
char *atom_names[MAX_ATOMS] = { "__not_an_atom", "__number", };
expr *atom_exprs[MAX_ATOMS] = { NULL, NULL, };
#define ATOM_NUMERIC 1
int num_atoms = 2;

static expr *axiom_atom(expr *e);
static expr *axiom_car(expr *e);
static expr *axiom_cdr(expr *e);
static expr *axiom_cons(expr *a, expr *b);
static expr *axiom_eq(expr *e);

static expr *builtin_add(expr *e);
static expr *builtin_sub(expr *e);
static expr *builtin_def(expr *e);

#define die(...) do { fprintf (stderr, __VA_ARGS__); exit(1); } while (0)

static expr *parse(FILE *f, int level);

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

static expr *find_atom(const char *label)
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

static void parse_print_expr(expr *e, int last_was_atom);
static expr *parse_read_atom(FILE *f);

static expr *parse(FILE *f, int level)
{
    expr *nila = find_atom("NIL");
    expr *e = nila;
    expr *tip = e;
    expr *next = NULL;
    for (;;) {
        int c = fgetc(f);
        if (c < 0)
            break;
        switch (c) {
        case '(':
            next = parse(f, level+1);
            if (e->atom == nila->atom) {
                e = tip = next;
            } else {
                tip->right = axiom_cons(next, nila);
                tip = tip->right;
            }
            break;
        case ')':
            if (level == 1) {
                parse_print_expr(e, 0);
                printf("\n");
            }
            return e;
            break;
        case ' ': case '\n': case '\r': case '\t':
            break;
        default:
            ungetc(c, f);
            next = parse_read_atom(f);
            if (e->atom == nila->atom) {
                e = tip = axiom_cons(next, nila);
            } else {
                tip->right = axiom_cons(next, nila);
                tip = tip->right;
            }
            break;
        }
    }
    return e;
}

static expr *parse_read_atom(FILE *f)
{
    char *buf = NULL;
    int buflen = 0;
    for (;;) {
        int c = fgetc(f);
        if (c < 0)
            die("End of input");
        switch (c) {
        case ' ': case '\t': case '\r': case '\n':
            return find_atom(buf);
        case '(': case ')':
            ungetc(c, f);
            return find_atom(buf);
        default:
            if (buflen % 5 == 0)
                buf = realloc(buf, buflen+6);
            buf[buflen++] = c;
            buf[buflen] = 0;
            break;
        }
    }
}

static void parse_print_expr(expr *e, int last_was_atom)
{
    if (last_was_atom)
        printf(" ");
    if (e->atom) {
        printf("%s", atom_names[e->atom]);
    } else {
        printf("(");
        for (int i = 0;; i++) {
            parse_print_expr(e->left, i);
            if (e->right->atom == find_atom("NIL")->atom)
                break;
            e = e->right;
        }
        printf(")");
    }
}

expr *axiom_atom(expr *e)
{
    if (e->atom)
        return find_atom("T");
    else
        return find_atom("F");
}

expr *axiom_car(expr *e)
{
    if (e->atom || !e->left)
        return find_atom("NIL");
    return e->left;
}

expr *axiom_cdr(expr *e)
{
    if (e->atom || !e->right)
        return find_atom("NIL");
    return e->right;
}

expr *axiom_cons(expr *a, expr *b)
{
    expr *e = calloc(sizeof *e, 1);
    e->left = a;
    e->right = b;
    return e;
}

expr *axiom_eq(expr *e)
{
    if (!e->left || !e->right || !e->left->atom || !e->right->atom)
        return find_atom("NIL");
    if (e->left->atom == e->right->atom && (e->left->numval == e->right->numval))
        return find_atom("T");
    else
        return find_atom("F");
}

static expr *builtin_add(expr *e)
{
    if (e->atom || e->left->atom != ATOM_NUMERIC || e->right->atom != ATOM_NUMERIC)
        return find_atom("NIL");
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = e->left->numval + e->right->numval;
    return result;
}

static expr *builtin_sub(expr *e)
{
    if (e->atom || e->left->atom != ATOM_NUMERIC || e->right->atom != ATOM_NUMERIC)
        return find_atom("NIL");
    expr *result = calloc(sizeof *result, 1);
    result->atom = ATOM_NUMERIC;
    result->numval = e->left->numval - e->right->numval;
    return result;
}

static expr *builtin_def_bind(expr *args, expr *func);

static expr *builtin_def(expr *e)
{
    expr *name = axiom_car(e);
    expr *args = axiom_car(axiom_cdr(e));
    expr *func = axiom_cdr(axiom_cdr(e));
    int i = 0;
    for (i = 0; i < MAX_DEFS && def_names[i]; i++)
        ;
    if (i >= MAX_DEFS)
        die("More than %d defs!\n", MAX_DEFS);
    def_names[i] = atom_names[name->atom];
    return def_exprs[i] = builtin_def_bind(args, func);
}

static expr *builtin_def_bind(expr *args, expr *func)
{
    if (func->left) {
        builtin_def_bind(args, func->left);
    } else if (func->right) {
        builtin_def_bind(args, func->right);
    } else if (func->atom) {
        int index = 0;
        for (expr *l = args; l->atom == 0; l = args->right, index++) {
            if (l->left->atom == func->atom) {
                func->atom = func->numval = 0;
                func->bound = index;
            }
        }
    }
    return func;
}
