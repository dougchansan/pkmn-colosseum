/**
 * @file gs_range_8010CBD0.c
 * @brief GScolsys2 floor/CCD management, 0x8010CBD0 - 0x8010D170.
 *
 * A function-boundary carve of the GScolsys2 core TU. The core owns the
 * collision state lbl_80404C68 and runs from GScolsys2GetObjEnable
 * (0x8010C7BC) through the debug draw (colsysDebugDraw), the last function
 * that addresses the state directly; everything after it goes through
 * the fn_8010CBC0 / GScolsys2GetCurFloor accessors. Built with the core
 * TU's flags (GC/1.3, project defaults), with which every core function
 * matched so far is exact; the state stays extern here.
 */
#include "dolphin/types.h"
#include "game/gs_colsys.h"

extern GSColSysState lbl_80404C68;

extern void GSgfxDLFree(void* displayList);

/* One CCD object record, 0x40 bytes; the offsets in it are relative to
 * the file head until _offsetCCD resolves them. */
typedef struct CcdOffsetBlock {
    void* field_00;
    u32 field_04;
    void* field_08;
    void* field_0C;
} CcdOffsetBlock;

typedef struct CcdOffsetRecord {
    Vec3f trans;
    Vec3f rot;
    Vec3f scale;
    CcdOffsetBlock* field_24;
    CcdOffsetBlock* field_28;
    CcdOffsetBlock* field_2C;
    CcdOffsetBlock* field_30;
    CcdOffsetBlock* field_34;
    CcdOffsetBlock* field_38;
    u16 flags;
    u8 pad_3E[2];
} CcdOffsetRecord;

typedef struct CCD_FILEHEAD {
    CcdOffsetRecord* records;
    u32 count;
} CCD_FILEHEAD;

/* 0x8010CBD0 | 0x34 */
GSColFloor* GScolsys2GetCurFloor(void)
{
    s32 layer;

    layer = lbl_80404C68.activeLayer;
    if (layer < 0 || layer >= GSCOLSYS_MAX_LAYERS) {
        return NULL;
    }
    return &lbl_80404C68.floors[layer];
}

/* 0x8010CC04 | 0x50 */
s32 GScolsys2UnloadCCD(void)
{
    lbl_80404C68.wzxDataPtr = NULL;
    if (lbl_80404C68.displayList != NULL) {
        GSgfxDLFree(lbl_80404C68.displayList);
        lbl_80404C68.displayList = NULL;
    }
    return 1;
}

/* 0x8010CC54 | 0x118 */
void GScolsys2ClearHitFlags(void)
{
    GSColFloor* floor;
    u32 i;

    floor = GScolsys2GetCurFloor();
    if (floor != NULL) {
        for (i = 0; i < 48; i++) {
            floor->events[i].flags &= 0xFFFE;
        }
    }
    lbl_80404C68.wzxDataPtr = NULL;
}

/* 0x8010CD6C | 0x98 */
void GScolsys2CopyTriangles(void)
{
    CCD_FILEHEAD* head;
    u32 i;
    CcdOffsetRecord* source;
    GSColFloorObj* destination;

    head = lbl_80404C68.wzxDataPtr;
    if (head == NULL) {
        return;
    }

    i = 0;
    source = head->records;
    destination = lbl_80404C68.floors[lbl_80404C68.activeLayer].objs;
    while (i < head->count) {
        destination->trans = source->trans;
        destination->rot = source->rot;
        destination->scale = source->scale;
        destination->flags = 0;
        i++;
        source++;
        destination++;
    }
}

