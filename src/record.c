#include <stdlib.h>
#include <dbg.h>

#include "suite.h"

records_t mk_records(char *suite_name) {
  record_t *rec = malloc(1024 * sizeof(record_t));

  records_t records = {
    .len = 0,
    .records = rec,
    .suite_name = suite_name
  };

  return records;
}

void free_records(records_t records) {
  // TODO: free(records.suite_name);
 
  for (size_t i = 0; i < records.len; i++) {
    record_t rec = records.records[i];
    if (rec.msg)
      free(rec.msg);

    // testname should be sourced from the .so
  }
}

void records_push(records_t *records, context_t* ctx) {
  record_t rec = {
    .testname = ctx->current_test,
    .msg = ctx->msg,
    .status = ctx->msg != 0,
  };

  records->records[records->len++] = rec;
}

void report_records(records_t records) {
  fprintf(stdout, "%s\n", records.suite_name);
  for (size_t i = 0; i < records.len; i++) {
    record_t rec = records.records[i];

    if (rec.status == STATUS_PASS) {
      fprintf(stdout, "  %s - \x1b[32mPASS\x1b[0m\n", rec.testname);
    } else {
      fprintf(stdout, "  %s - \x1b[31mFAIL\x1b[0m - %s\n", rec.testname, rec.msg);
    }
  }
}
