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

u16 fn_8010BBB8(void* pokemon)
{
    u16 species;
    u8 form;

    if (pokemon == NULL) {
        return 0;
    }
    if (!pokemonCheckValid(pokemon) || pokemonBiosGetTamagoFlag(pokemon)) {
        return 0x33D;
    }

    species = pokemonGetStatus(pokemon, 0, 0x6E, 0);
    if (species == 0xC9) {
        form = pokemonGetAnnonKatati(pokemonBiosGetRnd(pokemon));
        return lbl_8035B478[form][pokemonCheckRare(pokemon) != 0];
    }
    return pokemonGetStatus(NULL, species, 0x5B,
                            pokemonCheckRare(pokemon) != 0);
}

u32 fn_8010B560(void) {
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        u8 padding2[9];
    } Entry;
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    Entry* entry;
    s32 result;
    s32 i;

    result = 0;
    entry = lbl_8047AD4C;
    for (; result < lbl_8047AD48; entry++, result++) {
        if (entry->data == NULL) {
            break;
        }
    }

    entry = lbl_8047AD4C;
    for (i = 0; i < result; i++, entry++) {
        if (entry->state == 1) {
            result = 1;
            goto done;
        }
    }

    result = 0;
done:
    return result;
}

s8 fn_8010BCE4(void) {
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        u8 padding2[9];
    } Entry;
    extern void* _menuFaceBiosGetPtr__FUs(void);
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    void* data;
    Entry* entry;
    s32 i;

    data = _menuFaceBiosGetPtr__FUs();
    entry = lbl_8047AD4C;
    for (i = 0; i < lbl_8047AD48; entry++, i++) {
        if (data == entry->data) {
            goto found;
        }
    }
    i = -1;
found:
    if (i < 0) {
        return -1;
    }
    return lbl_8047AD4C[i].state == 2;
}

#pragma peephole off
s32 fn_8010BD6C(u16 key, void* (*callback)(u32), u32 arg)
{
    typedef struct Entry {
        void* data;
        u16 key;
        u8 state;
        s8 slot;
        void* (*callback)(u32);
        u32 arg;
    } Entry;
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void fn_800F9210(u32 size);
    void* data;
    Entry saved;
    s32 count;
    s32 index;
    s32 i;

    data = _menuFaceBiosGetPtr__FUs(key);
    if (data == NULL) {
        return -2;
    }

    for (count = 0; count < lbl_8047AD48; count++) {
        if (lbl_8047AD4C[count].data == NULL) {
            break;
        }
    }
    for (index = 0; index < lbl_8047AD48; index++) {
        if (lbl_8047AD4C[index].data == data) {
            break;
        }
    }

    if (index >= lbl_8047AD48) {
        index = count;
        if (count == lbl_8047AD48) {
            saved = lbl_8047AD4C[0];
            if (saved.state == 2) {
                fn_800F9210(0x5C0);
            }
            for (i = 0; i < count - 1; i++) {
                lbl_8047AD4C[i] = lbl_8047AD4C[i + 1];
            }
            index = count - 1;
            lbl_8047AD4C[index] = saved;
        }

        lbl_8047AD4C[index].data = data;
        lbl_8047AD4C[index].key = key;
        lbl_8047AD4C[index].state = 0;
        lbl_8047AD4C[index].callback = callback;
        lbl_8047AD4C[index].arg = arg;
        return index;
    }

    saved = lbl_8047AD4C[index];
    for (i = index; i < count - 1; i++) {
        lbl_8047AD4C[i] = lbl_8047AD4C[i + 1];
    }
    index = count - 1;
    lbl_8047AD4C[index] = saved;
    return index;
}
#pragma peephole reset

void fn_8010C220(void) {
}

void fn_8010C224(s32 count)
{
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        u8 slot;
        u8 padding2[8];
    } Entry;
    typedef struct Entry2 {
        u8 padding[2];
        u16 handle;
        void* data;
    } Entry2;
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    extern u16 lbl_8047AD50;
    extern Entry2* lbl_8047AD54;
    extern u16 lbl_8047AD58;
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* fn_800E27B0(u16 handle);
    extern u16 fn_800E2C04(u32 size, u32 alignment);

    Entry* e1;
    Entry2* e2;
    s32 i;

    lbl_8047AD48 = count;
    if (lbl_8047AD50 == 0) {
        lbl_8047AD50 = _toolentryAlloc__FUl(count * sizeof(Entry));
        lbl_8047AD4C = fn_800E27B0(lbl_8047AD50);
    }
    memset(lbl_8047AD4C, 0, count * sizeof(Entry));

    if (lbl_8047AD58 == 0) {
        lbl_8047AD58 = _toolentryAlloc__FUl(count * sizeof(Entry2));
        lbl_8047AD54 = fn_800E27B0(lbl_8047AD58);
    }
    memset(lbl_8047AD54, 0, count * sizeof(Entry2));

    for (i = 0; i < count; i++) {
        e2 = &lbl_8047AD54[i];
        if (e2->handle == 0) {
            e2->handle = fn_800E2C04(0x6ec0, 0x20);
            e2->data = fn_800E27B0(e2->handle);
        }
        memset(e2->data, 0, 0x6ec0);
        e1 = &lbl_8047AD4C[i];
        e1->slot = (s8)i;
    }
}

