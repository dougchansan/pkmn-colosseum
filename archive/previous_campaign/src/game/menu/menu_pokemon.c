/**
 * @file menu_pokemon.c
 * @brief Pokemon summary screen and party management UI.
 *
 * Implements the Pokemon summary screen, party list view, and all sub-screens
 * for viewing and managing Pokemon in the player's party. This is one of the
 * largest modules in the unattributed gap, containing 36 functions spanning
 * nearly 50KB of code.
 *
 * Key behaviors:
 *   - Uses BSS lbl_803A6818 (0x248 bytes) as the primary summary screen state.
 *     29 of 36 functions reference this structure directly.
 *   - The state machine at offset +0x1C in the BSS struct controls which
 *     sub-screen is active (values 0-13 observed in fn_8003D1FC's switch)
 *   - fn_80044630 (0x1B38 = 6,968 bytes) is the largest function -- the main
 *     summary screen update loop, dispatching to stat pages, move lists, etc.
 *   - fn_80042658 (0x10D0 = 4,304 bytes) handles party list selection
 *   - fn_80040308 (0xE0C = 3,596 bytes) handles summary page rendering
 *   - Heavily calls Pokemon data accessors fn_801FAA58 (129x) and
 *     fn_801FB1C0 (112x) from the pokemon.c / trainer.c area
 *   - Also references fn_8001E224 / fn_8001E200 (91x each) for string/text
 *     message lookups
 *   - Uses internal helpers fn_800478B4 and fn_80047CC0 for party cursor
 *     navigation and selection UI
 *   - Material/render setup via fn_800E01D0 using data from lbl_802E5448
 *   - Also uses lbl_803A67E8 (0x14 bytes) and lbl_803A67FC (0x1C bytes)
 *     for party selection sub-state
 *
 * BSS usage:
 *   - lbl_803A6818 (0x248 bytes): Summary screen state structure
 *   - lbl_803A67E8 (0x14 bytes): Party sub-state A
 *   - lbl_803A67FC (0x1C bytes): Party selection cursor state
 *
 * Address range: 0x8003D1FC - 0x800495C8 (36 functions)
 */

#include "dolphin/types.h"

/* ===== GS Engine ===== */
extern void  _threadSwitch(void);           /* GSthread yield */
extern void  fn_800E01D0(void* dst, void* src); /* material copy */
extern u32   fn_80102568(u32 a, u32 b, u32 c); /* scene load */
extern u32   fn_8010264C(u32 a, u32 b);        /* scene query */
extern u32   fn_80102510(u32 a);                /* scene unload */
extern void  fn_801026A4(u32 sceneId, u32 a, u32 b, u32 c,
                         u32 d, u32 e, ...);
extern u32   menuCloseSync(u32 a);                /* scene property get */
extern void  fn_80109220(u32 obj, u8 visible);  /* model visibility set */
extern void  fn_800FB680(u32 a, u32 b, s32 c, u32 d); /* sound trigger */
extern void  fn_80132A38(u32 effectId, u32 param);     /* effect trigger */
extern void  fn_80135938(u32 a, u32 b);         /* effect alloc */
extern void  fn_801240C4(void* obj, u16 species, u32 form); /* people set species */
extern void* fn_80123FBC(void* obj);            /* people get */
extern void  fn_8012640C(void* obj);            /* people field update */
extern void  fn_8011DFE0(void* obj, u32 pos);  /* people position set */
extern void  fn_8011DF90(void* obj, u32 rot);  /* people rotation set */
extern void  GSscene_SetMode(u32 sceneType);        /* scene type set */
extern u32   fn_80105624(u32 a);                /* model state query */
extern u32   fn_801080CC(u32 a);                /* model anim check */
extern void  fn_800FA280(void);                 /* field utility */
extern void  fn_800FA444(void* obj);            /* field model update */
extern void  fn_800D61E4(void* obj);            /* render setup */
extern void  fn_800D5CB8(void* obj);            /* render cleanup */
extern void  GScameraSetPerspective(void* obj, f32 a, f32 b, f32 c, f32 d); /* render matrix */
extern void  fn_8011F5C8(u32 a);               /* field world query */
extern void  fn_8011E778(u32 a);               /* field world accessor */

/* ===== Pokemon / Trainer data ===== */
extern void* fn_801FAA58(u32 slot);       /* Get Pokemon from party slot */
extern u32   fn_801FB1C0(void* pkmn, u32 field);  /* Get Pokemon field data */
extern u32   fn_8025FDDC(u32 a, u16 species);     /* Species data accessor A */
extern u32   fn_8025FD34(u32 a, u16 species);     /* Species data accessor B */
extern void* fn_801FBFBC(u16 species);             /* Species base data get */

