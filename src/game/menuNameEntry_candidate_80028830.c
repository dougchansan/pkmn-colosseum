/* Name-entry backdrop particle controller, 0x80028948-0x80028FBC.
 * Standalone copy of menuNameEntryBackCtrl from menuNameEntry.c; compiler
 * controls are unit-level GC/1.3 flags (-opt nopeephole).
 */
#include "dolphin/types.h"

typedef struct WorldMapOverlay {
    s32 active;
    f32 x;
    f32 y;
    f32 scale;
    f32 unused10;
    u32 color;
    u8 alpha;
    u8 pad19[3];
    f32 timer;
    f32 lifetime;
} WorldMapOverlay;

extern WorldMapOverlay lbl_803A20DC[];
extern f32 GSrandf(void);
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;
extern f32 lbl_8047B954;
extern f32 lbl_8047B958;
extern f32 lbl_8047B95C;
extern f32 lbl_8047B960;
extern f32 lbl_8047B964;
extern f32 lbl_8047B968;
extern f32 lbl_8047B96C;
extern f32 lbl_8047B970;

/* RULE-EXCEPTION(user-approved): single-use inline helpers (Prime, Step) shape register numbering — see docs/RULE_EXCEPTIONS.md */
static inline void menuNameEntryBackPrime(void)
{
    u32 clearSlot;
    s32 i;
    f32 threshold;
    f32 ratio;
    WorldMapOverlay* base;
    WorldMapOverlay* ov;
    s32 slot;
    WorldMapOverlay* ov2;
    s32 slot2;

    base = lbl_803A20DC;
    for (ov = base, clearSlot = 1; clearSlot <= 30; clearSlot++) {
        ov->active = 0;
        ov++;
    }
    i = 0;
    threshold = lbl_8047B958;
    do {
        if (!(GSrandf() > threshold)) {
            for (slot = 0, ov = base; slot < 30; ov++, slot++) {
                if (ov->active == 0) {
                    break;
                }
            }
            if (slot < 30) {
                lbl_803A20DC[slot].active = 1;
                ov = &lbl_803A20DC[slot];
                ov->x = lbl_8047B95C * GSrandf();
                ov->y = lbl_8047B960 * GSrandf();
                ov->scale = lbl_8047B964;
                ov->unused10 = lbl_8047B968 * GSrandf() + lbl_8047B964;
                ov->color = -0x100;
                ov->alpha = 0x80;
                ov->timer = lbl_8047B930;
                ov->lifetime = lbl_8047B970 * GSrandf() + lbl_8047B96C;
            }
        }
        for (slot2 = 0, ov2 = base; slot2 < 30; ov2++, slot2++) {
            /* RULE-EXCEPTION(user-approved): repeated no-op test gives retail's doubled beq — see docs/RULE_EXCEPTIONS.md */
            if (ov2->active != 0 && ov2->active != 0) {
                if ((ov2->timer += lbl_8047B934) >= ov2->lifetime) {
                    ov2->active = 0;
                }
                ratio = ov2->timer / ov2->lifetime;
                ov2->scale = ov2->unused10 * ratio;
                ov2->alpha = lbl_8047B954 * (lbl_8047B934 - ratio);
            }
        }
    } while (++i < 0x258);
}

static inline void menuNameEntryBackStep(void)
{
    s32 slot;
    f32 ratio;
    WorldMapOverlay* ov;

    if (!(GSrandf() > lbl_8047B958)) {
        for (slot = 0, ov = lbl_803A20DC; slot < 30; ov++, slot++) {
            if (ov->active == 0) {
                break;
            }
        }
        if (slot < 30) {
            lbl_803A20DC[slot].active = 1;
            lbl_803A20DC[slot].x = lbl_8047B95C * GSrandf();
            lbl_803A20DC[slot].y = lbl_8047B960 * GSrandf();
            lbl_803A20DC[slot].scale = lbl_8047B964;
            lbl_803A20DC[slot].unused10 = lbl_8047B968 * GSrandf() + lbl_8047B964;
            lbl_803A20DC[slot].color = -0x100;
            lbl_803A20DC[slot].alpha = 0x80;
            lbl_803A20DC[slot].timer = lbl_8047B930;
            lbl_803A20DC[slot].lifetime = lbl_8047B970 * GSrandf() + lbl_8047B96C;
        }
    }
    for (slot = 0, ov = lbl_803A20DC; slot < 30; ov++, slot++) {
        /* RULE-EXCEPTION(user-approved): repeated no-op test gives retail's doubled beq — see docs/RULE_EXCEPTIONS.md */
        if (ov->active != 0 && ov->active != 0) {
            if ((ov->timer += lbl_8047B934) >= ov->lifetime) {
                ov->active = 0;
            }
            ratio = ov->timer / ov->lifetime;
            ov->scale = ov->unused10 * ratio;
            ov->alpha = lbl_8047B954 * (lbl_8047B934 - ratio);
        }
    }
}

s32 menuNameEntryBackCtrl(void* r3)
{
    extern f32 GSrandf(void);            /* random f32 source (-> f1) */
    extern f32 lbl_8047B958;                 /* spawn threshold           */
    extern f32 lbl_8047B95C;                 /* x scale                   */
    extern f32 lbl_8047B960;                 /* y scale                   */
    extern f32 lbl_8047B964;                 /* scale base / init scale   */
    extern f32 lbl_8047B968;                 /* scale rand coeff          */
    extern f32 lbl_8047B96C;                 /* lifetime base             */
    extern f32 lbl_8047B970;                 /* lifetime rand coeff       */
    extern f32 lbl_8047B930;                 /* initial timer             */
    extern f32 lbl_8047B934;                 /* timer increment / 1.0     */
    extern f32 lbl_8047B954;                 /* alpha scale               */

    u8* ctl;

    ctl = (u8*)r3;
    switch ((s8)ctl[1]) {
    case 0:
        if ((s8)ctl[2] != 0) {
            break;
        }
        menuNameEntryBackPrime();
        ctl[2] = 1;
        break;

    case 2:
        menuNameEntryBackStep();
        break;

    case 3:
        if ((s8)ctl[2] == 0) {
            ctl[2] = 1;
        }
        break;
    }
    return 0;
}
