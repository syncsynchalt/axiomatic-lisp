#include "axiom.h"
#include <ctype.h>

static expr *parse_read_atom(FILE *f);
expr **parse_chain = base_registers + PARSE_OFFSET;

expr *parse(FILE *f, int level)
{
    if (level >= MAX_PARSE)
        die("More than %d parse levels!\n", MAX_PARSE);
    expr *e = NIL;
    expr *tip = e;
    expr *next = NULL;
    for (;;) {
        int c = fgetc(f);
        if (c < 0)
            break;
        switch (c) {
        case ';': // comment
            while (c != '\n' && !feof(f))
                c = fgetc(f);
            break;
        case '(':
            next = parse(f, level+1);
            if (isNIL(tip)) {
                parse_chain[level] = e = tip = cons(next, NIL);
            } else {
                tip->right = cons(next, NIL);
                tip = tip->right;
            }
            break;
        case ')':
            if (level == 1) {
                e = apply(car(e), cdr(e));
                print(stdout, e);
            }
            goto done;
        case ' ': case '\n': case '\r': case '\t':
            break;
        default:
            ungetc(c, f);
            next = parse_read_atom(f);
            if (isNIL(e)) {
                parse_chain[level] = e = tip = cons(next, NIL);
            } else {
                tip->right = cons(next, NIL);
                tip = tip->right;
            }
            break;
        }
    }
done:
    parse_chain[level] = NULL;
    return e;
}

static expr *parse_read_atom(FILE *f)
{
    static char *buf = NULL;
    static int buflen = 0;
    for (buflen = 0;;) {
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
    } else if (isT(atom(e))) {
        fprintf(f, "%s", atom_names[e->atom]);
    } else {
        fprintf(f, "(");
        for (int i = 0; !isNIL(e); i++, e = cdr(e)) {
            parse_print_expr(f, car(e), i);
        }
        fprintf(f, ")");
    }
}

void print(FILE *f, expr *e)
{
    parse_print_expr(f, e, 0);
    fprintf(f, "\n");
}
