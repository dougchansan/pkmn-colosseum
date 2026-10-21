#include "dolphin/types.h"

extern u32 lbl_80478960;
extern u32 lbl_8047A674;
extern u32 lbl_8047A670;

u32 fightGetResult80478960(void)
{
    return lbl_80478960;
}

u32 fn_800896C0(void)
{
    return lbl_8047A674;
}

u32 fn_800896C8(void)
{
    return lbl_8047A670;
}

void fn_800896D0(u32 value)
{
    lbl_80478960 = value;
}

void fn_800896D8(u32 value)
{
    lbl_8047A674 = value;
}

void fn_800896E0(u32 value)
{
    lbl_8047A670 = value;
}
