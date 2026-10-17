#include "dolphin/types.h"
#include "game/gba/gba_conv.h"

extern void heroMoveGetHeroRot(f32* rotation);
extern void heroMoveGetHeroPos(f32* position);
extern u32 std092_getCurrentRoomID(void);
extern u32 floorGetPrevFloorID(void);
extern u32 std082_flagGetValue(s32 flag);

void menuSaveLoadStorePosition(u8* output)
{
    GbaConvPlayerMemo* memo = (GbaConvPlayerMemo*)output;
    f32 position[3];
    f32 rotation[3];

    heroMoveGetHeroPos(position);
    heroMoveGetHeroRot(rotation);
    memo->posX = position[0];
    memo->posY = position[1];
    memo->posZ = position[2];
    memo->rotX = rotation[0];
    memo->rotY = rotation[1];
    memo->rotZ = rotation[2];
    memo->unk04 = std092_getCurrentRoomID();
    memo->prevFloorId = floorGetPrevFloorID();
    memo->valid = 1;
    memo->flagAfc = std082_flagGetValue(0xafc);
    memo->flagAfd = std082_flagGetValue(0xafd);
    memo->flagB11 = std082_flagGetValue(0xb11);
    memo->flagDe1 = std082_flagGetValue(0xde1);
}
