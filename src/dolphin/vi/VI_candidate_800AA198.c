#include "dolphin/types.h"

typedef struct VITiming_800AA198 {
    u8 equ;
    u16 acv;
    u16 prbOdd;
    u16 prbEven;
    u16 psbOdd;
    u16 psbEven;
} VITiming_800AA198;

typedef struct VIContext_800AA198 {
    u8 _00[0xF0];
    u8 horVer[0x30];
    void* bufAddr;
    u32 fbb;
    u32 tfbb;
    u8 _12C[0x04];
    BOOL black;
    u8 _134[0x08];
    u32 bfbb;
    u32 btfbb;
    VITiming_800AA198* timing;
} VIContext_800AA198;

extern VIContext_800AA198 lbl_803FC488;
extern u32 lbl_8047A898;
extern BOOL OSDisableInterrupts(void);
extern BOOL OSRestoreInterrupts(BOOL level);
extern void setFbbRegs(void* horVer, u32* fbb, u32* tfbb, u32* bfbb,
                       u32* btfbb);
extern void setVerticalRegs(u16 dispPosY, u16 dispSizeY, u8 equ, u16 acv,
                            u16 prbOdd, u16 prbEven, u16 psbOdd, u16 psbEven,
                            BOOL black);

void VISetNextFrameBuffer(void* fb)
{
    VIContext_800AA198* context = &lbl_803FC488;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    context->bufAddr = fb;
    lbl_8047A898 = TRUE;
    setFbbRegs(context->horVer, &context->fbb, &context->tfbb, &context->bfbb,
               &context->btfbb);
    OSRestoreInterrupts(enabled);
}

void VISetBlack(BOOL black)
{
    VIContext_800AA198* context = &lbl_803FC488;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    context->black = black;
    setVerticalRegs(*(u16*)((u8*)context + 0xFA), *(u16*)((u8*)context + 0xF6),
                    context->timing->equ, context->timing->acv,
                    context->timing->prbOdd, context->timing->prbEven,
                    context->timing->psbOdd, context->timing->psbEven,
                    context->black);
    OSRestoreInterrupts(enabled);
}
