#pragma once

#define k_demo_name "demo01"
#define k_win_width 1920
#define k_win_height 1080

void demo_update(double frame_time, float frame_delta_time);
void demo_draw(void);
void demo_init(void);
void demo_shutdown(void);
