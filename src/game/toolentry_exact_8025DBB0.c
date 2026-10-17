#include "dolphin/types.h"

typedef struct ToolentryBattleState {
    u32 unk_00;
    u32 battleType;
    u32 unk_08;
    u32 unk_0C;
    u32 unk_10;
    u32 count_14;
    u32 count_18;
} ToolentryBattleState;

extern ToolentryBattleState* fn_8006B5A8(void);

u32 battleResultGet14(void)
{
    return fn_8006B5A8()->count_14;
}
