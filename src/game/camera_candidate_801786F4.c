/** Candidate-only owner for 0x801786F4 - 0x80179DFC. */
#include "src/game/camera.c"

void cameraSetFloorDefault(f32 height, f32 distance, f32 rotationY) {
    CameraFloorEntry* floorEntry;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry == NULL) {
        return;
    }

    floorEntry->defaultHeight = height;
    floorEntry->defaultDistance = distance;
    floorEntry->defaultRotationY = rotationY;
    if (floorEntry->field_00 != 1) {
        return;
    }

    floorEntry->field_00 = 2;
    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->height = height;
    }
    ((CameraPadState*)lbl_80478C40)->height = height;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->distance = distance;
    }
    ((CameraPadState*)lbl_80478C40)->distance = distance;

    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->rotationY = rotationY;
    }
    ((CameraPadState*)lbl_80478C40)->rotation.y = rotationY;
}

void cameraResetFloor(void) {
    CameraFloorEntry* defaults;
    CameraFloorEntry* floorEntry;
    f32 value;

    if (((CameraPadState*)lbl_80478C40)->mode == 6) {
        if (((CameraPadState*)lbl_80478C40)->mode != 0) {
            ((CameraPadState*)lbl_80478C40)->mode = 0;
        }
    }

    defaults = cameraFindFloorEntry(fn_800FF56C());
    if (defaults == NULL) {
        return;
    }

    value = defaults->defaultHeight;
    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->height = value;
    }
    ((CameraPadState*)lbl_80478C40)->height = value;

    value = defaults->defaultDistance;
    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->distance = value;
    }
    ((CameraPadState*)lbl_80478C40)->distance = value;

    value = defaults->defaultRotationY;
    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->rotationY = value;
    }
    ((CameraPadState*)lbl_80478C40)->rotation.y = value;

    value = defaults->defaultFov;
    if (value < lbl_8047D728.value) {
        value = lbl_8047D728.value;
    }
    if (value > lbl_8047D72C.value) {
        value = lbl_8047D72C.value;
    }
    floorEntry = cameraFindFloorEntry(fn_800FF56C());
    if (floorEntry != NULL) {
        floorEntry->fov = value;
    }
    ((CameraPadState*)lbl_80478C40)->fov = value;
}

extern const GSSceneVec3 lbl_80273D98[4];
extern u8 lbl_80452EC8[];
extern u8* lbl_80478FBC;
extern u32 _toolentryAlloc__FUl(u32 size); /* GSmemAllocRaw, raw return */
extern void* fn_800E27B0(u16 handle); /* GSmemGetPtr */
extern void* fn_800D29A0(void);
extern void GSresRegisterResource(void* resource, u32 group, u32 id, u32 flags);
extern void clear__5GSvecFv(void* vector);
extern void fn_800FF4D4(void* data, u8 typeId);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);

void cameraInit(void) {
    GSSceneVec3 view = lbl_80273D98[0];
    GSSceneVec3 interest = lbl_80273D98[1];
    GSSceneVec3 eye = lbl_80273D98[2];
    GSSceneVec3 floorData = lbl_80273D98[3];
    void* camera;
    u32 i;
    u16 handle;

    memset(lbl_80452EC8, 0, sizeof(CameraPadState));
    lbl_80478C40 = lbl_80452EC8;
    camera = fn_800D29A0();
    GSresRegisterResource(camera, 0, 0, 0);

    if (((CameraPadState*)lbl_80478C40)->mode != 0) {
        ((CameraPadState*)lbl_80478C40)->mode = 0;
    }
    ((CameraPadState*)lbl_80478C40)->targetGroup = 0;
    ((CameraPadState*)lbl_80478C40)->targetId = 100;
    ((CameraPadState*)lbl_80478C40)->targetSubId = -1;
    GSvecCopy(&((CameraPadState*)lbl_80478C40)->view, &view);

    handle = _toolentryAlloc__FUl(*(u32*)lbl_80478FB8 *
                                  sizeof(CameraFloorEntry));
    lbl_8047B1AC = handle;
    lbl_8047B1A8 = fn_800E27B0(handle);
    memset(lbl_8047B1A8, 0, *(u32*)lbl_80478FB8 * sizeof(CameraFloorEntry));
    for (i = 0; i < *(u32*)lbl_80478FB8; i++) {
        ((CameraFloorEntry*)lbl_8047B1A8)[i].field_00 = 0;
        ((CameraFloorEntry*)lbl_8047B1A8)[i].floor =
            *(void**)(lbl_80478FBC + 0x0C + i * 0x4C);
    }

    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetPosition);
    clear__5GSvecFv(&((CameraPadState*)lbl_80478C40)->offsetRotation);
    set__5GSvecFfff(&((CameraPadState*)lbl_80478C40)->offsetScale,
                    lbl_8047D724, lbl_8047D724, lbl_8047D724);
    ((CameraPadState*)lbl_80478C40)->fov = lbl_8047D720.value;
    fn_800FF4D4(&floorData, 1);
    fn_800FF4D4(&floorData, 2);
    GScameraLookAt((GSRenderCamera*)camera,
                   (const GSRenderVec3*)&interest,
                   (const GSRenderVec3*)&view);
    GScameraSetPosition(camera, &eye);
    fn_800D258C(camera);
    _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
}
