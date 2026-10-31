/**
 * @file pda_exact_8003CF38.c
 * @brief PDA party list builder, 0x8003CF38 - 0x8003D1FC.
 *
 * Standalone copy of pdaSnagListBuild from pda_range_80037158.c. Unit flag
 * -opt nopeephole replaces the shared file's local "peephole off".
 */
#include "dolphin/types.h"

typedef struct PdaListEntry {
    u16 field_00;
    u16 battleId;
} PdaListEntry;

extern PdaListEntry* lbl_8047A4D4;
extern u32 lbl_8047A4D0;
extern u32 lbl_8047A4D8;
extern s32 lbl_8047A4DC;
extern u32 GSmemGetHandleByPtr();
extern u32 GSmemFree();
extern u32 fn_800E24B0();
extern u32 GSmemLock();
extern u32 GSmemAlloc();
extern s32 darkPokemonSaveGet4A4(u16 id);
extern void* memcpy(void* dst, const void* src, u32 size);

static inline void* pdaAlloc(s32 size, s32 align)
{
    u32 h = GSmemAlloc(size, align);

    if ((u16)h != 0) {
        return (void*)GSmemLock(h);
    }
    return NULL;
}

typedef struct PdaSortEntry {
    s32 key;
    PdaListEntry entry;
} PdaSortEntry;

void pdaSnagListBuild(void)
{
    extern u16 darkPokemonSaveGet4A0(u16 id);
    extern u8 darkPokemonCheckFlag16(u16 id);
    extern u16 darkPokemonGetFlag1C(u16 id);
    u8 temporary[8];
    s32 i;
    s32 count;
    u8* sortEntries;
    s32 j;
    s32 pos;
    s32 gap;
    u32 handle;
    u8* lower;
    s32 stride;
    u8* upper;

    lbl_8047A4DC = 0x60;
    lbl_8047A4D0 = 0;
    lbl_8047A4D0 = (u32)pdaAlloc(0x140, 0x20);
    lbl_8047A4D4 =
        (PdaListEntry*)pdaAlloc(((lbl_8047A4DC * 4) + 0x1F) & ~0x1F, 0x20);
    lbl_8047A4D8 =
        (u32)pdaAlloc(((lbl_8047A4DC * 8) + 0x1F) & ~0x1F, 0x20);

    count = 0;
    for (i = 0; i < (s32)lbl_8047A4DC; i++) {
        u16 value;

        value = darkPokemonSaveGet4A0(i + 1);
        if (value != 0 &&
            (darkPokemonCheckFlag16(i + 1) != 0 ||
             darkPokemonGetFlag1C(i + 1) != 0)) {
            lbl_8047A4D4[count].field_00 = value;
            lbl_8047A4D4[count].battleId = (u16)(i + 1);
            ((PdaSortEntry*)lbl_8047A4D8)[count].key = darkPokemonSaveGet4A4(i + 1);
            ((PdaSortEntry*)lbl_8047A4D8)[count].entry.field_00 =
                lbl_8047A4D4[count].field_00;
            ((PdaSortEntry*)lbl_8047A4D8)[count].entry.battleId =
                lbl_8047A4D4[count].battleId;
            count++;
        }
    }
    lbl_8047A4DC = count;

    sortEntries = (u8*)lbl_8047A4D8;
    for (gap = count / 2; gap > 0; gap /= 2) {
        stride = gap * 8;
        for (pos = gap; pos < count; pos++) {
            j = pos - gap;
            lower = sortEntries + j * 8;
            while (j >= 0 &&
                   *(s32*)lower > *(s32*)(upper = sortEntries + (j + gap) * 8)) {
                memcpy(temporary, lower, 8);
                memcpy(lower, upper, 8);
                memcpy(upper, temporary, 8);
                lower -= stride;
                j -= gap;
            }
        }
    }
    for (i = 0; i < (s32)lbl_8047A4DC; i++) {
        lbl_8047A4D4[i].field_00 = ((PdaSortEntry*)lbl_8047A4D8)[i].entry.field_00;
        lbl_8047A4D4[i].battleId = ((PdaSortEntry*)lbl_8047A4D8)[i].entry.battleId;
    }

    handle = GSmemGetHandleByPtr(lbl_8047A4D8);
    if ((u16)handle != 0) {
        fn_800E24B0(handle);
        GSmemFree(handle);
    }
    lbl_8047A4D8 = 0;
}
