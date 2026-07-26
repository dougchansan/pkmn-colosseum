/**
 * @file gs_render_util.c
 * @brief GS render utility / HSD bridge code before GSgfx.
 *
 * Contains utility functions for the rendering pipeline including
 * HSD object management, matrix/vector operations, and model
 * rendering helpers.
 *
 * Address range: 0x800D104C - 0x800D3074
 * ~40 functions
 */

#include "dolphin/types.h"
#include "game/gs_render_util.h"
#include "hsd/hsd_object.h"

/* ===== External references ===== */
extern void GSlogWrite(const char* fmt, ...);
extern void SISetSamplingRate(u32 rate);

/* ===== Global state (SDA) ===== */
extern u32 lbl_8047AA60;  /* SamplingRate passed to SISetSamplingRate */
extern u32 lbl_8047AA6C;  /* object array base pointer */
extern u32 lbl_8047AA70;  /* object array count */
extern u32 lbl_8047AA74;  /* current render object */
extern void* lbl_8047AA80; /* pointer to render state struct */

/* Matrix/vector math */
extern void PSMTXMultVec(void* mtx, void* vecIn, void* vecOut); /* MTXMultVec3 */
extern void fn_800A38C0(void* mtxA, void* mtxB, void* mtxOut); /* MTXConcat */
extern void fn_800A3544(void* mtx);                             /* MTXIdentity */
extern void fn_800A35D0(void* mtxA, void* mtxB);               /* MTXCopy */
extern void PSVECSubtract(void* out, void* in, f32 scale);       /* VECNormalize */
extern void PSMTXCopy(void* mtxA, void* mtxB);

/* GX functions */
extern void GXSetProjection(void* mtx, u32 type);
extern void GXSetViewport(f32 x, f32 y, f32 w, f32 h, f32 nearZ, f32 farZ);
extern void GXLoadPosMtxImm(void* mtx, u32 id);
extern void GXSetCurrentMtx(u32 id);

/* HSD functions */
extern void* fn_80362D0C(void* jobj);  /* HSD_JObjAnimAll */
extern void fn_80363CF4(void* jobj);   /* HSD_JObjRemoveAll */

/* GS render util internal functions */
extern void GSvecCopy(void* dst, const void* src);
extern void fn_800E0168(void* dst, void* src1, void* src2);
extern void fn_800E0628(void* dst, void* src);
extern void fn_800E0218(void* a, void* b, void* c, void* d);
extern void GSmtxMakeXRotation(void* a, f32 b);
extern void GSmtxMakeYRotation(void* a, f32 b);
extern void GSmtxMakeZRotation(void* a, f32 b);
extern void fn_800E05C0(void* a, f32 b, f32 c, f32 d);
extern void fn_800E0290(void* a, void* b, void* c);
extern void* fn_800E0264(void* dst, void* src);
extern void GSmtx44Perspective(void* a, f32 b, f32 c, f32 d, f32 e);
extern void GSmtx44Ortho(void* a, f32 b, f32 c, f32 d, f32 e, f32 f, f32 g);
extern void HSD_CObjGetEyePosition(void* jobj, void* data);
extern void HSD_CObjGetUpVector(void* jobj, void* data);
extern void HSD_CObjGetInterest(void* jobj, void* data);
extern void HSD_CObjSetEyePosition(void* jobj);
extern void HSD_CObjSetProjectionType(void* jobj, u32 flag, f32 a, f32 b, f32 c, f32 d);
extern void HSD_CObjSetPerspective(void* jobj, f32 x, f32 y);
extern void HSD_CObjSetNear(void* jobj, f32 z);
extern void HSD_CObjSetFar(void* jobj, f32 w);
extern void HSD_CObjSetScissorx4(void* jobj, u32 x0, u32 x1, u32 y0, u32 y1);
extern void HSD_CObjSetViewport(void* jobj, void* rect);
extern void HSD_CObjGetPerspective(void* jobj, void* outA, void* outB);
extern f32  HSD_CObjGetNear(void* jobj);
extern f32  HSD_CObjGetFar(void* jobj);
extern void HSD_CObjGetOrtho(void* jobj, void* outA, void* outB, void* outC, void* outD);
extern void* HSD_CObjGetViewingMtxPtr(void* jobj);
extern void HSD_CObjRemoveAnim(void* jobj);
extern void HSD_CObjAddAnim(void* jobj, void* ptr);
extern void HSD_CObjReqAnim(void* jobj, f32 val);
extern void HSD_CObjAnim(void* jobj);
extern void HSD_AObjSetRate(void* jobj);
extern double lbl_8047C9B8;  /* SDA double constant for animation step */
extern f32 lbl_8047C9B0;     /* SDA float: fov scaling constant */
extern void HSD_ForeachAnim(void* jobj, u32 a, u32 b, void (*cb)(void*), ...);
extern void __assert(const char* file, u32 line, const char* msg);
extern f32 lbl_8047C990;     /* SDA float: animation step increment */
extern char lbl_8047C9DC[] __attribute__((section(".sdata2")));  /* SDA2 string: assert filename */
extern char lbl_8047C9E4[] __attribute__((section(".sdata2")));  /* SDA2 string: assert condition */
extern u32 _toolentryAlloc__FUl(u32 size);   /* memory allocate, returns ptr as u32 */
extern u32 fn_800E27B0(u32 handle); /* map handle to object ptr */
extern u16 lbl_8047AA68;  /* render obj array low16 tag */
extern u32 lbl_8047AA6C;  /* render obj array base pointer */
extern f32 lbl_8047C994;  /* SDA float constant */
extern f32 lbl_8047C9C0;  /* SDA float constant */
extern f32 lbl_80478ACC;   /* f32 threshold constant, abs accessed via lis/lfs */
extern f32 lbl_8047C998;   /* SDA float 0.0 constant */
extern f32 lbl_8047AA78;   /* SDA float temp */
extern double lbl_8047C9A0; /* double 1.0 constant, SDA2 */
extern double lbl_8047C9A8; /* SDA2 double: 4503599627370496.0 (int-to-float bias, 0x4330000000000000) */
extern u8 lbl_804001B0[0x40]; /* light state buffer (.bss) */
extern s32  fn_800D37CC(void);
extern void fn_800D7FE4(void* obj);
extern void fn_800D834C(void);
extern void fn_800D9BD0(f32 a, f32 b, f32 c, f32 d);
extern void fn_800D9B58(f32 a, f32 b, f32 c, f32 d);
extern void* HSD_CObjLoadDesc(void* modelSub);
extern char lbl_8047C9C4[] __attribute__((section(".sdata2")));
extern char lbl_8047C9CC[] __attribute__((section(".sdata2")));
extern f32 lbl_8047C9D4;   /* SDA float constant */
extern f32 lbl_8047C9D8;   /* SDA float constant */
extern u32 lbl_8047AA8C;   /* SDA pointer: current lighting state */
extern f32 lbl_8047C9F0;   /* SDA2 float scaling constant */
extern double lbl_8047C9F8; /* SDA2 double constant */
extern f32 lbl_8047CA00;   /* SDA2 float constant */
extern f32 lbl_8047CA04;   /* SDA2 float constant */
extern f32 lbl_8047CA08;   /* SDA2 float constant */
extern f32 lbl_8047CA0C;   /* SDA2 float constant */
extern u32 lbl_80270350[4]; /* .rodata array */
extern void* fn_800D7BF8(u32 idx);
extern void GSvecTransform(void* out, void* sphere, void* ray);
extern void GXProject(void* sphere, void* center, void* radii, f32 x, f32 y, f32 z, void* outA, void* outB, void* outC);
extern void* HSD_FogLoadDesc(void* ctx);
extern void HSD_FogSet(u32 zero);
extern void fn_8016EA88(void);
extern void fn_8016EB30(void);

