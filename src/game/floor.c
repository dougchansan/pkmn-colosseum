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
void fn_801123D4(u32 floorDataEntry, u32 loadMode) {
    extern void GSscene_SetMode(s32);
    extern void fn_800D36B4(void*);
    extern u8 floorDataBiosGetFloorKind(void*);
    extern s32 fn_8017B2CC(s32);
    extern void fn_800F915C(s32);
    extern void fn_8017B1CC(s32);
    extern void fn_8017B13C(s32, u32);
    extern void fn_8017B3E4(s32);
    extern void _threadSwitch(void);
    extern u32 heroMoveGetKenObjID(void);
    extern void GSlogWrite(const char*, ...);
    extern u32 lbl_802720A0[];
    extern const char lbl_802720BC[];
    u32 sceneData[4];
    u32 setupData[4];
    u32 objectId;
    s32 status;
    u8 kind;

    sceneData[0] = lbl_802720A0[0];
    sceneData[1] = lbl_802720A0[1];
    sceneData[2] = lbl_802720A0[2];
    sceneData[3] = lbl_802720A0[3];
    GSscene_SetMode(3);
    setupData[0] = sceneData[0];
    setupData[1] = sceneData[1];
    setupData[2] = sceneData[2];
    setupData[3] = sceneData[3];
    fn_800D36B4(setupData);

    kind = floorDataBiosGetFloorKind((void*)floorDataEntry);
    if (kind == 6) {
        status = fn_8017B2CC(0x9A);
        if (status == 1) {
            do {
                _threadSwitch();
                status = fn_8017B2CC(0x9A);
            } while (status == 1);
        }
        if (status == 0) {
            fn_800F915C(0x9A);
            fn_8017B1CC(0x9A);
        }

        status = fn_8017B2CC(0x0C);
        if (status == 1) {
            do {
                _threadSwitch();
                status = fn_8017B2CC(0x0C);
            } while (status == 1);
        }
        if (status == 0) {
            fn_800F915C(0x0C);
            fn_8017B1CC(0x0C);
        }

        status = fn_8017B2CC(0x0B);
        if (status == 1) {
            do {
                _threadSwitch();
                status = fn_8017B2CC(0x0B);
            } while (status == 1);
        }
        if (status == 0) {
            fn_800F915C(0x0B);
            fn_8017B1CC(0x0B);
        }
    } else if (kind == 1) {
        status = fn_8017B2CC(0x0C);
        if (status == 1) {
            do {
                _threadSwitch();
                status = fn_8017B2CC(0x0C);
            } while (status == 1);
        }
        if (status == 0) {
            fn_800F915C(0x0C);
            fn_8017B1CC(0x0C);
        }

        objectId = heroMoveGetKenObjID();
        if (objectId != 0x00F70400 && fn_8017B2CC(0x9A) != 0) {
            fn_8017B13C(0x9A, objectId);
            do {
                status = fn_8017B2CC(0x9A);
                if (status < 0) {
                    GSlogWrite(lbl_802720BC);
                }
                if (status == 0) {
                    break;
                }
                _threadSwitch();
            } while (TRUE);
        }
        if (fn_8017B2CC(0x0B) != 0) {
            fn_8017B3E4(0x0B);
            do {
                status = fn_8017B2CC(0x0B);
                if (status < 0) {
                    GSlogWrite(lbl_802720BC);
                }
                if (status == 0) {
                    break;
                }
                _threadSwitch();
            } while (TRUE);
        }
    } else if (kind == 2) {
        status = fn_8017B2CC(0x0B);
        if (status == 1) {
            do {
                _threadSwitch();
                status = fn_8017B2CC(0x0B);
            } while (status == 1);
        }
        if (status == 0) {
            fn_800F915C(0x0B);
            fn_8017B1CC(0x0B);
        }
        if (fn_8017B2CC(0x0C) != 0) {
            fn_8017B3E4(0x0C);
            do {
                status = fn_8017B2CC(0x0C);
                if (status < 0) {
                    GSlogWrite(lbl_802720BC);
                }
                if (status == 0) {
                    break;
                }
                _threadSwitch();
            } while (TRUE);
        }
    }
}

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
    s32 kind;

    lbl_80478DD0 = 1;
    floorDataBiosGetCurrentPtr();
    kind = floorDataBiosGetFloorKind();
    switch (kind) {
    case 5:
    case 6:
        fn_800D3074(1);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    default:
        fn_800D3074(2);
        break;
    }
    if (*(u32*)(lbl_80408378 + 0x28) != 0) {
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
void fn_80112FEC(void* floor)
{
    typedef struct FloorObjectEntry {
        u8 flags;
        u8 pad_01;
        s16 rotation;
        u16 floorId;
        u16 eventFlag;
        u16 hideFlag;
        u8 pad_0A[6];
        f32 x;
        f32 y;
        f32 z;
    } FloorObjectEntry;
    extern u32 floorDataBiosGetGroupID(void);
    extern u8 fn_800FF548(void);
    extern void* fn_8018E050(u32, u32, u32);
    extern void* fn_8018D998(u32, u32);
    extern void set__5GSvecFfff(void*, f32, f32, f32);
    extern void fn_8018C0A8(u32, u32, void*);
    extern void fn_8018BF24(u32, u32, void*);
    extern void fn_8018CB5C(u32, u32);
    extern void fn_8018C7C8(u32, u32, s32);
    extern void fn_8018C1E8(u32, u32, s32);
    extern void* GSresGetResource(u32, u32);
    extern void GSmodelClearShadowFlags(void*, u32);
    extern u8 fn_801902E0(u16);
    extern void floorEventCtrlTresure(u32, u32, u32);
    extern void fn_801837D8(u32, u32, s32, u32, s32);
    extern u32* lbl_80478EB8;
    extern FloorObjectEntry* lbl_80478EBC;
    extern const f32 lbl_8047CF70;
    extern const f32 lbl_8047CF74;
    u32 group;
    u32 objectIndex;
    u32 i;

    group = floorDataBiosGetGroupID();
    objectIndex = 0;
    for (i = 0; i < *lbl_80478EB8; i++) {
        FloorObjectEntry* entry = &lbl_80478EBC[i];
        u32 peopleInfo;
        u32 resourceId;
        void* object;
        f32 position[3];
        f32 rotation[3];

        if (entry->floorId != *(u32*)((u8*)floor + 0xC)) {
            continue;
        }
        switch ((entry->flags >> 5) & 7) {
        case 1:
            peopleInfo = 0x03770400;
            break;
        case 2:
            peopleInfo = 0x03780400;
            break;
        case 3:
            peopleInfo = 0x03790400;
            break;
        default:
            continue;
        }

        resourceId = 0x7FFF0000 | objectIndex++;
        if (fn_800FF548() == 0) {
            object = fn_8018E050(group, resourceId, peopleInfo);
        } else {
            object = fn_8018D998(group, resourceId);
        }
        if (object == NULL || fn_800FF548() == 1) {
            continue;
        }

        set__5GSvecFfff(position, entry->x, entry->y, entry->z);
        fn_8018C0A8(group, resourceId, position);
        set__5GSvecFfff(rotation, lbl_8047CF70,
                        lbl_8047CF74 * entry->rotation,
                        lbl_8047CF70);
        fn_8018BF24(group, resourceId, rotation);
        fn_8018CB5C(group, resourceId);
        fn_8018C7C8(group, resourceId, 4);
        fn_8018C1E8(group, resourceId, 1);

        if (((entry->flags >> 5) & 7) == 2) {
            object = GSresGetResource(group, resourceId);
            if (object != NULL) {
                GSmodelClearShadowFlags(object, 1);
            }
        }
        if (entry->eventFlag != 0) {
            if (fn_801902E0(entry->eventFlag)) {
                floorEventCtrlTresure(group, resourceId, 0);
            } else {
                floorEventCtrlTresure(group, resourceId, 1);
                if (entry->hideFlag != 0 &&
                    !fn_801902E0(entry->hideFlag)) {
                    fn_8018C1E8(group, resourceId, 0);
                }
                fn_801837D8(group, resourceId, 0x0596000A,
                            entry->eventFlag, entry->hideFlag);
            }
        }
    }
}

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
void floorInitMap(u32 group, u32 floorId)
{
    typedef struct FloorSceneData {
        void** models;
        u32 pad_04;
        void** lights;
    } FloorSceneData;
    extern void* GSresGetResource(u32, u32);
    extern void* HSD_ArchiveGetPublicAddress(void*, const char*);
    extern u32 floorReadMakeModelResID(u32);
    extern u32 floorReadMakeLightResID(u32);
    extern u32 floorReadMakeCameraResID(u32);
    extern u32 floorReadMakeFogResID(u32);
    extern void fn_800F9210(u32, u32);
    extern void GSresRegisterResource(void*, u32, u32, void*);
    extern u32 _floorUnloadModel__FPvUlUl(u32);
    extern void GSmodelSetVisibility(void*, u32);
    extern s32 fn_800057A0(void);
    extern u32 fn_800FF56C(void);
    extern u8 GSmodelCanAnimate(void*);
    extern void GSmodelLinkTexAnimToAnim(void*, u32);
    extern void GSmodelSetTexAnimRate(void*, f32);
    extern void GSlightSetActive(void*, u32);
    extern u8 GSlightCanAnimate(void*);
    extern void GSlightSetAnimIndex(void*, u32);
    extern void GSlightSetAnimRate(void*, f32);
    extern void GSlightStartAnimation(void*);
    extern void cameraSetGScamera(void*);
    extern void fn_800D2B90(void*);
    extern const char lbl_80272088[];
    extern const char lbl_8035B878[];
    extern f32 lbl_8047CF94;
    extern f32 lbl_8047CF98;
    FloorSceneData* data;
    void* archive;
    void* object;
    const char* strings;
    u32 baseId;
    u32 index;

    strings = lbl_80272088;
    if (floorId == 0) {
        return;
    }
    archive = GSresGetResource(group, floorId);
    data = HSD_ArchiveGetPublicAddress(
        archive, strings + 0x28);
    if (data == NULL) {
        return;
    }

    if (data->models != NULL) {
        baseId = floorReadMakeModelResID(floorId);
        for (index = 0; data->models[index] != NULL; index++) {
            u32 resourceId = baseId | index;
            object = floorOpenObject(resourceId);
            if (object == NULL) {
                GSlogWrite(strings + 0x64, lbl_8035B878, index);
                GSlogWrite(strings + 0x88);
                continue;
            }
            fn_800F9210(group, resourceId);
            GSresRegisterResource(object, group, resourceId,
                                  _floorUnloadModel__FPvUlUl);
            GSmodelSetVisibility(object, 0);

            if (fn_800057A0() == 1) {
                switch (fn_800FF56C()) {
                case 3:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0x15:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x72:
                case 0x75:
                case 0x7B:
                case 0xCA:
                case 0xD0:
                case 0xDE:
                    if (GSmodelCanAnimate(object)) {
                        GSmodelLinkTexAnimToAnim(object, 0);
                        GSmodelSetTexAnimRate(object, lbl_8047CF94);
                    }
                    break;
                }
            }
        }
    }

    if (data->lights != NULL) {
        baseId = floorReadMakeLightResID(floorId);
        for (index = 0; data->lights[index] != NULL; index++) {
            object = GSresGetResource(group, baseId | index);
            if (object != NULL) {
                GSlightSetActive(object, 1);
                if (GSlightCanAnimate(object)) {
                    GSlightSetAnimIndex(object, 0);
                    GSlightSetAnimRate(object, lbl_8047CF98);
                    GSlightStartAnimation(object);
                }
            }
        }
    }

    baseId = floorReadMakeCameraResID(floorId);
    object = GSresGetResource(group, baseId);
    if (object != NULL) {
        cameraSetGScamera(object);
    }
    baseId = floorReadMakeFogResID(floorId);
    object = GSresGetResource(group, baseId);
    fn_800D2B90(object);
}

/* 0x80113778 | 0xB0 */
void floorChangePos(u32 floorId, s16 direction, f32 x, f32 y, f32 z) {
    extern const f32 lbl_8047CF70;
    extern const f32 lbl_8047CF74;
    extern u32 fn_800FF56C(void);
    extern void fn_800FF58C(u32);
    if (floorId == 0) {
        return;
    }
    set__5GSvecFfff((void*)(lbl_80408378 + 0x10), x, y, z);
    set__5GSvecFfff((void*)(lbl_80408378 + 0x1C), lbl_8047CF70,
                    lbl_8047CF74 * direction, lbl_8047CF70);
    *(u32*)(lbl_80408378 + 0x0) = fn_800FF56C();
    *(u32*)(lbl_80408378 + 0x4) = floorId;
    *(u8*)(lbl_80408378 + 0x8) = 0;
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
void _floorUpdate__FUi14FloorEnterMode(u32 floorId, s32 enterMode) {
    extern void fn_801171C8(void);
    extern void fn_80117D14(void);
    extern void cameraUpdate(void);
    extern u8* GSresGetResource(u32, u32);
    extern void fn_8010D8D4(s32);
    extern void fn_800D9ED8(s32);
    extern void fn_800FAEF8(s32, s32, u32, const char*, u32, ...);
    extern s8 lbl_8047AD60;
    extern u8 lbl_8035B818[];
    extern const char lbl_80272088[];
    s32 priority;
    s32 i;

    (void)floorId;
    (void)enterMode;
    fn_801171C8();
    fn_80117D14();
    cameraUpdate();
    if (GSresGetResource(0, 2) != NULL &&
        *GSresGetResource(0, 2) != 0) {
        fn_8010D8D4(0);
        if (GSresGetResource(0, 2) != NULL &&
            *GSresGetResource(0, 2) != 0) {
            fn_800D9ED8(1);
            priority = 0x1E;
            for (i = 0; i < 10; i++, priority += 12) {
                u8* entry = lbl_8035B818 + ((lbl_8047AD60 + i) % 10) * 8;
                switch ((s8)entry[0]) {
                case 1:
                    fn_800FAEF8(0x1E, priority, 0x8080FFFF,
                                lbl_80272088 + 0xA4, *(u32*)(entry + 4));
                    break;
                case 2:
                    fn_800FAEF8(0x1E, priority, 0x8080FFFF,
                                lbl_80272088 + 0xB4, *(u32*)(entry + 4));
                    break;
                case 3:
                    fn_800FAEF8(0x1E, priority, 0x8080FFFF,
                                lbl_80272088 + 0xC8, *(u32*)(entry + 4));
                    break;
                }
            }
            fn_800D9ED8(0);
        }
    }
}

/* 0x80113B84 | 0x18C */
void _floorInitialize__FUi14FloorEnterMode(void* floor, s32 enterMode) {
    extern void heroMoveInit(void*, void*);
    extern u8 fn_800FF548(void);
    extern void _floorInitCharacters__FP11GSfloor_dd_(void*);
    extern u8 fn_801902E0(u16);
    extern void floorEventCtrlDoor(u32, u32, u32);
    extern void fn_80112FEC(void*);
    extern void fn_80117164(void*);
    extern void GSscene_SetMode(s32);
    extern void cameraSetTarget(s32, s32);
    extern void GSscene_SetCameraViewVector(void*);
    extern void cameraUpdate(void);
    extern void fn_800F7D38(s32, s32, s32);
    extern void fn_800F7C8C(s32, s32, s32);
    extern u8 lbl_80408378[];
    extern u8* lbl_80478ECC;
    extern u32* lbl_80478EC8;
    extern const u8 lbl_80272088[];
    u32 floorId = *(u32*)((u8*)floor + 0xC);
    u32 i;

    (void)enterMode;
    heroMoveInit(lbl_80408378 + 0x10, lbl_80408378 + 0x1C);
    if (fn_800FF548() == 0) {
        _floorInitCharacters__FP11GSfloor_dd_(floor);
        for (i = 0; i < *lbl_80478EC8; i++) {
            u8* entry = lbl_80478ECC + i * 0x18;
            if (*(u16*)(entry + 0xC) == floorId) {
                switch (entry[7]) {
                case 1:
                case 3:
                    floorEventCtrlDoor(
                        floorId, i, fn_801902E0(*(u16*)(entry + 0xE)) ? 1 : 3);
                    break;
                case 2:
                    floorEventCtrlDoor(floorId, i, 3);
                    break;
                }
            }
        }
        fn_80112FEC(floor);
    }
    fn_80117164(floor);
    if (fn_800FF548() == 0) {
        u32 view[3];
        view[0] = *(const u32*)(lbl_80272088 + 0);
        view[1] = *(const u32*)(lbl_80272088 + 4);
        view[2] = *(const u32*)(lbl_80272088 + 8);
        GSscene_SetMode(0);
        cameraSetTarget(0, 0x64);
        GSscene_SetCameraViewVector(view);
    }
    cameraUpdate();
    fn_800F7D38(1, 0, 0);
    fn_800F7C8C(1, 0, 0);
}

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
