/* fn_80044630 (PDA summary-screen state machine), standalone copy of the shared PDA source.
 * Unit flag -opt nopeephole replaces the shared file's local "peephole off". */
#include "dolphin/types.h"

typedef struct PdaSceneWork {
    s32 currentIndex;
    u8 pad04[0xC];
    s32 field_10;
    u8 pad14[0x14];
    s32 field_28;
    u8 pad2C[0x14];
    f32 angle;
    u8 pad44[8];
    f32 alphaScale;
    u8 pad50[0x1F8];
} PdaSceneWork; /* lbl_803A6818, size 0x248 */

extern PdaSceneWork lbl_803A6818;
extern u16* lbl_8047A4E4;
extern f32 lbl_8047BC94;
extern f32 lbl_8047BCB8;
extern f32 lbl_8047BCBC;
extern f32 lbl_8047BCC0;
extern f32 lbl_8047BCC4;
extern f32 lbl_8047BCC8;

/* RULE-EXCEPTION(user-approved): reconstructed inline helpers pdaAbsF/pdaApproach (fn_80040018, pdaMemoUpdateScroll, fn_80044630) — see docs/RULE_EXCEPTIONS.md */
static inline f32 pdaAbsF(f32 x)
{
    return x > 0.0f ? x : -x;
}

/* fn_80044630's pdaApproach: the upper clamp is the 20.0f literal (as an extern read it colours the clamp's float registers in the other order). */
/* RULE-EXCEPTION(user-approved): reconstructed inline helper pda_80044630_Approach (fn_80044630's copy of pdaApproach) - see docs/RULE_EXCEPTIONS.md */
static inline void pda_80044630_Approach(f32* current, f32* target, f32 rate)
{
    f32 step;
    f32 distance;
    f32 scaled;

    step = *target - *current;
    scaled = lbl_8047BCC4 * step;
    step = scaled * rate;
    if (step > 20.0f) {
        step = 20.0f;
    }
    if (step <= lbl_8047BCC8) {
        step = lbl_8047BCC8;
    }
    distance = *target - (*current += step);
    if (pdaAbsF(distance) <= pdaAbsF(step) || pdaAbsF(distance) < lbl_8047BCBC) {
        *current = *target;
    }
}