/* Forward declarations for functions defined later in this file */
void fn_800D2B44(void* obj);

#if defined(GS_RENDER_UTIL_LEGACY_PREFIX_800D104C)

/* ==================================================================
 * fn_800D104C - GS render: set sampling rate
 * Address: 0x800D104C, Size: 0x24
 * ================================================================== */
void fn_800D104C(void) {
    SISetSamplingRate(lbl_8047AA60);
}

/* ==================================================================
 * fn_800D1070 - GS render utility: per-frame render object update
 * Address: 0x800D1070, Size: 0x354
 * Updates all active render objects: advances animation, updates transforms.
 * ================================================================== */
void fn_800D1070(u32 dtUnk) {
    u32 byteOff;
    u32 idx;
    byteOff = 0;
    idx = 0;
    while (idx < lbl_8047AA70) {
        void* obj = (u8*)lbl_8047AA6C + byteOff;
        if (*(u8*)((u8*)obj + 0x0) == 1) {
            if (*(u8*)((u8*)obj + 0x4) == 1) {
                s8 animDir;
                f32 speed;
                f32 lastFrame;
                f32 fov;
                s32 mode;
                GSvecCopy((u8*)obj + 0x7c, (u8*)obj + 0x70);
                HSD_CObjReqAnim(*(void**)((u8*)obj + 0xc), *(f32*)((u8*)obj + 0x11c));
                HSD_CObjAnim(*(void**)((u8*)obj + 0xc));
                HSD_CObjGetEyePosition(*(void**)((u8*)obj + 0xc), (u8*)obj + 0x70);
                HSD_CObjGetUpVector(*(void**)((u8*)obj + 0xc), (u8*)obj + 0xf4);
                HSD_CObjGetInterest(*(void**)((u8*)obj + 0xc), (u8*)obj + 0x100);
                animDir = *(s8*)((u8*)obj + 0x125);
                fov = *(f32*)((u8*)obj + 0x118);
                lastFrame = *(f32*)((u8*)obj + 0x120);
                speed = *(f32*)((u8*)obj + 0x11c);
                mode = *(s32*)((u8*)obj + 0x10c);
                {
                    f32 animStep = fov * (f32)dtUnk;
                    f32 threshold = lastFrame - lbl_8047C990;
                    if (animDir == -1) {
                        *(f32*)((u8*)obj + 0x11c) = speed - animStep;
                    } else if (animDir == 1) {
                        *(f32*)((u8*)obj + 0x11c) = speed + animStep;
                    }
                    speed = *(f32*)((u8*)obj + 0x11c);
                    if (mode == 1) {
                        if (speed >= threshold) {
                            *(s8*)((u8*)obj + 0x125) = -1;
                        } else if (speed <= lbl_8047C998) {
                            *(s8*)((u8*)obj + 0x125) = 1;
                        }
                    } else if (mode == 0) {
                        f32 thr2 = threshold - lbl_8047C994;
                        if (speed >= thr2) {
                            *(u8*)((u8*)obj + 0x124) = 1;
                            *(s8*)((u8*)obj + 0x125) = 0;
                            *(f32*)((u8*)obj + 0x11c) = thr2;
                        }
                    } else if (mode >= 2 && mode < 3) {
                        /* mode 2: no extra action */
                    }
                }
                *(u8*)((u8*)obj + 0x2) = 1;
            }
            /* transform update */
            if (*(u8*)((u8*)obj + 0x4) != 0) {
                fn_800E0628((u8*)obj + 0x94, HSD_CObjGetViewingMtxPtr(*(void**)((u8*)obj + 0xc)));
                HSD_CObjGetEyePosition(*(void**)((u8*)obj + 0xc), (u8*)obj + 0x70);
                HSD_CObjGetUpVector(*(void**)((u8*)obj + 0xc), (u8*)obj + 0xf4);
                HSD_CObjGetInterest(*(void**)((u8*)obj + 0xc), (u8*)obj + 0x100);
            } else if (*(u8*)((u8*)obj + 0x1) == 1) {
                f32 tmp[3];
                fn_800E0168(tmp, (u8*)obj + 0x70, (u8*)obj + 0x100);
                {
                    f32 ax = tmp[0];
                    if (ax < lbl_8047C998) ax = -ax;
                    if (ax < lbl_80478ACC) {
                        f32 ay = tmp[1];
                        if (ay < lbl_8047C998) ay = -ay;
                        if (ay < lbl_80478ACC) {
                            f32 az = tmp[2];
                            if (az < lbl_8047C998) az = -az;
                            if (az < lbl_80478ACC) {
                                f32 v = *(f32*)((u8*)obj + 0x100);
                                *(f32*)((u8*)obj + 0x100) = (f32)(v + lbl_8047C9A0);
                            }
                        }
                    }
                }
                fn_800E0218((u8*)obj + 0x94, (u8*)obj + 0x70, (u8*)obj + 0xf4, (u8*)obj + 0x100);
            } else {
                f32 tmp1[3][4];
                f32 tmp2[3][4];
                f32 tmp3[3][4];
                GSmtxMakeXRotation((u8*)obj + 0x94, -*(f32*)((u8*)obj + 0x88));
                GSmtxMakeYRotation(tmp1, -*(f32*)((u8*)obj + 0x8c));
                GSmtxMakeZRotation(tmp2, -*(f32*)((u8*)obj + 0x90));
                fn_800E05C0(tmp3, -*(f32*)((u8*)obj + 0x70), -*(f32*)((u8*)obj + 0x74), -*(f32*)((u8*)obj + 0x78));
                fn_800E0290((u8*)obj + 0x94, (u8*)obj + 0x94, tmp1);
                fn_800E0290((u8*)obj + 0x94, (u8*)obj + 0x94, tmp2);
                fn_800E0290((u8*)obj + 0x94, (u8*)obj + 0x94, tmp3);
            }
            *(u8*)((u8*)obj + 0x2) = 0;
        }
        byteOff += 0x128;
        idx++;
    }
}
#endif

#if defined(GS_RENDER_UTIL_EXACT_800D13C4)

