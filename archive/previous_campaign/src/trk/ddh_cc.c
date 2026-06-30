#include "dolphin/types.h"

/*
 * ddh_cc.c - DDH (AMC/EXI serial) communication backend.
 *
 * Implements the communication interface for the standard GameCube
 * serial debug connection via EXI channel 2 (AMC protocol).
 * This is the default debug communication path used by most
 * development hardware.
 */

extern void MWTRACE(s32 level, const char* fmt, ...);

/* Low-level AMC hardware interface functions */
extern void fn_800CE7A0(void);     /* AMC_InitInterrupts */
extern s32  fn_800CE7A4(void);     /* AMC_Peek - returns bytes available */
extern s32  fn_800CE7AC(void* buf, s32 size); /* AMC_Read */
extern s32  fn_800CE7B4(void* buf, s32 size); /* AMC_Write */
extern void fn_800CE7BC(void);     /* AMC_PostStop */
extern void fn_800CE7C0(void);     /* AMC_PreContinue */
extern void fn_800CE79C(void* callback, void* pendingPtr); /* AMC_Initialize */

/* Receive FIFO buffer management */
extern void fn_800C404C(void* fifo, void* data, s32 size); /* FIFO_Push */
extern s32  fn_800C41A4(void* fifo);                        /* FIFO_Count */
extern void fn_800C4154(void* fifo, void* buffer, s32 size); /* FIFO_Init */
extern s32  fn_800C3F44(void* fifo, void* dst, s32 size);   /* FIFO_Pop */

/* SDA-relative flag: nonzero if port is open */
extern s32 lbl_8047A9E0; /* ddh_cc open flag */

/* Receive FIFO at lbl_803FF578 */
extern u8 lbl_803FF578[];

/* FIFO backing buffer at lbl_803FED78, 0x800 bytes */
extern u8 lbl_803FED78[];

/* String tables for trace messages */
extern char lbl_8026FC98[];
extern char lbl_8026FCF4[];
extern char lbl_8026FD1C[];
extern char lbl_8026FD4C[];
extern char lbl_8026FD60[];

/*
 * ddh_cc_initinterrupts - Enable AMC interrupts for async reception.
 */
s32 ddh_cc_initinterrupts(void) {
    fn_800CE7A0();
    return 0;
}

/*
 * ddh_cc_peek - Check for available data and buffer it.
 *
 * Calls AMC_Peek to check how many bytes are available.
 * If data is available, reads it into a local buffer and pushes
 * it into the receive FIFO. Returns the number of bytes available,
 * or 0 if none, or a negative error code.
 */
s32 ddh_cc_peek(void) {
    s32 avail;
    u8 tmpBuf[0x800];

    avail = fn_800CE7A4();

    if (avail <= 0) {
        return 0;
    }

    {
        s32 err = fn_800CE7AC(tmpBuf, avail);
        if (err != 0) {
            return -10009; /* 0xD8E7 */
        }

        fn_800C404C(lbl_803FF578, tmpBuf, avail);
    }

    return avail;
}

/*
 * ddh_cc_post_stop - Called after the target stops.
 * Notifies the AMC hardware that the target has stopped.
 */
s32 ddh_cc_post_stop(void) {
    fn_800CE7BC();
    return 0;
}

/*
 * ddh_cc_pre_continue - Called before the target continues.
 * Notifies the AMC hardware that the target is about to resume.
 */
s32 ddh_cc_pre_continue(void) {
    fn_800CE7C0();
    return 0;
}

/*
 * ddh_cc_write - Write data to the debug port.
 *
 * Sends data in chunks via AMC_Write. Loops until all data is
 * sent or an error occurs. Returns 0 on success, negative on error.
 */
s32 ddh_cc_write(void* data, s32 size) {
    char* strings = lbl_8026FC98;
    u8* ptr = (u8*)data;
    s32 remaining = size;

    /* Check if port is open */
    if (lbl_8047A9E0 == 0) {
        MWTRACE(8, strings);
        return -10001; /* 0xD8EF */
    }

    MWTRACE(8, strings + 0x14, data, size);

    while (remaining > 0) {
        s32 written;

        MWTRACE(1, strings + 0x40, remaining);

        written = fn_800CE7B4(ptr, remaining);
        if (written == 0) {
            break;
        }

        ptr += written;
        remaining -= written;
    }

    return 0;
}

/*
 * ddh_cc_read - Read data from the debug port.
 *
 * Reads data from the receive FIFO, polling the AMC hardware
 * for more data as needed until the requested amount is available.
 * Returns 0 on success, negative on error.
 */
s32 ddh_cc_read(void* data, s32 size) {
    s32 err = 0;
    u8 tmpBuf[0x800];

    /* Check if port is open */
    if (lbl_8047A9E0 == 0) {
        return -10001; /* 0xD8EF */
    }

    MWTRACE(1, lbl_8026FCF4, size, size);

    {
        void* fifo = lbl_803FF578;

        /* Poll until we have enough data in the FIFO */
        while ((u32)fn_800C41A4(fifo) < (u32)size) {
            s32 avail;
            s32 readErr;

            err = 0;
            avail = fn_800CE7A4();
            if (avail == 0) {
                continue;
            }

            readErr = fn_800CE7AC(tmpBuf, avail);
            if (readErr != 0) {
                err = readErr;
                continue;
            }

            fn_800C404C(fifo, tmpBuf, avail);
        }

        /* Extract requested data from FIFO */
        if (err == 0) {
            fn_800C3F44(fifo, data, size);
        } else {
            MWTRACE(8, lbl_8026FD1C, err);
        }
    }

    return err;
}

/*
 * ddh_cc_close - Close the debug port.
 * Currently a no-op; returns success.
 */
s32 ddh_cc_close(void) {
    return 0;
}

/*
 * ddh_cc_open - Open the debug port.
 *
 * Checks if the port is already open. If so, returns an error.
 * Otherwise, sets the open flag and returns success.
 */
s32 ddh_cc_open(void) {
    if (lbl_8047A9E0 != 0) {
        return -10005; /* 0xD8EB */
    }

    lbl_8047A9E0 = 1;
    return 0;
}

/*
 * ddh_cc_shutdown - Shut down the debug port.
 * Currently a no-op; returns success.
 */
s32 ddh_cc_shutdown(void) {
    return 0;
}

/*
 * ddh_cc_initialize - Initialize the DDH communication backend.
 *
 * Sets up the AMC hardware with the given callback and pending pointer,
 * then initializes the receive FIFO with an 0x800-byte buffer.
 */
s32 ddh_cc_initialize(void* callback, void* pendingPtr) {
    MWTRACE(1, lbl_8026FD4C);

    fn_800CE79C(callback, pendingPtr);

    MWTRACE(1, lbl_8026FD60);

    fn_800C4154(lbl_803FF578, lbl_803FED78, 0x800);

    return 0;
}