void pdaStrategyMemoRun(void) {
    extern void  _threadSwitch(void);
    extern u32   menuCloseCustom(u32 a, u32 b, u32 c);
    extern s32   menuOpen(s32 menu, s32 mode);
    extern void  GSvecCopy(void* dst, void* src);
    extern void  GSscene_SetMode(u32 mode);
    extern void  GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
    extern const f32 lbl_80267150[];

    extern const f32 lbl_8047BCA8;
    extern const f32 lbl_8047BCCC;
    extern const f32 lbl_8047BCF0;
    extern const f32 lbl_8047BD18;

    extern u32   lbl_8047A4E0;

    extern void  pdaModelViewerSetup(void);
    extern void  pdaStrategyMemoUpdate(void);
    extern void  pdaStrategyMemoSetSelection(s32 a, s32 b);
    extern void* std112_modelGetResourceID(u32 id);
    extern void* GSresGetResource();
    extern void* std093_getCurrentGroupID(void);
    extern void  std113_mapObjectUnk_8018db68(void* h);
    extern void  std114_mapObjectSetVisibility(void* h, s32 b);
    extern void  GSmodelSetVisibility(void* a, s32 b);
    extern void  GSmodelSetAnimRate(void* model, f32 rate);
    extern void  GSmodelSet60fpsAnimFlag(void* h, s32 b);
    extern void  GSmodelSetBoundCheck(void* h, s32 b);
    extern void  GSmodelDestroyLinkedParticles(void* model);
    extern void* wazaSequenceOwnerGetModel(void* h);
    extern void  cameraPlayAnime(u32 a, u32 id, u32 c, u32 d);
    extern void  menuModelTerm(void* subobj);
    extern void  menuOffScreenSetDisp(s32 a);
    extern u32   GSmemGetHandleByPtr(u32 flag);
    extern void  fn_800E24B0(void);
    extern void  GSmemFree(u32 h);
    extern void  GScameraGetPosition(void* cam, void* outVec3);
    extern void  GScameraGetRotation(void* cam, void* outVec3);
    extern void  GScameraGetPerspective(void* cam, f32* a, f32* b, f32* c, f32* d);

    u8*  S;
    u8*  S124;
    u8*  S130;
    u8*  S218;
    u8*  S118;
    const f32* tbl;
    s32  running = 1;
    void* h;
    void* handle;
    u32 mem;

    tbl = lbl_80267150;
    pdaModelViewerSetup();
    S = (u8*)&lbl_803A6818;

    S118 = S + 0x118;
    S124 = S + 0x124;
    S130 = S + 0x130;
    S218 = S + 0x218;

    while (1) {
        switch (*(s32*)(S + 0x1C)) {
        case 0: {   /* .L_800446F0 */
            pdaStrategyMemoSetSelection(-1, 1);
            menuOpen(0x35, 0);
            menuOpen(0x3b, 0);
            menuOpen(0x36, 0);
            menuOpen(0x37, 0);
            menuOpen(0x38, 0);
            menuOpen(0x39, 0);
            *(s32*)(S + 0x1c) = 1;
        } break;

        case 1: {   /* .L_80044750 */
            s32 r;
            r = menuOpen(0x3a, 1);
            *(s32*)(S + 0x28) = 0;
            if (r >= 0) {
                if (*(s32*)(S + 0x18) == 1) {
                    *(s32*)(S + 0x1c) = 7;
                    *(f32*)(S + 0x50) = lbl_8047BC94;
                    *(f32*)(S + 0x58) = lbl_8047BCBC;
                } else {
                    *(s32*)(S + 0x1c) = 6;
                    *(f32*)(S + 0x50) = lbl_8047BC94;
                    *(f32*)(S + 0x58) = lbl_8047BCBC;
                }
            } else {
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
                *(s32*)(S + 0x1c) = 0xb;
            }
        } break;

        case 2: {   /* .L_800447CC */
            pdaStrategyMemoUpdate();
        } break;

        case 4: {   /* .L_800447D4 */
            s32 r;
            menuOpen(0x96, 0);
            menuOpen(0xa1, 0);
            r = menuOpen(0xa2, 1);
            if (r >= 0)
                break;
            if (*(s32*)(S + 0x1c) == 4) {
                *(s32*)(S + 0x1c) = 9;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
            }
            *(s32*)(S + 0x14) = 0;
        } break;

        case 5: {   /* .L_80044830 */
            s32 r;
            menuOpen(0x96, 0);
            r = menuOpen(0xa2, 1);
            if (r >= 0)
                break;
            if (*(s32*)(S + 0x1c) == 5) {
                *(s32*)(S + 0x1c) = 0xa;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
            }
            *(s32*)(S + 0x14) = 0;
        } break;

        case 3: {   /* .L_80044880 */
            s32 r;
            *(u8*)(S + 0x158) = 0;
            *(u8*)(S + 0x159) = 0;
            *(u8*)(S + 0x15b) = 0;
            *(u8*)(S + 0x15a) = 0;
            *(u8*)(S + 0x15c) = 0;
            *(u8*)(S + 0x15d) = 0;
            *(u8*)(S + 0x15f) = 0;
            *(u8*)(S + 0x15e) = 0;
            *(s32*)(S + 0x154) = 0;
            *(f32*)(S + 0x1e0) = lbl_8047BCCC;
            *(f32*)(S + 0x1dc) = lbl_8047BCCC;
            *(f32*)(S + 0x1cc) = lbl_8047BC94;
            *(f32*)(S + 0x1d0) = lbl_8047BC94;
            *(f32*)(S + 0x1d4) = lbl_8047BC94;
            *(f32*)(S + 0x1d8) = lbl_8047BC94;
            *(u8*)(S + 0x164) = 0;
            *(u8*)(S + 0x178) = 0;
            *(u8*)(S + 0x18c) = 0;
            *(u8*)(S + 0x1a0) = 0;
            *(u8*)(S + 0x1b4) = 0;
            *(s32*)(S + 0x1e4) = 0;
            r = menuOpen(0xce, 1);
            if (r < 0) {
                *(s32*)(S + 0x1c) = 0xc;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
                menuCloseCustom(0xce, 0, 0);
            }
            *(s32*)(S + 0x14) = 0;
        } break;

        case 6: {   /* .L_80044928 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nval;
            f32 delta;
            f32 rem;
            f32 mag;

            /* ease block A: unk4C toward unk50 (div BCB8) */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nval = cur + step;
                    *(f32*)(S + 0x4C) = nval;
                    if (nval > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nval = cur - step;
                    *(f32*)(S + 0x4C) = nval;
                    if (nval < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B: unk54 toward unk58 (div BCB8) */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nval = cur + step;
                    *(f32*)(S + 0x54) = nval;
                    if (nval > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nval = cur - step;
                    *(f32*)(S + 0x54) = nval;
                    if (nval < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C: unk5C toward unk60 (div BCC0) */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nval = cur + step;
                    *(f32*)(S + 0x5C) = nval;
                    if (nval > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nval = cur - step;
                    *(f32*)(S + 0x5C) = nval;
                    if (nval < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D: unk1DC toward unk1E0 (signed-delta + clamps) */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x14) = 1;
                menuCloseCustom(0x35, 0, 1);
                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
            {
                    void* cam;
                    f32   persp3;
                    f32   persp2;
                    f32   persp1;
                    f32   persp0;
                    f32   rot[3];
                    f32   pos[3];
                    f32   mtx[3];

                    std113_mapObjectUnk_8018db68(h);

                    *(f32*)(S + 0x6C) = lbl_8047BC94;
                    *(f32*)(S + 0x70) = lbl_8047BC94;
                    *(f32*)(S + 0x74) = lbl_8047BC94;
                    *(f32*)(S + 0x64) = lbl_8047BCF0;

                    cam = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D181000);
                    GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), cam),
                                       lbl_8047BD18);
                    GSmodelSet60fpsAnimFlag(*(void**)(S + 0x110), 1);
                    GSmodelSetBoundCheck(*(void**)(S + 0x110), 0);

                    ((u32*)mtx)[0] = ((const u32*)tbl)[6];
                    ((u32*)mtx)[1] = ((const u32*)tbl)[7];
                    ((u32*)mtx)[2] = ((const u32*)tbl)[8];
                    ((u32*)pos)[0] = ((const u32*)tbl)[9];
                    ((u32*)pos)[1] = ((const u32*)tbl)[10];
                    ((u32*)pos)[2] = ((const u32*)tbl)[11];
                    cam = GSresGetResource(0x17, 0x0D741800);
                    *(void**)(S + 0x114) = cam;
                    GScameraGetPosition(cam, pos);
                    GScameraGetRotation(cam, rot);
                    mtx[0] = *(f32*)((u8*)cam + 0x100);
                    mtx[1] = *(f32*)((u8*)cam + 0x104);
                    mtx[2] = *(f32*)((u8*)cam + 0x108);
                    GScameraGetPerspective(cam, &persp0, &persp1, &persp2, &persp3);
                    GScameraSetPerspective(cam, persp0, persp1, persp2, persp3);
                    GSvecCopy(S118, pos);
                    GSvecCopy(S124, rot);
                    GSvecCopy(S130, mtx);

                    *(f32*)(S + 0x13C) = persp0;
                    *(f32*)(S + 0x140) = persp1;
                    *(f32*)(S + 0x144) = persp2;
                    *(f32*)(S + 0x148) = persp3;

                    cameraPlayAnime(0x17, 0x0D741800, 0, 1);
                    GSscene_SetMode(4);

                    *(s32*)(S + 0x78) = 1;
                    pdaStrategyMemoSetSelection(-1, 2);

                    {
                        s32 sub = *(s32*)(S + 0x28);
                        void* particles;
                        *(f32*)(S + 0x6C) = lbl_8047BC94;
                        *(f32*)(S + 0x70) = lbl_8047BC94;
                        *(f32*)(S + 0x74) = lbl_8047BC94;
                        *(f32*)(S + 0x64) = lbl_8047BCF0;
                        switch (sub) {
                        case 0:
                            *(s32*)(S + 0x1C) = 0xD;
                            *(s32*)(S + 0x24) = 2;
                            *(f32*)(S + 0x60) = lbl_8047BC94;
                            if ((particles = wazaSequenceOwnerGetModel(*(void**)(S + 0xA0))) != 0)
                                GSmodelDestroyLinkedParticles(particles);
                            break;
                        case 1:
                            *(s32*)(S + 0x1C) = 0xD;
                            *(s32*)(S + 0x24) = 4;
                            *(f32*)(S + 0x60) = lbl_8047BC94;
                            if ((particles = wazaSequenceOwnerGetModel(*(void**)(S + 0xA0))) != 0)
                                GSmodelDestroyLinkedParticles(particles);
                            *(f32*)(S + 0x64) = lbl_8047BCA8;
                            break;
                        case 2:
                            *(s32*)(S + 0x1C) = 0xD;
                            *(s32*)(S + 0x24) = 5;
                            *(f32*)(S + 0x60) = lbl_8047BC94;
                            if ((particles = wazaSequenceOwnerGetModel(*(void**)(S + 0xA0))) != 0)
                                GSmodelDestroyLinkedParticles(particles);
                            break;
                        default:
                            break;
                        }
                    }

                    *(f32*)(S + 0x50) = lbl_8047BCBC;
                    *(f32*)(S + 0x58) = lbl_8047BC94;
                    break;
                }
            } else {
                _threadSwitch();
            }
            break;
        }

        case 8: {   /* .L_80044DA0 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            f32 delta;
            f32 mag;
            f32 rem;
            f32 absrem;
            f32 absmag;
            f32 staging3;
            f32 staging2;
            f32 staging1;
            f32 staging0;
            f32 rot[3];
            f32 pos[3];
            f32 mtx[3];

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x3C, 0, 1);
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);

                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
                std113_mapObjectUnk_8018db68(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D171000);
                GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), h),
                                   lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = GSresGetResource(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rot);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &staging0, &staging1, &staging2, &staging3);

                GSvecCopy(S218, pos);
                GSvecCopy(S218 + 0xC, mtx);
                *(f32*)(S218 + 0x18) = staging0;
                *(f32*)(S218 + 0x1C) = staging1;
                *(f32*)(S218 + 0x20) = staging2;
                *(f32*)(S218 + 0x24) = staging3;
                GScameraSetPerspective(h, staging0, staging1, staging2, staging3);

                GSvecCopy(S118, pos);
                GSvecCopy(S124, rot);
                GSvecCopy(S130, mtx);

                *(f32*)(S + 0x13C) = staging0;
                *(f32*)(S + 0x140) = staging1;
                *(f32*)(S + 0x144) = staging2;
                *(f32*)(S + 0x148) = staging3;

                cameraPlayAnime(0x17, 0x0D731800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 0;
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
        } break;

        case 9: {   /* .L_80045178 */
            f32 step;
            f32 cur;
            f32 tgt;
            f32 px3;
            f32 px2;
            f32 px1;
            f32 px0;
            f32 nv;
            f32 rot[3];
            f32 pos[3];
            f32 mtx[3];

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {

                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0xa1, 0, 1);
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xa2, 0, 1);

                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }

                /* .L_800453AC camera-setup */
                std113_mapObjectUnk_8018db68(h);
                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D171000);
                GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), h),
                                   lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = GSresGetResource(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rot);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &px0, &px1, &px2, &px3);

                GSvecCopy(S218, pos);
                GSvecCopy(S218 + 0xC, mtx);

                *(f32*)(S218 + 0x18) = px0;
                *(f32*)(S218 + 0x1C) = px1;
                *(f32*)(S218 + 0x20) = px2;
                *(f32*)(S218 + 0x24) = px3;
                GScameraSetPerspective(h, px0, px1, px2, px3);

                GSvecCopy(S118, pos);
                GSvecCopy(S124, rot);
                GSvecCopy(S130, mtx);

                *(f32*)(S + 0x13C) = px0;
                *(f32*)(S + 0x140) = px1;
                *(f32*)(S + 0x144) = px2;
                *(f32*)(S + 0x148) = px3;

                cameraPlayAnime(0x17, 0x0D731800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 0;
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
            break;
        }

        case 0xA: {   /* .L_80045550 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 up;
            f32 down;
            f32 delta;
            f32 gstep;
            f32 newcur;
            f32 rem;
            f32 arem;

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    up = cur + step;
                    *(f32*)(S + 0x4C) = up;
                    if (up > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    down = cur - step;
                    *(f32*)(S + 0x4C) = down;
                    if (down < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    up = cur + step;
                    *(f32*)(S + 0x54) = up;
                    if (up > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    down = cur - step;
                    *(f32*)(S + 0x54) = down;
                    if (down < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    up = cur + step;
                    *(f32*)(S + 0x5C) = up;
                    if (up > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    down = cur - step;
                    *(f32*)(S + 0x5C) = down;
                    if (down < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {

                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);
                h = *(void**)(S + 0x10C);
                std114_mapObjectSetVisibility(h, 0);
                {
                    void* rv = GSresGetResource(std093_getCurrentGroupID(), h);
                    if (rv != 0)
                        GSmodelSetVisibility(rv, 0);
                }
            {
                    f32 p3;
                    f32 p2;
                    f32 p1;
                    f32 p0;
                    f32 rotbuf[3];
                    f32 pos[3];
                    f32 mtx[3];

                    std113_mapObjectUnk_8018db68(h);

                    *(f32*)(S + 0x6C) = lbl_8047BC94;
                    *(f32*)(S + 0x70) = lbl_8047BC94;
                    *(f32*)(S + 0x74) = lbl_8047BC94;
                    *(f32*)(S + 0x64) = lbl_8047BCF0;

                    h = *(void**)(S + 0x10C) = std112_modelGetResourceID(0x0D171000);

                    GSmodelSetAnimRate(*(void**)(S + 0x110) = GSresGetResource(std093_getCurrentGroupID(), h),
                                       lbl_8047BD18);

                    ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                    ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                    ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                    ((u32*)pos)[0] = ((const u32*)tbl)[3];
                    ((u32*)pos)[1] = ((const u32*)tbl)[4];
                    ((u32*)pos)[2] = ((const u32*)tbl)[5];
                    h = GSresGetResource(0x17, 0x0D731800);
                    *(void**)(S + 0x114) = h;

                    GScameraGetPosition(h, pos);
                    GScameraGetRotation(h, rotbuf);

                    mtx[0] = *(f32*)((u8*)h + 0x100);
                    mtx[1] = *(f32*)((u8*)h + 0x104);
                    mtx[2] = *(f32*)((u8*)h + 0x108);
                    GScameraGetPerspective(h, &p0, &p1, &p2, &p3);

                    GSvecCopy(S218, pos);
                    GSvecCopy(S218 + 0x0C, mtx);

                    *(f32*)(S218 + 0x18) = p0;
                    *(f32*)(S218 + 0x1C) = p1;
                    *(f32*)(S218 + 0x20) = p2;
                    *(f32*)(S218 + 0x24) = p3;
                    GScameraSetPerspective(h, p0, p1, p2, p3);

                    GSvecCopy(S118, pos);
                    GSvecCopy(S124, rotbuf);
                    GSvecCopy(S130, mtx);

                    *(f32*)(S + 0x13C) = p0;
                    *(f32*)(S + 0x140) = p1;
                    *(f32*)(S + 0x144) = p2;
                    *(f32*)(S + 0x148) = p3;
                    cameraPlayAnime(0x17, 0x0D731800, 0, 1);
                    GSscene_SetMode(4);

                    *(s32*)(S + 0x78) = 0;
                    *(s32*)(S + 0x14) = 0;
                    *(f32*)(S + 0x50) = lbl_8047BCBC;
                    *(f32*)(S + 0x58) = lbl_8047BC94;

                    break;
                }
            } else {
                _threadSwitch();
            }
            break;
        }

        case 7: {   /* .L_80045918 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            f32 delta;
            f32 mag;
            f32 rem;
            f32 absrem;
            f32 absmag;

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                menuCloseCustom(0x35, 0, 1);
                *(s32*)(S + 0x1C) = 3;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
        } break;

        case 0xC: {   /* .L_80045B1C */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;

            /* ease A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* converge */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0;
                menuCloseCustom(0x96, 0, 1);
                menuCloseCustom(0xA2, 0, 1);
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            } else {
                _threadSwitch();
            }
        } break;

        case 0xD: {   /* .L_80045D38 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            s32 sub;

            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }
            if (*(f32*)(S + 0x60) == *(f32*)(S + 0x5C)) {
                sub = *(s32*)(S + 0x24);
                switch (sub) {
                case 2:
                    *(s32*)(S + 0x1C) = sub;
                    break;
                case 4:
                    *(s32*)(S + 0x1C) = sub;
                    break;
                case 5:
                    *(s32*)(S + 0x1C) = sub;
                    break;
                default:
                    break;
                }
                *(f32*)(S + 0x60) = lbl_8047BCBC;
            }
        } break;

        case 0xE: {   /* .L_80045DE8 */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;

            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }
            if (*(f32*)(S + 0x60) == *(f32*)(S + 0x5C)) {
                *(s32*)(S + 0x1C) = *(s32*)(S + 0x24);
            }
        } break;

        case 0xB: {   /* .L_80045E5C */
            f32 cur;
            f32 tgt;
            f32 step;
            f32 nv;
            f32 delta;
            f32 mag;
            f32 rem;
            f32 absrem;
            f32 absmag;

            /* ease A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x4C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x54) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    nv = cur + step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    nv = cur - step;
                    *(f32*)(S + 0x5C) = nv;
                    if (nv < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* block D */
            if (*(f32*)(S + 0x1DC) != *(f32*)(S + 0x1E0)) {
                pda_80044630_Approach((f32*)(S + 0x1DC), (f32*)(S + 0x1E0), *(f32*)(S + 0x3C));
            }

            /* settle */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0x64;
            } else {
                _threadSwitch();
            }
        } break;

        case 0x64: { /* .L_80046040 */
            menuCloseCustom(0x35, 0, 1);
            running = 0;
        } break;

        default:
            break;
        }
        if (running != 0) {
            continue;
        }
        break;
    }

    /* === EXIT / cleanup epilogue (.L_80046054 fall-through .. blr) === */
    handle = *(void**)(S + 0x10C);
    std114_mapObjectSetVisibility(handle, 0);
    {
        void* rv = GSresGetResource(std093_getCurrentGroupID(), handle);
        if (rv != 0)
            GSmodelSetVisibility(rv, 0);
    }
    std113_mapObjectUnk_8018db68(handle);

        handle = *(void**)((u8*)&lbl_803A6818 + 0x150);
    std114_mapObjectSetVisibility(handle, 0);
    {
        void* rv = GSresGetResource(std093_getCurrentGroupID(), handle);
        if (rv != 0)
            GSmodelSetVisibility(rv, 0);
    }
    std113_mapObjectUnk_8018db68(handle);

    if (lbl_8047A4E0 != 0) {
        mem = GSmemGetHandleByPtr(lbl_8047A4E0);
        if ((u16)mem != 0) {
            fn_800E24B0();
            GSmemFree(mem);
        }
        lbl_8047A4E0 = 0;
    }

    menuOffScreenSetDisp(1);
        menuModelTerm((u8*)&lbl_803A6818 + 0x7c);
        menuModelTerm((u8*)&lbl_803A6818 + 0xc4);

        mem = GSmemGetHandleByPtr((u32)lbl_8047A4E4);
    if ((u16)mem != 0) {
        fn_800E24B0();
        GSmemFree(mem);
    }
    lbl_8047A4E4 = 0;
}
