/**
 * @file pda_exact_8004A47C.c
 * @brief PDA mail screen layout, 0x8004A47C - 0x8004A7A8: pdaTrainerDataLayout points
 *        the mail window's layout records at the wide or narrow variant.
 *        Copied from pda_range_80037158.c.
 */
#include "dolphin/types.h"

/* Widget layout table: 0x1C-byte records indexed by event id; +2 x, +4 y. */
extern u8 lbl_802EF0A8[];
extern u8 std081_flagCheckTrue(s32 id);

typedef struct PdaPairS16 {
    s16 x;
    s16 y;
} PdaPairS16;

/* lbl_802EF0A8 menu item records, 0x1C bytes each. */
typedef struct PdaMenuItem {
    s16 id;
    s16 x;
    s16 y;
    u8 pad06[0x16];
} PdaMenuItem;

typedef struct PdaSpan12 {
    s16 v[12];
} PdaSpan12;

extern PdaPairS16 lbl_8047BD78;
extern PdaPairS16 lbl_8047BD7C;
extern PdaPairS16 lbl_8047BD80;
extern PdaPairS16 lbl_8047BD84;
extern PdaPairS16 lbl_8047BD88;
extern PdaPairS16 lbl_8047BD8C;
extern PdaPairS16 lbl_8047BD90;
extern PdaPairS16 lbl_8047BD94;
extern PdaSpan12 lbl_8026719C;
extern PdaSpan12 lbl_802671B4;

/* Re-point the mail window layout tables at the wide or narrow variant. */
void pdaTrainerDataLayout(void)
{
    PdaSpan12 wide;
    PdaSpan12 narrow;
    PdaPairS16 topWide[2];
    PdaPairS16 topNarrow[2];
    PdaPairS16 botWide[2];
    PdaPairS16 botNarrow[2];

    topWide[0] = lbl_8047BD78;
    topWide[1] = lbl_8047BD7C;
    topNarrow[0] = lbl_8047BD80;
    topNarrow[1] = lbl_8047BD84;
    if (std081_flagCheckTrue(0x3f0) != 0) {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8;

        items[0x437].y = topWide[0].x - 6;
        items[0x438].y = topWide[0].y - 6;
        items[0x439].y = topWide[1].x - 6;
        items[0x43a].y = topWide[1].y - 6;
    } else {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8;

        items[0x437].y = topNarrow[0].x - 6;
        items[0x438].y = topNarrow[0].y - 6;
        items[0x439].y = topNarrow[1].x - 6;
        items[0x43a].y = topNarrow[1].y - 6;
    }

    wide = lbl_8026719C;
    narrow = lbl_802671B4;
    if (std081_flagCheckTrue(0x3f0) != 0) {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e7;

        items[0].y = wide.v[0];
        items[1].y = wide.v[1];
        items[2].y = wide.v[2];
        items[0x76].y = wide.v[3];
        items[0x77].y = wide.v[4];
        items[0x78].y = wide.v[5];
        items[0x79].y = wide.v[6];
        items[0x7a].y = wide.v[7];
        items[0x7b].y = wide.v[8];
        items[0x7c].y = wide.v[9];
        items[0x7d].y = wide.v[10];
        items[0x7e].y = wide.v[11];
    } else {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e7;

        items[0].y = narrow.v[0];
        items[1].y = narrow.v[1];
        items[2].y = narrow.v[2];
        items[0x76].y = narrow.v[3];
        items[0x77].y = narrow.v[4];
        items[0x78].y = narrow.v[5];
        items[0x79].y = narrow.v[6];
        items[0x7a].y = narrow.v[7];
        items[0x7b].y = narrow.v[8];
        items[0x7c].y = narrow.v[9];
        items[0x7d].y = narrow.v[10];
        items[0x7e].y = narrow.v[11];
    }

    botWide[0] = lbl_8047BD88;
    botWide[1] = lbl_8047BD8C;
    botNarrow[0] = lbl_8047BD90;
    botNarrow[1] = lbl_8047BD94;
    if (std081_flagCheckTrue(0x3f0) != 0) {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e3;

        items[0].y = botWide[0].x - 6;
        items[1].y = botWide[0].y - 6;
        items[2].y = botWide[1].x - 6;
        items[3].y = botWide[1].y - 6;
    } else {
        PdaMenuItem* items = (PdaMenuItem*)lbl_802EF0A8 + 0x6e3;

        items[0].y = botNarrow[0].x - 6;
        items[1].y = botNarrow[0].y - 6;
        items[2].y = botNarrow[1].x - 6;
        items[3].y = botNarrow[1].y - 6;
    }
}
