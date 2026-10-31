/**
 * @file window_exact_80104160.c
 * @brief windowDrawSprite2 (0x80104160 - 0x80104318): draws one menu-sprite
 *        BIOS entry at a position/size through winSpriteDraw, with a
 *        throw-away draw context when none is given.
 *
 * Function-boundary carve of the window TU, built with its flags (GC/2.5,
 * -O4,p, "-opt nopeephole"; see window_exact_80104318.c). Text only.
 *
 * The node layout is winSpriteDraw's (win_sprite.c). The two scales read the
 * window TU's own 1.0f pool literal (0x8047CDEC, shared with windowOpen)
 * through the same extern stand-in window_r50_80104CA0_suffix.c uses.
 */
#include "dolphin/types.h"

extern void* memset(void* dst, int val, u32 size);
extern void spriteGetOrigin(s16* x, s16* y);
/* RULE-EXCEPTION(title-path): extern stand-in for the window TU's pool literal - see docs/RULE_EXCEPTIONS.md */
extern const f32 lbl_8047CDEC;

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

typedef struct WinSpriteContext {
    u8 pad_00[0x84];
    s16 x;
    s16 y;
    s32 layer;
    u8 pad_8C[0xB4 - 0x8C];
} WinSpriteContext;

typedef struct WinSpriteInfo {
    u8 pad_bits : 2;
    u8 type : 2;
    u8 pad_bits2 : 4;
    u8 pad_01[4];
    s8 offsetX;
    s8 offsetY;
    u8 alpha;
    s16 cropX;
    s16 cropY;
    s16 cropW;
    s16 cropH;
    u32 texture;
} WinSpriteInfo;

extern WinSpriteInfo* menuSpriteBiosGetPtr(u16 id);

extern void winSpriteDraw(WinSpriteContext* context, WinSpriteDrawNode* sprite);

/* 0x80104160 | 0x1B8 */
void windowDrawSprite2(s16 x, s16 y, s16 width, s16 height, u32 color,
                       WinSpriteContext* context, s32 spriteId, u32 flags) {
    WinSpriteDrawNode sprite;
    WinSpriteDrawNode* sp = &sprite; /* RULE-EXCEPTION(title-path): pointer copy only for retail's r30 - see docs/RULE_EXCEPTIONS.md */
    WinSpriteContext localContext;
    WinSpriteInfo* info;

    info = menuSpriteBiosGetPtr((u16)spriteId);
    memset(sp, 0, sizeof(sprite));
    sprite.flags = 7;
    sprite.rgba = color;
    sprite.scale_x = lbl_8047CDEC;
    sprite.scale_y = lbl_8047CDEC;
    if (info->type == 1) {
        sprite.drawFlags |= 1;
        sprite.texture_id = info->texture;
    } else if (info->type == 2) {
        sprite.drawFlags |= 2;
        sprite.primitive = info->texture;
    }
    sprite.x = x + info->offsetX;
    sprite.y = y + info->offsetY;
    sprite.width = width;
    sprite.height = height;
    sprite.crop_x = info->cropX;
    sprite.crop_y = info->cropY;
    sprite.crop_width = info->cropW;
    sprite.crop_height = info->cropH;
    sprite.color[3] = (info->alpha * sprite.color[3]) / 255;
    if (flags & 1) {
        sprite.width = -(s16)width; /* RULE-EXCEPTION(title-path): no-op cast, only for retail's extsh - see docs/RULE_EXCEPTIONS.md */
    }
    if (flags & 2) {
        sprite.height = -sprite.height;
    }
    if (context == NULL) {
        context = &localContext;
        memset(context, 0, sizeof(WinSpriteContext));
        spriteGetOrigin(&context->x, &context->y);
        context->layer = -1;
    }
    winSpriteDraw(context, sp);
}
