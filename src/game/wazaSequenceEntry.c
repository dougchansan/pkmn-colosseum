/**
 * @file wazaSequenceEntry.c
 * @brief wazaSequenceEntry: per-entry (particle/model/camera/sound) dispatchers
 * for a waza sequence.
 *
 * Split from the former game/battle/battle_waza.c CodeCandidate bucket
 * (0x801D1470-0x801DE698); see config/GC6E01/splits.txt for the exact
 * address range of this translation unit. Shared typedefs and cross-TU
 * forward declarations live in include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"


/**
 * wazaSequenceEntryStop / wazaSequenceEntryStop - Stop a single waza entry.
 * Address: 0x801D7E58 | Size: 0x374
 * Proposed name from symbols: wazaSequenceEntryStop.
 */
u8 wazaSequenceEntryStop(void* entry, BOOL immediate) {
    extern void GSmodelStopTexAnimation(void* model);
    extern void GSmodelGetScale(void* model, void* out);
    extern void GSmodelSetPosition(void* model, void* position);
    extern void GSmodelSetRotation(void* model, void* rotation);
    extern void GSmodelSetScale(void* model, void* scale);
    extern void* GSmodelGetBound(void* model);
    extern void GSmodelDetachFromGSpart(void* model, s32 applyTransform);
    extern void GSmodelSetModulationColor(void* model, void* color);
    extern void GSlerpGetLinearInterpolationVector(void* dst, void* src,
                                                   void* target, f32 t);
    extern void modelRemoveCenterNull(void* model);
    extern void* fn_8013151C(u32 arg);
    extern const char lbl_80279588[];
    extern u8 lbl_803725B0[];
    extern u8 lbl_803725BC[];
    extern f32 lbl_8047E348;
    extern s32 lbl_8047B408;
    u8* node = entry;
    u8* sequence = *(u8**)(node + 0xB0);
    u8* owner = *(u8**)(sequence + 0x3C);
    void* ownerModel = *(void**)(owner + 0x24);

    if (*(s32*)(node + 0x6C) == 2) {
        return TRUE;
    }
    if (!immediate && *(s32*)(node + 0x6C) != 1) {
        return FALSE;
    }

    switch (*(s32*)(node + 0x04)) {
    case 0:
    case 1:
    case 6:
        break;

    case 2: {
        void* model = *(void**)(node + 0xA4);

        if (model != NULL) {
            GSmodelStopAnimation(model);
            GSmodelStopTexAnimation(model);
            GSmodelSetVisibility(model, 0);

            if (*(s32*)(node + 0x94) != 0) {
                f32 position[3];
                f32 rotation[3];
                f32 scale[3];
                f32 ownerPosition[3];
                f32 boundCenter[3];

                GSmodelGetPosition(model, position);
                GSmodelGetRotation(model, rotation);
                GSmodelGetScale(model, scale);
                GSmodelGetPosition(ownerModel, ownerPosition);
                fn_800E0168(ownerPosition, ownerPosition, position);
                if (*(s32*)(node + 0xA0) != 0) {
                    GSmodelDetachFromGSpart(ownerModel, 0);
                }
                GSmodelSetPosition(ownerModel, position);
                GSmodelSetRotation(ownerModel, rotation);
                GSmodelSetScale(ownerModel, lbl_803725BC);
                if ((*(u32*)(node + 0x9C) & 0x10) != 0) {
                    void* bound = GSmodelGetBound(ownerModel);

                    GSlerpGetLinearInterpolationVector(
                        boundCenter, (u8*)bound + 0x10, (u8*)bound + 0x1C,
                        lbl_8047E348);
                    fn_800E0168(position, position, boundCenter);
                    GSmodelSetPosition(ownerModel, position);
                    GSmodelRemoveNull(ownerModel);
                }
                if ((*(u32*)(node + 0x9C) & 8) != 0) {
                    GSmodelAddNull(ownerModel, (GSvec*)ownerPosition, NULL, NULL);
                    GSvecCopy(owner + 0x5C, ownerPosition);
                }
            } else {
                if (*(s32*)(node + 0xA0) != 0) {
                    GSmodelDetachFromGSpart(model, 0);
                }
                if ((*(u32*)(node + 0x1C) & 1) == 0 &&
                    *(u32*)(node + 0x20) == 0x10) {
                    modelRemoveCenterNull(ownerModel);
                }
            }

            GSmodelSetPosition(model, lbl_803725B0);
            GSmodelSetRotation(model, lbl_803725B0);
            GSmodelSetScale(model, lbl_803725BC);
        }
        break;
    }

    case 3:
        if (*(u32*)(node + 0x90) != 0 &&
            (*(u32*)(node + 0x1C) & 1) == 0 &&
            *(u32*)(node + 0x20) == 0x10) {
            modelRemoveCenterNull(ownerModel);
        }
        break;

    case 4:
        if (*(u32*)(node + 0x78) != 0) {
            if (*(s32*)(node + 0x88) == 6) {
                u8* effect = fn_8013151C(*(u32*)(node + 0x78));

                if (*(u32*)(effect + 0xA8) != 0) {
                    f32 position[3];

                    GSmodelGetPosition(ownerModel, position);
                    fn_800E0168(position, position, effect + 0x48);
                    GSmodelSetPosition(ownerModel, position);
                }
            } else if (*(s32*)(node + 0x88) == 0) {
                u8* effect = fn_8013151C(*(u32*)(node + 0x78));

                if (*(u8*)(effect + 0x4D) != 0) {
                    GSmodelSetModulationColor(ownerModel, effect + 0x44);
                }
            }
            fn_80131010(*(u32*)(node + 0x78));
        }
        break;

    case 5:
        if ((*(u32*)(node + 0x7C) & 1) != 0) {
            lbl_8047B408 = 0;
        }
        fn_80166B18(*(u32*)(node + 0x78));
        break;

    default:
        GSlogWrite(lbl_80279588);
        *(s32*)(node + 0x6C) = -1;
        return FALSE;
    }

    *(s32*)(node + 0x6C) = 2;
    return TRUE;
}

