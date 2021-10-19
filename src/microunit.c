#include <dbg.h>
#include "microunit.h"
#include "context.h"

void push_context(microunit_ctx c, char *msg, ...) {
  if (!c) return;

  (void)msg;
}

void fail(microunit_ctx c, char *msg, ...) {
  if (!c) return;

  context_t *ctx = (context_t*)c;
  (void)msg;

  log_err("failed %s", ctx->current_test);
}
