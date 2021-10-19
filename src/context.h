#ifndef __context_h
#define __context_h

#include "suite.h"

typedef struct {
  char *current_test;
  microunit_suite *suite;

} context_t;

#endif
