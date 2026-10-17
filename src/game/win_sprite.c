/**
 * @file win_sprite.c
 * @brief winSeq tail + winSprite: 0x80107170 - 0x801093C8.
 *
 * XD's winSprite.cpp holds both the winSeq sequence code and the winSprite
 * draw/alloc code (TeamOrre/xd-decomp config/GXXE01/symbols.txt @4989794,
 * _winSeqMoveSub through winSpriteGetLayerID). This unit is that TU from
 * fn_80107170 on; its head (fn_80106F98 .. 0x80107170) links as the
 * win_sequence.c / win_sequence_exact_801070F4.c carves, which reference the
 * two move targets defined here.
 *
 * It owns the TU's data: the .sdata2 literal pool 0x8047CE20-0x8047CE48
 * (100.0f, the two int->float doubles, 0.0f, 1.0f, 0.5f), _winSeqMoveSub's
 * jump table (.data 0x8035B400-0x8035B430) and the .bss work area
 * 0x80404B68-0x80404BF0: the sprite and window move targets, then the
 * texture quad's v/u and corner y/x arrays. The arrays are file statics,
 * so MWCC addresses them off the pooled .bss base (addi rN, base, off),
 * which is what gives winSpriteDrawTexture its per-array bases and loop
 * IVs; statics are laid out in reverse definition order.
 *
 * Built with GC/1.3.2 (GC/1.3 folds those pooled offsets into the store
 * displacements), -opt nopeephole and -inline auto,deferred: winSetSequence
 * is expanded into its callers yet still emitted out of line, and deferred
 * mode emits functions in reverse definition order, so they are defined
 * here from the highest address down. The move blocks (copy the drawable
 * fields into the shared target, run _winSeqMoveSub fn_800D3088() times,
 * copy back) are static inline helpers in win_sequence.h.
 *
 * windowSearchItemID, fn_801081F8 and fn_80107170 take u16 item ids and
 * winSetSequence a u16 id (windowSearchItemID and winSetSequence extend
 * their own argument; the callers pass the register through).
 * gfxVtxColor4u8 takes u8 colour components.
 */
#include "game/win_sequence.h"

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

extern void GSlogWrite(const char* fmt, ...);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void gfxSetFlags(s32);
extern void gfxClearFlags(s32);
extern void gfxSetBlendMode(s32, s32, s32);
extern void gfxSetPrimitive(s32);
extern void gfxSetVtxDesc(s32);
extern void gfxBegin(s32);
extern void gfxVtxColor4u8(s32, u8, u8, u8, u8);
extern void gfxEnd(void);
extern void* GSresFindById(u32);
extern void gfxLoadTexture(s32, void*);
extern u16 GStextureGetXsize(void*);
extern u16 GStextureGetYsize(void*);
extern void gfxVtxTexCoord(s32, f32, f32);
extern void gfxVtxPos2s16(s32, s32);
extern void fn_800E0718(void*, const void*, f32);
extern void set__5GSvecFfff(void*, f32, f32, f32);
extern void GSvecTransformQuat(void*, void*, void*);
extern u8 lbl_8031554C[];
extern u8 lbl_80314F98[];
extern u16 lbl_8047AD18;
extern u8 lbl_80271EE8[];
extern u8 lbl_80271F18[];
extern u8 lbl_8035B3F0[];

static f32 sQuadX[4];
static f32 sQuadY[4];
static f32 sQuadU[4];
static f32 sQuadV[4];
WinSeqTarget lbl_80404B8C;
WinSeqTarget lbl_80404B68;

void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite);

/* 0x80109358 | 0x70 */
void winSpriteInit(void) {
    u16 h = _toolentryAlloc__FUl(0x10000 - 0x5740);
    lbl_8047AD18 = h;
    if ((u16)h == 0) {
        GSlogWrite((const char*)lbl_80271F18, (const char*)lbl_8035B3F0);
    } else {
        void* ptr = GSmemLock((u16)h);
        lbl_8047AD1C = (tagSPRITE_WORK*)ptr;
        memset(ptr, 0, 0x10000 - 0x5740);
    }
}

/* 0x80109290 | 0xC8 */
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
                sprite->scale_x = 1.0f;
                sprite->scale_y = 1.0f;
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

/* 0x8010925C | 0x34 */
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

/* 0x80109220 | 0x3C */
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

