/**
 * @file fsys_decomp.c
 * @brief LZSS decompression for FSYS archive files.
 *
 * Implements the LZSS decompression algorithm used by Pokemon Colosseum's
 * FSYS archive system. This is a standard LZSS variant (sometimes called
 * LZSS-10) with the following parameters:
 *
 *   Sliding window size:  4096 bytes (0x1000)
 *   Initial write pos:    4078 (0xFEE) -- window is pre-filled with zeros
 *   Max match length:     18 (stored as length - 3 in the low nibble + 2)
 *   Flag byte:            8 chunks per flag; bit set = literal, bit clear = reference
 *   Header:               16 bytes (0x10) prepended to compressed data, skipped
 *
 * Reference encoding (2 bytes, when flag bit = 0):
 *   Byte 1: low 8 bits of window offset
 *   Byte 2: high 4 bits = high 4 bits of window offset (bits 11-8)
 *            low 4 bits = match length minus 3 (so actual length = nibble + 3)
 *
 *   Effective offset = byte1 | (byte2 >> 4 << 8)
 *   Effective length = (byte2 & 0x0F) + 3
 *
 * The compressed data stream begins at src + 0x10 (after the 16-byte header).
 * The decompression context (gLZSSContext at lbl_80453FDC) stores metadata
 * extracted from this header, including the total compressed size which
 * determines where to stop reading.
 *
 * Address: 0x8017F2C4, size 0x134
 */

#include "game/fsys/fsys.h"

/* ===================================================================
 * External globals
 * =================================================================== */

/* LZSS sliding window -- lbl_80452FC8, 0x1014 bytes in .bss */
u8 gLZSSWindow[LZSS_BUFFER_SIZE + 0x14];

/* LZSS decompression context -- lbl_80453FDC, 0x10 bytes in .bss */
FSYSDecompContext gLZSSContext;

/* ===================================================================
 * fn_8017F2C4: FSYSDecompressLZSS
 *
 * Decompresses LZSS-compressed data from an FSYS archive entry.
 *
 * Register mapping from disassembly:
 *   r3 (param) -> dst buffer pointer            => dst
 *   r4 (param) -> src compressed data pointer    => src
 *   r5 (param) -> decompressed size (from ctx)   => size (unused directly)
 *   r6         -> &gLZSSContext
 *   r20        -> total chunks processed counter
 *   r22 (r3)   -> dst (base pointer)
 *   r24        -> output position (dstPos)
 *   r25        -> flags register
 *   r28        -> current byte read
 *   r29        -> window write position (windowPos)
 *   r30 (r4)   -> src + 0x10 (skip header)
 *   r31        -> source read position (srcPos)
 *   r23        -> end of source data (compSize - 0x10)
 *
 * The function processes the compressed stream byte-by-byte:
 *
 * 1. Read flag byte. The top byte is set to 0xFF (via ori r25, r28, 0xFF00)
 *    so bits 8-15 are all set; the loop shifts right and checks bit 8
 *    to know when all 8 chunks are consumed (flag exhausted).
 *
 * 2. For each chunk (8 per flag byte):
 *    - If flag bit 0 = 1: literal byte
 *      Read one byte, write to dst[dstPos++] and window[windowPos++].
 *      windowPos wraps at 0xFFF (12-bit mask, effectively mod 4096).
 *
 *    - If flag bit 0 = 0: back-reference
 *      Read two bytes: byte1 = offset low, byte2 = offset high | length.
 *      offset = byte1 | ((byte2 & 0xF0) << 4)
 *      length = (byte2 & 0x0F) + 2  (then loop length+1 times, so +3 total)
 *      Copy 'length+1' bytes from window[offset..] to dst and window.
 *
 * 3. Repeat until srcPos exceeds the compressed data length.
 * =================================================================== */
void FSYSDecompressLZSS(void* dst, const void* src, u32 size) {
    FSYSDecompContext* ctx = &gLZSSContext;
    u8* dstBuf    = (u8*)dst;
    const u8* srcBuf = (const u8*)src;
    u32 srcPos    = 0;        /* r31: current read offset into srcBuf */
    u32 dstPos    = 0;        /* r24: current write offset into dstBuf */
    u32 windowPos = LZSS_WINDOW_SIZE; /* r29: window write position, starts at 0xFEE */
    u32 flags     = 0;        /* r25: flag register */
    u32 chunkCount = 0;       /* r20: total chunks processed */
    u32 compEnd;              /* r23: end of compressed data */

    /* Skip the 16-byte header */
    srcBuf += LZSS_HEADER_SKIP;

    /* Compute the end of compressed data (total compressed size minus header) */
    compEnd = ctx->compSize - LZSS_HEADER_SKIP;

    for (;;) {
        /* Shift flags right by 1. When bits 8+ are exhausted, read new flag. */
        flags >>= 1;

        if ((flags & 0x100) == 0) {
            /* Need a new flag byte */
            u8 flagByte;

            if (srcPos >= compEnd) {
                break; /* End of compressed data */
            }
            flagByte = srcBuf[srcPos++];

            /* Set bits 8-15 to mark 8 remaining chunks:
             * flags = flagByte | 0xFF00 */
            flags = (u32)flagByte | 0xFF00u;
        }

        if (flags & 1) {
            /* ===== Literal byte ===== */
            u8 literal;

            if (srcPos >= compEnd) {
                break;
            }
            literal = srcBuf[srcPos++];

            /* Write literal to output */
            dstBuf[dstPos++] = literal;

            /* Write literal to sliding window */
            gLZSSWindow[windowPos++] = literal;
            windowPos &= 0xFFF; /* mod 4096 */
        } else {
            /* ===== Back-reference ===== */
            u8 byte1, byte2;
            u32 offset;
            u32 length;
            u32 j;

            if (srcPos >= compEnd) {
                break;
            }
            byte1 = srcBuf[srcPos++];

            if (srcPos >= compEnd) {
                break;
            }
            byte2 = srcBuf[srcPos++];

            /* Decode offset: low 8 bits from byte1, high 4 bits from byte2 */
            offset = (u32)byte1 | (((u32)byte2 & 0xF0) << 4);

            /* Decode length: low nibble of byte2, plus 2.
             * The loop runs length+1 times (ble), so effective copy = nibble + 3. */
            length = (byte2 & 0x0F) + 2;

            /* Copy from sliding window */
            for (j = 0; j <= length; j++) {
                u32 srcWindowIdx = (offset + j) & 0xFFF;
                u8 copyByte = gLZSSWindow[srcWindowIdx];

                /* Write to output buffer */
                dstBuf[dstPos++] = copyByte;

                /* Write to sliding window at current position */
                gLZSSWindow[windowPos++] = copyByte;
                windowPos &= 0xFFF;
            }
        }

        chunkCount++;
    }
}
