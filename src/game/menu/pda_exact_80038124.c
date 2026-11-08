/** Byte-exact PDA value callbacks, 0x80038124 - 0x80038170. */
#include "dolphin/types.h"

typedef struct PdaSprite {
    u8 pad00[0x70];
    f32 value;
} PdaSprite;

extern f32 lbl_8047A478;
extern f32 lbl_8047A494;
extern f32 lbl_8047BA60;
extern f32 lbl_8047BA74;
extern f32 lbl_8047BA78;

void menu1FDrawItem(void* window, PdaSprite* sprite)
{
    sprite->value = lbl_8047BA74 - lbl_8047A478;
}

void menu1EDrawItem(void* window, PdaSprite* sprite)
{
    f32 new_var;
    int new_var4;
    f32 value;
    f32 new_var2;
    float new_var3;

    new_var2 = (0, lbl_8047A494);
    new_var3 = lbl_8047BA78 * new_var2;
    if (((!lbl_8047A478) && (!lbl_8047A478)) && (!lbl_8047A478)) {
    }
    value = lbl_8047A478 + new_var3;
    lbl_8047A478 = value;
    new_var = value;
    if (new_var4 = new_var > lbl_8047BA60) {
        lbl_8047A478 = lbl_8047BA60;
        lbl_8047A478 = value - lbl_8047A478;
    }
    sprite->value = lbl_8047A478;
}
