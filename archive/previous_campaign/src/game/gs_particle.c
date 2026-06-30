/**
 * @file gs_particle.c
 * @brief GSparticle / GSpart -- Particle system and VI callback code.
 *
 * This module sits between GSmaterial and GStexture in the link order.
 * It contains the particle system registration, VI retrace callbacks,
 * and related utility functions.
 *
 * Address range: 0x800EE2C8 - 0x800EF098
 * ~20 functions
 */

#include "dolphin/types.h"

/* ===== External references ===== */
extern void fn_800DD970(const char* fmt, ...);

/* ==================================================================
 * fn_800EE2C8 -- GSpart_RegisterRotation | Size: 0xF4
 * r3=p (struct), r4=id (u16 from p->field_2), r5=callback, r30=userdata (r4?), r31=callback
 * Wait: r29=r3, r30=r4, r31=r5 (saved regs)
 * Actually: fn(p, userdata, callback) based on mr r29,r3 / mr r30,r4 / mr. r31,r5
 * If callback (r31) == NULL: skip, just try to store
 * Looks at p->field_4 (sub), iterate 4 slots at sub+0xe4 stride 0xc
 *   Check if slot->field_4 == p->field_2 (id). If dup: print error, return NULL.
 * Find free slot (field_4 == 0): fill with {callback, id, userdata}
 * ================================================================== */
extern u8 lbl_80270F10[];
extern u8 lbl_80270F44[];
#if 0
asm void fn_800EE2C8(void) {
#include "src/game/gs_particle_fn_800EE2C8.inc"
}
#else
void* fn_800EE2C8(void* p, void* userdata, void* callback) {
    void* sub;
    u32* slot;
    u32 id;
    void* freeSlot;
    u32 i;
    u32 tmp;

    sub = *(void**)((u8*)p + 4);
    id = *(u16*)((u8*)p + 2);

    if (callback != NULL) {
        slot = (u32*)((u8*)sub + 0xe4);
        for (i = 0; i < 4; i++) {
            if (slot[1] == id) {
                fn_800DD970((const char*)lbl_80270F10);
                return NULL;
            }
            slot = (u32*)((u8*)slot + 0xc);
        }

        /* Find free slot (field_4 == 0) */
        slot = (u32*)((u8*)sub + 0xe4);
        if (slot[0] == 0) {
            freeSlot = NULL;
        } else {
            freeSlot = (void*)slot;
            slot = (u32*)((u8*)slot + 0xc);
            tmp = slot[0];
            if (tmp == 0) {
                freeSlot = NULL;
            } else {
                freeSlot = (void*)slot;
                slot = (u32*)((u8*)slot + 0xc);
                tmp = slot[0];
                if (tmp == 0) {
                    freeSlot = NULL;
                } else {
                    freeSlot = (void*)slot;
                    slot = (u32*)((u8*)slot + 0xc);
                    tmp = slot[0];
                    if (tmp == 0) {
                        freeSlot = NULL;
                    } else {
                        fn_800DD970((const char*)lbl_80270F44);
                        freeSlot = NULL;
                    }
                }
            }
        }
    } else {
        freeSlot = NULL;
    }

    if (freeSlot != NULL) {
        *(u32*)((u8*)freeSlot + 0) = (u32)callback;
        *(u32*)((u8*)freeSlot + 4) = id;
        *(u32*)((u8*)freeSlot + 8) = (u32)userdata;
    }

    return freeSlot;
}
#endif

/* ==================================================================
 * fn_800EE3BC | Size: 0x2F8
 * Large function with stmw/lmw; uses r24-r31.
 * r3=p, r4=a, r5=b, r6=c
 * Too complex to match -- leaving as asm wrapper.
 * ================================================================== */
