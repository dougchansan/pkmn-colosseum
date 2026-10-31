#include "dolphin/types.h"

extern void EnableEXI2Interrupts(void); /* EnableEXI2Interrupts */

extern u8 gTRKCPUState[];
extern u8 gTRKState[];
extern u8 lbl_803FED58[]; /* exception table base */
extern u32 __TRK_get_MSR(void);
u32 TRKTargetTranslate(u32 addr);
extern u8 gTRKInterruptVectorTable[];
extern u32 lbl_80313848[];
extern void* TRK_memcpy(void* dst, const void* src, u32 length);
extern void TRK_flush_cache(void* address, u32 length);

static inline void TRK_copy_vector(u32 offset) {
    void* destination = (void*)TRKTargetTranslate(offset);

    TRK_memcpy(destination, gTRKInterruptVectorTable + offset, 0x100);
    TRK_flush_cache(destination, 0x100);
}

extern void TRKSaveExtended1Block(void);
extern int InitMetroTRKCommTable(int hwId);
extern void TRK_main(void);
extern char _db_stack_addr[];

/* InitMetroTRK - 0x800C2D80 | size: 0x98
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_init.md */
asm void InitMetroTRK(void) {
    nofralloc
    addi r1, r1, -4
    stw r3, 0(r1)
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    stmw r0, 0(r3)
    lwz r4, 0(r1)
    addi r1, r1, 4
    stw r1, 0x4(r3)
    stw r4, 0xc(r3)
    mflr r4
    stw r4, 0x84(r3)
    stw r4, 0x80(r3)
    mfcr r4
    stw r4, 0x88(r3)
    mfmsr r4
    ori r3, r4, 0x8000
    xori r3, r3, 0x8000
    mtmsr r3
    mtsrr1 r4
    bl TRKSaveExtended1Block
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    lmw r0, 0(r3)
    li r0, 0
    mtspr 0x3f2, r0
    mtspr 0x3f5, r0
    lis r1, _db_stack_addr@h
    ori r1, r1, _db_stack_addr@l
    mr r3, r5
    bl InitMetroTRKCommTable
    cmpwi r3, 1
    bne _comm_ok
    lwz r4, 0x84(r3)
    mtlr r4
    lmw r0, 0(r3)
    blr
_comm_ok:
    b TRK_main
    blr
}

/* InitMetroTRK_BBA - 0x800C2E18 | size: 0x94
 * Hand-written MetroTRK asm; evidence: docs/asm_evidence/trk_init.md */
asm void InitMetroTRK_BBA(void) {
    nofralloc
    addi r1, r1, -4
    stw r3, 0(r1)
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    stmw r0, 0(r3)
    lwz r4, 0(r1)
    addi r1, r1, 4
    stw r1, 0x4(r3)
    stw r4, 0xc(r3)
    mflr r4
    stw r4, 0x84(r3)
    stw r4, 0x80(r3)
    mfcr r4
    stw r4, 0x88(r3)
    mfmsr r4
    ori r3, r4, 0x8000
    mtmsr r3
    mtsrr1 r4
    bl TRKSaveExtended1Block
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    lmw r0, 0(r3)
    li r0, 0
    mtspr 0x3f2, r0
    mtspr 0x3f5, r0
    lis r1, _db_stack_addr@h
    ori r1, r1, _db_stack_addr@l
    li r3, 2
    bl InitMetroTRKCommTable
    cmpwi r3, 1
    bne _comm_ok
    lwz r4, 0x84(r3)
    mtlr r4
    lmw r0, 0(r3)
    blr
_comm_ok:
    b TRK_main
    blr
}

extern u32 ARGetDMAStatus(void);
extern void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);
extern u16 __ARGetInterruptStatus(void);
extern void __ARClearInterrupt(void);


/* This unit builds with -inline auto,deferred, under which MWCC emits the C
   functions in reverse source order (asm functions stay first). They are
   therefore written here last-to-first so the object matches retail order:
   TRK__write_aram, TRK__read_aram, TRKInitializeTarget, __TRK_copy_vectors,
   TRKTargetTranslate, EnableMetroTRKInterrupts. */

/* EnableMetroTRKInterrupts - 0x800C339C | size 0x20 | scope global */
void EnableMetroTRKInterrupts(void) {
    EnableEXI2Interrupts();
}

