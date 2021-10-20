#include "microunit.h"
#include <stdlib.h>
#include <stdio.h>

microunit(basic_fail) {
  fprintf(stderr, "failed to blah blah blah\n"); 
  assert(0, "0 should be false");
}