extern void fn_800EC0E8(u32 a);
extern u8 fn_800EC5AC(u32 a);
extern void __assert(u8* file, u32 line, u8* func);
extern void fn_8019D9DC(void* p);
extern void fn_800E01F4(void* dst, f32 x, f32 y, f32 z);
extern void fn_800E01D0(void* dst, void* src);
extern void fn_800E019C(void* dst, void* src, void* vec);
extern void fn_800E0108(void* dst, void* src, void* vec);
extern u8 lbl_8047CCA0[];
extern u8 lbl_8047CCA8[];
extern u8 lbl_80270F78[];
extern u8 lbl_804018B0[];
extern f32 lbl_8047CCB0;
extern f32 lbl_8047CCB4;
#if 1
asm void fn_800EE3BC(void) {
#include "src/game/gs_particle_fn_800EE3BC.inc"
}
#else
void fn_800EE3BC(void* p, void* a, void* b, void* c) {
    void* inner;
    u8 flags;
    u32 useGravity;
    u32 i;
    u32 count;
    void** list;
    void* slot;
    f32 vec[3];

    inner = *(void**)((u8*)p + 8);
    fn_800EC0E8(*(u32*)((u8*)p + 4));
    flags = fn_800EC5AC(*(u32*)((u8*)p + 4));

    if (flags == 0) {
        if (inner != NULL) {
            u32 innerFlags;
            u32 r3v;
            innerFlags = *(u32*)((u8*)inner + 0x14);
            r3v = 0;
            if (!(innerFlags & (1 << (31 - 8)))) {  /* bit 8 */
                if (innerFlags & (1 << (31 - 25))) { /* bit 25 */
                    r3v = 1;
                }
            }
            if (r3v) {
                fn_8019D9DC(inner);
            }
        }
        fn_800E01F4(vec, *(f32*)((u8*)inner + 0x50), *(f32*)((u8*)inner + 0x60), *(f32*)((u8*)inner + 0x70));
    } else {
        u32 innerFlags2 = *(u32*)((u8*)inner + 0x14);
        if (innerFlags2 & 0x600) {
            fn_800E01D0(vec, (u8*)inner + 0x38);
        } else {
            fn_800E01F4(vec, *(f32*)((u8*)inner + 0x50), *(f32*)((u8*)inner + 0x60), *(f32*)((u8*)inner + 0x70));
        }
    }

    if (b == NULL && c == NULL) {
        goto skip_bc;
    }

    /* Collect non-NULL inner ptrs into list at lbl_804018B0 */
    count = 0;
    list = (void**)lbl_804018B0;
    {
        void* cur = inner;
        while (cur != NULL) {
            if (count >= 0x10) {
                fn_800DD970((const char*)lbl_80270F78);
                cur = NULL;
            } else {
                list[count] = cur;
                count++;
                cur = *(void**)((u8*)cur + 0xc);
            }
        }
    }

    if (b != NULL) {
        f32 bvec[3];
        fn_800E01F4(bvec, lbl_8047CCB0, lbl_8047CCB0, lbl_8047CCB0);
        for (i = count; i > 0; i--) {
            slot = list[i - 1];
            if (slot == NULL) {
                __assert(lbl_8047CCA0, 0x2ec, lbl_8047CCA8);
            }
            {
                f32 sv[3];
                if (slot == NULL) {
                    __assert(lbl_8047CCA0, 0x2fa, lbl_8047CCA8);
                }
                sv[0] = *(f32*)((u8*)slot + 0x1c);
                if (slot == NULL) {
                    __assert(lbl_8047CCA0, 0x308, lbl_8047CCA8);
                }
                sv[1] = *(f32*)((u8*)slot + 0x20);
                sv[2] = *(f32*)((u8*)slot + 0x24);
                fn_800E019C(bvec, bvec, sv);
            }
        }
        fn_800E01D0(b, bvec);
    }

    if (c != NULL) {
        f32 cvec[3];
        fn_800E01F4(cvec, lbl_8047CCB4, lbl_8047CCB4, lbl_8047CCB4);
        for (i = count; i > 0; i--) {
            slot = list[i - 1];
            if (slot == NULL) {
                __assert(lbl_8047CCA0, 0x383, lbl_8047CCA8);
            }
            {
                f32 sv[3];
                if (slot == NULL) {
                    __assert(lbl_8047CCA0, 0x38f, lbl_8047CCA8);
                }
                sv[0] = *(f32*)((u8*)slot + 0x2c);
                if (slot == NULL) {
                    __assert(lbl_8047CCA0, 0x39b, lbl_8047CCA8);
                }
                sv[1] = *(f32*)((u8*)slot + 0x30);
                sv[2] = *(f32*)((u8*)slot + 0x34);
                fn_800E0108(cvec, cvec, sv);
            }
        }
        fn_800E01D0(c, cvec);
    }

skip_bc:
    if (a != NULL) {
        fn_800E01D0(a, vec);
    }
    if (b != NULL) {
        fn_800E01D0(b, vec + 0x2c / 4);
    }
    if (c != NULL) {
        fn_800E01D0(c, vec + 0x14 / 4);
    }
}
#endif

