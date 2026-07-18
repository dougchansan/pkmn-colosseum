/**
 * @file battle_range_801ED640.c
 * @brief battle-domain (direct calls into battle_*.c), 0x801ED640 - 0x801EF02C.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) — mixed-block split pass, 2026-07-01.
 * Small accessors/helpers are decompiled below; larger render/runtime helpers
 * remain to be matched.
 */
#include "dolphin/types.h"

#pragma use_lmw_stmw on

typedef struct BattleRangeVec {
    f32 x;
    f32 y;
    f32 z;
} BattleRangeVec;

typedef struct BattleRangeDef {
    u8 type;
    u8 pad01;
    u16 field02;
    u16 field04;
    u16 field06;
    u16 field08;
    u16 runtimeSlot;
    u16 flag0C;
    u16 flag0E;
    u16 pad10;
    u16 flag12;
    u16 flag14;
    u16 flag16;
    u16 pad18;
    u16 pad1A;
    u16 flag1C;
    u16 actionListId;
    u16 pad20;
    u16 pad22;
    u16 indexedFlags[10];
} BattleRangeDef;

typedef struct BattleRangeIndexedEntry {
    u8 type;
    u8 variant;
    u8 pad02[2];
    u32 value;
} BattleRangeIndexedEntry;

typedef struct BattleRangeIndexedHeader {
    u32 count;
} BattleRangeIndexedHeader;

typedef struct BattleRangeActionEntry {
    u8 value;
    u8 variant;
    u16 flag;
    u32 result;
} BattleRangeActionEntry;

typedef struct BattleRangeTextureEntry {
    u16 id;
    u16 pad02;
    void* data;
} BattleRangeTextureEntry;

typedef struct BattleRangeResource {
    u32 magic;
    u16 textureCount;
    u16 textureOffset;
    u8 pad08[0x14];
    u8 data[];
} BattleRangeResource;

extern u8 lbl_8047B5C0;
extern u8 lbl_8047B5C1;
extern BattleRangeResource* lbl_8047B5C4;
extern u8* lbl_8047B5C8;

extern BattleRangeVec lbl_80375230;
extern u16 lbl_80375240[];
extern void* lbl_8046D630[];
extern BattleRangeActionEntry* lbl_80478F64;
extern BattleRangeDef* lbl_80478F6C;
extern BattleRangeIndexedHeader* lbl_80478F78;
extern BattleRangeIndexedEntry* lbl_80478F7C;

extern void GSvecCopy(void* dst, void* src);
extern void* GStextureLoad(void* data);
extern u32 GSgappCreate(s32 state, u8 priority, u32 param, void* func);
extern u8 fn_801902E0(u16 flag);
extern u8 fn_801906A0(u16 flag);
extern void _flagSet(u16 flag, s32 value);

void fn_801ED780(void);

#if defined(BATTLE_RANGE_PREFIX)

void fn_801ED640(u8 value)
{
    lbl_8047B5C1 = value;
}

void fn_801ED648(BattleRangeVec* value)
{
    GSvecCopy(&lbl_80375230, value);
}

void fn_801ED674(void)
{
    lbl_8047B5C0 = 0;
}

void fn_801ED680(BattleRangeResource* resource)
{
    s32 i;
    BattleRangeTextureEntry* entry;
    void* texture;
    u8* data;

    lbl_8047B5C4 = resource;
    if (resource->magic == 0x7B1EE3F0) {
        data = resource->data;
        lbl_8047B5C8 = data;
        entry = (BattleRangeTextureEntry*)(data + resource->textureOffset * 12);

        for (i = 0; i < lbl_8047B5C4->textureCount; i++) {
            entry->data = (void*)((u32)entry->data + (u32)resource);
            texture = GStextureLoad(entry->data);
            if (texture == NULL) {
                break;
            }
            lbl_8046D630[entry->id] = texture;
            entry++;
        }
    }
    lbl_8047B5C0 = 1;
}

void fn_801ED740(void)
{
    lbl_8047B5C0 = 0;
    lbl_8047B5C1 = 0;
    GSgappCreate(1, 0xF0, 0xA, fn_801ED780);
}

#endif

#if defined(BATTLE_RANGE_EXACT_801EE034)

u32 fn_801EE034(BattleRangeIndexedEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return entry->value;
}

