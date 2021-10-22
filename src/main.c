#include <stdio.h>
#include <dbg.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include <unistd.h>

#include "list_syms.h"
#include "microunit.h"
#include "suite.h"
#include "cli.h"

#define MAX_BUILD_DIR_SIZE 1024
#define MAX_GLOB_PATTERN_SIZE 4096

int sout, serr;

int main(int argc, char** argv) {
  cli_opts opts = build_options(argc, argv);

  char *glob_pattern = malloc(MAX_GLOB_PATTERN_SIZE);
  strncat(glob_pattern, opts.build_dir, MAX_GLOB_PATTERN_SIZE); 
  strncat(glob_pattern, "/*.microunit", MAX_GLOB_PATTERN_SIZE); 
  glob_t globbuf;

  glob(glob_pattern, 0, NULL, &globbuf);

  sout = dup(1);
  serr = dup(2);

  for (size_t i = 0; i < globbuf.gl_pathc; i++) {
    char *filename = globbuf.gl_pathv[i];
    microunit_suite *suite = mk_microsuite(filename);

    records_t records = run_suite(suite);

    report_records(records);

    free_records(records);
    free_microunit_suite(suite);
  }

  globfree(&globbuf);

  return 0;
}