/* ==================================================================
 * fn_800EE6B4 | Size: 0xA4
 * r3 = outer ptr, r4 = index.
 * inner = *(void**)(r3+8); flags = *(u32*)(inner+0x14)
 * if flags & 0x4020: sub = *(void**)(inner+0x18)
 * else: sub = NULL
 * Walk linked list (field_4 = next) counting r3 from 0.
 * When r3 == r4: result = sub->field_8.
 * If result != NULL: call GSmaterialCreate(), if ret != NULL: ret->field_8 = result.
 * Return ret.
 * ================================================================== */
extern void* GSmaterialCreate(void);
#if 0
asm void fn_800EE6B4(void) {
#include "src/game/gs_particle_fn_800EE6B4.inc"
}
#else
void* fn_800EE6B4(void* outerP, u32 idx) {
    void** innerP;
    u32 flags;
    register u32 count;
    register void** sub;
    void** result;
    void** ret;

    innerP = *(void***)((u8*)outerP + 8);
    flags = *(u32*)((u8*)innerP + 0x14);

    if (((u32)__cntlzw(flags & 0x4020) >> 5) != 0) {
        sub = *(void***)((u8*)innerP + 0x18);
    } else {
        sub = NULL;
    }

    if (sub == NULL) {
        return NULL;
    }

    count = 0;
    while (sub != NULL) {
        if (count++ == idx) {
            result = (void**)*(u32*)((u8*)sub + 8);
            goto got_result;
        }
        sub = (void**)*(u32*)((u8*)sub + 4);
    }
    result = NULL;

got_result:
    if (result == NULL) {
        return NULL;
    }

    ret = (void**)GSmaterialCreate();
    if (ret != NULL) {
        *(u32*)((u8*)ret + 8) = (u32)result;
    }

    return ret;
}
#endif

/* ==================================================================
 * fn_800EE758 | Size: 0x88
 * r3 = outer ptr.
 * inner = *(void**)(r3+8); flags = *(u32*)(inner+0x14)
 * if flags & 0x4020: sub = *(void**)(inner+0x18)
 * else: sub = NULL
 * Walk linked list (field_4 = next) counting from 0 up to 0xffff.
 * Returns count of linked list entries (or -1 for overflow).
 * ================================================================== */
#if 0
asm void fn_800EE758(void) {
#include "src/game/gs_particle_fn_800EE758.inc"
}
#else
s32 fn_800EE758(void* outerP) {
    void** innerP;
    u32 flags;
    void** sub;
    u32 count;

    innerP = *(void***)((u8*)outerP + 8);
    flags = *(u32*)((u8*)innerP + 0x14);

    if (flags & 0x4020) {
        sub = *(void***)((u8*)innerP + 0x18);
    } else {
        sub = NULL;
    }

    if (sub == NULL) {
        return 0;
    }

    count = 0;
    while (sub != NULL) {
        count++;
        if ((count & 0xffff) == 0) {
            return -1;
        }
        sub = (void**)*(u32*)((u8*)sub + 4);
    }

    return (s32)count;
}
#endif