/* ==================================================================
 * fn_800D13C4 - Empty function (likely stripped debug)
 * Address: 0x800D13C4, Size: 0x4
 * ================================================================== */
void fn_800D13C4(void) {
    /* 4 bytes -- likely just blr (empty function) */
}

/* ==================================================================
 * fn_800D13C8 - GS render: copy saved state into render object
 * Address: 0x800D13C8, Size: 0x2AC
 * Copies state from src (saved snapshot) into dst (render object).
 * ================================================================== */
#pragma push
#pragma scheduling on
void fn_800D13C8(void* dst, void* src) {
    GSRenderCamera* c = (GSRenderCamera*)dst;
    GSRenderCameraSnapshot* s = (GSRenderCameraSnapshot*)src;
    f32 fov;
    GSvecCopy(&c->eye, &s->eye);
    GSvecCopy(&c->prevEye, &s->prevEye);
    GSvecCopy(&c->rotation, &s->rotation);
    fn_800E0628(c->viewMtx, s->viewMtx);
    fn_800E0628(c->projectionMtx, s->projectionMtx);
    GSvecCopy(&c->upVector, &s->upVector);
    GSvecCopy(&c->interest, &s->interest);
    c->animEnded = s->animEnded;
    if (s->animEnded != 0) {
        return;
    }
    if (s->isAnimating == 0) {
        return;
    }
    {
        u32 frameIdx = s->animIndex;
        if (c->hasAnimation != 0) {
            HSD_CObjRemoveAnim(c->cobj);
            if (frameIdx <= c->animCount) {
                c->animIndex = frameIdx;
                HSD_CObjAddAnim(c->cobj, c->desc->animations[c->animIndex]);
                HSD_CObjReqAnim(c->cobj, lbl_8047C998);
                lbl_8047AA78 = lbl_8047C998;
                HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, fn_800D2B44, (u32)0);
                c->animEndFrame = lbl_8047AA78;
            }
        }
        c->animMode = s->animMode;
        fov = s->animRate;
        if (c->hasAnimation != 0) {
            if (fn_800D37CC() == 0x32) {
                fov *= lbl_8047C9B0;
            }
            c->animRate = fov;
            HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, c->animRate, (u32)1);
        }
        {
            f32 src_ac = s->animFrame;
            if (c->hasAnimation != 0) {
                c->animFrame = src_ac;
                HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, lbl_8047C9B8, (u32)1);
                HSD_CObjReqAnim(c->cobj, c->animFrame);
                HSD_CObjAnim(c->cobj);
                HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, c->animRate, (u32)1);
            }
        }
        if (c->hasAnimation == 0) { goto done; }
        c->isAnimating = 1;
        c->animEnded = 0;
        c->animDirection = 1;
        {
            f32 dst_11c = c->animFrame;
            if (c->hasAnimation != 0) {
                c->animFrame = dst_11c;
                HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, lbl_8047C9B8, (u32)1);
                HSD_CObjReqAnim(c->cobj, c->animFrame);
                HSD_CObjAnim(c->cobj);
                HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, c->animRate, (u32)1);
            }
        }
done:
        ;
    }
}
#pragma pop

/* ==================================================================
 * fn_800D1674 - GS render: copy object data to dest struct
 * Address: 0x800D1674, Size: 0xB8
 * ================================================================== */
void fn_800D1674(void* src, void* dst) {
    GSRenderCamera* c = (GSRenderCamera*)src;
    GSRenderCameraSnapshot* s = (GSRenderCameraSnapshot*)dst;
    s->isAnimating = c->isAnimating;
    GSvecCopy(&s->eye, &c->eye);
    GSvecCopy(&s->prevEye, &c->prevEye);
    GSvecCopy(&s->rotation, &c->rotation);
    fn_800E0628(s->viewMtx, c->viewMtx);
    fn_800E0628(s->projectionMtx, c->projectionMtx);
    GSvecCopy(&s->upVector, &c->upVector);
    GSvecCopy(&s->interest, &c->interest);
    s->animMode = c->animMode;
    s->animIndex = c->animIndex;
    s->animRate = c->animRate;
    s->animFrame = c->animFrame;
    s->animEnded = c->animEnded;
}

/* ==================================================================
 * GScameraGetAnimFrame - GS render: get object speed float (field_0x11c)
 * Address: 0x800D172C, Size: 0x8
 * ================================================================== */
f32 GScameraGetAnimFrame(GSRenderCamera* camera) {
    return camera->animFrame;
}

/* ==================================================================
 * GScameraHasAnimationEnded - GS render: get object signed field_0x124
 * Address: 0x800D1734, Size: 0xC
 * ================================================================== */
s8 GScameraHasAnimationEnded(GSRenderCamera* camera) {
    return camera->animEnded;
}

/* ==================================================================
 * GScameraStopAnimation - GS render: init object animation state
 * Address: 0x800D173C, Size: 0x5C
 * ================================================================== */
void GScameraStopAnimation(GSRenderCamera* camera) {
    camera->isAnimating = 0;
    camera->useLookAt = 1;
    HSD_CObjGetEyePosition(camera->cobj, &camera->eye);
    HSD_CObjGetUpVector(camera->cobj, &camera->upVector);
    HSD_CObjGetInterest(camera->cobj, &camera->interest);
}

/* ==================================================================
 * GScameraStartAnimation - GS render: advance object animation by speed
 * Address: 0x800D1798, Size: 0xC0
 * ================================================================== */
#pragma push
#pragma scheduling on
void GScameraStartAnimation(GSRenderCamera* camera) {
    if (camera->hasAnimation != 0) {
        f32 speed;
        camera->isAnimating = 1;
        camera->animEnded = 0;
        camera->animDirection = 1;
        speed = camera->animFrame;
        if (camera->hasAnimation != 0) {
            camera->animFrame = speed;
            HSD_ForeachAnim(camera->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, lbl_8047C9B8, (u32)1);
            HSD_CObjReqAnim(camera->cobj, camera->animFrame);
            HSD_CObjAnim(camera->cobj);
            HSD_ForeachAnim(camera->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, (double)camera->animRate, (u32)1);
        }
    }
}
#pragma pop

/* ==================================================================
 * fn_800D1858 - GS render: set object fields 0x10c and 0x114
 * Address: 0x800D1858, Size: 0xC
 * ================================================================== */
void fn_800D1858(GSRenderCamera* camera, u32 val) {
    camera->animMode = val;
}

/* ==================================================================
 * GScameraSetAnimFrame - GS render: set object anim speed (field 0x11c) and advance
 * Address: 0x800D1860, Size: 0x9C
 * ================================================================== */
void GScameraSetAnimFrame(GSRenderCamera* camera, f32 frame) {
    if (camera->hasAnimation != 0) {
        camera->animFrame = frame;
        HSD_ForeachAnim(camera->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, lbl_8047C9B8, (u32)1);
        HSD_CObjReqAnim(camera->cobj, camera->animFrame);
        HSD_CObjAnim(camera->cobj);
        HSD_ForeachAnim(camera->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, (double)camera->animRate, (u32)1);
    }
}

