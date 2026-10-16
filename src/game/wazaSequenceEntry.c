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

#if !defined(WAZA_SEQUENCE_ENTRY_UPDATE_START_ONLY) && \
    !defined(WAZA_SEQUENCE_EFFECT_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_PARTICLE_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_MODEL_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_PARTICLE_TO_SCALE_ONLY) && \
    !defined(WAZA_SEQUENCE_FN_801D97F0_ONLY) && \
    !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)

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
    extern void* GSeffectGetData(u32 arg);
    /* RULE-EXCEPTION(title-path): shared log/vector/scalar pool stand-ins;
     * see docs/RULE_EXCEPTIONS.md. */
    extern const char lbl_80279588[];
    extern u8 lbl_803725B0[];
    extern u8 lbl_803725BC[];
    extern f32 lbl_8047E348;
    extern s32 lbl_8047B408;
    u8* node = entry;
    void* ownerModel;

    if (*(s32*)(node + 0x6C) == 2) {
        return TRUE;
    }
    if (!(u8)immediate && *(s32*)(node + 0x6C) != 1) {
        return FALSE;
    }

    ownerModel = *(void**)(*(u8**)(*(u8**)(node + 0xB0) + 0x3C) + 0x24);
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
                GSvecSub(ownerPosition, ownerPosition, position);
                if (*(s32*)(node + 0xA0) != 0) {
                    GSmodelDetachFromGSpart(ownerModel, 0);
                }
                GSmodelSetPosition(ownerModel, position);
                GSmodelSetRotation(ownerModel, rotation);
                GSmodelSetScale(ownerModel, lbl_803725BC);
                if ((*(s32*)(node + 0x9C) & 0x10) == 0x10) {
                    void* bound = GSmodelGetBound(ownerModel);

                    GSlerpGetLinearInterpolationVector(
                        boundCenter, (u8*)bound + 0x10, (u8*)bound + 0x1C,
                        lbl_8047E348);
                    GSvecSub(position, position, boundCenter);
                    GSmodelSetPosition(ownerModel, position);
                    GSmodelRemoveNull(ownerModel);
                }
                if ((*(s32*)(node + 0x9C) & 8) == 8) {
                    GSmodelAddNull(ownerModel, (GSvec*)ownerPosition, NULL, NULL);
                    GSvecCopy(*(u8**)(*(u8**)(node + 0xB0) + 0x3C) + 0x5C,
                              ownerPosition);
                }
            } else {
                if (*(s32*)(node + 0xA0) != 0) {
                    GSmodelDetachFromGSpart(model, 0);
                    if ((*(s32*)(node + 0x1C) & 1) != 1 &&
                        *(s32*)(node + 0x20) == 0x10) {
                        modelRemoveCenterNull(ownerModel);
                    }
                }
            }

            GSmodelSetPosition(model, lbl_803725B0);
            GSmodelSetRotation(model, lbl_803725B0);
            GSmodelSetScale(model, lbl_803725BC);
        }
        break;
    }

    case 3:
        if (*(s32*)(node + 0x90) != 0 &&
            (*(s32*)(node + 0x1C) & 1) != 1 &&
            *(s32*)(node + 0x20) == 0x10) {
            modelRemoveCenterNull(ownerModel);
        }
        break;

    case 4:
        if (*(s32*)(node + 0x78) != 0) {
            switch (*(s32*)(node + 0x88)) {
            case 6: {
                u8* effect = GSeffectGetData(*(u32*)(node + 0x78));

                if (*(s32*)(effect + 0xA8) != 0) {
                    f32 position[3];

                    GSmodelGetPosition(ownerModel, position);
                    GSvecSub(position, position, effect + 0x48);
                    GSmodelSetPosition(ownerModel, position);
                }
                break;
            }
            case 0: {
                u8* effect = GSeffectGetData(*(u32*)(node + 0x78));

                if (*(u8*)(effect + 0x4D) != 0) {
                    GSmodelSetModulationColor(ownerModel, effect + 0x44);
                }
                break;
            }
            }
        }
        GSeffectStop(*(u32*)(node + 0x78));
        break;

    case 5:
        if ((*(s32*)(node + 0x7C) & 1) == 1) {
            lbl_8047B408 = 0;
        }
        soundKill(*(u32*)(node + 0x78));
        break;

    default:
        GSlogWrite(lbl_80279588);
        *(s32*)(node + 0x6C) = -1;
        return FALSE;
    }

    *(s32*)(node + 0x6C) = 2;
    return TRUE;
}

#endif

#if !defined(WAZA_SEQUENCE_ENTRY_STOP_ONLY)
#if !defined(WAZA_SEQUENCE_EFFECT_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_PARTICLE_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_MODEL_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_PARTICLE_TO_SCALE_ONLY) && \
    !defined(WAZA_SEQUENCE_FN_801D97F0_ONLY) && \
    !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)

/* RULE-EXCEPTION(title-path): preserve timing-address temporaries;
 * see docs/RULE_EXCEPTIONS.md. */
#pragma push
#pragma opt_dead_assignments off

/* RULE-EXCEPTION(title-path): single-use sound-status inline helper;
 * see docs/RULE_EXCEPTIONS.md. */
