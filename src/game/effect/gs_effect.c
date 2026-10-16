/**
 * @file gs_effect.c
 * @brief GSeffect -- Core effect manager for Pokemon Colosseum.
 *
 * Implements the fn_ symbols in [0x80130CE0, 0x8013151C), all matched at
 * 100% against the retail binary:
 *   GSeffectInit (GSEffectInit)        -- System initialisation
 *   _GSeffectDrawAll (effectUpdateTask)    -- Per-frame update callback
 *   _GSeffectUpdateAll (effectRenderTask)    -- Per-frame render callback
 *   GSeffectStop (GSEffectStop)        -- Stop an active effect
 *   GSeffectIsRunning (GSEffectIsActive)    -- Query effect active state
 *   GSeffect (GSEffectTrigger)     -- Trigger an effect by ID (real symbol)
 *   GSeffectSetCallbacks (GSEffectRegister)    -- Register callbacks for an effect
 *   GSeffectFree (GSEffectFree)        -- Destroy and free an effect slot
 *   GSeffectSetup (GSEffectResetState)  -- Re-trigger an existing effect
 *   GSeffectCreate (GSEffectAllocSlot)   -- Allocate a new effect slot
 *
 * A prior campaign transplant had also left a full second, invented copy
 * of these ten functions under "GSEffectXxx"/effectUpdateTask/
 * effectRenderTask names (plus an invented effectLookup helper), none of
 * which were ever called from anywhere else in the tree -- the real fn_
 * names above are what every other file's extern declarations reference
 * (see effect_visual.c, tracefx.c). That dead duplicate has been removed;
 * only the matched fn_ implementations remain.
 *
 * Debug strings:
 *   "GSeffect: Cannot trigger effect - instance uninitialised: effect ID %d."
 *       (lbl_80272A58 -- referenced in GSeffect)
 *
 * Global state:
 *   lbl_803635C0 -- GSEffectGlobals (0x18 bytes in .data)
 *
 * The effect system uses a pool of GSEffectInstance entries (each 0x34 bytes).
 * Slots are managed via two doubly-linked lists: a free list (for unused
 * slots) and an active list (for running effects).
 *
 * Per-frame execution is driven by two task callbacks registered with
 * GSgappCreate (GStaskRegister):
 *   - _GSeffectUpdateAll (effectRenderTask) at priority 0x7F (render pass)
 *   - _GSeffectDrawAll (effectUpdateTask) at priority 0x80 (after rendering)
 *
 * GSeffectGetData (GSEffectGetUserData) was merged in from the former
 * game/effect/effect_util.c CodeCandidate bucket: it is a boundary function
 * that operates on the same lbl_803635C0 state table as the rest of this
 * file and was misattributed to the neighboring TU.
 *
 * Address range: 0x80130CE0 - 0x80131574
 */

#include "dolphin/types.h"
#include "game/effect/gs_effect.h"
#include "game/gs_mem.h"

/* ===== External SDK / engine functions ===== */
extern void  GSlogWrite(const char* fmt, ...);          /* OSReport / GSlog */
extern void  fn_800E24B0(u16 handle);                    /* GSmemLock (raw) */
extern void  fn_800E209C(u16 handle);                    /* GSmemFree (raw) */
extern u32   GSgetFrameStep(void);                          /* GSgfxGetTickCount */
extern s32   GSvtrRegisterGSgapp(u32 taskId);            /* battle VFX registration */
extern void  memset(void* dst, u32 val, u32 size);
extern void  memcpy(void* dst, void* src, u32 size);

/* ===== String constants (rodata) ===== */
extern const char lbl_80272A58[]; /* "GSeffect: Cannot trigger effect..." */

/* ===== Global state ===== */
extern u8 lbl_803635C0[];              /* master effect globals, 0x18 bytes */
#define gsEffectGlobals (*(GSEffectGlobals*)(void*)lbl_803635C0)

