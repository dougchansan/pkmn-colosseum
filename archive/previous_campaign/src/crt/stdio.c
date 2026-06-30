#include "dolphin/types.h"

/*
 * stdio.c - MetroWerks CRT stdio support.
 *
 * Provides the minimal stdio functions needed by the TRK debugger
 * and CRT. Most of these are stubs or simplified implementations
 * since GameCube has no filesystem or console I/O in the
 * traditional sense.
 */

/* File structure - simplified for GCN */
typedef struct __FILE {
    s32 handle;         /* 0x00 */
    s32 mode;           /* 0x04 */
    s32 state;          /* 0x08 */
    u8  isEOF;          /* 0x0C */
    u8  isErr;          /* 0x0D */
    u8  padding[2];     /* 0x0E */
    u32 position;       /* 0x10 */
    u8* buffer;         /* 0x14 */
    u32 bufferSize;     /* 0x18 */
    u32 bufferPos;      /* 0x1C */
    u32 bufferLen;      /* 0x20 */
    s32 ungetChar;      /* 0x24 */
    s32 wideOrient;     /* 0x28 */
    struct __FILE* next; /* 0x2C */
} __FILE;

typedef void (*FuncPtr)(void);

/* SDA-relative globals */
extern FuncPtr __stdio_exit;
extern void __close_all(void);

/* File list head for open files */
extern __FILE* __file_list;  /* not actually used on GCN but needed for linking */

/*
 * __begin_critical_region - Enter a critical section.
 * No-op on single-threaded GameCube TRK context.
 */
void __begin_critical_region(s32 region) {
    /* empty */
}

/*
 * __end_critical_region - Leave a critical section.
 * No-op on single-threaded GameCube TRK context.
 */
void __end_critical_region(s32 region) {
    /* empty */
}

/*
 * __kill_critical_regions - Destroy all critical sections.
 * No-op on single-threaded GameCube TRK context.
 */
void __kill_critical_regions(void) {
    /* empty */
}

/*
 * __stdio_atexit - Register the stdio cleanup handler.
 * Sets __stdio_exit to point to __close_all so it gets called
 * during program shutdown.
 */
void __stdio_atexit(void) {
    __stdio_exit = (FuncPtr)__close_all;
}

/*
 * __close_all - Close all open file streams.
 *
 * Iterates the linked list of open __FILE structures and
 * flushes/closes each one. On GameCube this primarily handles
 * the stdout/stderr buffers if they were used.
 *
 * NOTE: Full implementation requires matching the complex loop
 * structure from the original binary. This is a simplified version.
 */
void __close_all(void) {
    /* The original iterates __file_list and calls fclose on each.
     * On GCN with TRK, this is effectively empty since no real
     * files are opened. The asm version at 0x800C5458 is 0xA8 bytes. */
}

/*
 * __flush_buffer - Flush a file's output buffer to its destination.
 *
 * Writes buffered data to the underlying file handle via the
 * file's write function. Used by fwrite and fclose.
 *
 * NOTE: Full implementation at 0x800C7454, size 0xC4.
 */
s32 __flush_buffer(__FILE* file, s32* written) {
    /* Stub - needs full asm match for 0xC4 bytes */
    return 0;
}

/*
 * __prep_buffer - Prepare a file's buffer for I/O.
 *
 * Sets up buffer pointers and initializes the buffer state
 * for subsequent read/write operations.
 *
 * Size: 0x34 bytes at 0x800C7518.
 */
void __prep_buffer(__FILE* file) {
    /* Stub - needs full asm match for 0x34 bytes */
}

/*
 * __fwrite - Internal implementation of fwrite.
 *
 * Handles the buffered write logic: fills the current buffer,
 * flushes when full, and handles direct writes for large requests.
 *
 * Size: 0x30C bytes at 0x800C757C.
 */
u32 __fwrite(const void* ptr, u32 size, u32 count, __FILE* file) {
    /* Stub - needs full asm match for 0x30C bytes */
    return 0;
}