/* ==================================================================
 * GScameraSetAnimRate - GS render: set object fov angle (field 0x118)
 * Address: 0x800D18FC, Size: 0x88
 * ================================================================== */
void GScameraSetAnimRate(GSRenderCamera* camera, f32 rate) {
    if (camera->hasAnimation != 0) {
        s32 mode = fn_800D37CC();
        if (mode == 0x32) {
            rate *= lbl_8047C9B0;
        }
        camera->animRate = rate;
        HSD_ForeachAnim(camera->cobj, (u32)2, (u32)0xffff, HSD_AObjSetRate, camera->animRate, (u32)1);
    }
}

/* ==================================================================
 * GScameraSetAnimIndex - GS render: set object animation frame index
 * Address: 0x800D1984, Size: 0xB4
 * ================================================================== */
#pragma push
#pragma scheduling on
void GScameraSetAnimIndex(GSRenderCamera* camera, u32 animIndex) {
    if (camera->hasAnimation != 0) {
        HSD_CObjRemoveAnim(camera->cobj);
        if (animIndex <= camera->animCount) {
            camera->animIndex = animIndex;
            HSD_CObjAddAnim(camera->cobj, camera->desc->animations[camera->animIndex]);
            HSD_CObjReqAnim(camera->cobj, lbl_8047C998);
            lbl_8047AA78 = lbl_8047C998;
            HSD_ForeachAnim(camera->cobj, (u32)2, (u32)0xffff, fn_800D2B44, (u32)0);
            camera->animEndFrame = lbl_8047AA78;
        }
    }
}
#pragma pop

/* ==================================================================
 * GScameraIsAnimating - GS render: get object active flag (field_0x4)
 * Address: 0x800D1A38, Size: 0x8
 * ================================================================== */
u8 GScameraIsAnimating(GSRenderCamera* camera) {
    return camera->isAnimating;
}

/* ==================================================================
 * GScameraGetDistanceVector - GS render: get object angles (field_0x70, 0x100)
 * Address: 0x800D1A40, Size: 0x30
 * ================================================================== */
void GScameraGetDistanceVector(GSRenderCamera* camera, GSRenderVec3* dest) {
    fn_800E0168(dest, &camera->eye, &camera->interest);
}

/* ==================================================================
 * GScameraGetProjMatrixPtr - GS render: setup viewport/projection from JObj
 * Address: 0x800D1A70, Size: 0xCC
 * Sets up light/camera data from a JObj, writes to lbl_804001B0.
 * Returns pointer to lbl_804001B0.
 * ================================================================== */
#pragma push
#pragma scheduling on
void* GScameraGetProjMatrixPtr(GSRenderCamera* camera) {
    f32 x, z;
    f32 out3, out2, out1, out0;
    void* jobj = camera->cobj;
    if (*(u8*)((u8*)camera->cobj + 0x50) == 1) {
        f32 w, h;
        HSD_CObjGetPerspective(jobj, &x, &z);
        w = HSD_CObjGetNear(camera->cobj);
        h = HSD_CObjGetFar(camera->cobj);
        GSmtx44Perspective(lbl_804001B0, x, z, w, h);
    } else {
        HSD_CObjGetOrtho(jobj, &out2, &out0, &out3, &out1);
        GSmtx44Ortho(lbl_804001B0, out2, out0, out3, out1, lbl_8047C994, lbl_8047C9C0);
    }
    return lbl_804001B0;
}
#pragma pop
#endif

#if defined(GS_RENDER_UTIL_SUFFIX_800D1B3C)

/* ==================================================================
 * fn_800D1B3C - GS render: update transform and return matrix ptr
 * Address: 0x800D1B3C, Size: 0x1C4
 * Updates transform (same 3-way logic as fn_800D1D00), then
 * calls fn_800E0264(obj+0xc4, obj+0x94). Returns obj+0xc4.
 * ================================================================== */
void* fn_800D1B3C(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;
    if (c->dirty != 0) {
        if (c->isAnimating != 0) {
            fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
            HSD_CObjGetEyePosition(c->cobj, &c->eye);
            HSD_CObjGetUpVector(c->cobj, &c->upVector);
            HSD_CObjGetInterest(c->cobj, &c->interest);
        } else if (c->useLookAt == 1) {
            f32 tmp[3];
            fn_800E0168(tmp, &c->eye, &c->interest);
            {
                f32 ax = tmp[0];
                if (ax < lbl_8047C998) ax = -ax;
                if (ax < lbl_80478ACC) {
                    f32 ay = tmp[1];
                    if (ay < lbl_8047C998) ay = -ay;
                    if (ay < lbl_80478ACC) {
                        f32 az = tmp[2];
                        if (az < lbl_8047C998) az = -az;
                        if (az < lbl_80478ACC) {
                            f32 v = c->interest.x;
                            c->interest.x = (f32)(v + lbl_8047C9A0);
                        }
                    }
                }
            }
            fn_800E0218(c->viewMtx, &c->eye, &c->upVector, &c->interest);
        } else {
            f32 tmp1[3][4];
            f32 tmp2[3][4];
            f32 tmp3[3][4];
            GSmtxMakeXRotation(c->viewMtx, -c->rotation.x);
            GSmtxMakeYRotation(tmp1, -c->rotation.y);
            GSmtxMakeZRotation(tmp2, -c->rotation.z);
            fn_800E05C0(tmp3, -c->eye.x, -c->eye.y, -c->eye.z);
            fn_800E0290(c->viewMtx, c->viewMtx, tmp1);
            fn_800E0290(c->viewMtx, c->viewMtx, tmp2);
            fn_800E0290(c->viewMtx, c->viewMtx, tmp3);
        }
        c->dirty = 0;
        fn_800E0264(c->projectionMtx, c->viewMtx);
    }
    return c->projectionMtx;
}

/* ==================================================================
 * fn_800D1D00 - GS render: update object transform from animation
 * Address: 0x800D1D00, Size: 0x1B8
 * ================================================================== */