/* Forward declarations for converted functions */
void GSeffectInit(u16 maxEffects);
void _GSeffectDrawAll(void);
void _GSeffectUpdateAll(void);
void GSeffectStop(u32 effectId);
BOOL GSeffectIsRunning(u32 effectId);
BOOL GSeffect(u32 effectId);
void GSeffectSetCallbacks(u32 effectId, GSEffectStartFunc startFunc,
                 GSEffectStopFunc destroyFunc,
                 GSEffectStartFunc triggerFunc,
                 GSEffectStopFunc stopFunc,
                 void* extraParam,
                 GSEffectUpdateFunc updateFunc,
                 GSEffectRenderFunc renderFunc);
void GSeffectFree(u32 effectId);
void GSeffectSetup(u32 effectId, u32 param);
u16 GSeffectCreate(void* callbacks, u16 dataSize);
void* GSeffectGetData(u32 arg);



/* ===== Global state ===== */
extern u32 lbl_8047ADC0;

/* =======================================================================
 * GSeffectInit / GSEffectInit
 * Address: 0x80130CE0, Size: 0x224
 * ======================================================================= */
void GSeffectInit(u16 maxEffects) {
    u32 memHandle;
    u32 taskId;
    GSEffectInstance* table;
    GSEffectInstance* entry;
    u16 cnt;
    extern u16 _toolentryAlloc__FUl(u32);
    extern void* fn_800E27B0(u16);
    extern u32 GSgappCreate(s32 state, u8 priority, void* param, void* func);

    memHandle = _toolentryAlloc__FUl(maxEffects * sizeof(GSEffectInstance));
    gsEffectGlobals.memHandle = memHandle;

    if (memHandle != 0) {
        table = (GSEffectInstance*)fn_800E27B0(memHandle);
        gsEffectGlobals.instanceTable = table;
        memset(table, 0, maxEffects * sizeof(GSEffectInstance));
        gsEffectGlobals.freeListHead = table;
        table->prev = NULL;
        table->next = table + 1;
        table->id = 1;
        entry = table + 1;
        for (cnt = 1; cnt < maxEffects - 1; cnt++) {
            entry->prev = entry - 1;
            entry->next = entry + 1;
            entry->id = cnt + 1;
            entry++;
        }
        entry->prev = entry - 1;
        entry->next = NULL;
        entry->id = maxEffects;
        entry->state = GSEFFECT_STATE_UNINIT;
        *(volatile u32*)&gsEffectGlobals.maxEffects = (u32)maxEffects;
    } else {
        gsEffectGlobals.instanceTable = NULL;
        gsEffectGlobals.freeListHead = NULL;
        gsEffectGlobals.maxEffects = 0;
    }
    gsEffectGlobals.activeListHead = NULL;
    taskId = GSgappCreate(1, 0x7F, 0, (void*)_GSeffectUpdateAll);
    lbl_8047ADC0 = taskId;
    GSvtrRegisterGSgapp(taskId);
    GSgappCreate(1, 0x80, 0, (void*)_GSeffectDrawAll);
}

/* =======================================================================
 * _GSeffectDrawAll / effectUpdateTask
 * Address: 0x80130F04, Size: 0x64
 * ======================================================================= */
void _GSeffectDrawAll(void) {
    GSEffectInstance* cur;

    cur = ((GSEffectGlobals*)(void*)lbl_803635C0)->activeListHead;
    while (cur != NULL) {
        if (cur->renderFunc != NULL && cur->state == GSEFFECT_STATE_ACTIVE) {
            cur->renderFunc(cur->userData);
        }
        cur = cur->next;
    }
}

/* =======================================================================
 * _GSeffectUpdateAll / effectRenderTask
 * Address: 0x80130F68, Size: 0xA8
 * ======================================================================= */
