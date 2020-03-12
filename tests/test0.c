typedef struct test0_context {
  mzgr_context_t *gfx;
} test0_context_t;

static i32 test0_init(void *context, void *window) {
  MZ_ASSERT(context && window);
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
  MZ_ASSERT(context);
  test0_context_t *ctx = context;

  mzgr_wait_for_gpu(ctx->gfx);
  mzgr_destroy_context(ctx->gfx);
}

static void test0_update(void *context, f64 time, f32 delta_time) {
  MZ_ASSERT(context);
  test0_context_t *ctx = context;

  mzgr_begin_frame(ctx->gfx);
  mzgr_end_frame(ctx->gfx, 0);
}

static test_api_t get_test0(void) {
  static test0_context_t ctx;
  return (test_api_t) {
    .context = &ctx,
    .init = test0_init,
    .deinit = test0_deinit,
    .update = test0_update,
  };
}
