#include "dolphin/types.h"

extern s32 lbl_803FB308[4];
extern s32 lbl_803FB318[4];
extern u16 lbl_8047A67C[4];

void gbaCommandSendWazaText(s32 param0, s32 param1)
{
    extern void cardeEncodePartyMoveText(s32, void*);
    extern void gbaCmdUpload88(s32, void*);
    u8 buf[0x780];

    cardeEncodePartyMoveText(param1, buf);
    gbaCmdUpload88(param0 - 1, buf);
}

s32 fn_8008AB8C(s32 value)
{
    return lbl_8047A67C[value - 1];
}

u8 fn_8008ABA0(s32 index)
{
    u32 result = 0;

    if (lbl_803FB318[index - 1] != 0) {
        if (lbl_803FB308[index - 1] == 0) {
            result = 1;
        }
    }
    return (u8)result;
}
