#ifndef __cli_h
#define __cli_h

#include <getopt.h>

typedef struct {
  char *build_dir;
  int verbose;
  int quiet;
} cli_opts;

cli_opts build_options(int argc, char** argv);

#endif
