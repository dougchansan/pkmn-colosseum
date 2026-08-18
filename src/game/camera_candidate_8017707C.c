/** Candidate-only owner for 0x8017707C - 0x801773F4. */
#include "src/game/camera.c"

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