/* 0x801091F4 | 0x2C | nc_getter_s8 -- returns 1 if bit 1 of ptr[0x4] is set */
s32 winSpriteGetDisp(void* ptr) {
    if (ptr == (void*)0) { return 0; }
    {
        s8 r0 = (s8)*((u8*)ptr + 0x4);
        s32 r3 = (s32)r0 & 2;   /* extsb then rlwinm r3, r0, 0, 30, 30 */
        s32 neg = -r3;
        return (u32)(neg | r3) >> 31;
    }
}

void winSpriteDraw(u8* context, WinSpriteDrawNode* sprite)
{
    extern void gfxSetAlphaCompare(s32, s32, s32, s32, s32, s32);
    extern void fn_800FE6D0(s16, s16);
    extern void spriteSetEnv(void);
    extern void windowDrawPanel(s16, s16, s16, s16);
    extern void fn_8001E644(s16, s16, s16, s16, u8);
    extern void gfxSetLineWidth(f32);
    extern void GSmsgPrintRect(s16, s16, s16, s16, u32, void*);
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
        gfxSetBlendMode(1, 6, 7);
        break;
    case 1:
        gfxSetAlphaCompare(1, 6, 0, 1, 0, 0);
        gfxSetBlendMode(1, 4, 0);
        break;
    case 2:
        gfxSetBlendMode(1, 6, 1);
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
            gfxSetBlendMode(1, 6, 7);
            break;
        case 1:
            gfxSetAlphaCompare(1, 6, 0, 1, 0, 0);
            gfxSetBlendMode(1, 4, 0);
            break;
        case 2:
            gfxSetBlendMode(1, 6, 1);
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
        gfxSetFlags(1);
        gfxClearFlags(6);
        switch (sprite->primitive) {
        case 0x10000:
            gfxSetPrimitive(3);
            gfxSetVtxDesc((s32)lbl_80314E08);
            gfxBegin(3);
            gfxVtxPos2s16(sprite->x, sprite->y);
            gfxVtxColor4u8(0, red, green, blue, alpha);
            gfxVtxPos2s16((s16)(sprite->x + sprite->crop_x),
                         (s16)(sprite->y + sprite->crop_y));
            gfxVtxColor4u8(0, red, green, blue, alpha);
            gfxVtxPos2s16((s16)(sprite->x + sprite->width),
                         (s16)(sprite->y + sprite->height));
            gfxVtxColor4u8(0, red, green, blue, alpha);
            gfxEnd();
            break;
        case 0x10001:
            windowDrawPanel(sprite->x, sprite->y, sprite->width, sprite->height);
            break;
        case 0x10004:
            fn_8001E644(sprite->x, sprite->y, sprite->width,
                         sprite->height, alpha);
            break;
        case 0x10002:
            red2 = (u8)(((u16)sprite->crop_x * red) / 255);
            green2 = (u8)(((u16)sprite->crop_y * green) / 255);
            blue2 = (u8)(((u16)sprite->crop_width * blue) / 255);
            gfxSetPrimitive(7);
            gfxSetVtxDesc((s32)lbl_80314E08);
            gfxBegin(2);
            gfxVtxPos2s16(sprite->x, sprite->y);
            gfxVtxColor4u8(0, red2, green2, blue2, alpha);
            gfxVtxPos2s16((s16)(sprite->x + sprite->width),
                         (s16)(sprite->y + sprite->height));
            gfxVtxColor4u8(0, red2, green2, blue2, alpha);
            gfxEnd();
            break;
        case 0x10003:
            red2 = (u8)(((u16)sprite->crop_x * red) / 255);
            green2 = (u8)(((u16)sprite->crop_y * green) / 255);
            blue2 = (u8)(((u16)sprite->crop_width * blue) / 255);
            gfxSetLineWidth(1.0f);
            gfxSetPrimitive(1);
            gfxSetVtxDesc((s32)lbl_80314E08);
            gfxBegin(2);
            gfxVtxPos2s16(sprite->x, sprite->y);
            gfxVtxColor4u8(0, red2, green2, blue2, alpha);
            gfxVtxPos2s16(sprite->width, sprite->height);
            gfxVtxColor4u8(0, red2, green2, blue2, alpha);
            gfxEnd();
            break;
        }
    }

    if (sprite->drawArg != NULL) {
        GSmsgPrintRect(sprite->x, sprite->y, sprite->width, sprite->height,
                    color, sprite->drawArg);
    }
}

