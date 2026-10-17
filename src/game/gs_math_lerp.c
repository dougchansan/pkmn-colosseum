/**
 * @file gs_math_lerp.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: game/pxdvs/GSAPI/GSmath/GSlerp.cpp
 * Address range: 0x800E090C - 0x800E09E8 (2 functions)
 *
 * Both anchors monotonic (0x800EFBE8 < 0x800EFCB4); sizes 0xA8/0x34; our 0xDC vs XD 0x100 (XD has one extra 0x24 helper).
 *
 * Split from src/game/gs_render.c (physical XD source-unit split).
 * The dead #ifdef PCPORT reference block (never defined in configure.py,
 * same situation as gs_gfx.c) was stripped during the split.
 */

#include "dolphin/types.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char*, ...);             /* OSReport / GSlog */
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 size);

/* External functions referenced from asm wrappers */
extern void DCFlushRange(void* addr, u32 size);
extern u64 OSGetTime(void);
extern void GSgfxSetViewportJitter(s32, f32, f32, f32, f32, f32, f32);
extern void gfxRecordCommand(u32, ...);
extern void gfxBegin(u16);
extern void gfxSetupChannelsTev(u32);

/* GSmem */
extern u16   _toolentryAlloc__FUl(u32 size);                    /* GSmemAllocRaw */
extern void* GSmemLock(u16 handle);                  /* GSmemGetPtr */

/* SDK GX functions */
extern void  VIGetNextField(void);                        /* GXSetViewport */
extern void  GXSetViewportJitter(void);                        /* GXSetProjection */
extern void  GXSetViewport(void);                        /* GXLoadPosMtxImm */
extern void  GXInvalidateTexAll(void);                        /* GXInvalidateTexAll */

/* ===== String constants (rodata) ===== */
extern const char lbl_80270440[]; /* "GSgfx: invalid matrix index" */
extern const char lbl_80270460[]; /* "GSgfx: matrix stack underflow!" */
extern const char lbl_80270480[]; /* "GSgfx: matrix stack overflow!" */
extern const char lbl_802704A0[]; /* "0123456789ABCDEF" */
extern const char lbl_80270528[]; /* "GSmaterialSetPEdescr: Warning..." */
extern const char lbl_8027056C[]; /* "GSmaterialCreate: Run out of materials..." */
extern const char lbl_802705C0[]; /* "GSmaterial MObj" */
extern const char lbl_802705D0[]; /* "GSmaterial: Unsupported texture format..." */
extern const char lbl_80270610[]; /* "GSmaterial: Error creating environment map..." */

/* ===== BSS / global state ===== */
extern u32 lbl_8047AA80;   /* GSgfx state pointer (sda21) */
extern u8 lbl_80400248[];  /* GSgfx state backup buffer (0x5A0 bytes) */
extern u8 lbl_80400B28[];  /* light/material command buffer */

/* ===== Combined forward-decls (duplicated across split segments) ===== */

