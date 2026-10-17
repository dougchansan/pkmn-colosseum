/**
 * @file hero.c
 * @brief Saved-data slot creation and initialization, 0x80128E38 - 0x80129280.
 */
#include "dolphin/types.h"

/* A saved-data record (the in-memory image of one memory-card save). */
typedef struct SaveData SaveData;

extern void GScharMakeFromSJIS(u16* output, const u8* sjis);
extern void gamedataCreate(void* data, u8 a, u8 b, u8 c, u8 d);
extern void gamedatasaveSetStatus(void* data, u16 kind, u32 value);
extern void heroCreate(u8* data, const u16* name, u8 sex);
extern void heroPokemonGetBlacky(u8* data, u32 index);
extern void heroPokemonGetEifie(u8* data, u32 index);
extern void memoDataSet(u32 index, void* pokemon);
extern void* heroBiosGetPokemonPtr(u8* data, u32 index);
extern void heroSetStatus(u8* data, u32 kind, u32 value);
extern void heroItemAddItemDataId(u8* data, u32 item, u32 count, s32 slot);
extern s32 GSgetVersionMajor(void);
extern const u8 lbl_8047D028[8];

extern void GSflagClear(u32 flag);
extern void gamedataInit(void* data);
extern void heroInit(void* data);
extern void pcboxInit(void* data);
extern void mailInitMailbox(void* data);
extern void sodateyaInit(void* data);
extern void memoInit(void* data);
extern void exribbonInit(void* data);
extern void savedataBattleInit(void* data);
extern void fn_80083CBC(void* data);
extern void fn_801EF128(void* data);

/* Save-record section accessors (pokemon_range_exact_80128CC0.c) and the
 * current save record. */
extern void* fn_80128CC0(void* save);
extern void* fn_80128CDC(void* save);
extern void* savedataBiosGetE3E8Ptr(void* save);
extern void* fn_80128D14(void* save);
extern void* fn_80128D30(void* save);
extern void* fn_80128D4C(void* save);
extern void* fn_80128D68(void* save);
extern void* fn_80128DD4(void* save);
extern void* fn_80128DEC(void* save);
extern void* fn_80128E04(void* save);
extern void* savedataGetWorkPtr(void);

void savedataInit(SaveData* save);

/* A NULL save record means the current one; NULL if there is none. */
static inline void* savedataResolve(void* save)
{
    if (save == NULL) {
        save = savedataGetWorkPtr();
        if (save == NULL) {
            return NULL;
        }
    }
    return save;
}

/* Apply a section accessor to the resolved save record; NULL if there is
 * none. Expanded at every section lookup in this file. */
static inline void* savedataGetPart(void* save, void* (*get)(void*))
{
    if (save == NULL) {
        save = savedataGetWorkPtr();
        if (save == NULL) {
            return NULL;
        }
    }
    return get(save);
}

/* Create the initial game and hero records for a saved-data slot. */
void savedataCreate(SaveData* save, const u16* name)
{
    void* gameData;
    u8* heroData;
    u16 defaultName[11];
    s32 version;

    savedataInit(save);
    gameData = savedataGetPart(save, fn_80128E04);

    version = GSgetVersionMajor();
    switch (version) {
    case 0:
        gamedataCreate(gameData, 0xB, 3, 1, 1);
        break;
    case 1:
        gamedataCreate(gameData, 0xB, 3, 2, 2);
        break;
    case 2:
        gamedataCreate(gameData, 0xB, 3, 3, 8);
        break;
    }

    heroData = savedataGetPart(save, fn_80128DEC);
    if (name == NULL) {
        GScharMakeFromSJIS(defaultName, lbl_8047D028);
        name = defaultName;
    }
    heroCreate(heroData, name, 0);
    gamedatasaveSetStatus(gameData, 5, 2);
    gamedatasaveSetStatus(gameData, 7, 1);
    gamedatasaveSetStatus(gameData, 8, 1);
    heroPokemonGetBlacky(heroData, 0);
    heroPokemonGetEifie(heroData, 0);
    {
        void* pokemon = heroBiosGetPokemonPtr(heroData, 0);
        memoDataSet(0, pokemon);
        pokemon = heroBiosGetPokemonPtr(heroData, 1);
        memoDataSet(0, pokemon);
    }
    heroSetStatus(heroData, 0xC, 0x2710);
    heroItemAddItemDataId(heroData, 0x16, 2, -1);
    heroItemAddItemDataId(heroData, 0xD, 5, -1);
    heroItemAddItemDataId(heroData, 0xE, 2, -1);
    heroItemAddItemDataId(heroData, 0xF, 2, -1);
    heroItemAddItemDataId(heroData, 0x10, 2, -1);
    heroItemAddItemDataId(heroData, 0x12, 2, -1);
    heroItemAddItemDataId(heroData, 0x11, 2, -1);
    heroItemAddItemDataId(heroData, 0x17, 2, -1);
}

/* Initialize each saved-data subsystem from the selected slot. */
void savedataInit(SaveData* save)
{
    gamedataInit(savedataGetPart(save, fn_80128E04));
    heroInit(savedataGetPart(save, fn_80128DEC));
    pcboxInit(savedataGetPart(save, fn_80128DD4));

    if (save == NULL || save == savedataResolve(NULL)) {
        GSflagClear(1);
        GSflagClear(2);
        GSflagClear(3);
    }

    mailInitMailbox(savedataGetPart(save, fn_80128D68));
    sodateyaInit(savedataGetPart(save, fn_80128D4C));
    savedataBattleInit(savedataGetPart(save, savedataBiosGetE3E8Ptr));
    memoInit(savedataGetPart(save, fn_80128D30));
    fn_80083CBC(savedataGetPart(save, fn_80128D14));
    fn_801EF128(savedataGetPart(save, fn_80128CDC));
    exribbonInit(savedataGetPart(save, fn_80128CC0));
}
