#include "mz_direct3d12.h"
#include "mz_library.h"
#include "mz_windows.h"

/*
struct demo_t {
    struct mz_gfx_context_i *gfx;
};

static void update(struct demo_t *demo) {
}

static void draw(struct demo_t *demo) {
    struct mz_gfx_context_i *gfx = demo->gfx;

    ID3D12GraphicsCommandList3 *cmdlist = gfx->begin_frame(gfx->self);

    struct mz_gfx_resource_handle_t back_buffer_handle;
    D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_rtv;
    gfx->get_back_buffer(gfx->self, &back_buffer_handle, &back_buffer_rtv);
    gfx->transition_barrier(gfx->self, back_buffer_handle, D3D12_RESOURCE_STATE_RENDER_TARGET);

    float color[4] = { 0.2f, 0.4f, 0.8f, 1.0f };
    ID3D12GraphicsCommandList3_OMSetRenderTargets(cmdlist, 1, &back_buffer_rtv, TRUE, NULL);
    ID3D12GraphicsCommandList3_ClearRenderTargetView(cmdlist, back_buffer_rtv, color, 0, NULL);

    gfx->transition_barrier(gfx->self, back_buffer_handle, D3D12_RESOURCE_STATE_PRESENT);
    gfx->end_frame(gfx->self, 0);
}

static i32 init(struct demo_t *demo) {
    demo->gfx->end_frame(demo->gfx->self, 0);
    return 1;
}

static void shutdown(struct demo_t *demo) {
}
*/

static i32 run(void)
{
    void *window = mzl_create_window("demo", 1920, 1080);

    //struct demo_t demo = { 0 };
    //demo.gfx = mz_get_gfx_context(window);

    //if (init(&demo)) {
    for (;;) {
        MSG message = { 0 };
        if (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
            DispatchMessageA(&message);
            if (message.message == WM_QUIT) {
                break;
            }
        } else {
            f64 time;
            f32 delta_time;
            mzl_update_frame_stats(window, "demo", &time, &delta_time);
            //update(&demo);
            //draw(&demo);
        }
    }
    //}

    //shutdown(&demo);
    //demo.gfx->destroy(demo.gfx->self);
    return 0;
}

i32 __stdcall WinMain(void *_0, void *_1, const char *_2, i32 _3)
{
    (void)_0;
    (void)_1;
    (void)_2;
    (void)_3;
    SetProcessDPIAware();
    return run();
}
