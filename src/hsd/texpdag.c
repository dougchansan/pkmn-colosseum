/**
 * @file texpdag.c
 * @brief HSD TExp - expression simplification and TEV stage scheduling.
 *
 * Retail TU: .text 0x801B7CA0 - 0x801BBAC8, with its .rodata string pool
 * (0x802755F0), .data tables and jump table (0x8036D380), .sdata
 * (0x80478CA0), .sdata2 (0x8047DEA8) and .sbss2 (0x8047E730).
 *
 * HAL sysdolphin (>= 1.3.0.0) texpdag.c, reconstructed with the Melee
 * decompilation (doldecomp/melee, src/sysdolphin/baselib/texpdag.c) as
 * reference. The library is built with -inline auto,deferred, so functions
 * are emitted in reverse source order; in Colosseum's version the
 * simplification passes precede the DAG scheduler in the file.
 */
#include "hsd/hsd_texp.h"

#include "crt/string.h"
#include "hsd/hsd_debug.h"

/* An argument slot reset to constant zero. */
static HSD_TEArg zero_arg = { HSD_TE_ZERO, HSD_TE_0, 0xFF };

/* SimplifySrc */
int SimplifySrc(HSD_TETev* tev)
{
    BOOL result;
    int i;

    result = FALSE;
    for (i = 0; i < 4; i++) {
        if (tev->c_in[i].type == HSD_TE_TEV) {
            HSD_TExp* src = tev->c_in[i].exp;
            u8 sel = tev->c_in[i].sel;
            if (HSD_TExpSimplify(src) != 0) {
                result = TRUE;
            }
            if (sel == HSD_TE_RGB) {
                switch (src->tev.c_op) {
                case 0xFF:
                    HSD_TExpUnref(src, sel);
                    result = TRUE;
                    tev->c_in[i] = zero_arg;
                    break;
                case GX_TEV_ADD:
                    if (src->tev.c_in[0].sel == HSD_TE_0 &&
                        src->tev.c_in[1].sel == HSD_TE_0 &&
                        src->tev.c_bias == 0 && src->tev.c_scale == 0)
                    {
                        switch (src->tev.c_in[3].type) {
                        case HSD_TE_IMM:
                            tev->c_in[i] = src->tev.c_in[3];
                            HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                            HSD_TExpUnref(src, sel);
                            result = TRUE;
                            break;
                        case HSD_TE_TEV:
                            if (i != 3 || src->tev.c_clamp == 0 ||
                                src->tev.c_range != 1)
                            {
                                tev->c_in[i] = src->tev.c_in[3];
                                HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                                HSD_TExpUnref(src, sel);
                                result = TRUE;
                            }
                            break;
                        case HSD_TE_TEX:
                            if ((tev->tex == NULL ||
                                 tev->tex == src->tev.tex) &&
                                (tev->tex_swap == 0xFF ||
                                 src->tev.tex_swap == 0xFF ||
                                 tev->tex_swap == src->tev.tex_swap))
                            {
                                tev->c_in[i] = src->tev.c_in[3];
                                tev->tex = src->tev.tex;
                                if (tev->tex_swap == 0xFF) {
                                    tev->tex_swap = src->tev.tex_swap;
                                }
                                HSD_TExpUnref(src, sel);
                                result = TRUE;
                            }
                            break;
                        case HSD_TE_RAS:
                            if ((tev->chan == 0xFF ||
                                 tev->chan == src->tev.chan) &&
                                (tev->ras_swap == 0xFF ||
                                 src->tev.ras_swap == 0xFF ||
                                 tev->ras_swap == src->tev.ras_swap))
                            {
                                tev->c_in[i] = src->tev.c_in[3];
                                tev->chan = src->tev.chan;
                                if (tev->ras_swap == 0xFF) {
                                    tev->ras_swap = src->tev.ras_swap;
                                }
                                HSD_TExpUnref(src, sel);
                                result = TRUE;
                            }
                            break;
                        }
                    }
                    break;
                }
            } else {
                switch (src->tev.a_op) {
                case GX_TEV_ADD:
                    break;
                case 0xFF:
                    HSD_TExpUnref(src, sel);
                    result = TRUE;
                    tev->c_in[i] = zero_arg;
                    break;
                }
            }
        }
    }

    for (i = 0; i < 4; i++) {
        if (tev->a_in[i].type == HSD_TE_TEV) {
            HSD_TExp* src = tev->a_in[i].exp;
            u8 sel = tev->a_in[i].sel;
            HSD_TExpSimplify(src);
            switch (src->tev.a_op) {
            case 0xFF:
                HSD_TExpUnref(src, sel);
                result = TRUE;
                tev->a_in[i] = zero_arg;
                break;
            case GX_TEV_ADD:
                if (src->tev.a_in[0].sel == HSD_TE_0 &&
                    src->tev.a_in[1].sel == HSD_TE_0 && src->tev.a_bias == 0 &&
                    src->tev.a_scale == 0)
                {
                    switch (src->tev.a_in[3].type) {
                    case HSD_TE_IMM:
                        tev->a_in[i] = src->tev.a_in[3];
                        HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                        HSD_TExpUnref(src, sel);
                        result = TRUE;
                        /* The retail code has no break here. */
                    case HSD_TE_TEV:
                        if (i != 3 || src->tev.a_clamp == 0 ||
                            src->tev.a_range != 1)
                        {
                            tev->a_in[i] = src->tev.a_in[3];
                            HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                            HSD_TExpUnref(src, sel);
                            result = TRUE;
                        }
                        break;
                    case HSD_TE_TEX:
                        if (tev->tex == NULL ||
                            tev->tex == src->tev.tex)
                        {
                            tev->a_in[i] = src->tev.a_in[3];
                            tev->tex = src->tev.tex;
                            HSD_TExpUnref(src, sel);
                            result = TRUE;
                        }
                        break;
                    case HSD_TE_RAS:
                        if (tev->chan == 0xFF ||
                            tev->chan == src->tev.chan)
                        {
                            tev->a_in[i] = src->tev.a_in[3];
                            tev->chan = src->tev.chan;
                            HSD_TExpUnref(src, sel);
                            result = TRUE;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }
    return result;
}

/* SimplifyThis */
int SimplifyThis(HSD_TETev* tev)
{
    int color_tex;
    int alpha_tex;
    int color_ras;
    int alpha_ras;
    int result;
    int changed;
    int i;

    result = 0;
    do {
        color_tex = -1;
        alpha_tex = -1;
        color_ras = -1;
        alpha_ras = -1;

        for (i = 0; i < 4; i++) {
            switch (tev->c_in[i].type) {
            case HSD_TE_TEX:
                color_tex = i;
                break;
            case HSD_TE_RAS:
                color_ras = i;
                break;
            }
            switch (tev->a_in[i].type) {
            case HSD_TE_TEX:
                alpha_tex = i;
                break;
            case HSD_TE_RAS:
                alpha_ras = i;
                break;
            }
        }

        if (color_tex == -1 && alpha_tex == -1) {
            tev->tex = NULL;
            tev->tex_swap = 0xFF;
        }
        if (color_ras == -1 && alpha_ras == -1) {
            tev->chan = 0xFF;
            tev->ras_swap = 0xFF;
        }

        changed = 0;
        if (tev->a_op == 0xFF || tev->a_op == 0xE || tev->a_op == 0xF ||
            tev->a_op == GX_TEV_ADD || tev->a_op == GX_TEV_SUB)
        {
            if (tev->c_op != 0xFF && tev->c_ref == 0) {
                tev->c_op = 0xFF;
                for (i = 0; i < 4; i++) {
                    HSD_TExpUnref(tev->c_in[i].exp, tev->c_in[i].sel);
                    tev->c_in[i] = zero_arg;
                }
                changed = 1;
            }

            switch (tev->c_op) {
            case 0:
            case 1:
                if (tev->c_in[2].sel == HSD_TE_0) {
                    if (tev->c_in[1].sel != HSD_TE_0) {
                        HSD_TExpUnref(tev->c_in[1].exp, tev->c_in[1].sel);
                        changed = 1;
                        tev->c_in[1] = zero_arg;
                    }
                    if (tev->c_op == 0 &&
                        tev->c_in[3].sel == HSD_TE_0)
                    {
                        changed = 1;
                        tev->c_in[3] = tev->c_in[0];
                        tev->c_in[0] = zero_arg;
                        tev->c_clamp = 1;
                    }
                }
                if (tev->c_in[2].sel == HSD_TE_1) {
                    if (tev->c_in[0].sel != HSD_TE_0) {
                        HSD_TExpUnref(tev->c_in[0].exp, tev->c_in[0].sel);
                        changed = 1;
                        tev->c_in[0] = zero_arg;
                    }
                    if (tev->c_op == 0 &&
                        tev->c_in[3].sel == HSD_TE_0)
                    {
                        changed = 1;
                        tev->c_in[3] = tev->c_in[1];
                        tev->c_in[1] = zero_arg;
                        tev->c_in[2] = zero_arg;
                    }
                }
                if (tev->c_in[0].sel == HSD_TE_0 &&
                    tev->c_in[1].sel == HSD_TE_1)
                {
                    changed = 1;
                    tev->c_in[0] = tev->c_in[2];
                    tev->c_in[1] = zero_arg;
                    tev->c_in[2] = zero_arg;
                }
                if (tev->c_in[0].sel == HSD_TE_0 &&
                    tev->c_in[1].sel == HSD_TE_0 &&
                    tev->c_in[3].sel == HSD_TE_0 && tev->c_bias == 0)
                {
                    tev->c_op = 0xFF;
                    HSD_TExpUnref(tev->c_in[2].exp, tev->c_in[2].sel);
                    changed = 1;
                    tev->c_in[2] = zero_arg;
                }
                break;
            case 8:
            case 10:
            case 12:
            case 14:
                if (tev->c_in[2].sel == HSD_TE_0) {
                    tev->c_op = 0;
                    HSD_TExpUnref(tev->c_in[0].exp, tev->c_in[0].sel);
                    tev->c_in[0] = zero_arg;
                    HSD_TExpUnref(tev->c_in[1].exp, tev->c_in[1].sel);
                    changed = 1;
                    tev->c_in[1] = zero_arg;
                } else if (tev->c_in[0].sel == HSD_TE_0) {
                    tev->c_op = 0;
                    HSD_TExpUnref(tev->c_in[1].exp, tev->c_in[1].sel);
                    tev->c_in[1] = zero_arg;
                    HSD_TExpUnref(tev->c_in[2].exp, tev->c_in[2].sel);
                    changed = 1;
                    tev->c_in[2] = zero_arg;
                }
                break;
            case 9:
            case 11:
            case 13:
            case 15:
                if (tev->c_in[2].sel == HSD_TE_0) {
                    tev->c_op = 0;
                    HSD_TExpUnref(tev->c_in[0].exp, tev->c_in[0].sel);
                    tev->c_in[0] = zero_arg;
                    HSD_TExpUnref(tev->c_in[1].exp, tev->c_in[1].sel);
                    changed = 1;
                    tev->c_in[1] = zero_arg;
                } else if (tev->c_in[0].sel == HSD_TE_0 &&
                           tev->c_in[1].sel == HSD_TE_0)
                {
                    tev->c_op = 0;
                    changed = 1;
                    tev->c_in[0] = tev->c_in[2];
                    tev->c_in[2] = zero_arg;
                }
                break;
            }
        }

        if (tev->a_op != 0xFF && tev->a_ref == 0) {
            tev->a_op = 0xFF;
            for (i = 0; i < 4; i++) {
                HSD_TExpUnref(tev->a_in[i].exp, tev->a_in[i].sel);
                tev->a_in[i] = zero_arg;
            }
            changed = 1;
        }

        switch (tev->a_op) {
        case 0:
        case 1:
            if (tev->a_in[2].sel == HSD_TE_0) {
                if (tev->a_in[1].sel != HSD_TE_0) {
                    HSD_TExpUnref(tev->a_in[1].exp, tev->a_in[1].sel);
                    changed = 1;
                    tev->a_in[1] = zero_arg;
                }
                if (tev->a_op == 0 && tev->a_in[3].sel == HSD_TE_0) {
                    changed = 1;
                    tev->a_in[3] = tev->a_in[0];
                    tev->a_in[0] = zero_arg;
                }
            }
            if (tev->a_in[2].sel == HSD_TE_1) {
                if (tev->a_in[0].sel != HSD_TE_0) {
                    HSD_TExpUnref(tev->a_in[0].exp, tev->a_in[0].sel);
                    changed = 1;
                    tev->a_in[0] = zero_arg;
                }
                if (tev->a_op == 0 && tev->a_in[3].sel == HSD_TE_0) {
                    changed = 1;
                    tev->a_in[3] = tev->a_in[1];
                    tev->a_in[1] = zero_arg;
                    tev->a_in[2] = zero_arg;
                }
            }
            if (tev->a_in[0].sel == HSD_TE_0 &&
                tev->a_in[1].sel == HSD_TE_0 &&
                tev->a_in[3].sel == HSD_TE_0)
            {
                tev->a_op = 0xFF;
                changed = 1;
            }
            break;

        case 8:
        case 9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
            if (tev->a_in[2].sel == HSD_TE_0) {
                tev->a_op = 0;
                HSD_TExpUnref(tev->a_in[0].exp, tev->a_in[0].sel);
                tev->a_in[0] = zero_arg;
                HSD_TExpUnref(tev->a_in[1].exp, tev->a_in[1].sel);
                changed = 1;
                tev->a_in[1] = zero_arg;
            }
            break;

        case 0xE:
            if (tev->a_in[2].sel == HSD_TE_0) {
                tev->a_op = 0;
                HSD_TExpUnref(tev->a_in[0].exp, tev->a_in[0].sel);
                tev->a_in[0] = zero_arg;
                HSD_TExpUnref(tev->a_in[1].exp, tev->a_in[1].sel);
                changed = 1;
                tev->a_in[1] = zero_arg;
            } else if (tev->a_in[0].sel == HSD_TE_0) {
                tev->a_op = 0;
                HSD_TExpUnref(tev->a_in[1].exp, tev->a_in[1].sel);
                tev->a_in[1] = zero_arg;
                HSD_TExpUnref(tev->a_in[2].exp, tev->a_in[2].sel);
                changed = 1;
                tev->a_in[2] = zero_arg;
            }
            break;

        case 0xF:
            if (tev->a_in[2].sel == HSD_TE_0) {
                tev->a_op = 0;
                HSD_TExpUnref(tev->a_in[0].exp, tev->a_in[0].sel);
                tev->a_in[0] = zero_arg;
                HSD_TExpUnref(tev->a_in[1].exp, tev->a_in[1].sel);
                changed = 1;
                tev->a_in[1] = zero_arg;
            } else if (tev->a_in[0].sel == HSD_TE_0 &&
                       tev->a_in[1].sel == HSD_TE_0)
            {
                tev->a_op = 0;
                changed = 1;
                tev->a_in[0] = tev->a_in[2];
                tev->a_in[2] = zero_arg;
            }
            break;
        }

        if (changed != 0) {
            result = 1;
        }
    } while (changed != 0);

    return result;
}

/*
 * Helpers of SimplifyByMerge. Each one is expanded at several places in the
 * retail code (color and alpha, input a and input d).
 */
static inline BOOL TExpConflict(HSD_TETev* tev, HSD_TExp* child)
{
    if (tev->tex != NULL && child->tev.tex != NULL &&
        tev->tex != child->tev.tex)
    {
        return TRUE;
    }
    if (tev->chan != 0xFF && child->tev.chan != 0xFF &&
        tev->chan != child->tev.chan)
    {
        return TRUE;
    }
    return FALSE;
}

static inline BOOL TExpHasCnst(HSD_TEArg* in)
{
    return in[0].type == HSD_TE_CNST || in[1].type == HSD_TE_CNST ||
           in[2].type == HSD_TE_CNST || in[3].type == HSD_TE_CNST;
}

/* Adds the child's bias to this stage's; fails when the sum has no encoding. */
static inline BOOL TExpMergeBias(GXTevOp op, int bias, int child_bias,
                                 u8* result)
{
    int val;

    switch (child_bias) {
    case GX_TB_ADDHALF:
        val = 1;
        break;
    case GX_TB_SUBHALF:
        val = -1;
        break;
    default:
        val = 0;
        break;
    }
    if (op == GX_TEV_SUB) {
        val = -val;
    }
    switch (bias) {
    case GX_TB_ADDHALF:
        val += 1;
        break;
    case GX_TB_SUBHALF:
        val -= 1;
        break;
    }
    switch (val) {
    case 0:
        *result = GX_TB_ZERO;
        return TRUE;
    case 1:
        *result = GX_TB_ADDHALF;
        return TRUE;
    case -1:
        *result = GX_TB_SUBHALF;
        return TRUE;
    }
    return FALSE;
}

/* Scale factor in halves: 1/2 -> 1, x1 -> 2, x2 -> 4, x4 -> 8. */
static inline int TExpScale2Int(int scale)
{
    switch (scale) {
    case GX_CS_SCALE_1:
        return 2;
    case GX_CS_SCALE_2:
        return 4;
    case GX_CS_SCALE_4:
        return 8;
    case GX_CS_DIVIDE_2:
        return 1;
    default:
        return 2;
    }
}

/* Multiplies two TEV scales; fails when the product has no GX encoding. */
static inline BOOL TExpMergeScale(int scale0, int scale1, u8* result)
{
    switch (TExpScale2Int(scale0) * TExpScale2Int(scale1) / 2) {
    case 1:
        *result = GX_CS_DIVIDE_2;
        break;
    case 2:
        *result = GX_CS_SCALE_1;
        break;
    case 4:
        *result = GX_CS_SCALE_2;
        break;
    case 8:
        *result = GX_CS_SCALE_4;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

/*
 * SimplifyByMerge. Colosseum's version is a rewrite of the Melee one: it
 * tries four merges per pass (color input a, color input d, alpha input a,
 * alpha input d) and any failed test abandons the current merge and goes on
 * to the next one. Retail branches straight to the next attempt from every
 * failing test, so the attempts are written with labels.
 */
int SimplifyByMerge(HSD_TETev* tev)
{
    int result;
    int i;
    int merged;
    HSD_TExp* child;
    u8 sel;
    u8 bias;
    u8 scale;

    result = FALSE;
    do {
        merged = FALSE;

        /* Color, input a: fold a pass-through stage feeding a (or d, which is
         * swapped into a) into this one. */
        if ((tev->c_op == GX_TEV_ADD || tev->c_op == GX_TEV_SUB) &&
            tev->c_in[2].sel == HSD_TE_0)
        {
            child = tev->c_in[0].exp;
            sel = tev->c_in[0].sel;
            if (!(tev->c_in[0].type == HSD_TE_TEV &&
                  tev->c_in[0].sel == HSD_TE_RGB &&
                  child->tev.c_range == 0 &&
                  child->tev.c_in[3].sel == HSD_TE_0 &&
                  !TExpConflict(tev, child) &&
                  !(TExpHasCnst(tev->c_in) && TExpHasCnst(child->tev.c_in))))
            {
                child = tev->c_in[3].exp;
                sel = tev->c_in[3].sel;
                if (!(tev->c_op == GX_TEV_ADD &&
                      tev->c_in[3].type == HSD_TE_TEV &&
                      tev->c_in[3].sel == HSD_TE_RGB &&
                      child->tev.c_range == 0 &&
                      child->tev.c_in[3].sel == HSD_TE_0 &&
                      !TExpConflict(tev, child) &&
                      !(TExpHasCnst(tev->c_in) &&
                        TExpHasCnst(child->tev.c_in))))
                {
                    goto color_d;
                }
                if (tev->c_in[0].type == HSD_TE_TEV) {
                    if (tev->c_in[0].sel == HSD_TE_RGB) {
                        if (tev->c_in[0].exp->tev.c_range != 0 &&
                            tev->c_in[0].exp->tev.c_clamp != 1)
                        {
                            goto color_d;
                        }
                    } else {
                        if (tev->c_in[0].exp->tev.a_range != 0 &&
                            tev->c_in[0].exp->tev.a_clamp != 1)
                        {
                            goto color_d;
                        }
                    }
                }
                {
                    HSD_TEArg tmp = tev->c_in[0];
                    tev->c_in[0] = tev->c_in[3];
                    tev->c_in[3] = tmp;
                }
            }

            if (child->tev.c_scale == GX_CS_SCALE_1) {
                if (!TExpMergeBias(tev->c_op, tev->c_bias, child->tev.c_bias,
                                   &bias))
                {
                    goto color_d;
                }
                scale = tev->c_scale;
            } else {
                if (tev->c_in[3].sel != HSD_TE_0 || tev->c_bias != GX_TB_ZERO) {
                    goto color_d;
                }
                if (!TExpMergeScale(tev->c_scale, child->tev.c_scale,
                                    &scale))
                {
                    goto color_d;
                }
                bias = child->tev.c_bias;
            }

            if (tev->c_op == GX_TEV_SUB) {
                tev->c_op = child->tev.c_op == GX_TEV_ADD;
            } else {
                tev->c_op = child->tev.c_op;
            }
            for (i = 0; i < 3; i++) {
                tev->c_in[i] = child->tev.c_in[i];
                HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
            }
            tev->c_bias = bias;
            tev->c_scale = scale;
            HSD_TExpUnref(child, sel);
            if (tev->tex == NULL) {
                tev->tex = child->tev.tex;
            }
            if (tev->chan == 0xFF) {
                tev->chan = child->tev.chan;
            }
            if (tev->tex_swap == 0xFF) {
                tev->tex_swap = child->tev.tex_swap;
            }
            if (tev->ras_swap == 0xFF) {
                tev->ras_swap = child->tev.ras_swap;
            }
            merged = TRUE;
        }

    color_d:
        /* Color, input d: fold a stage feeding d into this one. */
        child = tev->c_in[3].exp;
        sel = tev->c_in[3].sel;
        if (tev->c_in[3].type == HSD_TE_TEV && sel == HSD_TE_RGB &&
            !TExpConflict(tev, child) &&
            !(TExpHasCnst(tev->c_in) && TExpHasCnst(child->tev.c_in)))
        {
            if (child->tev.c_range != 0 && child->tev.c_clamp == 1) {
                if (tev->c_in[0].sel == HSD_TE_0 &&
                    tev->c_in[2].sel == HSD_TE_0 &&
                    tev->c_bias == GX_TB_ZERO &&
                    tev->c_scale != GX_CS_DIVIDE_2 &&
                    TExpMergeScale(tev->c_scale, child->tev.c_scale, &scale))
                {
                    merged = TRUE;
                    tev->c_bias = child->tev.c_bias;
                    tev->c_scale = scale;
                    if (tev->c_clamp != 1) {
                        tev->c_clamp = child->tev.c_clamp;
                    }
                    if (tev->tex == NULL) {
                        tev->tex = child->tev.tex;
                    }
                    if (tev->chan == 0xFF) {
                        tev->chan = child->tev.chan;
                    }
                    if (tev->tex_swap == 0xFF) {
                        tev->tex_swap = child->tev.tex_swap;
                    }
                    if (tev->ras_swap == 0xFF) {
                        tev->ras_swap = child->tev.ras_swap;
                    }
                    for (i = 0; i < 4; i++) {
                        HSD_TExp* exp = tev->c_in[i].exp;
                        u8 exp_sel = tev->c_in[i].sel;
                        tev->c_in[i] = child->tev.c_in[i];
                        HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                        HSD_TExpUnref(exp, exp_sel);
                    }
                }
            } else if ((tev->a_op == 0xFF || tev->a_op == 0xE ||
                        tev->a_op == 0xF || tev->a_op == GX_TEV_ADD ||
                        tev->a_op == GX_TEV_SUB) ||
                       (child->tev.c_in[0].sel == HSD_TE_0 &&
                        child->tev.c_in[1].sel == HSD_TE_0 &&
                        child->tev.c_in[2].sel == HSD_TE_0))
            {
                if (child->tev.c_scale == GX_CS_SCALE_1) {
                    if (!((tev->c_in[0].sel == HSD_TE_0 &&
                           tev->c_in[2].sel == HSD_TE_0) ||
                          (child->tev.c_in[0].sel == HSD_TE_0 &&
                           child->tev.c_in[2].sel == HSD_TE_0)))
                    {
                        goto alpha_a;
                    }
                    if (!TExpMergeBias(GX_TEV_ADD, tev->c_bias, child->tev.c_bias,
                                       &bias))
                    {
                        goto alpha_a;
                    }
                    scale = tev->c_scale;
                } else {
                    if (tev->c_in[0].sel != HSD_TE_0 ||
                        tev->c_in[1].sel != HSD_TE_0 ||
                        tev->c_in[2].sel != HSD_TE_0 ||
                        tev->c_bias != GX_TB_ZERO)
                    {
                        goto alpha_a;
                    }
                    if (!TExpMergeScale(tev->c_scale, child->tev.c_scale,
                                        &scale))
                    {
                        goto alpha_a;
                    }
                    bias = child->tev.c_bias;
                }

                merged = TRUE;
                tev->c_in[3] = child->tev.c_in[3];
                HSD_TExpRef(tev->c_in[3].exp, tev->c_in[3].sel);
                tev->c_bias = bias;
                tev->c_scale = scale;
                if (tev->tex == NULL) {
                    tev->tex = child->tev.tex;
                }
                if (tev->chan == 0xFF) {
                    tev->chan = child->tev.chan;
                }
                if (tev->tex_swap == 0xFF) {
                    tev->tex_swap = child->tev.tex_swap;
                }
                if (tev->ras_swap == 0xFF) {
                    tev->ras_swap = child->tev.ras_swap;
                }
                if (!(child->tev.c_in[0].sel == HSD_TE_0 &&
                      child->tev.c_in[1].sel == HSD_TE_0 &&
                      child->tev.c_in[2].sel == HSD_TE_0))
                {
                    for (i = 0; i < 3; i++) {
                        HSD_TExp* exp = tev->c_in[i].exp;
                        u8 exp_sel = tev->c_in[i].sel;
                        tev->c_in[i] = child->tev.c_in[i];
                        HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                        HSD_TExpUnref(exp, exp_sel);
                    }
                }
                HSD_TExpUnref(child, sel);
            }
        }

    alpha_a:
        /* Alpha, input a (or d, swapped into a). */
        if ((tev->a_op == GX_TEV_ADD || tev->a_op == GX_TEV_SUB) &&
            tev->a_in[2].sel == HSD_TE_0)
        {
            child = tev->a_in[0].exp;
            sel = tev->a_in[0].sel;
            if (!(tev->a_in[0].type == HSD_TE_TEV &&
                  child->tev.a_range == 0 &&
                  child->tev.a_in[3].sel == HSD_TE_0 &&
                  !TExpConflict(tev, child) &&
                  !(TExpHasCnst(tev->a_in) && TExpHasCnst(child->tev.a_in))))
            {
                child = tev->a_in[3].exp;
                sel = tev->a_in[3].sel;
                /* The retail code tests the child's inputs twice here. */
                if (!(tev->a_op == GX_TEV_ADD &&
                      tev->a_in[3].type == HSD_TE_TEV &&
                      child->tev.a_range == 0 &&
                      child->tev.a_in[3].sel == HSD_TE_0 &&
                      !TExpConflict(tev, child) &&
                      !(TExpHasCnst(child->tev.a_in) &&
                        TExpHasCnst(child->tev.a_in))))
                {
                    goto alpha_d;
                }
                if (tev->a_in[0].type == HSD_TE_TEV &&
                    tev->a_in[0].exp->tev.a_range != 0 &&
                    tev->a_in[0].exp->tev.a_clamp != 1)
                {
                    goto alpha_d;
                }
                {
                    HSD_TEArg tmp = tev->a_in[0];
                    tev->a_in[0] = tev->a_in[3];
                    tev->a_in[3] = tmp;
                }
            }

            if (child->tev.a_scale == GX_CS_SCALE_1) {
                if (!TExpMergeBias(tev->a_op, tev->a_bias, child->tev.a_bias,
                                   &bias))
                {
                    goto alpha_d;
                }
                scale = tev->a_scale;
            } else {
                if (tev->a_in[3].sel != HSD_TE_0 || tev->a_bias != GX_TB_ZERO) {
                    goto alpha_d;
                }
                if (!TExpMergeScale(tev->a_scale, child->tev.a_scale,
                                    &scale))
                {
                    goto alpha_d;
                }
                bias = child->tev.a_bias;
            }

            if (tev->a_op == GX_TEV_SUB) {
                tev->a_op = child->tev.a_op == GX_TEV_ADD;
            } else {
                tev->a_op = child->tev.a_op;
            }
            for (i = 0; i < 3; i++) {
                tev->a_in[i] = child->tev.a_in[i];
                HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
            }
            tev->a_bias = bias;
            tev->a_scale = scale;
            HSD_TExpUnref(child, sel);
            if (tev->tex == NULL) {
                tev->tex = child->tev.tex;
            }
            if (tev->chan == 0xFF) {
                tev->chan = child->tev.chan;
            }
            if (tev->tex_swap == 0xFF) {
                tev->tex_swap = child->tev.tex_swap;
            }
            if (tev->ras_swap == 0xFF) {
                tev->ras_swap = child->tev.ras_swap;
            }
            merged = TRUE;
        }

    alpha_d:
        /* Alpha, input d. */
        child = tev->a_in[3].exp;
        sel = tev->a_in[3].sel;
        if (tev->a_in[3].type == HSD_TE_TEV && !TExpConflict(tev, child) &&
            !(TExpHasCnst(tev->a_in) && TExpHasCnst(child->tev.a_in)))
        {
            if (child->tev.a_range != 0 && child->tev.a_clamp == 1) {
                if (tev->a_in[0].sel == HSD_TE_0 &&
                    tev->a_in[2].sel == HSD_TE_0 &&
                    tev->a_bias == GX_TB_ZERO &&
                    tev->a_scale != GX_CS_DIVIDE_2 &&
                    TExpMergeScale(tev->a_scale, child->tev.a_scale, &scale))
                {
                    merged = TRUE;
                    tev->a_bias = child->tev.a_bias;
                    tev->a_scale = scale;
                    if (tev->a_clamp != 1) {
                        tev->a_clamp = child->tev.a_clamp;
                    }
                    if (tev->tex == NULL) {
                        tev->tex = child->tev.tex;
                    }
                    if (tev->chan == 0xFF) {
                        tev->chan = child->tev.chan;
                    }
                    if (tev->tex_swap == 0xFF) {
                        tev->tex_swap = child->tev.tex_swap;
                    }
                    if (tev->ras_swap == 0xFF) {
                        tev->ras_swap = child->tev.ras_swap;
                    }
                    for (i = 0; i < 4; i++) {
                        HSD_TExp* exp = tev->a_in[i].exp;
                        u8 exp_sel = tev->a_in[i].sel;
                        tev->a_in[i] = child->tev.a_in[i];
                        HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                        HSD_TExpUnref(exp, exp_sel);
                    }
                }
            } else {
                if (child->tev.a_scale == GX_CS_SCALE_1) {
                    if (!((tev->a_in[0].sel == HSD_TE_0 &&
                           tev->a_in[2].sel == HSD_TE_0) ||
                          (child->tev.a_in[0].sel == HSD_TE_0 &&
                           child->tev.a_in[2].sel == HSD_TE_0)))
                    {
                        goto next;
                    }
                    if (!TExpMergeBias(GX_TEV_ADD, tev->a_bias, child->tev.a_bias,
                                       &bias))
                    {
                        goto next;
                    }
                    scale = tev->a_scale;
                } else {
                    if (tev->a_in[0].sel != HSD_TE_0 ||
                        tev->a_in[1].sel != HSD_TE_0 ||
                        tev->a_in[2].sel != HSD_TE_0 ||
                        tev->a_bias != GX_TB_ZERO)
                    {
                        goto next;
                    }
                    if (!TExpMergeScale(tev->a_scale, child->tev.a_scale,
                                        &scale))
                    {
                        goto next;
                    }
                    bias = child->tev.a_bias;
                }

                merged = TRUE;
                tev->a_in[3] = child->tev.a_in[3];
                HSD_TExpRef(tev->a_in[3].exp, tev->a_in[3].sel);
                tev->a_bias = bias;
                tev->a_scale = scale;
                if (tev->tex == NULL) {
                    tev->tex = child->tev.tex;
                }
                if (tev->chan == 0xFF) {
                    tev->chan = child->tev.chan;
                }
                if (tev->tex_swap == 0xFF) {
                    tev->tex_swap = child->tev.tex_swap;
                }
                if (tev->ras_swap == 0xFF) {
                    tev->ras_swap = child->tev.ras_swap;
                }
                if (!(child->tev.a_in[0].sel == HSD_TE_0 &&
                      child->tev.a_in[1].sel == HSD_TE_0 &&
                      child->tev.a_in[2].sel == HSD_TE_0))
                {
                    for (i = 0; i < 3; i++) {
                        HSD_TExp* exp = tev->a_in[i].exp;
                        u8 exp_sel = tev->a_in[i].sel;
                        tev->a_in[i] = child->tev.a_in[i];
                        HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                        HSD_TExpUnref(exp, exp_sel);
                    }
                }
                HSD_TExpUnref(child, sel);
            }
        }

    next:
        if (merged) {
            result = TRUE;
        }
    } while (merged);

    return result;
}

/*
 * Works out whether the color and alpha results of a TEV stage can leave the
 * 0..255 range (c_range/a_range); an unclamped input that already overflows
 * propagates. No Melee counterpart. Each early decision jumps straight to the
 * alpha half, as retail does.
 */
void fn_801B9048(HSD_TETev* tev)
{
    int range;
    int i;

    range = 0;
    if (tev->c_in[3].sel != HSD_TE_0) {
        switch (HSD_TExpGetType(tev->c_in[3].exp)) {
        case HSD_TE_TEV:
            if (tev->c_in[3].sel == HSD_TE_RGB) {
                if (tev->c_in[3].exp->tev.c_clamp != 1 &&
                    tev->c_in[3].exp->tev.c_range == 1)
                {
                    tev->c_range = 1;
                    goto alpha;
                }
                range += 0x100;
            } else {
                if (tev->c_in[3].exp->tev.a_clamp != 1 &&
                    tev->c_in[3].exp->tev.a_range == 1)
                {
                    tev->c_range = 1;
                    goto alpha;
                }
                range += 0x100;
            }
            break;
        default:
            range += 0x100;
            break;
        }
    }
    for (i = 0; i < 3; i++) {
        if (tev->c_in[i].sel != HSD_TE_0) {
            if (tev->c_op == GX_TEV_SUB) {
                tev->c_range = 1;
                goto alpha;
            }
            range += 0x100;
            break;
        }
    }
    switch (tev->c_bias) {
    case GX_TB_ADDHALF:
        range += 0x80;
        break;
    case GX_TB_SUBHALF:
        tev->c_range = 1;
        goto alpha;
    }
    switch (tev->c_scale) {
    case GX_CS_SCALE_2:
        range *= 2;
        break;
    case GX_CS_SCALE_4:
        range *= 4;
        break;
    case GX_CS_DIVIDE_2:
        range /= 2;
        break;
    }
    tev->c_range = range > 0x100;

alpha:
    range = 0;
    if (tev->a_in[3].sel != HSD_TE_0) {
        switch (HSD_TExpGetType(tev->a_in[3].exp)) {
        case HSD_TE_TEV:
            if (tev->a_in[3].exp->tev.a_clamp != 1 &&
                tev->a_in[3].exp->tev.a_range == 1)
            {
                tev->a_range = 1;
                return;
            }
            range += 0x100;
            break;
        default:
            range += 0x100;
            break;
        }
    }
    for (i = 0; i < 3; i++) {
        if (tev->a_in[i].sel != HSD_TE_0) {
            if (tev->a_op == GX_TEV_SUB) {
                tev->a_range = 1;
                return;
            }
            range += 0x100;
            break;
        }
    }
    switch (tev->a_bias) {
    case GX_TB_ADDHALF:
        range += 0x80;
        break;
    case GX_TB_SUBHALF:
        tev->a_range = 1;
        return;
    }
    switch (tev->a_scale) {
    case GX_CS_SCALE_2:
        range *= 2;
        break;
    case GX_CS_SCALE_4:
        range *= 4;
        break;
    case GX_CS_DIVIDE_2:
        range /= 2;
        break;
    }
    tev->a_range = range > 0x100;
}

int HSD_TExpSimplify(HSD_TExp* texp)
{
    HSD_TETev* tev = &texp->tev;
    int res = FALSE;

    if (HSD_TExpGetType(texp) != HSD_TE_TEV) {
        return FALSE;
    }
    if (SimplifySrc(tev) != 0) {
        res = TRUE;
    }
    if (SimplifyThis(tev) != 0) {
        res = TRUE;
    }
    if (SimplifyByMerge(tev) != 0) {
        res = TRUE;
    }
    fn_801B9048(tev);
    return res;
}

/* HSD_TExpSimplify2 */
int HSD_TExpSimplify2(HSD_TExp* texp)
{
    HSD_TETev* tev = &texp->tev;
    HSD_TExp* src_exp;
    u8 src_sel;
    int result = 0;
    int i;

    for (i = 0; i < 4; i++) {
        src_exp = tev->c_in[i].exp;
        src_sel = tev->c_in[i].sel;
        if (tev->c_in[i].type == HSD_TE_TEV && src_sel == HSD_TE_RGB &&
            IsThroughColor(src_exp))
        {
            switch (src_exp->tev.c_in[3].type) {
            case HSD_TE_KONST:
                if (tev->kcsel == 0xFF) {
                    tev->kcsel = src_exp->tev.kcsel;
                } else if (tev->kcsel != src_exp->tev.kcsel) {
                    break;
                }
                /* fallthrough */
            case HSD_TE_IMM:
                tev->c_in[i] = src_exp->tev.c_in[3];
                HSD_TExpRef(tev->c_in[i].exp, tev->c_in[i].sel);
                HSD_TExpUnref(src_exp, src_sel);
                break;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        src_exp = tev->a_in[i].exp;
        src_sel = tev->a_in[i].sel;
        if (tev->a_in[i].type == HSD_TE_TEV && IsThroughAlpha(src_exp)) {
            switch (src_exp->tev.a_in[3].type) {
            case HSD_TE_KONST:
                if (tev->kasel == 0xFF) {
                    tev->kasel = src_exp->tev.kasel;
                } else if (tev->kasel != src_exp->tev.kasel) {
                    break;
                }
                /* fallthrough */
            case HSD_TE_IMM:
                tev->a_in[i] = src_exp->tev.a_in[3];
                HSD_TExpRef(tev->a_in[i].exp, tev->a_in[i].sel);
                HSD_TExpUnref(src_exp, src_sel);
                break;
            }
        }
    }
    return result;
}

/* assign_reg */
int assign_reg(int num, u32* dep, HSD_TExpDag* list, const int* order)
{
    int num_regs;
    int idx;
    int i;
    int min_color_reg = 4;
    int min_alpha_reg = 4;
    HSD_TETev* tev;
    HSD_TExpDag* dag_entry;
    u8 color_refs[4] = { 0 };
    u8 alpha_refs[4] = { 0 };

    for (idx = num - 1; idx >= 0; idx--) {
        dag_entry = &list[order[idx]];
        tev = dag_entry->tev;

        for (i = 0; i < 4; i++) {
            if (HSD_TExpGetType(tev->c_in[i].exp) == HSD_TE_TEV) {
                if (tev->c_in[i].sel == HSD_TE_RGB) {
                    color_refs[tev->c_in[i].exp->tev.c_dst] -= 1;
                } else {
                    alpha_refs[tev->c_in[i].exp->tev.a_dst] -= 1;
                }
            }
            if (HSD_TExpGetType(tev->a_in[i].exp) == HSD_TE_TEV) {
                alpha_refs[tev->a_in[i].exp->tev.a_dst] -= 1;
            }
        }

        tev = dag_entry->tev;
        if (tev->c_ref > 0) {
            for (i = 3; i >= 0; i--) {
                if (color_refs[i] == 0) {
                    color_refs[i] = tev->c_ref;
                    tev->c_dst = i;
                    if (min_color_reg > i) {
                        min_color_reg = i;
                    }
                    break;
                }
            }
        }
        if (tev->a_ref > 0) {
            for (i = 3; i >= 0; i--) {
                if (alpha_refs[i] == 0) {
                    alpha_refs[i] = tev->a_ref;
                    tev->a_dst = i;
                    if (min_alpha_reg > i) {
                        min_alpha_reg = i;
                    }
                    break;
                }
            }
        }
    }
    num_regs = (4 - min_color_reg) + (4 - min_alpha_reg);
    return num_regs;
}

/* order_dag */
void order_dag(int num, u32* dep, u32* full_dep, HSD_TExpDag* list,
                 int depth, int idx, u32 done_set, u32 ready_set, int* order,
                 int* min, int* min_order)
{
    u32 blocked;
    u32 full_bits;
    int score;
    int i;

    done_set |= 1 << idx;
    ready_set &= ~(1 << idx);
    order[depth++] = (u8) idx;

    if (depth == num) {
        score = assign_reg(num, dep, list, order);
        if (score < *min) {
            *min = score;
            for (i = 0; i < num; i++) {
                min_order[i] = order[i];
            }
        }
    } else {
        blocked = ready_set | dep[idx];
        full_bits = 0;
        for (i = 0; i < num; i++) {
            if (blocked & (1 << i)) {
                full_bits |= full_dep[i];
            }
        }
        ready_set = blocked & ~full_bits;
        if (list[idx].nb_dep == 1 && (ready_set & dep[idx])) {
            order_dag(num, dep, full_dep, list, depth,
                      list[idx].depend[0]->idx, done_set, ready_set, order,
                      min, min_order);
        } else {
            for (i = 0; i < num; i++) {
                if (ready_set & (1 << i)) {
                    order_dag(num, dep, full_dep, list, depth, i, done_set,
                              ready_set, order, min, min_order);
                }
            }
        }
    }
}

void CalcDistance(HSD_TExp** tevs, int* dist, HSD_TExp* tev, int num,
                  int depth)
{
    int i;

    for (i = 0; i < num; i++) {
        if (tevs[i] == tev) {
            if (dist[i] < depth) {
                dist[i] = depth;
                depth++;
                for (i = 0; i < 4; i++) {
                    if (tev->tev.c_in[i].type == HSD_TE_TEV) {
                        CalcDistance(tevs, dist, tev->tev.c_in[i].exp, num,
                                     depth);
                    }
                    if (tev->tev.a_in[i].type == HSD_TE_TEV) {
                        CalcDistance(tevs, dist, tev->tev.a_in[i].exp, num,
                                     depth);
                    }
                }
            }
            return;
        }
    }
}

#define HSD_TEXP_MAX_NUM 32

int HSD_TExpMakeDag(HSD_TExp* root, HSD_TExpDag* list)
{
    HSD_TExp* tevs[HSD_TEXP_MAX_NUM];
    int dist[HSD_TEXP_MAX_NUM];
    int n;
    int j;
    int i;
    int l;
    int num;
    HSD_TExpDag* dag;

    HSD_ASSERT(238, HSD_TExpGetType(root) == HSD_TE_TEV);

    n = 0;
    tevs[n++] = root;
    for (j = 0; j < n; j++) {
        HSD_TExp* tmp;
        HSD_ASSERT(246, j<HSD_TEXP_MAX_NUM);
        tmp = tevs[j];
        for (i = 0; i < 4; i++) {
            if (tmp->tev.c_in[i].type == HSD_TE_TEV) {
                for (l = 0; l < n; l++) {
                    if (tevs[l] == tmp->tev.c_in[i].exp) {
                        break;
                    }
                }
                if (l >= n) {
                    tevs[n++] = tmp->tev.c_in[i].exp;
                }
            }
        }

        for (i = 0; i < 4; i++) {
            if (tmp->tev.a_in[i].type == HSD_TE_TEV) {
                for (l = 0; l < n; l++) {
                    if (tevs[l] == tmp->tev.a_in[i].exp) {
                        break;
                    }
                }
                if (l >= n) {
                    tevs[n++] = tmp->tev.a_in[i].exp;
                }
            }
        }
    }

    num = n;
    for (n = 0; n < num; n++) {
        dist[n] = -1;
    }

    CalcDistance(tevs, dist, tevs[0], num, 0);
    for (n = 0; n < num; n++) {
        for (j = n + 1; j < num; j++) {
            if (dist[j - 1] > dist[j]) {
                {
                    HSD_TExp* tmp = tevs[j - 1];
                    tevs[j - 1] = tevs[j];
                    tevs[j] = tmp;
                }

                {
                    int tmp = dist[j - 1];
                    dist[j - 1] = dist[j];
                    dist[j] = tmp;
                }
            }
        }
    }

    for (n = num - 1; n >= 0; n--) {
        HSD_TExp* tmp;
        dag = &list[n];
        tmp = tevs[n];
        dag->idx = n;
        dag->nb_ref = 0;
        dag->nb_dep = 0;
        dag->tev = &tmp->tev;
        for (i = 0; i < 4; i++) {
            if (tmp->tev.c_in[i].type == HSD_TE_TEV) {
                for (l = n; l < num; l++) {
                    if (tmp->tev.c_in[i].exp == tevs[l]) {
                        HSD_TExpDag* dep = &list[l];
                        for (l = 0; l < dag->nb_dep; l++) {
                            if (dag->depend[l] == dep) {
                                break;
                            }
                        }
                        if (l >= dag->nb_dep) {
                            dag->depend[dag->nb_dep++] = dep;
                            dep->nb_ref++;
                        }
                        break;
                    }
                }
                HSD_ASSERT(325, l < num);
            }
        }

        for (i = 0; i < 4; i++) {
            if (tmp->tev.a_in[i].type == HSD_TE_TEV) {
                for (l = n; l < num; l++) {
                    if (tmp->tev.a_in[i].exp == tevs[l]) {
                        HSD_TExpDag* dep = &list[l];
                        for (l = 0; l < dag->nb_dep; l++) {
                            if (dag->depend[l] == dep) {
                                break;
                            }
                        }
                        if (l >= dag->nb_dep) {
                            dag->depend[dag->nb_dep++] = dep;
                            dep->nb_ref++;
                        }
                        break;
                    }
                }
                HSD_ASSERT(347, l < num);
            }
        }
    }
    return num;
}

/*
 * HAL's dependency-matrix helpers (named as in Melee's texpdag.c). Both are
 * expanded into HSD_TExpSchedule; retail has no symbol for either.
 */
static inline void make_dependancy_mtx(int num, HSD_TExpDag* list,
                                       u32* dep_mtx)
{
    int i, j;

    for (i = 0; i < num; i++) {
        dep_mtx[i] = 0;
        for (j = 0; j < list[i].nb_dep; j++) {
            dep_mtx[i] |= 1 << list[i].depend[j]->idx;
        }
    }
}

static inline void make_full_dependancy_mtx(int num, const u32* dep, u32* full)
{
    int i, k, j;
    BOOL changed;
    u32 flag;
    u32 bits;
    u32 old;

    for (i = 0; i < num; i++) {
        full[i] = dep[i];
    }
    do {
        changed = FALSE;
        for (j = 0; j < num; j++) {
            flag = 1 << j;
            bits = full[j];
            for (k = 0; k < num; k++) {
                if (flag & full[k]) {
                    old = full[k];
                    full[k] = old | bits;
                    if (old != full[k]) {
                        changed = TRUE;
                    }
                }
            }
        }
    } while (changed);
}

/* HSD_TExpSchedule */
void HSD_TExpSchedule(int num, HSD_TExpDag* list, HSD_TExp** result,
                 HSD_TExpRes* resource)
{
    static int c_in[4] = { GX_CC_C0, GX_CC_C1, GX_CC_C2, GX_CC_CPREV };
    static int a_in[4] = { GX_CC_A0, GX_CC_A1, GX_CC_A2, GX_CC_APREV };
    static int args[4] = { GX_CA_A0, GX_CA_A1, GX_CA_A2, GX_CA_APREV };

    u32 dep_mtx[32];
    u32 full_dep_matrix[32];
    u32 order[32];
    u32 min_order[32];

    int i, j;
    int min;

    min = 5;
    memset(min_order, 0, sizeof(min_order));
    make_dependancy_mtx(num, list, dep_mtx);
    make_full_dependancy_mtx(num, dep_mtx, full_dep_matrix);
    order_dag(num, dep_mtx, full_dep_matrix, list, 0, 0, 0, 0, (int*) order,
              &min, (int*) min_order);

    for (i = 0; i < num; i++) {
        result[i] = (HSD_TExp*) list[min_order[i]].tev;
        if (result[i]->tev.c_dst != 0xFF) {
            resource->reg[result[i]->tev.c_dst + 4].color = 3;

            for (j = 0; j < 4; j++) {
                if (HSD_TExpGetType(result[i]->tev.c_in[j].exp) == HSD_TE_TEV)
                {
                    if (result[i]->tev.c_in[j].sel == 1) {
                        result[i]->tev.c_in[j].arg =
                            c_in[result[i]->tev.c_in[j].exp->tev.c_dst];
                    } else {
                        result[i]->tev.c_in[j].arg =
                            a_in[result[i]->tev.c_in[j].exp->tev.a_dst];
                    }
                }
            }
        }
        if (result[i]->tev.a_dst != 0xFF) {
            resource->reg[result[i]->tev.a_dst + 4].alpha = 1;

            for (j = 0; j < 4; j++) {
                if (HSD_TExpGetType(result[i]->tev.a_in[j].exp) == HSD_TE_TEV)
                {
                    result[i]->tev.a_in[j].arg =
                        args[result[i]->tev.a_in[j].exp->tev.a_dst];
                }
            }
        }
    }
}
