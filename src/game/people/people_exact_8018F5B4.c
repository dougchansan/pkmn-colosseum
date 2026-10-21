#include "game/people/people.h"

extern f32 lbl_8047D8A8;
extern f32 lbl_8047D8AC;

f32 fn_8018F5B4(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return info->field_14;
    }
    return lbl_8047D8A8;
}

f32 fn_8018F5CC(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return info->field_10;
    }
    return lbl_8047D8A8;
}

f32 charModelGetRadius(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return info->field_18;
    }
    return lbl_8047D8A8;
}

u8 fn_8018F5FC(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return (info->flags >> 2) & 3;
    }
    return 0;
}

f32 charModelGetHeadPitchMax(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return lbl_8047D8AC * info->field_20;
    }
    return lbl_8047D8A8;
}

f32 charModelGetHeadPitchMin(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return lbl_8047D8AC * info->field_1C;
    }
    return lbl_8047D8A8;
}

f32 charModelGetHeadYawMax(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return lbl_8047D8AC * info->field_20;
    }
    return lbl_8047D8A8;
}

f32 charModelGetHeadYawMin(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return lbl_8047D8AC * info->field_1C;
    }
    return lbl_8047D8A8;
}

s8 charModelGetHeadPart(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return (s8)info->raw_09;
    }
    return -1;
}

void* fn_8018F6B4(const PeopleInfoBiosEntry* info)
{
    if (info != NULL) {
        return info->scriptRef;
    }
    return NULL;
}

extern u32* lbl_80478E78;
extern PeopleInfoBiosEntry* lbl_80478E7C;

PeopleInfoBiosEntry* peopleInfoBiosGetPtrFromIndex(u32 index)
{
    if (index >= *lbl_80478E78) {
        return NULL;
    }
    return &lbl_80478E7C[index];
}

PeopleInfoBiosEntry* peopleInfoBiosGetPtr(void* scriptObject)
{
    u32 count;
    PeopleInfoBiosEntry* entry;

    count = *lbl_80478E78;
    entry = lbl_80478E7C;
    while (count != 0) {
        if (entry->scriptRef == scriptObject) {
            return entry;
        }
        entry++;
        count--;
    }
    return NULL;
}
