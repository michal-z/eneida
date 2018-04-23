#pragma once
#include "types.h"

#define k_demo_name "demo01"
#define k_win_width 1280
#define k_win_height 720

typedef struct demo demo_t;

demo_t *demo_init(void *window);
void demo_shutdown(demo_t *demo);
void demo_update(demo_t *demo, f64 frame_time, f32 frame_delta_time);
void demo_draw(demo_t *demo);
