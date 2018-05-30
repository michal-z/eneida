#include "demo01.h"
#include "library.h"
#include "math.h"
#include "windows.h"
#include "renderer.h"


#define k_num_quads 128*1024

typedef struct quad
{
    f32 position[2];
} quad_t;

typedef struct experiment
{
    renderer_t *renderer;
    ID3D12PipelineState *pso;
    ID3D12RootSignature *root_sig;
    quad_t *quads;
    u32 rnd;
} experiment_t;

static void update(experiment_t *exp, f64 frame_time, f32 frame_delta_time)
{
    (void)exp;
    (void)frame_delta_time;
    (void)frame_time;

    for (u32 i = 0; i < k_num_quads; ++i) {
        exp->quads[i].position[0] = f32_rand_range(&exp->rnd, -0.7f, 0.7f);
        exp->quads[i].position[1] = f32_rand_range(&exp->rnd, -0.7f, 0.7f);
    }
}

static void draw(experiment_t *exp)
{
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (f32)k_win_width, (f32)k_win_height, 0.0f, 1.0f };
    D3D12_RECT scissor = { 0, 0, k_win_width, k_win_height };

    ID3D12CommandAllocator *cmdalloc = exp->renderer->cmdalloc[exp->renderer->frame_index];
    ID3D12CommandAllocator_Reset(cmdalloc);

    ID3D12GraphicsCommandList *cmdlist = exp->renderer->cmdlist;

    ID3D12GraphicsCommandList_Reset(cmdlist, cmdalloc, NULL);

    ID3D12GraphicsCommandList_RSSetViewports(cmdlist, 1, &viewport);
    ID3D12GraphicsCommandList_RSSetScissorRects(cmdlist, 1, &scissor);

    u32 back_buffer_index = exp->renderer->back_buffer_index;
    D3D12_RESOURCE_BARRIER barrier = {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
        .Transition.pResource = exp->renderer->color_buffers[back_buffer_index],
        .Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT,
        .Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
    };
    ID3D12GraphicsCommandList_ResourceBarrier(cmdlist, 1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = exp->renderer->dsv_heap_start;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = exp->renderer->rtv_heap_start;
    rtv_handle.ptr += back_buffer_index * exp->renderer->descriptor_size_rtv;

    ID3D12GraphicsCommandList_OMSetRenderTargets(cmdlist, 1, &rtv_handle, 0, &dsv_handle);

    f32 clear_color[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    ID3D12GraphicsCommandList_ClearRenderTargetView(cmdlist, rtv_handle, clear_color, 0, NULL);
    ID3D12GraphicsCommandList_ClearDepthStencilView(cmdlist, dsv_handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

    ID3D12GraphicsCommandList_SetPipelineState(cmdlist, exp->pso);
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(cmdlist, exp->root_sig);
    ID3D12GraphicsCommandList_IASetPrimitiveTopology(cmdlist, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

    for (u32 i = 0; i < k_num_quads; ++i) {
        ID3D12GraphicsCommandList_SetGraphicsRoot32BitConstants(cmdlist, 0, sizeof(quad_t) / 4, &exp->quads[i], 0);
        ID3D12GraphicsCommandList_DrawInstanced(cmdlist, 1, 1, 0, 0);
    }

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier(cmdlist, 1, &barrier);

    ID3D12GraphicsCommandList_Close(cmdlist);
    ID3D12CommandQueue_ExecuteCommandLists(exp->renderer->cmdqueue, 1, (ID3D12CommandList **)&cmdlist);

    gr_present_frame(exp->renderer);
}

static void init(experiment_t *exp)
{
    ID3D12Device *d3d = exp->renderer->d3d;

    /* pso */ {
        extern u8 vs_e01_transform[], ps_e01_shade[];
        extern u32 vs_e01_transform_size, ps_e01_shade_size;

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {
            .VS = { vs_e01_transform, vs_e01_transform_size },
            .PS = { ps_e01_shade, ps_e01_shade_size },
            .RasterizerState.FillMode = D3D12_FILL_MODE_SOLID,
            .RasterizerState.CullMode = D3D12_CULL_MODE_NONE,
            .BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
            .SampleMask = 0xffffffff,
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
            .NumRenderTargets = 1,
            .RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc.Count = 1,
        };
        VHR(ID3D12Device_CreateGraphicsPipelineState(d3d, &pso_desc, &IID_ID3D12PipelineState, &exp->pso));
        VHR(ID3D12Device_CreateRootSignature(d3d, 0, vs_e01_transform, vs_e01_transform_size,
                                             &IID_ID3D12RootSignature, &exp->root_sig));
    }

    exp->quads = (quad_t *)mem_alloc(k_num_quads * sizeof(quad_t));
    exp->rnd = 0;
}

static void shutdown(experiment_t *exp)
{
    gr_flush(exp->renderer);
    COMRELEASE(exp->pso);
    COMRELEASE(exp->root_sig);
    gr_shutdown(exp->renderer);
}

demo_module_t module_init_e01(void *window)
{
    static experiment_t experiment_storage;

    experiment_storage.renderer = gr_init(window);

    init(&experiment_storage);

    demo_module_t module = {
        .data = &experiment_storage,
        .update = update,
        .draw = draw,
        .shutdown = shutdown
    };
    return module;
}
