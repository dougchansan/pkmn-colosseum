#include "dolphin/types.h"
#include "hsd/hsd_tobj.h"

/*
 * Score-disclosed CodeCandidate partition for 0x801B8D5C..0x801B8FB8.
 * The partition preserves the retail function's ordinary external helper
 * visibility; it is not evidence that the retail source used this TU split.
 */

typedef struct ColTExpNode ColTExpNode;

typedef struct ColTEArg {
    u8 type;
    u8 sel;
    u8 arg;
    u8 pad_03;
    ColTExpNode* exp;
} ColTEArg;

struct ColTExpNode {
    s32 type;
    ColTExpNode* next;
    s32 c_ref;
    u8 c_dst;
    u8 c_op;
    u8 c_clamp;
    u8 c_bias;
    u8 c_scale;
    u8 c_range;
    u8 pad_12[2];
    s32 a_ref;
    u8 a_dst;
    u8 a_op;
    u8 a_clamp;
    u8 a_bias;
    u8 a_scale;
    u8 a_range;
    u8 tex_swap;
    u8 ras_swap;
    u8 kcsel;
    u8 kasel;
    u8 pad_22[2];
    s32 input_index[4];
    ColTEArg c_in[4];
    ColTEArg a_in[4];
    HSD_TObj* tex;
    u8 chan;
};

enum {
    COL_TE_TEV = 1,
    COL_TE_IMM = 5,
    COL_TE_KONST = 6,
    COL_TE_RGB = 1,
    COL_TE_0 = 7,
};

extern void fn_801B7BD4(ColTExpNode* exp, s32 sel);
extern void fn_801B750C(ColTExpNode* exp, u8 sel);

s32 fn_801B8D5C(ColTExpNode* texp)
{
    ColTExpNode* src;
    u8 sel;
    s32 i;

    for (i = 0; i < 4; i++) {
        src = texp->c_in[i].exp;
        sel = texp->c_in[i].sel;
        if (texp->c_in[i].type == COL_TE_TEV && sel == COL_TE_RGB &&
            src->c_op == 0 && src->c_in[0].sel == COL_TE_0 &&
            src->c_in[1].sel == COL_TE_0 && src->c_bias == 0 &&
            src->c_scale == 0)
        {
            switch (src->c_in[3].type) {
            case COL_TE_KONST:
                if (texp->kcsel == 0xFF) {
                    texp->kcsel = src->kcsel;
                } else if (texp->kcsel != src->kcsel) {
                    break;
                }
            case COL_TE_IMM:
                texp->c_in[i] = src->c_in[3];
                fn_801B7BD4(texp->c_in[i].exp, texp->c_in[i].sel);
                fn_801B750C(src, sel);
                break;
            }
        }
    }

    for (i = 0; i < 4; i++) {
        src = texp->a_in[i].exp;
        sel = texp->a_in[i].sel;
        if (texp->a_in[i].type == COL_TE_TEV && src->a_op == 0 &&
            src->a_in[0].sel == COL_TE_0 &&
            src->a_in[1].sel == COL_TE_0 && src->a_bias == 0 &&
            src->a_scale == 0)
        {
            switch (src->a_in[3].type) {
            case COL_TE_KONST:
                if (texp->kasel == 0xFF) {
                    texp->kasel = src->kasel;
                } else if (texp->kasel != src->kasel) {
                    break;
                }
            case COL_TE_IMM:
                texp->a_in[i] = src->a_in[3];
                fn_801B7BD4(texp->a_in[i].exp, texp->a_in[i].sel);
                fn_801B750C(src, sel);
                break;
            }
        }
    }
    return 0;
}
