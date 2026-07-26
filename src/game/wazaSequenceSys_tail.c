/**
 * @file wazaSequenceSys_tail.c
 * @brief Candidate waza sequence system lifecycle and resource helpers.
 *
 * Address range: 0x801DAC90 - 0x801DB988
 */

#include "game/battle/battle_waza_types.h"

#if defined(WAZA_SEQUENCE_SYS_TAIL_801DAC90_801DB060)

/* =========================================================================
 * WAZA SYSTEM LIFECYCLE (0x801DAC90 - 0x801DB100)
 *
 * System-level init/cleanup/reset functions.
 * Referenced by battle_main.c and battle_logic.c.
 * ========================================================================= */

/**
 * wazaSequenceSysRelease - Waza system cleanup.
 * Address: 0x801DAC90 | Size: 0x130
 * Referenced by battle_main.c (battle_FightEnd).
 * Stops all active waza effects, frees all allocated memory.
 */
void wazaSequenceSysRelease(void) {
    extern void fn_800E24B0(u16 handle);
    extern void fn_800E209C(u16 handle);
    u8* entry = *(u8**)lbl_80467CC0;
    u16 count = *(u16*)(lbl_80467CC0 + 4);
    u16 handle = *(u16*)(lbl_80467CC0 + 0x10);
    s32 i;

    for (i = 0; i < count; i++, entry += 0x8C) {
        if (entry != NULL) {
            if (entry[0x74] != 0) {
                fn_801DD3E4(entry);
                fn_801DD23C(entry);
            }
            memset(entry, 0, 0x8C);
        }
    }
    fn_800E24B0(handle);
    fn_800E209C(handle);
    memset(lbl_80467CC0, 0, 0x14);
}

/**
 * fn_801DADC0 - Waza system partial reset.
 * Address: 0x801DADC0 | Size: 0x138
 */
void fn_801DADC0(void* context) {
    memset(lbl_80467C80 + 0x40, 0, 0x14);
    memset(lbl_80467C80 + 0x54, 0, 0x20);
}

/**
 * fn_801DAEF8 - Waza system initialization.
 * Address: 0x801DAEF8 | Size: 0x168
 * Referenced by battle_main.c (battle_FightStart).
 * Allocates waza context and prepares the system for move animations.
 */
void fn_801DAEF8(s32 count) {
    /* TODO: Waza system init (0x168 bytes)
     * 1. Allocates waza context structure
     * 2. Initializes effect pool with 'count' entries
     * 3. Clears all sequence data
     * 4. Resets frame counters
     */
}

#endif

#if defined(WAZA_SEQUENCE_SYS_TAIL_801DB060_801DB288)

/**
 * wazaSequenceSysGetResID - Allocate the next waza resource ID.
 * Address: 0x801DB060 | Size: 0x28
 */
int wazaSequenceSysGetResID(void) {
    if ((u32)(lbl_8047B410 + 0x10000) == 0xFFFF) {
        lbl_8047B410 = 0;
    }

    lbl_8047B410++;
    return lbl_8047B410;
}

/**
 * fn_801DB088 - Waza system reset.
 * Address: 0x801DB088 | Size: 0x78
 * Referenced by battle_main.c (battle_FightCleanup).
 */
void fn_801DB088(void)
{
    u8* pool = lbl_80467CC0;
    WazaEffect* entry;
    s32 i = 0;
    int count;

    entry = *((WazaEffect**) pool);
    count = *((u16*) (pool + 4));
    for (; i < count; i++, entry++) {
        if (entry->active != 0) {
            fn_801DD158(entry);
            fn_801DF1D0(entry);
        }
    }

    {
        fn_801D2D28();
    }
}

/**
 * fn_801DB100 - Waza system get context.
 * Address: 0x801DB100 | Size: 0x54
 */
void fn_801DB100(void* obj) {
    if (obj != NULL) {
        if (*(u8*)((u8*)obj + 0x74) != 0) {
            fn_801DD3E4(obj);
            fn_801DD23C(obj);
        }

        memset(obj, 0, 0x8C);
    }
}

/* =========================================================================
 * WAZA EXTENDED FUNCTIONS (0x801DB154 - 0x801E03D4)
 *
 * Extended waza/scene animation functions including the remaining
 * move effect handlers, transition effects, and rendering helpers.
 * ========================================================================= */

/**
 * fn_801DB154 - Waza sequence data lookup.
 * Address: 0x801DB154 | Size: 0x78
 */
