#ifndef __cli_h
#define __cli_h

#include <getopt.h>

typedef struct {
  char *build_dir;
  int verbose;
} cli_opts;

cli_opts build_options(int argc, char** argv);

#endif
