#pragma once

#include "mz_fwd.h"

typedef struct test_api {
  void *context;
  i32 (*init)(void *context, void *window);
  void (*deinit)(void *context);
  void (*update)(void *context, f64 time, f32 delta_time);
} test_api_t;
