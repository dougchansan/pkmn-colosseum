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
    extern void GSmemFree(u16 handle);
    u8* entry = *(u8**)lbl_80467CC0;
    u16 count = *(u16*)(lbl_80467CC0 + 4);
    u16 handle = *(u16*)(lbl_80467CC0 + 0x10);
    s32 i;

    for (i = 0; i < count; i++, entry += 0x8C) {
        if (entry != NULL) {
            if (entry[0x74] != 0) {
                fn_801DD3E4(entry);
                wazaSequenceOwnerRelease(entry);
            }
            memset(entry, 0, 0x8C);
        }
    }
    fn_800E24B0(handle);
    GSmemFree(handle);
    memset(lbl_80467CC0, 0, 0x14);
}

/**
 * fn_801DADC0 - Waza system partial reset.
 * Address: 0x801DADC0 | Size: 0x138
 */
void fn_801DADC0(void* context) {
    typedef struct WazaPoolHeader {
        void* entries;
        u16 count;
        u8 active;
        u8 pad07;
        void* resourceA;
        void* resourceB;
        u16 handle;
    } WazaPoolHeader;
    typedef struct WazaSystemState {
        void* receivers[16];
        WazaPoolHeader pool;
        u8 scratch[0x20];
    } WazaSystemState;
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetGroupID(void*);
    extern u32 floorDataBiosGetShadowReciveNum(void*);
    extern void* floorDataBiosGetShadowReciveID(void*, u32);
    extern void* floorDataBiosGetShadowLightID(void*);
    extern void* GSresGetResource(u32, u32);
    extern void wazaSequenceSysClearWork(void);
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* GSmemLock(u16);
    u8* base = lbl_80467C80;
    WazaSystemState* state = (WazaSystemState*)lbl_80467C80;
    u32 groupId;
    void* floor;
    void* resource;
    s32 receiverCount;
    u16 handle;
    s32 count = (s32)context;
    s32 i;
    void** receivers;

    memset(&state->pool, 0, sizeof(state->pool));
    memset(state->scratch, 0, sizeof(state->scratch));

    floor = floorDataBiosGetCurrentPtr();
    groupId = floorDataBiosGetGroupID(floor);
    receiverCount = floorDataBiosGetShadowReciveNum(floor);
    lbl_8047B414 = 0;
    receivers = (void**)base;
    for (i = 0; i < receiverCount; i++) {
        resource = GSresGetResource(
            groupId, (u32)floorDataBiosGetShadowReciveID(floor, i));
        if (resource != NULL) {
            receivers[lbl_8047B414++] = resource;
        }
    }

    resource = floorDataBiosGetShadowLightID(floor);
    if (resource != NULL) {
        lbl_8047B418 = (s32)GSresGetResource(groupId, (u32)resource);
    }

    if (count != 0) {
        wazaSequenceSysClearWork();
        receiverCount = count * 0x8C;
        handle = _toolentryAlloc__FUl(receiverCount);
        if (handle != 0) {
            WazaPoolHeader* pool = &state->pool;

            pool->handle = handle;
            pool->count = count;
            pool->entries = GSmemLock(handle);
            memset(pool->entries, 0, receiverCount);
            battleCameraWazaInit();
            pool->resourceA = NULL;
            pool->resourceB = NULL;
            pool->active = 0;
        }
    }
}

/**
 * wazaSequenceSysInit - Waza system initialization.
 * Address: 0x801DAEF8 | Size: 0x168
 * Referenced by battle_main.c (battle_FightStart).
 * Allocates waza context and prepares the system for move animations.
 */
