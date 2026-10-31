#include "game/people/people.h"

extern u32 lbl_8047B1F0[2];

u32 floorGetShadowLight(u32 index)
{
    if (index >= 2) {
        return 0;
    }
    return lbl_8047B1F0[index];
}

u32 peopleInfoGetFlag4(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return (info->flags >> 4) & 1;
    }
    return 0;
}

u32 characterModelGetTalkSE(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return (info->flags >> 5) & 7;
    }
    return 0;
}