/**
 * wazaSequenceEntryUpdate / wazaSequenceEntryUpdate - Update a single waza entry.
 * Address: 0x801D81CC | Size: 0x328
 * Proposed name from symbols: wazaSequenceEntryUpdate.
 */
u8 wazaSequenceEntryUpdate(void* entry, s32 elapsed) {
    extern u8 GSmodelHasAnimationEnded(void* model);
    extern u8 GSmodelHasTexAnimationEnded(void* model);
    extern s32 fn_80118DA8(void* ptr);
    extern u32 fn_80118D84(void* obj);
    extern u8 GSthreadIsRunning(u32 task);
    extern BOOL fn_801310A8(u32 effectId);
    extern void* fn_8013151C(u32 arg);
    extern u32 fn_8013AB34(void* ptr);
    extern const char lbl_802795B4[];
    extern s32 lbl_8047B408;
    u8* node = entry;
    u32 kind;

    *(s32*)(node + 0x74) += elapsed;
    kind = *(u32*)(node + 0x04);

    if (kind > 6) {
        if (fn_800057A8() == 2) {
            fn_801D744C(1);
        }
        GSlogWrite(lbl_802795B4);
        return FALSE;
    }

    switch (kind) {
    case 2: {
        void* model = *(void**)(node + 0xA4);
        u8 done;

        if (model == NULL) {
            return TRUE;
        }

        if (*(s32*)(node + 0x88) < 0) {
            done = TRUE;
        } else if (*(s32*)(node + 0x84) == 1) {
            u8* sequence = *(u8**)(node + 0xB0);
            s32 target = *(s32*)(node + 0x70) +
                         *(s32*)(node + 0x28 + (*(s32*)(node + 0x14) * 4));
            done = *(s32*)sequence >= target;
        } else {
            done = GSmodelHasAnimationEnded(model);
        }

        if (*(s32*)(node + 0x88) != *(s32*)(node + 0x90) &&
            *(s32*)(node + 0x90) >= 0) {
            u8 texDone;

            if (*(s32*)(node + 0x8C) == 1) {
                u8* sequence = *(u8**)(node + 0xB0);
                s32 target = *(s32*)(node + 0x70) +
                             *(s32*)(node + 0x28 + (*(s32*)(node + 0x14) * 4));
                texDone = *(s32*)sequence >= target;
            } else {
                texDone = GSmodelHasTexAnimationEnded(model);
            }

            done = done && texDone;
        }

        if (!done) {
            return TRUE;
        }
        if ((*(u32*)(node + 0x9C) & 2) != 0) {
            *(s32*)(node + 0x6C) = 3;
            return TRUE;
        }
        return FALSE;
    }

    case 5:
        if ((*(u32*)(node + 0x7C) & 1) != 0) {
            if (lbl_8047B408 != 0) {
                return GSthreadIsRunning(lbl_8047B408);
            }
            return FALSE;
        } else {
            u32 status = fn_801666BC(*(u32*)(node + 0x78));
            return status == 2 || status == 3;
        }

    case 4:
        if (*(s32*)(node + 0x78) == 0) {
            return TRUE;
        } else {
            u32 effectId = *(u32*)(node + 0x78);

            if (!fn_801310A8(effectId)) {
                return FALSE;
            }
            if (*(s32*)(node + 0x88) == 0) {
                if (fn_8013AB34(fn_8013151C(effectId))) {
                    *(s32*)(node + 0x6C) = 3;
                }
            }
            return TRUE;
        }

    case 3:
        if (*(void**)(node + 0x8C) == NULL) {
            return TRUE;
        }
        if (fn_80118DA8(*(void**)(node + 0x8C)) != 0) {
            return TRUE;
        }
        return fn_80118D84(*(void**)(node + 0x8C)) != 0;

    case 1:
        if (*(s32*)(node + 0x78) != 0) {
            return FALSE;
        }
        if ((*(s32*)(node + 0x74) - *(s32*)(node + 0x70)) < *(s32*)(node + 0x7C)) {
            return TRUE;
        } else {
            u8* sequence = *(u8**)(node + 0xB0);
            u8* current = *(u8**)(sequence + 0x24);

            while (current != NULL) {
                if (*(s32*)(current + 0x6C) == 3) {
                    *(s8*)(sequence + 0x15) = -1;
                    return FALSE;
                }
                current = *(u8**)(current + 0xA8);
            }
            *(u8*)(sequence + 0x15) = 1;
            return FALSE;
        }

    case 6:
        return FALSE;

    case 0:
    default:
        if (fn_800057A8() == 2) {
            fn_801D744C(1);
        }
        GSlogWrite(lbl_802795B4);
        return FALSE;
    }
}

