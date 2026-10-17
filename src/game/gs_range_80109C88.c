/**
 * @file gs_range_80109C88.c
 * @brief gs-engine code, 0x80109C88 - 0x8010C364 (19 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

extern u8 pokemonCheckValid(void* pokemon);
extern u8 pokemonBiosGetTamagoFlag(void* pokemon);
extern u16 pokemonGetStatus(void* pokemon, u32 index, u32 field, u32 rare);
extern u32 pokemonBiosGetRnd(void* pokemon);
extern u8 pokemonGetAnnonKatati(u32 personality);
extern u8 pokemonCheckRare(void* pokemon);
extern u16 lbl_8035B478[][2];
extern void* memset(void* dst, int val, u32 size);

/* Face model key: either a plain 32-bit face id (type 0) or a description
 * built from a Pokemon (type 1). */
typedef struct FaceKey {
    u8 type;
    u8 pad1[3];
    union {
        u32 id;
        struct {
            u16 hi;
            u16 lo;
        } s;
    } u;
    u32 c;
    u16 species;
    u8 d;
} FaceKey;

typedef struct FaceModel {
    u8 state;
    u8 useCur;
    u8 pad2[2];
    FaceKey cur;
    FaceKey next;
    u32 handle24;
    u32 thread;
} FaceModel;

extern u32 floorGetCurrentContext(void);
extern u32 GSthreadCreate(s32 priority, void* stack, u32 stackSize,
                          s32 unk1, s32 unk2, void* entry);
extern void GSthreadSetArgs(u32 task, u32 count, ...);
extern u8 GSthreadIsRunning(u32 task);
extern void GSthreadClose(u32 task);
extern s32 _menuPokemonModelLoadThread(FaceModel* m);
extern u8 pokemonIsJoutaiDataId(void* pokemon, u32 slot);

static inline u8 faceKeyEqual(FaceKey* a, FaceKey* b)
{
    if (a->type == b->type) {
        if (a->type != 0) {
            if (a->u.s.hi == b->u.s.hi && a->u.s.lo == b->u.s.lo &&
                a->c == b->c && a->d == b->d) {
                return 1;
            }
        } else if (a->u.id == b->u.id) {
            return 1;
        }
    }
    return 0;
}

static inline u8 faceModelIsKey(FaceModel* m, u32 id)
{
    FaceKey k;

    if (m == NULL) {
        return 0;
    }
    k.type = 0;
    k.u.id = id;
    k.c = 0;
    if (m->useCur) {
        return faceKeyEqual(&m->cur, &k);
    }
    return faceKeyEqual(&m->next, &k);
}

static inline void faceKeyFromPokemon(FaceKey* k, void* pokemon)
{
    extern s32 pokemonGetStatus(void* pokemon, u32 index, u32 field, u32 rare);

    u16 species;

    k->type = 1;
    species = pokemonGetStatus(pokemon, 0, 0x6E, 0);
    k->species = species;
    k->u.s.hi = pokemonGetStatus(NULL, species, 0x66, 0);
    if (pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
        if (pokemonIsJoutaiDataId(pokemon, 0x3E) == 1) {
            k->u.s.lo = 0x87;
        } else {
            k->u.s.lo = 0x25;
        }
    } else {
        k->u.s.lo = 0;
    }
    k->c = pokemonGetStatus(pokemon, 0, 0x6F, 0);
    k->d = pokemonGetStatus(pokemon, 0, 0xC1, 0);
}

static inline u8 faceModelIsPokemon(FaceModel* m, void* pokemon)
{
    FaceKey k;

    if (m == NULL) {
        return 0;
    }
    faceKeyFromPokemon(&k, pokemon);
    if (m->useCur) {
        return faceKeyEqual(&m->cur, &k);
    }
    return faceKeyEqual(&m->next, &k);
}

#if !defined(GS_RANGE_80109C88_ALL) && !defined(GS_FN_8010A88C) && \
    !defined(GS_FN_8010AB00) && !defined(GS_FN_8010AE2C) && \
    !defined(GS_FN_8010B01C) && !defined(GS_FN_8010B16C) && \
    !defined(GS_FN_8010B560) && !defined(GS_FN_8010B5C4) && \
    !defined(GS_FN_8010B718) && !defined(GS_FN_8010B9E8) && \
    !defined(GS_FN_8010BBB8) && !defined(GS_FN_8010BCE4) && \
    !defined(GS_FN_8010BD6C)