/* No-op functions (1) */
/* Address: 0x800DC874 | Size: 0x4 */
/* Forward declarations for self-referencing asm blocks */
extern void gfxFlushPendingVertex(void);
extern void _gfxEmitTexCoord1x8(u32 idx);
extern void _gfxEmitTexCoord1x16(u32 idx);
extern void _gfxEmitTexCoord2u8(u32 idx);
extern void _gfxEmitTexCoord2s16(u32 idx);
extern void _gfxEmitTexCoord2u16(u32 idx);
extern void _gfxEmitTexCoord2f32(u32 idx);
extern void _gfxEmitColor1x8(u32 idx);
extern void _gfxEmitColor1x16(u32 idx);
extern void _gfxEmitColor1u16(u32 idx);
extern void _gfxEmitColor1u32(u32 idx);
extern void _gfxEmitColor3u8(u32 idx);
extern void _gfxEmitColor4u8(u32 idx);
extern void _gfxEmitNormal1x8(void);
extern void _gfxEmitNormal1x16(void);
extern void _gfxEmitNormal3s8(void);
extern void _gfxEmitNormal3s16(void);
extern void _gfxEmitNormal3f32(void);
extern void _gfxEmitPos1x8(void);
extern void _gfxEmitPos1x16(void);
extern void _gfxEmitPos2s8(void);
extern void _gfxEmitPos2u8(void);
extern void _gfxEmitPos2s16(void);
extern void _gfxEmitPos2u16(void);
extern void _gfxEmitPos2f32(void);
extern void _gfxEmitPos3s8(void);
extern void _gfxEmitPos3u8(void);
extern void _gfxEmitPos3s16(void);
extern void _gfxEmitPos3u16(void);
extern void gfxVtxDescClear(u8*);
extern void gfxVtxDescSetAttr(u8*, u32, u32, u32, u32, u8, u32, u8);
extern void fn_800D7940(u32, u16);
extern void gfxLoadVtxDesc(u32);
extern void fn_800DB098(void);
extern void fn_800DB758(u16);
extern void lightGetFrameCount__FP9_HSD_AObj(u8*);
extern void GSlogVPrintf(void);
extern void GSlogFormat(void);
extern void fn_800E09E8(void*, void*, u32);
extern u8 GSmemDumpMap(u32, u32);
extern u32 _matGSmatObjMakeTExp(void*, void*, void*, void*, void*);
extern void _matGSmatEnableEnvMapExt(u8*);
extern s32 _matGSmatObjLoad(u8*);
extern void GSmtxConcat(void*, void*, void*);
extern void fn_800E02C4(void*);
extern void GSmtxRotateZ(void*, f32);
extern void GSmtxRotateY(void*, f32);
extern void GSmtxRotateX(void*, f32);
extern void fn_800E03E8(void*, f32, f32, f32);
extern void fn_800E0628(void*, void*);
extern void fn_800E064C(void*);
extern void GSmtx44Perspective(u8*);
extern void GSmtx44Ortho(void*, f32, f32, f32, f32, f32, f32);
extern void fn_800E0C78(void);
extern void GSmathInitCosTable(void);


/* ===== Combined externs (duplicated across all gs_render.c split segments;
 * de-duplicated by identifier from the whole original TU so any
 * cross-segment call/reference resolves regardless of which segment
 * the callee's real definition ended up in). ===== */