/**
 * wazaSequenceEntryStart / wazaSequenceEntryStart - Start a single waza entry.
 * Address: 0x801D84F4 | Size: 0x2BC
 * Proposed name from symbols: wazaSequenceEntryStart.
 * Referenced by battle_logic.c.
 */
u8 wazaSequenceEntryStart(void* entry) {
    WazaSequenceNode* node = entry;
    WazaSequence* sequence = node->sequence;
    WazaSequenceOwner* owner = sequence->owner;
    u8 started = FALSE;

    if ((sequence->flags & 0x2000) == 0 &&
        (owner->flags & 1) == 0 &&
        node->kind != 5 && node->kind != 6 && node->kind != 1) {
        node->runtimeState = 2;
        node->currentTime = node->startTime;
        return TRUE;
    }

    switch (node->kind) {
    case 2:
        started = _wazaSequenceModelEntryStart(node);
        break;
    case 3:
        started = _wazaSequenceParticleEntryStart(node);
        break;
    case 4:
        started = _wazaSequenceEffectEntryStart(node);
        break;
    }

    if (started) {
        node->runtimeState = 1;
        node->currentTime = node->startTime;
        return TRUE;
    }
    node->runtimeState = 2;
    return FALSE;
}

/**
 * _wazaSequenceEffectEntryStart / wazaSequenceStartEntry - Initialize entry resources.
 * Address: 0x801D87B0 | Size: 0x388
 * Proposed name from symbols: wazaSequenceStartEntry.
 */
