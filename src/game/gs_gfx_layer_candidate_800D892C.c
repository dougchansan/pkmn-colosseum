/**
 * @file gs_gfx_layer_candidate_800D892C.c
 * @brief GSgfx: set up channels, texgens and TEV stages for a vertex
 * descriptor, then flush them (_gfxFlushTevState). 0x800D892C - 0x800D923C.
 *
 * Byte-exact (lane D6, 2026-09-29) and linked as Matching under the
 * byte-match-first policy, with the rule exceptions tagged below and listed
 * in docs/RULE_EXCEPTIONS.md. Function-boundary carve material: no jump
 * table, no pooled constant. The tables (lbl_80314404, lbl_803144F0,
 * lbl_80478AE0) and the saved TEV block lbl_80400B28 stay extern.
 *
 * The channel, texgen and TEV setters from game/gs_gfx_layer.h are
 * expanded throughout (the channel setter four times, the TEV-order
 * setter in both stage loops, the TEV colour setters again after
 * fn_800D963C's presets).
 *
 * `mode` is not set when flag 0x40000000 is present. Retail then reads it
 * uninitialised: r0 still holds the saved link register at the
 * `cmpwi r0, 0x1`. The source leaves it uninitialised to match.
 *
 * Open question: retail stores the saved indirect-stage count back twice
 * when it is non-zero, first as n + 1 and then as (u8)(n + 1) - 1. The
 * ++/-- pair below reproduces those stores, but it is a placeholder, not
 * the recovered source construct.
 *
 * Register allocation (lanes D5 and D6; see
 * docs/recon/gs_gfx_800D892C_wall.md):
 * - gfxSetChanCtrlIdx indexes chanCtrl with (u32)chan, so the backend,
 *   not the frontend, strength-reduces chan*6, as in retail. The cast is
 *   shaping: its only effect is where the reduction happens.
 * - gfxSetTevOrderIdx takes an s32 stage (the shared header's u32 stage
 *   ranks the tevOrder offset last among the nine loop offsets).
 * - `#pragma opt_dead_assignments off` for this function puts the
 *   saved-TEV loop's stage-source pointer in retail's r6 (without it, r9).
 * - `last` and `i` are shared by both paths, and the TEV-load path sets i
 *   before last. MWCC's live-range splitter then creates the default
 *   path's `last` temp after the loop-counter temps, which gives it r21.
 *   Because it is a compiler temp, CSE drops the redundant slot-0
 *   `last = 0` and leaves retail's branchless compare.
 * - The default path counts texgens slot by slot through one u8 `en`
 *   local. Eight separate load temps would push the layer pointer's degree
 *   past 32 (r4/r6/r5 instead of r6/r5/r4).
 */
#include "game/gs_gfx_layer.h"

extern void* memcpy(void* dst, const void* src, u32 n);


extern GSgfxTevState lbl_80400B28;
extern u32 lbl_80314404[8];
extern u32 lbl_803144F0[8];
extern u32 lbl_80478AE0[2];

extern void gfxSetTevOp(u32 stage, s32 mode);
extern void _gfxFlushTevState(void);

static inline void gfxSetChanCtrlIdx(s32 chan, u8 enable, u8 ambSrc, u8 matSrc, u8 lightMask, u8 diffFn, u8 attnFn) {
    u8 both = FALSE;
    u8 done;
    GSChanCtrl* ctrl;

    if (chan == 4) {
        chan = 0;
        both = TRUE;
    } else if (chan == 5) {
        chan = 1;
        both = TRUE;
    }
    done = FALSE;
    while (!done) {
        /* RULE-EXCEPTION(title-path): cast whose only effect is register allocation - see docs/RULE_EXCEPTIONS.md */
        ctrl = &lbl_8047AA80->tev.chanCtrl[(u32)chan];
        ctrl->enable = enable;
        ctrl->ambSrc = ambSrc;
        ctrl->matSrc = matSrc;
        ctrl->lightMask = lightMask;
        ctrl->diffFn = diffFn;
        ctrl->attnFn = attnFn;
        if (both) {
            both = FALSE;
            chan++;
        } else {
            done = TRUE;
        }
    }
    lbl_8047AA80->dirty |= 1;
}

static inline void gfxSetTevOrderIdx(s32 stage, s32 coord, s32 map, s32 color) {
    GSTevOrder* order = &lbl_8047AA80->tev.tevOrder[stage];

    order->coord = coord;
    order->map = map;
    order->color = color;
    lbl_8047AA80->dirty |= 4;
}

