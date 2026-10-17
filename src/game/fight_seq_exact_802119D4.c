#include "dolphin/types.h"

extern u32 lbl_8047B618;
extern void fightSeqTurnEnd(void);

void fightSeqPost(void)
{
    lbl_8047B618 &= ~0x00100000u;
    fightSeqTurnEnd();
}
