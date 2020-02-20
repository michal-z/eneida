#ifndef MZ_DIRECT3D12_INCLUDED__
#define MZ_DIRECT3D12_INCLUDED__

#include "mz_windows.h"

#define MZD3D_FAILED(hr) ((hr) < 0)
#define MZD3D_VHR(hr) \
    if ((hr) < 0) {   \
        assert(0);    \
    }
#define MZD3D_RELEASE(obj)           \
    if ((obj)) {                     \
        (obj)->vtbl->Release((obj)); \
        (obj) = NULL;                \
    }

typedef struct {
    u16 index;
    u16 generation;
} mzd3d_resource_handle;

typedef struct {
    u16 index;
    u16 generation;
} mzd3d_pipeline_handle;

typedef struct mzd3d_context mzd3d_context;

mzd3d_context *mzd3d_create_context(void *window);
void mzd3d_destroy_context(mzd3d_context *context);

D3D12_CPU_DESCRIPTOR_HANDLE mzd3d_allocate_cpu_descriptors(mzd3d_context *context, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count);

mzd3d_resource_handle mzd3d_create_committed_resource(mzd3d_context *context, D3D12_HEAP_TYPE heap_type, D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_DESC *desc, D3D12_RESOURCE_STATES initial_state, const D3D12_CLEAR_VALUE *clear_value);
void mzd3d_destroy_resource(mzd3d_context *context, mzd3d_resource_handle handle);
ID3D12Resource *mzd3d_get_resource(mzd3d_context *context, mzd3d_resource_handle handle);

ID3D12Device *mzd3d_get_device(mzd3d_context *context);
ID3D12GraphicsCommandList *mzd3d_get_command_list(mzd3d_context *context);

void mzd3d_get_back_buffer(mzd3d_context *context, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_rtv);
void mzd3d_get_depth_stencil_buffer(mzd3d_context *context, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_dsv);

ID3D12GraphicsCommandList *mzd3d_begin_frame(mzd3d_context *context);
void mzd3d_end_frame(mzd3d_context *context, u32 swap_interval);

void mzd3d_wait_for_gpu(mzd3d_context *context);

void mzd3d_transition_barrier(mzd3d_context *context, mzd3d_resource_handle handle, D3D12_RESOURCE_STATES state_after);

#endif // #ifndef MZ_DIRECT3D12_INCLUDED__

#if 0
typedef struct mz_gfx_context_i {
    void (*allocate_gpu_descriptors)(mz_gfx_context_o *self, u32 count, D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle);
    D3D12_GPU_DESCRIPTOR_HANDLE(*copy_descriptors_to_gpu_heap)(mz_gfx_context_o *self, u32 count, D3D12_CPU_DESCRIPTOR_HANDLE src_base_handle);
} mz_gfx_context_i;

mz_gfx_context_i *mz_get_gfx_context(void);
#endif

//
//
// Stop now, if you are only interested in the API.
// Below, you find the implementation.
//
//

#ifdef __INTELLISENSE__
// This makes MSVC intellisense work.
#define MZ_DIRECT3D12_IMPLEMENTATION
#endif

#ifdef MZ_DIRECT3D12_IMPLEMENTATION

#include "mz_libc.h"
#include "mz_library.h"

#define _MZD3D_MAX_NUM_RESOURCES 256
#define _MZD3D_MAX_NUM_PIPELINES 128
#define _MZD3D_NUM_FRAMES 2

typedef struct {
    ID3D12Resource *raw;
    D3D12_RESOURCE_STATES state;
    DXGI_FORMAT format;
} _mzd3d_resource;

typedef struct {
    ID3D12PipelineState *pso;
    ID3D12RootSignature *rs;
} _mzd3d_pipeline;

typedef struct {
    ID3D12DescriptorHeap *heap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start;
    u32 size;
    u32 capacity;
    u32 descriptor_size;
} _mzd3d_descriptor_heap;

typedef struct {
    ID3D12Resource *heap;
    u8 *cpu_start;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_start;
    u32 size;
    u32 capacity;
} _mzd3d_gpu_memory_heap;

typedef struct {
    _mzd3d_resource *resources;
    u16 *generations;
} _mzd3d_resource_pool;

typedef struct {
    _mzd3d_pipeline *pipelines;
    u16 *generations;
} _mzd3d_pipeline_pool;

