/**
 * @file win_sprite.c
 * @brief winSprite/winAnime -- sprite node list draw/alloc/release and GX
 *        texture draw (Colosseum "menu" UI subsystem). Split from the
 *        gs_model.c splitter bucket (address range 0x80108580 -
 *        0x80109664, 8 fns, 6 with real C bodies; winSpriteDrawTexture
 *        and winSpriteDraw at the segment head remain asm-only).
 *        Corresponds to XD's winSprite address run
 *        (0x8010D1A0-0x8010E8C0); anchors winSpriteGetDisp/SetDisp/
 *        Release/Add strictly monotonic with exact size matches.
 */
#include "dolphin/types.h"
#include "game/win_sprite.h"

/* ===== External SDK / engine functions ===== */
extern void* memset(void* dst, int val, u32 size);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void  GSlogWrite(const char* fmt, ...);         /* OSReport / GSlog */

/* GSmem */
extern u16   _toolentryAlloc__FUl(u32 size);                     /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);                   /* GSmemGetPtr */
extern void* fn_800E24B0(u16 handle);                   /* GSmemLock */
extern void  fn_800E209C(u16 handle);                   /* GSmemFree */

/* GSgfx state */
extern u8 lbl_8047AA80[];  /* GSgfx state pointer (via sda21) */

/* Matrix math */
extern void  PSMTXIdentity(void);                         /* MTXIdentity */
extern void  PSMTXCopy(void* mtxA, void* mtxB);      /* MTXConcat */
extern void  PSMTXMultVec(void* mtx, void* vec, void* out); /* MTXMultVec3 */

/* Model resource table (BSS) */
extern u8 lbl_80402518[];  /* model resource table -- 0x2400 bytes */

/* Global model system state block */
extern u8 lbl_80404ACC[];

typedef struct GSModelStateHeader {
    u16 count;          /* 0x00 */
    u16 entryHandle;    /* 0x02 */
    u32 unk_04;         /* 0x04 */
    void* entries;      /* 0x08 */
} GSModelStateHeader;

#define GS_MODEL_STATE ((GSModelStateHeader*)lbl_80404ACC)

/* Resource index table */
extern u32 lbl_80478B20;   /* max resource index (sda21) */
extern u8  lbl_80315690[]; /* resource table, 8-byte entries */

/* Additional externs used by various functions */
extern void  GSmodelFree(u32);     /* GSmem release/unref */
extern void  fn_800D2738(void);
extern void* menuDataBiosGetPtr(void);    /* linked list head */
extern void* menuItemBiosGetPtr(s16 idx); /* node by index */
extern void* menuSeBiosGetPtr(s32);
extern u16   fn_8005D798(void*, s32);
extern void* menuSpriteBiosGetPtr(s32);
extern int   fn_80166A28(u16);
extern s32   GSthreadGetCurrentThread(void);    /* poll/yield -- 0 if pending */
extern void  _threadSwitch(void);    /* yield */
extern u32   fn_800BE31C(void);    /* rand or tick */
extern u32   fn_800B8FD8(void*);   /* register fn, returns handle */
extern void  fn_800BD91C(s32, s32);
extern void  fn_800B8C58(s32);
extern void  GSgfxBeginBackFBCapture(u32, void*, void*);
extern u32   GStextureCreate(s32, s32, s32, s32, s32);
extern u32   GSmodelCanAnimate(u32);
extern void  GSmodelSetAnimIndex(u32, u32);
extern void  GSmodelSetAnimRate(u32, f32);
extern void  GSmodelStartAnimation(u32);
extern void  fn_801DB100(u32);
extern u32   OSGetTick(void);