/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
s32 menuPokemonModelRequest(FaceModel* m, void* pokemon)
{
    if (m == NULL) {
        return 0;
    }
    if (faceModelIsPokemon(m, pokemon)) {
        return 0;
    }

    switch (m->state) {
    case 0:
        m->state = 1;
        break;
    case 1:
    case 2:
        break;
    }

    faceKeyFromPokemon(&m->cur, pokemon);
    m->useCur = 1;

    if (m->thread != 0) {
        if (GSthreadIsRunning(m->thread)) {
            return 1;
        }
        GSthreadClose(m->thread);
    }
    m->thread = GSthreadCreate(1, (void*)floorGetCurrentContext(), 0x4000, 1, 1,
                               (void*)_menuPokemonModelLoadThread);
    if (m->thread != 0) {
        GSthreadSetArgs(m->thread, 1, m);
    }
    return 1;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
s32 menuPokemonModelRequestById(FaceModel* m, u32 id)
{
    if (m == NULL) {
        return 0;
    }
    if (faceModelIsKey(m, id)) {
        return 0;
    }

    switch (m->state) {
    case 0:
        m->state = 1;
        break;
    case 1:
    case 2:
        break;
    }

    m->cur.type = 0;
    m->cur.u.id = id;
    m->useCur = 1;

    if (m->thread != 0) {
        if (GSthreadIsRunning(m->thread)) {
            return 1;
        }
        GSthreadClose(m->thread);
    }
    m->thread = GSthreadCreate(1, (void*)floorGetCurrentContext(), 0x4000, 1, 1,
                               (void*)_menuPokemonModelLoadThread);
    if (m->thread != 0) {
        GSthreadSetArgs(m->thread, 1, m);
    }
    return 0;
}

/* RULE-EXCEPTION(user-approved): local peephole-control pragma — see docs/RULE_EXCEPTIONS.md */
u8 menuPokemonModelIsLoaded(FaceModel* m, void* pokemon)
{
    FaceKey k;
    u8 result;

    if (m == NULL) {
        return 0;
    }
    faceKeyFromPokemon(&k, pokemon);
    if (m->useCur) {
        result = faceKeyEqual(&m->cur, &k);
    } else {
        result = faceKeyEqual(&m->next, &k);
    }
    return result;
}
#endif

/* The unit built from this file directly is 0x80109C88 - 0x8010A420
 * (menuPokemonModelRequest, menuPokemonModelRequestById, menuPokemonModelIsLoaded). The rest of the range is
 * compiled through gs_range_8010A88C_suffix.c, which defines
 * GS_RANGE_80109C88_ALL. */
#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010A88C) || \
    defined(GS_FN_8010AB00) || defined(GS_FN_8010AE2C) || \
    defined(GS_FN_8010B01C) || defined(GS_FN_8010B16C) || \
    defined(GS_FN_8010B560) || defined(GS_FN_8010B5C4) || \
    defined(GS_FN_8010B718) || defined(GS_FN_8010B9E8) || \
    defined(GS_FN_8010BBB8) || defined(GS_FN_8010BCE4) || \
    defined(GS_FN_8010BD6C)

typedef struct FaceSlot {
    void* data;
    u16 key;
    u8 state;
    s8 slot;
    void* (*callback)(u32);
    u32 arg;
} FaceSlot;

extern s32 lbl_8047AD48;
extern FaceSlot* lbl_8047AD4C;

static inline s32 faceSlotCount(FaceSlot* entry)
{
    s32 i;

    for (i = 0; i < lbl_8047AD48; entry++, i++) {
        if (entry->data == NULL) {
            break;
        }
    }
    return i;
}

static inline s32 faceSlotFind(FaceSlot* entry, void* data)
{
    s32 i;

    for (i = 0; i < lbl_8047AD48; entry++, i++) {
        if (data == entry->data) {
            return i;
        }
    }
    return -1;
}

/* Whether a slot is waiting for its texture (state 1). */
/* RULE-EXCEPTION(user-approved): inline helper used by one function — see docs/RULE_EXCEPTIONS.md */
static inline u8 faceSlotIsLoading(void)
{
    s32 count;
    s32 i;

    count = faceSlotCount(lbl_8047AD4C);
    for (i = 0; i < count; i++) {
        if (lbl_8047AD4C[i].state == 1) {
            return 1;
        }
    }
    return 0;
}

/* Face key of a Pokemon: 0 for none, 0x33D for an invalid entry or an egg,
 * otherwise the species' face (Unown by form), shiny variant if rare. */
static inline u16 pokemonFaceKey(void* pokemon)
{
    extern u32 pokemonGetStatus(void* pokemon, u32 index, u32 field, u32 rare);
    extern u32 pokemonGetAnnonKatati(u32 personality);
    u16 species;
    u8 form;

    if (pokemon == NULL) {
        return 0;
    }
    if (!pokemonCheckValid(pokemon)) {
        return 0x33D;
    }
    if (pokemonBiosGetTamagoFlag(pokemon)) {
        return 0x33D;
    }
    species = pokemonGetStatus(pokemon, 0, 0x6E, 0);
    if (species == 0xC9) {
        form = pokemonGetAnnonKatati(pokemonBiosGetRnd(pokemon));
        if (!pokemonCheckRare(pokemon)) {
            return lbl_8035B478[form][0];
        }
        return lbl_8035B478[form][1];
    }
    if (!pokemonCheckRare(pokemon)) {
        return pokemonGetStatus(NULL, species, 0x5B, 0);
    }
    return pokemonGetStatus(NULL, species, 0x5B, 1);
}

/* -1: no slot for the key's face; 1: its texture is loaded (state 2). */
static inline s8 faceSlotIsReady(u16 key)
{
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    s32 i;

    i = faceSlotFind(lbl_8047AD4C, _menuFaceBiosGetPtr__FUs(key));
    if (i < 0) {
        return -1;
    }
    if (lbl_8047AD4C[i].state == 2) {
        return 1;
    }
    return 0;
}

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

static inline s32 faceDraw(u8* context, void* srcNode, u16 key)
{
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void* GSresFindById(u32 key);
    extern u8 fn_8010C388(u16 idx);
    extern u16 GStextureGetXsize(void* tex);
    extern u16 GStextureGetYsize(void* tex);
    extern void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite);
    extern WinSpriteDrawNode lbl_80404BF0;

    void* texKey;
    void* tex;

    if (faceSlotIsReady(key) != 1) {
        return 0;
    }

    texKey = _menuFaceBiosGetPtr__FUs(key);
    if (texKey == NULL) {
        return 0;
    }

    tex = GSresFindById((u32)texKey);
    if (tex == NULL) {
        return 0;
    }

    lbl_80404BF0 = *(WinSpriteDrawNode*)srcNode;
    lbl_80404BF0.texture_id = (u32)texKey;
    lbl_80404BF0.x = 0;
    lbl_80404BF0.y = 0;
    lbl_80404BF0.crop_x = 0;
    lbl_80404BF0.crop_y = 0;
    lbl_80404BF0.crop_width = 0x2A;
    lbl_80404BF0.crop_height = 0x2A;

    if (fn_8010C388(key)) {
        if (GStextureGetYsize(tex) > 0x2A) {
            lbl_80404BF0.crop_y = 0x2A;
        }
    }

    if (lbl_80404BF0.width < 0) {
        if (GStextureGetXsize(tex) > 0x2A) {
            lbl_80404BF0.crop_x = 0x2A;
            lbl_80404BF0.width = (lbl_80404BF0.width < 0) ? -lbl_80404BF0.width
                                                           : lbl_80404BF0.width;
        }
    }

    winSpriteDrawTexture(context, &lbl_80404BF0);
    return 1;
}

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010BBB8)
u16 pokemonGetFaceSpriteId(void* pokemon)
{
    return pokemonFaceKey(pokemon);
}
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010B560)
u8 fn_8010B560(void) {
    s32 count;
    s32 i;

    count = faceSlotCount(lbl_8047AD4C);
    for (i = 0; i < count; i++) {
        if (lbl_8047AD4C[i].state == 1) {
            return 1;
        }
    }
    return 0;
}
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010BCE4)
/* RULE-EXCEPTION(user-approved): repeats faceSlotIsReady's lookup in place — see docs/RULE_EXCEPTIONS.md */
s8 menuFaceIsReady(u16 key) {
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    s32 i;

    i = faceSlotFind(lbl_8047AD4C, _menuFaceBiosGetPtr__FUs(key));
    if (i < 0) {
        return -1;
    }
    return lbl_8047AD4C[i].state == 2;
}
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010BD6C)
/* Built as its own unit with -opt nopeephole (no local pragma). */
s32 fn_8010BD6C(u16 key, void* (*callback)(u32), u32 arg)
{
    /* FaceSlot as four words: key/state/slot packed into one. */
    typedef struct FaceSlotWords {
        void* data;
        u32 info;
        void* (*callback)(u32);
        u32 arg;
    } FaceSlotWords;
    extern s32 lbl_8047AD48;
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void fn_800F9210(u32 size);
    void* data;
    FaceSlotWords saved;
    FaceSlot cur;
    s32 count;
    s32 index;
    s32 i;
    u64 head;

    data = _menuFaceBiosGetPtr__FUs(key);
    if (data == NULL) {
        return -2;
    }

    count = faceSlotCount((FaceSlot*)lbl_8047AD4C);
    index = faceSlotFind((FaceSlot*)lbl_8047AD4C, _menuFaceBiosGetPtr__FUs(key));
    if (index < 0) {
        if (count == lbl_8047AD48) {
            /* RULE-EXCEPTION(user-approved): codegen-only 64-bit read of the first two words, split into halves, so the evicted slot's data/info words take retail's r4/r5 — see docs/RULE_EXCEPTIONS.md */
            head = *(u64*)lbl_8047AD4C;
            saved.data = (void*)(u32)(head >> 32);
            saved.info = (u32)head;
            saved.callback = lbl_8047AD4C[0].callback;
            saved.arg = lbl_8047AD4C[0].arg;
            if (lbl_8047AD4C[0].state == 2) {
                fn_800F9210(0x5C0);
            }
            for (i = 0; i < count - 1; i++) {
                lbl_8047AD4C[i] = lbl_8047AD4C[i + 1];
            }
            count = lbl_8047AD48 - 1;
            *(FaceSlotWords*)&lbl_8047AD4C[count] = saved;
        }

        lbl_8047AD4C[count].data = data;
        lbl_8047AD4C[count].key = key;
        lbl_8047AD4C[count].state = 0;
        lbl_8047AD4C[count].callback = callback;
        lbl_8047AD4C[count].arg = arg;
        i = count;
    } else {
        cur = lbl_8047AD4C[index];
        for (i = index; i < count - 1; i++) {
            lbl_8047AD4C[i] = lbl_8047AD4C[i + 1];
        }
        lbl_8047AD4C[i] = cur;
    }
    return i;
}
#endif
#ifdef GS_RANGE_80109C88_ALL
void fn_8010C220(void) {
}

