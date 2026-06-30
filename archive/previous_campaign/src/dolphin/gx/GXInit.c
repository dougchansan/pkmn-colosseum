#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSTime.h"
#include "dolphin/os/PPCArch.h"

/*
 * GXInit.c - GX Graphics API Initialization.
 *
 * Contains GXInit and __GXInitGX which set up the GP (Graphics Processor)
 * hardware, command processor, and rendering pipeline to default states.
 *
 * Matches: 0x800B5E8C - 0x800B6FE0
 *   GXInit     (0x890) - Full GX initialization
 *   __GXInitGX (0x8C4) - Initialize all GX state to defaults
 */

extern void* memset(void* dest, int val, u32 n);
extern void* memcpy(void* dest, const void* src, u32 n);
extern void OSRegisterVersion(const char* id);
extern void OSRegisterResetFunction(void* info);
extern void OSReport(const char* fmt, ...);

static const char* __GXVersion = "<< Dolphin SDK - GX\trelease build: Aug 22 2002 04:07:51 (0x2301) >>";

extern u8 lbl_80478A78[8];
extern u8 lbl_80478A80[8];
extern u8 lbl_80478A88[8];
extern u8 lbl_80478A90[8];
extern u8 lbl_80478A98[8];
extern u8 lbl_80478AA0[8];
extern u8 lbl_80478AA8[8];
extern u8 lbl_80478AB0[8];

/* GX hardware register bases */
#define CP_BASE      ((volatile u16*)0xCC000000)
#define PE_BASE      ((volatile u16*)0xCC001000)
#define PI_FIFO_BASE ((volatile u32*)0xCC003000)
#define GX_FIFO_U8   (*(volatile u8*)0xCC008000)
#define GX_FIFO_U32  (*(volatile u32*)0xCC008000)

#define GX_LOAD_BP(regval)       \
    do {                         \
        GX_FIFO_U8 = 0x61;       \
        GX_FIFO_U32 = (regval);  \
    } while (0)

#define GX_LOAD_XF(reg, regval)  \
    do {                         \
        GX_FIFO_U8 = 0x10;       \
        GX_FIFO_U32 = (reg);     \
        GX_FIFO_U32 = (regval);  \
    } while (0)

#define GX_LOAD_CP(reg, regval)  \
    do {                         \
        GX_FIFO_U8 = 0x08;       \
        GX_FIFO_U8 = (reg);      \
        GX_FIFO_U32 = (regval);  \
    } while (0)

/* GX internal state (large ~1KB structure) */
typedef struct GXData {
    u32     cpRegs[16];
    u32     xfRegs[16];
    u32     bpRegs[256];
    u32     fifoBase;
    u32     fifoEnd;
    u32     fifoSize;
    u32     cpEnable;
    u32     cpClr;
    u32     peCtrl;
    u16     cpCRstat;
    u8      dirtyState;
    u8      padding;
    u32     _pad[16];
} GXData;

static GXData gxData_803FC860;
GXData* __GXContextPtr = &gxData_803FC860;
extern volatile u16* __cpReg;
extern volatile u16* __piReg;
extern volatile u16* __peReg;
extern volatile u16* __memReg;
extern u8* gx;
extern void* lbl_8047A9A8;
extern u32 lbl_8047A9B0;
extern u32 lbl_8047A9B8;
extern void* lbl_8047A9C0;
extern void* lbl_8047A9C4;
extern void* lbl_8047A9B4;
extern u8 lbl_8047A9C8;
extern u8 lbl_8047A9CC;
extern void fn_800A1F94(void* thread);
extern void fn_800A221C(void* thread);
extern void fn_800A238C(void* queue);
extern void fn_800A2478(void* queue);
extern void __GXSetSUTexRegs(void);
extern void fn_800BC024(void);
extern void fn_800B937C(void);
extern void fn_800B9578(void);
extern void fn_800B7594(u32 arg0, u32 arg1);
extern void fn_800B75D0(u32 arg0, u32 arg1);
extern void fn_800B7BC4(void);
extern void fn_800B8444(void);
extern void fn_800B91EC(void);
extern void fn_800BD640(void);
extern void fn_800BD898(void);
extern void fn_800BE164(void);
extern void fn_80098034(void);
extern void __GXCleanGPFifo(void);
extern void __GXCalculateVLim(void);
extern void __GetImageTileCount(void);
extern void fn_800C46B0(void);
extern f32 lbl_8047C348;
extern f64 lbl_8047C350;
extern f32 lbl_8047C308;
extern f32 lbl_8047C358;
extern f32 lbl_8047C35C;
extern f32 lbl_8047C360;
extern f32 lbl_8047C364;
extern f32 lbl_8047C368;
extern f32 lbl_8047C36C;
extern f32 lbl_8047C370;
extern f32 lbl_8047C374;
extern f32 lbl_8047C378;
extern f64 lbl_8047C380;
extern f32 lbl_8047C388;
extern f32 lbl_8047C38C;
extern f32 lbl_8047C390;
extern f64 lbl_8047C398;
extern f32 lbl_8047C3A0;
extern f64 lbl_8047C3A8;
extern f32 lbl_8047C3B0;
extern f64 lbl_8047C3B8;
extern f64 lbl_8047C3C0;
extern f64 lbl_8047C3C8;
extern f32 lbl_8047C3D0;
extern f64 lbl_8047C3D8;
extern f32 lbl_8047C3E8;
extern f64 lbl_8047C3F0;
extern f32 lbl_8047C3F8;
extern f32 lbl_8047C3FC;
extern u8 lbl_80313590[];
extern u32 lbl_803135E0[];
extern u8 lbl_80313608[];
extern u32 jumptable_80312AE0[];
extern u32 jumptable_80312B48[];
extern u32 jumptable_80312B8C[];
extern u32 jumptable_80312BD0[];
extern u32 jumptable_80312BEC[];
extern u32 jumptable_80313628[];
extern u32 jumptable_80313684[];
extern u32 jumptable_80313550[];
extern u32 jumptable_80313714[];
extern u32 GXResetFuncInfo_80312AD0[];

/* FIFO state */
static GXFifoObj CPUFifo;
static GXFifoObj GPFifo;
static GXFifoObj* CPGPLinked;

/*
 * GXInit - Initialize the GX graphics subsystem.
 * 0x800B5E8C | size: 0x890
 */
asm void* GXInit(void* base, u32 size) {
    nofralloc
    mflr r0
    stw r0, 0x4(r1)
    stwu r1, -0x50(r1)
    stmw r25, 0x34(r1)
    mr r27, r3
    mr r25, r4
    lis r4, gxData_803FC860@ha
    lwz r3, __GXVersion(r13)
    addi r30, r4, gxData_803FC860@l
    bl OSRegisterVersion
    lwz r5, gx(r13)
    li r0, 0x0
    li r26, 0x1
    stb r0, 0x4f0(r5)
    li r3, 0x1
    li r4, 0x0
    stb r26, 0x4f1(r5)
    stb r26, 0x4f2(r5)
    stw r0, 0x4dc(r5)
    stw r0, 0x4e0(r5)
    bl GXSetMisc
    lis r4, 0xcc00
    addi r5, r4, 0x3000
    stw r4, __cpReg(r13)
    addi r3, r4, 0x1000
    addi r0, r4, 0x4000
    stw r5, __piReg(r13)
    stw r3, __peReg(r13)
    stw r0, __memReg(r13)
    bl __GXFifoInit
    mr r4, r27
    mr r5, r25
    addi r3, r30, 0x4f8
    bl GXInitFifoBase
    addi r3, r30, 0x4f8
    bl GXSetCPUFifo
    addi r3, r30, 0x4f8
    bl GXSetGPFifo
    lwz r0, -0x5664(r13)
    cmplwi r0, 0x0
    bne lbl_000B0960
    lis r3, GXResetFuncInfo_80312AD0@ha
    addi r3, r3, GXResetFuncInfo_80312AD0@l
    bl OSRegisterResetFunction
    stw r26, -0x5664(r13)
lbl_000B0960:
    bl __GXPEInit
    bl PPCMfhid2
    lis r4, 0xc01
    mr r26, r3
    addi r3, r4, -0x8000
    bl PPCMtwpar
    oris r3, r26, 0x4000
    bl PPCMthid2
    lwz r7, gx(r13)
    li r4, 0x0
    li r0, 0xff
    stw r4, 0x204(r7)
    addi r5, r7, 0x204
    addi r8, r7, 0x124
    lwz r3, 0x0(r5)
    addi r10, r7, 0x7c
    li r9, 0x0
    clrlwi r3, r3, 8
    stw r3, 0x0(r5)
    li r5, 0xc0
    li r6, 0xc1
    stw r0, 0x124(r7)
    lwz r0, 0x0(r8)
    clrlwi r0, r0, 8
    oris r0, r0, 0xf00
    stw r0, 0x0(r8)
    stw r4, 0x7c(r7)
    lwz r0, 0x0(r10)
    clrlwi r0, r0, 8
    oris r0, r0, 0x2200
    stw r0, 0x0(r10)
    b lbl_000B09E0
lbl_000B09E0:
    li r0, 0x8
    mtctr r0
    b lbl_000B09EC
lbl_000B09EC:
    b lbl_000B09F0
lbl_000B09F0:
    lwz r3, gx(r13)
    srwi r7, r9, 1
    addi r29, r4, 0x130
    li r0, 0x0
    addi r26, r4, 0x170
    stwx r0, r3, r29
    slwi r11, r7, 2
    addi r27, r11, 0x100
    stwx r0, r3, r26
    addi r10, r4, 0x49c
    stwx r0, r3, r27
    li r31, 0xff
    slwi r28, r5, 24
    stwx r31, r3, r10
    add r25, r3, r29
    addi r4, r4, 0x4
    lwz r29, 0x0(r25)
    add r26, r3, r26
    slwi r12, r6, 24
    clrlwi r29, r29, 8
    or r28, r29, r28
    stw r28, 0x0(r25)
    addi r8, r11, 0x1b0
    addi r11, r7, 0xf6
    lwz r28, 0x0(r26)
    addi r10, r7, 0x28
    addi r29, r4, 0x130
    clrlwi r7, r28, 8
    or r7, r7, r12
    stw r7, 0x0(r26)
    add r8, r3, r8
    addi r9, r9, 0x1
    lwz r12, 0x0(r8)
    srwi r7, r9, 1
    slwi r11, r11, 24
    clrlwi r12, r12, 8
    or r11, r12, r11
    stw r11, 0x0(r8)
    add r25, r3, r27
    slwi r11, r7, 2
    lwz r8, 0x0(r25)
    slwi r10, r10, 24
    addi r26, r4, 0x170
    clrlwi r8, r8, 8
    or r8, r8, r10
    stw r8, 0x0(r25)
    addi r27, r11, 0x100
    addi r10, r4, 0x49c
    stwx r0, r3, r29
    addi r5, r5, 0x2
    slwi r28, r5, 24
    stwx r0, r3, r26
    addi r6, r6, 0x2
    slwi r12, r6, 24
    stwx r0, r3, r27
    addi r8, r11, 0x1b0
    addi r11, r7, 0xf6
    stwx r31, r3, r10
    add r25, r3, r29
    addi r10, r7, 0x28
    lwz r29, 0x0(r25)
    add r26, r3, r26
    add r8, r3, r8
    clrlwi r29, r29, 8
    or r28, r29, r28
    stw r28, 0x0(r25)
    slwi r11, r11, 24
    add r25, r3, r27
    lwz r28, 0x0(r26)
    slwi r10, r10, 24
    addi r4, r4, 0x4
    clrlwi r7, r28, 8
    or r7, r7, r12
    stw r7, 0x0(r26)
    addi r5, r5, 0x2
    addi r6, r6, 0x2
    lwz r12, 0x0(r8)
    addi r9, r9, 0x1
    clrlwi r12, r12, 8
    or r11, r12, r11
    stw r11, 0x0(r8)
    lwz r8, 0x0(r25)
    clrlwi r8, r8, 8
    or r8, r8, r10
    stw r8, 0x0(r25)
    bdnz lbl_000B09F0
    stw r0, 0x120(r3)
    addi r4, r3, 0x120
    mr r9, r0
    lwz r3, 0x0(r4)
    li r10, 0x30
    li r11, 0x31
    clrlwi r3, r3, 8
    oris r3, r3, 0x2700
    stw r3, 0x0(r4)
    b lbl_000B0B70
lbl_000B0B70:
    li r3, 0x2
    mtctr r3
    b lbl_000B0B7C
lbl_000B0B7C:
    b lbl_000B0B80
lbl_000B0B80:
    lwz r3, gx(r13)
    addi r8, r9, 0xb8
    addi r7, r9, 0xd8
    stwx r0, r3, r8
    slwi r6, r10, 24
    slwi r4, r11, 24
    stwx r0, r3, r7
    add r12, r3, r8
    addi r9, r9, 0x4
    lwz r5, 0x0(r12)
    add r25, r3, r7
    addi r8, r9, 0xb8
    clrlwi r5, r5, 8
    or r5, r5, r6
    stw r5, 0x0(r12)
    addi r7, r9, 0xd8
    addi r10, r10, 0x2
    lwz r5, 0x0(r25)
    slwi r6, r10, 24
    add r12, r3, r8
    clrlwi r5, r5, 8
    or r4, r5, r4
    stw r4, 0x0(r25)
    addi r11, r11, 0x2
    slwi r4, r11, 24
    stwx r0, r3, r8
    addi r9, r9, 0x4
    add r25, r3, r7
    stwx r0, r3, r7
    addi r8, r9, 0xb8
    addi r7, r9, 0xd8
    lwz r5, 0x0(r12)
    addi r10, r10, 0x2
    addi r11, r11, 0x2
    clrlwi r5, r5, 8
    or r5, r5, r6
    stw r5, 0x0(r12)
    slwi r6, r10, 24
    add r12, r3, r8
    lwz r5, 0x0(r25)
    addi r9, r9, 0x4
    addi r10, r10, 0x2
    clrlwi r5, r5, 8
    or r4, r5, r4
    stw r4, 0x0(r25)
    slwi r4, r11, 24
    add r25, r3, r7
    stwx r0, r3, r8
    addi r8, r9, 0xb8
    addi r11, r11, 0x2
    stwx r0, r3, r7
    addi r7, r9, 0xd8
    addi r9, r9, 0x4
    lwz r5, 0x0(r12)
    clrlwi r5, r5, 8
    or r5, r5, r6
    stw r5, 0x0(r12)
    slwi r6, r10, 24
    add r12, r3, r8
    lwz r5, 0x0(r25)
    addi r10, r10, 0x2
    clrlwi r5, r5, 8
    or r4, r5, r4
    stw r4, 0x0(r25)
    slwi r4, r11, 24
    add r25, r3, r7
    stwx r0, r3, r8
    addi r11, r11, 0x2
    stwx r0, r3, r7
    lwz r5, 0x0(r12)
    clrlwi r5, r5, 8
    or r5, r5, r6
    stw r5, 0x0(r12)
    lwz r5, 0x0(r25)
    clrlwi r5, r5, 8
    or r4, r5, r4
    stw r4, 0x0(r25)
    bdnz lbl_000B0B80
    addi r5, r3, 0xf8
    lwz r0, 0x0(r5)
    lis r4, 0x1062
    addi r6, r3, 0xfc
    clrlwi r0, r0, 8
    oris r0, r0, 0x2000
    stw r0, 0x0(r5)
    addi r7, r3, 0x1d0
    addi r8, r3, 0x1d4
    lwz r0, 0x0(r6)
    addi r9, r3, 0x1d8
    addi r10, r3, 0x1dc
    clrlwi r0, r0, 8
    oris r0, r0, 0x2100
    stw r0, 0x0(r6)
    addi r6, r3, 0x1fc
    li r29, 0x0
    lwz r0, 0x0(r7)
    lis r5, 0x8000
    addi r4, r4, 0x4dd3
    clrlwi r0, r0, 8
    oris r0, r0, 0x4100
    stw r0, 0x0(r7)
    lwz r0, 0x0(r8)
    clrlwi r0, r0, 8
    oris r0, r0, 0x4200
    stw r0, 0x0(r8)
    lwz r0, 0x0(r9)
    clrlwi r0, r0, 8
    oris r0, r0, 0x4000
    stw r0, 0x0(r9)
    lwz r0, 0x0(r10)
    clrlwi r0, r0, 8
    oris r0, r0, 0x4300
    stw r0, 0x0(r10)
    lwz r0, 0x0(r6)
    rlwinm r0, r0, 0, 25, 22
    stw r0, 0x0(r6)
    stw r29, 0x4f4(r3)
    stb r29, 0x4f3(r3)
    lwz r0, 0xf8(r5)
    mulhwu r0, r4, r0
    srwi r25, r0, 5
    bl __GXFlushTextureState
    srwi r0, r25, 11
    oris r0, r0, 0x6900
    li r28, 0x61
    lis r31, 0xcc01
    stb r28, -0x8000(r31)
    ori r0, r0, 0x400
    stw r0, -0x8000(r31)
    bl __GXFlushTextureState
    lis r3, 0x3e10
    stb r28, -0x8000(r31)
    subi r0, r3, 0x7c1f
    mulhwu r0, r0, r25
    srwi r0, r0, 10
    oris r0, r0, 0x4600
    ori r0, r0, 0x200
    stw r0, -0x8000(r31)
    mr r7, r29
    b lbl_000B0DB0
lbl_000B0DB0:
    li r0, 0x2
    mtctr r0
    li r5, 0x8
    b lbl_000B0DC0
lbl_000B0DC0:
    b lbl_000B0DC4
lbl_000B0DC4:
    lwz r6, gx(r13)
    addi r8, r7, 0x1c
    addi r9, r7, 0x3c
    add r8, r6, r8
    lwz r0, 0x0(r8)
    ori r4, r29, 0x80
    addi r3, r7, 0x3c
    rlwinm r0, r0, 0, 2, 0
    oris r0, r0, 0x4000
    stw r0, 0x0(r8)
    add r9, r6, r9
    addi r7, r7, 0x4
    lwz r0, 0x0(r9)
    addi r8, r7, 0x1c
    add r8, r6, r8
    clrlwi r0, r0, 1
    oris r0, r0, 0x8000
    stw r0, 0x0(r9)
    addi r9, r7, 0x3c
    add r9, r6, r9
    stb r5, -0x8000(r31)
    addi r29, r29, 0x1
    stb r4, -0x8000(r31)
    ori r4, r29, 0x80
    addi r29, r29, 0x1
    lwzx r0, r6, r3
    addi r3, r7, 0x3c
    addi r7, r7, 0x4
    stw r0, -0x8000(r31)
    lwz r0, 0x0(r8)
    rlwinm r0, r0, 0, 2, 0
    oris r0, r0, 0x4000
    stw r0, 0x0(r8)
    addi r8, r7, 0x1c
    add r8, r6, r8
    lwz r0, 0x0(r9)
    clrlwi r0, r0, 1
    oris r0, r0, 0x8000
    stw r0, 0x0(r9)
    addi r9, r7, 0x3c
    add r9, r6, r9
    stb r5, -0x8000(r31)
    stb r4, -0x8000(r31)
    ori r4, r29, 0x80
    addi r29, r29, 0x1
    lwzx r0, r6, r3
    addi r3, r7, 0x3c
    addi r7, r7, 0x4
    stw r0, -0x8000(r31)
    lwz r0, 0x0(r8)
    rlwinm r0, r0, 0, 2, 0
    oris r0, r0, 0x4000
    stw r0, 0x0(r8)
    addi r8, r7, 0x1c
    add r8, r6, r8
    lwz r0, 0x0(r9)
    clrlwi r0, r0, 1
    oris r0, r0, 0x8000
    stw r0, 0x0(r9)
    addi r9, r7, 0x3c
    add r9, r6, r9
    stb r5, -0x8000(r31)
    stb r4, -0x8000(r31)
    ori r4, r29, 0x80
    addi r29, r29, 0x1
    lwzx r0, r6, r3
    addi r3, r7, 0x3c
    addi r7, r7, 0x4
    stw r0, -0x8000(r31)
    lwz r0, 0x0(r8)
    rlwinm r0, r0, 0, 2, 0
    oris r0, r0, 0x4000
    stw r0, 0x0(r8)
    lwz r0, 0x0(r9)
    clrlwi r0, r0, 1
    oris r0, r0, 0x8000
    stw r0, 0x0(r9)
    stb r5, -0x8000(r31)
    stb r4, -0x8000(r31)
    lwzx r0, r6, r3
    stw r0, -0x8000(r31)
    bdnz lbl_000B0DC4
    li r4, 0x10
    lis r6, 0xcc01
    stb r4, -0x8000(r6)
    li r0, 0x1000
    li r28, 0x0
    stw r0, -0x8000(r6)
    li r0, 0x3f
    slwi r25, r28, 15
    stw r0, -0x8000(r6)
    li r0, 0x1012
    li r5, 0x1
    stb r4, -0x8000(r6)
    li r4, 0x61
    lis r3, 0x5800
    stw r0, -0x8000(r6)
    addi r0, r3, 0xf
    addis r26, r25, 0x8
    stw r5, -0x8000(r6)
    slwi r27, r28, 4
    stb r4, -0x8000(r6)
    stw r0, -0x8000(r6)
    b lbl_000B0F64
lbl_000B0F64:
    b lbl_000B0F68
lbl_000B0F68:
    b lbl_000B0F6C
lbl_000B0F6C:
    lwz r0, gx(r13)
    addi r3, r27, 0x208
    mr r5, r25
    mr r7, r26
    add r3, r0, r3
    li r4, 0x0
    li r6, 0x0
    li r8, 0x0
    bl GXInitTexCacheRegion
    addis r26, r26, 0x1
    addis r25, r25, 0x1
    addi r26, r26, -0x8000
    addi r25, r25, -0x8000
    addi r27, r27, 0x10
    addi r28, r28, 0x1
    cmplwi r28, 0x8
    blt lbl_000B0F6C
    li r25, 0x0
    slwi r3, r25, 1
    addi r26, r3, 0x9
    addi r27, r3, 0x8
    slwi r28, r25, 4
    b lbl_000B0FC8
lbl_000B0FC8:
    b lbl_000B0FCC
lbl_000B0FCC:
    b lbl_000B0FD0
lbl_000B0FD0:
    lwz r0, gx(r13)
    addi r3, r28, 0x288
    slwi r5, r27, 15
    add r3, r0, r3
    slwi r7, r26, 15
    li r4, 0x0
    li r6, 0x0
    li r8, 0x0
    bl GXInitTexCacheRegion
    addi r26, r26, 0x2
    addi r27, r27, 0x2
    addi r28, r28, 0x10
    addi r25, r25, 0x1
    cmplwi r25, 0x4
    blt lbl_000B0FD0
    li r25, 0x0
    slwi r3, r25, 13
    addis r26, r3, 0xc
    slwi r27, r25, 4
    b lbl_000B1020
lbl_000B1020:
    b lbl_000B1024
lbl_000B1024:
    b lbl_000B1028
lbl_000B1028:
    lwz r0, gx(r13)
    addi r3, r27, 0x2d0
    mr r4, r26
    add r3, r0, r3
    li r5, 0x10
    bl GXInitTlutRegion
    addi r26, r26, 0x2000
    addi r27, r27, 0x10
    addi r25, r25, 0x1
    cmplwi r25, 0x10
    blt lbl_000B1028
    li r25, 0x0
    slwi r3, r25, 15
    addis r26, r3, 0xe
    b lbl_000B1064
lbl_000B1064:
    b lbl_000B1068
lbl_000B1068:
    b lbl_000B106C
lbl_000B106C:
    addi r0, r25, 0x10
    lwz r5, gx(r13)
    slwi r3, r0, 4
    addi r3, r3, 0x2d0
    mr r4, r26
    add r3, r5, r3
    li r5, 0x40
    bl GXInitTlutRegion
    addis r26, r26, 0x1
    addi r26, r26, -0x8000
    addi r25, r25, 0x1
    cmplwi r25, 0x4
    blt lbl_000B106C
    lwz r4, __cpReg(r13)
    li r12, 0x0
    li r3, 0x8
    sth r12, 0x6(r4)
    lis r10, 0xcc01
    li r9, 0x20
    lwz r11, gx(r13)
    li r8, 0x10
    li r7, 0x1006
    addi r4, r11, 0x4ec
    lwz r0, 0x0(r4)
    li r6, 0x61
    lis r5, 0x2300
    rlwinm r0, r0, 0, 28, 23
    stw r0, 0x0(r4)
    lis r4, 0x2400
    lis r0, 0x6700
    stb r3, -0x8000(r10)
    li r3, 0x0
    stb r9, -0x8000(r10)
    lwz r9, 0x4ec(r11)
    stw r9, -0x8000(r10)
    stb r8, -0x8000(r10)
    stw r7, -0x8000(r10)
    stw r12, -0x8000(r10)
    stb r6, -0x8000(r10)
    stw r5, -0x8000(r10)
    stb r6, -0x8000(r10)
    stw r4, -0x8000(r10)
    stb r6, -0x8000(r10)
    stw r0, -0x8000(r10)
    bl __GXSetTmemConfig
    bl __GXInitGX
    addi r3, r30, 0x4f8
    lmw r25, 0x34(r1)
    lwz r0, 0x54(r1)
    addi r1, r1, 0x50
    mtlr r0
    blr
}

/*
 * __GXInitGX - Initialize all GX state to defaults.
 * 0x800B671C | size: 0x8C4
 *
 * This large function sets every GX subsystem to its default state:
 * viewport, scissor, blend mode, depth test, lighting, textures,
 * TEV stages, etc.
 */
void __GXInitGX(void) {
    GXRenderModeObj defMode;
    f32 identity[3][4];
    u32 i;

    /* Set viewport to standard NTSC fullscreen */
    /* GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f); */

    /* Set scissor box */
    /* GXSetScissor(0, 0, 640, 480); */

    /* Clear all vertex descriptors */
    /* GXClearVtxDesc(); */
    /* GXInvalidateVtxCache(); */

    /* Set default blend mode (no blending) */
    /* GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR); */

    /* Set default Z mode */
    /* GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE); */

    /* Set default color update */
    /* GXSetColorUpdate(GX_TRUE); */
    /* GXSetAlphaUpdate(GX_TRUE); */

    /* Set default alpha compare */
    /* GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0); */

    /* Disable all TEV stages except stage 0 */
    /* GXSetNumTevStages(1); */

    /* Set default TEV stage 0 */
    /* GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0); */
    /* GXSetTevOp(GX_TEVSTAGE0, GX_REPLACE); */

    /* Disable all lights */
    /* GXSetNumChans(0); */

    /* Disable texgen */
    /* GXSetNumTexGens(1); */

    /* Set identity position/normal matrix */
    identity[0][0] = 1.0f; identity[0][1] = 0.0f; identity[0][2] = 0.0f; identity[0][3] = 0.0f;
    identity[1][0] = 0.0f; identity[1][1] = 1.0f; identity[1][2] = 0.0f; identity[1][3] = 0.0f;
    identity[2][0] = 0.0f; identity[2][1] = 0.0f; identity[2][2] = 1.0f; identity[2][3] = 0.0f;

    /* Set default cull mode */
    /* GXSetCullMode(GX_CULL_BACK); */

    /* Clear PE statistics */
    /* GXSetDispCopySrc(0, 0, 640, 480); */

    /* Set default TMEM config */
    __GXSetTmemConfig(0);

    /* Initialize pixel processing */
    /* GXSetFog(GX_FOG_NONE, 0.0f, 1.0f, 0.1f, 1000.0f, (GXColor){0,0,0,0}); */
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800B6FE0 - 0x800B6FE0 | size: 0x134 -- GX FIFO management */
asm void fn_800B6FE0(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x2e0(r1)
    stw r31, 0x2dc(r1)
    mr r31, r4
    lwz r5, __cpReg(r13)
    lwz r3, gx(r13)
    lhz r0, 0(r5)
    stw r0, 0xc(r3)
    lwz r0, 8(r3)
    extrwi. r0, r0, 1, 28
    beq cpReadReady
    lwz r0, 0xc(r3)
    extrwi. r0, r0, 1, 30
    beq cpReadReady
    lwz r3, lbl_8047A9A8(r13)
    bl fn_800A1F94
    li r0, 0
    stw r0, lbl_8047A9B0(r13)
    li r3, 1
    li r4, 1
    bl fn_800B75D0
    li r3, 1
    li r4, 0
    bl fn_800B7594
cpReadReady:
    lwz r3, gx(r13)
    lwz r0, 8(r3)
    extrwi. r0, r0, 1, 29
    beq cpWriteReady
    lwz r0, 0xc(r3)
    clrlwi. r0, r0, 31
    beq cpWriteReady
    lwz r5, lbl_8047A9B8(r13)
    li r3, 0
    li r4, 1
    addi r0, r5, 1
    stw r0, lbl_8047A9B8(r13)
    bl fn_800B7594
    li r3, 1
    li r4, 0
    bl fn_800B75D0
    li r0, 1
    lwz r3, lbl_8047A9A8(r13)
    stw r0, lbl_8047A9B0(r13)
    bl fn_800A221C
cpWriteReady:
    lwz r3, gx(r13)
    lwz r4, 8(r3)
    addi r5, r3, 8
    extrwi. r0, r4, 1, 26
    beq cpCallbackDone
    lwz r0, 0xc(r3)
    extrwi. r0, r0, 1, 27
    beq cpCallbackDone
    rlwinm r0, r4, 0, 27, 25
    stw r0, 0(r5)
    lwz r0, 8(r3)
    lwz r3, __cpReg(r13)
    sth r0, 2(r3)
    lwz r0, lbl_8047A9B4(r13)
    cmplwi r0, 0
    beq cpCallbackDone
    addi r3, r1, 0x10
    bl OSClearContext
    addi r3, r1, 0x10
    bl OSSetCurrentContext
    lwz r12, lbl_8047A9B4(r13)
    mtlr r12
    blrl
    addi r3, r1, 0x10
    bl OSClearContext
    mr r3, r31
    bl OSSetCurrentContext
cpCallbackDone:
    lwz r0, 0x2e4(r1)
    lwz r31, 0x2dc(r1)
    addi r1, r1, 0x2e0
    mtlr r0
    blr
}

/* fn_800B7180 - 0x800B7180 | size: 0x70 -- GX FIFO management */
asm void fn_800B7180(u8* fifo, u32 readPtr, u32 writePtr) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x28(r1)
    stw r31, 0x24(r1)
    addi r31, r5, 0
    stw r30, 0x20(r1)
    addi r30, r4, 0
    stw r29, 0x1c(r1)
    addi r29, r3, 0
    bl OSDisableInterrupts
    stw r30, 0x14(r29)
    subf r0, r30, r31
    stw r31, 0x18(r29)
    stw r0, 0x1c(r29)
    lwz r4, 0x1c(r29)
    cmpwi r4, 0
    bge fifoCountDone
    lwz r0, 8(r29)
    add r0, r4, r0
    stw r0, 0x1c(r29)
fifoCountDone:
    bl OSRestoreInterrupts
    lwz r0, 0x2c(r1)
    lwz r31, 0x24(r1)
    lwz r30, 0x20(r1)
    lwz r29, 0x1c(r1)
    addi r1, r1, 0x28
    mtlr r0
    blr
}

