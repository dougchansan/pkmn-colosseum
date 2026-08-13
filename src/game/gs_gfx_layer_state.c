/**
 * @file gs_gfx_layer_state.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: game/pxdvs/GSAPI/GSgfxM/layerState.cpp (structural)
 * Address range: 0x800D6B00 - 0x800D9AF0 (62 functions)
 *
 * No anchors but strong structural mirror of XD layerState.cpp (0x3D8C): our 0x730 lead fn ~ SetupGPUState (XD 0x1510 after inlining), a 31-fn tiny accessor farm (0x14-0x34) = dirty-state helpers, then ~20 setters sized 0x78-0x1D8 mirroring the GSgfx_GCSet{Fog,AlphaCompare,BlendMode,TevSwap,TevKColor,Ind*,Tev*,MatColor,AmbColor,ChanCtrl,Scissor,Projection} farm, closing with renderState trio 0x910/0x400/0x4B4 mirroring XD SetupRenderState/renderState_NL_VC_T/NL_NVC_T/NL_NVC_NT...
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
extern void fn_800D7940(u8*, u16);
extern void fn_800D7A70(u32);
extern void fn_800D923C(void);
extern void fn_800D963C(u32, s32);
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
extern u8 lbl_8047AA91;
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
extern u32 lbl_80478AE0[];
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

typedef struct GSgfxStatePartial {
    /* 0x000 */ s32 recordMode;
    /* 0x004 */ u8 pad_004[0x4A0];
    /* 0x4A4 */ u8 fifoAttrBase;
    /* 0x4A5 */ u8 pad_4A5[0x13];
    /* 0x4B8 */ f32 fifoVecX;
    /* 0x4BC */ f32 fifoVecY;
    /* 0x4C0 */ f32 fifoVecZ;
} GSgfxStatePartial;


/* ==================================================================
 * Matrix accessor functions (0x800D7230 - 0x800D75D0)
 *
 * A block of ~40 tiny functions (0x14-0x24 bytes each) that get/set
 * individual matrix elements, indices, and stack pointers.
 *
 * These follow two patterns:
 *   Pattern A (getter, 0x1C bytes):
 *     lwz r3, offset(r13/r0)   ; load from sda
 *     lwz r3, field(r3)        ; read struct field
 *     blr
 *
 *   Pattern B (setter, 0x24 bytes):
 *     lwz r4, offset(r13/r0)   ; load state ptr
 *     stw r3, field(r4)        ; store value
 *     blr
 * ================================================================== */

#if defined(GS_GFX_LAYER_800D7230_800D76A8)
void fn_800D7230(void) {
    GSgfxStatePartial* state = (GSgfxStatePartial*)lbl_8047AA80;
    u8 value = state->fifoAttrBase;

    *(volatile u8*)0xCC008000 = value + (value << 1);
}
#endif

/* ==================================================================
 * fn_800D892C -- GSgfx_ConfigurePipeline
 *
 * Configure the full GX rendering pipeline. At 2320 bytes, this is
 * a major setup function. Called once per render mode change.
 * ================================================================== */
#if defined(GS_GFX_LAYER_800D892C_800D9AF0) || defined(GS_GFX_LAYER_800D892C_800D923C)
void fn_800D892C(u32 objArg) {
    u8* obj;
    u8* state;
    u8* buffer;
    u32 flags;
    s32 mode;
    u32 mask;
    u32 bit;
    u8 stage;
    s32 i;
    s32 last;
    s32 count;
    s32 enabled;
    u8* src;
    u8* dst;

    obj = (u8*)objArg;
    state = (u8*)lbl_8047AA80;
    flags = *(u32*)(state + 0x10);
    mode = 0;

    if ((flags & 0x40000000) == 0) {
        mode = 1;
        if (obj[0x94] != 0) {
            mode = 2;
        } else if ((flags & 5) == 0) {
            mode = 0;
        }

        state[0x60] = mode;
        *(u32*)(state + 0x414) |= 1;

        if (mode > 0) {
            mask = 0;
            if (flags & 4) {
                bit = 0x10;
                for (i = 0; i < 0x7F1; i++, bit++) {
                    if (*(u32*)(state + 0x10) & bit) {
                        mask |= 1 << (bit - 0x10);
                    }
                }

                if (flags & 1) {
                    dst = state + 0x61;
                    for (stage = 0; stage < 2; stage++, dst += 6) {
                        dst[0] = 1;
                        dst[1] = 0;
                        dst[2] = 1;
                        dst[3] = mask;
                        dst[4] = 2;
                        dst[5] = 2;
                    }
                } else {
                    dst = state + 0x61;
                    for (stage = 0; stage < 2; stage++, dst += 6) {
                        dst[0] = 1;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = mask;
                        dst[4] = 2;
                        dst[5] = 2;
                    }
                }
            } else {
                dst = state + 0x61;
                for (stage = 0; stage < 2; stage++, dst += 6) {
                    dst[0] = 0;
                    dst[1] = 1;
                    dst[2] = 1;
                    dst[3] = 0;
                    dst[4] = 0;
                    dst[5] = 2;
                }
            }
            *(u32*)(state + 0x414) |= 1;

            if (mode == 2) {
                dst = state + 0x67;
                for (stage = 0; stage < 2; stage++, dst += 6) {
                    dst[0] = 0;
                    dst[1] = 1;
                    dst[2] = 1;
                    dst[3] = 0;
                    dst[4] = 0;
                    dst[5] = 2;
                }
                *(u32*)(state + 0x414) |= 1;
            }
        }
    }

    state = (u8*)lbl_8047AA80;
    flags = *(u32*)(state + 0x10);
    if ((flags & 2) == 0) {
        state[0x79] = 0;
        *(u32*)(state + 0x414) |= 2;

        state[0x7A] = mode == 1 ? 1 : 2;
        *(u32*)(state + 0x414) |= 4;
        state[0x7B] = 0xFF;
        state[0x7C] = 0xFF;
        state[0x7D] = 4;
        *(u32*)(state + 0x414) |= 4;
        fn_800D963C(0, 4);

        if (mode != 1) {
            state = (u8*)lbl_8047AA80;
            state[0x7E] = 0xFF;
            state[0x7F] = 0xFF;
            state[0x80] = 5;
            *(u32*)(state + 0x414) |= 4;

            state[0xB0] = 0;
            state[0xB1] = 0;
            state[0xB2] = 0;
            state[0xB3] = 1;
            state[0xB4] = 0;
            *(u32*)(state + 0x414) |= 4;

            state[0x14F] = 15;
            state[0x150] = 10;
            state[0x151] = 12;
            state[0x152] = 0;
            *(u32*)(state + 0x414) |= 4;
        }

        state = (u8*)lbl_8047AA80;
        state[0x3AC] = 0;
        *(u32*)(state + 0x414) |= 4;
    } else if ((flags & 0xFFFFFFFE) != 0) {
        buffer = lbl_80400B28;
        if (buffer[0x34C] != 0) {
            buffer[0x34C]++;
            buffer[0x34C]--;
        }

        last = 0;
        if (obj[0xB0] == 1) {
            last = 0;
        }
        if (obj[0xCC] == 1) {
            last = 1;
        }
        if (obj[0xE8] == 1) {
            last = 2;
        }
        if (obj[0x104] == 1) {
            last = 3;
        }
        if (obj[0x120] == 1) {
            last = 4;
        }
        if (obj[0x13C] == 1) {
            last = 5;
        }
        if (obj[0x158] == 1) {
            last = 6;
        }
        if (obj[0x174] == 1) {
            last = 7;
        }

        state = (u8*)lbl_8047AA80;
        count = buffer[0x1A];
        mode = buffer[0x34C];
        state[0x79] = last + 1;
        *(u32*)(state + 0x414) |= 2;
        state[0x7A] = count;
        *(u32*)(state + 0x414) |= 4;
        state[0x3AC] = mode;
        *(u32*)(state + 0x414) |= 4;

        for (i = 0; i < count; i++) {
            enabled = buffer[0x1FC + i] != 0 && mode > 0;
            state[0x25C + i] = enabled;

            src = state + 0x42E + i * 4;
            dst = state + 0x7B + i * 3;
            dst[0] = ((u32*)lbl_80314404)[src[1]];
            dst[1] = ((u32*)lbl_803144F0)[src[2]];
            dst[2] = lbl_80478AE0[src[0]];
            *(u32*)(state + 0x414) |= 4;

            memcpy(state + 0xAB + i * 5, buffer + 0x4B + i * 5, 5);
            memcpy(state + 0xFB + i * 5, buffer + 0x9B + i * 5, 5);
            memcpy(state + 0x14B + i * 4, buffer + 0xEB + i * 4, 4);
            memcpy(state + 0x18B + i * 4, buffer + 0x12B + i * 4, 4);
            *(u32*)(state + 0x1CC + i * 4) = *(u32*)(buffer + 0x16C + i * 4);
            *(u32*)(state + 0x20C + i * 4) = *(u32*)(buffer + 0x1AC + i * 4);
            if (enabled) {
                memcpy(state + 0x26C + i * 0x14, buffer + 0x20C + i * 0x14, 0x14);
            }
        }

        memcpy(state + 0x24C, buffer + 0x1EC, 0x10);
        if (mode != 0) {
            memcpy(state + 0x3AD, buffer + 0x34D, mode * 4);
            memcpy(state + 0x3C0, buffer + 0x360, 0x54);
        }
    } else {
        last = 0;
        if (obj[0xB0] == 1) {
            last = 0;
        }
        if (obj[0xCC] == 1) {
            last = 1;
        }
        if (obj[0xE8] == 1) {
            last = 2;
        }
        if (obj[0x104] == 1) {
            last = 3;
        }
        if (obj[0x120] == 1) {
            last = 4;
        }
        if (obj[0x13C] == 1) {
            last = 5;
        }
        if (obj[0x158] == 1) {
            last = 6;
        }
        if (obj[0x174] == 1) {
            last = 7;
        }

        state = (u8*)lbl_8047AA80;
        state[0x79] = last + 1;
        *(u32*)(state + 0x414) |= 2;
        state[0x7A] = last + 1;
        *(u32*)(state + 0x414) |= 4;
        state[0x3AC] = 0;
        *(u32*)(state + 0x414) |= 4;
        state[0x3AC] = lbl_80400B28[0x34C];

        for (i = 0; i <= last; i++) {
            src = state + 0x42E + i * 4;
            dst = state + 0x7B + i * 3;
            dst[0] = ((u32*)lbl_80314404)[src[1]];
            dst[1] = ((u32*)lbl_803144F0)[src[2]];
            dst[2] = lbl_80478AE0[src[0]];
            *(u32*)(state + 0x414) |= 4;
            fn_800D963C(i, src[3]);
        }
    }

    fn_800D923C();
}
#endif