void menuFaceInit(s32 count)
{
    typedef struct Entry2 {
        u8 padding[2];
        u16 handle;
        void* data;
    } Entry2;
    extern s32 lbl_8047AD48;
    extern u16 lbl_8047AD50;
    extern Entry2* lbl_8047AD54;
    extern u16 lbl_8047AD58;
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* GSmemLock(u16 handle);
    extern u16 GSmemAlloc(u32 size, u32 alignment);

    s32 i;

    lbl_8047AD48 = count;
    if (lbl_8047AD50 == 0) {
        lbl_8047AD50 = _toolentryAlloc__FUl(count * sizeof(FaceSlot));
        lbl_8047AD4C = GSmemLock(lbl_8047AD50);
    }
    memset(lbl_8047AD4C, 0, count * sizeof(FaceSlot));

    if (lbl_8047AD58 == 0) {
        lbl_8047AD58 = _toolentryAlloc__FUl(count * sizeof(Entry2));
        lbl_8047AD54 = GSmemLock(lbl_8047AD58);
    }
    memset(lbl_8047AD54, 0, count * sizeof(Entry2));

    for (i = 0; i < count; i++) {
        if (lbl_8047AD54[i].handle == 0) {
            lbl_8047AD54[i].handle = GSmemAlloc(0x6ec0, 0x20);
            lbl_8047AD54[i].data =
                GSmemLock(lbl_8047AD54[i].handle);
        }
        memset(lbl_8047AD54[i].data, 0, 0x6ec0);
        lbl_8047AD4C[i].slot = (s8)i;
    }
}
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010B01C)
u16 menuFaceRequestPokemonFace(void* pokemon, void* (*callback)(u32), u32 arg)
{
    extern u16 menuFaceRequestPokemon(u16 key, void* (*callback)(u32), u32 arg);

    return menuFaceRequestPokemon(pokemonFaceKey(pokemon), callback, arg);
}

