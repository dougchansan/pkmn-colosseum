#include "dolphin/types.h"

/*
 * gdev_cc.c - GDEV (USB Gecko) communication backend.
 *
 * Implements the communication interface for USB Gecko debug hardware.
 * Structurally identical to ddh_cc.c but uses the GDEV hardware
 * interface instead of AMC/EXI. The GDEV uses a 0x500-byte receive
 * buffer (vs 0x800 for DDH).
 */

extern void MWTRACE(s32 level, const char* fmt, ...);

/* Low-level GDEV hardware interface functions */
extern void fn_800CEB64(void);     /* GDEV_InitInterrupts */
extern s32  fn_800CEAC8(void);     /* GDEV_Peek - returns bytes available */
extern s32  fn_800CEA3C(void* buf, s32 size); /* GDEV_Read */
extern s32  fn_800CE7DC(void* buf, s32 size); /* GDEV_Write */
extern void fn_800CE7D8(void);     /* GDEV_PostStop */
extern void fn_800CE7D4(void);     /* GDEV_PreContinue */
extern void fn_800CEBB8(void* callback, void* pendingPtr); /* GDEV_Initialize */

/* Receive FIFO buffer management */
extern void fn_800C404C(void* fifo, void* data, s32 size); /* FIFO_Push */
extern s32  fn_800C41A4(void* fifo);                        /* FIFO_Count */
extern void fn_800C4154(void* fifo, void* buffer, s32 size); /* FIFO_Init */
extern s32  fn_800C3F44(void* fifo, void* dst, s32 size);   /* FIFO_Pop */

/* SDA-relative flag: nonzero if port is open */
extern s32 lbl_8047A9E8; /* gdev_cc open flag */

/* Receive FIFO at lbl_803FFA98 */
extern u8 lbl_803FFA98[];

/* FIFO backing buffer at lbl_803FF598, 0x500 bytes */
extern u8 lbl_803FF598[];

/* String tables for trace messages */
extern char lbl_8026FD78[];
extern char lbl_8026FDD4[];
extern char lbl_8026FDFC[];
extern char lbl_8026FE2C[];
extern char lbl_8026FE40[];

/*
 * gdev_cc_initinterrupts - Enable GDEV interrupts for async reception.
 */
s32 gdev_cc_initinterrupts(void) {
    fn_800CEB64();
    return 0;
}

/*
 * gdev_cc_peek - Check for available data and buffer it.
 *
 * Polls the GDEV hardware for available data. If data is present,
 * reads it into a temporary buffer and pushes it into the receive FIFO.
 * Returns the byte count available, 0 if none, or negative on error.
 */
s32 gdev_cc_peek(void) {
    s32 avail;
    u8 tmpBuf[0x500];

    avail = fn_800CEAC8();

    if (avail <= 0) {
        return 0;
    }

    {
        s32 err = fn_800CEA3C(tmpBuf, avail);
        if (err != 0) {
            return -10009; /* 0xD8E7 */
        }

        fn_800C404C(lbl_803FFA98, tmpBuf, avail);
    }

    return avail;
}

/*
 * gdev_cc_post_stop - Called after the target stops.
 * Notifies the GDEV hardware that the target has stopped.
 */
s32 gdev_cc_post_stop(void) {
    fn_800CE7D8();
    return 0;
}

/*
 * gdev_cc_pre_continue - Called before the target continues.
 * Notifies the GDEV hardware that the target is about to resume.
 */
s32 gdev_cc_pre_continue(void) {
    fn_800CE7D4();
    return 0;
}

/*
 * gdev_cc_write - Write data to the debug port.
 *
 * Sends data in chunks via GDEV_Write. Loops until all data is
 * sent or an error occurs. Returns 0 on success, negative on error.
 */
s32 gdev_cc_write(void* data, s32 size) {
    char* strings = lbl_8026FD78;
    u8* ptr = (u8*)data;
    s32 remaining = size;

    /* Check if port is open */
    if (lbl_8047A9E8 == 0) {
        MWTRACE(8, strings);
        return -10001; /* 0xD8EF */
    }

    MWTRACE(8, strings + 0x14, data, size);

    while (remaining > 0) {
        s32 written;

        MWTRACE(1, strings + 0x40, remaining);

        written = fn_800CE7DC(ptr, remaining);
        if (written == 0) {
            break;
        }

        ptr += written;
        remaining -= written;
    }

    return 0;
}

/*
 * gdev_cc_read - Read data from the debug port.
 *
 * Reads data from the receive FIFO, polling the GDEV hardware
 * for more data as needed until the requested amount is available.
 * Returns 0 on success, negative on error.
 */
s32 gdev_cc_read(void* data, s32 size) {
    s32 err = 0;
    u8 tmpBuf[0x500];
    s32 requestedSize = size;

    /* Check if port is open */
    if (lbl_8047A9E8 == 0) {
        return -10001; /* 0xD8EF */
    }

    MWTRACE(1, lbl_8026FDD4, size, size);

    {
        void* fifo = lbl_803FFA98;

        /* Poll until we have enough data in the FIFO */
        while ((u32)fn_800C41A4(fifo) < (u32)size) {
            s32 avail;
            s32 readErr;

            err = 0;
            avail = fn_800CEAC8();
            if (avail == 0) {
                continue;
            }

            readErr = fn_800CEA3C(tmpBuf, avail);
            if (readErr != 0) {
                err = readErr;
                continue;
            }

            fn_800C404C(fifo, tmpBuf, avail);
        }

        /* Extract requested data from FIFO */
        if (err == 0) {
            fn_800C3F44(fifo, data, requestedSize);
        } else {
            MWTRACE(8, lbl_8026FDFC, err);
        }
    }

    return err;
}

/*
 * gdev_cc_close - Close the debug port.
 * Currently a no-op; returns success.
 */
s32 gdev_cc_close(void) {
    return 0;
}

/*
 * gdev_cc_open - Open the debug port.
 *
 * Checks if the port is already open. If so, returns an error.
 * Otherwise, sets the open flag and returns success.
 */
s32 gdev_cc_open(void) {
    if (lbl_8047A9E8 != 0) {
        return -10005; /* 0xD8EB */
    }

    lbl_8047A9E8 = 1;
    return 0;
}

/*
 * gdev_cc_shutdown - Shut down the debug port.
 * Currently a no-op; returns success.
 */
s32 gdev_cc_shutdown(void) {
    return 0;
}

/*
 * gdev_cc_initialize - Initialize the GDEV communication backend.
 *
 * Sets up the GDEV hardware with the given callback and pending pointer,
 * then initializes the receive FIFO with a 0x500-byte buffer.
 */
s32 gdev_cc_initialize(void* callback, void* pendingPtr) {
    MWTRACE(1, lbl_8026FE2C);

    fn_800CEBB8(callback, pendingPtr);

    MWTRACE(1, lbl_8026FE40);

    fn_800C4154(lbl_803FFA98, lbl_803FF598, 0x500);

    return 0;
}
