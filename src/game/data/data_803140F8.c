#include "dolphin/types.h"

#pragma section ".data"

extern u8 fn_800D461C[];
extern u8 fn_800D4F98[];
extern u8 lbl_80312C40[];
extern u8 lbl_80312C7C[];
extern u8 lbl_80312CB8[];
extern u8 lbl_80312CF4[];
extern u8 lbl_80312D30[];
extern u8 lbl_80312D6C[];
extern u8 lbl_80312DA8[];
extern u8 lbl_80312DE4[];
extern u8 lbl_80312E20[];
extern u8 lbl_80312E5C[];
extern u8 lbl_80312E98[];
extern u8 lbl_80312ED4[];
extern u8 lbl_80312F10[];
extern u8 lbl_80312F4C[];
extern u8 lbl_80312F88[];
extern u8 lbl_80312FC4[];
extern u8 lbl_80313000[];
extern u8 lbl_8031303C[];
extern u8 lbl_80313078[];
extern u8 lbl_803130B4[];
extern u8 lbl_803130F0[];
extern u8 lbl_8031312C[];
extern u8 lbl_80313168[];
extern u8 lbl_803131A4[];
extern u8 lbl_803131E0[];
extern u8 lbl_8031321C[];
extern u8 lbl_80313258[];
extern u8 lbl_80313294[];
extern u8 lbl_803132D0[];
extern u8 lbl_8031330C[];

void* lbl_803140F8[36] = {
    (void*)((u8*)lbl_80312C40),
    (void*)((u8*)lbl_80312C7C),
    (void*)((u8*)lbl_80312CB8),
    (void*)((u8*)lbl_80312CF4),
    (void*)((u8*)lbl_80312D6C),
    (void*)((u8*)lbl_80312D30),
    (void*)((u8*)lbl_80312DA8),
    (void*)((u8*)lbl_80312DE4),
    (void*)((u8*)lbl_80312E20),
    (void*)((u8*)lbl_80313000),
    (void*)((u8*)lbl_8031303C),
    (void*)((u8*)lbl_80313078),
    (void*)((u8*)lbl_803130B4),
    (void*)((u8*)lbl_8031312C),
    (void*)((u8*)lbl_803130F0),
    (void*)((u8*)lbl_80313168),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)((u8*)lbl_803131A4),
    (void*)((u8*)lbl_803131E0),
    (void*)((u8*)lbl_8031321C),
    (void*)((u8*)lbl_80313258),
    (void*)((u8*)lbl_803132D0),
    (void*)((u8*)lbl_80313294),
    (void*)((u8*)lbl_8031330C),
    (void*)0x00000000,
    (void*)0x00000000,
    (void*)((u8*)lbl_80312E5C),
    (void*)((u8*)lbl_80312E98),
    (void*)((u8*)lbl_80312ED4),
    (void*)((u8*)lbl_80312F10),
    (void*)((u8*)lbl_80312F88),
    (void*)((u8*)lbl_80312F4C),
    (void*)((u8*)lbl_80312FC4),
    (void*)0x00000000,
    (void*)0x00000000,
};

