#include "dolphin/types.h"
#include "crt/string.h"
#include "hsd/hsd_debug.h"
#include "hsd/hsd_mobj.h"
#include "hsd/hsd_tobj.h"

/*
 * Score-disclosed CodeCandidate partition for 0x801B4300..0x801B45A4.
 * This keeps the compile pipeline's helper visibility explicit; it is not
 * evidence that the retail source used this TU split.
 */

typedef struct ColTExpNode ColTExpNode;

typedef struct HSD_TevDesc {
    struct HSD_TevDesc* next;
    u32 flag;
    u32 stage;
    u32 coord;
    u32 map;
    u32 color;
    u32 color_op;
    u32 color_a;
    u32 color_b;
    u32 color_c;
    u32 color_d;
    u32 color_scale;
    u32 color_bias;
    u8 color_clamp;
    u8 pad_35[3];
    u32 color_tevreg;
    u32 alpha_op;
    u32 alpha_a;
    u32 alpha_b;
    u32 alpha_c;
    u32 alpha_d;
    u32 alpha_scale;
    u32 alpha_bias;
    u8 alpha_clamp;
    u8 pad_59[3];
    u32 alpha_tevreg;
    u32 pad_60;
    s32 kcolor0;
    s32 kcolor1;
    u32 swap0;
    u32 swap1;
    u32 kr;
    u32 kg;
    u32 kb;
    u32 ka;
} HSD_TevDesc;

struct _HSD_TExpTevDesc {
    HSD_TevDesc desc;
    HSD_TObj* tobj;
};

typedef struct ColTExpRes {
    s32 failed;
    s32 texmap;
    s32 cnst_remain;
    struct {
        u8 color;
        u8 alpha;
    } reg[8];
    u8 c_ref[4];
    u8 a_ref[4];
    u8 c_use[4];
    u8 a_use[4];
} ColTExpRes;

typedef struct HSD_TExpDag {
    ColTExpNode* tev;
    u8 idx;
    u8 nb_dep;
    u8 nb_ref;
    u8 dist;
    struct HSD_TExpDag* depend[8];
} HSD_TExpDag;

extern void fn_801B7BD4(ColTExpNode* exp, s32 sel);
extern s32 HSD_TExpSimplify(ColTExpNode* exp);
extern s32 HSD_TExpMakeDag(u8* root, HSD_TExpDag* list);
extern void fn_801B7CA0(s32 num, HSD_TExpDag* list, ColTExpNode** result,
                        ColTExpRes* resource);
extern s32 fn_801B50C0(ColTExpNode* exp, ColTExpRes* resource);
extern s32 fn_801B8D5C(ColTExpNode* exp);
extern void* fn_80193B10(s32 size);
extern s32 HSD_Index2TevStage(u32 index);
extern void TExp2TevDesc(ColTExpNode* exp, HSD_TExpTevDesc* desc,
                         s32* init_cprev, s32* init_aprev);
extern ColTExpNode* fn_801B7178(ColTExpNode* list, s32 type, s32 all);

s32 HSD_TExpCompile(ColTExpNode* texp, HSD_TExpTevDesc** tevdesc,
                    ColTExpNode** texp_list)
{
    s32 num;
    s32 i;
    s32 value;
    ColTExpRes resource;
    ColTExpNode* order[32];
    HSD_TExpDag list[32];
    s32 init_cprev = 1;
    s32 init_aprev = 1;

    HSD_ASSERT(0x677, tevdesc != NULL);
    HSD_ASSERT(0x678, texp_list != NULL);

    memset(&resource, 0, sizeof(resource));

    fn_801B7BD4(texp, 1);
    fn_801B7BD4(texp, 5);
    HSD_TExpSimplify(texp);

    num = HSD_TExpMakeDag((u8*) texp, list);
    fn_801B7CA0(num, list, order, &resource);
    for (i = 0; i < num; i++) {
        value = fn_801B50C0(order[i], &resource);
        HSD_ASSERT(0x698, value >= 0);
    }

    for (i = num - 1; i >= 0; i--) {
        fn_801B8D5C(order[i]);
    }

    num = HSD_TExpMakeDag((u8*) texp, list);
    fn_801B7CA0(num, list, order, &resource);
    *tevdesc = NULL;
    for (i = 0; i < num; i++) {
        HSD_TExpTevDesc* desc = fn_80193B10(sizeof(HSD_TExpTevDesc));
        desc->desc.stage = HSD_Index2TevStage(i);
        TExp2TevDesc(order[(num - i) - 1], desc, &init_cprev, &init_aprev);
        desc->desc.next = &(*tevdesc)->desc;
        *tevdesc = desc;
    }

    *texp_list = fn_801B7178(*texp_list, 1, 1);
    *texp_list = fn_801B7178(*texp_list, 4, 0);
    return num;
}
