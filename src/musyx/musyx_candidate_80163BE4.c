#include "dolphin/types.h"

typedef struct PFNode {
    struct PFNode* next;
    u32 f4;
    u32 f8;
    u32 fc;
} PFNode;

extern PFNode lbl_80450098[];
extern PFNode* lbl_8047B060;
extern PFNode* lbl_8047B064;
extern PFNode* lbl_8047B068;
extern u32 lbl_8047B074;
extern u32 lbl_8047B078;
extern u32 lbl_8047B07C;
extern u8 lbl_8047B0A0;
extern u32 lbl_8047B08C;
extern u32 lbl_8047B090;
extern u32 lbl_8047B094;
extern u32 lbl_8047B098;
extern u32 lbl_8047B09C;
extern u32 lbl_8047B0A4;

extern void* memset(void* dest, int val, u32 len);
extern u32 OSDisableInterrupts(void);
extern u32 OSEnableInterrupts(void);
extern u32 OSGetTick(void);
extern void AIInitDMA(u8* ptr, u32 size);

void InitStreamBuffers(void)
{
    u32 i;

    lbl_8047B068 = 0;
    lbl_8047B064 = 0;
    lbl_8047B060 = &lbl_80450098[0];

    for (i = 1; i < 0x40; i++) {
        lbl_80450098[i - 1].next = &lbl_80450098[i];
    }

    lbl_80450098[i - 1].next = 0;
    lbl_8047B074 = lbl_8047B07C;
}

u8 fn_80163CA8(u32 size)
{
    PFNode* node;
    PFNode* prev;
    PFNode* best;
    u32 bestExcess;
    u32 aligned;

    aligned = (size + 0x1F) & ~0x1FU;
    node = lbl_8047B064;
    best = 0;
    prev = 0;
    bestExcess = (u32)-1;

    while (node != 0) {
        if (node->fc == aligned) {
            best = node;
            goto have_best;
        }

        if (node->fc > aligned) {
            if (bestExcess > node->fc) {
                best = node;
                bestExcess = node->fc;
            }
        }

        prev = node;
        node = node->next;
    }

have_best:
    if (best == 0) {
        PFNode* fh = lbl_8047B060;

        if (fh != 0) {
            if ((u32)(lbl_8047B074 - aligned) >= lbl_8047B078) {
                lbl_8047B060 = fh->next;
                best = fh;
                fh->fc = aligned;
                fh->f8 = aligned;
                lbl_8047B074 = lbl_8047B074 - aligned;
                fh->f4 = lbl_8047B074;
                fh->next = lbl_8047B068;
                lbl_8047B068 = fh;
            }
        }
    } else {
        if (prev != 0) {
            prev->next = best->next;
        } else {
            lbl_8047B064 = best->next;
        }

        best->f8 = aligned;
        best->next = lbl_8047B068;
        lbl_8047B068 = best;
    }

    if (best == 0) {
        return 0xFF;
    }

    return (u8)(best - lbl_80450098);
}

u32 aramGetStreamBufferAddress(u32 idx, u32* out)
{
    if (out != 0) {
        *out = lbl_80450098[(u8)idx].f8;
    }

    return lbl_80450098[(u8)idx].f4;
}

void aramFreeStreamBuffer(u32 idx)
{
    PFNode* blk;
    PFNode* cur;
    PFNode* prev;

    blk = &lbl_80450098[(u8)idx];
    cur = lbl_8047B068;
    prev = 0;

    while (cur != 0) {
        if (cur == blk) {
            if (prev != 0) {
                prev->next = blk->next;
            } else {
                lbl_8047B068 = blk->next;
            }
            break;
        }

        prev = cur;
        cur = cur->next;
    }

    if (blk->f4 == lbl_8047B074) {
        u32 mn;
        PFNode* scan;

        blk->next = lbl_8047B060;
        lbl_8047B060 = blk;

        mn = (u32)-1;
        scan = lbl_8047B068;
        while (scan != 0) {
            if (scan->f4 <= mn) {
                mn = scan->f4;
            }
            scan = scan->next;
        }

        scan = lbl_8047B064;
        while (scan != 0) {
            PFNode* nx = scan->next;

            if (scan->f4 < mn) {
                lbl_8047B064 = scan->next;
                scan->next = lbl_8047B060;
                lbl_8047B060 = scan;
            }

            scan = nx;
        }

        lbl_8047B074 = (mn != (u32)-1) ? mn : lbl_8047B07C;
        return;
    }

    blk->next = lbl_8047B064;
    lbl_8047B064 = blk;
}

void salCallback(void)
{
    int counter;
    u8* ptr;

    counter = ((int)lbl_8047B0A0 + 1) % 4;
    ptr = (u8*)(lbl_8047B09C + 0x80000000U) + (u8)counter * 0x280;
    lbl_8047B0A0 = counter;
    AIInitDMA(ptr, 0x280);
    *(volatile u32*)&lbl_8047B08C = OSGetTick();

    if (*(volatile u32*)&lbl_8047B098 != 0) {
        if (lbl_8047B090 == 0) {
            lbl_8047B090 = 1;
            OSEnableInterrupts();
            ((void (*)(void))lbl_8047B0A4)();
            OSDisableInterrupts();
            lbl_8047B090 = 0;
        }
    } else {
        lbl_8047B094 = 1;
    }
}
