#include "game/people/people_data.h"

s32 itemParamGetPPMaxFullFlag(const ItemParamData* item)
{
    if (item == NULL) {
        return 0;
    }
    return item->ppMaxFullFlag != 0;
}
