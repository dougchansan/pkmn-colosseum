#include "dolphin/types.h"

/*
 * TRKBoard.c - TRK board-level support functions.
 *
 * Tail range containing the communication-table read/write wrappers.
 * The preceding board support functions live in TRKBoard_range_800C33BC.c.
 */

/* Communication table */
extern u8 gDBCommTable[];

/* TRKWriteUARTN - 0x800C3588 | size: 0x3C */
int TRKWriteUARTN(int a, int b) {
    s32 ret = ((int (**)(int, int))gDBCommTable)[5](a, b);
    return (s32)((u32)-ret | (u32)ret) >> 31;
}

/* TRKReadUARTN - 0x800C35C4 | size: 0x3C */
int TRKReadUARTN(int a, int b) {
    s32 ret = ((int (**)(int, int))gDBCommTable)[4](a, b);
    return (s32)((u32)-ret | (u32)ret) >> 31;
}

/* TRKPollUART - 0x800C3600 | size: 0x30 */
void TRKPollUART(void) {
    typedef void (*CommFunc)(void);
    CommFunc func = ((CommFunc*)gDBCommTable)[3];

    func();
}
