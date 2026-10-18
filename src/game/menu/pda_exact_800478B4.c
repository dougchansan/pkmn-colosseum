#include "dolphin/types.h"

typedef struct PdaVec3 {
    f32 x;
    f32 y;
    f32 z;
} PdaVec3;

typedef struct PdaSceneWork {
    u8 pad00[0x248];
} PdaSceneWork;

extern PdaSceneWork lbl_803A6818;
extern u8 lbl_802E5424[];
extern u8 lbl_802E5430[];
extern u8 lbl_80267180[];
extern f32 lbl_8047BC94;
extern f32 lbl_8047BC98;
extern f32 lbl_8047BC9C;
extern f32 lbl_8047BD18;
extern f32 lbl_8047BD30;
extern f32 lbl_8047BD34;
extern void* wazaSequenceOwnerGetModel(void* h);
extern s32 wazaSequenceOwnerGetSizeClass(void* h);
extern void GSmodelGetPosition(void* model, void* out);
extern void GSmodelSetPosition(void* model, void* pos);
extern void GSmodelSetMatrix(void* model, void* mtx);
extern void fn_800E064C(f32* mtx);
extern void fn_800E03B4(f32* mtx, void* vec);
extern void GSvecCopy(void* dst, void* src);
extern void* GSmodelGetBound(void* model);
extern void ObjInfoInit(void* bound, void* out);
extern void GSscene_SetMode(s32 mode);
extern void GScameraGetPerspective(void* cam, f32* a, f32* b, f32* c, f32* d);
extern void GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
extern void GScameraSetPosition(void* cam, void* pos);
extern void GScameraSetRotation(void* cam, void* rot);
extern void GScameraLookAt(void* cam, void* a, void* b);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void GSlightSetType(void* light, s32 type);
extern void GSlightSetColor(void* light, void* color);
extern void GSlightSetPosition(void* light, void* pos);
extern void GSlightSetTarget(void* light, void* target);
extern void GSlightSetActive(void* light, s32 active);
extern void* memcpy(void* dst, const void* src, u32 size);

#pragma peephole off
u8 pdaModelFrameCamera(void* work, void* sub)
{
    extern f64 tan(f64 x);
    f32 mtx0[12];
    f32 mtx1[12];
    PdaVec3 pos0;
    PdaVec3 camPos;
    PdaVec3 pos1;
    PdaVec3 bound0;
    PdaVec3 bound1;
    PdaVec3 lightPos;
    PdaVec3 color;
    f32 persp0;
    f32 persp1;
    f32 persp2;
    f32 persp3;
    void* model0;
    void* model1;
    void* cam;
    f32 height;
    f32 width;
    f32 dist;
    f32 fovy;
    f32 aspect;
    f32 znear;
    f32 zfar;

    if (work == NULL) {
        return 0;
    }
    if (sub == NULL) {
        return 0;
    }
    if (*(u8*)((u8*)work + 0x14) != 0) {
        model0 = wazaSequenceOwnerGetModel(*(void**)((u8*)work + 0x24));
        if (model0 == NULL) {
            return 0;
        }
        wazaSequenceOwnerGetSizeClass(*(void**)((u8*)work + 0x24));
    } else {
        model0 = *(void**)((u8*)work + 0x24);
    }
    if (sub != NULL) {
        model1 = *(void**)((u8*)sub + 0x24);
    }
    if (model0 == NULL) {
        *(u8*)work = 0;
        return 0;
    }
    if (model1 == NULL) {
        return 0;
    }
    GSscene_SetMode(3);
    GScameraGetPerspective(*(void**)((u8*)work + 0x38), &persp0, &persp1,
                           &persp2, &persp3);
    ObjInfoInit(GSmodelGetBound(model0), &bound0);
    ObjInfoInit(GSmodelGetBound(model1), &bound1);
    width = bound0.x + bound1.x;
    height = (bound0.y >= bound1.y) ? bound0.y : bound1.y;
    {
        f32 ratio = (f32)*(s32*)((u8*)work + 0x2c) / (f32)*(s32*)((u8*)work + 0x30);
        persp0 = lbl_8047BD30;
        persp1 = ratio;
    }
    dist = (height >= width) ? height : width;
    dist = dist / (f32)tan(lbl_8047BD34);
    set__5GSvecFfff(&camPos, lbl_8047BC94, lbl_8047BC94, dist);
    GSvecCopy((u8*)&lbl_803A6818 + 0x1e8, &camPos);
    GSvecCopy((u8*)&lbl_803A6818 + 0x1f4, lbl_802E5430);
    fovy = persp0;
    aspect = persp1;
    znear = persp2;
    zfar = persp3;
    *(f32*)((u8*)&lbl_803A6818 + 0x200) = fovy;
    *(f32*)((u8*)&lbl_803A6818 + 0x204) = aspect;
    *(f32*)((u8*)&lbl_803A6818 + 0x208) = znear;
    *(f32*)((u8*)&lbl_803A6818 + 0x20c) = zfar;
    *(f32*)((u8*)&lbl_803A6818 + 0x210) = dist;
    GScameraSetPerspective(*(void**)((u8*)work + 0x38), fovy, aspect, znear,
                           zfar);
    GScameraSetPosition(*(void**)((u8*)work + 0x38), &camPos);
    *(f32*)(lbl_802E5430 + 4) = height * lbl_8047BD18;
    GScameraLookAt(*(void**)((u8*)work + 0x38), lbl_802E5424, lbl_802E5430);
    GSmodelGetPosition(model0, &pos0);
    pos0.x = -(bound0.x * lbl_8047BD18);
    pos0.y = lbl_8047BC94;
    pos0.z = lbl_8047BC94;
    GSmodelSetPosition(model0, &pos0);
    fn_800E064C(mtx0);
    fn_800E03B4(mtx0, &pos0);
    GSmodelSetMatrix(model0, mtx0);
    GSmodelGetPosition(model1, &pos1);
    pos1.x = bound1.x * lbl_8047BD18;
    pos1.y = lbl_8047BC94;
    pos1.z = lbl_8047BC94;
    GSmodelSetPosition(model1, &pos1);
    fn_800E064C(mtx1);
    fn_800E03B4(mtx1, &pos1);
    GSmodelSetMatrix(model1, mtx1);
    color = *(PdaVec3*)lbl_80267180;
    memcpy(&lightPos, &camPos, 12);
    lightPos.x = lightPos.x - lbl_8047BC98;
    lightPos.y = lightPos.y + lbl_8047BC9C;
    GSlightSetType(*(void**)((u8*)work + 0x44), 2);
    GSlightSetColor(*(void**)((u8*)work + 0x44), &color);
    GSlightSetPosition(*(void**)((u8*)work + 0x44), &lightPos);
    GSlightSetTarget(*(void**)((u8*)work + 0x44), &pos0);
    GSlightSetActive(*(void**)((u8*)work + 0x44), 1);
    cam = *(void**)((u8*)&lbl_803A6818 + 0x114);
    if (cam != NULL) {
        GScameraSetPosition(cam, (u8*)&lbl_803A6818 + 0x118);
        GScameraSetRotation(cam, (u8*)&lbl_803A6818 + 0x124);
        GScameraSetPerspective(cam, *(f32*)((u8*)&lbl_803A6818 + 0x13c),
                               *(f32*)((u8*)&lbl_803A6818 + 0x140),
                               *(f32*)((u8*)&lbl_803A6818 + 0x144),
                               *(f32*)((u8*)&lbl_803A6818 + 0x148));
    }
    GSscene_SetMode(3);
    GSscene_SetMode(4);
    return 1;
}
#pragma peephole reset
