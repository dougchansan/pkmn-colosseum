/**
 * @file sequence.c
 * @brief sequence / modelSequence lineage: Colosseum-era precursor of XD's
 * ModelSequence + NullSequence classes.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"

#if defined(SEQUENCE_801DCDA8_801DCF00)

/**
 * fn_801DCDA8 - Waza field effect get type.
 * Address: 0x801DCDA8 | Size: 0x24
 */
void* fn_801DCDA8(void* obj, s32 fieldEffect) {
    void* cur = *(void**)((u8*)obj + 0x24);

    while (cur != NULL) {
        if (*(s32*)cur == fieldEffect) {
            return cur;
        }
        cur = *(void**)((u8*)cur + 0xA8);
    }

    return cur;
}

/**
 * fn_801DCDCC - Waza field effect set type.
 * Address: 0x801DCDCC | Size: 0x40
 */
s32 fn_801DCDCC(void* obj) {
    if (obj == NULL) {
        return 0;
    }

    if (*(u8*)((u8*)obj + 0x77) == 0) {
        return 0;
    }

    if (*(u8*)((u8*)obj + 0x4E) != 0) {
        return *(u8*)((u8*)obj + 0x4F);
    }

    return 0;
}

/**
 * fn_801DCE0C - Waza field effect render.
 * Address: 0x801DCE0C | Size: 0x9C
 */
void fn_801DCE0C(void* obj) {
    extern void GSmodelEnableColorSwap();
    extern void GSmodelEnableModulation();

    s32 handle;

    if (obj != NULL && *(u8*)((u8*)obj + 0x4F) == 0 && *(u8*)((u8*)obj + 0x4E) != 0) {
        handle = *(s32*)((u8*)obj + 0x24);
        if (*(u8*)((u8*)obj + 0x4C) != 0) {
            GSmodelEnableColorSwap(handle, *(s32*)((u8*)obj + 0x38), *(s32*)((u8*)obj + 0x3C),
                        *(s32*)((u8*)obj + 0x40), *(s32*)((u8*)obj + 0x44));
        }
        if (*(u8*)((u8*)obj + 0x4D) != 0) {
            *(u8*)((u8*)obj + 0x4B) = 0xFF;
            GSmodelEnableModulation(handle, (u8*)obj + 0x48);
        }
        *(u8*)((u8*)obj + 0x4F) = 1;
    }
}

/**
 * fn_801DCEA8 - Waza field effect clear.
 * Address: 0x801DCEA8 | Size: 0x58
 */
extern void fn_801DEF0C(void* obj, s32 arg1, s32 arg2);
void fn_801DCEA8(void* obj) {
    WazaEffect* effect = obj;
    u8 flags = effect->flags;

    if ((flags & 2) == 2) {
        effect->flags = flags ^ 2;
        GSmodelRemoveNull(effect->model);
        fn_801DEF0C(effect, 1, 0);
    }
}

#endif

#if defined(SEQUENCE_801DCF00_801DCF84)

/**
 * fn_801DCF00 - Waza lighting override set.
 * Address: 0x801DCF00 | Size: 0x84
 */
void fn_801DCF00(u32 color, f32 intensity) {
    extern u8 GSmodelIsRootNullAdded(s32);
    extern void GSmodelGetRootPosition(s32, void*);
    extern void GSmodelAddNull(s32, void*, s32, s32);
    extern void fn_801DEF0C(void*, s32, s32);

    void* obj;
    u8 flags;

    obj = (void*)color;
    flags = *(u8*)((u8*)obj + 0x18);
    if ((flags & 2) != 2) {
        *(u8*)((u8*)obj + 0x18) = flags | 2;
        if (GSmodelIsRootNullAdded(*(s32*)((u8*)obj + 0x24)) != 0) {
            GSmodelGetRootPosition(*(s32*)((u8*)obj + 0x24), (u8*)obj + 0x5C);
        } else {
            GSmodelAddNull(*(s32*)((u8*)obj + 0x24), (u8*)obj + 0x5C, 0, 0);
        }
        fn_801DEF0C(obj, 1, 0);
    }
}

#endif

#if defined(SEQUENCE_801DCF84_801DD158)

/**
 * fn_801DCF84 - Waza lighting override clear.
 * Address: 0x801DCF84 | Size: 0x54
 */
void fn_801DCF84(void* obj) {
    u8 flags = *(u8*)((u8*)obj + 0x18);

    if ((flags & 8) == 8) {
        *(u8*)((u8*)obj + 0x18) = flags ^ 8;
        fn_801DEF0C(obj, 1, 1);
        fn_801DA014(obj);
    }
}

/**
 * fn_801DCFD8 - Waza lighting override get active.
 * Address: 0x801DCFD8 | Size: 0x50
 */
void fn_801DCFD8(void* obj) {
    WazaEffect* effect = obj;
    u8 flags = effect->flags;

    if ((flags & 8) != 8) {
        effect->flags = flags | 8;
        GSmodelStopAnimation(effect->model);
        fn_801DA070(effect);
    }
}

