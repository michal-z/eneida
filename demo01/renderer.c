#include "renderer.h"
#include "windows.h"


#define k_max_num_shader_descriptors 1024
#define k_max_num_non_shader_descriptors 1024

typedef struct priv_renderer
{
    ID3D12Fence *frame_fence;
    void *frame_fence_event;
    IDXGIFactory4 *dxgi_factory;
    IDXGISwapChain3 *swapchain;
    ID3D12DescriptorHeap *rtv_heap;
    ID3D12DescriptorHeap *dsv_heap;
    u64 frame_count;
    // shader visible descriptor heaps
    struct {
        ID3D12DescriptorHeap *heap;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
        D3D12_GPU_DESCRIPTOR_HANDLE gpu_start;
        u32 num_allocated;
    } shader_descriptors[2];
    // shader non-visible descriptor heap
    struct {
        ID3D12DescriptorHeap *heap;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
        u32 num_allocated;
    } non_shader_descriptors;
} priv_renderer_t;

renderer_t *gr_init(void *window)
{
    static renderer_t renderer_storage;
    static priv_renderer_t priv_renderer_storage;

    renderer_t *rend = &renderer_storage;
    priv_renderer_t *prend = rend->priv = &priv_renderer_storage;

    // init d3d12
    VHR(CreateDXGIFactory1(&IID_IDXGIFactory4, &prend->dxgi_factory));
#ifdef _DEBUG
    ID3D12Debug *dbg = NULL;
    D3D12GetDebugInterface(&IID_ID3D12Debug, &dbg);
    if (dbg) {
        ID3D12Debug_EnableDebugLayer(dbg);
        ID3D12Debug_Release(dbg);
    }
#endif
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_1, &IID_ID3D12Device, &rend->d3d) != 0) {
        MessageBox(window, "Program requires Windows 10 machine with DirectX 12 support (D3D_FEATURE_LEVEL_11_1 or better).",
                   "Error", 0);
        ExitProcess(1);
    }

    D3D12_COMMAND_QUEUE_DESC cmdqueue_desc = {
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT
    };
    VHR(ID3D12Device_CreateCommandQueue(rend->d3d, &cmdqueue_desc, &IID_ID3D12CommandQueue, &rend->cmdqueue));

    DXGI_SWAP_CHAIN_DESC swapchain_desc = {
        .BufferCount = 4,
        .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .OutputWindow = window,
        .SampleDesc.Count = 1,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL,
        .Windowed = 1
    };
    IDXGISwapChain *swapchain;
    VHR(IDXGIFactory4_CreateSwapChain(prend->dxgi_factory, (IUnknown *)rend->cmdqueue, &swapchain_desc, &swapchain));
    VHR(IDXGISwapChain_QueryInterface(swapchain, &IID_IDXGISwapChain3, &prend->swapchain));
    IDXGISwapChain_Release(swapchain);

    VHR(ID3D12Device_CreateFence(rend->d3d, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, &prend->frame_fence));
    prend->frame_fence_event = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
    assert(prend->frame_fence_event != NULL);

    for (u32 i = 0; i < 2; ++i) {
        VHR(ID3D12Device_CreateCommandAllocator(rend->d3d, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator,
                                                &rend->cmdalloc[i]));
    }

    rend->descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(rend->d3d, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    rend->descriptor_size_rtv = ID3D12Device_GetDescriptorHandleIncrementSize(rend->d3d, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    /* color buffers */ {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = { .NumDescriptors = 4, .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV };
        VHR(ID3D12Device_CreateDescriptorHeap(rend->d3d, &heap_desc, &IID_ID3D12DescriptorHeap, &prend->rtv_heap));
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(prend->rtv_heap, &rend->rtv_heap_start);

        D3D12_CPU_DESCRIPTOR_HANDLE handle = rend->rtv_heap_start;

        for (u32 i = 0; i < 4; ++i) {
            VHR(IDXGISwapChain3_GetBuffer(prend->swapchain, i, &IID_ID3D12Resource, &rend->color_buffers[i]));

            ID3D12Device_CreateRenderTargetView(rend->d3d, rend->color_buffers[i], NULL, handle);
            handle.ptr += rend->descriptor_size_rtv;
        }
    }
    /* depth buffer */ {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = { .NumDescriptors = 1, .Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV };
        VHR(ID3D12Device_CreateDescriptorHeap(rend->d3d, &heap_desc, &IID_ID3D12DescriptorHeap, &prend->dsv_heap));
        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(prend->dsv_heap, &rend->dsv_heap_start);

        RECT winrect;
        GetClientRect(window, &winrect);

        D3D12_CLEAR_VALUE clear_value = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .DepthStencil.Depth = 1.0f,
            .DepthStencil.Stencil = 0
        };
        D3D12_HEAP_PROPERTIES heap_props = { .Type = D3D12_HEAP_TYPE_DEFAULT };
        D3D12_RESOURCE_DESC image_desc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Width = winrect.right,
            .Height = winrect.bottom,
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .Format = DXGI_FORMAT_D32_FLOAT,
            .SampleDesc.Count = 1,
            .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
            .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        };
        VHR(ID3D12Device_CreateCommittedResource(rend->d3d, &heap_props, D3D12_HEAP_FLAG_NONE, &image_desc,
                                                 D3D12_RESOURCE_STATE_DEPTH_WRITE, &clear_value,
                                                 &IID_ID3D12Resource, &rend->depth_buffer));

        D3D12_DEPTH_STENCIL_VIEW_DESC view_desc = {
            .Format = DXGI_FORMAT_D32_FLOAT, .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D, .Flags = D3D12_DSV_FLAG_NONE
        };
        ID3D12Device_CreateDepthStencilView(rend->d3d, rend->depth_buffer, &view_desc, rend->dsv_heap_start);
    }
    // shader visible descriptor heaps
    for (u32 i = 0; i < 2; ++i) {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
            .NumDescriptors = k_max_num_shader_descriptors,
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
            .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
        };
        VHR(ID3D12Device_CreateDescriptorHeap(rend->d3d, &heap_desc, &IID_ID3D12DescriptorHeap,
                                              &prend->shader_descriptors[i].heap));

        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(prend->shader_descriptors[i].heap,
                                                                &prend->shader_descriptors[i].cpu_start);
        ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(prend->shader_descriptors[i].heap,
                                                                &prend->shader_descriptors[i].gpu_start);
    }
    /* shader non-visible descriptor heap */ {
        D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {
            .NumDescriptors = k_max_num_non_shader_descriptors,
            .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV
        };
        VHR(ID3D12Device_CreateDescriptorHeap(rend->d3d, &heap_desc, &IID_ID3D12DescriptorHeap,
                                              &prend->non_shader_descriptors.heap));

        ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(prend->non_shader_descriptors.heap,
                                                                &prend->non_shader_descriptors.cpu_start);
    }

    VHR(ID3D12Device_CreateCommandList(rend->d3d, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, rend->cmdalloc[0], NULL,
                                       &IID_ID3D12GraphicsCommandList, &rend->cmdlist));
    ID3D12GraphicsCommandList_Close(rend->cmdlist);

    return rend;
}