/* ==================================================================
 * fn_800EE7E0 | Size: 0x48
 * r3 = outer ptr.
 * inner = *(void**)(r3+8); flags = *(u32*)(inner+0x14)
 * if !(flags & 0x4020): sub = *(void**)(inner+0x18), return sub && sub->field_c != 0
 * else: return 0 (no active data)
 * Actually returns 1 if val==0, 0 if val!=0 after the neg/cntlzw idiom
 * ================================================================== */
#if 0
asm void fn_800EE7E0(void) {
#include "src/game/gs_particle_fn_800EE7E0.inc"
}
#else
u32 fn_800EE7E0(void* outerP) {
    void** innerP;
    u32 flags;
    void** sub;
    u32 val;

    innerP = *(void***)((u8*)outerP + 8);
    flags = *(u32*)((u8*)innerP + 0x14);

    if (((u32)__cntlzw(flags & 0x4020) >> 5) == 0) {
        val = 0;
    } else {
        sub = *(void***)((u8*)innerP + 0x18);
        if (sub == NULL) {
            val = 0;
        } else {
            val = *(u32*)((u8*)sub + 0xc);
        }
    }

    return (__cntlzw(0 - val) >> 5) & 0xFF;
}
#endif

/* ==================================================================
 * fn_800EE828 -- GSpart commit | Size: 0xC
 * li r0, 0; stb r0, 0x0(r3); blr
 * Sets byte at offset 0 of the given struct to 0.
 * ================================================================== */
#if 0
asm void fn_800EE828(void) {
#include "src/game/gs_particle_fn_800EE828.inc"
}
#else
void fn_800EE828(u8* p) {
    p[0] = 0;
}
#endif

/* ==================================================================
 * fn_800EE834 | Size: 0x4C
 * Searches lbl_8047ABBC array (stride 0xc, count = lbl_8047ABC0)
 * for the first entry whose byte[0] != 0. Returns the entry ptr
 * and marks it active (sets byte[0] = 1). Returns NULL on failure.
 * ================================================================== */
extern u32 lbl_8047ABC0;
extern u32 lbl_8047ABBC;
#if 0
asm void fn_800EE834(void) {
#include "src/game/gs_particle_fn_800EE834.inc"
}
#else
u8* fn_800EE834(void) {
    u32 count;
    u8* p;
    u32 i;

    count = lbl_8047ABC0;
    p = (u8*)lbl_8047ABBC;

    for (i = 0; i < count; i++) {
        if (p[0] != 0) {
            p[0] = 1;
            return p;
        }
        p += 0xc;
    }

    return NULL;
}
#endif

/* ==================================================================
 * fn_800EE880 -- VI callback setup | Size: 0x70
 * Allocates count * 0xc bytes via GSmemAllocRaw (fn_800E3534).
 * Stores count in lbl_8047ABC0, handle in lbl_8047ABB8.
 * Resolves pointer via fn_800E27B0, stores in lbl_8047ABBC.
 * Zeroes byte[0] of each entry (stride 0xc).
 * ================================================================== */
extern u16 fn_800E3534(u32 size);
extern void* fn_800E27B0(u32 handle);
extern u32 lbl_8047ABC0;
extern u32 lbl_8047ABB8;
extern u32 lbl_8047ABBC;
#if 0
asm void fn_800EE880(void) {
#include "src/game/gs_particle_fn_800EE880.inc"
}
#else
void fn_800EE880(u32 count) {
    u16 handle;
    u8* base;
    u32 i;
    u32 offset;

    lbl_8047ABC0 = count;
    handle = fn_800E3534(count * 0xc);
    *(u16*)&lbl_8047ABB8 = handle;

    if ((handle & 0xFFFF) == 0) {
        return;
    }

    base = (u8*)fn_800E27B0(handle & 0xFFFF);
    lbl_8047ABBC = (u32)base;

    offset = 0;
    for (i = 0; i < count; i++) {
        base[offset] = 0;
        offset += 0xc;
    }
}
#endif

