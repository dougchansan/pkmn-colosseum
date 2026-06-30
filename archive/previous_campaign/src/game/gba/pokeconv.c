/**
 * @file pokeconv.c
 * @brief Pokemon data conversion between GBA and GCN formats.
 *
 * This file corresponds to the original pokeconv.c source file,
 * confirmed by the string "pokeconv.c" at lbl_8026F568 in rodata.
 *
 * Address range: 0x80089048 - 0x800895A4 (2 functions)
 *
 * The GBA and GCN use different byte orders (GBA = little-endian,
 * GCN = big-endian), so all multi-byte fields must be byte-swapped
 * during transfer. The conversion functions handle this swapping
 * for Pokemon data structures.
 *
 * Assert string at lbl_8026F574:
 *   "cp->pc_items_num == 50 || cp->pc_items_num == 30"
 *
 * This assert validates the PC item count field in the GBA save data:
 *   50 = Ruby/Sapphire/Emerald (standard PC item capacity)
 *   30 = FireRed/LeafGreen (reduced PC item capacity)
 *
 * The byte-swapping pattern visible in the disassembly:
 *   rlwinm rN, rX, 0, 16, 23   ; extract byte 1 (mask 0x0000FF00)
 *   rlwinm rN, rX, 0, 8, 15    ; extract byte 2 (mask 0x00FF0000)
 *   slwi   rN, rX, 24          ; shift byte 0 to byte 3
 *   slwi   rN, rN, 8           ; shift byte 1 to byte 2
 *   srwi   rN, rN, 8           ; shift byte 2 to byte 1
 *   srwi   rN, rX, 24          ; shift byte 3 to byte 0
 *   or     rN, rN, rN          ; combine bytes
 *
 * This is equivalent to: result = __builtin_bswap32(input)
 *
 * Additional functions called:
 *   fn_80123FBC - Pokemon validity check (returns bool)
 *   fn_8011F5C8 - Get Pokemon species from save context
 *   fn_8008AE18 - Extract Pokemon supplementary data
 *   fn_80265F14 - Get ribbon value by index
 */

#include "dolphin/types.h"
#include "game/gba/gba.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* memset(void* dst, int val, u32 size);

/* Assert function (__assert) */
extern void __assert(const char* file, u32 line, const char* msg);

/* Pokemon validation (fn_80123FBC) */
extern u8  fn_80123FBC(void* pSaveCtx);

/* Get species from save context (fn_8011F5C8) */
extern u16 fn_8011F5C8(void* pSaveCtx);

/* Extract supplementary Pokemon data (fn_8008AE18) */
extern void fn_8008AE18(void* pSaveCtx, void* pDst);

/* Get ribbon value by index (fn_80265F14) */
extern u8  fn_80265F14(s32 ribbonIdx);

/* =========================================================================
 * File-scope strings (rodata)
 * ========================================================================= */

static const char sFile[] = "pokeconv.c";
static const char sAssertItemCount[] =
    "cp->pc_items_num == 50 || cp->pc_items_num == 30";

/* =========================================================================
 * Helper: 32-bit byte swap (little-endian <-> big-endian)
 *
 * Matches the PPC instruction pattern in the disassembly:
 *   rlwinm rN, rX, 0, 16, 23   ; (val >> 0) & 0x0000FF00
 *   rlwinm rN, rX, 0, 8, 15    ; (val >> 0) & 0x00FF0000
 *   slwi   rN, rX, 24          ; val << 24
 *   slwi   rN, rN, 8           ; (byte1) << 8
 *   srwi   rN, rN, 8           ; (byte2) >> 8
 *   srwi   rN, rX, 24          ; val >> 24
 *   or     ...                  ; combine all bytes
 *
 * The compiler generates this from a straightforward byte-swap.
 * ========================================================================= */
static u32 bswap32(u32 val) {
    return ((val & 0x000000FFu) << 24) |
           ((val & 0x0000FF00u) <<  8) |
           ((val & 0x00FF0000u) >>  8) |
           ((val & 0xFF000000u) >> 24);
}

/* =========================================================================
 * fn_80089048: pokeconv_ConvertPokemon
 * Address: 0x80089048, Size: 0x338
 *
 * Convert a Pokemon from GBA format to GCN format.
 *
 * Parameters (from register usage):
 *   r3 = pDst     (destination buffer in GCN format)
 *   r4 = pSrc     (source GBA Pokemon data)
 *   r5 = pSaveCtx (save context for ribbon extraction, may be NULL)
 *
 * The function performs:
 *   1. If pSaveCtx != NULL, validate the Pokemon via fn_80123FBC
 *      - If invalid, return 0
 *   2. Load 3 words from pSrc (PID, OTID, extra data)
 *   3. Byte-swap each word and store to pDst
 *   4. If pSaveCtx != NULL, get species via fn_8011F5C8
 *      and merge into the extra data word
 *   5. Byte-swap and store the 4th word (includes PC item count)
 *   6. Validate PC item count: must be 50 or 30
 *      - Assert at line 0xB7
 *   7. Convert remaining data words in batches of 4 (unrolled loop)
 *      - Each iteration reads 4 halfword pairs (lhz at +0x10/+0x12),
 *        combines them into 32-bit words, byte-swaps, and stores
 *   8. If pSaveCtx != NULL:
 *      - Call fn_8008AE18 to extract supplementary data
 *      - Zero 12 bytes at dst + 0x64 (memset)
 *      - Fill 11 ribbon bytes from fn_80265F14
 *   9. Return 1
 *
 * The unrolled conversion loop (lines 152944-153110 in disasm) processes
 * 4 items per iteration: each item is loaded as two 16-bit halves
 * (lhz at +0x10 and +0x12), combined into a 32-bit word, byte-swapped,
 * and stored. The source pointer advances by 4 bytes per item.
 *
 * Decompiled from PPC disassembly.
 * ========================================================================= */
