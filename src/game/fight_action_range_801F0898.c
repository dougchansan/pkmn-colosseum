/**
 * @file fight_action_range_801F0898.c
 * @brief Fight-action creation, priority, and FIFO dispatch.
 *
 * Split out of the former game/pokemon.c CodeCandidate bucket
 * (0x801F000C-0x801F7F80), which was mislabeled "pokemon" but is
 * entirely the XD-era fight-engine cluster. Address range covered by
 * this translation unit: 0x801F0898-0x801F108C (5 functions), per
 * config/GC6E01/splits.txt. fightActionFifoInit (0x801F108C) is carved
 * into fight_action_exact_801F108C.c, and
 * fight_action_range_candidate_801F1170.c defines
 * FIGHT_ACTION_RANGE_801F1170_ONLY to compile the 0x801F1170 tail.
 */

#include "game/pokemon_fight_types.h"

#ifndef FIGHT_ACTION_RANGE_801F1170_ONLY

/* 0x801F0898 | size: 0x90 | medium */
u32 fightActionGetKindDataId(u32 param) {
    extern u32 fightActionDataBiosGetKind();
    extern u32 fightActionBiosGetFightActionDataPtr();
    extern u32 fightActionBiosGetKind();
    u32 valid;
    if (param != 0) goto chk1;
    valid = 0;
    goto join;
    chk1:
    if ((u16)fightActionBiosGetKind() != 0) goto chk2;
    valid = 0;
    goto join;
    chk2:
    if (fightActionBiosGetFightActionDataPtr(param) != 0) goto set1;
    valid = 0;
    goto join;
    set1:
    valid = 1;
    join:
    if ((u8)valid == 0)
        return 0;
    if (fightActionBiosGetFightActionDataPtr(param) == 0)
        return 0;
    return fightActionDataBiosGetKind();
}

/* 0x801F0928 | size: 0xA8 | medium */
s32 fightActionGetPri(u32 param) {
    extern u32 fightActionKindDataBiosGetPri();
    extern u32 fightActionKindDataBiosGetPtr();
    extern u32 fightActionDataBiosGetKind();
    extern u32 fightActionBiosGetFightActionDataPtr();
    extern u32 fightActionBiosGetKind();
    u32 valid;
    if (param != 0) goto chk1;
    valid = 0;
    goto join;
    chk1:
    if ((u16)fightActionBiosGetKind() != 0) goto chk2;
    valid = 0;
    goto join;
    chk2:
    if (fightActionBiosGetFightActionDataPtr(param) != 0) goto set1;
    valid = 0;
    goto join;
    set1:
    valid = 1;
    join:
    if ((u8)valid == 0)
        return -0x80;
    if (fightActionBiosGetFightActionDataPtr(param) == 0)
        return -0x80;
    fightActionDataBiosGetKind();
    if (fightActionKindDataBiosGetPtr() == 0)
        return -0x80;
    return fightActionKindDataBiosGetPri();
}

#endif /* FIGHT_ACTION_RANGE_801F1170_ONLY */

extern void fightActionBiosSetFifoBanme(void*, s32);
extern s32 fightActionDataBiosGetBuff(void*);
extern u32 fightActionDataBiosGetKind(void*);
extern void fightActionBiosSetDispBuff(void*, u32, u32);
extern void fightActionBiosSetMotoFightActionDataPtr(void*, void*);
extern void fightActionBiosSetBuffDataId(void*, u32);
extern void fightActionBiosSetBuffDataPtr(void*, u32);
extern void fightActionBiosSetActorFightTargetPtr(void*, void*);
extern void fightActionBiosSetFightActionDataPtr(void*, void*);
extern void fightActionBiosSetBuff(void*, u32);
extern void fightActionBiosSetKind(void*, u32);
extern void* fightActionBiosGetFightActionDataPtr(void*);
extern s32 fightActionBiosGetBuff(void*);
extern u32 fightActionBiosGetKind(void*);

static inline void fightActionClear(void* action) {
    u32 i;

    fightActionBiosSetKind(action, 0);
    fightActionBiosSetBuff(action, 0);
    fightActionBiosSetFightActionDataPtr(action, 0);
    for (i = 0; (u16)i < 4; i++) {
        fightActionBiosSetDispBuff(action, i, 0);
    }
    fightActionBiosSetBuffDataPtr(action, 0);
    fightActionBiosSetBuffDataId(action, 0);
    fightActionBiosSetActorFightTargetPtr(action, 0);
    fightActionBiosSetMotoFightActionDataPtr(action, 0);
    fightActionBiosSetFifoBanme(action, -1);
}