/* ===== Text / Messages ===== */
extern void* fn_8001E224(u32 msgBank, u32 msgId);  /* Message string get A */
extern u32   fn_8001E200(u32 msgBank, u32 msgId);  /* Message string get B */
extern s8    fn_8001E074(u32 a, u32 b, u32 c, u32 d); /* Input wait / text input */

/* ===== Battle system ===== */
extern void  fn_801D1F7C(u32 a);           /* Battle data accessor */
extern void  fn_801D16F0(u32 a);           /* Battle type query */

/* ===== Internal helpers ===== */
extern void  fn_800478B4(void* party, void* cursor);  /* Party cursor update */
extern void  fn_80047CC0(void* party);                 /* Party cursor navigate */
extern void  fn_800492CC(void* state);                 /* Party selection finalize */

/* ===== BSS data ===== */
extern u8    lbl_803A67E8[];   /* Party sub-state A (0x14 bytes) */
extern u8    lbl_803A67FC[];   /* Party selection cursor (0x1C bytes) */
extern u8    lbl_803A6818[];   /* Summary screen state (0x248 bytes) */

/* ===== Data section ===== */
extern const u8 lbl_802E5448[];  /* Material preset data */

/* ===== Rodata tables ===== */
extern const u32 lbl_80267060[];  /* Summary screen layout rects (8 entries, 0x18 per entry) */
extern const u32 lbl_80267120[];  /* Page index table [0,1,2,3] */
extern const u32 lbl_80267130[];  /* Message IDs for stat labels [0x395,0x396,0x397,0] */
extern const u32 lbl_80267140[];  /* Message IDs for stat pages [0x39A,0x39C,0x39B,0x39D] */
extern const f32 lbl_80267150[];  /* Summary screen float positions (12 floats) */
extern const f32 lbl_80267180[];  /* Color values [255.0, 255.0, 255.0, 0.0] */
extern const f32 lbl_80267190[];  /* Camera offset [14.0, -12.0, 10.0] */
extern const u16 lbl_8026719C[];  /* Stat bar positions (12 halfwords) */
extern const u16 lbl_802671B4[];  /* Stat bar positions alt (14 halfwords) */
extern const u32 lbl_802671D0[];  /* Type icon message IDs (12 entries: 0x462-0x472) */
extern const u32 lbl_80267200[];  /* Level bar segments descending (0x47C-0x473) */
extern const u32 lbl_80267228[];  /* Level bar segments ascending (0x47D-0x486) */
extern const u32 lbl_80267250[];  /* HP bar segments descending (0x490-0x487) */
extern const u32 lbl_80267278[];  /* EXP bar segments (0x49E-0x495) */
extern const u32 lbl_802672A0[];  /* Status bar segments (10 entries) */
extern const u32 lbl_802672C8[];  /* Move slot message IDs [0x36BC-0x36BF] */
extern const u32 lbl_802672D8[];  /* Move detail positions (6 entries) */
extern const u32 lbl_802672F0[];  /* Contest stat table (12 entries) */
extern const u32 lbl_80267320[];  /* Ribbon display table (6 entries) */
extern const u32 lbl_80267338[];  /* Ribbon detail table (6 entries) */
extern const u32 lbl_80267350[];  /* Marking/icon table (18 entries) */

