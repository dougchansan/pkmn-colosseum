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

/* NullSequence::GetWaza takes (u16, u16), as its mangled name says. */
#define BATTLE_WAZA_GETWAZA_U16
#include "game/battle/battle_waza_types.h"

typedef struct SequenceResourcePair {
    u32 group;    /* 0x0 */
    u32 resource; /* 0x4 */
    u32 extra;    /* 0x8 */
} SequenceResourcePair;

typedef struct SequenceResourceRef {
    u32 group;    /* 0x0 */
    u32 resource; /* 0x4 */
} SequenceResourceRef;

typedef struct SequenceKindEntry {
    u32 key;                      /* 0x00 */
    SequenceResourceRef kinds[3]; /* 0x04 */
} SequenceKindEntry;

typedef struct SequenceVariantEntry {
    u32 group;                    /* 0x00 */
    u32 variant;                  /* 0x04 */
    SequenceResourceRef kinds[3]; /* 0x08 */
} SequenceVariantEntry;

static inline void sequenceWaitResource(u32 group, u32 resource) {
    extern void fn_8017B3E4(u32 group);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern const char lbl_802799C8[];
    s32 state;

    if (fn_8017B2CC(group) < 0) {
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
void* GetWaza__12NullSequenceCFUsUs(void* obj, u16 search_key1, u16 search_key2)
{
    WazaFxNode* cur = ((WazaFxOwner*)obj)->first_child;

    while (cur != NULL) {
        if (cur->field_2C == search_key1 && cur->field_2E == search_key2) {
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
            s32 result;

            if ((s8)sequence->stopping == -1) {
                wazaSequenceApplyStop(sequence);
            } else {
                result = wazaSequenceUpdate(sequence);
                if ((s8)result == 0) {
                    if ((s8)sequence->stopping != -1) {
                        wazaSequenceApplyStop(sequence);
                    }
                } else if ((s8)result < 0) {
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
    extern void* gsParticleBankLoad(void*);
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
    void* model;
    u8* sequence = effect;

    if (sequence == NULL) {
        return FALSE;
    }
    if (fn_801DD5E8(sequence, data)) {
        model = *(void**)(sequence + 0x24) =
            GSresGetResource(*(u32*)(sequence + 0), *(u32*)(sequence + 4));
        fn_800EB268(model, fn_801DF160(sequence));
        if (*(u32*)(sequence + 0xC) != 0) {
            *(void**)(sequence + 0x28) =
                gsParticleBankLoad(GSresGetResource(
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

typedef struct SequenceEntryBlock {
    u32 words[16]; /* 0x4C-0x8B */
} SequenceEntryBlock;
typedef struct SequenceEntryTrack {
    s32 kind;  /* 0x0 */
    s32 value; /* 0x4 */
} SequenceEntryTrack;
typedef struct SequenceEntrySrc {
    s32 countA;                  /* 0x00 */
    s32 countB;                  /* 0x04 */
    u32 field08;                 /* 0x08 */
    s32 values[16];              /* 0x0C */
    SequenceEntryBlock block;    /* 0x4C */
    SequenceEntryTrack tracks[8]; /* 0x8C */
} SequenceEntrySrc;
typedef struct SequenceEntryDst {
    s32 countA;                  /* 0x00 */
    s32 countB;                  /* 0x04 */
    u32 field08;                 /* 0x08 */
    s32 values[16];              /* 0x0C */
    SequenceEntryBlock block;    /* 0x4C */
    SequenceEntryTrack tracks[9]; /* 0x8C */
} SequenceEntryDst;

static inline s32 sequenceCopyValueCount(SequenceEntryDst* dst,
                                         SequenceEntrySrc* src) {
    s32 n;

    n = src->countA;
    dst->countA = n;
    return n;
}

/**
 * fn_801DD5E8 - Waza complex transition effect.
 * Address: 0x801DD5E8 | Size: 0x564
 * Large function handling elaborate transition effects between
 * phases of a move animation.
 */
u8 fn_801DD5E8(void* effect, u8* resource) {
    typedef struct SequenceLoadResourceHeader {
        s32 mainSize;      /* 0x00 */
        s32 auxSize;       /* 0x04 */
        u32 entryCount;    /* 0x08 */
        u32 sequenceKind;  /* 0x0C */
        s32 loadMode;      /* 0x10 */
        u32 flag14;        /* 0x14 */
        u32 value18;       /* 0x18 */
        u32 value1C;       /* 0x1C */
        u32 value20;       /* 0x20 */
    } SequenceLoadResourceHeader;

    extern u16 fn_800E2C04(u32 size, u32 alignment);
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* fn_800E27B0(u16 handle);
    extern void DCFlushRange(void* addr, s32 len);
    extern int wazaSequenceSysGetResID(void);
    extern void loadParticle(void* resource, u32 size, u32 group, u32 handle);
    extern void loadModel(u32 model, u32 group, u32 resource, u32 handle);
    extern void loadArchivePtr(void* resource, u32 size, u32 group, u32 handle);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern const f32 lbl_8047E3B8;

    u32 animResId;
    u16 count;
    u32 modelResId;
    SequenceLoadResourceHeader* header;
    u16 i;
    SequenceEntrySrc* src;
    SequenceEntryDst* dst;
    s32 n;
    u8* sequence;
    SequenceEntryTrack* track;
    s32 j;
    s32 trim;
    u32 auxResId;
    u8* workBase;
    u16 handle;

    sequence = effect;
    modelResId = wazaSequenceSysGetResID();
    animResId = wazaSequenceSysGetResID();
    header = (SequenceLoadResourceHeader*)resource;
    if (header->mainSize == 0) {
        return FALSE;
    }

    *(s16*)(sequence + 0x1A) = -1;
    *(s16*)(sequence + 0x1C) = -1;
    *(s16*)(sequence + 0x1E) = -1;
    *(u8*)(sequence + 0x4E) = 0;

    switch (header->loadMode) {
    case 1:
    case 2:
        trim = -0x10;
        break;
    case 3:
        trim = -0xC;
        *(u8*)(sequence + 0x4E) = header->flag14 != 0;
        break;
    case 4:
        trim = -0xC;
        *(u8*)(sequence + 0x4E) = header->flag14 >> 31;
        break;
    case 5:
    default:
        trim = 0;
        *(u8*)(sequence + 0x4E) = header->flag14 >> 31;
        *(s16*)(sequence + 0x1A) = header->value18;
        *(s16*)(sequence + 0x1C) = header->value20;
        *(s16*)(sequence + 0x1E) = header->value1C;
        break;
    }

    *(u32*)(sequence + 0x0) = 0x4E20;
    *(u32*)(sequence + 0x10) = header->sequenceKind;
    resource += (sizeof(SequenceLoadResourceHeader) + trim + 0x1F) & ~0x1F;
    *(u16*)(sequence + 0x32) = 0;
    *(u16*)(sequence + 0x34) = 0;
    *(u16*)(sequence + 0x14) = count = header->entryCount;
    *(u16*)(sequence + 0x7C) = 0;

    if ((*(u16*)(sequence + 0x70) == 0x18) ||
        (*(u16*)(sequence + 0x70) == 0x4A) ||
        (*(u16*)(sequence + 0x70) == 0x134)) {
        *(u16*)(sequence + 0x7C) =
            fn_800E2C04((header->mainSize + 0x1F) & ~0x1F, 0x20);
        if (*(u16*)(sequence + 0x7C) == 0) {
            return FALSE;
        }
        workBase = fn_800E27B0(*(u16*)(sequence + 0x7C));
        memcpy(workBase, resource, (header->mainSize + 0x1F) & ~0x1F);
        DCFlushRange(workBase, (header->mainSize + 0x1F) & ~0x1F);
        loadArchivePtr(workBase, header->mainSize, 0x4E20, modelResId);
    } else {
        loadArchivePtr(resource, header->mainSize, 0x4E20, modelResId);
    }

    if (GSresGetResource(0x4E20, modelResId) == NULL) {
        return FALSE;
    }
    *(u32*)(sequence + 0x8) = modelResId;

    loadModel((u32)GSresGetResource(0x4E20, modelResId), 0x4E20, 0,
                animResId);
    if (GSresGetResource(0x4E20, animResId) == NULL) {
        return FALSE;
    }
    *(u32*)(sequence + 0x4) = animResId;

    resource += (header->mainSize + 0x1F) & ~0x1F;
    if (header->auxSize != 0) {
        auxResId = wazaSequenceSysGetResID();
        loadParticle(resource, header->auxSize, 0x4E20, auxResId);
        if (GSresGetResource(0x4E20, auxResId) != NULL) {
            *(u32*)(sequence + 0xC) = auxResId;
        } else {
            *(u32*)(sequence + 0xC) = 0;
        }
        resource += (header->auxSize + 0x1F) & ~0x1F;
    }

    handle = _toolentryAlloc__FUl(count * 0xD4);
    *(u16*)(sequence + 0x30) = handle;
    if (handle != 0) {
        src = (SequenceEntrySrc*)resource;
        dst = (SequenceEntryDst*)(*(u8**)(sequence + 0x2C) = fn_800E27B0(handle));
        for (i = 0; i < count; i++) {
            n = sequenceCopyValueCount(dst, src);
            for (j = 0; j < n; j++) {
                dst->values[j] = (f32)src->values[j] * (f32)fn_800D37CC() /
                                 lbl_8047E3B8;
            }
            dst->field08 = src->field08;
            dst->tracks[0].kind = 1;
            dst->tracks[0].value = 0;
            n = src->countB;
            dst->countB = n + 1;
            track = &dst->tracks[1];
            for (j = 0; j < n; j++, track++) {
                track->kind = src->tracks[j].kind;
                track->value = src->tracks[j].value;
                if (track->kind == 1) {
                    track->value = 1;
                }
            }
            dst->block.words[0] = src->block.words[0];
            dst->block.words[1] = src->block.words[1];
            dst->block.words[2] = src->block.words[2];
            dst->block.words[3] = src->block.words[3];
            dst->block.words[4] = src->block.words[4];
            dst->block.words[5] = src->block.words[5];
            dst->block.words[6] = src->block.words[6];
            dst->block.words[7] = src->block.words[7];
            dst->block.words[8] = src->block.words[8];
            dst->block.words[9] = src->block.words[9];
            dst->block.words[10] = src->block.words[10];
            dst->block.words[11] = src->block.words[11];
            dst->block.words[12] = src->block.words[12];
            dst->block.words[13] = src->block.words[13];
            dst->block.words[14] = src->block.words[14];
            dst->block.words[15] = src->block.words[15];
            src = (SequenceEntrySrc*)((u8*)src + 0xD0);
            dst = (SequenceEntryDst*)((u8*)dst + 0xD4);
        }

        if (header->loadMode < 4) {
            resource += count * 0xD0;
        } else {
            resource += (count * 0xD0 + 0x1F) & ~0x1F;
        }

        if (*(u8*)(sequence + 0x4E) != 0) {
            *(s32*)(sequence + 0x38) = *(s32*)(resource + 0x0);
            *(s32*)(sequence + 0x3C) = *(s32*)(resource + 0x4);
            *(s32*)(sequence + 0x40) = *(s32*)(resource + 0x8);
            *(s32*)(sequence + 0x44) = *(s32*)(resource + 0xC);
            if ((*(s32*)(sequence + 0x38) != 0) ||
                (*(s32*)(sequence + 0x3C) != 1) ||
                (*(s32*)(sequence + 0x40) != 2) ||
                (*(s32*)(sequence + 0x44) != 3)) {
                *(u8*)(sequence + 0x4C) = 1;
            }
            *(u8*)(sequence + 0x4B) = *(s32*)(resource + 0x10) >> 24;
            *(u8*)(sequence + 0x4A) = *(u32*)(resource + 0x10) >> 16;
            *(u8*)(sequence + 0x49) = *(u32*)(resource + 0x10) >> 8;
            *(u8*)(sequence + 0x48) = *(s32*)(resource + 0x10);
            if ((*(u8*)(sequence + 0x48) != 0x7F) ||
                (*(u8*)(sequence + 0x49) != 0x7F) ||
                (*(u8*)(sequence + 0x4A) != 0x7F) ||
                (*(u8*)(sequence + 0x4B) != 0x7F)) {
                *(u8*)(sequence + 0x4D) = 1;
            }
        }
        return TRUE;
    }
    *(void**)(sequence + 0x2C) = NULL;
    return FALSE;
}

/**
 * fn_801DDB4C - Waza transition effect helper A.
 * Address: 0x801DDB4C | Size: 0xC4
 */
BOOL fn_801DDB4C(void* owner, void* resource) {
    u8* sequence;

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
    *(u8**)(sequence + 0x34) = *(u8**)((u8*)owner + 0x68);
    if (*(u8**)((u8*)owner + 0x68) != NULL) {
        *(u8**)(*(u8**)((u8*)owner + 0x68) + 0x38) = sequence;
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
    extern SequenceResourcePair lbl_803727C8[];
    extern SequenceKindEntry lbl_8036E150[];
    extern SequenceVariantEntry lbl_80373210[];
    u32 i;
    u32 group;
    u32 resource;
    u32 count = 0;

    if (type == 0) {
        return 0;
    }
    if (type == 4) {
        if (index == 0 || index >= lbl_80478CE0) {
            return 0;
        }
        group = lbl_803727C8[index].group;
        resource = lbl_803727C8[index].resource;
        if (group != 0 && resource != 0) {
            count = 1;
        }
    } else {
        u16 kind = type - 1;
        SequenceVariantEntry* entry = lbl_80373210;
        if (index == 0 || index >= lbl_80478CC0) {
            return 0;
        }
        group = lbl_8036E150[index].kinds[kind].group;
        resource = lbl_8036E150[index].kinds[kind].resource;
        if (group != 0 && resource != 0) {
            count = 1;
        }
        for (i = 0; i < lbl_80478CE8; i++, entry++) {
            if (entry->group == index) {
                group = entry->kinds[kind].group;
                resource = entry->kinds[kind].resource;
                if (group != 0 && resource != 0) {
                    count++;
                }
            }
        }
    }
    return count;
}

/**
 * fn_801DDD28 - Waza transition effect helper C.
 * Address: 0x801DDD28 | Size: 0x1BC
 */
BOOL fn_801DDD28(void* owner, u16 group, u16 index, u8 variant) {
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void fn_8017B3E4(u32 group);
    extern const char lbl_802799C8[];
    u32 resourceGroup;
    u32 resourceId;
    u8* sequence;
    void* resource;

    if (owner == NULL) {
        return FALSE;
    }
    if (index == 0) {
        return FALSE;
    }
    fn_801DDEE4(owner, group, index, variant, &resourceGroup, &resourceId);
    if (resourceGroup == 0 || resourceId == 0) {
        return FALSE;
    }
    if (GetWaza__12NullSequenceCFUsUs(owner, group, index) != NULL) {
        return TRUE;
    }

    sequenceWaitResource(resourceGroup, resourceId);

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
    *(u8**)(sequence + 0x34) = *(u8**)((u8*)owner + 0x68);
    if (*(u8**)((u8*)owner + 0x68) != NULL) {
        *(u8**)(*(u8**)((u8*)owner + 0x68) + 0x38) = sequence;
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
    extern SequenceResourcePair lbl_803727C8[];
    extern SequenceKindEntry lbl_8036E150[];
    extern SequenceVariantEntry lbl_80373210[];
    extern u8 lbl_80373750[];
    u32 i;

    *resourceId = 0;
    *resourceGroup = 0;
    if (type == 0 || type > 4) {
        return;
    }

    if (owner != NULL && ((u8*)owner)[0x75] != 0) {
        s32 modelId = *(u16*)((u8*)owner + 0x70);
        u16 first;
        u16 last;

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
            }
        }

        for (i = first; i < last; i++) {
            u8* entry = lbl_80373750 + i * 0x10;
            if (*(u16*)(entry + 2) == modelId &&
                *(u16*)(entry + 4) == group && entry[0] == variant) {
                u32 foundGroup = *(u32*)(entry + 8);
                u32 foundId = *(u32*)(entry + 0x0C);
                if (foundGroup != 0 && foundId != 0) {
                    *resourceGroup = foundGroup;
                    *resourceId = foundId;
                    return;
                }
                break;
            }
        }
    }

    if (type > 3) {
        if (group == 0 || group >= lbl_80478CE0) {
            return;
        }
        *resourceGroup = lbl_803727C8[group].group;
        *resourceId = lbl_803727C8[group].resource;
    } else {
        u16 kind = type - 1;
        SequenceVariantEntry* entry;
        if (group == 0 || group >= lbl_80478CC0) {
            return;
        }
        *resourceGroup = lbl_8036E150[group].kinds[kind].group;
        *resourceId = lbl_8036E150[group].kinds[kind].resource;
        if (variant == 0) {
            return;
        }
        entry = lbl_80373210;
        for (i = 0; i < lbl_80478CE8; i++, entry++) {
            if (entry->group == group && entry->variant == variant) {
                u32 foundGroup = entry->kinds[kind].group;
                u32 foundId = entry->kinds[kind].resource;
                if (foundGroup != 0 && foundId != 0) {
                    *resourceGroup = foundGroup;
                    *resourceId = foundId;
                }
                return;
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

#if !defined(SEQUENCE_CANDIDATE_801DE598_ONLY) && \
    !defined(SEQUENCE_CANDIDATE_801DE418_ONLY)
/**
 * fn_801DE190 - Waza hit flash update.
 * Address: 0x801DE190 | Size: 0x288
 */
void* fn_801DE190(u16 index, void* model, u8 variant) {
    typedef struct SequenceOverrideEntry {
        u16 index;    /* 0x0 */
        u32 group;    /* 0x4 */
        u32 resource; /* 0x8 */
    } SequenceOverrideEntry;
    extern u32 lbl_80478CD0;
    extern u32 lbl_80478CF0;
    extern u32 lbl_80478CF8;
    extern SequenceResourcePair lbl_80370BD0[];
    extern SequenceResourceRef lbl_80374E40[];
    extern SequenceOverrideEntry lbl_80374F20[];
    extern u8 pokemonGetAnnonKatati(void* model);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void* fn_801DB154(void);
    extern void fn_801DB100(void* sequence);
    extern void fn_801DCE0C(void* sequence);
    extern u8 sequenceLoad(void* sequence, void* resource);
    extern void fn_80140190(void* dst, u32 arg1, void* model);
    void* resourcePtr;
    u8* sequence;
    u32 group;
    u32 resource;
    u8 overridden = 0;

    if (index == 0 || index >= lbl_80478CD0) {
        return NULL;
    }

    if (index == 0xC9) {
        u8 form = pokemonGetAnnonKatati(model) % lbl_80478CF0;
        group = lbl_80374E40[form].group;
        resource = lbl_80374E40[form].resource;
    } else {
        group = lbl_80370BD0[index].group;
        resource = lbl_80370BD0[index].resource;
    }
    if (group == 0 || resource == 0) {
        group = lbl_80370BD0[0x11A].group;
        resource = lbl_80370BD0[0x11A].resource;
        if (group == 0 || resource == 0) {
            return NULL;
        }
    }

    if (variant != 0) {
        u32 overrideGroup = 0;
        u32 overrideResource = 0;
        u32 i;

        for (i = 0; i < lbl_80478CF8; i++) {
            if (index == lbl_80374F20[i].index) {
                overrideGroup = lbl_80374F20[i].group;
                overrideResource = lbl_80374F20[i].resource;
                break;
            }
        }
        if (overrideGroup != 0 && overrideResource != 0) {
            group = overrideGroup;
            resource = overrideResource;
            overridden = 1;
        }
    }

    sequenceWaitResource(group, resource);

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

#endif

#if !defined(SEQUENCE_CANDIDATE_801DE598_ONLY) && \
    !defined(SEQUENCE_CANDIDATE_801DE190_ONLY)
/**
 * fn_801DE418 - Waza HP drain effect.
 * Address: 0x801DE418 | Size: 0x180
 */
void* fn_801DE418(u16 index) {
    extern u32 lbl_80478CC8;
    extern SequenceResourcePair lbl_80370840[];
    extern void* GSresGetResource(u32 group, u32 resource);
    extern void* fn_801DB154(void);
    extern void fn_801DB100(void* sequence);
    extern u8 sequenceLoad(void* sequence, void* resource);
    u8* sequence;
    u32 group;
    u32 resourceId;
    void* resource;

    if (index == 0 || index >= lbl_80478CC8) {
        return NULL;
    }

    group = lbl_80370840[index].group;
    resourceId = lbl_80370840[index].resource;
    if (group == 0 || resourceId == 0) {
        group = lbl_80370840[1].group;
        resourceId = lbl_80370840[1].resource;
        if (group == 0 || resourceId == 0) {
            return NULL;
        }
    }

    sequenceWaitResource(group, resourceId);
    resource = GSresGetResource(group, resourceId);
    if (resource == NULL) {
        return NULL;
    }

    sequence = fn_801DB154();
    if (sequence == NULL) {
        return NULL;
    }
    *(u16*)(sequence + 0x70) = index;
    *(u16*)(sequence + 0x72) = group;
    sequence[0x75] = 0;
    if (sequenceLoad(sequence, resource) == 0) {
        fn_801DB100(sequence);
        return NULL;
    }
    return sequence;
}
#endif

#if !defined(SEQUENCE_CANDIDATE_801DE190_ONLY) && \
    !defined(SEQUENCE_CANDIDATE_801DE418_ONLY)
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
