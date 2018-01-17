#include "demo01.h"
#include "library.h"
#include "d3d12.h"


#if 0
static struct
{
    u32 BackBufferResolution[2];
    u32 BackBufferIndex;
    u32 FrameIndex;
    u32 DescriptorSize;
    u32 RTVDescriptorSize;
    D3D12_CPU_DESCRIPTOR_HANDLE RTVHeapStart;
    D3D12_CPU_DESCRIPTOR_HANDLE DSVHeapStart;
    ID3D12Device *Device;
    ID3D12CommandQueue *CmdQueue;
    ID3D12CommandAllocator *CmdAlloc[2];
    ID3D12GraphicsCommandList *CmdList;
    ID3D12Resource *SwapBuffers[4];
    ID3D12Resource *DepthStencilImage;
    IDXGIFactory4 *GiFactory;
    IDXGISwapChain3 *SwapChain;
    ID3D12DescriptorHeap *RTVHeap;
    ID3D12DescriptorHeap *DSVHeap;
    u64 CPUCompletedFrames;
    ID3D12Fence *FrameFence;
    void *FrameFenceEvent;
    i64 Frequency;
    i64 StartCounter;
    void *Window;
    double FrameTime;
    float FrameDeltaTime;
} G;

static i32 InitializeD3D12(void)
{
    // depth-stencil image, view and heap
    {
        D3D12_DESCRIPTOR_HEAP_DESC HeapDesc =
        {
            .NumDescriptors = 1,.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
        };
        VHR(ID3D12Device_CreateDescriptorHeap(G.Device, &HeapDesc, &IID_ID3D12DescriptorHeap, &G.DSVHeap));
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(G.DSVHeap, &G.DSVHeapStart);

        D3D12_CLEAR_VALUE ClearValue =
        {
            .Format = DXGI_FORMAT_D32_FLOAT,.DepthStencil.Depth = 1.0f,.DepthStencil.Stencil = 0
        };
        D3D12_HEAP_PROPERTIES HeapProps =
        {
            .Type = D3D12_HEAP_TYPE_DEFAULT
        };
        D3D12_RESOURCE_DESC ImageDesc =
        {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,.Width = G.BackBufferResolution[0],.Height = G.BackBufferResolution[1],
            .DepthOrArraySize = 1,.MipLevels = 1,.Format = DXGI_FORMAT_D32_FLOAT,.SampleDesc.Count = 1,
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        };
        VHR(ID3D12Device_CreateCommittedResource(G.Device, &HeapProps, D3D12_HEAP_FLAG_NONE, &ImageDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE, &ClearValue, &IID_ID3D12Resource, &G.DepthStencilImage));

        D3D12_DEPTH_STENCIL_VIEW_DESC ViewDesc =
        {
            .Format = DXGI_FORMAT_D32_FLOAT,.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,.Flags = D3D12_DSV_FLAG_NONE
        };
        ID3D12Device_CreateDepthStencilView(G.Device, G.DepthStencilImage, &ViewDesc, G.DSVHeapStart);
    }


    return 1;
}

static void Update(void)
{
    ID3D12CommandAllocator *CmdAlloc = G.CmdAlloc[G.FrameIndex];
    ID3D12CommandAllocator_Reset(CmdAlloc);

    ID3D12GraphicsCommandList *CmdList = G.CmdList;

    ID3D12GraphicsCommandList_Reset(CmdList, CmdAlloc, NULL);
    ID3D12GraphicsCommandList_RSSetViewports(CmdList, 1, &G.Viewport);
    ID3D12GraphicsCommandList_RSSetScissorRects(CmdList, 1, &G.Scissor);

    D3D12_RESOURCE_BARRIER Barrier =
    {
        .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,.Transition.pResource = G.SwapBuffers[G.BackBufferIndex],
        .Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT,.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET
    };
    ID3D12GraphicsCommandList_ResourceBarrier(CmdList, 1, &Barrier);

    D3D12_CPU_DESCRIPTOR_HANDLE RTVHandle = G.RTVHeapStart;
    RTVHandle.ptr += G.BackBufferIndex *G.RTVDescriptorSize;

    ID3D12GraphicsCommandList_OMSetRenderTargets(CmdList, 1, &RTVHandle, 0, &G.DSVHeapStart);

    const float ClearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    ID3D12GraphicsCommandList_ClearRenderTargetView(CmdList, RTVHandle, ClearColor, 0, NULL);
    ID3D12GraphicsCommandList_ClearDepthStencilView(CmdList, G.DSVHeapStart, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);

    Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    Barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    ID3D12GraphicsCommandList_ResourceBarrier(CmdList, 1, &Barrier);

    ID3D12GraphicsCommandList_Close(CmdList);

    ID3D12CommandQueue_ExecuteCommandLists(G.CmdQueue, 1, (ID3D12CommandList **)&CmdList);
}
#endif

