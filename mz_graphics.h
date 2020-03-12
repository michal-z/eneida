#ifndef MZ_GRAPHICS_INCLUDED_
#define MZ_GRAPHICS_INCLUDED_

#include "mz_d3d12.h"

#define MZGR_FAILED(hr) ((hr) < 0)
#define MZGR_VHR(hr)                                                                               \
  if ((hr) < 0) {                                                                                  \
    MZ_ASSERT(0);                                                                                  \
  }
#define MZGR_RELEASE(obj)                                                                          \
  if ((obj)) {                                                                                     \
    (obj)->vtbl->Release((obj));                                                                   \
    (obj) = NULL;                                                                                  \
  }

typedef struct mzgr_resource_handle {
  u16 index;
  u16 generation;
} mzgr_resource_handle_t;

typedef struct mzgr_pipeline_handle {
  u16 index;
  u16 generation;
} mzgr_pipeline_handle_t;

typedef struct mzgr_context mzgr_context_t;

mzgr_context_t *mzgr_create_context(void *window);

void mzgr_destroy_context(mzgr_context_t *gfx);

D3D12_CPU_DESCRIPTOR_HANDLE
mzgr_allocate_cpu_descriptors(mzgr_context_t *gfx, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count);

void mzgr_allocate_gpu_descriptors(mzgr_context_t *gfx, u32 count,
                                   D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle,
                                   D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle);

D3D12_GPU_DESCRIPTOR_HANDLE
mzgr_copy_descriptors_to_gpu_heap(mzgr_context_t *gfx, u32 count,
                                  D3D12_CPU_DESCRIPTOR_HANDLE src_base_handle);

void mzgr_allocate_upload_memory(mzgr_context_t *gfx, u32 size, u8 **out_cpu_addr,
                                 D3D12_GPU_VIRTUAL_ADDRESS *out_gpu_addr);

void mzgr_allocate_upload_buffer_region(mzgr_context_t *gfx, u32 size, u8 **out_cpu_addr,
                                        ID3D12Resource **out_buffer, u64 *out_buffer_offset);

mzgr_resource_handle_t mzgr_create_committed_resource(mzgr_context_t *gfx,
                                                      D3D12_HEAP_TYPE heap_type,
                                                      D3D12_HEAP_FLAGS heap_flags,
                                                      const D3D12_RESOURCE_DESC *desc,
                                                      D3D12_RESOURCE_STATES initial_state,
                                                      const D3D12_CLEAR_VALUE *clear_value);

void mzgr_destroy_resource(mzgr_context_t *gfx, mzgr_resource_handle_t handle);

ID3D12Resource *mzgr_get_resource(mzgr_context_t *gfx, mzgr_resource_handle_t handle);

ID3D12Device *mzgr_get_device(mzgr_context_t *gfx);

ID3D12GraphicsCommandList *mzgr_get_command_list(mzgr_context_t *gfx);

void mzgr_get_back_buffer(mzgr_context_t *gfx, mzgr_resource_handle_t *out_handle,
                          D3D12_CPU_DESCRIPTOR_HANDLE *out_rtv);

void mzgr_get_depth_stencil_buffer(mzgr_context_t *gfx, mzgr_resource_handle_t *out_handle,
                                   D3D12_CPU_DESCRIPTOR_HANDLE *out_dsv);

ID3D12GraphicsCommandList *mzgr_begin_frame(mzgr_context_t *gfx);

void mzgr_end_frame(mzgr_context_t *gfx, u32 swap_interval);

void mzgr_wait_for_gpu(mzgr_context_t *gfx);

void mzgr_transition_barrier(mzgr_context_t *gfx, mzgr_resource_handle_t handle,
                             D3D12_RESOURCE_STATES state_after);

#endif // #ifndef MZ_GRAPHICS_INCLUDED_

#ifdef MZ_GRAPHICS_IMPLEMENTATION

#include "mz_library.h"

#define _MZGR_MAX_NUM_RESOURCES 256
#define _MZGR_MAX_NUM_PIPELINES 128
#define _MZGR_NUM_FRAMES 2

