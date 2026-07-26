/**
 * @file gs_gfx_dl.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: game/pxdvs/GSAPI/GSgfxM/dl.cpp
 * Address range: 0x800DA578 - 0x800DB098 (7 functions)
 *
 * 3 anchors, monotonic vs XD (0x802B10F4 < 0x802B1590 < 0x802B1720), with EXACT size matches 0x178/0x190/0x440. Trailing fn_800DACC0(0x50~DLEnd/DLFree)/fn_800DAD10(0xA4)/fn_800DADB4(0x1AC~_dlCalculateStride 0x210)/fn_800DAF60(0x138~DLCreate 0x184) fit XD dl.cpp inventory; our total 0xB20 vs XD 0xC7C.
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

typedef struct GSgfxVtxDescList {
    s32 attr;
    u32 attr_type;
    u32 comp_cnt;
    u32 comp_type;
    u8 frac;
    u16 stride;
    void* vertex;
} GSgfxVtxDescList;

typedef struct GSgfxParseCallbackList {
    void (*begin)(s32 prim, u16 count, u32 attrs, void* user);
    void (*beginVertex)(void* user);
    void (*vertexAttr)(u32 attr, void* data, GSgfxVtxDescList* desc, void* user);
    void (*endVertex)(void* user);
    void (*end)(void* user);
} GSgfxParseCallbackList;

u8* _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(s32 prim, GSgfxVtxDescList* desc, u8* ptr, u16 count, GSgfxParseCallbackList* callbacks, void* user);
u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user);

#if 0
asm void fn_800DA578(void) {
#include "src/game/gs_render_fn_800DA578.inc"
}
#else
void GSgfxParseDisplayList(GSgfxVtxDescList* desc, u8* dl, u16 size, GSgfxParseCallbackList* callbacks, void* user) {
    u8* ptr;
    u8* end;
    s32 prim;
    u16 count;
    s32 cmd;

    ptr = dl;
    end = dl + size;
    while (ptr < end) {
        cmd = *ptr++ & 0xf8;
        switch (cmd) {
        case 0:
            break;
        case 0x61:
            ptr += 4;
            break;
        case 0x80:
        case 0x90:
        case 0x98:
        case 0xa0:
        case 0xa8:
        case 0xb0:
        case 0xb8:
            count = *(u16*)ptr;
            ptr += 2;
            switch (cmd) {
            case 0xb8:
                prim = 0;
                break;
            case 0xa8:
                prim = 1;
                break;
            case 0xb0:
                prim = 2;
                break;
            case 0x90:
                prim = 3;
                break;
            case 0x98:
                prim = 4;
                break;
            case 0xa0:
                prim = 5;
                break;
            case 0x80:
                prim = 6;
                break;
            }
            ptr = _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(prim, desc, ptr, count, callbacks, user);
            break;
        default:
            return;
        }
    }
}
#endif

extern void jumptable_803152B8();
#if 0
asm void _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(void) {
#include "src/game/gs_render_fn_800DA6F0.inc"
}
#else
u8* _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(s32 prim, GSgfxVtxDescList* desc, u8* ptr, u16 count, GSgfxParseCallbackList* callbacks, void* user) {
    GSgfxVtxDescList* it;
    u32 mask;
    s32 attr;

    mask = 0;
    it = desc;
    while ((attr = it->attr) != 0xff) {
        switch (attr) {
        case 0:
            mask |= 1;
            break;
        case 1:
            mask |= 0x40;
            break;
        case 9:
            mask |= 2;
            break;
        case 10:
            mask |= 4;
            break;
        case 11:
            mask |= 8;
            break;
        case 12:
            mask |= 0x10;
            break;
        case 13:
            mask |= 0x20;
            break;
        case 14:
            mask |= 0x80;
            break;
        case 15:
            mask |= 0x100;
            break;
        case 16:
            mask |= 0x200;
            break;
        case 17:
            mask |= 0x400;
            break;
        case 18:
            mask |= 0x800;
            break;
        case 19:
            mask |= 0x1000;
            break;
        case 20:
            mask |= 0x2000;
            break;
        case 25:
            mask |= 0x4000;
            break;
        }
        it++;
    }

    if (callbacks->begin != 0) {
        callbacks->begin(prim, count, mask, user);
    }

    while (count-- != 0) {
        if (callbacks->beginVertex != 0) {
            callbacks->beginVertex(user);
        }
        it = desc;
        while (it->attr != 0xff) {
            ptr = _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(it, ptr, callbacks, user);
            it++;
        }
        if (callbacks->endVertex != 0) {
            callbacks->endVertex(user);
        }
    }

    if (callbacks->end != 0) {
        callbacks->end(user);
    }
    return ptr;
}
#endif

extern void jumptable_80315340();
extern void jumptable_80315320();
#if 0
asm void _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(void) {
#include "src/game/gs_render_fn_800DA880.inc"
}
#else
u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user) {
    u8* data;
    u32 attr_flag;
    u32 comp_flag;
    u32 type_flag;
    s32 comp_count;
    u32 index;

    data = ptr;
    switch (desc->attr) {
    case 0:
        attr_flag = 1;
        comp_flag = 0x10000;
        type_flag = 0x10000000;
        ptr += 1;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        attr_flag = 0x40;
        comp_flag = 0x10000;
        type_flag = 0x10000000;
        ptr += 1;
        break;
    case 11:
    case 12:
        if (desc->attr == 11) {
            attr_flag = 0x10;
        } else {
            attr_flag = 0x20;
        }
        type_flag = 0x10000000;
        switch (desc->comp_type) {
        case 0:
            comp_flag = 0x200000;
            break;
        case 1:
            comp_flag = 0x400000;
            break;
        case 2:
            comp_flag = 0x800000;
            break;
        case 3:
            comp_flag = 0x1000000;
            break;
        case 4:
            comp_flag = 0x2000000;
            break;
        case 5:
            comp_flag = 0x4000000;
            break;
        }
        goto calc_direct_or_indexed;
    default:
        switch (desc->comp_type) {
        case 0:
            comp_flag = 0x10000;
            break;
        case 1:
            comp_flag = 0x20000;
            break;
        case 2:
            comp_flag = 0x40000;
            break;
        case 3:
            comp_flag = 0x80000;
            break;
        case 4:
            comp_flag = 0x100000;
            break;
        }
        switch (desc->attr) {
        case 9:
            attr_flag = 2;
            if (desc->comp_cnt == 0) {
                type_flag = 0x20000000;
            } else {
                type_flag = 0x40000000;
            }
            break;
        case 10:
            attr_flag = 4;
            if (desc->comp_cnt == 0) {
                type_flag = 0x40000000;
            } else {
                type_flag = 0x10000000;
            }
            break;
        case 25:
            attr_flag = 8;
            if (desc->comp_cnt == 1) {
                type_flag = 0x40000000;
            } else {
                type_flag = 0x10000000;
            }
            break;
        case 13:
            attr_flag = 0x80;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 14:
            attr_flag = 0x100;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 15:
            attr_flag = 0x200;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 16:
            attr_flag = 0x400;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 17:
            attr_flag = 0x800;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 18:
            attr_flag = 0x1000;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 19:
            attr_flag = 0x2000;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        case 20:
            attr_flag = 0x4000;
            if (desc->comp_cnt == 0) {
                type_flag = 0x10000000;
            } else {
                type_flag = 0x20000000;
            }
            break;
        }
calc_direct_or_indexed:
        if (desc->attr_type == 1) {
            switch (comp_flag) {
            case 0x10000:
            case 0x20000:
                comp_count = 1;
                break;
            case 0x40000:
            case 0x80000:
            case 0x200000:
            case 0x1000000:
                comp_count = 2;
                break;
            case 0x400000:
            case 0x2000000:
                comp_count = 3;
                break;
            case 0x100000:
            case 0x800000:
            case 0x4000000:
                comp_count = 4;
                break;
            }
            switch (type_flag) {
            case 0x20000000:
                comp_count <<= 1;
                break;
            case 0x40000000:
                comp_count *= 3;
                break;
            case 0x80000000:
                comp_count <<= 2;
                break;
            }
            ptr += comp_count;
        } else {
            if (desc->attr_type == 2) {
                index = *ptr++;
            } else {
                index = *(u16*)ptr;
                ptr += 2;
            }
            data = (u8*)desc->vertex + ((u16)index * desc->stride);
        }
        break;
    }

    if (callbacks->vertexAttr != 0) {
        callbacks->vertexAttr(type_flag | attr_flag | comp_flag, data, desc, user);
    }
    return ptr;
}
#endif

extern void fn_800E24B0(u16);
extern void fn_800E209C(u16);
#if 0
asm void GSgfxDLFree(void) {
#include "src/game/gs_render_fn_800DACC0.inc"
}
#else
void GSgfxDLFree(u8* obj) {
    if (*(u8**)(lbl_8047AA80 + 0x480) != obj) {
        fn_800E24B0(*(u16*)(obj + 0x2));
        fn_800E209C(*(u16*)(obj + 0x2));
        *(u8*)(obj + 0x0) = 0;
    }
}
#endif

extern void GXCallDisplayList(u32, u32);
#if 0
asm void GSgfxDLDraw(void) {
#include "src/game/gs_render_fn_800DAD10.inc"
}
#else
void GSgfxDLDraw(u32 obj) {
    u32 state = lbl_8047AA80;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x2a, 1, obj);
    } else if (*(u8*)(state + 0x1b) == *(u8*)(state + 0x1a) &&
               (*(u32*)(state + 0x4) & *(u32*)(state + 0x8)) &&
               *(u32*)(obj + 0x8) != 0) {
        fn_800D7A70(*(u32*)(obj + 0xc));
        fn_800D892C(*(u32*)(obj + 0xc));
        GXCallDisplayList(*(u32*)(obj + 0x4), *(u32*)(obj + 0x8));
        fn_800D6A5C(*(u32*)(obj + 0x10), *(u32*)(obj + 0x14));
    }
}
#endif

extern void fn_800E2AF8(u16);
#if 0
asm void GSgfxDLEnd(void) {
#include "src/game/gs_render_fn_800DADB4.inc"
}
#else
u32 GSgfxDLEnd(void) {
    u32 state; u32 obj; u32 wptr; u32 aligned; u32 limit; u32 size; u32 rem; u32 i;
    state = lbl_8047AA80;
    if (!*(u8*)(state + 0x47e)) { return 0; }
    obj = *(u32*)(state + 0x480);
    wptr = *(u32*)(state + 0x484);
    aligned = (wptr + 0x1f) & ~0x1f;
    limit = *(u32*)(obj + 0x4) + *(u32*)(obj + 0x8);
    if (aligned > limit || *(u8*)(obj + 0x1) != 0) {
        fn_800E24B0(*(u16*)(obj + 0x2));
        fn_800E209C(*(u16*)(obj + 0x2));
        return 0;
    }
    size = aligned - wptr;
    if (size != 0) {
        i = size >> 3;
        if (i != 0) {
            do {
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
            } while (--i);
        }
        rem = size & 7;
        if (rem != 0) {
            do {
                { u32 s = lbl_8047AA80; u32 p = *(u32*)(s + 0x484); *(u32*)(s + 0x484) = p + 1; *(u8*)p = 0; }
            } while (--rem);
        }
    }
    *(u32*)(obj + 0x8) = aligned - *(u32*)(obj + 0x4);
    fn_800E2AF8(*(u16*)(obj + 0x2));
    DCFlushRange((void*)*(u32*)(obj + 0x4), *(u32*)(obj + 0x8));
    { u32 s = lbl_8047AA80; *(u8*)(s + 0x47e) = 0; }
    { u32 s = lbl_8047AA80; *(u32*)(s + 0x480) = 0; }
    { u32 s = lbl_8047AA80; *(u32*)(s + 0x484) = 0; }
    return obj;
}
#endif

extern u16 fn_800E2C04(u32, u32);
extern u32 lbl_8047AAD8;
extern u32 lbl_8047AAD4;
typedef struct GSgfxDLCapture {
    u8 active;
    u8 overflow;
    u16 handle;
    u32 data;
    u32 size;
    u32 pipeline;
    u32 totalVerts;
    u32 totalPrims;
} GSgfxDLCapture;

typedef struct GSgfxCaptureState {
    u8 pad_000[0x47e];
    u8 captureActive;
    u8 pad_47f;
    GSgfxDLCapture* currentCapture;
    u32 writePtr;
    u8 pad_488[0x17];
    u8 pending;
} GSgfxCaptureState;

static inline GSgfxDLCapture* GSgfxFindFreeDLCapture(GSgfxDLCapture* capture) {
    u32 i;

    for (i = 0; i < lbl_8047AAD8; i++) {
        if (capture->active == 0) {
            return capture;
        }
        capture++;
    }
    return 0;
}

/* GSgfxDLBegin: inlined free-capture helper preserves target mtctr/bdnz
 * slot scan and the un-fused found/not-found branch shape. */
