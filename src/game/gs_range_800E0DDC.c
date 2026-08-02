/**
 * @file gs_range_800E0DDC.c
 * @brief GS render engine segment -- split from gs_render.c.
 *
 * XD source unit: unknown (best guess GSmodel head, e.g. effects.cpp analog)
 * Address range: 0x800E0DDC - 0x800E202C (3 functions)
 *
 * 3 fns (0x38, 0x730, 0xAE8). fn_800E0E14(0x730) ~ XD modelDistortionRender(0x798) which LEADS GSmodel/effects.cpp immediately after GSmath in XD - same position as here. In XD the GSmodel/GSpart/GSscratch block spans up to GStexture.cpp, matching our following gs_range_800E202C.c which runs to gs_texture.c at 0x800EF098; this segment is the head of that model block, cut by the existing bucket boundary. Zero anchors -> trivial pass.
 *
 * Split from src/game/gs_render.c (physical XD source-unit split).
 * The dead #ifdef PCPORT reference block (never defined in configure.py,
 * same situation as gs_gfx.c) was stripped during the split.
 */

#include "dolphin/types.h"
#include "dolphin/os/OS.h"

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
extern u32 lbl_8047AB40;
extern u32 lbl_8047AB44;


u32 fn_800E0DDC(void)
{
    typedef struct GSFreeBlock {
        struct GSFreeBlock* prev;
        struct GSFreeBlock* next;
        u32 size;
    } GSFreeBlock;
    typedef struct GSScratchState {
        u32 reserved[4];
        GSFreeBlock* head;
        u32 value;
    } GSScratchState;
    GSFreeBlock* cursor;
    GSScratchState* state;
    u32 total;

    state = (GSScratchState*)&__OSStartTime;
    cursor = state->head;
    lbl_8047AB44 = (u32)cursor;
    lbl_8047AB40 = state->value;
    for (total = 0, cursor = cursor->next; cursor != 0;
         cursor = cursor->next) {
        total += cursor->size;
    }
    return total;
}


