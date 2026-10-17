#include "dolphin/types.h"

extern u32 std093_getCurrentGroupID(void);
extern void* peopleIsInGroup(u32 group, u32 resource);
extern void std168_characterDetachFromCharacter(u32 group, u32 resource);
extern void* GSresGetResource(u32 group, u32 resource);
extern void GSmodelDetachFromGSpart(void* model, s32 arg);

s32 scriptCharacterDetach(u32 resource)
{
    u32 group = std093_getCurrentGroupID();
    void* model;

    if (peopleIsInGroup(group, resource) != NULL) {
        std168_characterDetachFromCharacter(group, resource);
    } else {
        model = GSresGetResource(group, resource);
        if (model == NULL) {
            return 0;
        }
        GSmodelDetachFromGSpart(model, 1);
    }
    return 1;
}
