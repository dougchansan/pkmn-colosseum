#include "dolphin/types.h"

extern void GSmsgPrintColor(s32, s32, s32, void*);
extern s32 lbl_8047A4B4;

/* RULE-EXCEPTION(user-approved): local peephole-control pragma; see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma peephole off
s32 fn_80039A50(u8* sprite)
{
    GSmsgPrintColor(0, 0, *(s32*)(sprite + 0x88), (void*)lbl_8047A4B4);
    return 0;
}
#pragma pop