static inline u8 wazaEntrySoundRunning(u32 id) {
    extern s32 soundGetStatus(u32 id);
    s32 status = soundGetStatus(id);
    return status == 2 || status == 3;
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
    extern BOOL GSeffectIsRunning(u32 effectId);
    extern void* GSeffectGetData(u32 arg);
    extern u32 filterIsHolding(void* ptr);
    /* RULE-EXCEPTION(title-path): shared log-pool stand-in, nodeadstore
     * compilation, and kind reused for texture comparison allocation;
     * see docs/RULE_EXCEPTIONS.md. */
    extern const char lbl_802795B4[];
    extern s32 lbl_8047B408;
    u8 done;
    u8* node = entry;
    u32 kind;

    *(s32*)(node + 0x74) += elapsed;
    kind = *(u32*)(node + 0x04);

    switch (kind) {
    case 2: {
        void* model = *(void**)(node + 0xA4);

        if (model == NULL) {
            break;
        }

        if (*(s32*)(node + 0x88) < 0) {
            done = TRUE;
        } else {
            switch (*(s32*)(node + 0x84)) {
            case 1: {
                WazaSequenceNode* timing =
                    (WazaSequenceNode*)(node + *(s32*)(node + 0x14) * 4);
                s32 time = *(s32*)(node + 0x70) + timing->positionType;
                done = time >= *(s32*)*(u8**)(node + 0xB0);
                break;
            }
            case 0:
            default:
                done = GSmodelHasAnimationEnded(model);
                break;
            }
        }

        if (*(s32*)(node + 0x88) != (s32)(kind = *(u32*)(node + 0x90)) &&
            (s32)kind >= 0) {
            u8 texDone;

            switch (*(s32*)(node + 0x8C)) {
            case 1: {
                WazaSequenceNode* timing =
                    (WazaSequenceNode*)(node + *(s32*)(node + 0x14) * 4);
                s32 time = *(s32*)(node + 0x70) + timing->positionType;
                texDone = time >= *(s32*)*(u8**)(node + 0xB0);
                break;
            }
            case 0:
            default:
                texDone = GSmodelHasTexAnimationEnded(*(void**)(node + 0xA4));
                break;
            }

            done = done && texDone;
        }

        if (!done) {
            break;
        }
        if ((*(u32*)(node + 0x9C) & 2) != 0) {
            *(s32*)(node + 0x6C) = 3;
            return TRUE;
        }
        return FALSE;
    }

    case 5:
        if ((*(s32*)(node + 0x7C) & 1) == 1) {
            if ((u32)lbl_8047B408 != 0) {
                return GSthreadIsRunning(lbl_8047B408);
            }
            return FALSE;
        } else {
            return wazaEntrySoundRunning(*(u32*)(node + 0x78));
        }

    case 4:
        if (*(s32*)(node + 0x78) == 0) {
            break;
        } else {
            u32 effectId = *(u32*)(node + 0x78);

            if ((u8)GSeffectIsRunning(effectId)) {
                switch (*(s32*)(node + 0x88)) {
                default:
                    break;
                case 0:
                    if ((u8)filterIsHolding(GSeffectGetData(effectId))) {
                        *(s32*)(node + 0x6C) = 3;
                    }
                    break;
                }
                return TRUE;
            }
            return FALSE;
        }

    case 3:
        if (*(void**)(node + 0x8C) == NULL) {
            break;
        }
        if ((u32)fn_80118DA8(*(void**)(node + 0x8C)) != 0 ||
            fn_80118D84(*(void**)(node + 0x8C)) != 0) {
            return TRUE;
        }
        return FALSE;

    case 1:
        switch (*(s32*)(node + 0x78)) {
        case 0:
            if ((*(s32*)(node + 0x74) - *(s32*)(node + 0x70)) < *(s32*)(node + 0x7C)) {
                break;
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
        case 1:
        case 2:
        case 3:
        default:
            return FALSE;
        }
        break;

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
    return TRUE;
}

#pragma pop

/**
 * wazaSequenceEntryStart / wazaSequenceEntryStart - Start a single waza entry.
 * Address: 0x801D84F4 | Size: 0x2BC
 * Proposed name from symbols: wazaSequenceEntryStart.
 * Referenced by battle_logic.c.
 */
u8 wazaSequenceEntryStart(void* entry) {
    extern void GSthreadSetArgs(void* thread, s32 priority, ...);
    extern void std130_soundUnk_801653cc(u32 id, void* buffer, u32 size);
    extern s32 lbl_8047B408;
    /* RULE-EXCEPTION(title-path): shared log-pool stand-ins;
     * see docs/RULE_EXCEPTIONS.md. */
    extern const char lbl_80279610[];
    extern const char lbl_802795E4[];
    WazaSequenceNode* node = entry;

    if ((s32)(node->sequence->flags & 0x2000) != 0x2000 &&
        (node->sequence->owner->flags & 1) != 1 &&
        node->kind != 5 && node->kind != 6 && node->kind != 1) {
        node->runtimeState = 2;
        node->currentTime = node->startTime;
        return TRUE;
    }

    switch (node->kind) {
    case 0:
        break;
    case 1: {
        u8 valid;
        switch (node->resourceId) {
        default:
            valid = FALSE;
            break;
        case 0:
        case 1:
        case 2:
        case 3:
            valid = TRUE;
            break;
        }
        if (!valid) {
            goto failed;
        }
        break;
    }
    case 2:
        if (!_wazaSequenceModelEntryStart(node)) {
            goto failed;
        }
        break;
    case 3:
        if (!_wazaSequenceParticleEntryStart(node)) {
            goto failed;
        }
        break;
    case 4:
        if (!_wazaSequenceEffectEntryStart(node)) {
            goto failed;
        }
        break;
    case 5: {
        s32 started = FALSE;

        if ((s32)(node->runtimeFlags & 1) == 1) {
            if ((u32)lbl_8047B408 == 0) {
                lbl_8047B408 = (s32)GSthreadCreate(0x15, 0x4E20, 0x2000,
                                                    1, 1, std130_soundUnk_801653cc);
                if ((u32)lbl_8047B408 != 0) {
                    GSthreadSetArgs((void*)lbl_8047B408, 3,
                                    node->resourceId, 0, 0xFF);
                    started = TRUE;
                }
            }
        } else {
            started = soundStartRaw(node->resourceId, 0, 0);
        }
        if (!(u8)started) {
            if (fn_800057A8() == 2) {
                fn_801D744C(0x100);
            }
            goto failed;
        }
        break;
    }
    case 6: {
        WazaSequenceOwner* owner = node->sequence->owner;
        u8 valid;

        switch (node->resourceId) {
        case 0:
            fn_801DD078(owner);
            break;
        case 1:
            fn_801DD028(owner);
            break;
        case 2:
            fn_801DA4E8(owner, 0);
            break;
        case 3:
            fn_801DA4E8(owner, 1);
            break;
        case 4:
            GSmodelRemoveNull(owner->model);
            break;
        case 5:
            fn_801DCF00(owner);
            break;
        case 6:
            fn_801DCEA8(owner);
            break;
        case 7:
            fn_801DCFD8(owner);
            break;
        case 9:
            fn_801DCF84(owner);
            break;
        case 8:
            if (node->sequence == owner->currentSequence) {
                fn_801DBC30(owner->currentSequence);
            }
            break;
        default:
            valid = FALSE;
            goto control_checked;
        }
        valid = TRUE;
control_checked:
        if (!valid) {
            goto failed;
        }
        break;
    }
    default:
        GSlogWrite(lbl_802795E4);
        if (fn_800057A8() == 2) {
            fn_801D744C(1);
        }
        goto failed;
    }

    node->runtimeState = 1;
    node->currentTime = node->startTime;
    return TRUE;

failed:
    GSlogWrite(lbl_80279610);
    node->runtimeState = 2;
    return FALSE;
}
#endif

#if !defined(WAZA_SEQUENCE_ENTRY_UPDATE_START_ONLY)
#if !defined(WAZA_SEQUENCE_PARTICLE_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_MODEL_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_PARTICLE_TO_SCALE_ONLY) && \
    !defined(WAZA_SEQUENCE_FN_801D97F0_ONLY) && \
    !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)
/**
 * _wazaSequenceEffectEntryStart / wazaSequenceStartEntry - Initialize entry resources.
 * Address: 0x801D87B0 | Size: 0x388
 * Proposed name from symbols: wazaSequenceStartEntry.
 */
u8 _wazaSequenceEffectEntryStart(void* entry) {
    extern const char lbl_8027964C[];
    extern BOOL GSeffect(u32 effectId);
    extern void* GSeffectGetData(u32 arg);
    extern void* GSresGetResource(u32 group, u32 resource);
    extern u32 std093_getCurrentGroupID(void);
    extern void* floorDataBiosGetCurrentPtr(void);
    extern u32 floorReadMakeModelResID(u32 value);
    extern void* HSD_ArchiveGetPublicAddress(void* archive, const char* symbols);
    extern u8 GSmodelIsModulationEnabled(void* model);
    extern void GSmodelGetModulationColor(void* model, void* color);
    extern void GSmodelGetRotation(void* model, void* out);
    extern void GSmodelGetPosition(void* model, void* out);
    extern void GSmodelSetPosition(void* model, void* position);
    u8* node = entry;
    u8* owner;
    WazaEffectTblEntry* table;
    u32 owner0;
    u32 owner1;
    s32 effectId;
    u8* effect;
    f32 scale[3];
    f32 position[3];
    u32 group;
    s32 offset;
    u8* out;
    u32 index;
    u16 count;
    u32** list;
    u32 baseId;
    void* floor;
    void* archive;
    void* resource;

    effectId = *(u32*)(node + 0x78);
    owner = *(u8**)(*(u8**)(node + 0xB0) + 0x3C);
    owner0 = *(u32*)(owner + 0x00);
    owner1 = *(u32*)(owner + 0x04);
    table = (WazaEffectTblEntry*)*(void**)(owner + 0x2C) + *(u16*)(owner + 0x32);

    if (effectId != 0) {
        effect = GSeffectGetData((u32)effectId);
        if (effect == NULL) {
            if (fn_800057A8() == 2) {
                fn_801D744C(0x80);
            }
            return FALSE;
        }

        fn_801D9950(*(void**)(node + 0xB0), scale, *(s32*)(owner + 0x10));

        switch (*(s32*)(node + 0x88)) {
        case 0:
            switch (*(u8*)(effect + 0x4C)) {
            case 0:
                break;

            case 1: {
                floor = floorDataBiosGetCurrentPtr();
                group = std093_getCurrentGroupID();
                index = 0;
                count = 0;
                archive = GSresGetResource(std093_getCurrentGroupID(), *(u32*)((u8*)floor + 0x08));

                if (archive != NULL) {
                    list = HSD_ArchiveGetPublicAddress(archive, (char*)lbl_8027964C);

                    if (list != NULL && *list != NULL) {
                        baseId = floorReadMakeModelResID(*(u32*)((u8*)floor + 0x08));
                        out = effect;
                        offset = 0;

                        while (*(u32*)((u8*)*list + offset) != 0) {
                            resource = GSresGetResource(group, baseId | index);

                            if (resource != NULL) {
                                *(void**)(out += 4) = resource;
                                count++;
                            }
                            offset += 4;
                            index++;
                        }
                    }
                }
                *(u32*)(effect + 0x48) = (u16)count;
                break;
            }

            case 2:
                *(void**)(effect + 0x04) = *(void**)(owner + 0x24);
                *(u32*)(effect + 0x48) = 1;
                if ((u8)fn_801DCDCC(owner) && GSmodelIsModulationEnabled(*(void**)(owner + 0x24))) {
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
            *(u32*)(effect + 0x50) = table->field_4C[*(u32*)(node + 0x80)];
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
            *(u16*)(effect + 0x0E) = table->field_4C[*(u32*)(node + 0x80)];
            break;

        case 4:
            *(u16*)(effect + 0x24) = owner0;
            *(u16*)(effect + 0x26) = owner1;
            *(u16*)(effect + 0x28) = table->field_4C[*(u32*)(node + 0x80)];
            *(u16*)(effect + 0x2A) = table->field_4C[*(u32*)(node + 0x84)];
            break;

        case 5:
            *(u16*)(effect + 0x46) = owner0;
            *(u16*)(effect + 0x48) = owner1;
            *(u16*)(effect + 0x4A) = table->field_4C[*(u32*)(node + 0x80)];
            *(f32*)(effect + 0x0C) = scale[0];
            break;

        case 6:
            GSmodelGetRotation(*(void**)(owner + 0x24), effect + 0x54);
            if (*(s32*)(effect + 0xA8) != 0) {
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
            if (*(s32*)(effect + 0x10) == 0) {
                *(void**)(effect + 0x00) = *(void**)(owner + 0x24);
            }
            break;

        case 10:
            *(void**)(effect + 0x00) = *(void**)(owner + 0x24);
            break;

        case 11:
            break;

        case 12:
            *(void**)(effect + 0x08) = *(void**)(owner + 0x24);
            *(u32*)(effect + 0x0C) = *(u32*)(*(u8**)(owner + 0x2C) + 0x54);
            *(f32*)(effect + 0x14) = *(f32*)(effect + 0x18) * scale[0];
            break;

        case -1:
        default:
            if (fn_800057A8() == 2) {
                fn_801D744C(0x80);
            }
            return FALSE;
        }

        GSeffect((u32)effectId);
    } else {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x80);
        }
        return FALSE;
    }
    return TRUE;
}
#endif

#if !defined(WAZA_SEQUENCE_EFFECT_ENTRY_START_ONLY)

#if !defined(WAZA_SEQUENCE_MODEL_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_FN_801D97F0_ONLY) && \
    !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)
/**
 * _wazaSequenceParticleEntryStart / _wazaSequenceParticleEntryStart - Particle entry init.
 * Address: 0x801D8B38 | Size: 0x6B4
 * Proposed name from symbols: _wazaSequenceParticleEntryStart.
 * Large function that initializes a particle effect for a move animation.
 */
u8 _wazaSequenceParticleEntryStart(WazaSequenceNode* node) {
    void* resource;
    WazaSequenceNode* linked;
    register s32 selector;
    void* owner;
    void* part;
    f32 position[3];
    f32 rotation[3];
    f32 scale[3];
    f32 temp[3];
    f32 offset[3];
    u32 animationMode;
    u32 angleRadiusScale;
    u32 applyToGenerator;
    void* model;

    extern void GSmodelGetPosition(void* model, void* out);
    extern void GSmodelGetRotation(void* model, void* out);
    extern void GSpartGetTransform(void* part, void* pos, void* rot,
                                   void* scale);
    extern void fn_800E0108(void* dst, void* lhs, void* rhs);
    extern void GSvecAdd(void* dst, void* lhs, void* rhs);
    extern void fn_80118CF4(void* particleNode, u8 enabledA, u8 enabledB,
                           u8 applyToGenerator);
    extern const char lbl_80279658[];

    resource = node->resource;
    owner = node->sequence->owner;

    switch ((u32)node->positionType) {
    case 1: selector = 1; break;
    case 2: selector = 2; break;
    case 3: selector = 3; break;
    case 4: selector = 4; break;
    case 5: selector = 5; break;
    case 6: selector = 6; break;
    case 7: selector = 7; break;
    default: selector = 0; break;
    }

    model = *(void**)((u8*)owner + 0x24);

    if ((s32)(node->flags & 1) == 1 && node->linkedEntryKey > 0) {
        linked = fn_801DCDA8(node->sequence, node->linkedEntryKey);
        if (linked->startTime <= node->startTime &&
            linked->kind == 2 && linked->model != NULL)
        {
            model = linked->model;
            GSmodelForceAnimTransformUpdate(model);
        }
    }

    GSmodelGetPosition(model, position);
    GSmodelGetRotation(model, rotation);
    fn_801D9950(node->sequence, scale, *(s32*)((u8*)owner + 0x10));

    if (resource == NULL) {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x20);
        }
        return FALSE;
    }

    *(void**)((u8*) node + 0x8C) =
        gsParticleCreate(node->resource, node->field_80,
                    (u32) node->animationMode & 1);
    *(u32*)((u8*) node + 0x90) = 0;

    if (*(void**)((u8*) node + 0x8C) == NULL) {
        if (fn_800057A8() == 2) {
            fn_801D744C(0x20);
        }
        return FALSE;
    }

    if (((u32)node->animationMode & 0x4) != 0) {
        fn_80118D3C(*(void**)((u8*) node + 0x8C), 1,
                    ((u32)node->animationMode >> 4) & 1);
    }
    if (((u32)node->animationMode & 0x8) != 0) {
        fn_80118D60(*(void**)((u8*) node + 0x8C), 1);
    }
    if (((u32)node->animationMode & 0x80) != 0) {
        fn_80118D18(*(void**)((u8*) node + 0x8C), 1);
    }
    if (((u32)node->animationMode & 0x800) != 0) {
        fn_80118CD0(*(void**)((u8*) node + 0x8C), 1);
    }
    if (((u32)node->animationMode & 0x1000) != 0) {
        fn_80118CAC(*(void**)((u8*) node + 0x8C), 1);
    }
    animationMode = (u32)node->animationMode;
    applyToGenerator = (animationMode >> 5) & 1;
    angleRadiusScale = (u8)((animationMode >> 9) & 1);
    fn_80118CF4(*(void**)((u8*) node + 0x8C),
                (animationMode >> 8) & 1,
                (animationMode >> 10) & 1,
                (animationMode >> 5) & 1);
    fn_80118F7C(*(void**)((u8*) node + 0x8C), offset);
    *(u32*)((u8*) node + 0x90) = 0;

    if ((s32)(node->flags & 4) == 4) {
        clear__5GSvecFv(offset);
        fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
        fn_80118E8C(*(void**)((u8*) node + 0x8C), offset);

        if ((s32)(node->flags & 8) == 8) {
            battleGridGetNormalisedScale(scale);
            fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                        applyToGenerator, angleRadiusScale);
        } else {
            set__5GSvecFfff(scale, 1.0f, 1.0f, 1.0f);
            fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                        applyToGenerator, angleRadiusScale);
        }
        return TRUE;
    }

    part = fn_801D97F0(node);
    if (part != NULL) {
        if (selector != 0) {
            fn_80118FB0(*(void**)((u8*) node + 0x8C), part, selector,
                        (node->flags >> 1) & 1, 1,
                        ((u32) node->animationMode >> 1) & 1);
            *(u32*)((u8*) node + 0x90) = 1;

            GSpartGetTransform(part, NULL, NULL, temp);
            fn_800E0108(temp, temp, scale);

            switch (selector) {
            case 7:
                offset[0] *= temp[0] - 1.0f;
                offset[1] *= temp[1] - 1.0f;
                offset[2] *= temp[2] - 1.0f;
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                break;
            case 4:
                offset[0] *= scale[0] - 1.0f;
                offset[1] *= scale[1] - 1.0f;
                offset[2] *= scale[2] - 1.0f;
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                            applyToGenerator, angleRadiusScale);
                break;
            case 1:
                offset[0] *= scale[0] - 1.0f;
                offset[1] *= scale[1] - 1.0f;
                offset[2] *= scale[2] - 1.0f;
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                            applyToGenerator, angleRadiusScale);
                fn_80118E8C(*(void**)((u8*) node + 0x8C), rotation);
                break;
            case 5:
                offset[0] *= temp[0] - 1.0f;
                offset[1] *= temp[1] - 1.0f;
                offset[2] *= temp[2] - 1.0f;
                GSvecAdd(offset, offset, position);
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                break;
            case 2:
                offset[0] *= scale[0] - 1.0f;
                offset[1] *= scale[1] - 1.0f;
                offset[2] *= scale[2] - 1.0f;
                GSvecAdd(offset, offset, position);
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                            applyToGenerator, angleRadiusScale);
                break;
            case 6:
                offset[0] *= temp[0] - 1.0f;
                offset[1] *= temp[1] - 1.0f;
                offset[2] *= temp[2] - 1.0f;
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                fn_80118E8C(*(void**)((u8*) node + 0x8C), rotation);
                break;
            case 3:
                offset[0] *= temp[0] - 1.0f;
                offset[1] *= temp[1] - 1.0f;
                offset[2] *= temp[2] - 1.0f;
                GSvecAdd(offset, offset, position);
                fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
                fn_80118E8C(*(void**)((u8*) node + 0x8C), rotation);
                break;
            default:
                if (fn_800057A8() == 2) {
                    fn_801D744C(1);
                }
                GSlogWrite(lbl_80279658);
                break;
            }
        } else {
            offset[0] *= scale[0] - 1.0f;
            offset[1] *= scale[1] - 1.0f;
            offset[2] *= scale[2] - 1.0f;
            GSvecAdd(offset, position, offset);
            fn_80118F04(*(void**)((u8*) node + 0x8C), offset);
            fn_80118E8C(*(void**)((u8*) node + 0x8C), rotation);
            fn_80118DE0(*(void**)((u8*) node + 0x8C), scale,
                        applyToGenerator, angleRadiusScale);
        }

        GSpartFree(part);
    }
    return TRUE;
}
#endif