#define WIN_ABS(v) ((v) < 0 ? -(v) : (v))

void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite)
{
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

    gfxSetFlags(3);
    gfxClearFlags(4);
    texture = GSresFindById(sprite->texture_id);
    if (texture != 0) {
        gfxLoadTexture(0, texture);
        if (sprite->crop_width != WIN_ABS(sprite->width)) {
            width = sprite->crop_width - 2;
            left = sprite->crop_x + 1;
            if (width < 0) {
                width = 0;
            }
        } else {
            left = sprite->crop_x;
            width = sprite->crop_width;
        }

        if (sprite->crop_height != WIN_ABS(sprite->height)) {
            height = sprite->crop_height - 2;
            top = sprite->crop_y + 1;
            if (height < 0) {
                height = 0;
            }
        } else {
            top = sprite->crop_y;
            height = sprite->crop_height;
        }

        sQuadU[0] = sQuadU[1] = (f32)left / (f32)GStextureGetXsize(texture);
        sQuadU[2] = sQuadU[3] = (f32)(left + width) / (f32)GStextureGetXsize(texture);
        sQuadV[0] = sQuadV[3] = (f32)(top + height) / (f32)GStextureGetYsize(texture);
        sQuadV[1] = sQuadV[2] = (f32)top / (f32)GStextureGetYsize(texture);
    } else {
        sQuadU[0] = sQuadU[1] = 0.0f;
        sQuadU[2] = sQuadU[3] = 1.0f;
        sQuadV[0] = sQuadV[3] = 1.0f;
        sQuadV[1] = sQuadV[2] = 0.0f;
    }

    center_x = (f32)sprite->x + (f32)WIN_ABS(sprite->width) / 2.0f;
    center_y = (f32)sprite->y + (f32)WIN_ABS(sprite->height) / 2.0f;

    if (sprite->width < 0) {
        sQuadX[0] = sQuadX[1] = (f32)(sprite->x + WIN_ABS(sprite->width)) - center_x;
        sQuadX[2] = sQuadX[3] = (f32)sprite->x - center_x;
    } else {
        sQuadX[0] = sQuadX[1] = (f32)sprite->x - center_x;
        sQuadX[2] = sQuadX[3] = (f32)(sprite->x + WIN_ABS(sprite->width)) - center_x;
    }

    if (sprite->height < 0) {
        sQuadY[0] = sQuadY[3] = (f32)sprite->y - center_y;
        sQuadY[1] = sQuadY[2] = (f32)(sprite->y + WIN_ABS(sprite->height)) - center_y;
    } else {
        sQuadY[0] = sQuadY[3] = (f32)(sprite->y + WIN_ABS(sprite->height)) - center_y;
        sQuadY[1] = sQuadY[2] = (f32)sprite->y - center_y;
    }

    fn_800E0718(quaternion, lbl_8031554C, sprite->rotation);
    for (i = 0; i < 4; i++) {
        set__5GSvecFfff(&point, sQuadX[i], 0.0f, sQuadY[i]);
        GSvecTransformQuat(&point, quaternion, &point);
        sQuadX[i] = point.x * sprite->scale_x;
        sQuadY[i] = point.z * sprite->scale_y;
    }

    gfxSetPrimitive(6);
    gfxSetVtxDesc((s32)lbl_80314F98);
    gfxBegin(4);
    for (i = 0; i < 4; i++) {
        gfxVtxPos2s16((s32)(center_x + sQuadX[i]), (s32)(center_y + sQuadY[i]));
        gfxVtxColor4u8(0, red, green, blue, alpha);
        gfxVtxTexCoord(0, sQuadU[i], sQuadV[i]);
    }
    gfxEnd();
}


/* 0x80108518 */
void winSetSequence(WinSeqState* state, u16 id) {
    if (id == 0) {
        state->commands = NULL;
        state->commandIndex = 0;
    } else {
        memset(state, 0, sizeof(WinSeqState));
        state->commands = menuSeqBiosGetPtr(id);
    }
}