/**
 * fn_801DD028 - Waza lighting ambient set.
 * Address: 0x801DD028 | Size: 0x50
 */
extern void fn_801DF33C(void* obj);
void fn_801DD028(void* obj) {
    u8 flags = *(u8*)((u8*)obj + 0x18);

    if ((flags & 4) == 4) {
        fn_801DF33C(obj);
        *(u8*)((u8*)obj + 0x18) = *(u8*)((u8*)obj + 0x18) ^ 4;
        fn_801D9E34(obj);
    }
}

/**
 * fn_801DD078 - Waza lighting ambient get.
 * Address: 0x801DD078 | Size: 0x50
 */
extern void fn_801DF3D4(void* obj);
void fn_801DD078(void* obj) {
    u8 flags = *(u8*)((u8*)obj + 0x18);

    if ((flags & 4) != 4) {
        fn_801DF3D4(obj);
        *(u8*)((u8*)obj + 0x18) = *(u8*)((u8*)obj + 0x18) | 4;
        fn_801D9E8C(obj);
    }
}


/**
 * GetWaza__12NullSequenceCFUsUs - Waza lighting reset.
 * Address: 0x801DD0C8 | Size: 0x38
 */
void* GetWaza__12NullSequenceCFUsUs(void* obj, s32 search_key1, s32 search_key2)
{
    WazaFxNode* cur = ((WazaFxOwner*)obj)->first_child;

    while (cur != NULL) {
        if (cur->field_2C == (u16)search_key1 && cur->field_2E == (u16)search_key2) {
            return cur;
        }
        cur = cur->next;
    }

    return cur;
}

/**
 * fn_801DD100 - 0x801DD100 | Size: 0x58
 * Two-arg (owner, obj) per the caller in wazaSequence.c; the prior
 * (u32 filterColor) signature was a placeholder.
 */
void fn_801DD100(WazaSequenceOwner* owner, WazaSequence* sequence) {
    if (owner == NULL) return;
    if (sequence == NULL) {
        owner->index = 0;
        owner->field_34 = 0;
        owner->table[0].field_90 = 0;
    } else {
        WazaEffectTblEntry* tblEntry;
        owner->index = (u8)sequence->kind;
        owner->field_34 = 0;
        tblEntry = &owner->table[sequence->kind];
        tblEntry->field_90 = sequence->field_10;
    }
}

#endif

#if defined(SEQUENCE_801DD158_801DD23C)

/**
 * fn_801DD158 - Waza color filter update.
 * Address: 0x801DD158 | Size: 0xE4
 */
void fn_801DD158(void* obj) {
    WazaEffect* effect = obj;
    WazaSequence* sequence;

    if (effect->model != NULL) {
        if ((effect->flags & 1) == 1) {
            GSmodelSetVisibility(effect->model, 1);
        } else {
            GSmodelSetVisibility(effect->model, 0);
        }
    }

    sequence = effect->sequenceList;
    while (sequence != NULL) {
        WazaSequence* next = *(WazaSequence**)((u8*)sequence + 0x34);

        if (sequence->active != 0) {
            s8 result;

            if ((s8)sequence->stopping == -1) {
                wazaSequenceApplyStop(sequence);
            } else {
                result = wazaSequenceUpdate(sequence);
                if (result == 0) {
                    if ((s8)sequence->stopping != -1) {
                        wazaSequenceApplyStop(sequence);
                    }
                } else if (result < 0) {
                    wazaSequenceApplyStop(sequence);
                    wazaSequenceFree(sequence);
                }
            }
        }
        sequence = next;
    }
}

#endif

#if defined(SEQUENCE_801DD23C_801DD45C)

/**
 * fn_801DD23C - Waza color filter transition.
 * Address: 0x801DD23C | Size: 0x1A8
 */
