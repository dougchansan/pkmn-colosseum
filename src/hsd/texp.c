/**
 * @file texp.c
 * @brief HSD TExp - TEV expression construction, register assignment and
 *        compilation into TEV stage descriptors.
 *
 * Retail TU: .text 0x801B42C0 - 0x801B7CA0, with its .rodata string pool
 * (0x802753A8), .data tables and jump tables (0x8036D258) and .sdata2
 * strings/constants (0x8047DE70).
 *
 * HAL sysdolphin (>= 1.3.0.0) texp.c, reconstructed with the Melee
 * decompilation (doldecomp/melee, src/sysdolphin/baselib/texp.c) as
 * reference. Functions appear in HAL source order; the library is built with
 * -inline auto,deferred, which emits them in reverse order.
 *
 * Colosseum's version differs from Melee's: HSD_TETev gained an alpha range
 * and a per-expression TEV swap table (set by fn_801B6DC0), HSD_TECnst keeps
 * comp/ctype as bytes, and the TEV descriptor carries the swap table.
 */
#include "hsd/hsd_texp.h"

#include "crt/string.h"
#include "hsd/hsd_debug.h"

/* hsdAllocMemPiece / hsdFreeMemPiece (class.c) */
extern void fn_80193AF0(void* mem, s32 size);
extern void* fn_80193B10(s32 size);

/* HSD_Panic (debug.c) */
extern void HSD_Panic(const char* file, int line, const char* msg);

/* GX */
extern void GXPixModeSync(void);                        /* GXPixModeSync */
extern void GXSetTevColor(GXTevRegID id, GXColor color); /* GXSetTevColor */
extern void GXSetTevKColor(GXTevKColorID id, GXColor color); /* GXSetTevKColor */

/* state.c / tev.c */
extern void HSD_StateInvalidate(u32 state);                   /* HSD_StateInvalidate */
extern int HSD_StateAssignTev(void);
extern void HSD_SetupTevStage(HSD_TevDesc* desc);           /* HSD_SetupTevStage */
extern u32 HSD_Index2TevStage(u32 idx);

HSD_TExpType HSD_TExpGetType(HSD_TExp* texp)
{
    if (texp == NULL) {
        return HSD_TE_ZERO;
    }
    if (texp == HSD_TEXP_TEX) {
        return HSD_TE_TEX;
    }
    if (texp == HSD_TEXP_RAS) {
        return HSD_TE_RAS;
    }
    return texp->type;
}

/*
 * TevAlloc, CnstAlloc and HSD_TExpFree are HAL helpers (Melee's texp.c has
 * them) that retail always expands: the allocation asserts keep their own
 * line numbers (63, 71) inside HSD_TExpTev/HSD_TExpCnst, HSD_TExpFree is
 * expanded four times in HSD_TExpFreeList, and none of them has a symbol.
 */
static inline HSD_TExp* TevAlloc(void)
{
    HSD_TExp* texp = fn_80193B10(sizeof(HSD_TETev));
    HSD_ASSERT(63, texp);
    return texp;
}

static inline HSD_TExp* CnstAlloc(void)
{
    HSD_TExp* texp = fn_80193B10(sizeof(HSD_TECnst));
    HSD_ASSERT(71, texp);
    return texp;
}

static inline void HSD_TExpFree(HSD_TExp* texp)
{
    switch (HSD_TExpGetType(texp)) {
    case HSD_TE_TEV:
        fn_80193AF0(texp, sizeof(HSD_TETev));
        break;
    case HSD_TE_CNST:
        fn_80193AF0(texp, sizeof(HSD_TECnst));
        break;
    }
}

/* HSD_TExpRef */
void HSD_TExpRef(HSD_TExp* texp, u8 sel)
{
    switch (HSD_TExpGetType(texp)) {
    case HSD_TE_TEV:
        if (sel == HSD_TE_RGB) {
            texp->tev.c_ref += 1;
        } else {
            texp->tev.a_ref += 1;
        }
        break;
    case HSD_TE_CNST:
        texp->cnst.ref += 1;
        break;
    }
}

/* HSD_TExpUnref */
void HSD_TExpUnref(HSD_TExp* texp, u8 sel)
{
    int i;

    switch (HSD_TExpGetType(texp)) {
    case HSD_TE_TEV:
        if (sel == HSD_TE_RGB) {
            if (texp->tev.c_ref != 0) {
                texp->tev.c_ref -= 1;
            }
        } else {
            if (texp->tev.a_ref != 0) {
                texp->tev.a_ref -= 1;
            }
        }
        if (texp->tev.c_ref == 0 && texp->tev.a_ref == 0) {
            for (i = 0; i < 4; i++) {
                HSD_TExpUnref(texp->tev.c_in[i].exp, texp->tev.c_in[i].sel);
                HSD_TExpUnref(texp->tev.a_in[i].exp, texp->tev.a_in[i].sel);
            }
        }
        break;
    case HSD_TE_CNST:
        if (texp->cnst.ref != 0) {
            texp->cnst.ref -= 1;
        }
        break;
    }
}

