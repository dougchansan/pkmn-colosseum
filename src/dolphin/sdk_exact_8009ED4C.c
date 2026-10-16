#include "dolphin/types.h"

typedef struct OSModuleInfo {
    u8 unused_00[0x1C];
    u32 version;
} OSModuleInfo;

extern BOOL Link(OSModuleInfo* module, void* bss, BOOL fixed);

BOOL OSLink(OSModuleInfo* module, void* bss)
{
    return Link(module, bss, FALSE);
}
