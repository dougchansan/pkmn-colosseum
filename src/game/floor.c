/**
 * @file floor.c
 * @brief floor -- fade/transition control, object list, character init,
 * model load/animation, and floor resource accessors.
 *
 * Sixth of six translation units recovered from the former
 * game/gs_field_colquery.c CodeCandidate bucket (0x8010F6A0-0x801140DC).
 * Unusually large vs. XD's floor.cpp TU size (our span ~0x1D5C bytes vs.
 * XD's ~0x3244 bytes) -- interpreted as Colosseum's floor.cpp/field-
 * control file carrying substantially more Colosseum-specific logic
 * (battle-arena field transitions etc.) than XD's slimmed-down version
 * of the same file. The 8 anchors inside it (floorCheckFightKind ..
 * EvlogSet, the last of which lands just past our end boundary) appear
 * in exactly the same relative order as in XD, which is strong evidence
 * this is still one TU rather than several.
 *
 * fn_801123D4 and fn_801129CC previously carried invented
 * "GSfield_ResourceInit" / "GSfield_UpdateObjects" names/signatures
 * from an earlier bad campaign pass (same class of issue documented in
 * include/game/gs_colsys.h); reverted to the standard fn_<addr>
 * placeholder since no confirmed symbols.txt name exists yet for
 * either.
 *
 * Address range: 0x80112380 - 0x801140DC
 */
#include "dolphin/types.h"
#include "game/world/gs_field.h"
#include "game/gs_field_colquery_types.h"
#include "game/gs_model_anim.h"

/* 0x80112380 | 0x54 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
s32 floorCheckFightKind(u32 id) {
#pragma optimization_level 4
    extern void floorDataBiosGetPtr(void);
    extern s32 floorDataBiosGetFloorKind(void);
    s32 ready = 0;

    if (id != 0xFFFFFFFF) {
        floorDataBiosGetPtr();
        switch (floorDataBiosGetFloorKind() & 0xFF) {
        case 2:
            ready = 1;
            break;
        }
    }
    return ready;
}
#pragma peephole on
#pragma pop

/* 0x801123D4 | 0x32C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801123D4(u32 floorDataEntry, u32 loadMode) {
    /* TODO: match -- 812 bytes at 0x801123D4 */
}
#pragma pop

/* 0x80112700 | 0x4C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112700(void) {
#pragma optimization_level 4
    extern void floorDataBiosGetCurrentPtr(void);
    extern u32 fn_801159A8(void);
    extern void fn_800F7318(s32, u32, s32, s32, s32, ...);
    u32 id;

    floorDataBiosGetCurrentPtr();
    if ((id = fn_801159A8()) != 0) {
        fn_800F7318(0xF, id, 0x1000, 1, 0, 0);
    }
}
#pragma pop

/* 0x8011274C | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void floorCheckFade(void) {
    extern void fadeCheck(s32);
    extern void fn_800D3074(s32);
    extern u8 lbl_80478DD0;

    fadeCheck(1);
    lbl_80478DD0 = 0;
    fn_800D3074(1);
}
#pragma peephole on
#pragma pop

/* 0x80112780 | 0x3C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void fn_80112780(void) {
#pragma optimization_level 4
    extern void fn_800F7434(void* callback, s32 arg, ...);
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;
    void* callback;

    callback = (void*)state->transitionBeginCallback;
    if (callback != NULL) {
        fn_800F7434(callback, 0);
    }
}
#pragma peephole on
#pragma pop

/* 0x801127BC | 0x88 */
void fn_801127BC(void) {
    extern u8 lbl_80478DD0;
    extern void floorDataBiosGetCurrentPtr(void);
    extern u8 floorDataBiosGetFloorKind(void);
    extern void fn_800D3074(s32);
    extern u32 fn_800FF560(void);
    extern void fn_800FF2A0(u32, u32, void*);
    extern void fn_80112844(void);
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;
    u8 kind;

    lbl_80478DD0 = 1;
    floorDataBiosGetCurrentPtr();
    kind = floorDataBiosGetFloorKind();
    if (kind >= 5 && kind < 7) {
        fn_800D3074(1);
    } else {
        fn_800D3074(2);
    }
    if (state->transitionPollCallback != 0) {
        fn_800FF2A0(0, fn_800FF560(), (void*)fn_80112844);
    }
}

