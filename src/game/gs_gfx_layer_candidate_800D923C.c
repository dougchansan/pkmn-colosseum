/**
 * @file gs_gfx_layer_candidate_800D923C.c
 * @brief GSgfx layer: flush dirty channel / texgen / TEV state to GX,
 * 0x800D923C - 0x800D963C.
 *
 * Function-boundary carve of _gfxFlushTevState. The switch on the indirect
 * stage type compiles to a compare tree (no jump table), there is no
 * pooled constant, and the GSgfx state (lbl_8047AA80) and the draw
 * statistics (lbl_804001F0) stay extern.
 *
 * The state layout (game/gs_gfx_layer.h) takes every offset from the
 * target's loads (0x60 channel count, 0x7A TEV stage count, the
 * per-stage arrays from 0x7B, 0x3AC indirect stage count, 0x414 dirty
 * mask).
 */
#include "game/gs_gfx_layer.h"

extern void GXSetNumChans(u8 num);
extern void GXSetChanCtrl(u32 chan, u8 enable, u8 ambSrc, u8 matSrc, u8 lightMask, u8 diffFn, u8 attnFn);
extern void GXSetNumTexGens(u8 num);
extern void GXSetNumTevStages(u8 num);
extern void GXSetNumIndStages(u8 num);
extern void GXSetIndTexOrder(u32 stage, u8 coord, u8 map);
extern void GXSetIndTexCoordScale(u32 stage, u8 scaleS, u8 scaleT);
extern void GXSetIndTexMtx(u32 id, f32 (*mtx)[3], u8 scale);
extern void GXSetTevOrder(u32 stage, u8 coord, u8 map, u8 color);
extern void GXSetTevColorOp(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out);
extern void GXSetTevAlphaOp(u32 stage, u8 op, u8 bias, u8 scale, u8 clamp, u8 out);
extern void GXSetTevColorIn(u32 stage, u8 a, u8 b, u8 c, u8 d);
extern void GXSetTevAlphaIn(u32 stage, u8 a, u8 b, u8 c, u8 d);
extern void GXSetTevKColorSel(u32 stage, u32 sel);
extern void GXSetTevKAlphaSel(u32 stage, u32 sel);
extern void GXSetTevIndirect(u32 stage, u8 indStage, u8 format, u8 biasSel, u8 mtxSel, u8 wrapS, u8 wrapT, u8 addPrev, u8 utcLod, u8 alphaSel);
extern void fn_800BBC7C(u32 stage, u8 indStage, u8 signedOffsets, u8 replaceMode, u8 mtxSel);
extern void fn_800BBCE0(u32 stage, u8 indStage, u16 sizeS, u16 sizeT, u16 spacingS, u16 spacingT, u8 format, u8 mtxSel, u8 biasSel, u8 alphaSel);
extern void fn_800BBE8C(u32 stage, u8 indStage, u8 mtxSel);
extern void fn_800BBF98(u32 stage, u8 indStage, u8 mtxSel);
extern void fn_800BBFDC(u32 stage);
extern void GXSetTevDirect(u32 stage);
extern void GXSetTevKColor(u32 id, GXColor color);
extern void GXSetTevSwapModeTable(u32 table, u32 r, u32 g, u32 b, u32 a);

/*
 * The GSgfx state pointer is re-read at every use, as in retail (it is
 * reloaded after each GX call). The per-stage arrays are walked through
 * locals taken once after the TEV stage count is set; the K colour
 * selectors are indexed through the global.
 */
