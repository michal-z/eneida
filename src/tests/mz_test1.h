#ifndef MZ_TEST1_INCLUDED__
#define MZ_TEST1_INCLUDED__

#include "mz_types.h"

typedef struct mztest1_context mztest1_context;

mztest1_context *mztest1_init(void *window);
void mztest1_shutdown(mztest1_context *test1);
void mztest1_update(mztest1_context *test1);
void mztest1_draw(mztest1_context *test1);

#endif // #ifndef MZ_TEST1_INCLUDED__

//
//
// Stop now, if you are only interested in the API.
// Below, you find the implementation.
//
//

#ifdef __INTELLISENSE__
// This makes MSVC intellisense work.
#define MZ_TEST1_IMPLEMENTATION
#endif

#ifdef MZ_TEST1_IMPLEMENTATION

#include "mz_direct3d12.h"

struct mztest1_context {
    mzd3d_context *gfx;
};

mztest1_context *mztest1_init(void *window)
{
    static mztest1_context test = { 0 };
    mztest1_context *test1 = &test;

    test1->gfx = mzd3d_create_context(window);

    mzd3d_end_frame(test1->gfx, 0);

    return test1;
}

void mztest1_shutdown(mztest1_context *test1)
{
    mzd3d_destroy_context(test1->gfx);
}

void mztest1_update(mztest1_context *test1)
{
}

void mztest1_draw(mztest1_context *test1)
{
    mzd3d_context *gfx = test1->gfx;

    ID3D12GraphicsCommandList *cmdlist = mzd3d_begin_frame(gfx);

    //mzd3d_resource_handle back_buffer_handle;
    //D3D12_CPU_DESCRIPTOR_HANDLE back_buffer_rtv;
    //gfx->get_back_buffer(gfx->self, &back_buffer_handle, &back_buffer_rtv);
    //gfx->transition_barrier(gfx->self, back_buffer_handle, D3D12_RESOURCE_STATE_RENDER_TARGET);

    //float color[4] = { 0.2f, 0.4f, 0.8f, 1.0f };
    //ID3D12GraphicsCommandList3_OMSetRenderTargets(cmdlist, 1, &back_buffer_rtv, TRUE, NULL);
    //ID3D12GraphicsCommandList3_ClearRenderTargetView(cmdlist, back_buffer_rtv, color, 0, NULL);

    //gfx->transition_barrier(gfx->self, back_buffer_handle, D3D12_RESOURCE_STATE_PRESENT);

    mzd3d_end_frame(gfx, 0);
}

#endif // MZ_TEST1_IMPLEMENTATION