void _GSeffectUpdateAll(void) {
    GSEffectInstance* cur;
    u32 tick;

    cur = ((GSEffectGlobals*)(void*)lbl_803635C0)->activeListHead;
    tick = GSgetFrameStep();

    while (cur != NULL) {
        if (cur->updateFunc != NULL && cur->state == GSEFFECT_STATE_ACTIVE) {
            if ((u32)((GSEffectStartFunc)cur->updateFunc)(cur->userData, tick) == 0) {
                cur->state = GSEFFECT_STATE_STOPPING;
                if (cur->stopFunc != NULL) {
                    cur->stopFunc(cur->userData);
                }
            }
        }
        cur = cur->next;
    }
}

/* =======================================================================
 * GSeffectStop / GSEffectStop
 * Address: 0x80131010, Size: 0x98
 * ======================================================================= */
void GSeffectStop(u32 effectId) {
    GSEffectInstance* inst;
    GSEffectGlobals* g;

    if (effectId == 0) {
        goto _null;
    }
    g = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (effectId > g->maxEffects) {
        goto _null;
    }
    inst = (GSEffectInstance*)((u8*)g->instanceTable +
                               (effectId - 1) * sizeof(GSEffectInstance));
    switch (inst->state) {
    case GSEFFECT_STATE_UNINIT:
        goto _null;
    }
    goto _got_inst;
_null:
    inst = NULL;
_got_inst:
    if (inst == NULL) {
        return;
    }
    {
        s32 state = inst->state;
        GSEffectStopFunc stopFn = inst->stopFunc;
        if (state == GSEFFECT_STATE_IDLE) {
            return;
        }
        if (state == GSEFFECT_STATE_STOPPING) {
            return;
        }
        inst->state = GSEFFECT_STATE_STOPPING;
        if (stopFn != NULL) {
            stopFn(inst->userData);
        }
    }
}

/* =======================================================================
 * GSeffectIsRunning / GSEffectIsActive
 * Address: 0x801310A8, Size: 0x74
 * ======================================================================= */
BOOL GSeffectIsRunning(u32 effectId) {
    GSEffectInstance* inst;
    GSEffectGlobals* g;

    if (effectId == 0) {
        goto _null;
    }
    g = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (effectId > g->maxEffects) {
        goto _null;
    }
    inst = (GSEffectInstance*)((u8*)g->instanceTable +
                               (effectId - 1) * sizeof(GSEffectInstance));
    switch (inst->state) {
    case GSEFFECT_STATE_UNINIT:
        goto _null;
    }
    goto _got_inst;
_null:
    inst = NULL;
_got_inst:
    if (inst == NULL) {
        goto _ret0;
    }
    if (inst->state == GSEFFECT_STATE_UNINIT) {
        goto _ret0;
    }
    if (inst->state == GSEFFECT_STATE_IDLE) {
        goto _ret0;
    }
    if (inst->state == GSEFFECT_STATE_STOPPING) {
        goto _ret0;
    }
    return TRUE;
_ret0:
    return FALSE;
}

/* =======================================================================
 * GSeffect / GSEffectTrigger
 * Address: 0x8013111C, Size: 0xE4
 * ======================================================================= */
BOOL GSeffect(u32 effectId) {
    GSEffectInstance* inst;
    GSEffectGlobals* g;
    GSEffectStartFunc trigFn;
    GSEffectStopFunc stopFn;

    inst = NULL;
    if (effectId == 0) {
        goto _null;
    }
    g = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (effectId > g->maxEffects) {
        goto _null;
    }
    inst = (GSEffectInstance*)((u8*)g->instanceTable +
                               (effectId - 1) * sizeof(GSEffectInstance));
    switch (inst->state) {
    case GSEFFECT_STATE_UNINIT:
        goto _null;
    }
    goto _got_inst;
_null:
    inst = NULL;
_got_inst:
    if (inst == NULL) {
        goto _final0;
    }
    if (inst->state == GSEFFECT_STATE_IDLE) {
        goto _print_error;
    }
    trigFn = inst->triggerFunc;
    inst->state = GSEFFECT_STATE_ACTIVE;
    if (trigFn == NULL) {
        goto _final0;
    }
    if ((u32)((BOOL(*)(void*))trigFn)(inst->userData) != 0) {
        goto _ret1;
    }
    stopFn = inst->stopFunc;
    inst->state = GSEFFECT_STATE_STOPPING;
    if (stopFn != NULL) {
        stopFn(inst->userData);
    }
    return FALSE;
_ret1:
    return TRUE;
_print_error:
    GSlogWrite(lbl_80272A58);
_final0:
    return FALSE;
}