/* ==================================================================
 * fn_800EE8F0 | Size: 0x38
 * If lbl_8047ABC4 == 1, return immediately.
 * If lbl_8047ABD0 == r3, set ABC4 = 1 and ABCC = ABC8.
 * Then increment ABC8.
 * ================================================================== */
extern u32 lbl_8047ABC4;
extern u32 lbl_8047ABD0;
extern u32 lbl_8047ABC8;
extern u32 lbl_8047ABCC;
#if 0
asm void fn_800EE8F0(void) {
#include "src/game/gs_particle_fn_800EE8F0.inc"
}
#else
void fn_800EE8F0(u32 val) {
    u32 curr;

    if (*(u8*)&lbl_8047ABC4 == 1) {
        return;
    }

    if (lbl_8047ABD0 == val) {
        curr = lbl_8047ABC8;
        *(u8*)&lbl_8047ABC4 = 1;
        lbl_8047ABCC = curr;
    }

    lbl_8047ABC8 = lbl_8047ABC8 + 1;
}
#endif

/* ==================================================================
 * fn_800EE928 | Size: 0x94
 * If lbl_8047ABE8 == 0, return immediately.
 * Iterates lbl_804018F0 array (32 entries, stride 8, counted down).
 * For each entry where byte[0] != 0xff and funcptr at +4 != NULL:
 *   compute r4 = lbl_8047ABE0 + (byte[0] * 512)
 *   r5 = entry->byte[1]
 *   call funcptr(1, r4, r5)
 * After loop: clear lbl_8047ABE8.
 * ================================================================== */
extern u32 lbl_8047ABE8;
extern u8 lbl_804018F0[];
extern u32 lbl_8047ABE0;
#if 0
asm void fn_800EE928(void) {
#include "src/game/gs_particle_fn_800EE928.inc"
}
#else
void fn_800EE928(void) {
    u8* entry;
    s32 i;
    void (*func)(u32, u8*, u8);

    if (*(u8*)&lbl_8047ABE8 == 0) {
        return;
    }

    entry = lbl_804018F0;
    i = 0x1f;
    while (i >= 0) {
        if (entry[0] != 0xff && *(u32*)(entry + 4) != 0) {
            func = (void (*)(u32, u8*, u8))(*(u32*)(entry + 4));
            func(1, (u8*)lbl_8047ABE0 + ((u32)entry[0] << 9), entry[1]);
        }
        entry += 8;
        i--;
    }

    *(u8*)&lbl_8047ABE8 = 0;
}
#endif

/* ==================================================================
 * fn_800EE9BC | Size: 0x94
 * If lbl_8047ABE8 == 1, return immediately.
 * Same as fn_800EE928 but calls funcptr(0, r4, r5)
 * and sets lbl_8047ABE8 = 1 at end.
 * ================================================================== */
extern u32 lbl_8047ABE8;
extern u32 lbl_8047ABE0;
#if 0
asm void fn_800EE9BC(void) {
#include "src/game/gs_particle_fn_800EE9BC.inc"
}
#else
void fn_800EE9BC(void) {
    u8* entry;
    s32 i;
    void (*func)(u32, u8*, u8);

    if (*(u8*)&lbl_8047ABE8 == 1) {
        return;
    }

    entry = lbl_804018F0;
    i = 0x1f;
    while (i >= 0) {
        if (entry[0] != 0xff && *(u32*)(entry + 4) != 0) {
            func = (void (*)(u32, u8*, u8))(*(u32*)(entry + 4));
            func(0, (u8*)lbl_8047ABE0 + ((u32)entry[0] << 9), entry[1]);
        }
        entry += 8;
        i--;
    }

    *(u8*)&lbl_8047ABE8 = 1;
}
#endif

