/* Tail split preserving the surrounding GC/2.5 -O4,p profile. */
#include "src/game/menu/cardesavedata.c"

s32 fn_80087AE8(u8* work, u32 flags)
{
    extern u32 fn_800D0F44(s32);
    extern u8 fn_80102620(s32);
    extern u8* fn_80105624(void);
    extern s8 fn_80106934(void);
    extern void _threadSwitch(void);
    u32 initialized;
    s32 result;

    work[0x28] = 0;
    work[0x29] = 0;
    work[0x2A] = 0;
    work[0x2B] = 0;
    initialized = 1;
    if ((s8)work[0x21] >= 0
     && fn_800D0F44((s8)work[0x21]) == 0x40000) {
        initialized = 0;
    }

    for (;;) {
        for (;;) {
            if (fn_80102620(0x10C) == 0) {
                break;
            }
            _threadSwitch();
        }
        if ((flags & 2) != 0 && (*(u16*)(fn_80105624() + 4) & 0x20) != 0) {
            *(u32*)(work + 0x28) = 2;
            return 0;
        }
        if (*(u32*)(work + 0x28) == 8) {
            return 0;
        }
        if ((flags & 1) != 0) {
            result = fn_80106934();
            if ((s8)result == 0
             || ((*(u16*)(fn_80105624() + 4) & 0x10) != 0
              && (s8)result == -1)) {
                *(u32*)(work + 0x28) = 1;
                return 1;
            }
        }
        if ((flags & 4) != 0) {
            result = fn_800D0F44((s8)work[0x21]);
            if (result != 0x80) {
                if (initialized != 0) {
                    if (result == 0x40000) {
                        *(u32*)(work + 0x28) = 4;
                        return 1;
                    }
                } else if (result != 0x40000) {
                    initialized = 1;
                }
            }
        }
        _threadSwitch();
    }
}
