#include "dolphin/types.h"

extern void _threadSwitch(void);
extern void fn_80018F54(s32 arg0, s32 arg1, s32 arg2);
extern s32 fn_80039498(s32 value);
extern void itemStorageTransfer(s32 mode);
extern void fn_800F915C(s32 id);
extern void menuClose(s32 id);
extern void menuCloseSync(s32 id, s32 sync);
extern void menuOpen(s32 id, s32 flag);
extern void GSresReleaseById(s32 id);
extern s32 fn_8017B2CC(s32 id);
extern void fn_8017B3E4(s32 id);

void itemStorageRun(void)
{
    s32 result;

    result = 0;
    while (1) {
        result = fn_80039498(result);
        if ((u32)(result - 3) <= 1) {
            break;
        }
        switch (result) {
        case 0:
            menuClose(0x19);
            menuClose(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1B, 1);
            itemStorageTransfer(0);
            menuOpen(0x1B, 0);
            menuOpen(0x19, 0);
            break;
        case 1:
            menuClose(0x19);
            menuClose(0x1A);
            menuClose(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1A, 1);
            menuCloseSync(0x1B, 1);
            fn_8017B3E4(0x66F);
            while (fn_8017B2CC(0x66F) == 1) {
                _threadSwitch();
            }
            fn_80018F54(4, 0, 0);
            GSresReleaseById(0x66F);
            fn_800F915C(0x66F);
            menuOpen(0x1A, 0);
            menuOpen(0x1B, 0);
            menuOpen(0x19, 0);
            break;
        case 2:
            menuClose(0x19);
            menuClose(0x1B);
            menuCloseSync(0x19, 1);
            menuCloseSync(0x1B, 1);
            itemStorageTransfer(1);
            menuOpen(0x1B, 0);
            menuOpen(0x19, 0);
            break;
        }
    }
}
