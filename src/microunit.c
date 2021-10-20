#include <dbg.h>
#include <stdarg.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>

#include "microunit.h"
#include "suite.h"

#define MAX_FAILURE_MESSAGE 4096

void fail(microunit_ctx c, char *msg, ...) {
  if (!c) return;

  context_t *ctx = (context_t*)c;
  char *buf = alloca(MAX_FAILURE_MESSAGE);

  va_list argp;
  va_start(argp, msg);
  vsnprintf(buf, MAX_FAILURE_MESSAGE, msg, argp);
  va_end(argp);

  // in case fail is called multiple times
  if (ctx->msg) free(ctx->msg); 

  size_t len = strlen(buf);
  ctx->msg = malloc(len);
  memcpy(ctx->msg, buf, len);
  ctx->msg[len] = 0;
}
