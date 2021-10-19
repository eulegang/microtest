#include "microunit.h"

microunit(basic_passing) {
  it ("should pass");
  assert(1, "1 should be true");
}

