/**
 * @file gs_material.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: game/pxdvs/GSAPI/GSmaterial/GSmaterial.cpp
 * Address range: 0x800DF028 - 0x800DFEEC (24 functions)
 *
 * 20 XD-shared anchors FULLY MONOTONIC (0x800EED24 -> 0x800EF908), same intra-TU order as XD. Ends exactly like XD TU: _matGSmatObjLoad is the last function (XD TU end 0x800EF95C = ObjLoad end), so boundary placed at 0x800DFEEC. GSmaterialCreate has no XD address (Colosseum-only symbol) but sits inside the anchored run.
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
extern void fn_800D3EC4(s32, f32, f32, f32, f32, f32, f32);
extern void fn_800D4F98(u32, ...);
extern void fn_800D67BC(u16);
extern void fn_800D892C(u32);

/* GSmem */
extern u16   _toolentryAlloc__FUl(u32 size);                    /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                  /* GSmemGetPtr */

/* SDK GX functions */
extern void  fn_800AA2F0(void);                        /* GXSetViewport */
extern void  fn_800BD640(void);                        /* GXSetProjection */
extern void  fn_800BD744(void);                        /* GXLoadPosMtxImm */
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
extern void fn_800D6B00(void);
extern void fn_800D724C(u32 idx);
extern void fn_800D7268(u32 idx);
extern void fn_800D72A4(u32 idx);
extern void fn_800D72C4(u32 idx);
extern void fn_800D72E4(u32 idx);
extern void fn_800D7304(u32 idx);
extern void fn_800D7328(u32 idx);
extern void fn_800D7344(u32 idx);
extern void fn_800D7360(u32 idx);
extern void fn_800D737C(u32 idx);
extern void fn_800D7398(u32 idx);
extern void fn_800D73C4(u32 idx);
extern void fn_800D73F8(void);
extern void fn_800D740C(void);
extern void fn_800D7420(void);
extern void fn_800D7444(void);
extern void fn_800D7468(void);
extern void fn_800D748C(void);
extern void fn_800D74A0(void);
extern void fn_800D74B4(void);
extern void fn_800D74D0(void);
extern void fn_800D74EC(void);
extern void fn_800D7508(void);
extern void fn_800D7524(void);
extern void fn_800D7540(void);
extern void fn_800D7564(void);
extern void fn_800D7588(void);
extern void fn_800D75AC(void);
extern void fn_800D7650(u8*);
extern void fn_800D7868(u8*, u32, u32, u32, u32, u8, u32, u8);
extern void fn_800D7940(u32, u16);
extern void fn_800D7A70(u32);
extern void fn_800DB098(void);
extern void fn_800DB758(u16);
extern void lightGetFrameCount__FP9_HSD_AObj(u8*);
extern void fn_800DE09C(void);
extern void fn_800DE128(void);
extern void fn_800E09E8(void*, void*, u32);
extern u8 fn_800E0E14(u32, u32);
extern u32 _matGSmatObjMakeTExp(void*, void*, void*, void*, void*);
extern void _matGSmatEnableEnvMapExt(u8*);
extern s32 _matGSmatObjLoad(u8*);
extern void fn_800E0290(void*, void*, void*);
extern void fn_800E02C4(void*);
extern void fn_800E02E8(void*, f32);
extern void fn_800E032C(void*, f32);
extern void fn_800E0370(void*, f32);
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
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 size);
extern void DCFlushRange(void* addr, u32 size);
extern u64 OSGetTime(void);
extern void fn_800D3EC4(s32, f32, f32, f32, f32, f32, f32);
extern void fn_800D4F98(u32, ...);
extern void fn_800D67BC(u16);
extern void fn_800D892C(u32);
extern u8 lbl_8047AA91;
extern void fn_800D6B00(void);
extern void fn_800D724C(u32 idx);
extern void fn_800D7268(u32 idx);
extern void fn_800D72A4(u32 idx);
extern void fn_800D72C4(u32 idx);
extern void fn_800D72E4(u32 idx);
extern void fn_800D7304(u32 idx);
extern void fn_800D7328(u32 idx);
extern void fn_800D7344(u32 idx);
extern void fn_800D7360(u32 idx);
extern void fn_800D737C(u32 idx);
extern void fn_800D7398(u32 idx);
extern void fn_800D73C4(u32 idx);
extern void fn_800D73F8(void);
extern void fn_800D740C(void);
extern void fn_800D7420(void);
extern void fn_800D7444(void);
extern void fn_800D7468(void);
extern void fn_800D748C(void);
extern void fn_800D74A0(void);
extern void fn_800D74B4(void);
extern void fn_800D74D0(void);
extern void fn_800D74EC(void);
extern void fn_800D7508(void);
extern void fn_800D7524(void);
extern void fn_800D7540(void);
extern void fn_800D7564(void);
extern void fn_800D7588(void);
extern void fn_800D75AC(void);
extern void fn_800D7650(u8*);
extern void fn_800D7868(u8*, u32, u32, u32, u32, u8, u32, u8);
extern void fn_800D7940(u32, u16);
extern void fn_800D7A70(u32);
extern void fn_800DB098(void);
extern void fn_800DB758(u16);
extern void lightGetFrameCount__FP9_HSD_AObj(u8*);
extern void fn_800DE09C(void);
extern void fn_800DE128(void);
extern void fn_800E09E8(void*, void*, u32);
extern u8 fn_800E0E14(u32, u32);
extern u32 _matGSmatObjMakeTExp(void*, void*, void*, void*, void*);
extern void _matGSmatEnableEnvMapExt(u8*);
extern s32 _matGSmatObjLoad(u8*);
extern void fn_800E0290(void*, void*, void*);
extern void fn_800E02C4(void*);
extern void fn_800E02E8(void*, f32);
extern void fn_800E032C(void*, f32);
extern void fn_800E0370(void*, f32);
extern void fn_800E03E8(void*, f32, f32, f32);
extern void fn_800E0628(void*, void*);
extern void fn_800E064C(void*);
extern void GSmtx44Perspective(u8*);
extern void GSmtx44Ortho(void*, f32, f32, f32, f32, f32, f32);
extern void fn_800E0C78(void);
extern void GSmathInitCosTable(void);
extern u32 lbl_8047AA80;
extern void fn_800B944C(u32, u32);
extern f32 lbl_8047CA30;
extern f32 lbl_8047CA34;
extern f32 lbl_8047CA38;
extern void fn_800B9404(u32, u32);
extern void fn_800D7230(void);
extern void fn_800D75D0(void);
extern void fn_800B928C(u32, u32, u16);
extern u8 lbl_80314350[];
extern u8 lbl_804001F0[];
extern void fn_800D6A80(u16, s32, u32*, u32*);
extern u8 lbl_804007E8[];
extern void fn_800B7D74(u32, u32, u32, u32, u8);
extern void fn_800B7D3C(void);
extern void fn_800B7874(u32, u32);
extern void fn_800B84E0(u32, u32, u8);
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
extern u32 fn_800D1D00(void);
extern u32 fn_800D1B3C(void);
extern u32 GScameraGetProjMatrixPtr(void);
extern void GXLoadPosMtxImm(u32, u32);
extern void GXLoadNrmMtxImm(u32, u32);
extern void fn_800BD554(u32);
extern u8 lbl_8047AAC8;
extern u8 lbl_80314610[];
extern u32 lbl_8047AAC0;
extern u8 lbl_80400948[];
extern u32 lbl_8047AAC4;
extern u32 lbl_8047AABC;
extern u16 lbl_8047AAB8;
extern void fn_800B857C(u32, u32, u32, u32, u32, u32);
extern void GXLoadTexMtxImm(void*, u32, u32);
extern u8 lbl_80314404[];
extern u8 lbl_80314454[];
extern u8 lbl_803144A8[];
extern u8 lbl_80314424[];
extern void fn_800BAE34();
extern void fn_800BACA0();
extern void fn_800BB098();
extern void GXLoadTexObj();
extern u8 lbl_80314530[];
extern u32 lbl_8047CA40;
extern u32 lbl_8047CA48;
extern u8 lbl_80314510[];
extern u8 lbl_803144F0[];
extern void fn_800BBC34(u32);
extern void fn_800BBC0C(u32);
extern void fn_800BA6B0();
extern void fn_800BA6F4();
extern void fn_800B884C();
extern void fn_800BC8C8();
extern void fn_800BBAF8();
extern void fn_800BB97C();
extern void fn_800BB81C();
extern void fn_800BC6F0();
extern void fn_800BC228();
extern void fn_800BC290();
extern void fn_800BC1A0();
extern void fn_800BC1E4();
extern void fn_800BC454();
extern void fn_800BC4C0();
extern void fn_800BB780();
extern void fn_800BBC7C();
extern void fn_800BBCE0();
extern void fn_800BBE8C();
extern void fn_800BBF98();
extern void fn_800BBFDC();
extern void fn_800BC3E0();
extern void fn_800BC580();
extern void fn_800BD2E0(void*, u32);
extern f32 lbl_8047CA50;
extern f32 lbl_8047CA54;
extern void GScameraSetViewport(void*, u16, u16, u16, u16);
extern u32 lbl_8047CA60;
extern u32 lbl_8047CA68;
extern u32 lbl_8047CA58;
extern void fn_800BD7A0(u32, u32, u32, u32);
extern void fn_800D2150(u32, u16, u16, u16, u16);
extern void HSD_FogSet(u32);
extern u32 lbl_8047AA8C;
extern void GXSetClipMode(u32);
extern void fn_800B94F0(u32);
extern u8 lbl_8031453C[];
extern void fn_800BCFDC(u32);
extern void fn_800BC618(u32, u8, u32, u32, u8);
extern u8 lbl_8031457C[];
extern u8 lbl_8031456C[];
extern void GXSetZMode(u32, u32, u32);
extern void fn_800BCEBC(u32);
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
extern void fn_800E209C(u16);
extern void GXCallDisplayList(u32, u32);
extern void fn_800E2AF8(u16);
extern u16 fn_800E2C04(u32, u32);
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
extern void fn_801B707C();
extern void fn_801B6E74();
extern void fn_801B64EC();
extern void fn_801B6CD8();
extern void fn_801B5F08();
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
extern void fn_801ADAAC(void*, void*);
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
extern f32 fn_801ADC7C(void);
extern f32 lbl_8047CB10;
extern u32 fn_801ADCD8(void);
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