static inline u8 fightActionIsValid(void* action) {
    if (action == 0) {
        return 0;
    }
    if ((u16)fightActionBiosGetKind(action) == 0) {
        return 0;
    }
    if (fightActionBiosGetFightActionDataPtr(action) == 0) {
        return 0;
    }
    return 1;
}

static inline void* fightActionDataSearch(void* data, u32 kind, s32 buff) {
    u16 i;
    void* entry;
    u32 entryKind;
    s32 entryBuff;

    if (data == 0) {
        return 0;
    }
    for (i = 0;; i++) {
        entry = (u8*)data + i * 8;
        entryKind = fightActionDataBiosGetKind(entry);
        if ((u16)entryKind == 0) {
            break;
        }
        entryBuff = fightActionDataBiosGetBuff(entry);
        if ((u16)kind == (u16)entryKind && buff == entryBuff) {
            return entry;
        }
    }
    return 0;
}

static inline void* fightActionSetData(void* action) {
    void* data;
    u32 kind;
    s32 buff;

    if (fightActionIsValid(action) == 0) {
        return 0;
    }
    kind = fightActionBiosGetKind(action);
    buff = fightActionBiosGetBuff(action);
    data = fightActionDataSearch(fightActionBiosGetFightActionDataPtr(action), kind, buff);
    if (data == 0) {
        return 0;
    }
    fightActionBiosSetFightActionDataPtr(action, data);
    return data;
}


typedef struct FightActionFifoEntry {
    u32 word[12];
} FightActionFifoEntry;
typedef u32 (*FightActionFunc)(void*);

extern FightActionFifoEntry lbl_8046D790[];
extern u32 lbl_8047B5E8;
extern u32 lbl_8047B5EC;
extern void* fightActionKindDataBiosGetPtr(u32);
extern FightActionFunc fightActionKindDataBiosGetFlowFuncPtr(void*);
extern FightActionFunc fightActionKindDataBiosGetDispFuncPtr(void*);
extern void fightActionBiosCopy(void*, void*);


static inline u8 fightActionFifoPop(void* action) {
    if (lbl_8047B5E8 != lbl_8047B5EC) {
        *(FightActionFifoEntry*)action = lbl_8046D790[lbl_8047B5EC];
        lbl_8047B5EC++;
        lbl_8047B5EC = lbl_8047B5EC % 32;
    } else {
        return 0;
    }
    return 1;
}

static inline void fightActionFifoDrop(void) {
    if (lbl_8047B5E8 != lbl_8047B5EC) {
        lbl_8047B5EC++;
        lbl_8047B5EC = lbl_8047B5EC % 32;
    }
}

static inline u32 fightActionFlow(void* action) {
    FightActionFunc func;

    if (fightActionIsValid(action) == 0) {
        return 0;
    }
    func = fightActionKindDataBiosGetFlowFuncPtr(fightActionKindDataBiosGetPtr(
        fightActionDataBiosGetKind(fightActionBiosGetFightActionDataPtr(action))));
    if (func != 0) {
        return func(action);
    }
    return 1;
}

static inline u32 fightActionDisp(void* action) {
    FightActionFunc func;

    if (fightActionIsValid(action) == 0) {
        return 0;
    }
    func = fightActionKindDataBiosGetDispFuncPtr(fightActionKindDataBiosGetPtr(
        fightActionDataBiosGetKind(fightActionBiosGetFightActionDataPtr(action))));
    if (func != 0) {
        return func(action);
    }
    return 1;
}

static inline s32 fightActionCreateInline(void* action, void* motoAction, void* actorTarget,
                                          u32 kind, u32 buff, void* data) {
    fightActionClear(action);
    fightActionBiosSetKind(action, kind);
    fightActionBiosSetBuff(action, buff);
    fightActionBiosSetFightActionDataPtr(action, data);
    fightActionBiosSetActorFightTargetPtr(action, actorTarget);
    if (fightActionSetData(action) == 0) {
        fightActionClear(action);
        return 4;
    }
    fightActionBiosSetBuffDataId(action, buff);
    fightActionBiosSetMotoFightActionDataPtr(action, motoAction);
    return 1;
}

#ifndef FIGHT_ACTION_RANGE_801F1170_ONLY

/* 0x801F09D0 | size: 0x130 */
void fightActionDispFifoAll(void)
{
    FightActionFifoEntry action;
    u32 result;

    do {
        if (fightActionFifoPop(&action) == 0) {
            result = 3;
        } else {
            result = fightActionDisp(&action);
        }
    } while ((u8)result != 3 && (u8)result == 1);
}