typedef struct _mzgr_resource {
  ID3D12Resource *raw;
  D3D12_RESOURCE_STATES state;
  DXGI_FORMAT format;
} _mzgr_resource_t;

typedef struct _mzgr_pipeline {
  ID3D12PipelineState *pso;
  ID3D12RootSignature *rs;
} _mzgr_pipeline_t;

typedef struct _mzgr_descriptor_heap {
  ID3D12DescriptorHeap *heap;
  D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_start;
  u32 size;
  u32 capacity;
  u32 descriptor_size;
} _mzgr_descriptor_heap_t;

typedef struct _mzgr_gpu_memory_heap {
  ID3D12Resource *heap;
  u8 *cpu_start;
  D3D12_GPU_VIRTUAL_ADDRESS gpu_start;
  u32 size;
  u32 capacity;
} _mzgr_gpu_memory_heap_t;

typedef struct _mzgr_resource_pool {
  _mzgr_resource_t *resources;
  u16 *generations;
} _mzgr_resource_pool_t;

typedef struct _mzgr_pipeline_pool {
  _mzgr_pipeline_t *pipelines;
  u16 *generations;
} _mzgr_pipeline_pool_t;

typedef struct mzgr_context {
  ID3D12Device *device;
  ID3D12GraphicsCommandList *cmd_list;
  u32 resolution[2];
  u32 frame_index;
  ID3D12CommandQueue *cmd_queue;
  _mzgr_descriptor_heap_t rtv_heap;
  _mzgr_descriptor_heap_t dsv_heap;
  _mzgr_descriptor_heap_t cbv_srv_uav_cpu_heap;
  _mzgr_descriptor_heap_t cbv_srv_uav_gpu_heaps[_MZGR_NUM_FRAMES];
  _mzgr_gpu_memory_heap_t upload_memory_heaps[_MZGR_NUM_FRAMES];
  _mzgr_resource_pool_t resource_pool;
  _mzgr_pipeline_pool_t pipeline_pool;
  void *window;
  ID3D12CommandAllocator *cmd_alloc[_MZGR_NUM_FRAMES];
  u32 back_buffer_index;
  IDXGISwapChain3 *swap_chain;
  mzgr_resource_handle_t back_buffers[4];
  mzgr_resource_handle_t depth_stencil_buffer;
  ID3D12Fence *frame_fence;
  void *frame_fence_event;
  u64 num_frames;
} mzgr_context_t;

ID3D12Device *mzgr_get_device(mzgr_context_t *gfx) {
  MZ_ASSERT(gfx);
  return gfx->device;
}

ID3D12GraphicsCommandList *mzgr_get_command_list(mzgr_context_t *gfx) {
  MZ_ASSERT(gfx);
  return gfx->cmd_list;
}

static D3D12_CPU_DESCRIPTOR_HANDLE _mzgr_allocate_cpu_descriptors(_mzgr_descriptor_heap_t *dh,
                                                                  u32 count) {
  MZ_ASSERT(dh && (dh->size + count) < dh->capacity);

  D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
  cpu_handle.ptr = dh->cpu_start.ptr + (u64)dh->size * dh->descriptor_size;
  dh->size += count;

  return cpu_handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE
mzgr_allocate_cpu_descriptors(mzgr_context_t *gfx, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count) {
  MZ_ASSERT(gfx);
  if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
    return _mzgr_allocate_cpu_descriptors(&gfx->rtv_heap, count);
  } else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
    return _mzgr_allocate_cpu_descriptors(&gfx->dsv_heap, count);
  } else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
    return _mzgr_allocate_cpu_descriptors(&gfx->cbv_srv_uav_cpu_heap, count);
  }
  MZ_ASSERT(0);
  return (D3D12_CPU_DESCRIPTOR_HANDLE){0};
}