extern void* HSD_ImageDescAlloc(void);
extern u16 GStextureGetXsize(void*);
extern u16 GStextureGetYsize(void*);
extern void* GStextureGetGXformat(void*, u32);
extern u8 GStextureGetMiplevels(void*);
extern f32 lbl_8047CAC8;
#if 0
asm void GSmaterialSetTexture(void) {
#include "src/game/gs_render_GSmaterialSetTexture.inc"
}
#else
void GSmaterialSetTexture(u8* obj, void* image) {
    void* desc;
    u32 transparent;
    f32 scale;

    if ((*(u32*)(obj + 0x38) + 0x01020000) == 0xfefe) {
        *(u32*)(obj + 0x38) = *(u32*)(*(u8**)(*(u8**)(obj + 0x8) + 0x8) + 0x58);
        desc = HSD_ImageDescAlloc();
    } else {
        desc = *(void**)(*(u8**)(*(u8**)(obj + 0x8) + 0x8) + 0x58);
    }

    if (desc != 0) {
        *(void**)desc = GStextureLockImage(image, 0);
        *(u16*)((u8*)desc + 0x4) = GStextureGetXsize(image);
        *(u16*)((u8*)desc + 0x6) = GStextureGetYsize(image);
        *(void**)((u8*)desc + 0x8) = GStextureGetGXformat(image, 1);
        transparent = GStextureGetMiplevels(image);
        scale = lbl_8047CAC8;
        *(u32*)((u8*)desc + 0xc) = ((0 - transparent) | transparent) >> 31;
        *(f32*)((u8*)desc + 0x10) = scale;
        *(f32*)((u8*)desc + 0x14) = scale;
        GXDrawDone(GStextureUnlockImage(image));
        *(void**)(*(u8**)(*(u8**)(obj + 0x8) + 0x8) + 0x58) = desc;
    }
}
#endif

