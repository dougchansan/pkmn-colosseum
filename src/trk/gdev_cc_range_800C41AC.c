#include "dolphin/types.h"

typedef struct CircleBuffer CircleBuffer;

/* Low-level GDEV hardware interface functions */
extern void DBInitInterrupts(void); /* GDEV_InitInterrupts */
extern u32 DBQueryData(void);
extern s32 DBRead(void* buffer, u32 size);
extern s32 DBWrite(const void* buffer, s32 size);
extern void fn_800CE7D8(void); /* GDEV_PostStop */
extern void fn_800CE7D4(void); /* GDEV_PreContinue */
extern u32 CBGetBytesAvailableForRead(CircleBuffer* circle);

extern s32 CircleBufferWriteBytes(CircleBuffer* circle, u8* buffer, u32 size);
extern s32 CircleBufferReadBytes(CircleBuffer* circle, u8* buffer, u32 size);

extern CircleBuffer lbl_803FFA98;
extern s32 lbl_8047A9E8;
extern void MWTRACE(s32 level, const char* format, ...);

/*
 * Partial source for the gdev_cc 0x800C41AC range. The two buffered I/O
 * routines use the same data/length roles as TRK_MINNOW_DOLPHIN.
 */

/* gdev_cc_initinterrupts - Enable GDEV interrupts for async reception. */
s32 gdev_cc_initinterrupts(void) {
    DBInitInterrupts();
    return 0;
}

/* gdev_cc_peek - Pull pending GDEV data into the receive circle buffer. */
s32 gdev_cc_peek(void) {
    u8 buffer[0x500];
    s32 size;

    size = DBQueryData();
    if (size <= 0) {
        return 0;
    }
    if (DBRead(buffer, size) == 0) {
        CircleBufferWriteBytes(&lbl_803FFA98, buffer, size);
    } else {
        return -0x2719;
    }
    return size;
}

/* gdev_cc_post_stop - Called after the target stops. */
s32 gdev_cc_post_stop(void) {
    fn_800CE7D8();
    return 0;
}

/* gdev_cc_pre_continue - Called before the target continues. */
s32 gdev_cc_pre_continue(void) {
    fn_800CE7D4();
    return 0;
}

/* gdev_cc_write - Write data to the GDEV debug port. */
s32 gdev_cc_write(void* data, s32 size) {
    s32 remaining = size;
    u8* buffer = data;
    s32 written;

    if (lbl_8047A9E8 == 0) {
        MWTRACE(8, "cc not initialized\n");
        return -0x2711;
    }

    MWTRACE(8, "cc_write : Output data 0x%08x %ld bytes\n", data, size);
    while (remaining > 0) {
        MWTRACE(1, "cc_write sending %ld bytes\n", remaining);
        written = DBWrite(buffer, remaining);
        if (written == 0) {
            break;
        }
        buffer += written;
        remaining -= written;
    }
    return 0;
}

/* gdev_cc_read - Read data from the GDEV debug port. */
#pragma use_lmw_stmw on
s32 gdev_cc_read(u8* buffer, s32 size) {
    u8 readBuffer[0x500];
    u32 readSize;
    u32 requestedSize;
    u32 error;

    error = 0;

    if (lbl_8047A9E8 == 0) {
        return -0x2711;
    }

    MWTRACE(1, "Expected packet size : 0x%08x (%ld)\n", size, size);
    requestedSize = size;
    readSize = size;
    {
        while (CBGetBytesAvailableForRead(&lbl_803FFA98) < readSize) {
            error = 0;
            size = DBQueryData();
            if (size != 0) {
                error = DBRead(readBuffer, readSize);
                if (error == 0) {
                    CircleBufferWriteBytes(&lbl_803FFA98, readBuffer, size);
                }
            }
        }

        if (error == 0) {
            CircleBufferReadBytes(&lbl_803FFA98, buffer, requestedSize);
        } else {
            MWTRACE(8, "cc_read : error reading bytes from EXI2 %ld\n", error);
        }
    }
    return error;
}
#pragma use_lmw_stmw reset
