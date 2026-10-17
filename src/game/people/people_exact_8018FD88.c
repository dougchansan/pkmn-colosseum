#include "game/people/people.h"

extern void* memset(void* destination, int value, u32 size);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u32 handle);
extern s32 lbl_8047B1F8;
extern u16 lbl_8047B1FC;
extern PeopleEntry* lbl_8047B200;

PeopleEntry* peopleGetEntry(s32 index)
{
    if (index < 0 || lbl_8047B1F8 <= index) {
        return 0;
    }
    return (PeopleEntry*)((u8*)lbl_8047B200 + index * PEOPLE_ENTRY_SIZE);
}

s32 peopleGetMaxCount(void)
{
    return lbl_8047B1F8;
}

s32 peopleFree(PeopleEntry* entry)
{
    entry->active = 0;
    entry->visible = 0;
    return 1;
}

PeopleEntry* peopleInit(u32 maxPeople)
{
    u32 totalSize;

    totalSize = maxPeople * PEOPLE_ENTRY_SIZE;
    lbl_8047B200 = (PeopleEntry*)GSmemLock(
        lbl_8047B1FC = _toolentryAlloc__FUl(totalSize));
    memset(lbl_8047B200, 0, totalSize);
    lbl_8047B1F8 = (s32)maxPeople;
    return lbl_8047B200;
}
