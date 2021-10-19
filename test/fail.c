#include "microunit.h"

microunit(basic_fail) {
  it ("should fail");
  assert(0, "0 should be false");
}