#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010AE2C)
#if !defined(NO_LOCAL_PRAGMAS_GS_RANGE_EXACT_8010AE2C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_GS_RANGE_EXACT_8010AE2C)
#endif
#if !defined(NO_LOCAL_PRAGMAS_GS_RANGE_EXACT_8010AE2C)
#endif
/* RULE-EXCEPTION(title-path): single-use inline preserves retail register allocation — see docs/RULE_EXCEPTIONS.md */
static inline s32 loadPokemonFace(void* pokemon, void* (*callback)(u32), u32 arg)
{
    extern u32 pokemonGetStatus(void* pokemon, u32 index, u32 field, u32 rare);
    extern u16 menuFaceRequestPokemon(u16 key, void* (*callback)(u32), u32 arg);
    extern void _threadSwitch(void);
    u16 key;

    key = pokemonFaceKey((void*)pokemonGetStatus(pokemon, 0, 0xCC, 0));
    menuFaceRequestPokemon(key, callback, arg);
    for (;;) {
        if (faceSlotIsReady(key) != 0) {
            break;
        }
        _threadSwitch();
    }
    return 1;
}

s32 fightPokemonGetFaceSpriteId(void* pokemon, void* (*callback)(u32), u32 arg)
{
    return loadPokemonFace(pokemon, callback, arg);
}
#if !defined(NO_LOCAL_PRAGMAS_GS_RANGE_EXACT_8010AE2C)
#endif
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010B5C4)
/* RULE-EXCEPTION(title-path): single-use inline preserves retail instruction scheduling — see docs/RULE_EXCEPTIONS.md */
static inline u32 getFaceResource(u16 key)
{
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);

    return (u32)_menuFaceBiosGetPtr__FUs(key);
}

void _menuFaceLoad(void* unused1, u32 unused2, u32 arg)
{
    typedef struct Entry2 {
        u8 padding[2];
        u16 handle;
        void* data;
    } Entry2;
    extern s32 lbl_8047AD48;
    extern Entry2* lbl_8047AD54;
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void fn_800F9210(u32 group, u32 resource);
    extern void* GSresGetResource(u32 group, u32 handle);
    extern void GSresRegisterResource(void* resource, u32 resourceArg,
                                       u32 callbackArg, void* callback);
    extern void* GStextureLoad(void* data);
    extern void DCFlushRange(void* addr, u32 length);
    extern void GXInvalidateTexAll(void);
    extern s32 fn_8010C364(void);
    extern u16 menuFaceRequestPokemon(u16 key, void* (*callback)(u32), u32 arg);

    u32 found1;
    u16 key = arg;
    FaceSlot* entry;
    s32 i;
    u8* res;
    void* tex;

    found1 = getFaceResource(key);
    i = faceSlotFind((FaceSlot*)lbl_8047AD4C, _menuFaceBiosGetPtr__FUs(key));

    if (i < 0) {
        if (found1 != 0) {
            fn_800F9210(0x5c0, found1);
        }
        return;
    }

    entry = &lbl_8047AD4C[i];
    res = GSresGetResource(0x5c0, found1);
    res[7] = 0;
    *(u32*)(res + 0x28) -= (u32)res;

    memcpy(lbl_8047AD54[entry->slot].data, res, 0x6ec0);

    fn_800F9210(0x5c0, found1);

    tex = GStextureLoad(lbl_8047AD54[entry->slot].data);
    GSresRegisterResource(tex, 0x5c0, found1, (void*)fn_8010C364);

    DCFlushRange(tex, 0x6ec0);
    GXInvalidateTexAll();

    entry->state = 2;
    menuFaceRequestPokemon(0, entry->callback, entry->arg);
}
#endif

#ifdef GS_RANGE_80109C88_ALL
/* Peephole off, as for its neighbours in this chunk: retail keeps the null
 * tests separate from the move, the flag test and the reference-count
 * decrement (mr/cmplwi, clrlwi/cmplwi, subi/cmpwi where the peephole pass
 * fuses each into a record form). The lights are cleared through the array,
 * which is what walks the object pointer at +0x3c, and `o` is declared after
 * the counters so it keeps retail's r31. */
