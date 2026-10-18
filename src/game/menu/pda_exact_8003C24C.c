#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x8b];
    u8 alphaByte;
} PdaSprite;

typedef struct PdaEvent {
    u8 pad00[6];
    s16 messageId;
} PdaEvent;

typedef struct PdaModelWindow {
    u8 pad00[0x44];
    f32 alphaScale;
} PdaModelWindow;

extern f32 lbl_8047BAC0;
extern PdaModelWindow lbl_803A6748;
extern void pdaSnagListDrawDetail(PdaSprite* sprite, PdaEvent* event);

void fn_8003C24C(PdaSprite* sprite, PdaEvent* event)
{
    switch (event->messageId) {
    case 0x2e2:
    case 0x2e4:
        sprite->alphaByte = lbl_8047BAC0 * lbl_803A6748.alphaScale;
        break;
    default:
        pdaSnagListDrawDetail(sprite, event);
        break;
    }
}