/* TRKTargetTranslate - 0x800C3344 | size 0x58 | scope none */
u32 TRKTargetTranslate(u32 addr) {
    u32 stackBase = *(u32*)lbl_803FED58;

    if (addr >= stackBase && addr < stackBase + 0x4000) {
        u32 msrBits = *(u32*)((u8*)gTRKCPUState + 0x238) & 0x3;
        if (msrBits != 0) {
            return addr;
        }
    }

    if (addr >= 0x7E000000 && addr <= 0x80000000) {
        return addr;
    }

    return (addr & 0x3FFFFFFF) | 0x80000000;
}

void __TRK_copy_vectors(void) {
    u32 base = *(u32*)lbl_803FED58;
    u32* offset;
    s32 i;
    u32 mask;

    if (base <= 0x44 && base + 0x4000 > 0x44 &&
        (*(u32*)(gTRKCPUState + 0x238) & 3) != 0)
    {
        base = 0x44;
    } else {
        base = 0x80000044;
    }

    mask = *(u32*)base;
    offset = lbl_80313848;
    i = 0;
    do {
        if ((mask & (1 << i)) && i != 4) {
            TRK_copy_vector(offset[i]);
        }
        i++;
    } while (i <= 14);
}

s32 TRKInitializeTarget(void) {
    *(s32*)&gTRKState[0x98] = 1;
    *(u32*)&gTRKState[0x8C] = __TRK_get_MSR();
    *(u32*)lbl_803FED58 = 0xE0000000;
    return 0;
}

/* TRK__read_aram - 0x800C3098 | size: 0x134
 * Hand-written MetroTRK cache-block asm; evidence: docs/asm_evidence/trk_init.md */
void TRK__read_aram(register int c, register u32 p2, void* p3) {
    u32 err;
    int i;
    register int counter;
    u16 r;
    u32 g;
    u32 x;
    u32 size;

    if ((u32)p2 < 0x4000 || p2 + *(u32*)p3 > 0x8000000) {
        return;
    }

    x = p2 & ~0x1F;
    size = *(u32*)p3 + (p2 & 0x1F);
    size = (size + 31) & ~31;
    counter = 0;

    for (i = 0; i < size; i += 0x20) {
        asm { dcbi counter, c }
        counter += 0x20;
    }

    do {
        err = ARGetDMAStatus();
    } while (err);

    r = __ARGetInterruptStatus();
    g = 0x8000000;
    __ARClearInterrupt();

    ARStartDMA(1, c, x, size);

    while (!__ARGetInterruptStatus()) {
    }

    if (!r) {
        __ARClearInterrupt();
    }
}

/* TRK__write_aram - 0x800C2EAC | size: 0x1EC
 * Hand-written MetroTRK cache-block asm; evidence: docs/asm_evidence/trk_init.md */
void TRK__write_aram(register int c, register u32 p2, void* p3) {
    u8 buff[32] __attribute__((aligned(32)));
    u32 err;
    register int count = c;
    register u32 bf;
    u32 uVar1;
    u32 size;
    u16 r;
    register u32 g;
    register int counter;
    u32 i;

    if ((u32)p2 < 0x4000 || p2 + *(u32*)p3 > 0x8000000) {
        return;
    }

    uVar1 = p2 & ~0x1f;
    counter = 0;
    size = *(u32*)p3 + (p2 & 0x1f);
    size = (size + 31) & ~31;

    for (i = 0; i < size; i += 0x20) {
        __dcbf((void*)counter, count);
        counter += 0x20;
    }

    do {
        err = ARGetDMAStatus();
    } while (err);

    r = __ARGetInterruptStatus();
    g = 0x8000000;

    counter = p2 & 0x1f;
    if (counter) {
        g = uVar1;
        bf = (u32)buff;
        asm { dcbi r0, bf }
        __ARClearInterrupt();

        ARStartDMA(1, bf, uVar1, 0x20);

        while (!__ARGetInterruptStatus()) {
        }

        TRK_memcpy((void*)c, buff, counter);
        __dcbf((void*)c, 0);
    }

    p2 += *(u32*)p3;
    counter = p2 & 0x1f;
    if (counter) {
        u32 val = p2 & ~0x1F;
        if (val != g) {
            bf = (u32)buff;
            asm { dcbi r0, bf }
            __ARClearInterrupt();
            ARStartDMA(1, bf, val, 0x20);

            while (!__ARGetInterruptStatus()) {
            }
        }
        g = c + p2;
        TRK_memcpy((void*)g, (void*)(buff + counter), 0x20 - counter);

        __dcbf((void*)g, 0);
    }
    __sync();
    __ARClearInterrupt();
    ARStartDMA(0, c, uVar1, size);
    if (!r) {
        while (!__ARGetInterruptStatus()) {
        }

        __ARClearInterrupt();
    }
}
