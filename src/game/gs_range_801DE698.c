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

#if !defined(GS_RANGE_EXACT_801DEE14_801DF160)
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

/* Animation list: a count at +0x4 and (key, index) pairs from +0x8C. */
static inline s32 wazaAnimFind(u8* table, s32 key) {
    s32 count;
    s32 i;
    u8* entry;

    count = *(s32*)(table + 4);
    entry = table + 0x8C;
    for (i = 0; i < count; i++) {
        if (*(s32*)entry == key) {
            return *(s32*)(entry + 4);
        }
        entry += 8;
    }
    return 0;
}

static inline u8* wazaAnimTable(u8* owner) {
    u8* base;
    u8* table;

    base = *(u8**)(owner + 0x2C);
    table = base;
    if ((owner[0x18] & 2) == 2 && *(u16*)(owner + 0x14) > 0x10) {
        table = base + 0xD40;
        if (*(s32*)(table + 0x94) == 1) {
            table = base;
        }
    }
    return table;
}

static inline void wazaPlayIdle(u8* owner, u32 type, s32 force) {
    void* model;
    s32 animIndex;
    s32 currentIndex;
    s32 unused;

    if (owner == NULL) {
        return;
    }
    if (owner[0x16] != 0) {
        return;
    }
    if ((owner[0x18] & 8) == 8) {
        return;
    }
    model = *(void**)(owner + 0x24);
    if ((owner[0x18] & 4) != 4) {
        owner[0x19] = 0;
        GSmodelLinkTexAnimToAnim(model, 1);
    }
    animIndex = wazaAnimFind(wazaAnimTable(owner), 0);
    GSmodelGetAnimIndex(model, (u32*)&currentIndex, (u32*)&unused);
    GSmodelSetAnimType(model, type);
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    if (force || animIndex != currentIndex || GSmodelHasAnimationEnded(model) != 0) {
        GSmodelSetAnimIndex(model, animIndex);
        GSmodelSetAnimFrame(model, lbl_8047E3CC);
    }
    GSmodelStartAnimation(model);
}

