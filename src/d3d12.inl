#include "d3d12.h"


inline ID3D12Resource *d3d_create_buffer(ID3D12Device *d3d, D3D12_HEAP_TYPE heap_type, u64 size)
{
    assert(heap_type != D3D12_HEAP_TYPE_CUSTOM);

    D3D12_HEAP_PROPERTIES heap_props = { .Type = heap_type };
    D3D12_RESOURCE_DESC buffer_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Width = size,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .SampleDesc.Count = 1
    };
    D3D12_RESOURCE_STATES state =
        (heap_type == D3D12_HEAP_TYPE_UPLOAD) ? D3D12_RESOURCE_STATE_GENERIC_READ : D3D12_RESOURCE_STATE_COPY_DEST;

    ID3D12Resource *res;
    VHR(ID3D12Device_CreateCommittedResource(d3d, &heap_props, D3D12_HEAP_FLAG_NONE, &buffer_desc, state, NULL,
                                             &IID_ID3D12Resource, &res));
    return res;
}
