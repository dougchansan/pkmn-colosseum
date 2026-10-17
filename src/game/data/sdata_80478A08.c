#include "dolphin/types.h"

extern void SPEC2_MakeStatus();
extern u8 lbl_803124B8[];
extern u8 lbl_80312510[];
extern u8 lbl_80312558[];
extern u8 lbl_803125A0[];
extern u8 lbl_803125E8[];
extern u8 lbl_803127A8[];
extern u8 lbl_803129A0[];

void* lbl_80478A08 = lbl_803124B8;
u32 lbl_80478A0C = 0x00000020;
u32 lbl_80478A10 = 0xF0000000;
u32 lbl_80478A14 = 0x00000300;
u32 lbl_80478A18 = 0x00000005;
void* lbl_80478A1C = SPEC2_MakeStatus;
u32 lbl_80478A20 = 0x41000000;
u32 lbl_80478A24 = 0x42000000;
void* lbl_80478A28[2] = {
    lbl_80312510, NULL,
};
void* lbl_80478A30[2] = {
    lbl_80312558, NULL,
};
void* lbl_80478A38[2] = {
    lbl_803125A0, NULL,
};
void* lbl_80478A40[2] = {
    lbl_803125E8, NULL,
};
void* lbl_80478A48[2] = {
    lbl_803127A8, NULL,
};
u32 lbl_80478A50[2] = {
    0x00000001, 0x00000000,
};
u32 lbl_80478A58[2] = {
    0xFFFF0000, 0x00000000,
};
void* __GXVersion = lbl_803129A0;
u32 lbl_80478A64[1] = { 0 }; /* array: a zero scalar would go to .bss */
u32 lbl_80478A68 = 0x00040102;
u32 lbl_80478A6C = 0x00080102;
u32 lbl_80478A70[2] = {
    0x000C0102, 0x00000000,
};
u32 lbl_80478A78[2] = {
    0x80818283, 0xA0A1A2A3,
};
u32 lbl_80478A80[2] = {
    0x84858687, 0xA4A5A6A7,
};
u32 lbl_80478A88[2] = {
    0x88898A8B, 0xA8A9AAAB,
};
u32 lbl_80478A90[2] = {
    0x8C8D8E8F, 0xACADAEAF,
};
u32 lbl_80478A98[2] = {
    0x90919293, 0xB0B1B2B3,
};
u32 lbl_80478AA0[2] = {
    0x94959697, 0xB4B5B6B7,
};
u32 lbl_80478AA8[2] = {
    0x98999A9B, 0xB8B9BABB,
};
u32 lbl_80478AB0[2] = {
    0x00040105, 0x02060000,
};