void fn_801DD23C(void* obj) {
    extern void fn_800E24B0(u16);
    extern void fn_800E209C(u16);
    extern void GSmodelDisableColorSwap(u32);
    extern void GSmodelDisableModulation(u32);
    extern void GSmodelSetAnimEndedCallback();
    extern void fn_801DA4E8();
    extern void fn_801193BC(s32);
    extern void fn_800F9210();
    extern void Unload__13ModelSequenceFPUc(void*);
    extern void wazaSequenceSysFreeSequenceResource(void* obj);
    extern void fn_801D9E34(void* obj);
    extern void fn_801DA014(void* obj);

    u8* data;
    u16 id;
    u8 enabled;
    u32 handle;

    data = (u8*)obj;
    if (data != NULL) {
        id = *(u16*)(data + 0x30);
        if (id != 0) {
            fn_800E24B0(id);
            fn_800E209C(id);
        }

        if (data == NULL) {
            enabled = 0;
        } else if (*(u8*)(data + 0x77) == 0) {
            enabled = 0;
        } else if (*(u8*)(data + 0x4E) == 0) {
            enabled = 0;
        } else {
            enabled = *(u8*)(data + 0x4F);
        }

        if (enabled != 0 && data != NULL && *(u8*)(data + 0x4F) != 0 && *(u8*)(data + 0x4E) != 0) {
            handle = *(u32*)(data + 0x24);
            if (*(u8*)(data + 0x4C) != 0) {
                GSmodelDisableColorSwap(handle);
            }
            if (*(u8*)(data + 0x4D) != 0) {
                GSmodelDisableModulation(handle);
            }
            *(u8*)(data + 0x4F) = 0;
        }

        if (*(u32*)(data + 0x24) != 0) {
            GSmodelSetAnimEndedCallback(*(u32*)(data + 0x24), 0, 0);
        }

        fn_801DA4E8(data, 0);

        if (*(u32*)(data + 0x0C) != 0) {
            fn_801193BC(*(s32*)(data + 0x28));
            fn_800F9210(*(u32*)data, *(u32*)(data + 0x0C));
        }

        Unload__13ModelSequenceFPUc(data + 0x50);

        if (*(u32*)data != 0) {
            if (*(u32*)(data + 4) != 0) {
                fn_800F9210(*(u32*)data, *(u32*)(data + 4));
            }
            if (*(u32*)(data + 8) != 0) {
                fn_800F9210(*(u32*)data, *(u32*)(data + 8));
            }
            id = *(u16*)(data + 0x7C);
            if (id != 0) {
                fn_800E24B0(id);
                fn_800E209C(id);
            }
        }

        wazaSequenceSysFreeSequenceResource(data);
        fn_801D9E34(data);
        fn_801DA014(data);
        memset(data, 0, 0x8C);
    }
}

/**
 * fn_801DD3E4 - Waza color filter clear.
 * Address: 0x801DD3E4 | Size: 0x78
 */
void fn_801DD3E4(void* obj) {
    extern void wazaSequenceApplyStop(void* obj);
    extern void wazaSequenceFree(void* obj);

    void* cur;
    void* next;

    if (obj != NULL) {
        cur = *(void**)((u8*)obj + 0x68);
        while (cur != NULL) {
            next = *(void**)((u8*)cur + 0x34);
            if (*(u8*)((u8*)cur + 0x14) != 0) {
                wazaSequenceApplyStop(cur);
            }
            wazaSequenceFree(cur);
            cur = next;
        }
        *(void**)((u8*)obj + 0x68) = NULL;
    }
}

#endif

#if defined(SEQUENCE_801DD45C_801DE164)

/**
 * sequenceLoad - Waza scene snapshot.
 * Address: 0x801DD45C | Size: 0x18C
 */
BOOL sequenceLoad(void* effect, void* data) {
    extern void* GSresGetResource(u32, u32);
    extern u32 fn_801DF160(void*);
    extern void fn_800EB268(void*, u32);
    extern void* fn_801195AC(void*);
    extern void GSmodelLinkToGSparticleBank(void*, void*);
    extern void GSmodelLinkTexAnimToAnim(void*, s32);
    extern void GSmodelSetAnimEndedCallback(void*, void*, void*);
    extern void sequenceAnimEndCallback(void);
    extern void fn_801DEF0C(void*, s32, s32);
    extern void fn_801DA4E8(void*, s32);
    extern void GSmodelSetPosition(void*, void*);
    extern void GSmodelSetRotation(void*, void*);
    extern void GSmodelSetScale(void*, void*);
    extern u32 wazaSequenceSysGetModelShadowLight__Fv(void);
    extern s32 wazaSequenceSysGetModelShadowCount__Fv(void);
    extern void* wazaSequenceSysGetModelShadowList__Fv(void);
    extern void GSmodelSetShadowFlags(void*, s32);
    extern void GSmodelSetShadowLight(void*, u32);
    extern void GSmodelSetShadowSurface(void*, s32, void*);
    extern void GSmodelSetBoundCheck(void*, s32);
    extern void fn_800E3B44(void*, s32);
    extern void GSlogWrite(const char*, ...);
    extern u8 lbl_803727B0[];
    extern u8 lbl_803727BC[];
    extern const char lbl_80279998[];
    u8* sequence = effect;
    void* model;

    if (sequence == NULL) {
        return FALSE;
    }
    if (fn_801DD5E8(sequence, data)) {
        model = GSresGetResource(*(u32*)(sequence + 0), *(u32*)(sequence + 4));
        *(void**)(sequence + 0x24) = model;
        fn_800EB268(model, fn_801DF160(sequence));
        if (*(u32*)(sequence + 0xC) != 0) {
            *(void**)(sequence + 0x28) =
                fn_801195AC(GSresGetResource(
                    *(u32*)(sequence + 0), *(u32*)(sequence + 0xC)));
            GSmodelLinkToGSparticleBank(model, *(void**)(sequence + 0x28));
        }
        GSmodelLinkTexAnimToAnim(model, 1);
        GSmodelSetAnimEndedCallback(model, sequenceAnimEndCallback, sequence);
        fn_801DEF0C(sequence, 1, 1);
        fn_801DA4E8(sequence, 0);
        GSmodelSetPosition(model, lbl_803727B0);
        GSmodelSetRotation(model, lbl_803727B0);
        GSmodelSetScale(model, lbl_803727BC);
        if (wazaSequenceSysGetModelShadowLight__Fv() != 0 &&
            wazaSequenceSysGetModelShadowCount__Fv() != 0) {
            GSmodelSetShadowFlags(model, 1);
            GSmodelSetShadowLight(
                model, wazaSequenceSysGetModelShadowLight__Fv());
            GSmodelSetShadowSurface(
                model, wazaSequenceSysGetModelShadowCount__Fv(),
                wazaSequenceSysGetModelShadowList__Fv());
            GSmodelSetBoundCheck(model, 1);
            fn_800E3B44(model, 1);
        }
        return TRUE;
    }
    GSlogWrite(lbl_80279998);
    return FALSE;
}

