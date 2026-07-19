/**
 * @file sdk_exact_8009A92C.c
 * @brief Exact free-list insertion helper, 0x8009A92C - 0x8009A9D8.
 */

#include "dolphin/types.h"

typedef struct AlarmCallback {
    s32 unk0;
    u32 unk4;
    u32 unk8;
} AlarmCallback;

AlarmCallback* fn_8009A92C(AlarmCallback* head, AlarmCallback* blk, void* unused)
{
    AlarmCallback* prev = 0;
    AlarmCallback* node = head;

    while (node != 0) {
        if (blk <= node) {
            break;
        }
        prev = node;
        node = (AlarmCallback*)node->unk4;
    }

    blk->unk4 = (u32)node;
    blk->unk0 = (s32)prev;
    if (node != 0) {
        node->unk0 = (s32)blk;
        if ((u8*)blk + blk->unk8 == (u8*)node) {
            blk->unk8 += node->unk8;
            node = (AlarmCallback*)node->unk4;
            blk->unk4 = (u32)node;
            if (node != 0) {
                node->unk0 = (s32)blk;
            }
        }
    }

    if (prev != 0) {
        prev->unk4 = (u32)blk;
        if ((u8*)prev + prev->unk8 == (u8*)blk) {
            prev->unk8 += blk->unk8;
            prev->unk4 = (u32)node;
            if (node != 0) {
                node->unk0 = (s32)prev;
            }
        }
        return head;
    }
    return blk;
}