static inline u32 fightActionFlowEntry(void* action, FightActionFifoEntry* entry) {
    u32 result;

    if (entry == 0) {
        return 2;
    }
    result = fightActionFlow(action);
    if ((u8)result != 1) {
        fightActionFifoDrop();
    } else {
        fightActionBiosCopy(entry, action);
    }
    return result;
}

/* 0x801F0B00 | size: 0x404 */
u32 fightActionCreateAndFlowFifo(void* action, void* motoAction, void* actorTarget,
                                 u32 kind, u32 buff, void* data)
{
    u32 result;
    int index;
    FightActionFifoEntry* entry;

    result = fightActionCreateInline(action, motoAction, actorTarget, kind, buff, data);
    if ((u8)result != 1) {
        return result;
    }
    index = lbl_8047B5E8;
    if (((index + 1) & 0x1F) != lbl_8047B5EC) {
        entry = &lbl_8046D790[index];
        *entry = *(FightActionFifoEntry*)action;
        lbl_8047B5E8++;
        lbl_8047B5E8 = lbl_8047B5E8 % 32;
    } else {
        entry = 0;
        goto check;
    }
    fightActionBiosSetFifoBanme(action, index);
    fightActionBiosSetFifoBanme(entry, index);
check:
    return fightActionFlowEntry(action, entry);
}

/* 0x801F0F04 | size: 0x188 | medium */
u32 fightActionFlowFifo(void* action) {
    int index;
    FightActionFifoEntry* entry;
    u32 result;

    index = lbl_8047B5E8;
    if (((index + 1) & 0x1F) != lbl_8047B5EC) {
        entry = &lbl_8046D790[index];
        *entry = *(FightActionFifoEntry*)action;
        lbl_8047B5E8++;
        lbl_8047B5E8 = lbl_8047B5E8 % 32;
    } else {
        entry = 0;
        goto check;
    }
    fightActionBiosSetFifoBanme(action, index);
    fightActionBiosSetFifoBanme(entry, index);
check:
    if (entry == 0) {
        return 2;
    }
    result = fightActionFlow(action);
    if ((u8)result != 1) {
        fightActionFifoDrop();
    } else {
        fightActionBiosCopy(entry, action);
    }
    return result;
}

/* 0x801F108C fightActionFifoInit lives in fight_action_exact_801F108C.c. */

#else /* FIGHT_ACTION_RANGE_801F1170_ONLY */

/* 0x801F1170 | size: 0x5C | small */
u32 fightActionCheckValid(void* param) {
    extern u16 fightActionBiosGetKind(void*);
    extern void* fightActionBiosGetFightActionDataPtr(void*);
    void* obj;

    obj = param;
    if (obj == 0) {
        return 0;
    }
    if ((fightActionBiosGetKind(obj) & 0xFFFF) == 0) {
        return 0;
    }
    return -(s32)fightActionBiosGetFightActionDataPtr(obj) != 0;
}

/* 0x801F11CC | size: 0x294 | large */
s32 fightActionCreate(void* action, void* motoAction, void* actorTarget, u32 kind,
                      u32 buff, void* data) {
    return fightActionCreateInline(action, motoAction, actorTarget, kind, buff, data);
}

/* 0x801F1460 | size: 0xB4 | fightActionInit */
void fightActionInit(u8* ptr) {
    extern void fightActionBiosSetKind(u8*, u32);
    extern void fightActionBiosSetBuff(u8*, u32);
    extern void fightActionBiosSetFightActionDataPtr(u8*, u32);
    extern void fightActionBiosSetDispBuff(u8*, u32, u32);
    extern void fightActionBiosSetBuffDataPtr(u8*, u32);
    extern void fightActionBiosSetBuffDataId(u8*, u32);
    extern void fightActionBiosSetActorFightTargetPtr(u8*, u32);
    extern void fightActionBiosSetMotoFightActionDataPtr(u8*, u32);
    extern void fightActionBiosSetFifoBanme(u8*, s32);
    u32 i;

    fightActionBiosSetKind(ptr, 0);
    fightActionBiosSetBuff(ptr, 0);
    fightActionBiosSetFightActionDataPtr(ptr, 0);
    for (i = 0; (u16)i < 4; i++) {
        fightActionBiosSetDispBuff(ptr, i, 0);
    }
    fightActionBiosSetBuffDataPtr(ptr, 0);
    fightActionBiosSetBuffDataId(ptr, 0);
    fightActionBiosSetActorFightTargetPtr(ptr, 0);
    fightActionBiosSetMotoFightActionDataPtr(ptr, 0);
    fightActionBiosSetFifoBanme(ptr, -1);
}

#endif /* FIGHT_ACTION_RANGE_801F1170_ONLY */
