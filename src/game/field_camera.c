/**
 * @file field_camera.c
 * @brief Field camera update: fieldCameraUpdate (per-frame blend of the floor's
 * field-camera weights into the camera) and fieldCameraMoveTo (move the camera to
 * the weighted field-camera position). .text 0x801171C8 - 0x801174C4.
 *
 * XD source unit: floorFieldCamera / fieldCamera module. The neighbouring
 * helpers are linked as their own carves (field_camera_exact_8011711C.c,
 * field_camera_exact_801174C4.c) and the .sdata2 constants live in
 * game/data/sdata2_8047CFD0.c.
 */
#include "dolphin/types.h"

extern u8 lbl_8047AD70;
extern u8 lbl_8047AD71;
extern u32 lbl_8047AD68;
extern u32 lbl_8047AD6C;
extern f32 lbl_8047AD74;
extern f32 lbl_8047AD78;
extern f32 lbl_8047AD7C;
extern f32 lbl_8047CFD0;
extern f32 lbl_8047CFD4;
extern f32 lbl_8047CFD8;

extern u8 GSscene_GetMode(void);
extern void GSscene_GetCameraPositionVector(void*);
extern void GSscene_GetCameraViewVector(void*);
extern void cameraSetHeight(f32);
extern void cameraSetDistance(f32);
extern void cameraSetRotY(f32);
extern f32 cameraGetHeight(void);
extern f32 cameraGetDistance(void);
extern f32 cameraGetRotY(void);
extern void cameraMoveTargetPos(u32, void*, f32);
extern void cameraMovePosition(u32, void*, f32);
extern void cameraMoveRotation(u32, void*, f32);
extern u8 floorUpdateFieldCamera(u8*, f32*, f32*, f32*);
extern void* GSresGetResource(u32, u32);
extern void GSmodelGetPosition(void*, void*);
extern void set__5GSvecFfff(void* obj, f32 f1, f32 f2, f32 f3);
extern void GSmtxMakeYRotation(void*, f32);
extern void GSvecTransform(void*, void*, void*);
extern void GSvecAdd(void*, void*, void*);
extern f64 atan2(f64, f64);

/* 0x801171C8 | 0x168 */
void fieldCameraUpdate(void) {
    u8 pos[0xC];
    f32 y;
    f32 x;
    f32 z;

    if (lbl_8047AD71 == 0) { return; }
    if (GSscene_GetMode() != 0) { return; }
    if (lbl_8047AD68 == 0) { return; }

    if (lbl_8047AD68 == 1) {
        f32* ptr = (f32*)lbl_8047AD6C;
        f32 direct_x;
        f32 direct_z;
        direct_z = ptr[5];
        direct_x = ptr[3];
        cameraSetHeight(ptr[4]);
        cameraSetDistance(direct_x);
        cameraSetRotY(direct_z);
        return;
    }

    GSscene_GetCameraPositionVector(pos);
    x = lbl_8047AD74;
    y = lbl_8047AD78;
    z = lbl_8047AD7C;
    if (floorUpdateFieldCamera(pos, &x, &y, &z) == 0) { return; }

    if (lbl_8047AD70 != 0) {
        lbl_8047AD74 = lbl_8047CFD4 * lbl_8047AD74 + lbl_8047CFD8 * x;
        lbl_8047AD78 = lbl_8047CFD4 * lbl_8047AD78 + lbl_8047CFD8 * y;
        lbl_8047AD7C = lbl_8047CFD4 * lbl_8047AD7C + lbl_8047CFD8 * z;
    } else {
        lbl_8047AD74 = x;
        lbl_8047AD78 = y;
        lbl_8047AD7C = z;
        lbl_8047AD70 = 1;
    }

    {
        f32 out_y;
        f32 out_z;
        out_z = lbl_8047AD7C;
        out_y = lbl_8047AD78;
        cameraSetHeight(lbl_8047AD74);
        cameraSetDistance(out_y);
        cameraSetRotY(out_z);
    }
}

/* 0x80117330 | 0x194 */
void fieldCameraMoveTo(f32 arg) {
    u8 tmp[0x30];
    u8 pos[0xC];
    u8 view[0xC];
    u8 rot[0xC];
    u8 rotation[0xC];
    u8 offset[0xC];
    f32 y;
    f32 x;
    f32 z;
    void* obj;

    if (lbl_8047AD71 == 0) { return; }
    if (GSscene_GetMode() != 0) { return; }
    if (lbl_8047AD68 == 0) { return; }

    obj = GSresGetResource(0, 0x64);
    if (obj != NULL) {
        GSmodelGetPosition(obj, pos);
    } else {
        GSscene_GetCameraPositionVector(pos);
    }
    GSscene_GetCameraViewVector(view);

    if (lbl_8047AD68 == 1) {
        f32* ptr = (f32*)lbl_8047AD6C;
        x = ptr[4];
        y = ptr[3];
        z = ptr[5];
    } else {
        x = cameraGetHeight();
        y = cameraGetDistance();
        z = cameraGetRotY();
        if (floorUpdateFieldCamera(pos, &x, &y, &z) == 0) { return; }
    }

    set__5GSvecFfff(offset, lbl_8047CFD0, x, y);
    GSmtxMakeYRotation(tmp, z);
    GSvecTransform(offset, tmp, offset);
    GSvecAdd(rot, pos, view);
    GSvecAdd(rot, rot, offset);
    *(f32*)(&rotation[4]) = z;
    *(f32*)(&rotation[0]) = -(f32)atan2(x, y);
    *(f32*)(&rotation[8]) = lbl_8047CFD0;
    cameraMoveTargetPos(0, pos, arg);
    cameraMovePosition(0, rot, arg);
    cameraMoveRotation(0, rotation, arg);
}