/*
 * fwrite - Standard C library fwrite.
 *
 * Writes count elements of size bytes each from ptr to file.
 * Returns the number of elements successfully written.
 *
 * Size: 0x7C bytes at 0x800C7888.
 */
u32 fwrite(const void* ptr, u32 size, u32 count, __FILE* file) {
    u32 total;
    u32 written;

    if (size == 0 || count == 0) {
        return 0;
    }

    total = size * count;
    written = __fwrite(ptr, 1, total, file);

    if (written != 0) {
        return written / size;
    }

    return 0;
}

/*
 * fseek - Seek to a position in a file stream.
 *
 * Size: 0x6C bytes at 0x800C7BF8.
 * On GameCube, this is primarily used by the CRT for
 * stream positioning.
 */
s32 fseek(__FILE* file, s32 offset, s32 whence) {
    /* Stub - needs full asm match for 0x6C bytes */
    return 0;
}

/*
 * fwide - Set or query the orientation of a file stream.
 *
 * If mode > 0, try to set wide orientation.
 * If mode < 0, try to set byte orientation.
 * If mode == 0, just query current orientation.
 *
 * Returns current orientation (positive=wide, negative=byte, 0=unset).
 *
 * Size: 0x88 bytes at 0x800CAA58.
 */
s32 fwide(__FILE* file, s32 mode) {
    s32 orient;

    if (file == NULL) {
        return 0;
    }

    orient = file->wideOrient;

    if (orient == 0) {
        if (mode > 0) {
            file->wideOrient = 1;
        } else if (mode < 0) {
            file->wideOrient = -1;
        }
    }

    return file->wideOrient;
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800C5500 - 0x800C5500 | size: 0x1A4 */
void fn_800C5500(void) {
    extern void fn_800C56A4();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r31 = r4;
    r30 = *(s16*)((u8*)r3 + 0x2);
    r3 = r31;
    fn_800C56A4();
    tmp = *(u8*)((u8*)r31 + 0x5);
    if (tmp > 9) return;
    tmp = (s16)r30;
    if ((s32)tmp > 0x24) {
        r30 = 0x24;
    }
    r6 = (s16)r30;
    if ((s32)tmp <= 0x24) return;
    tmp = *(u8*)((u8*)r31 + 0x4);
    if ((s32)r6 >= (s32)tmp) return;
    r5 = r31 + r6;
    tmp = *(u8*)((u8*)r5 + 0x5);
    if (tmp > 5) {
        tmp = 0x1;

    } else if (tmp < 5) {
        tmp = -0x1;

    }
    r3 = *(u8*)((u8*)r31 + 0x4);
    r4 = r5 + 0x6;
    r3 = r3 + 0x5;
    r3 = r31 + r3;
    tmp = r3 - r4;
    ctr_fn = (void(*)(void))tmp;
    if (r4 < r3) {
        do {
            tmp = *(u8*)((u8*)r4 + 0x0);
            if (tmp != 0) {
                tmp = 0x1;
                break;
            }
            r4 = r4 + 0x1;
        } while (--ctr != 0);
    }
    tmp = *(u8*)((u8*)r5 + 0x4);
    tmp = tmp & 0x1;
    if (tmp != 0) {
        tmp = 0x1;

    } else {
        tmp = -0x1;
    }
    *(u8*)((u8*)r31 + 0x4) = r6;
    if ((s32)tmp < 0) return;
    r5 = r31 + 0x5;
    r4 = r5 + r4;
    tmp = 0x0;
while (1) {
        r3 = *(u8*)((u8*)r4 + 0x0);
        if (r3 < 9) {
            tmp = r3 + 0x1;
            *(u8*)((u8*)r4 + 0x0) = tmp;

        } else if (r4 == r5) {
            tmp = 0x1;
            *(u8*)((u8*)r4 + 0x0) = tmp;
            r3 = *(s16*)((u8*)r31 + 0x2);
            tmp = r3 + 0x1;
            *(u16*)((u8*)r31 + 0x2) = tmp;

        } else {
            *(u8*)((u8*)r4 + 0x0) = tmp;
}
    }
    r5 = 0x0;
    while (1) {
        r3 = *(u8*)((u8*)r31 + 0x4);
        if ((s32)r3 >= (s32)r6) break;
        r4 = *(u8*)((u8*)r31 + 0x4);
        r3 = r4 + 0x1;
        tmp = r4 + 0x5;
        *(u8*)((u8*)r31 + 0x4) = r3;
        *(u8*)(r31 + tmp) = r5;

    }
    tmp = *(s16*)((u8*)r31 + 0x2);
    r5 = 0x0;
    tmp = tmp - r3;
    *(u16*)((u8*)r31 + 0x2) = tmp;
    while (1) {
        tmp = *(u8*)((u8*)r31 + 0x4);
        if ((s32)r5 >= (s32)tmp) break;
        r4 = r5 + 0x5;
        r5 = r5 + 0x1;
        r3 = *(u8*)(r31 + r4);
        tmp = r3 + 0x30;
        *(u8*)(r31 + r4) = tmp;

    }

    return;
}

/* fn_800C56A4 - 0x800C56A4 | size: 0x3B4 */
void fn_800C56A4(void) {
    extern f64 lbl_8047C400;
    extern void fn_800C4CC0();
    extern void fn_800C5A58();
    extern void fn_800C71DC();
    extern void fn_800CDDFC();
    extern void fn_800CDE88();
    extern void fn_800CE04C();
    u8 sp[0xB0];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r26 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    f32 f0 = 0.0f;
    f32 f1 = 0.0f;
    f32 f31 = 0.0f;

    f0 = lbl_8047C400;
    r30 = r3;
    tmp = (u32)tmp >> 31;
    r31 = (s8)tmp;
    if (f0 == f1) {
        *(u8*)((u8*)r30 + 0x0) = r31;
        r3 = 0x0;
        tmp = 0x1;
        *(u16*)((u8*)r30 + 0x2) = r3;
        *(u8*)((u8*)r30 + 0x4) = tmp;
        *(u8*)((u8*)r30 + 0x5) = r3;
        return;
    }
    tmp = 0x7FF00000;
    r3 = r4 & 0x7FF00000;
    if ((s32)r3 != (s32)tmp) {
        if ((s32)r3 >= (s32)tmp) goto L_800C5764;
        if ((s32)r3 != 0) {
            goto L_800C5764;
        }
        tmp = r4 & 0xFFFFF;
        if ((s32)r3 == 0) {
            if ((s32)tmp != 0) {
            }
            tmp = 0x1;
            goto L_800C5768;
            }
        tmp = 0x2;
        goto L_800C5768;
        }
    tmp = r4 & 0xFFFFF;
    if ((s32)tmp == 0) {
        if ((s32)tmp != 0) {
        }
        tmp = 0x5;
        goto L_800C5768;
        }
    tmp = 0x3;
    goto L_800C5768;
L_800C5764:
    tmp = 0x4;
L_800C5768:
    if ((s32)tmp <= 2) {
    do {
        r3 = 0x0;
        *(u8*)((u8*)r30 + 0x0) = r31;
        tmp = 0x7FF00000;
        r4 = 0x1;
        *(u16*)((u8*)r30 + 0x2) = r3;
        r3 = r5 & 0x7FF00000;
        *(u8*)((u8*)r30 + 0x4) = r4;
        if ((s32)r3 != (s32)tmp) {
            if ((s32)r3 >= (s32)tmp) { tmp = 0x4; break; }
            if ((s32)r3 != 0) {
                tmp = 0x4; break;
            }
            tmp = r5 & 0xFFFFF;
            if ((s32)r3 == 0) {
                if ((s32)tmp != 0) {
                }
                tmp = 0x1;
                break;
                }
            tmp = 0x2;
            break;
            }
        tmp = r5 & 0xFFFFF;
        if ((s32)tmp == 0) {
            if ((s32)tmp != 0) {
            }
            tmp = 0x5;
            break;
            }
        tmp = 0x3;
        break;


    } while (0);
        tmp = 0x49;
        if ((s32)tmp == 1) {
            tmp = 0x4e;
        }
        *(u8*)((u8*)r30 + 0x5) = tmp;
        return;
    }
    tmp = (s8)r31;
    if ((s32)tmp != 1) {
        f0 = -f1;
    }
    r3 = (u32)sp + 0x10;
    fn_800CDDFC();
    f31 = f1;
    if (r8 != 0) {
        r5 = 0x10;
        r3 = 0x10000;
        r7 = 0x0;
        r6 = 0x20;
        r4 = r5;
        goto L_800C58A4;
    while (1) {
            /* and. tmp, r8, r3 */;
            if (r8 == 0) {
                r7 = r7 + r4;
                r8 = (u32)r8 >> r4;
                r6 = r6 - r4;

            } else {
                if (r3 == 1) goto L_800C5924;
            }
            if ((s32)r5 > 1) {
                tmp = (u32)r5 >> 31;
                tmp = tmp + r5;
                r5 = (s32)tmp >> 1;
            }
            if (r3 > 1) {
                r3 = (u32)r3 >> r5;
                r4 = r4 - r5;
            }
        L_800C58A4:
            if ((s32)r6 != 0) continue;
        break;
    }

    } else {
        r5 = 0x10;
        r3 = 0x10000;
        r7 = 0x0;
        r4 = r5;
        r8 = tmp | (0x10 << 16);
        r6 = 0x20;
        goto L_800C5918;
    while (1) {
            /* and. tmp, r8, r3 */;
            if ((s32)r6 == 0) {
                r7 = r7 + r4;
                r8 = (u32)r8 >> r4;
                r6 = r6 - r4;

            } else {
                if (r3 == 1) break;
            }
            if ((s32)r5 > 1) {
                tmp = (u32)r5 >> 31;
                tmp = tmp + r5;
                r5 = (s32)tmp >> 1;
            }
            if (r3 > 1) {
                r3 = (u32)r3 >> r5;
                r4 = r4 - r5;
            }
        L_800C5918:
            if ((s32)r6 != 0) continue;
        break;
    }

        r7 = r7 + 0x20;
    }
L_800C5924:
    r26 = 0x35 - r7;
    r3 = (u32)sp + 0x38;
    r4 = tmp - r26;
    fn_800C5A58();
    f1 = f31;
    r3 = r26;
    fn_800CDE88();
    r3 = (u32)sp + 0x30;
    fn_800CE04C();
    fn_800C4CC0();
    r28 = 0x0;
    r27 = r3;
    r26 = r4;
    *(u8*)(sp + 0x64) = r28;
    r3 = r26 ^ r28;
    tmp = r27 ^ r28;
    /* or. tmp, r3, tmp */;
    if ((s32)r6 == 0) {
        tmp = 0x1;
        *(u16*)(sp + 0x66) = r28;
        *(u8*)(sp + 0x68) = tmp;
        *(u8*)(sp + 0x69) = r28;
    } else {

        *(u8*)(sp + 0x68) = r28;
        r29 = (u32)sp + 0x64;
        while (1) {
            r3 = r26 ^ r28;
            tmp = r27 ^ r28;
            /* or. tmp, r3, tmp */;
            if ((s32)r6 == 0) break;
            r3 = r27;
            r4 = r26;
            r5 = 0x0;
            r6 = 0xa;
            __mod2u();
            r8 = *(u8*)(sp + 0x68);
            r3 = r27;
            r5 = 0x0;
            r6 = 0xa;
            r7 = r8 + 0x1;
            tmp = r8 + 0x5;
            *(u8*)(sp + 0x68) = r7;
            *(u8*)(r29 + tmp) = r4;
            r4 = r26;
            __div2u();
            r26 = r4;
            r27 = r3;

        }
        r3 = *(u8*)(sp + 0x68);
        tmp = (u32)sp + 0x64;
        r5 = (u32)sp + 0x69;
        r4 = r3 + 0x5;
        r4 = tmp + r4;
        while (r5 < r4) {

            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = *(u8*)((u8*)r4 + 0x0);
            *(u8*)((u8*)r5 + 0x0) = tmp;
            r5 = r5 + 0x1;
            *(u8*)((u8*)r4 + 0x0) = r3;

        }
        r3 = *(u8*)(sp + 0x68);
        *(u16*)(sp + 0x66) = tmp;
    }
    r3 = r30;
    r4 = (u32)sp + 0x64;
    r5 = (u32)sp + 0x38;
    fn_800C71DC();
    *(u8*)((u8*)r30 + 0x0) = r31;

    return;
}

/* fn_800C5A58 - 0x800C5A58 | size: 0x1784 */
void fn_800C5A58(void) {
    extern u8 lbl_8026FF00[];
    extern void fn_800C5A58();
    extern void fn_800C71DC();
    extern u8 jumptable_803139F0[];
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;

    r31 = r3;
    r30 = r4;
    tmp = r30 + 0x40;
    if (tmp <= 0x48) {
        r3 = (u32)jumptable_803139F0;
        tmp = tmp << 2;
        r3 = (u32)jumptable_803139F0;
        tmp = *(u32*)(r3 + tmp);
        ctr_fn = (void(*)(void))tmp;
        tmp = -0x14;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0x25;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x10;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0x53;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0xa;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0x7a;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x5;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0x92;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x3;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0x9f;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x3;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xa6;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x2;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xac;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x2;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xb2;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x2;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xb7;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x1;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xbb;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x1;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xbf;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = -0x1;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xc2;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = r4;
        r3 = (u32)lbl_8026FF00;
        r5 = r3 + 0xc4;
        *(u8*)((u8*)r31 + 0x0) = r4;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = r4;
        r3 = (u32)lbl_8026FF00;
        r5 = r3 + 0xc6;
        *(u8*)((u8*)r31 + 0x0) = r4;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = r4;
        r3 = (u32)lbl_8026FF00;
        r5 = r3 + 0xc8;
        *(u8*)((u8*)r31 + 0x0) = r4;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = r4;
        r3 = (u32)lbl_8026FF00;
        r5 = r3 + 0xca;
        *(u8*)((u8*)r31 + 0x0) = r4;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = 0x1;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xcc;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = 0x1;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xcf;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = 0x1;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xd2;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = 0x2;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xd5;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
        tmp = 0x2;
        r3 = (u32)lbl_8026FF00;
        *(u16*)((u8*)r31 + 0x2) = tmp;
        r4 = 0x0;
        r3 = (u32)lbl_8026FF00;
        *(u8*)((u8*)r31 + 0x0) = r4;
        r5 = r3 + 0xd9;
        while (1) {
            if ((s32)r4 < 0x24) {
            tmp = *(u8*)((u8*)r5 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r4 == 0x24) break;
            r3 = *(u8*)((u8*)r5 + 0x0);
            tmp = r4 + 0x5;
            r5 = r5 + 0x1;
            r4 = r4 + 0x1;
            *(u8*)(r31 + tmp) = r3;

        }
        }
        *(u8*)((u8*)r31 + 0x4) = r4;
        r3 = *(u8*)((u8*)r5 + 0x0);
        tmp = (s8)r3;
        if ((s32)r4 == 0x24) return;
        tmp = (s8)r3;
        if ((s32)tmp < 5) return;
        if ((s32)tmp <= 5) {
            r3 = r5 + 0x1;
            while (1) {
                r5 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r5;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r5;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = r31 + r4;
            tmp = *(u8*)((u8*)r3 + 0x4);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) return;
        }
        r3 = *(u8*)((u8*)r31 + 0x4);
        r5 = r31 + 0x5;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                return;
            }
            if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)r31 + 0x2);
                tmp = r3 + 0x1;
                *(u16*)((u8*)r31 + 0x2) = tmp;
                return;
            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
        return;
    }
    tmp = (u32)r30 >> 31;
    r3 = (u32)sp + 0x34;
    tmp = tmp + r30;
    r4 = (s32)tmp >> 1;
    fn_800C5A58();
    r4 = (u32)sp + 0x34;
    r3 = r31;
    r5 = r4;
    fn_800C71DC();
    tmp = r30 & 0x1;
    if (r4 == r5) return;
    r3 = *(u32*)((u8*)r31 + 0x0);
    tmp = *(u32*)((u8*)r31 + 0x4);
    *(u32*)(sp + 0xC) = tmp;
    r3 = *(u32*)((u8*)r31 + 0x8);
    tmp = *(u32*)((u8*)r31 + 0xC);
    *(u32*)(sp + 0x14) = tmp;
    r3 = *(u32*)((u8*)r31 + 0x10);
    tmp = *(u32*)((u8*)r31 + 0x14);
    *(u32*)(sp + 0x1C) = tmp;
    r3 = *(u32*)((u8*)r31 + 0x18);
    tmp = *(u32*)((u8*)r31 + 0x1C);
    *(u32*)(sp + 0x24) = tmp;
    r3 = *(u32*)((u8*)r31 + 0x20);
    tmp = *(u32*)((u8*)r31 + 0x24);
    *(u32*)(sp + 0x2C) = tmp;
    tmp = *(u16*)((u8*)r31 + 0x28);
    *(u16*)(sp + 0x30) = tmp;
    if ((s32)r30 > 0) {
        r5 = 0x0;
        r3 = (u32)lbl_8026FF00;
        r4 = (u32)lbl_8026FF00;
        *(u16*)(sp + 0x36) = r5;
        r3 = (u32)sp + 0x34;
        *(u8*)(sp + 0x34) = r5;
        r6 = r4 + 0xc6;
        while (1) {
            if ((s32)r5 < 0x24) {
            tmp = *(u8*)((u8*)r6 + 0x0);
            tmp = (s8)tmp;
            if ((s32)r5 == 0x24) break;
            r4 = *(u8*)((u8*)r6 + 0x0);
            tmp = r5 + 0x5;
            r6 = r6 + 0x1;
            r5 = r5 + 0x1;
            *(u8*)(r3 + tmp) = r4;

        }
        }
        *(u8*)(sp + 0x38) = r5;
        r3 = *(u8*)((u8*)r6 + 0x0);
        tmp = (s8)r3;
        if ((s32)r5 != 0x24) {
            tmp = (s8)r3;
        }
        if ((s32)tmp < 5) goto L_800C71B4;
        if ((s32)tmp <= 5) {
            r3 = r6 + 0x1;
            while (1) {
                r4 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r4;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r4;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = (u32)sp + 0x38;
            tmp = *(u8*)(r3 + r5);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) goto L_800C71B4;
        }
        r3 = *(u8*)(sp + 0x38);
        r5 = (u32)sp + 0x39;
        tmp = 0x0;
        r4 = r5 + r4;
        do {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;

            } else if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)(u32)sp + 0x36);
                tmp = r3 + 0x1;
                *(u16*)(sp + 0x36) = tmp;

            }
            *(u8*)((u8*)r4 + 0x0) = tmp;
        } while (1);
    }
    r5 = 0x0;
    r3 = (u32)lbl_8026FF00;
    tmp = -0x1;
    *(u8*)(sp + 0x34) = r5;
    r3 = (u32)lbl_8026FF00;
    r6 = r3 + 0xc2;
    *(u16*)(sp + 0x36) = tmp;
    r3 = (u32)sp + 0x34;
    while (1) {
        if ((s32)r5 < 0x24) {
        tmp = *(u8*)((u8*)r6 + 0x0);
        tmp = (s8)tmp;
        if ((s32)r5 == 0x24) break;
        r4 = *(u8*)((u8*)r6 + 0x0);
        tmp = r5 + 0x5;
        r6 = r6 + 0x1;
        r5 = r5 + 0x1;
        *(u8*)(r3 + tmp) = r4;

    }
    }
    *(u8*)(sp + 0x38) = r5;
    r3 = *(u8*)((u8*)r6 + 0x0);
    tmp = (s8)r3;
    if ((s32)r5 != 0x24) {
        tmp = (s8)r3;
        if ((s32)tmp < 5) goto L_800C71B4;
        if ((s32)tmp <= 5) {
            r3 = r6 + 0x1;
            while (1) {
                r4 = *(u8*)((u8*)r3 + 0x0);
                tmp = (s8)r4;
                if ((s32)tmp == 0x30) break;
                tmp = (s8)r4;
                if ((s32)tmp != 0x30) break;
                r3 = r3 + 0x1;

            }
            r3 = (u32)sp + 0x38;
            tmp = *(u8*)(r3 + r5);
            tmp = tmp & 0x1;
            if ((s32)tmp == 0x30) goto L_800C71B4;
        }
        r3 = *(u8*)(sp + 0x38);
        r5 = (u32)sp + 0x39;
        tmp = 0x0;
        r4 = r5 + r4;
    while (1) {
            r3 = *(u8*)((u8*)r4 + 0x0);
            if (r3 < 9) {
                tmp = r3 + 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;

            } else if (r4 == r5) {
                tmp = 0x1;
                *(u8*)((u8*)r4 + 0x0) = tmp;
                r3 = *(s16*)((u8*)(u32)sp + 0x36);
                tmp = r3 + 0x1;
                *(u16*)(sp + 0x36) = tmp;

            } else {
                *(u8*)((u8*)r4 + 0x0) = tmp;
    }
        }
    }