void fn_800D1D00(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;
    if (c->dirty == 0) {
        return;
    }
    if (c->isAnimating != 0) {
        fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
        HSD_CObjGetEyePosition(c->cobj, &c->eye);
        HSD_CObjGetUpVector(c->cobj, &c->upVector);
        HSD_CObjGetInterest(c->cobj, &c->interest);
    } else if (c->useLookAt == 1) {
        f32 tmp[3];
        fn_800E0168(tmp, &c->eye, &c->interest);
        {
            f32 ax = tmp[0];
            if (ax < lbl_8047C998) ax = -ax;
            if (ax < lbl_80478ACC) {
                f32 ay = tmp[1];
                if (ay < lbl_8047C998) ay = -ay;
                if (ay < lbl_80478ACC) {
                    f32 az = tmp[2];
                    if (az < lbl_8047C998) az = -az;
                    if (az < lbl_80478ACC) {
                        f32 v = c->interest.x;
                        c->interest.x = (f32)(v + lbl_8047C9A0);
                    }
                }
            }
        }
        fn_800E0218(c->viewMtx, &c->eye, &c->upVector, &c->interest);
    } else {
        f32 tmp1[3][4];
        f32 tmp2[3][4];
        f32 tmp3[3][4];
        GSmtxMakeXRotation(c->viewMtx, -c->rotation.x);
        GSmtxMakeYRotation(tmp1, -c->rotation.y);
        GSmtxMakeZRotation(tmp2, -c->rotation.z);
        fn_800E05C0(tmp3, -c->eye.x, -c->eye.y, -c->eye.z);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp1);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp2);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp3);
    }
    c->dirty = 0;
}

/* ==================================================================
 * GScameraGetLookAt - GS render: get up and interest vectors
 * Address: 0x800D1EB8, Size: 0x4C
 * ================================================================== */
void GScameraGetLookAt(GSRenderCamera* camera, GSRenderVec3* dest1,
                       GSRenderVec3* dest2) {
    GSvecCopy(dest1, &camera->upVector);
    GSvecCopy(dest2, &camera->interest);
}

/* ==================================================================
 * GScameraLookAt - GS render: set up and interest vectors
 * Address: 0x800D1F04, Size: 0x54
 * ================================================================== */
void GScameraLookAt(GSRenderCamera* camera, const GSRenderVec3* src1,
                    const GSRenderVec3* src2) {
    GSvecCopy(&camera->upVector, src1);
    GSvecCopy(&camera->interest, src2);
    camera->dirty = 1;
    camera->useLookAt = 1;
}

/* ==================================================================
 * GScameraGetRotation - GS render: set object rotation data (field_0x88)
 * Address: 0x800D1F58, Size: 0x2C
 * ================================================================== */
void GScameraGetRotation(GSRenderCamera* camera, GSRenderVec3* dest) {
    GSvecCopy(dest, &camera->rotation);
}

/* ==================================================================
 * GScameraGetPosition - GS render: get position data, optionally update JObj
 * Address: 0x800D1F84, Size: 0x58
 * ================================================================== */
void GScameraGetPosition(GSRenderCamera* camera, GSRenderVec3* dest) {
    if (camera->isAnimating != 0) {
        HSD_CObjGetEyePosition(camera->cobj, &camera->eye);
    }
    GSvecCopy(dest, &camera->eye);
}

/* ==================================================================
 * GScameraGetPerspective - GS render: get object scissor rect extents
 * Address: 0x800D1FDC, Size: 0x60
 * ================================================================== */
void GScameraGetPerspective(GSRenderCamera* camera, f32* fov, f32* aspect,
                            f32* near, f32* far) {
    HSD_CObjGetPerspective(camera->cobj, fov, aspect);
    *near = HSD_CObjGetNear(camera->cobj);
    *far = HSD_CObjGetFar(camera->cobj);
}

/* ==================================================================
 * GScameraSetRotation - GS render: set rotation data, mark dirty
 * Address: 0x800D203C, Size: 0x40
 * ================================================================== */
void GScameraSetRotation(GSRenderCamera* camera, const GSRenderVec3* src) {
    GSvecCopy(&camera->rotation, src);
    camera->dirty = 1;
    camera->useLookAt = 0;
}

/* ==================================================================
 * GScameraSetPosition - GS render: set position data, mark dirty
 * Address: 0x800D207C, Size: 0x50
 * ================================================================== */
void GScameraSetPosition(GSRenderCamera* camera, void* src) {
    HSD_CObjSetEyePosition(camera->cobj);
    GSvecCopy(&camera->eye, src);
    camera->dirty = 1;
}

/* ==================================================================
 * GScameraSetPerspective - GS render: set object transform params
 * Address: 0x800D20CC, Size: 0x84
 * ================================================================== */
void GScameraSetPerspective(GSRenderCamera* camera, f32 x, f32 y, f32 z, f32 w) {
    HSD_CObjSetProjectionType(camera->cobj, 1, x, y, z, w);
    HSD_CObjSetPerspective(camera->cobj, x, y);
    HSD_CObjSetNear(camera->cobj, z);
    HSD_CObjSetFar(camera->cobj, w);
    camera->dirty = 1;
}

/* ==================================================================
 * fn_800D2150 - GS render: set scissor rect (clamped)
 * Address: 0x800D2150, Size: 0x78
 * ================================================================== */
void fn_800D2150(GSRenderCamera* camera, u32 x0, u32 y0, u32 x1, u32 y1) {
    if ((u16)x0 > 0x27e) x0 = 0x27e;
    if ((u16)y0 > 0x1de) y0 = 0x1de;
    if ((u16)x1 > 0x27f) x1 = 0x27f;
    if ((u16)y1 > 0x1df) y1 = 0x1df;
    HSD_CObjSetScissorx4(camera->cobj, (u16)x0, (u16)(x1 + 1), (u16)y0, (u16)(y1 + 1));
}

/* ==================================================================
 * GScameraSetViewport - GS render: set viewport rect (clamped, packed)
 * Address: 0x800D21C8, Size: 0x80
 * ================================================================== */
void GScameraSetViewport(GSRenderCamera* camera, u32 x0, u32 y0, u32 x1, u32 y1) {
    u16 rect[4];
    if ((u16)x0 > 0x27e) x0 = 0x27e;
    if ((u16)y0 > 0x1de) y0 = 0x1de;
    if ((u16)x1 > 0x27f) x1 = 0x27f;
    if ((u16)y1 > 0x1df) y1 = 0x1df;
    rect[0] = (u16)x0;
    rect[2] = (u16)y0;
    rect[1] = (u16)(x1 + 1);
    rect[3] = (u16)(y1 + 1);
    HSD_CObjSetViewport(camera->cobj, rect);
}

/* ==================================================================
 * _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID - GS render: main render loop (uses lbl_8047AA74 list)
 * Address: 0x800D2248, Size: 0x33C
 * ================================================================== */
