#include <stdio.h>
#include <dbg.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>

#include "list_syms.h"
#include "microunit.h"
#include "suite.h"
#include "cli.h"

#define MAX_BUILD_DIR_SIZE 1024

int main(int argc, char** argv) {
  cli_opts opts = build_options(argc, argv);
  
  DIR* dir = opendir(opts.build_dir);

  if (dir == NULL) {
    log_err("failed to read directory: %s", opts.build_dir);
    return 1;
  }

  int suites_len = 0;
  char **suites = malloc(MAX_BUILD_DIR_SIZE * sizeof(char*));

  size_t dir_len = strlen(opts.build_dir);
  struct dirent *d_entry;

  while ((d_entry = readdir(dir)) != NULL) {
    if (d_entry->d_type != DT_REG)
      continue;

    size_t len = strlen(d_entry->d_name);

    if (len < 10)
      continue;

    char *name = d_entry->d_name + (len - 10);

    if (strncmp(name, ".microtest", 10) != 0)
      continue;
    
    char *suite = malloc((len + dir_len) + 2 * sizeof(char));

    memcpy(suite, (void*) opts.build_dir, dir_len);
    suite[dir_len] = '/';
    memcpy(suite + dir_len+1, (void*) d_entry->d_name, len);
    suite[dir_len + 1 + len] = 0;

    suites[suites_len++] = suite;
  }


  for (int i = 0; i < suites_len; i++) {
    microunit_suite *suite = mk_microsuite(suites[i]);

    records_t records = run_suite(suite);

    report_records(records);

    free_records(records);
    free_microunit_suite(suite);
  }


  for (int i = 0; i < suites_len; i++) {
    free(suites[i]);
  }

  free(suites);

  return 0;
}

