#include "parse.h"
#include "axiom.h"
#include "builtins.h"
#include "execute.h"
#include "fancy.h"
#include <ctype.h>
#include <stdlib.h>

static void parse_print_expr(expr *e, int last_was_atom);
static expr *parse_read_atom(FILE *f);

expr *parse(FILE *f, int level)
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
                tip->right = cons(next, nila);
                tip = tip->right;
            }
            break;
        case ')':
            e = execute(e);
            if (level == 1) {
                if (getenv("FANCY")) {
                    fancy_print(e);
                } else {
                    parse_print_expr(e, 0);
                    printf("\n");
                }
            }
            return e;
            break;
        case ' ': case '\n': case '\r': case '\t':
            break;
        default:
            ungetc(c, f);
            next = parse_read_atom(f);
            if (e->atom == nila->atom) {
                e = tip = cons(next, nila);
            } else {
                tip->right = cons(next, nila);
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
    if (e->atom == ATOM_NUMERIC) {
        printf("%d", e->numval);
    } else if (e->atom) {
        printf("%s", atom_names[e->atom]);
    } else {
        printf("(");
        for (int i = 0; e->atom != find_atom("NIL")->atom; i++, e = cdr(e)) {
            parse_print_expr(car(e), i);
        }
        printf(")");
    }
}