extern u8 lbl_8047AA91;
extern u32 lbl_8047AA80;
extern void GXSetPointSize(u32, u32);
extern f32 lbl_8047CA30;
extern f32 lbl_8047CA34;
extern f32 lbl_8047CA38;
extern void GXSetLineWidth(u32, u32);
extern void _gfxEmitMtxIdx(void);
extern void _gfxEmitPos3f32(void);
extern void GXBegin(u32, u32, u16);
extern u8 lbl_80314350[];
extern u8 lbl_804001F0[];
extern void fn_800D6A80(u16, s32, u32*, u32*);
extern u8 lbl_804007E8[];
extern void GXSetVtxAttrFmt(u32, u32, u32, u32, u8);
extern void GXClearVtxDesc(void);
extern void GXSetVtxDesc(u32, u32);
extern void GXSetArray(u32, u32, u8);
extern u8 lbl_80314370[];
extern u8 lbl_803143B4[];
extern u8 lbl_803143D8[];
extern u8 lbl_803143A8[];
extern u32 lbl_8047AAB0;
extern u32 lbl_8047AAAC;
extern u8 lbl_803144D0[];
extern u32 lbl_8047AAB4;
extern u16 lbl_8047AAA8;
extern u32 GScameraGetActiveCamera(void);
extern u32 GScameraGetMatrixPtr(void);
extern u32 GScameraGetInvMatrixPtr(void);
extern u32 GScameraGetProjMatrixPtr(void);
extern void GXLoadPosMtxImm(u32, u32);
extern void GXLoadNrmMtxImm(u32, u32);
extern void GXSetCurrentMtx(u32);
extern u8 lbl_8047AAC8;
extern u8 lbl_80314610[];
extern u32 lbl_8047AAC0;
extern u8 lbl_80400948[];
extern u32 lbl_8047AAC4;
extern u32 lbl_8047AABC;
extern u16 lbl_8047AAB8;
extern void GXSetTexCoordGen2(u32, u32, u32, u32, u32, u32);
extern void GXLoadTexMtxImm(void*, u32, u32);
extern u8 lbl_80314404[];
extern u8 lbl_80314454[];
extern u8 lbl_803144A8[];
extern u8 lbl_80314424[];
extern void GXInitTexObjWrapMode();
extern void GXInitTexObjLOD();
extern void GXLoadTlut();
extern void GXLoadTexObj();
extern u8 lbl_80314530[];
extern u32 lbl_8047CA40;
extern u32 lbl_8047CA48;
extern u8 lbl_80314510[];
extern u8 lbl_803144F0[];
extern void GXSetTevDirect(u32);
extern void GXSetNumIndStages(u32);
extern void GXSetNumChans();
extern void GXSetChanCtrl();
extern void GXSetNumTexGens();
extern void GXSetNumTevStages();
extern void GXSetIndTexOrder();
extern void GXSetIndTexCoordScale();
extern void GXSetIndTexMtx();
extern void GXSetTevOrder();
extern void GXSetTevColorOp();
extern void GXSetTevAlphaOp();
extern void GXSetTevColorIn();
extern void GXSetTevAlphaIn();
extern void GXSetTevKColorSel();
extern void GXSetTevKAlphaSel();
extern void GXSetTevIndirect();
extern void fn_800BBC7C();
extern void fn_800BBCE0();
extern void fn_800BBE8C();
extern void fn_800BBF98();
extern void fn_800BBFDC();
extern void GXSetTevKColor();
extern void GXSetTevSwapModeTable();
extern void GXSetProjection(void*, u32);
extern f32 lbl_8047CA50;
extern f32 lbl_8047CA54;
extern void GScameraSetViewport(void*, u16, u16, u16, u16);
extern u32 lbl_8047CA60;
extern u32 lbl_8047CA68;
extern u32 lbl_8047CA58;
extern void GXSetScissor(u32, u32, u32, u32);
extern void GScameraSetScissor(u32, u16, u16, u16, u16);
extern void HSD_FogSet(u32);
extern u32 lbl_8047AA8C;
extern void GXSetClipMode(u32);
extern void GXSetCullMode(u32);
extern u8 lbl_8031453C[];
extern void GXSetDither(u32);
extern void GXSetAlphaCompare(u32, u8, u32, u32, u8);
extern u8 lbl_8031457C[];
extern u8 lbl_8031456C[];
extern void GXSetZMode(u32, u32, u32);
extern void GXSetZCompLoc(u32);
extern u8 lbl_8031454C[];
extern void GXSetDstAlpha(u32);
extern void GXSetBlendMode(u32, u32, u32, u32);
extern u8 lbl_803145D0[];
extern u32 lbl_8031459C[];
extern u32 lbl_803145A8[];
extern void jumptable_803152B8();
extern void jumptable_80315340();
extern void jumptable_80315320();
extern void fn_800E24B0(u16);
extern void GSmemFree(u16);
extern void GXCallDisplayList(u32, u32);
extern void fn_800E2AF8(u16);
extern u16 GSmemAlloc(u32, u32);
extern u32 lbl_8047AAD8;
extern u32 lbl_8047AAD4;
extern void jumptable_80315364();
extern u32 GStextureUnlockImage(void*);
extern u8 lbl_80400EE0[];
extern u8 lbl_8047AAE0;
extern void GStextureGetFormat(void);
extern void GStextureSetWrap(void);
extern void GStextureSetFilter(void);
extern void* GStextureLockImage(void*, u32);
extern void GStextureConvertFromHW(void);
extern void HSD_LObjReqAnimAll(void*, f32);
extern void HSD_LObjAnimAll(void*);
extern u32 lbl_8047AAEC;
extern u32 lbl_8047CA80;
extern f32 lbl_8047CA70;
extern u32 lbl_8047CA74;
extern f32 lbl_8047CA78;
extern u32 lbl_8047AAF0;
extern void HSD_LObjSetPosition();
extern void HSD_LObjSetInterest();
extern void HSD_LObjRemoveAnimAll(void*);
extern void HSD_LObjAddAnimAll(void*, void*);
extern void HSD_ForeachAnim(void*, u32, u32, void*, u32, ...);
extern s32 fn_800D37CC(void);
extern void HSD_AObjSetRate(void);
extern f32 lbl_8047AAF4;
extern f32 lbl_8047CA88;
extern void GSlightSetAnimIndex(u8*, u32);
extern void HSD_LObjGetPosition(void*, void*);
extern void HSD_LObjGetInterest(void*, void*);
extern void HSD_LObjSetColor(u32, u8*);
extern void HSD_LObjClearFlags(u32, u32);
extern void HSD_LObjSetFlags(u32, u32);
extern u32 HSD_LObjLoadDesc(void*);
extern u32 lbl_8047CA8C;
extern u16 lbl_8047AAE8;
extern void __assert(u8*, u32, u8*);
extern u8 lbl_8047CA90;
extern u8 lbl_8047CA98;
extern void HSD_LObjDeleteCurrentAll(void*);
extern void HSD_LObjAddCurrentAll(void);
extern void HSD_LObjSetup(void*);
extern u32 lbl_8047AAF8;
extern u32 lbl_8047AB08;
extern u32 lbl_8047AAFA;
extern u32 lbl_8047AAFC;
extern void OSTicksToCalendarTime(void);
extern void logVsnprintf_float(void);
extern u32 strlen(const char* s);
extern u32 lbl_8047AB11;
extern u8 lbl_80400F30[];
extern u8 lbl_802704B4[];
extern u8 lbl_80400F44[];
extern u32 lbl_8047AB0C;
extern u32 lbl_8047AB00;
extern u32 lbl_8047AB04;
extern u8 lbl_80401044[];
extern u8 lbl_80401058[];
extern u32 lbl_8047AB10;
extern void jumptable_80315388();
extern void __va_arg();
extern u8 lbl_80401168[];
extern u8 lbl_80401178[];
extern u32 lbl_80478AE8;
extern u8 lbl_8047CAA0[];
extern u8 lbl_8047CAA8[];
extern void GXDrawDone(u32);
extern void HSD_ImageDescFree(u32);
extern void* HSD_ImageDescAlloc(void);
extern u16 GStextureGetXsize(void*);
extern u16 GStextureGetYsize(void*);
extern void* GStextureGetGXformat(void*, u32);
extern u8 GStextureGetMiplevels(void*);
extern f32 lbl_8047CAC8;
extern void HSD_MObjSetAlpha(u32, ...);
extern f64 lbl_8047CAD0;
extern f32 lbl_8047CACC;
extern void HSD_TObjRemove(void*);
extern void HSD_MObjCompileTev(void*);
extern u32 HSD_MObjGetFlags(void*);
extern void HSD_MObjClearFlags(void*, u32);
extern void HSD_MObjSetFlags(void*, void*);
extern u32 lbl_8047AB20;
extern u32 lbl_8047AB1C;
extern void HSD_MObjSetDefaultClass(void*);
extern u16 lbl_8047AB18;
extern u8 lbl_80315490[];
extern void hsdInitClassInfo(void*, void*, void*, void*, u32, u32);
extern u8 lbl_8036CB30[];
extern void HSD_TExpGetType();
extern void fn_801B6DC0();
extern void HSD_TExpCnst();
extern void HSD_TExpTev();
extern void HSD_TExpColorOp();
extern void HSD_TExpColorIn();
extern void HSD_TExpAlphaOp();
extern void HSD_TExpAlphaIn();
extern void HSD_ImageDescRemove(void);
extern void HSD_TObjLoadDesc(void);
extern void HSD_MObjGetTObj(void);
extern void HSD_MObjAddTObjNext(void);
extern void fn_801A6DA0(void);
extern u8 lbl_803154E4[];
extern void PSMTXMultVec(void*, void*, void*);
extern void PSVECCrossProduct(void*, void*, void*);
extern void PSVECDotProduct(void);
extern void PSVECSquareDistance(void);
extern void PSVECDistance(void);
extern void PSVECNormalize(void*, void*);
extern void PSVECMag(void);
extern void PSVECScale(void*, void*, f32);
extern const f32 lbl_8047CAD8;
extern void PSVECSubtract(void*, void*, void*);
extern void PSVECAdd(void*, void*, void*);
extern f32 lbl_8047CADC;
extern void C_MTXLookAt(void);
extern void PSMTXTranspose(void*, void*);
extern void PSMTXInverse(void*, void*);
extern void PSMTXConcat(void*, void*, void*);
extern void PSMTXScaleApply(void*, void*);
extern void PSMTXRotRad(void*, u32);
extern void PSMTXTransApply(void*, void*, f32, f32, f32);
extern void PSMTXQuat(void);
extern u8 lbl_80315568[];
extern void C_MTXPerspective(void);
extern void C_MTXOrtho(void);
extern void C_QUATSlerp(void*, void*, void*);
extern void EulerToQuat(void*, void*);
extern void C_QUATRotAxisRad(void);
extern void PSQUATMultiply(void*, void*, void*);
extern u32 lbl_8047CAE4;
extern u32 lbl_8047CAE0;
extern u32 lbl_8047CAE8;
extern f32 lbl_8047CAF0;
extern f32 lbl_8047CAF4;
extern u32 lbl_8047CB00;
extern u32 lbl_8047CAF8;
extern u32 lbl_8047CB08;
extern u32 lbl_8047CAFC;
extern f32 HSD_Randf(void);
extern f32 lbl_8047CB10;
extern u32 HSD_Rand(void);
extern u32 lbl_80478C94;
extern f64 fmod(f64 x, f64 y);
extern s32 __cvt_fp2unsigned(f32 x);
extern f32 lbl_8047CB20;
extern f32 lbl_8047CB1C;
extern f32 lbl_8047CB24;
extern f64 lbl_8047CB28;
extern f32 lbl_8047CB18;
extern f32 lbl_8047CB34;
extern f32 lbl_8047CB30;
extern f32 lbl_804011B8[];
extern f64 cos(f32);
extern f32 lbl_8047CB38;
extern f64 lbl_8047CB40;
extern u8 lbl_80270658[];
extern u32 lbl_8047AB30;
extern u32 lbl_8047AB68;
extern u32 lbl_8047AB64;
extern u32 lbl_8047AB38;
extern u32 lbl_8047AB34;
extern u32 lbl_8047AB28;
extern u32 lbl_8047AB4C;
extern u32 lbl_8047AB48;
extern u32 lbl_8047AB60;
extern u32 lbl_8047AB5C;
extern u32 lbl_8047AB58;
extern u32 lbl_8047AB54;
extern u32 lbl_8047AB50;
extern u32 lbl_8047CB50;
extern u32 lbl_8047CB48;
extern u32 lbl_8047AB3C;

extern f32 lbl_8047CAF0;
extern f32 lbl_8047CAF4;
#if 0
asm void GSlerpGetLinearInterpolationVector(void) {
#include "src/game/gs_render_GSlerpGetLinearInterpolationVector.inc"
}
#else
void GSlerpGetLinearInterpolationVector(void* dst, void* src, void* target, f32 t) {
    if (t <= lbl_8047CAF0) {
        GSvecCopy(dst, src);
    } else if (t >= lbl_8047CAF4) {
        GSvecCopy(dst, target);
    } else {
        GSvecSub(dst, target, src);
        vecScale(dst, dst, t);
        GSvecAdd(dst, dst, src);
    }
}
#endif

extern f32 lbl_8047CAF0;
extern f32 lbl_8047CAF4;
#if 0
asm void GSlerpGetLinearInterpolationFloat(void) {
#include "src/game/gs_render_fn_800E09B4.inc"
}
#else
f32 GSlerpGetLinearInterpolationFloat(f32 start, f32 end, f32 t) {
    if (t <= lbl_8047CAF0) return start;
    if (t >= lbl_8047CAF4) return end;
    return t * (end - start) + start;
}
#endif

