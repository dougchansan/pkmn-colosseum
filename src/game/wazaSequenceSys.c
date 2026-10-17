/**
 * @file wazaSequenceSys.c
 * @brief Matching waza motion and effect helpers.
 *
 * Address range: 0x801D9E1C - 0x801DAC90
 * Shared typedefs and cross-TU forward declarations live in
 * include/game/battle/battle_waza_types.h.
 */

#include "game/battle/battle_waza_types.h"


/**
 * wazaSequenceOwnerGetFlag77 - Pokemon motion get complete. Returns obj->0x77 or 0 if NULL.
 * Address: 0x801D9E1C | Size: 0x18
 */
u8 wazaSequenceOwnerGetFlag77(void* obj) {
    if (obj != NULL) {
        return *(u8*)((u8*)obj + 0x77);
    }
    return 0;
}

/**
 * fn_801D9E34 - Pokemon motion cancel.
 * Address: 0x801D9E34 | Size: 0x58
 */
void fn_801D9E34(void* obj) {
    extern void gsParticleKillOwned(u8* resource, u32 notify);
    WazaEffect* effect = obj;

    if (*(u32*)(lbl_80467CC0 + 0xC) != 0 && effect->effect_handle != NULL) {
        gsParticleKillOwned(effect->effect_handle, 1);
        effect->effect_handle = NULL;
    }
}

/* =========================================================================
 * WAZA EFFECT HELPERS (0x801D9E8C - 0x801DAC78)
 * ========================================================================= */


#define WAZA_SDATA2_FLOAT(addr) extern const f32 lbl_##addr
#define WAZA_SDATA2_VALUE(addr) lbl_##addr
WAZA_SDATA2_FLOAT(8047E388);
WAZA_SDATA2_FLOAT(8047E38C);
WAZA_SDATA2_FLOAT(8047E390);
WAZA_SDATA2_FLOAT(8047E394);
WAZA_SDATA2_FLOAT(8047E398);
WAZA_SDATA2_FLOAT(8047E39C);
#undef WAZA_SDATA2_FLOAT


/**
 * fn_801D9E8C - Waza effect interpolation (position lerp).
 * Address: 0x801D9E8C | Size: 0x188
 */
void fn_801D9E8C(void* effect) {
    WazaEffect* fx;
    GSpart* target;
    u8* ctx;
    f32 val;
    s32 scale_selector;
    f32 scale[3];

    if (effect == NULL) return;
    fx = (WazaEffect*)effect;
    ctx = *(u8**)(lbl_80467CC0 + 0xC);
    if (ctx == 0) return;
    if (fx->effect_handle != 0) return;

    fx->effect_handle = gsParticleCreate(ctx, 0, 0);
    if (fx->effect_handle == 0) return;

    if ((fx->flags & 1) != 1) {
        fn_80118C88(fx->effect_handle, 0);
    }

    {
        s32 val_idx = *(s32*)((u8*)fx->table +
                              fx->index * sizeof(WazaEffectTblEntry) + 0x50);
        if (val_idx > 0) {
            target = GSmodelGetPart(fx->model, val_idx);
        } else {
            target = NULL;
        }
    }

    if (target == NULL) return;

    if (effect != NULL) {
        scale_selector = fx->scale_selector;
    } else {
        scale_selector = 0;
    }

    switch (scale_selector) {
    case -2: val = WAZA_SDATA2_VALUE(8047E388); break;
    case -1: val = WAZA_SDATA2_VALUE(8047E38C); break;
    case 1:  val = WAZA_SDATA2_VALUE(8047E390); break;
    case 2:  val = WAZA_SDATA2_VALUE(8047E394); break;
    case 3:  val = WAZA_SDATA2_VALUE(8047E398); break;
    default: val = WAZA_SDATA2_VALUE(8047E39C); break;
    }

    set__5GSvecFfff(scale, val, val, val);

    gsParticleLinkToJObj(fx->effect_handle, target, 4, 0, 1, 0);
    fn_80118D18(fx->effect_handle, 1);
    fn_80118DE0(fx->effect_handle, scale, 1, 0);
    GSpartFree(target);
}
#undef WAZA_SDATA2_VALUE