u8 _wazaSequenceEffectEntryStart(void* entry) {
    extern const char lbl_8027964C[];
    extern BOOL GSeffect(u32 effectId);
    extern void* fn_8013151C(u32 arg);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern u32 fn_80113F48(void);
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorReadMakeModelResID(u32 value);
    extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* symbols);
    extern u8 GSmodelIsModulationEnabled(void* model);
    extern void GSmodelGetModulationColor(void* model, void* color);
    extern void GSmodelGetRotation(void* model, void* out);
    extern void GSmodelGetPosition(void* model, void* out);
    extern void GSmodelSetPosition(void* model, void* position);
    u8* node = entry;
    u8* sequence = *(u8**)(node + 0xB0);
    u8* owner = *(u8**)(sequence + 0x3C);
    u8* table = *(u8**)(owner + 0x2C) + *(u16*)(owner + 0x32) * 0xD4;
    u16 owner0 = *(u32*)(owner + 0x00);
    u16 owner1 = *(u32*)(owner + 0x04);
    u32 effectId = *(u32*)(node + 0x78);
    u8* effect;
    f32 scale[3];

    if (effectId == 0) {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x80);
        }
        return FALSE;
    }

    effect = fn_8013151C(effectId);
    if (effect == NULL) {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x80);
        }
        return FALSE;
    }

    fn_801D9950(sequence, scale, *(s32*)(owner + 0x10));

    switch (*(s32*)(node + 0x88)) {
    case 0:
        switch (*(u8*)(effect + 0x4C)) {
        case 1: {
            void* floor = floorDataBiosGetCurrentPtr();
            u32 group = fn_80113F48();
            void* archive = GSresGetResource(group, *(u32*)((u8*)floor + 0x08));
            void* list = NULL;
            u32 baseId;
            s32 count = 0;
            s32 i;

            if (archive != NULL) {
                list = HSD_ArchiveGetPublicAddress(archive, (char*)lbl_8027964C);
            }
            if (list != NULL && *(u32*)list != 0) {
                baseId = floorReadMakeModelResID(*(u32*)((u8*)floor + 0x08));
                for (i = 0; ((u32*)list)[i] != 0; i++) {
                    void* resource = GSresGetResource(group, baseId | i);

                    if (resource != NULL) {
                        *(void**)(effect + 4 + count * 4) = resource;
                        count++;
                    }
                }
            }
            *(u32*)(effect + 0x48) = (u16)count;
            break;
        }

        case 2:
            *(void**)(effect + 0x04) = *(void**)(owner + 0x24);
            *(u32*)(effect + 0x48) = 1;
            if (fn_801DCDCC(owner) && GSmodelIsModulationEnabled(*(void**)(owner + 0x24))) {
                *(u8*)(effect + 0x4D) = 1;
                *(u8*)(effect + 0x4F) = 1;
                GSmodelGetModulationColor(*(void**)(owner + 0x24), effect + 0x44);
            } else {
                *(u8*)(effect + 0x4D) = 0;
                *(u8*)(effect + 0x4F) = 0;
            }
            break;
        }
        break;

    case 1:
        *(u16*)(effect + 0x4C) = owner0;
        *(u16*)(effect + 0x4E) = owner1;
        *(u32*)(effect + 0x50) = *(u32*)(table + *(u32*)(node + 0x80) * 4 + 0x4C);
        break;

    case 2:
        *(f32*)(effect + 0x10) = scale[0];
        *(u16*)(effect + 0x0C) = owner0;
        *(u16*)(effect + 0x0E) = owner1;
        break;

    case 3:
        GSvecCopy(effect + 0x28, scale);
        *(u16*)(effect + 0x0A) = owner0;
        *(u16*)(effect + 0x0C) = owner1;
        *(u16*)(effect + 0x0E) = *(u32*)(table + *(u32*)(node + 0x80) * 4 + 0x4C);
        break;

    case 4:
        *(u16*)(effect + 0x24) = owner0;
        *(u16*)(effect + 0x26) = owner1;
        *(u16*)(effect + 0x28) = *(u32*)(table + *(u32*)(node + 0x80) * 4 + 0x4C);
        *(u16*)(effect + 0x2A) = *(u32*)(table + *(u32*)(node + 0x84) * 4 + 0x4C);
        break;

    case 5:
        *(u16*)(effect + 0x46) = owner0;
        *(u16*)(effect + 0x48) = owner1;
        *(u16*)(effect + 0x4A) = *(u32*)(table + *(u32*)(node + 0x80) * 4 + 0x4C);
        *(f32*)(effect + 0x0C) = scale[0];
        break;

    case 6:
        GSmodelGetRotation(*(void**)(owner + 0x24), effect + 0x54);
        if (*(u32*)(effect + 0xA8) != 0) {
            f32 position[3];

            GSmodelGetPosition(*(void**)(owner + 0x24), position);
            GSvecAdd(position, position, effect + 0x48);
            GSmodelSetPosition(*(void**)(owner + 0x24), position);
        }
        break;

    case 7:
        GSmodelGetRotation(*(void**)(owner + 0x24), effect + 0x30);
        break;

    case 8:
        *(void**)(effect + 0x00) = *(void**)(owner + 0x24);
        break;

    case 9:
        if (*(u32*)(effect + 0x10) == 0) {
            *(void**)(effect + 0x00) = *(void**)(owner + 0x24);
        }
        break;

    case 10:
        *(void**)(effect + 0x00) = *(void**)(owner + 0x24);
        break;

    case 11:
        *(void**)(effect + 0x08) = *(void**)(owner + 0x24);
        *(u32*)(effect + 0x0C) = *(u32*)(*(u8**)(owner + 0x2C) + 0x54);
        *(f32*)(effect + 0x14) = *(f32*)(effect + 0x18) * scale[0];
        break;

    case 12:
        break;

    default:
        if (fn_800057A8() == 2) {
            fn_801D744C(0x80);
        }
        return FALSE;
    }

    GSeffect(effectId);
    return TRUE;
}

