/**
 * @file field_range_801CA7EC.c
 * @brief field/hero, 0x801CA7EC - 0x801CB180.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) -- mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
#include "dolphin/types.h"

/* ===== Data ===== */
extern s32 lbl_80478CB0;

typedef struct TemochiEntry {
    u32 field_0;
    u32 field_4;
} TemochiEntry;

extern TemochiEntry lbl_804670B4[];
extern const f32 lbl_8047E114;

/* ===== Engine / hero helpers ===== */
extern void* heroGetStatus(void* ptr, u32 a, u32 b);
extern u32   heroSetStatus(u8* ptr, u32 a, u32 b);
extern void  heroAddPokecoupon(u8* ptr, s32 offset);
extern void  heroDecPokecoupon(u8* ptr, s32 offset);
extern void  heroAddPokedoru(u8* ptr, u32 offset);
extern void  heroDecPokedoru(u8* ptr, u32 offset);
extern void* heroBiosGetPokemonPtr(void* status, u16 idx);

extern void* savedataGetStatus(u32 side, u32 slotType);

/* ===== Flag helpers ===== */
extern s32 fn_801906A0(u32 flag);
extern s32  _flagSet(u32 flag, s32 value);

/* ===== Pokemon helpers ===== */
extern u32  pokemonCheckValid(void* mon);
extern u32  pokemonGetStatus(void* mon, s32 a, s32 b, s32 c);
extern u32  pokemonBiosGetDarkFlag(void* mon);
extern f32  pokemonGetDp(void* mon);

/* ===== Special gift pokemon helpers ===== */
extern void* heroPokemonGetBlacky(void* status, u32 id);
extern void* heroPokemonGetEifie(void* status, u32 id);
extern void* heroPokemonGetPrasle(void* status, u32 id);
extern void* heroPokemonGetHouou(void* status, u32 id);
extern void* heroPokemonGetPikachu(void* status, u32 id);
extern void* heroPokemonGetCelebi(void* status, u32 id);

/* ===== Item / treasure ===== */
extern u32 heroItemCheckHaveItemDataId(u8* ptr, u32 itemId);
extern s32 heroItemAddItemDataId(u8* ptr, u32 itemId, u32 count, u32 arg4);
extern s32 heroItemDecItemDataId(u8* ptr, u32 itemId, u32 count, u32 arg4);
extern u32 floorEventGetTresure(s32 a, s32 b, s32 c);

/* ===== Collision / character ===== */
extern s32 GScolsys2SetObjEnable(s32 triIndex, s32 visible);
extern u8* fn_8011711C();
extern void  set__5GSvecFfff(f32* out, f32 x, f32 y, f32 z);
extern void  floorCharacterBiosSetPos(void* bios, f32* pos);
extern void  floorCharacterBiosSetVisibility();

/* ===== Misc ===== */
extern u32 exribbonSetEarthRibbon(void* mon);
extern void etctoolSetPokemonNakigoe();

#if defined(FIELD_801CA7EC_PREFIX_ACTIVE)
#if !defined(PR424_FIELD_PREFIX_SPLIT) || \
    defined(PR424_FIELD_801CA7EC_801CA9F0)
u32 scriptAddPokecoupon(s32 delta)
{
    heroGetStatus((u8*)0, 0xd, 0);
    if (delta >= 0) {
        heroAddPokecoupon((u8*)0, delta);
    } else {
        heroDecPokecoupon((u8*)0, -delta);
    }
    return (u32)heroGetStatus((u8*)0, 0xd, 0);
}

#pragma push
#pragma scheduling off
u32 fn_801CA858(u32 arg)
{
    return heroSetStatus((u8*)0, 0xd, arg);
}
#pragma pop

#pragma push
#pragma scheduling off
u32 fn_801CA884(void)
{
    return (u32)heroGetStatus((u8*)0, 0xd, 0);
}
#pragma pop

#pragma push
#pragma peephole off
s32 scriptAddPremium(s32 delta)
{
    s32 newVal;

    newVal = fn_801906A0(0xa9e);
    newVal += delta;
    _flagSet(0xa9e, newVal);
    return newVal;
}
#pragma pop

#pragma push
#pragma scheduling off
s32 scriptSetPremium(s32 value)
{
    return _flagSet(0xa9e, value);
}
#pragma pop