#if 0
asm void GSgfxDLBegin(void) {
#include "src/game/gs_render_fn_800DAF60.inc"
}
#else
u32 GSgfxDLBegin(u32 a, u32 b) {
    u32 r29; u32 r30;
    GSgfxDLCapture* r31;
    r29 = a; r30 = b;
    {
        GSgfxCaptureState* s = (GSgfxCaptureState*)lbl_8047AA80;
        if (s->captureActive == 1) { return 0; }
        if (s->pending == 1) { return 0; }
    }
    r31 = GSgfxFindFreeDLCapture((GSgfxDLCapture*)lbl_8047AAD4);
    if (r31 == 0) { return 0; }
    r31->active = 1;
    r31->overflow = 0;
    r31->totalVerts = 0;
    r31->totalPrims = 0;
    r31->handle = fn_800E2C04(r30, 0x20);
    if (r31->handle == 0) { return 0; }
    r31->size = r30;
    r31->data = (u32)fn_800E27B0(r31->handle);
    if (r31->data == 0) {
        fn_800E209C(r31->handle);
        return 0;
    }
    r31->pipeline = r29;
    {
        GSgfxCaptureState* s = (GSgfxCaptureState*)lbl_8047AA80;
        s->captureActive = 1;
    }
    {
        GSgfxCaptureState* s = (GSgfxCaptureState*)lbl_8047AA80;
        s->currentCapture = r31;
    }
    {
        GSgfxCaptureState* s = (GSgfxCaptureState*)lbl_8047AA80;
        s->writePtr = r31->data;
    }
    return 1;
}
#endif
