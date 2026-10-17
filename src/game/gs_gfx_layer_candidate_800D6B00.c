/**
 * @file gs_gfx_layer_candidate_800D6B00.c
 * @brief GSgfx vertex flush for line primitives, 0x800D6B00 - 0x800D7230.
 *
 * gfxFlushPendingVertex replays the pending vertex
 * through the installed emitters; for primitive type 7 it keeps the
 * previous vertex in two save slots and emits the extra segment vertex.
 * The emitter sequence is expanded three times (repeated expansion), so
 * it is recovered as the static inline GSgfxEmitVertex.
 *
 * The unit owns the save slots: the pooled .bss at 0x804007E8-0x80400948
 * (0x160 bytes, retail label lbl_804007E8). Retail addresses them from one
 * base register and passes the first as "addi r3,r31,0x0", which MWCC
 * emits for three or more pooled objects under GC/1.3.2 (see the GS VM
 * note in 488eeb59), so the unit builds with GC/1.3.2.
 *
 * Retail lays the objects out as tex0, clr0, pos0, tex1, clr1, pos1
 * (0x0/0x80/0x98/0xB0/0x130/0x148). Without deferred inlining MWCC orders
 * pooled .bss by first reference (pos0 first); with -inline deferred it
 * uses reverse definition order, the same rule the linked pslist.c relies
 * on. The slots are declared held vertex (slot 1) first, each in the
 * vertex state's own pos/clr/tex field order, which gives retail's
 * layout. The object links as Matching with retail DOL/REL SHA-1.
 */
#include "game/gs_gfx_layer.h"

extern void* memcpy(void* dst, const void* src, u32 n);
extern void fn_800DB098(void);

/* Save slot 0 (the vertex being replaced) and slot 1 (the held vertex). */
static GSVtxValue3 sPos1;
static GSVtxColor sClr1[2];
static GSVtxValue2 sTex1[8];
static GSVtxValue3 sPos0;
static GSVtxColor sClr0[2];
static GSVtxValue2 sTex0[8];

static inline void GSgfxEmitVertex(void) {
    GSVtxDesc* desc;
    s32 i;
    s32 n;

    if (lbl_8047AA80->captureActive == 1) {
        fn_800DB098();
    } else if (lbl_8047AA80->layerDraw == lbl_8047AA80->layerCur &&
               (lbl_8047AA80->drawMask & lbl_8047AA80->drawEnable)) {
        desc = lbl_8047AA80->vtxDesc;
        if (desc->attr[0].enabled) {
            lbl_8047AA80->emitMtxIdx(0);
        }
        lbl_8047AA80->emitPos(0);
        if (desc->attr[2].enabled) {
            lbl_8047AA80->emitNrm(0);
        }
        for (i = 4; i <= 5; i++) {
            if (desc->attr[i].enabled) {
                n = i - 4;
                lbl_8047AA80->emitClr[n](n);
            }
        }
        for (i = 6; i <= 13; i++) {
            if (desc->attr[i].enabled) {
                n = i - 6;
                lbl_8047AA80->emitTex[n](n);
            }
        }
    }
}

void gfxFlushPendingVertex(void) {
    s32 k;

    if (lbl_8047AA80->vtxPending == 0) {
        return;
    }
    if ((lbl_8047AA80->captureActive == 0 && lbl_8047AA80->primType == 7) ||
        (lbl_8047AA80->captureActive == 1 && lbl_8047AA80->capturePrimType == 7)) {
        if (lbl_8047AA80->lineHalf == 1) {
            memcpy(&sPos0, &lbl_8047AA80->pos, sizeof(GSVtxValue3));
            memcpy(&sClr0, lbl_8047AA80->clr, sizeof(lbl_8047AA80->clr));
            memcpy(sTex0, lbl_8047AA80->tex, sizeof(sTex0));
            lbl_8047AA80->pos.b[0] = sPos1.b[0];
            lbl_8047AA80->pos.s[0] = sPos1.s[0];
            lbl_8047AA80->pos.f[0] = sPos1.f[0];
            memcpy(&lbl_8047AA80->clr, sClr1, sizeof(sClr1));
            for (k = 0; k < 8; k++) {
                lbl_8047AA80->tex[k].b[0] = sTex1[k].b[0];
                lbl_8047AA80->tex[k].s[0] = sTex1[k].s[0];
                lbl_8047AA80->tex[k].f[0] = sTex1[k].f[0];
            }
            GSgfxEmitVertex();
            memcpy(&lbl_8047AA80->pos, &sPos0, sizeof(GSVtxValue3));
            memcpy(&lbl_8047AA80->clr, sClr0, sizeof(sClr0));
            memcpy(lbl_8047AA80->tex, sTex0, sizeof(sTex0));
            GSgfxEmitVertex();
            lbl_8047AA80->pos.b[1] = sPos1.b[1];
            lbl_8047AA80->pos.s[1] = sPos1.s[1];
            lbl_8047AA80->pos.f[1] = sPos1.f[1];
            for (k = 0; k < 8; k++) {
                lbl_8047AA80->tex[k].b[1] = sTex1[k].b[1];
                lbl_8047AA80->tex[k].s[1] = sTex1[k].s[1];
                lbl_8047AA80->tex[k].f[1] = sTex1[k].f[1];
            }
            lbl_8047AA80->lineHalf = 0;
        } else {
            memcpy(&sPos1, &lbl_8047AA80->pos, sizeof(GSVtxValue3));
            memcpy(&sClr1, lbl_8047AA80->clr, sizeof(lbl_8047AA80->clr));
            memcpy(sTex1, lbl_8047AA80->tex, sizeof(sTex1));
            lbl_8047AA80->lineHalf = 1;
        }
    }
    GSgfxEmitVertex();
    lbl_8047AA80->vtxPending = 0;
}