/**
 * fn_801DD5E8 - Waza complex transition effect.
 * Address: 0x801DD5E8 | Size: 0x564
 * Large function handling elaborate transition effects between
 * phases of a move animation.
 */
u8 fn_801DD5E8(void* effect, void* resource) {
    typedef struct SequenceLoadResourceHeader {
        u32 mainSize;      /* 0x00 */
        u32 auxSize;       /* 0x04 */
        u32 entryCount;    /* 0x08 */
        u32 sequenceKind;  /* 0x0C */
        u32 loadMode;      /* 0x10 */
        u32 flag14;        /* 0x14 */
        u32 value18;       /* 0x18 */
        u32 value1C;       /* 0x1C */
        u32 value20;       /* 0x20 */
    } SequenceLoadResourceHeader;
    typedef struct SequenceEntrySrc {
        u32 countA;      /* 0x00 */
        u32 countB;      /* 0x04 */
        u32 field08;     /* 0x08 */
        u8  payload[0x88 - 0x0C];
        u32 field88;     /* 0x88 */
    } SequenceEntrySrc;

    extern u16 fn_800E2C04(u32 size, u32 alignment);
    extern u16 fn_800E3534(u32 size);
    extern void* fn_800E27B0(u16 handle);
    extern void DCFlushRange(void* addr, s32 len);
    extern int wazaSequenceSysGetResID(void);
    extern void fn_80101244(void* resource, u32 size, u32 group, u32 handle);
    extern void fn_801013A0(u32 model, u32 group, u32 data, u32 handle);
    extern void fn_8010147C(void* resource, u32 size, u32 group, u32 handle);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern f32 lbl_8047E3B8;

    u8* sequence;
    SequenceLoadResourceHeader* header;
    u8* data;
    u32 modelResId;
    u32 animResId;
    u32 auxResId;
    s32 startOffset;
    u32 alignedMainSize;
    u16 workHandle;
    u8* workBase;
    u32 i;

    sequence = effect;
    header = (SequenceLoadResourceHeader*)resource;
    if (header == NULL || header->mainSize == 0) {
        return FALSE;
    }

    modelResId = wazaSequenceSysGetResID();
    animResId = wazaSequenceSysGetResID();

    *(s16*)(sequence + 0x1A) = -1;
    *(s16*)(sequence + 0x1C) = -1;
    *(s16*)(sequence + 0x1E) = -1;
    *(u8*)(sequence + 0x4E) = 0;

    switch (header->loadMode) {
    case 1:
    case 2:
        startOffset = 0x20;
        break;
    case 3:
        startOffset = 0x20;
        *(u8*)(sequence + 0x4E) = header->flag14 != 0;
        break;
    case 4:
        startOffset = 0x20;
        *(u8*)(sequence + 0x4E) = (u8)(header->flag14 >> 31);
        break;
    default:
        startOffset = 0x40;
        *(u8*)(sequence + 0x4E) = (u8)(header->flag14 >> 31);
        *(s16*)(sequence + 0x1A) = (s16)header->value18;
        *(s16*)(sequence + 0x1C) = (s16)header->value20;
        *(s16*)(sequence + 0x1E) = (s16)header->value1C;
        break;
    }

    *(u32*)(sequence + 0x0) = 0x4E20;
    *(u32*)(sequence + 0x10) = header->sequenceKind;
    *(u16*)(sequence + 0x14) = (u16)header->entryCount;
    *(u32*)(sequence + 0x32) = 0;
    *(u32*)(sequence + 0x34) = 0;
    *(u16*)(sequence + 0x7C) = 0;

    data = (u8*)header + startOffset;
    alignedMainSize = (header->mainSize + 0x1F) & ~0x1F;

    if ((*(u16*)(sequence + 0x70) == 0x18) ||
        (*(u16*)(sequence + 0x70) == 0x4A) ||
        (*(u16*)(sequence + 0x70) == 0x134)) {
        *(u16*)(sequence + 0x7C) = fn_800E2C04(alignedMainSize, 0x20);
        if (*(u16*)(sequence + 0x7C) == 0) {
            return FALSE;
        }
        workBase = fn_800E27B0(*(u16*)(sequence + 0x7C));
        memcpy(workBase, data, alignedMainSize);
        DCFlushRange(workBase, alignedMainSize);
        fn_8010147C(workBase, header->mainSize, 0x4E20, modelResId);
    } else {
        fn_8010147C(data, header->mainSize, 0x4E20, modelResId);
    }

    if (GSresGetResource(0x4E20, modelResId) == NULL) {
        return FALSE;
    }
    *(u32*)(sequence + 0x8) = modelResId;

    fn_801013A0((u32)GSresGetResource(0x4E20, modelResId), 0x4E20, 0,
                animResId);
    if (GSresGetResource(0x4E20, animResId) == NULL) {
        return FALSE;
    }
    *(u32*)(sequence + 0x4) = animResId;

    if (header->auxSize != 0) {
        auxResId = wazaSequenceSysGetResID();
        fn_80101244(data + alignedMainSize, header->auxSize, 0x4E20, auxResId);
        if (GSresGetResource(0x4E20, auxResId) != NULL) {
            *(u32*)(sequence + 0xC) = auxResId;
        } else {
            *(u32*)(sequence + 0xC) = 0;
        }
        data = data + alignedMainSize + ((header->auxSize + 0x1F) & ~0x1F);
    } else {
        *(u32*)(sequence + 0xC) = 0;
        data = data + alignedMainSize;
    }

    workHandle = fn_800E3534((u16)header->entryCount * 0xD4);
    *(u16*)(sequence + 0x30) = workHandle;
    if (workHandle == 0) {
        *(u32*)(sequence + 0x2C) = 0;
        return FALSE;
    }

    workBase = fn_800E27B0(workHandle);
    *(u8**)(sequence + 0x2C) = workBase;

    for (i = 0; i < header->entryCount; i++) {
        SequenceEntrySrc* src = (SequenceEntrySrc*)data;
        u8* dst = workBase + i * 0xD4;
        u32 j;

        *(u32*)(dst + 0x0) = src->countA;
        for (j = 0; j < src->countA; j++) {
            s32 scale = fn_800D37CC();
            s32 value = *(s32*)((u8*)src + 0x0C + j * 4);
            *(s32*)(dst + 0x0C + j * 4) = (s32)(((f32)value * (f32)scale) /
                                                *(f32*)&lbl_8047E3B8);
        }
        *(u32*)(dst + 0x08) = src->field08;
        *(u32*)(dst + 0x8C) = 1;
        *(u32*)(dst + 0x90) = 0;
        *(u32*)(dst + 0x04) = src->countB + 1;

        for (j = 0; j < src->countB; j++) {
            *(u32*)(dst + 0x94 + j * 8) =
                *(u32*)((u8*)src + 0x8C + j * 8);
            *(u32*)(dst + 0x98 + j * 8) =
                *(u32*)((u8*)src + 0x90 + j * 8);
            if (*(u32*)(dst + 0x94 + j * 8) == 1) {
                *(u32*)(dst + 0x98 + j * 8) = 1;
            }
        }

        memcpy(dst + 0x4C, (u8*)src + 0x4C, 0x40);
        *(u32*)(dst + 0x88) = src->field88;
        data += 0xD0;
    }

    if (header->loadMode >= 4) {
        data = (u8*)header + startOffset +
               ((((u16)header->entryCount * 0xD0) + 0x1F) & ~0x1F) +
               alignedMainSize + ((header->auxSize + 0x1F) & ~0x1F);
    } else {
        data = (u8*)header + startOffset +
               ((u16)header->entryCount * 0xD0) + alignedMainSize +
               ((header->auxSize + 0x1F) & ~0x1F);
    }

    if (*(u8*)(sequence + 0x4E) != 0) {
        *(u32*)(sequence + 0x38) = *(u32*)(data + 0x0);
        *(u32*)(sequence + 0x3C) = *(u32*)(data + 0x4);
        *(u32*)(sequence + 0x40) = *(u32*)(data + 0x8);
        *(u32*)(sequence + 0x44) = *(u32*)(data + 0xC);
        if ((*(u32*)(sequence + 0x38) != 0) || (*(u32*)(sequence + 0x3C) != 1) ||
            (*(u32*)(sequence + 0x40) != 2) || (*(u32*)(sequence + 0x44) != 3)) {
            *(u8*)(sequence + 0x4C) = 1;
        }
        *(u8*)(sequence + 0x4B) = *(u8*)(data + 0x10);
        *(u8*)(sequence + 0x4A) = *(u8*)(data + 0x11);
        *(u8*)(sequence + 0x49) = *(u8*)(data + 0x12);
        *(u8*)(sequence + 0x48) = *(u8*)(data + 0x13);
        if ((*(u8*)(sequence + 0x48) != 0x7F) ||
            (*(u8*)(sequence + 0x49) != 0x7F) ||
            (*(u8*)(sequence + 0x4A) != 0x7F) ||
            (*(u8*)(sequence + 0x4B) != 0x7F)) {
            *(u8*)(sequence + 0x4D) = 1;
        }
    }

    return TRUE;
}