/**
 * fn_801DA014 - Waza effect timer cancel.
 * Address: 0x801DA014 | Size: 0x5C
 */
void fn_801DA014(void* effect) {
    extern void gsParticleKillOwned(u8* resource, u32 notify);
    WazaEffect* fx = effect;

    if (fx != NULL && *(u32*)(lbl_80467CC0 + 0x8) != 0 && fx->field_80 != NULL) {
        gsParticleKillOwned(fx->field_80, 1);
        fx->field_80 = NULL;
    }
}

/**
 * fn_801DA070 - Waza effect bezier curve eval.
 * Address: 0x801DA070 | Size: 0x1B4
 */
#define WAZA_SDATA2_VALUE(addr) lbl_##addr
void fn_801DA070(void* effect) {
    WazaEffect* fx;
    GSpart* target;
    void* entry;
    u8* ctx;
    s32 n;
    f32 val;
    s32 scale_selector;
    f32 scale[3];

    fx = (WazaEffect*)effect;
    if (fx == NULL) return;
    ctx = *(u8**)(lbl_80467CC0 + 0x8);
    if (ctx == 0) return;
    if (fx->field_80 != 0) return;

    fx->field_80 = gsParticleCreate(ctx, 0, 0);
    if (fx->field_80 == 0) return;

    if ((fx->flags & 1) != 1) {
        fn_80118C88(fx->field_80, 0);
    }

    entry = (u8*)fx->table + fx->index * sizeof(WazaEffectTblEntry);
    if ((u8)GSmodelCenterNull(fx->model) != 0) {
        n = GSmodelWalkJObjs(fx->model) - 1;
    } else {
        n = *(s32*)((u8*)entry + 0x54);
    }

    if (n > 0) {
        target = GSmodelGetPart(fx->model, n);
    } else {
        target = NULL;
    }

    if (target == NULL) return;

    if (effect != NULL) {
        scale_selector = fx->scale_selector;
    } else {
        scale_selector = 0;
    }

    switch (scale_selector) {
    case -2: val = WAZA_SDATA2_VALUE(8047E388); break;
    case -1: val = WAZA_SDATA2_VALUE(8047E38C); break;
    case 1:  val = WAZA_SDATA2_VALUE(8047E390); break;
    case 2:  val = WAZA_SDATA2_VALUE(8047E394); break;
    case 3:  val = WAZA_SDATA2_VALUE(8047E398); break;
    default: val = WAZA_SDATA2_VALUE(8047E39C); break;
    }

    set__5GSvecFfff(scale, val, val, val);

    gsParticleLinkToJObj(fx->field_80, target, 4, 0, 1, 0);
    fn_80118D18(fx->field_80, 1);
    fn_80118CAC(fx->field_80, 1);
    fn_80118DE0(fx->field_80, scale, 1, 0);
    GSpartFree(target);
}
#undef WAZA_SDATA2_VALUE

/**
 * fn_801DA224 - Apply effect state flags.
 * Address: 0x801DA224 | Size: 0xA0
 */
void fn_801DA224(void* effect, s32 flags) {
    if (effect != NULL) {
        u8 value = flags;

        if ((flags & 4) == 4) {
            wazaSequenceOwnerSetAura(effect);
        } else {
            wazaSequenceOwnerClearAura(effect);
        }

        if ((value & 8) == 8) {
            fn_801DCFD8(effect);
        } else {
            wazaSequenceOwnerClearStop(effect);
        }

        if ((value & 2) == 2) {
            wazaSequenceOwnerSetNull(effect);
        } else {
            fn_801DCEA8(effect);
        }

        *(u8*)((u8*)effect + 0x18) = flags;
    }
}

/**
 * fn_801DA2C4 - Reset an effect's trajectory and make its resources visible.
 * Address: 0x801DA2C4 | Size: 0x90
 */