u16 fn_8010B01C(void* pokemon, void* (*callback)(u32), u32 arg)
{
    extern u16 fn_8010B16C(u16 key, void* (*callback)(u32), u32 arg);
    u16 key;

    if (pokemon == NULL) {
        key = 0;
    } else if (!pokemonCheckValid(pokemon) || pokemonBiosGetTamagoFlag(pokemon)) {
        key = 0x33D;
    } else {
        u16 species = pokemonGetStatus(pokemon, 0, 0x6E, 0);
        if (species == 0xC9) {
            u8 form = pokemonGetAnnonKatati(pokemonBiosGetRnd(pokemon));
            key = lbl_8035B478[form][pokemonCheckRare(pokemon) != 0];
        } else {
            key = pokemonGetStatus(NULL, species, 0x5B, pokemonCheckRare(pokemon) != 0);
        }
    }
    return fn_8010B16C(key, callback, arg);
}

s32 fn_8010AE2C(void* pokemon, void* (*callback)(u32), u32 arg)
{
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        u8 padding2[9];
    } Entry;
    extern u32 pokemonGetStatus(void* pokemon, u32 index, u32 field, u32 rare);
    extern u16 fn_8010B16C(u16 key, void* (*callback)(u32), u32 arg);
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void _threadSwitch(void);
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;

    void* pkm;
    u16 key;
    void* found;
    Entry* entry;
    s32 i;

    pkm = (void*)pokemonGetStatus(pokemon, 0, 0xCC, 0);
    if (pkm == NULL) {
        key = 0;
    } else if (!pokemonCheckValid(pkm) || pokemonBiosGetTamagoFlag(pkm)) {
        key = 0x33D;
    } else {
        u16 species = pokemonGetStatus(pkm, 0, 0x6E, 0);
        if (species == 0xC9) {
            u8 form = pokemonGetAnnonKatati(pokemonBiosGetRnd(pkm));
            key = lbl_8035B478[form][pokemonCheckRare(pkm) != 0];
        } else {
            key = pokemonGetStatus(NULL, species, 0x5B, pokemonCheckRare(pkm) != 0);
        }
    }

    fn_8010B16C(key, callback, arg);

    for (;;) {
        found = _menuFaceBiosGetPtr__FUs(key);
        entry = lbl_8047AD4C;
        for (i = 0; i < lbl_8047AD48; i++, entry++) {
            if (found == entry->data) {
                break;
            }
        }
        if (i >= lbl_8047AD48 || entry->state == 2) {
            return 1;
        }
        _threadSwitch();
    }
}

void fn_8010B5C4(void* unused1, u32 unused2, u16 key)
{
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        s8 slot;
        void* (*callback)(u32);
        u32 arg;
    } Entry;
    typedef struct Entry2 {
        u8 padding[2];
        u16 handle;
        void* data;
    } Entry2;
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
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
    extern u16 fn_8010B16C(u16 key, void* (*callback)(u32), u32 arg);

    void* found1;
    void* found2;
    Entry* entry;
    s32 i;
    u8* res;
    void* tex;

    found1 = _menuFaceBiosGetPtr__FUs(key);
    found2 = _menuFaceBiosGetPtr__FUs(key);

    entry = lbl_8047AD4C;
    for (i = 0; i < lbl_8047AD48; i++, entry++) {
        if (found2 == entry->data) {
            break;
        }
    }

    if (i >= lbl_8047AD48) {
        if (found1 != NULL) {
            fn_800F9210(0x5c0, (u32)found1);
        }
        return;
    }

    res = GSresGetResource(0x5c0, (u32)found1);
    res[7] = 0;
    *(u32*)(res + 0x28) -= (u32)res;

    memcpy(lbl_8047AD54[entry->slot].data, res, 0x6ec0);

    fn_800F9210(0x5c0, (u32)found1);

    tex = GStextureLoad(lbl_8047AD54[entry->slot].data);
    GSresRegisterResource(tex, 0x5c0, (u32)found1, (void*)fn_8010C364);

    DCFlushRange(tex, 0x6ec0);
    GXInvalidateTexAll();

    entry->state = 2;
    fn_8010B16C(0, entry->callback, entry->arg);
}

