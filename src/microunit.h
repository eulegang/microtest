#ifndef __microunit_h
#define __microunit_h

typedef void* microunit_ctx;

#define microunit(A) void __microunit_ ## A(microunit_ctx *_ctx)

#define assert(A, MSG, ...) if (!(A)) { __mu_fail(_ctx, MSG, #__VA_ARGS__); return; }
#define refute(A, MSG, ...) if ((A)) { __mu_fail(_ctx, MSG, #__VA_ARGS__); return; }
#define fail(MSG, ...) { __mu_fail(_ctx, MSG, #__VA_ARGS__); return; }

void __mu_fail(microunit_ctx ctx, char *msg, ...);

#endif