static _mzgr_descriptor_heap_t _mzgr_create_descriptor_heap(ID3D12Device *device, u32 capacity,
                                                            D3D12_DESCRIPTOR_HEAP_TYPE type,
                                                            D3D12_DESCRIPTOR_HEAP_FLAGS flags) {
  _mzgr_descriptor_heap_t dh = {0};
  dh.capacity = capacity;

  D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {0};
  heap_desc.NumDescriptors = capacity;
  heap_desc.Type = type;
  heap_desc.Flags = flags;

  MZGR_VHR(ID3D12Device_CreateDescriptorHeap(device, &heap_desc, &IID_ID3D12DescriptorHeap,
                                             (void **)&dh.heap));

  ID3D12DescriptorHeap_GetCPUDescriptorHandleForHeapStart(dh.heap, &dh.cpu_start);
  if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
    ID3D12DescriptorHeap_GetGPUDescriptorHandleForHeapStart(dh.heap, &dh.gpu_start);
  }

  dh.descriptor_size = ID3D12Device_GetDescriptorHandleIncrementSize(device, type);
  return dh;
}

static _mzgr_gpu_memory_heap_t _mzgr_create_gpu_memory_heap(ID3D12Device *device, u32 capacity,
                                                            D3D12_HEAP_TYPE type) {
  _mzgr_gpu_memory_heap_t mh = {0};
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
  MZGR_VHR(ID3D12Device_CreateCommittedResource(
      device, &(D3D12_HEAP_PROPERTIES){.Type = type}, D3D12_HEAP_FLAG_NONE, &buffer_desc,
      D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void **)&mh.heap));

  ID3D12Resource_Map(mh.heap, 0, &(D3D12_RANGE){0, 0}, (void **)&mh.cpu_start);
  mh.gpu_start = ID3D12Resource_GetGPUVirtualAddress(mh.heap);

  return mh;
}

static mzgr_resource_handle_t _mzgr_add_resource(_mzgr_resource_pool_t *pool, ID3D12Resource *raw,
                                                 D3D12_RESOURCE_STATES initial_state,
                                                 DXGI_FORMAT format) {
  MZ_ASSERT(pool && raw);

  u32 slot_idx = 0;
  for (u32 i = 1; i <= _MZGR_MAX_NUM_RESOURCES; ++i) {
    if (pool->resources[i].raw == NULL) {
      slot_idx = i;
      break;
    }
  }
  MZ_ASSERT(slot_idx > 0 && slot_idx <= _MZGR_MAX_NUM_RESOURCES);

  pool->resources[slot_idx].raw = raw;
  pool->resources[slot_idx].state = initial_state;
  pool->resources[slot_idx].format = format;

  return (mzgr_resource_handle_t){(u16)slot_idx, pool->generations[slot_idx] += 1};
}

static mzgr_pipeline_handle_t
_mzgr_add_pipeline(_mzgr_pipeline_pool_t *pool, ID3D12PipelineState *pso, ID3D12RootSignature *rs) {
  MZ_ASSERT(pool && pso && rs);

  u32 slot_idx = 0;
  for (u32 i = 1; i <= _MZGR_MAX_NUM_PIPELINES; ++i) {
    if (pool->pipelines[i].pso == NULL) {
      slot_idx = i;
      break;
    }
  }
  MZ_ASSERT(slot_idx > 0 && slot_idx <= _MZGR_MAX_NUM_PIPELINES);

  pool->pipelines[slot_idx].pso = pso;
  pool->pipelines[slot_idx].rs = rs;

  return (mzgr_pipeline_handle_t){(u16)slot_idx, pool->generations[slot_idx] += 1};
}

ID3D12Resource *mzgr_get_resource(mzgr_context_t *gfx, mzgr_resource_handle_t handle) {
  MZ_ASSERT(handle.index > 0 && handle.index <= _MZGR_MAX_NUM_RESOURCES);
  MZ_ASSERT(handle.generation > 0 &&
            handle.generation == gfx->resource_pool.generations[handle.index]);
  MZ_ASSERT(gfx->resource_pool.resources[handle.index].raw);
  return gfx->resource_pool.resources[handle.index].raw;
}