/* HSD_TExpFreeList */
HSD_TExp* HSD_TExpFreeList(HSD_TExp* texp_list, HSD_TExpType type, s32 all)
{
    HSD_TExp** handle;
    HSD_TExp* ptr;
    HSD_TExp* next;

    handle = &texp_list;

    if (all != 0) {
        while (*handle != NULL) {
            if (type == HSD_TE_ALL || type == (*handle)->type) {
                switch ((*handle)->type) {
                case HSD_TE_TEV:
                    next = (*handle)->tev.next;
                    HSD_TExpFree(*handle);
                    *handle = next;
                    continue;
                case HSD_TE_CNST:
                    next = (*handle)->tev.next;
                    HSD_TExpFree(*handle);
                    *handle = next;
                    continue;
                default:
                    HSD_ASSERT(220, 0);
                }
            }
            handle = &(*handle)->tev.next;
        }
    } else {
        if (type == HSD_TE_ALL || type == HSD_TE_TEV) {
            for (ptr = texp_list; ptr != NULL; ptr = ptr->tev.next) {
                switch (ptr->type) {
                case HSD_TE_TEV:
                    if (ptr->tev.c_ref == 0 && ptr->tev.c_ref == 0) {
                        HSD_TExpUnref(ptr, HSD_TE_RGB);
                        HSD_TExpUnref(ptr, HSD_TE_A);
                    }
                    break;
                default:
                    break;
                }
            }
        }

        while (*handle != NULL) {
            if (type == HSD_TE_ALL || type == (*handle)->type) {
                switch ((*handle)->type) {
                case HSD_TE_TEV:
                    if ((*handle)->tev.c_ref != 0 || (*handle)->tev.a_ref != 0)
                    {
                        break;
                    }
                    next = (*handle)->tev.next;
                    HSD_TExpFree(*handle);
                    *handle = next;
                    continue;
                case HSD_TE_CNST:
                    if ((*handle)->cnst.ref != 0) {
                        break;
                    }
                    next = (*handle)->tev.next;
                    HSD_TExpFree(*handle);
                    *handle = next;
                    continue;
                default:
                    HSD_ASSERT(268, 0);
                }
            }
            handle = &(*handle)->tev.next;
        }
    }

    return texp_list;
}

/* HSD_TExpTev */
HSD_TExp* HSD_TExpTev(HSD_TExp** texp_list)
{
    int i;
    HSD_TExp* texp;

    HSD_ASSERT(295, texp_list);
    texp = TevAlloc();
    memset(texp, 0xFF, sizeof(HSD_TETev));
    texp->type = HSD_TE_TEV;
    texp->tev.next = *texp_list;
    *texp_list = texp;
    texp->tev.c_ref = 0;
    texp->tev.a_ref = 0;
    texp->tev.swap_red = GX_CH_RED;
    texp->tev.swap_green = GX_CH_GREEN;
    texp->tev.swap_blue = GX_CH_BLUE;
    texp->tev.swap_alpha = GX_CH_ALPHA;
    texp->tev.tex = NULL;
    for (i = 0; i < 4; i++) {
        texp->tev.c_in[i].exp = NULL;
        texp->tev.a_in[i].exp = NULL;
    }
    return texp;
}

HSD_TExp* HSD_TExpCnst(void* val, HSD_TEInput comp, HSD_TEType type,
                       HSD_TExp** texp_list)
{
    HSD_TExp* texp;

    HSD_ASSERT(371, texp_list);

    for (texp = *texp_list; texp != NULL; texp = texp->cnst.next) {
        if (texp->type == HSD_TE_CNST && texp->cnst.val == val &&
            texp->cnst.comp == comp)
        {
            HSD_ASSERT(378, texp->cnst.ctype == type);
            return texp;
        }
    }

    if (comp == HSD_TE_0) {
        return NULL;
    }
    texp = CnstAlloc();
    texp->cnst.type = HSD_TE_CNST;
    texp->cnst.next = *texp_list;
    *texp_list = texp;
    texp->cnst.ref = 0;
    texp->cnst.val = val;
    texp->cnst.comp = comp;
    texp->cnst.ctype = type;
    texp->cnst.reg = HSD_TE_UNDEF;
    texp->cnst.idx = HSD_TE_UNDEF;
    texp->cnst.range = 0;
    return texp;
}

/* HSD_TExpColorOp */
void HSD_TExpColorOp(HSD_TExp* texp, GXTevOp op, GXTevBias bias,
                 GXTevScale scale, u8 clamp)
{
    HSD_ASSERT(419, texp);
    HSD_ASSERT(420, HSD_TExpGetType(texp) == HSD_TE_TEV);

    texp->tev.c_op = op;
    texp->tev.c_clamp = (clamp ? GX_ENABLE : GX_DISABLE);
    if (op == GX_TEV_ADD || op == GX_TEV_SUB) {
        texp->tev.c_bias = bias;
        texp->tev.c_scale = scale;
    } else {
        texp->tev.c_bias = 0;
        texp->tev.c_scale = 0;
    }
}

/* Sets the TEV swap table used by this expression's stage. */
void fn_801B6DC0(HSD_TExp* texp, GXTevColorChan red, GXTevColorChan green,
                 GXTevColorChan blue, GXTevColorChan alpha)
{
    HSD_ASSERT(443, texp);
    HSD_ASSERT(444, HSD_TExpGetType(texp) == HSD_TE_TEV);

    texp->tev.swap_red = red;
    texp->tev.swap_green = green;
    texp->tev.swap_blue = blue;
    texp->tev.swap_alpha = alpha;
}

/* HSD_TExpAlphaOp */
void HSD_TExpAlphaOp(HSD_TExp* texp, GXTevOp op, GXTevBias bias,
                 GXTevScale scale, u8 clamp)
{
    HSD_ASSERT(467, texp);
    HSD_ASSERT(468, HSD_TExpGetType(texp) == HSD_TE_TEV);

    texp->tev.a_op = op;
    texp->tev.a_clamp = (clamp ? GX_ENABLE : GX_DISABLE);
    if (op == GX_TEV_ADD || op == GX_TEV_SUB) {
        texp->tev.a_bias = bias;
        texp->tev.a_scale = scale;
    } else {
        texp->tev.a_bias = 0;
        texp->tev.a_scale = 0;
    }
}

