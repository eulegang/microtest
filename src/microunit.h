#ifndef __microunit_h
#define __microunit_h

typedef void* microunit_ctx;

#define microunit(A) void __microunit_ ## A(microunit_ctx *_ctx)
#define it(MSG, ...) push_context(_ctx, MSG, ##__VA_ARGS__);

#define assert(A, MSG, ...) if (!(A)) { fail(_ctx, MSG, #__VA_ARGS__); return; }
#define refute(A, MSG, ...) if ((A)) { fail(_ctx, MSG, #__VA_ARGS__); return; }


void push_context(microunit_ctx ctx, char *msg, ...);
void fail(microunit_ctx ctx, char *msg, ...);

#endif
