/**
 * @file gs_floor_data_exact_800FF660.c
 * @brief Exact active-floor resource cleanup helper.
 */
#include "dolphin/types.h"
#include "game/gs_floor.h"

extern u32 floorTerminateScriptThreads(u16 handle);
extern void floorSetFadeScript(u32 a, u32 b);

extern u32 lbl_80478B18;
extern u32 lbl_8047ACB0;
extern u32 lbl_8047ACB4;
extern u32 lbl_8047ACB8;
extern u32 lbl_8047ACC4;
extern u32 lbl_8047ACC8;
extern u32 lbl_8047ACD8;
extern u32 lbl_8047ACDC;

void floorBattleEndTerminateScripts(void)
{
    GSFloorContext* currentFloor;
    GSFloorResource* resource;
    u32 remaining;

    floorSetFadeScript(0x05960009, 0x05960008);
    if (lbl_8047ACC4 != 0) {
        currentFloor = (GSFloorContext*)lbl_8047ACC8;
        if ((s32)lbl_8047ACD8 == 2) {
            resource = (GSFloorResource*)((u8*)lbl_8047ACB0 +
                (lbl_8047ACB4 * sizeof(GSFloorResource)));
            remaining = lbl_8047ACB8;
            while (remaining-- != 0) {
                if ((s32)resource->active != 0 &&
                    (s32)resource->status == 1 &&
                    resource->floorId == currentFloor->floorId) {
                    floorTerminateScriptThreads(resource->textureHandle);
                }
                resource++;
            }
            currentFloor->isActive = 5;
            lbl_80478B18 = (u32)-1;
        }
        lbl_8047ACDC = 5;
    }
}
