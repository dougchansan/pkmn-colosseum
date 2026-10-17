#include "dolphin/types.h"

/* Uninitialized; MWCC emits .bss/.sbss in reverse definition order. */
u8 DBVerbose[0x4];
u8 __DBInterface[0x4];
