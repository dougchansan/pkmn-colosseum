#include "dolphin/types.h"

/* Low-level AMC hardware interface functions */
extern void EXI2_EnableInterrupts(void); /* AMC_InitInterrupts */
extern void EXI2_Reserve(void); /* AMC_PostStop */
extern void EXI2_Unreserve(void); /* AMC_PreContinue */

/* Source for the ddh_cc 0x800C3C00 communication helpers. */

/* ddh_cc_initinterrupts - Enable AMC interrupts for async reception. */
s32 ddh_cc_initinterrupts(void) {
    EXI2_EnableInterrupts();
    return 0;
}

/* ddh_cc_peek - Pull pending AMC data into the receive circle buffer. */
s32 ddh_cc_peek(void) {
    typedef struct CircleBuffer CircleBuffer;
    extern s32 EXI2_Poll(void);
    extern s32 EXI2_ReadN(void* buffer, s32 size);
    extern CircleBuffer lbl_803FF578;
    extern s32 CircleBufferWriteBytes(CircleBuffer* circle, u8* buffer, u32 size);
    u8 buffer[0x800];
    s32 size;

    size = EXI2_Poll();
    if (size <= 0) {
        return 0;
    }
    if (EXI2_ReadN(buffer, size) == 0) {
        CircleBufferWriteBytes(&lbl_803FF578, buffer, size);
    } else {
        return -0x2719;
    }
    return size;
}

/* ddh_cc_post_stop - Called after the target stops. */
s32 ddh_cc_post_stop(void) {
    EXI2_Reserve();
    return 0;
}

/* ddh_cc_pre_continue - Called before the target continues. */
s32 ddh_cc_pre_continue(void) {
    EXI2_Unreserve();
    return 0;
}

/* ddh_cc_write - Write data to the AMC debug port. */
s32 ddh_cc_write(void* data, s32 size) {
    extern s32 lbl_8047A9E0;
    extern void MWTRACE(s32 level, const char* format, ...);
    extern s32 EXI2_WriteN(u8* buffer, s32 size);
    s32 remaining = size;
    u8* buffer = data;
    s32 written;

    if (lbl_8047A9E0 == 0) {
        MWTRACE(8, "cc not initialized\n");
        return -0x2711;
    }

    MWTRACE(8, "cc_write : Output data 0x%08x %ld bytes\n", data, size);
    while (remaining > 0) {
        MWTRACE(1, "cc_write sending %ld bytes\n", remaining);
        written = EXI2_WriteN(buffer, remaining);
        if (written == 0) {
            break;
        }
        buffer += written;
        remaining -= written;
    }
    return 0;
}

/* ddh_cc_read - Read data from the AMC debug port. */
#pragma use_lmw_stmw on
s32 ddh_cc_read(u8* buffer, s32 size) {
    typedef struct CircleBuffer CircleBuffer;
    extern s32 lbl_8047A9E0;
    extern CircleBuffer lbl_803FF578;
    extern void MWTRACE(s32 level, const char* format, ...);
    extern s32 EXI2_Poll(void);
    extern s32 EXI2_ReadN(void* buffer, s32 size);
    extern u32 CBGetBytesAvailableForRead(CircleBuffer* circle);
    extern s32 CircleBufferWriteBytes(CircleBuffer* circle, u8* buffer, u32 size);
    extern s32 CircleBufferReadBytes(CircleBuffer* circle, u8* buffer, u32 size);
    u8 readBuffer[0x800];
    u32 error = 0;

    if (lbl_8047A9E0 == 0) {
        return -0x2711;
    }

    MWTRACE(1, "Expected packet size : 0x%08x (%ld)\n", size, size);
    while (CBGetBytesAvailableForRead(&lbl_803FF578) < (u32)size) {
        s32 readSize;

        error = 0;
        readSize = EXI2_Poll();
        if (readSize == 0) {
            continue;
        }
        error = EXI2_ReadN(readBuffer, readSize);
        if (error != 0) {
            continue;
        }
        CircleBufferWriteBytes(&lbl_803FF578, readBuffer, readSize);
    }

    if (error == 0) {
        CircleBufferReadBytes(&lbl_803FF578, buffer, size);
    } else {
        MWTRACE(8, "cc_read : error reading bytes from EXI2 %ld\n", error);
    }
    return error;
}
#pragma use_lmw_stmw reset