extern u32 lbl_8047ACF0;
extern u32 lbl_8047ACF4;  /* function pointer for callback */
extern u32 lbl_8047ACF8;  /* saved tick */
extern u32 lbl_8047ACEC;  /* tick counter */
extern u32 lbl_8047ACE8;  /* tick base */
extern u8 lbl_80478B28;   /* max slot byte */
extern u8 lbl_8047AD20;
extern u8 lbl_8047AD21;
extern u8 lbl_8047AD22;
extern u8 lbl_8047AD23;
extern u8 lbl_8047AD24;
extern f32 lbl_8047AD2C;
extern f32 lbl_8047AD30;
extern f32 lbl_8047AD34;
extern f32 lbl_8047AD38;
extern f32 lbl_8047AD3C;
extern u32 lbl_8047AD28;
extern u16 lbl_8047AD18;  /* GSmem handle */
extern u8* lbl_8047AD1C;  /* object pool pointer */
extern f32 lbl_8047CDC0;  /* sdata2: float constant */
extern f32 lbl_8047CDC4;  /* sdata2: float constant */
extern u16 lbl_8047CDE0;  /* sdata2: */
extern u16 lbl_8047CDE4;  /* sdata2: */
extern f32 lbl_8047CD80;  /* sdata2: float constant */
extern f32 lbl_8047CD84;  /* sdata2: float constant */
extern f32 lbl_8047CD88;  /* sdata2: float constant */
extern f32 lbl_8047CD8C;  /* sdata2: float constant */
extern f32 lbl_8047CD90;  /* sdata2: float constant */
extern f32 lbl_8047CD94;  /* sdata2: float constant */
extern f64 lbl_8047CD98;  /* sdata2: double constant */
extern const f32 lbl_8047CE3C;  /* sdata2: float constant */
extern f32 lbl_8047CE50;  /* sdata2: float constant */
extern f32 lbl_8047CE5C;  /* sdata2: float constant */
extern f32 lbl_8047CE70;  /* sdata2: float constant */
extern u8  lbl_80404A98[];  /* table for display */
extern u8  lbl_80271E10[];  /* format string */
extern u8  lbl_80271E4C[];  /* format string */
extern u8  lbl_80271EE8[];  /* format string */
extern u8  lbl_80271F18[];  /* format string */
extern u8  lbl_8035B060[];  /* module name string */
extern u8  lbl_8035B3F0[];  /* module name string */

