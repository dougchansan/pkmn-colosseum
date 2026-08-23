#define PR424_MEMO_SPLIT
#include "src/game/memo.c"

u32 memoDataSetMemoFlag(u16 *r3) {
    u32 uVar2;
    u32 uVar3;
    u32 uVar1;

    uVar3 = 0;
    if (r3 == (u16 *)0x0) {
        r3 = (u16 *)savedataGetStatus(0, 0xc);
    }
    uVar2 = 0;
    while ((u32)(u16)uVar2 < (u32)*r3) {
        uVar1 = (uVar2 & 0xffff) * 12;
        if ((*(volatile u16 *)((u8 *)r3 + uVar1 + 4) & 0x8000) != 0) {
            uVar3 = 1;
        }
        uVar2 = uVar2 + 1;
        *(volatile u16 *)((u8 *)r3 + uVar1 + 4) = *(volatile u16 *)((u8 *)r3 + uVar1 + 4) & 0x3fff;
    }
    return uVar3;
}

void memoDataSet(u16 *r3, u32 r4) {
    extern u32 pokemonBiosGetPokemonDataId();
    u16 *queue;
    u8 *entry;
    u32 r30;
    u32 count;
    u32 rawLookup;
    u16 lookup;
    u32 r5;
    u32 i;
    u32 r0;

    queue = r3;
    r30 = r4;
    if (queue == (u16 *)0) {
        queue = (u16 *)savedataGetStatus(0, 0xC);
    }
    rawLookup = pokemonBiosGetPokemonDataId(r30);
    count = *queue;
    for (i = 0; (u32)(u16)i < (u32)count; i = i + 1) {
        lookup = rawLookup;
        r5 = (i & 0xffff) * 12;
        r0 = (u32)*(u16 *)((u8 *)queue + r5 + 4) & 0x3FFF;
        if (r0 == lookup) {
            return;
        }
    }
    queue[(u32)count * 6 + 2] = (u16)(rawLookup | 0x8000);
    r5 = pokemonBiosGetRnd(r30);
    lookup = *queue;
    entry = (u8 *)queue;
    entry = entry + (u32)lookup * 12;
    *(u32 *)(entry + 0xC) = r5;
    r5 = pokemonBiosGetCatchTrainerRnd(r30);
    entry = (u8 *)queue;
    entry = entry + (u32)*queue * 12;
    *(u32 *)(entry + 0x8) = r5;
    *queue = (u16)(*queue + 1);
}

void memoInit(u16 *r3) {
    u32 r31;
    u32 r30;
    u16 *r29;
    u32 offsetMask;
    u8 *r28;
    s32 r27;

    r29 = r3;
    if (r29 == (u16 *)0) {
        r29 = (u16 *)savedataGetStatus(0, 0xC);
    }
    r31 = 0;
    r30 = r31;
    r27 = r31;
    *r29 = r30;
    r31 = r30;
    while (r27 < 0x1F4) {
        offsetMask = ~0u;
        r28 = (u8 *)r29 + (r31 & offsetMask);
        *(u16 *)(r28 + 4) = r30;
        *(u32 *)(r28 + 0xC) = _fadeEffectGetRandom__FUl(-1);
        r27++;
        r31 += 12;
    }
}
