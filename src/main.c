#include <stdio.h>
#include <dbg.h>
#include <dlfcn.h>

#include "list_syms.h"
#include "microunit.h"

int main(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    microunit_suite *suite = mk_microsuite(argv[i]);

    run_suite(suite);

    free_microunit_suite(suite);
  }

  return 0;
}

