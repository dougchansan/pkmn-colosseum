#include "dolphin/types.h"
#include "game/people/people.h"

extern u8 lbl_8047B3C8;
extern u32 fn_80113F48(void);
extern void* fn_8018E050(u32 group, s32 people_id, void* param);
extern void* peopleSearchID(void* people);
extern void GSmodelSetBoundCheck(void* model, u8 enable);

s32 fn_801CBA0C(void* param)
{
    s32 people_id;
    u32 raw_id;
    void* people;

    raw_id = lbl_8047B3C8;
    lbl_8047B3C8 = raw_id + 1;
    people_id = (s8)raw_id | 0x7FFE0000;

    people = fn_8018E050(fn_80113F48(), people_id, param);
    if (people == NULL) {
        return 0;
    }

    GSmodelSetBoundCheck(peopleGetModel(peopleSearchID(people)), 0);
    return people_id;
}
