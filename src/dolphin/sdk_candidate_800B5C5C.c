/** Candidate-only residual range. */
#include "src/dolphin/sdk_range_800AE3F0.c"

extern GXData* const gx;
extern u16* __memReg;
extern u16* __cpReg;
extern u32 lbl_8047A988;
extern OSTime lbl_8047A990;
extern u32 lbl_8047A998;
extern OSTime OSGetTime(void);
extern void fn_800B7484(void* callback);
extern void fn_800B8FD8(void* callback);
extern void fn_800B90A4(void* callback);
extern void PPCSync(void);
extern void __GXAbort(void);

static inline u32 ReadMEMCounter(u32 high, u32 low)
{
    u16 high0;
    u16 high1;
    u16 lowValue;

    high0 = __memReg[high];
    do {
        high1 = high0;
        high0 = __memReg[high];
        lowValue = __memReg[low];
    } while (high0 != high1);
    return ((u32)high0 << 16) | lowValue;
}

s32 __GXShutdown_800C6260(BOOL final)
{
    u32 reg;
    u32 peCountNew;
    OSTime timeNew;
    volatile u32* fifo = (volatile u32*)0xCC008000;

    if (!final) {
        if (!lbl_8047A998) {
            lbl_8047A988 = ReadMEMCounter(0x27, 0x28);
            lbl_8047A990 = OSGetTime();
            lbl_8047A998 = 1;
            return 0;
        }
        timeNew = OSGetTime();
        peCountNew = ReadMEMCounter(0x27, 0x28);
        if (timeNew - lbl_8047A990 < 10) {
            return 0;
        }
        if (peCountNew != lbl_8047A988) {
            lbl_8047A988 = peCountNew;
            lbl_8047A990 = timeNew;
            return 0;
        }
    } else {
        fn_800B7484(NULL);
        fn_800B8FD8(NULL);
        fn_800B90A4(NULL);
        *fifo = 0;
        *fifo = 0;
        *fifo = 0;
        *fifo = 0;
        *fifo = 0;
        *fifo = 0;
        *fifo = 0;
        *fifo = 0;
        PPCSync();
        reg = 0;
        __cpReg[1] = reg;
        reg = 3;
        __cpReg[2] = reg;
        ((u8*)gx)[0x4F2] = 1;
        __GXAbort();
    }
    return 1;
}