#if defined(GS_GFX_LAYER_800D6B00_800D7230)
extern u8 lbl_804007E8[];
#if 0
asm void fn_800D6B00(void) {
#include "src/game/gs_render_fn_800D6B00.inc"
}
#else
void fn_800D6B00(void) {
    u32 state;
    u32 obj;
    u32 i;
    void (*fn)(u32);
    u8* shadow;
    u8 captureActive;

    state = lbl_8047AA80;
    if (*(u8*)(state + 0x49f) == 0) {
        return;
    }

    shadow = lbl_804007E8;
    captureActive = *(u8*)(state + 0x47e);
    if ((captureActive == 0 && *(s32*)(state + 0x14) == 7) ||
        (captureActive == 1 && *(s32*)(state + 0x488) == 7)) {
        if (*(u8*)(state + 0x18) == 1) {
            memcpy(shadow + 0x98, (void*)(state + 0x4ac), 0x18);
            memcpy(shadow + 0x80, (void*)(state + 0x4e8), 0x18);
            memcpy(shadow + 0x00, (void*)(state + 0x520), 0x80);
            *(u8*)(state + 0x4ac) = *(u8*)(shadow + 0x148);
            *(u16*)(state + 0x4b0) = *(u16*)(shadow + 0x14c);
            *(f32*)(state + 0x4b8) = *(f32*)(shadow + 0x154);
            memcpy((void*)(state + 0x4e8), shadow + 0x130, 0x18);
            *(u8*)(state + 0x520) = *(u8*)(shadow + 0x0b0);
            *(u16*)(state + 0x522) = *(u16*)(shadow + 0x0b2);
            *(f32*)(state + 0x528) = *(f32*)(shadow + 0x0b8);
            *(u8*)(state + 0x530) = *(u8*)(shadow + 0x0c0);
            *(u16*)(state + 0x532) = *(u16*)(shadow + 0x0c2);
            *(f32*)(state + 0x538) = *(f32*)(shadow + 0x0c8);
            *(u8*)(state + 0x540) = *(u8*)(shadow + 0x0d0);
            *(u16*)(state + 0x542) = *(u16*)(shadow + 0x0d2);
            *(f32*)(state + 0x548) = *(f32*)(shadow + 0x0d8);
            *(u8*)(state + 0x550) = *(u8*)(shadow + 0x0e0);
            *(u16*)(state + 0x552) = *(u16*)(shadow + 0x0e2);
            *(f32*)(state + 0x558) = *(f32*)(shadow + 0x0e8);
            *(u8*)(state + 0x560) = *(u8*)(shadow + 0x0f0);
            *(u16*)(state + 0x562) = *(u16*)(shadow + 0x0f2);
            *(f32*)(state + 0x568) = *(f32*)(shadow + 0x0f8);
            *(u8*)(state + 0x570) = *(u8*)(shadow + 0x100);
            *(u16*)(state + 0x572) = *(u16*)(shadow + 0x102);
            *(f32*)(state + 0x578) = *(f32*)(shadow + 0x108);
            *(u8*)(state + 0x580) = *(u8*)(shadow + 0x110);
            *(u16*)(state + 0x582) = *(u16*)(shadow + 0x112);
            *(f32*)(state + 0x588) = *(f32*)(shadow + 0x118);
            *(u8*)(state + 0x590) = *(u8*)(shadow + 0x120);
            *(u16*)(state + 0x592) = *(u16*)(shadow + 0x122);
            *(f32*)(state + 0x598) = *(f32*)(shadow + 0x128);

            captureActive = *(u8*)(state + 0x47e);
            if (captureActive == 1) {
                fn_800DB098();
            } else if (*(u8*)(state + 0x1b) == *(u8*)(state + 0x1a) &&
                       (*(u32*)(state + 0x4) & *(u32*)(state + 0x8)) != 0) {
                obj = *(u32*)(state + 0x24);
                if (obj != 0) {
                    if (*(u8*)(obj + 0x8) != 0) {
                        fn = *(void (**)(u32))(state + 0x4a0);
                        fn(0);
                    }
                    fn = *(void (**)(u32))(state + 0x4a8);
                    fn(0);
                    if (*(u8*)(obj + 0x40) != 0) {
                        fn = *(void (**)(u32))(state + 0x4c4);
                        fn(0);
                    }
                    for (i = 4; i <= 5; i++) {
                        if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                            fn = *(void (**)(u32))(state + 0x4e0 + (i - 4) * 4);
                            fn(i - 4);
                        }
                    }
                    for (i = 6; i <= 13; i++) {
                        if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                            fn = *(void (**)(u32))(state + 0x500 + (i - 6) * 4);
                            fn(i - 6);
                        }
                    }
                }
            }

            memcpy((void*)(state + 0x4ac), shadow + 0x98, 0x18);
            memcpy((void*)(state + 0x4e8), shadow + 0x80, 0x18);
            memcpy((void*)(state + 0x520), shadow + 0x00, 0x80);

            captureActive = *(u8*)(state + 0x47e);
            if (captureActive == 1) {
                fn_800DB098();
            } else if (*(u8*)(state + 0x1b) == *(u8*)(state + 0x1a) &&
                       (*(u32*)(state + 0x4) & *(u32*)(state + 0x8)) != 0) {
                obj = *(u32*)(state + 0x24);
                if (obj != 0) {
                    if (*(u8*)(obj + 0x8) != 0) {
                        fn = *(void (**)(u32))(state + 0x4a0);
                        fn(0);
                    }
                    fn = *(void (**)(u32))(state + 0x4a8);
                    fn(0);
                    if (*(u8*)(obj + 0x40) != 0) {
                        fn = *(void (**)(u32))(state + 0x4c4);
                        fn(0);
                    }
                    for (i = 4; i <= 5; i++) {
                        if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                            fn = *(void (**)(u32))(state + 0x4e0 + (i - 4) * 4);
                            fn(i - 4);
                        }
                    }
                    for (i = 6; i <= 13; i++) {
                        if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                            fn = *(void (**)(u32))(state + 0x500 + (i - 6) * 4);
                            fn(i - 6);
                        }
                    }
                }
            }

            *(u8*)(state + 0x4ad) = *(u8*)(shadow + 0x149);
            *(u16*)(state + 0x4b2) = *(u16*)(shadow + 0x14e);
            *(f32*)(state + 0x4bc) = *(f32*)(shadow + 0x158);
            *(u8*)(state + 0x521) = *(u8*)(shadow + 0x0b1);
            *(u16*)(state + 0x524) = *(u16*)(shadow + 0x0b4);
            *(f32*)(state + 0x52c) = *(f32*)(shadow + 0x0bc);
            *(u8*)(state + 0x531) = *(u8*)(shadow + 0x0c1);
            *(u16*)(state + 0x534) = *(u16*)(shadow + 0x0c4);
            *(f32*)(state + 0x53c) = *(f32*)(shadow + 0x0cc);
            *(u8*)(state + 0x541) = *(u8*)(shadow + 0x0d1);
            *(u16*)(state + 0x544) = *(u16*)(shadow + 0x0d4);
            *(f32*)(state + 0x54c) = *(f32*)(shadow + 0x0dc);
            *(u8*)(state + 0x551) = *(u8*)(shadow + 0x0e1);
            *(u16*)(state + 0x554) = *(u16*)(shadow + 0x0e4);
            *(f32*)(state + 0x55c) = *(f32*)(shadow + 0x0ec);
            *(u8*)(state + 0x561) = *(u8*)(shadow + 0x0f1);
            *(u16*)(state + 0x564) = *(u16*)(shadow + 0x0f4);
            *(f32*)(state + 0x56c) = *(f32*)(shadow + 0x0fc);
            *(u8*)(state + 0x571) = *(u8*)(shadow + 0x101);
            *(u16*)(state + 0x574) = *(u16*)(shadow + 0x104);
            *(f32*)(state + 0x57c) = *(f32*)(shadow + 0x10c);
            *(u8*)(state + 0x581) = *(u8*)(shadow + 0x111);
            *(u16*)(state + 0x584) = *(u16*)(shadow + 0x114);
            *(f32*)(state + 0x58c) = *(f32*)(shadow + 0x11c);
            *(u8*)(state + 0x591) = *(u8*)(shadow + 0x121);
            *(u16*)(state + 0x594) = *(u16*)(shadow + 0x124);
            *(f32*)(state + 0x59c) = *(f32*)(shadow + 0x12c);
            *(u8*)(state + 0x18) = 0;
        } else {
            memcpy(shadow + 0x148, (void*)(state + 0x4ac), 0x18);
            memcpy(shadow + 0x130, (void*)(state + 0x4e8), 0x18);
            memcpy(shadow + 0x0b0, (void*)(state + 0x520), 0x80);
            *(u8*)(state + 0x18) = 1;

            captureActive = *(u8*)(state + 0x47e);
            if (captureActive == 1) {
                fn_800DB098();
            } else if (*(u8*)(state + 0x1b) == *(u8*)(state + 0x1a) &&
                       (*(u32*)(state + 0x4) & *(u32*)(state + 0x8)) != 0) {
                obj = *(u32*)(state + 0x24);
                if (obj != 0) {
                    if (*(u8*)(obj + 0x8) != 0) {
                        fn = *(void (**)(u32))(state + 0x4a0);
                        fn(0);
                    }
                    fn = *(void (**)(u32))(state + 0x4a8);
                    fn(0);
                    if (*(u8*)(obj + 0x40) != 0) {
                        fn = *(void (**)(u32))(state + 0x4c4);
                        fn(0);
                    }
                    for (i = 4; i <= 5; i++) {
                        if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                            fn = *(void (**)(u32))(state + 0x4e0 + (i - 4) * 4);
                            fn(i - 4);
                        }
                    }
                    for (i = 6; i <= 13; i++) {
                        if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                            fn = *(void (**)(u32))(state + 0x500 + (i - 6) * 4);
                            fn(i - 6);
                        }
                    }
                }
            }
        }
    } else if (captureActive == 1) {
        fn_800DB098();
    } else if (*(u8*)(state + 0x1b) == *(u8*)(state + 0x1a) &&
               (*(u32*)(state + 0x4) & *(u32*)(state + 0x8)) != 0) {
        obj = *(u32*)(state + 0x24);
        if (obj != 0) {
            if (*(u8*)(obj + 0x8) != 0) {
                fn = *(void (**)(u32))(state + 0x4a0);
                fn(0);
            }

            fn = *(void (**)(u32))(state + 0x4a8);
            fn(0);

            if (*(u8*)(obj + 0x40) != 0) {
                fn = *(void (**)(u32))(state + 0x4c4);
                fn(0);
            }

            for (i = 4; i <= 5; i++) {
                if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                    fn = *(void (**)(u32))(state + 0x4e0 + (i - 4) * 4);
                    fn(i - 4);
                }
            }

            for (i = 6; i <= 13; i++) {
                if (*(u8*)(obj + i * 0x1c + 0x8) != 0) {
                    fn = *(void (**)(u32))(state + 0x500 + (i - 6) * 4);
                    fn(i - 6);
                }
            }
        }
    }

    *(u8*)(state + 0x49f) = 0;
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7230_800D76A8)
extern u32 lbl_8047AA80;
#if 0
asm void fn_800D724C(void) {
#include "src/game/gs_render_fn_800D724C.inc"
}
#else
void fn_800D724C(u32 idx) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x520 + idx*16); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7268(void) {
#include "src/game/gs_render_fn_800D7268.inc"
}
#else
void fn_800D7268(u32 idx) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x522 + idx*16); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7284(void) {
#include "src/game/gs_render_fn_800D7284.inc"
}
#else
void fn_800D7284(u32 idx) { u8 v = *(u8*)(lbl_8047AA80 + 0x520 + idx*16); *(volatile u8*)0xCC008000 = v; *(volatile u8*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D72A4(void) {
#include "src/game/gs_render_fn_800D72A4.inc"
}
#else
void fn_800D72A4(u32 idx) { u8 v = *(u8*)(lbl_8047AA80 + 0x520 + idx*16); *(volatile u8*)0xCC008000 = v; *(volatile u8*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D72C4(void) {
#include "src/game/gs_render_fn_800D72C4.inc"
}
#else
void fn_800D72C4(u32 idx) { u16 v = *(u16*)(lbl_8047AA80 + 0x522 + idx*16); *(volatile u16*)0xCC008000 = v; *(volatile u16*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D72E4(void) {
#include "src/game/gs_render_fn_800D72E4.inc"
}
#else
void fn_800D72E4(u32 idx) { u16 v = *(u16*)(lbl_8047AA80 + 0x522 + idx*16); *(volatile u16*)0xCC008000 = v; *(volatile u16*)0xCC008000 = v; }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7304(void) {
#include "src/game/gs_render_fn_800D7304.inc"
}
#else
void fn_800D7304(u32 idx) {
    u32 base = lbl_8047AA80 + idx*16;
    f32 a = *(f32*)(base + 0x528);
    f32 b = *(f32*)(base + 0x52c);
    *(volatile f32*)0xCC008000 = a;
    *(volatile f32*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7328(void) {
#include "src/game/gs_render_fn_800D7328.inc"
}
#else
void fn_800D7328(u32 idx) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x4e8 + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7344(void) {
#include "src/game/gs_render_fn_800D7344.inc"
}
#else
void fn_800D7344(u32 idx) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4ec + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7360(void) {
#include "src/game/gs_render_fn_800D7360.inc"
}
#else
void fn_800D7360(u32 idx) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4ec + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D737C(void) {
#include "src/game/gs_render_fn_800D737C.inc"
}
#else
void fn_800D737C(u32 idx) { *(volatile u32*)0xCC008000 = *(u32*)(lbl_8047AA80 + 0x4f0 + idx*12); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7398(void) {
#include "src/game/gs_render_fn_800D7398.inc"
}
#else
void fn_800D7398(u32 idx) {
    u8 b;
    u8 c;
    u8 a;
    u32 base = lbl_8047AA80 + idx*12;
    c = *(u8*)(base + 0x4ea);
    b = *(u8*)(base + 0x4e9);
    a = *(u8*)(base + 0x4e8);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D73C4(void) {
#include "src/game/gs_render_fn_800D73C4.inc"
}
#else
void fn_800D73C4(u32 idx) {
    u8 b;
    u8 c;
    u8 d;
    u8 a;
    u32 base = lbl_8047AA80 + idx*12;
    d = *(u8*)(base + 0x4eb);
    c = *(u8*)(base + 0x4ea);
    b = *(u8*)(base + 0x4e9);
    a = *(u8*)(base + 0x4e8);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
    *(volatile u8*)0xCC008000 = d;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D73F8(void) {
#include "src/game/gs_render_fn_800D73F8.inc"
}
#else
void fn_800D73F8(void) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x4c8); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D740C(void) {
#include "src/game/gs_render_fn_800D740C.inc"
}
#else
void fn_800D740C(void) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4cc); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7420(void) {
#include "src/game/gs_render_fn_800D7420.inc"
}
#else
void fn_800D7420(void) {
    u8 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = base[0x4ca];
    b = base[0x4c9];
    a = base[0x4c8];
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7444(void) {
#include "src/game/gs_render_fn_800D7444.inc"
}
#else
void fn_800D7444(void) {
    u16 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u16*)(base + 0x4d0);
    b = *(u16*)(base + 0x4ce);
    a = *(u16*)(base + 0x4cc);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
    *(volatile u16*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7468(void) {
#include "src/game/gs_render_fn_800D7468.inc"
}
#else
void fn_800D7468(void) {
    u8 *base = (u8*)lbl_8047AA80;
    f32 b;
    f32 c;
    f32 a;
    c = *(f32*)(base + 0x4dc);
    b = *(f32*)(base + 0x4d8);
    a = *(f32*)(base + 0x4d4);
    *(volatile f32*)0xCC008000 = a;
    *(volatile f32*)0xCC008000 = b;
    *(volatile f32*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D748C(void) {
#include "src/game/gs_render_fn_800D748C.inc"
}
#else
void fn_800D748C(void) { *(volatile u8*)0xCC008000 = *(u8*)(lbl_8047AA80 + 0x4ac); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74A0(void) {
#include "src/game/gs_render_fn_800D74A0.inc"
}
#else
void fn_800D74A0(void) { *(volatile u16*)0xCC008000 = *(u16*)(lbl_8047AA80 + 0x4b0); }
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74B4(void) {
#include "src/game/gs_render_fn_800D74B4.inc"
}
#else
void fn_800D74B4(void) {
    u8 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74D0(void) {
#include "src/game/gs_render_fn_800D74D0.inc"
}
#else
void fn_800D74D0(void) {
    u8 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D74EC(void) {
#include "src/game/gs_render_fn_800D74EC.inc"
}
#else
void fn_800D74EC(void) {
    u16 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7508(void) {
#include "src/game/gs_render_fn_800D7508.inc"
}
#else
void fn_800D7508(void) {
    u16 a, b;
    u8 *base = (u8*)lbl_8047AA80;
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7524(void) {
#include "src/game/gs_render_fn_800D7524.inc"
}
#else
void fn_800D7524(void) {
    u8 *base = (u8*)lbl_8047AA80;
    f32 b = *(f32*)(base + 0x4bc);
    f32 a = *(f32*)(base + 0x4b8);
    *(volatile f32*)0xCC008000 = a;
    *(volatile f32*)0xCC008000 = b;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7540(void) {
#include "src/game/gs_render_fn_800D7540.inc"
}
#else
void fn_800D7540(void) {
    u8 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u8*)(base + 0x4ae);
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7564(void) {
#include "src/game/gs_render_fn_800D7564.inc"
}
#else
void fn_800D7564(void) {
    u8 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u8*)(base + 0x4ae);
    b = *(u8*)(base + 0x4ad);
    a = *(u8*)(base + 0x4ac);
    *(volatile u8*)0xCC008000 = a;
    *(volatile u8*)0xCC008000 = b;
    *(volatile u8*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D7588(void) {
#include "src/game/gs_render_fn_800D7588.inc"
}
#else
void fn_800D7588(void) {
    u16 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u16*)(base + 0x4b4);
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
    *(volatile u16*)0xCC008000 = c;
}
#endif

extern u32 lbl_8047AA80;
#if 0
asm void fn_800D75AC(void) {
#include "src/game/gs_render_fn_800D75AC.inc"
}
#else
void fn_800D75AC(void) {
    u16 a, b, c;
    u8 *base = (u8*)lbl_8047AA80;
    c = *(u16*)(base + 0x4b4);
    b = *(u16*)(base + 0x4b2);
    a = *(u16*)(base + 0x4b0);
    *(volatile u16*)0xCC008000 = a;
    *(volatile u16*)0xCC008000 = b;
    *(volatile u16*)0xCC008000 = c;
}
#endif

#if 0
asm void fn_800D75F4(void) {
#include "src/game/gs_render_fn_800D75F4.inc"
}
#else
void fn_800D75D0(void) {
    GSgfxStatePartial* state = (GSgfxStatePartial*)lbl_8047AA80;
    f32 x;
    f32 y;
    f32 z;

    z = state->fifoVecZ;
    y = state->fifoVecY;
    x = state->fifoVecX;

    *(volatile f32*)0xCC008000 = x;
    *(volatile f32*)0xCC008000 = y;
    *(volatile f32*)0xCC008000 = z;
}

void fn_800D75F4(u8* obj) {
    if (*(u32*)(lbl_8047AA80 + 0x24) == (u32)obj) *(u32*)(lbl_8047AA80 + 0x24) = 0;
    obj[0x008] = 0; obj[0x024] = 0; obj[0x040] = 0; obj[0x05c] = 0;
    obj[0x078] = 0; obj[0x094] = 0; obj[0x0b0] = 0; obj[0x0cc] = 0;
    obj[0x0e8] = 0; obj[0x104] = 0; obj[0x120] = 0; obj[0x13c] = 0;
    obj[0x158] = 0; obj[0x174] = 0; obj[0x000] = 0;
}
#endif

#if 0
asm void fn_800D7650(void) {
#include "src/game/gs_render_fn_800D7650.inc"
}
#else
void fn_800D7650(u8* obj) {
    if (*(u32*)(lbl_8047AA80 + 0x24) == (u32)obj) *(u32*)(lbl_8047AA80 + 0x24) = 0;
    obj[0x008] = 0; obj[0x024] = 0; obj[0x040] = 0; obj[0x05c] = 0;
    obj[0x078] = 0; obj[0x094] = 0; obj[0x0b0] = 0; obj[0x0cc] = 0;
    obj[0x0e8] = 0; obj[0x104] = 0; obj[0x120] = 0; obj[0x13c] = 0;
    obj[0x158] = 0; obj[0x174] = 0;
}
#endif
#endif

extern u8 lbl_80314370[];
extern u8 lbl_803143B4[];
extern u8 lbl_803143D8[];
extern u8 lbl_803143A8[];
#if defined(GS_GFX_LAYER_800D76A8_800D7820)
#if 0
asm void fn_800D76A8(void) {
#include "src/game/gs_render_fn_800D76A8.inc"
}
#else
void fn_800D76A8(u32 obj, u16 vertCount) {
    u32 state;
    u32 oldObj;

    state = lbl_8047AA80;
    if (*(u8*)(state + 0x47e) == 1) {
        oldObj = *(u32*)(state + 0x24);
        *(u32*)(state + 0x24) = obj;
        fn_800D7940((u8*)obj, vertCount);
        *(u32*)(state + 0x24) = oldObj;
        return;
    }

    if (*(s32*)state == 1) {
        fn_800D4F98(0x47, 2, obj, (u32)vertCount);
        return;
    }

    fn_800D7A70(obj);
    fn_800D892C(obj);
    fn_800D7940((u8*)obj, vertCount);
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7820_800D7894)
#if 0
asm void fn_800D7820(void) {
#include "src/game/gs_render_fn_800D7820.inc"
}
#else
void fn_800D7820(u32 val) {
    if (*(s32*)lbl_8047AA80 == 1) { fn_800D4F98(0x46, 1, val); }
    else { *(s32*)(lbl_8047AA80 + 0x24) = val; }
}
#endif

#if 0
asm void fn_800D7868(void) {
#include "src/game/gs_render_fn_800D7868.inc"
}
#else
void fn_800D7868(u8* arr, u32 idx, u32 a, u32 b, u32 c, u8 d, u32 e, u8 f) {
    u8* p = arr + idx * 0x1c;
    p[0x8] = 1;
    *(u32*)(p + 0xc) = a;
    *(u32*)(p + 0x10) = b;
    *(u32*)(p + 0x14) = c;
    p[0x18] = d;
    *(u32*)(p + 0x1c) = e;
    p[0x20] = f;
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7894_800D7940)
extern u32 lbl_8047AAB0;
extern u32 lbl_8047AAAC;
extern u8 lbl_803144D0[];
extern u32 lbl_8047AAB4;
#if 0
asm void fn_800D7894(void) {
#include "src/game/gs_render_fn_800D7894.inc"
}
#else
#pragma push
#pragma peephole off
u8* fn_800D7894(void) {
    u32 i;
    u32 cnt = lbl_8047AAB0;
    u8* p = (u8*)lbl_8047AAAC;
    for (i = 0; i < cnt; i++) {
        if (*p == 0) {
            *p = 1;
            *(u32*)(p + 4) = *(u32*)(lbl_803144D0 + lbl_8047AAB4 * 4);
            lbl_8047AAB4++;
            if (lbl_8047AAB4 >= 8) {
                lbl_8047AAB4 = 0;
            }
            p[0x8] = 0;
            p[0x24] = 0;
            p[0x40] = 0;
            p[0x5c] = 0;
            p[0x78] = 0;
            p[0x94] = 0;
            p[0xb0] = 0;
            p[0xcc] = 0;
            p[0xe8] = 0;
            p[0x104] = 0;
            p[0x120] = 0;
            p[0x13c] = 0;
            p[0x158] = 0;
            p[0x174] = 0;
            return p;
        }
        p += 0x190;
    }
    return 0;
}
#pragma pop
#endif

#endif

#if defined(GS_GFX_LAYER_800D7940_800D7A70)
#if 0
asm void fn_800D7940(void) {
#include "src/game/gs_render_fn_800D7940.inc"
}
#else
extern void fn_800D6028(u16);
extern void fn_800D5FA4(u8);
extern void fn_800D5DD0(u16);
extern void fn_800D5D6C(u8);
extern void fn_800D5AB0(u32, u16);
extern void fn_800D5A38(u32, u8);
extern void fn_800D579C(u32, u16);
extern void fn_800D5724(u32, u8);
extern void fn_800D6728(void);

void fn_800D7940(u8* obj, u16 vertCount) {
    u16 vertex;
    s32 i;
    u32 layer;
    u8* entry;

    fn_800D67BC(vertCount);
    for (vertex = 0; vertex < vertCount; vertex++) {
        if (*(s32*)(obj + 0x28) == 2) {
            fn_800D6028(vertex);
        } else {
            fn_800D5FA4(vertex);
        }

        if (*(u8*)(obj + 0x40) == 1) {
            if (*(s32*)(obj + 0x44) == 2) {
                fn_800D5DD0(vertex);
            } else {
                fn_800D5D6C(vertex);
            }
        }

        for (i = 4; i <= 5; i++) {
            entry = (u8*)(obj + i * 0x1c);
            if (entry[0x8] == 1) {
                layer = i - 4;
                if (*(s32*)(entry + 0xc) == 2) {
                    fn_800D5AB0(layer, vertex);
                } else {
                    fn_800D5A38(layer, vertex);
                }
            }
        }

        for (i = 6; i <= 13; i++) {
            entry = (u8*)(obj + i * 0x1c);
            if (entry[0x8] == 1) {
                layer = i - 6;
                if (*(s32*)(entry + 0xc) == 2) {
                    fn_800D579C(layer, vertex);
                } else {
                    fn_800D5724(layer, vertex);
                }
            }
        }
    }
    fn_800D6728();
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7A70_800D7D90)
#if 0
asm void fn_800D7A70(u32) {
#include "src/game/gs_render_fn_800D7A70.inc"
}
#else
void fn_800D7A70(u32 obj) {
    u32 i;
    u8* entry;
    u32 attr;

    for (i = 0; (s32)i < 14; i++) {
        entry = (u8*)(obj + i * 0x1c);
        if (entry[0x8] != 0 && (s32)i != 0) {
            attr = ((u32*)lbl_80314370)[i];
            fn_800B7D74(*(u32*)(obj + 0x4), attr,
                        ((u32*)lbl_803143B4)[*(u32*)(entry + 0x10)],
                        ((u32*)lbl_803143D8)[*(u32*)(entry + 0x14)],
                        entry[0x18]);
        }
    }

    fn_800B7D3C();

    for (i = 0; (s32)i < 14; i++) {
        entry = (u8*)(obj + i * 0x1c);
        if (entry[0x8] != 0) {
            fn_800B7874(((u32*)lbl_80314370)[i], ((u32*)lbl_803143A8)[*(u32*)(entry + 0xc)]);
            if (*(u32*)(entry + 0x1c) != 0) {
                fn_800B84E0(((u32*)lbl_80314370)[i], *(u32*)(entry + 0x1c), entry[0x20]);
            }
        }
    }

    *(u32*)(lbl_804001F0 + 0x14) += 1;
}
#endif

extern u32 lbl_8047AAB0;
extern u16 lbl_8047AAA8;
extern u32 lbl_8047AAAC;
extern u32 lbl_8047AAB4;
#if 0
asm void fn_800D7B80(void) {
#include "src/game/gs_render_fn_800D7B80.inc"
}
#else
void fn_800D7B80(u32 count) {
    u16 handle;
    u32 off;
    u32 i;
    lbl_8047AAB0 = count;
    off = _toolentryAlloc__FUl(count * 0x190);
    handle = off;
    lbl_8047AAA8 = handle;
    if (handle != 0) {
        lbl_8047AAAC = (u32)fn_800E27B0(handle);
        for (off = 0, i = 0; i < lbl_8047AAB0; i++, off += 0x190) {
            *(u8*)(lbl_8047AAAC + off) = 0;
        }
        lbl_8047AAB4 = 0;
    }
}
#endif

#if 0
asm void fn_800D7BF8(void) {
#include "src/game/gs_render_fn_800D7BF8.inc"
}
#else
u32 fn_800D7BF8(s32 mode) {
    if (!GScameraGetActiveCamera()) return 0;
    switch (mode) {
        case 0: return fn_800D1D00();
        case 1: return fn_800D1B3C();
        case 2: return GScameraGetProjMatrixPtr();
        default: return 0;
    }
}
#endif

extern u8 lbl_8047AAC8;
extern u8 lbl_80314610[];
extern u32 lbl_8047AAC0;
extern u8 lbl_80400948[];
#if 0
asm void fn_800D7C74(void) {
#include "src/game/gs_render_fn_800D7C74.inc"
}
#else
void fn_800D7C74(void) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x45, 0);
    } else if (lbl_8047AAC8) {
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

#if 0
asm void fn_800D7D10(void) {
#include "src/game/gs_render_fn_800D7D10.inc"
}
#else
void fn_800D7D10(u8 idx, void* src) {
    if (*(s32*)lbl_8047AA80 == 1) { fn_800D4F98(0x44, 2, (u32)idx, src); }
    else if (idx > 9) { GSlogWrite(lbl_80270440); }
    else { fn_800E0628(src, &lbl_80400948[idx * 0x30]); }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7D90_800D7E5C)
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7D90(void) {
#include "src/game/gs_render_fn_800D7D90.inc"
}
#else
void fn_800D7D90(u8 idx, void* src) {
    void* r30;
    u32 r31;
    r30 = src;
    if (*(s32*)lbl_8047AA80 == 1) {
        fn_800D4F98(0x43, 0x11, (u32)idx, r30);
    } else {
        r31 = (u32)idx;
        if (r31 > 9) {
            GSlogWrite(lbl_80270440);
        } else {
            GXLoadPosMtxImm((u32)r30, *(u32*)((u8*)lbl_80314610 + (u32)idx * 4));
            fn_800E0628((void*)(lbl_80400948 + r31 * 0x30), r30);
            if (r31 == 9) {
                lbl_8047AAC8 = 1;
            }
        }
    }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D7E5C_800D85D4)
extern u32 lbl_8047AAC0;
extern u32 lbl_8047AAC4;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7E5C(void) {
#include "src/game/gs_render_fn_800D7E5C.inc"
}
#else
void fn_800D7E5C(void) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x42, 0);
    } else if (lbl_8047AAC0 >= lbl_8047AAC4) {
        GSlogWrite(lbl_80270460);
    } else {
        lbl_8047AAC0 = lbl_8047AAC0 + 0x30;
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u32 lbl_8047AABC;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7F14(void) {
#include "src/game/gs_render_fn_800D7F14.inc"
}
#else
void fn_800D7F14(void* src) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x41, 0x10, src);
    } else if (lbl_8047AAC0 <= lbl_8047AABC) {
        GSlogWrite(lbl_80270480);
    } else {
        lbl_8047AAC0 = lbl_8047AAC0 - 0x30;
        fn_800E0290((void*)lbl_8047AAC0, (void*)(lbl_8047AAC0 + 0x30), src);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D7FE4(void) {
#include "src/game/gs_render_fn_800D7FE4.inc"
}
#else
void fn_800D7FE4(void* param) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x40, 0x10, param);
    } else {
        fn_800E0290((void*)lbl_8047AAC0, (void*)lbl_8047AAC0, param);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u32 lbl_8047AABC;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D8088(void) {
#include "src/game/gs_render_fn_800D8088.inc"
}
#else
void fn_800D8088(void* src) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x3f, 0x10, src);
    } else if (lbl_8047AAC0 <= lbl_8047AABC) {
        GSlogWrite(lbl_80270480);
    } else {
        lbl_8047AAC0 = lbl_8047AAC0 - 0x30;
        fn_800E0628((void*)lbl_8047AAC0, src);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D8154(void) {
#include "src/game/gs_render_fn_800D8154.inc"
}
#else
void fn_800D8154(f32 a, f32 b, f32 c) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x3e, 0xd, a, b, c);
    } else {
        fn_800E02C4((void*)lbl_8047AAC0);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D81EC(void) {
#include "src/game/gs_render_fn_800D81EC.inc"
}
#else
void fn_800D81EC(f32 a, f32 b, f32 c) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x3d, 0xd, a, b, c);
    } else {
        fn_800E03E8((void*)lbl_8047AAC0, a, b, c);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D8284(void) {
#include "src/game/gs_render_fn_800D8284.inc"
}
#else
void fn_800D8284(f32 a, f32 b, f32 c) {
    f32 y;
    f32 z;
    u32 matrixIndex;
    y = b;
    z = c;
    if (*(s32*)lbl_8047AA80 == 1) {
        fn_800D4F98(0x3c, 0xd, a, y, z);
    } else {
        fn_800E0370((void*)lbl_8047AAC0, a);
        fn_800E032C((void*)lbl_8047AAC0, y);
        fn_800E02E8((void*)lbl_8047AAC0, z);
        matrixIndex = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, matrixIndex);
        GXLoadNrmMtxImm(lbl_8047AAC0, matrixIndex);
        fn_800BD554(matrixIndex);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D834C(void) {
#include "src/game/gs_render_fn_800D834C.inc"
}
#else
void fn_800D834C(void) {
    u32 state = lbl_8047AA80;
    u32 mtx;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x3b, 0);
    } else {
        fn_800E064C((void*)lbl_8047AAC0);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u16 lbl_8047AAB8;
extern u32 lbl_8047AABC;
extern u32 lbl_8047AAC4;
extern u32 lbl_8047AAC0;
extern u8 lbl_8047AAC8;
#if 0
asm void fn_800D83E4(void) {
#include "src/game/gs_render_fn_800D83E4.inc"
}
#else
void fn_800D83E4(u32 count) {
    u16 handle;
    u32 mtx;
    u32 ptr;
    u32 val;
    handle = _toolentryAlloc__FUl(count * 0x30);
    val = (u16)handle;
    lbl_8047AAB8 = handle;
    if (val != 0) {
        ptr = (u32)fn_800E27B0((u16)val);
        lbl_8047AABC = ptr;
        ptr = ptr + (count - 1) * 0x30;
        lbl_8047AAC4 = ptr;
        lbl_8047AAC0 = ptr;
        fn_800E064C((void*)ptr);
        mtx = *(u32*)(lbl_80314610 + 0x24);
        GXLoadPosMtxImm(lbl_8047AAC0, mtx);
        GXLoadNrmMtxImm(lbl_8047AAC0, mtx);
        fn_800BD554(mtx);
        fn_800E0628((void*)(lbl_80400948 + 0x1b0), (void*)lbl_8047AAC0);
        lbl_8047AAC8 = 0;
    }
}
#endif

extern u8 lbl_80314404[];
extern u8 lbl_80314454[];
extern u8 lbl_803144A8[];
extern u8 lbl_80314424[];
#if 0
asm void fn_800D848C(void) {
#include "src/game/gs_render_fn_800D848C.inc"
}
#else
void fn_800D848C(u32 idx, s32 mode, u32 arg, void* mtx) {
    if (*(s32*)lbl_8047AA80 == 1) {
        fn_800D4F98(0x27, 0x13, idx, mode, arg, mtx);
    } else {
        if (mode == 0) {
            fn_800B857C(((u32*)lbl_80314404)[idx], 1, ((u32*)lbl_80314454)[arg], 0x3c, 0, 0x7d);
        } else {
            if (mode == 1) {
                GXLoadTexMtxImm(mtx, ((u32*)lbl_803144A8)[idx], 0);
            } else if (mode == 2) {
                GXLoadTexMtxImm(mtx, ((u32*)lbl_803144A8)[idx], 1);
            }
            fn_800B857C(((u32*)lbl_80314404)[idx], ((u32*)lbl_80314424)[mode], ((u32*)lbl_80314454)[arg], ((u32*)lbl_803144A8)[idx], 0, 0x7d);
        }
    }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D85D4_800D87AC)
extern u8 lbl_80314530[];
extern u32 lbl_8047CA40;
extern u32 lbl_8047CA48;
extern u8 lbl_80314510[];
extern u8 lbl_803144F0[];
#if 0
asm void fn_800D85D4(void) {
#include "src/game/gs_render_fn_800D85D4.inc"
}
#else
void fn_800D85D4(s32 slot, void* model) {
    u32 state;
    u8* obj;
    s32 mode;

    if (model == 0) {
        return;
    }

    state = lbl_8047AA80;
    obj = (u8*)model;
    if (*(s32*)state == 1) {
        fn_800D4F98(0x26, 2, slot, model);
        return;
    }
    if (*(void**)(state + 0x28 + slot * 4) == model) {
        return;
    }

    if (obj[0x7] != 0) {
        fn_800BAE34(obj + 0x54, ((u32*)lbl_80314530)[*(u32*)(obj + 0x10)],
                    ((u32*)lbl_80314530)[*(u32*)(obj + 0x14)]);
        switch (*(s32*)(obj + 0x20)) {
        case 0:
            mode = (*(s32*)(obj + 0x18) == 2) ? 1 : 0;
            break;
        case 1:
            mode = (*(s32*)(obj + 0x18) == 2) ? 3 : 2;
            break;
        case 2:
            mode = (*(s32*)(obj + 0x18) == 2) ? 5 : 4;
            break;
        }
        fn_800BACA0(obj + 0x54, mode, (*(u32*)(obj + 0x1c) == 2),
                    *(f32*)&lbl_8047CA40, (f32)(obj[0x5] - 1),
                    *(f32*)&lbl_8047CA40, 0, 0, 0);
        obj[0x7] = 0;
    }

    *(void**)(state + 0x28 + slot * 4) = model;
    if (*(u32*)(obj + 0x48) != 0) {
        fn_800BB098(obj + 0x74, ((u32*)lbl_80314510)[slot]);
    }
    GXLoadTexObj(obj + 0x54, ((u32*)lbl_803144F0)[slot]);
    *(u32*)(lbl_804001F0 + 0x24) += 1;
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D87AC_800D892C)
#if 0
asm void fn_800D87AC(void) {
#include "src/game/gs_render_fn_800D87AC.inc"
}
#else
void fn_800D87AC(u32 mask) {
    s32 i;
    u8* textureSlot;

    *(u32*)(lbl_8047AA80 + 0x414) |= mask;
    if (mask & 2) {
        for (i = 0; i < 8; i++) {
            ((u32*)lbl_8047AA80)[i + 10] = 0;
            fn_800B857C(((u32*)lbl_80314404)[i], 1, i + 4, 0x3c, 0, 0x7d);
        }
    }
    if (mask & 4) {
        for (i = 0; i < 0x10; i++) {
            textureSlot = (u8*)lbl_8047AA80 + i + 0x25c;
            if (*textureSlot) {
                *textureSlot = 0;
                fn_800BBC34(i);
            }
        }
        *(u8*)(lbl_8047AA80 + 0x3ac) = 0;
        fn_800BBC0C(0);
    }
}
#endif

#if 0
asm void fn_800D888C(void) {
#include "src/game/gs_render_fn_800D888C.inc"
}
#else
void fn_800D888C(u32 mask) {
    if (*(s32*)lbl_8047AA80 == 1) { fn_800D4F98(0x29, 1, mask); }
    else { *(s32*)(lbl_8047AA80 + 0x10) &= ~mask; }
}
#endif

#if 0
asm void fn_800D88DC(void) {
#include "src/game/gs_render_fn_800D88DC.inc"
}
#else
void fn_800D88DC(u32 mask) {
    if (*(s32*)lbl_8047AA80 == 1) { fn_800D4F98(0x28, 1, mask); }
    else { *(s32*)(lbl_8047AA80 + 0x10) |= mask; }
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D892C_800D9AF0) || defined(GS_GFX_LAYER_800D923C_800D963C)
#if 0
asm void fn_800D923C(void) {
#include "src/game/gs_render_fn_800D923C.inc"
}
#else
void fn_800D923C(void) {
    u32 state;
    u32 flags;
    u32 i;
    u8* p;
    u32 tmp;

    state = lbl_8047AA80;
    flags = *(u32*)(state + 0x414);

    if (flags & 1) {
        fn_800BA6B0(*(u8*)(state + 0x60));
        for (i = 0; i < *(u8*)(state + 0x60); i++) {
            p = (u8*)(state + 0x61 + i * 12);
            fn_800BA6F4(i, p[0], p[1], p[2], p[3], p[4], p[5]);
            fn_800BA6F4(i + 2, p[6], p[7], p[8], p[9], p[10], p[11]);
        }
        *(u32*)(lbl_804001F0 + 0x18) += 1;
    }

    if (flags & 2) {
        fn_800B884C(*(u8*)(state + 0x79));
        *(u32*)(lbl_804001F0 + 0x1c) += 1;
    }

    if (flags & 4) {
        fn_800BC8C8(*(u8*)(state + 0x7a));
        fn_800BBC0C(*(u8*)(state + 0x3ac));
        for (i = 0; i < *(u8*)(state + 0x7a); i++) {
            fn_800BC6F0(i, *(u8*)(state + 0x7b + i * 3),
                        *(u8*)(state + 0x7c + i * 3),
                        *(u8*)(state + 0x7d + i * 3));
            fn_800BC228(i, *(u8*)(state + 0xab + i * 5),
                        *(u8*)(state + 0xac + i * 5),
                        *(u8*)(state + 0xad + i * 5),
                        *(u8*)(state + 0xae + i * 5),
                        *(u8*)(state + 0xaf + i * 5));
            fn_800BC290(i, *(u8*)(state + 0xfb + i * 5),
                        *(u8*)(state + 0xfc + i * 5),
                        *(u8*)(state + 0xfd + i * 5),
                        *(u8*)(state + 0xfe + i * 5),
                        *(u8*)(state + 0xff + i * 5));
            fn_800BC1A0(i, *(u8*)(state + 0x14b + i * 4),
                        *(u8*)(state + 0x14c + i * 4),
                        *(u8*)(state + 0x14d + i * 4),
                        *(u8*)(state + 0x14e + i * 4));
            fn_800BC1E4(i, *(u8*)(state + 0x18b + i * 4),
                        *(u8*)(state + 0x18c + i * 4),
                        *(u8*)(state + 0x18d + i * 4),
                        *(u8*)(state + 0x18e + i * 4));
            fn_800BC454(i, *(u32*)(state + 0x1cc + i * 4));
            fn_800BC4C0(i, *(u32*)(state + 0x20c + i * 4));
            if (*(u8*)(state + 0x25c + i) != 0) {
                p = (u8*)(state + 0x26c + i * 0x14);
                switch (p[0]) {
                    case 0:
                        fn_800BB780(i, p[1], p[3], p[4], p[2], p[6], p[7], p[8],
                                    p[9], p[5]);
                        break;
                    case 1:
                        fn_800BBC7C(i, p[1], p[10], p[11], p[2]);
                        break;
                    case 2:
                        fn_800BBCE0(i, p[1], *(u16*)(p + 0xc), *(u16*)(p + 0xe),
                                    *(u16*)(p + 0x10), *(u16*)(p + 0x12), p[3], p[2]);
                        break;
                    case 3:
                        fn_800BBE8C(i, p[1], p[2]);
                        break;
                    case 4:
                        fn_800BBF98(i, p[1], p[2]);
                        break;
                    case 5:
                        fn_800BBFDC(i);
                        break;
                }
            } else {
                fn_800BBC34(i);
            }
        }
        for (i = 0; i < 4; i++) {
            tmp = *(u32*)(state + 0x24c + i * 4);
            fn_800BC3E0(i, &tmp);
        }
        fn_800BC580(0, 0, 1, 2, 3);
        *(u32*)(lbl_804001F0 + 0x20) += 1;
    }

    *(u32*)(state + 0x414) = 0;
}
#endif
#endif

#if defined(GS_GFX_LAYER_800D892C_800D9AF0) || defined(GS_GFX_LAYER_800D963C_800D9AF0)
#if 0
asm void fn_800D963C(void) {
#include "src/game/gs_render_fn_800D963C.inc"
}
#else
void fn_800D963C(u32 idx, s32 mode) {
    switch (mode) {
    case 2: {
        u8* ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xAB;
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        ptr[3] = 1;
        ptr[4] = 0;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x14B;
        ptr[0] = 15;
        ptr[1] = 15;
        ptr[2] = 15;
        ptr[3] = 8;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xFB;
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        ptr[3] = 1;
        ptr[4] = 0;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x18B;
        ptr[0] = 7;
        ptr[1] = 7;
        ptr[2] = 7;
        ptr[3] = 4;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;
        break;
    }
    case 0: {
            u8* ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xAB;
            ptr[0] = 0;
            ptr[1] = 0;
            ptr[2] = 0;
            ptr[3] = 1;
            ptr[4] = 0;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

            ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x14B;
            ptr[0] = 15;
            ptr[1] = 10;
            ptr[2] = 8;
            ptr[3] = 15;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

            ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xFB;
            ptr[0] = 0;
            ptr[1] = 0;
            ptr[2] = 0;
            ptr[3] = 1;
            ptr[4] = 0;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

            ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x18B;
            ptr[0] = 7;
            ptr[1] = 4;
            ptr[2] = 5;
            ptr[3] = 7;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;
        break;
    }
    case 1: {
            u8* ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xAB;
            ptr[0] = 0;
            ptr[1] = 0;
            ptr[2] = 0;
            ptr[3] = 1;
            ptr[4] = 0;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

            ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x14B;
            ptr[0] = 10;
            ptr[1] = 8;
            ptr[2] = 9;
            ptr[3] = 15;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

            ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xFB;
            ptr[0] = 0;
            ptr[1] = 0;
            ptr[2] = 0;
            ptr[3] = 1;
            ptr[4] = 0;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

            ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x18B;
            ptr[0] = 7;
            ptr[1] = 7;
            ptr[2] = 7;
            ptr[3] = 5;
            *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;
        break;
    }
    case 4: {
        u8* ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xAB;
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        ptr[3] = 1;
        ptr[4] = 0;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x14B;
        ptr[0] = 15;
        ptr[1] = 15;
        ptr[2] = 15;
        ptr[3] = 10;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xFB;
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        ptr[3] = 1;
        ptr[4] = 0;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x18B;
        ptr[0] = 7;
        ptr[1] = 7;
        ptr[2] = 7;
        ptr[3] = 5;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;
        break;
    }
    case 3: {
        u8* ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xAB;
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        ptr[3] = 1;
        ptr[4] = 0;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x14B;
        ptr[0] = 10;
        ptr[1] = 12;
        ptr[2] = 8;
        ptr[3] = 15;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 5 + 0xFB;
        ptr[0] = 0;
        ptr[1] = 0;
        ptr[2] = 0;
        ptr[3] = 1;
        ptr[4] = 0;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;

        ptr = (u8*) lbl_8047AA80 + idx * 4 + 0x18B;
        ptr[0] = 7;
        ptr[1] = 5;
        ptr[2] = 4;
        ptr[3] = 7;
        *(u32*) ((u8*) lbl_8047AA80 + 0x414) |= 4;
        break;
    }
    }
}
#endif
#endif
