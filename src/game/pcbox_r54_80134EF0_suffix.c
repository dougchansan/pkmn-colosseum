#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"

void* getPokemon__5PCBOXFScSc(void* base, s8 slot, s8 entry)
{
    u8* result;

    if (base == NULL) {
        base = (void*)savedataGetStatus(0, 3);
    }

    if (slot < 0 || slot >= 3) {
        result = NULL;
        goto done;
    }

    if (entry < 0 || entry >= 0x1E) {
        result = NULL;
        goto done;
    }

    result = (u8*)base + (s32)slot * 0x24A4 + (s32)entry * 0x138 + 0x14;

done:
    return result;
}

void pcboxInit(void* base)
{
    extern void fn_800F96E4(void*, u32, u32);
    extern void pokemonInitAry(void*, u32);
    extern void fn_80142A88(void*, u32);
    u8* status;
    s32 i;
    u8* cur;

    status = base;
    if (base == NULL) {
        status = (u8*)savedataGetStatus(0, 3);
    }

    i = 0;
    cur = status;
    do {
        msgctrlSetValue(0x34, i + 1);
        fn_800F96E4(cur, 9, 0x32C9);
        pokemonInitAry(cur + 0x14, 0x1E);
        i++;
        cur += 0x24A4;
    } while (i < 3);

    fn_80142A88(status + 0x6DEC, 0xEB);
}

void pcboxSetStatus(void)
{
}