s32 menuModelTerm(void* obj)
{
    typedef struct Obj {
        u8 state;
        u8 flag1;
        u8 pad2[0x12];
        u8 flag14;
        u8 pad15[0xF];
        u32 handle24;
        u32 taskHandle;
        u8 pad2C[8];
        void* texture;
        void* camera;
        void* lights[3];
    } Obj;
    extern s32 lbl_8047AD40;
    extern u8 lbl_8047AD44;
    extern char lbl_80271F80[];
    extern char lbl_8035B458[];
    extern void GSlogWrite(const char* fmt, ...);
    extern u8 GSthreadIsRunning(u32 task);
    extern void GSthreadClose(u32 task);
    extern void wazaSequenceOwnerFree(u32 handle);
    extern void GSmodelFree(void* a);
    extern void GSlightFree(void* a);
    extern void GScameraFree(void* camera);
    extern void GStextureFree(void* a);
    extern int wazaSequenceSysRelease();

    s32 i;
    u32 temp;
    Obj* o = (Obj*)obj;

    if (o == NULL) {
        return 0;
    }
    if (lbl_8047AD40 <= 0) {
        GSlogWrite(lbl_80271F80, lbl_8035B458);
        return 0;
    }

    o->flag1 = 0;
    for (;;) {
        if (o->taskHandle == 0) {
            break;
        }
        temp = GSthreadIsRunning(o->taskHandle);
        if (temp == 0) {
            GSthreadClose(o->taskHandle);
            break;
        }
        _threadSwitch();
    }

    if (o != NULL) {
        o->flag1 = 0;
        if (o->flag14) {
            if (o->handle24 != 0) {
                wazaSequenceOwnerFree(o->handle24);
                o->handle24 = 0;
            }
        } else if (o->handle24 != 0) {
            GSmodelFree((void*)o->handle24);
            o->handle24 = 0;
        }
        o->state = 0;
    }

    for (i = 0; i < 3; i++) {
        if (o->lights[i] != NULL) {
            GSlightFree(o->lights[i]);
            o->lights[i] = NULL;
        }
    }

    if (o->camera != NULL) {
        GScameraFree(o->camera);
        o->camera = NULL;
    }

    if (o->texture != NULL) {
        GStextureFree(o->texture);
        o->texture = NULL;
    }
    lbl_8047AD40 = lbl_8047AD40 - 1;
    if (lbl_8047AD40 == 0 && lbl_8047AD44) {
        wazaSequenceSysRelease();
        lbl_8047AD44 = 0;
    }

    return 1;
}
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010B718)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; optimization_level 3; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 3
#pragma peephole off
/* RULE-EXCEPTION(title-path): single-use inline preserves retail register allocation — see docs/RULE_EXCEPTIONS.md */
static inline s32 faceDrawPokemon(u8* context, void* srcNode, void* pokemon)
{
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void* GSresFindById(u32 key);
    extern u8 fn_8010C388(u16 idx);
    extern u16 GStextureGetXsize(void* tex);
    extern u16 GStextureGetYsize(void* tex);
    extern void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite);
    extern WinSpriteDrawNode lbl_80404BF0;

    u16 key;
    void* texKey;
    void* tex;

    key = pokemonFaceKey(pokemon);
    if (faceSlotIsReady(key) != 1) {
        return 0;
    }

    texKey = _menuFaceBiosGetPtr__FUs(key);
    if (texKey == NULL) {
        return 0;
    }

    tex = GSresFindById((u32)texKey);
    if (tex == NULL) {
        return 0;
    }

    lbl_80404BF0 = *(WinSpriteDrawNode*)srcNode;
    lbl_80404BF0.texture_id = (u32)texKey;
    lbl_80404BF0.x = 0;
    lbl_80404BF0.y = 0;
    lbl_80404BF0.crop_x = 0;
    lbl_80404BF0.crop_y = 0;
    lbl_80404BF0.crop_width = 0x2A;
    lbl_80404BF0.crop_height = 0x2A;

    if (fn_8010C388(key)) {
        if (GStextureGetYsize(tex) > 0x2A) {
            lbl_80404BF0.crop_y = 0x2A;
        }
    }

    if (lbl_80404BF0.width < 0) {
        if (GStextureGetXsize(tex) > 0x2A) {
            lbl_80404BF0.crop_x = 0x2A;
            lbl_80404BF0.width = (lbl_80404BF0.width < 0) ? -lbl_80404BF0.width
                                                           : lbl_80404BF0.width;
        }
    }

    winSpriteDrawTexture(context, &lbl_80404BF0);
    return 1;
}

s32 menuFaceDrawPokemon(u8* context, void* srcNode, void* pokemon)
{
    return faceDrawPokemon(context, srcNode, pokemon);
}
#pragma pop
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010B9E8)
#if !defined(GS_FN_8010B9E8)
#endif
s32 fn_8010B9E8(u8* context, void* srcNode, u16 key)
{
    return faceDraw(context, srcNode, key);
}

#if !defined(GS_FN_8010B9E8)
#endif
#endif

#ifdef GS_RANGE_80109C88_ALL
typedef struct MenuModel {
    u8 pad[0x2C];
    s32 w;
    s32 h;
    void* texture;
    void* camera;
    void* lights[3];
} MenuModel;

/* Exact (2026-09-30). The object is the parameter itself, not a local copy:
 * a local takes a vreg above the parameters and colours into r30, where
 * retail has the object in r28 with w and h in r29/r30. The table pointer is
 * a local too, so its address stays in r31 for the five vector copies and the
 * three log strings instead of being rebuilt at each call. GSlogWrite takes
 * the unit's name string as its second argument, as menuModelTerm's call does.
 * `i` is declared before `light`, which is what gives them retail's r24/r25. */
