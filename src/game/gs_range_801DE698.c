/**
 * @file gs_range_801DE698.c
 * @brief gs-engine, 0x801DE698 - 0x801DF790.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) -- mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 *
 * The 10 functions below (0x801DE698-0x801DF474, the full function count
 * for this TU's declared range) previously lived, misattributed, in
 * game/battle/battle_waza.c (whose splits.txt range ends at 0x801DE698);
 * relocated here so this unit's real C source is scored where it belongs.
 */
#include "dolphin/types.h"

/**
 * fn_801DE698 - Waza stat change effect.
 * Address: 0x801DE698 | Size: 0x5CC
 */
void fn_801DE698(u32 model, u8* callback) {
    extern void GSmodelLinkTexAnimToAnim(void*, u32);
    extern u8 GSmodelHasAnimationEnded(void*);
    extern void GSmodelGetAnimIndex(void*, u32*, u32*);
    extern void GSmodelSetAnimIndex(void*, u32);
    extern void GSmodelSetAnimType(void*, u32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern void GSmodelSetAnimFrame(void*, f32);
    extern void GSmodelStartAnimation(void*);
    extern void GSmodelSetAnimEndedCallback(void*, void*, void*);
    extern void fn_801DD100(void*, u32);
    extern f32 lbl_8047E3C8;
    extern f32 lbl_8047E3CC;
    u32 flags;
    u8* owner;
    u8* group;
    u8* entry;
    void* modelPtr;
    u32 animIndex;
    u32 currentIndex;
    u32 unused;
    u32 count;
    u16 phase;

    flags = *(u32*)callback;
    if ((flags & 3) != 1 || (flags & 4) == 4) {
        return;
    }

    owner = *(u8**)(callback + 8);
    if (owner == NULL || model != *(u32*)(owner + 0x24) || owner[0x16] != 0) {
        return;
    }

    group = *(u8**)(owner + 0x2C) + (*(u16*)(owner + 0x32) * 0xD4);
    if (*(u16*)(owner + 0x34) == 0 && *(u32*)(group + 0x90) != 0) {
        *(u32*)(group + 0x90) = *(u32*)(group + 0x90) - 1;
        if (owner[0x16] != 0 || (owner[0x18] & 8) == 8) {
            return;
        }
        modelPtr = *(void**)(owner + 0x24);
        if ((owner[0x18] & 4) != 4) {
            owner[0x19] = 0;
            GSmodelLinkTexAnimToAnim(modelPtr, 1);
        }
        group = *(u8**)(owner + 0x2C);
        if ((owner[0x18] & 2) == 2 && *(u16*)(owner + 0x14) > 0x10) {
            if (*(s32*)(group + 0xDD4) != 1) {
                group += 0xD40;
            }
        }
        animIndex = 0;
        count = *(u32*)(group + 4);
        entry = group + 0x8C;
        while (count > 0) {
            if (*(s32*)entry == 0) {
                animIndex = *(u32*)(entry + 4);
                break;
            }
            entry += 8;
            count--;
        }
        GSmodelGetAnimIndex(modelPtr, &currentIndex, &unused);
        GSmodelSetAnimType(modelPtr, 0);
        GSmodelSetAnimRate(modelPtr, lbl_8047E3C8);
        if (animIndex != currentIndex || GSmodelHasAnimationEnded(modelPtr) != 0) {
            GSmodelSetAnimIndex(modelPtr, animIndex);
            GSmodelSetAnimFrame(modelPtr, lbl_8047E3CC);
        }
        GSmodelStartAnimation(modelPtr);
        return;
    }

    *(u16*)(owner + 0x34) = *(u16*)(owner + 0x34) + 1;
    if (*(u16*)(owner + 0x34) >= *(u32*)(group + 4)) {
        phase = *(u16*)(owner + 0x32);
        if (phase < 9 || phase >= 0xB) {
            if (owner[0x16] == 0 && (owner[0x18] & 8) != 8) {
                modelPtr = *(void**)(owner + 0x24);
                if ((owner[0x18] & 4) != 4) {
                    owner[0x19] = 0;
                    GSmodelLinkTexAnimToAnim(modelPtr, 1);
                }
                group = *(u8**)(owner + 0x2C);
                if ((owner[0x18] & 2) == 2 && *(u16*)(owner + 0x14) > 0x10) {
                    if (*(s32*)(group + 0xDD4) != 1) {
                        group += 0xD40;
                    }
                }
                animIndex = 0;
                count = *(u32*)(group + 4);
                entry = group + 0x8C;
                while (count > 0) {
                    if (*(s32*)entry == 0) {
                        animIndex = *(u32*)(entry + 4);
                        break;
                    }
                    entry += 8;
                    count--;
                }
                GSmodelGetAnimIndex(modelPtr, &currentIndex, &unused);
                GSmodelSetAnimType(modelPtr, 1);
                GSmodelSetAnimRate(modelPtr, lbl_8047E3C8);
                if (animIndex != currentIndex || GSmodelHasAnimationEnded(modelPtr) != 0) {
                    GSmodelSetAnimIndex(modelPtr, animIndex);
                    GSmodelSetAnimFrame(modelPtr, lbl_8047E3CC);
                }
                GSmodelStartAnimation(modelPtr);
            }
            fn_801DD100(owner, 0);
        }
        return;
    }

    entry = group + 0x8C + (*(u16*)(owner + 0x34) * 8);
    if (*(s32*)entry == 0) {
        phase = *(u16*)(owner + 0x32);
        if (phase == 0xA) {
            if (owner != NULL) {
                modelPtr = *(void**)(owner + 0x24);
                owner[0x19] = 0;
                GSmodelLinkTexAnimToAnim(modelPtr, 1);
                group = *(u8**)(owner + 0x2C);
                animIndex = 0;
                count = *(u32*)(group + 0x84C);
                entry = group + 0x8D4;
                while (count > 0) {
                    if (*(s32*)entry == 0) {
                        animIndex = *(u32*)(entry + 4);
                        break;
                    }
                    entry += 8;
                    count--;
                }
                GSmodelSetAnimIndex(modelPtr, animIndex);
                if (owner[0x75] != 0) {
                    GSmodelSetAnimType(modelPtr, 0);
                } else {
                    GSmodelSetAnimType(modelPtr, 1);
                }
                GSmodelSetAnimRate(modelPtr, lbl_8047E3C8);
                GSmodelSetAnimFrame(modelPtr, lbl_8047E3CC);
                GSmodelStartAnimation(modelPtr);
                if (owner[0x75] != 0) {
                    GSmodelSetAnimEndedCallback(modelPtr, 0, 0);
                    owner[0x16] = 1;
                }
            }
        } else {
            animIndex = *(u32*)(entry + 4);
            if (owner != NULL) {
                u32 animType = 0;

                if (owner[0x75] == 0) {
                    switch (phase) {
                    case 1:
                        break;
                    default:
                    case 10:
                        animType = 1;
                        break;
                    }
                }
                if ((owner[0x18] & 8) != 8) {
                    modelPtr = *(void**)(owner + 0x24);
                    if ((owner[0x18] & 4) != 4) {
                        owner[0x19] = 0;
                        GSmodelLinkTexAnimToAnim(modelPtr, 1);
                    }
                    GSmodelSetAnimIndex(modelPtr, animIndex);
                    GSmodelSetAnimType(modelPtr, animType);
                    GSmodelSetAnimRate(modelPtr, lbl_8047E3C8);
                    GSmodelSetAnimFrame(modelPtr, lbl_8047E3CC);
                    GSmodelStartAnimation(modelPtr);
                }
            }
        }
    } else if (owner != NULL && owner[0x16] == 0 && (owner[0x18] & 8) != 8) {
        modelPtr = *(void**)(owner + 0x24);
        if ((owner[0x18] & 4) != 4) {
            owner[0x19] = 0;
            GSmodelLinkTexAnimToAnim(modelPtr, 1);
        }
        group = *(u8**)(owner + 0x2C);
        if ((owner[0x18] & 2) == 2 && *(u16*)(owner + 0x14) > 0x10) {
            if (*(s32*)(group + 0xDD4) != 1) {
                group += 0xD40;
            }
        }
        animIndex = 0;
        count = *(u32*)(group + 4);
        entry = group + 0x8C;
        while (count > 0) {
            if (*(s32*)entry == 0) {
                animIndex = *(u32*)(entry + 4);
                break;
            }
            entry += 8;
            count--;
        }
        GSmodelGetAnimIndex(modelPtr, &currentIndex, &unused);
        GSmodelSetAnimType(modelPtr, 0);
        GSmodelSetAnimRate(modelPtr, lbl_8047E3C8);
        GSmodelSetAnimIndex(modelPtr, animIndex);
        GSmodelSetAnimFrame(modelPtr, lbl_8047E3CC);
        GSmodelStartAnimation(modelPtr);
    }
}

/**
 * _eyeTexAnimEnded - Waza stat change update.
 * Address: 0x801DEC64 | Size: 0x1B0
 */
void _eyeTexAnimEnded(void* model, u8* callback) {
    extern void GSmodelLinkTexAnimToAnim(void*, u32);
    extern u8 GSmodelIsAnimating(void*);
    extern f32 GSmodelGetAnimFrame(void*);
    extern f32 GSmodelGetAnimRate(void*);
    extern u32 GSmodelGetAnimType(void*);
    extern void GSmodelGetAnimIndex(void*, u32*, u32*);
    extern void GSmodelSetTexAnimIndex(void*, u32);
    extern u32 fn_800D3088(void);
    extern void GSmodelSetTexAnimFrame(void*, f32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSmodelSetTexAnimType(void*, u32);
    extern void GSmodelStartTexAnimation(void*);
    extern void GSlogWrite(const char*, ...);
    extern const char lbl_802799D8[];
    u8* owner;
    u32 animIndex;
    u32 unused;
    f32 frame;
    f32 rate;
    u32 type;

    if ((*(u32*)callback & 3) != 2 || (*(u32*)callback & 4) != 0) {
        return;
    }
    owner = *(u8**)(callback + 8);
    if (owner == NULL || (owner[0x18] & 8) != 0 ||
        model != *(void**)(owner + 0x24)) {
        return;
    }
    animIndex = *(u32*)(callback + 4);
    if (animIndex != (s16)*(u16*)(owner + 0x1A) &&
        animIndex != (s16)*(u16*)(owner + 0x1E) &&
        animIndex != (s16)*(u16*)(owner + 0x1C)) {
        return;
    }
    switch (owner[0x19]) {
    case 1:
        owner[0x19] = 6;
        break;
    case 2:
    case 3:
        GSmodelLinkTexAnimToAnim(model, 1);
        if (GSmodelIsAnimating(model)) {
            frame = GSmodelGetAnimFrame(model);
            rate = GSmodelGetAnimRate(model);
            type = GSmodelGetAnimType(model);
            GSmodelGetAnimIndex(model, &animIndex, &unused);
            GSmodelSetTexAnimIndex(model, animIndex);
            GSmodelSetTexAnimFrame(model, frame + rate * fn_800D3088());
            GSmodelSetTexAnimRate(model, rate);
            GSmodelSetTexAnimType(model, type);
            GSmodelStartTexAnimation(model);
        }
        owner[0x19] = 0;
        break;
    default:
        GSlogWrite(lbl_802799D8);
        break;
    }
}

/**
 * fn_801DEE14 - Waza status effect visual.
 * Address: 0x801DEE14 | Size: 0xF8
 */
void fn_801DEE14(u8* obj) {
    extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
    extern void GSmodelSetAnimIndex(void* model, u32 index);
    extern void GSmodelSetAnimType(void* model, u32 type);
    extern void GSmodelSetAnimRate(void* model, f32 rate);
    extern void GSmodelSetAnimFrame(void* model, f32 frame);
    extern void GSmodelStartAnimation(void* model);
    extern void GSmodelSetAnimEndedCallback(void* model, void* callback, void* arg);
    extern f32 lbl_8047E3C8;
    extern f32 lbl_8047E3CC;
    void* model;
    u8* entry;
    s32 count;
    u32 animIndex;

    if (obj == 0) {
        return;
    }

    model = *(void**)(obj + 0x24);
    obj[0x19] = 0;
    GSmodelLinkTexAnimToAnim(model, 1);

    entry = *(u8**)(obj + 0x2C) + 0x8D4;
    count = *(s32*)(*(u8**)(obj + 0x2C) + 0x84C);
    while (count-- > 0) {
        if (*(s32*)entry == 0) {
            animIndex = *(u32*)(entry + 4);
            goto found;
        }
        entry += 8;
    }
    animIndex = 0;

found:
    GSmodelSetAnimIndex(model, animIndex);

    if (obj[0x75] != 0) {
        GSmodelSetAnimType(model, 0);
    } else {
        GSmodelSetAnimType(model, 1);
    }
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    GSmodelSetAnimFrame(model, lbl_8047E3CC);
    GSmodelStartAnimation(model);

    if (obj[0x75] != 0) {
        GSmodelSetAnimEndedCallback(model, 0, 0);
        obj[0x16] = 1;
    }
}

/**
 * fn_801DEF0C - Waza status effect update.
 * Address: 0x801DEF0C | Size: 0x164
 */
void fn_801DEF0C(void* obj, s32 arg1, s32 arg2) {
    extern void GSmodelLinkTexAnimToAnim(void*, u32);
    extern void GSmodelGetAnimIndex(void*, u32*, u32*);
    extern void GSmodelSetAnimType(void*, u32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern u8 GSmodelHasAnimationEnded(void*);
    extern void GSmodelSetAnimIndex(void*, u32);
    extern void GSmodelSetAnimFrame(void*, f32);
    extern void GSmodelStartAnimation(void*);
    extern f32 lbl_8047E3C8;
    extern f32 lbl_8047E3CC;
    u8* effect = obj;
    u8* table;
    s32 count;
    u32 animIndex = 0;
    u32 currentIndex;
    u32 unused;
    void* model;

    if (effect == NULL || effect[0x16] != 0 || (effect[0x18] & 8) != 0) {
        return;
    }
    model = *(void**)(effect + 0x24);
    if ((effect[0x18] & 4) == 0) {
        effect[0x19] = 0;
        GSmodelLinkTexAnimToAnim(model, 1);
    }
    table = *(u8**)(effect + 0x2C);
    if ((effect[0x18] & 2) != 0 && *(u16*)(effect + 0x14) > 0x10) {
        if (*(s32*)(table + 0xDD4) != 1) {
            table += 0xD40;
        }
    }
    count = *(s32*)(table + 4);
    table += 0x8C;
    while (count-- > 0) {
        if (*(s32*)table == 0) {
            animIndex = *(u32*)(table + 4);
            break;
        }
        table += 8;
    }
    GSmodelGetAnimIndex(model, &currentIndex, &unused);
    GSmodelSetAnimType(model, arg1);
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    if (animIndex != currentIndex || (u8)arg2 != 0 ||
        GSmodelHasAnimationEnded(model)) {
        GSmodelSetAnimIndex(model, animIndex);
        GSmodelSetAnimFrame(model, lbl_8047E3CC);
    }
    GSmodelStartAnimation(model);
}

/**
 * fn_801DF070 - Waza weather effect setup.
 * Address: 0x801DF070 | Size: 0xF0
 */
void fn_801DF070(u8* obj, u32 animIndex, u32 animType) {
    extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
    extern void GSmodelSetAnimIndex(void* model, u32 index);
    extern void GSmodelSetAnimType(void* model, u32 type);
    extern void GSmodelSetAnimRate(void* model, f32 rate);
    extern void GSmodelSetAnimFrame(void* model, f32 frame);
    extern void GSmodelStartAnimation(void* model);
    void* model;

    if (obj == 0) {
        return;
    }

    if (obj[0x75] == 0) {
        switch (*(u16*)(obj + 0x32)) {
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 10:
            animType = 1;
            break;
        }
    }

    if ((obj[0x18] & 8) == 8) {
        return;
    }

    model = *(void**)(obj + 0x24);
    if ((obj[0x18] & 4) != 4) {
        obj[0x19] = 0;
        GSmodelLinkTexAnimToAnim(model, 1);
    }
    GSmodelSetAnimIndex(model, animIndex);
    GSmodelSetAnimType(model, animType);
    GSmodelSetAnimRate(model, 1.0f);
    GSmodelSetAnimFrame(model, 0.0f);
    GSmodelStartAnimation(model);
}

/**
 * fn_801DF160 - Waza weather effect update.
 * Address: 0x801DF160 | Size: 0x70
 */
s32 fn_801DF160(u8* obj) {
    u8* base;
    u8* table;
    s32* entry;
    s32 count;

    base = *(u8**)(obj + 0x2C);
    table = base;
    if ((obj[0x18] & 2) == 2 && *(u16*)(obj + 0x14) > 0x10) {
        table = base + 0xD40;
        if (*(volatile s32*)(table + 0x94) == 1) {
            table = base;
        }
    }

    count = *(s32*)(table + 4);
    entry = (s32*)(table + 0x8C);
    while (count-- > 0) {
        if (entry[0] == 0) {
            return entry[1];
        }
        entry += 2;
    }
    return 0;
}

/**
 * fn_801DF1D0 - Waza weather effect render.
 * Address: 0x801DF1D0 | Size: 0x16C
 */
void fn_801DF1D0(void* obj) {
    extern u8 fn_801DAC54(void*);
    extern u32 fn_800D3088(void);
    extern f32 fn_800E0BE4(void);
    extern void GSmodelLinkTexAnimToAnim(void*, u32);
    extern void GSmodelSetTexAnimIndex(void*, u32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSmodelSetTexAnimType(void*, u32);
    extern void GSmodelSetTexAnimFrame(void*, f32);
    extern void GSmodelStartTexAnimation(void*);
    extern f32 lbl_8047E3C8;
    extern f32 lbl_8047E3CC;
    extern f32 lbl_8047E3D8;
    extern f32 lbl_8047E3DC;
    extern f32 lbl_8047E3E0;
    extern f32 lbl_8047E3E4;
    u8* effect = obj;
    void* model;
    u16 timer;
    f32 threshold;

    if (fn_801DAC54(effect) != 0 || (effect[0x18] & 8) != 0) {
        return;
    }
    if (effect[0x19] != 0) {
        *(u16*)(effect + 0x20) = 0;
        return;
    }
    if (*(s16*)(effect + 0x1C) < 0) {
        return;
    }
    *(u16*)(effect + 0x20) += (u16)fn_800D3088();
    timer = *(u16*)(effect + 0x20);
    if (timer < 10) {
        return;
    }
    if (timer < 60) {
        f32 t = (f32)(timer - 10) / lbl_8047E3E0;
        threshold = t * (lbl_8047E3E4 - t) * lbl_8047E3DC;
    } else if (timer < 180) {
        threshold = lbl_8047E3DC;
    } else {
        threshold = lbl_8047E3D8;
    }
    if (threshold < fn_800E0BE4()) {
        return;
    }
    model = *(void**)(effect + 0x24);
    if (model != NULL) {
        effect[0x19] = 3;
        GSmodelLinkTexAnimToAnim(model, 0);
        GSmodelSetTexAnimIndex(model, *(s16*)(effect + 0x1C));
        GSmodelSetTexAnimRate(model, lbl_8047E3C8);
        GSmodelSetTexAnimType(model, 0);
        GSmodelSetTexAnimFrame(model, lbl_8047E3CC);
        GSmodelStartTexAnimation(model);
        *(u16*)(effect + 0x20) = 0;
    }
}

/**
 * fn_801DF33C - Waza weather effect clear.
 * Address: 0x801DF33C | Size: 0x98
 */
void fn_801DF33C(u8* obj) {
    extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
    extern void GSmodelSetTexAnimIndex(void* model, u32 index);
    extern void GSmodelSetTexAnimRate(void* model, f32 rate);
    extern void GSmodelSetTexAnimType(void* model, u32 type);
    extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
    extern void GSmodelStartTexAnimation(void* model);
    void* model = *(void**)(obj + 0x24);

    if (*(s16*)(obj + 0x1E) >= 0 && obj[0x19] == 6) {
        obj[0x19] = 2;
        GSmodelLinkTexAnimToAnim(model, 0);
        GSmodelSetTexAnimIndex(model, *(s16*)(obj + 0x1E));
        GSmodelSetTexAnimRate(model, 1.0f);
        GSmodelSetTexAnimType(model, 0);
        GSmodelSetTexAnimFrame(model, 0.0f);
        GSmodelStartTexAnimation(model);
    }
}

/**
 * fn_801DF3D4 - Waza weather get type.
 * Address: 0x801DF3D4 | Size: 0xA0
 */
void fn_801DF3D4(u8* obj) {
    extern void GSmodelLinkTexAnimToAnim(void* model, u32 enable);
    extern void GSmodelSetTexAnimIndex(void* model, u32 index);
    extern void GSmodelSetTexAnimRate(void* model, f32 rate);
    extern void GSmodelSetTexAnimType(void* model, u32 type);
    extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
    extern void GSmodelStartTexAnimation(void* model);
    void* model = *(void**)(obj + 0x24);

    if (*(s16*)(obj + 0x1A) >= 0 && obj[0x19] != 6 && obj[0x19] != 1) {
        obj[0x19] = 1;
        GSmodelLinkTexAnimToAnim(model, 0);
        GSmodelSetTexAnimIndex(model, *(s16*)(obj + 0x1A));
        GSmodelSetTexAnimRate(model, 1.0f);
        GSmodelSetTexAnimType(model, 0);
        GSmodelSetTexAnimFrame(model, 0.0f);
        GSmodelStartTexAnimation(model);
    }
}

/**
 * fn_801DF474 - Waza ability effect handler.
 * Address: 0x801DF474 | Size: 0x31C
 */
void fn_801DF474(s32 slot, s32 abilityID) {
    /* TODO: Ability effect handler (0x31C bytes)
     * Handles visual effects for ability activations
     * (Intimidate, Levitate, etc.).
     */
}
