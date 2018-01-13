#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef long i32;
typedef long long i64;

#define NULL ((void *)0)

#ifdef _DEBUG
#define assert(expression) if (!(expression)) { __debugbreak(); }
#else
#define assert(expression)
#endif

char *load_text_file(const char *filename);

void *mem_alloc(u64 size);
void mem_free(void *addr);

#define k_demo_name "demo01"
#define k_win_width 1920
#define k_win_height 1080

void demo_update(double frame_time, float frame_delta_time);
void demo_draw(void);
void demo_init(void);
void demo_shutdown(void);
