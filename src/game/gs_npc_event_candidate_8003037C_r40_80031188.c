#include "dolphin/types.h"

extern u8 lbl_803A2688[];

extern s32 heroGetStatus(void* partyData, s32 slot, s32 p3);
extern void msgctrlSetValue(s32 paramId, s32 value);
extern void fn_800FB680(s32 x, s32 y, u32 color, u16 messageId);

void fn_80031188(u8* r3, u8* r4)
{
    u32 flags = *(u32*) (r4 + 0x64);
    s32 evtype = *(s16*) (r4 + 0x6);
    u32 combined = (flags & 0xA1400000) | *(u8*) (r3 + 0x8B);

    switch (evtype) {
    case 0xF6B:
        fn_800FB680(0, 0, combined, 0x4412);
        break;
    case 0xF6A:
    {
        s32 val = heroGetStatus((void*) lbl_803A2688, 1, 0);
        msgctrlSetValue(0x4D, val);
        fn_800FB680(0, 0, combined, 0x4413);
        break;
    }
    }
}
