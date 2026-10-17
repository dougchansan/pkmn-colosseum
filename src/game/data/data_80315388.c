#include "dolphin/types.h"

#pragma section ".data"

extern void* lbl_80315490[];
extern u8 lbl_803154E4[];
extern u8 lbl_80315540[];
extern u8 lbl_8031554C[];
extern u8 lbl_80315558[];
extern u8 lbl_80315568[];
extern void* lbl_80315598[];
extern void* jumptable_803155B0[];
extern void* lbl_803155D0[];
extern u8 lbl_80315668[];

extern u8 _GSmaterialObjInit[];
extern u8 _modelBoundVertex__FUlPvPv[];
extern u8 _modelBoundBeginSurface__F13GSgfxPrimTypeUsUlPv[];
extern u8 modelUpdateAttachments__FP8_GSmodel[];
extern u8 cms_op37_callStandard[];
extern u8 cms_op36_yield[];
extern u8 cms_op35_printf[];
extern u8 cms_op34_orl[];
extern u8 cms_op33_andl[];
extern u8 cms_op32_copyStruct[];
extern u8 cms_op31_setVar[];
extern u8 cms_op30_notEqual[];
extern u8 cms_op29_equal[];
extern u8 cms_op28_greaterThanEqual[];
extern u8 cms_op27_greaterThan[];
extern u8 cms_op26_lessThanEqual[];
extern u8 cms_op25_lessThan[];
extern u8 cms_op24_subtract[];
extern u8 cms_op23_add[];
extern u8 cms_op22_mod[];
extern u8 cms_op21_divide[];
extern u8 cms_op20_multiply[];
extern u8 cms_op19_pushVar2[];
extern u8 cms_op18_negate[];
extern u8 cms_op17_not[];
extern u8 cms_op16_pushMultiVar[];
extern u8 cms_op15_pushVarRef[];
extern u8 cms_op14_or[];
extern u8 cms_op13_and[];
extern u8 cms_op08_pushImmediate[];
extern u8 cms_op07_jumpFalse[];
extern u8 cms_op06_jump[];
extern u8 cms_op05_return[];
extern u8 cms_op04_call[];
extern u8 cms_op03_pop[];
extern u8 cms_op02_pushMinus1[];
extern u8 cms_op01_exit[];
extern u8 cms_op00_nop[];

/* Auto-carved .data unit 0x80315490..0x80315678 (10 objects; the GSlog formatters' jump tables at 0x80315388..0x80315490 are compiled by game/gs_log.cpp). Non-relocated data as byte-exact u8[]; pointer/jump tables as void*[] for R_PPC_ADDR32 relocations. */

#if !defined(DATA_80315388_SPLIT) || defined(DATA_80315388_PREFIX)
void* lbl_80315490[21] = {
    (void*)((u8*)_GSmaterialObjInit),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
};

u8 lbl_803154E4[92] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_80315540[12] = {
    0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_8031554C[12] = {
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

u8 lbl_80315558[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
};

u8 lbl_80315568[48] = {
    0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

void* lbl_80315598[6] = {
    (void*)((u8*)_modelBoundBeginSurface__F13GSgfxPrimTypeUsUlPv),
    (void*)0x00000000,
    (void*)((u8*)_modelBoundVertex__FUlPvPv),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
};
#endif

#if !defined(DATA_80315388_SPLIT)
void* jumptable_803155B0[8] = {
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0x194),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0xEC),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0xFC),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0x10C),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0x11C),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0x138),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0x154),
    (void*)((u8*)modelUpdateAttachments__FP8_GSmodel + 0x170),
};
#endif

#if !defined(DATA_80315388_SPLIT) || defined(DATA_80315388_SUFFIX)
void* lbl_803155D0[38] = {
    (void*)((u8*)cms_op00_nop),
    (void*)((u8*)cms_op01_exit),
    (void*)((u8*)cms_op02_pushMinus1),
    (void*)((u8*)cms_op03_pop),
    (void*)((u8*)cms_op04_call),
    (void*)((u8*)cms_op05_return),
    (void*)((u8*)cms_op06_jump),
    (void*)((u8*)cms_op07_jumpFalse),
    (void*)((u8*)cms_op08_pushImmediate),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)((u8*)cms_op13_and),
    (void*)((u8*)cms_op14_or),
    (void*)((u8*)cms_op15_pushVarRef),
    (void*)((u8*)cms_op16_pushMultiVar),
    (void*)((u8*)cms_op17_not),
    (void*)((u8*)cms_op18_negate),
    (void*)((u8*)cms_op19_pushVar2),
    (void*)((u8*)cms_op20_multiply),
    (void*)((u8*)cms_op21_divide),
    (void*)((u8*)cms_op22_mod),
    (void*)((u8*)cms_op23_add),
    (void*)((u8*)cms_op24_subtract),
    (void*)((u8*)cms_op25_lessThan),
    (void*)((u8*)cms_op26_lessThanEqual),
    (void*)((u8*)cms_op27_greaterThan),
    (void*)((u8*)cms_op28_greaterThanEqual),
    (void*)((u8*)cms_op29_equal),
    (void*)((u8*)cms_op30_notEqual),
    (void*)((u8*)cms_op31_setVar),
    (void*)((u8*)cms_op32_copyStruct),
    (void*)((u8*)cms_op33_andl),
    (void*)((u8*)cms_op34_orl),
    (void*)((u8*)cms_op35_printf),
    (void*)((u8*)cms_op36_yield),
    (void*)((u8*)cms_op37_callStandard),
};

u8 lbl_80315668[16] = {
    0x5F, 0x76, 0x6D, 0x54, 0x68, 0x72, 0x65, 0x61, 0x64, 0x43, 0x72, 0x65,
    0x61, 0x74, 0x65, 0x00,
};
#endif
