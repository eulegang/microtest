#include <stdlib.h>
#include <dbg.h>
#include <dlfcn.h>

#include "suite.h"
#include "microunit.h"

microunit_suite *mk_microsuite(char *filename) {
  microunit_suite* suite = malloc(sizeof(microunit_suite));
  check_mem(suite);
  
  suite->filename = filename;
  suite->searcher = mk_symbol_searcher(filename);

  check(suite->searcher, "failed to create searcher");

  suite->handle = dlopen(filename, RTLD_NOW);
  check(suite->handle, "failed to load: %s", dlerror());
  dlerror();

  return suite;
error:
  free_microunit_suite(suite);
  return NULL;
}

void free_microunit_suite(microunit_suite *suite) {
  if (!suite) return;

  if (suite->handle) dlclose(suite->handle);
  if (suite->searcher) free_symbol_searcher(suite->searcher);
}

char* __suite_find_name(char *file) {
  char *start, *cur, *end;

  start = cur = file;

  while (*cur) {
    if (*cur == '/')
      start = cur;
    cur++;
  }

  start++;

  cur = end = start;

  while (*cur) {
    if (*cur == '.')
      end = cur;
    cur++;
  }

  size_t len = end - start;
  char *copy = malloc((len + 1)* sizeof(char));
  memcpy(copy, start, len);
  copy[len] = 0;

  return copy;
}

records_t run_suite(microunit_suite *suite) {
  void (*test_sym)(microunit_ctx*);
  char *sym = symbol_searcher_next_symbol(suite->searcher);

  char *name = __suite_find_name(suite->filename);

  records_t records = mk_records(name);


  while (sym) {
    if (strncmp(sym, "__microunit_", 12) == 0) {

      context_t ctx;
      ctx.current_test = sym + 12; //cut out the __
      ctx.suite = suite;
      ctx.msg = 0;

      test_sym = (void (*)(microunit_ctx*)) dlsym(suite->handle, sym);

      test_sym((microunit_ctx *)&ctx);

      records_push(&records, &ctx);
    }

    sym = symbol_searcher_next_symbol(suite->searcher);
  }

  return records;
}