void _gfxFlushTevState(void) {
    s32 i;

    if (lbl_8047AA80->dirty & 1) {
        GXSetNumChans(lbl_8047AA80->tev.numChans);
        for (i = 0; i < lbl_8047AA80->tev.numChans; i++) {
            GSChanCtrl* chan = &lbl_8047AA80->tev.chanCtrl[i * 2];
            GXSetChanCtrl(i, chan[0].enable, chan[0].ambSrc, chan[0].matSrc,
                        chan[0].lightMask, chan[0].diffFn, chan[0].attnFn);
            GXSetChanCtrl(i + 2, chan[1].enable, chan[1].ambSrc, chan[1].matSrc,
                        chan[1].lightMask, chan[1].diffFn, chan[1].attnFn);
        }
        lbl_804001F0.chanUpdates++;
    }

    if (lbl_8047AA80->dirty & 2) {
        GXSetNumTexGens(lbl_8047AA80->tev.numTexGens);
        lbl_804001F0.texGenUpdates++;
    }

    if (lbl_8047AA80->dirty & 4) {
        GSTevOrder* order;
        GSTevOp* colorOp;
        GSTevOp* alphaOp;
        GSTevIn* colorIn;
        GSTevIn* alphaIn;
        u8* indEnable;
        u8 numInd;
        GSTevInd* ind;

        GXSetNumTevStages(lbl_8047AA80->tev.numTevStages);
        numInd = lbl_8047AA80->tev.numIndStages;
        order = lbl_8047AA80->tev.tevOrder;
        colorOp = lbl_8047AA80->tev.colorOp;
        alphaOp = lbl_8047AA80->tev.alphaOp;
        colorIn = lbl_8047AA80->tev.colorIn;
        alphaIn = lbl_8047AA80->tev.alphaIn;
        indEnable = lbl_8047AA80->tev.indEnable;
        ind = lbl_8047AA80->tev.ind;
        GXSetNumIndStages(numInd);
        if (numInd != 0) {
            GSIndOrder* indOrder = lbl_8047AA80->tev.indOrder;
            GSIndMtx* indMtx = lbl_8047AA80->tev.indMtx;

            for (i = 0; i < numInd; indOrder++, i++) {
                GXSetIndTexOrder(i, indOrder->coord, indOrder->map);
                GXSetIndTexCoordScale(i, indOrder->scaleS, indOrder->scaleT);
            }
            for (i = 0; i < 3; i++, indMtx++) {
                GXSetIndTexMtx(i + 1, indMtx->mtx, indMtx->scale);
            }
        }

        for (i = 0; i < lbl_8047AA80->tev.numTevStages; order++, colorOp++, alphaOp++, colorIn++, alphaIn++, indEnable++, ind++, i++) {
            GXSetTevOrder(i, order->coord, order->map, order->color);
            GXSetTevColorOp(i, colorOp->op, colorOp->bias, colorOp->scale, colorOp->clamp, colorOp->out);
            GXSetTevAlphaOp(i, alphaOp->op, alphaOp->bias, alphaOp->scale, alphaOp->clamp, alphaOp->out);
            GXSetTevColorIn(i, colorIn->a, colorIn->b, colorIn->c, colorIn->d);
            GXSetTevAlphaIn(i, alphaIn->a, alphaIn->b, alphaIn->c, alphaIn->d);
            GXSetTevKColorSel(i, lbl_8047AA80->tev.kcolorSel[i]);
            GXSetTevKAlphaSel(i, lbl_8047AA80->tev.kalphaSel[i]);
            if (*indEnable != 0 && numInd != 0) {
                switch (ind->type) {
                case 0:
                    GXSetTevIndirect(i, ind->indStage, ind->format, ind->biasSel, ind->mtxSel,
                                ind->wrapS, ind->wrapT, ind->addPrev, ind->utcLod, ind->alphaSel);
                    break;
                case 1:
                    fn_800BBC7C(i, ind->indStage, ind->warpSigned, ind->warpReplace, ind->mtxSel);
                    break;
                case 2:
                    fn_800BBCE0(i, ind->indStage, ind->tileSizeS, ind->tileSizeT, ind->tileSpacingS,
                                ind->tileSpacingT, ind->format, ind->mtxSel, ind->biasSel, ind->alphaSel);
                    break;
                case 3:
                    fn_800BBE8C(i, ind->indStage, ind->mtxSel);
                    break;
                case 4:
                    fn_800BBF98(i, ind->indStage, ind->mtxSel);
                    break;
                case 5:
                    fn_800BBFDC(i);
                    break;
                }
            } else {
                GXSetTevDirect(i);
            }
        }

        for (i = 0; i < 4; i++) {
            GXSetTevKColor(i, lbl_8047AA80->tev.kcolor[i]);
        }
        GXSetTevSwapModeTable(0, 0, 1, 2, 3);
        lbl_804001F0.tevUpdates++;
    }

    lbl_8047AA80->dirty = 0;
}