#if 0
asm void GSmaterialGetModulate(void) {
#include "src/game/gs_render_GSmaterialGetModulate.inc"
}
#else
void GSmaterialGetModulate(u8* src, u8* dst) { dst[0] = src[0xc]; dst[1] = src[0xd]; dst[2] = src[0xe]; dst[3] = src[0xf]; }
#endif

extern void HSD_MObjSetAlpha(u32, ...);
extern f64 lbl_8047CAD0;
extern f32 lbl_8047CACC;
#if 0
asm void GSmaterialResetAlpha(void) {
#include "src/game/gs_render_GSmaterialResetAlpha.inc"
}
#else
void GSmaterialResetAlpha(u8* obj) {
    extern void HSD_MObjSetAlpha(u32, f32);
    HSD_MObjSetAlpha(*(u32*)(obj + 0x8), (f32)obj[0x1] / lbl_8047CACC);
}
#endif

extern f32 lbl_8047CACC;
#if 0
asm void GSmaterialStoreAlpha(void) {
#include "src/game/gs_render_GSmaterialStoreAlpha.inc"
}
#else
#pragma scheduling off
void GSmaterialStoreAlpha(u8* obj) { f32 scale = lbl_8047CACC; obj[0x1] = (u8)(s32)(scale * *(f32*)(*(u32*)(*(u32*)((u8*)obj + 0x8) + 0xc) + 0xc)); }
#pragma scheduling on
#endif

