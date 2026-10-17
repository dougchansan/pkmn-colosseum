/** Exact PDA-mail page-count callbacks, 0x8004C2D8 - 0x8004C3E4. */
#include "dolphin/types.h"

extern s32 mailGetNbMailInMailbox(void);
extern void msgctrlSetValue(s32 id, void* value);
extern void GSmsgPrintColor(s32 x, s32 y, u32 color, s32 msgId);
extern u32 GSmsgGetRect(s32 msgId);

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 fn_8004C2D8(u8* ctx, u8* p)
{
    s32 count;
    s32 pages;
    u32 color = (u32) ctx[0x8b] | 0xe66e0000u;
    count = mailGetNbMailInMailbox();
    if ((pages = (count + 9) / 10) <= 0) {
        pages = 1;
    }
    msgctrlSetValue(0x34, (void*) pages);
    GSmsgPrintColor(*(s16*) (p + 0x54) - (s32) (GSmsgGetRect(0xca) >> 16),
                 0, color, 0xca);
    return 0;
}
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole reset; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole reset

#pragma peephole off
s32 fn_8004C36C(u8* ctx, u8* p)
{
    u32 color = (u32) ctx[0x8b] | 0xe66e0000u;
    msgctrlSetValue(0x34, (void*) ((s8) ctx[0x94] + 1));
    GSmsgPrintColor(*(s16*) (p + 0x54) - (s32) (GSmsgGetRect(0xca) >> 16),
                 0, color, 0xca);
    return 0;
}
#pragma peephole reset
