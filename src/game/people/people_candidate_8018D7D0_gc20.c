#include "game/people/people.h"

extern void GSlogWrite(const char* fmt, ...);
extern const char lbl_80273FD8[];

static inline PeopleEntry* peopleFindSelf(u32 groupId, u32 index)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) {
            continue;
        }
        if (entry->groupId != groupId) {
            continue;
        }
        if (entry->index != index) {
            continue;
        }
        return entry->selfPtr;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) {
            continue;
        }
        if (entry->index != index) {
            continue;
        }
        GSlogWrite(lbl_80273FD8, groupId, index);
        return entry->selfPtr;
    }

    return NULL;
}

static inline PeopleEntry* peopleFindBySelf(PeopleEntry* found)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) {
            continue;
        }
        if (entry->selfPtr != found) {
            continue;
        }
        return entry;
    }

    return NULL;
}

u8 fn_8018D7D0(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    return (entry->index & 0x7FFF0000) == 0x7FFF0000;
}