/**
 * _wazaSequenceParticleEntryStart / _wazaSequenceParticleEntryStart - Particle entry init.
 * Address: 0x801D8B38 | Size: 0x6B4
 * Proposed name from symbols: _wazaSequenceParticleEntryStart.
 * Large function that initializes a particle effect for a move animation.
 */
u8 _wazaSequenceParticleEntryStart(void* entry) {
    WazaSequenceNode* node = entry;
    WazaSequence* sequence;
    WazaEffect* owner;
    WazaSequenceNode* linked;
    void* model;
    void* part;
    f32 position[3];
    f32 rotation[3];
    f32 scale[3];
    u32 flags;
    s32 selector;

    extern void GSmodelGetPosition(void* model, void* out);
    extern void GSmodelGetRotation(void* model, void* out);
    extern void GSpartGetTransform(void* part, void* pos, void* rot,
                                   void* scale);

    /*
     * Particle entries cannot start until their resource was loaded.  The
     * remaining target code selects an attachment mode and places the emitter.
     */
    if (node->resource == NULL) {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x20);
        }
        return FALSE;
    }

    sequence = node->sequence;
    owner = sequence->owner;
    model = owner->model;

    switch ((u32) node->positionType) {
    case 0: selector = 1; break;
    case 1: selector = 2; break;
    case 2: selector = 3; break;
    case 3: selector = 4; break;
    case 4: selector = 5; break;
    case 5: selector = 6; break;
    case 6: selector = 7; break;
    default: selector = 0; break;
    }

    if ((node->flags & 1) != 0 && node->linkedEntryKey > 0) {
        linked = fn_801DCDA8(sequence, node->linkedEntryKey);
        if (linked != NULL && linked->startTime <= node->startTime &&
            linked->kind == 2 && linked->model != NULL)
        {
            model = linked->model;
            GSmodelForceAnimTransformUpdate(model);
        }
    }

    GSmodelGetPosition(model, position);
    GSmodelGetRotation(model, rotation);
    fn_801D9950(sequence, scale, owner->scale_selector);

    *(void**)((u8*) node + 0x8C) =
        fn_801190DC((u8*) node->resource, node->field_80,
                    (u32) node->animationMode & 1);
    *(u32*)((u8*) node + 0x90) = 0;

    if (*(void**)((u8*) node + 0x8C) == NULL) {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x20);
        }
        return FALSE;
    }

    flags = (u32) node->animationMode;
    if ((flags & 0x4) != 0) {
        fn_80118D3C(*(void**)((u8*) node + 0x8C), 1, (flags >> 4) & 1);
    }
    if ((flags & 0x8) != 0) {
        fn_80118D60(*(void**)((u8*) node + 0x8C), 1);
    }
    if ((flags & 0x80) != 0) {
        fn_80118D18(*(void**)((u8*) node + 0x8C), 1);
    }
    if ((flags & 0x800) != 0) {
        fn_80118CD0(*(void**)((u8*) node + 0x8C), 1);
    }
    fn_80118CF4(*(void**)((u8*) node + 0x8C),
                (flags >> 8) & 1,
                (flags >> 10) & 1);
    fn_80118F7C(*(void**)((u8*) node + 0x8C), position);
    *(u32*)((u8*) node + 0x90) = 0;

    if ((node->flags & 4) != 0) {
        return TRUE;
    }

    part = fn_801D97F0(node);
    if (part == NULL) {
        return FALSE;
    }

    if (selector != 0) {
        fn_80118FB0(*(void**)((u8*) node + 0x8C), part,
                    (node->flags >> 1) & 1, selector, 1,
                    ((u32) node->animationMode >> 1) & 1);
        *(u32*)((u8*) node + 0x90) = 1;
    }

    if ((node->flags & 0x10) != 0) {
        set__5GSvecFfff(position, 0.0f, 0.0f, 0.0f);
        fn_80118F04(*(void**)((u8*) node + 0x8C), position, 0, 0, 0);
        fn_80118E8C(*(void**)((u8*) node + 0x8C), position, 0, 0, 0);
    } else {
        GSpartGetTransform(part, NULL, NULL, position);
        set__5GSvecFfff(scale, 1.0f, 1.0f, 1.0f);
        fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                    (flags >> 5) & 1, (flags >> 9) & 1);
        fn_80118F04(*(void**)((u8*) node + 0x8C), position, 0, 0, 0);
    }

    GSpartFree(part);
    return TRUE;
}

/**
 * _wazaSequenceModelEntryStart / _wazaSequenceModelEntryStart - Model entry init.
 * Address: 0x801D91EC | Size: 0x604
 * Proposed name from symbols: _wazaSequenceModelEntryStart.
 * Initializes a 3D model effect for a move animation.
 */
