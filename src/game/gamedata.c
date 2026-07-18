/**
 * @file gamedata.c
 * @brief Decompiled functions.
 *
 * Address range: 0x8013528C - 0x80135A30
 *
 * Split out of the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114); see effect_util_types.h for
 * shared cross-TU declarations.
 */

#include "dolphin/types.h"
/* This TU recovers the return type hidden by the shared legacy declaration. */
#define gamedataGetStatus gamedataGetStatus_legacy_decl
#include "game/effect/effect_util_types.h"
#undef gamedataGetStatus

typedef struct GameData GameData;
typedef struct GameDataAttest GameDataAttest;

static inline u32 gamedataGetStatusInline(GameData* ptr, u32 effect_type)
{
    GameDataAttest* base;

    if ((u16)effect_type == 0 || (u16)effect_type >= 7) {
        return 0;
    }
    if (ptr == NULL) {
        ptr = (GameData*)savedataGetStatus(0, 0);
        if (ptr == NULL) {
            return 0;
        }
        ptr = (GameData*)savedataGetStatus((u32)ptr, 1);
        if (ptr == NULL) {
            return 0;
        }
    }
    base = (GameDataAttest*)gamedataBiosGetGamedataAtttestPtr(ptr);
    if (base == NULL) {
        return 0;
    }
    switch ((u16)effect_type) {
    case 1:
        return (u32)base;
    case 2:
        return gamedataAttestBiosGetVerId(base);
    case 3:
        return gamedataAttestBiosGetGenId(base);
    case 4:
        return gamedataAttestBiosGetAreaId(base);
    case 5:
        return gamedataAttestBiosGetLangareaId(base);
    default:
        return 0;
    }
}

/* 0x8013528C | 0xAC */
#if 0
asm void fn_8013528C(void) {
#include "src/game/effect/effect_util_fn_8013528C.inc"
}
#else
#pragma optimization_level 4
void gamedataCreate(void* ptr, u8 r4, u8 r5, u8 r6, u8 r7) {
    void* base;

    if (ptr == 0) {
        return;
    }
    gamedataInit(ptr);

    base = ptr;
    if (ptr == 0) {
        base = (void*)savedataGetStatus(0, 0);
        if (base == 0) {
            base = 0;
            goto done;
        } else {
            base = (void*)savedataGetStatus((u32)base, 1);
            if (base == 0) {
                base = 0;
                goto done;
            }
        }
    }

    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) {
        base = 0;
    }

done:
    if (base == 0) {
        return;
    }
    gamedataAttestCreate(base, r4, r5, r6, r7);
}
#endif

/* 0x80135338 | 0x88 */
#if 0
asm void gamedataInit(void) {
#include "src/game/effect/effect_util_gamedataInit.inc"
}
#else
#pragma optimization_level 4
void gamedataInit(void* ptr) {
    void* base;
    if (ptr == 0) {
        base = (void*)savedataGetStatus(0, 0);
        if (base == 0) return;
        base = (void*)savedataGetStatus((u32)base, 1);
        if (base == 0) return;
    } else {
        base = ptr;
    }
    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) return;
    gamedataAttestInit(base);
    gamedatasaveInit(ptr);
}
#endif


/* 0x801353C0 | 0x170 */
#if 0
asm void fn_801353C0(void) {
#include "src/game/effect/effect_util_fn_801353C0.inc"
}
#else
#pragma optimization_level 4
void fn_801353C0(void* ptr, u8 r4, u8 r5, u8 r6, u8 r7) {
    void* base;
    if (ptr == 0) return;
    if ((r4 & 0xFF) == 0) return;
    if ((r5 & 0xFF) == 0) return;
    if ((r6 & 0xFF) == 0) return;
    if ((r7 & 0xFF) == 0) return;
    gamedataAttestInit(ptr);
    /* A60 */
    if (ptr == 0) {
        base = (void*)savedataGetStatus(0, 0);
        if (base == 0) { base = 0; } else { base = (void*)savedataGetStatus((u32)base, 1); }
        base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
        if (base != 0) { gamedataAttestBiosSetVerId(base, (u8)(r4 & 0xFF)); }
    } else {
        base = (void*)gamedataBiosGetGamedataAtttestPtr(ptr);
        if (base != 0) { gamedataAttestBiosSetVerId(base, (u8)(r4 & 0xFF)); }
    }
    /* A50 */
    if (ptr == 0) {
        base = (void*)savedataGetStatus(0, 0);
        if (base == 0) { base = 0; } else { base = (void*)savedataGetStatus((u32)base, 1); }
        base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
        if (base != 0) { gamedataAttestBiosSetGenId(base, (u8)(r5 & 0xFF)); }
    } else {
        base = (void*)gamedataBiosGetGamedataAtttestPtr(ptr);
        if (base != 0) { gamedataAttestBiosSetGenId(base, (u8)(r5 & 0xFF)); }
    }
    /* A40 */
    if (ptr == 0) {
        base = (void*)savedataGetStatus(0, 0);
        if (base == 0) { base = 0; } else { base = (void*)savedataGetStatus((u32)base, 1); }
        base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
        if (base != 0) { gamedataAttestBiosSetAreaId(base, (u8)(r6 & 0xFF)); }
    } else {
        base = (void*)gamedataBiosGetGamedataAtttestPtr(ptr);
        if (base != 0) { gamedataAttestBiosSetAreaId(base, (u8)(r6 & 0xFF)); }
    }
    /* A30 */
    if (ptr == 0) {
        base = (void*)savedataGetStatus(0, 0);
        if (base == 0) return;
        base = (void*)savedataGetStatus((u32)base, 1);
        if (base == 0) return;
    } else {
        base = ptr;
    }
    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) return;
    gamedataAttestBiosSetLangareaId(base, (u8)(r7 & 0xFF));
}
#endif


