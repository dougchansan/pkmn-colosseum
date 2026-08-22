/** Exact-owner isolate for fn_800218BC at 0x800218BC. */
#include "dolphin/types.h"

extern s32 pokemonCheckValid(s32);
extern u8 pokemonIsDarkPokemon(s32);
extern f32 pokemonGetDp(void*);
extern void winMsgOpen(s32, s32, s32, s32);
extern void winMsgClose(s32);
extern void fn_80014118(s32, void*, void*);
extern s32 fn_800141BC(void*, s32);
extern void fn_80014198(s32);
extern u8 lbl_803A1B90[];
extern u32 lbl_8047B8A0;

s32 fn_800218BC(u32 arg0, u32* arg1)
{
    f32 thresh;
    void* sp_c;
    s32 sp_8;
    s32 count;
    s32 idx;
    s32 ok;
    s32 ctr;
    u8* p;
    s32 i;

    p = lbl_803A1B90;
    for (count = *((s32*)(p + 0x40)), ctr = count, i = 0; ctr > 0; ctr--) {
        if (arg0 == *((u16*)p)) {
            break;
        }
        p += 8;
        i++;
    }

    if (i >= count) {
        winMsgOpen(2, 0x426A, 1, 0);
        winMsgClose(1);
        return 1;
    }
    i = 0;
    thresh = *((f32*)(&lbl_8047B8A0));
    do {
        fn_80014118(i, &sp_c, &sp_8);
        if (((u8)pokemonCheckValid((s32)sp_c)) != 0) {
            if (pokemonIsDarkPokemon((s32)sp_c) != 0) {
                if (pokemonGetDp(sp_c) > thresh) {
                    break;
                }
            }
        }
        i++;
    } while (i < 6);

    if (i >= 6) {
        winMsgOpen(2, 0x4261, 1, 0);
        winMsgClose(1);
        return 1;
    }
    idx = fn_800141BC((void*)arg0, 1);
    if (idx >= 0) {
        fn_80014118(idx, &sp_c, &sp_8);
        ok = 0;
        if (pokemonIsDarkPokemon((s32)sp_c) != 0) {
            if (pokemonGetDp(sp_c) > *((f32*)(&lbl_8047B8A0))) {
                ok = 1;
            }
        }
        if (ok == 0) {
            winMsgOpen(2, 0x4261, 1, 0);
            winMsgClose(1);
        }
    }
    fn_80014198(idx);
    if ((idx >= 0) && (ok != 0)) {
        *arg1 = 1;
        return 2;
    }
    return 1;
}
