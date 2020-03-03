#ifndef TEST0_INCLUDED_
#define TEST0_INCLUDED_

#include "eneida.h"

void test0_load_api(test_api_t *out_api);

#endif // #ifndef TEST0_INCLUDED_

#ifdef TEST0_IMPLEMENTATION

#include "mz_graphics.h"

typedef struct test0_context {
  mzgr_context_t *gfx;
} test0_context_t;

static i32 test0_init(void *context, void *window) {
  assert(context && window);
  test0_context_t *ctx = context;

  ctx->gfx = mzgr_create_context(window);
  /*
  i32 *arr = NULL;
  // arrsetcap(arr, 128);
  for (i32 i = 0; i < 32; ++i) {
    arrpush(arr, i);
  }
  for (u64 i = 0; i < arrlenu(arr); ++i) {
    char buf[64];
    wsprintf(buf, "%d\n", arr[i]);
    OutputDebugString(buf);
  }
  */
  return 1;
}

static void test0_deinit(void *context) {
  assert(context);
  test0_context_t *ctx = context;

  mzgr_wait_for_gpu(ctx->gfx);
  mzgr_destroy_context(ctx->gfx);
}

static void test0_update(void *context, f64 time, f32 delta_time) {
  assert(context);
  test0_context_t *ctx = context;

  mzgr_begin_frame(ctx->gfx);
  mzgr_end_frame(ctx->gfx, 0);
}

void test0_load_api(test_api_t *out_api) {
  static test0_context_t ctx;
  out_api->context = &ctx;
  out_api->init = test0_init;
  out_api->deinit = test0_deinit;
  out_api->update = test0_update;
}

#undef TEST0_IMPLEMENTATION
#endif // #ifdef TEST0_IMPLEMENTATION