/* 0x80112844 | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void fn_80112844(void) {
#pragma optimization_level 4
    extern void fn_800F7434(void* callback, s32 arg, ...);
    extern void fn_800FF0A0(void (*callback)(void));
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;
    void* callback;

    callback = (void*)state->transitionPollCallback;
    if (callback != NULL) {
        fn_800F7434(callback, 0);
        fn_800FF0A0(fn_80112844);
    }
}
#pragma peephole on
#pragma pop

/* 0x8011288C | 0x14 */
#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void floorSetFadeScript(u32 a, u32 b) {
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;

    state->transitionPollCallback = b;
    state->transitionBeginCallback = a;
}
#pragma pop

/* 0x801128A0 | 0x10C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void fn_801128A0(void) {
#pragma optimization_level 4
    extern u32 lbl_80272094[];
    extern u8 lbl_80408378[];
    extern void fn_800FF3C0(s32, s32, void*);
    extern void fn_800FF178(s32, s32, void*);
    extern void fn_800FF4D4(void*, u32);
    extern s32 fn_800057A8(void);
    extern void fn_800FF788(u32);
    extern void fn_800FF784(u32);
    extern void fn_80118020(void);
    extern void fn_801129CC(void);
    extern void fn_801129AC(void);
    u32 sp8[3];
    u32 r;

    sp8[0] = lbl_80272094[0];
    sp8[1] = lbl_80272094[1];
    sp8[2] = lbl_80272094[2];
    fn_800FF3C0(0, 0x5000, (void*)fn_801129CC);
    fn_800FF178(0xFF, 0x5000, (void*)fn_801129AC);
    fn_800FF4D4(sp8, 1);
    fn_800FF4D4(sp8, 2);
    switch (fn_800057A8()) {
    case 5:
        r = 0x3E4;
        break;
    case 4:
        r = 0x320;
        break;
    case 1:
        r = 0x3E6;
        break;
    case 2:
        r = 0x3E7;
        break;
    case 3:
    default:
        r = 0x399;
        break;
    }
    *(u32*)(lbl_80408378 + 0x0) = 0;
    *(u32*)(lbl_80408378 + 0x4) = r;
    *(u8*)(lbl_80408378 + 0x8) = 1;
    *(u32*)(lbl_80408378 + 0xC) = 0;
    fn_800FF788(r);
    fn_800FF784(r);
    fn_80118020();
}
#pragma peephole on
#pragma pop

/* 0x801129AC | 0x20 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801129AC(void) {
    extern void mailMainReceiveTerminate(void);
    mailMainReceiveTerminate();
}
#pragma pop

/* 0x801129CC | 0x5C0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801129CC(void) {
    /* TODO: match -- 1472 bytes at 0x801129CC */
}
#pragma pop

/* 0x80112F8C | 0x60 */
#pragma push
#pragma peephole off
void fn_80112F8C(void) {
    extern void* floorDataBiosGetMainFunc(void);
    extern u32 fn_800FF560(void);
    extern void GSthreadCreate(s32 a, u32 b, u32 c, u32 d, u32 e, void* f);
    extern void fn_800FF0A0(void (*callback)(void));
    void* obj;

    obj = floorDataBiosGetMainFunc();
    if (obj != NULL) {
        GSthreadCreate(1, fn_800FF560(), 0x4000, 1, 1, obj);
    }
    fn_800FF0A0(fn_80112F8C);
}
#pragma pop

/* 0x80112FEC | 0x25C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80112FEC(void) {
    /* TODO: match -- 604 bytes at 0x80112FEC */
}
#pragma pop

