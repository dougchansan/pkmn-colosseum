#include "dolphin/types.h"

extern f64 lbl_8047BE28;
extern f64 lbl_8047BE30;

typedef struct PdaMailSpinWork {
    u8 pad00[0x70];
    f32 angle;
} PdaMailSpinWork;

s32 pdaMailAttachDrawItem(void* window, PdaMailSpinWork* sprite)
{
    f64 new_var2;
    f64 wrap = lbl_8047BE30;
    f64 new_var;

    /* The local literal preserves MWCC's f1 allocation; the build imports it onto the retail 2pi symbol. */
    wrap = 6.283185307179586;
    new_var = wrap;
    if ((sprite->angle += lbl_8047BE28) >= new_var) {
        new_var2 = wrap;
        sprite->angle -= (wrap, new_var2);
    }
    return 0;
}