/**
 * fn_801DDB4C - Waza transition effect helper A.
 * Address: 0x801DDB4C | Size: 0xC4
 */
BOOL fn_801DDB4C(void* owner, void* resource) {
    u8* sequence;
    u8* previous;

    if (resource == NULL) {
        return FALSE;
    }
    sequence = (u8*)fn_801DBFB0();
    if (sequence == NULL) {
        return FALSE;
    }
    *(u16*)(sequence + 0x2C) = 0;
    *(u16*)(sequence + 0x2E) = 0;
    *(u16*)(sequence + 0x30) = 0;
    *(void**)(sequence + 0x3C) = owner;
    if (!wazaSequenceLoadData(sequence, resource)) {
        wazaSequenceFree(sequence);
        return FALSE;
    }
    sequence[0x14] = 0;
    sequence[0x15] = 0;
    previous = *(u8**)((u8*)owner + 0x68);
    *(u8**)(sequence + 0x34) = previous;
    if (previous != NULL) {
        *(u8**)(previous + 0x38) = sequence;
    }
    *(void**)(sequence + 0x38) = NULL;
    *(u8**)((u8*)owner + 0x68) = sequence;
    return TRUE;
}

/**
 * fn_801DDC10 - Waza transition effect helper B.
 * Address: 0x801DDC10 | Size: 0x118
 */
