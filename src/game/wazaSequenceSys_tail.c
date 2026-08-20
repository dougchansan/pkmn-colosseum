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
    extern u8 lbl_80467CD4[];
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorDataBiosGetGroupID(void*);
    extern u32 floorDataBiosGetShadowReciveNum(void*);
    extern void* floorDataBiosGetShadowReciveID(void*, u32);
    extern void* floorDataBiosGetShadowLightID(void*);
    extern void* GSresGetResource(u32, u32);
    extern void fn_801019F8(void);
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* fn_800E27B0(u16);
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
        fn_801019F8();
        size = count * 0x8C;
        handle = _toolentryAlloc__FUl(size);
        if (handle != 0) {
            *(u16*)(lbl_80467CC0 + 0x10) = handle;
            *(u16*)(lbl_80467CC0 + 4) = count;
            *(void**)lbl_80467CC0 = fn_800E27B0(handle);
            memset(*(void**)lbl_80467CC0, 0, size);
            fn_801D301C();
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
void wazaSequenceSysFreeWazaResource(void* seqData, ...) {
    extern u8 lbl_80467CD4[];
    u16 resourceId = *(u16*)((u8*)seqData + 0x30);
    s32 references = 0;
    s32 i;

    if (resourceId != 0) {
        u8* owner = *(u8**)lbl_80467CC0;
        u16 ownerCount = *(u16*)(lbl_80467CC0 + 4);

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
            fn_8017B1CC(resourceId);
        }
        *(u16*)((u8*)seqData + 0x30) = 0;
    }
}

/**
 * wazaSequenceSysGetWazaTime - Waza sequence data complex parse.
 * Address: 0x801DB3F8 | Size: 0x450
 */
s32 wazaSequenceSysGetWazaTime(void* owner, void* sequence, s32 timeType) {
    extern const char lbl_80279788[];
    WazaSequence* waza = sequence;
    const char* logBase = lbl_80279788;
    s32 pointName = waza->kind;
    s32 time = waza->field_10;
    u8 type = timeType;
    u8* timing = (u8*)*(void**)((u8*)owner + 0x2C) + (pointName * 0xD4);

    switch (waza->animationMode) {
    case 1:
        switch (pointName) {
        case 0:
        case 0xB:
        case 0x10:
            switch (type) {
            case 0:
                return time;
            case 3:
            case 4:
                return time + *(s32*)(timing + 0x0C);
            case 5:
                GSlogWrite(&logBase[0]);
                return time + *(s32*)(timing + 0x0C);
            case 1:
            case 2:
            case 6:
                return time + *(s32*)(timing + 0x10);
            case 7:
                return 0;
            default:
                GSlogWrite(&logBase[0x4C]);
                return 0;
            }
        default:
            if (pointName >= 8 && pointName < 0xB) {
                GSlogWrite(&logBase[0x88]);
                return 0;
            }
            switch (type) {
            case 0:
                return time;
            case 1:
                GSlogWrite(&logBase[0]);
                return time + *(s32*)(timing + 0x0C);
            case 2:
                return time + *(s32*)(timing + 0x10);
            case 3:
            case 4:
                return time + *(s32*)(timing + 0x14);
            case 5:
                return time + *(s32*)(timing + 0x18);
            case 6:
                return time + *(s32*)(timing + 0x10);
            case 7:
                return 0;
            default:
                GSlogWrite(&logBase[0x4C]);
                return 0;
            }
        }
    case 2:
        switch (pointName) {
        case 0:
        case 0xB:
        case 0x10:
            switch (type) {
            case 0:
                return time;
            case 3:
            case 4:
                GSlogWrite(&logBase[0xDC]);
                return time + *(s32*)(timing + 0x0C);
            case 5:
                return time + *(s32*)(timing + 0x0C);
            case 1:
            case 2:
            case 6:
                return time + *(s32*)(timing + 0x10);
            case 7:
                return 0;
            default:
                GSlogWrite(&logBase[0x4C]);
                return 0;
            }
        default:
            if (pointName >= 8 && pointName < 0xB) {
                GSlogWrite(&logBase[0x88]);
                return 0;
            }
            switch (type) {
            case 0:
                return time;
            case 1:
            case 5:
                GSlogWrite(&logBase[0xDC]);
                return time + *(s32*)(timing + 0x0C);
            case 2:
                return time + *(s32*)(timing + 0x10);
            case 3:
            case 4:
            case 6:
                return time + *(s32*)(timing + 0x14);
            case 7:
                return 0;
            default:
                GSlogWrite(&logBase[0x4C]);
                return 0;
            }
        }
    case 3:
    case 4:
        switch (pointName) {
        case 0:
        case 0xB:
        case 0x10:
            switch (type) {
            case 0:
                return time;
            case 3:
            case 4:
            case 5:
                return time + *(s32*)(timing + 0x0C);
            case 1:
            case 2:
            case 6:
                return time + *(s32*)(timing + 0x10);
            case 7:
                return 0;
            default:
                GSlogWrite(&logBase[0x4C]);
                return 0;
            }
        default:
            if (pointName >= 8 && pointName < 0xB) {
                switch (type) {
                case 0:
                    return time;
                case 3:
                case 4:
                case 5:
                    return time + *(s32*)(timing + 0x0C);
                case 1:
                    return time + *(s32*)(timing + 0x10);
                case 2:
                case 6:
                    return time + *(s32*)(timing + 0x14);
                case 7:
                    return 0;
                default:
                    GSlogWrite(&logBase[0x4C]);
                    return 0;
                }
            }
            switch (type) {
            case 0:
                return time;
            case 1:
                return time + *(s32*)(timing + 0x0C);
            case 2:
                return time + *(s32*)(timing + 0x10);
            case 3:
                return time + *(s32*)(timing + 0x14);
            case 4:
                return time + *(s32*)(timing + 0x18);
            case 5:
                return time + *(s32*)(timing + 0x14);
            case 6:
                return time + *(s32*)(timing + 0x10);
            case 7:
                return 0;
            default:
                GSlogWrite(&logBase[0x4C]);
                return 0;
            }
        }
    default:
        GSlogWrite(&logBase[0x128]);
        return 0;
    }
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