s32 pokeconv_ConvertPokemon(void* pDst, void* pSrc, void* pSaveCtx) {
    u32* dst = (u32*)pDst;
    u16* src16 = (u16*)pSrc;
    u32* src32 = (u32*)pSrc;
    u32 word0, word1, word2, word3;
    u16 itemCount;
    u32* outPtr;
    s32 count;
    s32 i;
    s32 remainder;

    /* Step 1: Validate Pokemon if save context provided */
    if (pSaveCtx != NULL) {
        u8 valid = fn_80123FBC(pSaveCtx);
        if ((valid & 0xFF) == 0) {
            return 0; /* Invalid Pokemon */
        }
    }

    /* Step 2-3: Load and byte-swap first 3 words (PID, OTID, extra) */
    word0 = src32[0];
    word1 = src32[1];
    word2 = src32[2];

    dst[0] = bswap32(word0);
    dst[1] = bswap32(word1);
    dst[2] = bswap32(word2);

    /* Step 4: Handle species from save context */
    word3 = *(u16*)(((u8*)pSrc) + 0x0E); /* lhz r31, 0xe(r29) */

    if (pSaveCtx != NULL) {
        u16 species = fn_8011F5C8(pSaveCtx);
        word3 = (word3 & 0xFFFF) | ((u32)species << 16);
    }

    /* Step 5: Byte-swap and store 4th word */
    dst[3] = bswap32(word3);

    /* Step 6: Validate PC item count */
    itemCount = *(u16*)(((u8*)pSrc) + 0x0E);
    {
        BOOL valid = FALSE;
        if (itemCount == GBA_PC_ITEMS_50) {
            valid = TRUE;
        } else if (itemCount == GBA_PC_ITEMS_30) {
            valid = TRUE;
        }
        if (valid == 0) {
            /* Assert at line 0xB7 in pokeconv.c */
            __assert(sFile, 0xB7, sAssertItemCount);
        }
    }

    /* Step 7: Convert remaining data items */
    outPtr = dst + 4; /* Start after the 4 header words (offset 0x10) */
    count = (s32)itemCount;

    if (count > 0) {
        u16* srcPtr = (u16*)pSrc;
        s32 fullIters = count >> 2; /* count / 4 */

        /* Unrolled loop: 4 items per iteration */
        if (fullIters > 0) {
            for (i = 0; i < fullIters; i++) {
                u32 val;

                /* Item 0 */
                val = ((u32)srcPtr[9] << 16) | srcPtr[8]; /* +0x12, +0x10 */
                srcPtr += 2;
                *outPtr++ = bswap32(val);

                /* Item 1 */
                val = ((u32)srcPtr[9] << 16) | srcPtr[8];
                srcPtr += 2;
                *outPtr++ = bswap32(val);

                /* Item 2 */
                val = ((u32)srcPtr[9] << 16) | srcPtr[8];
                srcPtr += 2;
                *outPtr++ = bswap32(val);

                /* Item 3 */
                val = ((u32)srcPtr[9] << 16) | srcPtr[8];
                srcPtr += 2;
                *outPtr++ = bswap32(val);
            }

            /* Handle remainder */
            remainder = count & 3;
        } else {
            remainder = count;
            srcPtr = (u16*)pSrc;
        }

        /* Process remaining items one at a time */
        for (i = 0; i < remainder; i++) {
            u32 val = ((u32)srcPtr[9] << 16) | srcPtr[8];
            srcPtr += 2;
            *outPtr++ = bswap32(val);
        }
    }

    /* Step 8: Extract supplementary data and ribbons if save context provided */
    if (pSaveCtx != NULL) {
        u8* ribbonDst;

        /* Extract supplementary Pokemon data */
        fn_8008AE18(pSaveCtx, outPtr);

        /* Clear 12 bytes at offset 0x64 from outPtr */
        ribbonDst = ((u8*)outPtr) + 0x64;
        memset(ribbonDst, 0, 0x0C);

        /* Fill 11 ribbon bytes */
        {
            u8* rPtr = ribbonDst;
            s32 ribbonIdx;

            for (ribbonIdx = 0; ribbonIdx < 11; ribbonIdx++) {
                *rPtr = fn_80265F14(ribbonIdx);
                rPtr++;
            }
        }
    }

    /* Step 9: Return success */
    return 1;
}

