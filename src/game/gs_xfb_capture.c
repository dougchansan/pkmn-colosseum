/**
 * @file gs_xfb_capture.c
 * @brief GSgfxCapture -- framebuffer (XFB) capture debug feature
 *        (0x80175F6C - 0x80176068).
 *
 * Split from the former game/gs_scene.c CodeCandidate bucket
 * (0x8017572C - 0x8017A5FC); see config/GC6E01/splits.txt for the exact
 * address ranges of the four resulting translation units:
 *   game/generator.c                    0x80173624 - 0x80175F6C
 *   game/gs_xfb_capture.c               0x80175F6C - 0x80176068 (this file)
 *   game/gs_spline.c                    0x80176068 - 0x801765F4
 *   game/camera.c                       0x801765F4 - 0x80179F4C
 *
 * This four-function span has NO XD counterpart translation unit: XD's
 * generator.cpp end equals GSspline.cpp start with zero gap, so these
 * XFB-dump debug functions were deleted in the XD-era codebase.
 * Classification rests on the "gs%04d.xfb" rodata block position and
 * the call into gs_render_util's fn_800D305C; a separate TU is the
 * honest reading since these functions share no globals/calls with the
 * spline code in gs_spline.c. Shared externs/typedefs for the whole
 * former gs_scene.c range live in include/game/gs_scene_types.h.
 *
 * Functions (4, per config/GC6E01/symbols.txt):
 *   fn_80175F6C          (0x80175F6C, not yet decompiled -- "GSgfxCaptureUpdate")
 *   fn_80175FFC          (0x80175FFC, sda21 getter, matched)
 *   GSgfxCaptureMovieStop  (0x80176004, was fn_80176004)
 *   GSgfxCaptureMovieStart (0x80176030, was fn_80176030)
 */

#include "game/gs_scene_types.h"

extern u8 lbl_8047B1A0;
void GSgfxCaptureUpdate(void) {
    typedef struct CaptureEntry {
        u8 pad[0x58];
        void* xfb;
        u8 pad5C[4];
    } CaptureEntry;
    char filename[16];
    extern CaptureEntry lbl_80466BC0[];
    extern u32 HSD_VIGetXFBLastDrawDone(void);
    extern int sprintf(char* dst, const char* format, ...);

    if (lbl_8047B1A0 == 1) {
        u32 index = HSD_VIGetXFBLastDrawDone();
        CaptureEntry* entry = lbl_80466BC0;
        entry += index;
        if (entry->xfb != 0) {
            sprintf(filename, lbl_80273A00, lbl_8047B1A4++);
            if (lbl_8047B1A4 >= 0x4650) {
                lbl_8047B1A0 = 0;
                fn_800D305C(1);
            }
        }
    } else {
        lbl_8047B1A0 = 0;
    }
}

u8 fn_80175FFC(void) {
    return lbl_8047B1A0;
}
#if 0
asm void GSgfxCaptureMovieStop(void) {
#include "src/game/gs_scene_fn_80176004.inc"
}
#else
void GSgfxCaptureMovieStop(void) {
    lbl_8047B1A0 = 0;
    fn_800D305C(1);
}
#endif
#if 0
asm void GSgfxCaptureMovieStart(void) {
#include "src/game/gs_scene_fn_80176030.inc"
}
#else
void GSgfxCaptureMovieStart(u32 param) {
    lbl_8047B1A2 = param;
    lbl_8047B1A0 = 1;
    lbl_8047B1A4 = 0;
    fn_800D305C(0);
}
#endif
