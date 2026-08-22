#include "dolphin/types.h"

extern u32 fn_8015B250(u32, u32);
extern u32 lbl_8047B010;
extern u32 lbl_8047B098;
extern u16 lbl_8047B00C;
extern u32 fn_800AE794(void);
extern void PPCSync(void);
extern void DSPSendMailToDSP(u32);
extern u32 OSGetTick(void);
extern u32 lbl_8047B08C;
extern u32 OSDisableInterrupts(void);
extern u32 lbl_8047B080;
extern u16 lbl_8047B084;

u32 salGetStartDelay(void);

void salCtrlDsp(u32 arg)
{
    fn_8015B250(arg, salGetStartDelay());
    arg = lbl_8047B010;
    lbl_8047B098 = 0;
    PPCSync();
    DSPSendMailToDSP((u32)lbl_8047B00C | 0xBABE0000);
    while (fn_800AE794() != 0) {
    }
    DSPSendMailToDSP(arg);
    while (fn_800AE794() != 0) {
    }
}

u32 salGetStartDelay(void)
{
    u32 tick;
    u32 divisor;
    u32 prev;

    tick = OSGetTick();
    divisor = *(volatile u32*)0x800000F8;
    prev = lbl_8047B08C;
    divisor = __mulhwu(0x431BDE83u, divisor >> 2);
    tick = tick - prev;
    tick = tick << 3;
    return tick / (divisor >> 15);
}

void hwInitIrq(void)
{
    lbl_8047B080 = OSDisableInterrupts();
    lbl_8047B084 = 1;
}

void fn_80164324(void)
{
}
