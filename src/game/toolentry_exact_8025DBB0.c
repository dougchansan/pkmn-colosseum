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

extern ToolentryBattleState* battleResultGetSave(void);

u32 battleResultGet14(void)
{
    return battleResultGetSave()->count_14;
}
