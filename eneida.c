#define MZ_TINYWIN_IMPLEMENTATION
#include "mz_tinywin.h"
#define MZ_LIBRARY_IMPLEMENTATION
#include "mz_library.h"
#define MZ_CD3D12_IMPLEMENTATION
#include "mz_cd3d12.h"

int _fltused;

void start(void)
{
    mztw_load_api();
    mzcd3d12_load_api();

    SetProcessDPIAware();

    void *window = mzl_create_window("eneida", 1920, 1080);

    for (;;) {
        MSG message = { 0 };
        if (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&message);
            if (message.message == WM_QUIT) {
                break;
            }
        } else {
            f64 frame_time;
            f32 frame_delta_time;
            mzl_update_frame_stats(window, "eneida", &frame_time, &frame_delta_time);
            Sleep(1);
        }
    }

    ExitProcess(0);
}
