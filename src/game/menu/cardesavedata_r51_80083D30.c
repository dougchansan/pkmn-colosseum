/**
 * @file cardesavedata_r51_80083D30.c
 * @brief fn_80083D30 / fn_80083ECC (0x80083D30 - 0x80084034): encode the
 *        party's move descriptions into Card-e text records.
 *
 * Function-boundary carve of the Card-e save-data TU (see cardesavedata.c),
 * text only, on the TU's unit-wide -opt nopeephole. fn_80083D30 carries
 * fn_80083ECC's body inline. MWCC only auto-inlines a function defined
 * earlier, so the unit is built with -inline auto,deferred, under which it
 * is generated in reverse definition order (like pslist.c). Each line is
 * copied and encoded by an inline that owns its bounce buffer and advances
 * the caller's text and destination pointers; that gives both functions
 * retail's buffer layout and registers.
 */
#include "dolphin/types.h"

extern void* memset(void* dst, int val, u32 size);
extern void* heroBiosGetPokemonPtr(void* hero, u16 index);
extern u16 pokemonBiosGetPokemonWazaDataId(void* pokemon, u16 index);
extern void* wazaDataBiosGetPtr(u16 id);
extern u32 wazaDataBiosGetDoc(void* waza);
extern const u16* GSmsgGetGSchar(u32 msg);
extern u32 gamedataGetStatus(s32, s32);
extern u32 GScharToGBA(u8* destination, const u16* text, u32 font);

/* Copy one 0x0000/0xFFFF-terminated line (at most 0x50 characters) and
 * encode it at *destination, advancing both pointers. */
static inline void CardEPutLine(u8** destination, const u16** text)
{
    u16 buffer[0x52];
    s32 count;

    count = 0;
    while (**text != 0 && **text != 0xFFFF) {
        if (count < 0x50) {
            buffer[count++] = **text;
        }
        (*text)++;
    }
    buffer[count] = 0;
    *destination += GScharToGBA(*destination, buffer, gamedataGetStatus(0, 5));
}

/* Encode one move's description (two lines split at 0xFFFF). */
s32 fn_80083ECC(u8* destination, u16 id)
{
    void* waza;
    const u16* text;

    waza = wazaDataBiosGetPtr(id);
    memset(destination, 0, 0x50);
    if (waza != NULL) {
        text = GSmsgGetGSchar(wazaDataBiosGetDoc(waza));
        CardEPutLine(&destination, &text);
        if (*text == 0xFFFF) {
            *destination++ = 0xFE;
            text = (const u16*)((const u8*)text + 3);
            CardEPutLine(&destination, &text);
        }
        *destination = 0xFF;
    }
    return 0;
}

/* Encode the four move descriptions of each of the six party members. */
s32 fn_80083D30(void* hero, u8* destination)
{
    s32 partyIndex;
    s32 moveIndex;
    void* pokemon;

    for (partyIndex = 0; partyIndex < 6; partyIndex++) {
        pokemon = heroBiosGetPokemonPtr(hero, partyIndex);
        for (moveIndex = 0; moveIndex < 4; moveIndex++) {
            fn_80083ECC(destination,
                        pokemonBiosGetPokemonWazaDataId(pokemon, moveIndex));
            destination += 0x50;
        }
    }
    return 0;
}