static _mzgr_resource_t *_mzgr_get_resource_state(mzgr_context_t *gfx,
                                                  mzgr_resource_handle_t handle) {
  MZ_ASSERT(handle.index > 0 && handle.index <= _MZGR_MAX_NUM_RESOURCES);
  MZ_ASSERT(handle.generation > 0 &&
            handle.generation == gfx->resource_pool.generations[handle.index]);
  MZ_ASSERT(gfx->resource_pool.resources[handle.index].raw);
  return &gfx->resource_pool.resources[handle.index];
}

mzgr_context_t *mzgr_create_context(void *window) {
  MZ_ASSERT(window);
  mzgr_context_t *gfx = MZL_MALLOC(sizeof(mzgr_context_t));
  mzl_memset(gfx, 0, sizeof(mzgr_context_t));

  IDXGIFactory4 *factory;
#ifdef _DEBUG
  MZGR_VHR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, &IID_IDXGIFactory4, (void **)&factory));
#else
  MZGR_VHR(CreateDXGIFactory2(0, &IID_IDXGIFactory4, (void **)&factory));
#endif
#ifdef _DEBUG
  {
    ID3D12Debug *dbg;
    D3D12GetDebugInterface(&IID_ID3D12Debug, (void **)&dbg);
    if (dbg) {
      ID3D12Debug_EnableDebugLayer(dbg);
      ID3D12Debug1 *dbg1;
      ID3D12Debug_QueryInterface(dbg, &IID_ID3D12Debug1, (void **)&dbg1);
      if (dbg1) {
        ID3D12Debug1_SetEnableGPUBasedValidation(dbg1, TRUE);
      }
      MZGR_RELEASE(dbg);
      MZGR_RELEASE(dbg1);
    }
  }