struct mzd3d_context {
    ID3D12Device *device;
    ID3D12GraphicsCommandList *cmd_list;
    u32 resolution[2];
    u32 frame_index;
    ID3D12CommandQueue *cmd_queue;
    _mzd3d_descriptor_heap rtv_heap;
    _mzd3d_descriptor_heap dsv_heap;
    _mzd3d_descriptor_heap cbv_srv_uav_cpu_heap;
    _mzd3d_descriptor_heap cbv_srv_uav_gpu_heaps[_MZD3D_NUM_FRAMES];
    _mzd3d_gpu_memory_heap upload_memory_heaps[_MZD3D_NUM_FRAMES];
    _mzd3d_resource_pool resource_pool;
    _mzd3d_pipeline_pool pipeline_pool;
    void *window;
    ID3D12CommandAllocator *cmd_alloc[_MZD3D_NUM_FRAMES];
    u32 back_buffer_index;
    IDXGISwapChain3 *swap_chain;
    mzd3d_resource_handle back_buffers[4];
    mzd3d_resource_handle depth_stencil_buffer;
    ID3D12Fence *frame_fence;
    void *frame_fence_event;
    u64 num_frames;
};

ID3D12Device *mzd3d_get_device(mzd3d_context *context)
{
    assert(context);
    return context->device;
}

ID3D12GraphicsCommandList *mzd3d_get_command_list(mzd3d_context *context)
{
    assert(context);
    return context->cmd_list;
}

static D3D12_CPU_DESCRIPTOR_HANDLE _mzd3d_allocate_cpu_descriptors(_mzd3d_descriptor_heap *dh, u32 count)
{
    assert(dh && (dh->size + count) < dh->capacity);

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
    cpu_handle.ptr = dh->cpu_start.ptr + (u64)dh->size * dh->descriptor_size;
    dh->size += count;

    return cpu_handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE mzd3d_allocate_cpu_descriptors(mzd3d_context *context, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count)
{
    assert(context);
    if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
        return _mzd3d_allocate_cpu_descriptors(&context->rtv_heap, count);
    } else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
        return _mzd3d_allocate_cpu_descriptors(&context->dsv_heap, count);
    } else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        return _mzd3d_allocate_cpu_descriptors(&context->cbv_srv_uav_cpu_heap, count);
    }
    assert(0);
    return (D3D12_CPU_DESCRIPTOR_HANDLE) { 0 };
}

static _mzd3d_descriptor_heap _mzd3d_create_descriptor_heap(ID3D12Device *device, u32 capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    _mzd3d_descriptor_heap dh = { 0 };
    dh.capacity = capacity;

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = { 0 };
    heap_desc.NumDescriptors = capacity;
    heap_desc.Type = type;
    heap_desc.Flags = flags;

    MZD3D_VHR(device->vtbl->CreateDescriptorHeap(device, &heap_desc, &IID_ID3D12DescriptorHeap, (void **)&dh.heap));

    dh.heap->vtbl->GetCPUDescriptorHandleForHeapStart(dh.heap, &dh.cpu_start);
    if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
        dh.heap->vtbl->GetGPUDescriptorHandleForHeapStart(dh.heap, &dh.gpu_start);
    }

    dh.descriptor_size = device->vtbl->GetDescriptorHandleIncrementSize(device, type);
    return dh;
}

static _mzd3d_gpu_memory_heap _mzd3d_create_gpu_memory_heap(ID3D12Device *device, u32 capacity, D3D12_HEAP_TYPE type)
{
    _mzd3d_gpu_memory_heap mh = { 0 };
    mh.capacity = capacity;

    const D3D12_RESOURCE_DESC buffer_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Width = capacity,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .SampleDesc.Count = 1,
    };
    MZD3D_VHR(device->vtbl->CreateCommittedResource(device, &(D3D12_HEAP_PROPERTIES) { .Type = type }, D3D12_HEAP_FLAG_NONE, &buffer_desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void **)&mh.heap));

    mh.heap->vtbl->Map(mh.heap, 0, &(D3D12_RANGE) { 0, 0 }, (void **)&mh.cpu_start);
    mh.gpu_start = mh.heap->vtbl->GetGPUVirtualAddress(mh.heap);

    return mh;
}