void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void) {
    void* obj = (void*)lbl_8047AA74;
    GSRenderCamera* c = (GSRenderCamera*)obj;
    void* jobj;
    if (obj == 0) {
        return;
    }
    if (c->isAnimating != 0) {
        f32 x, z, w, h;
        jobj = c->cobj;
        if (jobj == 0) {
            __assert(lbl_8047C9C4, 0x1ae, lbl_8047C9CC);
        }
        *(u32*)((u8*)jobj + 0x8) &= ~0x2u;
        HSD_CObjGetPerspective(c->cobj, &x, &z);
        w = HSD_CObjGetNear(c->cobj);
        h = HSD_CObjGetFar(c->cobj);
        fn_800D9BD0(x, z, w, h);
        fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
        fn_800D834C();
        fn_800D7FE4(c->viewMtx);
        HSD_CObjGetEyePosition(c->cobj, &c->eye);
        HSD_CObjGetUpVector(c->cobj, &c->upVector);
        HSD_CObjGetInterest(c->cobj, &c->interest);
        return;
    }
    jobj = c->cobj;
    if (*(u8*)((u8*)jobj + 0x50) != 1) {
        f32 a, b, c2, d;
        HSD_CObjGetOrtho(jobj, &b, &d, &a, &c2);
        fn_800D9B58(b, d, a, c2);
        fn_800D834C();
        return;
    }
    if (c->dirty != 0) {
        if (c->isAnimating != 0) {
            fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
            HSD_CObjGetEyePosition(c->cobj, &c->eye);
            HSD_CObjGetUpVector(c->cobj, &c->upVector);
            HSD_CObjGetInterest(c->cobj, &c->interest);
        } else if (c->useLookAt == 1) {
            f32 tmp[3];
            fn_800E0168(tmp, &c->eye, &c->interest);
            {
                f32 ax = tmp[0];
                if (ax < lbl_8047C998) ax = -ax;
                if (ax < lbl_80478ACC) {
                    f32 ay = tmp[1];
                    if (ay < lbl_8047C998) ay = -ay;
                    if (ay < lbl_80478ACC) {
                        f32 az = tmp[2];
                        if (az < lbl_8047C998) az = -az;
                        if (az < lbl_80478ACC) {
                            f32 v = c->interest.x;
                            c->interest.x = (f32)(v + lbl_8047C9A0);
                        }
                    }
                }
            }
            fn_800E0218(c->viewMtx, &c->eye, &c->upVector, &c->interest);
        } else {
            f32 tmp1[3][4];
            f32 tmp2[3][4];
            f32 tmp3[3][4];
            GSmtxMakeXRotation(c->viewMtx, -c->rotation.x);
            GSmtxMakeYRotation(tmp1, -c->rotation.y);
            GSmtxMakeZRotation(tmp2, -c->rotation.z);
            fn_800E05C0(tmp3, -c->eye.x, -c->eye.y, -c->eye.z);
            fn_800E0290(c->viewMtx, c->viewMtx, tmp1);
            fn_800E0290(c->viewMtx, c->viewMtx, tmp2);
            fn_800E0290(c->viewMtx, c->viewMtx, tmp3);
        }
        c->dirty = 0;
    }
    {
        f32 x, z, w, h;
        jobj = c->cobj;
        if (jobj == 0) {
            __assert(lbl_8047C9C4, 0x1a2, lbl_8047C9CC);
        }
        *(u32*)((u8*)jobj + 0x8) |= 0x80000002u;
        PSMTXCopy(c->viewMtx, (u8*)jobj + 0x54);
        HSD_CObjGetPerspective(c->cobj, &x, &z);
        w = HSD_CObjGetNear(c->cobj);
        h = HSD_CObjGetFar(c->cobj);
        fn_800D9BD0(x, z, w, h);
        fn_800D834C();
        fn_800D7FE4(c->viewMtx);
    }
}

/* ==================================================================
 * GScameraGetActiveCamera - GS render: get batch count
 * Address: 0x800D2584, Size: 0x8
 * ================================================================== */
u32 GScameraGetActiveCamera(void) {
    return lbl_8047AA74;
}

/* ==================================================================
 * fn_800D258C - GS render: set current render obj, update transform
 * Address: 0x800D258C, Size: 0x1AC
 * Sets lbl_8047AA74 to obj, then updates transform (same logic as fn_800D1D00).
 * ================================================================== */
void fn_800D258C(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;
    lbl_8047AA74 = (u32)obj;
    if (c->isAnimating != 0) {
        fn_800E0628(c->viewMtx, HSD_CObjGetViewingMtxPtr(c->cobj));
        HSD_CObjGetEyePosition(c->cobj, &c->eye);
        HSD_CObjGetUpVector(c->cobj, &c->upVector);
        HSD_CObjGetInterest(c->cobj, &c->interest);
    } else if (c->useLookAt == 1) {
        f32 tmp[3];
        fn_800E0168(tmp, &c->eye, &c->interest);
        {
            f32 ax = tmp[0];
            if (ax < lbl_8047C998) ax = -ax;
            if (ax < lbl_80478ACC) {
                f32 ay = tmp[1];
                if (ay < lbl_8047C998) ay = -ay;
                if (ay < lbl_80478ACC) {
                    f32 az = tmp[2];
                    if (az < lbl_8047C998) az = -az;
                    if (az < lbl_80478ACC) {
                        f32 v = c->interest.x;
                        c->interest.x = (f32)(v + lbl_8047C9A0);
                    }
                }
            }
        }
        fn_800E0218(c->viewMtx, &c->eye, &c->upVector, &c->interest);
    } else {
        f32 tmp1[3][4];
        f32 tmp2[3][4];
        f32 tmp3[3][4];
        GSmtxMakeXRotation(c->viewMtx, -c->rotation.x);
        GSmtxMakeYRotation(tmp1, -c->rotation.y);
        GSmtxMakeZRotation(tmp2, -c->rotation.z);
        fn_800E05C0(tmp3, -c->eye.x, -c->eye.y, -c->eye.z);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp1);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp2);
        fn_800E0290(c->viewMtx, c->viewMtx, tmp3);
    }
    c->dirty = 0;
}

/* ==================================================================
 * fn_800D2738 - GS render: effect render cleanup
 * Address: 0x800D2738, Size: 0xC4
 * ================================================================== */
void fn_800D2738(void* obj) {
    GSRenderCamera* c = (GSRenderCamera*)obj;
    void* jobj;
    if (obj == (void*)lbl_8047AA74) {
        lbl_8047AA74 = (u32)0;
    }
    jobj = c->cobj;
    if (jobj != NULL && ref_DEC(jobj)) {
        hsdDelete(jobj);
    }
    c->active = 0;
}

/* ==================================================================
 * fn_800D27FC - GS render: find free slot and init with model data
 * Address: 0x800D27FC, Size: 0x1A4
 * ================================================================== */
static inline void GScameraIncrementAnimCount(GSRenderCamera* camera)
{
    camera->animCount++;
}