#endif
  if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_1, &IID_ID3D12Device, (void **)&gfx->device) <
      0) {
    MessageBox(window,
               "This application requires Windows 10 (May 2019) or newer and GPU with "
               "raytracing support.",
               "D3D12CreateDevice failed", MB_OK | MB_ICONERROR);
    ExitProcess(1);
  }

  gfx->window = window;

  D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {0};
  cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  cmd_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
  cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  MZGR_VHR(ID3D12Device_CreateCommandQueue(gfx->device, &cmd_queue_desc, &IID_ID3D12CommandQueue,
                                           (void **)&gfx->cmd_queue));

  DXGI_SWAP_CHAIN_DESC swap_chain_desc = {0};
  swap_chain_desc.BufferCount = 4;
  swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_desc.OutputWindow = window;
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swap_chain_desc.Windowed = TRUE;

  IDXGISwapChain *temp_swap_chain;
  MZGR_VHR(IDXGIFactory4_CreateSwapChain(factory, (IUnknown *)gfx->cmd_queue, &swap_chain_desc,
                                         &temp_swap_chain));
  MZGR_VHR(IDXGISwapChain_QueryInterface(temp_swap_chain, &IID_IDXGISwapChain3,
                                         (void **)&gfx->swap_chain));
  MZGR_RELEASE(temp_swap_chain);
  MZGR_RELEASE(factory);

  RECT rect;
  GetClientRect(window, &rect);
  gfx->resolution[0] = (u32)rect.right;
  gfx->resolution[1] = (u32)rect.bottom;

  for (u32 i = 0; i < _MZGR_NUM_FRAMES; ++i) {
    MZGR_VHR(ID3D12Device_CreateCommandAllocator(gfx->device, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 &IID_ID3D12CommandAllocator,
                                                 (void **)&gfx->cmd_alloc[i]));
  }

  gfx->rtv_heap = _mzgr_create_descriptor_heap(gfx->device, 1024, D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                                               D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
  gfx->dsv_heap = _mzgr_create_descriptor_heap(gfx->device, 1024, D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
                                               D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
  gfx->cbv_srv_uav_cpu_heap =
      _mzgr_create_descriptor_heap(gfx->device, 16 * 1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                   D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

  for (u32 i = 0; i < _MZGR_NUM_FRAMES; ++i) {
    gfx->cbv_srv_uav_gpu_heaps[i] =
        _mzgr_create_descriptor_heap(gfx->device, 16 * 1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                     D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
  }

  for (u32 i = 0; i < _MZGR_NUM_FRAMES; ++i) {
    gfx->upload_memory_heaps[i] =
        _mzgr_create_gpu_memory_heap(gfx->device, 8 * 1024 * 1024, D3D12_HEAP_TYPE_UPLOAD);
  }

  gfx->resource_pool.resources =
      MZL_MALLOC((_MZGR_MAX_NUM_RESOURCES + 1) * sizeof(_mzgr_resource_t));
  gfx->resource_pool.generations = MZL_MALLOC((_MZGR_MAX_NUM_RESOURCES + 1) * sizeof(u16));

  mzl_memset(gfx->resource_pool.resources, 0,
             (_MZGR_MAX_NUM_RESOURCES + 1) * sizeof(_mzgr_resource_t));
  mzl_memset(gfx->resource_pool.generations, 0, (_MZGR_MAX_NUM_RESOURCES + 1) * sizeof(u16));

  gfx->pipeline_pool.pipelines =
      MZL_MALLOC((_MZGR_MAX_NUM_PIPELINES + 1) * sizeof(_mzgr_pipeline_t));
  gfx->pipeline_pool.generations = MZL_MALLOC((_MZGR_MAX_NUM_PIPELINES + 1) * sizeof(u16));

  mzl_memset(gfx->pipeline_pool.pipelines, 0,
             (_MZGR_MAX_NUM_PIPELINES + 1) * sizeof(_mzgr_pipeline_t));
  mzl_memset(gfx->pipeline_pool.generations, 0, (_MZGR_MAX_NUM_PIPELINES + 1) * sizeof(u16));

  {
    D3D12_CPU_DESCRIPTOR_HANDLE handle =
        mzgr_allocate_cpu_descriptors(gfx, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 4);

    for (u32 i = 0; i < 4; ++i) {
      ID3D12Resource *buffer;
      MZGR_VHR(
          IDXGISwapChain3_GetBuffer(gfx->swap_chain, i, &IID_ID3D12Resource, (void **)&buffer));

      gfx->back_buffers[i] =
          _mzgr_add_resource(&gfx->resource_pool, buffer, D3D12_RESOURCE_STATE_PRESENT,
                             swap_chain_desc.BufferDesc.Format);

      ID3D12Device_CreateRenderTargetView(gfx->device, mzgr_get_resource(gfx, gfx->back_buffers[i]),
                                          NULL, handle);

      handle.ptr += gfx->rtv_heap.descriptor_size;
    }
  }

  {
    const D3D12_RESOURCE_DESC image_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
        .Format = DXGI_FORMAT_D32_FLOAT,
        .Width = gfx->resolution[0],
        .Height = gfx->resolution[1],
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .SampleDesc.Count = 1,
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
    };
    gfx->depth_stencil_buffer = mzgr_create_committed_resource(
        gfx, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, &image_desc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &(D3D12_CLEAR_VALUE){.Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = {1.0f, 0}});

    D3D12_CPU_DESCRIPTOR_HANDLE handle =
        mzgr_allocate_cpu_descriptors(gfx, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

    const D3D12_DEPTH_STENCIL_VIEW_DESC view_desc = {
        .Format = DXGI_FORMAT_D32_FLOAT,
        .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
        .Flags = D3D12_DSV_FLAG_NONE,
    };
    ID3D12Device_CreateDepthStencilView(
        gfx->device, mzgr_get_resource(gfx, gfx->depth_stencil_buffer), &view_desc, handle);
  }

  MZGR_VHR(ID3D12Device_CreateCommandList(gfx->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                          gfx->cmd_alloc[0], NULL, &IID_ID3D12GraphicsCommandList,
                                          (void **)&gfx->cmd_list));
  MZGR_VHR(ID3D12GraphicsCommandList_Close(gfx->cmd_list));

  MZGR_VHR(ID3D12Device_CreateFence(gfx->device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence,
                                    (void **)&gfx->frame_fence));
  gfx->frame_fence_event = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

  return gfx;
}

mzgr_resource_handle_t mzgr_create_committed_resource(mzgr_context_t *gfx,
                                                      D3D12_HEAP_TYPE heap_type,
                                                      D3D12_HEAP_FLAGS heap_flags,
                                                      const D3D12_RESOURCE_DESC *desc,
                                                      D3D12_RESOURCE_STATES initial_state,
                                                      const D3D12_CLEAR_VALUE *clear_value) {
  MZ_ASSERT(gfx && desc);

  ID3D12Resource *raw;
  MZGR_VHR(ID3D12Device_CreateCommittedResource(
      gfx->device, &(D3D12_HEAP_PROPERTIES){.Type = heap_type}, heap_flags, desc, initial_state,
      clear_value, &IID_ID3D12Resource, (void **)&raw));

  return _mzgr_add_resource(&gfx->resource_pool, raw, initial_state, desc->Format);
}

void mzgr_destroy_context(mzgr_context_t *gfx) {
  MZ_ASSERT(gfx);
  for (u32 i = 1; i <= _MZGR_MAX_NUM_RESOURCES; ++i) {
    MZGR_RELEASE(gfx->resource_pool.resources[i].raw);
  }
  MZL_FREE(gfx->resource_pool.resources);
  MZL_FREE(gfx->resource_pool.generations);

  for (u32 i = 1; i <= _MZGR_MAX_NUM_PIPELINES; ++i) {
    MZGR_RELEASE(gfx->pipeline_pool.pipelines[i].pso);
    MZGR_RELEASE(gfx->pipeline_pool.pipelines[i].rs);
  }
  MZL_FREE(gfx->pipeline_pool.pipelines);
  MZL_FREE(gfx->pipeline_pool.generations);

  MZGR_RELEASE(gfx->rtv_heap.heap);
  MZGR_RELEASE(gfx->dsv_heap.heap);
  MZGR_RELEASE(gfx->cbv_srv_uav_cpu_heap.heap);
  for (u32 i = 0; i < _MZGR_NUM_FRAMES; ++i) {
    MZGR_RELEASE(gfx->cbv_srv_uav_gpu_heaps[i].heap);
  }
  for (u32 i = 0; i < _MZGR_NUM_FRAMES; ++i) {
    MZGR_RELEASE(gfx->upload_memory_heaps[i].heap);
  }

  for (u32 i = 0; i < _MZGR_NUM_FRAMES; ++i) {
    MZGR_RELEASE(gfx->cmd_alloc[i]);
  }
  MZGR_RELEASE(gfx->swap_chain);
  if (gfx->frame_fence_event) {
    CloseHandle(gfx->frame_fence_event);
  }
  MZGR_RELEASE(gfx->frame_fence);
  MZGR_RELEASE(gfx->cmd_queue);
  MZGR_RELEASE(gfx->cmd_list);
  MZGR_RELEASE(gfx->device);
}

ID3D12GraphicsCommandList *mzgr_begin_frame(mzgr_context_t *gfx) {
  ID3D12CommandAllocator *cmdalloc = gfx->cmd_alloc[gfx->frame_index];
  ID3D12GraphicsCommandList *cmdlist = gfx->cmd_list;
  ID3D12CommandAllocator_Reset(cmdalloc);
  ID3D12GraphicsCommandList_Reset(cmdlist, cmdalloc, NULL);
  ID3D12GraphicsCommandList_SetDescriptorHeaps(cmdlist, 1,
                                               &gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index].heap);
  return cmdlist;
}

void mzgr_end_frame(mzgr_context_t *gfx, u32 swap_interval) {
  MZGR_VHR(ID3D12GraphicsCommandList_Close(gfx->cmd_list));
  ID3D12CommandQueue_ExecuteCommandLists(gfx->cmd_queue, 1, (ID3D12CommandList **)&gfx->cmd_list);

  IDXGISwapChain3_Present(gfx->swap_chain, swap_interval, 0);
  ID3D12CommandQueue_Signal(gfx->cmd_queue, gfx->frame_fence, ++gfx->num_frames);

  u64 num_gpu_frames = ID3D12Fence_GetCompletedValue(gfx->frame_fence);

  if ((gfx->num_frames - num_gpu_frames) >= _MZGR_NUM_FRAMES) {
    ID3D12Fence_SetEventOnCompletion(gfx->frame_fence, num_gpu_frames + 1, gfx->frame_fence_event);
    WaitForSingleObject(gfx->frame_fence_event, INFINITE);
  }

  gfx->frame_index = (gfx->frame_index + 1) % _MZGR_NUM_FRAMES;
  gfx->back_buffer_index = IDXGISwapChain3_GetCurrentBackBufferIndex(gfx->swap_chain);
  gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index].size = 0;
  gfx->upload_memory_heaps[gfx->frame_index].size = 0;
}

void mzgr_wait_for_gpu(mzgr_context_t *gfx) {
  ID3D12CommandQueue_Signal(gfx->cmd_queue, gfx->frame_fence, ++gfx->num_frames);
  ID3D12Fence_SetEventOnCompletion(gfx->frame_fence, gfx->num_frames, gfx->frame_fence_event);
  WaitForSingleObject(gfx->frame_fence_event, INFINITE);

  gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index].size = 0;
  gfx->upload_memory_heaps[gfx->frame_index].size = 0;
}