static mzd3d_resource_handle _mzd3d_add_resource(_mzd3d_resource_pool *pool, ID3D12Resource *raw, D3D12_RESOURCE_STATES initial_state, DXGI_FORMAT format)
{
    assert(pool && raw);

    u32 slot_idx = 0;
    for (u32 i = 1; i <= _MZD3D_MAX_NUM_RESOURCES; ++i) {
        if (pool->resources[i].raw == NULL) {
            slot_idx = i;
            break;
        }
    }
    assert(slot_idx > 0 && slot_idx <= _MZD3D_MAX_NUM_RESOURCES);

    pool->resources[slot_idx].raw = raw;
    pool->resources[slot_idx].state = initial_state;
    pool->resources[slot_idx].format = format;

    return (mzd3d_resource_handle) { (u16)slot_idx, pool->generations[slot_idx] += 1 };
}

static mzd3d_pipeline_handle _mzd3d_add_pipeline(_mzd3d_pipeline_pool *pool, ID3D12PipelineState *pso, ID3D12RootSignature *rs)
{
    assert(pool && pso && rs);

    u32 slot_idx = 0;
    for (u32 i = 1; i <= _MZD3D_MAX_NUM_PIPELINES; ++i) {
        if (pool->pipelines[i].pso == NULL) {
            slot_idx = i;
            break;
        }
    }
    assert(slot_idx > 0 && slot_idx <= _MZD3D_MAX_NUM_PIPELINES);

    pool->pipelines[slot_idx].pso = pso;
    pool->pipelines[slot_idx].rs = rs;

    return (mzd3d_pipeline_handle) { (u16)slot_idx, pool->generations[slot_idx] += 1 };
}

ID3D12Resource *mzd3d_get_resource(mzd3d_context *context, mzd3d_resource_handle handle)
{
    assert(handle.index > 0 && handle.index <= _MZD3D_MAX_NUM_RESOURCES);
    assert(handle.generation > 0 && handle.generation == context->resource_pool.generations[handle.index]);
    assert(context->resource_pool.resources[handle.index].raw);
    return context->resource_pool.resources[handle.index].raw;
}

_mzd3d_resource *_mzd3d_get_resource_state(mzd3d_context *context, mzd3d_resource_handle handle)
{
    assert(handle.index > 0 && handle.index <= _MZD3D_MAX_NUM_RESOURCES);
    assert(handle.generation > 0 && handle.generation == context->resource_pool.generations[handle.index]);
    assert(context->resource_pool.resources[handle.index].raw);
    return &context->resource_pool.resources[handle.index];
}

mzd3d_context *mzd3d_create_context(void *window)
{
    assert(window);
    mzd3d_context *context = MZL_MALLOC(sizeof(mzd3d_context));
    memset(context, 0, sizeof(mzd3d_context));

    IDXGIFactory4 *factory;
#ifdef _DEBUG
    MZD3D_VHR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, &IID_IDXGIFactory4, (void **)&factory));
#else
    MZD3D_VHR(CreateDXGIFactory2(0, &IID_IDXGIFactory4, (void **)&factory));
#endif
#ifdef _DEBUG
    {
        ID3D12Debug *dbg;
        D3D12GetDebugInterface(&IID_ID3D12Debug, (void **)&dbg);
        if (dbg) {
            dbg->vtbl->EnableDebugLayer(dbg);
            ID3D12Debug1 *dbg1;
            dbg->vtbl->QueryInterface(dbg, &IID_ID3D12Debug1, (void **)&dbg1);
            if (dbg1) {
                dbg1->vtbl->SetEnableGPUBasedValidation(dbg1, TRUE);
            }
            MZD3D_RELEASE(dbg);
            MZD3D_RELEASE(dbg1);
        }
    }