void* fn_801DB154(void) {
    u8* pool = lbl_80467CC0;
    u8* entry = *(u8**)pool;
    u16 count = *(u16*)(pool + 4);
    s32 i;

    for (i = 0; i < count; i++, entry += 0x8C) {
        if (*(u8*)(entry + 0x74) == 0) {
            memset(entry, 0, 0x8C);
            *(u8*)(entry + 0x74) = 1;
            return entry;
        }
    }

    return NULL;
}

/**
 * wazaSequenceSysFreeSequenceResource - Waza sequence data validate.
 * Address: 0x801DB1CC | Size: 0xBC
 */
void wazaSequenceSysFreeSequenceResource(void* obj) {
    u16 id = *(u16*)((u8*)obj + 0x72);

    if (id != 0) {
        u8* pool = lbl_80467CC0;
        u8* entry = *(u8**)pool;
        u16 count = *(u16*)(pool + 4);
        s32 matches = 0;
        s32 i;

        for (i = 0; i < count; i++, entry += 0x8C) {
            if (*(u16*)(entry + 0x72) == id) {
                matches++;
            }
        }

        if (matches == 1) {
            while (fn_8017B2CC(id) == 1) {
                _threadSwitch();
            }

            fn_800F915C(id);
            fn_8017B1CC(id);
        }

        *(u16*)((u8*)obj + 0x72) = 0;
    }
}

#endif

#if defined(WAZA_SEQUENCE_SYS_TAIL_801DB288_801DB848)

/**
 * wazaSequenceSysFreeWazaResource - Waza sequence data parse.
 * Address: 0x801DB288 | Size: 0x170
 */
void wazaSequenceSysFreeWazaResource(void* seqData, s32 moveID) {
    /* TODO: Parse sequence data (0x170 bytes) */
}

/**
 * wazaSequenceSysGetWazaTime - Waza sequence data complex parse.
 * Address: 0x801DB3F8 | Size: 0x450
 */
s32 wazaSequenceSysGetWazaTime(void* owner, void* sequence, s32 timeType) {
    /* TODO: Complex sequence data parse (0x450 bytes) */
    return 0;
}

#endif

#if defined(WAZA_SEQUENCE_SYS_TAIL_801DB848_801DB988)

/**
 * wazaSequenceSysGetModelShadowLight__Fv - Waza data get move count.
 * Address: 0x801DB848 | Size: 0x8
 */
extern s32 lbl_8047B418;
s32 wazaSequenceSysGetModelShadowLight__Fv(void) {
    return lbl_8047B418;
}

/**
 * wazaSequenceSysGetModelShadowCount__Fv - Waza data get entry count for move.
 * Address: 0x801DB850 | Size: 0x8
 */
extern s32 lbl_8047B414;
s32 wazaSequenceSysGetModelShadowCount__Fv(void) {
    return lbl_8047B414;
}

/**
 * wazaSequenceSysGetModelShadowList__Fv - Waza data get move flags.
 * Address: 0x801DB858 | Size: 0xC
 */
extern u8 lbl_80467C80[];
void* wazaSequenceSysGetModelShadowList__Fv(void) {
    return lbl_80467C80;
}

/**
 * wazaSequenceSysResetAnimationExcept - Reset every active animation except one.
 * Address: 0x801DB864 | Size: 0x98
 */
void wazaSequenceSysResetAnimationExcept(void* except) {
    u8* pool = lbl_80467CC0;
    WazaEffect* effect;
    s32 i = 0;
    int count;

    effect = *(WazaEffect**)pool;
    count = *(u16*)(pool + 4);
    for (; i < count; i++, effect++) {
        if (except != effect && effect->active != 0 && effect->model != NULL) {
            fn_801DEF0C(effect, 1, 0);
        }
    }
}

/**
 * fn_801DB8FC - Draw matching models from a sequence entry.
 * Address: 0x801DB8FC | Size: 0x8C
 */
void fn_801DB8FC(void* entry, u32 drawFlags, u8 modelID) {
    u8* node;

    node = *(u8**)((u8*)entry + 0x24);
    while (node != NULL) {
        if (*(s32*)(node + 0x6C) == 1) {
            switch (*(s32*)(node + 4)) {
            case 2:
                if (modelID ==
                    (u8)fn_800E3CBC(
                        *(struct GSmodel**)(node + 0xA4))) {
                    GSmodelDrawModel(
                        *(struct GSmodel**)(node + 0xA4), drawFlags);
                }
                break;
            }
        }
        node = *(u8**)(node + 0xA8);
    }
}

#endif
