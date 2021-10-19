#ifndef LIST_SYMS_H
#define LIST_SYMS_H

typedef void* symbol_searcher; 

symbol_searcher mk_symbol_searcher(char *filename);
char *symbol_searcher_next_symbol(symbol_searcher);
void free_symbol_searcher(symbol_searcher);

#endif
