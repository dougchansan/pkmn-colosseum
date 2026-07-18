/**
 * @file gs_party_access.c
 * @brief Debug-menu helpers for the 0x8000CA34-0x8000D290 split.
 */

#include "dolphin/types.h"

extern u32 menuIsCheck(s32);
extern void menuClose(s32);
extern void menuOpenCustom(s32, ...);
extern void menuSetPosition(s32, s32, s32);
extern void d2presentOpen(void);
extern void fn_801EF02C(void);
extern void fn_801EF080(void);
extern void fn_801EF0D4(void);
extern void memoDataSetMemoFlag(u32);
extern void memoInitDebug(u32);
extern void fn_80190528(u32);
extern void fn_800884BC(u32, u32, u32);
extern void fn_800FF660(void);
extern u32 fn_800FF560(void);
extern u32 GSthreadCreate(s32, u32, s32, s32, s32, u32);
extern u32 savedataGetStatus(s32, s32);
extern u32 heroBiosGetPokemonPtr(u32, s32);
extern u32 pokemonCheckValid(u32);
extern void fn_80097A38(u32, s32);
extern void fn_800FF730(u32);
extern void menuPokemonOpen(s32, s32, s32);
extern void GSthreadSetArgs(u32, s32, ...);
extern u32 evolutionOpen(u32, u32, u32, u16*, u32, u8*);
extern void menuShopOpen(u32);
extern void menuNameEntryOpen(u32, s32);
extern u32 heroGetStatus(u8*, u32, u32);
extern void fadeSet(u32, f32);
extern void fadeCheck(u32);
extern void _threadSwitch(void);
extern u8 lbl_804673F8[];
extern u32 lbl_80266770[];
extern u32 lbl_80266700[];
extern u32 lbl_802666E0[];
extern f32 lbl_8047B6E8;

/* Retail retains this otherwise-unreferenced debug-menu entry-point group. */
#pragma force_active on
#pragma peephole off

/* 0x8000CA34 */
s32 fn_8000CA34(void) {
    u32 r;
    r = menuIsCheck(0x9);
    if ((u8)r != 0) {
        menuClose(0x9);
    } else {
        menuOpenCustom(0x9, 0, 0, 0, 1, 0);
        menuSetPosition(0x9, 0xC, 0xA);
    }
    return 0;
}

#ifndef PCPORT
typedef struct {
    s8 b80 : 1;
    s8 b40 : 1;
    s8 b20 : 1;
    s8 b10 : 1;
    s8 b08 : 1;
    s8 b04 : 1;
    s8 b02 : 1;
    s8 b01 : 1;
} PartyFlags8;

typedef struct {
    u8 pad_00[0x08];
    PartyFlags8 modeFlags;
    u8 pad_09[0x86B];
    s32 activeFlag;
} BattleAnimState;
#endif

#pragma push
#pragma peephole off

/* 0x8000CAA4 */
s32 fn_8000CAA4(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x10;
#else
    PartyFlags8* p = &((BattleAnimState*)lbl_804673F8)->modeFlags;
    p->b10 ^= 1;
#endif
    return 0;
}

/* 0x8000CAD0 */
s32 fn_8000CAD0(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x20;
#else
    PartyFlags8* p = &((BattleAnimState*)lbl_804673F8)->modeFlags;
    p->b20 ^= 1;
#endif
    return 0;
}

/* 0x8000CAFC */
s32 fn_8000CAFC(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x40;
#else
    PartyFlags8* p = &((BattleAnimState*)lbl_804673F8)->modeFlags;
    p->b40 ^= 1;
#endif
    return 0;
}

/* 0x8000CB28 */
s32 fn_8000CB28(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x80;
#else
    PartyFlags8* p = &((BattleAnimState*)lbl_804673F8)->modeFlags;
    p->b80 ^= 1;
#endif
    return 0;
}
#pragma pop

/* 0x8000CB54 */
void fn_8000CB54(void) {
    d2presentOpen();
}

/* 0x8000CB74 */
u32 dbgMenuMenuTestD2Present(u32 arg) {
    u32 table[6];
    u32 idx = 0;
    u32 val;
    u32 r;
    u32 *p = lbl_80266770;
    table[0] = p[0];
    table[1] = p[1];
    table[2] = p[2];
    table[3] = p[3];
    table[4] = p[4];
    table[5] = p[5];
    for (idx = 0; idx < 3; idx++) {
        if ((s32)arg == (s32)table[idx * 2]) {
            break;
        }
    }
    val = table[idx * 2 + 1];
    r = GSthreadCreate(1, fn_800FF560(), 0x4000, 1, 1, (u32)(void(*)(void))fn_8000CB54);
    GSthreadSetArgs(r, 1, val);
    return 0;
}

/* 0x8000CC3C */
s32 fn_8000CC3C(void) {
    fn_801EF02C();
    return 1;
}

/* 0x8000CC60 */
s32 fn_8000CC60(void) {
    fn_801EF080();
    return 1;
}

/* 0x8000CC84 */
s32 fn_8000CC84(void) {
    fn_801EF0D4();
    return 1;
}

/* 0x8000CCA8 */
s32 fn_8000CCA8(void) {
    memoDataSetMemoFlag(0);
    return 1;
}

/* 0x8000CCD0 */
s32 fn_8000CCD0(void) {
    memoInitDebug(0);
    return 1;
}