void wazaSequenceSysInit(s32 count) {
    extern u8 lbl_80467CD4[];
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetGroupID(void*);
    extern u32 floorDataBiosGetShadowReciveNum(void*);
    extern void* floorDataBiosGetShadowReciveID(void*, u32);
    extern void* floorDataBiosGetShadowLightID(void*);
    extern void* GSresGetResource(u32, u32);
    extern void wazaSequenceSysClearWork(void);
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* GSmemLock(u16);
    void* floor;
    void* resource;
    u32 groupId;
    u32 receiverCount;
    u32 size;
    u16 handle;
    s32 i;

    memset(lbl_80467CC0, 0, 0x14);
    memset(lbl_80467CD4, 0, 0x20);

    floor = floorDataBiosGetCurrentPtr();
    groupId = floorDataBiosGetGroupID(floor);
    receiverCount = floorDataBiosGetShadowReciveNum(floor);
    lbl_8047B414 = 0;
    for (i = 0; i < receiverCount; i++) {
        resource = GSresGetResource(
            groupId, (u32)floorDataBiosGetShadowReciveID(floor, i));
        if (resource != NULL) {
            ((void**)lbl_80467C80)[lbl_8047B414++] = resource;
        }
    }

    resource = floorDataBiosGetShadowLightID(floor);
    if (resource != NULL) {
        lbl_8047B418 = (s32)GSresGetResource(groupId, (u32)resource);
    }

    if (count != 0) {
        wazaSequenceSysClearWork();
        size = count * 0x8C;
        handle = _toolentryAlloc__FUl(size);
        if (handle != 0) {
            *(u16*)(lbl_80467CC0 + 0x10) = handle;
            *(u16*)(lbl_80467CC0 + 4) = count;
            *(void**)lbl_80467CC0 = GSmemLock(handle);
            memset(*(void**)lbl_80467CC0, 0, size);
            battleCameraWazaInit();
            fn_801DE598(0x6F7, 0);
            *(void**)(lbl_80467CC0 + 8) =
                GSresGetResource(0x6F7, 0x11EF2400);
            *(void**)(lbl_80467CC0 + 0xC) =
                GSresGetResource(0x6F7, 0x11EE2400);
            lbl_80467CC0[6] = 0;
        }
    }
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
 * wazaSequenceSysUpdate - Waza system reset.
 * Address: 0x801DB088 | Size: 0x78
 * Referenced by battle_main.c (battle_FightCleanup).
 */
void wazaSequenceSysUpdate(void)
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
            wazaSequenceOwnerUpdateBlink(entry);
        }
    }

    {
        fn_801D2D28();
    }
}

/**
 * wazaSequenceOwnerFree - Waza system get context.
 * Address: 0x801DB100 | Size: 0x54
 */
void wazaSequenceOwnerFree(void* obj) {
    if (obj != NULL) {
        if (*(u8*)((u8*)obj + 0x74) != 0) {
            fn_801DD3E4(obj);
            wazaSequenceOwnerRelease(obj);
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
            GSresReleaseById(id);
        }

        *(u16*)((u8*)obj + 0x72) = 0;
    }
}

#endif

#if defined(WAZA_SEQUENCE_SYS_TAIL_801DB288_801DB848)

#if !defined(WAZA_SEQUENCE_SYS_TIME_ONLY)
/**
 * wazaSequenceSysFreeWazaResource - Waza sequence data parse.
 * Address: 0x801DB288 | Size: 0x170
 */
void wazaSequenceSysFreeWazaResource(void* seqData) {
    extern u8 lbl_80467CD4[];
    u16 resourceId = *(u16*)((u8*)seqData + 0x30);
    s32 references;
    s32 i;

    if (resourceId != 0) {
        u8* owner = *(u8**)lbl_80467CC0;
        u16 ownerCount = *(u16*)(lbl_80467CC0 + 4);
        references = 0;

        for (i = 0; i < ownerCount; i++, owner += 0x8C) {
            u8* entry = *(u8**)(owner + 0x68);
            while (entry != NULL) {
                if (*(u16*)(entry + 0x30) == resourceId) {
                    references++;
                }
                entry = *(u8**)(entry + 0x34);
            }
        }
        for (i = 0; i < 16; i++) {
            if (*(u16*)(lbl_80467CD4 + i * 2) == resourceId) {
                references++;
            }
        }
        if (references == 1) {
            while (fn_8017B2CC(resourceId) == 1) {
                _threadSwitch();
            }
            fn_800F915C(resourceId);
            GSresReleaseById(resourceId);
        }
        *(u16*)((u8*)seqData + 0x30) = 0;
    }
}