static inline void wazaPlayStep(u8* owner, u8* entry, u16 phase) {
    u32 animType;
    u32 animIndex;
    void* model;

    animIndex = *(u32*)(entry + 4);
    animType = 0;
    if (owner == NULL) {
        return;
    }
    if (owner[0x75] == 0) {
        switch (phase) {
        case 1:
            break;
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
    if ((owner[0x18] & 8) == 8) {
        return;
    }
    model = *(void**)(owner + 0x24);
    if ((owner[0x18] & 4) != 4) {
        owner[0x19] = 0;
        GSmodelLinkTexAnimToAnim(model, 1);
    }
    GSmodelSetAnimIndex(model, animIndex);
    GSmodelSetAnimType(model, animType);
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    GSmodelSetAnimFrame(model, lbl_8047E3CC);
    GSmodelStartAnimation(model);
}

static inline void wazaPlayFinish(u8* owner) {
    void* model;

    if (owner == NULL) {
        return;
    }
    model = *(void**)(owner + 0x24);
    owner[0x19] = 0;
    GSmodelLinkTexAnimToAnim(model, 1);
    GSmodelSetAnimIndex(model, wazaAnimFind(*(u8**)(owner + 0x2C) + 0x848, 0));
    if (owner[0x75] != 0) {
        GSmodelSetAnimType(model, 0);
    } else {
        GSmodelSetAnimType(model, 1);
    }
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    GSmodelSetAnimFrame(model, lbl_8047E3CC);
    GSmodelStartAnimation(model);
    if (owner[0x75] != 0) {
        GSmodelSetAnimEndedCallback(model, 0, 0);
        owner[0x16] = 1;
    }
}

/**
 * fn_801DE698 - Waza stat change effect.
 * Address: 0x801DE698 | Size: 0x5CC
 */
void fn_801DE698(u32 model, u8* callback) {
    u32 flags;
    u8* owner;
    u8* entry;
    u8* group;
    void* modelPtr;
    u32 animIndex;
    u32 animType;
    s32 phase;

    flags = *(u32*)callback;
    if ((flags & 3) == 1 && (flags & 4) != 4) {

    owner = *(u8**)(callback + 8);
    if (owner != NULL && model == *(u32*)(owner + 0x24) && owner[0x16] == 0) {

    group = *(u8**)(owner + 0x2C) + (*(u16*)(owner + 0x32) * 0xD4);
    if (*(u16*)(owner + 0x34) == 0 && *(s32*)(group + 0x90) != 0) {
        *(s32*)(group + 0x90) = *(s32*)(group + 0x90) - 1;
        wazaPlayIdle(owner, 0, 0);
        return;
    }

    (*(u16*)(owner + 0x34))++;
    if (*(u16*)(owner + 0x34) >= *(s32*)(group + 4)) {
        phase = *(u16*)(owner + 0x32);
        if (phase >= 0xB || phase < 9) {
            wazaPlayIdle(owner, 1, 0);
            fn_801DD100(owner, 0);
        }
        return;
    }

    entry = group + 0x8C + ((u16)*(u16*)(owner + 0x34) * 8);
    if (*(s32*)entry == 0) {
        phase = *(u16*)(owner + 0x32);
        if ((u16)phase == 0xA) {
            wazaPlayFinish(owner);
        } else {
            wazaPlayStep(owner, entry, phase);
        }
    } else {
        wazaPlayIdle(owner, 0, 1);
    }
    }
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
    extern u32 GSgetFrameStep(void);
    extern void GSmodelSetTexAnimFrame(void*, f32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSmodelSetTexAnimType(void*, u32);
    extern void GSmodelStartTexAnimation(void*);
    extern void GSlogWrite(const char*, ...);
    extern const char lbl_802799D8[];
    u8* owner;
    s32 animIndex;
    u32 unused;
    f32 frame;
    f32 rate;
    u32 type;
    if ((*(u32*)callback & 3) != 2) {
        return;
    }
    if ((*(u32*)callback & 4) == 4) {
        return;
    }
    owner = *(u8**)(callback + 8);
    if (owner == NULL) {
        return;
    }
    if ((owner[0x18] & 8) == 8) {
        return;
    }
    if (model != *(void**)(owner + 0x24)) {
        return;
    }
    animIndex = *(u32*)(callback + 4);
    if (animIndex != *(s16*)(owner + 0x1A) &&
        animIndex != *(s16*)(owner + 0x1E) &&
        animIndex != *(s16*)(owner + 0x1C)) {
        return;
    }
    switch (owner[0x19]) {
    case 2:
    case 3:
        GSmodelLinkTexAnimToAnim(model, 1);
        if (GSmodelIsAnimating(model)) {
            frame = GSmodelGetAnimFrame(model);
            rate = GSmodelGetAnimRate(model);
            type = GSmodelGetAnimType(model);
            GSmodelGetAnimIndex(model, (u32*)&animIndex, &unused);
            GSmodelSetTexAnimIndex(model, animIndex);
            GSmodelSetTexAnimFrame(model, frame + rate * GSgetFrameStep());
            GSmodelSetTexAnimRate(model, rate);
            GSmodelSetTexAnimType(model, type);
            GSmodelStartTexAnimation(model);
        }
        owner[0x19] = 0;
        break;
    case 1:
        owner[0x19] = 6;
        break;
    default:
        GSlogWrite(lbl_802799D8);
        break;
    }
}

#endif

/* The rest of the range is linked from its own carves; this unit owns only
 * 0x801DE698 - 0x801DEE14. */
#if defined(GS_RANGE_EXACT_801DEE14_801DF160)

/* RULE-EXCEPTION(title-path): shared named pool literals in this linked carve;
 * see docs/RULE_EXCEPTIONS.md. */
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

/* Retail expands the fn_801DF160 lookup here. */
static inline s32 wazaDefaultAnimIndex(u8* obj) {
    u8* base = *(u8**)(obj + 0x2C);
    u8* table = base;
    s32* entry;
    s32 count;

    if ((obj[0x18] & 2) == 2 && *(u16*)(obj + 0x14) > 0x10) {
        table = base + 0xD40;
        if (*(s32*)(table + 0x94) == 1) {
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
    s32 animIndex;
    u32 currentIndex;
    u32 unused;
    void* model;

    if (effect == NULL) {
        return;
    }
    if (effect[0x16] != 0) {
        return;
    }
    if ((effect[0x18] & 8) == 8) {
        return;
    }
    model = *(void**)(effect + 0x24);
    if ((effect[0x18] & 4) != 4) {
        effect[0x19] = 0;
        GSmodelLinkTexAnimToAnim(model, 1);
    }
    animIndex = wazaDefaultAnimIndex(effect);
    GSmodelGetAnimIndex(model, (u32*)&currentIndex, (u32*)&unused);
    GSmodelSetAnimType(model, arg1);
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    if (animIndex != (s32)currentIndex || (u8)arg2 != 0 ||
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
    /* RULE-EXCEPTION(title-path): retain the original shared literal pool;
     * see docs/RULE_EXCEPTIONS.md. */
    extern f32 lbl_8047E3C8;
    extern f32 lbl_8047E3CC;
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
    GSmodelSetAnimRate(model, lbl_8047E3C8);
    GSmodelSetAnimFrame(model, lbl_8047E3CC);
    GSmodelStartAnimation(model);
}

#endif

#if defined(GS_RANGE_801DE698_WHOLE_RANGE)
/**
 * fn_801DF160 - Waza weather effect update.
 * Address: 0x801DF160 | Size: 0x70
 * Linked from gs_range_exact_801DF160.c; kept here for the candidate.
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
        if (*(s32*)(table + 0x94) == 1) {
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
    extern u32 GSgetFrameStep(void);
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

    if (fn_801DAC54(effect) != 0) {
        return;
    }
    if ((effect[0x18] & 8) == 0) {
        return;
    }
    if (effect[0x19] != 0) {
        *(u16*)(effect + 0x20) = 0;
        return;
    }
    if (*(s16*)(effect + 0x1C) < 0) {
        return;
    }
    *(u16*)(effect + 0x20) += (u16)GSgetFrameStep();
    timer = *(u16*)(effect + 0x20);
    if (timer < 10) {
        return;
    }
    if (timer >= 180) {
        threshold = lbl_8047E3D8;
    } else if (timer >= 60) {
        threshold = lbl_8047E3DC;
    } else {
        f32 t = (f32)(timer - 10) / lbl_8047E3E0;
        threshold = t * (lbl_8047E3E4 - t) * lbl_8047E3DC;
    }
    if (threshold >= fn_800E0BE4()) {
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
void std087_sequenceUnk87(s32 slot, s32 abilityID) {
    typedef struct AbilityItemWeight {
        u16 item;
        u16 weight;
    } AbilityItemWeight;
    typedef struct AbilityVec {
        f32 x;
        f32 y;
        f32 z;
    } AbilityVec;
    extern u32* lbl_80478EB0;
    extern AbilityItemWeight* lbl_80478EB4;
    extern f32 lbl_8047E3F0;
    extern u16 GSrand(void);
    extern void* peopleIsInGroup(s32, s32);
    extern void* peopleSearchID(void*);
    extern void* peopleGetPosition(void*);
    extern void heroMoveGetHeroPos(void*);
    extern void* peopleGetPositionPtr(void*);
    extern void GSvecSub(void*, void*, void*);
    extern f64 atan2(f64, f64);
    extern void peopleStartTurn(s32, s32, f32, f32);
    extern void peopleMoveCheck(s32, s32, u8);
    extern u32 std082_flagGetValue(u32);
    extern void _flagSet(u32, s32);
    extern u8 std081_flagCheckTrue(u32);
    extern void winMsgOpenFieldWithSE(s32, s32, s32, s32);
    extern void std130_soundUnk_801653cc(s32, s32, s32);
    extern void msgctrlSetValue(s32, u32);
    extern void winMsgOpen(s32, s32, s32, s32);
    extern s32 heroItemAddItemDataId(void*, u32, u32, s32);
    extern void pcboxDelItem(s32, u32, u32);
    AbilityItemWeight* weights;
    AbilityVec direction;
    AbilityVec heroPosition;
    u32 state;
    s32 running;
    u32 totalWeight;
    u32 i;
    s32 selectedItem;
    u32 selectedData;
    u32 cumulative;
    u32 randomValue;
    u32 count;
    s32 result;
    void* person;

    state = 0;
    running = 1;
    totalWeight = 0;
    count = *lbl_80478EB0;
    weights = lbl_80478EB4;
    for (i = 0; i < count; i++) {
        totalWeight += (weights++)->weight;
    }

    randomValue = (u16)GSrand() % totalWeight;
    cumulative = 0;
    {
        AbilityItemWeight* entry = lbl_80478EB4;

        for (i = 0; i < count; i++, entry++) {
            cumulative += entry->weight;
            if (randomValue < cumulative) {
                selectedItem = entry->item;
                selectedData = entry->item;
                break;
            }
        }
    }

    do {
        switch (state) {
        case 0:
            person = peopleSearchID(peopleIsInGroup(slot, abilityID));
            if (person != NULL) {
                peopleGetPosition(person);
                heroMoveGetHeroPos(&heroPosition);
                GSvecSub(&direction, &heroPosition,
                            peopleGetPositionPtr(person));
                peopleStartTurn(slot, abilityID,
                            (f32)atan2(direction.x, direction.z),
                            lbl_8047E3F0);
                peopleMoveCheck(slot, abilityID, 1);
            }

            if (std082_flagGetValue(0xD0) != 0) {
                if (std082_flagGetValue(0xD1) >= 500) {
                    _flagSet(0xD1, 0);
                    result = 1;
                } else if (std081_flagCheckTrue(0xAFE) != 0) {
                    _flagSet(0xAFE, 0);
                    result = -1;
                } else {
                    result = 0;
                }
            } else {
                _flagSet(0xD0, 1);
                result = 1;
            }

            if (result > 0) {
                state = 1;
            } else if (result < 0) {
                state = 3;
            } else {
                state = 4;
            }
            break;
        case 1:
            winMsgOpenFieldWithSE(0x5571, 1, 0, 1);
            state = 2;
            break;
        case 2:
            std130_soundUnk_801653cc(0x3CA, 0, 0xFF);
            msgctrlSetValue(0x2D, selectedItem);
            winMsgOpen(3, 0x3CB8, 1, 0);
            result = heroItemAddItemDataId(NULL, selectedData, 1, -1);
            if (result != 0 && result > 0) {
                /* Masks rather than (u16) casts: MWCC's frontend hoists a cast
                 * of selectedData out of the state loop, which reorders the
                 * preheader; the mask is hoisted by the backend, as retail. */
                pcboxDelItem(0, selectedData & 0xFFFF, result & 0xFFFF);
            }
            state = 7;
            break;
        case 3:
            winMsgOpenFieldWithSE(0x5573, 1, 0, 1);
            state = 7;
            break;
        case 4:
            winMsgOpenFieldWithSE(0x5574, 1, 0, 1);
            state = 7;
            break;
        case 7:
            running = 0;
            break;
        case 5:
        case 6:
        default:
            break;
        }
    } while (running != 0);
}

#endif