u8 _wazaSequenceModelEntryStart(void* entry) {
    WazaSequenceNode* node = entry;
    WazaSequence* sequence = node->sequence;
    WazaEffect* owner = sequence->owner;
    void* ownerModel = owner->model;
    void* model = node->model;
    void* sourceModel = ownerModel;
    WazaSequenceNode* linked;
    GSpart* part;
    f32 position[3];
    f32 rotation[3];
    f32 scale[3];
    f32 temp[3];
    u32 flags9c;
    s32 selector;

    extern void GSmodelGetPosition(void* model, void* out);
    extern void GSmodelGetRotation(void* model, void* out);
    extern void GSmodelSetPosition(void* model, void* position);
    extern void GSmodelSetRotation(void* model, void* rotation);
    extern void GSmodelSetScale(void* model, void* scale);
    extern void* GSmodelGetBound(void* model);
    extern void GSlerpGetLinearInterpolationVector(void* dst, void* src,
                                                   void* target, f32 t);
    extern void GSvecAdd(void* dst, void* lhs, void* rhs);
    extern void fn_800E00E0(void* dst, void* src);
    extern void GSmodelAttachToGSpart(void* model, void* part, s32 arg2,
                                      s32 arg3, s32 arg4);
    extern void GSmodelSetVisibility(void* model, u8 visible);
    extern void fn_800E3CC8(void* model, u8 enable);
    extern void GSmodelSet60fpsAnimFlag(void* model, u8 enable);
    extern u8 GSmodelCanAnimate(void* model);
    extern void GSmodelSetAnimIndex(void* model, s32 index);
    extern void GSmodelSetAnimType(void* model, s32 type);
    extern void GSmodelSetAnimRate(void* model, f32 rate);
    extern void GSmodelSetAnimFrame(void* model, f32 frame);
    extern void GSmodelStartAnimation(void* model);
    extern u8 GSmodelCanTexAnimate(void* model);
    extern void GSmodelSetTexAnimIndex(void* model, s32 index);
    extern void GSmodelSetTexAnimType(void* model, s32 type);
    extern void GSmodelSetTexAnimRate(void* model, f32 rate);
    extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
    extern void GSmodelStartTexAnimation(void* model);
    extern void GSmodelSetShadowFlags(void* model, u32 flags);
    extern void GSmodelSetBoundCheck(void* model, u8 enable);
    extern void fn_800E3B44(void* model, u8 enable);
    extern const char lbl_80279694[];
    extern const char lbl_802796CC[];
    extern f32 lbl_8047E348;
    extern f32 lbl_8047E34C;
    extern f32 lbl_8047E350;

    if (model == NULL) {
        return FALSE;
    }

    if ((u32)node->positionType <= 7) {
        selector = node->positionType;
    } else {
        selector = 0;
    }

    if ((node->flags & 1) != 0 && node->linkedEntryKey > 0) {
        linked = fn_801DCDA8(sequence, node->linkedEntryKey);
        if (linked != NULL && linked->startTime <= node->startTime &&
            linked->kind == 2 && linked->model != NULL)
        {
            sourceModel = linked->model;
            GSmodelForceAnimTransformUpdate(sourceModel);
        }
    }

    GSmodelGetPosition(sourceModel, position);
    GSmodelGetRotation(sourceModel, rotation);
    fn_801D9950(sequence, scale, owner->scale_selector);
    node->attached = 0;
    flags9c = *(u32*)((u8*)node + 0x9C);

    if ((node->flags & 4) != 0) {
        if ((node->flags & 8) != 0) {
            battleGridGetNormalisedScale(scale);
            if ((node->flags & 0x10) != 0) {
                scale[1] = lbl_8047E34C;
            }
            GSmodelSetScale(model, scale);
        }
    } else if (*(s32*)((u8*)node + 0x94) != 0) {
        part = GSmodelGetPart(model, *(s32*)((u8*)node + 0x98));
        GSmodelSetPosition(model, position);
        GSmodelSetRotation(model, rotation);

        if (selector != 7 && selector != 3 && selector != 5 && selector != 6)
        {
            GSmodelSetScale(model, scale);
        }

        if ((flags9c & 0x10) != 0) {
            if (part != NULL && selector != 0) {
                void* bound = GSmodelGetBound(ownerModel);

                GSlerpGetLinearInterpolationVector(
                    temp, (u8*)bound + 0x10, (u8*)bound + 0x1C, lbl_8047E348);
                GSvecAdd(position, position, temp);
                GSmodelSetPosition(model, position);
                fn_800E00E0(temp, temp);
                GSmodelAddNull(ownerModel, (GSvec*)temp, NULL, NULL);
                GSmodelAttachToGSpart(ownerModel, part, selector, 0, 1);
                node->attached = 1;
            }
            GSpartFree(part);
        } else {
            if (part != NULL && selector != 0) {
                GSmodelAttachToGSpart(ownerModel, part, selector, 0, 1);
                node->attached = 1;
            }
            GSpartFree(part);

            switch (selector) {
            case 0:
            case 3:
                GSmodelSetRotation(ownerModel, rotation);
                GSmodelSetPosition(ownerModel, position);
                break;
            case 1:
            case 6:
                GSmodelSetRotation(ownerModel, rotation);
                break;
            case 2:
            case 5:
                GSmodelSetPosition(ownerModel, position);
                break;
            case 4:
            case 7:
                break;
            default:
                if (fn_800057A8() == 2) {
                    fn_801D744C(1);
                }
                GSlogWrite(lbl_80279694);
                break;
            }
        }
    } else if (selector != 0) {
        part = fn_801D97F0(node);
        if (part != NULL) {
            GSmodelAttachToGSpart(model, part, selector,
                                  (node->flags >> 1) & 1, 1);
            node->attached = 1;

            switch (selector) {
            case 1:
                GSmodelSetScale(model, scale);
                GSmodelSetRotation(model, rotation);
                break;
            case 2:
                GSmodelSetScale(model, scale);
                GSmodelSetPosition(model, position);
                break;
            case 3:
                GSmodelSetPosition(model, position);
                GSmodelSetRotation(model, rotation);
                break;
            case 4:
                GSmodelSetScale(model, scale);
                break;
            case 5:
                GSmodelSetPosition(model, position);
                break;
            case 6:
                GSmodelSetRotation(model, rotation);
                break;
            case 7:
                break;
            default:
                if (fn_800057A8() == 2) {
                    fn_801D744C(1);
                }
                GSlogWrite(lbl_802796CC);
                break;
            }
            GSpartFree(part);
        }
    }

    GSmodelSetVisibility(model, 1);
    fn_800E3CC8(model, flags9c & 1);
    GSmodelSet60fpsAnimFlag(model, (flags9c >> 2) & 1);

    if (*(s32*)((u8*)node + 0x88) >= 0) {
        if (GSmodelCanAnimate(model)) {
            GSmodelSetAnimIndex(model, *(s32*)((u8*)node + 0x88));
            GSmodelSetAnimType(model, *(s32*)((u8*)node + 0x84));
            GSmodelSetAnimRate(model, lbl_8047E348);
            GSmodelSetAnimFrame(model, lbl_8047E350);
            GSmodelStartAnimation(model);
        } else {
            if (fn_800057A8() == 2) {
                fn_801D744C(0x200);
            }
            *(s32*)((u8*)node + 0x88) = -1;
        }
    }

    if (*(s32*)((u8*)node + 0x88) != *(s32*)((u8*)node + 0x90) &&
        *(s32*)((u8*)node + 0x90) >= 0)
    {
        if (GSmodelCanTexAnimate(model)) {
            GSmodelSetTexAnimIndex(model, *(s32*)((u8*)node + 0x90));
            GSmodelSetTexAnimType(model, *(s32*)((u8*)node + 0x8C));
            GSmodelSetTexAnimRate(model, lbl_8047E348);
            GSmodelSetTexAnimFrame(model, lbl_8047E350);
            GSmodelStartTexAnimation(model);
        } else {
            if (fn_800057A8() == 2) {
                fn_801D744C(0x400);
            }
            *(s32*)((u8*)node + 0x90) = -1;
        }
    }

    if ((flags9c & 0x20) != 0 &&
        wazaSequenceSysGetModelShadowLight__Fv() != 0 &&
        wazaSequenceSysGetModelShadowCount__Fv() != 0)
    {
        void* shadowLight = (void*)wazaSequenceSysGetModelShadowLight__Fv();
        void* shadowList = wazaSequenceSysGetModelShadowList__Fv();
        s32 shadowCount = wazaSequenceSysGetModelShadowCount__Fv();

        GSmodelSetShadowFlags(model, 1);
        GSmodelSetShadowLight(model, shadowLight);
        GSmodelSetShadowSurface(model, shadowCount, shadowList);
        GSmodelSetBoundCheck(model, 1);
        fn_800E3B44(model, 1);
    }

    return TRUE;
}