void gr_shutdown(renderer_t *rend)
{
    assert(rend && rend->priv);
    priv_renderer_t *prend = rend->priv;

    COMRELEASE(rend->cmdlist);
    COMRELEASE(rend->cmdalloc[0]);
    COMRELEASE(rend->cmdalloc[1]);
    COMRELEASE(prend->rtv_heap);
    COMRELEASE(prend->dsv_heap);
    COMRELEASE(prend->shader_descriptors[0].heap);
    COMRELEASE(prend->shader_descriptors[1].heap);
    COMRELEASE(prend->non_shader_descriptors.heap);
    COMRELEASE(rend->depth_buffer);
    for (i32 i = 0; i < 4; ++i) {
        COMRELEASE(rend->color_buffers[i]);
    }
    COMRELEASE(prend->frame_fence);
    COMRELEASE(prend->swapchain);
    COMRELEASE(rend->cmdqueue);
    COMRELEASE(rend->d3d);
    COMRELEASE(prend->dxgi_factory);
}

void gr_flush(renderer_t *rend)
{
    assert(rend && rend->priv);
    priv_renderer_t *prend = rend->priv;
    assert(prend->frame_fence && rend->cmdqueue);

    ID3D12CommandQueue_Signal(rend->cmdqueue, prend->frame_fence, ++prend->frame_count);
    ID3D12Fence_SetEventOnCompletion(prend->frame_fence, prend->frame_count, prend->frame_fence_event);
    WaitForSingleObject(prend->frame_fence_event, INFINITE);
}

void gr_present_frame(renderer_t *rend)
{
    assert(rend && rend->priv && rend->cmdqueue);
    priv_renderer_t *prend = rend->priv;

    IDXGISwapChain3_Present(prend->swapchain, 0, 0);
    ID3D12CommandQueue_Signal(rend->cmdqueue, prend->frame_fence, ++prend->frame_count);

    u64 gpu_frame_count = ID3D12Fence_GetCompletedValue(prend->frame_fence);

    if ((prend->frame_count - gpu_frame_count) >= 2) {
        ID3D12Fence_SetEventOnCompletion(prend->frame_fence, gpu_frame_count + 1, prend->frame_fence_event);
        WaitForSingleObject(prend->frame_fence_event, INFINITE);
    }

    rend->frame_index = !rend->frame_index;
    rend->back_buffer_index = IDXGISwapChain3_GetCurrentBackBufferIndex(prend->swapchain);

    prend->shader_descriptors[rend->frame_index].num_allocated = 0;
}

void gr_alloc_non_shader_descriptors(renderer_t *rend, u32 num, D3D12_CPU_DESCRIPTOR_HANDLE *out_first)
{
    assert(rend && rend->priv && out_first);
    priv_renderer_t *prend = rend->priv;

    assert((prend->non_shader_descriptors.num_allocated + num) < k_max_num_non_shader_descriptors);

    out_first->ptr = prend->non_shader_descriptors.cpu_start.ptr +
        prend->non_shader_descriptors.num_allocated * rend->descriptor_size;

    prend->non_shader_descriptors.num_allocated += num;
}

void gr_alloc_shader_descriptors(renderer_t *rend, u32 num, D3D12_CPU_DESCRIPTOR_HANDLE *out_first_cpu,
                                 D3D12_GPU_DESCRIPTOR_HANDLE *out_first_gpu)
{
    assert(rend && rend->priv && out_first_cpu && out_first_gpu);
    priv_renderer_t *prend = rend->priv;

    u32 frame = rend->frame_index;

    assert((prend->shader_descriptors[frame].num_allocated + num) < k_max_num_shader_descriptors);

    out_first_cpu->ptr = prend->shader_descriptors[frame].cpu_start.ptr +
        prend->shader_descriptors[frame].num_allocated * rend->descriptor_size;

    out_first_gpu->ptr = prend->shader_descriptors[frame].gpu_start.ptr +
        prend->shader_descriptors[frame].num_allocated * rend->descriptor_size;

    prend->shader_descriptors[frame].num_allocated += num;
}
