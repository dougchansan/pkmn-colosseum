#define GBA_COMMUNICATION_DECLS_ONLY
#include "src/game/gbaCommunication.c"

void _gbaCommYield(u32 a, u32 b)
{
    u32 r31;
    u32 result;

    r31 = b;
    result = OSGetCurrentThread();
    if (r31 != 0) {
        if (r31 != result) {
            return;
        }
    }
    OSSetThreadPriority((void*)result, 0x10);
    OSYieldThread();
}