s32 menuModelInit(MenuModel* obj, s32 w, s32 h)
{
    typedef struct Vec3 {
        f32 x, y, z;
    } Vec3;
    extern s32 lbl_8047AD40;
    extern u8 lbl_8047AD44;
    extern const u8 lbl_80271F38[];
    extern char lbl_8035B448[];
    extern f32 lbl_8047CE74;
    extern void GSlogWrite(const char* fmt, ...);
    extern void* GStextureCreate(u16 w, u16 h, u32 format, u32 a, u32 b);
    extern void GStextureSetFilter(void* tex, u32 min, u32 mag, u32 unk);
    extern void* GScameraCreate(void);
    extern void GScameraSetPosition(void* camera, Vec3* pos);
    extern void GScameraSetRotation(void* camera, Vec3* rot);
    extern void* GSlightCreate(void);
    extern void GSlightSetType(void* light, u32 type);
    extern void GSlightSetColor(void* light, Vec3* color);
    extern void GSlightSetPosition(void* light, Vec3* pos);
    extern void GSlightSetTarget(void* light, Vec3* target);
    extern void GSlightSetActive(void* light, u32 active);
    extern void set__5GSvecFfff(Vec3* out, f32 x, f32 y, f32 z);

    const u8* tbl = lbl_80271F38;
    Vec3 pos;
    Vec3 rot;
    Vec3 target;
    Vec3 color1;
    Vec3 color0;
    s32 i;
    void* light;

    pos = *(Vec3*)(tbl + 0xc);
    rot = *(Vec3*)(tbl + 0x18);
    target = *(Vec3*)(tbl + 0x24);
    color1 = *(Vec3*)(tbl + 0x30);
    color0 = *(Vec3*)(tbl + 0x3c);

    if (obj == NULL) {
        return 0;
    }

    memset(obj, 0, 0x48);

    if (lbl_8047AD40 >= 4) {
        GSlogWrite((const char*)(tbl + 0x70), lbl_8035B448);
        return 0;
    }

    obj->w = w;
    obj->h = h;
    if (w < 0x100) {
        w = 0x100;
    }
    if (w > 0x280) {
        w = 0x280;
    }
    if (h < 0x100) {
        h = 0x100;
    }
    if (h > 0x1e0) {
        h = 0x1e0;
    }

    obj->texture = GStextureCreate((u16)w, (u16)h, 0x45, 0, 0);
    if (obj->texture == NULL) {
        GSlogWrite((const char*)(tbl + 0x8c), lbl_8035B448);
        return 0;
    }

    GStextureSetFilter(obj->texture, 2, 2, 0);

    obj->camera = GScameraCreate();
    if (obj->camera == NULL) {
        GSlogWrite((const char*)(tbl + 0xac), lbl_8035B448);
        return 0;
    }

    GScameraSetPosition(obj->camera, &pos);
    GScameraSetRotation(obj->camera, &rot);

    for (i = 0; i < 3; i++) {
        light = GSlightCreate();
        if (light != NULL) {
            switch (i) {
            case 0:
                GSlightSetType(light, 0);
                GSlightSetColor(light, &color0);
                break;
            case 1:
                GSlightSetType(light, 2);
                GSlightSetColor(light, &color1);
                GSlightSetPosition(light, &pos);
                GSlightSetTarget(light, &target);
                break;
            case 2:
                set__5GSvecFfff(&pos, lbl_8047CE74, lbl_8047CE74, lbl_8047CE74);
                GSlightSetType(light, 2);
                GSlightSetColor(light, &color1);
                GSlightSetPosition(light, &pos);
                GSlightSetTarget(light, &target);
                break;
            }
            GSlightSetActive(light, 0);
            obj->lights[i] = light;
        }
    }

    if (lbl_8047AD40 == 0) {
        lbl_8047AD44 = 0;
    }
    lbl_8047AD40++;
    return 1;
}
#endif
#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010A88C)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; optimization_level 3; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 3
#pragma peephole off
s32 _menuPokemonModelLoadThread(FaceModel* m)
{
    extern s32 lbl_8047AD40;
    extern u8 lbl_8047AD44;
    extern u8 fn_8010B560(void);
    extern void _threadSwitch(void);
    extern void fn_801DADC0(u32 arg);
    extern void wazaSequenceOwnerFree(u32 handle);
    extern void GSmodelFree(u32 model);
    extern u32 fn_801DE190(u16 hi, u32 key, u8 rare);
    extern u32 wazaSequenceOwnerGetModel(u32 object);
    extern void GSmodelSetVisibility(u32 model, u32 visible);
    extern u8 fn_801DDD28(u32 model, u16 a, u32 b, u32 c);
    extern void wazaSequenceClearFlag16(u32 model, u16 a, u32 b);
    extern void wazaSequenceSysStartWaza(u32 model, u16 a, u32 b);
    extern u32 floorOpenObject(u32 key);
    extern u8 GSmodelCanAnimate(u32 model);
    extern void GSmodelSetAnimIndex(u32 model, u32 index);
    extern void GSmodelSetAnimRate(u32 model, f32 rate);
    extern void GSmodelStartAnimation(u32 model);
    extern void GSmodelSetAnimType(u32 model, u32 type);
    extern void set__5GSvecFfff(void* out, f32 x, f32 y, f32 z);
    extern void GSmodelSetRotation(u32 model, void* rot);
    extern void _menuPokemonModelSetupView(FaceModel* m);
    extern f32 lbl_8047CE70;
    extern f32 lbl_8047CE78;
    extern f32 lbl_8047CE7C;
    extern f32 lbl_8047CE80;
    typedef struct Vec3 {
        f32 x, y, z;
    } Vec3;

    u32 model;
    Vec3 rotVec;

    if (m == NULL) {
        return 0;
    }
    if (m->useCur == 0) {
        return 0;
    }

    while (fn_8010B560()) {
        _threadSwitch();
    }

    if (!lbl_8047AD44) {
        lbl_8047AD44 = 1;
        fn_801DADC0(4);
    }

    do {
        if (m != NULL) {
            m->useCur = 0;
            if (m->next.type != 0) {
                if (m->handle24 != 0) {
                    wazaSequenceOwnerFree(m->handle24);
                    m->handle24 = 0;
                }
            } else if (m->handle24 != 0) {
                GSmodelFree(m->handle24);
                m->handle24 = 0;
            }
            m->state = 0;
        }

        m->state = 1;
        m->useCur = 0;
        m->next = m->cur;

        if (m->next.type != 0) {
            m->handle24 = fn_801DE190(m->next.u.s.hi, m->next.c, m->next.d);
            if (m->handle24 == 0) {
                m->state = 0;
                return 0;
            }
            model = wazaSequenceOwnerGetModel(m->handle24);
            if (model == 0) {
                m->state = 0;
                return 0;
            }
            GSmodelSetVisibility(model, 0);
            if (m->next.u.s.lo != 0) {
                if (fn_801DDD28(m->handle24, m->next.u.s.lo, 4, 0)) {
                    wazaSequenceClearFlag16(m->handle24, m->next.u.s.lo, 4);
                    wazaSequenceSysStartWaza(m->handle24, m->next.u.s.lo, 4);
                }
            }
        } else {
            m->handle24 = floorOpenObject(m->next.u.id);
            if (m->handle24 == 0) {
                return 0;
            }
            if (GSmodelCanAnimate(m->handle24)) {
                GSmodelSetAnimIndex(m->handle24, m->next.c);
                GSmodelSetAnimRate(m->handle24, lbl_8047CE70);
                GSmodelStartAnimation(m->handle24);
            }
            GSmodelSetVisibility(m->handle24, 0);
            model = m->handle24;
        }

        GSmodelSetAnimType(model, 1);
        set__5GSvecFfff(&rotVec, lbl_8047CE78, lbl_8047CE7C, lbl_8047CE80);
        GSmodelSetRotation(model, &rotVec);
        _menuPokemonModelSetupView(m);

        m->state = 2;
    } while (m->useCur == 1);

    m->thread = 0;
    return 1;
}
#pragma pop
#endif
#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010AB00)
/* RULE-EXCEPTION(user-approved): local compiler-control pragma (push; optimization_level 3; peephole off) needed for the byte match of the code it covers - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma optimization_level 3
#pragma peephole off
s32 _menuPokemonModelSetupView(void* objPtr)
{
    typedef struct Vec3 {
        f32 x, y, z;
    } Vec3;
    extern u32 wazaSequenceOwnerGetModel(u32 object);
    extern s32 wazaSequenceOwnerGetSizeClass(u32 object);
    extern void memoGetScaleAngle(u16 key, f32* scale, f32* angle);
    extern void GSmodelCenterNull(u32 model);
    extern s32 GSmodelWalkJObjs(u32 model);
    extern void* GSmodelGetPart(u32 model, s32 index);
    extern void GSpartGetTransform(void* part, Vec3* out, u32 a, u32 b);
    extern void GSpartFree(void* part);
    extern void modelRemoveCenterNull(u32 model);
    extern void* GSmodelGetBound(u32 model);
    extern void ObjInfoInit(void* bound, void* out);
    extern void GScameraGetPerspective(void* camera, f32* fovY, f32* aspect,
                                        f32* near, f32* far);
    extern double tan(double x);
    extern void set__5GSvecFfff(Vec3* out, f32 x, f32 y, f32 z);
    extern void GScameraSetPosition(void* camera, Vec3* pos);
    extern void GScameraSetPerspective(void* camera, f32 fovY, f32 aspect,
                                        f32 near, f32 far);
    extern void GScameraLookAt(void* camera, Vec3* eye, Vec3* target);
    extern void GSlightSetType(void* light, u32 type);
    extern void GSlightSetColor(void* light, Vec3* color);
    extern void GSlightSetPosition(void* light, Vec3* pos);
    extern void GSlightSetTarget(void* light, Vec3* target);
    extern void GSlightSetActive(void* light, u32 active);
    extern void* memcpy(void* dst, const void* src, u32 size);
    extern const Vec3 lbl_80271F38;
    extern Vec3 lbl_8035B430;
    extern Vec3 lbl_8035B43C;
    extern f32 lbl_8047CE80;
    extern f32 lbl_8047CE84;
    extern f32 lbl_8047CE88;
    extern f32 lbl_8047CE8C;
    extern f32 lbl_8047CE90;
    extern f32 lbl_8047CE94;
    extern f32 lbl_8047CE98;
    extern f32 lbl_8047CE9C;
    extern f32 lbl_8047CEA0;
    extern f32 lbl_8047CEA4;
    extern f32 lbl_8047CEA8;

    u8* obj = (u8*)objPtr;
    u32 model;
    f32 fovScale;
    f32 maxBound;
    f32 distance;
    void* part;
    f32 fovY, aspect, near, far;
    f32 scaleParam;
    f32 angleParam;
    u8 bound[0x10];
    Vec3 eyePos;
    Vec3 transform;
    Vec3 color;
    Vec3 lightPos;

    if (obj == NULL) {
        return 0;
    }

    if (obj[0x14] != 0) {
        model = wazaSequenceOwnerGetModel(*(u32*)(obj + 0x24));
        switch (wazaSequenceOwnerGetSizeClass(*(u32*)(obj + 0x24))) {
        case -2:
            fovScale = lbl_8047CE84;
            break;
        case -1:
            fovScale = lbl_8047CE88;
            break;
        case 0:
            fovScale = lbl_8047CE8C;
            break;
        case 1:
            fovScale = lbl_8047CE90;
            break;
        case 2:
            fovScale = lbl_8047CE94;
            break;
        case 3:
            fovScale = lbl_8047CE98;
            break;
        default:
            fovScale = lbl_8047CE8C;
            break;
        }
        memoGetScaleAngle(*(u16*)(obj + 0x20), &scaleParam, &angleParam);
    } else {
        model = *(u32*)(obj + 0x24);
        scaleParam = fovScale = lbl_8047CE8C;
    }

    if (model == 0) {
        obj[0] = 0;
        return 0;
    }

    GSmodelCenterNull(model);
    part = GSmodelGetPart(model, GSmodelWalkJObjs(model) - 1);
    if (part != NULL) {
        GSpartGetTransform(part, &transform, 0, 0);
        GSpartFree(part);
    }
    modelRemoveCenterNull(model);
    ObjInfoInit(GSmodelGetBound(model), bound);

    if (*(u32*)(obj + 0x38) == 0) {
        return 0;
    }

    GScameraGetPerspective((void*)*(u32*)(obj + 0x38), &fovY, &aspect, &near, &far);
    aspect = (f32)(*(s32*)(obj + 0x2c)) / (f32)(*(s32*)(obj + 0x30));
    fovY = lbl_8047CE9C;
    if (*(f32*)(bound + 0) > *(f32*)(bound + 4)) {
        maxBound = *(f32*)(bound + 0);
    } else {
        maxBound = *(f32*)(bound + 4);
    }

    distance = lbl_8047CE84 * (maxBound / fovScale) / (f32)tan(lbl_8047CEA0);
    set__5GSvecFfff(&eyePos, lbl_8047CE8C, transform.y, distance * scaleParam);
    GScameraSetPosition((void*)*(u32*)(obj + 0x38), &eyePos);
    GScameraSetPerspective((void*)*(u32*)(obj + 0x38), fovY, aspect, near, far);
    set__5GSvecFfff(&lbl_8035B43C, lbl_8047CE80, transform.y, transform.z);
    GScameraLookAt((void*)*(u32*)(obj + 0x38), &lbl_8035B430, &lbl_8035B43C);

    color = lbl_80271F38;
    memcpy(&lightPos, &eyePos, sizeof(Vec3));
    lightPos.x = lightPos.x - lbl_8047CEA4;
    lightPos.y = lightPos.y + lbl_8047CEA8;

    GSlightSetType((void*)*(u32*)(obj + 0x44), 2);
    GSlightSetColor((void*)*(u32*)(obj + 0x44), &color);
    GSlightSetPosition((void*)*(u32*)(obj + 0x44), &lightPos);
    GSlightSetTarget((void*)*(u32*)(obj + 0x44), &transform);
    GSlightSetActive((void*)*(u32*)(obj + 0x44), 1);

    return 1;
}
#pragma pop
#endif

#if defined(GS_RANGE_80109C88_ALL) || defined(GS_FN_8010B16C)
/* RULE-EXCEPTION(user-approved): identity inline pass-through around the face-pointer lookup, chosen for register allocation (found by the permuter farm) — see docs/RULE_EXCEPTIONS.md */
static inline void* faceBiosPtrPassThrough(void* ptr) {
    return ptr;
}

