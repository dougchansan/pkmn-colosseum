/**
 * @file pcbox.c
 * @brief Decompiled functions.
 *
 * Address range: 0x8013433C - 0x80135030
 *
 * Split out of the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114); see effect_util_types.h for
 * shared cross-TU declarations.
 */

#include "dolphin/types.h"
#include "game/effect/effect_util_types.h"


#ifndef PCBOX_SETPOKEMON_ONLY

/* 0x8013433C | 0xE4 */
#if 0
asm void pcboxSwapItemSlot(void) {
#include "src/game/effect/effect_util_pcboxSwapItemSlot.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
s32 pcboxSwapItemSlot(void* base, s16 idx1, s16 idx2) {
    extern void fn_80140A9C(void*, void*);
    void* b;
    void* b2;
    void* entry1;
    void* entry2;
    s16 i;
    b = base;
    if (b == 0) {
        b = (void*)savedataGetStatus(0, 3);
    }
    i = idx1;
    if (i < 0 || i >= 0xeb) {
        entry1 = 0;
    } else {
        entry1 = (u8*)b + 0x6dec + (s32)i * 4;
    }
    if (entry1 == 0) return 0;
    b2 = base;
    if (base == 0) {
        b2 = (void*)savedataGetStatus(0, 3);
    }
    i = idx2;
    if (i < 0 || i >= 0xeb) {
        entry2 = 0;
    } else {
        entry2 = (u8*)b2 + 0x6dec + (s32)i * 4;
    }
    if (entry2 == 0) return 0;
    fn_80140A9C(entry1, entry2);
    return 1;
}
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
#endif


static inline void* pcboxGetSaveStatus(void* base) {
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    return base;
}

static inline s16 pcboxFindItem(void* status, u16 effect_id) {
    extern u8 fn_801429E8(void*);
    extern s32 itemGetStatus(void*, u32, u32, u32);
    s32 i;
    for (i = 0; i < 0xeb; i++) {
        if (fn_801429E8((u8*)status + 0x6dec + i * 4)) {
            if (itemGetStatus((u8*)status + 0x6dec + i * 4, 0, 0x1b, 0) == effect_id) break;
        }
    }
    if (i < 0xeb) return i;
    return -1;
}

/* 0x80134420 | 0x164 */
#if 0
asm void pcboxGetItemCapacity(void) {
#include "src/game/effect/effect_util_pcboxGetItemCapacity.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
static inline void* pcboxGetItemSlot(void* status, s16 idx) {
    void* entry;
    if (idx < 0 || idx >= 0xeb) {
        entry = 0;
    } else {
        entry = (u8*)status + 0x6dec + (s32)idx * 4;
    }
    return entry;
}

static inline u16 pcboxGetItemSlotNum(void* entry) {
    extern u8 fn_801429E8(void*);
    extern s32 itemGetStatus(void*, u32, u32, u32);
    if (entry == 0) {
        return 0xFFFF;
    }
    if (!fn_801429E8(entry)) {
        return 0xFFFF;
    }
    return itemGetStatus(entry, 0, 0x1c, 0);
}

static inline u16 pcboxGetItemNum(void* status, u16 effect_id) {
    s16 idx;
    u16 num;
    idx = pcboxFindItem(status, effect_id);
    if (idx >= 0) {
        num = pcboxGetItemSlotNum(pcboxGetItemSlot(status, idx));
        if (num > 0x3e7) {
            num = 0;
        }
    } else {
        num = 0;
    }
    return num;
}

/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
u16 pcboxGetItemCapacity(void* base, u16 effect_id) {
    extern void* itemDataBiosGetPtr(u16);
    void* status;
    status = pcboxGetSaveStatus(base);
    if (!itemDataBiosGetPtr(effect_id)) return 0;
    return (u16)(0x3e7 - pcboxGetItemNum(status, effect_id));
}
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
#endif


/* 0x80134584 | 0xF8 */
#if 0
asm void pcboxAddItem(void) {
#include "src/game/effect/effect_util_pcboxAddItem.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
u16 pcboxAddItem(void* base, u16 effect_id, u16 r5) {
    extern void* itemDataBiosGetPtr(u16);
    extern u8 fn_801429E8(void*);
    extern s32 itemGetStatus(void*, u32, u32, u32);
    extern u16 fn_80140ACC(void*, u16, u16, u16, s16, u16, u32);
    void* status; void* cur; s16 idx; s32 i;
    status = pcboxGetSaveStatus(base);
    if (r5 == 0) return r5;
    if (!itemDataBiosGetPtr(effect_id)) return r5;
    idx = pcboxFindItem(status, effect_id);
    if (idx < 0) return r5;
    return (u16)fn_80140ACC((u8*)status + 0x6dec, 0xeb, effect_id, r5, idx, 0x3e7, 0);
}
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
#endif


/* 0x8013467C | 0xEC */
#if 0
asm void pcboxDelItem(void) {
#include "src/game/effect/effect_util_pcboxDelItem.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
u16 pcboxDelItem(void* base, u16 effect_id, u16 r5) {
    extern void* itemDataBiosGetPtr(u16);
    extern u8 fn_801429E8(void*);
    extern s32 itemGetStatus(void*, u32, u32, u32);
    extern u16 fn_80141308(void*, u16, u16, u16, s16, u16, u32, u32);
    void* status; void* cur; s16 idx; s32 i;
    status = pcboxGetSaveStatus(base);
    if (r5 == 0) return r5;
    if (!itemDataBiosGetPtr(effect_id)) return r5;
    idx = pcboxFindItem(status, effect_id);
    return (u16)fn_80141308((u8*)status + 0x6dec, 0xeb, effect_id, r5, idx, 0x3e7, 0, 0);
}
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
#endif

#if 0
asm void pcboxGetItem(void) {
#include "src/game/effect/effect_util_pcboxGetItem.inc"
}
#else
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
/* RULE-EXCEPTION(user-approved): local compiler-control pragma — see docs/RULE_EXCEPTIONS.md */
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
void* pcboxGetItem(void* base, s16 index) {
    void* status;
    void* entry;
    status = base;
    if (status == 0) {
        status = (void*)savedataGetStatus(0, 3);
    }
    if (index < 0 || index >= 0xeb) {
        entry = 0;
    } else {
        entry = (u8*)status + 0x6dec + (s32)index * 4;
    }
    return entry;
}
#if !defined(NO_LOCAL_PRAGMAS_PCBOX_CANDIDATE_8013433C)
#endif
#endif


#endif /* PCBOX_SETPOKEMON_ONLY */

#ifndef PCBOX_ITEMS_ONLY

#ifndef PCBOX_SETPOKEMON_ONLY

/* 0x801347D0 | 0x8 | return_const */
u16 pcboxGetNbItemSlot(s32 box) { return 235; }


/* 0x801347D8 | 0x8 | return_const */
s8 fn_801347D8(void) { return 30; }


/* 0x801347E0 | 0x8 | return_const */
s8 pcboxGetNbPokemonBox(void) { return 3; }


#endif /* PCBOX_SETPOKEMON_ONLY */

/* One box contains its name/header followed by 30 Pokemon records. */
typedef struct PCBoxPokemonRecord {
    u8 data[0x138];
} PCBoxPokemonRecord;

typedef struct PCBoxData {
    u8 header[0x14];
    PCBoxPokemonRecord pokemon[30];
} PCBoxData;

static inline u8* pcboxGetPokemonRecord(PCBoxData* base, s8 box, s8 index)
{
    if (box < 0 || box >= 3) {
        return 0;
    }
    if (index < 0 || index >= 30) {
        return 0;
    }
    return base[box].pokemon[index].data;
}

#ifndef PCBOX_SETPOKEMON_ONLY

/* 0x801347E8 | 0x104 */
#if 0
asm void fn_801347E8(void) {
#include "src/game/effect/effect_util_fn_801347E8.inc"
}
#else
s8 pcboxGetPokemonBoxNbEmptySlot(PCBoxData* base, s8 box) {
    s8 index;
    s8 count = 0;
    u8* pokemon;
    u8* record;

    if (base == 0) {
        base = (PCBoxData*)savedataGetStatus(0, 3);
    }
    if (box < 0 || box >= 3) {
        count = -1;
    } else {
        index = 0;
        pokemon = (u8*)base + (s32)box * sizeof(PCBoxData);
        while (index < 30) {
            if (box < 0 || box >= 3) {
                record = 0;
            } else if (index < 0 || index >= 30) {
                record = 0;
            } else {
                record = pokemon + 0x14;
            }
            if (record != 0 && pokemonCheckValid(record)) {
                count++;
            }
            pokemon += sizeof(PCBoxPokemonRecord);
            index++;
        }
    }

    if (count < 0) {
        return -1;
    }
    return (s8)(30 - count);
}
#endif


/* 0x801348EC | 0xF0 */
#if 0
asm void getPokemonBoxNbUsedSlot__5PCBOXFSc(void) {
#include "src/game/effect/effect_util_getPokemonBoxNbUsedSlot__5PCBOXFSc.inc"
}
#else
s32 getPokemonBoxNbUsedSlot__5PCBOXFSc(PCBoxData* base, s8 box) {
    u8* pokemon;
    s32 count = 0;
    s8 index;

    if (base == 0) {
        base = (PCBoxData*)savedataGetStatus(0, 3);
    }
    if (box < 0 || box >= 3) {
        return -1;
    }

    for (index = 0; index < 30; index++) {
        pokemon = pcboxGetPokemonRecord(base, box, index);
        if (pokemon != 0 && pokemonCheckValid(pokemon)) {
            count++;
        }
    }
    return count;
}
#endif


/* 0x801349DC | 0xBC */
#if 0
asm void fn_801349DC(void) {
#include "src/game/effect/effect_util_fn_801349DC.inc"
}
#else
s32 pcboxSetPokemonBoxName(void* base, s8 slot, u16* name) {
    extern void GScharCpy(void*, u16*);
    s32 len;
    u16* p;
    s8 s;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    s = slot;
    if (s < 0 || s >= 3) return 0;
    if (name == 0) return 0;
    p = name;
    len = 0;
    while (*p != 0) {
        p++;
        len++;
    }
    if (len > 8) return 0;
    GScharCpy((u8*)base + (s32)s * 0x24a4, name);
    return 1;
}
#endif

#if 0
asm void pcboxGetPokemonBoxName(void) {
#include "src/game/effect/effect_util_fn_80134A98.inc"
}
#else
void* pcboxGetPokemonBoxName(void* base, s8 index) {
    s8 i;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    i = index;
    if (i < 0) goto _ret0;
    i = index;
    if (i < 3) goto _compute;
_ret0:
    return 0;
_compute:
    return (u8*)base + (s32)i * 0x24a4;
}
#endif


/* 0x80134AF8 | 0xC8 */
#if 0
asm void delPokemon__5PCBOXFScSc(void) {
#include "src/game/effect/effect_util_delPokemon__5PCBOXFScSc.inc"
}
#else
s32 delPokemon__5PCBOXFScSc(void* base, s8 slot, s8 idx) {
    extern u8 pokemonCheckValid(void*);
    extern void pokemonInit(void*);
    u8* entry;
    s8 s;
    s8 e;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    s = slot;
    if (s < 0 || s >= 3) {
        entry = 0;
    } else {
        e = idx;
        if (e < 0 || e >= 0x1e) {
            entry = 0;
        } else {
            entry = (u8*)base + (s32)s * 0x24a4 + (s32)e * 0x138 + 0x14;
        }
    }
    if (entry == 0) return 0;
    if (!pokemonCheckValid(entry)) return 0;
    pokemonInit(entry);
    return 1;
}
#endif


/* 0x80134BC0 | 0x250 */
#if 0
asm void pcboxAddPokemon(void) {
#include "src/game/effect/effect_util_fn_80134BC0.inc"
}
#else
s32 pcboxAddPokemon(void* base, void* src, s8 slot) {
    extern u8 pokemonCheckValid(void*);
    extern void pokemonAllKaihuku(void*);
    u8* slotbase;
    u8* entry;
    s8 found_slot;
    s8 found_entry;
    s8 si;
    s8 ei;
    u32 i;
    u32* dst32;
    u32* src32;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    if (slot < -1 || slot >= 3) return 0;
    if (src == 0) return 0;
    found_slot = slot;
    found_entry = -1;
    if (slot == -1) {
        /* Search all slots for first free entry */
        slotbase = (u8*)base;
        for (si = 0; si < 3; si++) {
            for (ei = 0; ei < 0x1e; ei++) {
                entry = (ei >= 0 && ei < 0x1e) ? (slotbase + (s32)ei * 0x138 + 0x14) : 0;
                if (entry != 0 && !pokemonCheckValid(entry)) {
                    found_entry = ei;
                    break;
                }
            }
            if (found_entry >= 0) {
                found_slot = si;
                break;
            }
            slotbase += 0x24a4;
        }
        if (si >= 3) return 0;
    } else {
        /* Search specific slot for first free entry */
        slotbase = (u8*)base + (s32)slot * 0x24a4;
        for (ei = 0; ei < 0x1e; ei++) {
            entry = (ei >= 0 && ei < 0x1e) ? (slotbase + (s32)ei * 0x138 + 0x14) : 0;
            if (entry != 0 && !pokemonCheckValid(entry)) {
                found_entry = ei;
                break;
            }
        }
        if (found_entry < 0) return 0;
    }
    /* Compute entry address */
    if (found_slot < 0 || found_slot >= 3 || found_entry < 0 || found_entry >= 0x1e) {
        entry = 0;
    } else {
        entry = (u8*)base + (s32)found_slot * 0x24a4 + (s32)found_entry * 0x138 + 0x14;
    }
    if (entry == 0) return 0;
    dst32 = (u32*)entry;
    src32 = (u32*)src;
    for (i = 0; i < 0x27; i++) {
        dst32[0] = src32[0];
        dst32[1] = src32[1];
        dst32 += 2;
        src32 += 2;
    }
    pokemonAllKaihuku(entry);
    return 1;
}
#endif


#endif /* PCBOX_SETPOKEMON_ONLY */

/* 0x80134E10 | 0xE0 */
#if 0
asm void setPokemon__5PCBOXFP7PokemonScSc(void) {
#include "src/game/effect/effect_util_setPokemon__5PCBOXFP7PokemonScSc.inc"
}
#else
s32 setPokemon__5PCBOXFP7PokemonScSc(PCBoxData* base, PCBoxPokemonRecord* src, s8 box, s8 index) {
    extern void pokemonAllKaihuku(void*);
    PCBoxPokemonRecord* entry;
    if (base == 0) {
        base = (PCBoxData*)savedataGetStatus(0, 3);
    }
    if (box < 0 || box >= 3) {
        entry = 0;
    } else if (index < 0 || index >= 30) {
        entry = 0;
    } else {
        entry = &base[box].pokemon[index];
    }
    if (entry == 0) {
        return 0;
    }
    *entry = *src;
    pokemonAllKaihuku(entry);
    return 1;
}
#endif


#ifndef PCBOX_SETPOKEMON_ONLY

/* 0x80134EF0 | 0x98 */
#if 0
asm void getPokemon__5PCBOXFScSc(void) {
#include "src/game/effect/effect_util_fn_80134EF0.inc"
}
#else
void* getPokemon__5PCBOXFScSc(void* base, s8 r4, s8 r5) {
    u8* new_var;
    s8 slot;
    s8 entry;
    if (base == 0) {
        base = (void*)savedataGetStatus(0, 3);
    }
    slot = r4;
    if (slot < 0 || slot >= 3) {
        new_var = 0;
        goto done;
    }
    entry = r5;
    if (entry < 0 || entry >= 0x1e) {
        new_var = 0;
        goto done;
    }
    new_var = (u8*)base + (s32)slot * 0x24a4 +
              (s32)entry * 0x138 + 0x14;
done:
    return new_var;
}
#endif


/* 0x80134F88 | 0x9C */
#if 0
asm void pcboxInit(void) {
#include "src/game/effect/effect_util_pcboxInit.inc"
}
#else
void pcboxInit(void* base) {
    extern void msgctrlSetValue(u32, u32);
    extern void GSmsgGetText(void*, u32, u32);
    extern void pokemonInitAry(void*, u32);
    extern void fn_80142A88(void*, u32);
    u8* status;
    s32 i;
    u8* cur;

    status = base;
    if (base == 0) {
        status = (u8*)savedataGetStatus(0, 3);
    }
    i = 0;
    cur = status;
    do {
        msgctrlSetValue(0x34, i + 1);
        GSmsgGetText(cur, 9, 0x32c9);
        pokemonInitAry(cur + 0x14, 0x1e);
        i++;
        cur += 0x24a4;
    } while (i < 3);
    fn_80142A88(status + 0x6dec, 0xeb);
}
#endif


/* 0x80135024 | 0x4 | void_stub */
#if 0
asm void pcboxSetStatus(void) {
#include "src/game/effect/effect_util_fn_80135024.inc"
}
#else
void pcboxSetStatus(void) {
}
#endif


/* 0x80135028 | 0x8 | return_const */
u32 pcboxGetStatus() { return 0; }

#endif /* PCBOX_SETPOKEMON_ONLY */

#endif /* PCBOX_ITEMS_ONLY */