void fn_801DA2C4(void* effect, f32 radius, f32 t) {
    WazaEffect* fx = effect;
    struct GSmodel* model;

    if (fx != NULL) {
        wazaSequenceOwnerClearAura(fx);
        wazaSequenceOwnerClearStop(fx);
        fn_801DCEA8(fx);

        if (fx != NULL && (model = fx->model) != NULL) {
            fx->flags |= 1;
            GSmodelSetVisibility(model, 1);
            if (fx->field_80 != 0) {
                fn_80118C88(fx->field_80, 1);
            }
            if (fx->effect_handle != 0) {
                fn_80118C88(fx->effect_handle, 1);
            }
        }
    }
}

/**
 * wazaSequenceOwnerGetFlags - Waza effect get trajectory type. Returns effect->0x18 or 0 if NULL.
 * Address: 0x801DA354 | Size: 0x18
 */
u8 wazaSequenceOwnerGetFlags(void* effect) {
    if (effect != NULL) {
        return *(u8*)((u8*)effect + 0x18);
    }
    return 0;
}

/**
 * fn_801DA36C - Waza effect set trajectory type.
 * Address: 0x801DA36C | Size: 0x60
 */
void fn_801DA36C(void* effect, s32 trajType) {
    if (effect != NULL) {
        switch ((u8)trajType) {
        case 1:
            wazaSequenceOwnerClearAura(effect);
            return;
        case 2:
            wazaSequenceOwnerClearStop(effect);
            return;
        case 3:
            fn_801DCEA8(effect);
            return;
        }
    }
}

/**
 * fn_801DA3CC - Waza effect set velocity.
 * Address: 0x801DA3CC | Size: 0x60
 */
void fn_801DA3CC(void* effect, s32 trajType) {
    if (effect != NULL) {
        switch ((u8)trajType) {
        case 1:
            wazaSequenceOwnerSetAura(effect);
            return;
        case 2:
            fn_801DCFD8(effect);
            return;
        case 3:
            wazaSequenceOwnerSetNull(effect);
            return;
        }
    }
}

/**
 * wazaSequenceOwnerIsVisible - Waza effect get bit 0 of field 0x18.
 * Address: 0x801DA42C | Size: 0x1C
 */
u32 wazaSequenceOwnerIsVisible(void* effect) {
    if (effect != NULL) {
        return *(u8*)((u8*)effect + 0x18) & 1;
    }
    return 0;
}

/**
 * fn_801DA448 - Set visibility on particle banks attached to an effect.
 * Address: 0x801DA448 | Size: 0xA0
 */
void fn_801DA448(void* effect, u32 visible) {
    extern void GSmodelSetParticleVisibility(void* bank, u32 visible);
    u8* sequence;
    u8* model;

    if (effect != NULL) {
        sequence = *(u8**)((u8*)effect + 0x68);
        while (sequence != NULL) {
            if (*(u8*)(sequence + 0x14) != 0) {
                model = *(u8**)(sequence + 0x24);
                while (model != NULL) {
                    if (*(s32*)(model + 4) == 3 && *(s32*)(model + 0x18) == 0 &&
                        *(void**)(model + 0x88) != NULL) {
                        GSmodelSetParticleVisibility(*(void**)(model + 0x88), visible);
                    }
                    model = *(u8**)(model + 0xA8);
                }
            }
            sequence = *(u8**)(sequence + 0x34);
        }
    }
}

/**
 * wazaSequenceOwnerSetVisibility - Set visibility on a waza effect and its attached effects.
 * Address: 0x801DA4E8 | Size: 0xC4
 */
void wazaSequenceOwnerSetVisibility(void* effect, u32 visible) {
    WazaEffect* fx = effect;
    struct GSmodel* model;

    if (fx != NULL && (model = fx->model) != NULL) {
        if ((u8)visible != 0) {
            fx->flags |= 1;
            GSmodelSetVisibility(model, 1);
            if (fx->field_80 != 0) {
                fn_80118C88(fx->field_80, 1);
            }
            if (fx->effect_handle != 0) {
                fn_80118C88(fx->effect_handle, 1);
            }
        } else {
            u8 flags = fx->flags;

            if ((flags & 1) != 0) {
                fx->flags = flags ^ 1;
            }
            GSmodelSetVisibility(model, 0);
            if (fx->field_80 != 0) {
                fn_80118C88(fx->field_80, 0);
            }
            if (fx->effect_handle != 0) {
                fn_80118C88(fx->effect_handle, 0);
            }
        }
    }
}

