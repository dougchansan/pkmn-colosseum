#include "dolphin/types.h"

extern u8* savedataGetStatus(u8* arg0, u16 arg1);

void sodateyaInit(u8* data)
{
    u8* status;

    status = data;
    if (status == NULL) {
        status = savedataGetStatus(NULL, 0xB);
    }
    if (status != NULL) {
        data = status;
        if (status == NULL) {
            data = savedataGetStatus(NULL, 0xB);
        }
        if (data != NULL) {
            *(u16*)(data + 2) = 0;
        }
        status[1] = 0;
        status[0] = 0;
    }
}