/* 0x8010CE04 | 0x1E0 */
void _offsetCCD__FP12CCD_FILEHEAD(CCD_FILEHEAD* head)
{
    CcdOffsetRecord* record;
    CcdOffsetBlock* block;
    u32 i;

    if (head == NULL) {
        return;
    }
    head->records = (CcdOffsetRecord*)((u32)head->records + (u32)head);
    record = head->records;

    for (i = 0; i < head->count; i++, record++) {
        if (record->field_24 != NULL) {
            record->field_24 =
                (CcdOffsetBlock*)((u32)record->field_24 + (u32)head);
            block = record->field_24;
            if (block->field_00 != NULL) {
                block->field_00 = (void*)((u32)block->field_00 + (u32)head);
            }
            if (block->field_08 != NULL) {
                block->field_08 = (void*)((u32)block->field_08 + (u32)head);
            }
            if (block->field_0C != NULL) {
                block->field_0C = (void*)((u32)block->field_0C + (u32)head);
            }
        }
        if (record->field_28 != NULL) {
            record->field_28 =
                (CcdOffsetBlock*)((u32)record->field_28 + (u32)head);
            block = record->field_28;
            if (block->field_00 != NULL) {
                block->field_00 = (void*)((u32)block->field_00 + (u32)head);
            }
            if (block->field_08 != NULL) {
                block->field_08 = (void*)((u32)block->field_08 + (u32)head);
            }
            if (block->field_0C != NULL) {
                block->field_0C = (void*)((u32)block->field_0C + (u32)head);
            }
        }
        if (record->field_2C != NULL) {
            record->field_2C =
                (CcdOffsetBlock*)((u32)record->field_2C + (u32)head);
            block = record->field_2C;
            if (block->field_00 != NULL) {
                block->field_00 = (void*)((u32)block->field_00 + (u32)head);
            }
            if (block->field_08 != NULL) {
                block->field_08 = (void*)((u32)block->field_08 + (u32)head);
            }
            if (block->field_0C != NULL) {
                block->field_0C = (void*)((u32)block->field_0C + (u32)head);
            }
        }
        if (record->field_30 != NULL) {
            record->field_30 =
                (CcdOffsetBlock*)((u32)record->field_30 + (u32)head);
            block = record->field_30;
            if (block->field_00 != NULL) {
                block->field_00 = (void*)((u32)block->field_00 + (u32)head);
            }
        }
        if (record->field_34 != NULL) {
            record->field_34 =
                (CcdOffsetBlock*)((u32)record->field_34 + (u32)head);
            block = record->field_34;
            if (block->field_00 != NULL) {
                block->field_00 = (void*)((u32)block->field_00 + (u32)head);
            }
            if (block->field_08 != NULL) {
                block->field_08 = (void*)((u32)block->field_08 + (u32)head);
            }
            if (block->field_0C != NULL) {
                block->field_0C = (void*)((u32)block->field_0C + (u32)head);
            }
        }
        if (record->field_38 != NULL) {
            record->field_38 =
                (CcdOffsetBlock*)((u32)record->field_38 + (u32)head);
            block = record->field_38;
            if (block->field_00 != NULL) {
                block->field_00 = (void*)((u32)block->field_00 + (u32)head);
            }
        }
    }
}

/* 0x8010CFE4 | 0x54 */
s32 GScolsys2SetFloor(CCD_FILEHEAD* fileHead)
{
    if (lbl_80404C68.activeLayer < 0) {
        return 0;
    }
    _offsetCCD__FP12CCD_FILEHEAD(fileHead);
    lbl_80404C68.wzxDataPtr = fileHead;
    return 1;
}

/* 0x8010D038 | 0x2C */
s32 GScolsys2PopFloor(void)
{
    s32 layer;

    layer = lbl_80404C68.activeLayer;
    if (layer < 0) {
        return 0;
    }
    lbl_80404C68.activeLayer = layer - 1;
    return 1;
}

/* 0x8010D064 | 0x10C */
s32 GScolsys2PushFloor(void)
{
    s32 layer;
    u32 i;

    layer = lbl_80404C68.activeLayer + 1;
    if (layer >= GSCOLSYS_MAX_LAYERS) {
        return 0;
    }

    lbl_80404C68.wzxDataPtr = NULL;
    for (i = 0; i < 48; i++) {
        lbl_80404C68.floors[layer].events[i].flags &= 0xFFFE;
    }
    lbl_80404C68.activeLayer = layer;
    return 1;
}