/**
 * wazaSequenceSetDownFlag - Waza effect set fields 0x16=0, 0x17=r4.
 * Address: 0x801DA5AC | Size: 0x18
 */
void wazaSequenceSetDownFlag(void* effect, u8 val) {
    if (effect == NULL) return;
    *(u8*)((u8*)effect + 0x16) = 0;
    *(u8*)((u8*)effect + 0x17) = val;
}

/**
 * wazaSequenceSysIsAllEnd - Check whether every active effect sequence has finished.
 * Address: 0x801DA5C4 | Size: 0xD4
 */
u8 wazaSequenceSysIsAllEnd(s32 timeType) {
    u8* pool = lbl_80467CC0;
    WazaEffect* effect;
    s32 i = 0;
    s32 count;
    void* waza;

    effect = *(WazaEffect**)pool;
    count = *(u16*)(pool + 4);
    for (; i < count; i++, effect++) {
        if (effect->active != 0) {
            waza = *(void**)((u8*)effect + 0x6C);
            if (waza != NULL) {
                if (waza == NULL) {
                    return TRUE;
                }

                if ((u8)timeType == 6) {
                    if (*(u8*)((u8*)waza + 0x14) != 0 && *(s8*)((u8*)waza + 0x15) != -1) {
                        return FALSE;
                    }
                    return TRUE;
                }

                return *(s32*)waza >=
                       wazaSequenceSysGetWazaTime(*(void**)((u8*)waza + 0x3C), waza, timeType);
            }
        }
    }
    return TRUE;
}

/**
 * wazaSequenceSysIsWazaEnd - Waza effect tick lifetime.
 * Address: 0x801DA698 | Size: 0xB4
 */
u8 wazaSequenceSysIsWazaEnd(void* sequence, s32 moveID, s32 variant, s32 timeType) {
    void* waza;

    if (sequence == NULL) {
        return FALSE;
    }

    waza = GetWaza__12NullSequenceCFUsUs(sequence, moveID, variant);
    if (waza == NULL) {
        return TRUE;
    }

    if ((u8)timeType == 6) {
        if (*(u8*)((u8*)waza + 0x14) != 0 && *(s8*)((u8*)waza + 0x15) != -1) {
            return FALSE;
        }
        return TRUE;
    }

    return *(s32*)waza >= wazaSequenceSysGetWazaTime(*(void**)((u8*)waza + 0x3C), waza, timeType);
}

/**
 * fn_801DA74C - Get the time for a move's sequence entry.
 * Address: 0x801DA74C | Size: 0x60
 */
s32 fn_801DA74C(void* sequence, s32 moveID, s32 variant, s32 timeType) {
    void* waza;

    if (sequence == NULL) {
        return 0;
    }

    waza = GetWaza__12NullSequenceCFUsUs(sequence, moveID, variant);
    if (waza != NULL) {
        return wazaSequenceSysGetWazaTime(sequence, waza, timeType);
    }
    return 0;
}

/**
 * wazaSequenceSysFreeAll - Clear child sequences from every active effect.
 * Address: 0x801DA7AC | Size: 0x90
 */
void wazaSequenceSysFreeAll(void) {
    extern void wazaSequenceApplyStop(void* obj);
    extern void wazaSequenceFree(void* obj);
    u8* pool;
    WazaEffect* effect;
    s32 i;
    s32 count;
    void* child;
    void* next;

    pool = lbl_80467CC0;
    effect = *(WazaEffect**)pool;
    count = *(u16*)(pool + 4);
    for (i = 0; i < count; i++, effect++) {
        if (effect->active != 0) {
            child = *(void**)((u8*)effect + 0x68);
            while (child != NULL) {
                next = *(void**)((u8*)child + 0x34);
                if (*(u8*)((u8*)child + 0x14) != 0) {
                    wazaSequenceApplyStop(child);
                }
                wazaSequenceFree(child);
                child = next;
            }
            *(void**)((u8*)effect + 0x68) = NULL;
        }
    }
}