#if !defined(WAZA_SEQUENCE_PARTICLE_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)
#if !defined(WAZA_SEQUENCE_FN_801D97F0_ONLY)
/**
 * _wazaSequenceModelEntryStart / _wazaSequenceModelEntryStart - Model entry init.
 * Address: 0x801D91EC | Size: 0x604
 * Proposed name from symbols: _wazaSequenceModelEntryStart.
 * Initializes a 3D model effect for a move animation.
 */
u8 _wazaSequenceModelEntryStart(WazaSequenceNode* node) {
    s32 selector;
    WazaEffect* owner;
    void* model;
    void* ownerModel;
    void* sourceModel;
    WazaSequenceNode* linked;
    f32 scale[3];
    f32 rotation[3];
    f32 position[3];
    f32 temp[3];
    s32 positionType;
    s32 textureAnimation;

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
    extern f32 lbl_8047E350;

    positionType = node->positionType;
    owner = node->sequence->owner;
    model = node->model;

    switch ((u32)positionType) {
    case 1: selector = 1; break;
    case 2: selector = 2; break;
    case 3: selector = 3; break;
    case 4: selector = 4; break;
    case 5: selector = 5; break;
    case 6: selector = 6; break;
    case 7: selector = 7; break;
    default: selector = 0; break;
    }

    ownerModel = owner->model;
    if (model == NULL) {
        return FALSE;
    }

    sourceModel = ownerModel;
    if ((s32)(node->flags & 1) == 1 && node->linkedEntryKey > 0) {
        linked = fn_801DCDA8(node->sequence, node->linkedEntryKey);
        if (linked->startTime <= node->startTime &&
            linked->kind == 2 && linked->model != NULL)
        {
            sourceModel = linked->model;
            GSmodelForceAnimTransformUpdate(sourceModel);
        }
    }

    GSmodelGetPosition(sourceModel, position);
    GSmodelGetRotation(sourceModel, rotation);
    fn_801D9950(node->sequence, scale, owner->scale_selector);
    node->attached = 0;

    if ((s32)(node->flags & 4) == 4) {
        if ((s32)(node->flags & 8) == 8) {
            battleGridGetNormalisedScale(scale);
            if ((s32)(node->flags & 0x10) == 0x10) {
                scale[1] = 1.0f;
            }
            GSmodelSetScale(model, scale);
        }
    } else if (*(s32*)((u8*)node + 0x94) != 0) {
        GSpart* part;

        part = GSmodelGetPart(model, *(s32*)((u8*)node + 0x98));
        GSmodelSetPosition(model, position);
        GSmodelSetRotation(model, rotation);

        if (selector != 7 && selector != 3 && selector != 5 && selector != 6)
        {
            GSmodelSetScale(model, scale);
        }

        if ((s32)(*(u32*)((u8*)node + 0x9C) & 0x10) == 0x10) {
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
            case 1:
            case 6:
                GSmodelSetRotation(ownerModel, rotation);
                break;
            case 2:
            case 5:
                GSmodelSetPosition(ownerModel, position);
                break;
            case 3:
                GSmodelSetRotation(ownerModel, rotation);
                GSmodelSetPosition(ownerModel, position);
                break;
            case 0:
                GSmodelSetRotation(ownerModel, rotation);
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
        GSpart* part;

        part = fn_801D97F0(node);
        if (part != NULL) {
            GSmodelAttachToGSpart(model, part, selector,
                                  (node->flags >> 1) & 1, 1);
            node->attached = 1;

            switch (selector) {
            case 4:
                GSmodelSetScale(model, scale);
                break;
            case 1:
                GSmodelSetScale(model, scale);
                GSmodelSetRotation(model, rotation);
                break;
            case 5:
                GSmodelSetPosition(model, position);
                break;
            case 2:
                GSmodelSetScale(model, scale);
                GSmodelSetPosition(model, position);
                break;
            case 6:
                GSmodelSetRotation(model, rotation);
                break;
            case 3:
                GSmodelSetPosition(model, position);
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
    fn_800E3CC8(model, *(u32*)((u8*)node + 0x9C) & 1);
    GSmodelSet60fpsAnimFlag(
        model, (*(u32*)((u8*)node + 0x9C) >> 2) & 1);

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

    if (*(s32*)((u8*)node + 0x88) !=
            (textureAnimation = *(s32*)((u8*)node + 0x90)) &&
        textureAnimation >= 0)
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

    if ((s32)(*(u32*)((u8*)node + 0x9C) & 0x20) == 0x20 &&
        (u32)wazaSequenceSysGetModelShadowLight__Fv() != 0 &&
        wazaSequenceSysGetModelShadowCount__Fv() != 0)
    {
        GSmodelSetShadowFlags(model, 1);
        GSmodelSetShadowLight(
            model, (void*)wazaSequenceSysGetModelShadowLight__Fv());
        GSmodelSetShadowSurface(
            model, wazaSequenceSysGetModelShadowCount__Fv(),
            wazaSequenceSysGetModelShadowList__Fv());
        GSmodelSetBoundCheck(model, 1);
        fn_800E3B44(model, 1);
    }

    return TRUE;
}
#endif
#endif

#if !defined(WAZA_SEQUENCE_PARTICLE_ENTRY_START_ONLY) && \
    !defined(WAZA_SEQUENCE_MODEL_ENTRY_START_ONLY)
#if !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)
#if defined(WAZA_SEQUENCE_PARTICLE_TO_SCALE_ONLY)
/* RULE-EXCEPTION(title-path): preserve fn_801D97F0's original prologue mode;
 * see docs/RULE_EXCEPTIONS.md. */
#endif
/**
 * fn_801D97F0 - Waza entry camera movement init.
 * Address: 0x801D97F0 | Size: 0x160
 */
void* fn_801D97F0(void* entry) {
    WazaSequenceNode* linked;
    WazaSequenceNode* node = entry;
    void* part = NULL;
    u8* owner;
    s32 (*table)[1];
    s32 partIndex;
    s32 selector;
    s32 linkedKey;

    extern const char lbl_80279700[];

    if ((s32)(node->flags & 1) == 1) {
        linkedKey = node->linkedEntryKey;
        if (linkedKey <= 0) {
            goto failed;
        }
        linked = fn_801DCDA8(node->sequence, linkedKey);
        if (linked->startTime > node->startTime) {
            GSlogWrite(lbl_80279700);
        }
        if (linked->kind == 2) {
            if (linked->model != NULL) {
                part = GSmodelGetPart(linked->model, node->partIndex);
                if (fn_800057A8() == 2 && part == NULL) {
                    fn_801D744C(2);
                }
                return part;
            }
            goto failed;
        }
        return NULL;
    }

    selector = *(s32*)((u8*)node + 0x20);
    owner = (u8*)node->sequence->owner;
    table = (s32 (*)[1])(*(u8**)(owner + 0x2C) +
                         *(u16*)(owner + 0x32) * 0xD4);
    if (selector == 0x10) {
        if (GSmodelCenterNull(*(void**)(owner + 0x24))) {
            partIndex = fn_800EE0E8(*(void**)(owner + 0x24)) - 1;
        } else {
            partIndex = *(s32*)((u8*)table + 0x54);
        }
    } else {
        partIndex = table[selector][0x13];
    }
    if (partIndex >= 0) {
        part = GSmodelGetPart(*(void**)(owner + 0x24), partIndex);
    }
    if (fn_800057A8() == 2 && part == NULL) {
        fn_801D744C(2);
    }

failed:
    return part;
}
#if defined(WAZA_SEQUENCE_PARTICLE_TO_SCALE_ONLY)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (use_lmw_stmw on) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#endif
#endif

#if !defined(WAZA_SEQUENCE_FN_801D97F0_ONLY)
#if !defined(WAZA_SEQUENCE_POKEMON_MOTION_START_ONLY)
/**
 * fn_801D9950 / wazaSequencePokemonMotionStart - Pokemon motion during move.
 * Address: 0x801D9950 | Size: 0x2CC
 * Proposed name from symbols: wazaSequencePokemonMotionStart.
 * Controls the Pokemon's physical movement during an attack animation
 * (e.g., lunging forward for Tackle, jumping for Bounce).
 */
void fn_801D9950(void* owner, f32* scale, s32 selector) {
    typedef struct WazaSequenceScaleCtx {
        u8 pad_00[0x2C];
        u16 field_2C;
        u16 field_2E;
    } WazaSequenceScaleCtx;
    extern void vecScale(void* dst, void* src, f32 scaleValue);
    extern f32 lbl_8047E348;
    extern const f32 lbl_8047E354;
    extern const f32 lbl_8047E358;
    extern const f32 lbl_8047E35C;
    extern const f32 lbl_8047E360;
    extern const f32 lbl_8047E364;
    extern const f32 lbl_8047E368;
    extern const f32 lbl_8047E36C;
    extern const f32 lbl_8047E370;
    extern const f32 lbl_8047E374;
    extern const f32 lbl_8047E378;
    extern const f32 lbl_8047E37C;
    WazaSequenceScaleCtx* ctx = owner;
    u16 variant;

    switch (selector) {
    case -2:
        set__5GSvecFfff(scale, lbl_8047E348, lbl_8047E348, lbl_8047E348);
        break;
    case -1:
        set__5GSvecFfff(scale, lbl_8047E354, lbl_8047E354, lbl_8047E354);
        break;
    case 1:
        set__5GSvecFfff(scale, lbl_8047E358, lbl_8047E358, lbl_8047E358);
        break;
    case 2:
        set__5GSvecFfff(scale, lbl_8047E35C, lbl_8047E35C, lbl_8047E35C);
        break;
    case 3:
        set__5GSvecFfff(scale, lbl_8047E360, lbl_8047E360, lbl_8047E360);
        break;
    default:
        set__5GSvecFfff(scale, 1.0f, 1.0f, 1.0f);
        break;
    }

    switch (ctx->field_2C) {
    case 0x8F:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E364);
        }
        return;
    case 0xB1:
        vecScale(scale, scale, lbl_8047E348);
        return;
    case 0xC4:
        variant = ctx->field_2E;
        if (variant == 1) {
            vecScale(scale, scale, lbl_8047E368);
            return;
        }
        if (variant == 2) {
            vecScale(scale, scale, lbl_8047E348);
            return;
        }
        break;
    case 0xFA:
        variant = ctx->field_2E;
        if (variant == 1) {
            vecScale(scale, scale, lbl_8047E36C);
            return;
        }
        if (variant == 2) {
            vecScale(scale, scale, lbl_8047E370);
            return;
        }
        break;
    case 0x119:
        vecScale(scale, scale, lbl_8047E374);
        return;
    case 0x133:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E374);
            return;
        }
        break;
    case 0x13B:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E374);
            return;
        }
        break;
    case 0x143:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E378);
            return;
        }
        break;
    case 0x149:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E37C);
            return;
        }
        break;
    case 0x162:
        if (ctx->field_2E == 2) {
            vecScale(scale, scale, lbl_8047E37C);
        }
        break;
    }
}
#endif

