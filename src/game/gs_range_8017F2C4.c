/**
 * @file gs_range_8017F2C4.c
 * @brief gs-engine code, 0x8017F2C4 - 0x8017F3F8 (1 fn): the FSYS LZSS
 *        decoder.
 *
 * Prefix of the 0x8017F2C4 - 0x80180C78 range (see
 * gs_range_8017F3F8_middle.c for the exact island that follows). Built at
 * `-opt level=0` like the rest of the range.
 *
 * This is Okumura's LZSS Decode (N = 4096, F = 18, THRESHOLD = 2) reading
 * from memory: the stream starts after a 16-byte header, the sliding window
 * is lbl_80452FC8 and lbl_80453FDC holds the header copied by the caller
 * (word 2 is the compressed size).
 *
 * The `size` argument (the caller passes the decompressed size) is never
 * read: the stream end comes from the header. At optimisation level 0 an
 * argument register that the body references stays reserved for the whole
 * function, and retail shows exactly that for r5: every scratch temporary
 * starts at r6 (lis r6 / addi r6,r6 / lwz r6,8(r6) for the header, then
 * lis r6 / add r6,r0,r29 for the window stores and lis r6 / lis r7 in the
 * copy loop), while an unreferenced `size` hands r5 to those temporaries.
 * The explicit `(void)size;` is that reference; it emits no instruction.
 */
#include "dolphin/types.h"

extern u8 lbl_80452FC8[0x1000];
extern u32 lbl_80453FDC[];

void fsysLZSSDecode(u8* destination, u8* source, u32 size)
{
    u32 inPos;
    u8* in;
    u32 r;
    u32 c;
    s32 j;
    s32 k;
    u32 flags;
    u32 outPos;
    u32 inEnd;
    u8* out;
    s32 i;
    s32 count;

    (void)size;
    out = destination;
    in = source;
    inPos = 0;
    outPos = 0;
    r = 0xFEE;
    flags = 0;
    count = 0;
    in += 0x10;
    inEnd = lbl_80453FDC[2] - 0x10;

    for (;;) {
        if (((flags >>= 1) & 0x100) == 0) {
            c = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            flags = c | 0xFF00;
        }
        if (flags & 1) {
            c = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            out[outPos++] = c;
            lbl_80452FC8[r++] = c;
            r &= 0xFFF;
        } else {
            i = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            j = in[inPos++];
            if (inPos > inEnd) {
                return;
            }
            i |= (j & 0xF0) << 4;
            j = (j & 0xF) + 2;
            for (k = 0; k <= j; k++) {
                c = lbl_80452FC8[(i + k) & 0xFFF];
                out[outPos++] = c;
                lbl_80452FC8[r++] = c;
                r &= 0xFFF;
            }
        }
        count++;
    }
}