/* Additional external functions (not already declared above) */
extern void fn_800BF74C(void);
extern void fn_800D9ED8(s32);
extern void fn_800D88DC(s32);
extern void fn_800D888C(s32);
extern void fn_800D9B58(f32, f32, f32, f32);
extern void fn_800DA4C4(s32, s32, s32);
extern void fn_800DA2BC(s32, s32, s32);
extern void fn_800DA1E8(s32, s32, s32);
extern void fn_800DA028(s32);
extern void fn_800D6A00(s32);
extern void fn_800D7820(s32);
extern void fn_800D67BC(s32);
extern void fn_800D6680(f32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D6728(void);
extern void* fn_800F92D4(u32);
extern void fn_800D85D4(s32, void*);
extern u16 GStextureGetXsize(void*);
extern u16 GStextureGetYsize(void*);
extern void fn_800D59B8(s32, f32, f32);
extern void fn_800D61E4(s32, s32);
extern void fn_800E0718(void*, const void*, f32);
extern void set__5GSvecFfff(void*, f32, f32, f32);
extern void GSvecTransformQuat(void*, void*, void*);
extern u8 lbl_8031554C[];
extern u8 lbl_80314F98[];

/* Forward declarations for functions defined later in this TU */
extern u8    menuOffScreenCheckEnable(u8 param);
extern void  windowClose(void* ptr, u32 flags);
extern void* windowSearchID(s32 param);
extern s32   _menuCBOffScreen__FP9GStextureUlPv(void);
extern void  windowGetValue(s32 param);
extern void  windowCheckCursor(void* p, u8 flags);
extern void  windowDrawSprite2(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10);
extern u8    menuOffScreenFadeSync(u8 param);
extern void  menuOffScreenFadeSet(f32 f1, f32 f2);
extern u8    menuOffScreenCreate(u32 param);
extern void  menuOffScreenRelease(void);
extern u32   windowGetActiveID(void);
extern void* windowGetKeyInfo(void);
extern void* menuSeqBiosGetPtr(u32 idx);
extern void* windowSearchItemID(void* head, s32 key);
extern void  menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
extern u8    menuOffScreenSetPriority(u8 val);
extern u8    menuOffScreenSetDisp(u8 val);
extern u32   fn_800D3088(void);
extern u8    lbl_80404B68[];  /* scratch table for fn_80107F38, fn_801081F8 */
extern u8    lbl_80404B8C[];  /* scratch table for winSeqSetMenu */
extern u8    lbl_8047AD10;     /* resource request gate byte (sda21) - authoritative decl, use as-is */

/* Cross-segment prototypes for menu/window-engine functions defined in
 * sibling segment files split from the gs_model.c splitter bucket. */
extern void fn_801019F8(void);
extern void* kaisuuBiosGetMax(u32 index);
extern void* kaisuuBiosGetMin(u32 index);
extern u32 kaisuuGetKaisuu(u32 param);
extern void fn_80101B34(u32 param);
extern void fn_80101B88(u32 val);
extern void fn_80101B90(void);
extern void fn_80101D5C(void);
extern void fn_80101D8C(void);
extern void fn_80101FB8(u8 param);
extern void menuGetOffScreenFlag(void);
extern void menuReleaseOffScreen(f32 f1);
extern void menuCreateOffScreen(f32 param);
extern s32 menuGetSelectItemNum(void);
extern s32 menuGetCursorFromItemID(void* unused, u32 param);
extern void fn_801021F8(void* p, u32 val);
extern void menuSetDisp(void* p, u32 enable);
extern void* menuGetCursorItemID(void* p, u32 target);
extern s32 menuSetCursor(void* p, u32 val);
extern s32 menuGetCursor(void* p);
extern s32 menuCloseSync(void* p, u8 flag);
extern void menuCloseFloor(void);
extern void fn_801024E8(void);
extern void menuClose(s32 p);
extern s32 menuCloseCustom(void* p, u32 mode, u8 wait);
extern s32 menuIsCheck(s32 param);
extern void menuOpen(void* p, void* q);
extern void menuOpenCustom(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
extern void menuSetPosition(void* p, s16 a, s16 b);
extern void menuButtonNormal(void* p);
extern void menuPlaySe(void* p, void* q);
extern void fn_801034DC(void);
extern void _menuGetGcKeyInfo__FlPUs(void);
extern void _menuUpdateKeyInfo__FP15WINDOW_SYS_WORK(void);
extern void menuGetKeyInfo(void);
extern u8 menuGetEnablePort(void);
extern u8 menuSetEnablePort(u8 val);
extern void menuInit(void);
extern u32 cursorBiosGetPos(u16 idx);
extern u32 cursorBiosSetPos(u16 idx, u16* out);
extern void cursorBiosInit(void);
extern void fn_80103F74(void* head, u16 key, u32 data);
extern void* windowGetAllocPtr(void* ptr);
extern void* windowAllocMemory(void* p, s32 size);
extern void* windowGetFreeWork(void* ptr);
extern void windowSetParam(void* ptr, u32 idx, u32 val);
extern u32 windowGetParam(void* ptr, u32 idx);
extern void windowDrawSprite(void* p, void* a, void* b, u16 key, u32 data);
extern void windowDrawSprite2(void* r3, void* r4, s16 r5, s16 r6, s32 r7, s32 r8, s32 r9, s32 r10);
extern u8* windowGetCursorToItem(u8* arg);
extern void windowGetValue(s32 param);
extern s32 fn_801044D0(s32 param, u16* val);
extern void windowGetCursor(void);
extern void windowCheckCursor(void* p, u8 flags);
extern u32 windowGetActiveID(void);
extern void* windowSearchItemID(void* head, s32 key);
extern void* windowSearchID(s32 param);
extern void windowCloseMain(void* obj);
extern void windowClose(void* ptr, u32 flags);
extern void _windowCreateItemSprite__FP14tagWINDOW_WORK(void);
extern void windowCreateCursorSprite(void);
extern void windowOpen(void);
extern void _winCalcWindowSize__FlPC13MENU_ITEM_dd_PsPs(void);
extern void windowInit(u16 count);
extern void windowGetPortKeyInfo(void);
extern void* windowGetKeyInfo(void);
extern void fn_80105634(void);
extern void winMsgDraw(void);
extern void winMsgCtrl(void);
extern void winMsgButton(void* p);
extern void winMsgCloseLevelUpStatus(void);
extern void winMsgOpenLevelUpFiledStatus(void);
extern void winMsgOpenLevelUpStatus(void);
extern void winMsgCloseError(void);
extern void winMsgOpenError(void);
extern void winMsgCloseFight(void);
extern void winMsgCloseCheckFight(void);
extern void winMsgOpenFightNoWait(void);
extern void winMsgOpenFight(void);
extern void winMsgCheckField(void);
extern void winMsgCloseField(void);
extern void winMsgOpenFieldWithSE(void);
extern void winMsgOpenField(void);
extern void winMsgCheck(void);
extern void winMsgClose(void);
extern void winMsgOpenWithSE(void);
extern void winMsgOpen(void);
extern void fn_80106F98(void);
extern s32 winSeqCheckMove(s32 param);
extern s32 fn_80107170(s32 r3, s32 r31);
extern void winSeqMoveMenu(void);
extern s32 fn_80107E78(void* r3, s32 r4, u16 r30);
extern s32 winSeqIsCheck(s32 r3, u16 r30);
extern void fn_80107F38(s32 param, u32 key);
extern void winSeqSetMenu(s32 param, u32 key);
extern void fn_801081F8(void* r3_arg, u16 r4, u16 r5);
extern void winSetSequence(void* out, u32 idx);
extern s32 winSpriteGetDisp(void* ptr);
extern void winSpriteRelease(void* head);
extern void winSpriteInit(void);
extern void fn_801093C8(void);
extern u8 menuOffScreenFadeSync(u8 param);
extern void menuOffScreenFadeSet(f32 f1, f32 f2);
extern u8 menuOffScreenSetPriority(u8 val);
extern u8 menuOffScreenSetDisp(u8 val);
extern u8 menuOffScreenIsDoing(void);
extern u32 menuOffScreenGetPtr(void);
extern u8 menuOffScreenCheckEnable(u8 param);
extern void menuOffScreenRelease(void);
extern u8 menuOffScreenCreate(u32 param);
extern void menuOffScreenInit(void);
extern s32 _menuCBOffScreen__FP9GStextureUlPv(void);
extern s32 menuModelSetMotion(void* p, u32 val);
extern void menuModelRender(void);
extern s32 menuModelCheck(void* obj, u8 wait);
extern s32 menuModelFree(void* p);

typedef struct WinSpriteDrawNode {
    struct WinSpriteDrawNode* next;
    s8 flags;
    u8 drawFlags;
    u8 pad_06[2];
    u32 primitive;
    u8 pad_0C[0x48 - 0x0C];
    void (*drawCallback)(u8*, struct WinSpriteDrawNode*);
    void* drawArg;
    s16 x;
    s16 y;
    s16 width;
    s16 height;
    u32 texture_id;
    s16 crop_x;
    s16 crop_y;
    s16 crop_width;
    s16 crop_height;
    union {
        u8 color[4];
        u32 rgba;
    };
    f32 scale_x;
    f32 scale_y;
    f32 rotation;
    u8 kind;
} WinSpriteDrawNode;

typedef struct WinSpriteVec3 {
    f32 x;
    f32 y;
    f32 z;
} WinSpriteVec3;

void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite);

#if defined(WIN_SPRITE_EXACT_80108C14)
#pragma push
#pragma peephole off
void winSpriteDraw(u8* context, WinSpriteDrawNode* sprite)
{
    extern void fn_800DA100(s32, s32, s32, s32, s32, s32);
    extern void fn_800FE6D0(s16, s16);
    extern void spriteSetEnv(void);
    extern void fn_8001EA98(s16, s16, s16, s16);
    extern void fn_8001E644(s16, s16, s16, s16, u8);
    extern void fn_800D5648(f32);
    extern void fn_800FBB34(s16, s16, s16, s16, u32, void*);
    extern u8 lbl_80314E08[];
    u8 red;
    u8 red2;
    u8 green;
    u8 blue;
    u8 alpha;
    u8 green2;
    u8 blue2;
    u32 color;
    s32 drawFlag;
    s32 visible;
    u32 displayFlag;

    switch (sprite->kind) {
    case 0:
        fn_800DA4C4(1, 6, 7);
        break;
    case 1:
        fn_800DA100(1, 6, 0, 1, 0, 0);
        fn_800DA4C4(1, 4, 0);
        break;
    case 2:
        fn_800DA4C4(1, 6, 1);
        break;
    }

    drawFlag = sprite->drawFlags & 8;
    if (drawFlag != 0) {
        fn_800FE6D0((s16)(*(s16*)(context + 0x84) + sprite->x),
                    (s16)(*(s16*)(context + 0x86) + sprite->y));
        spriteSetEnv();
        sprite->drawCallback(context, sprite);
        fn_800FE6D0(*(s16*)(context + 0x84), *(s16*)(context + 0x86));
        spriteSetEnv();
        switch (sprite->kind) {
        case 0:
            fn_800DA4C4(1, 6, 7);
            break;
        case 1:
            fn_800DA100(1, 6, 0, 1, 0, 0);
            fn_800DA4C4(1, 4, 0);
            break;
        case 2:
            fn_800DA4C4(1, 6, 1);
            break;
        }
    }

    if (sprite == NULL) {
        visible = 0;
    } else {
        s8 spriteFlags = sprite->flags;
        if (((s32)spriteFlags & 2) != 0) {
            visible = 1;
        } else {
            visible = 0;
        }
    }
    displayFlag = (u8)visible;
    if (displayFlag == 0) {
        return;
    }

    red = (u8)((sprite->color[0] * context[0x88]) / 255);
    green = (u8)((sprite->color[1] * context[0x89]) / 255);
    blue = (u8)((sprite->color[2] * context[0x8A]) / 255);
    alpha = (u8)((sprite->color[3] * context[0x8B]) / 255);
    color = (red << 24) | (green << 16) | (blue << 8) | alpha;
    if (alpha == 0) {
        return;
    }

    drawFlag = sprite->drawFlags & 1;
    if (drawFlag != 0) {
        winSpriteDrawTexture(context, sprite);
    }

    drawFlag = sprite->drawFlags & 2;
    if (drawFlag != 0) {
        fn_800D88DC(1);
        fn_800D888C(6);
        switch (sprite->primitive) {
        case 0x10000:
            fn_800D6A00(3);
            fn_800D7820((s32)lbl_80314E08);
            fn_800D67BC(3);
            fn_800D61E4(sprite->x, sprite->y);
            fn_800D5CB8(0, red, green, blue, alpha);
            fn_800D61E4((s16)(sprite->x + sprite->crop_x),
                         (s16)(sprite->y + sprite->crop_y));
            fn_800D5CB8(0, red, green, blue, alpha);
            fn_800D61E4((s16)(sprite->x + sprite->width),
                         (s16)(sprite->y + sprite->height));
            fn_800D5CB8(0, red, green, blue, alpha);
            fn_800D6728();
            break;
        case 0x10001:
            fn_8001EA98(sprite->x, sprite->y, sprite->width, sprite->height);
            break;
        case 0x10004:
            fn_8001E644(sprite->x, sprite->y, sprite->width,
                         sprite->height, alpha);
            break;
        case 0x10002:
            red2 = (u8)(((u16)sprite->crop_x * red) / 255);
            green2 = (u8)(((u16)sprite->crop_y * green) / 255);
            blue2 = (u8)(((u16)sprite->crop_width * blue) / 255);
            fn_800D6A00(7);
            fn_800D7820((s32)lbl_80314E08);
            fn_800D67BC(2);
            fn_800D61E4(sprite->x, sprite->y);
            fn_800D5CB8(0, red2, green2, blue2, alpha);
            fn_800D61E4((s16)(sprite->x + sprite->width),
                         (s16)(sprite->y + sprite->height));
            fn_800D5CB8(0, red2, green2, blue2, alpha);
            fn_800D6728();
            break;
        case 0x10003:
            red2 = (u8)(((u16)sprite->crop_x * red) / 255);
            green2 = (u8)(((u16)sprite->crop_y * green) / 255);
            blue2 = (u8)(((u16)sprite->crop_width * blue) / 255);
            fn_800D5648(lbl_8047CE3C);
            fn_800D6A00(1);
            fn_800D7820((s32)lbl_80314E08);
            fn_800D67BC(2);
            fn_800D61E4(sprite->x, sprite->y);
            fn_800D5CB8(0, red2, green2, blue2, alpha);
            fn_800D61E4(sprite->width, sprite->height);
            fn_800D5CB8(0, red2, green2, blue2, alpha);
            fn_800D6728();
            break;
        }
    }

    if (sprite->drawArg != NULL) {
        fn_800FBB34(sprite->x, sprite->y, sprite->width, sprite->height,
                    color, sprite->drawArg);
    }
}
#pragma pop
#endif

#if defined(WIN_SPRITE_PREFIX_80108580)
void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite)
{
    f32* tex_v = (f32*)(lbl_80404B68 + 0x48);
    f32* tex_u = (f32*)(lbl_80404B68 + 0x58);
    f32* quad_y = (f32*)(lbl_80404B68 + 0x68);
    f32* quad_x = (f32*)(lbl_80404B68 + 0x78);
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
    void* texture;
    s16 left;
    s16 top;
    s16 width;
    s16 height;
    f32 center_x;
    f32 center_y;
    f32 quaternion[4];
    WinSpriteVec3 point;
    s32 i;

    red = (u8)((sprite->color[0] * context[0x88]) / 255);
    green = (u8)((sprite->color[1] * context[0x89]) / 255);
    blue = (u8)((sprite->color[2] * context[0x8A]) / 255);
    alpha = (u8)((sprite->color[3] * context[0x8B]) / 255);

    fn_800D88DC(3);
    fn_800D888C(4);
    texture = fn_800F92D4(sprite->texture_id);
    if (texture != 0) {
        u16 texture_width;
        u16 texture_height;

        fn_800D85D4(0, texture);
        width = sprite->width < 0 ? -sprite->width : sprite->width;
        if (sprite->crop_width != width) {
            width = sprite->crop_width - 2;
            left = sprite->crop_x + 1;
            if (width < 0) {
                width = 0;
            }
        } else {
            left = sprite->crop_x;
            width = sprite->crop_width;
        }

        height = sprite->height < 0 ? -sprite->height : sprite->height;
        if (sprite->crop_height != height) {
            height = sprite->crop_height - 2;
            top = sprite->crop_y + 1;
            if (height < 0) {
                height = 0;
            }
        } else {
            top = sprite->crop_y;
            height = sprite->crop_height;
        }

        texture_width = GStextureGetXsize(texture);
        tex_u[0] = tex_u[1] = (f32)left / (f32)texture_width;
        texture_width = GStextureGetXsize(texture);
        tex_u[2] = tex_u[3] = (f32)(left + width) / (f32)texture_width;
        texture_height = GStextureGetYsize(texture);
        tex_v[0] = tex_v[3] = (f32)(top + height) / (f32)texture_height;
        texture_height = GStextureGetYsize(texture);
        tex_v[1] = tex_v[2] = (f32)top / (f32)texture_height;
    } else {
        tex_u[0] = tex_u[1] = 0.0f;
        tex_u[2] = tex_u[3] = 1.0f;
        tex_v[0] = tex_v[3] = 1.0f;
        tex_v[1] = tex_v[2] = 0.0f;
    }

    width = sprite->width < 0 ? -sprite->width : sprite->width;
    height = sprite->height < 0 ? -sprite->height : sprite->height;
    center_x = (f32)sprite->x + (f32)width * 0.5f;
    center_y = (f32)sprite->y + (f32)height * 0.5f;

    if (sprite->width < 0) {
        quad_x[0] = quad_x[1] = (f32)(sprite->x + width) - center_x;
        quad_x[2] = quad_x[3] = (f32)sprite->x - center_x;
    } else {
        quad_x[0] = quad_x[1] = (f32)sprite->x - center_x;
        quad_x[2] = quad_x[3] = (f32)(sprite->x + width) - center_x;
    }

    if (sprite->height < 0) {
        quad_y[0] = quad_y[3] = (f32)sprite->y - center_y;
        quad_y[1] = quad_y[2] = (f32)(sprite->y + height) - center_y;
    } else {
        quad_y[0] = quad_y[3] = (f32)(sprite->y + height) - center_y;
        quad_y[1] = quad_y[2] = (f32)sprite->y - center_y;
    }

    fn_800E0718(quaternion, lbl_8031554C, sprite->rotation);
    for (i = 0; i < 4; i++) {
        set__5GSvecFfff(&point, quad_x[i], 0.0f, quad_y[i]);
        GSvecTransformQuat(&point, quaternion, &point);
        quad_x[i] = point.x * sprite->scale_x;
        quad_y[i] = point.z * sprite->scale_y;
    }

    fn_800D6A00(6);
    fn_800D7820((s32)lbl_80314F98);
    fn_800D67BC(4);
    for (i = 0; i < 4; i++) {
        fn_800D61E4((s32)(center_x + quad_x[i]),
                    (s32)(center_y + quad_y[i]));
        fn_800D5CB8(0, red, green, blue, alpha);
        fn_800D59B8(0, tex_u[i], tex_v[i]);
    }
    fn_800D6728();
}
#endif

