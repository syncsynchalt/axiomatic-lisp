#ifndef INC_PARSE_H__
#define INC_PARSE_H__

typedef struct _expr expr;
#include <stdio.h>
extern expr *parse(FILE *f, int level);
extern void print(FILE *f, expr *e);

#endif
