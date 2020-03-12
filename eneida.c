#define MZ_TINYWIN_IMPLEMENTATION
#include "mz_tinywin.h"
#define MZ_LIBRARY_IMPLEMENTATION
#include "mz_library.h"
#define MZ_D3D12_IMPLEMENTATION
#include "mz_d3d12.h"
#define MZ_GRAPHICS_IMPLEMENTATION
#include "mz_graphics.h"
#define MZ_MATH_IMPLEMENTATION
#include "mz_math.h"
#include "stb_ds.h"
#include "stb_perlin.h"

typedef struct test_api {
  void *context;
  i32 (*init)(void *context, void *window);
  void (*deinit)(void *context);
  void (*update)(void *context, f64 time, f32 delta_time);
} test_api_t;

#include "tests/test0.c"

int _fltused;

void start(void) {
  mztinywin_load_api();
  mzd3d12_load_api();
  mzl_load_api();

  SetProcessDPIAware();

  test_api_t tests[] = {
      get_test0(),
  };

  mzm_unit_tests();

  {
    f32 x = mzm_f32_sqrt(100.0f);
    MZ_ASSERT(x == 10.0f);
  }

  test_api_t *test = &tests[0];

  void *window = mzl_create_window("eneida", 1920, 1080);

  if (window && test->init(test->context, window)) {
    for (;;) {
      MSG message = {0};
      if (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
        DispatchMessage(&message);
        if (message.message == WM_QUIT) {
          break;
        }
      } else {
        f64 frame_time;
        f32 frame_delta_time;
        mzl_update_frame_stats(window, "eneida", &frame_time, &frame_delta_time);
        test->update(test->context, frame_time, frame_delta_time);
      }
    }
    test->deinit(test->context);
  }

  ExitProcess(0);
}
