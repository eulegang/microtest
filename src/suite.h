#ifndef __suite_h
#define __suite_h

#include "list_syms.h"

typedef struct {
  char *filename;
  void *handle;
  symbol_searcher searcher;
} microunit_suite;

microunit_suite *mk_microsuite(char *filename);
int run_suite(microunit_suite *suite);
void free_microunit_suite(microunit_suite *suite);

#endif