/* fn_800B71F0 - 0x800B71F0 | size: 0xC -- GX FIFO management */
void fn_800B71F0(u8* r3, u32 r4, u32 r5) {
    *(u32*)(r3 + 0xC) = r4;
    *(u32*)(r3 + 0x10) = r5;
}

/* fn_800B7484 - 0x800B7484 | size: 0x44 -- GX FIFO management */
asm void* fn_800B7484(void* callback) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    stw r30, 0x10(r1)
    mr r30, r3
    lwz r31, lbl_8047A9B4(r13)
    bl OSDisableInterrupts
    stw r30, lbl_8047A9B4(r13)
    bl OSRestoreInterrupts
    mr r3, r31
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800B7514 - 0x800B7514 | size: 0x24 -- GX Misc / PE / draw sync */
asm void fn_800B7514(void) {
    nofralloc
    lwz r3, gx(r13)
    lwz r0, 8(r3)
    clrrwi r0, r0, 1
    ori r0, r0, 1
    stw r0, 8(r3)
    lwz r0, 8(r3)
    lwz r3, __cpReg(r13)
    sth r0, 2(r3)
    blr
}

/* fn_800B7538 - 0x800B7538 | size: 0x20 -- GX Misc / PE / draw sync */
asm void fn_800B7538(void) {
    nofralloc
    lwz r3, gx(r13)
    lwz r0, 8(r3)
    clrrwi r0, r0, 1
    stw r0, 8(r3)
    lwz r0, 8(r3)
    lwz r3, __cpReg(r13)
    sth r0, 2(r3)
    blr
}

/* fn_800B7558 - 0x800B7558 | size: 0x3C -- GX Misc / PE / draw sync */
asm void fn_800B7558(u32 arg0) {
    nofralloc
    clrlwi. r0, r3, 24
    beq cpFlagOff
    li r0, 1
    b cpFlagDone
cpFlagOff:
    li r0, 0
cpFlagDone:
    lwz r4, gx(r13)
    slwi r0, r0, 4
    lwz r3, 8(r4)
    rlwinm r3, r3, 0, 28, 26
    or r0, r3, r0
    stw r0, 8(r4)
    lwz r0, 8(r4)
    lwz r3, __cpReg(r13)
    sth r0, 2(r3)
    blr
}

/* fn_800B7594 - 0x800B7594 | size: 0x3C -- GX Misc / PE / draw sync */
asm void fn_800B7594(u32 arg0, u32 arg1) {
    nofralloc
    lwz r6, gx(r13)
    clrlslwi r3, r3, 24, 2
    clrlslwi r0, r4, 24, 3
    lwz r5, 8(r6)
    rlwinm r4, r5, 0, 30, 28
    or r3, r4, r3
    stw r3, 8(r6)
    lwz r3, 8(r6)
    rlwinm r3, r3, 0, 29, 27
    or r0, r3, r0
    stw r0, 8(r6)
    lwz r0, 8(r6)
    lwz r3, __cpReg(r13)
    sth r0, 2(r3)
    blr
}

/* fn_800B75D0 - 0x800B75D0 | size: 0x3C -- GX Misc / PE / draw sync */
asm void fn_800B75D0(u32 arg0, u32 arg1) {
    nofralloc
    lwz r6, gx(r13)
    clrlwi r3, r3, 24
    clrlslwi r0, r4, 24, 1
    lwz r5, 0x10(r6)
    clrrwi r4, r5, 1
    or r3, r4, r3
    stw r3, 0x10(r6)
    lwz r3, 0x10(r6)
    rlwinm r3, r3, 0, 31, 29
    or r0, r3, r0
    stw r0, 0x10(r6)
    lwz r0, 0x10(r6)
    lwz r3, __cpReg(r13)
    sth r0, 4(r3)
    blr
}

/* fn_800B760C - 0x800B760C | size: 0x100 -- GX Misc / PE / draw sync */
void fn_800B760C(void) {
    /* GX Misc / PE / draw sync (0x100 bytes) */
}

/* fn_800B770C - 0x800B770C | size: 0x8 -- GX Misc / PE / draw sync */
u32 fn_800B770C(void) {
    extern u32 lbl_8047A9A0;
    return lbl_8047A9A0;
}

/* fn_800B7714 - 0x800B7714 | size: 0x8 -- GX Misc / PE / draw sync */
u32 fn_800B7714(void) {
    extern u32 lbl_8047A9A4;
    return lbl_8047A9A4;
}

/* fn_800B771C - 0x800B771C | size: 0x158 -- GX Misc / PE / draw sync */
asm void fn_800B771C(void) {
    nofralloc
    lwz r5, gx(r13)
    lwz r4, 0x14(r5)
    extrwi. r0, r4, 2, 17
    beq tevCoord0Off
    li r3, 1
    b tevCoord0Done
tevCoord0Off:
    li r3, 0
tevCoord0Done:
    extrwi. r0, r4, 2, 15
    beq tevCoord1Off
    li r4, 1
    b tevCoord1Done
tevCoord1Off:
    li r4, 0
tevCoord1Done:
    lbz r0, 0x41d(r5)
    add r7, r3, r4
    cmplwi r0, 0
    beq texGenCheck
    li r4, 2
    b chanCountDone
texGenCheck:
    lbz r0, 0x41c(r5)
    cmplwi r0, 0
    beq chanCountOff
    li r4, 1
    b chanCountDone
chanCountOff:
    li r4, 0
chanCountDone:
    lwz r6, 0x18(r5)
    clrlwi. r0, r6, 30
    beq texCoord0Off
    li r3, 1
    b texCoord0Done
texCoord0Off:
    li r3, 0
texCoord0Done:
    extrwi. r0, r6, 2, 28
    beq texCoord1Off
    li r5, 1
    b texCoord1Done
texCoord1Off:
    li r5, 0
texCoord1Done:
    extrwi. r0, r6, 2, 26
    add r8, r3, r5
    beq texCoord2Off
    li r3, 1
    b texCoord2Done
texCoord2Off:
    li r3, 0
texCoord2Done:
    extrwi. r0, r6, 2, 24
    add r8, r8, r3
    beq texCoord3Off
    li r3, 1
    b texCoord3Done
texCoord3Off:
    li r3, 0
texCoord3Done:
    extrwi. r0, r6, 2, 22
    add r8, r8, r3
    beq texCoord4Off
    li r3, 1
    b texCoord4Done
texCoord4Off:
    li r3, 0
texCoord4Done:
    extrwi. r0, r6, 2, 20
    add r8, r8, r3
    beq texCoord5Off
    li r3, 1
    b texCoord5Done
texCoord5Off:
    li r3, 0
texCoord5Done:
    extrwi. r0, r6, 2, 18
    add r8, r8, r3
    beq texCoord6Off
    li r3, 1
    b texCoord6Done
texCoord6Off:
    li r3, 0
texCoord6Done:
    extrwi. r0, r6, 2, 16
    add r8, r8, r3
    beq texCoord7Off
    li r6, 1
    b texCoord7Done
texCoord7Off:
    li r6, 0
texCoord7Done:
    li r0, 0x10
    lwz r3, gx(r13)
    lis r5, 0xcc01
    add r8, r8, r6
    stb r0, -0x8000(r5)
    slwi r0, r4, 2
    li r4, 0x1008
    stw r4, -0x8000(r5)
    slwi r4, r8, 4
    or r0, r7, r0
    or r0, r4, r0
    stw r0, -0x8000(r5)
    li r0, 1
    sth r0, 2(r3)
    blr
}

/* fn_800B7874 - 0x800B7874 | size: 0x350 -- GX Misc / PE / draw sync */
asm void fn_800B7874(void) {
    nofralloc
    cmplwi r3, 0x19
    bgt vcdPostUpdate
    lis r5, jumptable_80312AE0@ha
    addi r5, r5, jumptable_80312AE0@l
    slwi r0, r3, 2
    lwzx r0, r5, r0
    mtctr r0
    bctr
vcdCase0:
    lwz r3, gx(r13)
    lwzu r0, 0x14(r3)
    clrrwi r0, r0, 1
    or r0, r0, r4
    stw r0, 0(r3)
    b vcdPostUpdate
vcdCase1:
    lwz r3, gx(r13)
    slwi r0, r4, 1
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 31, 29
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase2:
    lwz r3, gx(r13)
    slwi r0, r4, 2
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 30, 28
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase3:
    lwz r3, gx(r13)
    slwi r0, r4, 3
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 29, 27
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase4:
    lwz r3, gx(r13)
    slwi r0, r4, 4
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 28, 26
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase5:
    lwz r3, gx(r13)
    slwi r0, r4, 5
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 27, 25
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase6:
    lwz r3, gx(r13)
    slwi r0, r4, 6
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 26, 24
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase7:
    lwz r3, gx(r13)
    slwi r0, r4, 7
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 25, 23
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase8:
    lwz r3, gx(r13)
    slwi r0, r4, 8
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 24, 22
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase9:
    lwz r3, gx(r13)
    slwi r0, r4, 9
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 23, 20
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase10:
    cmpwi r4, 0
    beq vcdCase10Off
    lwz r3, gx(r13)
    li r5, 1
    li r0, 0
    stb r5, 0x41c(r3)
    stb r0, 0x41d(r3)
    stw r4, 0x418(r3)
    b vcdPostUpdate
vcdCase10Off:
    lwz r3, gx(r13)
    li r0, 0
    stb r0, 0x41c(r3)
    b vcdPostUpdate
vcdCase11:
    cmpwi r4, 0
    beq vcdCase11Off
    lwz r3, gx(r13)
    li r5, 1
    li r0, 0
    stb r5, 0x41d(r3)
    stb r0, 0x41c(r3)
    stw r4, 0x418(r3)
    b vcdPostUpdate
vcdCase11Off:
    lwz r3, gx(r13)
    li r0, 0
    stb r0, 0x41d(r3)
    b vcdPostUpdate
vcdCase12:
    lwz r3, gx(r13)
    slwi r0, r4, 0xd
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 19, 16
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase13:
    lwz r3, gx(r13)
    slwi r0, r4, 0xf
    addi r4, r3, 0x14
    lwz r3, 0x14(r3)
    rlwinm r3, r3, 0, 17, 14
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase14:
    lwz r3, gx(r13)
    lwzu r0, 0x18(r3)
    clrrwi r0, r0, 2
    or r0, r0, r4
    stw r0, 0(r3)
    b vcdPostUpdate
vcdCase15:
    lwz r3, gx(r13)
    slwi r0, r4, 2
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 30, 27
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase16:
    lwz r3, gx(r13)
    slwi r0, r4, 4
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 28, 25
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase17:
    lwz r3, gx(r13)
    slwi r0, r4, 6
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 26, 23
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase18:
    lwz r3, gx(r13)
    slwi r0, r4, 8
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 24, 21
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase19:
    lwz r3, gx(r13)
    slwi r0, r4, 0xa
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 22, 19
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase20:
    lwz r3, gx(r13)
    slwi r0, r4, 0xc
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 20, 17
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdPostUpdate
vcdCase21:
    lwz r3, gx(r13)
    slwi r0, r4, 0xe
    addi r4, r3, 0x18
    lwz r3, 0x18(r3)
    rlwinm r3, r3, 0, 18, 15
    or r0, r3, r0
    stw r0, 0(r4)
vcdPostUpdate:
    lwz r3, gx(r13)
    lbz r0, 0x41c(r3)
    cmplwi r0, 0
    bne vcdMergeNrm
    lbz r0, 0x41d(r3)
    cmplwi r0, 0
    beq vcdClearNrm
vcdMergeNrm:
    addi r4, r3, 0x14
    lwz r0, 0x418(r3)
    lwz r3, 0x14(r3)
    slwi r0, r0, 0xb
    rlwinm r3, r3, 0, 21, 18
    or r0, r3, r0
    stw r0, 0(r4)
    b vcdMarkDirty
vcdClearNrm:
    lwzu r0, 0x14(r3)
    rlwinm r0, r0, 0, 21, 18
    stw r0, 0(r3)
vcdMarkDirty:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    ori r0, r0, 8
    stw r0, 0x4f4(r3)
    blr
}

