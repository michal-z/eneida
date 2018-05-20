#pragma once
#include "types.h"

#define k_demo_name "demo01"
#define k_win_width 1280
#define k_win_height 720

typedef struct experiment_dispatch
{
    void *experiment;
    void (*update)(void *experiment, f64 frame_time, f32 frame_delta_time);
    void (*draw)(void *experiment);
    void (*shutdown)(void *experiment);
} experiment_dispatch_t;

experiment_dispatch_t experiment01_init(void *window);
