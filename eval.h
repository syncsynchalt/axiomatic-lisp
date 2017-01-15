#ifndef INC_EVAL_H__
#define INC_EVAL_H__

typedef struct _expr expr;
extern expr *apply(expr *f, expr *args);

#endif