s32 fn_801DDC10(u16 index, u16 type) {
    extern u32 lbl_80478CE0;
    extern u32 lbl_80478CC0;
    extern u32 lbl_80478CE8;
    extern u8 lbl_803727C8[];
    extern u8 lbl_8036E150[];
    extern u8 lbl_80373210[];
    u32 count = 0;
    u32 i;

    if (type == 0) {
        return 0;
    }
    if (type == 4) {
        u8* entry;
        if (index == 0 || index >= lbl_80478CE0) {
            return 0;
        }
        entry = lbl_803727C8 + index * 12;
        if (*(void**)entry != NULL && *(void**)(entry + 4) != NULL) {
            count = 1;
        }
    } else {
        u32 kind = type - 1;
        u8* entry;
        if (index == 0 || index >= lbl_80478CC0) {
            return 0;
        }
        entry = lbl_8036E150 + index * 0x1C + kind * 8;
        if (*(void**)(entry + 4) != NULL && *(void**)(entry + 8) != NULL) {
            count = 1;
        }
        entry = lbl_80373210;
        for (i = 0; i < lbl_80478CE8; i++, entry += 0x20) {
            if (*(u32*)entry == index &&
                *(void**)(entry + kind * 8 + 8) != NULL &&
                *(void**)(entry + kind * 8 + 0x0C) != NULL) {
                count++;
            }
        }
    }
    return count;
}

/**
 * fn_801DDD28 - Waza transition effect helper C.
 * Address: 0x801DDD28 | Size: 0x1BC
 */
BOOL fn_801DDD28(void* owner, u16 group, u16 index, u32 variant) {
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void fn_8017B3E4(u32 group);
    extern const char lbl_802799C8[];
    u32 resourceGroup;
    u32 resourceId;
    u8* sequence;
    u8* previous;
    void* resource;
    s32 state;

    if (owner == NULL || index == 0) {
        return FALSE;
    }
    fn_801DDEE4(owner, group, index, (u8)variant, &resourceGroup, &resourceId);
    if (resourceGroup == 0 || resourceId == 0) {
        return FALSE;
    }
    if (GetWaza__12NullSequenceCFUsUs(owner, group, index) != NULL) {
        return TRUE;
    }

    state = fn_8017B2CC(resourceGroup);
    if (state < 0) {
        fn_8017B3E4(resourceGroup);
    } else if (state == 0 && resourceId != 0 &&
               GSresGetResource(resourceGroup, resourceId) == NULL) {
        fn_8017B3E4(resourceGroup);
    }
    do {
        state = fn_8017B2CC(resourceGroup);
        if (state < 0) {
            GSlogWrite(lbl_802799C8);
        }
        if (state != 0) {
            _threadSwitch();
        }
    } while (state != 0);

    resource = GSresGetResource(resourceGroup, resourceId);
    if (resource == NULL) {
        return FALSE;
    }
    sequence = (u8*)fn_801DBFB0();
    if (sequence == NULL) {
        return FALSE;
    }
    *(u16*)(sequence + 0x2C) = group;
    *(u16*)(sequence + 0x2E) = index;
    *(u16*)(sequence + 0x30) = (u16)resourceGroup;
    *(void**)(sequence + 0x3C) = owner;
    if (!wazaSequenceLoadData(sequence, resource)) {
        wazaSequenceFree(sequence);
        return FALSE;
    }
    sequence[0x14] = 0;
    sequence[0x15] = 0;
    previous = *(u8**)((u8*)owner + 0x68);
    *(u8**)(sequence + 0x34) = previous;
    if (previous != NULL) {
        *(u8**)(previous + 0x38) = sequence;
    }
    *(void**)(sequence + 0x38) = NULL;
    *(u8**)((u8*)owner + 0x68) = sequence;
    return TRUE;
}