/* HSD_TExpColorInSub */
static void HSD_TExpColorInSub(HSD_TETev* tev, HSD_TEInput sel, HSD_TExp* exp,
                        s32 idx)
{
    HSD_TEArg prev;
    u8 ksel;

    prev = tev->c_in[idx];
    tev->c_in[idx].type = HSD_TExpGetType(exp);
    tev->c_in[idx].sel = sel;
    tev->c_in[idx].exp = exp;
    tev->c_in[idx].arg = 0xFF;

    switch (sel) {
    case HSD_TE_0:
        tev->c_in[idx].arg = GX_CC_ZERO;
        tev->c_in[idx].type = HSD_TE_ZERO;
        tev->c_in[idx].exp = NULL;
        break;
    case HSD_TE_1:
        tev->c_in[idx].arg = GX_CC_ONE;
        tev->c_in[idx].type = HSD_TE_IMM;
        tev->c_in[idx].exp = NULL;
        break;
    case HSD_TE_4_8:
        tev->c_in[idx].arg = GX_CC_HALF;
        tev->c_in[idx].type = HSD_TE_IMM;
        tev->c_in[idx].exp = NULL;
        break;
    case HSD_TE_1_8:
    case HSD_TE_2_8:
    case HSD_TE_3_8:
    case HSD_TE_5_8:
    case HSD_TE_6_8:
    case HSD_TE_7_8:
        tev->c_in[idx].arg = GX_CC_KONST;
        switch (sel) {
        case HSD_TE_1_8:
            ksel = GX_TEV_KCSEL_1_8;
            break;
        case HSD_TE_2_8:
            ksel = GX_TEV_KCSEL_1_4;
            break;
        case HSD_TE_3_8:
            ksel = GX_TEV_KCSEL_3_8;
            break;
        case HSD_TE_5_8:
            ksel = GX_TEV_KCSEL_5_8;
            break;
        case HSD_TE_6_8:
            ksel = GX_TEV_KCSEL_3_4;
            break;
        case HSD_TE_7_8:
            ksel = GX_TEV_KCSEL_7_8;
            break;
        default:
            HSD_ASSERT(531, 0);
            break;
        }

        if (tev->kcsel == HSD_TE_UNDEF) {
            tev->kcsel = ksel;
        } else if (tev->kcsel == ksel) {
            HSD_Panic(__FILE__, 536,
                      "tev can't select multiple konst input.\n");
        }
        tev->c_in[idx].type = HSD_TE_KONST;
        break;

    default: {
        switch (tev->c_in[idx].type) {
        case HSD_TE_ZERO:
            tev->c_in[idx].type = HSD_TE_ZERO;
            tev->c_in[idx].sel = HSD_TE_0;
            tev->c_in[idx].arg = GX_CC_ZERO;
            break;
        case HSD_TE_TEV:
            HSD_ASSERT(552, sel == HSD_TE_RGB || sel == HSD_TE_A);
            HSD_ASSERT(554, idx == 3 || sel != HSD_TE_RGB || exp->tev.c_clamp);
            HSD_ASSERT(555, idx == 3 || sel != HSD_TE_A || exp->tev.a_clamp);
            HSD_TExpRef(tev->c_in[idx].exp, tev->c_in[idx].sel);
            break;
        case HSD_TE_CNST:
            tev->c_in[idx].sel = exp->cnst.comp;
            HSD_TExpRef(tev->c_in[idx].exp, tev->c_in[idx].sel);
            break;
        case HSD_TE_TEX: {
            u8 swap = HSD_TE_UNDEF;
            switch (sel) {
            case HSD_TE_RGB:
                tev->c_in[idx].arg = GX_CC_TEXC;
                swap = GX_TEV_SWAP0;
                break;
            case HSD_TE_R:
                tev->c_in[idx].arg = GX_CC_TEXC;
                swap = GX_TEV_SWAP1;
                break;
            case HSD_TE_G:
                tev->c_in[idx].arg = GX_CC_TEXC;
                swap = GX_TEV_SWAP2;
                break;
            case HSD_TE_B:
                tev->c_in[idx].arg = GX_CC_TEXC;
                swap = GX_TEV_SWAP3;
                break;
            case HSD_TE_A:
                tev->c_in[idx].arg = GX_CC_TEXA;
                break;
            default:
                HSD_ASSERT(590, 0);
                break;
            }
            if (tev->tex_swap == HSD_TE_UNDEF) {
                tev->tex_swap = swap;
            } else {
                HSD_ASSERT(595, swap == HSD_TE_UNDEF || tev->tex_swap == swap);
            }
        } break;
        case HSD_TE_RAS: {
            u8 swap = HSD_TE_UNDEF;
            switch (sel) {
            case HSD_TE_RGB:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = GX_TEV_SWAP0;
                break;
            case HSD_TE_R:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = GX_TEV_SWAP1;
                break;
            case HSD_TE_G:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = GX_TEV_SWAP2;
                break;
            case HSD_TE_B:
                tev->c_in[idx].arg = GX_CC_RASC;
                swap = GX_TEV_SWAP3;
                break;
            case HSD_TE_A:
                tev->c_in[idx].arg = GX_CC_RASA;
                break;
            default:
                HSD_ASSERT(638, 0);
                break;
            }
            if (tev->ras_swap == HSD_TE_UNDEF) {
                tev->ras_swap = swap;
            } else {
                HSD_ASSERT(643, swap == HSD_TE_UNDEF || tev->ras_swap == swap);
            }
        } break;
        default:
            HSD_ASSERT(661, 0);
            break;
        }
    }
    }
    HSD_TExpUnref(prev.exp, prev.sel);
}

/* HSD_TExpColorIn */
void HSD_TExpColorIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a,
                 HSD_TEInput sel_b, HSD_TExp* exp_b, HSD_TEInput sel_c,
                 HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    HSD_TETev* tev;

    HSD_ASSERT(724, texp);
    HSD_ASSERT(725, HSD_TExpGetType(texp) == HSD_TE_TEV);

    tev = &texp->tev;
    HSD_TExpColorInSub(tev, sel_a, exp_a, 0);
    HSD_TExpColorInSub(tev, sel_b, exp_b, 1);
    HSD_TExpColorInSub(tev, sel_c, exp_c, 2);
    HSD_TExpColorInSub(tev, sel_d, exp_d, 3);
}