/* 0x801081F8 */
void fn_801081F8(tagWINDOW_WORK* window, u16 itemId, u16 seqId) {
    tagSPRITE_WORK* sprite;
    s32 i;

    if (window == NULL) {
        for (i = 0; i < 0x168; i++) {
            sprite = &lbl_8047AD1C[i];
            if (sprite->flags != 0 && sprite->itemId == itemId) {
                winSetSequence(&sprite->sequence, seqId);
                winSeqMoveSprite(sprite);
            }
        }
    } else {
        sprite = windowSearchItemID(window, itemId);
        if (sprite == NULL) {
            return;
        }
        winSetSequence(&sprite->sequence, seqId);
        winSeqMoveSprite(sprite);
    }
}

/* 0x801080CC */
void winSeqSetMenu(s32 id, u32 seqId) {
    tagWINDOW_WORK* window = windowSearchID(id);

    if (window == NULL) {
        return;
    }
    winSetSequence(&window->menuSequence, seqId);
    winSeqMoveWindow(window);
}

/* 0x80107F38 */
void fn_80107F38(s32 id, u32 seqId) {
    tagWINDOW_WORK* window = windowSearchID(id);
    tagSPRITE_WORK* sprite;

    if (window == NULL) {
        return;
    }
    for (sprite = window->overlaySprites; sprite != NULL; sprite = sprite->next) {
        winSetSequence(&sprite->sequence, seqId);
        winSeqMoveSprite(sprite);
    }
}

/* 0x80107ED8 */
s32 winSeqIsCheck(s32 id, u32 seqId) {
    tagWINDOW_WORK* window = windowSearchID(id);

    if (window != NULL && window->menuSequence.commands == menuSeqBiosGetPtr((u16)seqId)) {
        return 1;
    }
    return 0;
}

/* 0x80107E78 */
s32 fn_80107E78(tagWINDOW_WORK* window, u16 itemId, u32 seqId) {
    tagSPRITE_WORK* sprite = windowSearchItemID(window, itemId);

    if (sprite != NULL && sprite->sequence.commands == menuSeqBiosGetPtr((u16)seqId)) {
        return 1;
    }
    return 0;
}