#pragma push
#pragma scheduling off
s32 scriptGetPremium(void)
{
    return fn_801906A0(0xa9e);
}
#pragma pop

u32 scriptAddPokedoru(s32 delta)
{
    if (delta >= 0) {
        heroAddPokedoru((u8*)0, delta);
    } else {
        heroDecPokedoru((u8*)0, -delta);
    }
    return (u32)heroGetStatus((u8*)0, 0xc, 0);
}

#pragma push
#pragma scheduling off
u32 scriptGetPokedoru(void)
{
    return (u32)heroGetStatus((u8*)0, 0xc, 0);
}
#pragma pop

s32 scriptSetEventCol(u8 enable)
{
    s32 id = lbl_80478CB0;
    if (id >= 0) {
        GScolsys2SetObjEnable(id, enable);
    }
    return id;
}
#endif

#if !defined(PR424_FIELD_PREFIX_SPLIT)
s32 scriptGetEventColID(void)
{
    return lbl_80478CB0;
}

s32 scriptSetEventColID(s32 id)
{
    s32 old = lbl_80478CB0;
    lbl_80478CB0 = id;
    return old;
}
#endif

#if !defined(PR424_FIELD_PREFIX_SPLIT) || \
    defined(PR424_FIELD_801CAA08_801CADA0)
#pragma push
#pragma scheduling off
s32 scriptSetCol(s32 index, u8 enable)
{
    if (index >= 0) {
        index = GScolsys2SetObjEnable(index, enable);
    }
    return index;
}
#pragma pop

#pragma push
#pragma scheduling off
u32 scriptHaveItem(u32 itemId)
{
    return heroItemCheckHaveItemDataId((u8*)0, itemId) & 0xFF;
}
#pragma pop

u32 scriptAddItem(u16 itemId, s32 count)
{
    s32 ret = 0;
    if (count > 0) {
        ret = heroItemAddItemDataId((u8*)0, itemId, (u16)count, -1);
    } else if (count < 0) {
        ret = heroItemDecItemDataId((u8*)0, itemId, (u16)(-count), -1);
    }
    return ret;
}

#pragma push
#pragma scheduling off
u32 scriptGetItem(s32 a, s32 b)
{
    return floorEventGetTresure(4, a, b);
}
#pragma pop

u32 scriptCheckTemochiPokemon(u8* arg)
{
    u8 used[6];
    s32 i, j;

    used[0] = 0;
    used[1] = 0;
    used[2] = 0;
    used[3] = 0;
    used[4] = 0;
    used[5] = 0;

    for (i = 0; i < 6; i++) {
        if (lbl_804670B4[i].field_4 != 0) {
            for (j = 0; j < 6; j++) {
                if (used[j]) {
                    continue;
                }
                {
                    void* mon = heroGetStatus(arg, 3, j);
                    if (mon != NULL && pokemonCheckValid(mon) != 0) {
                        u16 v1 = (u16)pokemonGetStatus(mon, 0, 0x6e, 0);
                        u32 v2 = pokemonGetStatus(mon, 0, 0x6f, 0);
                        if (v1 == lbl_804670B4[i].field_4 && v2 == lbl_804670B4[i].field_0) {
                            used[j] = 1;
                            break;
                        }
                    }
                }
            }
            if (j == 6) {
                return 1;
            }
        }
    }

    for (i = 0; i < 6; i++) {
        if (!used[i]) {
            void* mon = heroGetStatus(arg, 3, i);
            if (mon != NULL && pokemonCheckValid(mon) != 0) {
                return 1;
            }
        }
    }
    return 0;
}

void scriptStoreTemochiPokemon(u8* arg)
{
    TemochiEntry* entry = lbl_804670B4;
    s32 i;

    for (i = 0; i < 6; i++) {
        entry[i].field_4 = 0;
        {
            void* mon = heroGetStatus(arg, 3, i);
            if (mon != NULL && pokemonCheckValid(mon) != 0) {
                u16 nakigoe = (u16)pokemonGetStatus(mon, 0, 0x6e, 0);
                u32 pid = pokemonGetStatus(mon, 0, 0x6f, 0);
                entry[i].field_4 = nakigoe;
                entry[i].field_0 = pid;
            }
        }
    }
}