void* fn_800D27FC(void* model) {
    u32 i;
    void* slot;
    GSRenderCamera* c;
    u32 count = lbl_8047AA70;
    slot = (void*)lbl_8047AA6C;
    for (i = 0; i < count; i++) {
        if (*(u8*)slot == 0) {
            goto found;
        }
        slot = (u8*)slot + 0x128;
    }
    slot = 0;
found:
    if (slot == 0) {
        return 0;
    }
    c = (GSRenderCamera*)slot;
    c->desc = (GSRenderCameraDesc*)model;
    c->cobj = HSD_CObjLoadDesc(c->desc->cobjDesc);
    c->active = 1;
    c->isAnimating = 0;
    if (c->desc->animations != 0) {
        c->hasAnimation = 1;
        c->animRate = lbl_8047C990;
        c->animMode = 1;
        c->animEnded = 0;
        c->animCount = 0;
        while (c->desc->animations[(s32)c->animCount] != 0) {
            GScameraIncrementAnimCount(c);
        }
        if (c->hasAnimation != 0) {
            HSD_CObjRemoveAnim(c->cobj);
            if ((s32)c->animCount >= 0) {
                c->animIndex = 0;
                HSD_CObjAddAnim(c->cobj, c->desc->animations[c->animIndex]);
                HSD_CObjReqAnim(c->cobj, lbl_8047C998);
                lbl_8047AA78 = lbl_8047C998;
                {
                    lis_r3_fn_800D2B44:
                    HSD_ForeachAnim(c->cobj, (u32)2, (u32)0xffff, fn_800D2B44, (u32)0);
                }
                c->animEndFrame = lbl_8047AA78;
            }
        }
    } else {
        c->hasAnimation = 0;
        c->active = 1;
        c->useLookAt = 0;
        GSvecCopy(&c->eye, (u8*)*(void**)((u8*)c->cobj + 0x24) + 0xc);
        GSvecCopy(&c->interest, (u8*)*(void**)((u8*)c->cobj + 0x28) + 0xc);
    }
    return slot;
}

/* ==================================================================
 * fn_800D29A0 - GS render: find free slot and init with defaults
 * Address: 0x800D29A0, Size: 0x134
 * ================================================================== */
void* fn_800D29A0(void) {
    u32 count = lbl_8047AA70;
    void* slot = (void*)lbl_8047AA6C;
    GSRenderCamera* c;
    f32 f_c9d8;
    f32 f_c9d4;
    f32 f_c9c0;
    f32 f_c994;
    f32 f_c990;
    f32 f_0;
    u32 zero;
    u32 one;
    u32 w;
    u32 h;
    {
        u32 i;
        for (i = 0; i < count; i++) {
            if (*(u8*)slot != 0) {
                slot = (u8*)slot + 0x128;
            } else {
                goto found;
            }
        }
    }
    slot = 0;
found:
    if (slot == 0) {
        return 0;
    }
    c = (GSRenderCamera*)slot;
    zero = 0;
    f_0 = lbl_8047C998;
    one = 1;
    f_c990 = lbl_8047C990;
    w = 0x280;
    h = 0x1e0;
    f_c994 = lbl_8047C994;
    f_c9c0 = lbl_8047C9C0;
    f_c9d4 = lbl_8047C9D4;
    f_c9d8 = lbl_8047C9D8;
    {
        u32 ptr10 = (u32)((u8*)slot + 0x10);
        u32 ptr24 = (u32)((u8*)slot + 0x24);
        *(u32*)((u8*)slot + 0x10) = zero;
        *(f32*)((u8*)slot + 0x14) = f_0;
        *(f32*)((u8*)slot + 0x18) = f_0;
        *(f32*)((u8*)slot + 0x1c) = f_0;
        *(u32*)((u8*)slot + 0x20) = zero;
        *(u32*)((u8*)slot + 0x24) = zero;
        *(f32*)((u8*)slot + 0x28) = f_0;
        *(f32*)((u8*)slot + 0x2c) = f_0;
        *(f32*)((u8*)slot + 0x30) = f_c990;
        *(u32*)((u8*)slot + 0x34) = zero;
        *(u32*)((u8*)slot + 0x38) = zero;
        *(u16*)((u8*)slot + 0x3c) = (u16)zero;
        *(u16*)((u8*)slot + 0x3e) = (u16)one;
        *(u16*)((u8*)slot + 0x40) = (u16)zero;
        *(u16*)((u8*)slot + 0x42) = (u16)w;
        *(u16*)((u8*)slot + 0x44) = (u16)zero;
        *(u16*)((u8*)slot + 0x46) = (u16)h;
        *(u16*)((u8*)slot + 0x48) = (u16)zero;
        *(u16*)((u8*)slot + 0x4a) = (u16)w;
        *(u16*)((u8*)slot + 0x4c) = (u16)zero;
        *(u16*)((u8*)slot + 0x4e) = (u16)h;
        *(u32*)((u8*)slot + 0x50) = ptr10;
        *(u32*)((u8*)slot + 0x54) = ptr24;
        *(f32*)((u8*)slot + 0x58) = f_0;
        *(u32*)((u8*)slot + 0x5c) = zero;
    }
    *(f32*)((u8*)slot + 0x60) = f_c994;
    *(f32*)((u8*)slot + 0x64) = f_c9c0;
    *(f32*)((u8*)slot + 0x68) = f_c9d4;
    *(f32*)((u8*)slot + 0x6c) = f_c9d8;
    c->cobj = HSD_CObjLoadDesc((u8*)slot + 0x38);
    c->active = (u8)one;
    c->useLookAt = (u8)zero;
    c->isAnimating = (u8)zero;
    c->hasAnimation = (u8)zero;
    c->desc = (GSRenderCameraDesc*)zero;
    return slot;
}

/* ==================================================================
 * fn_800D2AD4 - GS render: init render object array
 * Address: 0x800D2AD4, Size: 0x70
 * #pragma scheduling on required for correct instruction order
 * Allocates and zero-initializes an array of count render objects
 * (each 0x128 bytes). Stores array ptr to lbl_8047AA6C.
 * ================================================================== */
#pragma push
#pragma scheduling on
#pragma push
#pragma optimization_level 1
void fn_800D2AD4(u32 count) {
    u32 raw;
    u32 masked;
    lbl_8047AA70 = count;
    raw = _toolentryAlloc__FUl(count * 0x128);
    lbl_8047AA68 = (u16)raw;
    masked = (u16)raw;
    if (masked != 0) {
        u32 zero;
        u32 off;
        u32 i;
        lbl_8047AA6C = fn_800E27B0(masked);
        off = 0;
        zero = off;
        i = 0;
        while (i < lbl_8047AA70) {
            *(u8*)((u8*)lbl_8047AA6C + off) = (u8)zero;
            off += 0x128;
            i++;
        }
    }
}
#pragma pop
#pragma pop

/* ==================================================================
 * fn_800D2B44 - GS render: animation step callback
 * Address: 0x800D2B44, Size: 0x4C
 * Called by HSD_ForeachAnim as a callback with the JObj*.
 * Accumulates animation step into lbl_8047AA78.
 * ================================================================== */
void fn_800D2B44(void* obj) {
    if (!obj) {
        __assert(lbl_8047C9DC, 0xab, lbl_8047C9E4);
    }
    lbl_8047AA78 = lbl_8047C990 + *(f32*)((u8*)obj + 0xc);
}

/* ==================================================================
 * fn_800D2B90 - GS render: set current lighting context
 * Address: 0x800D2B90, Size: 0x258
 * ================================================================== */
