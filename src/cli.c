#include <stdlib.h>
#include <stdio.h>

#include "cli.h"

const char* VERSION = "0.1.0";

static struct option cli_options[] = {
  { "verbose", no_argument, 0, 'v' },
  { "quiet", no_argument, 0, 'q' },
  { "help", no_argument, 0, 'h' },
  { "dir", required_argument, 0, 'd' },
  { "version", no_argument, 0, 'V' },

  { 0, 0, 0, 0}
};

void show_help();
void show_version();

cli_opts build_options(int argc, char** argv) {
  int c = 0;

  cli_opts cli = { .build_dir = ".", .verbose = 0 };

  int option_index = 0;
  while (1) {
    c = getopt_long(argc, argv, "vVqhd:", cli_options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 'h':
        show_help();
        break;

      case 'v':
        cli.verbose = 1;
        break;

      case 'q':
        cli.quiet = 1;
        break;

      case 'V':
        show_version();
        break;

      case 'd':
        cli.build_dir = optarg;
        break;

      case '?':
        break;

      default:
        abort();
    }
  }

  if (cli.quiet && cli.verbose) {
    fprintf(stderr, "microunit does not run in both quiet and verbose modes\n");
    exit(2);
  }

  return cli;
}

void show_help() {
  fprintf(stdout, "microunit [-d .] [-v] [-V]\n");
  
  exit(0);
}

void show_version() {
  fprintf(stdout, "%s\n", VERSION);

  exit(0);
}
