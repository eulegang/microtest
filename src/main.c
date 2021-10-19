#include <stdio.h>
#include <dbg.h>

#include "list_syms.h"

int main(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    debug("inspecting %s", argv[i]);

    symbol_searcher searcher = mk_symbol_searcher(argv[i]);
    if (!searcher) continue;

    debug("checking symbols");

    char *sym = symbol_searcher_next_symbol(searcher);

    while (sym) {
      debug("found symbol: %s", sym);

      sym = symbol_searcher_next_symbol(searcher);
    }

    debug("done with %s", argv[i]);

    free_symbol_searcher(searcher);
  }

  return 0;
}


