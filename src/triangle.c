#include "demo01.h"
#include "library.h"
#include "d3d12.h"


static u32 s_descriptor_size;
static u32 s_descriptor_size_rtv;
static IDXGISwapChain3 *s_swapchain;
static ID3D12Device *s_d3d;
static ID3D12GraphicsCommandList *s_cmdlist;
static ID3D12CommandQueue *s_cmdqueue;
static ID3D12CommandAllocator *s_cmdalloc[2];
static D3D12_CPU_DESCRIPTOR_HANDLE s_rtv_heap_start;
static D3D12_CPU_DESCRIPTOR_HANDLE s_dsv_heap_start;
static ID3D12DescriptorHeap *s_rtv_heap;
static ID3D12DescriptorHeap *s_dsv_heap;
static ID3D12Resource *s_swap_buffers[4];
static ID3D12Resource *s_depth_buffer;

static ID3D12PipelineState *s_pso;
static ID3D12RootSignature *s_rsig;

void demo_update(double frame_time, float frame_delta_time)
{
    (void)frame_time; (void)frame_delta_time;
}

void demo_draw(u32 frame_index)
{
    D3D12_VIEWPORT viewport = {
        .TopLeftX = 0.0f, .TopLeftY = 0.0f, .Width = (float)k_win_width, .Height = (float)k_win_height,
        .MinDepth = 0.0f, .MaxDepth = 1.0f
    };
    D3D12_RECT scissor = { .left = 0, .top = 0, .right = k_win_width, .bottom = k_win_height };

    ID3D12CommandAllocator *cmdalloc = s_cmdalloc[frame_index];
    ID3D12CommandAllocator_Reset(cmdalloc);

    ID3D12GraphicsCommandList_Reset(s_cmdlist, cmdalloc, NULL);
    ID3D12GraphicsCommandList_RSSetViewports(s_cmdlist, 1, &viewport);
    ID3D12GraphicsCommandList_RSSetScissorRects(s_cmdlist, 1, &scissor);

    u32 back_buffer_index = IDXGISwapChain3_GetCurrentBackBufferIndex(s_swapchain);
    D3D12_RESOURCE_BARRIER barrier = {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Transition.pResource = s_swap_buffers[back_buffer_index],
        .Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT, .Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
    };
    ID3D12GraphicsCommandList_ResourceBarrier(s_cmdlist, 1, &barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = s_rtv_heap_start;
    rtv_handle.ptr += back_buffer_index * s_descriptor_size_rtv;

    ID3D12GraphicsCommandList_OMSetRenderTargets(s_cmdlist, 1, &rtv_handle, 0, &s_dsv_heap_start);

    float clear_color[] = { 0.0f, sqrtf((float)frame_index), 0.4f, 1.0f };
    ID3D12GraphicsCommandList_ClearRenderTargetView(s_cmdlist, rtv_handle, clear_color, 0, NULL);
    ID3D12GraphicsCommandList_ClearDepthStencilView(s_cmdlist, s_dsv_heap_start, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

    ID3D12GraphicsCommandList_SetPipelineState(s_cmdlist, s_pso);
    ID3D12GraphicsCommandList_SetGraphicsRootSignature(s_cmdlist, s_rsig);
    ID3D12GraphicsCommandList_IASetPrimitiveTopology(s_cmdlist, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D12GraphicsCommandList_DrawInstanced(s_cmdlist, 3, 1, 0, 0);

    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier(s_cmdlist, 1, &barrier);

    ID3D12GraphicsCommandList_Close(s_cmdlist);


    ID3D12CommandQueue_ExecuteCommandLists(s_cmdqueue, 1, (ID3D12CommandList **)&s_cmdlist);
}

void demo_init(IDXGISwapChain3 *swapchain, ID3D12Device *d3d, ID3D12CommandQueue *cmdqueue)
{
    s_swapchain = swapchain;
    s_d3d = d3d;
    s_cmdqueue = cmdqueue;

    for (u32 i = 0; i < 2; ++i)
        VHR(ID3D12Device_CreateCommandAllocator(d3d, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator,
                                                &s_cmdalloc[i]));

    s_descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(d3d, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    s_descriptor_size_rtv = ID3D12Device_GetDescriptorHandleIncrementSize(d3d, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    /* swap buffers */ {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
            .NumDescriptors = 4, .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV
        };
        VHR(ID3D12Device_CreateDescriptorHeap(d3d, &heap_desc, &IID_ID3D12DescriptorHeap, &s_rtv_heap));
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_rtv_heap, &s_rtv_heap_start);

        D3D12_CPU_DESCRIPTOR_HANDLE handle = s_rtv_heap_start;

        for (u32 i = 0; i < 4; ++i) {
            VHR(IDXGISwapChain3_GetBuffer(swapchain, i, &IID_ID3D12Resource, &s_swap_buffers[i]));

            ID3D12Device_CreateRenderTargetView(d3d, s_swap_buffers[i], NULL, handle);
            handle.ptr += s_descriptor_size_rtv;
        }
    }
    /* depth buffer */ {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
            .NumDescriptors = 1, .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV
        };
        VHR(ID3D12Device_CreateDescriptorHeap(d3d, &heap_desc, &IID_ID3D12DescriptorHeap, &s_dsv_heap));
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_dsv_heap, &s_dsv_heap_start);

        D3D12_CLEAR_VALUE clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil.Depth = 1.0f, .DepthStencil.Stencil = 0
        };
        D3D12_HEAP_PROPERTIES heap_props = { .Type = D3D12_HEAP_TYPE_DEFAULT };
        D3D12_RESOURCE_DESC image_desc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Width = k_win_width, .Height = k_win_height,
            .DepthOrArraySize = 1, .MipLevels = 1, .Format = DXGI_FORMAT_D32_FLOAT, .SampleDesc.Count = 1,
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN, .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        };
        VHR(ID3D12Device_CreateCommittedResource(s_d3d, &heap_props, D3D12_HEAP_FLAG_NONE, &image_desc,
                                                 D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value,
                                                 &IID_ID3D12Resource, &s_depth_buffer));

        D3D12_DEPTH_STENCIL_VIEW_DESC view_desc = {
            .Format = DXGI_FORMAT_D32_FLOAT, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE
        };
        ID3D12Device_CreateDepthStencilView(d3d, s_depth_buffer, &view_desc, s_dsv_heap_start);
    }
    /* pso */ {
        u32 vscode_size, pscode_size;
        void *vscode = load_file("data/vs_triangle.cso", &vscode_size);
        void *pscode = load_file("data/ps_triangle.cso", &pscode_size);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {
            .VS = { vscode, vscode_size },
            .PS = { pscode, pscode_size },
            .RasterizerState.FillMode = D3D12_FILL_MODE_SOLID,
            .RasterizerState.CullMode = D3D12_CULL_MODE_NONE,
            .BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL,
            .SampleMask = 0xffffffff,
            .PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
            .NumRenderTargets = 1,
            .RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc.Count = 1,
        };
        VHR(ID3D12Device_CreateGraphicsPipelineState(s_d3d, &pso_desc, &IID_ID3D12PipelineState, &s_pso));
        VHR(ID3D12Device_CreateRootSignature(s_d3d, 0, vscode, vscode_size, &IID_ID3D12RootSignature, &s_rsig));
    }

    VHR(ID3D12Device_CreateCommandList(d3d, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, s_cmdalloc[0], NULL,
                                       &IID_ID3D12GraphicsCommandList, &s_cmdlist));
    ID3D12GraphicsCommandList_Close(s_cmdlist);
}

void demo_shutdown(void)
{
    COMRELEASE(s_cmdlist);
}