#if 0
asm void GSmaterialSetEnvMapBlendValue(void) {
#include "src/game/gs_render_GSmaterialSetEnvMapBlendValue.inc"
}
#else
void GSmaterialSetEnvMapBlendValue(u8* obj, f32 val) { u32 ptr = *(u32*)(obj + 0x20); *(f32*)(obj + 0x34) = val; if (ptr) *(f32*)(ptr + 0x50) = val; }
#endif

#if 0
asm void GSmaterialSetEnvMapParams(void) {
#include "src/game/gs_render_GSmaterialSetEnvMapParams.inc"
}
#else
void GSmaterialSetEnvMapParams(u8* obj, u32 a, u32 b, f32 c, u32 d) { *(u32*)(obj+0x2c) = a; *(u32*)(obj+0x30) = b; *(f32*)(obj+0x34) = c; *(u32*)(obj+0x28) = d; }
#endif

#if 0
asm void GSmaterialSetModulate(void) {
#include "src/game/gs_render_GSmaterialSetModulate.inc"
}
#else
void GSmaterialSetModulate(u8* dst, u8* src) { dst[0xc] = src[0]; dst[0xd] = src[1]; dst[0xe] = src[2]; dst[0xf] = src[3]; }
#endif

#if 0
asm void GSmaterialSetColorChannels(void) {
#include "src/game/gs_render_fn_800DF208.inc"
}
#else
void GSmaterialSetColorChannels(u8* obj, u32 a, u32 b, u32 c, u32 d) { *(u32*)(obj+0x10) = a; *(u32*)(obj+0x14) = b; *(u32*)(obj+0x18) = c; *(u32*)(obj+0x1c) = d; }
#endif

#if 0
asm void GSmaterialSetAlpha(void) {
#include "src/game/gs_render_GSmaterialSetAlpha.inc"
}
#else
void GSmaterialSetAlpha(u8* obj) {
    extern void HSD_MObjSetAlpha(u32);
    HSD_MObjSetAlpha(*(u32*)((u8*)obj + 0x8));
}
#endif

#if 0
asm void GSmaterialGetEnabledExtensions(void) {
#include "src/game/gs_render_GSmaterialGetEnabledExtensions.inc"
}
#else
u16 GSmaterialGetEnabledExtensions(u8* obj) { return *(u16*)((u8*)obj + 0x2); }
#endif

