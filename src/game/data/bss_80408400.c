#include "dolphin/types.h"

/* MWCC emits zero storage in reverse declaration order. Keep every mapped
 * object active so unreferenced retail storage retains its original extent. */
#pragma force_active on

#if !defined(BSS_80408400_ISOLATED)
#define BSS_80408400_PREFIX
#define BSS_80408400_AFTER_SYNTH
#define BSS_80408400_SYNTHMACROS_LEGACY
#define BSS_80408400_SUFFIX
#endif

#if defined(BSS_80408400_PREFIX)
u8 lbl_804271B0[0x20];
u8 lbl_80427190[0x20];
u8 lbl_80427170[0x20];
u8 lbl_80427150[0x20];
u8 lbl_80427130[0x20];
u8 lbl_80427110[0x20];
u8 lbl_804270F0[0x20];
u8 lbl_804270D0[0x20];
u8 lbl_804270B0[0x20];
u8 lbl_80427090[0x20];
u8 lbl_80427070[0x20];
u8 lbl_80427050[0x20];
u8 lbl_80427030[0x20];
u8 lbl_80427010[0x20];
u8 lbl_80426FF0[0x20];
u8 lbl_80426BD0[0x420];
u8 lbl_80408400[0x1E7D0];
#endif

/* musyx/runtime/synth.c owns 0x80434A10 - 0x80435FF8. */

#if defined(BSS_80408400_AFTER_SYNTH)
u8 lbl_80445F24[0xC];
u8 lbl_80445F18[0xC];
u8 lbl_80445EF8[0x20];
u8 lbl_8043DEF8[0x8000];
u8 lbl_8043D6F8[0x800];
u8 lbl_8043D2F8[0x400];
u8 lbl_8043CCF8[0x600];
u8 lbl_80438CF8[0x4000];
u8 lbl_804380F8[0xC00];
u8 lbl_804378F8[0x800];
u8 lbl_80435FF8[0x1900];
#endif

#if defined(BSS_80408400_SYNTHMACROS_LEGACY)
u8 lbl_80445F30[0x20];
#endif

#if defined(BSS_80408400_SUFFIX)
/* musyx/runtime/synthvoice.c owns 0x80445F50 - 0x80446F10. */
u8 lbl_80447860[0x600];
u8 lbl_80446F10[0x950];
#endif

#pragma force_active reset
