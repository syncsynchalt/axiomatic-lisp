#include "axiom.h"
#include <ctype.h>
#include <stdlib.h>

static expr *parse_read_atom(FILE *f);

expr *parse(FILE *f, int level)
{
    expr *e = NIL;
    expr *tip = e;
    expr *next = NULL;
    for (;;) {
        int c = fgetc(f);
        if (c < 0)
            break;
        switch (c) {
        case ';': // comment
            while (c != ';' && !feof(f))
                c = fgetc(f);
            break;
        case '(':
            next = parse(f, level+1);
            if (tip->atom == NIL->atom) {
                e = tip = cons(next, NIL);
            } else {
                tip->right = cons(next, NIL);
                tip = tip->right;
            }
            break;
        case ')':
            if (level == 1) {
                e = eval(e, NIL);
                print(e);
            }
            return e;
            break;
        case ' ': case '\n': case '\r': case '\t':
            break;
        default:
            ungetc(c, f);
            next = parse_read_atom(f);
            if (e->atom == NIL->atom) {
                e = tip = cons(next, NIL);
            } else {
                tip->right = cons(next, NIL);
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

static void parse_print_expr(FILE *f, expr *e, int last_was_atom)
{
    if (last_was_atom)
        fprintf(f, " ");
    if (e->atom == ATOM_NUMERIC) {
        fprintf(f, "%d", e->numval);
    } else if (e->atom) {
        fprintf(f, "%s", atom_names[e->atom]);
    } else {
        fprintf(f, "(");
        for (int i = 0; e->atom != NIL->atom; i++, e = cdr(e)) {
            parse_print_expr(f, car(e), i);
        }
        fprintf(f, ")");
    }
}

void print(expr *e)
{
    parse_print_expr(stdout, e, 0);
    fprintf(stdout, "\n");
}

void dprint(expr *e)
{
    parse_print_expr(stderr, e, 0);
    fprintf(stderr, "\n");
}