extern void HSD_TObjRemove(void*);
extern void HSD_MObjCompileTev(void*);
#if 0
asm void GSmaterialDisableExtension(void) {
#include "src/game/gs_render_GSmaterialDisableExtension.inc"
}
#else
void GSmaterialDisableExtension(u8* obj, u32 flags) {
    u32 bits;
    u8* mobj;
    u8* image;
    u8* it;
    u32 desc;

    bits = flags & *(u16*)(obj + 0x2);
    mobj = *(u8**)(obj + 0x8);
    if (bits & 1) {
        obj[0xf] = 0x7f;
        obj[0xe] = 0x7f;
        obj[0xd] = 0x7f;
        obj[0xc] = 0x7f;
    }
    if (bits & 2) {
        *(u32*)(obj + 0x10) = 0;
        *(u32*)(obj + 0x14) = 1;
        *(u32*)(obj + 0x18) = 2;
        *(u32*)(obj + 0x1c) = 3;
    }
    if (bits & 4) {
        image = *(u8**)(obj + 0x20);
        desc = *(u32*)(obj + 0x24);
        it = *(u8**)(mobj + 0x8);
        if (it != 0 && image != 0) {
            if (it == image) {
                *(u32*)(mobj + 0x8) = *(u32*)(image + 0x8);
            } else {
                while (it != 0) {
                    if (*(u8**)(it + 0x8) == image) {
                        *(u32*)(it + 0x8) = *(u32*)(image + 0x8);
                        break;
                    }
                    it = *(u8**)(it + 0x8);
                }
            }
        }
        if (desc != 0) {
            HSD_ImageDescFree(desc);
        }
        if (image != 0) {
            HSD_TObjRemove(image);
        }
        *(u32*)(obj + 0x28) = 0;
    }

    *(u16*)(obj + 0x2) &= ~bits;
    if (*(u16*)(obj + 0x2) == 0) {
        *(u32*)(mobj + 0x20) = 0;
    }
    HSD_MObjCompileTev(mobj);
}
#endif

#if 0
asm void GSmaterialEnableExtension(void) {
#include "src/game/gs_render_fn_800DF384.inc"
}
#else
void GSmaterialEnableExtension(u8* obj, u32 flags) {
    u32 new_bits;
    u8* ptr;
    u16 cur;
    cur = *(u16*)(obj + 0x2);
    ptr = *(u8**)(obj + 0x8);
    new_bits = flags & ~cur;
    if (!new_bits) return;
    *(u8**)(ptr + 0x20) = obj;
    if (new_bits & (1 << 2)) _matGSmatEnableEnvMapExt(obj);
    *(u16*)(obj + 0x2) = *(u16*)(obj + 0x2) | new_bits;
    HSD_MObjCompileTev(ptr);
}
#endif

extern u32 HSD_MObjGetFlags(void*);
#if 0
asm void GSmaterialGetFlags(void) {
#include "src/game/gs_render_fn_800DF3F0.inc"
}
#else
u32 GSmaterialGetFlags(void* obj) {
    u32 v = HSD_MObjGetFlags(*(void**)((u8*)obj + 8));
    u32 flags = 0;
    if (v & 0x1) flags |= 0x1;
    if (v & 0x2) flags |= 0x2;
    if (v & 0x4) flags |= 0x4;
    if (v & 0x8) flags |= 0x8;
    if (v & 0x40000000) flags |= 0x10;
    if (v & 0x2000) flags |= 0x20;
    if (v & 0x4000) flags |= 0x40;
    return flags;
}
#endif

#if 0
asm void GSmaterialResetPEdescr(void) {
#include "src/game/gs_render_fn_800DF470.inc"
}
#else
void GSmaterialResetPEdescr(u8* obj) {
    u32 v = *(u32*)(obj + 0x3c);
    if ((u32)(v + 0x01020000) == 0xfefe) return;
    *(u32*)(*(u32*)(obj + 0x8) + 0x10) = v;
    *(u32*)(obj + 0x3c) = 0xfefefefe;
}
#endif

#if 0
asm void GSmaterialSetPEdescr(void) {
#include "src/game/gs_render_GSmaterialSetPEdescr.inc"
}
#else
void GSmaterialSetPEdescr(u8* obj, u32 new_val) {
    u32 sentinel = *(u32*)(obj + 0x3c);
    if ((u32)(sentinel + 0x01020000) == 0xfefe) {
        *(u32*)(obj + 0x3c) = *(u32*)(*(u8**)(obj + 0x8) + 0x10);
    } else {
        GSlogWrite(lbl_80270528);
    }
    *(u32*)(*(u8**)(obj + 0x8) + 0x10) = new_val;
}
#endif

extern void HSD_MObjClearFlags(void*, u32);
extern void HSD_MObjSetFlags(void*, void*);
#if 0
asm void GSmaterialResetFlags(void) {
#include "src/game/gs_render_fn_800DF504.inc"
}
#else
void GSmaterialResetFlags(u8* obj) {
    HSD_MObjClearFlags(*(void**)(obj + 0x8), 0x4000600f);
    HSD_MObjSetFlags(*(void**)(obj + 0x8), *(void**)(obj + 0x4));
    HSD_MObjCompileTev(*(void**)(obj + 0x8));
}
#endif

