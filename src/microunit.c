#include "microunit.h"

typedef struct inner_context {


} inner_context;

void push_context(microtest_ctx *i, char *msg, ...) {
  (void)i;
  (void)msg;

}

void fail(microtest_ctx *i, char *msg, ...) {
  (void)i;
  (void)msg;
}
