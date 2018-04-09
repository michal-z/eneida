#pragma once
#include "types.h"

#define k_demo_name "demo01"
#define k_win_width 1280
#define k_win_height 720

void demo_update(double frame_time, float frame_delta_time);
void demo_draw(u32 frame_index);
void demo_init(IDXGISwapChain3 *swapchain, ID3D12Device *d3d, ID3D12CommandQueue *cmdqueue);
void demo_shutdown(void);
