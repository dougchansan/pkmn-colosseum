/**
 * @file gs_vm.h
 * @brief GSvm -- Genius Sonority script virtual machine (shared layout and
 * operand-stack helpers).
 *
 * The GS VM translation unit, as far as the retail data shows:
 *   .text   0x800F106C-0x800F78A4: fn_800F106C (hand-written native-call
 *           trampoline) up to cms_InitThreadPool, the VM initializer (it prints
 *           the pool's last string, "GSvm ... failed", and fills
 *           lbl_80401BF8; its code is currently scored in input.c's unit).
 *   .rodata 0x80271068-0x80271300: one literal pool, "Stack overflow.\n",
 *           "Stack underflow.\n", the undefined-operand message, ... up to
 *           the init failure message. Handlers address it through a base
 *           register (base+0x14, +0x28, +0x120, ...).
 *   .data   0x803155D0-0x80315678: the 0x26-entry opcode table (0 = fn_800F6BBC
 *           ... 0x25 = fn_800F10E8) and "_vmThreadCreate", the name
 *           _vmThreadCreate prints.
 *   .sdata  0x80478B00 (VM manager), .sdata2 0x8047CCB8-0x8047CCC8 ("\n",
 *           0.0f, the int-to-float bias), .sbss2 0x8047E710 (the zero
 *           initializer of a GSVMValue), .sbss 0x8047AC38-0x8047AC48 (the
 *           native-call registers), .bss 0x80401A78-0x80401C10 (print
 *           buffers, native-call argument images, init state).
 * Compiler: GC/1.3.2 -O4,p. With pooled literals, GC/1.3 passes the pool's
 * first string as "mr r3,r31"; retail has "addi r3,r31,0x0" (fn_800F1A0C,
 * fn_800F1E38, fn_800F6D18), which GC/1.3.2 emits.
 *
 * The pool is TU-local and read by nearly every function in the range.
 * Functions retaining its base in a register (including fn_800F7434) link
 * with the source object that owns the pool. Some functions outside the
 * linked range address strings separately and remain .text-only units.
 *
 * The whole unit's source is src/game/gs_vm.c, linked over
 * 0x800F1A0C-0x800F75FC with the string pool it owns (the definitions
 * outside that range are under GS_VM_WHOLE_UNIT). Linked .text-only
 * pieces: fn_800F10E8, fn_800F13D0, fn_800F16C0, fn_800F75FC,
 * cms_UnregisterScript, cms_RegisterScript, and cms_InitThreadPool (inside input.c's unit).
 */
#ifndef GS_VM_H
#define GS_VM_H

#include "dolphin/types.h"

struct GSThread;

/* One loaded GS script bank, linked into GSVMPool.scripts by cms_RegisterScript. */
typedef struct GSVMScript {
    /* 0x00 */ u16   id;
    /* 0x02 */ u8    unk02[2];
    /* 0x04 */ u16   funcCount;
    /* 0x06 */ u16   relocCount;
    /* 0x08 */ u8    unk08[2];
    /* 0x0A */ u8    relocated;
    /* 0x0B */ u8    unk0B;
    /* 0x0C */ u32   relocOffset;    /* offset of the u32 relocation table */
    /* 0x10 */ s32   globalsOffset;
    /* 0x14 */ struct GSVMScript* next;
    /* 0x18 */ u32   funcOffsets[1]; /* funcCount bank offsets */
} GSVMScript;

/* A GS VM operand-stack slot. Script values are raw 32-bit words that a
 * native call's argument-type table reinterprets as an integer or a float;
 * the pops return it by value, which is why the target spills each popped
 * value through a stack temporary. */
typedef union GSVMValue {
    u32 u;
    s32 s;
    f32 f;
    char* p;
} GSVMValue;

/* GS VM interpreter context (one script task). */
typedef struct GSVMCtx GSVMCtx;

struct GSVMCtx {
    /* 0x00 */ GSVMScript* script;
    /* 0x04 */ u8    status;   /* 0 idle, 1 running, 2 waiting, 3 stop, 4 stop request */
    /* 0x05 */ u8    unk05;
    /* 0x06 */ u16   key;
    /* 0x08 */ u32   scriptId; /* script id << 16 | function index */
    /* 0x0C */ struct GSThread* thread;
    /* 0x10 */ void (*callback)(GSVMCtx* ctx, u32 result);
    /* 0x14 */ u8*   ip;
    /* 0x18 */ union GSVMValue* globals;
    /* 0x1C */ s32   frame;
    /* 0x20 */ u8    unk20[0x28 - 0x20];
    /* 0x28 */ s32   stackCount;
    /* 0x2C */ u8    unk2C[0x6C - 0x2C];
    /* 0x6C */ union GSVMValue stack[0x40]; /* operand stack; a push guards depth > 0x40 */
}; /* size 0x16C */

/* The VM manager (lbl_80401BF8, reached through lbl_80478B00). */
typedef struct GSVMPool {
    /* 0x00 */ u16   count;
    /* 0x02 */ u16   handle;         /* GSmem handle of the context array */
    /* 0x04 */ u16   lastKey;
    /* 0x06 */ u16   unk06;
    /* 0x08 */ GSVMScript* scripts;
    /* 0x0C */ GSVMCtx* contexts;
    /* 0x10 */ void* natives;        /* native-call table, set by fn_800F75FC */
} GSVMPool; /* size 0x14 */