/* HSD_TExpAlphaInSub */
static void HSD_TExpAlphaInSub(HSD_TETev* tev, HSD_TEInput sel, HSD_TExp* exp,
                        s32 idx)
{
    HSD_TEArg prev;
    u8 ksel;

    prev = tev->a_in[idx];
    tev->a_in[idx].sel = sel;
    tev->a_in[idx].exp = exp;
    tev->a_in[idx].type = HSD_TExpGetType(exp);
    tev->a_in[idx].arg = 0xFF;

    switch (sel) {
    case HSD_TE_0:
        tev->a_in[idx].type = HSD_TE_ZERO;
        tev->a_in[idx].arg = GX_CA_ZERO;
        tev->a_in[idx].exp = NULL;
        break;

    case HSD_TE_1:
    case HSD_TE_1_8:
    case HSD_TE_2_8:
    case HSD_TE_3_8:
    case HSD_TE_4_8:
    case HSD_TE_5_8:
    case HSD_TE_6_8:
    case HSD_TE_7_8:
        tev->a_in[idx].exp = NULL;
        tev->a_in[idx].arg = GX_CA_KONST;
        switch (sel) {
        case HSD_TE_1_8:
            ksel = GX_TEV_KASEL_1_8;
            break;
        case HSD_TE_2_8:
            ksel = GX_TEV_KASEL_1_4;
            break;
        case HSD_TE_3_8:
            ksel = GX_TEV_KASEL_3_8;
            break;
        case HSD_TE_4_8:
            ksel = GX_TEV_KASEL_1_2;
            break;
        case HSD_TE_5_8:
            ksel = GX_TEV_KASEL_5_8;
            break;
        case HSD_TE_6_8:
            ksel = GX_TEV_KASEL_3_4;
            break;
        case HSD_TE_7_8:
            ksel = GX_TEV_KASEL_7_8;
            break;
        default:
            ksel = GX_TEV_KASEL_1;
            break;
        }

        if (tev->kasel == HSD_TE_UNDEF) {
            tev->kasel = ksel;
        } else {
            HSD_ASSERT(781, tev->kasel == ksel);
        }
        tev->a_in[idx].type = HSD_TE_KONST;
        break;

    default: {
        switch (tev->a_in[idx].type) {
        case HSD_TE_ZERO:
            tev->a_in[idx].exp = NULL;
            tev->a_in[idx].type = HSD_TE_ZERO;
            tev->a_in[idx].sel = HSD_TE_0;
            tev->a_in[idx].arg = GX_CA_ZERO;
            break;
        case HSD_TE_TEV:
            HSD_ASSERT(804, sel == HSD_TE_A);
            HSD_ASSERT(805, idx == 3 || exp->tev.a_clamp);
            HSD_TExpRef(tev->a_in[idx].exp, tev->a_in[idx].sel);
            break;
        case HSD_TE_CNST:
            HSD_ASSERT(809, sel == HSD_TE_A || sel == HSD_TE_X);
            HSD_ASSERT(810, exp->cnst.comp == HSD_TE_X);
            tev->a_in[idx].sel = HSD_TE_X;
            HSD_TExpRef(tev->a_in[idx].exp, tev->a_in[idx].sel);
            break;
        case HSD_TE_TEX:
            HSD_ASSERT(815, sel == HSD_TE_A);
            tev->a_in[idx].arg = GX_CA_TEXA;
            break;
        case HSD_TE_RAS:
            HSD_ASSERT(819, sel == HSD_TE_A);
            tev->a_in[idx].arg = GX_CA_RASA;
            break;
        default:
            HSD_ASSERT(823, 0);
            break;
        }
    }
    }
    HSD_TExpUnref(prev.exp, prev.sel);
}

/* HSD_TExpAlphaIn */
void HSD_TExpAlphaIn(HSD_TExp* texp, HSD_TEInput sel_a, HSD_TExp* exp_a,
                 HSD_TEInput sel_b, HSD_TExp* exp_b, HSD_TEInput sel_c,
                 HSD_TExp* exp_c, HSD_TEInput sel_d, HSD_TExp* exp_d)
{
    HSD_TETev* tev;

    HSD_ASSERT(854, texp);
    HSD_ASSERT(855, HSD_TExpGetType(texp) == HSD_TE_TEV);

    tev = &texp->tev;
    HSD_TExpAlphaInSub(tev, sel_a, exp_a, 0);
    HSD_TExpAlphaInSub(tev, sel_b, exp_b, 1);
    HSD_TExpAlphaInSub(tev, sel_c, exp_c, 2);
    HSD_TExpAlphaInSub(tev, sel_d, exp_d, 3);
}

/* HSD_TExpOrder */
void HSD_TExpOrder(HSD_TExp* texp, HSD_TObj* tex, GXChannelID chan)
{
    HSD_ASSERT(870, texp);
    HSD_ASSERT(871, HSD_TExpGetType(texp) == HSD_TE_TEV);

    texp->tev.tex = tex;
    if (chan == GX_COLOR_NULL) {
        texp->tev.chan = GX_COLOR_NULL;
        return;
    }
    texp->tev.chan = chan;
}

/* AssignColorReg */
static int AssignColorReg(HSD_TETev* tev, int idx, HSD_TExpRes* res)
{
    static GXTevColorArg a_in[4] = { GX_CC_A0, GX_CC_A1, GX_CC_A2,
                                     GX_CC_APREV };
    static GXTevColorArg c_in[4] = { GX_CC_C0, GX_CC_C1, GX_CC_C2,
                                     GX_CC_CPREV };

    HSD_TECnst* cnst;
    int j;

    cnst = &tev->c_in[idx].exp->cnst;
    if (cnst->reg != HSD_TE_UNDEF) {
        if (cnst->reg < 4) {
            return -1;
        }
        tev->c_in[idx].type = HSD_TE_IMM;
        if (cnst->comp == HSD_TE_X) {
            tev->c_in[idx].arg = a_in[cnst->reg - 4];
        } else {
            tev->c_in[idx].arg = c_in[cnst->reg - 4];
        }
        return 0;
    } else {
        if (cnst->comp == HSD_TE_X) {
            for (j = 4; j < 8; ++j) {
                if (res->reg[j].alpha == 0) {
                    res->reg[j].alpha = 1;
                    cnst->reg = j;
                    cnst->idx = 3;
                    tev->c_in[idx].type = HSD_TE_IMM;
                    tev->c_in[idx].arg = a_in[j - 4];
                    return 0;
                }
            }
        } else {
            for (j = 4; j < 8; ++j) {
                if (res->reg[j].color == 0) {
                    res->reg[j].color = 3;
                    cnst->reg = j;
                    cnst->idx = 0;
                    tev->c_in[idx].type = HSD_TE_IMM;
                    tev->c_in[idx].arg = c_in[j - 4];
                    return 0;
                }
            }
        }
    }
    return -1;
}