#pragma peephole off
s32 fn_8010A420(void* obj)
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
    extern u32 GSthreadIsRunning(u32 task);
    extern void GSthreadClose(u32 task);
    extern void fn_801DB100(u32 handle);
    extern void GSmodelFree(void* a);
    extern void GSlightFree(void* a);
    extern void fn_800D2738(void* camera);
    extern void GStextureFree(void* a);
    extern int wazaSequenceSysRelease();

    Obj* o = (Obj*)obj;
    s32 i;

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
        if (GSthreadIsRunning(o->taskHandle)) {
            _threadSwitch();
            continue;
        }
        GSthreadClose(o->taskHandle);
        break;
    }

    if (o != NULL) {
        o->flag1 = 0;
        if (o->flag14) {
            if (o->handle24 != 0) {
                fn_801DB100(o->handle24);
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
        fn_800D2738(o->camera);
        o->camera = NULL;
    }

    if (o->texture != NULL) {
        GStextureFree(o->texture);
        o->texture = NULL;
    }

    lbl_8047AD40--;
    if (lbl_8047AD40 == 0 && lbl_8047AD44) {
        wazaSequenceSysRelease();
        lbl_8047AD44 = 0;
    }

    return 1;
}
#pragma peephole reset

s32 fn_8010B718(u8* context, void* srcNode, void* pokemon)
{
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        u8 padding2[9];
    } Entry;
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
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void* fn_800F92D4(u32 key);
    extern u32 fn_8010C388(u16 idx);
    extern u16 GStextureGetXsize(void* tex);
    extern u16 GStextureGetYsize(void* tex);
    extern void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite);
    extern WinSpriteDrawNode lbl_80404BF0;
    Entry* entry;
    void* face;
    void* texture;
    u16 species;
    u16 key;
    u8 form;
    s32 i;

    if (pokemon == NULL) {
        key = 0;
    } else if (!pokemonCheckValid(pokemon) ||
               pokemonBiosGetTamagoFlag(pokemon)) {
        key = 0x33D;
    } else {
        species = pokemonGetStatus(pokemon, 0, 0x6E, 0);
        if (species == 0xC9) {
            form = pokemonGetAnnonKatati(pokemonBiosGetRnd(pokemon));
            key = lbl_8035B478[form][pokemonCheckRare(pokemon) != 0];
        } else {
            key = pokemonGetStatus(NULL, species, 0x5B,
                                   pokemonCheckRare(pokemon) != 0);
        }
    }

    face = _menuFaceBiosGetPtr__FUs(key);
    entry = lbl_8047AD4C;
    for (i = 0; i < lbl_8047AD48; i++, entry++) {
        if (face == entry->data) {
            break;
        }
    }
    if (i >= lbl_8047AD48 || entry->state != 2) {
        return 0;
    }

    face = _menuFaceBiosGetPtr__FUs(key);
    if (face == NULL) {
        return 0;
    }
    texture = fn_800F92D4((u32)face);
    if (texture == NULL) {
        return 0;
    }

    lbl_80404BF0 = *(WinSpriteDrawNode*)srcNode;
    lbl_80404BF0.texture_id = (u32)face;
    lbl_80404BF0.x = 0;
    lbl_80404BF0.y = 0;
    lbl_80404BF0.crop_x = 0;
    lbl_80404BF0.crop_y = 0;
    lbl_80404BF0.crop_width = 0x2A;
    lbl_80404BF0.crop_height = 0x2A;

    if (fn_8010C388(key) && GStextureGetYsize(texture) > 0x2A) {
        lbl_80404BF0.crop_y = 0x2A;
    }
    if (lbl_80404BF0.width < 0 &&
        GStextureGetXsize(texture) > 0x2A) {
        lbl_80404BF0.crop_x = 0x2A;
        if (lbl_80404BF0.width < 0) {
            lbl_80404BF0.width = -lbl_80404BF0.width;
        }
    }

    winSpriteDrawTexture(context, &lbl_80404BF0);
    return 1;
}

#pragma peephole off
s32 fn_8010B9E8(u8* context, void* srcNode, u16 key)
{
    typedef struct Entry {
        void* data;
        u8 padding[2];
        u8 state;
        u8 padding2[9];
    } Entry;
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
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void* fn_800F92D4(u32 key);
    extern u32 fn_8010C388(u16 idx);
    extern u16 GStextureGetXsize(void* tex);
    extern u16 GStextureGetYsize(void* tex);
    extern void winSpriteDrawTexture(u8* context, WinSpriteDrawNode* sprite);
    extern WinSpriteDrawNode lbl_80404BF0;

    Entry* entry;
    s32 i;
    void* found;
    void* texKey;
    void* tex;

    found = _menuFaceBiosGetPtr__FUs(key);
    entry = lbl_8047AD4C;
    for (i = 0; i < lbl_8047AD48; i++, entry++) {
        if (found == entry->data) {
            break;
        }
    }

    if (i >= lbl_8047AD48 || entry->state != 2) {
        return 0;
    }

    texKey = _menuFaceBiosGetPtr__FUs(key);
    if (texKey == NULL) {
        return 0;
    }

    tex = fn_800F92D4((u32)texKey);
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
            s16 w = lbl_80404BF0.width;
            lbl_80404BF0.crop_x = 0x2A;
            if (w < 0) {
                w = -w;
            }
            lbl_80404BF0.width = w;
        }
    }

    winSpriteDrawTexture(context, &lbl_80404BF0);
    return 1;
}
#pragma peephole reset