/* ==================================================================
 * fn_800EEA50 | Size: 0x1C
 * Takes a pointer r3, aligns both r3 and lbl_8047ABE0 to 16 bytes,
 * subtracts, counts leading zeros, extracts 8 bits from position 19.
 * Effectively: returns (base - ptr) >> (extrwi 8,19) as index.
 * clrlwi = keep bottom 4 bits; subf; cntlzw; extrwi 8,19
 * ================================================================== */
extern u32 lbl_8047ABE0;
#if 0
asm void fn_800EEA50(void) {
#include "src/game/gs_particle_fn_800EEA50.inc"
}
#else
u32 fn_800EEA50(u32 ptr) {
    u32 base;
    u32 diff;
    u32 lz;

    base = lbl_8047ABE0 & 0xF0000000;
    ptr = ptr & 0xF0000000;
    diff = base - ptr;
    lz = __cntlzw(diff);
    return (lz >> 5) & 0xFF;
}
#endif

/* ==================================================================
 * fn_800EEA6C | Size: 0x2C
 * Loads lbl_8047ABDC, calls fn_8009B614 with it, then zeros lbl_8047ABDC.
 * ================================================================== */
extern void fn_8009B614(u32 handle);
extern u32 lbl_8047ABDC;
#if 0
asm void fn_800EEA6C(void) {
#include "src/game/gs_particle_fn_800EEA6C.inc"
}
#else
void fn_800EEA6C(void) {
    fn_8009B614(lbl_8047ABDC);
    lbl_8047ABDC = 0;
}
#endif

/* ==================================================================
 * GSscratchStore | Size: 0x9C
 * r3=a, r4=b, r5=c.
 * bl fn_8009B608 -- returns some count in r3
 * if count >= 15, return 1
 * if (b & 0x1f) != 0, return 2
 * if (a & 0x1f) != 0, return 2
 * if (c & 0x1f) != 0, return 2
 * LCStoreData(a, b, c)
 * lbl_8047ABDC += return_val
 * return 0
 * ================================================================== */
extern u32 fn_8009B608(void);
extern u32 LCStoreData(u32 a, u32 b, u32 c);
extern u32 lbl_8047ABDC;
#if 0
asm void GSscratchStore(void) {
#include "src/game/gs_particle_fn_800EEA98.inc"
}
#else
u32 GSscratchStore(u32 a, u32 b, u32 c) {
    u32 count;
    u32 ret;

    count = fn_8009B608();
    if (count >= 15) {
        return 1;
    }

    if ((b & 0x1f) != 0 || (a & 0x1f) != 0) {
        return 2;
    }

    if ((c & 0x1f) != 0) {
        return 2;
    }

    ret = LCStoreData(a, b, c);
    lbl_8047ABDC = lbl_8047ABDC + ret;
    return 0;
}
#endif

/* ==================================================================
 * fn_800EEB34 | Size: 0x104
 * r3 = data pointer.
 * Computes slot index = extrwi((ptr - lbl_8047ABE0), 8, 15) = (ptr-base)>>9 & 0xff
 * Searches lbl_804018F0 table (32 entries, stride 8) for entry with byte[0] == index.
 * If found:
 *   Shift mask right by entry->byte[0] positions
 *   Clear entry->byte[1] bits from lbl_8047ABEC bitmap
 *   Mark entry as free (byte[0] = 0xff)
 * ================================================================== */