/* Expanded at its five call sites in TExpAssignReg; retail has no symbol. */
static inline int AssignAlphaReg(HSD_TETev* tev, int idx, HSD_TExpRes* res)
{
    static GXTevAlphaArg in[4] = { GX_CA_A0, GX_CA_A1, GX_CA_A2, GX_CA_APREV };

    HSD_TECnst* cnst;
    int j;

    cnst = &tev->a_in[idx].exp->cnst;
    if (cnst->reg != HSD_TE_UNDEF) {
        if (cnst->reg < 4) {
            return -1;
        }
        tev->a_in[idx].type = HSD_TE_IMM;
        tev->a_in[idx].arg = in[cnst->reg - 4];
        return 0;
    } else {
        for (j = 4; j < 8; ++j) {
            if (res->reg[j].alpha == 0) {
                res->reg[j].alpha = 1;
                cnst->reg = j;
                cnst->idx = 3;
                tev->a_in[idx].type = HSD_TE_IMM;
                tev->a_in[idx].arg = in[j - 4];
                return 0;
            }
        }
    }
    return -1;
}

/* AssignColorKonst */
static int AssignColorKonst(HSD_TETev* tev, int idx, HSD_TExpRes* res)
{
    static GXTevKColorSel xsel[4][4] = {
        { GX_TEV_KCSEL_K0_R, GX_TEV_KCSEL_K0_G, GX_TEV_KCSEL_K0_B,
          GX_TEV_KCSEL_K0_A },
        { GX_TEV_KCSEL_K1_R, GX_TEV_KCSEL_K1_G, GX_TEV_KCSEL_K1_B,
          GX_TEV_KCSEL_K1_A },
        { GX_TEV_KCSEL_K2_R, GX_TEV_KCSEL_K2_G, GX_TEV_KCSEL_K2_B,
          GX_TEV_KCSEL_K2_A },
        { GX_TEV_KCSEL_K3_R, GX_TEV_KCSEL_K3_G, GX_TEV_KCSEL_K3_B,
          GX_TEV_KCSEL_K3_A }
    };

    static GXTevKColorSel csel[4] = { GX_TEV_KCSEL_K0, GX_TEV_KCSEL_K1,
                                      GX_TEV_KCSEL_K2, GX_TEV_KCSEL_K3 };

    HSD_TECnst* cnst;
    int j;

    cnst = &tev->c_in[idx].exp->cnst;
    if (cnst->reg != HSD_TE_UNDEF) {
        if (cnst->reg >= 4) {
            return -1;
        }
        if (cnst->comp == HSD_TE_X) {
            tev->kcsel = xsel[cnst->reg][cnst->idx];
            tev->c_in[idx].type = HSD_TE_KONST;
            tev->c_in[idx].arg = GX_CC_KONST;
        } else {
            tev->kcsel = csel[cnst->reg];
            tev->c_in[idx].type = HSD_TE_KONST;
            tev->c_in[idx].arg = GX_CC_KONST;
        }
        return 0;
    } else {
        if (cnst->comp == HSD_TE_X) {
            for (j = 1; j < 4; j++) {
                if (res->reg[j].alpha == 0) {
                    res->reg[j].alpha = 1;
                    cnst->reg = j;
                    cnst->idx = 3;
                    tev->kcsel = xsel[cnst->reg][cnst->idx];
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = GX_CC_KONST;
                    return 0;
                }
            }

            for (j = 0; j < 4; j++) {
                if (res->reg[j].color < 3) {
                    cnst->reg = j;
                    cnst->idx = res->reg[j].color++;
                    tev->kcsel = xsel[cnst->reg][cnst->idx];
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = GX_CC_KONST;
                    return 0;
                }
            }
        } else {
            for (j = 0; j < 4; j++) {
                if (res->reg[j].color == 0) {
                    res->reg[j].color = 3;
                    cnst->reg = j;
                    cnst->idx = 0;
                    tev->kcsel = csel[cnst->reg];
                    tev->c_in[idx].type = HSD_TE_KONST;
                    tev->c_in[idx].arg = GX_CC_KONST;
                    return 0;
                }
            }
        }
    }
    return -1;
}

/* AssignAlphaKonst */
static int AssignAlphaKonst(HSD_TETev* tev, int idx, HSD_TExpRes* res)
{
    static GXTevKAlphaSel sel[4][4] = {
        { GX_TEV_KASEL_K0_R, GX_TEV_KASEL_K0_G, GX_TEV_KASEL_K0_B,
          GX_TEV_KASEL_K0_A },
        { GX_TEV_KASEL_K1_R, GX_TEV_KASEL_K1_G, GX_TEV_KASEL_K1_B,
          GX_TEV_KASEL_K1_A },
        { GX_TEV_KASEL_K2_R, GX_TEV_KASEL_K2_G, GX_TEV_KASEL_K2_B,
          GX_TEV_KASEL_K2_A },
        { GX_TEV_KASEL_K3_R, GX_TEV_KASEL_K3_G, GX_TEV_KASEL_K3_B,
          GX_TEV_KASEL_K3_A }
    };

    HSD_TECnst* cnst;
    int j;

    cnst = &tev->a_in[idx].exp->cnst;
    if (cnst->reg != HSD_TE_UNDEF) {
        if (cnst->reg >= 4) {
            return -1;
        }
        tev->kasel = sel[cnst->reg][cnst->idx];
        tev->a_in[idx].type = HSD_TE_KONST;
        tev->a_in[idx].arg = GX_CA_KONST;
        return 0;
    } else {
        for (j = 1; j < 4; j++) {
            if (res->reg[j].alpha == 0) {
                res->reg[j].alpha = 1;
                cnst->reg = j;
                cnst->idx = 3;
                tev->kasel = sel[cnst->reg][cnst->idx];
                tev->a_in[idx].type = HSD_TE_KONST;
                tev->a_in[idx].arg = GX_CA_KONST;
                return 0;
            }
        }

        for (j = 0; j < 4; j++) {
            if (res->reg[j].color < 3) {
                cnst->reg = j;
                cnst->idx = res->reg[j].color++;
                tev->kasel = sel[cnst->reg][cnst->idx];
                tev->a_in[idx].type = HSD_TE_KONST;
                tev->a_in[idx].arg = GX_CA_KONST;
                return 0;
            }
        }
    }
    return -1;
}