s32 fn_8010A010(void* objPtr, u32 key)
{
    extern u32 fn_800FF560(void);
    extern u32 GSthreadCreate(s32 priority, void* stack, u32 stackSize,
                               s32 unk1, s32 unk2, void* entry);
    extern void GSthreadSetArgs(u32 task, u32 count, ...);
    extern u32 GSthreadIsRunning(u32 task);
    extern void GSthreadClose(u32 task);
    extern void fn_8010A88C(void);

    u8* obj = (u8*)objPtr;
    u8 kind;
    s32 match;
    u32 threadHandle;
    u16 hi;
    u16 lo;

    if (obj == NULL) {
        return 0;
    }

    hi = (u16)(key >> 16);
    lo = (u16)key;

    if (obj[1] != 0) {
        if (obj[4] == 0) {
            match = (*(u16*)(obj + 8) == hi) && (*(u16*)(obj + 0xA) == lo) &&
                    (*(u32*)(obj + 0xC) == 0) && (obj[0x12] == 0);
        } else {
            match = (*(u32*)(obj + 8) == key);
        }
    } else {
        if (obj[0x14] == 0) {
            match = (*(u16*)(obj + 0x18) == hi) && (*(u16*)(obj + 0x1A) == lo) &&
                    (*(u32*)(obj + 0x1C) == 0) && (obj[0x22] == 0);
        } else {
            match = (*(u32*)(obj + 0x18) == key);
        }
    }

    if (!match) {
        return 0;
    }

    kind = obj[0];
    if (kind == 0) {
        obj[0] = 1;
    }

    obj[4] = 0;
    *(u32*)(obj + 8) = key;
    obj[1] = 1;

    threadHandle = *(u32*)(obj + 0x28);
    if (threadHandle != 0) {
        if (GSthreadIsRunning(threadHandle)) {
            return 1;
        }
        GSthreadClose(threadHandle);
    }

    threadHandle = GSthreadCreate(1, (void*)fn_800FF560(), 0x4000, 1, 1,
                                   (void*)fn_8010A88C);
    *(u32*)(obj + 0x28) = threadHandle;
    if (threadHandle != 0) {
        GSthreadSetArgs(threadHandle, 1, obj);
    }
    return 0;
}

s32 fn_8010A210(void* objPtr, void* pokemon)
{
    extern u32 pokemonGetStatus(void* pokemon, u32 index, u32 field, u32 rare);
    extern u8 fn_80121ADC(void* pokemon, u32 slot);
    u8* obj = (u8*)objPtr;
    u8 valid;
    u16 f66;
    u16 f0E;
    u32 f6F;
    u8 f0C1;
    s32 match;

    if (obj == NULL) {
        return 0;
    }

    valid = 1;
    pokemonGetStatus(pokemon, 0, 0x6E, 0);
    f66 = (u16)pokemonGetStatus(pokemon, 0, 0x66, 0);
    if (pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
        f0E = (fn_80121ADC(pokemon, 0x3E) == 1) ? 0x87 : 0x25;
    } else {
        f0E = 0;
    }
    f6F = pokemonGetStatus(pokemon, 0, 0x6F, 0);
    f0C1 = (u8)pokemonGetStatus(pokemon, 0, 0xC1, 0);

    if (obj[1] != 0) {
        if (obj[4] != valid) {
            match = 0;
        } else if (obj[4] == 0) {
            match = (*(u32*)(obj + 8) == f66);
        } else {
            match = (*(u16*)(obj + 8) == f66) && (*(u16*)(obj + 0xA) == f0E) &&
                    (*(u32*)(obj + 0xC) == f6F) && (obj[0x12] == f0C1);
        }
    } else {
        if (obj[0x14] != valid) {
            match = 0;
        } else if (obj[0x14] == 0) {
            match = (*(u32*)(obj + 0x18) == f66);
        } else {
            match = (*(u16*)(obj + 0x18) == f66) && (*(u16*)(obj + 0x1A) == f0E) &&
                    (*(u32*)(obj + 0x1C) == f6F) && (obj[0x22] == f0C1);
        }
    }

    return match;
}