/*
 * Functions in this translation unit (36 total):
 *
 * fn_8003D1FC  0x2CC  Summary screen top-level state machine
 * fn_8003D4C8  0x350  Summary sub-state dispatcher (calls fn_8003D8CC, fn_8003DC54, fn_8003E394)
 * fn_8003D818  0x0B4  Summary page transition (calls fn_8003E394)
 * fn_8003D8CC  0x388  Summary data loader
 * fn_8003DC54  0x740  Stat page renderer (large)
 * fn_8003E394  0xCAC  Move list / detail page renderer (very large, 3244 bytes)
 * fn_8003F040  0x29C  Utility sub-handler A
 * fn_8003F2DC  0x188  Utility sub-handler B
 * fn_8003F464  0xBB4  Contest/ribbon page renderer (2996 bytes)
 * fn_80040018  0x2F0  Info page renderer
 * fn_80040308  0xE0C  Summary page rendering core (3596 bytes)
 * fn_80041114  0x048  Party select entry A (calls fn_80041E48, fn_800439BC)
 * fn_8004115C  0x048  Party select entry B (calls fn_80042658, fn_800439BC)
 * fn_800411A4  0x048  Party select entry C (calls fn_80042658, fn_800439BC)
 * fn_800411EC  0x010  Party get current selection (lbl_803A6818+0x28)
 * fn_800411FC  0x960  Party list navigation handler (lbl_803A67E8, lbl_803A6818)
 * fn_80041B5C  0x074  Party list sub-handler (calls fn_800411FC)
 * fn_80041BD0  0x278  Party list display
 * fn_80041E48  0x810  Party select state machine (lbl_803A67FC, lbl_803A6818)
 * fn_80042658  0x10D0 Party list main handler (4304 bytes, calls fn_80043728, fn_800478B4, fn_80047CC0)
 * fn_80043728  0x294  Party swap handler
 * fn_800439BC  0x338  Party data refresh
 * fn_80043CD8  0x0E8  Party status icon updater
 * fn_80043DC0  0x108  Party HP bar updater
 * fn_80043EC8  0x0E0  Party EXP bar updater
 * fn_80043FA8  0x3D0  Party item display
 * fn_80044378  0x2B8  Party action handler (calls fn_800492CC, fn_8004BDEC, fn_8004BDFC)
 * fn_80044630  0x1B38 Summary screen main update (6968 bytes, calls fn_80046168, fn_80048918)
 * fn_80046168  0x1164 Summary input handler (4452 bytes)
 * fn_800472CC  0x114  Summary page navigator (calls fn_800473E0)
 * fn_800473E0  0x4D4  Summary page state machine
 * fn_800478B4  0x40C  Party cursor update
 * fn_80047CC0  0x7E4  Party cursor navigation (2020 bytes)
 * fn_800484A4  0x474  Party cursor visual
 * fn_80048918  0x9B4  Summary detail panel (2484 bytes)
 * fn_800492CC  0x304  Party selection finalize
 */


/* 0x8003D1FC | size: 0x2CC */
asm void fn_8003D1FC(void) { nofralloc
    #include "asm/GC6E01/nonmatching/menu_pokemon/fn_8003D1FC.s"
}

