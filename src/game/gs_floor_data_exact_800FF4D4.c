/**
 * @file gs_floor_data_exact_800FF4D4.c
 * @brief Exact floor resource registration and state accessors.
 */
#include "dolphin/types.h"
#include "game/gs_floor.h"

extern void memcpy(void* dst, const void* src, u32 n);

extern u32 lbl_8047ACC4;
extern u32 lbl_8047ACC8;
extern u32 lbl_8047ACE0;
extern GSFloorResHandler lbl_80404918[];

void fn_800FF4D4(void* data, u8 typeId)
{
    GSFloorResHandler* handler;

    if (lbl_8047ACE0 < 0x18) {
        handler = &lbl_80404918[lbl_8047ACE0];
        handler->typeId = typeId;
        memcpy(&handler->loadFunc, data, 0xC);
        lbl_8047ACE0++;
    }
}

u32 fn_800FF52C(void)
{
    return lbl_8047ACC4 != 0;
}

u32 fn_800FF540(void)
{
    return lbl_8047ACC4;
}

u8 floorGetFadeOutFlag(void)
{
    return *(u8*)((u8*)lbl_8047ACC8 + 0xB);
}

u8 fn_800FF554(void)
{
    return *(u8*)((u8*)lbl_8047ACC8 + 0xA);
}

u32 floorGetCurrentContext(void)
{
    return *(u32*)((u8*)lbl_8047ACC8 + 0x4);
}

u32 std092_getCurrentRoomID(void)
{
    void* floorData;

    floorData = ((GSFloorContext*)lbl_8047ACC8)->floorDataEntry;
    if (floorData != NULL) {
        return *(u32*)((u8*)floorData + 0xC);
    }
    return 0;
}