/* 0x8000CCF8 */
s32 fn_8000CCF8(void) {
    fn_80190528(0x3F0);
    return 1;
}

/* 0x8000CD20 */
s32 fn_8000CD20(void) {
    fn_800884BC(0x99, 0x9C, 0x9F);
    return 0;
}

/* 0x8000CD50 */
void testEvolution__Fv(void) {
    typedef struct {
        u8 work[20];
        u16 left;
        u16 right;
        u8 reserved[32];
    } GSpartyEvolutionArgs;

    u32 val1, val2;
    volatile GSpartyEvolutionArgs locals;

    val1 = heroGetStatus(NULL, 3, 0);
    if ((u8)pokemonCheckValid(val1) == 0) { return; }
    val2 = heroGetStatus(NULL, 3, 1);
    if ((u8)pokemonCheckValid(val2) == 0) { return; }
    fadeSet(3, lbl_8047B6E8);
    fadeCheck(1);
    locals.left = 1;
    locals.right = 2;
    evolutionOpen(val1, val2, 1, (u16*)&locals.left, 2, (u8*)locals.work);
    fadeSet(2, lbl_8047B6E8);
    fadeCheck(1);
}

/* 0x8000CE18 */
s32 dbgMenuMenuTestEvolution(void) {
    u32 r;
    r = (u32)fn_800FF560();
    GSthreadCreate(1, r, 0x4000, 1, 1, (u32)testEvolution__Fv);
    return 0;
}

/* 0x8000CE5C */
#pragma peephole off
u32 dbgMenuMenuTestWazaMenu(void) {
    u32 val;
    val = savedataGetStatus(0, 2);
    if (val == 0) { return 0; }
    val = heroBiosGetPokemonPtr(val, 0);
    if ((u8)pokemonCheckValid(val) == 0) { return 0; }
    fn_80097A38(val, 0x25);
    return 0;
}
#pragma peephole on

/* 0x8000CED0 */
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
s32 fn_8000CED0(void) {
    fn_800FF730(0x391);
    return 0;
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on

/* 0x8000CEF8 */
#pragma peephole off
u32 dbgMenuMenuTestPokemonMenu(s32 arg) {
    switch (arg) {
        case 0x110:
            menuPokemonOpen(1, 0, 0);
            break;
        case 0x111:
            menuPokemonOpen(2, 0, 0);
            break;
        default:
            menuPokemonOpen(2, 0, 0);
            break;
    }
    return 0;
}
#pragma peephole on

/* 0x8000CF68 */
u32 fn_8000CF68(u32 arg) {
#pragma peephole off
    typedef struct {
        u32 key;
        u32 value;
    } GSpartyKeyValuePair;
    struct StatCopyBlk { GSpartyKeyValuePair data[14]; };
    GSpartyKeyValuePair local[14];
    u32 val;
    GSpartyKeyValuePair *p;
    u32 idx;
    int i;
    int j;
    *(struct StatCopyBlk*)local = *(struct StatCopyBlk*)lbl_80266700;
    p = local;
    idx = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 6; j++) {
            if ((s32)arg == (s32)p[j].key) { goto idx_done; }
            idx++;
        }
        if ((s32)arg == (s32)p[6].key) { goto idx_done; }
        p += 7;
        idx++;
    }
idx_done:
    if ((s32)idx >= 14) {
        return 0;
    }
    val = local[idx].value;
    menuShopOpen(val);
    return 0;
}

/* 0x8000D05C */
u32 dbgMenuMenuTestNameEntryMenu(u32 arg) {
    typedef struct {
        u32 data[8];
    } NameEntryTable;
    NameEntryTable table;
    u32 idx;
    u32 val;
    table = *(NameEntryTable*)lbl_802666E0;
    for (idx = 0; idx < 4; idx++) {
        if ((s32)arg == (s32)table.data[idx * 2]) {
            break;
        }
    }
    if ((s32)idx >= 4) {
        return 0;
    }
    val = table.data[idx * 2 + 1];
    menuNameEntryOpen(val, 0);
    return 0;
}
#pragma peephole reset

/* 0x8000D11C */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 dbgMenuWazaDebugStop(void) {
    s32 v = ((BattleAnimState*)lbl_804673F8)->activeFlag;
    if (v != 0) {
        fn_800FF660();
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

/* 0x8000D154 */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 fn_8000D154(void) {
    u32 r;
    r = menuIsCheck(0xCF);
    if ((u8)r != 0) {
        menuClose(0xCF);
    } else {
        menuOpenCustom(0xCF, 0, 0, 0, 1, 0);
        menuSetPosition(0xCF, 0xC, 0xA);
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

/* 0x8000D1C4 */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 fn_8000D1C4(void) {
    u32 r;
    r = menuIsCheck(0x78);
    if ((u8)r != 0) {
        menuClose(0x78);
    } else {
        menuOpenCustom(0x78, 0, 0, 0, 1, 0);
        menuSetPosition(0x78, 0xC, 0xA);
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

/* 0x8000D234 */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 dbgMenuWazaDebugStart(void) {
    u32 r;
    r = menuIsCheck(0x13);
    if ((u8)r != 0) {
        menuClose(0x13);
    }
    if (((BattleAnimState*)lbl_804673F8)->activeFlag == 0) {
        fn_800FF730(0x3E7);
        _threadSwitch();
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