/* 0x80135530 | 0x1D8 */
u32 gamedataAttestCheckValid(GameData* ptr) {
    if (ptr == NULL) {
        return 0;
    }
    if ((s32)gamedataGetStatusInline(ptr, 2) == 0) {
        return 0;
    }
    if ((s32)gamedataGetStatusInline(ptr, 3) == 0) {
        return 0;
    }
    if ((s32)gamedataGetStatusInline(ptr, 4) == 0) {
        return 0;
    }
    return gamedataGetStatusInline(ptr, 5) != 0;
}


/* 0x80135708 | 0x134 */
#if 0
asm void fn_80135708(void) {
#include "src/game/effect/effect_util_fn_80135708.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
void gamedataAttestInit(void* ptr) {
    void* r31 = ptr;
    void* base;
    if (r31 == 0) goto _end;
    /* A60 */
    base = r31;
    if (r31 != 0) goto _a60_handler;
    base = (void*)savedataGetStatus(0, 0);
    if (base == 0) goto _a50;
    base = (void*)savedataGetStatus((u32)base, 1);
    if (base == 0) goto _a50;
_a60_handler:
    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) goto _a50;
    gamedataAttestBiosSetVerId(base, 0);
_a50:
    /* A50 */
    base = r31;
    if (r31 != 0) goto _a50_handler;
    base = (void*)savedataGetStatus(0, 0);
    if (base == 0) goto _a40;
    base = (void*)savedataGetStatus((u32)base, 1);
    if (base == 0) goto _a40;
_a50_handler:
    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) goto _a40;
    gamedataAttestBiosSetGenId(base, 0);
_a40:
    /* A40 */
    base = r31;
    if (r31 != 0) goto _a40_handler;
    base = (void*)savedataGetStatus(0, 0);
    if (base == 0) goto _a30;
    base = (void*)savedataGetStatus((u32)base, 1);
    if (base == 0) goto _a30;
_a40_handler:
    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) goto _a30;
    gamedataAttestBiosSetAreaId(base, 0);
_a30:
    /* A30 */
    base = r31;
    if (r31 != 0) goto _a30_handler;
    base = (void*)savedataGetStatus(0, 0);
    if (base == 0) goto _end;
    base = (void*)savedataGetStatus((u32)base, 1);
    if (base == 0) goto _end;
_a30_handler:
    base = (void*)gamedataBiosGetGamedataAtttestPtr(base);
    if (base == 0) goto _end;
    gamedataAttestBiosSetLangareaId(base, 0);
_end:;
}
#pragma scheduling off
#endif


/* 0x8013583C | 0xFC */
#if 0
asm void fn_8013583C(void) {
#include "src/game/effect/effect_util_fn_8013583C.inc"
}
#else
#pragma push
#pragma scheduling on
void gamedataSetStatus(ptr, effect_type, value)
GameData* ptr;
u32 effect_type;
u32 value;
{
    GameDataAttest* base;

    if ((u16)effect_type == 0 || (u16)effect_type >= 7) {
        return;
    }
    if (ptr == NULL) {
        ptr = (GameData*)savedataGetStatus(0, 0);
        if (ptr == NULL) {
            return;
        }
        ptr = (GameData*)savedataGetStatus((u32)ptr, 1);
        if (ptr == NULL) {
            return;
        }
    }
    base = (GameDataAttest*)gamedataBiosGetGamedataAtttestPtr(ptr);
    if (base == NULL) {
        return;
    }
    switch ((u16)effect_type) {
    case 1:
        gamedataBiosSetGamedataAtttestPtr((u32*)ptr, (u32*)value);
        break;
    case 2:
        gamedataAttestBiosSetVerId(base, (u8)value);
        break;
    case 3:
        gamedataAttestBiosSetGenId(base, (u8)value);
        break;
    case 4:
        gamedataAttestBiosSetAreaId(base, (u8)value);
        break;
    case 5:
        gamedataAttestBiosSetLangareaId(base, (u8)value);
        break;
    default:
        break;
    }
}
#pragma pop
#endif


/* 0x80135938 | 0xF8 */
#pragma push
#pragma scheduling on
u32 gamedataGetStatus(ptr, effect_type)
GameData* ptr;
u32 effect_type;
{
    GameDataAttest* base;

    if ((u16)effect_type == 0 || (u16)effect_type >= 7) {
        return 0;
    }
    if (ptr == NULL) {
        ptr = (GameData*)savedataGetStatus(0, 0);
        if (ptr == NULL) {
            return 0;
        }
        ptr = (GameData*)savedataGetStatus((u32)ptr, 1);
        if (ptr == NULL) {
            return 0;
        }
    }
    base = (GameDataAttest*)gamedataBiosGetGamedataAtttestPtr(ptr);
    if (base == NULL) {
        return 0;
    }
    switch ((u16)effect_type) {
    case 1:
        return (u32)base;
    case 2:
        return gamedataAttestBiosGetVerId(base);
    case 3:
        return gamedataAttestBiosGetGenId(base);
    case 4:
        return gamedataAttestBiosGetAreaId(base);
    case 5:
        return gamedataAttestBiosGetLangareaId(base);
    default:
        return 0;
    }
}
#pragma pop