s32 fn_80109C88(void* objPtr, void* pokemon)
{
    extern u32 GSthreadCreate(s32 priority, void* stack, u32 stackSize,
                               s32 unk1, s32 unk2, void* entry);
    extern void GSthreadSetArgs(u32 task, u32 count, ...);
    extern u32 GSthreadIsRunning(u32 task);
    extern void GSthreadClose(u32 task);
    extern u32 fn_800FF560(void);
    extern void fn_8010A88C(void);
    extern u8 fn_80121ADC(void* pokemon, u32 slot);

    u8* obj = (u8*)objPtr;
    u8 valid;
    u16 f66;
    u16 f0E;
    u32 f6F;
    u8 f0C1;
    s32 match;
    u8 kind;
    u32 threadHandle;

    if (obj == NULL) {
        return 0;
    }

    valid = 1;
    pokemonGetStatus(pokemon, 0, 0x6E, 0);
    f66 = (u16)pokemonGetStatus(pokemon, 0, 0x66, 0);
    if (pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
        f0E = (fn_80121ADC(pokemon, 0x3E) == 1) ? 0x87 : 0x25;
    } else {
        f0E = 0;
    }
    f6F = pokemonGetStatus(pokemon, 0, 0x6F, 0);
    f0C1 = (u8)pokemonGetStatus(pokemon, 0, 0xC1, 0);

    if (obj[1] != 0) {
        if (obj[4] != valid) {
            match = 0;
        } else if (obj[4] == 0) {
            match = (*(u32*)(obj + 8) == f66);
        } else {
            match = (*(u16*)(obj + 8) == f66) && (*(u16*)(obj + 0xA) == f0E) &&
                    (*(u32*)(obj + 0xC) == f6F) && (obj[0x12] == f0C1);
        }
    } else {
        if (obj[0x14] != valid) {
            match = 0;
        } else if (obj[0x14] == 0) {
            match = (*(u32*)(obj + 0x18) == f66);
        } else {
            match = (*(u16*)(obj + 0x18) == f66) && (*(u16*)(obj + 0x1A) == f0E) &&
                    (*(u32*)(obj + 0x1C) == f6F) && (obj[0x22] == f0C1);
        }
    }

    if (!match) {
        return 0;
    }

    kind = obj[0];
    if (kind == 0) {
        obj[0] = 1;
    }

    obj[4] = 1;
    *(u16*)(obj + 0x10) = (u16)pokemonGetStatus(pokemon, 0, 0x6E, 0);
    *(u16*)(obj + 8) = (u16)pokemonGetStatus(pokemon, 0, 0x66, 0);
    if (pokemonGetStatus(pokemon, 0, 0xC2, 0) != 0) {
        *(u16*)(obj + 0xA) = (fn_80121ADC(pokemon, 0x3E) == 1) ? 0x87 : 0x25;
    } else {
        *(u16*)(obj + 0xA) = 0;
    }
    *(u32*)(obj + 0xC) = pokemonGetStatus(pokemon, 0, 0x6F, 0);
    obj[0x12] = (u8)pokemonGetStatus(pokemon, 0, 0xC1, 0);
    obj[1] = 1;

    threadHandle = *(u32*)(obj + 0x28);
    if (threadHandle != 0) {
        if (GSthreadIsRunning(threadHandle)) {
            return 1;
        }
        GSthreadClose(threadHandle);
    }

    threadHandle = GSthreadCreate(1, (void*)fn_800FF560(), 0x4000, 1, 1,
                                   (void*)fn_8010A88C);
    *(u32*)(obj + 0x28) = threadHandle;
    if (threadHandle != 0) {
        GSthreadSetArgs(threadHandle, 1, obj);
    }
    return 1;
}

