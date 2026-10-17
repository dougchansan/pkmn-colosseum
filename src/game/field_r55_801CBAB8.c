#include "dolphin/types.h"

extern void winMsgOpen(s32, s32, s32, s32);
extern s8 std011_dialogDisplayYesNoMenu(void);
extern void winMsgClose(s32);
extern s32 menuSaveLoadSaveGame(s32);
extern s32 floorRequestChange(s32);

s32 std223_askYesNoThenWarp395(void)
{
    s32 done;
    s32 state;
    s32 input;
    s32 result;

    done = state = result = 0;
    while (done == 0) {
        switch (state) {
        case 0:
            winMsgOpen(2, 0x3C46, 1, 1);
            input = std011_dialogDisplayYesNoMenu();
            winMsgClose(1);
            if (input != 0) {
                done = 1;
            } else {
                state = 2;
            }
            break;
        case 2:
            if (menuSaveLoadSaveGame(1) == 0) {
                state = 3;
                result = 1;
            } else {
                state = 4;
            }
            break;
        case 3:
            floorRequestChange(0x395);
            state = 4;
            break;
        case 4:
            done = 1;
            break;
        }
    }

    return result;
}