/* =========================================================================
 * fn_80089380: pokeconv_ValidateItems
 * Address: 0x80089380, Size: 0x224
 *
 * Convert a Pokemon from GCN format back to GBA format.
 * Reverse byte-swap operation: big-endian -> little-endian.
 *
 * Parameters (from register usage):
 *   r3 = pDst  (destination buffer in GBA format)
 *   r4 = pSrc  (source GCN-format Pokemon data)
 *
 * The function performs:
 *   1. Load 4 words from pSrc
 *   2. Byte-swap each word (GCN big-endian -> GBA little-endian)
 *   3. Store byte-swapped words to pDst
 *   4. Validate PC item count from the converted 4th word:
 *      - must be 50 or 30
 *      - Assert at line 0x6F
 *   5. Convert remaining data in batches (same unrolled pattern)
 *   6. Write converted halfwords back to destination
 *
 * The output format uses paired halfwords (sth at +0x0C/+0x0E)
 * for the header, then paired halfwords for each data item.
 *
 * Decompiled from PPC disassembly.
 * ========================================================================= */
void pokeconv_ValidateItems(void* pDst, void* pSrc) {
    u32* src32 = (u32*)pSrc;
    u8*  dst = (u8*)pDst;
    u32  word0, word1, word2, word3;
    u32  swapped;
    u16  itemCount;
    s32  count;
    s32  i;
    s32  fullIters;
    s32  remainder;
    u32* srcPtr;
    BOOL validCount;

    /* Load and byte-swap first 4 words (GCN -> GBA) */
    word0 = bswap32(src32[0]);
    word1 = bswap32(src32[1]);
    word2 = bswap32(src32[2]);
    word3 = bswap32(src32[3]);

    /* Store to destination */
    *(u32*)(dst + 0x00) = word0;
    *(u32*)(dst + 0x04) = word1;
    *(u32*)(dst + 0x08) = word2;

    /* Store 4th word as two halfwords */
    *(u16*)(dst + 0x0C) = (u16)(word3 >> 16);
    *(u16*)(dst + 0x0E) = (u16)(word3 & 0xFFFF);

    /* Validate PC item count */
    itemCount = *(u16*)(dst + 0x0E);
    validCount = FALSE;
    if (itemCount == GBA_PC_ITEMS_50) {
        validCount = TRUE;
    } else if (itemCount == GBA_PC_ITEMS_30) {
        validCount = TRUE;
    }
    if (validCount == 0) {
        /* Assert at line 0x6F in pokeconv.c */
        __assert(sFile, 0x6F, sAssertItemCount);
    }

    /* Convert remaining data items */
    count = (s32)itemCount;
    srcPtr = src32 + 4; /* Start after header (offset 0x10) */

    if (count <= 0) {
        return;
    }

    fullIters = count >> 2; /* count / 4 */

    /* Unrolled loop: 4 items per iteration */
    if (fullIters > 0) {
        u8* dstItem = dst + 0x10; /* First data item destination */

        for (i = 0; i < fullIters; i++) {
            u32 val;

            /* Item 0 */
            val = bswap32(*srcPtr++);
            *(u16*)(dstItem + 0x00) = (u16)(val & 0xFFFF);
            *(u16*)(dstItem + 0x02) = (u16)(val >> 16);
            dstItem += 4;

            /* Item 1 */
            val = bswap32(*srcPtr++);
            *(u16*)(dstItem + 0x00) = (u16)(val & 0xFFFF);
            *(u16*)(dstItem + 0x02) = (u16)(val >> 16);
            dstItem += 4;

            /* Item 2 */
            val = bswap32(*srcPtr++);
            *(u16*)(dstItem + 0x00) = (u16)(val & 0xFFFF);
            *(u16*)(dstItem + 0x02) = (u16)(val >> 16);
            dstItem += 4;

            /* Item 3 */
            val = bswap32(*srcPtr++);
            *(u16*)(dstItem + 0x00) = (u16)(val & 0xFFFF);
            *(u16*)(dstItem + 0x02) = (u16)(val >> 16);
            dstItem += 4;
        }

        remainder = count & 3;
        if (remainder == 0) {
            return;
        }

        /* Process remaining items */
        for (i = 0; i < remainder; i++) {
            u32 val = bswap32(*srcPtr++);
            *(u16*)(dstItem + 0x00) = (u16)(val & 0xFFFF);
            *(u16*)(dstItem + 0x02) = (u16)(val >> 16);
            dstItem += 4;
        }
    } else {
        /* Less than 4 items total */
        u8* dstItem = dst + 0x10;
        for (i = 0; i < count; i++) {
            u32 val = bswap32(*srcPtr++);
            *(u16*)(dstItem + 0x00) = (u16)(val & 0xFFFF);
            *(u16*)(dstItem + 0x02) = (u16)(val >> 16);
            dstItem += 4;
        }
    }
}