u8 fn_801EE04C(BattleRangeIndexedEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return entry->variant;
}

u8 fn_801EE064(BattleRangeIndexedEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return entry->type;
}

BattleRangeIndexedEntry* fn_801EE07C(u32 index)
{
    u32 id = index & 0xFFFF;
    BattleRangeIndexedHeader* header = lbl_80478F78;

    if (id >= header->count) {
        return lbl_80478F7C;
    }
    return &lbl_80478F7C[id];
}

#pragma peephole off
u16 fn_801EE0A8(u32 idx)
{
    extern u16* lbl_80478F74;
    u8 i;

    i = idx;
    return lbl_80478F74[i];
}
#pragma peephole on

s32 fn_801EE0BC(u16 id)
{
    extern void* fn_801EF1E4(void* data);
    void* data = fn_801EF1E4(NULL);

    if (data != NULL) {
        return *(s16*)((u8*)data + id * 0x18 + 0x4A4);
    }
    return -1;
}

#pragma push
#pragma optimize_for_size on
void fn_801EE10C(u16 id, u8 value)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        *(u8*)(data + slot * 0x18 + 0x49C) = value;
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
u8 fn_801EE174(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        return *(u8*)(data + slot * 0x18 + 0x49C);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
void fn_801EE1E0(u16 id, u16 value)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        *(u16*)(data + slot * 0x18 + 0x4A0) = value;
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
u16 fn_801EE248(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        return *(u16*)(data + slot * 0x18 + 0x4A0);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
void fn_801EE2B4(u16 id, u8* value)
{
    extern u8* fn_801EF1E4(u32 data);
    extern u8* GScharCpy(u8* dst, u8* src);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        GScharCpy(data + slot * 0x18 + 0x490, value);
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
u8* fn_801EE328(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        return data + slot * 0x18 + 0x490;
    }
    return NULL;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
u8 fn_801EE398(void)
{
    u16 i;

    for (i = 0; i < 0x30; i++) {
        u16 id = lbl_80375240[i];
        BattleRangeDef* def = &lbl_80478F6C[id];
        u8 value;

        if (id == 0 || id > 0x60) {
            def = NULL;
        }
        if (def != NULL) {
            if (def->flag14 != 0) {
                value = fn_801902E0(def->flag14);
            } else {
                value = 0;
            }
        } else {
            value = 0;
        }
        if (value == 0) {
            return 0;
        }
    }
    return 1;
}
#pragma pop

u16 fn_801EE440(u16 index)
{
    if (index > 0x30) {
        return 0;
    }
    return lbl_80375240[index];
}

u16 fn_801EE468(void)
{
    return 0x30;
}

#pragma push
#pragma optimize_for_size on
u16 fn_801EE470(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        return *(u16*)(data + slot * 0xC + 6);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
void fn_801EE4DC(u16 id, u16 value)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = fn_801EF1E4(0);
        *(u16*)(data + slot * 0xC + 6) = value;
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
u32 fn_801EE544(u16 id, u8* variant)
{
    BattleRangeDef* def = &lbl_80478F6C[id];
    u32 result = 0;
    BattleRangeActionEntry* entry;
    u8 flagValue;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        entry = &lbl_80478F64[def->actionListId];
        while (1) {
            if (entry->result == 0) {
                break;
            }
            if (entry->flag != 0) {
                flagValue = fn_801906A0(entry->flag);
                if (flagValue == entry->value) {
                    result = entry->result;
                    if (variant != NULL) {
                        *variant = entry->variant;
                    }
                }
            } else {
                result = entry->result;
                if (variant != NULL) {
                    *variant = entry->variant;
                }
            }
            entry++;
        }
        return result;
    }
    return 0;
}
#pragma pop

s32 fn_801EE614(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag1C != 0) {
            return (s8)fn_801906A0(def->flag1C);
        }
        return 0;
    }
    return 0;
}

void fn_801EE67C(u32 id, u32 value)
{
    extern u8* fn_801EF1E4(u32 data);
    BattleRangeDef* def;
    u16 index;
    u32 defOffset;
    BattleRangeDef* flagDef;
    u8* data;
    u32 next;
    s16* slot;

    index = id;
    defOffset = index * sizeof(BattleRangeDef);
    def = (BattleRangeDef*)((u8*)lbl_80478F6C + defOffset);

    if (index == 0 || index > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        data = fn_801EF1E4(0);
        if (data != NULL) {
            slot = (s16*)(data + index * 0x18 + 0x4A4);
            if (*slot < 0) {
                next = *(u32*)data;
                *(u32*)data = next + 1;
                *slot = next;
            }
        }

        flagDef = (BattleRangeDef*)((u8*)lbl_80478F6C + defOffset);
        if ((u16)id == 0 || (u16)id > 0x60) {
            flagDef = NULL;
        }
        if (flagDef != NULL && flagDef->flag0C != 0) {
            _flagSet(flagDef->flag0C, 1);
        }
        if (def->flag1C != 0) {
            _flagSet(def->flag1C, (u16)value);
        }
    }
}

#pragma push
#pragma optimize_for_size on
u32 fn_801EE750(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    BattleRangeDef* def = &lbl_80478F6C[id];
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        slot = def->runtimeSlot;
        data = fn_801EF1E4(0);
        return *(u32*)(data + slot * 0xC + 8);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma optimize_for_size on
void fn_801EE7BC(u16 id, u32 value)
{
    extern u8* fn_801EF1E4(u32 data);
    BattleRangeDef* def = &lbl_80478F6C[id];
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        slot = def->runtimeSlot;
        data = fn_801EF1E4(0);
        *(u32*)(data + slot * 0xC + 8) = value;
    }
}
#pragma pop

s32 fn_801EE824(u16 id, u16 index)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        u16 flag = def->indexedFlags[index];

        if (flag != 0) {
            return (s8)fn_801906A0(flag);
        }
        return 0;
    }
    return 0;
}