L_800C71B4:
    r3 = r31;
    r4 = (u32)sp + 0x8;
    r5 = (u32)sp + 0x34;
    fn_800C71DC();

    return;
}

/* fn_800C71DC - 0x800C71DC | size: 0x278 */
void fn_800C71DC(void) {
    u8 sp[0x70];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r10 = 0;
    u32 r11 = 0;
    u32 r12 = 0;
    u32 r27 = 0;
    u32 r28 = 0;
    u32 r29 = 0;
    u32 r30 = 0;
    u32 r31 = 0;
    void (*ctr_fn)(void) = 0;
    u32 ctr = 0;

    r7 = 0xCCCD0000;
    r12 = *(u8*)((u8*)r5 + 0x4);
    tmp = 0x0;
    r8 = (u32)sp + 0x8;
    r31 = *(u8*)((u8*)r4 + 0x4);
    r30 = 0x0;
    r29 = r31 + r29;
    r6 = r29 + 0x1;
    r6 = r8 + r6;
    *(u8*)((u8*)r3 + 0x0) = tmp;
    tmp = r6;
    while (1) {
        if ((s32)r29 <= 0) break;
        r7 = r29 - r8;
        /* subic. r7, r7, 0x1 */;
        if ((s32)tmp < 0) {
            r7 = 0x0;
        }
        r10 = r8 + 0x1;
        r11 = r31 - r7;
        r28 = r7 + 0x5;
        r27 = r8 + 0x5;
        r28 = r4 + r28;
        r27 = r5 + r27;
        if ((s32)r10 > (s32)r11) {
            r10 = r11;
        }
        r8 = r10;
        do {
            if ((s32)r10 <= 0) break;
            /* srwi. r7, r10, 3 */;
            ctr_fn = (void(*)(void))r7;
            if ((s32)r10 != 0) {
                do {
                    r11 = *(u8*)((u8*)r28 + 0x0);
                    r10 = *(u8*)((u8*)r27 + 0x0);
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x1);
                    r10 = *(u8*)((u8*)r27 + (-1));
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x2);
                    r10 = *(u8*)((u8*)r27 + (-2));
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x3);
                    r10 = *(u8*)((u8*)r27 + (-3));
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x4);
                    r10 = *(u8*)((u8*)r27 + (-4));
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x5);
                    r10 = *(u8*)((u8*)r27 + (-5));
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x6);
                    r10 = *(u8*)((u8*)r27 + (-6));
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r11 = *(u8*)((u8*)r28 + 0x7);
                    r10 = *(u8*)((u8*)r27 + (-7));
                    r28 = r28 + 0x8;
                    r30 = r30 + r7;
                    r7 = r11 * r10;
                    r30 = r30 + r7;
                } while (--ctr != 0);
                r8 = r8 & 0x7;
                if ((s32)r10 == 0) break;
            }
            ctr_fn = (void(*)(void))r8;
            do {
                r11 = *(u8*)((u8*)r28 + 0x0);
                r28 = r28 + 0x1;
                r10 = *(u8*)((u8*)r27 + 0x0);
                r7 = r11 * r10;
                r30 = r30 + r7;
            } while (--ctr != 0);
        } while (0);

        r8 = (u32)((u64)r9 * (u64)r30 >> 32);
        r7 = r8;
        r8 = (u32)r8 >> 3;
        r8 = r8 * 0xa;
        r8 = r30 - r8;
        r30 = (u32)r7 >> 3;
        r6 += -1; *(u8*)r6 = r8;

    }
    r7 = *(s16*)((u8*)r4 + 0x2);
    r4 = *(s16*)((u8*)r5 + 0x2);
    r4 = r7 + r4;
    *(u16*)((u8*)r3 + 0x2) = r4;
    if (r30 != 0) {
        r6 += -1; *(u8*)r6 = r30;
        r4 = *(s16*)((u8*)r3 + 0x2);
        r4 = r4 + 0x1;
        *(u16*)((u8*)r3 + 0x2) = r4;
    }
    r7 = 0x0;
    while ((s32)r7 < 0x24 && r6 < tmp) {

        r5 = *(u8*)((u8*)r6 + 0x0);
        r4 = r7 + 0x5;
        r7 = r7 + 0x1;
        r6 = r6 + 0x1;
        *(u8*)(r3 + r4) = r5;

    }

    *(u8*)((u8*)r3 + 0x4) = r7;
    if (r6 >= tmp) return;
    r4 = *(u8*)((u8*)r6 + 0x0);
    if (r4 < 5) return;
    if (r4 == 5) {
        r5 = r6 + 0x1;
        r4 = tmp - r5;
        ctr_fn = (void(*)(void))r4;
        if (r5 < tmp) {
            do {
                tmp = *(u8*)((u8*)r5 + 0x0);
                if (tmp != 0) break;
                r5 = r5 + 0x1;
            } while (--ctr != 0);
        }
        tmp = *(u8*)((u8*)r6 + (-1));
        tmp = tmp & 0x1;
        if (tmp == 0) return;
    }
    r4 = *(u8*)((u8*)r3 + 0x4);
    r6 = r3 + 0x5;
    tmp = 0x0;
    r5 = r6 + r5;
    do {
        r4 = *(u8*)((u8*)r5 + 0x0);
        if (r4 < 9) {
            tmp = r4 + 0x1;
            *(u8*)((u8*)r5 + 0x0) = tmp;
            return;
        }
        if (r5 == r6) {
            tmp = 0x1;
            *(u8*)((u8*)r5 + 0x0) = tmp;
            r4 = *(s16*)((u8*)r3 + 0x2);
            tmp = r4 + 0x1;
            *(u16*)((u8*)r3 + 0x2) = tmp;
            return;
        }
        *(u8*)((u8*)r5 + 0x0) = tmp;
    } while (1);

    return;
}

/* fn_800C7558 - 0x800C7558 | size: 0x24 */
s32 fn_800C7558(s32 ch) {
    extern u8 lbl_80313C18[];

    if (ch == -1) {
        return -1;
    }
    return lbl_80313C18[(u8)ch];
}