extern u32 lbl_8047ABE0;
extern u32 lbl_8047ABEC;
#if 0
asm void fn_800EEB34(void) {
#include "src/game/gs_particle_fn_800EEB34.inc"
}
#else
void fn_800EEB34(u32 ptr) {
    u32 base;
    u32 idx;
    u8* table;
    u8* entry;
    u32 bitmap;
    u32 mask;
    u32 count;
    u32 i;
    u8 slot;
    u8 nbits;

    base = lbl_8047ABE0;
    idx = (u8)((ptr - base) >> 9);

    table = lbl_804018F0;
    entry = NULL;
    i = 0;
    while (i < 32) {
        if (table[i * 8] == idx) {
            entry = table + i * 8;
            break;
        }
        i++;
    }

    if (entry == NULL) {
        return;
    }

    slot = entry[0];
    nbits = entry[1];

    /* Build mask shifted by slot positions */
    mask = 0x80000000u;
    while (slot > 0) {
        mask >>= 1;
        slot--;
    }

    /* Clear nbits bits from bitmap */
    bitmap = lbl_8047ABEC;
    while (nbits > 0) {
        bitmap &= ~mask;
        mask >>= 1;
        nbits--;
    }

    lbl_8047ABEC = bitmap;
    entry[0] = 0xff;
}
#endif

/* ==================================================================
 * fn_800EEC38 -- VI register retrace callback | Size: 0x1C0
 * r3 = count (1-32), r4 = data pointer, r5 = param.
 * If lbl_8047ABE8 == 1, return NULL.
 * If count == 0 or count > 32, return NULL.
 * Check bitmap at lbl_8047ABEC for a free run of 'count' bits.
 * If found: allocate slot in lbl_804018F0 table, set bits in bitmap.
 * Returns pointer into lbl_8047ABE0 data pool.
 * ================================================================== */
extern u32 lbl_8047ABE8;
extern u32 lbl_8047ABEC;
extern u32 lbl_8047ABE0;
#if 0
asm void fn_800EEC38(void) {
#include "src/game/gs_particle_fn_800EEC38.inc"
}
#else
void* fn_800EEC38(u8 count, u32 dataPtr, u8 param) {
    u32 bitmap;
    u8 startBit;
    u8 i;
    u8* table;
    u8* entry;
    u32 mask;
    u32 bits;

    if (*(u8*)&lbl_8047ABE8 == 1) {
        return NULL;
    }

    if (count == 0 || count > 0x20) {
        return NULL;
    }

    bitmap = lbl_8047ABEC;

    /* Scan for a free run of 'count' consecutive zero bits */
    for (startBit = 0; (startBit & 0xff) < 0x20; startBit++) {
        /* Build mask for 'count' bits at startBit */
        mask = 0x80000000u;
        i = startBit;
        while (i > 0) {
            mask >>= 1;
            i--;
        }
        bits = 0;
        i = count;
        while (i > 0) {
            if (mask == 0) {
                bits |= 1;
            } else {
                if (bitmap & mask) {
                    bits |= 1;
                }
            }
            mask >>= 1;
            i--;
        }
        if (bits == 0) {
            break;
        }
        startBit++;
    }

    if ((startBit & 0xff) >= 0x20) {
        return NULL;
    }

    /* Find free slot in lbl_804018F0 table */
    table = lbl_804018F0;
    entry = NULL;
    i = 0;
    while (i < 32) {
        if (table[i * 8] == 0xff) {
            entry = table + i * 8;
            break;
        }
        i++;
    }

    if (entry == NULL) {
        return NULL;
    }

    /* Fill in the entry */
    entry[0] = startBit;
    entry[1] = count;
    *(u32*)(entry + 4) = dataPtr;

    /* Set bits in bitmap */
    mask = 0x80000000u;
    i = startBit;
    while (i > 0) {
        mask >>= 1;
        i--;
    }
    bitmap = lbl_8047ABEC;
    i = count;
    while (i > 0) {
        bitmap |= mask;
        mask >>= 1;
        i--;
    }
    lbl_8047ABEC = bitmap;

    return (void*)(lbl_8047ABE0 + ((u32)startBit << 9));
}
#endif

/* ==================================================================
 * fn_800EEDF8 -- VI init | Size: 0x150
 * ================================================================== */
void fn_800EEDF8(void) { /* TODO */ }

/* ==================================================================
 * fn_800EEF48 | Size: 0x150
 * ================================================================== */
void fn_800EEF48(void) { /* TODO */ }
