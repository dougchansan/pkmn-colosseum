#include "dolphin/types.h"

extern u32 lbl_80478960;
extern u32 lbl_8047A674;
extern u32 lbl_8047A670;

u32 fightGetResult80478960(void)
{
    return lbl_80478960;
}

u32 cardeGetEncounterParam8047A674(void)
{
    return lbl_8047A674;
}

u32 cardeGetEncounterParam8047A670(void)
{
    return lbl_8047A670;
}

void cardeSetEncounterParam80478960(u32 value)
{
    lbl_80478960 = value;
}

void cardeSetEncounterParam8047A674(u32 value)
{
    lbl_8047A674 = value;
}

void cardeSetEncounterParam8047A670(u32 value)
{
    lbl_8047A670 = value;
}
