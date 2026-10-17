/**
 * @file fsys_file_exact_8017BC90.c
 * @brief fsysLZSSDecodeJob: LZSS-decode a job's compressed entry image into the
 *        entry's buffer (0x8017BC90 - 0x8017BD34).
 *
 * The decode step of the asynchronous entry-load job: fn_8018114C (the
 * GSgapp task that fn_8018094C starts for a job of type 0) calls it with the
 * job's slot, the entry's name hash, the compressed image fsysLZSSJobSetup moved
 * aside, and the entry's runtime trailer. It returns the output buffer, or
 * NULL when the trailer has none.
 *
 * Built at optimisation level 0 with peephole and scheduling on, like the
 * rest of the fsys code; exact (41/41 instructions and relocations) with the
 * unit-wide `-opt level=0` and no local pragmas.
 *
 * The decode is fsysDecodeLZSS (with fsysLZSSInit), the helper the
 * synchronous loader fsysLoadEntrySync expands for the same step
 * (fsys_file_candidate_8017E30C_o2.c); the bodies here are the same text.
 * `out` is this function's own local, so the helper's destination parameter
 * is substituted, not copied (retail passes r30 straight to fn_8017F2C4 and
 * DCFlushRange). Retail's saved-but-unused r29 is the helper's `decoded`
 * flag, whose two stores leave no instruction, as in fsysLoadEntrySync; the
 * trailer parameter's r28 is folded into the lwz 0x4(r6) by the peephole
 * pass.
 */
#include "dolphin/types.h"
#include "game/fsys/fsys_entry.h"

typedef struct FSYSLZSSHeader {
    u32 magic;
    u32 decompSize;
    u32 compSize;
    u32 field_0C;
} FSYSLZSSHeader;

extern FSYSLZSSHeader lbl_80453FDC; /* LZSS header of the entry being decoded */
extern u8 lbl_80452FC8[];           /* LZSS sliding window */

extern void* memcpy(void* dst, const void* src, u32 n);
extern void DCFlushRange(void* addr, u32 nBytes);
extern void fn_8017F2C4(void* dst, const void* src, u32 size);

/* Same expansion as fsysLZSSInit in fsys_file_candidate_8017E30C_o2.c. */
static inline void fsysLZSSInit(void* src)
{
    s32 i;

    memcpy(&lbl_80453FDC, src, 0x10);
    for (i = 0; i < 0xFEE; i++) {
        lbl_80452FC8[i] = 0;
    }
}

/* Same expansion as fsysDecodeLZSS in fsys_file_candidate_8017E30C_o2.c. */
static inline void fsysDecodeLZSS(void* dst, void* src)
{
    u8 decoded;

    decoded = 0;
    fsysLZSSInit(src);
    fn_8017F2C4(dst, src, lbl_80453FDC.decompSize);
    DCFlushRange(dst, lbl_80453FDC.decompSize);
    decoded = 1;
}

/* Address: 0x8017BC90 | size: 0xA4 */
void* fsysLZSSDecodeJob(FSYSSlot* slot, u32 nameHash, void* compressed, FSYSSubEntry* sub)
{
    void* out;

    out = sub->buffer;
    if (out) {
        fsysDecodeLZSS(out, compressed);
    }
    return out;
}
