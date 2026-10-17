/**
 * @file menu_middle_exact_8006B6B4.c
 * @brief savedataBattleInit (0x8006B6B4 - 0x8006B8E8): resets the rule save
 *        section (six fixed 0x54-byte rules copied from
 *        menuCBRule_ConstantRule, the player counts, and the seven
 *        per-player flag pairs).
 *
 * One-function carve. It opens the code after menuCB_Bios.c (which ends at
 * 0x8006B6B4; see menu_middle_range_8006AF44.c) and is built with the menu
 * units' flags (GC/1.3, -O4,p, -opt nopeephole). Text only. The rule
 * records start at 0xC9DC. The flag pairs are cleared through a pointer
 * that advances two bytes per player, which is what keeps retail's
 * per-iteration address arithmetic after unrolling.
 */
#include "dolphin/types.h"

extern void* memset(void* dst, int val, u32 size);
extern u8 lbl_8047A5E0;

/* Retail copies each fixed 0x54-byte rule as a value record. */
typedef struct MenuRuleCopy {
    u32 word[0x15];
} MenuRuleCopy;

/* 0x8006B6B4 | size: 0x234 */
void savedataBattleInit(void* saveSection)
{
    extern void* menuCBRule_ConstantRule(s32 index);
    u8* status = (u8*)saveSection;
    s32 i;

    lbl_8047A5E0 = 0;
    memset(status, 0, 0xCC2C);
    status[0x1C] = 0;

    *(MenuRuleCopy*)(status + 0xC9DC) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);
    *(MenuRuleCopy*)(status + 0xCA30) = *(MenuRuleCopy*)menuCBRule_ConstantRule(1);
    *(MenuRuleCopy*)(status + 0xCA84) = *(MenuRuleCopy*)menuCBRule_ConstantRule(2);
    *(MenuRuleCopy*)(status + 0xCAD8) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);
    *(MenuRuleCopy*)(status + 0xCB2C) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);
    *(MenuRuleCopy*)(status + 0xCB80) = *(MenuRuleCopy*)menuCBRule_ConstantRule(0);

    *(u16*)(status + 0xCB86) = 6;
    *(u16*)(status + 0xCB32) = 6;
    *(u16*)(status + 0xCADE) = 6;
    status[0xCBD4] = 1;
    status[0xCBD5] = 1;
    status[0xCBD6] = 1;
    status[0xCBD7] = 0;
    status[0xCBD8] = 1;
    status[0xCBD9] = 0;
    {
        u8* p = status;
        for (i = 0; i < 7; i++, p += 2) {
            p[0xCBDB] = 0;
            p[0xCBDC] = 0;
        }
    }
}