#if !defined(WAZA_SEQUENCE_PARTICLE_TO_SCALE_ONLY)
/**
 * wazaSequencePokemonMotionStart - Pokemon motion update.
 * Address: 0x801D9C1C | Size: 0x200
 */
u8 wazaSequencePokemonMotionStart(void* ownerPtr, BOOL enabled) {
    extern void fn_800E3CC8(void* model, s32 enable);
    extern void GSmodelLinkTexAnimToAnim(void* model, s32 enable);
    extern void GSmodelSetAnimIndex(void* model, s32 index);
    extern void GSmodelSetAnimType(void* model, s32 type);
    extern void GSmodelSetAnimRate(void* model, f32 rate);
    extern void GSmodelGetFrameCount(void* model, f32* frameCount,
                                     f32* texFrameCount);
    extern void GSmodelSetAnimFrame(void* model, f32 frame);
    extern void GSmodelStartAnimation(void* model);
    extern void fn_801DEE14(void* owner);
    extern void fn_801DF070(void* owner, s32 arg1, s32 arg2);
    extern const char lbl_80279740[];
    extern f32 lbl_8047E348;
    extern f64 lbl_8047E380;
    u8* owner = ownerPtr;
    void* model = *(void**)(owner + 0x24);
    u8* table;
    s32 duration;
    s32 animIndex;
    s32 frameCountInt;
    s32 halfDuration;
    f32 frameCount;
    f32 texFrameCount;

    if (*(u8*)(owner + 0x16) != 0) {
        return FALSE;
    }
    if (model != NULL) {
        fn_800E3CC8(model, enabled);
        table = *(u8**)(owner + 0x2C) +
                *(u16*)(owner + 0x32) * 0xD4 + 0x8C;
        if ((*(u8*)(owner + 0x18) & 8) == 8) {
            return TRUE;
        }

        duration = *(s32*)(table + 0x04);
        if (duration == 0) {
            if (*(u16*)(owner + 0x32) == 10) {
                fn_801DEE14(owner);
                return TRUE;
            }
            *(u16*)(owner + 0x34) = 1;
            if (*(s32*)(table + 0x08) == 0) {
                fn_801DF070(owner, *(s32*)(table + 0x0C), 0);
            } else {
                fn_801DEF0C(owner, 0, 0);
            }
            goto done;
        }

        *(u16*)(owner + 0x34) = 0;
        animIndex = fn_801DF160(owner);
        if ((*(u8*)(owner + 0x18) & 4) != 4) {
            *(u8*)(owner + 0x19) = 0;
            GSmodelLinkTexAnimToAnim(model, 1);
        }
        GSmodelSetAnimIndex(model, animIndex);
        GSmodelSetAnimType(model, 0);
        GSmodelSetAnimRate(model, lbl_8047E348);
        GSmodelGetFrameCount(model, &frameCount, &texFrameCount);

        halfDuration = duration >> 1;
        *(s32*)(table + 0x04) = halfDuration / (s32)frameCount;
        frameCountInt = (s32)frameCount;
        halfDuration %= frameCountInt;
        if (halfDuration == 0 && *(s32*)(table + 0x04) != 0) {
            halfDuration = frameCountInt;
            *(s32*)(table + 0x04) -= 1;
        }

        GSmodelSetAnimFrame(model, frameCount - (f32)halfDuration);
        GSmodelStartAnimation(model);
done:
        return TRUE;
    }
    if (fn_800057A8() == 2) {
        fn_801D744C(4);
    }
    GSlogWrite(lbl_80279740);
    return FALSE;
}
#endif

#endif
#endif
#endif
#endif
#endif