/**
 * fn_801DA83C - Waza effect pool free.
 * Address: 0x801DA83C | Size: 0x88
 */
void fn_801DA83C(void* effect) {
    extern void wazaSequenceApplyStop(void* obj);
    extern void wazaSequenceFree(void* obj);

    void* cur;
    void* next;

    if (effect != NULL) {
        cur = *(void**)((u8*)effect + 0x68);
        if (*(u8*)((u8*)effect + 0x74) == 0) {
            return;
        }
        while (cur != NULL) {
            next = *(void**)((u8*)cur + 0x34);
            if (*(u8*)((u8*)cur + 0x14) != 0) {
                wazaSequenceApplyStop(cur);
            }
            wazaSequenceFree(cur);
            cur = next;
        }
        *(void**)((u8*)effect + 0x68) = NULL;
    }
}

/**
 * wazaSequenceSysStopWaza - Waza resolve/update helper.
 * Address: 0x801DA8C4 | Size: 0x50
 */
void wazaSequenceSysStopWaza(void* obj, s32 search_key1, s32 search_key2) {
    void* resolved;
    if (obj != NULL) {
        resolved = GetWaza__12NullSequenceCFUsUs(obj, search_key1, search_key2);
        if (resolved != NULL) {
            if (*(u8*)((u8*)resolved + 0x14) != 0) {
                wazaSequenceApplyStop(resolved);
            }
            wazaSequenceFree(resolved);
        }
    }
}

/**
 * wazaSequenceClearFlag16 - Waza effect pool get used count.
 * Address: 0x801DA914 | Size: 0x38
 */
void wazaSequenceClearFlag16(void* obj, s32 search_key1, s32 search_key2) {
    void* resolved;
    if (obj != NULL) {
        resolved = GetWaza__12NullSequenceCFUsUs(obj, search_key1, search_key2);
        if (resolved != NULL) {
            *(u8*)((u8*)resolved + 0x16) = 0;
        }
    }
}

/**
 * wazaSequenceSysIsPlayingWaza - Waza resolved effect has active non-minus-one byte.
 * Address: 0x801DA94C | Size: 0x68
 */
s32 wazaSequenceSysIsPlayingWaza(void* obj, s32 search_key1, s32 search_key2) {
    void* resolved;
    u8 result;
    if (obj == NULL) {
        return 0;
    }
    resolved = GetWaza__12NullSequenceCFUsUs(obj, search_key1, search_key2);
    if (resolved != NULL) {
        result = 0;
        if (*(u8*)((u8*)resolved + 0x14) != 0) {
            if (*(s8*)((u8*)resolved + 0x15) != -1) {
                result = 1;
            }
        }
        return result;
    }
    return 0;
}

/**
 * fn_801DA9B4 - Waza effect pool clear all.
 * Address: 0x801DA9B4 | Size: 0x34
 */
void fn_801DA9B4(void* obj, s32 search_key1, s32 search_key2) {
    void* resolved;
    if (obj != NULL) {
        resolved = GetWaza__12NullSequenceCFUsUs(obj, search_key1, search_key2);
        if (resolved != NULL) {
            wazaSequenceApplyStop(resolved);
        }
    }
}

/**
 * wazaSequenceSysStartWaza - Waza sound effect play.
 * Address: 0x801DA9E8 | Size: 0xC4
 */
void wazaSequenceSysStartWaza(void* sequence, s32 moveID, s32 variant) {
    u8* pool = lbl_80467CC0;
    WazaEffect* entry;
    s32 i = 0;
    int count;
    void* waza;

    entry = *(WazaEffect**)pool;
    count = *(u16*)(pool + 4);
    for (; i < count; i++, entry++) {
        if (entry->active != 0) {
            waza = GetWaza__12NullSequenceCFUsUs(entry, moveID, variant);
            if (waza != NULL && *(u8*)((u8*)waza + 0x14) != 0) {
                wazaSequenceApplyStop(waza);
            }
        }
    }

    if (sequence != NULL) {
        waza = GetWaza__12NullSequenceCFUsUs(sequence, moveID, variant);
        if (sequence != NULL) {
            *(u8*)((u8*)sequence + 0x16) = 0;
            *(u8*)((u8*)sequence + 0x17) = 0;
        }
        if (waza != NULL) {
            wazaSequenceStart(waza);
        }
    }
}