#pragma push
#pragma scheduling off
u32 scriptGetEarthRibbon(void)
{
    void* status = savedataGetStatus(0, 2);
    u16 i;

    for (i = 0; i < 6; i++) {
        void* mon = heroBiosGetPokemonPtr(status, i);
        if (pokemonCheckValid(mon) != 0) {
            exribbonSetEarthRibbon(mon);
        }
    }
    return 0;
}
#pragma pop
#endif

#if !defined(PR424_FIELD_PREFIX_SPLIT)
u32 fn_801CADA0(void)
{
    return 0;
}
#endif

#if !defined(PR424_FIELD_PREFIX_SPLIT) || \
    defined(PR424_FIELD_801CADA8_801CAE80)
#pragma push
#pragma peephole off
void* fn_801CADA8(u8 kind)
{
    void* result = NULL;
    void* status = savedataGetStatus(0, 2);

    switch (kind) {
        case 1:
            result = heroPokemonGetBlacky(status, 0xfe);
            break;
        case 2:
            result = heroPokemonGetEifie(status, 0xfe);
            break;
        case 3:
            result = heroPokemonGetPrasle(status, 0xfe);
            break;
        case 4:
            result = heroPokemonGetHouou(status, 0xff);
            break;
        case 5:
            result = heroPokemonGetPikachu(status, 0xff);
            break;
        case 6:
            result = heroPokemonGetCelebi(status, 0xff);
            break;
    }
    return result;
}
#pragma pop
#endif

#if !defined(PR424_FIELD_PREFIX_SPLIT)
void scriptSetPokemonNakigoe(void) {
    extern void etctoolSetPokemonNakigoe(void);
    etctoolSetPokemonNakigoe();
}
#endif
#endif

#if defined(FIELD_EXACT_801CAEA0)
u32 scriptGetPokemonNickName(s32 slot)
{
    void* mon;

    if (slot >= 6) {
        return 0;
    }
    mon = heroBiosGetPokemonPtr(savedataGetStatus(0, 2), (u16)slot);
    if (mon == NULL) {
        return 0;
    }
    return pokemonGetStatus(mon, 0, 0x77, 0);
}
#endif

#if defined(FIELD_801CAF0C_SUFFIX_ACTIVE)
u32 scriptGetDarkPointZeroPokemonNum(void)
{
    u16 count = 0;
    void* status = savedataGetStatus(0, 2);
    u16 i;
    f32 zero = lbl_8047E114;

    for (i = 0; i < 6; i++) {
        void* mon = heroBiosGetPokemonPtr(status, i);
        if (pokemonCheckValid(mon) != 0 && pokemonBiosGetDarkFlag(mon) != 0 && pokemonGetDp(mon) == zero) {
            count++;
        }
    }
    return count;
}

u32 scriptGetDarkPokemonNum(void)
{
    void* status = savedataGetStatus(0, 2);
    u16 count = 0;
    u16 i;

    for (i = 0; i < 6; i++) {
        void* mon = heroBiosGetPokemonPtr(status, i);
        if (pokemonCheckValid(mon) != 0 && pokemonBiosGetDarkFlag(mon) != 0) {
            count++;
        }
    }
    return count;
}

u32 scriptGetPokemonNum(void)
{
    void* status = savedataGetStatus(0, 2);
    u16 count = 0;
    u16 i;

    for (i = 0; i < 6; i++) {
        void* mon = heroBiosGetPokemonPtr(status, i);
        if (pokemonCheckValid(mon) != 0) {
            count++;
        }
    }
    return count;
}

#pragma push
#pragma peephole off
u32 floorCharacterSetPos(u32 id, f32 x, f32 y, f32 z)
{
    f32 pos[3];
    void* bios;

    set__5GSvecFfff(pos, x, y, z);
    bios = fn_8011711C(id);
    floorCharacterBiosSetPos(bios, pos);
    return 1;
}
#pragma pop

#pragma push
#pragma peephole off
s32 scriptFloorCharSetDisp(u32 a0, u32 a1) {
    extern u8* fn_8011711C();
    extern void floorCharacterBiosSetVisibility();

    floorCharacterBiosSetVisibility((u8*)fn_8011711C(a0), (u8)a1);
    return 1;
}
#pragma pop
#endif