void mzgr_destroy_resource(mzgr_context_t *gfx, mzgr_resource_handle_t handle) {
  _mzgr_resource_t *resource = _mzgr_get_resource_state(gfx, handle);

  u32 refcount = ID3D12Resource_Release(resource->raw);
  if (refcount != 0) {
    MZ_ASSERT(0);
  }

  resource->raw = NULL;
  resource->state = D3D12_RESOURCE_STATE_COMMON;
  resource->format = DXGI_FORMAT_UNKNOWN;
}

void mzgr_transition_barrier(mzgr_context_t *gfx, mzgr_resource_handle_t handle,
                             D3D12_RESOURCE_STATES state_after) {
  _mzgr_resource_t *resource = _mzgr_get_resource_state(gfx, handle);

  if (state_after != resource->state) {
    // NOTE(mziulek): Buffer barriers and submit all at once? Support
    // subresource granularity?
    ID3D12GraphicsCommandList_ResourceBarrier(
        gfx->cmd_list, 1,
        &(D3D12_RESOURCE_BARRIER){.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                                  .Transition.pResource = resource->raw,
                                  .Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                                  .Transition.StateBefore = resource->state,
                                  .Transition.StateAfter = state_after});
    resource->state = state_after;
  }
}

void mzgr_get_back_buffer(mzgr_context_t *gfx, mzgr_resource_handle_t *out_handle,
                          D3D12_CPU_DESCRIPTOR_HANDLE *out_rtv) {
  MZ_ASSERT(out_handle && out_rtv);
  *out_handle = gfx->back_buffers[gfx->back_buffer_index];
  *out_rtv = gfx->rtv_heap.cpu_start;
  out_rtv->ptr += (u64)gfx->back_buffer_index * gfx->rtv_heap.descriptor_size;
  MZ_ASSERT(mzgr_get_resource(gfx, *out_handle));
}