/**
 * fn_801DDEE4 - Waza hit flash effect.
 * Address: 0x801DDEE4 | Size: 0x280
 */
void fn_801DDEE4(void* owner, u16 group, u16 type, u8 variant,
                 u32* resourceGroup, u32* resourceId) {
    extern u32 lbl_80478CE0;
    extern u32 lbl_80478CC0;
    extern u32 lbl_80478CE8;
    extern u8 lbl_803727C8[];
    extern u8 lbl_8036E150[];
    extern u8 lbl_80373210[];
    extern u8 lbl_80373750[];
    u32 i;

    *resourceGroup = 0;
    *resourceId = 0;
    if (type == 0 || type > 4) {
        return;
    }

    if (owner != NULL && ((u8*)owner)[0x75] != 0) {
        u16 modelId = *(u16*)((u8*)owner + 0x70);
        u32 first;
        u32 last;

        if (variant != 0 && type != 4 && type != 2 &&
            variant < fn_801DDC10(group, type)) {
            first = 0x162;
            last = 0x16E;
        } else {
            switch (type) {
            case 1:
                first = 1;
                last = 0x11C;
                break;
            case 2:
                first = 0x11E;
                last = 0x12A;
                break;
            case 3:
                first = 0x12C;
                last = 0x15E;
                break;
            case 4:
                first = 0x160;
                last = 0x160;
                break;
            default:
                first = 0;
                last = 0;
                break;
            }
        }

        for (i = first; i < last; i++) {
            u8* entry = lbl_80373750 + i * 0x10;
            if (*(u16*)(entry + 2) == modelId &&
                *(u16*)(entry + 4) == group && entry[0] == variant &&
                *(u32*)(entry + 8) != 0 && *(u32*)(entry + 0x0C) != 0) {
                *resourceGroup = *(u32*)(entry + 8);
                *resourceId = *(u32*)(entry + 0x0C);
                return;
            }
        }
    }

    if (type > 3) {
        u8* entry;
        if (group == 0 || group >= lbl_80478CE0) {
            return;
        }
        entry = lbl_803727C8 + group * 12;
        *resourceGroup = *(u32*)entry;
        *resourceId = *(u32*)(entry + 4);
    } else {
        u32 kind = type - 1;
        u8* entry;
        if (group == 0 || group >= lbl_80478CC0) {
            return;
        }
        entry = lbl_8036E150 + group * 0x1C + kind * 8;
        *resourceGroup = *(u32*)(entry + 4);
        *resourceId = *(u32*)(entry + 8);
        if (variant == 0) {
            return;
        }
        entry = lbl_80373210;
        for (i = 0; i < lbl_80478CE8; i++, entry += 0x20) {
            if (*(u32*)entry == group && *(u32*)(entry + 4) == variant) {
                u8* resource = entry + kind * 8;
                if (*(u32*)(resource + 8) != 0 &&
                    *(u32*)(resource + 0x0C) != 0) {
                    *resourceGroup = *(u32*)(resource + 8);
                    *resourceId = *(u32*)(resource + 0x0C);
                    return;
                }
            }
        }
    }
}

#endif

#if defined(SEQUENCE_801DE164_801DE190)

/**
 * fn_801DE164 - Waza hit flash get active.
 * Address: 0x801DE164 | Size: 0x2C
 */
BOOL fn_801DE164(s32 slot) {
    void* obj;

    obj = (void*)slot;
    if (obj == NULL) {
        return FALSE;
    }
    if (*(u8*)((u8*)obj + 0x75) != 0) {
        return *(s32*)((u8*)obj + 0x78);
    }
    return FALSE;
}

#endif

#if defined(SEQUENCE_801DE190_801DE654)

#if !defined(SEQUENCE_CANDIDATE_801DE598_ONLY)
/**
 * fn_801DE190 - Waza hit flash update.
 * Address: 0x801DE190 | Size: 0x288
 */
