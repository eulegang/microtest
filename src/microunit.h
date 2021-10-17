#ifndef __microunit_h
#define __microunit_h

typedef struct microtest_ctx {
  char *current_test;
  void *_inner;
} microtest_ctx;

#define microtest(A) void __microtest_ ## A(microtest_ctx *_ctx)
#define it(MSG, ...) push_context(_ctx, MSG, ##__VA_ARGS__)

#define assert(A, MSG, ...) if (!(A)) { fail(_ctx, MSG, #__VA_ARGS__); return; }
#define refute(A, MSG, ...) if ((A)) { fail(_ctx, MSG, #__VA_ARGS__); return; }


void push_context(microtest_ctx *ctx, char *msg, ...);
void fail(microtest_ctx *ctx, char *msg, ...);

#endif
