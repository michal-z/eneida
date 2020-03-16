typedef struct test0_context {
  mzgr_context_t *gfx;
  mzgr_pipeline_handle_t pso;
} test0_context_t;

static i32 test0_init(void *context, void *window) {
  MZ_ASSERT(context && window);
  test0_context_t *ctx = context;

  ctx->gfx = mzgr_create_context(window);

  ctx->pso = mzgr_create_graphics_pipeline(
      ctx->gfx,
      &(D3D12_GRAPHICS_PIPELINE_STATE_DESC){
          .RasterizerState = mzd3d12_rasterizer_desc(),
          .BlendState = mzd3d12_blend_desc(),
          .RTVFormats = {DXGI_FORMAT_R8G8B8A8_UNORM},
          .DepthStencilState = mzd3d12_depth_stencil_desc(),
          .NumRenderTargets = 1,
          .SampleMask = 0xffffffff,
          .SampleDesc =
              (DXGI_SAMPLE_DESC){
                  .Count = 1,
                  .Quality = 0,
              },
          .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
      },
      "test.vs.cso", "test.ps.cso");

  return 1;
}

static void test0_deinit(void *context) {
  MZ_ASSERT(context);
  test0_context_t *ctx = context;

  mzgr_wait_for_gpu(ctx->gfx);
  mzgr_destroy_context(ctx->gfx);
}

static void test0_update(void *context, f64 time, f32 delta_time) {
  (void)time;
  (void)delta_time;
  MZ_ASSERT(context);

  test0_context_t *ctx = context;
  mzgr_context_t *gfx = ctx->gfx;

  ID3D12GraphicsCommandList *cmdlist = mzgr_begin_frame(gfx);

  mzgr_resource_handle_t back_buffer;
  D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_rtv;
  mzgr_get_back_buffer(gfx, &back_buffer, &back_buffer_rtv);

  mzgr_cmd_transition_barrier(gfx, back_buffer, D3D12_RESOURCE_STATE_RENDER_TARGET);

  ID3D12GraphicsCommandList_RSSetViewports(cmdlist, 1,
                                           &(D3D12_VIEWPORT){.TopLeftX = 0.0f,
                                                             .TopLeftY = 0.0f,
                                                             .Width = gfx->resolution[0],
                                                             .Height = gfx->resolution[1],
                                                             .MinDepth = 0.0f,
                                                             .MaxDepth = 1.0f});
  ID3D12GraphicsCommandList_RSSetScissorRects(
      cmdlist, 1,
      &(D3D12_RECT){
          .top = 0, .left = 0, .right = gfx->resolution[0], .bottom = gfx->resolution[1]});

  f32 color[4] = {0.2f, 0.4f, 0.8f, 1.0f};
  ID3D12GraphicsCommandList_OMSetRenderTargets(cmdlist, 1, &back_buffer_rtv, TRUE, NULL);
  ID3D12GraphicsCommandList_ClearRenderTargetView(cmdlist, back_buffer_rtv, color, 0, NULL);
  ID3D12GraphicsCommandList_IASetPrimitiveTopology(cmdlist, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  mzgr_cmd_set_graphics_pipeline(gfx, ctx->pso);

  ID3D12GraphicsCommandList_DrawInstanced(cmdlist, 3, 1, 0, 0);

  mzgr_cmd_transition_barrier(gfx, back_buffer, D3D12_RESOURCE_STATE_PRESENT);

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