#endif
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_1, &IID_ID3D12Device, (void **)&context->device) < 0) {
        MessageBoxA(window, "This application requires Windows 10 (May 2019) or newer and GPU with raytracing support.", "D3D12CreateDevice failed", MB_OK | MB_ICONERROR);
        exit(0);
    }

    context->window = window;

    D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = { 0 };
    cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cmd_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    MZD3D_VHR(context->device->vtbl->CreateCommandQueue(context->device, &cmd_queue_desc, &IID_ID3D12CommandQueue, (void **)&context->cmd_queue));

    DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
    swap_chain_desc.BufferCount = 4;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = window;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Windowed = TRUE;

    IDXGISwapChain *temp_swap_chain;
    MZD3D_VHR(factory->vtbl->CreateSwapChain(factory, (IUnknown *)context->cmd_queue, &swap_chain_desc, &temp_swap_chain));
    MZD3D_VHR(temp_swap_chain->vtbl->QueryInterface(temp_swap_chain, &IID_IDXGISwapChain3, (void **)&context->swap_chain));
    MZD3D_RELEASE(temp_swap_chain);
    MZD3D_RELEASE(factory);

    RECT rect;
    GetClientRect(window, &rect);
    context->resolution[0] = (u32)rect.right;
    context->resolution[1] = (u32)rect.bottom;

    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_VHR(context->device->vtbl->CreateCommandAllocator(context->device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (void **)&context->cmd_alloc[i]));
    }

    context->rtv_heap = _mzd3d_create_descriptor_heap(context->device, 1024, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    context->dsv_heap = _mzd3d_create_descriptor_heap(context->device, 1024, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    context->cbv_srv_uav_cpu_heap = _mzd3d_create_descriptor_heap(context->device, 16 * 1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        context->cbv_srv_uav_gpu_heaps[i] = _mzd3d_create_descriptor_heap(context->device, 16 * 1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    }
    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        context->upload_memory_heaps[i] = _mzd3d_create_gpu_memory_heap(context->device, 8 * 1024 * 1024, D3D12_HEAP_TYPE_UPLOAD);
    }

    context->resource_pool.resources = MZL_MALLOC((_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(_mzd3d_resource));
    context->resource_pool.generations = MZL_MALLOC((_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(u16));
    memset(context->resource_pool.resources, 0, (_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(_mzd3d_resource));
    memset(context->resource_pool.generations, 0, (_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(u16));

    context->pipeline_pool.pipelines = MZL_MALLOC((_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(_mzd3d_pipeline));
    context->pipeline_pool.generations = MZL_MALLOC((_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(u16));
    memset(context->pipeline_pool.pipelines, 0, (_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(_mzd3d_pipeline));
    memset(context->pipeline_pool.generations, 0, (_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(u16));

    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = mzd3d_allocate_cpu_descriptors(context, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 4);

        for (u32 i = 0; i < 4; ++i) {
            ID3D12Resource *buffer;
            MZD3D_VHR(context->swap_chain->vtbl->GetBuffer(context->swap_chain, i, &IID_ID3D12Resource, (void **)&buffer));
            context->back_buffers[i] = _mzd3d_add_resource(&context->resource_pool, buffer, D3D12_RESOURCE_STATE_PRESENT, swap_chain_desc.BufferDesc.Format);
            context->device->vtbl->CreateRenderTargetView(context->device, mzd3d_get_resource(context, context->back_buffers[i]), NULL, handle);
            handle.ptr += context->rtv_heap.descriptor_size;
        }
    }

    {
        const D3D12_RESOURCE_DESC image_desc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Format = DXGI_FORMAT_D32_FLOAT,
            .Width = context->resolution[0],
            .Height = context->resolution[1],
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .SampleDesc.Count = 1,
            .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
        };
        context->depth_stencil_buffer = mzd3d_create_committed_resource(context, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, &image_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &(D3D12_CLEAR_VALUE) { .Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = { 1.0f, 0 } });

        D3D12_CPU_DESCRIPTOR_HANDLE handle = mzd3d_allocate_cpu_descriptors(context, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

        const D3D12_DEPTH_STENCIL_VIEW_DESC view_desc = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
            .Flags = D3D12_DSV_FLAG_NONE,
        };
        context->device->vtbl->CreateDepthStencilView(context->device, mzd3d_get_resource(context, context->depth_stencil_buffer), &view_desc, handle);
    }

    MZD3D_VHR(context->device->vtbl->CreateCommandList(context->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, context->cmd_alloc[0], NULL, &IID_ID3D12GraphicsCommandList, (void **)&context->cmd_list));
    MZD3D_VHR(context->cmd_list->vtbl->Close(context->cmd_list));

    MZD3D_VHR(context->device->vtbl->CreateFence(context->device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void **)&context->frame_fence));
    context->frame_fence_event = CreateEventExA(NULL, NULL, 0, EVENT_ALL_ACCESS);

    mzd3d_begin_frame(context);

    return context;
}

mzd3d_resource_handle mzd3d_create_committed_resource(mzd3d_context *context, D3D12_HEAP_TYPE heap_type, D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_DESC *desc, D3D12_RESOURCE_STATES initial_state, const D3D12_CLEAR_VALUE *clear_value)
{
    assert(context);
    ID3D12Resource *raw;
    MZD3D_VHR(context->device->vtbl->CreateCommittedResource(context->device, &(D3D12_HEAP_PROPERTIES) { .Type = heap_type }, heap_flags, desc, initial_state, clear_value, &IID_ID3D12Resource, (void **)&raw));
    return _mzd3d_add_resource(&context->resource_pool, raw, initial_state, desc->Format);
}

void mzd3d_destroy_context(mzd3d_context *context)
{
    assert(context);
    for (uint32_t i = 1; i <= _MZD3D_MAX_NUM_RESOURCES; ++i) {
        MZD3D_RELEASE(context->resource_pool.resources[i].raw);
    }
    MZL_FREE(context->resource_pool.resources);
    MZL_FREE(context->resource_pool.generations);

    for (uint32_t i = 1; i <= _MZD3D_MAX_NUM_PIPELINES; ++i) {
        MZD3D_RELEASE(context->pipeline_pool.pipelines[i].pso);
        MZD3D_RELEASE(context->pipeline_pool.pipelines[i].rs);
    }
    MZL_FREE(context->pipeline_pool.pipelines);
    MZL_FREE(context->pipeline_pool.generations);

    MZD3D_RELEASE(context->rtv_heap.heap);
    MZD3D_RELEASE(context->dsv_heap.heap);
    MZD3D_RELEASE(context->cbv_srv_uav_cpu_heap.heap);
    for (uint32_t i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_RELEASE(context->cbv_srv_uav_gpu_heaps[i].heap);
    }
    for (uint32_t i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_RELEASE(context->upload_memory_heaps[i].heap);
    }

    for (uint32_t i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_RELEASE(context->cmd_alloc[i]);
    }
    MZD3D_RELEASE(context->swap_chain);
    if (context->frame_fence_event) {
        CloseHandle(context->frame_fence_event);
    }
    MZD3D_RELEASE(context->frame_fence);
    MZD3D_RELEASE(context->cmd_queue);
    MZD3D_RELEASE(context->cmd_list);
    MZD3D_RELEASE(context->device);
}

ID3D12GraphicsCommandList *mzd3d_begin_frame(mzd3d_context *context)
{
    ID3D12CommandAllocator *cmdalloc = context->cmd_alloc[context->frame_index];
    ID3D12GraphicsCommandList *cmdlist = context->cmd_list;
    cmdalloc->vtbl->Reset(cmdalloc);
    cmdlist->vtbl->Reset(cmdlist, cmdalloc, NULL);
    cmdlist->vtbl->SetDescriptorHeaps(cmdlist, 1, &context->cbv_srv_uav_gpu_heaps[context->frame_index].heap);
    return cmdlist;
}

void mzd3d_end_frame(mzd3d_context *context, u32 swap_interval)
{
    MZD3D_VHR(context->cmd_list->vtbl->Close(context->cmd_list));
    context->cmd_queue->vtbl->ExecuteCommandLists(context->cmd_queue, 1, (ID3D12CommandList **)&context->cmd_list);

    context->swap_chain->vtbl->Present(context->swap_chain, swap_interval, 0);
    context->cmd_queue->vtbl->Signal(context->cmd_queue, context->frame_fence, ++context->num_frames);

    const u64 num_gpu_frames = context->frame_fence->vtbl->GetCompletedValue(context->frame_fence);

    if ((context->num_frames - num_gpu_frames) >= _MZD3D_NUM_FRAMES) {
        context->frame_fence->vtbl->SetEventOnCompletion(context->frame_fence, num_gpu_frames + 1, context->frame_fence_event);
        WaitForSingleObject(context->frame_fence_event, INFINITE);
    }

    context->frame_index = (context->frame_index + 1) % _MZD3D_NUM_FRAMES;
    context->back_buffer_index = context->swap_chain->vtbl->GetCurrentBackBufferIndex(context->swap_chain);
    context->cbv_srv_uav_gpu_heaps[context->frame_index].size = 0;
    context->upload_memory_heaps[context->frame_index].size = 0;
}

void mzd3d_wait_for_gpu(mzd3d_context *context)
{
    context->cmd_queue->vtbl->Signal(context->cmd_queue, context->frame_fence, ++context->num_frames);
    context->frame_fence->vtbl->SetEventOnCompletion(context->frame_fence, context->num_frames, context->frame_fence_event);
    WaitForSingleObject(context->frame_fence_event, INFINITE);

    context->cbv_srv_uav_gpu_heaps[context->frame_index].size = 0;
    context->upload_memory_heaps[context->frame_index].size = 0;
}

void mzd3d_destroy_resource(mzd3d_context *gfx, mzd3d_resource_handle handle)
{
    _mzd3d_resource *resource = _mzd3d_get_resource_state(gfx, handle);

    u32 refcount = resource->raw->vtbl->Release(resource->raw);
    assert(refcount == 0);

    resource->raw = NULL;
    resource->state = D3D12_RESOURCE_STATE_COMMON;
    resource->format = DXGI_FORMAT_UNKNOWN;
}

void mzd3d_transition_barrier(mzd3d_context *context, mzd3d_resource_handle handle, D3D12_RESOURCE_STATES state_after)
{
    _mzd3d_resource *resource = _mzd3d_get_resource_state(context, handle);

    if (state_after != resource->state) {
        // NOTE(mziulek): Buffer barriers and submit all at once? Support subresource granularity?
        context->cmd_list->vtbl->ResourceBarrier(context->cmd_list, 1,
            &(D3D12_RESOURCE_BARRIER) {
                .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                .Transition.pResource = resource->raw,
                .Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .Transition.StateBefore = resource->state,
                .Transition.StateAfter = state_after });
        resource->state = state_after;
    }
}

void mzd3d_get_back_buffer(mzd3d_context *context, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_rtv)
{
    assert(out_handle && out_rtv);
    *out_handle = context->back_buffers[context->back_buffer_index];
    *out_rtv = context->rtv_heap.cpu_start;
    out_rtv->ptr += (size_t)context->back_buffer_index * context->rtv_heap.descriptor_size;
    assert(mzd3d_get_resource(context, *out_handle));
}

void mzd3d_get_depth_stencil_buffer(mzd3d_context *context, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_dsv)
{
    assert(out_handle && out_dsv);
    *out_handle = context->depth_stencil_buffer;
    *out_dsv = context->dsv_heap.cpu_start;
    assert(mzd3d_get_resource(context, *out_handle));
}

#if 0
inline void mz_allocate_gpu_descriptors(mz_gfx_context_t *gfx, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle)
{
    assert(out_cpu_handle && out_gpu_handle);
    mz_gfx_context_t::descriptor_heap_t *dh = &gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index];

    assert((dh->size + count) < dh->capacity);

    out_cpu_handle->ptr = dh->cpu_start.ptr + (size_t)dh->size * dh->descriptor_size;
    out_gpu_handle->ptr = dh->gpu_start.ptr + (size_t)dh->size * dh->descriptor_size;

    dh->size += count;
}

inline D3D12_GPU_DESCRIPTOR_HANDLE mz_copy_descriptors_to_gpu_heap(mz_gfx_context_t *gfx, uint32_t count, D3D12_CPU_DESCRIPTOR_HANDLE src_base_handle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_base_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_base_handle;
    mz_allocate_gpu_descriptors(gfx, count, &cpu_base_handle, &gpu_base_handle);
    gfx->device->CopyDescriptorsSimple(count, cpu_base_handle, src_base_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    return gpu_base_handle;
}
#endif

/*
void mz_allocate_gpu_memory(mz_gfx_context_t *context, uint32_t size, uint8_t **cpu_addr, D3D12_GPU_VIRTUAL_ADDRESS *gpu_addr) {
    assert(mheap && size > 0 && cpu_addr && gpu_addr);

    if (size & 0xff) {
        // Always align to 256 bytes.
        size = (size + 255) & ~0xff;
    }
    assert((mheap->size + size) < mheap->capacity);

    *cpu_addr = mheap->cpu_start + mheap->size;
    *gpu_addr = mheap->gpu_start + mheap->size;

    mheap->size += size;
}
*/

#endif // #ifdef MZ_DIRECT3D12_IMPLEMENTATION
