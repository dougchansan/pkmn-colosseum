#include "dolphin/types.h"

extern s32 OSDisableInterrupts(void);
extern void OSRestoreInterrupts(s32 enabled);
extern void DBGRead(u32 type, void* buf, u32 len);
extern void DBGReadStatus(void* resp);
extern void DBGReadMailbox(void* resp);

extern u32 lbl_8047AA30;
extern u32 lbl_8047AA34;
extern u8 lbl_8047AA3C;

/*
 * DBRead - EXI2 read function.
 * 0x800CEA3C | size: 0x8C
 */
s32 DBRead(void* buf, s32 len) {
    s32 enabled;
    enabled = OSDisableInterrupts();
    DBGRead(((lbl_8047AA30 & 0x10000) ? 0x1000 : 0) + 0x1E000, buf,
            (len + 3) & ~3);
    lbl_8047AA34 = 0;
    lbl_8047AA3C = 0;
    OSRestoreInterrupts(enabled);
    return 0;
}

/*
 * DBQueryData - EXI2 peek/status check.
 * 0x800CEAC8 | size: 0x9C
 */
u32 DBQueryData(void) {
    s32 enabled;
    u32 resp;
    lbl_8047AA3C = 0;
    if ((s32)lbl_8047AA34 == 0) {
        enabled = OSDisableInterrupts();
        DBGReadStatus(&resp);
        if (resp & 1) {
            DBGReadMailbox(&resp);
            resp &= 0x1FFFFFFF;
            if ((resp & 0x1F000000) == 0x1F000000) {
                lbl_8047AA30 = resp;
                lbl_8047AA34 = resp & 0x7FFF;
                lbl_8047AA3C = 1;
            }
        }
        OSRestoreInterrupts(enabled);
    }
    return lbl_8047AA34;
}
