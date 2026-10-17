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
extern u8 std081_flagCheckTrue(u16 flag);
extern u8 std082_flagGetValue(u16 flag);
extern void _flagSet(u16 flag, s32 value);

#if defined(BATTLE_RANGE_EXACT_801EE034)

u32 darkPokemonEntryGet4(BattleRangeIndexedEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return entry->value;
}

u8 darkPokemonEntryGet1(BattleRangeIndexedEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return entry->variant;
}

u8 darkPokemonEntryGet0(BattleRangeIndexedEntry* entry)
{
    if (entry == NULL) {
        return 0;
    }
    return entry->type;
}

BattleRangeIndexedEntry* darkPokemonGetTableEntry(u32 index)
{
    u32 id = index & 0xFFFF;
    BattleRangeIndexedHeader* header = lbl_80478F78;

    if (id >= header->count) {
        return lbl_80478F7C;
    }
    return &lbl_80478F7C[id];
}

u16 darkPokemonGetIdByIndex(u32 idx)
{
    extern u16* lbl_80478F74;
    u8 i;

    i = idx;
    return lbl_80478F74[i];
}

#endif

#if defined(BATTLE_RANGE_EXACT_801EE0BC)

s32 fn_801EE0BC(u16 id)
{
    extern void* darkPokemonSaveGetPtr(void* data);
    void* data = darkPokemonSaveGetPtr(NULL);

    if (data != NULL) {
        return *(s16*)((u8*)data + id * 0x18 + 0x4A4);
    }
    return -1;
}

void fn_801EE10C(u16 id, u8 value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        *(u8*)(data + slot * 0x18 + 0x49C) = value;
    }
}

u8 darkPokemonSaveGet49C(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        return *(u8*)(data + slot * 0x18 + 0x49C);
    }
    return 0;
}

void darkPokemonSaveSet4A0(u16 id, u16 value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        *(u16*)(data + slot * 0x18 + 0x4A0) = value;
    }
}

u16 darkPokemonSaveGet4A0(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        return *(u16*)(data + slot * 0x18 + 0x4A0);
    }
    return 0;
}

void darkPokemonSaveSetName(u16 id, u8* value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    extern u8* GScharCpy(u8* dst, u8* src);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        GScharCpy(data + slot * 0x18 + 0x490, value);
    }
}

u8* darkPokemonSaveGetName(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        return data + slot * 0x18 + 0x490;
    }
    return NULL;
}

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
                value = std081_flagCheckTrue(def->flag14);
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

u16 fn_801EE440(u16 index)
{
    if (index > 0x30) {
        return 0;
    }
    return lbl_80375240[index];
}

u16 darkPokemonListCount(void)
{
    return 0x30;
}

u16 fn_801EE470(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        return *(u16*)(data + slot * 0xC + 6);
    }
    return 0;
}

void darkPokemonSaveSetDp(u16 id, u16 value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* entry = (u8*)lbl_80478F6C + id * 0x38;
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        entry = NULL;
    }
    if (entry != NULL) {
        slot = *(u16*)(entry + 0xA);
        data = darkPokemonSaveGetPtr(0);
        *(u16*)(data + slot * 0xC + 6) = value;
    }
}

u32 darkPokemonGetFlagListValue(u16 id, u8* variant)
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
                flagValue = std082_flagGetValue(entry->flag);
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

s32 darkPokemonGetFlag1C(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag1C != 0) {
            return (s8)std082_flagGetValue(def->flag1C);
        }
        return 0;
    }
    return 0;
}

void std243_shadowPokemonSetState(u32 id, u32 value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
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
        data = darkPokemonSaveGetPtr(0);
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

u32 darkPokemonSaveGet8(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    BattleRangeDef* def = &lbl_80478F6C[id];
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        slot = def->runtimeSlot;
        data = darkPokemonSaveGetPtr(0);
        return *(u32*)(data + slot * 0xC + 8);
    }
    return 0;
}

void darkPokemonSaveSet8(u16 id, u32 value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    BattleRangeDef* def = &lbl_80478F6C[id];
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        slot = def->runtimeSlot;
        data = darkPokemonSaveGetPtr(0);
        *(u32*)(data + slot * 0xC + 8) = value;
    }
}

s32 fn_801EE824(u16 id, u16 index)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        u16 flag = def->indexedFlags[index];

        if (flag != 0) {
            return (s8)std082_flagGetValue(flag);
        }
        return 0;
    }
    return 0;
}

void darkPokemonSetFlag24(u16 id, u16 index, s8 value)
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

u8 darkPokemonCheckFlag16(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag16 != 0) {
            return std081_flagCheckTrue(def->flag16);
        }
        return 0;
    }
    return 0;
}

#endif

#if defined(BATTLE_RANGE_CANDIDATE_801EE958)

static inline BattleRangeDef* battleRangeGetDefInline(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    return def;
}

/* darkPokemonCheckFlag16 */
static inline u8 battleRangeCheckFlag16Inline(u16 id)
{
    BattleRangeDef* def = battleRangeGetDefInline(id);

    if (def != NULL) {
        if (def->flag16 != 0) {
            return std081_flagCheckTrue(def->flag16);
        }
        return 0;
    }
    return 0;
}

static inline void battleRangeSetFlag0CInline(u16 id)
{
    BattleRangeDef* def = battleRangeGetDefInline(id);

    if (def != NULL) {
        if (def->flag0C != 0) {
            _flagSet(def->flag0C, 1);
        }
    }
}

