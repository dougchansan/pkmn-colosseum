#include "game/people/people.h"
#include "game/gs_model.h"

extern void GSmodelSetPosition(struct GSmodel* model,
                               struct GSvec* position);
extern void GSmodelGetPosition(struct GSmodel* model,
                               struct GSvec* position);
extern struct GSvec* GSmodelGetPositionPtr(struct GSmodel* model);

void peopleApplyPosition(PeopleEntry* entry, struct GSvec* position)
{
    GSmodelSetPosition((struct GSmodel*)entry->modelHandle, position);
}

void fn_8018FC98(PeopleEntry* entry, struct GSvec* position)
{
    GSmodelGetPosition((struct GSmodel*)entry->modelHandle, position);
}

struct GSvec* peopleGetPositionPtr(PeopleEntry* entry)
{
    return GSmodelGetPositionPtr((struct GSmodel*)entry->modelHandle);
}
