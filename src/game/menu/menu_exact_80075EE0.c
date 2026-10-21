/** Exact e-Reader rule callbacks, 0x80075EE0 - 0x80076054. */
#include "dolphin/types.h"

extern s32 std104_soundSetBackgroundMusic(s32, s32, s32);
extern s32 lbl_8047A618;
extern f32 lbl_8047C0C8;
extern u32 floorGetCurrentContext(void);
extern void* GSthreadCreate(s32, u32, u32, s32, s32, void*);
extern void fadeSet(s32, f32);
extern u32 GSmsgGetGSchar(s32 messageId);
extern void msgctrlSetValue(s32 id, u32 value);
extern u16 pokemonBiosGetPokemonDataId(u8* pokemon);
extern u8 pokemonBiosGetEventGetFlag(u8* pokemon);

s32 fn_80075F4C(void);

void fn_80075EE0(void)
{
    if (lbl_8047A618 == 0) {
        GSthreadCreate(1, floorGetCurrentContext(), 0x4000, 1, 1, fn_80075F4C);
    } else {
        std104_soundSetBackgroundMusic(0x46a, 0, 0x7f);
    }
    fadeSet(2, lbl_8047C0C8);
}

s32 fn_80075F4C(void) { return std104_soundSetBackgroundMusic(0x46a, 0, 0x7f); }

void exDiscStoryMenuDrawText(void* rule)
{
    s32 messageId;

    switch (*(s8*)((u8*)rule + 0x95)) {
    case 0:
        messageId = 0x43bc;
        break;
    case 1:
        messageId = 0x43ba;
        break;
    case 2:
        messageId = 0x43be;
        break;
    default:
        messageId = 1;
        break;
    }
    msgctrlSetValue(0x37, GSmsgGetGSchar(messageId));
}

u8 menuCBRule_CheckPokemonEventFlag(u8* pokemon)
{
    switch (pokemonBiosGetPokemonDataId(pokemon)) {
    case 0x97:
    case 0x19A:
        if (pokemonBiosGetEventGetFlag(pokemon) == 0) {
            return 0;
        }
        break;
    }
    return 1;
}
