#ifndef INC_MEM_H__
#define INC_MEM_H__

typedef struct _expr expr;
extern void init_cells();
extern expr *get_free_cell(expr *left, expr *right);
extern expr *find_atom(const char *label);

extern expr *arena;
extern expr *freelist;

#endif