/* 0x80113248 | 0x29C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void _floorInitCharacters__FP11GSfloor_dd_(void* a) {
#pragma optimization_level 4
    extern u32 floorDataBiosGetGroupID(void);
    extern u32 floorDataBiosGetCharNum(void* a);
    extern u8* fn_8011711C(u32 i);
    extern void floorCharacterBiosGetPeopleInfoPtr(void);
    extern s32 fn_8018F6B4(void);
    extern u8 fn_800FF548(void);
    extern u32 fn_8018E050(u32 model, u32 i, s32 x);
    extern u32 fn_8018D998(u32 model, u32 i);
    extern s32 fn_80183958(u32 model, u32 i);
    extern void fn_801837D8(u32 model, u32 i, s32 a, u32 b, s32 c);
    extern void fn_8018C7C8(u32 model, u32 i, s32 flag);
    extern void floorCharacterBiosGetPos(u8* obj, void* out);
    extern void fn_8018C0A8(u32 model, u32 i, void* p);
    extern void floorCharacterBiosGetRot(u8* obj, void* out);
    extern void fn_8018BF24(u32 model, u32 i, void* p);
    extern void fn_8018CB5C(u32 model, u32 i);
    extern s32 floorCharacterBiosGetVisibility(u8* obj);
    extern void fn_8018C1E8(u32 model, u32 i, s32 x);
    extern s32 floorCharacterBiosGetLoadInit(u8* obj);
    extern void fn_8018CA20(u32 model, u32 i, s32 x);
    extern u8 floorCharacterBiosGetMoveType(u8* obj);
    extern void fn_80183B44(u32 model, u32 i, f32 x);
    extern void fn_801839A0(u32 model, u32 i, f32 x, f32 y);
    extern u8 floorCharacterBiosGetTalkStartType(u8* obj);
    extern u8 floorCharacterBiosGetTalkEndType(u8* obj);
    extern void fn_8018C69C(u32 model, u32 i, s32 flag);
    extern void fn_80188F78(u32 model, u32 i);
    extern const char lbl_802720CC[];
    extern const char lbl_8035B888[];
    extern f32 lbl_8047CF88;
    extern f32 lbl_8047CF8C;
    extern f32 lbl_8047CF90;
    u32 model;
    u32 count;
    u32 i;
    u32 result;
    u8* obj;
    f32 v14[3];
    f32 v8[3];

    model = floorDataBiosGetGroupID();
    count = floorDataBiosGetCharNum(a);
    for (i = 0; i < count; i++) {
        obj = fn_8011711C(i);
        floorCharacterBiosGetPeopleInfoPtr();
        result = fn_8018F6B4();
        if (fn_800FF548() == 0) {
            result = fn_8018E050(model, i, result);
        } else {
            result = fn_8018D998(model, i);
        }
        if (result == 0) {
            GSlogWrite(lbl_802720CC, lbl_8035B888);
        } else if (fn_800FF548() != 1) {
            {
                s32 emitId;

                emitId = fn_80183958(model, i);
                fn_801837D8(model, i, emitId, result, 0);
            }
            fn_8018C7C8(model, i, 4);
            fn_8018C7C8(model, i, 8);
            floorCharacterBiosGetPos(obj, v14);
            fn_8018C0A8(model, i, v14);
            floorCharacterBiosGetRot(obj, v8);
            fn_8018BF24(model, i, v8);
            fn_8018CB5C(model, i);
            fn_8018C1E8(model, i, floorCharacterBiosGetVisibility(obj));
            fn_8018CA20(model, i, floorCharacterBiosGetLoadInit(obj));
            switch (floorCharacterBiosGetMoveType(obj)) {
            case 0:
            case 1:
                break;
            case 2:
                fn_80183B44(model, i, lbl_8047CF88);
                break;
            case 3:
                fn_801839A0(model, i, lbl_8047CF8C, lbl_8047CF90);
                break;
            }
            switch (floorCharacterBiosGetTalkStartType(obj)) {
            case 0:
                break;
            case 1:
                fn_8018C7C8(model, i, 0x10);
                break;
            case 2:
                fn_8018C7C8(model, i, 0x20);
                break;
            }
            switch (floorCharacterBiosGetTalkEndType(obj)) {
            case 0:
                fn_8018C69C(model, i, 0x40);
                break;
            case 1:
                fn_8018C7C8(model, i, 0x40);
                break;
            }
            if (((u32)((*obj >> 5) & 1)) != 0U) {
                fn_80188F78(model, i);
            }
        }
    }
}
#pragma peephole on
#pragma pop

/* 0x801134E4 | 0x294 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void floorInitMap(void) {
    /* TODO: match -- 660 bytes at 0x801134E4 */
}
#pragma pop

/* 0x80113778 | 0xB0 */
void floorChangePos(u32 floorId, s16 direction, f32 x, f32 y, f32 z) {
    extern const f32 lbl_8047CF70;
    extern const f32 lbl_8047CF74;
    extern u32 fn_800FF56C(void);
    extern void fn_800FF58C(u32);
    GSFieldColqueryState* state = (GSFieldColqueryState*)lbl_80408378;

    if (floorId == 0) {
        return;
    }
    set__5GSvecFfff((void*)(lbl_80408378 + 0x10), x, y, z);
    set__5GSvecFfff((void*)(lbl_80408378 + 0x1C), lbl_8047CF70,
                    lbl_8047CF74 * direction, lbl_8047CF70);
    state->field_00 = fn_800FF56C();
    state->field_04 = floorId;
    state->field_08 = 0;
    fn_800FF58C(floorId);
}