/* =======================================================================
 * GSeffectSetCallbacks / GSEffectRegister
 * Address: 0x80131200, Size: 0x68
 * ======================================================================= */
void GSeffectSetCallbacks(u32 effectId, GSEffectStartFunc startFunc,
                 GSEffectStopFunc destroyFunc,
                 GSEffectStartFunc triggerFunc,
                 GSEffectStopFunc stopFunc,
                 void* extraParam,
                 GSEffectUpdateFunc updateFunc,
                 GSEffectRenderFunc renderFunc) {
    GSEffectInstance* inst;
    GSEffectGlobals* g;

    if (effectId == 0) {
        goto _null;
    }
    g = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (effectId > g->maxEffects) {
        goto _null;
    }
    inst = (GSEffectInstance*)((u8*)g->instanceTable +
                               (effectId - 1) * sizeof(GSEffectInstance));
    switch (inst->state) {
    case GSEFFECT_STATE_UNINIT:
        goto _null;
    }
    goto _got_inst;
_null:
    inst = NULL;
_got_inst:
    if (inst == NULL) {
        return;
    }

    inst->startFunc   = startFunc;
    inst->destroyFunc = destroyFunc;
    inst->triggerFunc = triggerFunc;
    inst->stopFunc    = stopFunc;
    inst->extraParam  = extraParam;
    inst->updateFunc  = updateFunc;
    inst->renderFunc  = renderFunc;
}

/* =======================================================================
 * GSeffectFree / GSEffectFree
 * Address: 0x80131268, Size: 0x134
 * ======================================================================= */
void GSeffectFree(u32 effectId) {
    GSEffectInstance* inst;
    GSEffectInstance* prevInst;
    GSEffectInstance* nextInst;
    GSEffectGlobals* g;

    inst = NULL;
    if (effectId == 0) {
        goto _null;
    }
    g = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (effectId > g->maxEffects) {
        goto _null;
    }
    inst = (GSEffectInstance*)((u8*)g->instanceTable +
                               (effectId - 1) * sizeof(GSEffectInstance));
    switch (inst->state) {
    case GSEFFECT_STATE_UNINIT:
        goto _null;
    }
    goto _got_inst;
_null:
    inst = NULL;
_got_inst:
    if (inst == NULL) {
        return;
    }
    prevInst = inst->prev;
    nextInst = inst->next;

    if (inst->state != GSEFFECT_STATE_IDLE) {
        if (inst->state != GSEFFECT_STATE_STOPPING) {
            if (inst->stopFunc != NULL) {
                inst->stopFunc(inst->userData);
            }
        }
        if (inst->destroyFunc != NULL) {
            inst->destroyFunc(inst->userData);
        }
        inst->state = GSEFFECT_STATE_UNINIT;
    }

    if (prevInst != NULL) {
        prevInst->next = nextInst;
    } else {
        ((GSEffectGlobals*)(void*)lbl_803635C0)->activeListHead = nextInst;
    }

    if (nextInst != NULL) {
        nextInst->prev = prevInst;
    }

    inst->prev = NULL;
    {
        GSEffectGlobals* gf = (GSEffectGlobals*)(void*)lbl_803635C0;
        inst->next = gf->freeListHead;
        if (gf->freeListHead != NULL) {
            gf->freeListHead->prev = inst;
        }
        gf->freeListHead = inst;
    }

    fn_800E24B0(inst->memHandle);
    fn_800E209C(inst->memHandle);
}

