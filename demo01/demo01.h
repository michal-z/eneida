#pragma once
#include "types.h"

#define k_demo_name "demo01"
#define k_win_width 1280
#define k_win_height 720

typedef struct demo_module
{
    void *data;
    void (*update)(void *data, f64 frame_time, f32 frame_delta_time);
    void (*draw)(void *data);
    void (*shutdown)(void *data);
} demo_module_t;

demo_module_t module_init_e01(void *window);
