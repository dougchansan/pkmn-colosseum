/**
 * @file gs_gfx_exact_800D4F98.c
 * @brief GSgfx request buffer: command recorder and allocator,
 *        0x800D4F98 - 0x800D55D0.
 *
 * While the GSgfx command mode (state+0x000) is 1, the GS state setters
 * record their calls here instead of touching GX: fn_800D4F98 appends the
 * command id and its arguments, laid out by a format code, at the request
 * write pointer.  gfxAllocRequestBuffer allocates that buffer (GSgfxInit passes
 * 0x6DDD0 bytes).
 *
 * The unit owns fn_800D4F98's switch jump table (.data 0x803142F8) and the
 * three request-buffer messages (.rodata 0x802703C0).  MWCC addresses the
 * messages through one string-pool base register; GC/1.3.2 keeps the first
 * one as `addi r3,r31,0` where GC/1.3 emits `mr`.
 */
#include "dolphin/types.h"
#include "dolphin/gx/GX.h"
#include "crt/stdarg.h"

extern void GSlogWrite(const char*, ...);
extern void* memcpy(void* dst, const void* src, u32 n);
extern u16 _toolentryAlloc__FUl(u32 size); /* GSmemAllocRaw */
extern void* GSmemLock(u16 handle);      /* GSmemGetPtr */
extern u8 lbl_804001F0[];                  /* per-frame timing counters */

typedef struct GSgfxState {
    u8 pad_000[0x48C];
    u16 requestBufferHandle;
    u16 pad_48E;
    u32* requestBuffer;
    u32* requestBufferPos;
    u32 requestBufferSize;
} GSgfxState;

extern GSgfxState* lbl_8047AA80;

/*
 * Record one GS command.  `format` selects how the variadic arguments are
 * stored: 1-10 words, 11-14 floats, 15 word + 2 floats, 16 a 3x4 matrix,
 * 17 word + matrix, 18 word + 0x18 bytes (0x60-byte slot) + word,
 * 19 three words + matrix, 20 word + GXColor.
 */
void gfxRecordCommand(u32 command, u32 format, ...)
{
    va_list ap;
    f32 value;
    GXColor color;

    *lbl_8047AA80->requestBufferPos++ = command;
    va_start(ap, format);

    switch (format) {
    case 10:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 9:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 8:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 7:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 6:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 5:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 4:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 3:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 2:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
    case 1:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        break;
    case 14:
        value = va_arg(ap, f64);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&value;
    case 13:
        value = va_arg(ap, f64);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&value;
    case 12:
        value = va_arg(ap, f64);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&value;
    case 11:
        value = va_arg(ap, f64);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&value;
        break;
    case 15:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        value = va_arg(ap, f64);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&value;
        value = va_arg(ap, f64);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&value;
        break;
    case 16:
        memcpy(lbl_8047AA80->requestBufferPos, va_arg(ap, void*), 0x30);
        lbl_8047AA80->requestBufferPos += 12;
        break;
    case 17:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        memcpy(lbl_8047AA80->requestBufferPos, va_arg(ap, void*), 0x30);
        lbl_8047AA80->requestBufferPos += 12;
        break;
    case 18:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        memcpy(lbl_8047AA80->requestBufferPos, va_arg(ap, void*), 0x18);
        lbl_8047AA80->requestBufferPos += 24;
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        break;
    case 19:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        memcpy(lbl_8047AA80->requestBufferPos, va_arg(ap, void*), 0x30);
        lbl_8047AA80->requestBufferPos += 12;
        break;
    case 20:
        *lbl_8047AA80->requestBufferPos++ = va_arg(ap, u32);
        color = va_arg(ap, GXColor);
        *lbl_8047AA80->requestBufferPos++ = *(u32*)&color;
        break;
    }
    va_end(ap);
}

/* Allocate the request buffer that fn_800D4F98 records into. */
void gfxAllocRequestBuffer(u32 size)
{
    if (lbl_8047AA80->requestBuffer != NULL) {
        GSlogWrite("GSgfx: request buffer already alloced\n");
        return;
    }

    lbl_8047AA80->requestBufferHandle = _toolentryAlloc__FUl(size);
    if (lbl_8047AA80->requestBufferHandle == 0) {
        GSlogWrite("GSgfx: can't alloc request buffer\n");
        return;
    }

    lbl_8047AA80->requestBuffer = GSmemLock(lbl_8047AA80->requestBufferHandle);
    lbl_8047AA80->requestBufferPos = lbl_8047AA80->requestBuffer;
    lbl_8047AA80->requestBufferSize = size;
    *(u32*)(lbl_804001F0 + 0x28) = 0;
    GSlogWrite("GSgfx: alloced request buffer of %d bytes at %08X\n",
               lbl_8047AA80->requestBufferSize, lbl_8047AA80->requestBuffer);
}
