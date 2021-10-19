#ifndef __record_h
#define __record_h

#include <sys/types.h>

typedef enum {
  STATUS_PASS,
  STATUS_FAIL,
} status_t;

typedef struct {
  char* testname;
  status_t  status;
} record_t;

typedef struct {
  size_t len;
  record_t *records;
} microunit_suite_t;

#endif
