/**
 * @file hsd_texp_exact_801B387C.c
 * @brief Exact pure-C HSD render-state island, 0x801B387C - 0x801B3D1C.
 */

#include "dolphin/gx/GX.h"
#include "dolphin/types.h"
#include "hsd/hsd_debug.h"

typedef GXColor HsdChanColor;

typedef struct HSD_Chan HSD_Chan;

struct HSD_Chan {
    HSD_Chan* next;
    s32 chan;
    u32 flags;
    HsdChanColor amb_color;
    HsdChanColor mat_color;
    u8 enable;
    u8 pad_15[3];
    s32 amb_src;
    s32 mat_src;
    s32 light_mask;
    s32 diff_fn;
    s32 attn_fn;
    void* aobj;
};

extern HSD_Chan lbl_8036D018[4];
extern s32 lbl_8047B35C;
extern s32 lbl_8047B358;
extern u32 lbl_8047B370;
extern char lbl_8047DE60;
extern char lbl_8047DE68;

extern void fn_800B884C(u32 value);
extern void fn_800BA6B0(u8 num_chans);
extern void fn_800BA6F4(s32 chan, u8 enable, s32 amb_src, s32 mat_src,
                       s32 light_mask, s32 diff_fn, s32 attn_fn);
extern void fn_801B3D1C(HSD_Chan* chan);

void HSD_StateSetNumChans(s32 n);

s32 fn_801B387C(void)
{
    return lbl_8047B370;
}

void fn_801B3884(void)
{
    lbl_8047B370 = 0;
}

void fn_801B3890(void)
{
    fn_800B884C((u32) (u8) lbl_8047B358);
    lbl_8047B358 = 0;
}

static inline s32 HSD_TexCoordID2Num(s32 coord)
{
    switch (coord) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 3;
    case 3:
        return 4;
    case 4:
        return 5;
    case 5:
        return 6;
    case 6:
        return 7;
    case 7:
        return 8;
    case 0xFF:
        return 0;
    default:
        __assert((const char*) &lbl_8047DE60, 0x453,
                 (const char*) &lbl_8047DE68);
        return 0;
    }
}

void HSD_StateRegisterTexGen(s32 coord)
{
    s32 num = HSD_TexCoordID2Num(coord);

    if (num > lbl_8047B358) {
        lbl_8047B358 = num;
    }
}

static inline s32 TExpChannel2Num(s32 chan)
{
    switch (chan) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 1;
    case 5:
        return 2;
    case 0xFF:
        return 0;
    default:
        __assert((const char*) &lbl_8047DE60, 0x2F1,
                 (const char*) &lbl_8047DE68);
        return 0;
    }
}

#pragma push
#pragma optimization_level 1
void fn_801B3998(HSD_Chan* chan)
{
    s32 num_chans = 0;

    while (chan != NULL) {
        s32 chan_count = TExpChannel2Num(chan->chan);
        if (chan_count > num_chans) {
            num_chans = chan_count;
        }
        fn_801B3D1C(chan);
        chan = chan->next;
    }
    if (lbl_8047B35C != (u8) num_chans) {
        fn_800BA6B0((u8) num_chans);
        lbl_8047B35C = (u8) num_chans;
    }
}
#pragma pop

void HSD_StateSetNumChans(s32 n)
{
    if (lbl_8047B35C != n) {
        fn_800BA6B0((u8) n);
        lbl_8047B35C = n;
    }
}

#pragma push
#pragma optimization_level 1
void fn_801B3AE8(s32 chan)
{
    s32 idx = chan & 3;
    s32 amb_src;
    s32 mat_src;
    s32 diff_fn;
    s32 attn_fn;

    switch (chan) {
    case 4:
    case 5:
        if (lbl_8036D018[idx].enable != 0 ||
            lbl_8036D018[idx + 2].enable != 0) {
            lbl_8036D018[idx].enable =
                lbl_8036D018[idx + 2].enable = 0;
            lbl_8036D018[idx].light_mask =
                lbl_8036D018[idx + 2].light_mask = 0;
            amb_src = lbl_8036D018[idx].amb_src;
            mat_src = lbl_8036D018[idx].mat_src;
            diff_fn = lbl_8036D018[idx].diff_fn;
            attn_fn = lbl_8036D018[idx].attn_fn;
            lbl_8036D018[idx + 2].amb_src = amb_src;
            lbl_8036D018[idx + 2].mat_src = mat_src;
            lbl_8036D018[idx + 2].diff_fn = diff_fn;
            lbl_8036D018[idx + 2].attn_fn = attn_fn;
            fn_800BA6F4(chan, 0, amb_src, mat_src, 0, diff_fn, attn_fn);
        }
        break;
    default:
        if (lbl_8036D018[idx].enable != 0) {
            lbl_8036D018[idx].enable = 0;
            lbl_8036D018[idx].light_mask = 0;
            amb_src = lbl_8036D018[idx].amb_src;
            mat_src = lbl_8036D018[idx].mat_src;
            diff_fn = lbl_8036D018[idx].diff_fn;
            attn_fn = lbl_8036D018[idx].attn_fn;
            fn_800BA6F4(chan, 0, amb_src, mat_src, 0, diff_fn, attn_fn);
        }
        break;
    }
}
#pragma pop