void fn_801EE894(u16 id, u16 index, s8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        u16 flag = def->indexedFlags[index];

        if (flag != 0) {
            _flagSet(flag, value);
        }
    }
}

u8 fn_801EE8F4(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag16 != 0) {
            return fn_801902E0(def->flag16);
        }
        return 0;
    }
    return 0;
}

#endif

#if defined(BATTLE_RANGE_CANDIDATE_801EE958)

u8 fn_801EEAD0(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag12 != 0) {
            return fn_801902E0(def->flag12);
        }
        return 0;
    }
    return 0;
}

u8 fn_801EEC74(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag14 != 0) {
            return fn_801902E0(def->flag14);
        }
        return 0;
    }
    return 0;
}

void fn_801EECD8(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag14 != 0) {
        _flagSet(def->flag14, value);
    }
}

void fn_801EED30(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag0E != 0) {
        _flagSet(def->flag0E, value);
    }
}

u8 fn_801EED88(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag0C != 0) {
            return fn_801902E0(def->flag0C);
        }
        return 0;
    }
    return 0;
}

void fn_801EEDEC(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag0C != 0) {
        _flagSet(def->flag0C, value);
    }
}

u8 fn_801EEE44(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    return def->type;
}

#pragma push
#pragma optimize_for_size on
void fn_801EEE6C(u16 id, u16 value)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* data = fn_801EF1E4(0);

    if (data != NULL) {
        *(u16*)(data + id * 0x18 + 0x4A6) = value;
    }
}
#pragma pop

#pragma push
#pragma optimize_for_size on
u16 fn_801EEEB8(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    u8* data = fn_801EF1E4(0);

    if (data != NULL) {
        return *(u16*)(data + id * 0x18 + 0x4A6);
    }
    return 0;
}
#pragma pop

u16 fn_801EEF08(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        return def->field08;
    }
    return 0;
}

#pragma push
#pragma optimize_for_size on
u32 fn_801EEF40(u16 id)
{
    extern u8* fn_801EF1E4(u32 data);
    BattleRangeDef* def = &lbl_80478F6C[id];
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        slot = def->runtimeSlot;
        data = fn_801EF1E4(0);
        return *(u32*)(data + slot * 0xC + 0xC);
    }
    return 0;
}
#pragma pop

u16 fn_801EEFAC(u16 id, s32 side)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (side == 0) {
            return def->field04;
        }
        return def->field06;
    }
    return 0;
}

u16 fn_801EEFF4(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        return def->field02;
    }
    return 0;
}

#endif
