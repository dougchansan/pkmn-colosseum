/* Name-entry animation callbacks, 0x80026740-0x800268F0.
 * RULE-EXCEPTION(user-approved): retained allocation-shaped locals;
 * see docs/RULE_EXCEPTIONS.md. Compiler controls are at unit scope.
 */
#include "dolphin/types.h"

extern f32 lbl_8047B938;
extern f32 lbl_8047B934;

s32 menuNameEntryDrawItem11(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];   /* state-machine entry table: each entry 16 bytes */
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 7) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x48));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}

s32 menuNameEntryDrawItem12(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 8) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x44));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}

s32 menuNameEntryDrawItem13(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    s32  x;
    f32  one;
    f32  new_var;
    void* ctx;
    f32  scale;
    u8* entry;

    ctx = *(void**)((u8*)r3 + 0x60);
    entry = lbl_80266DD8;
    index = *(s32*)((u8*)ctx + 0x1c);
    entry += index << 4;
    state = *(s32*)(entry + 4);
    if (state != 0xa) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        scale = lbl_8047B938;
        one = lbl_8047B934;
        x = index * 0x1a;
        x += *(s32*)(*(s32**)((u8*)ctx + 0x40));
        *(s16*)(r4 + 0x50) = (s16)x;
        new_var = *(f32*)(*(f32**)((u8*)ctx + 0x30));
        new_var = one - (scale * new_var);
        r4[0x67] = new_var;
    }
    return 0;
}