#pragma peephole off
s32 menuModelInit(u8* objPtr, s32 w, s32 h)
{
    typedef struct Vec3 {
        f32 x, y, z;
    } Vec3;
    typedef struct Obj {
        u8 pad[0x2C];
        s32 w;
        s32 h;
        void* texture;
        void* camera;
        void* lights[3];
    } Obj;
    extern s32 lbl_8047AD40;
    extern u8 lbl_8047AD44;
    extern const u8 lbl_80271F38[];
    extern f32 lbl_8047CE74;
    extern void GSlogWrite(const char* fmt, ...);
    extern void* GStextureCreate(u16 w, u16 h, u32 format, u32 a, u32 b);
    extern void GStextureSetFilter(void* tex, u32 min, u32 mag, u32 unk);
    extern void* fn_800D29A0(void);
    extern void GScameraSetPosition(void* camera, Vec3* pos);
    extern void GScameraSetRotation(void* camera, Vec3* rot);
    extern void* GSlightCreate(void);
    extern void GSlightSetType(void* light, u32 type);
    extern void GSlightSetColor(void* light, Vec3* color);
    extern void GSlightSetPosition(void* light, Vec3* pos);
    extern void GSlightSetTarget(void* light, Vec3* target);
    extern void GSlightSetActive(void* light, u32 active);
    extern void set__5GSvecFfff(Vec3* out, f32 x, f32 y, f32 z);

    Obj* obj = (Obj*)objPtr;
    Vec3 pos;
    Vec3 rot;
    Vec3 target;
    Vec3 color1;
    Vec3 color0;
    void* light;
    s32 i;

    pos = *(Vec3*)(lbl_80271F38 + 0xc);
    rot = *(Vec3*)(lbl_80271F38 + 0x18);
    target = *(Vec3*)(lbl_80271F38 + 0x24);
    color1 = *(Vec3*)(lbl_80271F38 + 0x30);
    color0 = *(Vec3*)(lbl_80271F38 + 0x3c);

    if (obj == NULL) {
        return 0;
    }

    memset(obj, 0, 0x48);

    if (lbl_8047AD40 >= 4) {
        GSlogWrite((const char*)(lbl_80271F38 + 0x70));
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
        GSlogWrite((const char*)(lbl_80271F38 + 0x8c));
        return 0;
    }

    GStextureSetFilter(obj->texture, 2, 2, 0);

    obj->camera = fn_800D29A0();
    if (obj->camera == NULL) {
        GSlogWrite((const char*)(lbl_80271F38 + 0xac));
        return 0;
    }

    GScameraSetPosition(obj->camera, &pos);
    GScameraSetRotation(obj->camera, &rot);

    for (i = 0; i < 3; i++) {
        light = GSlightCreate();
        if (light != NULL) {
            if (i == 1) {
                GSlightSetType(light, 2);
                GSlightSetColor(light, &color1);
                GSlightSetPosition(light, &pos);
                GSlightSetTarget(light, &target);
            } else if (i == 0) {
                GSlightSetType(light, 0);
                GSlightSetColor(light, &color0);
            } else {
                set__5GSvecFfff(&pos, lbl_8047CE74, lbl_8047CE74, lbl_8047CE74);
                GSlightSetType(light, 2);
                GSlightSetColor(light, &color1);
                GSlightSetPosition(light, &pos);
                GSlightSetTarget(light, &target);
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
#pragma peephole reset

#pragma peephole off
s32 fn_8010A88C(void* objPtr)
{
    extern s32 lbl_8047AD40;
    extern u8 lbl_8047AD44;
    extern s32 fn_8010B560(void);
    extern void _threadSwitch(void);
    extern void fn_801DADC0(u32 arg);
    extern void fn_801DB100(u32 handle);
    extern void GSmodelFree(void* model);
    extern u32 fn_801DE190(u16 hi, u32 key, u8 rare);
    extern u32 fn_801DAC3C(void* object);
    extern void GSmodelSetVisibility(u32 model, u32 visible);
    extern s32 fn_801DDD28(u32 model, u32 a, u32 b);
    extern void fn_801DA914(u32 model, u32 a, u16 b);
    extern void fn_801DA9E8(u32 model, u32 a, u16 b);
    extern u32 floorOpenObject(u32 key);
    extern u32 GSmodelCanAnimate(void* model);
    extern void GSmodelSetAnimIndex(u32 model, u32 index);
    extern void GSmodelSetAnimRate(u32 model, f32 rate);
    extern void GSmodelStartAnimation(u32 model);
    extern void GSmodelSetAnimType(u32 model, u32 type);
    extern void set__5GSvecFfff(void* out, f32 x, f32 y, f32 z);
    extern void GSmodelSetRotation(u32 model, void* rot);
    extern void fn_8010AB00(void* obj);
    extern f32 lbl_8047CE70;
    extern f32 lbl_8047CE78;
    extern f32 lbl_8047CE7C;
    extern f32 lbl_8047CE80;
    typedef struct Vec3 {
        f32 x, y, z;
    } Vec3;

    u8* obj = (u8*)objPtr;
    u32 model;
    Vec3 rotVec;

    if (obj == NULL) {
        return 0;
    }
    if (obj[1] == 0) {
        return 0;
    }

    while (!fn_8010B560()) {
        _threadSwitch();
    }

    if (!lbl_8047AD44) {
        lbl_8047AD44 = 1;
        fn_801DADC0(4);
    }

    for (;;) {
        if (obj != NULL) {
            obj[1] = 0;
            if (obj[0x14] != 0) {
                if (*(u32*)(obj + 0x24) != 0) {
                    fn_801DB100(*(u32*)(obj + 0x24));
                    *(u32*)(obj + 0x24) = 0;
                }
            } else if (*(u32*)(obj + 0x24) != 0) {
                GSmodelFree((void*)*(u32*)(obj + 0x24));
                *(u32*)(obj + 0x24) = 0;
            }
            obj[0] = 0;
        }

        obj[0] = 1;
        obj[1] = 0;
        *(u32*)(obj + 0x14) = *(u32*)(obj + 4);
        *(u32*)(obj + 0x18) = *(u32*)(obj + 8);
        *(u32*)(obj + 0x1c) = *(u32*)(obj + 0xc);
        *(u32*)(obj + 0x20) = *(u32*)(obj + 0x10);

        if (obj[0x14] != 0) {
            model = fn_801DE190(*(u16*)(obj + 0x18), *(u32*)(obj + 0x1c), obj[0x22]);
            *(u32*)(obj + 0x24) = model;
            if (model == 0) {
                obj[0] = 0;
                return 0;
            }
            model = fn_801DAC3C((void*)model);
            if (model == 0) {
                obj[0] = 0;
                return 0;
            }
            GSmodelSetVisibility(model, 0);
            if (*(u16*)(obj + 0x1a) != 0) {
                if (fn_801DDD28(model, 4, 0)) {
                    fn_801DA914(model, 4, *(u16*)(obj + 0x1a));
                    fn_801DA9E8(model, 4, *(u16*)(obj + 0x1a));
                }
            }
        } else {
            model = floorOpenObject(*(u32*)(obj + 0x18));
            *(u32*)(obj + 0x24) = model;
            if (model == 0) {
                return 0;
            }
            if (GSmodelCanAnimate((void*)model)) {
                GSmodelSetAnimIndex(model, *(u32*)(obj + 0x1c));
                GSmodelSetAnimRate(model, lbl_8047CE70);
                GSmodelStartAnimation(model);
            }
            GSmodelSetVisibility(model, 0);
            model = *(u32*)(obj + 0x24);
        }

        GSmodelSetAnimType(model, 1);
        set__5GSvecFfff(&rotVec, lbl_8047CE78, lbl_8047CE7C, lbl_8047CE80);
        GSmodelSetRotation(model, &rotVec);
        fn_8010AB00(obj);

        obj[0] = 2;
        if (obj[1] != 1) {
            break;
        }
    }

    *(u32*)(obj + 0x28) = 0;
    return 1;
}
#pragma peephole reset

s32 fn_8010AB00(void* objPtr)
{
    typedef struct Vec3 {
        f32 x, y, z;
    } Vec3;
    extern u32 fn_801DAC3C(void* object);
    extern u32 fn_801DAC24(void* object);
    extern void memoGetScaleAngle(u16 key, f32* scale, f32* angle);
    extern void GSmodelCenterNull(u32 model);
    extern s32 fn_800EE0E8(u32 model);
    extern void* GSmodelGetPart(u32 model, s32 index);
    extern void GSpartGetTransform(void* part, Vec3* out, u32 a, u32 b);
    extern void GSpartFree(void* part);
    extern void modelRemoveCenterNull(u32 model);
    extern void* GSmodelGetBound(u32 model);
    extern void ObjInfoInit(void* bound, void* out);
    extern void GScameraGetPerspective(void* camera, f32* fovY, f32* aspect,
                                        f32* near, f32* far);
    extern f32 tan(f32 x);
    extern void set__5GSvecFfff(Vec3* out, f32 x, f32 y, f32 z);
    extern void GScameraSetPosition(void* camera, Vec3* pos);
    extern void GScameraSetPerspective(void* camera, f32 fovY, f32 aspect,
                                        f32 near, f32 far);
    extern void GScameraLookAt(void* camera, Vec3* eye, Vec3* target);
    extern const u8 lbl_80271F38[];
    extern Vec3 lbl_8035B430;
    extern Vec3 lbl_8035B43C;
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
    f32 scaleParam;
    f32 angleParam;
    void* part;
    s32 partIndex;
    Vec3 transform;
    u8 bound[0x14];
    f32 fovY, aspect, near, far;
    f32 maxBound;
    Vec3 eyePos;
    Vec3 lightPos;
    Vec3 ambientColor;

    if (obj == NULL) {
        return 0;
    }

    if (obj[0x14] != 0) {
        s32 category;
        model = fn_801DAC3C((void*)*(u32*)(obj + 0x24));
        category = fn_801DAC24((void*)*(u32*)(obj + 0x24));

        if (category == 1) {
            fovScale = lbl_8047CE90;
        } else if (category >= 1) {
            if (category == 3) {
                fovScale = lbl_8047CE98;
            } else if (category >= 3) {
                fovScale = lbl_8047CE8C;
            } else {
                fovScale = lbl_8047CE94;
            }
        } else if (category == -1) {
            fovScale = lbl_8047CE88;
        } else if (category >= -1) {
            fovScale = lbl_8047CE8C;
        } else if (category >= -2) {
            fovScale = lbl_8047CE84;
        } else {
            fovScale = lbl_8047CE8C;
        }

        memoGetScaleAngle(*(u16*)(obj + 0x20), &scaleParam, &angleParam);
    } else {
        model = *(u32*)(obj + 0x24);
        fovScale = lbl_8047CE8C;
        scaleParam = lbl_8047CE8C;
    }

    if (model == 0) {
        obj[0] = 0;
        return 0;
    }

    GSmodelCenterNull(model);
    partIndex = fn_800EE0E8(model) - 1;
    transform.x = 0.0f;
    transform.y = 0.0f;
    transform.z = 0.0f;
    part = GSmodelGetPart(model, partIndex);
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

    fovY = lbl_8047CE9C;
    aspect = (f32)(*(s32*)(obj + 0x2c)) / (f32)(*(s32*)(obj + 0x30));
    maxBound = (*(f32*)(bound + 0) > *(f32*)(bound + 4)) ? *(f32*)(bound + 0)
                                                          : *(f32*)(bound + 4);

    set__5GSvecFfff(&eyePos, lbl_8047CE8C, transform.y,
                     (lbl_8047CE84 * (maxBound / fovScale) / (f32)tan(lbl_8047CEA0)) *
                         scaleParam);
    GScameraSetPosition((void*)*(u32*)(obj + 0x38), &eyePos);
    GScameraSetPerspective((void*)*(u32*)(obj + 0x38), fovY, aspect, near, far);
    GScameraLookAt((void*)*(u32*)(obj + 0x38), &lbl_8035B430, &lbl_8035B43C);

    ambientColor = *(Vec3*)lbl_80271F38;
    lightPos = eyePos;
    lightPos.x = eyePos.x - lbl_8047CEA4;
    lightPos.y = eyePos.y + lbl_8047CEA8;

    GSlightSetType((void*)*(u32*)(obj + 0x44), 2);
    GSlightSetColor((void*)*(u32*)(obj + 0x44), &ambientColor);
    GSlightSetPosition((void*)*(u32*)(obj + 0x44), &lightPos);
    GSlightSetTarget((void*)*(u32*)(obj + 0x44), &transform);
    GSlightSetActive((void*)*(u32*)(obj + 0x44), 1);

    return 1;
}

u16 fn_8010B16C(u16 key, void* (*callback)(u32), u32 arg)
{
    typedef struct Entry {
        void* data;
        u16 key;
        u8 state;
        s8 slot;
        void* (*callback)(u32);
        u32 arg;
    } Entry;
    extern s32 lbl_8047AD48;
    extern Entry* lbl_8047AD4C;
    extern void* _menuFaceBiosGetPtr__FUs(u16 key);
    extern void fn_8010BD6C(u16 key, void* (*cb)(u32), u32 a);
    extern void fn_8017B1CC(u32 group);
    extern s32 fn_8017B000(u32 fileHandle, u32 requestID, u32 callbackA,
                            u32 callbackB, u32 callbackC);

    void* pokemon;
    void* found;
    Entry* entry;
    s32 count;
    s32 i;
    s32 idx;

retry:
    if (key == 0) {
        pokemon = NULL;
        if (callback != NULL) {
            pokemon = callback(arg);
        }
        if (pokemon == NULL) {
            key = 0;
        } else if (!pokemonCheckValid(pokemon) || pokemonBiosGetTamagoFlag(pokemon)) {
            key = 0x33D;
        } else {
            u16 species = pokemonGetStatus(pokemon, 0, 0x6E, 0);
            if (species == 0xC9) {
                u8 form = pokemonGetAnnonKatati(pokemonBiosGetRnd(pokemon));
                key = lbl_8035B478[form][pokemonCheckRare(pokemon) != 0];
            } else {
                key = pokemonGetStatus(NULL, species, 0x5B, pokemonCheckRare(pokemon) != 0);
            }
        }
    }

    if (key != 0) {
        goto haveKey;
    }

    count = 0;
    entry = lbl_8047AD4C;
    for (; count < lbl_8047AD48; entry++, count++) {
        if (entry->data == NULL) {
            break;
        }
    }
    entry = lbl_8047AD4C;
    for (i = 0; i < count; i++, entry++) {
        if (entry->state == 1) {
            return 0;
        }
    }

    count = 0;
    entry = lbl_8047AD4C;
    for (; count < lbl_8047AD48; entry++, count++) {
        if (entry->data == NULL) {
            break;
        }
    }
    entry = lbl_8047AD4C;
    idx = 0;
    for (; idx < count; idx++, entry++) {
        if (entry->state == 0) {
            key = entry->key;
            break;
        }
    }
    if (idx == count) {
        fn_8017B1CC(0x5c0);
        return 0;
    }

haveKey:
    found = _menuFaceBiosGetPtr__FUs(key);
    if (found == NULL) {
        key = 0;
        goto retry;
    }

    count = 0;
    entry = lbl_8047AD4C;
    for (; count < lbl_8047AD48; entry++, count++) {
        if (entry->data == NULL) {
            break;
        }
    }
    entry = lbl_8047AD4C;
    for (i = 0; i < count; i++, entry++) {
        if (entry->state == 1) {
            return 1;
        }
    }

    found = _menuFaceBiosGetPtr__FUs(key);
    entry = lbl_8047AD4C;
    idx = -1;
    for (i = 0; i < lbl_8047AD48; i++, entry++) {
        if (found == entry->data) {
            idx = i;
            break;
        }
    }

    lbl_8047AD4C[idx].state = 1;
    fn_8017B000(0x5c0, (u32)found, (u32)fn_8010B5C4, 0, key);
    return 1;
}