/* 0x80044630 | size: 0x1B38 — assembled real-C decompilation */
#pragma peephole off
void fn_80044630(void) {
    extern void  _threadSwitch(void);
    extern u32   fn_80102568(u32 a, u32 b, u32 c);
    extern u32   fn_8010264C(u32 a, u32 b);
    extern void  fn_800E01D0(void* dst, void* src);
    extern void  GSscene_SetMode(u32 mode);
    extern void  GScameraSetPerspective(void* cam, f32 a, f32 b, f32 c, f32 d);
    extern u8    lbl_803A6818[];

    extern const f32 lbl_80267150[];

    extern const f32 lbl_8047BC94;
    extern const f32 lbl_8047BC98;
    extern const f32 lbl_8047BCA8;
    extern const f32 lbl_8047BCB8;
    extern const f32 lbl_8047BCBC;
    extern const f32 lbl_8047BCC0;
    extern const f32 lbl_8047BCC4;
    extern const f32 lbl_8047BCC8;
    extern const f32 lbl_8047BCCC;
    extern const f32 lbl_8047BCF0;
    extern const f32 lbl_8047BD18;

    extern u32   lbl_8047A4E0;
    extern u32   lbl_8047A4E4;

    extern void  fn_80048918(void);
    extern void  fn_80046168(void);
    extern void  fn_8004BDB8(s32 a, s32 b);
    extern void* fn_801CBA0C(u32 id);
    extern void* fn_800F9318();
    extern void* fn_80113F48(void);
    extern void  fn_801CB9D8(void* h);
    extern void  fn_801CB954(void* h, s32 b);
    extern void  fn_800E4014(void* a, s32 b);
    extern void  fn_800EC9DC(f32 v);
    extern void  fn_800EC160(void* h, s32 b);
    extern void  fn_800E3C94(void* h, s32 b);
    extern void  fn_800E3BC0(void);
    extern void* fn_801DAC3C(void* h);
    extern void  fn_80176E0C(u32 a, u32 id, u32 c, u32 d);
    extern void  fn_8010A420(void* subobj);
    extern void  fn_801096F8(s32 a);
    extern u32   fn_800E202C(u32 flag);
    extern void  fn_800E24B0(void);
    extern void  fn_800E209C(u32 h);
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

    fn_80048918();
    tbl = lbl_80267150;
    S = (u8*)lbl_803A6818;

    S118 = S + 0x118;
    S124 = S + 0x124;
    S130 = S + 0x130;
    S218 = S + 0x218;

    while (1) {
        switch (*(s32*)(S + 0x1C)) {
        case 0: {   /* .L_800446F0 */
            fn_8004BDB8(-1, 1);
            fn_8010264C(0x35, 0);
            fn_8010264C(0x3b, 0);
            fn_8010264C(0x36, 0);
            fn_8010264C(0x37, 0);
            fn_8010264C(0x38, 0);
            fn_8010264C(0x39, 0);
            *(s32*)(S + 0x1c) = 1;
        } break;

        case 1: {   /* .L_80044750 */
            s32 r;
            r = (s32)fn_8010264C(0x3a, 1);
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
            fn_80046168();
        } break;

        case 4: {   /* .L_800447D4 */
            s32 r;
            fn_8010264C(0x96, 0);
            fn_8010264C(0xa1, 0);
            r = (s32)fn_8010264C(0xa2, 1);
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
            fn_8010264C(0x96, 0);
            r = (s32)fn_8010264C(0xa2, 1);
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
            r = (s32)fn_8010264C(0xce, 1);
            if (r < 0) {
                *(s32*)(S + 0x1c) = 0xc;
                *(f32*)(S + 0x50) = lbl_8047BC94;
                *(f32*)(S + 0x58) = lbl_8047BCBC;
                fn_80102568(0xce, 0, 0);
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                step = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (step > lbl_8047BC98)
                    step = lbl_8047BC98;
                if (step <= lbl_8047BCC8)
                    step = lbl_8047BCC8;
                nval = *(f32*)(S + 0x1DC) + step;
                tgt = *(f32*)(S + 0x1E0);
                rem = tgt - nval;
                *(f32*)(S + 0x1DC) = nval;
                if (step <= lbl_8047BC94)
                    step = -step;
                if (rem > lbl_8047BC94)
                    mag = rem;
                else
                    mag = -rem;
                if (mag <= step) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem <= lbl_8047BC94)
                        rem = -rem;
                    if (rem < lbl_8047BCBC)
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                _threadSwitch();
                goto L_80046054;
            }
            *(s32*)(S + 0x14) = 1;
            fn_80102568(0x35, 0, 1);
            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = fn_800F9318(fn_80113F48(), h);
                if (rv != 0)
                    fn_800E4014(rv, 0);
            }
            /* fall through into shared camera block (.L_80044B3C) */
        L_80044B3C: {
                void* cam;
                f32   persp3;
                f32   persp2;
                f32   persp1;
                f32   persp0;
                f32   rot[3];
                f32   pos[3];
                f32   mtx[3];

                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                cam = fn_801CBA0C(0x0D181000);
                *(void**)(S + 0x10C) = cam;
                *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), cam);
                fn_800EC9DC(lbl_8047BD18);
                fn_800EC160(*(void**)(S + 0x110), 1);
                fn_800E3C94(*(void**)(S + 0x110), 0);

                ((u32*)mtx)[0] = ((const u32*)tbl)[6];
                ((u32*)mtx)[1] = ((const u32*)tbl)[7];
                ((u32*)mtx)[2] = ((const u32*)tbl)[8];
                ((u32*)pos)[0] = ((const u32*)tbl)[9];
                ((u32*)pos)[1] = ((const u32*)tbl)[10];
                ((u32*)pos)[2] = ((const u32*)tbl)[11];
                cam = fn_800F9318(0x17, 0x0D741800);
                *(void**)(S + 0x114) = cam;
                GScameraGetPosition(cam, pos);
                GScameraGetRotation(cam, rot);
                mtx[0] = *(f32*)((u8*)cam + 0x100);
                mtx[1] = *(f32*)((u8*)cam + 0x104);
                mtx[2] = *(f32*)((u8*)cam + 0x108);
                GScameraGetPerspective(cam, &persp0, &persp1, &persp2, &persp3);
                GScameraSetPerspective(cam, persp0, persp1, persp2, persp3);
                fn_800E01D0(S118, pos);
                fn_800E01D0(S124, rot);
                fn_800E01D0(S130, mtx);

                *(f32*)(S + 0x13C) = persp0;
                *(f32*)(S + 0x140) = persp1;
                *(f32*)(S + 0x144) = persp2;
                *(f32*)(S + 0x148) = persp3;

                fn_80176E0C(0x17, 0x0D741800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 1;
                fn_8004BDB8(-1, 2);

                {
                    s32 sub = *(s32*)(S + 0x28);
                    *(f32*)(S + 0x6C) = lbl_8047BC94;
                    *(f32*)(S + 0x70) = lbl_8047BC94;
                    *(f32*)(S + 0x74) = lbl_8047BC94;
                    *(f32*)(S + 0x64) = lbl_8047BCF0;
                    switch (sub) {
                    case 1:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 4;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            fn_800E3BC0();
                        *(f32*)(S + 0x64) = lbl_8047BCA8;
                        break;
                    case 2:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 5;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            fn_800E3BC0();
                        break;
                    case 0:
                        *(s32*)(S + 0x1C) = 0xD;
                        *(s32*)(S + 0x24) = 2;
                        *(f32*)(S + 0x60) = lbl_8047BC94;
                        if (fn_801DAC3C(*(void**)(S + 0xA0)) != 0)
                            fn_800E3BC0();
                        break;
                    default:
                        break;
                    }
                }

                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
                goto L_80046054;
            }
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                mag = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (mag > lbl_8047BC98)
                    mag = lbl_8047BC98;
                if (mag <= lbl_8047BCC8)
                    mag = lbl_8047BCC8;
                nv = *(f32*)(S + 0x1DC) + mag;
                rem = *(f32*)(S + 0x1E0) - nv;
                *(f32*)(S + 0x1DC) = nv;
                if (mag > lbl_8047BC94) {
                } else {
                    mag = -mag;
                }
                absmag = mag;
                if (rem > lbl_8047BC94)
                    absrem = rem;
                else
                    absrem = -rem;
                if (absrem <= absmag) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94) {
                    } else {
                        rem = -rem;
                    }
                    if (rem >= lbl_8047BCBC)
                        ;
                    else
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                _threadSwitch();
            } else {
                *(s32*)(S + 0x1C) = 0;
                fn_80102568(0x3C, 0, 1);
                fn_80102568(0x96, 0, 1);
                fn_80102568(0xA2, 0, 1);

                h = *(void**)(S + 0x10C);
                fn_801CB954(h, 0);
                {
                    void* rv = fn_800F9318(fn_80113F48(), h);
                    if (rv != 0)
                        fn_800E4014(rv, 0);
                }
                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = fn_801CBA0C(0x0D171000);
                *(void**)(S + 0x10C) = h;
                *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), h);
                fn_800EC9DC(lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = fn_800F9318(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rot);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &staging0, &staging1, &staging2, &staging3);

                fn_800E01D0(S218, pos);
                fn_800E01D0(S218 + 0xC, mtx);
                *(f32*)(S218 + 0x18) = staging0;
                *(f32*)(S218 + 0x1C) = staging1;
                *(f32*)(S218 + 0x20) = staging2;
                *(f32*)(S218 + 0x24) = staging3;
                GScameraSetPerspective(h, staging0, staging1, staging2, staging3);

                fn_800E01D0(S118, pos);
                fn_800E01D0(S124, rot);
                fn_800E01D0(S130, mtx);

                *(f32*)(S + 0x13C) = staging0;
                *(f32*)(S + 0x140) = staging1;
                *(f32*)(S + 0x144) = staging2;
                *(f32*)(S + 0x148) = staging3;

                fn_80176E0C(0x17, 0x0D731800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 0;
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;
            }
        } break;

        case 9: {   /* .L_80045178 */
            f32 step;
            f32 cur;
            f32 tgt;
            f32 delta;
            f32 rem;
            f32 acur;
            f32 arem;
            f32 astep;
            f32 px3;
            f32 px2;
            f32 px1;
            f32 px0;
            f32 rot[3];
            f32 pos[3];
            f32 mtx[3];

            /* ease block A */
            tgt = *(f32*)(S + 0x50);
            cur = *(f32*)(S + 0x4C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    cur = cur + step;
                    *(f32*)(S + 0x4C) = cur;
                    if (cur > lbl_8047BCBC)
                        *(f32*)(S + 0x4C) = lbl_8047BCBC;
                } else {
                    cur = cur - step;
                    *(f32*)(S + 0x4C) = cur;
                    if (cur < lbl_8047BC94)
                        *(f32*)(S + 0x4C) = lbl_8047BC94;
                }
            }

            /* ease block B */
            tgt = *(f32*)(S + 0x58);
            cur = *(f32*)(S + 0x54);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCB8;
                if (tgt > cur) {
                    cur = cur + step;
                    *(f32*)(S + 0x54) = cur;
                    if (cur > lbl_8047BCBC)
                        *(f32*)(S + 0x54) = lbl_8047BCBC;
                } else {
                    cur = cur - step;
                    *(f32*)(S + 0x54) = cur;
                    if (cur < lbl_8047BC94)
                        *(f32*)(S + 0x54) = lbl_8047BC94;
                }
            }

            /* ease block C */
            tgt = *(f32*)(S + 0x60);
            cur = *(f32*)(S + 0x5C);
            if (tgt != cur) {
                step = *(f32*)(S + 0x3C) / lbl_8047BCC0;
                if (tgt > cur) {
                    cur = cur + step;
                    *(f32*)(S + 0x5C) = cur;
                    if (cur > lbl_8047BCBC)
                        *(f32*)(S + 0x5C) = lbl_8047BCBC;
                } else {
                    cur = cur - step;
                    *(f32*)(S + 0x5C) = cur;
                    if (cur < lbl_8047BC94)
                        *(f32*)(S + 0x5C) = lbl_8047BC94;
                }
            }

            /* ease block D */
            acur = *(f32*)(S + 0x1DC);
            tgt  = *(f32*)(S + 0x1E0);
            if (acur != tgt) {
                delta = tgt - acur;
                astep = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (astep > lbl_8047BC98)
                    astep = lbl_8047BC98;
                if (astep <= lbl_8047BCC8)
                    astep = lbl_8047BCC8;
                acur = *(f32*)(S + 0x1DC) + astep;
                rem  = tgt - acur;
                *(f32*)(S + 0x1DC) = acur;
                if (astep > lbl_8047BC94) {
                } else {
                    astep = -astep;
                }
                if (rem > lbl_8047BC94)
                    arem = rem;
                else
                    arem = -rem;
                if (arem <= astep) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94)
                        rem = rem;
                    else
                        rem = -rem;
                    if (rem < lbl_8047BCBC)
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                goto L_80045548;
            }

            *(s32*)(S + 0x1C) = 0;
            fn_80102568(0xa1, 0, 1);
            fn_80102568(0x96, 0, 1);
            fn_80102568(0xa2, 0, 1);

            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = fn_800F9318(fn_80113F48(), h);
                if (rv != 0)
                    fn_800E4014(rv, 0);
            }

            /* .L_800453AC camera-setup */
            fn_801CB9D8(h);
            *(f32*)(S + 0x6C) = lbl_8047BC94;
            *(f32*)(S + 0x70) = lbl_8047BC94;
            *(f32*)(S + 0x74) = lbl_8047BC94;
            *(f32*)(S + 0x64) = lbl_8047BCF0;

            h = fn_801CBA0C(0x0D171000);
            *(void**)(S + 0x10C) = h;
            *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), h);
            fn_800EC9DC(lbl_8047BD18);

            ((u32*)mtx)[0] = ((const u32*)tbl)[0];
            ((u32*)mtx)[1] = ((const u32*)tbl)[1];
            ((u32*)mtx)[2] = ((const u32*)tbl)[2];
            ((u32*)pos)[0] = ((const u32*)tbl)[3];
            ((u32*)pos)[1] = ((const u32*)tbl)[4];
            ((u32*)pos)[2] = ((const u32*)tbl)[5];
            h = fn_800F9318(0x17, 0x0D731800);
            *(void**)(S + 0x114) = h;

            GScameraGetPosition(h, pos);
            GScameraGetRotation(h, rot);

            mtx[0] = *(f32*)((u8*)h + 0x100);
            mtx[1] = *(f32*)((u8*)h + 0x104);
            mtx[2] = *(f32*)((u8*)h + 0x108);
            GScameraGetPerspective(h, &px0, &px1, &px2, &px3);

            fn_800E01D0(S218, pos);
            fn_800E01D0(S218 + 0xC, mtx);

            *(f32*)(S218 + 0x18) = px0;
            *(f32*)(S218 + 0x1C) = px1;
            *(f32*)(S218 + 0x20) = px2;
            *(f32*)(S218 + 0x24) = px3;
            GScameraSetPerspective(h, px0, px1, px2, px3);

            fn_800E01D0(S118, pos);
            fn_800E01D0(S124, rot);
            fn_800E01D0(S130, mtx);

            *(f32*)(S + 0x13C) = px0;
            *(f32*)(S + 0x140) = px1;
            *(f32*)(S + 0x144) = px2;
            *(f32*)(S + 0x148) = px3;

            fn_80176E0C(0x17, 0x0D731800, 0, 1);
            GSscene_SetMode(4);

            *(s32*)(S + 0x78) = 0;
            *(s32*)(S + 0x14) = 0;
            *(f32*)(S + 0x50) = lbl_8047BCBC;
            *(f32*)(S + 0x58) = lbl_8047BC94;
            goto L_80046054;

        L_80045548:
            _threadSwitch();
            goto L_80046054;
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                gstep = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (gstep > lbl_8047BC98)
                    gstep = lbl_8047BC98;
                if (gstep <= lbl_8047BCC8)
                    gstep = lbl_8047BCC8;
                newcur = *(f32*)(S + 0x1DC) + gstep;
                rem    = *(f32*)(S + 0x1E0) - newcur;
                *(f32*)(S + 0x1DC) = newcur;
                if (gstep <= lbl_8047BC94)
                    gstep = -gstep;
                if (rem > lbl_8047BC94)
                    arem = rem;
                else
                    arem = -rem;
                if (arem <= gstep) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94)
                        arem = rem;
                    else
                        arem = -rem;
                    if (arem < lbl_8047BCBC)
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) != *(f32*)(S + 0x4C)) {
                goto L_80045910;
            }

            *(s32*)(S + 0x1C) = 0;
            fn_80102568(0x96, 0, 1);
            fn_80102568(0xA2, 0, 1);
            h = *(void**)(S + 0x10C);
            fn_801CB954(h, 0);
            {
                void* rv = fn_800F9318(fn_80113F48(), h);
                if (rv != 0)
                    fn_800E4014(rv, 0);
            }
            /* fall through into shared camera block (.L_80045774) */
        L_80045774: {
                f32 p3;
                f32 p2;
                f32 p1;
                f32 p0;
                f32 rotbuf[3];
                f32 pos[3];
                f32 mtx[3];

                fn_801CB9D8(h);

                *(f32*)(S + 0x6C) = lbl_8047BC94;
                *(f32*)(S + 0x70) = lbl_8047BC94;
                *(f32*)(S + 0x74) = lbl_8047BC94;
                *(f32*)(S + 0x64) = lbl_8047BCF0;

                h = fn_801CBA0C(0x0D171000);
                *(void**)(S + 0x10C) = h;

                *(void**)(S + 0x110) = fn_800F9318(fn_80113F48(), h);
                fn_800EC9DC(lbl_8047BD18);

                ((u32*)mtx)[0] = ((const u32*)tbl)[0];
                ((u32*)mtx)[1] = ((const u32*)tbl)[1];
                ((u32*)mtx)[2] = ((const u32*)tbl)[2];
                ((u32*)pos)[0] = ((const u32*)tbl)[3];
                ((u32*)pos)[1] = ((const u32*)tbl)[4];
                ((u32*)pos)[2] = ((const u32*)tbl)[5];
                h = fn_800F9318(0x17, 0x0D731800);
                *(void**)(S + 0x114) = h;

                GScameraGetPosition(h, pos);
                GScameraGetRotation(h, rotbuf);

                mtx[0] = *(f32*)((u8*)h + 0x100);
                mtx[1] = *(f32*)((u8*)h + 0x104);
                mtx[2] = *(f32*)((u8*)h + 0x108);
                GScameraGetPerspective(h, &p0, &p1, &p2, &p3);

                fn_800E01D0(S218, pos);
                fn_800E01D0(S218 + 0x0C, mtx);

                *(f32*)(S218 + 0x18) = p0;
                *(f32*)(S218 + 0x1C) = p1;
                *(f32*)(S218 + 0x20) = p2;
                *(f32*)(S218 + 0x24) = p3;
                GScameraSetPerspective(h, p0, p1, p2, p3);

                fn_800E01D0(S118, pos);
                fn_800E01D0(S124, rotbuf);
                fn_800E01D0(S130, mtx);

                *(f32*)(S + 0x13C) = p0;
                *(f32*)(S + 0x140) = p1;
                *(f32*)(S + 0x144) = p2;
                *(f32*)(S + 0x148) = p3;
                fn_80176E0C(0x17, 0x0D731800, 0, 1);
                GSscene_SetMode(4);

                *(s32*)(S + 0x78) = 0;
                *(s32*)(S + 0x14) = 0;
                *(f32*)(S + 0x50) = lbl_8047BCBC;
                *(f32*)(S + 0x58) = lbl_8047BC94;

                goto L_80046054;
            }

        L_80045910:
            _threadSwitch();
            goto L_80046054;
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                mag = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (mag > lbl_8047BC98)
                    mag = lbl_8047BC98;
                if (mag <= lbl_8047BCC8)
                    mag = lbl_8047BCC8;
                nv = *(f32*)(S + 0x1DC) + mag;
                rem = *(f32*)(S + 0x1E0) - nv;
                *(f32*)(S + 0x1DC) = nv;
                if (mag > lbl_8047BC94) {
                } else {
                    mag = -mag;
                }
                absmag = mag;
                if (rem > lbl_8047BC94)
                    absrem = rem;
                else
                    absrem = -rem;
                if (absrem <= absmag) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94) {
                    } else {
                        rem = -rem;
                    }
                    if (rem >= lbl_8047BCBC)
                        ;
                    else
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle test */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                fn_80102568(0x35, 0, 1);
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
            f32 dlt;
            f32 dstep;
            f32 ncur;
            f32 rem;
            f32 arem;
            f32 dtgt;

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
            cur = *(volatile f32*)(S + 0x1DC);
            dtgt = *(volatile f32*)(S + 0x1E0);
            if (cur != dtgt) {
                dlt = dtgt - cur;
                dstep = lbl_8047BCC4 * dlt;
                dstep = dstep * *(f32*)(S + 0x3C);
                if (dstep > lbl_8047BC98)
                    dstep = lbl_8047BC98;
                if (dstep <= lbl_8047BCC8)
                    dstep = lbl_8047BCC8;
                ncur = *(volatile f32*)(S + 0x1DC) + dstep;
                dtgt = *(volatile f32*)(S + 0x1E0);
                rem = dtgt - ncur;
                *(f32*)(S + 0x1DC) = ncur;
                if (dstep > lbl_8047BC94) {
                } else {
                    dstep = -dstep;
                }
                if (rem > lbl_8047BC94)
                    arem = rem;
                else
                    arem = -rem;
                if (arem <= dstep)
                    goto snapD;
                if (rem > lbl_8047BC94) {
                } else {
                    rem = -rem;
                }
                if (rem >= lbl_8047BCBC)
                    goto skipD;
            snapD:
                *(f32*)(S + 0x1DC) = dtgt;
            skipD:
                ;
            }

            /* converge */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0;
                fn_80102568(0x96, 0, 1);
                fn_80102568(0xA2, 0, 1);
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
            cur = *(f32*)(S + 0x1DC);
            tgt = *(f32*)(S + 0x1E0);
            if (cur != tgt) {
                delta = tgt - cur;
                mag = (lbl_8047BCC4 * delta) * *(f32*)(S + 0x3C);
                if (mag > lbl_8047BC98)
                    mag = lbl_8047BC98;
                if (mag <= lbl_8047BCC8)
                    mag = lbl_8047BCC8;
                nv = *(f32*)(S + 0x1DC) + mag;
                rem = *(f32*)(S + 0x1E0) - nv;
                *(f32*)(S + 0x1DC) = nv;
                if (mag > lbl_8047BC94) {
                } else {
                    mag = -mag;
                }
                absmag = mag;
                if (rem > lbl_8047BC94)
                    absrem = rem;
                else
                    absrem = -rem;
                if (absrem <= absmag) {
                    *(f32*)(S + 0x1DC) = tgt;
                } else {
                    if (rem > lbl_8047BC94) {
                    } else {
                        rem = -rem;
                    }
                    if (rem >= lbl_8047BCBC)
                        ;
                    else
                        *(f32*)(S + 0x1DC) = tgt;
                }
            }

            /* settle */
            if (*(f32*)(S + 0x50) == *(f32*)(S + 0x4C)) {
                *(s32*)(S + 0x1C) = 0x64;
            } else {
                _threadSwitch();
            }
        } break;

        case 0x64: { /* .L_80046040 */
            fn_80102568(0x35, 0, 1);
            running = 0;
        } break;

        default:
            break;
        }
        goto L_80046054;   /* normal switch exits skip the shared goto-blocks */

    L_80046054:
        if (running != 0)
            continue;
        break;
    }

    /* === EXIT / cleanup epilogue (.L_80046054 fall-through .. blr) === */
    h = *(void**)(S + 0x10C);
    fn_801CB954(h, 0);
    {
        void* rv = fn_800F9318(fn_80113F48(), h);
        if (rv != 0)
            fn_800E4014(rv, 0);
    }
    fn_801CB9D8(h);

    h = *(void**)((u8*)lbl_803A6818 + 0x150);
    fn_801CB954(h, 0);
    {
        void* rv = fn_800F9318(fn_80113F48(), h);
        if (rv != 0)
            fn_800E4014(rv, 0);
    }
    fn_801CB9D8(h);

    if (lbl_8047A4E0 != 0) {
        h = (void*)fn_800E202C(lbl_8047A4E0);
        if ((u16)(u32)h != 0) {
            fn_800E24B0();
            fn_800E209C((u32)h);
        }
        lbl_8047A4E0 = 0;
    }

    fn_801096F8(1);
    fn_8010A420((u8*)lbl_803A6818 + 0x7c);
    fn_8010A420((u8*)lbl_803A6818 + 0xc4);

    h = (void*)fn_800E202C(lbl_8047A4E4);
    if ((u16)(u32)h != 0) {
        fn_800E24B0();
        fn_800E209C((u32)h);
    }
    lbl_8047A4E4 = 0;
}
#pragma peephole on


