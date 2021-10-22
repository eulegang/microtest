#include <stdlib.h>
#include <dbg.h>
#include <unistd.h>

#include "suite.h"

#define READ_BUF_SIZE 4096

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
  free(records.suite_name);
 
  for (size_t i = 0; i < records.len; i++) {
    record_t rec = records.records[i];

    if (rec.msg)
      free(rec.msg);
    if (rec.out)
      free(rec.out);
    if (rec.err)
      free(rec.err);
  }
}

char *collect_fd(int fd);

void records_push(records_t *records, context_t* ctx, int out, int err) {
  record_t rec = {
    .testname = ctx->current_test,
    .msg = ctx->msg,
    .status = ctx->msg != 0,
    .out = collect_fd(out),
    .err = collect_fd(err),
  };

  records->records[records->len++] = rec;
}

extern int sout, serr;

void report_records(records_t records, int flags) {
  dprintf(sout, "%s\n", records.suite_name);
  for (size_t i = 0; i < records.len; i++) {
    record_t rec = records.records[i];

    if (rec.status == STATUS_PASS) {
      dprintf(sout, "  %s - \x1b[32mPASS\x1b[0m\n", rec.testname);
    } else {
      dprintf(sout, "  %s - \x1b[31mFAIL\x1b[0m - %s\n", rec.testname, rec.msg);
    }

    if (rec.out) {
      dprintf(sout, "\x1b[36m--- begin stdout ---\x1b[0m\n");
      dprintf(sout, "%s", rec.out);
      dprintf(sout, "\x1b[36m--- end   stdout ---\x1b[0m\n");
    }

    if (rec.err && (rec.status == STATUS_FAIL || flags & MICRO_SUITE_VERBOSE) ) {
      dprintf(sout, "\x1b[31m--- begin stderr ---\x1b[0m\n");
      dprintf(sout, "%s", rec.err);
      dprintf(sout, "\x1b[31m--- end   stdout ---\x1b[0m\n");
    }
  }
}

int records_fail(records_t records) {
  for (size_t i = 0; i < records.len; i++) {
    record_t rec = records.records[i];
    if (rec.status == STATUS_FAIL) {
      return 1;
    }
  }

  return 0;
}

char *collect_fd(int fd) {
  char *buf = alloca(READ_BUF_SIZE);

  int n = read(fd, buf, READ_BUF_SIZE);

  if (n == -1 && errno == EWOULDBLOCK) {
    return NULL;
  }

  check(n != -1, "failed to read pipe");

  if (n == 0) {
    return NULL;
  }

  char *res = malloc(READ_BUF_SIZE);

  memcpy(res, buf, n);
  res[n] = 0;

  return res;

error:
  return NULL;
}