static u32 s_descriptor_size;
static u32 s_descriptor_size_rtv;
static ID3D12Device *s_d3d;
static ID3D12GraphicsCommandList *s_cmdlist;
static ID3D12CommandQueue *s_cmdqueue;
static ID3D12CommandAllocator *s_cmdalloc[2];
static D3D12_CPU_DESCRIPTOR_HANDLE s_rtv_heap_start;
static ID3D12DescriptorHeap *s_rtv_heap;
static ID3D12Resource *s_swap_buffers[4];

void demo_update(double frame_time, float frame_delta_time)
{
    (void)frame_time; (void)frame_delta_time;
}

void demo_draw(u32 frame_index)
{
    (void)frame_index;

    /*
    D3D12_VIEWPORT Viewport;
    D3D12_RECT Scissor;

    G.Viewport.TopLeftX = 0.0f;
    G.Viewport.TopLeftY = 0.0f;
    G.Viewport.Width = (float)G.BackBufferResolution[0];
    G.Viewport.Height = (float)G.BackBufferResolution[1];
    G.Viewport.MinDepth = 0.0f;
    G.Viewport.MaxDepth = 1.0f;

    G.Scissor.left = 0;
    G.Scissor.top = 0;
    G.Scissor.right = G.BackBufferResolution[0];
    G.Scissor.bottom = G.BackBufferResolution[1];
    */
}

void demo_init(IDXGISwapChain3 *swapchain, ID3D12Device *d3d, ID3D12CommandQueue *cmdqueue)
{
    (void)swapchain;
    s_d3d = d3d;
    s_cmdqueue = cmdqueue;

    for (u32 i = 0; i < 2; ++i)
        VHR(ID3D12Device_CreateCommandAllocator(s_d3d, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator,
                                                &s_cmdalloc[i]));

    s_descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(s_d3d, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    s_descriptor_size_rtv = ID3D12Device_GetDescriptorHandleIncrementSize(s_d3d, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    /* rtv descriptor heap */ {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
            .NumDescriptors = 4, .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV
        };
        VHR(ID3D12Device_CreateDescriptorHeap(s_d3d, &heap_desc, &IID_ID3D12DescriptorHeap, &s_rtv_heap));
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(s_rtv_heap, &s_rtv_heap_start);

        D3D12_CPU_DESCRIPTOR_HANDLE handle = s_rtv_heap_start;

        for (u32 i = 0; i < 4; ++i) {
            VHR(IDXGISwapChain3_GetBuffer(swapchain, i, &IID_ID3D12Resource, &s_swap_buffers[i]));

            ID3D12Device_CreateRenderTargetView(s_d3d, s_swap_buffers[i], NULL, handle);
            handle.ptr += s_descriptor_size_rtv;
        }
    }

    VHR(ID3D12Device_CreateCommandList(s_d3d, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, s_cmdalloc[0], NULL,
                                       &IID_ID3D12GraphicsCommandList, &s_cmdlist));
    ID3D12GraphicsCommandList_Close(s_cmdlist);
}

void demo_shutdown(void)
{
}
