#include "dolphin/types.h"

extern u32 std093_getCurrentGroupID(void);
extern void peopleCloseByID(u32 group, u32 resource);

void std113_mapObjectUnk_8018db68(u32 resource)
{
    peopleCloseByID(std093_getCurrentGroupID(), resource);
}
