/* .init 0x80003458 - 0x800034AC: the two memory helpers preceding the
   original interrupt-vector table (TRK_memset and TRK_memcpy). */
#define START_MEM_PART
#define START_MEM_PREFIX
#include "src/crt/__start.c"