/* 0x80113828 | 0x64 */
#pragma push
#pragma peephole off
void floorLink(u32 arg0, s32 arg1) {
    extern u32 fn_800FF56C(void);
    extern void fn_800FF58C(s32);

    if (arg0 != 0) {
        *(u32*)(lbl_80408378 + 0x0) = fn_800FF56C();
        *(s32*)(lbl_80408378 + 0x4) = arg0;
        *(u8*)(lbl_80408378 + 0x8) = 1;
        *(s32*)(lbl_80408378 + 0xC) = arg1;
        fn_800FF58C(arg0);
    }
}
#pragma peephole on
#pragma pop

/* 0x8011388C | 0xA0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void floorLinkWithSE(void* a, void* b, s32 c) {
#pragma optimization_level 4
    extern u8 lbl_80408378[];
    extern u32 fn_800FF56C(void);
    extern void fn_80166A28(u32);
    extern void fn_800FF58C(void*);
    u32 sndId;

    if (a == NULL) {
        return;
    }
    *(u32*)(lbl_80408378 + 0x0) = fn_800FF56C();
    *(void**)(lbl_80408378 + 0x4) = a;
    *(u8*)(lbl_80408378 + 0x8) = 1;
    *(void**)(lbl_80408378 + 0xC) = b;
    switch (c) {
    case 0:
        sndId = 0;
        break;
    case 1:
    default:
        sndId = 0x3F9;
        break;
    }
    if (sndId != 0) {
        fn_80166A28(sndId);
    }
    fn_800FF58C(a);
}
#pragma peephole on
#pragma pop

/* 0x8011392C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 floorGetNextPosIndex(void) {
    return *(u32*)(lbl_80408378 + 0xC);
}
#pragma pop

/* 0x8011393C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 floorGetNextFloorID(void) {
    return *(u32*)(lbl_80408378 + 0x4);
}
#pragma pop

/* 0x8011394C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 floorGetPrevFloorID(void) {
    return *(u32*)(lbl_80408378 + 0x0);
}
#pragma pop

/* 0x8011395C | 0x10 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void floorSetPrevFloorID(u32 value) {
    asm {
        lis r4, lbl_80408378@ha
        addi r4, r4, lbl_80408378@l
        stw r3, 0(r4)
    }
}
#pragma pop

/* 0x8011396C | 0x50 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
s32 fn_8011396C(s32 param) {
#pragma optimization_level 4
    extern u32 floorDataBiosGetPtr(void);
    extern s32 fn_80115840(void);

    switch (param) {
    case 0xFD:
    case 0xFE:
    case 0xFF:
        return 0;
    }
    if (floorDataBiosGetPtr() == 0) {
        return 0;
    }
    return fn_80115840();
}
#pragma scheduling on
#pragma pop

/* 0x801139BC | 0x50 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void fn_801139BC(void) {
    extern void fn_8018B76C(s32, s32, s32, s32, s32);
    extern void fn_80117154(void);

    fn_8018B76C(0, 0x64, 1, 0, 1);
    fn_8018B76C(0, 0x65, 1, 0, 1);
    fn_80117154();
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma pop

/* 0x80113A0C | 0x178 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void _floorUpdate__FUi14FloorEnterMode(void) {
    /* TODO: match -- 376 bytes at 0x80113A0C */
}
#pragma pop

/* 0x80113B84 | 0x18C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void _floorInitialize__FUi14FloorEnterMode(void) {
    /* TODO: match -- 396 bytes at 0x80113B84 */
}
#pragma pop

