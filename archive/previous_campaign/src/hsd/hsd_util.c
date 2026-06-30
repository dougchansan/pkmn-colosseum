/**
 * @file hsd_util.c
 * @brief HSD internal functions (0x8019C3C4-0x8019C690).
 *
 * Stub coverage for 1 functions.
 */

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"

typedef struct __va_list_struct {
    u8  gpr;
    u8  fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} __va_list_struct;
typedef __va_list_struct va_list[1];
extern void* __va_arg(void* ap, u32 type);
#define va_start(ap, last) ((void)last, __builtin_va_info(&ap))
#define va_end(ap)         ((void)0)
#define va_arg(ap, type)   (*(type*)__va_arg(ap, 1))

extern u8 lbl_80274818[];
extern u32 lbl_80478C74;
extern u32 lbl_80478C7C;
extern u32 lbl_80478C80;
extern u32 lbl_8047B270;
extern u32 lbl_8047B274;
extern s32 lbl_8047B280;
extern u32 lbl_8047B284;
extern u32 lbl_8047B288;
extern s32 lbl_8047B28C;
extern u32 fn_8009F3D4(void);
extern void OSReport(const char* fmt, ...);
extern void _HSD_MemSetCallbacks();

/* 0x8019C3C4 | 0x2CC */
int fn_8019C3C4(u32 cmd, ...) {
    char* strings;
    va_list ap;
    u32 buf[5];
    u32 result;
    u32 val;
    u32 first;
    u32 second;
    s32 sval;

    strings = (char*)lbl_80274818;
    result = 0;
    if (lbl_8047B280 != 0) {
        if (lbl_8047B28C == 0) {
            OSReport(strings + 0x8c);
            lbl_8047B28C = 1;
        }
        return result;
    }

    va_start(ap, cmd);
    switch (cmd) {
    case 0:
        val = va_arg(ap, u32);
        if (val != 0) {
            lbl_80478C7C = val;
            result = 1;
        }
        break;
    case 1:
        val = va_arg(ap, u32);
        if (val != 0) {
            lbl_80478C80 = val;
            result = 1;
        }
        break;
    case 2:
        val = va_arg(ap, u32);
        if (val < fn_8009F3D4()) {
            lbl_8047B284 = val;
            result = 1;
        }
        break;
    case 3:
        first = va_arg(ap, u32);
        second = va_arg(ap, u32);
        lbl_8047B270 = first;
        lbl_8047B274 = second;
        result = 1;
        break;
    case 4:
        buf[0] = va_arg(ap, u32);
        buf[1] = va_arg(ap, u32);
        buf[2] = va_arg(ap, u32);
        buf[3] = va_arg(ap, u32);
        buf[4] = va_arg(ap, u32);
        if (va_arg(ap, u32) != 0) {
            OSReport(strings + 0xc8);
            OSReport(strings + 0xec);
            break;
        }
        _HSD_MemSetCallbacks(buf, 0x14);
        lbl_8047B288 = 1;
        result = 1;
        break;
    case 5:
        val = va_arg(ap, u32);
        if (val != 0) {
            lbl_80478C74 = val;
            result = 1;
        }
        break;
    case 6:
        OSReport(strings + 0xc8);
        OSReport(strings + 0x128);
        sval = va_arg(ap, s32);
        if (sval == 0) {
            result = 1;
        }
        break;
    case 7:
        OSReport(strings + 0xc8);
        OSReport(strings + 0x160);
        break;
    }
    return result;
}