#if defined(WIN_SPRITE_EXACT_80108C14)
/* 0x801091F4 | 0x2C | nc_getter_s8 -- returns 1 if bit 1 of ptr[0x4] is set */
#pragma push
#pragma peephole off
s32 winSpriteGetDisp(void* ptr) {
    if (ptr == (void*)0) { return 0; }
    {
        s8 r0 = (s8)*((u8*)ptr + 0x4);
        s32 r3 = (s32)r0 & 2;   /* extsb then rlwinm r3, r0, 0, 30, 30 */
        s32 neg = -r3;
        return (u32)(neg | r3) >> 31;
    }
}
#pragma pop

/* 0x80109220 | 0x3C */
#pragma push
#pragma peephole off
#pragma scheduling off
void winSpriteSetDisp(void* node, u32 enable) {
    if (node == (void*)0) { return; }
    if ((u8)enable != 0) {
        s8 r0 = (s8)(*(u8*)((u8*)node + 0x4) | 2);
        *(s8*)((u8*)node + 0x4) = r0;
    } else {
        s8 r0 = (s8)(*(u8*)((u8*)node + 0x4) & ~2);
        *(s8*)((u8*)node + 0x4) = r0;
    }
}
#pragma pop

/* 0x8010925C | 0x34 */
#pragma push
#pragma peephole off
void winSpriteRelease(void* head) {
    if (head == (void*)0) { return; }
    {
        u8 r0 = 0;
        void* r4;
        void* r5 = head;
        while ((r4 = *(void**)r5) != (void*)0) {
            *(u8*)((u8*)r4 + 0x4) = r0;
            r5 = *(void**)r5;
        }
        *(u32*)head = 0;
    }
}
#pragma pop

