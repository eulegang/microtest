#ifndef __suite_h
#define __suite_h

#include "list_syms.h"

typedef enum {
  STATUS_PASS,
  STATUS_FAIL,
} status_t;

typedef struct {
  char* testname;
  char* msg;
  char* out;
  char* err;
  status_t  status;
} record_t;

typedef struct {
  size_t len;
  record_t *records;
  char* suite_name;
} records_t;

typedef struct {
  char *filename;
  void *handle;
  symbol_searcher searcher;
} microunit_suite;

typedef struct {
  char *current_test;
  microunit_suite *suite;
  char *msg;
} context_t;

microunit_suite *mk_microsuite(char *filename);
records_t run_suite(microunit_suite *suite);
void free_microunit_suite(microunit_suite *suite);

records_t mk_records(char *suite_name);
void records_push(records_t*, context_t*, int, int);
void free_records(records_t);

void report_records(records_t);

#endif