/* =======================================================================
 * GSeffectSetup / GSEffectResetState
 * Address: 0x8013139C, Size: 0x8C
 * ======================================================================= */
void GSeffectSetup(u32 effectId, u32 param) {
    GSEffectInstance* inst;
    GSEffectGlobals* g;
    GSEffectStartFunc startFunc;

    inst = NULL;
    if (effectId == 0) {
        goto _null;
    }
    g = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (effectId > g->maxEffects) {
        goto _null;
    }
    inst = (GSEffectInstance*)((u8*)g->instanceTable +
                               (effectId - 1) * sizeof(GSEffectInstance));
    switch (inst->state) {
    case GSEFFECT_STATE_UNINIT:
        goto _null;
    }
    goto _got_inst;
_null:
    inst = NULL;
_got_inst:
    if (inst == NULL) {
        return;
    }
    startFunc = inst->startFunc;
    if (startFunc != NULL) {
        startFunc(inst->userData, param);
    }
    inst->state = GSEFFECT_STATE_STOPPING;
}

/* =======================================================================
 * GSeffectCreate / GSEffectAllocSlot
 * Address: 0x80131428, Size: 0xF4
 * ======================================================================= */
u16 GSeffectCreate(void* callbacks, u16 dataSize) {
    GSEffectGlobals* globals;
    void* cbArg;
    u16 cbSize;
    GSEffectInstance* slot;
    u16 savedId;
    void* userData;
    u16 memHandle;
    GSEffectGlobals* ga;
    extern u16 _toolentryAlloc__FUl(u32 size);
    extern void* fn_800E27B0(u16 handle);

    globals = (GSEffectGlobals*)(void*)lbl_803635C0;
    cbArg = callbacks;
    cbSize = dataSize;
    slot = globals->freeListHead;
    if (slot == NULL) {
        goto _retNull;
    }

    savedId = slot->id;
    memHandle = _toolentryAlloc__FUl((u32)cbSize);
    if (memHandle == 0) {
        return 0;
    }

    userData = fn_800E27B0(memHandle);

    globals->freeListHead = slot->next;
    if (slot->next != NULL) {
        slot->next->prev = NULL;
    }

    memset(slot, 0, sizeof(GSEffectInstance));
    slot->id = savedId;

    ga = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (ga->activeListHead != NULL) {
        ga->activeListHead->prev = slot;
    }
    slot->next = ga->activeListHead;
    ga->activeListHead = slot;
    slot->prev = NULL;
    slot->memHandle = memHandle;
    slot->userData = userData;

    memcpy(userData, cbArg, (u32)cbSize);
    slot->dataSize = dataSize;
    slot->state = GSEFFECT_STATE_IDLE;

    return slot->id;
_retNull:
    return 0;
}

/* =======================================================================
 * GSeffectGetData / GSEffectGetUserData
 * Address: 0x8013151C, Size: 0x58
 *
 * Merged in from the former game/effect/effect_util.c CodeCandidate
 * bucket (0x8013151C - 0x80137114): this boundary function operates on
 * the same lbl_803635C0 state table as the rest of this file.
 * ======================================================================= */
void* GSeffectGetData(u32 arg) {
    GSEffectInstance* entry;
    unsigned int new_var;
    GSEffectGlobals* globals;
    if (arg == 0) goto _ret0;
    globals = (GSEffectGlobals*)(void*)lbl_803635C0;
    if (arg > globals->maxEffects) goto _ret0;
    entry = &((GSEffectInstance*)globals->instanceTable)[arg - 1];
    new_var = entry->state == GSEFFECT_STATE_UNINIT;
    if (new_var) goto _ret0;
    if ((arg && arg) && arg) {
    }
    goto _compute;
_ret0:
    entry = 0;
_compute:
    if (entry == 0) goto _null_ret;
    return entry->userData;
_null_ret:
    return 0;
}