/* 0x80109290 | 0xC8 */
#pragma push
#pragma peephole off
WinSpriteDrawNode* winSpriteAdd(WinSpriteDrawNode* root) {
    WinSpriteDrawNode* list = root;
    if (list == NULL) { return NULL; }
    {
        WinSpriteDrawNode* sprite = (WinSpriteDrawNode*)lbl_8047AD1C;
        s32 count = 0x168;
        while (count-- > 0) {
            s8 flags = sprite->flags;
            if (flags == 0) {
                WinSpriteDrawNode* current;

                memset(sprite, 0, sizeof(*sprite));
                sprite->flags = 7;
                sprite->scale_x = lbl_8047CE3C;
                sprite->scale_y = lbl_8047CE3C;
                sprite->rgba = -1;
                current = list;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = sprite;
                return sprite;
            }
            sprite++;
        }
        GSlogWrite((const char*)lbl_80271EE8);
        return NULL;
    }
}
#pragma pop

/* 0x80109358 | 0x70 */
#pragma push
#pragma peephole off
void winSpriteInit(void) {
    u16 h = _toolentryAlloc__FUl(0x10000 - 0x5740);
    lbl_8047AD18 = h;
    if ((u16)h == 0) {
        GSlogWrite((const char*)lbl_80271F18, (const char*)lbl_8035B3F0);
    } else {
        void* ptr = fn_800E27B0((u16)h);
        lbl_8047AD1C = (u8*)ptr;
        memset(ptr, 0, 0x10000 - 0x5740);
    }
}
#pragma pop
#endif

#if defined(WIN_SPRITE_SUFFIX_801093C8)
/* 0x801093C8 | 0x29C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_801093C8(void) {
    /* TODO: match -- 668 bytes at 0x801093C8 */
}
#pragma pop
#endif
