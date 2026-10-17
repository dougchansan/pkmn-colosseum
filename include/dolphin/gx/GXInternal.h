#ifndef DOLPHIN_GX_GX_INTERNAL_H
#define DOLPHIN_GX_GX_INTERNAL_H

#include "dolphin/gx/GX.h"

typedef union GXStatus {
    u32 word;
    struct {
        u16 field_000;
        u16 field_002;
    } half;
} GXStatus;

struct GXData {
    /* 0x000 */ GXStatus status;
    /* 0x004 */ u8 pad_004[0x78];
    /* 0x07C */ u32 lpSize;
    /* 0x080 */ u32 mtxIdx0;
    /* 0x084 */ u32 mtxIdx1;
    /* 0x088 */ u8 pad_088[0x30];
    /* 0x0B8 */ u32 suTs0[8];
    /* 0x0D8 */ u32 suTs1[8];
    /* 0x0F8 */ u32 scissorTL;
    /* 0x0FC */ u32 scissorBR;
    /* 0x100 */ u8 pad_100[0x20];
    /* 0x120 */ u32 field_120;
    /* 0x124 */ u32 field_124;
    /* 0x128 */ u8 pad_128[0x8];
    /* 0x130 */ u32 tevColorEnv[16];
    /* 0x170 */ u32 tevAlphaEnv[16];
    /* 0x1B0 */ u32 field_1B0[8];
    /* 0x1D0 */ u32 field_1D0;
    /* 0x1D4 */ u32 dstAlpha;
    /* 0x1D8 */ u32 zMode;
    /* 0x1DC */ u32 field_1DC;
    /* 0x1E0 */ u32 field_1E0;
    /* 0x1E4 */ u32 field_1E4;
    /* 0x1E8 */ u32 field_1E8;
    /* 0x1EC */ u32 field_1EC;
    /* 0x1F0 */ u32 field_1F0;
    /* 0x1F4 */ u32 field_1F4;
    /* 0x1F8 */ u32 field_1F8;
    /* 0x1FC */ u32 field_1FC;
    /* 0x200 */ u8 field_200;
    /* 0x201 */ u8 pad_201[3];
    /* 0x204 */ u32 genMode;
    /* 0x208 */ u8 pad_208[0x218];
    /* 0x420 */ u32 field_420;
    /* 0x424 */ f32 field_424;
    /* 0x428 */ f32 field_428;
    /* 0x42C */ f32 field_42C;
    /* 0x430 */ f32 field_430;
    /* 0x434 */ f32 field_434;
    /* 0x438 */ f32 field_438;
    /* 0x43C */ f32 projection[6];
    /* 0x454 */ u8 pad_454[0x8];
    /* 0x45C */ u32 texMapSize[8];
    /* 0x47C */ u32 texMapWrap[8];
    /* 0x49C */ u8 pad_49C[0x40];
    /* 0x4DC */ u32 field_4DC;
    /* 0x4E0 */ u32 field_4E0;
    /* 0x4E4 */ u8 pad_4E4[0x10];
    /* 0x4F4 */ u32 dirtyState;
};

extern GXData* gx;

/* __cpReg is the canonical volatile view of CP MMIO at 0xCC000000. */
extern volatile u16* __cpReg;

extern u32 lbl_80313608[];

void __GXSetDirtyState(void);
void __GXSendFlushPrim(void);
void GXSetViewportJitter(f32 left, f32 top, f32 width, f32 height, f32 nearz,
                 f32 farz, u32 field);
void __GXSetMatrixIndex(s32 value);
void fn_800BE164(u32* hi, u32* lo);
u32 __cvt_fp2unsigned(f32 value);

/* The only other volatile accesses in these sources are the GX FIFO MMIO. */
#define GX_FIFO_U8  (*(volatile u8*)0xCC008000)
#define GX_FIFO_U32 (*(volatile u32*)0xCC008000)
#define GX_FIFO_F32 (*(volatile f32*)0xCC008000)

#define GX_BP_REG(reg)       \
    do {                     \
        GX_FIFO_U8 = 0x61;   \
        GX_FIFO_U32 = (reg); \
    } while (0)

#endif /* DOLPHIN_GX_GX_INTERNAL_H */