u16 menuFaceRequestPokemon(u16 key, void* (*callback)(u32), u32 arg)
{
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void fn_8010BD6C(u16 key, void* (*cb)(u32), u32 a);
    extern void GSresReleaseById(u32 group);
    extern s32 fn_8017B000(u32 fileHandle, u32 requestID, u32 callbackA,
                            u32 callbackB, u32 callbackC);
    extern void _menuFaceLoad(void* unused1, u32 unused2, u32 arg);
    void* ptr;
    s32 count;
    s32 i;

    for (;;) {
        if (key == 0) {
            ptr = NULL;
            if (callback != NULL) {
                ptr = callback(arg);
            }
            if (ptr != NULL) {
                key = pokemonFaceKey(ptr);
            }
        }
        if (key == 0) {
            if (faceSlotIsLoading()) {
                return 0;
            }
            count = faceSlotCount(lbl_8047AD4C);
            for (i = 0; i < count; i++) {
                if (lbl_8047AD4C[i].state == 0) {
                    key = lbl_8047AD4C[i].key;
                    break;
                }
            }
            if (i == count) {
                GSresReleaseById(0x5C0);
                return 0;
            }
        } else {
            if (faceSlotFind(lbl_8047AD4C, _menuFaceBiosGetPtr__FUs(key)) >= 0) {
                fn_8010BD6C(key, callback, arg);
                key = 0;
                continue;
            }
            fn_8010BD6C(key, callback, arg);
        }
        ptr = faceBiosPtrPassThrough(_menuFaceBiosGetPtr__FUs(key));
        if (ptr == NULL) {
            key = 0;
            continue;
        }
        break;
    }

    if (faceSlotIsLoading()) {
        return 1;
    }
    lbl_8047AD4C[faceSlotFind(lbl_8047AD4C, _menuFaceBiosGetPtr__FUs(key))].state = 1;
    fn_8017B000(0x5C0, (u32)ptr, (u32)_menuFaceLoad, 0, key);
    return 1;
}
#endif

#endif /* suffix function selectors */
