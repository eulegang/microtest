#include <stdlib.h>
#include <dbg.h>
#include <dlfcn.h>
#include <alloca.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "suite.h"
#include "microunit.h"

microunit_suite *mk_microsuite(char *filename) {
  microunit_suite* suite = malloc(sizeof(microunit_suite));
  check_mem(suite);
  
  suite->filename = filename;
  suite->searcher = mk_symbol_searcher(filename);

  check(suite->searcher, "failed to create searcher");

  suite->handle = dlopen(filename, RTLD_NOW);
  check(suite->handle, "failed to load: %s", dlerror());
  dlerror();

  return suite;
error:
  free_microunit_suite(suite);
  return NULL;
}

void free_microunit_suite(microunit_suite *suite) {
  if (!suite) return;

  if (suite->handle) dlclose(suite->handle);
  if (suite->searcher) free_symbol_searcher(suite->searcher);
}

char* __suite_find_name(char *file) {
  char *start, *cur, *end;

  start = cur = file;

  while (*cur) {
    if (*cur == '/')
      start = cur;
    cur++;
  }

  start++;

  cur = end = start;

  while (*cur) {
    if (*cur == '.')
      end = cur;
    cur++;
  }

  size_t len = end - start;
  char *copy = malloc((len + 1)* sizeof(char));
  memcpy(copy, start, len);
  copy[len] = 0;

  return copy;
}

int hookstream(int fd);

extern int snull;

records_t run_suite(microunit_suite *suite, int flags) {
  void (*test_sym)(microunit_ctx*);
  char *sym = symbol_searcher_next_symbol(suite->searcher);

  char *name = __suite_find_name(suite->filename);

  records_t records = mk_records(name);

  while (sym) {
    if (strncmp(sym, "__microunit_", 12) == 0) {
      int bstdout, bstderr;

      if (flags & MICRO_SUITE_VERBOSE) {
        bstdout = hookstream(1);
      } else {
        dup2(snull, 1);
        bstdout = snull;
      }

      if (flags & MICRO_SUITE_QUIET) {
        dup2(snull, 2);
        bstderr = snull;
      } else {
        bstderr = hookstream(2);
      }

      context_t ctx;
      ctx.current_test = sym + 12; //cut out the __
      ctx.suite = suite;
      ctx.msg = 0;

      test_sym = (void (*)(microunit_ctx*)) dlsym(suite->handle, sym);

      test_sym((microunit_ctx *)&ctx);

      fflush(stdout);
      fflush(stderr);

      records_push(&records, &ctx, bstdout, bstderr);
    }

    sym = symbol_searcher_next_symbol(suite->searcher);
  }

  return records;
}

int hookstream(int fd) {
  int fds[2];
  int res;
  int flags;

  res = pipe(fds);
  check(res != -1, "failed to pipe");


  res = close(fd);
  check(res != -1, "failed to copy pipe to standard file descriptor");
  res = dup2(fds[1], fd);
  check(res != -1, "failed to copy pipe to standard file descriptor");
  
  flags = fcntl(fds[0], F_GETFL);
  flags |= O_NONBLOCK;
  check(!fcntl(fds[0], F_SETFL, flags), "failed to set nonblocking");

  return fds[0];

error:
  return -1;
}

