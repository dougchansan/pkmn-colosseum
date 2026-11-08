/* Standalone copy of fn_8003D818 from pda_range_80037158.c, linked on its own. */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;

/* Memo id table loader; the shared PDA source also uses it in fn_8003D4C8. */
/* RULE-EXCEPTION(user-approved): single-use inline helper pda_8003D818_LoadIds in this standalone copy (fn_8003D818) - see docs/RULE_EXCEPTIONS.md */
static inline u16 pda_8003D818_LoadIds(void)
{
    extern u16* lbl_8047A4E4;
    extern u16 memoDataGetPokemonID(u16* data, u32 index);
    extern u16 memoDataGetCount(u16* data);
    u32 index;
    u16 count;

    count = 0;
    for (index = 0; (u16)index < memoDataGetCount(0); index++) {
        lbl_8047A4E4[(u16)index] = memoDataGetPokemonID(0, index);
        count++;
    }
    return count;
}

/* Reload the memo id table, clear the list filters and refresh the list. */
u16 pdaMemoBuildList(void)
{
    extern u16 lbl_8047A4E8;
    extern void pdaMemoDrawData(void);
    u8* work;

    work = (u8*)&lbl_803A6818;
    lbl_8047A4E8 = pda_8003D818_LoadIds();
    work[0x158] = 0;
    work[0x159] = 0;
    work[0x15b] = 0;
    work[0x15a] = 0;
    work[0x15c] = 0;
    pdaMemoDrawData();
    lbl_803A6818.field_10 = lbl_8047A4E8;
    return lbl_8047A4E8;
}