/* The VM's data, defined in game/gs_vm.c. */
extern u8 lbl_80401A78[0x40];            /* print: one conversion spec */
extern u8 lbl_80401AB8[0x100];           /* print: output buffer */
extern f32 lbl_80401BB8[8];              /* native call: float argument image */
extern u32 lbl_80401BD8[8];              /* native call: integer argument image */
extern GSVMPool lbl_80401BF8;            /* the VM manager */
extern GSVMPool* lbl_80478B00;           /* -> lbl_80401BF8 */
extern u32 (*lbl_8047AC38)(void);        /* native call: function */
extern f32* lbl_8047AC3C;                /* native call: float argument image */
extern u32* lbl_8047AC40;                /* native call: integer argument image */


extern void GSlogWritef(const char* fmt, ...);

/*
 * The operand-stack messages, the first two strings of game/gs_vm.c's pool.
 * gs_vm.c defines them under these names (retail passes them as literals;
 * a named array in the owning TU gives the same base-register code and
 * layout) so the .text-only units carved from the TU (fn_800F10E8,
 * fn_800F13D0, fn_800F16C0) can link to them.
 */
extern const char lbl_80271068[];        /* "Stack overflow.\n" */
extern const char lbl_8027107C[];        /* "Stack underflow.\n" */
#define GS_VM_MSG_OVERFLOW lbl_80271068
#define GS_VM_MSG_UNDERFLOW lbl_8027107C

/*
 * Operand-stack helpers shared by the GS VM opcode handlers.  Recovered as
 * static inline: every handler expands the same push/pop sequences, each
 * push evaluates its value before the overflow guard, and every pop routes
 * its by-value result through a stack temporary before it lands in the
 * caller's own slot.
 */

/* Push a raw word. */
static inline void GSvmPush(GSVMCtx* ctx, u32 value)
{
    if (ctx->stackCount > 0x40) {
        GSlogWritef(GS_VM_MSG_OVERFLOW);
    } else {
        ctx->stack[ctx->stackCount++].u = value;
    }
}

/* Push a script value.  The slot is copied as a whole, so the value is read
 * from its home (often the operand fetch's result) at the store. */
static inline void GSvmPushValue(GSVMCtx* ctx, GSVMValue value)
{
    s32 sp;

    sp = ctx->stackCount;
    if (sp > 0x40) {
        GSlogWritef(GS_VM_MSG_OVERFLOW);
    } else {
        ctx->stackCount = sp + 1;
        ctx->stack[sp] = value;
    }
}

static inline GSVMValue GSvmPop(GSVMCtx* ctx)
{
    GSVMValue value;

    if (ctx->stackCount <= 0) {
        GSlogWritef(GS_VM_MSG_UNDERFLOW);
        value = ctx->stack[0];
    } else {
        value = ctx->stack[--ctx->stackCount];
    }
    return value;
}

/* Frame-relative slot: n == 0 is the callee index, n >= 1 the arguments. */
static inline GSVMValue* GSvmFrameSlot(GSVMCtx* ctx, s32 n)
{
    return &ctx->stack[ctx->frame + n];
}

/* Enter a native call (fn_800F10E8, fn_800F13D0, fn_800F16C0): skip the
 * call's operand word, read the argument count, save the frame base and the
 * count on the stack and point the frame at the callee slot. */
static inline void GSvmEnterNative(GSVMCtx* ctx)
{
    u16 argc;

    ctx->ip += 2;
    argc = *(u16*)ctx->ip;
    ctx->ip += 2;
    GSvmPush(ctx, ctx->frame);
    GSvmPush(ctx, argc);
    ctx->frame = ctx->stackCount - (argc + 2);
}

/* Leave a native call frame: pop the saved argument count and frame base,
 * restore the frame and discard the call's arguments. */
static inline void GSvmReturn(GSVMCtx* ctx)
{
    GSVMValue argc;
    GSVMValue frame;
    s32 i;

    argc = GSvmPop(ctx);
    frame = GSvmPop(ctx);
    ctx->frame = frame.s;
    for (i = 0; i < argc.s; i++) {
        GSvmPop(ctx);
    }
}

/* Script-context lookup, shared by the script control calls
 * (fn_800F6D18, fn_800F7068, fn_800F7108, fn_800F716C, fn_800F7274). */
extern void GSthreadTerminate(struct GSThread* thread);

/* The live context whose key is `key`, or NULL. */
static inline GSVMCtx* GSvmFindByKey(u16 key)
{
    GSVMPool* pool = lbl_80478B00;
    GSVMCtx* ctx;
    s32 i;

    for (i = 0; i < pool->count; i++) {
        ctx = &pool->contexts[i];
        if (ctx->status != 0 && ctx->key == key) {
            return ctx;
        }
    }
    return NULL;
}

/* Stop the script with `key` and terminate its thread. */
static inline s32 GSvmStopByKey(u16 key)
{
    GSVMCtx* ctx;

    ctx = GSvmFindByKey(key);
    if (ctx == NULL) {
        return 0;
    }
    ctx->status = 4;
    if (ctx->thread != NULL) {
        GSthreadTerminate(ctx->thread);
        ctx->thread = NULL;
    }
    return 0;
}

#endif /* GS_VM_H */