#if 0
asm void GSmaterialSetFlags(void) {
#include "src/game/gs_render_GSmaterialSetFlags.inc"
}
#else
void GSmaterialSetFlags(u8* obj, u32 flags) {
    u32 r4;
    *(u32*)(obj + 4) = HSD_MObjGetFlags(*(void**)(obj + 8));
    HSD_MObjClearFlags(*(void**)(obj + 8), 0x4000600f);
    r4 = 0;
    if (flags & 0x01) r4 |= 0x1;
    if (flags & 0x02) r4 |= 0x2;
    if (flags & 0x04) r4 |= 0x4;
    if (flags & 0x08) r4 |= 0x8;
    if (flags & 0x10) r4 |= 0x40000000;
    if (flags & 0x20) r4 |= 0x2000;
    if (flags & 0x40) r4 |= 0x4000;
    HSD_MObjSetFlags(*(void**)(obj + 8), (void*)r4);
    HSD_MObjCompileTev(*(void**)(obj + 8));
}
#endif

#if 0
asm void fn_800DF608(void) {
#include "src/game/gs_render_fn_800DF608.inc"
}
#else
void fn_800DF608(u8* obj) {
    u32 sentinel;
    u32 desc;
    u8* mobj;

    if (obj == 0) {
        return;
    }

    sentinel = *(u32*)(obj + 0x3c);
    if ((u32)(sentinel + 0x01020000) != 0xfefe) {
        *(u32*)(*(u8**)(obj + 0x8) + 0x10) = sentinel;
        *(u32*)(obj + 0x3c) = 0xfefefefe;
    }

    desc = *(u32*)(obj + 0x38);
    if ((u32)(desc + 0x01020000) != 0xfefe) {
        GXDrawDone(desc);
        mobj = *(u8**)(obj + 0x8);
        if (mobj != 0 && *(u32*)(mobj + 0x8) != 0) {
            HSD_ImageDescFree(*(u32*)(*(u32*)(mobj + 0x8) + 0x58));
            *(u32*)(*(u32*)(mobj + 0x8) + 0x58) = desc;
        }
        *(u32*)(obj + 0x38) = 0xfefefefe;
    }

    GSmaterialDisableExtension(obj, *(u16*)(obj + 0x2));
    obj[0] = 0;
}
#endif

extern u32 lbl_8047AB20;
extern u32 lbl_8047AB1C;
#if 0
asm void GSmaterialCreate(void) {
#include "src/game/gs_render_GSmaterialCreate.inc"
}
#else
u8* GSmaterialCreate(void) {
    u32 count;
    u32 i;
    u8* p;
    count = lbl_8047AB20;
    p = (u8*)lbl_8047AB1C;
    for (i = 0; i < count; i++) {
        if (p[0] != 0) {
            p += 0x40;
        } else {
            goto found;
        }
    }
    p = 0;
    found:
    if (p == 0) {
        GSlogWrite(lbl_8027056C);
        return 0;
    }
    p[0] = 1;
    *(u32*)(p + 0x3c) = 0xFEFEFEFE;
    *(u16*)(p + 0x2) = 0;
    *(u32*)(p + 0x10) = 0;
    *(u32*)(p + 0x14) = 1;
    *(u32*)(p + 0x18) = 2;
    *(u32*)(p + 0x1c) = 3;
    *(u32*)(p + 0x28) = 0;
    *(u32*)(p + 0x20) = 0;
    *(u32*)(p + 0x24) = 0;
    *(u32*)(p + 0x38) = 0xFEFEFEFE;
    return p;
}
#endif

extern void HSD_MObjSetDefaultClass(void*);
extern u32 lbl_8047AB20;
extern u16 lbl_8047AB18;
extern u32 lbl_8047AB1C;
extern u8 lbl_80315490[];
#if 0
asm void GSmaterialInit(void) {
#include "src/game/gs_render_fn_800DF854.inc"
}
#else
void GSmaterialInit(u32 count) {
    u16 handle;
    u16 new_var;
    lbl_8047AB20 = count;
    handle = _toolentryAlloc__FUl(count * 0x40);
    new_var = handle;
    lbl_8047AB18 = new_var;
    if (new_var) {
        u32 off;
        u32 i;
        off = (u32)fn_800E27B0((u16)new_var);
        lbl_8047AB1C = off;
        for (off = 0, i = 0; i < lbl_8047AB20; i++, off += 0x40) {
            *(u8*)(lbl_8047AB1C + off) = 0;
        }
        HSD_MObjSetDefaultClass(lbl_80315490);
    }
}
#endif

