#include <stdio.h>
#include <dbg.h>

#include "list_syms.h"

int main(int argc, char** argv) {
  for (int i = 1; i < argc; i++) {
    debug("inspecting %s", argv[i]);

    symbol_dict_t *dict = mk_symbol_dict(argv[i]);
    if (!dict) continue;

    debug("checking symbols");

    for (int i = 0; i < 4096; i++) {
      if (dict->buffer[i]) {
        debug("found symbol %s", dict[i]);
      } else {
        continue;
      }
    }

    debug("done with %s", argv[i]);

    free_symbol_dict(dict);
  }

  return 0;
}


