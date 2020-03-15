typedef struct test0_context {
  mzgr_context_t *gfx;
} test0_context_t;

static i32 test0_init(void *context, void *window) {
  MZ_ASSERT(context && window);
  test0_context_t *ctx = context;

  ctx->gfx = mzgr_create_context(window);

  i32 *arr = NULL;

  i32 *ptr = arraddn(arr, 3);
  i32 a123[3] = {1, 2, 3};
  memcpy(ptr, a123, sizeof a123);

  ptr = arraddn(arr, 4);
  i32 a4567[4] = {4, 5, 6, 7};
  memcpy(ptr, a4567, sizeof a4567);

  for (u64 i = 0; i < arrlenu(arr); ++i) {
    char buf[64];
    wsprintf(buf, "%d\n", arr[i]);
    OutputDebugString(buf);
  }

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
  mzgr_context_t *gfx = ctx->gfx;

  ID3D12GraphicsCommandList *cmdlist = mzgr_begin_frame(gfx);

  mzgr_resource_handle_t back_buffer;
  D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_rtv;
  mzgr_get_back_buffer(gfx, &back_buffer, &back_buffer_rtv);

  mzgr_transition_barrier(gfx, back_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

  f32 color[4] = {0.2f, 0.4f, 0.8f, 1.0f};
  ID3D12GraphicsCommandList_OMSetRenderTargets(cmdlist, 1, &back_buffer_rtv, TRUE, NULL);
  ID3D12GraphicsCommandList_ClearRenderTargetView(cmdlist, back_buffer_rtv, color, 0, NULL);

  mzgr_transition_barrier(gfx, back_buffer, D3D12_RESOURCE_STATE_PRESENT);

  mzgr_end_frame(ctx->gfx, 0);
}

static test_api_t get_test0(void) {
  static test0_context_t ctx;
  return (test_api_t){
      .context = &ctx,
      .init = test0_init,
      .deinit = test0_deinit,
      .update = test0_update,
  };
}