#endif

#if !defined(WAZA_SEQUENCE_SYS_FREE_ONLY)
/**
 * wazaSequenceSysGetWazaTime - Waza sequence data complex parse.
 * Address: 0x801DB3F8 | Size: 0x450
 */
s32 wazaSequenceSysGetWazaTime(void* owner, void* sequence, s32 timeType) {
    WazaSequence* waza = sequence;
    s32 pointName = waza->kind;
    s32 time = waza->field_10;
    s32 offset;
    u8* timing;

    owner = *(void**)((u8*)owner + 0x2C);
    timing = (u8*)owner + (pointName * 0xD4);

    switch (waza->animationMode) {
    case 1:
        switch (pointName) {
        case 0:
        case 0xB:
        case 0x10:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 5:
                GSlogWrite("wazaSequenceSysGetWazaTime: requesting damage time for attack waza type!\n");
            case 3:
            case 4:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 1:
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x10);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        case 8:
        case 9:
        case 10:
            GSlogWrite("wazaSequenceSysGetWazaTime: DATA ERROR - attack waza triggers damage animation!\n");
            return 0;
        default:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 5:
                GSlogWrite("wazaSequenceSysGetWazaTime: requesting damage time for attack waza type!\n");
            case 3:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 4:
                offset = *(s32*)(timing + 0x10);
                break;
            case 1:
                offset = *(s32*)(timing + 0x14);
                break;
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x18);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        }
        break;
    case 2:
        switch (pointName) {
        case 0:
        case 0xB:
        case 0x10:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 3:
            case 4:
                GSlogWrite("wazaSequenceSysGetWazaTime: requesting attack time for damage waza type!\n");
            case 5:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 1:
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x10);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        case 8:
        case 9:
        case 10:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 3:
            case 4:
                GSlogWrite("wazaSequenceSysGetWazaTime: requesting attack time for damage waza type!\n");
            case 5:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 1:
                offset = *(s32*)(timing + 0x10);
                break;
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x14);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        default:
            GSlogWrite("wazaSequenceSysGetWazaTime: DATA ERROR - attack waza triggers damage animation!\n");
            return 0;
        }
        break;
    case 3:
    case 4:
        switch (pointName) {
        case 0:
        case 0xB:
        case 0x10:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 3:
            case 4:
            case 5:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 1:
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x10);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        case 8:
        case 9:
        case 10:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 3:
            case 4:
            case 5:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 1:
                offset = *(s32*)(timing + 0x10);
                break;
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x14);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        default:
            switch ((u8)timeType) {
            case 7:
                return 0;
            case 0:
                offset = 0;
                break;
            case 3:
            case 5:
                offset = *(s32*)(timing + 0x0C);
                break;
            case 4:
                offset = *(s32*)(timing + 0x10);
                break;
            case 1:
                offset = *(s32*)(timing + 0x14);
                break;
            case 2:
            case 6:
                offset = *(s32*)(timing + 0x18);
                break;
            default:
                GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for pointName!\n");
                return 0;
            }
            break;
        }
        break;
    default:
        GSlogWrite("wazaSequenceSysGetWazaTime: invalid value for waza_type!\n");
        return 0;
    }

    return time + offset;
}

#endif

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
            wazaSequenceOwnerPlayIdle(effect, 1, 0);
        }
    }
}

/**
 * wazaSequenceOwnerDrawAttached - Draw matching models from a sequence entry.
 * Address: 0x801DB8FC | Size: 0x8C
 */
void wazaSequenceOwnerDrawAttached(void* entry, u32 drawFlags, u8 modelID) {
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
