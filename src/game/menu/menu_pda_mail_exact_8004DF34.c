/** Exact PDA-mail phase callback, 0x8004DF34 - 0x8004DFCC. */
#include "dolphin/types.h"

typedef struct PdaMailPhaseWidget {
    u8 pad00;
    s8 phase;
    s8 guard;
    u8 pad03;
    s32 msgObj;
} PdaMailPhaseWidget;

extern void winSeqSetMenu(s32 ctx, s32 id);

/* RULE-EXCEPTION(user-approved): local compiler-control pragma (peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma peephole off
s32 pdaMailPickerCtrl(PdaMailPhaseWidget* w)
{
    switch (w->phase) {
    case 0:
        if (w->guard == 0) {
            winSeqSetMenu(w->msgObj, 0x1c2);
            w->guard = 1;
        }
        break;
    case 3:
        if (w->guard == 0) {
            winSeqSetMenu(w->msgObj, 0x1c6);
            w->guard = 1;
        }
        break;
    }
    return 0;
}
#pragma peephole reset
