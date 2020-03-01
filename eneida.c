#include "stb_ds.h"
#include "stb_perlin.h"
#define MZ_TINYWIN_IMPLEMENTATION
#include "mz_tinywin.h"
#define MZ_LIBRARY_IMPLEMENTATION
#include "mz_library.h"
#define MZ_D3D12_IMPLEMENTATION
#include "mz_d3d12.h"
#define MZ_GRAPHICS_IMPLEMENTATION
#include "mz_graphics.h"

int _fltused;

void start(void) {
  mztinywin_load_api();
  mzd3d12_load_api();
  mzl_load_api();

  SetProcessDPIAware();

  i32 *arr = NULL;
  // arrsetcap(arr, 128);
  for (i32 i = 0; i < 32; ++i) {
    arrpush(arr, i);
  }
  for (u64 i = 0; i < arrlenu(arr); ++i) {
    char buf[64];
    wsprintf(buf, "%d\n", arr[i]);
    OutputDebugString(buf);
  }

  void *window = mzl_create_window("eneida", 1920, 1080);
  mzgr_context_t *gfx = mzgr_create_context(window);

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
      mzgr_begin_frame(gfx);
      mzgr_end_frame(gfx, 0);
    }
  }

  mzgr_wait_for_gpu(gfx);
  mzgr_destroy_context(gfx);

  ExitProcess(0);
}