void* jumptable_80314188[92] = {
    (void*)((u8*)fn_800D461C + 0x960),
    (void*)((u8*)fn_800D461C + 0x3C),
    (void*)((u8*)fn_800D461C + 0x4C),
    (void*)((u8*)fn_800D461C + 0x60),
    (void*)((u8*)fn_800D461C + 0x68),
    (void*)((u8*)fn_800D461C + 0x70),
    (void*)((u8*)fn_800D461C + 0x88),
    (void*)((u8*)fn_800D461C + 0xAC),
    (void*)((u8*)fn_800D461C + 0xD0),
    (void*)((u8*)fn_800D461C + 0xF4),
    (void*)((u8*)fn_800D461C + 0x118),
    (void*)((u8*)fn_800D461C + 0x12C),
    (void*)((u8*)fn_800D461C + 0x148),
    (void*)((u8*)fn_800D461C + 0x164),
    (void*)((u8*)fn_800D461C + 0x180),
    (void*)((u8*)fn_800D461C + 0x19C),
    (void*)((u8*)fn_800D461C + 0x1B0),
    (void*)((u8*)fn_800D461C + 0x1C4),
    (void*)((u8*)fn_800D461C + 0x1DC),
    (void*)((u8*)fn_800D461C + 0x200),
    (void*)((u8*)fn_800D461C + 0x224),
    (void*)((u8*)fn_800D461C + 0x238),
    (void*)((u8*)fn_800D461C + 0x24C),
    (void*)((u8*)fn_800D461C + 0x27C),
    (void*)((u8*)fn_800D461C + 0x2A4),
    (void*)((u8*)fn_800D461C + 0x2B8),
    (void*)((u8*)fn_800D461C + 0x2D0),
    (void*)((u8*)fn_800D461C + 0x2E8),
    (void*)((u8*)fn_800D461C + 0x300),
    (void*)((u8*)fn_800D461C + 0x318),
    (void*)((u8*)fn_800D461C + 0x338),
    (void*)((u8*)fn_800D461C + 0x358),
    (void*)((u8*)fn_800D461C + 0x378),
    (void*)((u8*)fn_800D461C + 0x398),
    (void*)((u8*)fn_800D461C + 0x3B0),
    (void*)((u8*)fn_800D461C + 0x3C8),
    (void*)((u8*)fn_800D461C + 0x3DC),
    (void*)((u8*)fn_800D461C + 0x3EC),
    (void*)((u8*)fn_800D461C + 0x3FC),
    (void*)((u8*)fn_800D461C + 0x410),
    (void*)((u8*)fn_800D461C + 0x430),
    (void*)((u8*)fn_800D461C + 0x440),
    (void*)((u8*)fn_800D461C + 0x450),
    (void*)((u8*)fn_800D461C + 0x460),
    (void*)((u8*)fn_800D461C + 0x478),
    (void*)((u8*)fn_800D461C + 0x488),
    (void*)((u8*)fn_800D461C + 0x4A0),
    (void*)((u8*)fn_800D461C + 0x4B8),
    (void*)((u8*)fn_800D461C + 0x4D0),
    (void*)((u8*)fn_800D461C + 0x4FC),
    (void*)((u8*)fn_800D461C + 0x50C),
    (void*)((u8*)fn_800D461C + 0x51C),
    (void*)((u8*)fn_800D461C + 0x52C),
    (void*)((u8*)fn_800D461C + 0x53C),
    (void*)((u8*)fn_800D461C + 0x54C),
    (void*)((u8*)fn_800D461C + 0x55C),
    (void*)((u8*)fn_800D461C + 0x588),
    (void*)((u8*)fn_800D461C + 0x5B4),
    (void*)((u8*)fn_800D461C + 0x5D0),
    (void*)((u8*)fn_800D461C + 0x5EC),
    (void*)((u8*)fn_800D461C + 0x5F4),
    (void*)((u8*)fn_800D461C + 0x60C),
    (void*)((u8*)fn_800D461C + 0x624),
    (void*)((u8*)fn_800D461C + 0x63C),
    (void*)((u8*)fn_800D461C + 0x64C),
    (void*)((u8*)fn_800D461C + 0x65C),
    (void*)((u8*)fn_800D461C + 0x66C),
    (void*)((u8*)fn_800D461C + 0x674),
    (void*)((u8*)fn_800D461C + 0x690),
    (void*)((u8*)fn_800D461C + 0x6A8),
    (void*)((u8*)fn_800D461C + 0x6B0),
    (void*)((u8*)fn_800D461C + 0x6C0),
    (void*)((u8*)fn_800D461C + 0x6D8),
    (void*)((u8*)fn_800D461C + 0x6F8),
    (void*)((u8*)fn_800D461C + 0x70C),
    (void*)((u8*)fn_800D461C + 0x734),
    (void*)((u8*)fn_800D461C + 0x754),
    (void*)((u8*)fn_800D461C + 0x77C),
    (void*)((u8*)fn_800D461C + 0x79C),
    (void*)((u8*)fn_800D461C + 0x7B0),
    (void*)((u8*)fn_800D461C + 0x7C4),
    (void*)((u8*)fn_800D461C + 0x7F8),
    (void*)((u8*)fn_800D461C + 0x808),
    (void*)((u8*)fn_800D461C + 0x84C),
    (void*)((u8*)fn_800D461C + 0x874),
    (void*)((u8*)fn_800D461C + 0x8C0),
    (void*)((u8*)fn_800D461C + 0x8D8),
    (void*)((u8*)fn_800D461C + 0x8F0),
    (void*)((u8*)fn_800D461C + 0x900),
    (void*)((u8*)fn_800D461C + 0x914),
    (void*)((u8*)fn_800D461C + 0x92C),
    (void*)((u8*)fn_800D461C + 0x944),
};

void* jumptable_803142F8[21] = {
    (void*)((u8*)fn_800D4F98 + 0x554),
    (void*)((u8*)fn_800D4F98 + 0x1EC),
    (void*)((u8*)fn_800D4F98 + 0x1C8),
    (void*)((u8*)fn_800D4F98 + 0x1A4),
    (void*)((u8*)fn_800D4F98 + 0x180),
    (void*)((u8*)fn_800D4F98 + 0x15C),
    (void*)((u8*)fn_800D4F98 + 0x138),
    (void*)((u8*)fn_800D4F98 + 0x114),
    (void*)((u8*)fn_800D4F98 + 0xF0),
    (void*)((u8*)fn_800D4F98 + 0xCC),
    (void*)((u8*)fn_800D4F98 + 0xA8),
    (void*)((u8*)fn_800D4F98 + 0x2A4),
    (void*)((u8*)fn_800D4F98 + 0x274),
    (void*)((u8*)fn_800D4F98 + 0x244),
    (void*)((u8*)fn_800D4F98 + 0x214),
    (void*)((u8*)fn_800D4F98 + 0x2D8),
    (void*)((u8*)fn_800D4F98 + 0x360),
    (void*)((u8*)fn_800D4F98 + 0x394),
    (void*)((u8*)fn_800D4F98 + 0x3EC),
    (void*)((u8*)fn_800D4F98 + 0x468),
    (void*)((u8*)fn_800D4F98 + 0x508),
};