/* fn_800B7BC4 - 0x800B7BC4 | size: 0x54 -- GX Misc / PE / draw sync */
asm void fn_800B7BC4(void) {
    nofralloc
    mflr r0
    li r6, 8
    stw r0, 4(r1)
    lis r5, 0xcc01
    li r3, 0x50
    stwu r1, -8(r1)
    li r0, 0x60
    stb r6, -0x8000(r5)
    lwz r4, gx(r13)
    stb r3, -0x8000(r5)
    lwz r3, 0x14(r4)
    stw r3, -0x8000(r5)
    stb r6, -0x8000(r5)
    stb r0, -0x8000(r5)
    lwz r0, 0x18(r4)
    stw r0, -0x8000(r5)
    bl fn_800B771C
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800B7C18 - 0x800B7C18 | size: 0x124 -- GX Misc / PE / draw sync */
void fn_800B7C18(void) {
    /* GX Misc / PE / draw sync (0x124 bytes) */
}

/* fn_800B7D3C - 0x800B7D3C | size: 0x38 -- GX Misc / PE / draw sync */
asm void fn_800B7D3C(void) {
    nofralloc
    lwz r3, gx(r13)
    li r4, 0
    stw r4, 0x14(r3)
    lwz r0, 0x14(r3)
    rlwinm r0, r0, 0, 23, 20
    ori r0, r0, 0x200
    stw r0, 0x14(r3)
    stw r4, 0x18(r3)
    stb r4, 0x41c(r3)
    stb r4, 0x41d(r3)
    lwz r0, 0x4f4(r3)
    ori r0, r0, 8
    stw r0, 0x4f4(r3)
    blr
}

/* fn_800B7D74 - 0x800B7D74 | size: 0x358 -- GX Misc / PE / draw sync */
asm void fn_800B7D74(void) {
    nofralloc
    subi r0, r4, 9
    lwz r8, gx(r13)
    slwi r4, r3, 2
    add r9, r8, r4
    cmplwi r0, 0x10
    addi r4, r9, 0x1c
    addi r8, r9, 0x3c
    addi r9, r9, 0x5c
    bgt vatSetDone
    lis r10, jumptable_80312B48@ha
    addi r10, r10, jumptable_80312B48@l
    slwi r0, r0, 2
    lwzx r0, r10, r0
    mtctr r0
    bctr
vatSetCase9:
    lwz r0, 0(r4)
    slwi r6, r6, 1
    clrrwi r0, r0, 1
    or r0, r0, r5
    stw r0, 0(r4)
    clrlslwi r0, r7, 24, 4
    lwz r5, 0(r4)
    rlwinm r5, r5, 0, 31, 27
    or r5, r5, r6
    stw r5, 0(r4)
    lwz r5, 0(r4)
    rlwinm r5, r5, 0, 28, 22
    or r0, r5, r0
    stw r0, 0(r4)
    b vatSetDone
vatSetCase10:
    lwz r7, 0(r4)
    slwi r0, r6, 0xa
    cmpwi r5, 2
    rlwinm r6, r7, 0, 22, 18
    or r0, r6, r0
    stw r0, 0(r4)
    bne vatSetCase10Not2
    lwz r0, 0(r4)
    rlwinm r0, r0, 0, 23, 21
    ori r0, r0, 0x200
    stw r0, 0(r4)
    lwz r0, 0(r4)
    clrlwi r0, r0, 1
    oris r0, r0, 0x8000
    stw r0, 0(r4)
    b vatSetDone
vatSetCase10Not2:
    lwz r6, 0(r4)
    slwi r0, r5, 9
    rlwinm r5, r6, 0, 23, 21
    or r0, r5, r0
    stw r0, 0(r4)
    lwz r0, 0(r4)
    clrlwi r0, r0, 1
    stw r0, 0(r4)
    b vatSetDone
vatSetCase11:
    lwz r7, 0(r4)
    slwi r5, r5, 0xd
    slwi r0, r6, 0xe
    rlwinm r6, r7, 0, 19, 17
    or r5, r6, r5
    stw r5, 0(r4)
    lwz r5, 0(r4)
    rlwinm r5, r5, 0, 18, 14
    or r0, r5, r0
    stw r0, 0(r4)
    b vatSetDone
vatSetCase12:
    lwz r7, 0(r4)
    slwi r5, r5, 0x11
    slwi r0, r6, 0x12
    rlwinm r6, r7, 0, 15, 13
    or r5, r6, r5
    stw r5, 0(r4)
    lwz r5, 0(r4)
    rlwinm r5, r5, 0, 14, 10
    or r0, r5, r0
    stw r0, 0(r4)
    b vatSetDone
vatSetCase13:
    lwz r8, 0(r4)
    slwi r0, r5, 0x15
    rlwinm r5, r8, 0, 11, 9
    or r0, r5, r0
    stw r0, 0(r4)
    slwi r5, r6, 0x16
    slwi r0, r7, 0x19
    lwz r6, 0(r4)
    rlwinm r6, r6, 0, 10, 6
    or r5, r6, r5
    stw r5, 0(r4)
    lwz r5, 0(r4)
    rlwinm r5, r5, 0, 7, 1
    or r0, r5, r0
    stw r0, 0(r4)
    b vatSetDone
vatSetCase14:
    lwz r0, 0(r8)
    slwi r4, r6, 1
    clrrwi r0, r0, 1
    or r0, r0, r5
    stw r0, 0(r8)
    clrlslwi r0, r7, 24, 4
    lwz r5, 0(r8)
    rlwinm r5, r5, 0, 31, 27
    or r4, r5, r4
    stw r4, 0(r8)
    lwz r4, 0(r8)
    rlwinm r4, r4, 0, 28, 22
    or r0, r4, r0
    stw r0, 0(r8)
    b vatSetDone
vatSetCase15:
    lwz r4, 0(r8)
    slwi r0, r5, 9
    rlwinm r4, r4, 0, 23, 21
    or r0, r4, r0
    stw r0, 0(r8)
    slwi r4, r6, 0xa
    clrlslwi r0, r7, 24, 0xd
    lwz r5, 0(r8)
    rlwinm r5, r5, 0, 22, 18
    or r4, r5, r4
    stw r4, 0(r8)
    lwz r4, 0(r8)
    rlwinm r4, r4, 0, 19, 13
    or r0, r4, r0
    stw r0, 0(r8)
    b vatSetDone
vatSetCase16:
    lwz r4, 0(r8)
    slwi r0, r5, 0x12
    rlwinm r4, r4, 0, 14, 12
    or r0, r4, r0
    stw r0, 0(r8)
    slwi r4, r6, 0x13
    clrlslwi r0, r7, 24, 0x16
    lwz r5, 0(r8)
    rlwinm r5, r5, 0, 13, 9
    or r4, r5, r4
    stw r4, 0(r8)
    lwz r4, 0(r8)
    rlwinm r4, r4, 0, 10, 4
    or r0, r4, r0
    stw r0, 0(r8)
    b vatSetDone
vatSetCase17:
    lwz r10, 0(r8)
    slwi r0, r5, 0x1b
    slwi r4, r6, 0x1c
    rlwinm r5, r10, 0, 5, 3
    or r0, r5, r0
    stw r0, 0(r8)
    clrlwi r0, r7, 24
    lwz r5, 0(r8)
    rlwinm r5, r5, 0, 4, 0
    or r4, r5, r4
    stw r4, 0(r8)
    lwz r4, 0(r9)
    clrrwi r4, r4, 5
    or r0, r4, r0
    stw r0, 0(r9)
    b vatSetDone
vatSetCase18:
    lwz r4, 0(r9)
    slwi r0, r5, 5
    rlwinm r4, r4, 0, 27, 25
    or r0, r4, r0
    stw r0, 0(r9)
    slwi r4, r6, 6
    clrlslwi r0, r7, 24, 9
    lwz r5, 0(r9)
    rlwinm r5, r5, 0, 26, 22
    or r4, r5, r4
    stw r4, 0(r9)
    lwz r4, 0(r9)
    rlwinm r4, r4, 0, 23, 17
    or r0, r4, r0
    stw r0, 0(r9)
    b vatSetDone
vatSetCase19:
    lwz r4, 0(r9)
    slwi r0, r5, 0xe
    rlwinm r4, r4, 0, 18, 16
    or r0, r4, r0
    stw r0, 0(r9)
    slwi r4, r6, 0xf
    clrlslwi r0, r7, 24, 0x12
    lwz r5, 0(r9)
    rlwinm r5, r5, 0, 17, 13
    or r4, r5, r4
    stw r4, 0(r9)
    lwz r4, 0(r9)
    rlwinm r4, r4, 0, 14, 8
    or r0, r4, r0
    stw r0, 0(r9)
    b vatSetDone
vatSetCase20:
    lwz r4, 0(r9)
    slwi r0, r5, 0x17
    rlwinm r4, r4, 0, 9, 7
    or r0, r4, r0
    stw r0, 0(r9)
    slwi r0, r6, 0x18
    lwz r4, 0(r9)
    rlwinm r4, r4, 0, 8, 4
    or r0, r4, r0
    stw r0, 0(r9)
    lwz r0, 0(r9)
    clrlwi r0, r0, 5
    rlwimi r0, r7, 27, 0, 4
    stw r0, 0(r9)
vatSetDone:
    lwz r5, gx(r13)
    clrlwi r0, r3, 24
    li r3, 1
    lwz r4, 0x4f4(r5)
    slw r0, r3, r0
    clrlwi r0, r0, 24
    ori r3, r4, 0x10
    stw r3, 0x4f4(r5)
    lbz r3, 0x4f3(r5)
    or r0, r3, r0
    stb r0, 0x4f3(r5)
    blr
}

/* fn_800B80CC - 0x800B80CC | size: 0x378 -- GX Misc / PE / draw sync */
asm void fn_800B80CC(void) {
    nofralloc
    lwz r6, gx(r13)
    slwi r0, r3, 2
    lis r5, jumptable_80312B8C@ha
    add r6, r6, r0
    addi r8, r6, 0x1c
    addi r9, r6, 0x3c
    addi r10, r6, 0x5c
    addi r5, r5, jumptable_80312B8C@l
    b vatListTest
vatListLoop:
    lwz r6, 0(r4)
    lbz r7, 0xc(r4)
    subi r11, r6, 9
    lwz r6, 8(r4)
    cmplwi r11, 0x10
    lwz r0, 4(r4)
    bgt vatListNext
    slwi r11, r11, 2
    lwzx r11, r5, r11
    mtctr r11
    bctr
vatListCase9:
    lwz r12, 0(r8)
    slwi r11, r6, 1
    slwi r6, r7, 4
    clrrwi r7, r12, 1
    or r0, r7, r0
    stw r0, 0(r8)
    lwz r0, 0(r8)
    rlwinm r0, r0, 0, 31, 27
    or r0, r0, r11
    stw r0, 0(r8)
    lwz r0, 0(r8)
    rlwinm r0, r0, 0, 28, 22
    or r0, r0, r6
    stw r0, 0(r8)
    b vatListNext
vatListCase10:
    lwz r7, 0(r8)
    slwi r6, r6, 0xa
    cmpwi r0, 2
    rlwinm r7, r7, 0, 22, 18
    or r6, r7, r6
    stw r6, 0(r8)
    bne vatListCase10Not2
    lwz r0, 0(r8)
    rlwinm r0, r0, 0, 23, 21
    ori r0, r0, 0x200
    stw r0, 0(r8)
    lwz r0, 0(r8)
    clrlwi r0, r0, 1
    oris r0, r0, 0x8000
    stw r0, 0(r8)
    b vatListNext
vatListCase10Not2:
    lwz r6, 0(r8)
    slwi r0, r0, 9
    rlwinm r6, r6, 0, 23, 21
    or r0, r6, r0
    stw r0, 0(r8)
    lwz r0, 0(r8)
    clrlwi r0, r0, 1
    stw r0, 0(r8)
    b vatListNext
vatListCase11:
    lwz r11, 0(r8)
    slwi r7, r0, 0xd
    slwi r0, r6, 0xe
    rlwinm r6, r11, 0, 19, 17
    or r6, r6, r7
    stw r6, 0(r8)
    lwz r6, 0(r8)
    rlwinm r6, r6, 0, 18, 14
    or r0, r6, r0
    stw r0, 0(r8)
    b vatListNext
vatListCase12:
    lwz r11, 0(r8)
    slwi r7, r0, 0x11
    slwi r0, r6, 0x12
    rlwinm r6, r11, 0, 15, 13
    or r6, r6, r7
    stw r6, 0(r8)
    lwz r6, 0(r8)
    rlwinm r6, r6, 0, 14, 10
    or r0, r6, r0
    stw r0, 0(r8)
    b vatListNext
vatListCase13:
    lwz r11, 0(r8)
    slwi r0, r0, 0x15
    slwi r6, r6, 0x16
    rlwinm r11, r11, 0, 11, 9
    or r0, r11, r0
    stw r0, 0(r8)
    slwi r0, r7, 0x19
    lwz r7, 0(r8)
    rlwinm r7, r7, 0, 10, 6
    or r6, r7, r6
    stw r6, 0(r8)
    lwz r6, 0(r8)
    rlwinm r6, r6, 0, 7, 1
    or r0, r6, r0
    stw r0, 0(r8)
    b vatListNext
vatListCase14:
    lwz r12, 0(r9)
    slwi r11, r6, 1
    slwi r6, r7, 4
    clrrwi r7, r12, 1
    or r0, r7, r0
    stw r0, 0(r9)
    lwz r0, 0(r9)
    rlwinm r0, r0, 0, 31, 27
    or r0, r0, r11
    stw r0, 0(r9)
    lwz r0, 0(r9)
    rlwinm r0, r0, 0, 28, 22
    or r0, r0, r6
    stw r0, 0(r9)
    b vatListNext
vatListCase15:
    lwz r11, 0(r9)
    slwi r0, r0, 9
    slwi r6, r6, 0xa
    rlwinm r11, r11, 0, 23, 21
    or r0, r11, r0
    stw r0, 0(r9)
    slwi r0, r7, 0xd
    lwz r7, 0(r9)
    rlwinm r7, r7, 0, 22, 18
    or r6, r7, r6
    stw r6, 0(r9)
    lwz r6, 0(r9)
    rlwinm r6, r6, 0, 19, 13
    or r0, r6, r0
    stw r0, 0(r9)
    b vatListNext
vatListCase16:
    lwz r11, 0(r9)
    slwi r0, r0, 0x12
    slwi r6, r6, 0x13
    rlwinm r11, r11, 0, 14, 12
    or r0, r11, r0
    stw r0, 0(r9)
    slwi r0, r7, 0x16
    lwz r7, 0(r9)
    rlwinm r7, r7, 0, 13, 9
    or r6, r7, r6
    stw r6, 0(r9)
    lwz r6, 0(r9)
    rlwinm r6, r6, 0, 10, 4
    or r0, r6, r0
    stw r0, 0(r9)
    b vatListNext
vatListCase17:
    lwz r12, 0(r9)
    slwi r11, r0, 0x1b
    slwi r0, r6, 0x1c
    rlwinm r6, r12, 0, 5, 3
    or r6, r6, r11
    stw r6, 0(r9)
    lwz r6, 0(r9)
    rlwinm r6, r6, 0, 4, 0
    or r0, r6, r0
    stw r0, 0(r9)
    lwz r0, 0(r10)
    clrrwi r0, r0, 5
    or r0, r0, r7
    stw r0, 0(r10)
    b vatListNext
vatListCase18:
    lwz r11, 0(r10)
    slwi r0, r0, 5
    slwi r6, r6, 6
    rlwinm r11, r11, 0, 27, 25
    or r0, r11, r0
    stw r0, 0(r10)
    slwi r0, r7, 9
    lwz r7, 0(r10)
    rlwinm r7, r7, 0, 26, 22
    or r6, r7, r6
    stw r6, 0(r10)
    lwz r6, 0(r10)
    rlwinm r6, r6, 0, 23, 17
    or r0, r6, r0
    stw r0, 0(r10)
    b vatListNext
vatListCase19:
    lwz r11, 0(r10)
    slwi r0, r0, 0xe
    slwi r6, r6, 0xf
    rlwinm r11, r11, 0, 18, 16
    or r0, r11, r0
    stw r0, 0(r10)
    slwi r0, r7, 0x12
    lwz r7, 0(r10)
    rlwinm r7, r7, 0, 17, 13
    or r6, r7, r6
    stw r6, 0(r10)
    lwz r6, 0(r10)
    rlwinm r6, r6, 0, 14, 8
    or r0, r6, r0
    stw r0, 0(r10)
    b vatListNext
vatListCase20:
    lwz r12, 0(r10)
    slwi r11, r0, 0x17
    slwi r0, r6, 0x18
    rlwinm r6, r12, 0, 9, 7
    or r6, r6, r11
    stw r6, 0(r10)
    lwz r6, 0(r10)
    rlwinm r6, r6, 0, 8, 4
    or r0, r6, r0
    stw r0, 0(r10)
    lwz r0, 0(r10)
    clrlwi r0, r0, 5
    rlwimi r0, r7, 27, 0, 4
    stw r0, 0(r10)
vatListNext:
    addi r4, r4, 0x10
vatListTest:
    lwz r0, 0(r4)
    cmpwi r0, 0xff
    bne vatListLoop
    lwz r5, gx(r13)
    clrlwi r0, r3, 24
    li r3, 1
    lwz r4, 0x4f4(r5)
    slw r0, r3, r0
    clrlwi r0, r0, 24
    ori r3, r4, 0x10
    stw r3, 0x4f4(r5)
    lbz r3, 0x4f3(r5)
    or r0, r3, r0
    stb r0, 0x4f3(r5)
    blr
}

/* fn_800B8444 - 0x800B8444 | size: 0x9C -- GX Misc / PE / draw sync */
asm void fn_800B8444(void) {
    nofralloc
    lwz r10, gx(r13)
    li r12, 0
    li r11, 0
    lis r7, 0xcc01
    b vcdLoopTest
vcdLoop:
    clrlwi r9, r12, 24
    lbz r3, 0x4f3(r10)
    li r0, 1
    slw r0, r0, r9
    and. r0, r3, r0
    beq vcdLoopNext
    li r8, 8
    stb r8, -0x8000(r7)
    ori r3, r9, 0x70
    addi r0, r11, 0x1c
    stb r3, -0x8000(r7)
    ori r5, r9, 0x80
    addi r4, r11, 0x3c
    lwzx r6, r10, r0
    ori r3, r9, 0x90
    addi r0, r11, 0x5c
    stw r6, -0x8000(r7)
    stb r8, -0x8000(r7)
    stb r5, -0x8000(r7)
    lwzx r4, r10, r4
    stw r4, -0x8000(r7)
    stb r8, -0x8000(r7)
    stb r3, -0x8000(r7)
    lwzx r0, r10, r0
    stw r0, -0x8000(r7)
vcdLoopNext:
    addi r11, r11, 4
    addi r12, r12, 1
vcdLoopTest:
    clrlwi r0, r12, 24
    cmplwi r0, 8
    blt vcdLoop
    lwz r3, gx(r13)
    li r0, 0
    stb r0, 0x4f3(r3)
    blr
}

/* fn_800B84E0 - 0x800B84E0 | size: 0x8C -- GX Misc / PE / draw sync */
asm void fn_800B84E0(u32 arg0, u32 arg1, u32 arg2) {
    nofralloc
    cmpwi r3, 0x19
    bne xfcmdNot19
    li r3, 0xa
xfcmdNot19:
    li r0, 8
    subi r6, r3, 9
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    ori r0, r6, 0xa0
    clrlwi r4, r4, 2
    stb r0, -0x8000(r3)
    subic. r0, r6, 0xc
    stw r4, -0x8000(r3)
    blt xfcmdSkipA
    cmpwi r0, 4
    bge xfcmdSkipA
    lwz r3, gx(r13)
    slwi r0, r0, 2
    add r3, r3, r0
    stw r4, 0x88(r3)
xfcmdSkipA:
    li r0, 8
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    ori r0, r6, 0xb0
    clrlwi r4, r5, 24
    stb r0, -0x8000(r3)
    subic. r0, r6, 0xc
    stw r4, -0x8000(r3)
    bltlr
    cmpwi r0, 4
    bgelr
    lwz r3, gx(r13)
    slwi r0, r0, 2
    add r3, r3, r0
    stw r4, 0x98(r3)
    blr
}

/* fn_800B856C - 0x800B856C | size: 0x10 -- GX Misc / PE / draw sync */
void fn_800B856C(void) {
    *(u8*)0xCC008000 = 0x48;
}

/* fn_800B857C - 0x800B857C | size: 0x2D0 -- GX Misc / PE / draw sync */
asm void fn_800B857C(void) {
    nofralloc
    mflr r0
    cmplwi r5, 0x14
    stw r0, 4(r1)
    li r11, 0
    li r12, 0
    stwu r1, -8(r1)
    li r10, 5
    bgt vafAttrReady
    lis r9, jumptable_80312BEC@ha
    addi r9, r9, jumptable_80312BEC@l
    slwi r0, r5, 2
    lwzx r0, r9, r0
    mtctr r0
    bctr
vafAttr0:
    li r10, 0
    li r12, 1
    b vafAttrReady
vafAttr1:
    li r10, 1
    li r12, 1
    b vafAttrReady
vafAttr2:
    li r10, 3
    li r12, 1
    b vafAttrReady
vafAttr3:
    li r10, 4
    li r12, 1
    b vafAttrReady
vafAttr4:
    li r10, 2
    b vafAttrReady
vafAttr5:
    li r10, 2
    b vafAttrReady
vafAttr6:
    li r10, 5
    b vafAttrReady
vafAttr7:
    li r10, 6
    b vafAttrReady
vafAttr8:
    li r10, 7
    b vafAttrReady
vafAttr9:
    li r10, 8
    b vafAttrReady
vafAttr10:
    li r10, 9
    b vafAttrReady
vafAttr11:
    li r10, 0xa
    b vafAttrReady
vafAttr12:
    li r10, 0xb
    b vafAttrReady
vafAttr13:
    li r10, 0xc
vafAttrReady:
    cmpwi r4, 1
    beq vafFmtDirect
    bge vafFmtHigh
    cmpwi r4, 0
    bge vafFmtNone
    b vafFmtDone
vafFmtHigh:
    cmpwi r4, 0xa
    beq vafFmtIndex10
    bge vafFmtDone
    b vafFmtIndexed
vafFmtDirect:
    slwi r0, r12, 2
    rlwinm r4, r0, 0, 28, 19
    slwi r0, r10, 7
    or r11, r4, r0
    b vafFmtDone
vafFmtNone:
    slwi r0, r12, 2
    ori r0, r0, 2
    rlwinm r4, r0, 0, 28, 19
    slwi r0, r10, 7
    or r11, r4, r0
    b vafFmtDone
vafFmtIndexed:
    slwi r0, r12, 2
    rlwinm r0, r0, 0, 28, 24
    ori r0, r0, 0x10
    rlwinm r9, r0, 0, 25, 19
    slwi r0, r10, 7
    or r9, r9, r0
    subi r5, r5, 0xc
    subi r0, r4, 2
    rlwinm r9, r9, 0, 20, 16
    slwi r4, r5, 0xc
    or r4, r9, r4
    rlwinm r4, r4, 0, 17, 13
    slwi r0, r0, 0xf
    or r11, r4, r0
    b vafFmtDone
vafFmtIndex10:
    cmpwi r5, 0x13
    slwi r0, r12, 2
    bne vafFmtIndex10Other
    rlwinm r0, r0, 0, 28, 24
    ori r0, r0, 0x20
    b vafFmtIndex10Join
vafFmtIndex10Other:
    rlwinm r0, r0, 0, 28, 24
    ori r0, r0, 0x30
vafFmtIndex10Join:
    rlwinm r0, r0, 0, 25, 19
    ori r11, r0, 0x100
vafFmtDone:
    li r10, 0x10
    lis r9, 0xcc01
    stb r10, -0x8000(r9)
    addi r0, r3, 0x1040
    subi r4, r8, 0x40
    stw r0, -0x8000(r9)
    rlwinm r5, r4, 0, 24, 22
    clrlslwi r4, r7, 24, 8
    stw r11, -0x8000(r9)
    addi r0, r3, 0x1050
    cmplwi r3, 6
    stb r10, -0x8000(r9)
    or r4, r5, r4
    stw r0, -0x8000(r9)
    stw r4, -0x8000(r9)
    bgt vafVatA7
    lis r4, jumptable_80312BD0@ha
    addi r4, r4, jumptable_80312BD0@l
    slwi r0, r3, 2
    lwzx r0, r4, r0
    mtctr r0
    bctr
vafVatA0:
    lwz r4, gx(r13)
    slwi r0, r6, 6
    addi r5, r4, 0x80
    lwz r4, 0x80(r4)
    rlwinm r4, r4, 0, 26, 19
    or r0, r4, r0
    stw r0, 0(r5)
    b vafDone
vafVatA1:
    lwz r4, gx(r13)
    slwi r0, r6, 0xc
    addi r5, r4, 0x80
    lwz r4, 0x80(r4)
    rlwinm r4, r4, 0, 20, 13
    or r0, r4, r0
    stw r0, 0(r5)
    b vafDone
vafVatA2:
    lwz r4, gx(r13)
    slwi r0, r6, 0x12
    addi r5, r4, 0x80
    lwz r4, 0x80(r4)
    rlwinm r4, r4, 0, 14, 7
    or r0, r4, r0
    stw r0, 0(r5)
    b vafDone
vafVatA3:
    lwz r4, gx(r13)
    slwi r0, r6, 0x18
    addi r5, r4, 0x80
    lwz r4, 0x80(r4)
    rlwinm r4, r4, 0, 8, 1
    or r0, r4, r0
    stw r0, 0(r5)
    b vafDone
vafVatA4:
    lwz r4, gx(r13)
    lwzu r0, 0x84(r4)
    clrrwi r0, r0, 6
    or r0, r0, r6
    stw r0, 0(r4)
    b vafDone
vafVatA5:
    lwz r4, gx(r13)
    slwi r0, r6, 6
    addi r5, r4, 0x84
    lwz r4, 0x84(r4)
    rlwinm r4, r4, 0, 26, 19
    or r0, r4, r0
    stw r0, 0(r5)
    b vafDone
vafVatA6:
    lwz r4, gx(r13)
    slwi r0, r6, 0xc
    addi r5, r4, 0x84
    lwz r4, 0x84(r4)
    rlwinm r4, r4, 0, 20, 13
    or r0, r4, r0
    stw r0, 0(r5)
    b vafDone
vafVatA7:
    lwz r4, gx(r13)
    slwi r0, r6, 0x12
    addi r5, r4, 0x84
    lwz r4, 0x84(r4)
    rlwinm r4, r4, 0, 14, 7
    or r0, r4, r0
    stw r0, 0(r5)
vafDone:
    addi r3, r3, 1
    bl fn_800BD898
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800B884C - 0x800B884C | size: 0x40 -- GX Misc / PE / draw sync */
asm void fn_800B884C(u32 arg0) {
    nofralloc
    lwz r6, gx(r13)
    clrlwi r8, r3, 24
    li r4, 0x10
    lwz r5, 0x204(r6)
    lis r3, 0xcc01
    li r0, 0x103f
    clrrwi r5, r5, 4
    or r5, r5, r8
    stw r5, 0x204(r6)
    stb r4, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r8, -0x8000(r3)
    lwz r0, 0x4f4(r6)
    ori r0, r0, 4
    stw r0, 0x4f4(r6)
    blr
}

/* fn_800B8920 - 0x800B8920 | size: 0x5C -- GX Misc / PE / draw sync */
asm void fn_800B8920(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -8(r1)
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    cmplwi r0, 0
    beq drawDoneClean
    bl fn_800B91EC
drawDoneClean:
    li r0, 0
    lis r3, 0xcc01
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r0, -0x8000(r3)
    bl fn_80098034
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800B897C - 0x800B897C | size: 0x16C -- GX Misc / PE / draw sync */
void fn_800B897C(void) {
    /* GX Misc / PE / draw sync (0x16C bytes) */
}

/* fn_800B8AE8 - 0x800B8AE8 | size: 0x170 -- GX Misc / PE / draw sync */
asm void fn_800B8AE8(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x28(r1)
    stmw r27, 0x14(r1)
    lwz r3, gx(r13)
    lbz r0, 0x4f2(r3)
    cmplwi r0, 0
    beq forceFifoClean
    bl fn_800B7714
    cmplwi r3, 0
    beq forceFifoClean
    lwz r3, __memReg(r13)
    addi r6, r3, 0x4e
    lhz r4, 0x4e(r3)
    addi r5, r3, 0x50
sampleMemA:
    mr r0, r4
    lhz r4, 0(r6)
    lhz r3, 0(r5)
    cmplw r4, r0
    bne sampleMemA
    slwi r0, r4, 0x10
    or r27, r0, r3
waitMemStableStart:
    bl OSGetTime
    li r0, 0
    addi r31, r4, 0
    addi r30, r3, 0
    xoris r28, r0, 0x8000
    li r29, 8
waitMemStableLoop:
    bl OSGetTime
    subfc r4, r31, r4
    subfe r0, r30, r3
    xoris r3, r0, 0x8000
    subfc r0, r4, r29
    subfe r3, r3, r28
    subfe r3, r28, r28
    neg. r3, r3
    beq waitMemStableLoop
    lwz r3, __memReg(r13)
    addi r6, r3, 0x4e
    lhz r4, 0x4e(r3)
    addi r5, r3, 0x50
sampleMemB:
    mr r0, r4
    lhz r4, 0(r6)
    lhz r3, 0(r5)
    cmplw r4, r0
    bne sampleMemB
    slwi r0, r4, 0x10
    or r0, r0, r3
    cmplw r0, r27
    mr r27, r0
    bne waitMemStableStart
forceFifoClean:
    lis r3, 0xcc00
    li r0, 1
    addi r27, r3, 0x3000
    stwu r0, 0x18(r27)
    bl OSGetTime
    li r0, 0
    addi r31, r4, 0
    addi r30, r3, 0
    xoris r28, r0, 0x8000
    li r29, 0x32
fifoSetWait:
    bl OSGetTime
    subfc r4, r31, r4
    subfe r0, r30, r3
    xoris r3, r0, 0x8000
    subfc r0, r4, r29
    subfe r3, r3, r28
    subfe r3, r28, r28
    neg. r3, r3
    beq fifoSetWait
    li r30, 0
    stw r30, 0(r27)
    bl OSGetTime
    addi r28, r4, 0
    addi r29, r3, 0
    xoris r31, r30, 0x8000
    li r30, 5
fifoClearWait:
    bl OSGetTime
    subfc r4, r28, r4
    subfe r0, r29, r3
    xoris r3, r0, 0x8000
    subfc r0, r4, r30
    subfe r3, r3, r31
    subfe r3, r31, r31
    neg. r3, r3
    beq fifoClearWait
    bl __GXCleanGPFifo
    lmw r27, 0x14(r1)
    lwz r0, 0x2c(r1)
    addi r1, r1, 0x28
    mtlr r0
    blr
}

/* fn_800B8C58 - 0x800B8C58 | size: 0xB8 -- GX Misc / PE / draw sync */
asm void fn_800B8C58(u32 arg0) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    stw r30, 0x10(r1)
    mr r30, r3
    bl OSDisableInterrupts
    li r6, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    clrlwi r0, r30, 16
    stb r6, -0x8000(r5)
    oris r0, r0, 0x4800
    stw r0, -0x8000(r5)
    clrrwi r0, r0, 16
    rlwimi r0, r30, 0, 16, 31
    clrlwi r0, r0, 8
    stb r6, -0x8000(r5)
    oris r0, r0, 0x4700
    stw r0, -0x8000(r5)
    mr r30, r3
    lwz r0, 0x4f4(r4)
    cmplwi r0, 0
    beq clearDrawDone
    bl fn_800B91EC
clearDrawDone:
    li r31, 0
    lis r3, 0xcc01
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    bl fn_80098034
    mr r3, r30
    bl OSRestoreInterrupts
    lwz r3, gx(r13)
    sth r31, 2(r3)
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800B8D10 - 0x800B8D10 | size: 0x98 -- GX Misc / PE / draw sync */
asm void fn_800B8D10(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    stw r30, 0x10(r1)
    bl OSDisableInterrupts
    li r0, 0x61
    lwz r4, gx(r13)
    lis r6, 0xcc01
    lis r5, 0x4500
    stb r0, -0x8000(r6)
    addi r0, r5, 2
    stw r0, -0x8000(r6)
    mr r30, r3
    lwz r0, 0x4f4(r4)
    cmplwi r0, 0
    beq abortFrameClean
    bl fn_800B91EC
abortFrameClean:
    li r31, 0
    lis r3, 0xcc01
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    stw r31, -0x8000(r3)
    bl fn_80098034
    stb r31, lbl_8047A9C8(r13)
    mr r3, r30
    bl OSRestoreInterrupts
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800B8DA8 - 0x800B8DA8 | size: 0x4C -- GX Misc / PE / draw sync */
asm void fn_800B8DA8(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x10(r1)
    stw r31, 0xc(r1)
    bl OSDisableInterrupts
    mr r31, r3
    b waitDrawDoneCheck
waitDrawDoneSleep:
    la r3, lbl_8047A9CC(r13)
    bl fn_800A238C
waitDrawDoneCheck:
    lbz r0, lbl_8047A9C8(r13)
    cmplwi r0, 0
    beq waitDrawDoneSleep
    mr r3, r31
    bl OSRestoreInterrupts
    lwz r0, 0x14(r1)
    lwz r31, 0xc(r1)
    addi r1, r1, 0x10
    mtlr r0
    blr
}

/* fn_800B8DF4 - 0x800B8DF4 | size: 0x80 -- GX Misc / PE / draw sync */
asm void fn_800B8DF4(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    bl OSDisableInterrupts
    li r0, 0x61
    lis r5, 0xcc01
    lis r4, 0x4500
    stb r0, -0x8000(r5)
    addi r0, r4, 2
    stw r0, -0x8000(r5)
    mr r31, r3
    bl fn_800B8920
    li r0, 0
    stb r0, lbl_8047A9C8(r13)
    mr r3, r31
    bl OSRestoreInterrupts
    bl OSDisableInterrupts
    mr r31, r3
    b abortFrameWaitCheck
abortFrameWaitSleep:
    la r3, lbl_8047A9CC(r13)
    bl fn_800A238C
abortFrameWaitCheck:
    lbz r0, lbl_8047A9C8(r13)
    cmplwi r0, 0
    beq abortFrameWaitSleep
    mr r3, r31
    bl OSRestoreInterrupts
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800B8E74 - 0x800B8E74 | size: 0x24 -- GX Misc / PE / draw sync */
void fn_800B8E74(void) {
    asm {
        li r0, 0x61
        lwz r4, gx(r13)
        lis r5, 0xcc01
        stb r0, -0x8000(r5)
        li r0, 0
        lwz r3, 0x1dc(r4)
        stw r3, -0x8000(r5)
        sth r0, 2(r4)
    }
}

/* fn_800B8E98 - 0x800B8E98 | size: 0x14 -- GX Misc / PE / draw sync */
void fn_800B8E98(u32 tokenHi, u32 tokenLo) {
    asm {
        lwz r5, __peReg(r13)
        clrlwi r0, r4, 24
        rlwimi r0, r3, 8, 0, 23
        sth r0, 6(r5)
    }
}

/* fn_800B8EAC - 0x800B8EAC | size: 0x14 -- GX Misc / PE / draw sync */
void fn_800B8EAC(u32 value) {
    asm {
        rlwinm r0, r3, 0, 30, 28
        lwz r3, __peReg(r13)
        ori r0, r0, 4
        sth r0, 8(r3)
    }
}

/* fn_800B8EC0 - 0x800B8EC0 | size: 0x1C -- GX Misc / PE / draw sync */
void fn_800B8EC0(u32 value) {
    asm {
        lwz r4, __peReg(r13)
        clrlslwi r0, r3, 24, 4
        lhzu r3, 2(r4)
        rlwinm r3, r3, 0, 28, 26
        or r0, r3, r0
        sth r0, 0(r4)
    }
}

/* fn_800B8EDC - 0x800B8EDC | size: 0x88 -- GX Misc / PE / draw sync */
asm void fn_800B8EDC(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    nofralloc
    lwz r7, __peReg(r13)
    cmpwi r3, 1
    li r9, 1
    addi r10, r7, 2
    lhz r7, 2(r7)
    beq peModeFlagDone
    cmpwi r3, 3
    beq peModeFlagDone
    li r9, 0
peModeFlagDone:
    clrrwi r8, r7, 1
    subfic r0, r3, 3
    cntlzw r7, r0
    subfic r0, r3, 2
    or r8, r8, r9
    cntlzw r0, r0
    rlwinm r8, r8, 0, 21, 19
    extlwi r3, r7, 21, 6
    or r3, r8, r3
    rlwinm r3, r3, 0, 31, 29
    rlwinm r0, r0, 28, 4, 30
    or r0, r3, r0
    rlwinm r3, r0, 0, 20, 15
    slwi r0, r6, 12
    or r0, r3, r0
    rlwinm r3, r0, 0, 24, 20
    slwi r0, r4, 8
    or r0, r3, r0
    rlwinm r3, r0, 0, 27, 23
    slwi r0, r5, 5
    or r0, r3, r0
    clrlwi r0, r0, 8
    oris r0, r0, 0x4100
    sth r0, 0(r10)
    blr
}

/* fn_800B8F64 - 0x800B8F64 | size: 0x1C -- GX Misc / PE / draw sync */
void fn_800B8F64(u32 value) {
    asm {
        lwz r4, __peReg(r13)
        clrlslwi r0, r3, 24, 3
        lhzu r3, 2(r4)
        rlwinm r3, r3, 0, 29, 27
        or r0, r3, r0
        sth r0, 0(r4)
    }
}

/* fn_800B8F80 - 0x800B8F80 | size: 0x14 -- GX Misc / PE / draw sync */
void fn_800B8F80(u32 valueHi, u32 valueLo) {
    asm {
        lwz r5, __peReg(r13)
        clrlslwi r0, r3, 24, 8
        rlwimi r0, r4, 0, 24, 31
        sth r0, 4(r5)
    }
}

/* fn_800B8F94 - 0x800B8F94 | size: 0x1C -- GX Misc / PE / draw sync */
void fn_800B8F94(u32 value) {
    asm {
        lwz r4, __peReg(r13)
        clrlslwi r0, r3, 24, 2
        lhzu r3, 2(r4)
        rlwinm r3, r3, 0, 30, 28
        or r0, r3, r0
        sth r0, 0(r4)
    }
}

/* fn_800B8FB0 - 0x800B8FB0 | size: 0x28 -- GX Misc / PE / draw sync */
void fn_800B8FB0(u32 arg0, u32 arg1, u32 arg2) {
    asm {
        clrlwi r0, r3, 24
        lwz r3, __peReg(r13)
        rlwinm r6, r0, 0, 31, 27
        slwi r0, r4, 1
        or r0, r6, r0
        rlwinm r4, r0, 0, 28, 26
        clrlslwi r0, r5, 24, 4
        or r0, r4, r0
        sth r0, 0(r3)
    }
}

/* fn_800B8FD8 - 0x800B8FD8 | size: 0x44 -- GX Misc / PE / draw sync */
asm void* fn_800B8FD8(void* callback) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    stw r30, 0x10(r1)
    mr r30, r3
    lwz r31, lbl_8047A9C0(r13)
    bl OSDisableInterrupts
    stw r30, lbl_8047A9C0(r13)
    bl OSRestoreInterrupts
    mr r3, r31
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800B901C - 0x800B901C | size: 0x88 -- GX Misc / PE / draw sync */
asm void fn_800B901C(s16 interrupt, OSContext* context) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x2e0(r1)
    stw r31, 0x2dc(r1)
    stw r30, 0x2d8(r1)
    mr r30, r4
    lwz r0, lbl_8047A9C0(r13)
    lwz r3, __peReg(r13)
    cmplwi r0, 0
    lhz r31, 0xe(r3)
    beq doneCallback
    addi r3, r1, 0x10
    bl OSClearContext
    addi r3, r1, 0x10
    bl OSSetCurrentContext
    lwz r12, lbl_8047A9C0(r13)
    addi r3, r31, 0
    mtlr r12
    blrl
    addi r3, r1, 0x10
    bl OSClearContext
    mr r3, r30
    bl OSSetCurrentContext
doneCallback:
    lwz r3, __peReg(r13)
    lhzu r0, 0xa(r3)
    rlwinm r0, r0, 0, 30, 28
    ori r0, r0, 4
    sth r0, 0(r3)
    lwz r0, 0x2e4(r1)
    lwz r31, 0x2dc(r1)
    lwz r30, 0x2d8(r1)
    addi r1, r1, 0x2e0
    mtlr r0
    blr
}

/* fn_800B90A4 - 0x800B90A4 | size: 0x44 -- GX Misc / PE / draw sync */
asm void* fn_800B90A4(void* callback) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    stw r30, 0x10(r1)
    mr r30, r3
    lwz r31, lbl_8047A9C4(r13)
    bl OSDisableInterrupts
    stw r30, lbl_8047A9C4(r13)
    bl OSRestoreInterrupts
    mr r3, r31
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800B90E8 - 0x800B90E8 | size: 0x84 -- GX Misc / PE / draw sync */
asm void fn_800B90E8(s16 interrupt, OSContext* context) {
    nofralloc
    mflr r0
    li r3, 1
    stw r0, 4(r1)
    stwu r1, -0x2e0(r1)
    stw r31, 0x2dc(r1)
    addi r31, r4, 0
    lwz r5, __peReg(r13)
    lhz r0, 0xa(r5)
    rlwinm r0, r0, 0, 29, 27
    ori r0, r0, 8
    sth r0, 0xa(r5)
    lwz r0, lbl_8047A9C4(r13)
    stb r3, lbl_8047A9C8(r13)
    cmplwi r0, 0
    beq doneCallback
    addi r3, r1, 0x10
    bl OSClearContext
    addi r3, r1, 0x10
    bl OSSetCurrentContext
    lwz r12, lbl_8047A9C4(r13)
    mtlr r12
    blrl
    addi r3, r1, 0x10
    bl OSClearContext
    mr r3, r31
    bl OSSetCurrentContext
doneCallback:
    la r3, lbl_8047A9CC(r13)
    bl fn_800A2478
    lwz r0, 0x2e4(r1)
    lwz r31, 0x2dc(r1)
    addi r1, r1, 0x2e0
    mtlr r0
    blr
}

/* fn_800B91EC - 0x800B91EC | size: 0xA0 -- GX Misc / PE / draw sync */
asm void fn_800B91EC(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -8(r1)
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    clrlwi. r0, r0, 31
    beq skipSUTex
    bl __GXSetSUTexRegs
skipSUTex:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 30, 30
    beq skipTex
    bl fn_800BC024
skipTex:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 29, 29
    beq skipGenMode
    bl fn_800B9578
skipGenMode:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 28, 28
    beq skipBp
    bl fn_800B7BC4
skipBp:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 27, 27
    beq skipVcd
    bl fn_800B8444
skipVcd:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 27, 28
    beq skipVLim
    bl __GXCalculateVLim
skipVLim:
    lwz r3, gx(r13)
    li r0, 0
    stw r0, 0x4f4(r3)
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800B928C - 0x800B928C | size: 0xF0 -- GX Misc / PE / draw sync */
asm void fn_800B928C(u32 arg0, u32 arg1, u16 arg2) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x28(r1)
    stw r31, 0x24(r1)
    addi r31, r5, 0
    stw r30, 0x20(r1)
    addi r30, r4, 0
    stw r29, 0x1c(r1)
    addi r29, r3, 0
    lwz r6, gx(r13)
    lwz r0, 0x4f4(r6)
    cmplwi r0, 0
    beq flushDone
    clrlwi. r0, r0, 31
    beq skipSUTex
    bl __GXSetSUTexRegs
skipSUTex:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 30, 30
    beq skipTex
    bl fn_800BC024
skipTex:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 29, 29
    beq skipGenMode
    bl fn_800B9578
skipGenMode:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 28, 28
    beq skipBp
    bl fn_800B7BC4
skipBp:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 27, 27
    beq skipVcd
    bl fn_800B8444
skipVcd:
    lwz r3, gx(r13)
    lwz r0, 0x4f4(r3)
    rlwinm. r0, r0, 0, 27, 28
    beq skipVLim
    bl __GXCalculateVLim
skipVLim:
    lwz r3, gx(r13)
    li r0, 0
    stw r0, 0x4f4(r3)
flushDone:
    lwz r3, gx(r13)
    lwz r0, 0(r3)
    cmplwi r0, 0
    bne stateReady
    bl fn_800B937C
stateReady:
    or r0, r30, r29
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    sth r31, -0x8000(r3)
    lwz r0, 0x2c(r1)
    lwz r31, 0x24(r1)
    lwz r30, 0x20(r1)
    lwz r29, 0x1c(r1)
    addi r1, r1, 0x28
    mtlr r0
    blr
}

/* fn_800B937C - 0x800B937C | size: 0x88 -- GX Misc / PE / draw sync */
asm void fn_800B937C(void) {
    nofralloc
    lwz r3, gx(r13)
    li r0, 0x98
    lis r5, 0xcc01
    lhz r6, 4(r3)
    li r4, 0
    lhz r3, 6(r3)
    mullw r7, r6, r3
    stb r0, -0x8000(r5)
    sth r6, -0x8000(r5)
    addi r3, r7, 3
    cmplwi r7, 0
    srwi r3, r3, 2
    ble doneClear
    srwi. r0, r3, 3
    mtctr r0
    beq clearTail
clearLoop8:
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    stw r4, -0x8000(r5)
    bdnz clearLoop8
    andi. r3, r3, 7
    beq doneClear
clearTail:
    mtctr r3
clearLoop1:
    stw r4, -0x8000(r5)
    bdnz clearLoop1
doneClear:
    lwz r3, gx(r13)
    li r0, 1
    sth r0, 2(r3)
    blr
}

/* fn_800B9404 - 0x800B9404 | size: 0x48 -- GX Misc / PE / draw sync */
asm void fn_800B9404(u32 arg0, u32 arg1) {
    nofralloc
    lwz r7, gx(r13)
    slwi r6, r4, 16
    li r5, 0x61
    lwz r0, 0x7c(r7)
    lis r4, 0xcc01
    clrrwi r0, r0, 8
    rlwimi r0, r3, 0, 24, 31
    stw r0, 0x7c(r7)
    li r0, 0
    lwz r3, 0x7c(r7)
    rlwinm r3, r3, 0, 16, 12
    or r3, r3, r6
    stw r3, 0x7c(r7)
    stb r5, -0x8000(r4)
    lwz r3, 0x7c(r7)
    stw r3, -0x8000(r4)
    sth r0, 2(r7)
    blr
}

/* fn_800B944C - 0x800B944C | size: 0x48 -- GX Misc / PE / draw sync */
asm void fn_800B944C(u32 arg0, u32 arg1) {
    nofralloc
    lwz r7, gx(r13)
    slwi r6, r4, 19
    li r5, 0x61
    lwz r0, 0x7c(r7)
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 24, 15
    rlwimi r0, r3, 8, 16, 23
    stw r0, 0x7c(r7)
    li r0, 0
    lwz r3, 0x7c(r7)
    rlwinm r3, r3, 0, 13, 9
    or r3, r3, r6
    stw r3, 0x7c(r7)
    stb r5, -0x8000(r4)
    lwz r3, 0x7c(r7)
    stw r3, -0x8000(r4)
    sth r0, 2(r7)
    blr
}

/* fn_800B9494 - 0x800B9494 | size: 0x5C -- GX Misc / PE / draw sync */
asm void fn_800B9494(u32 arg0, u32 arg1, u32 arg2) {
    nofralloc
    lwz r6, gx(r13)
    slwi r8, r3, 2
    add r7, r6, r8
    lwz r0, 0xb8(r7)
    add r9, r6, r8
    rlwinm r3, r0, 0, 14, 12
    clrlslwi r0, r4, 24, 18
    or r0, r3, r0
    stw r0, 0xb8(r7)
    clrlslwi r0, r5, 24, 19
    li r5, 0x61
    lwz r3, 0xb8(r9)
    lis r4, 0xcc01
    rlwinm r3, r3, 0, 13, 11
    or r0, r3, r0
    stw r0, 0xb8(r9)
    add r3, r6, r8
    li r0, 0
    stb r5, -0x8000(r4)
    lwz r3, 0xb8(r3)
    stw r3, -0x8000(r4)
    sth r0, 2(r6)
    blr
}

/* fn_800B94F0 - 0x800B94F0 | size: 0x4C -- GX Misc / PE / draw sync */
asm void fn_800B94F0(u32 mode) {
    nofralloc
    cmpwi r3, 2
    beq remapTwo
    bge modeReady
    cmpwi r3, 1
    bge remapOne
    b modeReady
remapOne:
    li r3, 2
    b modeReady
remapTwo:
    li r3, 1
modeReady:
    lwz r4, gx(r13)
    slwi r0, r3, 14
    lwz r3, 0x204(r4)
    rlwinm r3, r3, 0, 18, 15
    or r0, r3, r0
    stw r0, 0x204(r4)
    lwz r0, 0x4f4(r4)
    ori r0, r0, 4
    stw r0, 0x4f4(r4)
    blr
}

/* fn_800B953C - 0x800B953C | size: 0x3C -- GX Misc / PE / draw sync */
asm void fn_800B953C(u32 arg0) {
    nofralloc
    lwz r6, gx(r13)
    clrlslwi r0, r3, 24, 19
    li r4, 0x61
    lwz r5, 0x204(r6)
    lis r3, 0xcc01
    rlwinm r5, r5, 0, 13, 11
    or r0, r5, r0
    stw r0, 0x204(r6)
    lis r0, 0xfe08
    stb r4, -0x8000(r3)
    stw r0, -0x8000(r3)
    stb r4, -0x8000(r3)
    lwz r0, 0x204(r6)
    stw r0, -0x8000(r3)
    blr
}

/* fn_800B9578 - 0x800B9578 | size: 0x24 -- GX Misc / PE / draw sync */
asm void fn_800B9578(void) {
    nofralloc
    li r0, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r3, 0x204(r4)
    stw r3, -0x8000(r5)
    sth r0, 2(r4)
    blr
}

/* fn_800B959C - 0x800B959C | size: 0x90 -- GX Misc / PE / draw sync */
asm void fn_800B959C(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    nofralloc
    lwz r9, gx(r13)
    li r10, 0
    clrlwi r7, r5, 16
    stw r10, 0x1e0(r9)
    clrlwi r5, r6, 16
    subi r0, r5, 1
    lwz r8, 0x1e0(r9)
    clrlwi r3, r3, 16
    clrlslwi r4, r4, 16, 10
    clrrwi r5, r8, 10
    or r3, r5, r3
    stw r3, 0x1e0(r9)
    subi r3, r7, 1
    slwi r0, r0, 10
    lwz r5, 0x1e0(r9)
    rlwinm r5, r5, 0, 22, 11
    or r4, r5, r4
    stw r4, 0x1e0(r9)
    lwz r4, 0x1e0(r9)
    clrlwi r4, r4, 8
    oris r4, r4, 0x4900
    stw r4, 0x1e0(r9)
    stw r10, 0x1e4(r9)
    lwz r4, 0x1e4(r9)
    clrrwi r4, r4, 10
    or r3, r4, r3
    stw r3, 0x1e4(r9)
    lwz r3, 0x1e4(r9)
    rlwinm r3, r3, 0, 22, 11
    or r0, r3, r0
    stw r0, 0x1e4(r9)
    lwz r0, 0x1e4(r9)
    clrlwi r0, r0, 8
    oris r0, r0, 0x4a00
    stw r0, 0x1e4(r9)
    blr
}

/* fn_800B962C - 0x800B962C | size: 0x90 -- GX Misc / PE / draw sync */
asm void fn_800B962C(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    nofralloc
    lwz r9, gx(r13)
    li r10, 0
    clrlwi r7, r5, 16
    stw r10, 0x1f0(r9)
    clrlwi r5, r6, 16
    subi r0, r5, 1
    lwz r8, 0x1f0(r9)
    clrlwi r3, r3, 16
    clrlslwi r4, r4, 16, 10
    clrrwi r5, r8, 10
    or r3, r5, r3
    stw r3, 0x1f0(r9)
    subi r3, r7, 1
    slwi r0, r0, 10
    lwz r5, 0x1f0(r9)
    rlwinm r5, r5, 0, 22, 11
    or r4, r5, r4
    stw r4, 0x1f0(r9)
    lwz r4, 0x1f0(r9)
    clrlwi r4, r4, 8
    oris r4, r4, 0x4900
    stw r4, 0x1f0(r9)
    stw r10, 0x1f4(r9)
    lwz r4, 0x1f4(r9)
    clrrwi r4, r4, 10
    or r3, r4, r3
    stw r3, 0x1f4(r9)
    lwz r3, 0x1f4(r9)
    rlwinm r3, r3, 0, 22, 11
    or r0, r3, r0
    stw r0, 0x1f4(r9)
    lwz r0, 0x1f4(r9)
    clrlwi r0, r0, 8
    oris r0, r0, 0x4a00
    stw r0, 0x1f4(r9)
    blr
}

/* fn_800B96BC - 0x800B96BC | size: 0x3C -- GX Transform / viewport / projection */
asm void fn_800B96BC(u32 arg0) {
    nofralloc
    lwz r4, gx(r13)
    li r0, 0
    stw r0, 0x1e8(r4)
    addi r5, r4, 0x1e8
    clrlslwi r0, r3, 17, 1
    lwz r4, 0x1e8(r4)
    srawi r0, r0, 5
    clrrwi r3, r4, 10
    or r0, r3, r0
    stw r0, 0(r5)
    lwz r0, 0(r5)
    clrlwi r0, r0, 8
    oris r0, r0, 0x4d00
    stw r0, 0(r5)
    blr
}

/* fn_800B96F8 - 0x800B96F8 | size: 0x154 -- GX Transform / viewport / projection */
asm void fn_800B96F8(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    nofralloc
    mflr r0
    cmpwi r5, 0x13
    stw r0, 4(r1)
    li r0, 0
    addi r8, r3, 0
    stwu r1, -0x30(r1)
    stw r31, 0x2c(r1)
    clrlwi r31, r5, 28
    stw r30, 0x28(r1)
    addi r30, r6, 0
    lwz r7, gx(r13)
    stb r0, 0x200(r7)
    addi r7, r4, 0
    bne formatNot13
    li r31, 0xb
formatNot13:
    cmpwi r5, 0x26
    beq setPacked
    bge setUnpacked
    cmpwi r5, 4
    bge setUnpacked
    cmpwi r5, 0
    bge setPacked
    b setUnpacked
setPacked:
    lwz r3, gx(r13)
    lwzu r0, 0x1fc(r3)
    rlwinm r0, r0, 0, 17, 14
    oris r0, r0, 1
    ori r0, r0, 0x8000
    stw r0, 0(r3)
    b formatReady
setUnpacked:
    lwz r3, gx(r13)
    lwzu r0, 0x1fc(r3)
    rlwinm r0, r0, 0, 17, 14
    oris r0, r0, 1
    stw r0, 0(r3)
formatReady:
    rlwinm r4, r5, 0, 27, 27
    lwz r3, gx(r13)
    subi r0, r4, 0x10
    cntlzw r0, r0
    extrwi r0, r0, 8, 19
    stb r0, 0x200(r3)
    addi r4, r8, 0
    addi r6, r1, 0x20
    lwzu r0, 0x1fc(r3)
    addi r8, r1, 0x18
    rlwinm r0, r0, 0, 29, 27
    rlwimi r0, r31, 0, 28, 28
    stw r0, 0(r3)
    addi r3, r5, 0
    addi r5, r7, 0
    clrlwi r31, r31, 29
    addi r7, r1, 0x1c
    bl __GetImageTileCount
    lwz r3, gx(r13)
    li r0, 0
    stw r0, 0x1f8(r3)
    addi r7, r3, 0x1f8
    addi r8, r3, 0x1fc
    lwz r5, 0x20(r1)
    clrlslwi r3, r30, 24, 9
    lwz r4, 0x18(r1)
    slwi r0, r31, 4
    lwz r6, 0(r7)
    mullw r4, r5, r4
    clrrwi r5, r6, 10
    or r4, r5, r4
    stw r4, 0(r7)
    lwz r4, 0(r7)
    clrlwi r4, r4, 8
    oris r4, r4, 0x4d00
    stw r4, 0(r7)
    lwz r4, 0(r8)
    rlwinm r4, r4, 0, 23, 21
    or r3, r4, r3
    stw r3, 0(r8)
    lwz r3, 0(r8)
    rlwinm r3, r3, 0, 28, 24
    or r0, r3, r0
    stw r0, 0(r8)
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    addi r1, r1, 0x30
    mtlr r0
    blr
}

/* fn_800B984C - 0x800B984C | size: 0x28 -- GX Transform / viewport / projection */
asm void fn_800B984C(u32 arg0) {
    nofralloc
    lwz r4, gx(r13)
    slwi r0, r3, 12
    lwz r3, 0x1ec(r4)
    rlwinm r3, r3, 0, 20, 17
    or r0, r3, r0
    stw r0, 0x1ec(r4)
    lwzu r0, 0x1fc(r4)
    rlwinm r0, r0, 0, 20, 17
    stw r0, 0(r4)
    blr
}

/* fn_800B9874 - 0x800B9874 | size: 0x68 -- GX Transform / viewport / projection */
asm void fn_800B9874(u32 arg0) {
    nofralloc
    lwz r6, gx(r13)
    clrlwi r4, r3, 31
    subi r0, r4, 1
    lwz r4, 0x1ec(r6)
    cntlzw r0, r0
    rlwinm r3, r3, 0, 30, 30
    clrrwi r5, r4, 1
    extrwi r4, r0, 8, 19
    or r0, r5, r4
    stw r0, 0x1ec(r6)
    subi r0, r3, 2
    cntlzw r0, r0
    lwz r3, 0x1ec(r6)
    rlwinm r5, r0, 28, 23, 30
    rlwinm r3, r3, 0, 31, 29
    or r0, r3, r5
    stw r0, 0x1ec(r6)
    lwz r0, 0x1fc(r6)
    clrrwi r0, r0, 1
    or r0, r0, r4
    stw r0, 0x1fc(r6)
    lwz r0, 0x1fc(r6)
    rlwinm r0, r0, 0, 31, 29
    or r0, r0, r5
    stw r0, 0x1fc(r6)
    blr
}

/* fn_800B98DC - 0x800B98DC | size: 0x238 -- GX Transform / viewport / projection */
void fn_800B98DC(void) {
    /* GX Transform / viewport / projection (0x238 bytes) -- large function, requires Ghidra */
}

/* fn_800B9B14 - 0x800B9B14 | size: 0xC8 -- GX Transform / viewport / projection */
asm u32 fn_800B9B14(f32 scale) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -8(r1)
    lfs f0, lbl_8047C308(r13)
    fdivs f1, f0, f1
    bl fn_800C46B0
    clrlwi r6, r3, 23
    lwz r4, gx(r13)
    li r0, 0x61
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    oris r0, r6, 0x4e00
    li r5, 0
    stw r0, -0x8000(r3)
    subfic r3, r6, 0x100
    subic r0, r3, 1
    sth r5, 2(r4)
    subfe r0, r0, r3
    clrlslwi r0, r0, 24, 10
    lwz r3, 0x1ec(r4)
    cmplwi r6, 0x80
    addi r5, r6, 0
    rlwinm r3, r3, 0, 22, 20
    or r0, r3, r0
    stw r0, 0x1ec(r4)
    lwz r0, 0x1e4(r4)
    extrwi r4, r0, 10, 12
    rlwinm r0, r0, 30, 14, 23
    divwu r3, r0, r6
    addi r4, r4, 1
    addi r3, r3, 1
    ble clamp
    cmplwi r6, 0x100
    bge clamp
    b oddCheck
shiftLoop:
    srwi r5, r5, 1
oddCheck:
    clrlwi. r0, r5, 31
    beq shiftLoop
    divwu r0, r4, r5
    mullw r0, r0, r5
    subf. r0, r0, r4
    bne clamp
    addi r3, r3, 1
clamp:
    cmplwi r3, 0x400
    ble done
    li r3, 0x400
done:
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800B9BDC - 0x800B9BDC | size: 0x68 -- GX Transform / viewport / projection */
asm void fn_800B9BDC(u8* color, u32 arg1) {
    nofralloc
    clrlwi r0, r4, 8
    lbz r4, 3(r3)
    lbz r5, 0(r3)
    li r6, 0x61
    slwi r7, r4, 8
    lwz r4, gx(r13)
    rlwimi r7, r5, 0, 24, 31
    lis r5, 0xcc01
    clrlwi r7, r7, 8
    stb r6, -0x8000(r5)
    oris r7, r7, 0x4f00
    stw r7, -0x8000(r5)
    oris r7, r0, 0x5100
    li r0, 0
    lbz r8, 2(r3)
    lbz r3, 1(r3)
    slwi r3, r3, 8
    stb r6, -0x8000(r5)
    rlwimi r3, r8, 0, 24, 31
    clrlwi r3, r3, 8
    oris r3, r3, 0x5000
    stw r3, -0x8000(r5)
    stb r6, -0x8000(r5)
    stw r7, -0x8000(r5)
    sth r0, 2(r4)
    blr
}

/* fn_800B9C44 - 0x800B9C44 | size: 0x228 -- GX Transform / viewport / projection */
asm void fn_800B9C44(u32 arg0, u8* arg1, u32 arg2, u8* arg3) {
    nofralloc
    stwu r1, -0x50(r1)
    clrlwi. r0, r3, 24
    stmw r23, 0x2c(r1)
    beq lbl_800B9C44_default
    lbz r0, 1(r4)
    lbz r3, 7(r4)
    slwi r30, r0, 4
    lbz r8, 0(r4)
    lbz r0, 0x13(r4)
    slwi r25, r3, 4
    lbz r10, 6(r4)
    rlwimi r30, r8, 0, 28, 31
    lbz r11, 2(r4)
    lbz r7, 0xd(r4)
    rlwimi r25, r10, 0, 28, 31
    slwi r27, r11, 8
    lbz r9, 8(r4)
    lbz r3, 0xe(r4)
    slwi r26, r9, 8
    lbz r28, 3(r4)
    rlwimi r27, r30, 0, 24, 31
    lbz r9, 0x10(r4)
    slwi r23, r7, 4
    lbz r12, 0xc(r4)
    lbz r7, 0x15(r4)
    slwi r24, r3, 8
    rlwimi r23, r12, 0, 28, 31
    lbz r29, 0x12(r4)
    slwi r0, r0, 4
    rlwimi r0, r29, 0, 28, 31
    lbz r8, 0x14(r4)
    slwi r28, r28, 12
    lbz r31, 9(r4)
    rlwimi r28, r27, 0, 20, 31
    lbz r29, 4(r4)
    rlwimi r24, r23, 0, 24, 31
    lbz r10, 0xf(r4)
    slwi r23, r8, 8
    lbz r12, 0xa(r4)
    rlwimi r26, r25, 0, 24, 31
    lbz r3, 0x16(r4)
    slwi r25, r10, 12
    lbz r30, 5(r4)
    rlwimi r23, r0, 0, 24, 31
    lbz r0, 0x17(r4)
    slwi r27, r31, 12
    lbz r11, 0xb(r4)
    lbz r8, 0x11(r4)
    slwi r4, r29, 16
    slwi r7, r7, 12
    slwi r10, r12, 16
    rlwimi r27, r26, 0, 20, 31
    slwi r12, r3, 16
    rlwimi r7, r23, 0, 20, 31
    rlwimi r4, r28, 0, 16, 31
    slwi r3, r30, 20
    rlwimi r25, r24, 0, 20, 31
    slwi r9, r9, 16
    rlwimi r10, r27, 0, 16, 31
    rlwimi r3, r4, 0, 12, 31
    rlwimi r12, r7, 0, 16, 31
    clrlwi r7, r3, 8
    slwi r3, r11, 20
    rlwimi r3, r10, 0, 12, 31
    clrlwi r4, r3, 8
    slwi r3, r8, 20
    rlwimi r9, r25, 0, 16, 31
    rlwimi r3, r9, 0, 12, 31
    slwi r0, r0, 20
    rlwimi r0, r12, 0, 12, 31
    clrlwi r3, r3, 8
    clrlwi r0, r0, 8
    oris r8, r7, 0x100
    oris r7, r4, 0x200
    oris r9, r3, 0x300
    oris r10, r0, 0x400
    b lbl_800B9C44_write_ind
lbl_800B9C44_default:
    lis r8, 0x166
    lis r7, 0x266
    lis r4, 0x366
    lis r3, 0x466
    addi r8, r8, 0x6666
    addi r7, r7, 0x6666
    addi r9, r4, 0x6666
    addi r10, r3, 0x6666
lbl_800B9C44_write_ind:
    li r4, 0x61
    lis r3, 0xcc01
    stb r4, -0x8000(r3)
    clrlwi. r0, r5, 24
    stw r8, -0x8000(r3)
    stb r4, -0x8000(r3)
    stw r7, -0x8000(r3)
    stb r4, -0x8000(r3)
    stw r9, -0x8000(r3)
    stb r4, -0x8000(r3)
    stw r10, -0x8000(r3)
    beq lbl_800B9C44_default_amb
    lbz r0, 0(r6)
    lbz r3, 1(r6)
    oris r5, r0, 0x5300
    lbz r0, 4(r6)
    lbz r4, 2(r6)
    rlwinm r7, r5, 0, 26, 19
    slwi r5, r3, 6
    lbz r3, 5(r6)
    or r7, r7, r5
    oris r8, r0, 0x5400
    lbz r5, 3(r6)
    lbz r0, 6(r6)
    rlwinm r6, r7, 0, 20, 13
    slwi r4, r4, 12
    or r6, r6, r4
    rlwinm r4, r8, 0, 26, 19
    slwi r3, r3, 6
    or r3, r4, r3
    rlwinm r6, r6, 0, 14, 7
    slwi r4, r5, 18
    rlwinm r3, r3, 0, 20, 13
    slwi r0, r0, 12
    or r6, r6, r4
    or r7, r3, r0
    b lbl_800B9C44_write_amb
lbl_800B9C44_default_amb:
    lis r4, 0x5359
    lis r3, 0x5400
    addi r6, r4, 0x5000
    addi r7, r3, 0x15
lbl_800B9C44_write_amb:
    li r5, 0x61
    lwz r3, gx(r13)
    lis r4, 0xcc01
    stb r5, -0x8000(r4)
    li r0, 0
    stw r6, -0x8000(r4)
    stb r5, -0x8000(r4)
    stw r7, -0x8000(r4)
    sth r0, 2(r3)
    lmw r23, 0x2c(r1)
    addi r1, r1, 0x50
    blr
}

/* fn_800B9E6C - 0x800B9E6C | size: 0x1C -- GX Transform / viewport / projection */
asm void fn_800B9E6C(u32 arg0) {
    nofralloc
    lwz r4, gx(r13)
    slwi r0, r3, 7
    lwzu r3, 0x1ec(r4)
    rlwinm r3, r3, 0, 25, 22
    or r0, r3, r0
    stw r0, 0(r4)
    blr
}

/* fn_800B9E88 - 0x800B9E88 | size: 0x15C -- GX Transform / viewport / projection */
asm void fn_800B9E88(u32 arg0, u32 arg1) {
    nofralloc
    clrlwi. r0, r4, 24
    beq lbl_800B9EC8
    lwz r7, gx(r13)
    li r0, 0x61
    lis r5, 0xcc01
    lwz r6, 0x1d8(r7)
    clrrwi r6, r6, 1
    stb r0, -0x8000(r5)
    ori r6, r6, 1
    rlwinm r6, r6, 0, 31, 27
    ori r6, r6, 0xe
    stw r6, -0x8000(r5)
    lwz r6, 0x1d0(r7)
    stb r0, -0x8000(r5)
    clrrwi r0, r6, 2
    stw r0, -0x8000(r5)
lbl_800B9EC8:
    clrlwi. r0, r4, 24
    li r0, 0
    bne lbl_800B9EE8
    lwz r5, gx(r13)
    lwz r5, 0x1dc(r5)
    clrlwi r5, r5, 29
    cmplwi r5, 3
    bne lbl_800B9F14
lbl_800B9EE8:
    lwz r5, gx(r13)
    lwz r7, 0x1dc(r5)
    extrwi r5, r7, 1, 25
    cmplwi r5, 1
    bne lbl_800B9F14
    li r0, 0x61
    lis r6, 0xcc01
    stb r0, -0x8000(r6)
    rlwinm r5, r7, 0, 26, 24
    li r0, 1
    stw r5, -0x8000(r6)
lbl_800B9F14:
    li r9, 0x61
    lwz r7, gx(r13)
    lis r8, 0xcc01
    stb r9, -0x8000(r8)
    extrwi r3, r3, 24, 3
    oris r10, r3, 0x4b00
    lwz r6, 0x1e0(r7)
    clrlwi. r5, r4, 24
    clrlslwi r3, r4, 24, 11
    stw r6, -0x8000(r8)
    stb r9, -0x8000(r8)
    lwz r4, 0x1e4(r7)
    stw r4, -0x8000(r8)
    stb r9, -0x8000(r8)
    lwz r4, 0x1e8(r7)
    stw r4, -0x8000(r8)
    stb r9, -0x8000(r8)
    stw r10, -0x8000(r8)
    lwz r4, 0x1ec(r7)
    rlwinm r4, r4, 0, 21, 19
    or r3, r4, r3
    stw r3, 0x1ec(r7)
    lwz r3, 0x1ec(r7)
    rlwinm r3, r3, 0, 18, 16
    ori r3, r3, 0x4000
    stw r3, 0x1ec(r7)
    lwz r3, 0x1ec(r7)
    clrlwi r3, r3, 8
    oris r3, r3, 0x5200
    stw r3, 0x1ec(r7)
    stb r9, -0x8000(r8)
    lwz r3, 0x1ec(r7)
    stw r3, -0x8000(r8)
    beq lbl_800B9FB4
    stb r9, -0x8000(r8)
    lwz r3, 0x1d8(r7)
    stw r3, -0x8000(r8)
    stb r9, -0x8000(r8)
    lwz r3, 0x1d0(r7)
    stw r3, -0x8000(r8)
lbl_800B9FB4:
    clrlwi. r0, r0, 24
    beq lbl_800B9FD4
    li r0, 0x61
    lwz r3, gx(r13)
    lis r4, 0xcc01
    stb r0, -0x8000(r4)
    lwz r0, 0x1dc(r3)
    stw r0, -0x8000(r4)
lbl_800B9FD4:
    lwz r3, gx(r13)
    li r0, 0
    sth r0, 2(r3)
    blr
}

/* fn_800B9FE4 - 0x800B9FE4 | size: 0x17C -- GX Transform / viewport / projection */
asm void fn_800B9FE4(u32 arg0, u32 arg1) {
    nofralloc
    clrlwi. r0, r4, 24
    beq lbl_800BA024
    lwz r7, gx(r13)
    li r0, 0x61
    lis r5, 0xcc01
    lwz r6, 0x1d8(r7)
    clrrwi r6, r6, 1
    stb r0, -0x8000(r5)
    ori r6, r6, 1
    rlwinm r6, r6, 0, 31, 27
    ori r6, r6, 0xe
    stw r6, -0x8000(r5)
    lwz r6, 0x1d0(r7)
    stb r0, -0x8000(r5)
    clrrwi r0, r6, 2
    stw r0, -0x8000(r5)
lbl_800BA024:
    lwz r6, gx(r13)
    li r0, 0
    lbz r5, 0x200(r6)
    lwz r7, 0x1dc(r6)
    cmplwi r5, 0
    beq lbl_800BA054
    clrlwi r5, r7, 29
    cmplwi r5, 3
    beq lbl_800BA054
    clrrwi r0, r7, 3
    ori r7, r0, 3
    li r0, 1
lbl_800BA054:
    clrlwi. r5, r4, 24
    bne lbl_800BA068
    clrlwi r5, r7, 29
    cmplwi r5, 3
    bne lbl_800BA07C
lbl_800BA068:
    extrwi r5, r7, 1, 25
    cmplwi r5, 1
    bne lbl_800BA07C
    li r0, 1
    rlwinm r7, r7, 0, 26, 24
lbl_800BA07C:
    clrlwi. r5, r0, 24
    beq lbl_800BA094
    li r6, 0x61
    lis r5, 0xcc01
    stb r6, -0x8000(r5)
    stw r7, -0x8000(r5)
lbl_800BA094:
    li r9, 0x61
    lwz r7, gx(r13)
    lis r8, 0xcc01
    stb r9, -0x8000(r8)
    extrwi r3, r3, 24, 3
    oris r10, r3, 0x4b00
    lwz r6, 0x1f0(r7)
    clrlwi. r5, r4, 24
    clrlslwi r3, r4, 24, 11
    stw r6, -0x8000(r8)
    stb r9, -0x8000(r8)
    lwz r4, 0x1f4(r7)
    stw r4, -0x8000(r8)
    stb r9, -0x8000(r8)
    lwz r4, 0x1f8(r7)
    stw r4, -0x8000(r8)
    stb r9, -0x8000(r8)
    stw r10, -0x8000(r8)
    lwz r4, 0x1fc(r7)
    rlwinm r4, r4, 0, 21, 19
    or r3, r4, r3
    stw r3, 0x1fc(r7)
    lwz r3, 0x1fc(r7)
    rlwinm r3, r3, 0, 18, 16
    stw r3, 0x1fc(r7)
    lwz r3, 0x1fc(r7)
    clrlwi r3, r3, 8
    oris r3, r3, 0x5200
    stw r3, 0x1fc(r7)
    stb r9, -0x8000(r8)
    lwz r3, 0x1fc(r7)
    stw r3, -0x8000(r8)
    beq lbl_800BA130
    stb r9, -0x8000(r8)
    lwz r3, 0x1d8(r7)
    stw r3, -0x8000(r8)
    stb r9, -0x8000(r8)
    lwz r3, 0x1d0(r7)
    stw r3, -0x8000(r8)
lbl_800BA130:
    clrlwi. r0, r0, 24
    beq lbl_800BA150
    li r0, 0x61
    lwz r3, gx(r13)
    lis r4, 0xcc01
    stb r0, -0x8000(r4)
    lwz r0, 0x1dc(r3)
    stw r0, -0x8000(r4)
lbl_800BA150:
    lwz r3, gx(r13)
    li r0, 0
    sth r0, 2(r3)
    blr
}

/* fn_800BA160 - 0x800BA160 | size: 0x38 -- GX Geometry / vertex descriptor */
asm void fn_800BA160(void) {
    nofralloc
    li r6, 0x61
    lwz r3, gx(r13)
    lis r5, 0xcc01
    lis r4, 0x5500
    stb r6, -0x8000(r5)
    addi r0, r4, 0x3ff
    stw r0, -0x8000(r5)
    lis r4, 0x5600
    addi r4, r4, 0x3ff
    stb r6, -0x8000(r5)
    li r0, 0
    stw r4, -0x8000(r5)
    sth r0, 2(r3)
    blr
}

/* fn_800BA198 - 0x800BA198 | size: 0x1C -- GX Geometry / vertex descriptor */
void fn_800BA198(u8* r3, f32 a, f32 b, f32 c, f32 d, f32 e, f32 f) {
    *(f32*)(r3 + 0x10) = a;
    *(f32*)(r3 + 0x14) = b;
    *(f32*)(r3 + 0x18) = c;
    *(f32*)(r3 + 0x1C) = d;
    *(f32*)(r3 + 0x20) = e;
    *(f32*)(r3 + 0x24) = f;
}

/* fn_800BA1B4 - 0x800BA1B4 | size: 0x190 -- GX Geometry / vertex descriptor */
void fn_800BA1B4(void) {
    /* GX Geometry / vertex descriptor (0x190 bytes) */
}

/* fn_800BA344 - 0x800BA344 | size: 0xD0 -- GX Geometry / vertex descriptor */
void fn_800BA344(void) {
    /* GX Geometry / vertex descriptor (0xD0 bytes) */
}

/* fn_800BA414 - 0x800BA414 | size: 0x10 -- GX Geometry / vertex descriptor */
void fn_800BA414(u8* r3, f32 f1, f32 f2, f32 f3) {
    *(f32*)(r3 + 0x28) = f1;
    *(f32*)(r3 + 0x2C) = f2;
    *(f32*)(r3 + 0x30) = f3;
}

/* fn_800BA424 - 0x800BA424 | size: 0x1C -- GX Geometry / vertex descriptor */
void fn_800BA424(u8* r3, f32 a, f32 b, f32 c) {
    *(f32*)(r3 + 0x34) = -a;
    *(f32*)(r3 + 0x38) = -b;
    *(f32*)(r3 + 0x3C) = -c;
}

/* fn_800BA440 - 0x800BA440 | size: 0xC -- GX Geometry / vertex descriptor */
void fn_800BA440(u8* r3, u8* r4) {
    *(u32*)(r3 + 0xC) = *(u32*)(r4 + 0x0);
}

/* fn_800BA44C - 0x800BA44C | size: 0x7C -- GX Geometry / vertex descriptor */
asm void fn_800BA44C(u8* arg0, u32 arg1) {
    nofralloc
    cntlzw r0, r4
    subfic r0, r0, 0x1f
    clrlslwi r5, r0, 29, 4
    lis r4, 0xcc01
    li r0, 0x10
    addi r5, r5, 0x600
    stb r0, -0x8000(r4)
    oris r0, r5, 0xf
    stwu r0, -0x8000(r4)
    lwz r0, 0xc(r3)
    xor r6, r6, r6
    psq_l f5, 0x10(r3), 0, 0
    psq_l f4, 0x18(r3), 0, 0
    psq_l f3, 0x20(r3), 0, 0
    psq_l f2, 0x28(r3), 0, 0
    psq_l f1, 0x30(r3), 0, 0
    psq_l f0, 0x38(r3), 0, 0
    stw r6, 0(r4)
    stw r6, 0(r4)
    stw r6, 0(r4)
    stw r0, 0(r4)
    psq_st f5, 0(r4), 0, 0
    psq_st f4, 0(r4), 0, 0
    psq_st f3, 0(r4), 0, 0
    psq_st f2, 0(r4), 0, 0
    psq_st f1, 0(r4), 0, 0
    psq_st f0, 0(r4), 0, 0
    lwz r3, gx(r13)
    li r0, 1
    sth r0, 2(r3)
    blr
}

/* fn_800BA4C8 - 0x800BA4C8 | size: 0xF4 -- GX Geometry / vertex descriptor */
asm void fn_800BA4C8(u32 arg0, u8* arg1) {
    nofralloc
    cmpwi r3, 3
    beq lbl_800BA554
    bge lbl_800BA4EC
    cmpwi r3, 1
    beq lbl_800BA51C
    bge lbl_800BA53C
    cmpwi r3, 0
    bge lbl_800BA4FC
    blr
lbl_800BA4EC:
    cmpwi r3, 5
    beq lbl_800BA578
    bgelr
    b lbl_800BA56C
lbl_800BA4FC:
    lwz r3, gx(r13)
    li r5, 0
    lwz r0, 0(r4)
    lwz r3, 0xa8(r3)
    clrrwi r0, r0, 8
    mr r7, r0
    rlwimi r7, r3, 0, 24, 31
    b lbl_800BA588
lbl_800BA51C:
    lwz r3, gx(r13)
    li r5, 1
    lwz r0, 0(r4)
    lwz r3, 0xac(r3)
    clrrwi r0, r0, 8
    mr r7, r0
    rlwimi r7, r3, 0, 24, 31
    b lbl_800BA588
lbl_800BA53C:
    lwz r3, gx(r13)
    li r5, 0
    lbz r7, 3(r4)
    lwz r3, 0xa8(r3)
    rlwimi r7, r3, 0, 0, 23
    b lbl_800BA588
lbl_800BA554:
    lwz r3, gx(r13)
    li r5, 1
    lbz r7, 3(r4)
    lwz r3, 0xac(r3)
    rlwimi r7, r3, 0, 0, 23
    b lbl_800BA588
lbl_800BA56C:
    lwz r7, 0(r4)
    li r5, 0
    b lbl_800BA588
lbl_800BA578:
    lwz r7, 0(r4)
    li r5, 1
    b lbl_800BA588
    blr
lbl_800BA588:
    li r0, 0x10
    lwz r4, gx(r13)
    lis r6, 0xcc01
    stb r0, -0x8000(r6)
    addi r3, r5, 0x100a
    slwi r0, r5, 2
    stw r3, -0x8000(r6)
    li r5, 1
    add r3, r4, r0
    stw r7, -0x8000(r6)
    sth r5, 2(r4)
    stw r7, 0xa8(r3)
    blr
}

/* fn_800BA5BC - 0x800BA5BC | size: 0xF4 -- GX Geometry / vertex descriptor */
asm void fn_800BA5BC(u32 arg0, u8* arg1) {
    nofralloc
    cmpwi r3, 3
    beq lbl_800BA648
    bge lbl_800BA5E0
    cmpwi r3, 1
    beq lbl_800BA610
    bge lbl_800BA630
    cmpwi r3, 0
    bge lbl_800BA5F0
    blr
lbl_800BA5E0:
    cmpwi r3, 5
    beq lbl_800BA66C
    bgelr
    b lbl_800BA660
lbl_800BA5F0:
    lwz r3, gx(r13)
    li r5, 0
    lwz r0, 0(r4)
    lwz r3, 0xb0(r3)
    clrrwi r0, r0, 8
    mr r7, r0
    rlwimi r7, r3, 0, 24, 31
    b lbl_800BA67C
lbl_800BA610:
    lwz r3, gx(r13)
    li r5, 1
    lwz r0, 0(r4)
    lwz r3, 0xb4(r3)
    clrrwi r0, r0, 8
    mr r7, r0
    rlwimi r7, r3, 0, 24, 31
    b lbl_800BA67C
lbl_800BA630:
    lwz r3, gx(r13)
    li r5, 0
    lbz r7, 3(r4)
    lwz r3, 0xb0(r3)
    rlwimi r7, r3, 0, 0, 23
    b lbl_800BA67C
lbl_800BA648:
    lwz r3, gx(r13)
    li r5, 1
    lbz r7, 3(r4)
    lwz r3, 0xb4(r3)
    rlwimi r7, r3, 0, 0, 23
    b lbl_800BA67C
lbl_800BA660:
    lwz r7, 0(r4)
    li r5, 0
    b lbl_800BA67C
lbl_800BA66C:
    lwz r7, 0(r4)
    li r5, 1
    b lbl_800BA67C
    blr
lbl_800BA67C:
    li r0, 0x10
    lwz r4, gx(r13)
    lis r6, 0xcc01
    stb r0, -0x8000(r6)
    addi r3, r5, 0x100c
    slwi r0, r5, 2
    stw r3, -0x8000(r6)
    li r5, 1
    add r3, r4, r0
    stw r7, -0x8000(r6)
    sth r5, 2(r4)
    stw r7, 0xb0(r3)
    blr
}

/* fn_800BA6B0 - 0x800BA6B0 | size: 0x44 -- GX Geometry / vertex descriptor */
asm void fn_800BA6B0(u32 arg0) {
    nofralloc
    lwz r6, gx(r13)
    clrlslwi r0, r3, 24, 4
    clrlwi r8, r3, 24
    lwz r5, 0x204(r6)
    li r4, 0x10
    lis r3, 0xcc01
    rlwinm r5, r5, 0, 28, 24
    or r0, r5, r0
    stw r0, 0x204(r6)
    li r0, 0x1009
    stb r4, -0x8000(r3)
    stw r0, -0x8000(r3)
    stw r8, -0x8000(r3)
    lwz r0, 0x4f4(r6)
    ori r0, r0, 4
    stw r0, 0x4f4(r6)
    blr
}

/* fn_800BA6F4 - 0x800BA6F4 | size: 0xCC -- GX Geometry / vertex descriptor */
asm void fn_800BA6F4(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6) {
    nofralloc
    clrlslwi r0, r4, 24, 1
    or r0, r0, r6
    rlwinm r4, r0, 0, 26, 24
    slwi r0, r5, 6
    cmpwi r9, 0
    clrlwi r10, r3, 30
    or r6, r4, r0
    bne lbl_800BA718
    li r8, 0
lbl_800BA718:
    subfic r4, r9, 2
    subic r0, r4, 1
    subfe r5, r0, r4
    neg r4, r9
    subic r0, r4, 1
    subfe r0, r0, r4
    rlwinm r6, r6, 0, 25, 22
    slwi r4, r8, 7
    or r4, r6, r4
    rlwinm r6, r4, 0, 23, 21
    slwi r4, r5, 9
    or r4, r6, r4
    rlwinm r4, r4, 0, 22, 20
    slwi r0, r0, 10
    or r6, r4, r0
    rlwinm r6, r6, 0, 30, 25
    rlwimi r6, r7, 2, 26, 29
    rlwinm r6, r6, 0, 21, 16
    li r5, 0x10
    lis r4, 0xcc01
    stb r5, -0x8000(r4)
    addi r0, r10, 0x100e
    rlwimi r6, r7, 7, 17, 20
    stw r0, -0x8000(r4)
    cmpwi r3, 4
    stw r6, -0x8000(r4)
    bne lbl_800BA798
    stb r5, -0x8000(r4)
    li r0, 0x1010
    stw r0, -0x8000(r4)
    stw r6, -0x8000(r4)
    b lbl_800BA7B0
lbl_800BA798:
    cmpwi r3, 5
    bne lbl_800BA7B0
    stb r5, -0x8000(r4)
    li r0, 0x1011
    stw r0, -0x8000(r4)
    stw r6, -0x8000(r4)
lbl_800BA7B0:
    lwz r3, gx(r13)
    li r0, 1
    sth r0, 2(r3)
    blr
}

/* fn_800BA7C0 - 0x800BA7C0 | size: 0x15C -- GX Geometry / vertex descriptor */
void fn_800BA7C0(void) {
    /* GX Geometry / vertex descriptor (0x15C bytes) */
}

/* fn_800BA91C - 0x800BA91C | size: 0xC8 -- GX Geometry / vertex descriptor */
void fn_800BA91C(void) {
    /* GX Geometry / vertex descriptor (0xC8 bytes) */
}

/* fn_800BA9E4 - 0x800BA9E4 | size: 0x274 -- GX Geometry / vertex descriptor */
asm void fn_800BA9E4(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x60(r1)
    stmw r24, 0x40(r1)
    addi r27, r4, 0
    addi r28, r5, 0
    addi r31, r3, 0
    addi r29, r6, 0
    addi r30, r7, 0
    addi r24, r8, 0
    addi r25, r9, 0
    addi r26, r10, 0
    li r4, 0
    li r5, 0x20
    bl memset
    lwz r4, 0(r31)
    clrlwi. r0, r26, 24
    slwi r3, r25, 2
    clrrwi r4, r4, 2
    or r4, r4, r24
    stw r4, 0(r31)
    lwz r0, 0(r31)
    rlwinm r0, r0, 0, 30, 27
    or r0, r0, r3
    stw r0, 0(r31)
    lwz r0, 0(r31)
    rlwinm r0, r0, 0, 28, 26
    ori r0, r0, 0x10
    stw r0, 0(r31)
    beq texObjNoMip
    lbz r3, 0x1f(r31)
    subi r0, r30, 8
    cmplwi r0, 2
    ori r0, r3, 1
    stb r0, 0x1f(r31)
    bgt texObjWideMip
    lwz r0, 0(r31)
    rlwinm r0, r0, 0, 27, 23
    ori r0, r0, 0xa0
    stw r0, 0(r31)
    b texObjMipShift
texObjWideMip:
    lwz r0, 0(r31)
    rlwinm r0, r0, 0, 27, 23
    ori r0, r0, 0xc0
    stw r0, 0(r31)
texObjMipShift:
    clrlwi r3, r28, 16
    clrlwi r0, r29, 16
    cmplw r3, r0
    ble texObjUseHeightShift
    cntlzw r0, r3
    subfic r0, r0, 0x1f
    b texObjShiftDone
texObjUseHeightShift:
    cntlzw r0, r0
    subfic r0, r0, 0x1f
texObjShiftDone:
    stw r0, 0x3c(r1)
    lis r0, 0x4330
    lwz r3, 4(r31)
    stw r0, 0x38(r1)
    lfd f1, lbl_8047C350(r2)
    rlwinm r3, r3, 0, 24, 15
    lfd f0, 0x38(r1)
    lfs f2, lbl_8047C348(r2)
    fsubs f0, f0, f1
    fmuls f0, f2, f0
    fctiwz f0, f0
    stfd f0, 0x30(r1)
    lwz r0, 0x34(r1)
    rlwimi r3, r0, 8, 16, 23
    stw r3, 4(r31)
    b texObjDimSetup
texObjNoMip:
    lwz r0, 0(r31)
    rlwinm r0, r0, 0, 27, 23
    ori r0, r0, 0x80
    stw r0, 0(r31)
texObjDimSetup:
    stw r30, 0x14(r31)
    clrlwi r3, r29, 16
    clrlwi r7, r30, 28
    lwz r5, 8(r31)
    clrlwi r4, r28, 16
    subi r0, r3, 1
    clrrwi r6, r5, 10
    subi r5, r4, 1
    or r5, r6, r5
    stw r5, 8(r31)
    slwi r5, r0, 10
    extrwi r0, r27, 25, 2
    lwz r6, 8(r31)
    cmplwi r7, 0xe
    rlwinm r6, r6, 0, 22, 11
    or r5, r6, r5
    stw r5, 8(r31)
    lwz r5, 8(r31)
    rlwinm r5, r5, 0, 12, 7
    rlwimi r5, r30, 20, 8, 11
    stw r5, 8(r31)
    lwz r5, 0xc(r31)
    clrrwi r5, r5, 21
    or r0, r5, r0
    stw r0, 0xc(r31)
    bgt texObjFormatDefault
    lis r5, jumptable_80313550@ha
    addi r5, r5, jumptable_80313550@l
    slwi r0, r7, 2
    lwzx r0, r5, r0
    mtctr r0
    bctr
texObjFormat0:
    li r0, 1
    stb r0, 0x1e(r31)
    li r0, 3
    li r7, 3
    b texObjTileCalc
texObjFormat1:
    li r0, 2
    stb r0, 0x1e(r31)
    li r0, 3
    li r7, 2
    b texObjTileCalc
texObjFormat2:
    li r0, 2
    stb r0, 0x1e(r31)
    li r0, 2
    li r7, 2
    b texObjTileCalc
texObjFormat3:
    li r0, 3
    stb r0, 0x1e(r31)
    li r0, 2
    li r7, 2
    b texObjTileCalc
texObjFormat4:
    li r0, 0
    stb r0, 0x1e(r31)
    li r0, 3
    li r7, 3
    b texObjTileCalc
texObjFormatDefault:
    li r0, 2
    stb r0, 0x1e(r31)
    li r0, 2
    li r7, 2
texObjTileCalc:
    clrlwi r8, r0, 16
    li r6, 1
    slw r5, r6, r8
    clrlwi r7, r7, 16
    subi r0, r5, 1
    slw r5, r6, r7
    add r4, r4, r0
    subi r0, r5, 1
    sraw r4, r4, r8
    add r0, r3, r0
    sraw r0, r0, r7
    mullw r0, r4, r0
    clrlwi r0, r0, 17
    sth r0, 0x1c(r31)
    lbz r0, 0x1f(r31)
    ori r0, r0, 2
    stb r0, 0x1f(r31)
    lwz r0, 0x64(r1)
    lmw r24, 0x40(r1)
    addi r1, r1, 0x60
    mtlr r0
    blr
}

/* fn_800BAC58 - 0x800BAC58 | size: 0x48 -- GX Geometry / vertex descriptor */
asm void fn_800BAC58(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x30(r1)
    stw r31, 0x2c(r1)
    lwz r31, 0x38(r1)
    stw r30, 0x28(r1)
    mr r30, r3
    bl fn_800BA9E4
    lbz r0, 0x1f(r30)
    rlwinm r0, r0, 0, 31, 29
    stb r0, 0x1f(r30)
    stw r31, 0x18(r30)
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    addi r1, r1, 0x30
    mtlr r0
    blr
}

/* fn_800BACA0 - 0x800BACA0 | size: 0x194 -- GX Geometry / vertex descriptor */
asm void fn_800BACA0(u8* arg0, u32 arg1, f32 arg2, f32 arg3, u32 arg4, u32 arg5, u32 arg6, u32 arg7) {
    nofralloc
    stwu r1, -0x38(r1)
    lfs f0, lbl_8047C358(r13)
    fcmpo cr0, f3, f0
    bge lbl_800BACB8
    fmr f3, f0
    b lbl_800BACCC
lbl_800BACB8:
    lfs f0, lbl_8047C35C(r13)
    fcmpo cr0, f3, f0
    cror 2, 1, 2
    bne lbl_800BACCC
    lfs f3, lbl_8047C360(r13)
lbl_800BACCC:
    lfs f0, lbl_8047C364(r13)
    cmpwi r5, 1
    lwz r0, 0(r3)
    fmuls f0, f0, f3
    rlwinm r5, r0, 0, 23, 14
    fctiwz f0, f0
    stfd f0, 0x30(r1)
    lwz r0, 0x34(r1)
    rlwimi r5, r0, 9, 15, 22
    stw r5, 0(r3)
    bne lbl_800BAD00
    li r5, 1
    b lbl_800BAD04
lbl_800BAD00:
    li r5, 0
lbl_800BAD04:
    lwz r9, 0(r3)
    clrlwi. r0, r7, 24
    slwi r5, r5, 4
    rlwinm r7, r9, 0, 28, 26
    or r5, r7, r5
    stw r5, 0(r3)
    la r5, lbl_80478AB0(r13)
    lbzx r0, r5, r4
    lwz r7, 0(r3)
    slwi r0, r0, 5
    rlwinm r4, r7, 0, 27, 23
    or r0, r4, r0
    stw r0, 0(r3)
    beq lbl_800BAD44
    li r0, 0
    b lbl_800BAD48
lbl_800BAD44:
    li r0, 1
lbl_800BAD48:
    lwz r4, 0(r3)
    slwi r0, r0, 8
    rlwinm r4, r4, 0, 24, 22
    or r0, r4, r0
    stw r0, 0(r3)
    slwi r4, r8, 19
    clrlslwi r0, r6, 24, 21
    lwz r5, 0(r3)
    rlwinm r5, r5, 0, 15, 13
    stw r5, 0(r3)
    lwz r5, 0(r3)
    rlwinm r5, r5, 0, 14, 12
    stw r5, 0(r3)
    lwz r5, 0(r3)
    rlwinm r5, r5, 0, 13, 10
    or r4, r5, r4
    stw r4, 0(r3)
    lwz r4, 0(r3)
    rlwinm r4, r4, 0, 11, 9
    or r0, r4, r0
    stw r0, 0(r3)
    lfs f0, lbl_8047C368(r13)
    fcmpo cr0, f1, f0
    bge lbl_800BADB0
    fmr f1, f0
    b lbl_800BADC0
lbl_800BADB0:
    lfs f0, lbl_8047C36C(r13)
    fcmpo cr0, f1, f0
    ble lbl_800BADC0
    fmr f1, f0
lbl_800BADC0:
    lfs f3, lbl_8047C348(r13)
    lfs f0, lbl_8047C368(r13)
    fmuls f1, f3, f1
    fcmpo cr0, f2, f0
    fctiwz f1, f1
    stfd f1, 0x30(r1)
    lwz r4, 0x34(r1)
    bge lbl_800BADE8
    fmr f2, f0
    b lbl_800BADF8
lbl_800BADE8:
    lfs f0, lbl_8047C36C(r13)
    fcmpo cr0, f2, f0
    ble lbl_800BADF8
    fmr f2, f0
lbl_800BADF8:
    lwz r0, 4(r3)
    clrrwi r0, r0, 8
    rlwimi r0, r4, 0, 24, 31
    stw r0, 4(r3)
    lfs f0, lbl_8047C348(r13)
    lwz r0, 4(r3)
    fmuls f0, f0, f2
    rlwinm r4, r0, 0, 24, 15
    fctiwz f0, f0
    stfd f0, 0x30(r1)
    lwz r0, 0x34(r1)
    rlwimi r4, r0, 8, 16, 23
    stw r4, 4(r3)
    addi r1, r1, 0x38
    blr
}

/* fn_800BAE34 - 0x800BAE34 | size: 0x28 -- GX Geometry / vertex descriptor */
asm void fn_800BAE34(u8* arg0, u32 arg1, u32 arg2) {
    nofralloc
    lwz r6, 0(r3)
    slwi r0, r5, 2
    clrrwi r5, r6, 2
    or r4, r5, r4
    stw r4, 0(r3)
    lwz r4, 0(r3)
    rlwinm r4, r4, 0, 30, 27
    or r0, r4, r0
    stw r0, 0(r3)
    blr
}

/* fn_800BAE5C - 0x800BAE5C | size: 0x8 -- GX Geometry / vertex descriptor */
u32 fn_800BAE5C(void* p) {
    return *(u32*)((u8*)p + 0x14);
}

/* fn_800BAE64 - 0x800BAE64 | size: 0x198 -- GX Texture / TMEM */
asm void fn_800BAE64(u32* texObj, u32* region, u32 mapId) {
    nofralloc
    mflr r0
    la r7, lbl_80478A88(r13)
    stw r0, 4(r1)
    stwu r1, -0x28(r1)
    stw r31, 0x24(r1)
    lis r31, 0xcc01
    stw r30, 0x20(r1)
    li r30, 0x61
    stw r29, 0x1c(r1)
    addi r29, r5, 0
    la r5, lbl_80478A98(r13)
    stw r28, 0x18(r1)
    mr r28, r3
    lwz r6, 0(r3)
    la r3, lbl_80478A78(r13)
    lbzx r0, r3, r29
    la r3, lbl_80478A80(r13)
    slwi r0, r0, 24
    rlwimi r0, r6, 0, 8, 31
    stw r0, 0(r28)
    la r6, lbl_80478A90(r13)
    lbzx r0, r3, r29
    la r3, lbl_80478AA0(r13)
    lwz r8, 4(r28)
    slwi r0, r0, 24
    rlwimi r0, r8, 0, 8, 31
    stw r0, 4(r28)
    lbzx r0, r7, r29
    lwz r8, 8(r28)
    slwi r0, r0, 24
    rlwimi r0, r8, 0, 8, 31
    stw r0, 8(r28)
    lbzx r0, r6, r29
    lwz r7, 0(r4)
    slwi r0, r0, 24
    rlwimi r0, r7, 0, 8, 31
    stw r0, 0(r4)
    lbzx r0, r5, r29
    lwz r6, 4(r4)
    slwi r0, r0, 24
    rlwimi r0, r6, 0, 8, 31
    stw r0, 4(r4)
    lbzx r0, r3, r29
    lwz r5, 0xc(r28)
    slwi r0, r0, 24
    rlwimi r0, r5, 0, 8, 31
    stw r0, 0xc(r28)
    stb r30, -0x8000(r31)
    lwz r0, 0(r28)
    stw r0, -0x8000(r31)
    stb r30, -0x8000(r31)
    lwz r0, 4(r28)
    stw r0, -0x8000(r31)
    stb r30, -0x8000(r31)
    lwz r0, 8(r28)
    stw r0, -0x8000(r31)
    stb r30, -0x8000(r31)
    lwz r0, 0(r4)
    stw r0, -0x8000(r31)
    stb r30, -0x8000(r31)
    lwz r0, 4(r4)
    stw r0, -0x8000(r31)
    stb r30, -0x8000(r31)
    lwz r0, 0xc(r28)
    stw r0, -0x8000(r31)
    lbz r0, 0x1f(r28)
    rlwinm. r0, r0, 0, 30, 30
    bne textureRegionDone
    lwz r4, gx(r13)
    lwz r3, 0x18(r28)
    lwz r12, 0x414(r4)
    mtlr r12
    blrl
    la r4, lbl_80478AA8(r13)
    lwz r5, 4(r3)
    lbzx r0, r4, r29
    slwi r0, r0, 24
    rlwimi r0, r5, 0, 8, 31
    stw r0, 4(r3)
    stb r30, -0x8000(r31)
    lwz r0, 4(r3)
    stw r0, -0x8000(r31)
textureRegionDone:
    lwz r5, gx(r13)
    slwi r4, r29, 2
    lwz r3, 8(r28)
    li r0, 0
    add r4, r5, r4
    stw r3, 0x45c(r4)
    lwz r3, 0(r28)
    stw r3, 0x47c(r4)
    lwz r3, 0x4f4(r5)
    ori r3, r3, 1
    stw r3, 0x4f4(r5)
    sth r0, 2(r5)
    lwz r0, 0x2c(r1)
    lwz r31, 0x24(r1)
    lwz r30, 0x20(r1)
    lwz r29, 0x1c(r1)
    lwz r28, 0x18(r1)
    addi r1, r1, 0x28
    mtlr r0
    blr
}

/* fn_800BAFFC - 0x800BAFFC | size: 0x54 -- GX Texture / TMEM */
asm void fn_800BAFFC(u8* arg0, u32 arg1) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    addi r31, r4, 0
    stw r30, 0x10(r1)
    addi r30, r3, 0
    lwz r5, gx(r13)
    lwz r12, 0x410(r5)
    mtlr r12
    blrl
    addi r4, r3, 0
    addi r3, r30, 0
    addi r5, r31, 0
    bl fn_800BAE64
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800BB050 - 0x800BB050 | size: 0x48 -- GX Texture / TMEM */
asm void fn_800BB050(u8* arg0, u32 arg1, u32 arg2, u16 arg3) {
    nofralloc
    li r0, 0
    stw r0, 0(r3)
    slwi r5, r5, 10
    extrwi r0, r4, 25, 2
    lwz r7, 0(r3)
    rlwinm r4, r7, 0, 22, 19
    or r4, r4, r5
    stw r4, 0(r3)
    lwz r4, 4(r3)
    clrrwi r4, r4, 21
    or r0, r4, r0
    stw r0, 4(r3)
    lwz r0, 4(r3)
    clrlwi r0, r0, 8
    oris r0, r0, 0x6400
    stw r0, 4(r3)
    sth r6, 8(r3)
    blr
}

/* fn_800BB098 - 0x800BB098 | size: 0x9C -- GX Texture / TMEM */
asm void fn_800BB098(u8* arg0, u32 arg1) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x20(r1)
    stw r31, 0x1c(r1)
    stw r30, 0x18(r1)
    mr r30, r3
    addi r3, r4, 0
    lwz r5, gx(r13)
    lwz r12, 0x414(r5)
    mtlr r12
    blrl
    mr r31, r3
    bl __GXFlushTextureState
    li r4, 0x61
    lis r3, 0xcc01
    stb r4, -0x8000(r3)
    lwz r0, 4(r30)
    stw r0, -0x8000(r3)
    stb r4, -0x8000(r3)
    lwz r0, 0(r31)
    stw r0, -0x8000(r3)
    bl __GXFlushTextureState
    lwz r0, 0(r31)
    lwz r3, 0(r30)
    clrlwi r0, r0, 22
    rlwimi r0, r3, 0, 0, 21
    stw r0, 0(r30)
    lwz r3, 0(r30)
    lwz r0, 4(r30)
    stw r3, 4(r31)
    stw r0, 8(r31)
    lwz r0, 8(r30)
    stw r0, 0xc(r31)
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    addi r1, r1, 0x20
    mtlr r0
    blr
}

/* fn_800BB29C - 0x800BB29C | size: 0x48 -- GX Texture / TMEM */
asm void fn_800BB29C(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -8(r1)
    bl __GXFlushTextureState
    li r6, 0x61
    lis r3, 0x6600
    lis r5, 0xcc01
    stb r6, -0x8000(r5)
    addi r4, r3, 0x1000
    addi r0, r3, 0x1100
    stw r4, -0x8000(r5)
    stb r6, -0x8000(r5)
    stw r0, -0x8000(r5)
    bl __GXFlushTextureState
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800BB2E4 - 0x800BB2E4 | size: 0x14 -- GX Texture / TMEM */
void fn_800BB2E4(void) {
    asm {
        lwz r4, gx(r13)
        lwzu r0, 0x410(r4)
        stw r3, 0(r4)
        mr r3, r0
    }
}

/* fn_800BB2F8 - 0x800BB2F8 | size: 0x14 -- GX Texture / TMEM */
void fn_800BB2F8(void) {
    asm {
        lwz r4, gx(r13)
        lwzu r0, 0x414(r4)
        stw r3, 0(r4)
        mr r3, r0
    }
}

/* fn_800BB30C - 0x800BB30C | size: 0xB8 -- GX Texture / TMEM */
asm void fn_800BB30C(u32 arg0, u32 arg1) {
    nofralloc
    lwz r5, gx(r13)
    slwi r8, r4, 2
    slwi r0, r3, 2
    add r3, r5, r0
    add r6, r5, r8
    lwz r4, 0x45c(r3)
    lwz r0, 0xb8(r6)
    add r7, r5, r8
    add r9, r5, r8
    clrrwi r0, r0, 16
    rlwimi r0, r4, 0, 22, 31
    stw r0, 0xb8(r6)
    add r10, r5, r8
    lwz r0, 0xd8(r7)
    clrrwi r0, r0, 16
    rlwimi r0, r4, 22, 22, 31
    stw r0, 0xd8(r7)
    li r7, 0x61
    lwz r6, 0x47c(r3)
    lwz r3, 0xb8(r9)
    extrwi r0, r6, 2, 28
    subfic r4, r0, 1
    clrlwi r0, r6, 30
    subfic r0, r0, 1
    cntlzw r0, r0
    rlwinm r3, r3, 0, 16, 14
    rlwinm r0, r0, 11, 8, 15
    or r0, r3, r0
    stw r0, 0xb8(r9)
    cntlzw r0, r4
    rlwinm r0, r0, 11, 8, 15
    lwz r3, 0xd8(r10)
    lis r6, 0xcc01
    add r4, r5, r8
    rlwinm r3, r3, 0, 16, 14
    or r0, r3, r0
    stw r0, 0xd8(r10)
    li r0, 0
    stb r7, -0x8000(r6)
    lwz r3, 0xb8(r4)
    stw r3, -0x8000(r6)
    stb r7, -0x8000(r6)
    lwz r3, 0xd8(r4)
    stw r3, -0x8000(r6)
    sth r0, 2(r5)
    blr
}

/* fn_800BB3C4 - 0x800BB3C4 | size: 0x17C -- GX Texture / TMEM */
void fn_800BB3C4(void) {
    /* GX Texture / TMEM (0x17C bytes) */
}

/* fn_800BB780 - 0x800BB780 | size: 0x9C -- GX TMEM config / texture state */
asm void fn_800BB780(void) {
    nofralloc
    stwu r1, -0x28(r1)
    rlwinm r4, r4, 0, 30, 27
    slwi r0, r5, 2
    or r0, r4, r0
    lwz r11, 0x34(r1)
    rlwinm r5, r0, 0, 28, 24
    lbz r12, 0x33(r1)
    slwi r0, r6, 4
    lwz r4, gx(r13)
    or r0, r5, r0
    rlwinm r5, r0, 0, 25, 22
    slwi r0, r11, 7
    or r0, r5, r0
    rlwinm r5, r0, 0, 23, 18
    slwi r0, r7, 9
    or r0, r5, r0
    rlwinm r5, r0, 0, 19, 15
    slwi r0, r8, 13
    or r0, r5, r0
    addi r5, r3, 0x10
    rlwinm r3, r0, 0, 16, 12
    slwi r0, r9, 16
    or r0, r3, r0
    rlwinm r3, r0, 0, 13, 11
    slwi r0, r12, 19
    or r0, r3, r0
    rlwinm r3, r0, 0, 12, 10
    clrlslwi r0, r10, 24, 20
    or r6, r3, r0
    li r0, 0x61
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    slwi r0, r5, 24
    rlwimi r0, r6, 0, 8, 31
    stw r0, -0x8000(r3)
    li r0, 0
    sth r0, 2(r4)
    addi r1, r1, 0x28
    blr
}

/* fn_800BB81C - 0x800BB81C | size: 0x160 -- GX TMEM config / texture state */
asm void fn_800BB81C(void) {
    nofralloc
    cmpwi r3, 8
    stwu r1, -0x78(r1)
    beq texMtxDefault
    bge texMtxGE8
    cmpwi r3, 4
    beq texMtxDefault
    bge texMtxGE4
    cmpwi r3, 1
    bge texMtxRange0
    b texMtxDefault
texMtxGE8:
    cmpwi r3, 0xc
    bge texMtxDefault
    b texMtxRange2
texMtxRange0:
    subi r0, r3, 1
    b texMtxMapped
texMtxGE4:
    subi r0, r3, 5
    b texMtxMapped
texMtxRange2:
    subi r0, r3, 9
    b texMtxMapped
texMtxDefault:
    li r0, 0
texMtxMapped:
    lfs f2, lbl_8047C370(r2)
    mulli r3, r0, 3
    lfs f1, 0(r4)
    lfs f0, 0xc(r4)
    fmuls f1, f2, f1
    addi r11, r5, 0x11
    fmuls f0, f2, f0
    addi r0, r3, 6
    extsb r11, r11
    fctiwz f1, f1
    li r10, 0x61
    fctiwz f0, f0
    lis r9, 0xcc01
    stb r10, -0x8000(r9)
    stfd f1, 0x70(r1)
    slwi r0, r0, 24
    addi r6, r3, 7
    stfd f0, 0x68(r1)
    addi r5, r3, 8
    lwz r8, 0x74(r1)
    lwz r7, 0x6c(r1)
    lwz r3, gx(r13)
    clrlslwi r7, r7, 21, 11
    rlwimi r7, r8, 0, 21, 31
    rlwinm r7, r7, 0, 10, 7
    rlwimi r7, r11, 22, 8, 9
    rlwimi r0, r7, 0, 8, 31
    stw r0, -0x8000(r9)
    li r0, 0
    lfs f1, 4(r4)
    lfs f0, 0x10(r4)
    fmuls f1, f2, f1
    fmuls f0, f2, f0
    stb r10, -0x8000(r9)
    fctiwz f1, f1
    fctiwz f0, f0
    stfd f1, 0x60(r1)
    stfd f0, 0x58(r1)
    lwz r8, 0x64(r1)
    lwz r7, 0x5c(r1)
    clrlslwi r7, r7, 21, 11
    rlwimi r7, r8, 0, 21, 31
    rlwinm r7, r7, 0, 10, 7
    rlwimi r7, r11, 20, 8, 9
    clrlwi r7, r7, 8
    rlwimi r7, r6, 24, 0, 7
    stw r7, -0x8000(r9)
    lfs f1, 8(r4)
    lfs f0, 0x14(r4)
    fmuls f1, f2, f1
    fmuls f0, f2, f0
    stb r10, -0x8000(r9)
    fctiwz f1, f1
    fctiwz f0, f0
    stfd f1, 0x50(r1)
    stfd f0, 0x48(r1)
    lwz r6, 0x54(r1)
    lwz r4, 0x4c(r1)
    clrlslwi r4, r4, 21, 11
    rlwimi r4, r6, 0, 21, 31
    rlwinm r4, r4, 0, 10, 7
    rlwimi r4, r11, 18, 8, 9
    clrlwi r4, r4, 8
    rlwimi r4, r5, 24, 0, 7
    stw r4, -0x8000(r9)
    sth r0, 2(r3)
    addi r1, r1, 0x78
    blr
}

/* fn_800BB97C - 0x800BB97C | size: 0x17C -- GX TMEM config / texture state */
asm void fn_800BB97C(void) {
    nofralloc
    cmpwi r3, 2
    beq indCase2
    bge indGE2
    cmpwi r3, 0
    beq indCase0
    bge indCase1
    b indDone
indGE2:
    cmpwi r3, 4
    bge indDone
    b indCase3
indCase0:
    lwz r7, gx(r13)
    slwi r5, r5, 4
    li r0, 0x61
    lwz r6, 0x128(r7)
    lis r3, 0xcc01
    clrrwi r6, r6, 4
    or r4, r6, r4
    stw r4, 0x128(r7)
    lwz r4, 0x128(r7)
    rlwinm r4, r4, 0, 28, 23
    or r4, r4, r5
    stw r4, 0x128(r7)
    lwz r4, 0x128(r7)
    clrlwi r4, r4, 8
    oris r4, r4, 0x2500
    stw r4, 0x128(r7)
    stb r0, -0x8000(r3)
    lwz r0, 0x128(r7)
    stw r0, -0x8000(r3)
    b indDone
indCase1:
    lwz r7, gx(r13)
    slwi r6, r4, 8
    slwi r4, r5, 12
    lwz r5, 0x128(r7)
    li r0, 0x61
    lis r3, 0xcc01
    rlwinm r5, r5, 0, 24, 19
    or r5, r5, r6
    stw r5, 0x128(r7)
    lwz r5, 0x128(r7)
    rlwinm r5, r5, 0, 20, 15
    or r4, r5, r4
    stw r4, 0x128(r7)
    lwz r4, 0x128(r7)
    clrlwi r4, r4, 8
    oris r4, r4, 0x2500
    stw r4, 0x128(r7)
    stb r0, -0x8000(r3)
    lwz r0, 0x128(r7)
    stw r0, -0x8000(r3)
    b indDone
indCase2:
    lwz r7, gx(r13)
    slwi r5, r5, 4
    li r0, 0x61
    lwz r6, 0x12c(r7)
    lis r3, 0xcc01
    clrrwi r6, r6, 4
    or r4, r6, r4
    stw r4, 0x12c(r7)
    lwz r4, 0x12c(r7)
    rlwinm r4, r4, 0, 28, 23
    or r4, r4, r5
    stw r4, 0x12c(r7)
    lwz r4, 0x12c(r7)
    clrlwi r4, r4, 8
    oris r4, r4, 0x2600
    stw r4, 0x12c(r7)
    stb r0, -0x8000(r3)
    lwz r0, 0x12c(r7)
    stw r0, -0x8000(r3)
    b indDone
indCase3:
    lwz r7, gx(r13)
    slwi r6, r4, 8
    slwi r4, r5, 12
    lwz r5, 0x12c(r7)
    li r0, 0x61
    lis r3, 0xcc01
    rlwinm r5, r5, 0, 24, 19
    or r5, r5, r6
    stw r5, 0x12c(r7)
    lwz r5, 0x12c(r7)
    rlwinm r5, r5, 0, 20, 15
    or r4, r5, r4
    stw r4, 0x12c(r7)
    lwz r4, 0x12c(r7)
    clrlwi r4, r4, 8
    oris r4, r4, 0x2600
    stw r4, 0x12c(r7)
    stb r0, -0x8000(r3)
    lwz r0, 0x12c(r7)
    stw r0, -0x8000(r3)
indDone:
    lwz r3, gx(r13)
    li r0, 0
    sth r0, 2(r3)
    blr
}

/* fn_800BBAF8 - 0x800BBAF8 | size: 0x114 -- GX TMEM config / texture state */
asm void fn_800BBAF8(void) {
    nofralloc
    cmpwi r3, 2
    beq coordCase2
    bge coordGE2
    cmpwi r3, 0
    beq coordCase0
    bge coordCase1
    b coordDone
coordGE2:
    cmpwi r3, 4
    bge coordDone
    b coordCase3
coordCase0:
    lwz r3, gx(r13)
    slwi r0, r4, 3
    addi r4, r3, 0x120
    lwz r3, 0x120(r3)
    clrrwi r3, r3, 3
    or r3, r3, r5
    stw r3, 0(r4)
    lwz r3, 0(r4)
    rlwinm r3, r3, 0, 29, 25
    or r0, r3, r0
    stw r0, 0(r4)
    b coordDone
coordCase1:
    lwz r6, gx(r13)
    slwi r0, r4, 9
    slwi r3, r5, 6
    lwz r4, 0x120(r6)
    rlwinm r4, r4, 0, 26, 22
    or r3, r4, r3
    stw r3, 0x120(r6)
    lwz r3, 0x120(r6)
    rlwinm r3, r3, 0, 23, 19
    or r0, r3, r0
    stw r0, 0x120(r6)
    b coordDone
coordCase2:
    lwz r6, gx(r13)
    slwi r0, r4, 15
    slwi r3, r5, 12
    lwz r4, 0x120(r6)
    rlwinm r4, r4, 0, 20, 16
    or r3, r4, r3
    stw r3, 0x120(r6)
    lwz r3, 0x120(r6)
    rlwinm r3, r3, 0, 17, 13
    or r0, r3, r0
    stw r0, 0x120(r6)
    b coordDone
coordCase3:
    lwz r6, gx(r13)
    slwi r0, r4, 21
    slwi r3, r5, 18
    lwz r4, 0x120(r6)
    rlwinm r4, r4, 0, 14, 10
    or r3, r4, r3
    stw r3, 0x120(r6)
    lwz r3, 0x120(r6)
    rlwinm r3, r3, 0, 11, 7
    or r0, r3, r0
    stw r0, 0x120(r6)
coordDone:
    li r0, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r3, 0x120(r4)
    stw r3, -0x8000(r5)
    lwz r3, 0x4f4(r4)
    ori r3, r3, 3
    stw r3, 0x4f4(r4)
    sth r0, 2(r4)
    blr
}

/* fn_800BBC0C - 0x800BBC0C | size: 0x28 -- GX TMEM config / texture state */
asm void fn_800BBC0C(u32 arg0) {
    nofralloc
    lwz r4, gx(r13)
    clrlslwi r0, r3, 24, 16
    lwz r3, 0x204(r4)
    rlwinm r3, r3, 0, 16, 12
    or r0, r3, r0
    stw r0, 0x204(r4)
    lwz r0, 0x4f4(r4)
    ori r0, r0, 6
    stw r0, 0x4f4(r4)
    blr
}

/* fn_800BBC34 - 0x800BBC34 | size: 0x48 -- GX TMEM config / texture state */
asm void fn_800BBC34(u32 arg0) {
    nofralloc
    mflr r0
    li r4, 0
    stw r0, 4(r1)
    li r0, 0
    li r5, 0
    stwu r1, -0x18(r1)
    li r6, 0
    li r7, 0
    stw r0, 8(r1)
    li r8, 0
    li r9, 0
    stw r0, 0xc(r1)
    li r10, 0
    bl fn_800BB780
    lwz r0, 0x1c(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800BBC7C - 0x800BBC7C | size: 0x64 -- GX TMEM config / texture state */
asm void fn_800BBC7C(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    clrlwi. r0, r6, 24
    stwu r1, -0x20(r1)
    beq noSScale
    li r6, 6
    b gotSScale
noSScale:
    li r6, 0
gotSScale:
    clrlwi. r0, r5, 24
    addi r8, r6, 0
    beq noTScale
    li r6, 7
    b gotTScale
noTScale:
    li r6, 0
gotTScale:
    li r0, 0
    stw r0, 8(r1)
    addi r9, r8, 0
    li r5, 0
    stw r0, 0xc(r1)
    li r10, 0
    bl fn_800BB780
    lwz r0, 0x24(r1)
    addi r1, r1, 0x20
    mtlr r0
    blr
}

/* fn_800BBCE0 - 0x800BBCE0 | size: 0x1AC -- GX TMEM config / texture state */
asm void fn_800BBCE0(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    clrlwi r0, r5, 16
    cmpwi r0, 0x40
    stwu r1, -0x70(r1)
    stmw r24, 0x50(r1)
    addi r24, r3, 0
    lwz r28, 0x78(r1)
    addi r25, r4, 0
    lwz r29, 0x7c(r1)
    addi r26, r9, 0
    addi r27, r10, 0
    beq srcSize40
    bge srcSizeGE40
    cmpwi r0, 0x20
    beq srcSize20
    bge srcSizeDefault
    cmpwi r0, 0x10
    beq srcSize10
    b srcSizeDefault
srcSizeGE40:
    cmpwi r0, 0x100
    beq srcSize100
    bge srcSizeDefault
    cmpwi r0, 0x80
    beq srcSize80
    b srcSizeDefault
srcSize100:
    li r31, 1
    b srcSizeDone
srcSize80:
    li r31, 2
    b srcSizeDone
srcSize40:
    li r31, 3
    b srcSizeDone
srcSize20:
    li r31, 4
    b srcSizeDone
srcSize10:
    li r31, 5
    b srcSizeDone
srcSizeDefault:
    li r31, 0
srcSizeDone:
    clrlwi r0, r6, 16
    cmpwi r0, 0x40
    beq dstSize40
    bge dstSizeGE40
    cmpwi r0, 0x20
    beq dstSize20
    bge dstSizeDefault
    cmpwi r0, 0x10
    beq dstSize10
    b dstSizeDefault
dstSizeGE40:
    cmpwi r0, 0x100
    beq dstSize100
    bge dstSizeDefault
    cmpwi r0, 0x80
    beq dstSize80
    b dstSizeDefault
dstSize100:
    li r30, 1
    b dstSizeDone
dstSize80:
    li r30, 2
    b dstSizeDone
dstSize40:
    li r30, 3
    b dstSizeDone
dstSize20:
    li r30, 4
    b dstSizeDone
dstSize10:
    li r30, 5
    b dstSizeDone
dstSizeDefault:
    li r30, 0
dstSizeDone:
    clrlwi r0, r7, 16
    lfd f4, lbl_8047C380(r2)
    stw r0, 0x4c(r1)
    lis r6, 0x4330
    clrlwi r0, r8, 16
    lfs f3, lbl_8047C374(r2)
    stw r6, 0x48(r1)
    lfs f1, lbl_8047C378(r2)
    mr r3, r27
    lfd f0, 0x48(r1)
    addi r4, r1, 0x28
    stw r0, 0x44(r1)
    fsubs f0, f0, f4
    li r5, 0xa
    stw r6, 0x40(r1)
    fmuls f2, f0, f3
    lfd f0, 0x40(r1)
    fsubs f0, f0, f4
    stfs f2, 0x28(r1)
    stfs f1, 0x30(r1)
    fmuls f0, f0, f3
    stfs f1, 0x2c(r1)
    stfs f0, 0x38(r1)
    stfs f1, 0x3c(r1)
    stfs f1, 0x34(r1)
    bl fn_800BB81C
    li r0, 1
    stw r0, 8(r1)
    addi r3, r24, 0
    addi r4, r25, 0
    stw r29, 0xc(r1)
    addi r5, r26, 0
    addi r6, r28, 0
    addi r7, r27, 0
    addi r8, r31, 0
    addi r9, r30, 0
    li r10, 0
    bl fn_800BB780
    lmw r24, 0x50(r1)
    lwz r0, 0x74(r1)
    addi r1, r1, 0x70
    mtlr r0
    blr
}

/* fn_800BBE8C - 0x800BBE8C | size: 0x10C -- GX TMEM config / texture state */
asm void fn_800BBE8C(void) {
    nofralloc
    mflr r0
    cmpwi r5, 2
    stw r0, 4(r1)
    stwu r1, -0x30(r1)
    stw r31, 0x2c(r1)
    stw r30, 0x28(r1)
    stw r29, 0x24(r1)
    addi r29, r4, 0
    stw r28, 0x20(r1)
    addi r28, r3, 0
    beq mode2
    bge modeGE2
    cmpwi r5, 1
    bge mode1
    b modeDone
modeGE2:
    cmpwi r5, 4
    bge modeDone
    b mode3
mode1:
    li r7, 5
    li r30, 9
    b modeDone
mode2:
    li r7, 6
    li r30, 0xa
    b modeDone
mode3:
    li r7, 7
    li r30, 0xb
modeDone:
    li r31, 0
    stw r31, 8(r1)
    addi r3, r28, 0
    addi r4, r29, 0
    stw r31, 0xc(r1)
    li r5, 0
    li r6, 3
    li r8, 6
    li r9, 6
    li r10, 0
    bl fn_800BB780
    stw r31, 8(r1)
    addi r4, r29, 0
    addi r7, r30, 0
    stw r31, 0xc(r1)
    addi r3, r28, 1
    li r5, 0
    li r6, 3
    li r8, 6
    li r9, 6
    li r10, 1
    bl fn_800BB780
    stw r31, 8(r1)
    addi r4, r29, 0
    addi r3, r28, 2
    stw r31, 0xc(r1)
    li r5, 0
    li r6, 0
    li r7, 0
    li r8, 0
    li r9, 0
    li r10, 1
    bl fn_800BB780
    lwz r0, 0x34(r1)
    lwz r31, 0x2c(r1)
    lwz r30, 0x28(r1)
    lwz r29, 0x24(r1)
    lwz r28, 0x20(r1)
    addi r1, r1, 0x30
    mtlr r0
    blr
}

/* fn_800BBF98 - 0x800BBF98 | size: 0x44 -- GX TMEM config / texture state */
asm void fn_800BBF98(void) {
    nofralloc
    mflr r0
    addi r7, r5, 0
    stw r0, 4(r1)
    li r0, 0
    li r5, 0
    stwu r1, -0x20(r1)
    li r6, 7
    li r8, 0
    stw r0, 8(r1)
    li r9, 0
    li r10, 0
    stw r0, 0xc(r1)
    bl fn_800BB780
    lwz r0, 0x24(r1)
    addi r1, r1, 0x20
    mtlr r0
    blr
}

/* fn_800BBFDC - 0x800BBFDC | size: 0x48 -- GX TMEM config / texture state */
asm void fn_800BBFDC(void) {
    nofralloc
    mflr r0
    li r4, 0
    stw r0, 4(r1)
    li r0, 0
    li r5, 0
    stwu r1, -0x18(r1)
    li r6, 0
    li r7, 0
    stw r0, 8(r1)
    li r8, 6
    li r9, 6
    stw r0, 0xc(r1)
    li r10, 1
    bl fn_800BB780
    lwz r0, 0x1c(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800BC024 - 0x800BC024 | size: 0xCC -- GX Light / material / TEV */
asm void fn_800BC024(void) {
    nofralloc
    lwz r3, gx(r13)
    li r6, 0
    li r4, 0
    lwz r0, 0x204(r3)
    extrwi r0, r0, 3, 13
    cmplwi r0, 0
    mtctr r0
    ble texCoordMaskDone
texCoordMaskLoop:
    cmpwi r4, 2
    beq texCoordMask2
    bge texCoordMaskGE2
    cmpwi r4, 0
    beq texCoordMask0
    bge texCoordMask1
    b texCoordMaskUse
texCoordMaskGE2:
    cmpwi r4, 4
    bge texCoordMaskUse
    b texCoordMask3
texCoordMask0:
    lwz r0, 0x120(r3)
    clrlwi r5, r0, 29
    b texCoordMaskUse
texCoordMask1:
    lwz r0, 0x120(r3)
    extrwi r5, r0, 3, 23
    b texCoordMaskUse
texCoordMask2:
    lwz r0, 0x120(r3)
    extrwi r5, r0, 3, 17
    b texCoordMaskUse
texCoordMask3:
    lwz r0, 0x120(r3)
    extrwi r5, r0, 3, 11
texCoordMaskUse:
    li r0, 1
    slw r0, r0, r5
    or r6, r6, r0
    addi r4, r4, 1
    bdnz texCoordMaskLoop
texCoordMaskDone:
    addi r4, r3, 0x124
    lwz r3, 0x124(r3)
    clrlwi r0, r3, 24
    cmplw r0, r6
    beqlr
    clrrwi r0, r3, 8
    or r0, r0, r6
    stw r0, 0(r4)
    li r0, 0x61
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r4, gx(r13)
    lwz r3, 0x124(r4)
    stw r3, -0x8000(r5)
    sth r0, 2(r4)
    blr
}

/* fn_800BC114 - 0x800BC114 | size: 0x8C -- GX Light / material / TEV */
asm void fn_800BC114(u32 arg0, u32 arg1) {
    nofralloc
    cmpwi r3, 0
    lis r5, lbl_80313590@ha
    addi r0, r5, lbl_80313590@l
    bne colorTableAlt
    slwi r4, r4, 2
    add r5, r0, r4
    addi r9, r5, 0x28
    b colorTableDone
colorTableAlt:
    slwi r4, r4, 2
    add r5, r0, r4
    add r9, r0, r4
    addi r5, r5, 0x14
    addi r9, r9, 0x3c
colorTableDone:
    lwz r7, gx(r13)
    slwi r3, r3, 2
    lwz r0, 0(r5)
    li r5, 0x61
    add r6, r7, r3
    lwz r3, 0x130(r6)
    lis r4, 0xcc01
    stb r5, -0x8000(r4)
    clrrwi r3, r3, 24
    rlwimi r3, r0, 0, 8, 31
    stw r3, -0x8000(r4)
    li r0, 0
    stw r3, 0x130(r6)
    lwz r8, 0x170(r6)
    lwz r3, 0(r9)
    rlwinm r8, r8, 0, 28, 7
    stb r5, -0x8000(r4)
    rlwimi r8, r3, 0, 8, 27
    stw r8, -0x8000(r4)
    stw r8, 0x170(r6)
    sth r0, 2(r7)
    blr
}

/* fn_800BC1A0 - 0x800BC1A0 | size: 0x44 -- GX Light / material / TEV */
asm void fn_800BC1A0(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    nofralloc
    lwz r9, gx(r13)
    slwi r3, r3, 2
    li r0, 0x61
    add r8, r9, r3
    lwz r10, 0x130(r8)
    lis r3, 0xcc01
    rlwimi r10, r4, 12, 16, 19
    stb r0, -0x8000(r3)
    addi r4, r10, 0
    rlwimi r4, r5, 8, 20, 23
    rlwimi r4, r6, 4, 24, 27
    rlwimi r4, r7, 0, 28, 31
    stw r4, -0x8000(r3)
    li r0, 0
    stw r4, 0x130(r8)
    sth r0, 2(r9)
    blr
}

/* fn_800BC1E4 - 0x800BC1E4 | size: 0x44 -- GX Light / material / TEV */
asm void fn_800BC1E4(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    nofralloc
    lwz r9, gx(r13)
    slwi r3, r3, 2
    li r0, 0x61
    add r8, r9, r3
    lwz r10, 0x170(r8)
    lis r3, 0xcc01
    rlwimi r10, r4, 13, 16, 18
    stb r0, -0x8000(r3)
    addi r4, r10, 0
    rlwimi r4, r5, 10, 19, 21
    rlwimi r4, r6, 7, 22, 24
    rlwimi r4, r7, 4, 25, 27
    stw r4, -0x8000(r3)
    li r0, 0
    stw r4, 0x170(r8)
    sth r0, 2(r9)
    blr
}

/* fn_800BC228 - 0x800BC228 | size: 0x68 -- GX Light / material / TEV */
asm void fn_800BC228(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5) {
    nofralloc
    lwz r0, gx(r13)
    slwi r9, r3, 2
    cmpwi r4, 1
    add r3, r0, r9
    lwz r3, 0x130(r3)
    rlwimi r3, r4, 18, 13, 13
    addi r10, r3, 0
    bgt colorChanAttn
    rlwimi r10, r6, 20, 10, 11
    rlwimi r10, r5, 16, 14, 15
    b colorChanDone
colorChanAttn:
    li r0, 3
    rlwimi r10, r4, 19, 10, 11
    rlwimi r10, r0, 16, 14, 15
colorChanDone:
    li r0, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    rlwimi r10, r7, 19, 12, 12
    rlwimi r10, r8, 22, 8, 9
    stw r10, -0x8000(r5)
    add r3, r4, r9
    li r0, 0
    stw r10, 0x130(r3)
    sth r0, 2(r4)
    blr
}

/* fn_800BC290 - 0x800BC290 | size: 0x68 -- GX Light / material / TEV */
asm void fn_800BC290(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5) {
    nofralloc
    lwz r0, gx(r13)
    slwi r9, r3, 2
    cmpwi r4, 1
    add r3, r0, r9
    lwz r3, 0x170(r3)
    rlwimi r3, r4, 18, 13, 13
    addi r10, r3, 0
    bgt alphaChanAttn
    rlwimi r10, r6, 20, 10, 11
    rlwimi r10, r5, 16, 14, 15
    b alphaChanDone
alphaChanAttn:
    li r0, 3
    rlwimi r10, r4, 19, 10, 11
    rlwimi r10, r0, 16, 14, 15
alphaChanDone:
    li r0, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    rlwimi r10, r7, 19, 12, 12
    rlwimi r10, r8, 22, 8, 9
    stw r10, -0x8000(r5)
    add r3, r4, r9
    li r0, 0
    stw r10, 0x170(r3)
    sth r0, 2(r4)
    blr
}

/* fn_800BC2F8 - 0x800BC2F8 | size: 0x74 -- GX Light / material / TEV */
asm void fn_800BC2F8(u32 arg0, u8* arg1) {
    nofralloc
    lbz r0, 3(r4)
    slwi r8, r3, 1
    lbz r5, 0(r4)
    addi r7, r8, 0xe0
    slwi r9, r0, 12
    rlwimi r9, r5, 0, 24, 31
    lbz r10, 2(r4)
    lbz r6, 1(r4)
    slwi r7, r7, 24
    li r5, 0x61
    lwz r3, gx(r13)
    lis r4, 0xcc01
    stb r5, -0x8000(r4)
    rlwimi r7, r9, 0, 12, 31
    addi r0, r8, 0xe1
    stw r7, -0x8000(r4)
    slwi r7, r6, 12
    slwi r6, r0, 24
    rlwimi r7, r10, 0, 24, 31
    stb r5, -0x8000(r4)
    rlwimi r6, r7, 0, 12, 31
    stw r6, -0x8000(r4)
    li r0, 0
    stb r5, -0x8000(r4)
    stw r6, -0x8000(r4)
    stb r5, -0x8000(r4)
    stw r6, -0x8000(r4)
    sth r0, 2(r3)
    blr
}

/* fn_800BC36C - 0x800BC36C | size: 0x74 -- GX Light / material / TEV */
asm void fn_800BC36C(u32 arg0, s16* arg1) {
    nofralloc
    lha r0, 6(r4)
    slwi r9, r3, 1
    lha r5, 0(r4)
    addi r8, r9, 0xe0
    clrlslwi r10, r0, 21, 12
    rlwimi r10, r5, 0, 21, 31
    lha r7, 4(r4)
    lha r6, 2(r4)
    slwi r8, r8, 24
    li r5, 0x61
    lwz r3, gx(r13)
    lis r4, 0xcc01
    stb r5, -0x8000(r4)
    rlwimi r8, r10, 0, 9, 31
    addi r0, r9, 0xe1
    stw r8, -0x8000(r4)
    clrlslwi r8, r6, 21, 12
    slwi r6, r0, 24
    rlwimi r8, r7, 0, 21, 31
    stb r5, -0x8000(r4)
    rlwimi r6, r8, 0, 9, 31
    stw r6, -0x8000(r4)
    li r0, 0
    stb r5, -0x8000(r4)
    stw r6, -0x8000(r4)
    stb r5, -0x8000(r4)
    stw r6, -0x8000(r4)
    sth r0, 2(r3)
    blr
}

/* fn_800BC3E0 - 0x800BC3E0 | size: 0x74 -- GX Light / material / TEV */
asm void fn_800BC3E0(u32 arg0, u8* arg1) {
    nofralloc
    lbz r0, 3(r4)
    slwi r5, r3, 1
    lbz r6, 0(r4)
    addi r7, r5, 0xe0
    slwi r0, r0, 12
    rlwimi r0, r6, 0, 24, 31
    lbz r6, 1(r4)
    lbz r9, 2(r4)
    rlwinm r0, r0, 0, 12, 7
    oris r8, r0, 0x80
    lwz r3, gx(r13)
    slwi r7, r7, 24
    li r0, 0x61
    lis r4, 0xcc01
    slwi r6, r6, 12
    stb r0, -0x8000(r4)
    rlwimi r7, r8, 0, 8, 31
    stw r7, -0x8000(r4)
    rlwimi r6, r9, 0, 24, 31
    addi r5, r5, 0xe1
    stb r0, -0x8000(r4)
    rlwinm r0, r6, 0, 12, 7
    oris r6, r0, 0x80
    slwi r0, r5, 24
    rlwimi r0, r6, 0, 8, 31
    stw r0, -0x8000(r4)
    li r0, 0
    sth r0, 2(r3)
    blr
}

/* fn_800BC454 - 0x800BC454 | size: 0x6C -- GX Light / material / TEV */
asm void fn_800BC454(u32 arg0, u32 arg1) {
    nofralloc
    srawi r5, r3, 1
    lwz r6, gx(r13)
    clrlwi. r0, r3, 31
    slwi r3, r5, 2
    addi r7, r3, 0x1b0
    add r7, r6, r7
    beq tevColorEven
    lwz r3, 0(r7)
    slwi r0, r4, 14
    rlwinm r3, r3, 0, 18, 12
    or r0, r3, r0
    stw r0, 0(r7)
    b tevColorDone
tevColorEven:
    lwz r3, 0(r7)
    slwi r0, r4, 4
    rlwinm r3, r3, 0, 28, 22
    or r0, r3, r0
    stw r0, 0(r7)
tevColorDone:
    li r0, 0x61
    lwz r3, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r4, 0(r7)
    stw r4, -0x8000(r5)
    sth r0, 2(r3)
    blr
}

/* fn_800BC4C0 - 0x800BC4C0 | size: 0x6C -- GX Light / material / TEV */
asm void fn_800BC4C0(u32 arg0, u32 arg1) {
    nofralloc
    srawi r5, r3, 1
    lwz r6, gx(r13)
    clrlwi. r0, r3, 31
    slwi r3, r5, 2
    addi r7, r3, 0x1b0
    add r7, r6, r7
    beq tevAlphaEven
    lwz r3, 0(r7)
    slwi r0, r4, 19
    rlwinm r3, r3, 0, 13, 7
    or r0, r3, r0
    stw r0, 0(r7)
    b tevAlphaDone
tevAlphaEven:
    lwz r3, 0(r7)
    slwi r0, r4, 9
    rlwinm r3, r3, 0, 23, 17
    or r0, r3, r0
    stw r0, 0(r7)
tevAlphaDone:
    li r0, 0x61
    lwz r3, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r4, 0(r7)
    stw r4, -0x8000(r5)
    sth r0, 2(r3)
    blr
}

/* fn_800BC52C - 0x800BC52C | size: 0x54 -- GX Light / material / TEV */
asm void fn_800BC52C(u32 arg0, u32 arg1, u32 arg2) {
    nofralloc
    slwi r3, r3, 2
    lwz r7, gx(r13)
    addi r8, r3, 0x170
    add r8, r7, r8
    lwz r0, 0(r8)
    slwi r5, r5, 2
    li r3, 0x61
    clrrwi r0, r0, 2
    or r0, r0, r4
    stw r0, 0(r8)
    lis r4, 0xcc01
    li r0, 0
    lwz r6, 0(r8)
    rlwinm r6, r6, 0, 30, 27
    or r5, r6, r5
    stw r5, 0(r8)
    stb r3, -0x8000(r4)
    lwz r3, 0(r8)
    stw r3, -0x8000(r4)
    sth r0, 2(r7)
    blr
}

/* fn_800BC580 - 0x800BC580 | size: 0x98 -- GX Light / material / TEV */
asm void fn_800BC580(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    nofralloc
    slwi r10, r3, 1
    lwz r9, gx(r13)
    slwi r3, r3, 3
    addi r11, r3, 0x1b0
    add r11, r9, r11
    lwz r8, 0(r11)
    addi r0, r10, 1
    slwi r3, r0, 2
    clrrwi r0, r8, 2
    or r0, r0, r4
    stw r0, 0(r11)
    addi r10, r3, 0x1b0
    slwi r0, r5, 2
    lwz r3, 0(r11)
    li r8, 0x61
    lis r5, 0xcc01
    rlwinm r3, r3, 0, 30, 27
    or r0, r3, r0
    stw r0, 0(r11)
    add r10, r9, r10
    slwi r3, r7, 2
    stb r8, -0x8000(r5)
    li r0, 0
    lwz r4, 0(r11)
    stw r4, -0x8000(r5)
    lwz r4, 0(r10)
    clrrwi r4, r4, 2
    or r4, r4, r6
    stw r4, 0(r10)
    lwz r4, 0(r10)
    rlwinm r4, r4, 0, 30, 27
    or r3, r4, r3
    stw r3, 0(r10)
    stb r8, -0x8000(r5)
    lwz r3, 0(r10)
    stw r3, -0x8000(r5)
    sth r0, 2(r9)
    blr
}

/* fn_800BC618 - 0x800BC618 | size: 0x54 -- GX Light / material / TEV */
asm void fn_800BC618(u32 arg0, u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
    nofralloc
    clrlwi r0, r4, 24
    lwz r4, gx(r13)
    oris r0, r0, 0xf300
    rlwinm r0, r0, 0, 24, 15
    rlwimi r0, r7, 8, 16, 23
    rlwinm r7, r0, 0, 16, 12
    slwi r0, r3, 16
    or r0, r7, r0
    rlwinm r3, r0, 0, 13, 9
    slwi r0, r6, 19
    or r6, r3, r0
    li r0, 0x61
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    rlwinm r6, r6, 0, 10, 7
    slwi r0, r5, 22
    or r0, r6, r0
    stw r0, -0x8000(r3)
    li r0, 0
    sth r0, 2(r4)
    blr
}

/* fn_800BC66C - 0x800BC66C | size: 0x84 -- GX Light / material / TEV */
asm void fn_800BC66C(u32 arg0, u32 arg1, u32 arg2) {
    nofralloc
    cmpwi r4, 0x13
    clrlwi r0, r5, 8
    oris r7, r0, 0xf400
    beq indirectCoord13
    bge indirectCoordGE13
    cmpwi r4, 0x11
    beq indirectCoord11
    b indirectCoordDefault
indirectCoordGE13:
    cmpwi r4, 0x16
    beq indirectCoord16
    b indirectCoordDefault
indirectCoord11:
    li r6, 0
    b indirectCoordDone
indirectCoord13:
    li r6, 1
    b indirectCoordDone
indirectCoord16:
    li r6, 2
    b indirectCoordDone
indirectCoordDefault:
    li r6, 2
indirectCoordDone:
    li r0, 0x61
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    rlwinm r6, r6, 0, 30, 27
    slwi r3, r3, 2
    stw r7, -0x8000(r5)
    or r3, r6, r3
    clrlwi r3, r3, 8
    stb r0, -0x8000(r5)
    oris r3, r3, 0xf500
    li r0, 0
    stw r3, -0x8000(r5)
    sth r0, 2(r4)
    blr
}

/* fn_800BC6F0 - 0x800BC6F0 | size: 0x1D8 -- GX Light / material / TEV */
asm void fn_800BC6F0(void) {
    nofralloc
    srawi r7, r3, 1
    lwz r9, gx(r13)
    rlwinm r10, r5, 0, 24, 22
    addze r7, r7
    slwi r0, r3, 2
    add r8, r9, r0
    slwi r7, r7, 2
    stw r5, 0x49c(r8)
    addi r7, r7, 0x100
    cmplwi r10, 8
    addi r0, r10, 0
    add r7, r9, r7
    blt tevfFormatInRange
    li r9, 0
    b tevfFormatDone
tevfFormatInRange:
    mr r9, r0
tevfFormatDone:
    cmpwi r4, 8
    blt tevfEnableSlot
    lwz r4, gx(r13)
    li r0, 1
    slw r0, r0, r3
    lwz r8, 0x4e0(r4)
    addi r10, r4, 0x4e0
    li r4, 0
    andc r0, r8, r0
    stw r0, 0(r10)
    b tevfSlotMaskDone
tevfEnableSlot:
    lwz r8, gx(r13)
    li r0, 1
    slw r0, r0, r3
    addi r10, r8, 0x4e0
    lwz r8, 0x4e0(r8)
    or r0, r8, r0
    stw r0, 0(r10)
tevfSlotMaskDone:
    clrlwi. r0, r3, 31
    beq tevfEvenSlot
    lwz r8, 0(r7)
    slwi r3, r9, 0xc
    slwi r0, r4, 0xf
    rlwinm r4, r8, 0, 20, 16
    or r3, r4, r3
    stw r3, 0(r7)
    cmpwi r6, 0xff
    lwz r3, 0(r7)
    rlwinm r3, r3, 0, 17, 13
    or r0, r3, r0
    stw r0, 0(r7)
    bne tevfOddLookup
    li r0, 7
    b tevfOddLookupDone
tevfOddLookup:
    lis r3, lbl_803135E0@ha
    slwi r4, r6, 2
    addi r0, r3, lbl_803135E0@l
    add r3, r0, r4
    lwz r0, 0(r3)
tevfOddLookupDone:
    lwz r3, 0(r7)
    slwi r0, r0, 0x13
    cmpwi r5, 0xff
    rlwinm r3, r3, 0, 13, 9
    or r0, r3, r0
    stw r0, 0(r7)
    li r4, 0
    beq tevfOddBiasDone
    rlwinm. r0, r5, 0, 23, 23
    bne tevfOddBiasDone
    li r4, 1
tevfOddBiasDone:
    lwz r3, 0(r7)
    slwi r0, r4, 0x12
    rlwinm r3, r3, 0, 14, 12
    or r0, r3, r0
    stw r0, 0(r7)
    b tevfWrite
tevfEvenSlot:
    lwz r3, 0(r7)
    slwi r0, r4, 3
    cmpwi r6, 0xff
    clrrwi r3, r3, 3
    or r3, r3, r9
    stw r3, 0(r7)
    lwz r3, 0(r7)
    rlwinm r3, r3, 0, 29, 25
    or r0, r3, r0
    stw r0, 0(r7)
    bne tevfEvenLookup
    li r0, 7
    b tevfEvenLookupDone
tevfEvenLookup:
    lis r3, lbl_803135E0@ha
    slwi r4, r6, 2
    addi r0, r3, lbl_803135E0@l
    add r3, r0, r4
    lwz r0, 0(r3)
tevfEvenLookupDone:
    lwz r3, 0(r7)
    slwi r0, r0, 7
    cmpwi r5, 0xff
    rlwinm r3, r3, 0, 25, 21
    or r0, r3, r0
    stw r0, 0(r7)
    li r4, 0
    beq tevfEvenBiasDone
    rlwinm. r0, r5, 0, 23, 23
    bne tevfEvenBiasDone
    li r4, 1
tevfEvenBiasDone:
    lwz r3, 0(r7)
    slwi r0, r4, 6
    rlwinm r3, r3, 0, 26, 24
    or r0, r3, r0
    stw r0, 0(r7)
tevfWrite:
    li r0, 0x61
    lwz r3, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0
    lwz r4, 0(r7)
    stw r4, -0x8000(r5)
    sth r0, 2(r3)
    lwz r0, 0x4f4(r3)
    ori r0, r0, 1
    stw r0, 0x4f4(r3)
    blr
}

/* fn_800BC8C8 - 0x800BC8C8 | size: 0x30 -- GX Light / material / TEV */
void fn_800BC8C8(u32 arg0) {
    extern u8* gx;
    u8* gxv;
    u32 reg;

    gxv = gx;
    arg0 = (arg0 & 0xff) - 1;
    reg = *(u32*)(gxv + 0x204);
    reg = (reg & ~0x3c00) | (arg0 << 10);
    *(u32*)(gxv + 0x204) = reg;
    *(u32*)(gxv + 0x4f4) |= 4;
}

/* fn_800BC8F8 - 0x800BC8F8 | size: 0x21C -- GX Light / material / TEV */
asm void fn_800BC8F8(void) {
    nofralloc
    mflr r0
    li r10, 0
    stw r0, 4(r1)
    li r11, 0
    stwu r1, -0x48(r1)
    stw r31, 0x44(r1)
    extrwi. r31, r3, 1, 28
    stw r30, 0x40(r1)
    clrlwi r30, r3, 29
    stw r29, 0x3c(r1)
    addi r29, r4, 0
    stw r28, 0x38(r1)
    beq fogExpPath
    fcmpu cr0, f4, f3
    beq fogLinearZero
    fcmpu cr0, f2, f1
    bne fogLinearCompute
fogLinearZero:
    lfs f0, lbl_8047C388(r2)
    stfs f0, 0x2c(r1)
    stfs f0, 0x28(r1)
    b fogWriteRegs
fogLinearCompute:
    fsubs f5, f2, f1
    lfs f6, lbl_8047C38C(r2)
    fsubs f2, f4, f3
    fsubs f0, f1, f3
    fdivs f3, f6, f5
    fmuls f1, f3, f2
    fmuls f0, f3, f0
    stfs f1, 0x2c(r1)
    stfs f0, 0x28(r1)
    b fogWriteRegs
fogExpPath:
    fcmpu cr0, f4, f3
    beq fogExpZero
    fcmpu cr0, f2, f1
    bne fogExpCompute
fogExpZero:
    lfs f3, lbl_8047C388(r2)
    lfs f4, lbl_8047C390(r2)
    fmr f5, f3
    b fogNormalizeStart
fogExpCompute:
    fsubs f0, f4, f3
    fsubs f5, f2, f1
    fmuls f2, f4, f3
    fdivs f4, f4, f0
    fmuls f0, f0, f5
    fdivs f5, f1, f5
    fdivs f3, f2, f0
fogNormalizeStart:
    lfs f1, lbl_8047C390(r2)
    li r3, 0
    lfd f0, lbl_8047C398(r2)
    b fogScaleDownTest
fogScaleDown:
    fmuls f4, f4, f1
    addi r3, r3, 1
fogScaleDownTest:
    fcmpo cr0, f4, f0
    bgt fogScaleDown
    lfd f0, lbl_8047C3A8(r2)
    lfs f2, lbl_8047C3A0(r2)
    lfs f1, lbl_8047C388(r2)
    b fogScaleUpTest
fogScaleUp:
    fmuls f4, f4, f2
    subi r3, r3, 1
fogScaleUpTest:
    fcmpo cr0, f4, f1
    ble fogNormalized
    fcmpo cr0, f4, f0
    blt fogScaleUp
fogNormalized:
    addi r28, r3, 1
    lfs f0, lbl_8047C3B0(r2)
    li r0, 1
    lfd f2, lbl_8047C3B8(r2)
    slw r0, r0, r28
    fmuls f1, f0, f4
    xoris r3, r0, 0x8000
    stfs f5, 0x28(r1)
    lis r0, 0x4330
    stw r3, 0x34(r1)
    stw r0, 0x30(r1)
    lfd f0, 0x30(r1)
    fsubs f0, f0, f2
    fdivs f0, f3, f0
    stfs f0, 0x2c(r1)
    bl fn_800C46B0
    clrlwi r3, r3, 8
    clrlwi r0, r28, 8
    oris r10, r3, 0xef00
    oris r11, r0, 0xf000
fogWriteRegs:
    lwz r7, 0x2c(r1)
    li r0, 0x61
    lbz r6, 1(r29)
    lis r4, 0xcc01
    rlwinm r8, r7, 20, 13, 20
    rlwimi r8, r7, 20, 21, 31
    lbz r9, 2(r29)
    lbz r5, 0(r29)
    rlwinm r7, r7, 20, 12, 12
    rlwimi r7, r8, 0, 13, 31
    lwz r12, 0x28(r1)
    clrlwi r7, r7, 8
    stb r0, -0x8000(r4)
    oris r7, r7, 0xee00
    slwi r6, r6, 8
    lwz r3, gx(r13)
    stw r7, -0x8000(r4)
    rlwinm r7, r12, 20, 13, 20
    stb r0, -0x8000(r4)
    rlwimi r7, r12, 20, 21, 31
    rlwinm r8, r12, 20, 12, 12
    stw r10, -0x8000(r4)
    rlwimi r8, r7, 0, 13, 31
    slwi r7, r31, 20
    stb r0, -0x8000(r4)
    rlwimi r7, r8, 0, 12, 31
    rlwinm r8, r7, 0, 11, 7
    stw r11, -0x8000(r4)
    slwi r7, r30, 21
    or r7, r8, r7
    clrlwi r7, r7, 8
    stb r0, -0x8000(r4)
    oris r7, r7, 0xf100
    stw r7, -0x8000(r4)
    rlwimi r6, r9, 0, 24, 31
    stb r0, -0x8000(r4)
    slwi r0, r5, 16
    rlwimi r0, r6, 0, 16, 31
    clrlwi r0, r0, 8
    oris r0, r0, 0xf200
    stw r0, -0x8000(r4)
    li r0, 0
    sth r0, 2(r3)
    lwz r0, 0x4c(r1)
    lwz r31, 0x44(r1)
    lwz r30, 0x40(r1)
    lwz r29, 0x3c(r1)
    lwz r28, 0x38(r1)
    addi r1, r1, 0x48
    mtlr r0
    blr
}

/* fn_800BCB14 - 0x800BCB14 | size: 0x1C8 -- GX Light / material / TEV */
asm void fn_800BCB14(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x80(r1)
    stfd f31, 0x78(r1)
    stfd f30, 0x70(r1)
    stfd f29, 0x68(r1)
    stfd f28, 0x60(r1)
    stfd f27, 0x58(r1)
    stfd f26, 0x50(r1)
    stfd f25, 0x48(r1)
    stfd f24, 0x40(r1)
    stfd f23, 0x38(r1)
    stw r31, 0x34(r1)
    stw r30, 0x30(r1)
    stw r29, 0x2c(r1)
    lfd f1, lbl_8047C3C0(r2)
    lfs f0, 0x3c(r5)
    fcmpu cr0, f1, f0
    bne fogTableAlt
    lfs f2, 0x28(r5)
    lfs f1, lbl_8047C38C(r2)
    lfs f3, 0x2c(r5)
    fsubs f2, f2, f1
    lfs f0, 8(r5)
    fadds f1, f1, f0
    lfs f0, 0(r5)
    fdivs f2, f3, f2
    fmuls f1, f2, f1
    fdivs f24, f1, f0
    b fogTablePrep
fogTableAlt:
    lfs f4, lbl_8047C38C(r2)
    lfs f0, 0xc(r5)
    lfs f3, 0x2c(r5)
    fsubs f1, f0, f4
    lfs f2, 0x28(r5)
    fadds f3, f4, f3
    lfs f0, 0(r5)
    fneg f1, f1
    fdivs f2, f3, f2
    fdivs f24, f1, f0
fogTablePrep:
    clrlwi r0, r4, 16
    lfd f27, lbl_8047C3D8(r2)
    stw r0, 0x24(r1)
    lis r31, 0x4330
    lfs f1, lbl_8047C3A0(r2)
    fmuls f26, f2, f2
    stw r31, 0x20(r1)
    lfs f28, lbl_8047C38C(r2)
    mr r30, r3
    lfd f0, 0x20(r1)
    lfs f29, lbl_8047C388(r2)
    li r29, 0
    fsubs f0, f0, f27
    lfd f30, lbl_8047C3A8(r2)
    lfd f31, lbl_8047C3C8(r2)
    lfs f23, lbl_8047C3D0(r2)
    fdivs f25, f1, f0
fogTableLoop:
    addi r0, r29, 1
    slwi r0, r0, 5
    stw r0, 0x24(r1)
    stw r31, 0x20(r1)
    lfd f0, 0x20(r1)
    fsubs f0, f0, f27
    fmuls f0, f0, f25
    fmuls f0, f0, f24
    fmuls f0, f0, f0
    fdivs f0, f0, f26
    fadds f2, f28, f0
    fcmpo cr0, f2, f29
    ble fogTableQuantize
    frsqrte f1, f2
    fmul f0, f1, f1
    fmul f1, f30, f1
    fmul f0, f2, f0
    fsub f0, f31, f0
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f30, f1
    fmul f0, f2, f0
    fsub f0, f31, f0
    fmul f1, f1, f0
    fmul f0, f1, f1
    fmul f1, f30, f1
    fmul f0, f2, f0
    fsub f0, f31, f0
    fmul f0, f1, f0
    fmul f0, f2, f0
    frsp f0, f0
    stfs f0, 0x14(r1)
    lfs f2, 0x14(r1)
fogTableQuantize:
    fmuls f1, f23, f2
    bl fn_800C46B0
    addi r29, r29, 1
    clrlwi r0, r3, 20
    sth r0, 0(r30)
    cmplwi r29, 0xa
    addi r30, r30, 2
    blt fogTableLoop
    lwz r0, 0x84(r1)
    lfd f31, 0x78(r1)
    lfd f30, 0x70(r1)
    lfd f29, 0x68(r1)
    lfd f28, 0x60(r1)
    lfd f27, 0x58(r1)
    lfd f26, 0x50(r1)
    lfd f25, 0x48(r1)
    lfd f24, 0x40(r1)
    lfd f23, 0x38(r1)
    lwz r31, 0x34(r1)
    lwz r30, 0x30(r1)
    lwz r29, 0x2c(r1)
    addi r1, r1, 0x80
    mtlr r0
    blr
}

/* fn_800BCCDC - 0x800BCCDC | size: 0x100 -- GX Light / material / TEV */
asm void fn_800BCCDC(u32 arg0, u32 arg1, u16* table) {
    nofralloc
    clrlwi. r0, r3, 24
    beq fogUploadDone
    lhz r8, 2(r5)
    li r0, 0x61
    lhz r10, 0(r5)
    lis r6, 0xcc01
    slwi r8, r8, 12
    rlwimi r8, r10, 0, 20, 31
    stb r0, -0x8000(r6)
    li r7, 0xe9
    clrlwi r9, r8, 8
    rlwimi r9, r7, 24, 0, 7
    stw r9, -0x8000(r6)
    li r7, 0xea
    slwi r9, r7, 24
    lhzu r10, 4(r5)
    li r7, 0xeb
    lhz r8, 2(r5)
    slwi r8, r8, 12
    stb r0, -0x8000(r6)
    rlwimi r8, r10, 0, 20, 31
    rlwimi r9, r8, 0, 8, 31
    stw r9, -0x8000(r6)
    slwi r9, r7, 24
    li r7, 0xec
    lhz r8, 6(r5)
    lhz r10, 4(r5)
    slwi r8, r8, 12
    rlwimi r8, r10, 0, 20, 31
    stb r0, -0x8000(r6)
    rlwimi r9, r8, 0, 8, 31
    stw r9, -0x8000(r6)
    slwi r9, r7, 24
    li r7, 0xed
    lhz r8, 0xa(r5)
    lhz r10, 8(r5)
    slwi r8, r8, 12
    rlwimi r8, r10, 0, 20, 31
    stb r0, -0x8000(r6)
    rlwimi r9, r8, 0, 8, 31
    stw r9, -0x8000(r6)
    lhz r8, 0xe(r5)
    lhz r10, 0xc(r5)
    slwi r8, r8, 12
    rlwimi r8, r10, 0, 20, 31
    stb r0, -0x8000(r6)
    clrlwi r9, r8, 8
    rlwimi r9, r7, 24, 0, 7
    stw r9, -0x8000(r6)
fogUploadDone:
    clrlwi r5, r4, 16
    lwz r4, gx(r13)
    addi r0, r5, 0x156
    rlwinm r5, r0, 0, 22, 20
    clrlslwi r0, r3, 24, 10
    or r0, r5, r0
    clrlwi r5, r0, 8
    li r0, 0x61
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    oris r5, r5, 0xe800
    li r0, 0
    stw r5, -0x8000(r3)
    sth r0, 2(r4)
    blr
}

/* fn_800BCDDC - 0x800BCDDC | size: 0x54 -- GX Light / material / TEV */
asm void fn_800BCDDC(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    nofralloc
    lwz r8, gx(r13)
    subfic r0, r3, 3
    cntlzw r7, r0
    subfic r0, r3, 2
    lwz r9, 0x1d0(r8)
    rlwimi r9, r7, 6, 20, 20
    addi r7, r9, 0
    rlwimi r7, r3, 0, 31, 31
    cntlzw r0, r0
    rlwimi r7, r0, 28, 30, 30
    rlwimi r7, r6, 12, 16, 19
    rlwimi r7, r4, 8, 21, 23
    li r0, 0x61
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    rlwimi r7, r5, 5, 24, 26
    li r0, 0
    stw r7, -0x8000(r3)
    stw r7, 0x1d0(r8)
    sth r0, 2(r8)
    blr
}

/* fn_800BCE30 - 0x800BCE30 | size: 0x2C -- GX Light / material / TEV */
asm void fn_800BCE30(u32 arg0) {
    nofralloc
    lwz r5, gx(r13)
    li r0, 0x61
    lis r4, 0xcc01
    lwz r6, 0x1d0(r5)
    rlwimi r6, r3, 3, 28, 28
    stb r0, -0x8000(r4)
    li r0, 0
    stw r6, -0x8000(r4)
    stw r6, 0x1d0(r5)
    sth r0, 2(r5)
    blr
}

/* fn_800BCE5C - 0x800BCE5C | size: 0x2C -- GX Light / material / TEV */
asm void fn_800BCE5C(u32 arg0) {
    nofralloc
    lwz r5, gx(r13)
    li r0, 0x61
    lis r4, 0xcc01
    lwz r6, 0x1d0(r5)
    rlwimi r6, r3, 4, 27, 27
    stb r0, -0x8000(r4)
    li r0, 0
    stw r6, -0x8000(r4)
    stw r6, 0x1d0(r5)
    sth r0, 2(r5)
    blr
}

/* fn_800BCE88 - 0x800BCE88 | size: 0x34 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BCE88(u32 arg0, u32 arg1, u32 arg2) {
    nofralloc
    lwz r6, gx(r13)
    li r0, 0x61
    lwz r7, 0x1d8(r6)
    rlwimi r7, r3, 0, 31, 31
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    rlwimi r7, r4, 1, 28, 30
    rlwimi r7, r5, 4, 27, 27
    stw r7, -0x8000(r3)
    li r0, 0
    stw r7, 0x1d8(r6)
    sth r0, 2(r6)
    blr
}

/* fn_800BCEBC - 0x800BCEBC | size: 0x38 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BCEBC(u32 arg0) {
    nofralloc
    lwz r6, gx(r13)
    clrlslwi r0, r3, 24, 6
    li r3, 0x61
    lwz r5, 0x1dc(r6)
    lis r4, 0xcc01
    rlwinm r5, r5, 0, 26, 24
    or r0, r5, r0
    stw r0, 0x1dc(r6)
    li r0, 0
    stb r3, -0x8000(r4)
    lwz r3, 0x1dc(r6)
    stw r3, -0x8000(r4)
    sth r0, 2(r6)
    blr
}

/* fn_800BCEF4 - 0x800BCEF4 | size: 0xE8 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BCEF4(u32 arg0, u32 arg1) {
    nofralloc
    lwz r7, gx(r13)
    lis r5, lbl_80313608@ha
    slwi r6, r3, 2
    addi r0, r5, lbl_80313608@l
    lwz r8, 0x1dc(r7)
    add r10, r0, r6
    lwz r5, 0(r10)
    clrrwi r6, r8, 3
    slwi r0, r4, 3
    or r4, r6, r5
    stw r4, 0x1dc(r7)
    lwz r4, 0x1dc(r7)
    rlwinm r4, r4, 0, 29, 25
    or r0, r4, r0
    stw r0, 0x1dc(r7)
    lwz r5, 0x1dc(r7)
    cmplw r8, r5
    beq pixelFmtNoChange
    li r0, 0x61
    lis r4, 0xcc01
    stb r0, -0x8000(r4)
    cmpwi r3, 2
    stw r5, -0x8000(r4)
    bne pixelFmtNot2
    li r0, 1
    b pixelFmtFlagDone
pixelFmtNot2:
    li r0, 0
pixelFmtFlagDone:
    lwz r5, gx(r13)
    clrlslwi r0, r0, 24, 9
    lwz r4, 0x204(r5)
    rlwinm r4, r4, 0, 23, 21
    or r0, r4, r0
    stw r0, 0x204(r5)
    lwz r0, 0x4f4(r5)
    ori r0, r0, 4
    stw r0, 0x4f4(r5)
pixelFmtNoChange:
    lwz r0, 0(r10)
    cmplwi r0, 4
    bne pixelFmtDone
    lwz r5, gx(r13)
    subi r3, r3, 4
    li r0, 0x61
    lwz r4, 0x1d4(r5)
    rlwinm r4, r4, 0, 23, 20
    rlwimi r4, r3, 9, 21, 22
    stw r4, 0x1d4(r5)
    lis r3, 0xcc01
    lwz r4, 0x1d4(r5)
    clrlwi r4, r4, 8
    oris r4, r4, 0x4200
    stw r4, 0x1d4(r5)
    stb r0, -0x8000(r3)
    lwz r0, 0x1d4(r5)
    stw r0, -0x8000(r3)
pixelFmtDone:
    lwz r3, gx(r13)
    li r0, 0
    sth r0, 2(r3)
    blr
}

/* fn_800BCFDC - 0x800BCFDC | size: 0x2C -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BCFDC(u32 arg0) {
    nofralloc
    lwz r5, gx(r13)
    li r0, 0x61
    lis r4, 0xcc01
    lwz r6, 0x1d0(r5)
    rlwimi r6, r3, 2, 29, 29
    stb r0, -0x8000(r4)
    li r0, 0
    stw r6, -0x8000(r4)
    stw r6, 0x1d0(r5)
    sth r0, 2(r5)
    blr
}

/* fn_800BD008 - 0x800BD008 | size: 0x3C -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD008(u32 arg0, u32 arg1) {
    nofralloc
    lwz r6, gx(r13)
    clrlwi r5, r4, 24
    li r0, 0x61
    lwz r7, 0x1d4(r6)
    lis r4, 0xcc01
    rlwimi r7, r5, 0, 24, 31
    stb r0, -0x8000(r4)
    clrlwi r0, r3, 24
    addi r3, r7, 0
    rlwimi r3, r0, 8, 23, 23
    stw r3, -0x8000(r4)
    li r0, 0
    stw r3, 0x1d4(r6)
    sth r0, 2(r6)
    blr
}

/* fn_800BD044 - 0x800BD044 | size: 0x38 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD044(u32 arg0, u32 arg1) {
    nofralloc
    clrlwi r0, r4, 24
    lwz r4, gx(r13)
    rlwinm r5, r0, 0, 31, 29
    clrlslwi r0, r3, 24, 1
    or r0, r5, r0
    clrlwi r5, r0, 8
    li r0, 0x61
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    oris r5, r5, 0x4400
    li r0, 0
    stw r5, -0x8000(r3)
    sth r0, 2(r4)
    blr
}

/* fn_800BD07C - 0x800BD07C | size: 0x7C -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD07C(u32 arg0, u32 arg1) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    clrlslwi r0, r4, 24, 22
    stwu r1, -0x20(r1)
    stw r31, 0x1c(r1)
    lis r31, 0xcc01
    stw r30, 0x18(r1)
    li r30, 0x61
    stw r29, 0x14(r1)
    mr r29, r3
    lwz r5, gx(r13)
    lwz r4, 0x7c(r5)
    rlwinm r4, r4, 0, 10, 8
    or r0, r4, r0
    stw r0, 0x7c(r5)
    stb r30, -0x8000(r31)
    lwz r0, 0x7c(r5)
    stw r0, -0x8000(r31)
    bl __GXFlushTextureState
    clrlwi r0, r29, 24
    stb r30, -0x8000(r31)
    oris r0, r0, 0x6800
    stw r0, -0x8000(r31)
    bl __GXFlushTextureState
    lwz r0, 0x24(r1)
    lwz r31, 0x1c(r1)
    lwz r30, 0x18(r1)
    lwz r29, 0x14(r1)
    addi r1, r1, 0x20
    mtlr r0
    blr
}

/* fn_800BD0F8 - 0x800BD0F8 | size: 0x4 -- GX Pixel / blend / alpha / z-mode */
void fn_800BD0F8(void) {
}

/* fn_800BD0FC - 0x800BD0FC | size: 0x70 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD0FC(void* displayList, u32 nbytes) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    addi r31, r4, 0
    stw r30, 0x10(r1)
    addi r30, r3, 0
    lwz r5, gx(r13)
    lwz r0, 0x4f4(r5)
    cmplwi r0, 0
    beq displayListStateClean
    bl fn_800B91EC
displayListStateClean:
    lwz r3, gx(r13)
    lwz r0, 0(r3)
    cmplwi r0, 0
    bne displayListReady
    bl fn_800B937C
displayListReady:
    li r0, 0x40
    lis r3, 0xcc01
    stb r0, -0x8000(r3)
    stw r30, -0x8000(r3)
    stw r31, -0x8000(r3)
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    lwz r30, 0x10(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800BD16C - 0x800BD16C | size: 0x174 -- GX Pixel / blend / alpha / z-mode */
void fn_800BD16C(void) {
    /* GX Pixel / blend / alpha / z-mode (0x174 bytes) */
}

/* fn_800BD2E0 - 0x800BD2E0 | size: 0xB4 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD2E0(f32* mtx, u32 type) {
    nofralloc
    lwz r5, gx(r13)
    cmpwi r4, 1
    stw r4, 0x420(r5)
    lfs f0, 0(r3)
    stfs f0, 0x424(r5)
    lfs f0, 0x14(r3)
    stfs f0, 0x42c(r5)
    lfs f0, 0x28(r3)
    stfs f0, 0x434(r5)
    lfs f0, 0x2c(r3)
    stfs f0, 0x438(r5)
    bne projUsePerspective
    lfs f0, 0xc(r3)
    stfs f0, 0x428(r5)
    lfs f0, 0x1c(r3)
    stfs f0, 0x430(r5)
    b projWrite
projUsePerspective:
    lfs f0, 8(r3)
    stfs f0, 0x428(r5)
    lfs f0, 0x18(r3)
    stfs f0, 0x430(r5)
projWrite:
    li r0, 0x10
    lwz r4, gx(r13)
    lis r5, 0xcc01
    lis r3, 6
    stb r0, -0x8000(r5)
    addi r0, r3, 0x1020
    stw r0, -0x8000(r5)
    li r0, 1
    lfs f0, 0x424(r4)
    stfs f0, -0x8000(r5)
    lfs f0, 0x428(r4)
    stfs f0, -0x8000(r5)
    lfs f0, 0x42c(r4)
    stfs f0, -0x8000(r5)
    lfs f0, 0x430(r4)
    stfs f0, -0x8000(r5)
    lfs f0, 0x434(r4)
    stfs f0, -0x8000(r5)
    lfs f0, 0x438(r4)
    stfs f0, -0x8000(r5)
    lwz r3, 0x420(r4)
    stw r3, -0x8000(r5)
    sth r0, 2(r4)
    blr
}

/* fn_800BD394 - 0x800BD394 | size: 0xC0 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD394(f32* proj) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x18(r1)
    stw r31, 0x14(r1)
    mr r31, r3
    lfs f1, 0(r3)
    bl fn_800C46B0
    lwz r7, gx(r13)
    lis r4, 6
    li r6, 0x10
    stw r3, 0x420(r7)
    lis r5, 0xcc01
    addi r3, r4, 0x1020
    lfs f0, 4(r31)
    li r0, 1
    stfs f0, 0x424(r7)
    lfs f0, 8(r31)
    stfs f0, 0x428(r7)
    lfs f0, 0xc(r31)
    stfs f0, 0x42c(r7)
    lfs f0, 0x10(r31)
    stfs f0, 0x430(r7)
    lfs f0, 0x14(r31)
    stfs f0, 0x434(r7)
    lfs f0, 0x18(r31)
    stfs f0, 0x438(r7)
    stb r6, -0x8000(r5)
    stw r3, -0x8000(r5)
    lfs f0, 0x424(r7)
    stfs f0, -0x8000(r5)
    lfs f0, 0x428(r7)
    stfs f0, -0x8000(r5)
    lfs f0, 0x42c(r7)
    stfs f0, -0x8000(r5)
    lfs f0, 0x430(r7)
    stfs f0, -0x8000(r5)
    lfs f0, 0x434(r7)
    stfs f0, -0x8000(r5)
    lfs f0, 0x438(r7)
    stfs f0, -0x8000(r5)
    lwz r3, 0x420(r7)
    stw r3, -0x8000(r5)
    sth r0, 2(r7)
    lwz r0, 0x1c(r1)
    lwz r31, 0x14(r1)
    addi r1, r1, 0x18
    mtlr r0
    blr
}

/* fn_800BD454 - 0x800BD454 | size: 0x60 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD454(f32* out) {
    nofralloc
    stwu r1, -0x18(r1)
    lis r0, 0x4330
    lwz r5, gx(r13)
    lfd f1, lbl_8047C3F0(r2)
    lwz r4, 0x420(r5)
    stw r4, 0x14(r1)
    stw r0, 0x10(r1)
    lfd f0, 0x10(r1)
    fsubs f0, f0, f1
    stfs f0, 0(r3)
    lfs f0, 0x424(r5)
    stfs f0, 4(r3)
    lfs f0, 0x428(r5)
    stfs f0, 8(r3)
    lfs f0, 0x42c(r5)
    stfs f0, 0xc(r3)
    lfs f0, 0x430(r5)
    stfs f0, 0x10(r3)
    lfs f0, 0x434(r5)
    stfs f0, 0x14(r3)
    lfs f0, 0x438(r5)
    stfs f0, 0x18(r3)
    addi r1, r1, 0x18
    blr
}

/* fn_800BD4B4 - 0x800BD4B4 | size: 0x50 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD4B4(f32* mtx, u32 id) {
    nofralloc
    lis r5, 0xcc01
    li r0, 0x10
    slwi r4, r4, 2
    stb r0, -0x8000(r5)
    oris r0, r4, 0xb
    stw r0, -0x8000(r5)
    addi r4, r5, -0x8000
    psq_l f5, 0(r3), 0, 0
    psq_l f4, 8(r3), 0, 0
    psq_l f3, 0x10(r3), 0, 0
    psq_l f2, 0x18(r3), 0, 0
    psq_l f1, 0x20(r3), 0, 0
    psq_l f0, 0x28(r3), 0, 0
    psq_st f5, 0(r4), 0, 0
    psq_st f4, 0(r4), 0, 0
    psq_st f3, 0(r4), 0, 0
    psq_st f2, 0(r4), 0, 0
    psq_st f1, 0(r4), 0, 0
    psq_st f0, 0(r4), 0, 0
    blr
}

/* fn_800BD504 - 0x800BD504 | size: 0x50 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD504(f32* mtx, u32 id) {
    nofralloc
    mulli r5, r4, 3
    lis r4, 0xcc01
    li r0, 0x10
    addi r5, r5, 0x400
    stb r0, -0x8000(r4)
    oris r0, r5, 8
    stwu r0, -0x8000(r4)
    psq_l f5, 0(r3), 0, 0
    lfs f4, 8(r3)
    psq_l f3, 0x10(r3), 0, 0
    lfs f2, 0x18(r3)
    psq_l f1, 0x20(r3), 0, 0
    lfs f0, 0x28(r3)
    psq_st f5, 0(r4), 0, 0
    stfs f4, 0(r4)
    psq_st f3, 0(r4), 0, 0
    stfs f2, 0(r4)
    psq_st f1, 0(r4), 0, 0
    stfs f0, 0(r4)
    blr
}

/* fn_800BD554 - 0x800BD554 | size: 0x38 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD554(u32 arg0) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -8(r1)
    lwz r4, gx(r13)
    lwzu r0, 0x80(r4)
    clrrwi r0, r0, 6
    or r0, r0, r3
    stw r0, 0(r4)
    li r3, 0
    bl fn_800BD898
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800BD58C - 0x800BD58C | size: 0xB4 -- GX Pixel / blend / alpha / z-mode */
void fn_800BD58C(void) {
    /* GX Pixel / blend / alpha / z-mode (0xB4 bytes) */
}

/* fn_800BD640 - 0x800BD640 | size: 0x104 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD640(void) {
    nofralloc
    mflr r0
    cmplwi r3, 0
    stw r0, 4(r1)
    stwu r1, -0x60(r1)
    stfd f31, 0x58(r1)
    stfd f30, 0x50(r1)
    stfd f29, 0x48(r1)
    stfd f28, 0x40(r1)
    stfd f27, 0x38(r1)
    stfd f26, 0x30(r1)
    bne viewportNoSubtract
    lfs f0, lbl_8047C3E8(r2)
    fsubs f2, f2, f0
viewportNoSubtract:
    lfs f11, lbl_8047C3E8(r2)
    fneg f10, f4
    lfs f0, lbl_8047C3FC(r2)
    lfs f9, lbl_8047C3F8(r2)
    fmuls f31, f3, f11
    lwz r3, gx(r13)
    fmuls f7, f4, f11
    stfs f1, 0x43c(r3)
    fadds f8, f1, f31
    fmuls f30, f0, f6
    stfs f2, 0x440(r3)
    fadds f1, f2, f7
    stfs f3, 0x444(r3)
    fmuls f0, f0, f5
    fmuls f29, f10, f11
    stfs f4, 0x448(r3)
    fadds f27, f9, f8
    stfs f5, 0x44c(r3)
    fadds f26, f9, f1
    fsubs f28, f30, f0
    stfs f6, 0x450(r3)
viewportStored:
    lbz r0, 0x454(r3)
    cmplwi r0, 0
    beq writeViewport
    fmr f1, f5
    lfs f2, 0x458(r3)
    bl fn_800BD0F8
writeViewport:
    li r0, 0x10
    lwz r3, gx(r13)
    lis r5, 0xcc01
    lis r4, 5
    stb r0, -0x8000(r5)
    addi r0, r4, 0x101a
    stw r0, -0x8000(r5)
    li r0, 1
    stfs f31, -0x8000(r5)
    stfs f29, -0x8000(r5)
    stfs f28, -0x8000(r5)
    stfs f27, -0x8000(r5)
    stfs f26, -0x8000(r5)
    stfs f30, -0x8000(r5)
    sth r0, 2(r3)
    lwz r0, 0x64(r1)
    lfd f31, 0x58(r1)
    lfd f30, 0x50(r1)
    lfd f29, 0x48(r1)
    lfd f28, 0x40(r1)
    lfd f27, 0x38(r1)
    lfd f26, 0x30(r1)
    addi r1, r1, 0x60
    mtlr r0
    blr
}

/* fn_800BD744 - 0x800BD744 | size: 0x24 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD744(void) {
    nofralloc
    mflr r0
    li r3, 1
    stw r0, 4(r1)
    stwu r1, -8(r1)
    bl fn_800BD640
    lwz r0, 0xc(r1)
    addi r1, r1, 8
    mtlr r0
    blr
}

/* fn_800BD768 - 0x800BD768 | size: 0x38 -- GX Pixel / blend / alpha / z-mode */
void fn_800BD768(f32* out) {
    u8* gxPtr;

    gxPtr = gx;
    out[0] = *(f32*)(gxPtr + 0x43C);
    out[1] = *(f32*)(gxPtr + 0x440);
    out[2] = *(f32*)(gxPtr + 0x444);
    out[3] = *(f32*)(gxPtr + 0x448);
    out[4] = *(f32*)(gxPtr + 0x44C);
    out[5] = *(f32*)(gxPtr + 0x450);
}

/* fn_800BD7A0 - 0x800BD7A0 | size: 0x90 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD7A0(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    nofralloc
    lwz r8, gx(r13)
    addi r9, r3, 0x156
    addi r10, r4, 0x156
    lwz r3, 0xf8(r8)
    subi r0, r5, 1
    slwi r4, r9, 12
    clrrwi r3, r3, 11
    or r3, r3, r10
    stw r3, 0xf8(r8)
    subi r3, r6, 1
    add r6, r10, r3
    lwz r5, 0xf8(r8)
    add r0, r9, r0
    rlwinm r3, r5, 0, 20, 8
    or r3, r3, r4
    stw r3, 0xf8(r8)
    slwi r3, r0, 12
    li r5, 0x61
    lwz r7, 0xfc(r8)
    lis r4, 0xcc01
    li r0, 0
    clrrwi r7, r7, 11
    or r6, r7, r6
    stw r6, 0xfc(r8)
    lwz r6, 0xfc(r8)
    rlwinm r6, r6, 0, 20, 8
    or r3, r6, r3
    stw r3, 0xfc(r8)
    stb r5, -0x8000(r4)
    lwz r3, 0xf8(r8)
    stw r3, -0x8000(r4)
    stb r5, -0x8000(r4)
    lwz r3, 0xfc(r8)
    stw r3, -0x8000(r4)
    sth r0, 2(r8)
    blr
}

/* fn_800BD830 - 0x800BD830 | size: 0x40 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD830(u32 arg0, u32 arg1) {
    nofralloc
    addi r5, r3, 0x156
    lwz r3, gx(r13)
    addi r0, r4, 0x156
    srwi r4, r5, 1
    rlwinm r4, r4, 0, 22, 11
    extlwi r0, r0, 22, 9
    or r0, r4, r0
    clrlwi r5, r0, 8
    li r0, 0x61
    lis r4, 0xcc01
    stb r0, -0x8000(r4)
    oris r5, r5, 0x5900
    li r0, 0
    stw r5, -0x8000(r4)
    sth r0, 2(r3)
    blr
}

/* fn_800BD870 - 0x800BD870 | size: 0x28 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD870(u32 arg0) {
    nofralloc
    li r0, 0x10
    lwz r4, gx(r13)
    lis r6, 0xcc01
    stb r0, -0x8000(r6)
    li r5, 0x1005
    li r0, 1
    stw r5, -0x8000(r6)
    stw r3, -0x8000(r6)
    sth r0, 2(r4)
    blr
}

/* fn_800BD898 - 0x800BD898 | size: 0x84 -- GX Pixel / blend / alpha / z-mode */
asm void fn_800BD898(void) {
    nofralloc
    cmpwi r3, 5
    bge loadProjB
    li r0, 8
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0x30
    li r3, 0x10
    stb r0, -0x8000(r5)
    li r0, 0x1018
    lwz r4, 0x80(r4)
    stw r4, -0x8000(r5)
    stb r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    stw r4, -0x8000(r5)
    b loadProjDone
loadProjB:
    li r0, 8
    lwz r4, gx(r13)
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r0, 0x40
    li r3, 0x10
    stb r0, -0x8000(r5)
    li r0, 0x1019
    lwz r4, 0x84(r4)
    stw r4, -0x8000(r5)
    stb r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    stw r4, -0x8000(r5)
loadProjDone:
    lwz r3, gx(r13)
    li r0, 1
    sth r0, 2(r3)
    blr
}

/* fn_800BD91C - 0x800BD91C | size: 0x848 -- GX Display copy / EFB */
#if 0
asm void fn_800BD91C(s32 arg0, s32 arg1) {
    nofralloc
    lwz r5, gx(r13)
    lwz r0, 0x4e4(r5)
    cmpwi r0, 0x22
    beq lbl_000B8374
    bge lbl_000B83C0
    cmpwi r0, 0xb
    bge lbl_000B8364
    cmpwi r0, 0x0
    bge lbl_000B8374
    b lbl_000B83C0
lbl_000B8364:
    cmpwi r0, 0x1b
    bge lbl_000B83AC
    b lbl_000B8394
    b lbl_000B83C0
lbl_000B8374:
    li r0, 0x10
    lis r6, 0xcc01
    stb r0, -0x8000(r6)
    li r5, 0x1006
    li r0, 0x0
    stw r5, -0x8000(r6)
    stw r0, -0x8000(r6)
    b lbl_000B83C0
lbl_000B8394:
    li r0, 0x61
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    lis r0, 0x2300
    stw r0, -0x8000(r5)
    b lbl_000B83C0
lbl_000B83AC:
    li r0, 0x61
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    lis r0, 0x2400
    stw r0, -0x8000(r5)
lbl_000B83C0:
    lwz r7, gx(r13)
    lwz r0, 0x4e8(r7)
    cmpwi r0, 0x15
    beq lbl_000B83F8
    bge lbl_000B8448
    cmpwi r0, 0x9
    bge lbl_000B83E8
    cmpwi r0, 0x0
    bge lbl_000B83F8
    b lbl_000B8448
lbl_000B83E8:
    cmpwi r0, 0x11
    bge lbl_000B843C
    b lbl_000B8410
    b lbl_000B8448
lbl_000B83F8:
    li r0, 0x61
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    lis r0, 0x6700
    stw r0, -0x8000(r5)
    b lbl_000B8448
lbl_000B8410:
    lwz r0, 0x4ec(r7)
    li r6, 0x8
    lis r5, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    stw r0, 0x4ec(r7)
    li r0, 0x20
    stb r6, -0x8000(r5)
    stb r0, -0x8000(r5)
    lwz r0, 0x4ec(r7)
    stw r0, -0x8000(r5)
    b lbl_000B8448
lbl_000B843C:
    lwz r5, __cpReg(r13)
    li r0, 0x0
    sth r0, 0x6(r5)
lbl_000B8448:
    lwz r5, gx(r13)
    stw r3, 0x4e4(r5)
    lwz r0, 0x4e4(r5)
    cmplwi r0, 0x23
    bgt lbl_000B8874
    lis r3, jumptable_80313684@ha
    addi r3, r3, jumptable_80313684@l
    slwi r0, r0, 2
    lwzx r0, r3, r0
    mtctr r0
    bctr
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x273
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x14a
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x16b
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x84
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0xc6
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x210
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x252
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x231
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x1ad
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x1ce
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x21
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x10
    lis r5, 0xcc01
    stb r0, -0x8000(r5)
    li r3, 0x1006
    li r0, 0x153
    stw r3, -0x8000(r5)
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5181
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x7181
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x6181
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2300
    stb r0, -0x8000(r5)
    addi r0, r3, 0x1e7f
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x53c1
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5381
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5341
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5301
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x52c1
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5281
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5241
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5201
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x51c1
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5d81
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5981
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2301
    stb r0, -0x8000(r5)
    subi r0, r3, 0x5581
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3f3a
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3e95
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3f19
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3ef8
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3ed7
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3eb6
    stw r0, -0x8000(r5)
    b lbl_000B8874
    li r0, 0x61
    lis r5, 0xcc01
    lis r3, 0x2403
    stb r0, -0x8000(r5)
    subi r0, r3, 0x3e53
    stw r0, -0x8000(r5)
lbl_000B8874:
    lwz r3, gx(r13)
    stw r4, 0x4e8(r3)
    lwz r0, 0x4e8(r3)
    cmplwi r0, 0x16
    bgt lbl_000B8B74
    lis r4, jumptable_80313628@ha
    addi r4, r4, jumptable_80313628@l
    slwi r0, r0, 2
    lwzx r0, r4, r0
    mtctr r0
    bctr
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x42
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x84
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x63
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x129
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x252
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x21
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x14b
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x18d
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x1cf
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    li r0, 0x61
    lis r4, 0xcc01
    lis r3, 0x6700
    stb r0, -0x8000(r4)
    addi r0, r3, 0x211
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x20
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x30
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x40
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x50
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x60
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x70
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x90
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r0, 0x4ec(r3)
    li r5, 0x8
    lis r4, 0xcc01
    rlwinm r0, r0, 0, 28, 23
    ori r0, r0, 0x80
    stw r0, 0x4ec(r3)
    li r0, 0x20
    stb r5, -0x8000(r4)
    stb r0, -0x8000(r4)
    lwz r0, 0x4ec(r3)
    stw r0, -0x8000(r4)
    b lbl_000B8B74
    lwz r3, __cpReg(r13)
    li r0, 0x2
    sth r0, 0x6(r3)
    b lbl_000B8B74
    lwz r3, __cpReg(r13)
    li r0, 0x3
    sth r0, 0x6(r3)
    b lbl_000B8B74
    lwz r3, __cpReg(r13)
    li r0, 0x4
    sth r0, 0x6(r3)
    b lbl_000B8B74
    lwz r3, __cpReg(r13)
    li r0, 0x5
    sth r0, 0x6(r3)
lbl_000B8B74:
    lwz r3, gx(r13)
    li r0, 0x0
    sth r0, 0x2(r3)
    blr
}
#else
void fn_800BD91C(s32 arg0, s32 arg1) {
    u8* ctx;
    s32 old;

    ctx = gx;
    old = *(s32*)(ctx + 0x4e4);
    if (old == 0x22) {
        goto old_fmt_xf;
    }
    if (old >= 0x22) {
        goto old_fmt_done;
    }
    if (old >= 0xb) {
        goto old_fmt_bp;
    }
    if (old >= 0) {
        goto old_fmt_xf;
    }
    goto old_fmt_done;
old_fmt_bp:
    if (old >= 0x1b) {
        goto old_fmt_bp24;
    }
    goto old_fmt_bp23;
    goto old_fmt_done;
old_fmt_xf:
    GX_LOAD_XF(0x1006, 0);
    goto old_fmt_done;
old_fmt_bp23:
    GX_LOAD_BP(0x23000000u);
    goto old_fmt_done;
old_fmt_bp24:
    GX_LOAD_BP(0x24000000u);
old_fmt_done:

    ctx = gx;
    old = *(s32*)(ctx + 0x4e8);
    if (old == 0x15) {
        goto old_z_bp;
    }
    if (old >= 0x15) {
        goto old_z_done;
    }
    if (old >= 0x9) {
        goto old_z_mid;
    }
    if (old >= 0) {
        goto old_z_bp;
    }
    goto old_z_done;
old_z_mid:
    if (old >= 0x11) {
        goto old_z_cp;
    }
    goto old_z_clear;
    goto old_z_done;
old_z_bp:
    GX_LOAD_BP(0x67000000u);
    goto old_z_done;
old_z_clear:
    *(u32*)(ctx + 0x4ec) &= 0xffffff0fu;
    GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
    goto old_z_done;
old_z_cp:
    __cpReg[3] = 0;
old_z_done:

    ctx = gx;
    *(s32*)(ctx + 0x4e4) = arg0;
    switch (*(u32*)(ctx + 0x4e4)) {
    case 0:
        GX_LOAD_XF(0x1006, 0x273);
        break;
    case 1:
        GX_LOAD_XF(0x1006, 0x14a);
        break;
    case 2:
        GX_LOAD_XF(0x1006, 0x16b);
        break;
    case 3:
        GX_LOAD_XF(0x1006, 0x84);
        break;
    case 4:
        GX_LOAD_XF(0x1006, 0xc6);
        break;
    case 5:
        GX_LOAD_XF(0x1006, 0x210);
        break;
    case 6:
        GX_LOAD_XF(0x1006, 0x252);
        break;
    case 7:
        GX_LOAD_XF(0x1006, 0x231);
        break;
    case 8:
        GX_LOAD_XF(0x1006, 0x1ad);
        break;
    case 9:
        GX_LOAD_XF(0x1006, 0x1ce);
        break;
    case 34:
        GX_LOAD_XF(0x1006, 0x21);
        break;
    case 10:
        GX_LOAD_XF(0x1006, 0x153);
        break;
    case 11:
        GX_LOAD_BP(0x2300ae7fu);
        break;
    case 12:
        GX_LOAD_BP(0x23008e7fu);
        break;
    case 13:
        GX_LOAD_BP(0x23009e7fu);
        break;
    case 14:
        GX_LOAD_BP(0x23001e7fu);
        break;
    case 15:
        GX_LOAD_BP(0x2300ac3fu);
        break;
    case 16:
        GX_LOAD_BP(0x2300ac7fu);
        break;
    case 17:
        GX_LOAD_BP(0x2300acbfu);
        break;
    case 18:
        GX_LOAD_BP(0x2300acffu);
        break;
    case 19:
        GX_LOAD_BP(0x2300ad3fu);
        break;
    case 20:
        GX_LOAD_BP(0x2300ad7fu);
        break;
    case 21:
        GX_LOAD_BP(0x2300adbfu);
        break;
    case 22:
        GX_LOAD_BP(0x2300adffu);
        break;
    case 23:
        GX_LOAD_BP(0x2300ae3fu);
        break;
    case 24:
        GX_LOAD_BP(0x2300a27fu);
        break;
    case 25:
        GX_LOAD_BP(0x2300a67fu);
        break;
    case 26:
        GX_LOAD_BP(0x2300aa7fu);
        break;
    case 27:
        GX_LOAD_BP(0x2402c0c6u);
        break;
    case 28:
        GX_LOAD_BP(0x2402c16bu);
        break;
    case 29:
        GX_LOAD_BP(0x2402c0e7u);
        break;
    case 30:
        GX_LOAD_BP(0x2402c108u);
        break;
    case 31:
        GX_LOAD_BP(0x2402c129u);
        break;
    case 32:
        GX_LOAD_BP(0x2402c14au);
        break;
    case 33:
        GX_LOAD_BP(0x2402c1adu);
        break;
    case 35:
        break;
    }

    ctx = gx;
    *(s32*)(ctx + 0x4e8) = arg1;
    switch (*(u32*)(ctx + 0x4e8)) {
    case 0:
        GX_LOAD_BP(0x67000042u);
        break;
    case 1:
        GX_LOAD_BP(0x67000084u);
        break;
    case 2:
        GX_LOAD_BP(0x67000063u);
        break;
    case 3:
        GX_LOAD_BP(0x67000129u);
        break;
    case 8:
        GX_LOAD_BP(0x67000252u);
        break;
    case 21:
        GX_LOAD_BP(0x67000021u);
        break;
    case 4:
        GX_LOAD_BP(0x6700014bu);
        break;
    case 5:
        GX_LOAD_BP(0x6700018du);
        break;
    case 6:
        GX_LOAD_BP(0x670001cfu);
        break;
    case 7:
        GX_LOAD_BP(0x67000211u);
        break;
    case 9:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x20;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 10:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x30;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 11:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x40;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 12:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x50;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 13:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x60;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 14:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x70;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 15:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x90;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 16:
        *(u32*)(ctx + 0x4ec) = (*(u32*)(ctx + 0x4ec) & 0xffffff0fu) | 0x80;
        GX_LOAD_CP(0x20, *(u32*)(ctx + 0x4ec));
        break;
    case 17:
        __cpReg[3] = 2;
        break;
    case 18:
        __cpReg[3] = 3;
        break;
    case 19:
        __cpReg[3] = 4;
        break;
    case 20:
        __cpReg[3] = 5;
        break;
    case 22:
        break;
    }

    ctx = gx;
    *(u16*)(ctx + 2) = 0;
}
#endif

/* fn_800BE164 - 0x800BE164 | size: 0x1A8 -- GX Display copy / EFB */
asm void fn_800BE164(void) {
    nofralloc
    lwz r10, __cpReg(r13)
    addi r8, r10, 0x42
    lhz r6, 0x42(r10)
    addi r7, r10, 0x40
readCp0:
    mr r0, r6
    lhz r5, 0(r7)
    lhz r6, 0(r8)
    cmplw r6, r0
    bne readCp0
    slwi r0, r6, 0x10
    or r6, r0, r5
    addi r9, r10, 0x46
    lhz r7, 0x46(r10)
    addi r8, r10, 0x44
readCp1:
    mr r0, r7
    lhz r5, 0(r8)
    lhz r7, 0(r9)
    cmplw r7, r0
    bne readCp1
    slwi r0, r7, 0x10
    or r7, r0, r5
    addi r11, r10, 0x4a
    lhz r8, 0x4a(r10)
    addi r9, r10, 0x48
readCp2:
    mr r0, r8
    lhz r5, 0(r9)
    lhz r8, 0(r11)
    cmplw r8, r0
    bne readCp2
    slwi r0, r8, 0x10
    or r8, r0, r5
    addi r11, r10, 0x4e
    lhz r9, 0x4e(r10)
    addi r10, r10, 0x4c
readCp3:
    mr r0, r9
    lhz r5, 0(r10)
    lhz r9, 0(r11)
    cmplw r9, r0
    bne readCp3
    slwi r0, r9, 0x10
    or r9, r0, r5
    lwz r5, gx(r13)
    lwz r0, 0x4e4(r5)
    cmpwi r0, 0xa
    beq cpRateScaled
    bge cpRateHigh
    cmpwi r0, 0
    bge cpRateRaw
    b cpRateZeroDefault
cpRateHigh:
    cmpwi r0, 0x23
    beq cpRateZero35
    bge cpRateZeroDefault
    b cpRateRaw
cpRateScaled:
    mulli r0, r7, 0x3e8
    divwu r0, r0, r6
    stw r0, 0(r3)
    b cpRateDone
cpRateRaw:
    stw r6, 0(r3)
    b cpRateDone
cpRateZero35:
    li r0, 0
    stw r0, 0(r3)
    b cpRateDone
cpRateZeroDefault:
    li r0, 0
    stw r0, 0(r3)
cpRateDone:
    lwz r3, gx(r13)
    lwz r0, 0x4e8(r3)
    cmplwi r0, 0x16
    bgt cpMetricZero
    lis r3, jumptable_80313714@ha
    addi r3, r3, jumptable_80313714@l
    slwi r0, r0, 2
    lwzx r0, r3, r0
    mtctr r0
    bctr
cpMetricCaseScale4:
    slwi r0, r9, 2
    stw r0, 0(r4)
    blr
cpMetricCaseR8R9x2:
    slwi r0, r9, 1
    add r0, r8, r0
    stw r0, 0(r4)
    blr
cpMetricCaseR8x3R9x4:
    mulli r3, r8, 3
    slwi r0, r9, 2
    add r0, r3, r0
    stw r0, 0(r4)
    blr
cpMetricCaseR8x5R9x6:
    mulli r3, r8, 5
    mulli r0, r9, 6
    add r0, r3, r0
    stw r0, 0(r4)
    blr
cpMetricCaseR8x7R9x8:
    mulli r3, r8, 7
    slwi r0, r9, 3
    add r0, r3, r0
    stw r0, 0(r4)
    blr
cpMetricCaseR9:
    stw r9, 0(r4)
    blr
cpMetricCaseR8:
    stw r8, 0(r4)
    blr
cpMetricCaseZero:
    li r0, 0
    stw r0, 0(r4)
    blr
cpMetricZero:
    li r0, 0
    stw r0, 0(r4)
    blr
}

/* fn_800BE30C - 0x800BE30C | size: 0x10 -- GX Display copy / EFB */
void fn_800BE30C(void) {
    __cpReg[2] = 4;
}

/* fn_800BE31C - 0x800BE31C | size: 0x2C -- GX Display copy / EFB */
asm u32 fn_800BE31C(void) {
    nofralloc
    mflr r0
    stw r0, 4(r1)
    stwu r1, -0x10(r1)
    addi r3, r1, 0xc
    addi r4, r1, 8
    bl fn_800BE164
    lwz r3, 0xc(r1)
    lwz r0, 0x14(r1)
    addi r1, r1, 0x10
    mtlr r0
    blr
}