#pragma push
#pragma opt_dead_assignments off /* RULE-EXCEPTION(title-path): local compiler-control pragma - see docs/RULE_EXCEPTIONS.md */
void gfxSetupChannelsTev(GSVtxDesc* desc) {
    s32 mode;
    u32 mask;
    u32 bit;
    s32 last;
    s32 i;

    if (!(lbl_8047AA80->flags & 0x40000000)) {
        mode = 1;
        if (desc->attr[5].enabled != 0) {
            mode = 2;
        } else if (!(lbl_8047AA80->flags & 4) && !(lbl_8047AA80->flags & 1)) {
            mode = 0;
        }
        GSgfxSetNumChans(mode);
        if (mode > 0) {
            if (lbl_8047AA80->flags & 4) {
                mask = 0;
                for (bit = 0x10; bit <= 0x800; bit++) {
                    if (lbl_8047AA80->flags & bit) {
                        mask |= 1 << (bit - 0x10);
                    }
                }
                if (lbl_8047AA80->flags & 1) {
                    gfxSetChanCtrlIdx(4, 1, 0, 1, mask, 2, 2);
                } else {
                    gfxSetChanCtrlIdx(4, 1, 0, 0, mask, 2, 2);
                }
            } else {
                gfxSetChanCtrlIdx(4, 0, 1, 1, 0, 0, 2);
            }
            if (mode == 2) {
                gfxSetChanCtrlIdx(5, 0, 1, 1, 0, 0, 2);
            }
        }
    }

    if (!(lbl_8047AA80->flags & 2)) {
        GSgfxSetNumTexGens(0);
        if (mode == 1) {
            GSgfxSetNumTevStages(1);
            GSgfxSetTevOrder(0, 0xFF, 0xFF, 4);
            gfxSetTevOp(0, 4);
        } else {
            GSgfxSetNumTevStages(2);
            GSgfxSetTevOrder(0, 0xFF, 0xFF, 4);
            gfxSetTevOp(0, 4);
            GSgfxSetTevOrder(1, 0xFF, 0xFF, 5);
            GSgfxSetTevColorOp(1, 0, 0, 0, 1, 0);
            GSgfxSetTevColorIn(1, 15, 10, 12, 0);
        }
        GSgfxSetNumIndStages(0);
    } else if (lbl_8047AA80->flags & 0x80000000) {
        s32 numInd;
        s32 numTev;
        u8 indEnable;

        /* RULE-EXCEPTION(title-path): unrecovered ++/-- placeholder construct - see docs/RULE_EXCEPTIONS.md */
        if (lbl_80400B28.numIndStages != 0) {
            lbl_80400B28.numIndStages++;
            lbl_80400B28.numIndStages--;
        }

        /* RULE-EXCEPTION(title-path): i before last only orders the live-range split - see docs/RULE_EXCEPTIONS.md */
        for (i = 0, last = 0; i < 8; i++) {
            if (desc->attr[6 + i].enabled == 1) {
                last = i;
            }
        }

        numTev = lbl_80400B28.numTevStages;
        numInd = lbl_80400B28.numIndStages;
        GSgfxSetNumTexGens(last + 1);
        GSgfxSetNumTevStages(numTev);
        GSgfxSetNumIndStages(numInd);
        for (i = 0; i < numTev; i++) {
            GSTevStageSrc* src;

            src = &lbl_8047AA80->stageSrc[i];
            lbl_8047AA80->tev.indEnable[i] = indEnable = (lbl_80400B28.indEnable[i] != 0 && numInd > 0);
            gfxSetTevOrderIdx(i, lbl_80314404[src->coord], lbl_803144F0[src->map], lbl_80478AE0[src->color]);
            memcpy(&lbl_8047AA80->tev.colorOp[i], &lbl_80400B28.colorOp[i], sizeof(GSTevOp));
            memcpy(&lbl_8047AA80->tev.alphaOp[i], &lbl_80400B28.alphaOp[i], sizeof(GSTevOp));
            memcpy(&lbl_8047AA80->tev.colorIn[i], &lbl_80400B28.colorIn[i], sizeof(GSTevIn));
            memcpy(&lbl_8047AA80->tev.alphaIn[i], &lbl_80400B28.alphaIn[i], sizeof(GSTevIn));
            lbl_8047AA80->tev.kcolorSel[i] = lbl_80400B28.kcolorSel[i];
            lbl_8047AA80->tev.kalphaSel[i] = lbl_80400B28.kalphaSel[i];
            if (indEnable) {
                memcpy(&lbl_8047AA80->tev.ind[i], &lbl_80400B28.ind[i], sizeof(GSTevInd));
            }
        }
        memcpy(lbl_8047AA80->tev.kcolor, lbl_80400B28.kcolor, sizeof(lbl_80400B28.kcolor));
        if (numInd != 0) {
            memcpy(lbl_8047AA80->tev.indOrder, lbl_80400B28.indOrder, numInd * sizeof(GSIndOrder));
            memcpy(lbl_8047AA80->tev.indMtx, lbl_80400B28.indMtx, sizeof(lbl_80400B28.indMtx));
        }
    } else {
        u8 en; /* RULE-EXCEPTION(title-path): one load local for register allocation - see docs/RULE_EXCEPTIONS.md */

        last = 0;
        en = desc->attr[6].enabled;
        if (en == 1) {
            last = 0;
        }
        en = desc->attr[7].enabled;
        if (en == 1) {
            last = 1;
        }
        en = desc->attr[8].enabled;
        if (en == 1) {
            last = 2;
        }
        en = desc->attr[9].enabled;
        if (en == 1) {
            last = 3;
        }
        en = desc->attr[10].enabled;
        if (en == 1) {
            last = 4;
        }
        en = desc->attr[11].enabled;
        if (en == 1) {
            last = 5;
        }
        en = desc->attr[12].enabled;
        if (en == 1) {
            last = 6;
        }
        en = desc->attr[13].enabled;
        if (en == 1) {
            last = 7;
        }

        GSgfxSetNumTexGens(last + 1);
        GSgfxSetNumTevStages(last + 1);
        GSgfxSetNumIndStages(0);
        lbl_8047AA80->tev.numIndStages = lbl_80400B28.numIndStages;
        for (i = 0; i <= last; i++) {
            GSTevStageSrc* src = &lbl_8047AA80->stageSrc[i];

            gfxSetTevOrderIdx(i, lbl_80314404[src->coord], lbl_803144F0[src->map], lbl_80478AE0[src->color]);
            gfxSetTevOp(i, src->mode);
        }
    }

    _gfxFlushTevState();
}
#pragma pop