#pragma push
#pragma optimization_level 2
void fn_800D2B90(void* arg1) {
    u32 savedColors[4];
    void* prevLight;
    savedColors[0] = lbl_80270350[0];
    savedColors[1] = lbl_80270350[1];
    savedColors[2] = lbl_80270350[2];
    savedColors[3] = lbl_80270350[3];
    prevLight = (void*)lbl_8047AA8C;
    if (prevLight != 0) {
        u16 ref = *(u16*)((u8*)prevLight + 0x4);
        if (ref != 0xffff) {
            *(u16*)((u8*)prevLight + 0x4) = ref - 1;
            if (ref != 0) {
                if (prevLight != 0) {
                    void** vtable = *(void***)prevLight;
                    ((void(*)(void*))vtable[0xc])(prevLight);
                    vtable = *(void***)prevLight;
                    ((void(*)(void*))vtable[0xd])(prevLight);
                }
            }
        }
        lbl_8047AA8C = 0;
    }
    if (arg1 == 0) {
        HSD_FogSet(0);
        fn_8016EA88();
        return;
    }
    {
        void* state = HSD_FogLoadDesc(arg1);
        lbl_8047AA8C = (u32)state;
    }
    fn_8016EB30();
    {
        u8 r = *(u8*)((u8*)arg1 + 0x10);
        u8 g = *(u8*)((u8*)arg1 + 0x11);
        u8 b2 = *(u8*)((u8*)arg1 + 0x12);
        u8 a = *(u8*)((u8*)arg1 + 0x13);
        f32 scale = lbl_8047C9F0;
        f32 fr, fg, fb2, fa;
        GSRenderState* rs = (GSRenderState*)lbl_8047AA80;
        fr = (f32)r;
        fg = (f32)g;
        fb2 = (f32)b2;
        fa = (f32)a;
        fr = fr / scale;
        fg = fg / scale;
        fb2 = fb2 / scale;
        fa = fa / scale;
        rs->fogEnabled = 1;
        rs->fogColorR = (u8)(s32)(fr * scale);
        rs->fogColorG = (u8)(s32)(fg * scale);
        rs->fogColorB = (u8)(s32)(fb2 * scale);
        rs->fogColorA = (u8)(s32)(fa * scale);
        if (rs->fogColorR == 0 &&
            rs->fogColorG == 0 &&
            rs->fogColorB == 0 &&
            rs->fogColorA == 0) {
            rs->fogEnabled = 0;
        }
    }
}
#pragma pop

/* ==================================================================
 * fn_800D2DE8 - GS render: sphere/ray intersection loop
 * Address: 0x800D2DE8, Size: 0x14C
 * ================================================================== */
s32 fn_800D2DE8(void* arg1, void* arg2, u32 count) {
    u32 n = count;
    void* sphere;
    u32 i;
    void* rays = arg1;
    void* out = arg2;
    s32 result;
    void* model;
    sphere = fn_800D7BF8(0);
    model = fn_800D7BF8(2);
    if (sphere == 0 || model == 0) {
        return 0;
    }
    {
        f32 center[7];
        f32 radii[6];
        center[0] = lbl_8047CA00;
        center[1] = *(f32*)((u8*)model + 0x0);
        center[2] = *(f32*)((u8*)model + 0x8);
        center[3] = *(f32*)((u8*)model + 0x14);
        center[4] = *(f32*)((u8*)model + 0x18);
        center[5] = *(f32*)((u8*)model + 0x28);
        center[6] = *(f32*)((u8*)model + 0x2c);
        radii[0] = lbl_8047CA00;
        radii[1] = lbl_8047CA00;
        radii[2] = lbl_8047CA04;
        radii[3] = lbl_8047CA08;
        radii[4] = lbl_8047CA00;
        radii[5] = lbl_8047CA0C;
        result = 2;
        i = 0;
        while (i < n) {
            f32 test[3];
            GSvecTransform(test, sphere, rays);
            if (test[2] >= *(f32*)((u8*)model + 0x2c)) {
                f32 val = lbl_8047CA0C;
                result = 3;
                *(f32*)((u8*)out + 0x8) = val;
                *(f32*)((u8*)out + 0x4) = val;
                *(f32*)((u8*)out + 0x0) = val;
            } else {
                GXProject(sphere,
                    center,
                    radii,
                    *(f32*)((u8*)rays + 0x0),
                    *(f32*)((u8*)rays + 0x4),
                    *(f32*)((u8*)rays + 0x8),
                    out,
                    (u8*)out + 0x4,
                    (u8*)out + 0x8);
            }
            i++;
            rays = (u8*)rays + 0xc;
            out = (u8*)out + 0xc;
        }
        return result;
    }
}

/* ==================================================================
 * fn_800D2F34 - GS render: single sphere/ray intersection
 * Address: 0x800D2F34, Size: 0x128
 * ================================================================== */
s32 fn_800D2F34(void* arg1, void* arg2) {
    f32 center[7];
    f32 radii[6];
    f32 testOut[3];
    void* sphere = fn_800D7BF8(0);
    void* model = fn_800D7BF8(2);
    if (sphere == 0 || model == 0) {
        return 0;
    }
    GSvecTransform(testOut, sphere, arg1);
    {
        f32 t = *(volatile f32*)&testOut[2];
        f32 model2c = *(f32*)((u8*)model + 0x2c);
        if (t >= model2c) {
            return 1;
        }
        center[0] = lbl_8047CA00;
        center[1] = *(f32*)((u8*)model + 0x0);
        center[2] = *(f32*)((u8*)model + 0x8);
        center[3] = *(f32*)((u8*)model + 0x14);
        center[4] = *(f32*)((u8*)model + 0x18);
        center[5] = *(f32*)((u8*)model + 0x28);
        center[6] = model2c;
    }
    radii[0] = lbl_8047CA00;
    radii[1] = lbl_8047CA00;
    radii[2] = lbl_8047CA04;
    radii[3] = lbl_8047CA08;
    radii[4] = lbl_8047CA00;
    radii[5] = lbl_8047CA0C;
    GXProject(sphere,
        center,
        radii,
        *(f32*)((u8*)arg1 + 0x0),
        *(f32*)((u8*)arg1 + 0x4),
        *(f32*)((u8*)arg1 + 0x8),
        arg2,
        (u8*)arg2 + 0x4,
        (u8*)arg2 + 0x8);
    return 2;
}

/* ==================================================================
 * fn_800D305C - GS render: get frame counter
 * Address: 0x800D305C, Size: 0xC
 * ================================================================== */
void fn_800D305C(u8 val) {
    GSRenderState* rs = (GSRenderState*)lbl_8047AA80;
    rs->frameLevel = val;
}

/* ==================================================================
 * fn_800D3068 - GS render: get render target width
 * Address: 0x800D3068, Size: 0xC
 * ================================================================== */
u32 fn_800D3068(void) {
    GSRenderState* rs = (GSRenderState*)lbl_8047AA80;
    return rs->renderWidth;
}
#endif
