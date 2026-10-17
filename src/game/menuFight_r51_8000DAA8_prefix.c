/** Exact menuFight menuFightDrawTimer, 0x8000DAE8 - 0x8000DC88. */
#include "dolphin/types.h"

extern void* menuSubCalcColor(void*, void*);
extern u16 lbl_802E4B98[];
extern u16 lbl_803A1B80[];

void menuFightDrawTimer(u8* ctx, u8* npc) {
    extern s32 windowGetParam(u8* a, s32 b);
    extern void msgctrlSetValue(s32 a, s32 b);
    extern void GSmsgPrintColor();
    extern void GSmsgPrintRect();
    s32 value;
    s32 hour;
    s32 minute;

    switch (*(s16*)(npc + 6)) {
    case 0x12AD:
        value = windowGetParam(ctx, 0);
        hour = value / 60;
        minute = value % 60;
        lbl_803A1B80[0] = lbl_802E4B98[hour / 10];
        lbl_803A1B80[2] = 0x3A;
        lbl_803A1B80[5] = 0;
        lbl_803A1B80[1] = lbl_802E4B98[hour % 10];
        lbl_803A1B80[3] = lbl_802E4B98[minute / 10];
        lbl_803A1B80[4] = lbl_802E4B98[minute % 10];
        msgctrlSetValue(0x37, (s32)lbl_803A1B80);
        GSmsgPrintColor(0, 0, (s32)menuSubCalcColor(ctx, npc), 0xCF);
        break;
    case 0x12AC:
    case 0x12AE:
        break;
    case 0x12AF:
        msgctrlSetValue(0x34, windowGetParam(ctx, 0));
        GSmsgPrintRect(0, 0, *(s16*)(npc + 0x54), *(s16*)(npc + 0x56),
                    (s32)menuSubCalcColor(ctx, npc), 0xDE);
        break;
    }
}