static inline void battleRangeSetRuntimeStatusInline(u16 id, u32 status)
{
    extern u8* darkPokemonSaveGetPtr(void* data);
    BattleRangeDef* def = battleRangeGetDefInline(id);

    if (def != NULL) {
        u16 slot = def->runtimeSlot;
        u8* data = darkPokemonSaveGetPtr(NULL);

        *(u32*)(data + slot * 0xC + 0xC) = status;
    }
}

#if !defined(BATTLE_RANGE_ONLY_801EEB34)
void fn_801EE958(u32 id, u8 value)
{
    extern u8* darkPokemonSaveGetPtr(void* data);
    extern u32 fightFloorGetStatus(s32, s32, s32, s32);
    BattleRangeDef* def;
    u8* data;
    u32 next;
    s16* activeIndex;
    u16 key;
    u16 flag;
    BattleRangeDef* tmp;
    u32 status;
    u16 slot;

    key = id;
    tmp = &lbl_80478F6C[(u16)id];
    if ((u16)id == 0 || (u16)id > 0x60) {
        def = NULL;
    } else {
        def = tmp;
    }
    if (def == NULL) {
        return;
    }
    if (battleRangeCheckFlag16Inline(id) == 0) {
        data = darkPokemonSaveGetPtr(NULL);
        if (data != NULL) {
            activeIndex = (s16*)(data + key * 0x18 + 0x4A4);
            if (*activeIndex < 0) {
                next = *(u32*)data;
                *(u32*)data = next + 1;
                *activeIndex = next;
            }
        }
        battleRangeSetFlag0CInline(id);
    }
    flag = def->flag16;
    status = fightFloorGetStatus(0, 0, 0x4A, 0);
    tmp = battleRangeGetDefInline(id);
    if (tmp != NULL) {
        slot = tmp->runtimeSlot;
        data = darkPokemonSaveGetPtr(NULL);
        *(u32*)(data + slot * 0xC + 0xC) = status;
    }
    if (flag != 0) {
        _flagSet(flag, value);
    }
}

#endif

#if !defined(BATTLE_RANGE_ONLY_801EE958)
void fn_801EEB34(u32 id, u8 value)
{
    extern u8* darkPokemonSaveGetPtr(void* data);
    extern u32 fightFloorGetStatus(s32, s32, s32, s32);
    /* RULE-EXCEPTION(user-approved): the two def pointers held in one two-element array, chosen for register allocation (permuter farm) — see docs/RULE_EXCEPTIONS.md */
    BattleRangeDef* defs[2]; /* [0] def, [1] lookup */
    u8* data;
    u32 status;
    u16 slot;

    defs[1] = &lbl_80478F6C[(u16)id];
    if ((u16)id == 0 || (u16)id > 0x60) {
        defs[0] = NULL;
    } else {
        defs[0] = defs[1];
    }
    if (defs[0] == NULL) {
        return;
    }
    if (battleRangeCheckFlag16Inline(id) == 0) {
        battleRangeSetFlag0CInline(id);
    }
    status = fightFloorGetStatus(0, 0, 0x4A, 0);
    defs[1] = battleRangeGetDefInline(id);
    if (defs[1] != NULL) {
        slot = defs[1]->runtimeSlot;
        data = darkPokemonSaveGetPtr(NULL);
        *(u32*)(data + slot * 0xC + 0xC) = status;
    }
    if (defs[0]->flag12 != 0) {
        _flagSet(defs[0]->flag12, value);
    }
}

#endif

#if !defined(BATTLE_RANGE_ONLY_801EE958) && !defined(BATTLE_RANGE_ONLY_801EEB34)
u8 darkPokemonCheckFlag12(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag12 != 0) {
            return std081_flagCheckTrue(def->flag12);
        }
        return 0;
    }
    return 0;
}

u8 darkPokemonCheckFlag14(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag14 != 0) {
            return std081_flagCheckTrue(def->flag14);
        }
        return 0;
    }
    return 0;
}

void darkPokemonSetFlag14(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag14 != 0) {
        _flagSet(def->flag14, value);
    }
}

void darkPokemonSetFlag0E(u16 id, u8 value)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL && def->flag0E != 0) {
        _flagSet(def->flag0E, value);
    }
}

u8 darkPokemonCheckFlag0C(u16 id)
{
    BattleRangeDef* def = &lbl_80478F6C[id];

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        if (def->flag0C != 0) {
            return std081_flagCheckTrue(def->flag0C);
        }
        return 0;
    }
    return 0;
}

void darkPokemonSetFlag0C(u16 id, u8 value)
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

void darkPokemonSaveSet4A6(u16 id, u16 value)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* data = darkPokemonSaveGetPtr(0);

    if (data != NULL) {
        *(u16*)(data + id * 0x18 + 0x4A6) = value;
    }
}

u16 darkPokemonSaveGet4A6(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    u8* data = darkPokemonSaveGetPtr(0);

    if (data != NULL) {
        return *(u16*)(data + id * 0x18 + 0x4A6);
    }
    return 0;
}

u16 darkPokemonGet8(u16 id)
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

u32 darkPokemonSaveGetC(u16 id)
{
    extern u8* darkPokemonSaveGetPtr(u32 data);
    BattleRangeDef* def = &lbl_80478F6C[id];
    u16 slot;
    u8* data;

    if (id == 0 || id > 0x60) {
        def = NULL;
    }
    if (def != NULL) {
        slot = def->runtimeSlot;
        data = darkPokemonSaveGetPtr(0);
        return *(u32*)(data + slot * 0xC + 0xC);
    }
    return 0;
}

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

u16 darkPokemonGet2(u16 id)
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
#endif
