#include "dolphin/types.h"

extern void GSdvdGetDriveStatus(void);

void fsysCheckDrive(void)
{
    GSdvdGetDriveStatus();
}