/* TExpAssignReg */
static int TExpAssignReg(HSD_TExp* texp, HSD_TExpRes* res)
{
    HSD_TETev* tev = (HSD_TETev*) texp;
    int i, val;

    if (tev->c_ref > 0) {
        if (tev->kcsel != HSD_TE_UNDEF) {
            for (i = 0; i < 4; i++) {
                if (tev->c_in[i].type == HSD_TE_CNST) {
                    val = AssignColorReg(tev, i, res);
                    if (val < 0) {
                        HSD_ASSERT(1180, val >= 0);
                        return val;
                    }
                }
            }
        } else if (IsThroughColor((HSD_TExp*) tev) &&
                   tev->c_in[3].type == HSD_TE_CNST)
        {
            val = AssignColorReg(tev, 3, res);
            if (val < 0) {
                val = AssignColorKonst(tev, 3, res);
                if (val < 0) {
                    HSD_ASSERT(1192, val >= 0);
                    return val;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (tev->c_in[i].type == HSD_TE_CNST) {
                    val = AssignColorKonst(tev, i, res);
                    if (val < 0) {
                        val = AssignColorReg(tev, i, res);
                        if (val < 0) {
                            HSD_ASSERT(1205, val >= 0);
                            return val;
                        }
                    }
                }
            }
            for (; i < 4; i++) {
                if (tev->c_in[i].type == HSD_TE_CNST) {
                    val = AssignColorReg(tev, i, res);
                    if (val < 0) {
                        HSD_ASSERT(1214, val >= 0);
                        return val;
                    }
                }
            }
        }
    }

    if (tev->a_ref > 0) {
        if (tev->kasel != HSD_TE_UNDEF) {
            for (i = 0; i < 4; i++) {
                if (tev->a_in[i].type == HSD_TE_CNST) {
                    val = AssignAlphaReg(tev, i, res);
                    if (val < 0) {
                        HSD_ASSERT(1230, val >= 0);
                        return val;
                    }
                }
            }
        } else {
            if (IsThroughAlpha((HSD_TExp*) tev) &&
                tev->a_in[3].type == HSD_TE_CNST)
            {
                val = AssignAlphaReg(tev, 3, res);
                if (val < 0) {
                    val = AssignAlphaKonst(tev, 3, res);
                    if (val < 0) {
                        HSD_ASSERT(1242, val >= 0);
                        return val;
                    }
                }
            } else {
                for (i = 0; i < 4; i++) {
                    if (tev->a_in[i].type == HSD_TE_CNST) {
                        val = AssignAlphaKonst(tev, i, res);
                        if (val < 0) {
                            val = AssignAlphaReg(tev, i, res);
                            if (val < 0) {
                                HSD_ASSERT(1255, val >= 0);
                                return val;
                            }
                        }
                    }
                }
                for (; i < 4; i++) {
                    if (tev->a_in[i].type == HSD_TE_CNST) {
                        val = AssignAlphaReg(tev, i, res);
                        if (val < 0) {
                            HSD_ASSERT(1264, val >= 0);
                            return val;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

static void TExp2TevDesc(HSD_TExp* texp, HSD_TExpTevDesc* desc,
                         int* init_cprev, int* init_aprev)
{
    static GXTevRegID dst[4] = { GX_TEVREG0, GX_TEVREG1, GX_TEVREG2,
                                 GX_TEVPREV };

    HSD_TETev* tev;
    HSD_TevDesc* tevdesc;

    HSD_ASSERT(1320, texp);
    HSD_ASSERT(1321, desc);
    HSD_ASSERT(1322, HSD_TExpGetType(texp) == HSD_TE_TEV);

    tev = &texp->tev;
    tevdesc = &desc->desc;

    tevdesc->next = NULL;
    tevdesc->flags = 1;
    desc->tobj = tev->tex;
    if (tev->tex == NULL) {
        tevdesc->coord = HSD_TE_UNDEF;
        tevdesc->map = HSD_TE_UNDEF;
    }
    tevdesc->color = tev->chan == HSD_TE_UNDEF ? HSD_TE_UNDEF : tev->chan;
    tevdesc->u.tevconf.ras_swap =
        tev->ras_swap == HSD_TE_UNDEF ? 0 : tev->ras_swap;
    tevdesc->u.tevconf.tex_swap =
        tev->tex_swap == HSD_TE_UNDEF ? 0 : tev->tex_swap;
    tevdesc->u.tevconf.kcsel = tev->kcsel == HSD_TE_UNDEF ? 0 : tev->kcsel;
    tevdesc->u.tevconf.kasel = tev->kasel == HSD_TE_UNDEF ? 0 : tev->kasel;
    tevdesc->u.tevconf.swap_red =
        tev->swap_red == HSD_TE_UNDEF ? GX_CH_RED : tev->swap_red;
    tevdesc->u.tevconf.swap_green =
        tev->swap_green == HSD_TE_UNDEF ? GX_CH_GREEN : tev->swap_green;
    tevdesc->u.tevconf.swap_blue =
        tev->swap_blue == HSD_TE_UNDEF ? GX_CH_BLUE : tev->swap_blue;
    tevdesc->u.tevconf.swap_alpha =
        tev->swap_alpha == HSD_TE_UNDEF ? GX_CH_ALPHA : tev->swap_alpha;

    if ((tev->c_op == HSD_TE_UNDEF) ||
        (tev->c_ref == 0 && tev->a_op != 8 && tev->a_op != 9 &&
         tev->a_op != 0xA && tev->a_op != 0xB && tev->a_op != 0xC &&
         tev->a_op != 0xD))
    {
        tevdesc->u.tevconf.clr_op = GX_TEV_ADD;
        tevdesc->u.tevconf.clr_a = GX_CC_ZERO;
        tevdesc->u.tevconf.clr_b = GX_CC_ZERO;
        tevdesc->u.tevconf.clr_c = GX_CC_ZERO;
        if (*init_cprev != 0) {
            *init_cprev = 0;
            tevdesc->u.tevconf.clr_d = GX_CC_ZERO;
        } else {
            tevdesc->u.tevconf.clr_d = GX_CC_CPREV;
        }
        tevdesc->u.tevconf.clr_scale = 0;
        tevdesc->u.tevconf.clr_bias = 0;
        tevdesc->u.tevconf.clr_clamp = 0;
        tevdesc->u.tevconf.clr_out_reg = 0;
    } else {
        tevdesc->u.tevconf.clr_op = tev->c_op;
        tevdesc->u.tevconf.clr_a =
            tev->c_in[0].arg == HSD_TE_UNDEF ? GX_CC_ZERO : tev->c_in[0].arg;
        tevdesc->u.tevconf.clr_b =
            tev->c_in[1].arg == HSD_TE_UNDEF ? GX_CC_ZERO : tev->c_in[1].arg;
        tevdesc->u.tevconf.clr_c =
            tev->c_in[2].arg == HSD_TE_UNDEF ? GX_CC_ZERO : tev->c_in[2].arg;
        tevdesc->u.tevconf.clr_d =
            tev->c_in[3].arg == HSD_TE_UNDEF ? GX_CC_ZERO : tev->c_in[3].arg;
        tevdesc->u.tevconf.clr_scale =
            tev->c_scale == HSD_TE_UNDEF ? 0 : tev->c_scale;
        tevdesc->u.tevconf.clr_bias =
            tev->c_bias == HSD_TE_UNDEF ? 0 : tev->c_bias;
        tevdesc->u.tevconf.clr_clamp = tev->c_clamp != 0 ? GX_TRUE : GX_FALSE;

        HSD_ASSERT(1393, tev->c_dst != HSD_TE_UNDEF);
        tevdesc->u.tevconf.clr_out_reg = dst[tev->c_dst];
        if (tevdesc->u.tevconf.clr_out_reg == 0) {
            *init_cprev = 0;
        }
    }

    if (tev->a_op == HSD_TE_UNDEF || tev->a_ref == 0) {
        tevdesc->u.tevconf.alpha_op = GX_TEV_ADD;
        tevdesc->u.tevconf.alpha_a = GX_CA_ZERO;
        tevdesc->u.tevconf.alpha_b = GX_CA_ZERO;
        tevdesc->u.tevconf.alpha_c = GX_CA_ZERO;
        if (*init_aprev != 0) {
            *init_aprev = 0;
            tevdesc->u.tevconf.alpha_d = GX_CA_ZERO;
        } else {
            tevdesc->u.tevconf.alpha_d = GX_CA_APREV;
        }
        tevdesc->u.tevconf.alpha_scale = 0;
        tevdesc->u.tevconf.alpha_bias = 0;
        tevdesc->u.tevconf.alpha_clamp = 0;
        tevdesc->u.tevconf.alpha_out_reg = 0;
    } else {
        tevdesc->u.tevconf.alpha_op = tev->a_op;
        tevdesc->u.tevconf.alpha_a =
            tev->a_in[0].arg == HSD_TE_UNDEF ? GX_CA_ZERO : tev->a_in[0].arg;
        tevdesc->u.tevconf.alpha_b =
            tev->a_in[1].arg == HSD_TE_UNDEF ? GX_CA_ZERO : tev->a_in[1].arg;
        tevdesc->u.tevconf.alpha_c =
            tev->a_in[2].arg == HSD_TE_UNDEF ? GX_CA_ZERO : tev->a_in[2].arg;
        tevdesc->u.tevconf.alpha_d =
            tev->a_in[3].arg == HSD_TE_UNDEF ? GX_CA_ZERO : tev->a_in[3].arg;
        tevdesc->u.tevconf.alpha_scale =
            tev->a_scale == HSD_TE_UNDEF ? 0 : tev->a_scale;
        tevdesc->u.tevconf.alpha_bias =
            tev->a_bias == HSD_TE_UNDEF ? 0 : tev->a_bias;
        tevdesc->u.tevconf.alpha_clamp =
            tev->a_clamp != 0 ? GX_TRUE : GX_FALSE;

        HSD_ASSERT(1429, tev->a_dst != HSD_TE_UNDEF);
        tevdesc->u.tevconf.alpha_out_reg = dst[tev->a_dst];
        if (tevdesc->u.tevconf.alpha_out_reg == 0) {
            *init_aprev = 0;
        }
    }
    tevdesc->u.tevconf.mode = GX_TC_LINEAR;
}

static GXTevKColorID id1[4] = { GX_KCOLOR0, GX_KCOLOR1, GX_KCOLOR2,
                                GX_KCOLOR3 };
static GXTevRegID id2[3] = { GX_TEVREG0, GX_TEVREG1, GX_TEVREG2 };

void HSD_TExpSetReg(HSD_TExp* texp)
{
    int i;
    GXColor reg[8];
    u32 changed;
    HSD_TECnst* clist;

    clist = &texp->cnst;
    changed = 0;

    while (clist != NULL) {
        HSD_ASSERT(1453, clist->type == HSD_TE_CNST);
        if (clist->reg < 8) {
            changed |= 1 << clist->reg;
            if (clist->comp == HSD_TE_RGB) {
                switch (clist->ctype) {
                case HSD_TE_U8: {
                    GXColor col;
                    col = *(GXColor*) clist->val;
                    col.a = reg[clist->reg].a;
                    reg[clist->reg] = col;
                    break;
                }
                case HSD_TE_U32: {
                    u32* ptr = clist->val;
                    reg[clist->reg].r = ptr[0] < 0x100 ? ptr[0] : 0xFF;
                    reg[clist->reg].g = ptr[1] < 0x100 ? ptr[1] : 0xFF;
                    reg[clist->reg].b = ptr[2] < 0x100 ? ptr[2] : 0xFF;
                    break;
                }
                default: {
                    int r;
                    int g;
                    int b;
                    switch (clist->ctype) {
                    case HSD_TE_U16: {
                        u16* ptr = clist->val;
                        r = ptr[0];
                        g = ptr[1];
                        b = ptr[2];
                        break;
                    }
                    case HSD_TE_F32: {
                        f32* ptr = clist->val;
                        r = 0xFF * ptr[0];
                        g = 0xFF * ptr[1];
                        b = 0xFF * ptr[2];
                        break;
                    }
                    default: {
                        f64* ptr = clist->val;
                        r = 0xFF * ptr[0];
                        g = 0xFF * ptr[1];
                        b = 0xFF * ptr[2];
                        break;
                    }
                    }
                    reg[clist->reg].r = r > 0xFF ? 0xFF : r < 0 ? 0 : r;
                    reg[clist->reg].g = g > 0xFF ? 0xFF : g < 0 ? 0 : g;
                    reg[clist->reg].b = b > 0xFF ? 0xFF : b < 0 ? 0 : b;
                }
                }
            } else {
                int x;
                u8 val;
                switch (clist->ctype) {
                case HSD_TE_U8:
                    x = *(u8*) clist->val;
                    val = x;
                    break;
                case HSD_TE_U16: {
                    x = *(u16*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                }
                case HSD_TE_U32:
                    x = *(u32*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                case HSD_TE_F32:
                    x = 0xFF * *(f32*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                default:
                    x = 0xFF * *(f64*) clist->val;
                    val = x > 0xFF ? 0xFF : x < 0 ? 0 : x;
                    break;
                }
                if (clist->reg < 4) {
                    switch (clist->idx) {
                    case 0:
                        reg[clist->reg].r = val;
                        break;
                    case 1:
                        reg[clist->reg].g = val;
                        break;
                    case 2:
                        reg[clist->reg].b = val;
                        break;
                    default:
                        reg[clist->reg].a = val;
                        break;
                    }
                } else if (clist->idx == 3) {
                    reg[clist->reg].a = val;
                } else {
                    reg[clist->reg].r = val;
                    reg[clist->reg].g = val;
                    reg[clist->reg].b = val;
                }
            }
        }
        clist = &clist->next->cnst;
    }
    if (changed != 0) {
        GXPixModeSync();
        for (i = 0; i < 4; i++) {
            if (changed & (1 << i)) {
                GXSetTevKColor(id1[i], reg[i]);
            }
        }
        for (i = 4; i < 7; i++) {
            if (changed & (1 << i)) {
                GXSetTevColor(id2[i - 4], reg[i]);
            }
        }
        HSD_StateInvalidate(0x10);
        GXPixModeSync();
    }
}

/* HSD_TExpSetupTev */
void HSD_TExpSetupTev(HSD_TExpTevDesc* tevdesc, HSD_TExp* texp)
{
    HSD_TExpSetReg(texp);
    for (; tevdesc != NULL; tevdesc = (HSD_TExpTevDesc*) tevdesc->desc.next) {
        if (tevdesc->tobj != NULL) {
            tevdesc->desc.map = tevdesc->tobj->id;
            tevdesc->desc.coord = tevdesc->tobj->coord;
        }
        HSD_StateAssignTev();
        HSD_SetupTevStage(&tevdesc->desc);
    }
}

int HSD_TExpCompile(HSD_TExp* texp, HSD_TExpTevDesc** tevdesc,
                    HSD_TExp** texp_list)
{
    int num, i, val;
    HSD_TExpRes res;
    HSD_TExp* order[32];
    HSD_TExpDag list[32];
    int init_cprev = 1;
    int init_aprev = 1;

    HSD_ASSERT(1655, tevdesc);
    HSD_ASSERT(1656, texp_list);

    memset(&res, 0, sizeof(HSD_TExpRes));

    HSD_TExpRef(texp, HSD_TE_RGB);
    HSD_TExpRef(texp, HSD_TE_A);
    HSD_TExpSimplify(texp);

    num = HSD_TExpMakeDag(texp, list);
    HSD_TExpSchedule(num, list, order, &res);
    for (i = 0; i < num; ++i) {
        val = TExpAssignReg(order[i], &res);
        HSD_ASSERT(1688, val >= 0);
    }

    for (i = num - 1; i >= 0; --i) {
        HSD_TExpSimplify2(order[i]);
    }

    num = HSD_TExpMakeDag(texp, list);
    HSD_TExpSchedule(num, list, order, &res);
    *tevdesc = NULL;
    for (i = 0; i < num; ++i) {
        HSD_TExpTevDesc* tdesc = fn_80193B10(sizeof(HSD_TExpTevDesc));
        tdesc->desc.stage = HSD_Index2TevStage(i);
        TExp2TevDesc(order[(num - i) - 1], tdesc, &init_cprev, &init_aprev);
        tdesc->desc.next = &(*tevdesc)->desc;
        *tevdesc = tdesc;
    }

    *texp_list = HSD_TExpFreeList(*texp_list, HSD_TE_TEV, 1);
    *texp_list = HSD_TExpFreeList(*texp_list, HSD_TE_CNST, 0);
    return num;
}

void HSD_TExpFreeTevDesc(HSD_TExpTevDesc* tdesc)
{
    HSD_TExpTevDesc* next = tdesc;
    while (next != NULL) {
        HSD_TExpTevDesc* temp = (HSD_TExpTevDesc*) next->desc.next;
        fn_80193AF0(next, sizeof(HSD_TExpTevDesc));
        next = temp;
    }
}