/**
 * fn_801D97F0 - Waza entry camera movement init.
 * Address: 0x801D97F0 | Size: 0x160
 */
void* fn_801D97F0(void* entry) {
    WazaSequenceNode* node = entry;
    WazaSequenceNode* linked;

    if ((node->flags & 1) != 0) {
        if (node->linkedEntryKey <= 0) {
            return NULL;
        }
        linked = fn_801DCDA8(node->sequence, node->linkedEntryKey);
        if (linked->kind == 2 && linked->model != NULL) {
            return GSmodelGetPart(linked->model, node->partIndex);
        }
        return NULL;
    }

    /*
     * Unlinked entries continue by resolving a part on the owning battler's
     * model in the remainder of the target routine.
     */
    return NULL;
}

/**
 * fn_801D9950 / wazaSequencePokemonMotionStart - Pokemon motion during move.
 * Address: 0x801D9950 | Size: 0x2CC
 * Proposed name from symbols: wazaSequencePokemonMotionStart.
 * Controls the Pokemon's physical movement during an attack animation
 * (e.g., lunging forward for Tackle, jumping for Bounce).
 */
void fn_801D9950(void* owner, f32* scale, s32 selector) {
    f32 value;

    switch (selector) {
    case -2:
        value = 0.5f;
        break;
    case -1:
        value = 0.75f;
        break;
    case 1:
        value = 1.33329999f;
        break;
    case 2:
        value = 2.0f;
        break;
    case 3:
        value = 3.25f;
        break;
    default:
        value = 1.0f;
        break;
    }
    set__5GSvecFfff(scale, value, value, value);
}

