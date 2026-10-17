#include "hsd/hsd_object.h"

extern void* lbl_8036C638[16];
extern u32 lbl_8036CC40[];
extern const u8 lbl_80274EC8[24];
extern const u8 lbl_8047DCA0[8];
extern const u8 lbl_8047DCA8[7];
extern const u8 lbl_8047DCB0[7];

void ObjInfoInit_801AA568(void)
{
    hsdInitClassInfo(&hsdObj, (HSD_ClassInfo*) lbl_8036C638,
                     (char*) lbl_80274EC8,
                     (char*) lbl_8047DCA0, 0x3C, 8);
}

void HSD_PerfCountEnvelopeBlending(s32 index)
{
    if (index >= 32) {
        __assert((const char*) lbl_8047DCA8, 0xA4,
                 (const char*) lbl_8047DCB0);
    }
    lbl_8036CC40[index + 4] += 1;
}
