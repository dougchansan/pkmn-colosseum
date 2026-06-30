#ifndef PCPORT_FIELD_MOTION_HOST_H
#define PCPORT_FIELD_MOTION_HOST_H

#include "pcport_compat.h"

/* Installs the raw lbl_80478E7C-style table: count records, stride 0x2c,
 * character/model key at record+0xc. The table storage must outlive gameplay. */
void PCPort_FieldMotionInstallRecordTable(const void* table, u32 count);
void PCPort_FieldMotionClearRecordTable(void);

#endif /* PCPORT_FIELD_MOTION_HOST_H */