/**
 * wazaSequencePokemonMotionStart - Pokemon motion update.
 * Address: 0x801D9C1C | Size: 0x200
 */
u8 wazaSequencePokemonMotionStart(void* ownerPtr, BOOL enabled) {
    extern void fn_800E3CC8(void* model);
    extern void GSmodelLinkTexAnimToAnim(void* model, s32 enable);
    extern void GSmodelSetAnimIndex(void* model, s32 index);
    extern void GSmodelSetAnimType(void* model, s32 type);
    extern void GSmodelSetAnimRate(void* model, f32 rate);
    extern void GSmodelGetFrameCount(void* model, f32* frameCount,
                                     f32* texFrameCount);
    extern void GSmodelSetAnimFrame(void* model, f32 frame);
    extern void GSmodelStartAnimation(void* model);
    extern void fn_801DF070(void* owner, s32 arg1, s32 arg2);
    extern const char lbl_80279740[];
    extern f32 lbl_8047E348;
    extern f64 lbl_8047E380;
    u8* owner = ownerPtr;
    void* model = *(void**)(owner + 0x24);
    u8* table = *(u8**)(owner + 0x2C) + *(u16*)(owner + 0x32) * 0xD4 + 0x8C;
    s32 duration;
    s32 frameCountInt;
    s32 quotient;
    s32 remainder;
    f32 frameCount;
    f32 texFrameCount;

    if (*(u8*)(owner + 0x16) != 0) {
        return FALSE;
    }
    if (model == NULL) {
        if (fn_800057A8() == 2) {
            fn_801D744C(4);
        }
        GSlogWrite(lbl_80279740);
        return FALSE;
    }

    fn_800E3CC8(model);
    if ((*(u8*)(owner + 0x18) & 4) == 0) {
        *(u8*)(owner + 0x19) = 0;
        GSmodelLinkTexAnimToAnim(model, 1);
    }

    duration = *(s32*)(table + 0x04);
    if (duration == 0) {
        *(u16*)(owner + 0x34) = 1;
        if (*(s32*)(table + 0x08) == 0) {
            fn_801DF070(owner, *(s32*)(table + 0x0C), 0);
        } else {
            fn_801DEF0C(owner, 0, 0);
        }
        return TRUE;
    }

    *(u16*)(owner + 0x34) = 0;
    duration = fn_801DF160(owner);
    GSmodelSetAnimIndex(model, duration);
    GSmodelSetAnimType(model, 0);
    GSmodelSetAnimRate(model, lbl_8047E348);
    GSmodelGetFrameCount(model, &frameCount, &texFrameCount);

    frameCountInt = (s32)frameCount;
    quotient = (s32)(*(s32*)(table + 0x04) >> 1) / frameCountInt;
    *(s32*)(table + 0x04) = quotient;
    remainder = (s32)(*(s32*)(table + 0x04) >> 1) - quotient * frameCountInt;
    if (remainder == 0 && *(s32*)(table + 0x04) != 0) {
        *(s32*)(table + 0x04) -= 1;
        remainder = frameCountInt;
    }

    GSmodelSetAnimFrame(model, frameCount - (f32)remainder);
    GSmodelStartAnimation(model);
    return TRUE;
}