/**
 * fn_801DAAAC - Draw every model layer attached to a waza effect.
 * Address: 0x801DAAAC | Size: 0x100
 */
void fn_801DAAAC(void* effectPtr) {
    extern void* GScameraGetActiveCamera(void);
    extern s32 HSD_CObjSetCurrent(void* cobj);
    extern void gsParticleDraw(u32 drawFlags, u8 modelID);
    extern void HSD_CObjEndCurrent(void);
    WazaEffect* effect = effectPtr;
    u8* effectBytes = effectPtr;
    u32 flags;
    u8* entry;
    u32 modelID;
    s32 modelGroup;
    s32 pass;
    void* camera;

    if (effect != NULL) {
        GSmodelForceAnimTransformUpdate(effect->model);
        for (modelGroup = 0; modelGroup < 2; modelGroup++) {
            modelID = modelGroup != 0;
            for (pass = 0; pass < 3; pass++) {
                if (pass == 0) {
                    flags = 0x10;
                } else if (pass == 1) {
                    flags = 0x1000;
                } else {
                    flags = 0x2000;
                }

                if (modelID == (u8)fn_800E3CBC(effect->model)) {
                    GSmodelDrawModel(effect->model, flags);
                }

                entry = *(u8**)(effectBytes + 0x68);
                while (entry != NULL) {
                    if (*(u8*)(entry + 0x14) != 0) {
                        wazaSequenceOwnerDrawAttached(entry, flags, modelID);
                    }
                    entry = *(u8**)(entry + 0x34);
                }

                camera = GScameraGetActiveCamera();
                if (camera != NULL && HSD_CObjSetCurrent(*(void**)((u8*)camera + 0xC))) {
                    gsParticleDraw(flags, modelID);
                    HSD_CObjEndCurrent();
                }
            }
        }
    }
}

/**
 * fn_801DABAC - Get the scale for a waza selector.
 * Address: 0x801DABAC | Size: 0x78
 */
f32 fn_801DABAC(void* obj) {
    s32 selector;

    if (obj != NULL) {
        selector = *(s32*)((u8*)obj + 0x10);
    } else {
        selector = 0;
    }

    switch (selector) {
    case -2:
        return lbl_8047E388;
    case -1:
        return lbl_8047E38C;
    case 1:
        return lbl_8047E390;
    case 2:
        return lbl_8047E394;
    case 3:
        return lbl_8047E398;
    default:
        return lbl_8047E39C;
    }
}

/**
 * wazaSequenceOwnerGetSizeClass - Waza get field 0x10.
 * Address: 0x801DAC24 | Size: 0x18
 */
u32 wazaSequenceOwnerGetSizeClass(void* obj) {
    if (obj != NULL) {
        return *(u32*)((u8*)obj + 0x10);
    }
    return 0;
}

/**
 * wazaSequenceOwnerGetModel - Waza get field 0x24.
 * Address: 0x801DAC3C | Size: 0x18
 */
u32 wazaSequenceOwnerGetModel(void* obj) {
    if (obj != NULL) {
        return *(u32*)((u8*)obj + 0x24);
    }
    return 0;
}

/**
 * wazaSequenceOwnerIsPlaying - Waza check field 0x6C nonzero.
 * Address: 0x801DAC54 | Size: 0x24
 */
u32 wazaSequenceOwnerIsPlaying(void* obj) {
    if (obj == NULL) {
        return 0;
    }
    return *(u32*)((u8*)obj + 0x6C) != 0;
}

/**
 * wazaSequenceOwnerGetId - Waza get field 0x70.
 * Address: 0x801DAC78 | Size: 0x18
 */
u16 wazaSequenceOwnerGetId(void* obj) {
    if (obj != NULL) {
        return *(u16*)((u8*)obj + 0x70);
    }
    return 0;
}