void* fn_801DE190(u16 index, void* model, u8 variant) {
    extern u32 lbl_80478CD0;
    extern u32 lbl_80478CF0;
    extern u32 lbl_80478CF8;
    extern u8 lbl_80370BD0[];
    extern u32 lbl_80374E40[][2];
    extern u8 lbl_80374F20[];
    extern u8 pokemonGetAnnonKatati(void* model);
    extern void fn_8017B3E4(u32 group);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void* fn_801DB154(void);
    extern void fn_801DB100(void* sequence);
    extern void fn_801DCE0C(void* sequence);
    extern u8 sequenceLoad(void* sequence, void* resource);
    extern void fn_80140190(void* dst, u32 arg1, void* model);
    extern const char lbl_802799C8[];
    u8* entry;
    u8* override;
    void* resourcePtr;
    u8* sequence;
    u32 group;
    u32 resource;
    u32 i;
    s32 state;
    u8 overridden = 0;

    if (index == 0 || index >= lbl_80478CD0) {
        return NULL;
    }

    if (index == 0xC9) {
        i = pokemonGetAnnonKatati(model) % lbl_80478CF0;
        group = lbl_80374E40[i][0];
        resource = lbl_80374E40[i][1];
    } else {
        entry = lbl_80370BD0 + index * 12;
        group = *(u32*)entry;
        resource = *(u32*)(entry + 4);
    }
    if (group == 0 || resource == 0) {
        group = *(u32*)(lbl_80370BD0 + 0xD38);
        resource = *(u32*)(lbl_80370BD0 + 0xD3C);
        if (group == 0 || resource == 0) {
            return NULL;
        }
    }

    if (variant != 0) {
        override = lbl_80374F20;
        for (i = 0; i < lbl_80478CF8; i++, override += 12) {
            if (*(u16*)override == index) {
                u32 overrideGroup = *(u32*)(override + 4);
                u32 overrideResource = *(u32*)(override + 8);
                if (overrideGroup != 0 && overrideResource != 0) {
                    group = overrideGroup;
                    resource = overrideResource;
                    overridden = 1;
                }
                break;
            }
        }
    }

    state = fn_8017B2CC(group);
    if (state < 0) {
        fn_8017B3E4(group);
    } else if (fn_8017B2CC(group) == 0 && resource != 0 &&
               GSresGetResource(group, resource) == NULL) {
        fn_8017B3E4(group);
    }

    for (;;) {
        state = fn_8017B2CC(group);
        if (state < 0) {
            GSlogWrite(lbl_802799C8);
        }
        if (state == 0) {
            break;
        }
        _threadSwitch();
    }

    resourcePtr = GSresGetResource(group, resource);
    if (resourcePtr == NULL) {
        return NULL;
    }
    sequence = fn_801DB154();
    if (sequence == NULL) {
        return NULL;
    }
    *(u16*)(sequence + 0x70) = index;
    *(u16*)(sequence + 0x72) = group;
    sequence[0x75] = 1;
    *(void**)(sequence + 0x78) = model;
    sequence[0x77] = variant;
    if (sequenceLoad(sequence, resourcePtr) == 0) {
        fn_801DB100(sequence);
        return NULL;
    }
    if (variant != 0 && overridden == 0) {
        fn_801DCE0C(sequence);
    }
    if (index == 0x134) {
        fn_80140190(sequence + 0x50, *(u32*)(sequence + 0x24), model);
    }
    return sequence;
}

/**
 * fn_801DE418 - Waza HP drain effect.
 * Address: 0x801DE418 | Size: 0x180
 */
void* fn_801DE418(u16 index) {
    extern u32 lbl_80478CC8;
    extern u8 lbl_80370840[];
    extern void* GSresGetResource(u32 group, u32 resource);
    u8* entry;
    u32 group;
    u32 resourceId;
    void* resource;

    if (index == 0 || index >= lbl_80478CC8) {
        return NULL;
    }

    entry = lbl_80370840 + index * 12;
    group = *(u32*)entry;
    resourceId = *(u32*)(entry + 4);
    if (group == 0 || resourceId == 0) {
        group = *(u32*)(lbl_80370840 + 12);
        resourceId = *(u32*)(lbl_80370840 + 16);
        if (group == 0 || resourceId == 0) {
            return NULL;
        }
    }

    fn_801DE598(group, resourceId);
    resource = GSresGetResource(group, resourceId);
    if (resource == NULL) {
        return NULL;
    }

    /* TODO: Allocate and initialize the effect from the loaded resource. */
    return NULL;
}
#endif

/**
 * fn_801DE598 - Waza HP drain update.
 * Address: 0x801DE598 | Size: 0xBC
 */
void fn_801DE598(u32 group, u32 resource) {
    extern void fn_8017B3E4(u32 group);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern const char lbl_802799C8[];
    s32 state;

    state = fn_8017B2CC(group);
    if (state < 0) {
        fn_8017B3E4(group);
    } else if (fn_8017B2CC(group) == 0 && resource != 0 &&
               GSresGetResource(group, resource) == NULL) {
        fn_8017B3E4(group);
    }

    for (;;) {
        state = fn_8017B2CC(group);
        if (state < 0) {
            GSlogWrite(lbl_802799C8);
        }
        if (state == 0) {
            break;
        }
        _threadSwitch();
    }
}

#endif

#if defined(SEQUENCE_801DE654_801DE698)

/**
 * sequenceAnimEndCallback - Waza HP drain get active.
 * Address: 0x801DE654 | Size: 0x44
 */
void sequenceAnimEndCallback(s32 arg0, s32 arg1) {
    fn_801DE698(arg0, arg1);
    _eyeTexAnimEnded(arg0, arg1);
}

#endif
