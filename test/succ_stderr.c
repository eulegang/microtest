#include "microunit.h"
#include <stdlib.h>
#include <stdio.h>

microunit(basic_fail) {
  dprintf(2, "failed to blah blah blah\n");
  assert(1, "1 should be true");
}

