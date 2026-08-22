#include "dolphin/types.h"

/* Address: 0x801B06DC | Size: 0x60 */
/* Shadow state setter / initializer */
void fn_801B06DC(void* arg0)
{
    extern void* fn_800E202C(void*);
    extern void fn_800E24B0(void);
    extern void fn_800E209C(void*);
    void* saved;
    void** slot;
    void* obj;

    obj = *(void**)((u8*)arg0 + 8);
    slot = *(void***)((u8*)obj + 0x58);
    if (*(void* volatile*)slot != NULL) {
        saved = fn_800E202C(*slot);
        fn_800E24B0();
        fn_800E209C(saved);
        *slot = NULL;
    }
}