#if !defined(GS_RANGE_800E0DDC_ONLY)
/* Compact the movable allocations in the GS scratch heap. */
u32 fn_800E1544(void)
{
    typedef struct GSFreeBlock {
        struct GSFreeBlock* prev;
        struct GSFreeBlock* next;
        u32 size;
    } GSFreeBlock;
    typedef struct GSAllocDesc {
        u16 used;
        u16 locked;
        u8* data;
        u32 size;
        u16 pinned;
        u16 checksum;
    } GSAllocDesc;
    extern const char lbl_80270BB8[];

    GSFreeBlock* block;
    GSFreeBlock* scan;
    GSFreeBlock* before;
    GSFreeBlock* after;
    GSFreeBlock* fresh;
    GSAllocDesc* desc;
    GSAllocDesc* candidates[4];
    GSAllocDesc* chosen[4];
    u8* source;
    u8* destination;
    u32 oldLargest;
    u32 newLargest;
    u32 total;
    u32 bestTotal;
    u32 remainder;
    u32 mask;
    u32 bestMask;
    u32 candidateCount;
    u32 chosenCount;
    u32 i;
    u32 j;
    u32 sum;
    u32 wasHead;

#define COPY_BYTES(dst_, src_, count_)                                     \
    do {                                                                    \
        u8* copyDst = (u8*)(dst_);                                         \
        u8* copySrc = (u8*)(src_);                                         \
        u32 copyCount = (count_);                                          \
        if (copyDst < copySrc) {                                           \
            for (i = 0; i < copyCount; i++) {                             \
                copyDst[i] = copySrc[i];                                   \
            }                                                               \
        } else if (copyDst > copySrc) {                                    \
            for (i = copyCount; i != 0; i--) {                            \
                copyDst[i - 1] = copySrc[i - 1];                           \
            }                                                               \
        }                                                                   \
    } while (0)

#define COALESCE_FREE(node_)                                                \
    do {                                                                    \
        GSFreeBlock* mergeNode = (node_);                                  \
        if (mergeNode->next != 0 &&                                        \
            (u8*)mergeNode + mergeNode->size ==                            \
                (u8*)mergeNode->next) {                                    \
            GSFreeBlock* mergeNext = mergeNode->next;                      \
            mergeNode->size += mergeNext->size;                            \
            mergeNode->next = mergeNext->next;                             \
            if (mergeNode->next != 0) {                                    \
                mergeNode->next->prev = mergeNode;                         \
            }                                                               \
        }                                                                   \
        if (mergeNode->prev != 0 &&                                        \
            (u8*)mergeNode->prev + mergeNode->prev->size ==                \
                (u8*)mergeNode) {                                          \
            GSFreeBlock* mergePrev = mergeNode->prev;                      \
            mergePrev->size += mergeNode->size;                            \
            mergePrev->next = mergeNode->next;                             \
            if (mergePrev->next != 0) {                                    \
                mergePrev->next->prev = mergePrev;                         \
            }                                                               \
        }                                                                   \
    } while (0)

#define INSERT_FREE(node_)                                                  \
    do {                                                                    \
        GSFreeBlock* insertNode = (node_);                                 \
        before = 0;                                                        \
        after = (GSFreeBlock*)lbl_8047AB30;                                \
        while (after != 0 && after < insertNode) {                         \
            before = after;                                                \
            after = after->next;                                           \
        }                                                                   \
        insertNode->prev = before;                                         \
        insertNode->next = after;                                          \
        if (before != 0) {                                                 \
            before->next = insertNode;                                     \
        } else {                                                            \
            lbl_8047AB30 = (u32)insertNode;                                \
        }                                                                   \
        if (after != 0) {                                                  \
            after->prev = insertNode;                                      \
        }                                                                   \
        COALESCE_FREE(insertNode);                                         \
    } while (0)

    oldLargest = 0;
    for (block = (GSFreeBlock*)lbl_8047AB30; block != 0;
         block = block->next) {
        if (oldLargest < block->size) {
            oldLargest = block->size;
        }
    }

    block = (GSFreeBlock*)lbl_8047AB30;
    while (block != 0 && block->next != 0) {
        u8* blockEnd = (u8*)block + block->size;
        if (blockEnd == (u8*)lbl_8047AB38) {
            block = block->next;
            continue;
        }

        desc = (GSAllocDesc*)lbl_8047AB34;
        while (desc >= (GSAllocDesc*)lbl_8047AB38) {
            if (desc->used != 0 && desc->data == blockEnd) {
                break;
            }
            desc--;
        }
        if (desc < (GSAllocDesc*)lbl_8047AB38) {
            GSlogWrite(lbl_80270BB8);
            return 0;
        }

        if (desc->locked == 0 && desc->pinned == 0) {
            wasHead = (block == (GSFreeBlock*)lbl_8047AB30);
            before = block->prev;
            after = block->next;
            source = desc->data;
            destination = (u8*)block;
            COPY_BYTES(destination, source, desc->size);
            desc->data = destination;

            fresh = (GSFreeBlock*)(destination + desc->size);
            fresh->prev = before;
            fresh->next = after;
            fresh->size = block->size;
            if (before != 0) {
                before->next = fresh;
            }
            if (after != 0) {
                after->prev = fresh;
            }
            if (wasHead != 0) {
                lbl_8047AB30 = (u32)fresh;
            }
            COALESCE_FREE(fresh);
            block = (GSFreeBlock*)lbl_8047AB30;
            continue;
        }

        candidateCount = 0;
        for (desc = (GSAllocDesc*)lbl_8047AB34;
             desc >= (GSAllocDesc*)lbl_8047AB38; desc--) {
            if (desc->used == 0 || desc->locked != 0 || desc->pinned != 0 ||
                desc->data <= (u8*)block || desc->size > block->size) {
                continue;
            }
            if (candidateCount < 4) {
                candidates[candidateCount++] = desc;
            } else {
                bestTotal = 0;
                bestMask = 0;
                for (mask = 1; mask < 16; mask++) {
                    total = 0;
                    for (i = 0; i < 4; i++) {
                        if ((mask & (1 << i)) != 0) {
                            total += candidates[i]->size;
                        }
                    }
                    if (total <= block->size && total > bestTotal) {
                        bestTotal = total;
                        bestMask = mask;
                    }
                }
                for (i = 0; i < 4; i++) {
                    if ((bestMask & (1 << i)) == 0 &&
                        desc->size > candidates[i]->size) {
                        candidates[i] = desc;
                        break;
                    }
                }
            }
        }

        bestTotal = 0;
        bestMask = 0;
        for (mask = 1; mask < (1U << candidateCount); mask++) {
            total = 0;
            for (i = 0; i < candidateCount; i++) {
                if ((mask & (1U << i)) != 0) {
                    total += candidates[i]->size;
                }
            }
            if (total <= block->size && total > bestTotal) {
                bestTotal = total;
                bestMask = mask;
            }
        }
        if (bestMask == 0) {
            block = block->next;
            continue;
        }

        chosenCount = 0;
        for (i = 0; i < candidateCount; i++) {
            if ((bestMask & (1U << i)) != 0) {
                chosen[chosenCount++] = candidates[i];
            }
        }

        before = block->prev;
        after = block->next;
        remainder = block->size;
        if (block == (GSFreeBlock*)lbl_8047AB30) {
            lbl_8047AB30 = (u32)after;
        }
        if (before != 0) {
            before->next = after;
        }
        if (after != 0) {
            after->prev = before;
        }

        destination = (u8*)block;
        for (j = 0; j < chosenCount; j++) {
            desc = chosen[j];
            source = desc->data;
            COPY_BYTES(destination, source, desc->size);
            desc->data = destination;

            fresh = (GSFreeBlock*)source;
            fresh->size = desc->size;
            INSERT_FREE(fresh);
            destination += desc->size;
            remainder -= desc->size;
        }

        if (remainder >= sizeof(GSFreeBlock)) {
            fresh = (GSFreeBlock*)destination;
            fresh->size = remainder;
            INSERT_FREE(fresh);
        } else {
            desc = chosen[chosenCount - 1];
            desc->size += remainder;
            if (*(u8*)&lbl_8047AB28 != 0) {
                desc->data[0] = 0;
                desc->data[1] = 0;
                desc->data[2] = 0;
                desc->data[3] = 0;
                desc->data[desc->size - 4] = 0;
                desc->data[desc->size - 3] = 0;
                desc->data[desc->size - 2] = 0;
                desc->data[desc->size - 1] = 0;
                sum = 0x3D94;
                for (i = 0; i + 1 < desc->size; i += 2) {
                    sum += *(u16*)&desc->data[i];
                }
                if ((desc->size & 1) != 0) {
                    sum += desc->data[desc->size - 1];
                }
                desc->checksum = (u16)sum;
            }
        }
        block = (GSFreeBlock*)lbl_8047AB30;
    }

    newLargest = 0;
    for (scan = (GSFreeBlock*)lbl_8047AB30; scan != 0;
         scan = scan->next) {
        if (newLargest < scan->size) {
            newLargest = scan->size;
        }
    }

#undef INSERT_FREE
#undef COALESCE_FREE
#undef COPY_BYTES
    return newLargest - oldLargest;
}

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
#if 0
asm void fn_800E0E14(void) {
#include "src/game/gs_render_fn_800E0E14.inc"
}
#else
u8 fn_800E0E14(u32 verbose, u32 dumpMap) {
    u8 ok;
    u32* block;
    u8* cursor;
    u8* end;
    u32 count;

    ok = 1;
    if (verbose != 0) {
        GSlogWrite((char*)lbl_80270658);
    }

    block = (u32*)lbl_8047AB30;
    count = 0;
    while (block != 0) {
        count++;
        if ((u32)block < lbl_8047AB68 || (u32)block > lbl_8047AB64) {
            ok = 0;
            GSlogWrite((char*)lbl_80270658 + 0x64);
        }
        if ((u32)block >= lbl_8047AB38) {
            ok = 0;
            GSlogWrite((char*)lbl_80270658 + 0x90);
        }
        if ((u32)block + block[2] > lbl_8047AB38) {
            ok = 0;
            GSlogWrite((char*)lbl_80270658 + 0xc0);
        }
        block = (u32*)block[1];
    }

    cursor = (u8*)lbl_8047AB68;
    end = (u8*)lbl_8047AB38;
    while (cursor < end) {
        u32 size = *(u32*)(cursor + 0x8);
        if (size == 0 || cursor + size > end) {
            ok = 0;
            GSlogWrite((char*)lbl_80270658 + 0x2e8, cursor);
            break;
        }
        if (dumpMap != 0) {
            GSlogWrite((char*)lbl_80270658 + 0x268, cursor, cursor + size - 1,
                        *(u32*)cursor, *(u32*)(cursor + 4));
        }
        cursor += size;
    }

    if ((u32)cursor != lbl_8047AB38) {
        ok = 0;
        GSlogWrite((char*)lbl_80270658 + 0x318, cursor);
    }
    if (count != lbl_8047AB4C) {
        ok = 0;
        GSlogWrite((char*)lbl_80270658 + 0x36c);
    }

    if (verbose != 0) {
        GSlogWrite((char*)lbl_80270658 + 0x3a4, lbl_8047AB68, lbl_8047AB64);
        GSlogWrite((char*)lbl_80270658 + 0x3cc, lbl_8047AB4C);
        GSlogWrite((char*)lbl_80270658 + 0x3e8, lbl_8047AB48);
        GSlogWrite((char*)lbl_80270658 + 0x404, lbl_8047AB34 - lbl_8047AB38 + 0x10);
    }

    if (!ok) {
        lbl_8047AB3C = 0;
    }
    return ok;
}
#endif
#endif
