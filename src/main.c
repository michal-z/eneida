#include "mz_direct3d12.h"
#include "mz_library.h"
#define MZ_TEST1_IMPLEMENTATION
#include "tests/mz_test1.h"

i32 __stdcall WinMain(void *_0, void *_1, const char *_2, i32 _3)
{
    (void)_0;
    (void)_1;
    (void)_2;
    (void)_3;
    SetProcessDPIAware();

    void *window = mzl_create_window("demo", 1920, 1080);

    mztest1_context *test = mztest1_init(window);

    if (test) {
        for (;;) {
            MSG message = { 0 };
            if (PeekMessageA(&message, 0, 0, 0, PM_REMOVE)) {
                DispatchMessageA(&message);
                if (message.message == WM_QUIT) {
                    break;
                }
            } else {
                f64 time;
                f32 delta_time;
                mzl_update_frame_stats(window, "demo", &time, &delta_time);
                mztest1_update(test);
                mztest1_draw(test);
            }
        }
    }

    mztest1_shutdown(test);
    return 0;
}
