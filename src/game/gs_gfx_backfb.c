/**
 * @file gs_gfx_backfb.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: game/pxdvs/GSAPI/GSgfxM/backfb.cpp
 * Address range: 0x800DC298 - 0x800DC560 (3 functions)
 *
 * 3 anchors monotonic (0x802A6E90 < 0x802A6F88 < 0x802A72D8) covering the whole XD TU; GSgfxEndBackFBCapture size 0xF8 EXACT; internal order End->Begin->Init identical in both games; XD TU end 0x802A72F8 = Init end, ours likewise ends the TU at 0x800DC560.
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

extern u32 GStextureUnlockImage(void*);
extern u8 lbl_80400EE0[];
extern u8 lbl_8047AAE0;
typedef struct GSbackFBCapture {
    u8 active;
    u8 _pad[3];
    void* texture;
    void* callback;
    void* userData;
    u32 param;
} GSbackFBCapture;

static inline GSbackFBCapture* GSbackFBFindCapture(void* texture)
{
    GSbackFBCapture* capture;
    u32 i;

    capture = (GSbackFBCapture*)lbl_80400EE0;
    for (i = 0; i < 4; i++, capture++) {
        if (capture->active == 1 && capture->texture == texture) {
            return capture;
        }
    }
    return NULL;
}

u32 GSgfxEndBackFBCapture(void* texture) {
    GSbackFBCapture* capture;
    u32 i;

    capture = GSbackFBFindCapture(texture);
    if (capture == NULL) {
        return 0;
    }

    GStextureUnlockImage(capture->texture);
    capture->active = 0;
    lbl_8047AAE0 = 0;
    for (i = 0; i < 4; i++) {
        if (((GSbackFBCapture*)lbl_80400EE0)[i].active == 1) {
            lbl_8047AAE0 = 1;
            break;
        }
    }
    return 1;
}

extern void GStextureGetFormat(void);
extern void GStextureSetWrap(void);
extern void GStextureSetFilter(void);
extern void* GStextureLockImage(void*, u32);
extern u8 lbl_8047AAE0;
#if 0
asm void GSgfxBeginBackFBCapture(void) {
#include "src/game/gs_render_GSgfxBeginBackFBCapture.inc"
}
#else
void GSgfxBeginBackFBCapture(void) {
    if (lbl_8047AAE0 == 0) {
        GStextureGetFormat();
        GStextureLockImage(lbl_80400EE0, 0);
        GStextureSetWrap();
        GStextureSetFilter();
        lbl_8047AAE0 = 1;
    }
}
#endif


extern u8 lbl_8047AAE0;
#if 0
asm void GSgfxBackFBInit__Fv(void) {
#include "src/game/gs_render_GSgfxBackFBInit__Fv.inc"
}
#else
void GSgfxBackFBInit__Fv(void) {
    lbl_80400EE0[0] = 0;
    lbl_8047AAE0 = 0;
    lbl_80400EE0[0x14] = 0;
    lbl_80400EE0[0x28] = 0;
    lbl_80400EE0[0x3c] = 0;
}
#endif