extern void hsdInitClassInfo(void*, void*, void*, void*, u32, u32);
extern u8 lbl_8036CB30[];
#if 0
asm void _GSmaterialObjInit_800EF33C(void) {
#include "src/game/gs_render__GSmaterialObjInit_800EF33C.inc"
}
#else
void _GSmaterialObjInit_800EF33C(void) {
    hsdInitClassInfo(lbl_80315490, lbl_8036CB30, (void*)lbl_802705C0,
                     (void*)lbl_802705C0, 0x54, 0x24);
    *(void**)&lbl_80315490[0x40] = (void*)_matGSmatObjLoad;
    *(void**)&lbl_80315490[0x44] = (void*)_matGSmatObjMakeTExp;
}
#endif

extern void HSD_TExpGetType();
extern void fn_801B6DC0();
extern void HSD_TExpCnst();
extern void fn_801B707C();
extern void fn_801B6E74();
extern void fn_801B64EC();
extern void fn_801B6CD8();
extern void fn_801B5F08();
#if 0
asm void _matGSmatObjMakeTExp(void) {
#include "src/game/gs_render__matGSmatObjMakeTExp.inc"
}
#else
u32 _matGSmatObjMakeTExp(void* obj, void* a, void* b, void* c, void* d) {
    u32 result;
    u8* ext;
    u32 (*base)(void*, void*, void*, void*, void*);

    result = 0;
    base = *(u32 (**)(void*, void*, void*, void*, void*))(lbl_8036CB30 + 0x44);
    if (base != 0) {
        result = base(obj, a, b, c, d);
    }

    ext = *(u8**)((u8*)obj + 0x20);
    if (ext != 0) {
        if (*(u16*)(ext + 0x2) & 2) {
            fn_801B6DC0(result, *(u32*)(ext + 0x10), *(u32*)(ext + 0x14),
                        *(u32*)(ext + 0x18), *(u32*)(ext + 0x1c));
        }
        if (*(u16*)(ext + 0x2) & 1) {
            HSD_TExpCnst(ext + 0x0c, 1, 0, d);
            HSD_TExpCnst(ext + 0x0f, 6, 0, d);
        }
    }

    return result;
}
#endif

extern void HSD_ImageDescRemove(void);
extern void HSD_TObjLoadDesc(void);
extern void HSD_MObjGetTObj(void);
extern void HSD_MObjAddTObjNext(void);
extern void fn_801A6DA0(void);
extern u8 lbl_803154E4[];
extern f32 lbl_8047CAC8;
#if 0
asm void _matGSmatEnableEnvMapExt(void) {
#include "src/game/gs_render__matGSmatEnableEnvMapExt.inc"
}
#else
void _matGSmatEnableEnvMapExt(u8* obj) {
    u32 image;

    if (obj == 0) {
        return;
    }

    image = *(u32*)(obj + 0x28);
    if (image == 0) {
        GSlogWrite(lbl_80270610);
        return;
    }

    *(u32*)(*(u8**)(obj + 0x8) + 0x20) = (u32)obj;
    *(u32*)(obj + 0x20) = image;
    *(u32*)(obj + 0x24) = 0;
    GStextureUnlockImage((void*)image);
    HSD_MObjCompileTev(*(void**)(obj + 0x8));
}
#endif

#if 0
asm s32 _matGSmatObjLoad(u8* obj) {
#include "src/game/gs_render_fn_800DFE98.inc"
}
#else
s32 _matGSmatObjLoad(u8* obj) {
    u8* table;
    s32 r;
    u32 arg;

    table = lbl_8036CB30;
    r = ((s32(*)(u8*, u32))*(u32*)(table + 0x40))(obj, arg);
    switch (r) {
        case 0:
            *(u32*)(obj + 0x20) = 0;
            return 0;
    }
    return r;
}
#endif