/* 0x80113D10 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
u32 _floorUnloadModel__FPvUlUl(u32 group) {
#pragma optimization_level 4
    extern void GSmodelFree();

    GSmodelFree(group);
    return 1;
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma pop

/* 0x80113D34 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
#pragma scheduling off
void floorOpenModel(u32 unused, u32 modelIndex) {
#pragma optimization_level 4
    floorOpenObject(modelIndex);
}
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma scheduling on
#pragma pop

/* 0x80113D58 | 0x1F0 */
#pragma push
#pragma optimization_level 4
#pragma peephole off
void* floorOpenObject(u32 modelIndex) {
    extern const char lbl_80272088[];
    extern const char lbl_8035B868[];
    extern f32 lbl_8047CF98;
    extern void* fn_800F92D4(u32);
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern void* GSmodelLoad(void*);
    extern void GSmodelSetVisibility(void*, u32);
    extern void GSmodelSetAnimIndex(void*, u32);
    extern void GSmodelSetAnimRate(void*, f32);
    extern void GSmodelStartAnimation(void*);
    extern void GSmodelSetTexAnimIndex(void*, u32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSmodelStartTexAnimation(void*);
    const char* strings = lbl_80272088;
    u8 special = 0;
    void* archive;
    void* pub;
    void* model;

    switch (modelIndex) {
    case 3:
    case 100:
        modelIndex = 0x00F71000;
        break;
    case 4:
    case 101:
        modelIndex = 0x00F31000;
        break;
    default:
        if (((modelIndex >> 9) & 0x3F) == 2) {
            special = 1;
        }
        break;
    }

    if (modelIndex == 0) {
        GSlogWrite(strings + 0xDC, lbl_8035B868);
        return NULL;
    }

    archive = fn_800F92D4(modelIndex);
    if (special != 0) {
        if (archive == NULL) {
            GSlogWrite(strings + 0xF8, lbl_8035B868);
            return NULL;
        }
        pub = HSD_ArchiveGetPublicAddress(archive, strings + 0x28);
        if (pub == NULL) {
            GSlogWrite(strings + 0x118, lbl_8035B868);
            return NULL;
        }
        archive = *(void**)pub;
        if (*(void**)archive == NULL) {
            GSlogWrite(strings + 0x140, lbl_8035B868);
            return NULL;
        }
        archive = *(void**)archive;
    }

    model = GSmodelLoad(archive);
    if (model == NULL) {
        GSlogWrite(strings + 0x64, lbl_8035B868);
        GSlogWrite(strings + 0x15C);
    } else {
        GSmodelSetVisibility(model, 1);
        if ((u8)GSmodelCanAnimate(model) != 0) {
            GSmodelSetAnimIndex(model, 0);
            GSmodelSetAnimRate(model, lbl_8047CF98);
            GSmodelStartAnimation(model);
        }
        if ((u8)GSmodelCanTexAnimate(model) != 0) {
            GSmodelSetTexAnimIndex(model, 0);
            GSmodelSetTexAnimRate(model, lbl_8047CF98);
            GSmodelStartTexAnimation(model);
        }
    }

    return model;
}
#pragma pop

/* 0x80113F48 | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80113F48(void) {
    extern void floorDataBiosGetCurrentPtr(void);
    extern void floorDataBiosGetGroupID(void);

    floorDataBiosGetCurrentPtr();
    floorDataBiosGetGroupID();
}
#pragma pop

/* 0x80113F6C | 0x48 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void* floorGetResource(u32 key, u32 arg) {
#pragma optimization_level 4
    extern void* floorDataBiosGetPtr(u32);
    extern u32 floorDataBiosGetGroupID(void*);
    extern void* GSresGetResource(u32, u32);
    void* resource;

    resource = floorDataBiosGetPtr(key);
    if (resource == NULL) {
        return NULL;
    }
    return GSresGetResource(floorDataBiosGetGroupID(resource), arg);
}
#pragma pop

/* 0x80113FB4 | 0x34 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
u32 fn_80113FB4(u32 key) {
#pragma optimization_level 4
    extern void* floorDataBiosGetPtr(u32);
    extern u32 floorDataBiosGetGroupID(void*);
    void* resource;

    resource = floorDataBiosGetPtr(key);
    if (resource == NULL) {
        return 0;
    }
    return floorDataBiosGetGroupID(resource);
}
#pragma pop

/* 0x80113FE8 | 0xE0 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#pragma peephole off
void fn_80113FE8(void) {
#pragma optimization_level 4
    extern u8 lbl_80408378[];
    extern u32 gamedatasaveGetStatus(s32, s32);
    extern u32 fn_800FF56C(void);
    extern void fn_800FF58C(u32);
    u8* state = lbl_80408378;
    u32 a;
    u32 b;
    u8 c;

    if (*(u8*)(state + 0x51) != 0) {
        a = *(u32*)(state + 0x48);
        b = *(u32*)(state + 0x4C);
        c = *(u8*)(state + 0x50);
    } else {
        a = gamedatasaveGetStatus(0, 5);
        b = gamedatasaveGetStatus(0, 7);
        c = (u8)gamedatasaveGetStatus(0, 8);
    }
    if (a != 0) {
        *(u32*)(lbl_80408378 + 0x0) = fn_800FF56C();
        *(u32*)(lbl_80408378 + 0x4) = a;
        *(u8*)(lbl_80408378 + 0x8) = 1;
        *(u32*)(lbl_80408378 + 0xC) = c;
        fn_800FF58C(a);
    }
    *(u8*)(state + 0x51) = 0;
    *(u32*)(lbl_80408378 + 0x0) = b;
}
#pragma peephole on
#pragma pop

/* 0x801140C8 | 0x14 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801140C8(void) {
    *(u8*)(lbl_80408378 + 0x51) = 0;
}
#pragma pop
