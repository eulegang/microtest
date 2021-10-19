#include <stdlib.h>
#include <dbg.h>
#include <dlfcn.h>

#include "suite.h"
#include "context.h"
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

int run_suite(microunit_suite *suite) {
  void (*test_sym)(microunit_ctx*);
  char *sym = symbol_searcher_next_symbol(suite->searcher);

  while (sym) {
    if (strncmp(sym, "__microunit_", 12) == 0) {

      context_t ctx;
      ctx.current_test = sym + 12; //cut out the __
      ctx.suite = suite;

      test_sym = (void (*)(microunit_ctx*)) dlsym(suite->handle, sym);

      test_sym((microunit_ctx *)&ctx);
    }

    sym = symbol_searcher_next_symbol(suite->searcher);
  }

  return 0;
}