void mzgr_get_depth_stencil_buffer(mzgr_context_t *gfx, mzgr_resource_handle_t *out_handle,
                                   D3D12_CPU_DESCRIPTOR_HANDLE *out_dsv) {
  MZ_ASSERT(out_handle && out_dsv);
  *out_handle = gfx->depth_stencil_buffer;
  *out_dsv = gfx->dsv_heap.cpu_start;
  MZ_ASSERT(mzgr_get_resource(gfx, *out_handle));
}

void mzgr_allocate_gpu_descriptors(mzgr_context_t *gfx, u32 count,
                                   D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle,
                                   D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle) {
  MZ_ASSERT(out_cpu_handle && out_gpu_handle);

  _mzgr_descriptor_heap_t *dh = &gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index];

  MZ_ASSERT((dh->size + count) < dh->capacity);

  out_cpu_handle->ptr = dh->cpu_start.ptr + (u64)dh->size * dh->descriptor_size;
  out_gpu_handle->ptr = dh->gpu_start.ptr + (u64)dh->size * dh->descriptor_size;

  dh->size += count;
}

D3D12_GPU_DESCRIPTOR_HANDLE
mzgr_copy_descriptors_to_gpu_heap(mzgr_context_t *gfx, u32 count,
                                  D3D12_CPU_DESCRIPTOR_HANDLE src_base_handle) {
  D3D12_CPU_DESCRIPTOR_HANDLE cpu_base_handle;
  D3D12_GPU_DESCRIPTOR_HANDLE gpu_base_handle;
  mzgr_allocate_gpu_descriptors(gfx, count, &cpu_base_handle, &gpu_base_handle);
  ID3D12Device_CopyDescriptorsSimple(gfx->device, count, cpu_base_handle, src_base_handle,
                                     D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
  return gpu_base_handle;
}

static void _mzgr_allocate_gpu_memory(_mzgr_gpu_memory_heap_t *mheap, u32 size, u8 **out_cpu_addr,
                                      D3D12_GPU_VIRTUAL_ADDRESS *out_gpu_addr) {
  MZ_ASSERT(mheap && size > 0 && out_cpu_addr && out_gpu_addr);

  if (size & 0xff) {
    // Always align to 256 bytes.
    size = (size + 255) & ~0xff;
  }
  if ((mheap->size + size) >= mheap->capacity) {
    *out_cpu_addr = NULL;
    *out_gpu_addr = 0;
    return;
  }

  *out_cpu_addr = mheap->cpu_start + mheap->size;
  *out_gpu_addr = mheap->gpu_start + mheap->size;

  mheap->size += size;
}

void mzgr_allocate_upload_memory(mzgr_context_t *gfx, u32 size, u8 **out_cpu_addr,
                                 D3D12_GPU_VIRTUAL_ADDRESS *out_gpu_addr) {
  MZ_ASSERT(gfx && size > 0 && out_cpu_addr && out_gpu_addr);

  u8 *cpu_addr;
  D3D12_GPU_VIRTUAL_ADDRESS gpu_addr;
  _mzgr_allocate_gpu_memory(&gfx->upload_memory_heaps[gfx->frame_index], size, &cpu_addr,
                            &gpu_addr);

  if (cpu_addr == NULL && gpu_addr == 0) {
    MZGR_VHR(ID3D12GraphicsCommandList_Close(gfx->cmd_list));
    ID3D12CommandQueue_ExecuteCommandLists(gfx->cmd_queue, 1, (ID3D12CommandList **)&gfx->cmd_list);
    mzgr_wait_for_gpu(gfx);
    mzgr_begin_frame(gfx);
  }

  _mzgr_allocate_gpu_memory(&gfx->upload_memory_heaps[gfx->frame_index], size, &cpu_addr,
                            &gpu_addr);
  MZ_ASSERT(cpu_addr != NULL && gpu_addr != 0);

  *out_cpu_addr = cpu_addr;
  *out_gpu_addr = gpu_addr;
}

void mzgr_allocate_upload_buffer_region(mzgr_context_t *gfx, u32 size, u8 **out_cpu_addr,
                                        ID3D12Resource **out_buffer, u64 *out_buffer_offset) {
  MZ_ASSERT(gfx && size > 0 && out_cpu_addr && out_buffer && out_buffer_offset);

  if (size & 0xff) {
    // Always align to 256 bytes.
    size = (size + 255) & ~0xff;
  }

  u8 *cpu_addr;
  D3D12_GPU_VIRTUAL_ADDRESS gpu_addr;
  mzgr_allocate_upload_memory(gfx, size, &cpu_addr, &gpu_addr);

  *out_cpu_addr = cpu_addr;
  *out_buffer = gfx->upload_memory_heaps[gfx->frame_index].heap;
  *out_buffer_offset = gfx->upload_memory_heaps[gfx->frame_index].size - size;
}

#undef MZ_GRAPHICS_IMPLEMENTATION
#endif // #ifdef MZ_GRAPHICS_IMPLEMENTATION
