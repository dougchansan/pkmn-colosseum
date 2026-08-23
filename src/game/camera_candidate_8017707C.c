/** Source owner for 0x8017707C - 0x801773F4. */
#include "game/data/sdata2_8047D690.h"
#include "game/camera_types.h"

typedef struct CameraFloorEntry {
    /* 0x00 */ s32 field_00;
    /* 0x04 */ void* floor;
    /* 0x08 */ f32 defaultHeight;
    /* 0x0C */ f32 defaultDistance;
    /* 0x10 */ f32 defaultRotationY;
    /* 0x14 */ f32 defaultFov;
    /* 0x18 */ f32 height;
    /* 0x1C */ f32 distance;
    /* 0x20 */ f32 rotationY;
    /* 0x24 */ f32 fov;
} CameraFloorEntry;

typedef struct CameraFloatConstant {
    f32 value;
} CameraFloatConstant;

extern const CameraFloatConstant lbl_8047D728;
extern const CameraFloatConstant lbl_8047D72C;

static inline CameraFloorEntry* cameraFindFloorEntry(void* floor)
{
    CameraFloorEntry* entries = (CameraFloorEntry*)lbl_8047B1A8;
    u32 i;

    for (i = 0; i < *(u32*)lbl_80478FB8; i++) {
        if (floor == entries[i].floor) {
            return &entries[i];
        }
    }
    return 0;
}

static inline u32 cameraMoveEndCheck(u8 wait)
{
    CameraPadState* state;

    for (;;) {
        state = lbl_80478C40;
        if (state->targetMoveActive == 0 &&
            state->targetOffsetMoveActive == 0 &&
            state->positionMoveActive == 0 &&
            state->rotationMoveActive == 0) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

extern u8 fn_801174C4(void);
extern void fn_80117500(void);
extern void fn_80117330(f32 duration);
extern const GSSceneVec3 lbl_80273DC8;

void cameraReturn(u8 wait, f32 duration)
{
    CameraFloorEntry* defaults;
    CameraFloorEntry* floorEntry;
    GSSceneVec3 target;
    GSSceneVec3 rotation;
    void* model;
    f32 value;

    if (((CameraPadState*)lbl_80478C40)->mode != 0) {
        ((CameraPadState*)lbl_80478C40)->mode = 0;
    }

    if (fn_801174C4() != 0) {
        fn_80117500();
        fn_80117330(duration);
    } else {
        defaults = cameraFindFloorEntry(fn_800FF56C());
        if (defaults != 0) {
            value = defaults->defaultHeight;
            floorEntry = cameraFindFloorEntry(fn_800FF56C());
            if (floorEntry != 0) {
                floorEntry->height = value;
            }
            ((CameraPadState*)lbl_80478C40)->height = value;

            value = defaults->defaultDistance;
            floorEntry = cameraFindFloorEntry(fn_800FF56C());
            if (floorEntry != 0) {
                floorEntry->distance = value;
            }
            ((CameraPadState*)lbl_80478C40)->distance = value;

            value = defaults->defaultFov;
            if (value < lbl_8047D728.value) {
                value = lbl_8047D728.value;
            }
            if (value > lbl_8047D72C.value) {
                value = lbl_8047D72C.value;
            }
            floorEntry = cameraFindFloorEntry(fn_800FF56C());
            if (floorEntry != 0) {
                floorEntry->fov = value;
            }
            ((CameraPadState*)lbl_80478C40)->fov = value;
        }

        target = lbl_80273DC8;
        ((CameraPadState*)lbl_80478C40)->targetGroup = 0;
        ((CameraPadState*)lbl_80478C40)->targetId = 100;
        ((CameraPadState*)lbl_80478C40)->targetSubId = -1;
        model = GSresGetResource(0, 100);
        if (model != 0) {
            GSmodelGetPosition(model, &target);
        }

        ((CameraPadState*)lbl_80478C40)->flags[0] = 1;
        GSvecCopy(&((CameraPadState*)lbl_80478C40)->targetMoveEnd, &target);
        ((CameraPadState*)lbl_80478C40)->targetMoveTime = lbl_8047D740;
        ((CameraPadState*)lbl_80478C40)->targetMoveDuration = duration;
        ((CameraPadState*)lbl_80478C40)->targetMoveActive = 1;
        GSvecCopy(&((CameraPadState*)lbl_80478C40)->targetMoveStart,
                  &((CameraPadState*)lbl_80478C40)->position);

        set__5GSvecFfff(&rotation, lbl_8047D740, defaults->defaultRotationY,
                        lbl_8047D740);
        ((CameraPadState*)lbl_80478C40)->flags[0] = 1;
        GSvecCopy(&((CameraPadState*)lbl_80478C40)->rotationMoveEnd, &rotation);
        ((CameraPadState*)lbl_80478C40)->rotationMoveTime = lbl_8047D740;
        ((CameraPadState*)lbl_80478C40)->rotationMoveDuration = duration;
        ((CameraPadState*)lbl_80478C40)->rotationMoveActive = 1;
        GSvecCopy(&((CameraPadState*)lbl_80478C40)->rotationMoveStart,
                  &((CameraPadState*)lbl_80478C40)->rotation);
    }

    cameraMoveEndCheck(wait);
}