/* 0x801074D4 */
void _winSeqMoveSub(WinSeqTarget* target, WinSeqState* state) {
    WinSeqCommand* command;
    WinSeqSpriteData* sprite;
    f32 t;
    u8 stop;

    stop = 0;
    if (state->commands == NULL) {
        return;
    }

    if (state->positionMode != 0) {
        state->positionFrame++;
        switch (state->positionMode) {
        case 1:
            t = (f32)state->positionFrame / (f32)state->positionDuration;
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        case 2:
            t = ((f32)state->positionFrame * (f32)state->positionFrame /
                 (f32)state->positionDuration) /
                (f32)state->positionDuration;
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        case 3:
            {
                f64 durationRoot = sqrt((f64)state->positionDuration);
                t = (f32)(sqrt((f64)state->positionFrame) / durationRoot);
            }
            target->x = state->startX + t * (state->endX - state->startX);
            target->y = state->startY + t * (state->endY - state->startY);
            break;
        }
        if (state->positionFrame >= state->positionDuration) {
            state->positionMode = 0;
        }
    }

    if (state->colorMode != 0) {
        state->colorFrame++;
        switch (state->colorMode) {
        case 1:
            t = (f32)state->colorFrame / (f32)state->colorDuration;
            target->color.r = state->startColor[0] +
                               t * (state->endColor[0] - state->startColor[0]);
            target->color.g = state->startColor[1] +
                               t * (state->endColor[1] - state->startColor[1]);
            target->color.b = state->startColor[2] +
                               t * (state->endColor[2] - state->startColor[2]);
            target->color.a = state->startColor[3] +
                               t * (state->endColor[3] - state->startColor[3]);
            break;
        }
        if (state->colorFrame >= state->colorDuration) {
            state->colorMode = 0;
        }
    }

    if (state->scaleMode != 0) {
        state->scaleFrame++;
        switch (state->scaleMode) {
        case 1:
            t = (f32)state->scaleFrame / (f32)state->scaleDuration;
            target->scaleX = state->startScaleX +
                             t * (state->endScaleX - state->startScaleX);
            target->scaleY = state->startScaleY +
                             t * (state->endScaleY - state->startScaleY);
            break;
        }
        if (state->scaleFrame >= state->scaleDuration) {
            state->scaleMode = 0;
        }
    }

next_command:
    if (state->delay > 0) {
        state->delay--;
        return;
    }

    command = &state->commands[state->commandIndex];
    switch (command->type) {
    case 0:
        state->commands = NULL;
        state->commandIndex = 0;
        stop = 1;
        break;
    case 1:
        state->delay = command->duration;
        break;
    case 3:
    case 9:
        if (command->type == 9) {
            state->endX = target->baseX;
            state->endY = target->baseY;
        } else {
            switch (command->relative) {
            case 0:
                state->endX = (s16)command->value0;
                state->endY = (s16)command->value1;
                break;
            case 1:
                state->endX = (s16)(target->x + command->value0);
                state->endY = (s16)(target->y + command->value1);
                break;
            }
        }
        if ((command->keep & 1) != 0) {
            state->endX = target->x;
        }
        if ((command->keep & 2) != 0) {
            state->endY = target->y;
        }
        state->positionDuration = command->duration;
        if (state->positionDuration == 0) {
            target->x = state->endX;
            target->y = state->endY;
            state->positionMode = 0;
        } else {
            state->startX = target->x;
            state->startY = target->y;
            state->positionFrame = 0;
            switch (command->interp) {
            case 0:
                state->positionMode = 1;
                break;
            case 1:
                state->positionMode = 2;
                break;
            case 2:
                state->positionMode = 3;
                break;
            case 3:
                state->positionMode = 1;
                break;
            }
        }
        break;
    case 7:
        state->startColor[0] = target->color.r;
        state->startColor[1] = target->color.g;
        state->startColor[2] = target->color.b;
        state->startColor[3] = target->color.a;
        state->endColor[0] = (u32)command->value0 >> 24;
        state->endColor[1] = (command->value0 >> 16) & 0xFF;
        state->endColor[2] = (command->value0 >> 8) & 0xFF;
        state->endColor[3] = command->value0;
        state->colorFrame = 0;
        state->colorDuration = command->duration;
        state->colorMode = 1;
        if (state->colorDuration == 0) {
            target->color.r = state->endColor[0];
            target->color.g = state->endColor[1];
            target->color.b = state->endColor[2];
            target->color.a = state->endColor[3];
            state->colorMode = 0;
        }
        break;
    case 6:
        state->startScaleX = target->scaleX;
        state->startScaleY = target->scaleY;
        state->endScaleX = (f32)command->value0 / 100.0f;
        state->endScaleY = (f32)command->value1 / 100.0f;
        state->scaleFrame = 0;
        state->scaleDuration = command->duration;
        state->scaleMode = 1;
        if (state->scaleDuration == 0) {
            target->scaleX = state->endScaleX;
            target->scaleY = state->endScaleY;
            state->scaleMode = 0;
        }
        break;
    case 2:
        if (command->value0 != 0) {
            target->flags = (s8)(target->flags | 2);
        } else {
            target->flags = (s8)(target->flags & ~2);
        }
        break;
    case 11:
        if (command->value0 != 0) {
            state->enabled = 1;
        } else {
            state->enabled = 0;
        }
        break;
    case 10:
        if (state->loopActive == 0) {
            state->loopActive = 1;
            state->loopCount = command->duration;
        }
        if (command->relative == 0) {
            state->loopCount--;
        }
        if (state->loopCount < 0) {
            state->loopCount = 0;
            state->loopActive = 0;
        } else {
            state->commandIndex = (s16)command->value0;
            goto next_command;
        }
        break;
    case 8:
        sprite = menuSpriteBiosGetPtr(command->value0);
        target->image = sprite->image;
        target->imageX = sprite->x;
        target->imageY = sprite->y;
        target->imageW = sprite->width;
        target->imageH = sprite->height;
        break;
    }

    if (stop != 0) {
        return;
    }
    state->commandIndex++;
    goto next_command;
}

/* 0x801071D0 */
void winSeqMoveMenu(tagWINDOW_WORK* window) {
    tagSPRITE_WORK* sprite;

    winSeqMoveWindow(window);
    for (sprite = window->sprites; sprite != NULL; sprite = sprite->next) {
        winSeqMoveSprite(sprite);
    }
    for (sprite = window->overlaySprites; sprite != NULL; sprite = sprite->next) {
        winSeqMoveSprite(sprite);
    }
}

/* 0x80107170 */
s32 fn_80107170(s32 id, u16 itemId) {
    tagSPRITE_WORK* sprite = windowSearchItemID(windowSearchID(id), itemId);

    if (sprite != NULL && sprite->sequence.commands != NULL && sprite->sequence.enabled == 0) {
        return 1;
    }
    return 0;
}
