/**
 * @file gs_vm.c
 * @brief GSvm -- Genius Sonority script virtual machine (the whole unit).
 *
 * Address range: 0x800F10E8 - 0x800F78A4 (fn_800F10E8 .. fn_800F7758), with
 * its data: the .rodata string pool 0x80271068-0x80271300, the opcode table
 * and fn_800F7318's name (.data 0x803155D0-0x80315678), the manager pointer
 * (.sdata 0x80478B00), "\n", 0.0f and the int-to-float bias (.sdata2
 * 0x8047CCB8-0x8047CCC8), the GSVMValue zero initializer (.sbss2
 * 0x8047E710), the native-call registers (.sbss 0x8047AC38) and buffers
 * (.bss 0x80401A78-0x80401C10). See include/game/gs_vm.h. The unit opens
 * with fn_800F106C, the hand-written native-call trampoline, which stays in
 * its own unit.
 *
 * Build: GC/1.3.2 -O4,p with the project's normal inlining, -rostr (the
 * pool is .rodata). No pragmas. Every definition is in address order and
 * the string pool comes out in retail's first-use order.
 *
 * Linked (lane V23b, 2026-09-28) over the split 0x800F1A0C-0x800F75FC with
 * the pool (.rodata 0x80271068-0x80271300), the manager pointer (.sdata
 * 0x80478B00), "\n"/0.0f/int-to-float bias (.sdata2 0x8047CCB8-0x8047CCC8,
 * moved here from game/data/sdata2_8047CC98.c because they are this TU's
 * own literals) and the .sbss2 zero initializer. The range was extended
 * through fn_800F7434 after the pooled-literal caller matched exactly;
 * fn_800F7068 through fn_800F7434 now link in this same object. The
 * definitions outside that range remain under GS_VM_WHOLE_UNIT; the linked
 * .text-only pieces are gs_vm_exact_800F10E8.c,
 * gs_vm_exact_800F13D0.c, gs_vm_exact_800F16C0.c,
 * input_exact_800F75FC.c, input_candidate_800F760C.c,
 * input_exact_800F76E4.c, and input.c's fn_800F7758.
 * The pool strings those pieces print are defined here under
 * their symbol names (tagged RULE-EXCEPTION(title-path) below): a named
 * const array gives the same base-register code and pool layout as the
 * literal, and the pieces link to it by name.
 *
 * Rule exceptions (title-path directive, 2026-09-28; see
 * docs/RULE_EXCEPTIONS.md): the dead `desc &= ~0x80;` in GSvmGetOperand
 * (makes fn_800F5A3C and fn_800F5CA0 exact, see "Formerly not exact"
 * below), and the named pool strings above.
 *
 * Reconstructed static inline helpers (all repeated expansions):
 * GSvmPush/GSvmPushValue/GSvmPop/GSvmFrameSlot/GSvmEnterNative/GSvmReturn
 * (gs_vm.h), GSvmGetOperand (every operand read: two per binary operator,
 * one in fn_800F24F4/fn_800F5404/fn_800F55DC/fn_800F57F0/fn_800F5A3C/
 * fn_800F5CA0), and in gs_vm.h GSvmFindByKey (fn_800F6D18, fn_800F7068,
 * fn_800F7108, fn_800F716C, fn_800F7274) and GSvmStopByKey (fn_800F716C,
 * fn_800F7274).
 * setValue (fn_800F24F4) is named by retail's own message, "setValue
 * 例外エラー...", the same way "_codeStructAssign:" names fn_800F2264; it
 * has no symbol of its own, so it was inlined.
 *
 * Formerly not exact (they blocked linking the unit; both 100% since lane
 * V23b through the RULE-EXCEPTION dead store, analysis kept for a clean fix):
 *   fn_800F5A3C  91.8  MWCC hoists the operand descriptor's bit tests
 *                      (desc & 0x80/0x20/0x40/0x100) out of the loop into
 *                      four registers and then keeps `start` in its stack
 *                      slot; retail tests them inside the loop and keeps
 *                      start in r30. No loop form (for/while/do, s32/u32/
 *                      int counters, u16/s32/u32/int count, u8/u16/u32/s32
 *                      descriptor, start as value or word) nor a shared
 *                      push-operand helper stops the hoisting.
 *                      Cause (lane U1, 2026-09-28): the inliner replaces
 *                      the helper's u16 `desc` with the caller's (u16)desc,
 *                      so the four tests are invariant expressions and
 *                      LICM hoists them. A u16 copy assigned inside the
 *                      loop body (the operand fetch written in place) keeps
 *                      them in the loop with retail's rlwinm. forms and
 *                      hoists only the clrlwi, as retail does, but loses the
 *                      helper's early-return temps (retail's 0x14/0x8/0x1C
 *                      slots) and still reloads `start`, so it is not
 *                      retail's source either. The helper's parameter must
 *                      stay u16: u32/s32/int/s16 break 19 other handlers.
 *                      GC/1.3 to GC/2.7 all hoist the helper form.
 *                      Lane B23 (2026-09-28): 89.9 -> 91.8 by naming the
 *                      pushed variable index (see the function). The rest
 *                      is one property of GSvmGetOperand: in retail its
 *                      `desc` parameter behaves as a real local of the
 *                      inlined body (bound to the argument at each
 *                      expansion), not a substituted expression. MWCC's
 *                      inliner (CInline_SetupArgsExpression) binds a
 *                      parameter that the body assigns to; any assignment
 *                      does it, even a dead `desc = 0;` before the final
 *                      `return value;`. That probe makes fn_800F5A3C AND
 *                      fn_800F5CA0 100% at once, with every other handler
 *                      unchanged (19 callers stay exact), so it is strong
 *                      evidence of retail's helper shape. It is NOT applied:
 *                      a dead store is a dummy assignment under the policy,
 *                      and `const`/`register`/`unsigned short` parameters,
 *                      `*&desc`, `desc &= 0xFFFF` (98.3%) and a plain
 *                      `static` helper do not reproduce it. In the caller,
 *                      a u16 copy of desc assigned inside the loop also gives
 *                      it (pure copy: rejected). Still wanted: a natural
 *                      helper body that assigns to its descriptor parameter.
 *                      Lane B23b (2026-09-28) tried consuming forms of the
 *                      helper. Only an assignment that is dead leaves the
 *                      other 19 callers exact: e.g. `desc &= ~0x80;` after
 *                      the pop in the 0x80 branch (the stack flag consumed)
 *                      makes fn_800F5A3C and fn_800F5CA0 100% with nothing
 *                      else changed, but desc is never read after it, so it
 *                      is the same dead store as `desc = 0;` (not applied).
 *                      Any live assignment moves the later bit tests to
 *                      the caller's byte (copy propagation of the masked
 *                      value), so code changes: `desc &= ~0x20;` inside the
 *                      0x20 branch gives fn_800F5CA0 100% but the 0x40 test
 *                      there reads the caller's register in all callers
 *                      (-0.04% each, fn_800F5404 98.1); `desc &= ~0x3F`
 *                      after the zero check, the same in the else branch,
 *                      `desc &= ~0x80` after the index pop, and folding
 *                      0x20 into 0x100 (`if (desc & 0x20) desc |= 0x100;`
 *                      with one address path) all lose 1-20% everywhere.
 *                      An address-request parameter (`if (wantAddr) desc
 *                      |= 0x100;`, fn_800F5CA0 passing 1) gives fn_800F5A3C
 *                      100% but leaves fn_800F5CA0 at 99.46 (the bound
 *                      copy is then (u16)desc, not (u16)(desc | 0x100)).
 *                      So retail's helper most likely held a dead store to
 *                      its parameter; no admissible source is known.
 *                      Lane V23 (2026-09-28), GC/2.6 IRO trace (GC/2.6
 *                      reproduces both probes byte for byte): retail needs
 *                      the bound copy to be a PURE `desc' = (u16)desc`.
 *                      LICM hoists (u16)desc to a preheader temp, copy
 *                      propagation replaces the bound local with it, and the
 *                      0x80 test's CSE temp (int)temp then reuses the raw
 *                      byte (r26) while 0x20/0x40/0x100 test the clrlwi copy
 *                      (r31). Findings:
 *                      - The inliner substitutes every side-effect-free
 *                        argument: `desc & 0xFF`, `(u8)desc`, `desc | 0`,
 *                        `(u16)(u32)desc`, `desc + 0` in fn_800F5A3C all
 *                        leave it at 91.8 (92.4 for the first two). A caller
 *                        local assigned twice is still substituted. Caller
 *                        descriptors other than u8 (u16/u32/s32/int/s16/
 *                        char) break all 19 exact handlers.
 *                      - Taking the parameter's address also binds it as a
 *                        pure copy: `GSlogWritef(..., rd(&desc))` with
 *                        `static inline u16 rd(u16* p) { return *p; }` gives
 *                        fn_800F5A3C and fn_800F5CA0 100% with nothing else
 *                        changed. The reader is an invented helper and a
 *                        pointer alias, so it is NOT applied.
 *                      - A K&R (unprototyped) definition,
 *                        `GSvmGetOperand(ctx, desc) GSVMCtx* ctx; u16 desc;`,
 *                        binds the parameter through the default argument
 *                        promotion: fn_800F5CA0 100%, the other 19 unchanged,
 *                        fn_800F5A3C 98.29. The bound value is
 *                        (u16)(int)desc, so the hoisted temp is (int)desc,
 *                        the in-loop (u16) is not a pure copy, and the 0x80
 *                        test reads the u16 copy: r26 dies, one register
 *                        less is saved (frame 0x40, not 0x50). Caller locals
 *                        of u16/u32/int/s32 or a `(u16)` cast do not change
 *                        that. setValue written the same way breaks
 *                        fn_800F24F4 (97.7), so the unit was not written
 *                        K&R; not applied.
 *                      - A live `desc &= 0x1FF;` on entry gives the same
 *                        K&R shape (98.26 / 99.97): any value-changing
 *                        binding moves the 0x80 test off the raw byte.
 *                      - A shared address helper for the 0x40 frame/global
 *                        choice (called in the 0x20 and non-0x20 branches)
 *                        lowers all 21 callers to about 99.2.
 *                      - XD has no GS VM (its scripts are tiga/tvariant),
 *                        so the sister-title clause has nothing to offer.
 *                        find_inline_expansions finds no match for the
 *                        operand fetch in xd-asm.
 *                      Lane V23b (2026-09-28): evidenced release-build
 *                      debug constructs do not bind the parameter. Tested
 *                      in GSvmGetOperand, all leaving 91.77/99.46 and the
 *                      other 19 unchanged: GS_ASSERT (the release assert
 *                      `((cond) ? (void)0 : (void)0)` of
 *                      gs_range_8017FA5C_exact_80180320.c) on desc < 0x200,
 *                      desc != 0, desc & 0x3F at entry, after the zero
 *                      check, in the 0x80/else/0x20 branches and before the
 *                      return; `(void)desc;` and `(void)sizeof(desc);`;
 *                      GS_ASSERT(&desc) and GS_ASSERT(*&desc < 0x200). Only
 *                      `(void)&desc;` binds (100/100, nothing else changed),
 *                      but no macro in this codebase or retail takes an
 *                      operand's address, so it has no evidence. GC/2.6 IRO
 *                      trace: with GS_ASSERT, fn_800F5A3C's log equals the
 *                      plain one except "Removing branch around goto" and
 *                      "Nop out with side-effects checking" (the empty-armed
 *                      conditional is deleted); the inliner already
 *                      substituted `(u16)desc` into the condition. With
 *                      `(void)&desc` the log gains "Found propagatable
 *                      expression assignment" (the bound `desc' =
 *                      (u16)desc`), which is what retail needs. The inliner
 *                      binds a parameter only when the body assigns it or
 *                      takes its address; a read never does. Retail's
 *                      .rodata pool (0x80271068-0x80271300) holds no assert
 *                      text, file name or descriptor message other than the
 *                      two "変数の型が未定義..." logs already in the source.
 *   fn_800F5CA0  99.5  the descriptor and the pool base swap r30/r31 (the
 *                      descriptor is coloured first here); every
 *                      declaration order, descriptor type, pop/push form
 *                      and an ip-fetch helper give the same colouring.
 *                      Lane B23: same cause as fn_800F5A3C (the helper's
 *                      parameter must be a bound local; the dead-store
 *                      probe gives 100%). A named `u16 flags = desc |
 *                      0x100;` passed to the helper also gives 100%, but
 *                      retail's instruction order is the same as with the
 *                      expression argument (only registers differ), so the
 *                      "named computed values" clause does not cover it;
 *                      not applied.
 *   (fn_800F6D18 is exact since lane B23: the key search reads
 *   lbl_80478B00->lastKey directly, see the comment there.)
 *
 * Regalloc replay notes (lane TW, GC/2.6 replay; the allocator's order is
 * reproduced exactly by: rescan the virtual registers in ascending number,
 * pushing each with fewer than 29 neighbours, spill-pick the lowest
 * cost/degree when stuck, colour in reverse push order):
 *   fn_800F5CA0 (replay faithful): after the first scan only ctx (r32), the
 *     pool base (r40) and GSvmGetOperand's `desc | 0x100` (r61) remain, so
 *     they are pushed in that order and coloured r61, r40, ctx. Retail's
 *     r40 -> r31, r61 -> r30, ctx -> r29 needs ctx pushed first and r40
 *     last, which no renumbering of one value and no single added or removed
 *     interference gives; retail's interference graph is different there.
 *     `u16 desc; desc |= 0x100;` does not help (the parameter is still its
 *     own temporary).
 *   fn_800F6D18 (replay not faithful): the pool pointer is always a
 *     temporary (the global load, shared with GSvmFindByKey's copy), which
 *     is coloured before any declared local, so a declared `lastKey` loses
 *     r6 to it. Solved (lane B23): with no `lastKey` and no `pool` local in
 *     the key search, `key = lbl_80478B00->lastKey` and the wrap test
 *     `key == lbl_80478B00->lastKey` give retail's lhz r6 / mr r29,r6
 *     (the hoisted re-read of lastKey is a temporary numbered after the
 *     pointer). Through a `pool` local the same form stays at 99.81%.
 */

#include "dolphin/types.h"
#include "crt/stdarg.h"
#include "game/gs_thread.h"
#include "game/gs_vm.h"

/* RULE-EXCEPTION(title-path): pool literals given global names (named
 * stand-ins for the TU's own literals) - see docs/RULE_EXCEPTIONS.md.
 * Retail passes these as literals of this TU's pool; the .text-only
 * pieces carved from the unit (fn_800F10E8, fn_800F13D0, fn_800F16C0,
 * fn_800F7318, fn_800F760C, input.c's fn_800F7758) link to them by
 * name. A named const array gives the same code (base-register
 * addressing) and the same pool layout as the literal. */
const char lbl_80271068[] = "Stack overflow.\n";
const char lbl_8027107C[] = "Stack underflow.\n";
/* RULE-EXCEPTION(title-path): named stand-in for the TU's "\n" literal
 * (.sdata2, used by fn_800F16C0) - see docs/RULE_EXCEPTIONS.md. It opens
 * the unit's .sdata2 ahead of the 0.0f and int-to-float literals. */
const u8 lbl_8047CCB8[2] = "\n";

extern void* memset(void* dest, int val, u32 n);
extern s32 sprintf(u8* buf, const char* fmt, ...);
extern f64 fmod(f64 value, f64 modulus);
extern void GSlogWrite(const char* fmt, ...);
extern void GSlogWritef(const char* fmt, ...);
extern u32 cms_CallStdTrampoline(void);
extern u32 GSgetFrameStep(void);
extern void _threadSwitch(void);
extern u32 fn_800F0374(GSThread* thread);
extern void GSthreadTerminate(GSThread* thread);
extern u32 floorGetCurrentContext(void);
extern void GSthreadSetArgs(void* thread, s32 count, ...);
extern u16 _toolentryAlloc__FUl(u32 size);   /* GSmemAllocRaw */
extern void* fn_800E27B0(u16 handle);         /* GSmemGetPtr */

#ifdef GS_VM_WHOLE_UNIT
u8 lbl_80401A78[0x40];          /* print: one conversion spec */
u8 lbl_80401AB8[0x100];         /* print: output buffer */
f32 lbl_80401BB8[8];            /* native call: float argument image */
u32 lbl_80401BD8[8];            /* native call: integer argument image */
GSVMPool lbl_80401BF8;          /* the VM manager */
#endif
GSVMPool* lbl_80478B00 = &lbl_80401BF8;
#ifdef GS_VM_WHOLE_UNIT
u32 (*lbl_8047AC38)(void);      /* native call: function */
f32* lbl_8047AC3C;              /* native call: float argument image */
u32* lbl_8047AC40;              /* native call: integer argument image */
#endif

/*
 * Operand fetch.  Each opcode byte is followed by one descriptor byte per
 * operand: 0 is undefined (logged, reads as 0), bit 0x80 means the value
 * itself is on the operand stack, otherwise the stack holds a variable index
 * into the frame (0x40) or the globals, and 0x20/0x100 ask for the variable's
 * address instead of its contents.  The low six bits are the type (2 = int,
 * anything else is read as a float).
 */
static inline GSVMValue GSvmGetOperand(GSVMCtx* ctx, u16 desc)
{
    GSVMValue value = {0};
    GSVMValue idx;
    GSVMValue* addr;

    if (desc == 0) {
        GSlogWritef("変数の型が未定義です:[%02x]\n", desc);
        return value;
    }
    if (desc & 0x80) {
        value = GSvmPop(ctx);
        /* RULE-EXCEPTION(title-path): dead store to an inline parameter
         * (dummy assignment) - see docs/RULE_EXCEPTIONS.md. It makes the
         * inliner bind `desc` as a pure copy of the argument, which retail's
         * fn_800F5A3C/fn_800F5CA0 codegen requires (see the file header). */
        desc &= ~0x80;
    } else {
        idx = GSvmPop(ctx);
        if (desc & 0x20) {
            if (desc & 0x40) {
                value.u = (u32)&ctx->stack[ctx->frame + idx.s];
            } else {
                value.u = (u32)&ctx->globals[idx.s];
            }
        } else {
            if (desc & 0x40) {
                addr = &ctx->stack[ctx->frame + idx.s];
            } else {
                addr = &ctx->globals[idx.s];
            }
            if (desc & 0x100) {
                value.u = (u32)addr;
            } else {
                value = *addr;
            }
        }
    }
    return value;
}

#ifdef GS_VM_WHOLE_UNIT
/* 0x800F10E8 | 0x2E8 */
/* Script VM "call native" opcode: builds a call frame from the operand
 * stack, looks the callee up in the native-function table (0xC-byte records:
 * function pointer + 8 argument-type bytes, 2 = float, 0 = end), splits the
 * arguments into the integer/float register images consumed by
 * fn_800F106C, stores the result in the frame's return slot and unwinds. */
s32 cms_op37_callStandard(GSVMCtx* ctx)
{
    u8* record;
    u32 result;
    s32 valueCount;
    s32 intArgCount;
    s32 floatArgCount;
    s32 i;

    result = 0;
    GSvmEnterNative(ctx);

    record = (u8*)lbl_80478B00->natives + GSvmFrameSlot(ctx, 0)->u * 0xC;
    lbl_8047AC38 = *(u32 (**)(void))record;
    if (lbl_8047AC38 != NULL) {
        floatArgCount = 0;
        intArgCount = 0;
        memset(lbl_80401BD8, 0, 0x20);
        memset(lbl_80401BB8, 0, 0x20);
        valueCount = ctx->stackCount - ctx->frame - 3;
        if (valueCount > 8) {
            valueCount = 8;
        }
        for (i = 0; i < valueCount; i++) {
            if (record[i + 4] == 0) {
                break;
            }
            if (record[i + 4] == 2) {
                lbl_80401BB8[floatArgCount++] = GSvmFrameSlot(ctx, i + 1)->f;
            } else {
                lbl_80401BD8[intArgCount++] = GSvmFrameSlot(ctx, i + 1)->u;
            }
        }
        lbl_8047AC3C = lbl_80401BB8;
        lbl_8047AC40 = lbl_80401BD8;
        result = cms_CallStdTrampoline();
    }

    ctx->stack[ctx->frame - 1].u = result;
    GSvmReturn(ctx);
    return 1;
}

/* 0x800F13D0 | 0x2F0 */
/*
 * Native "wait" call: yields the script thread (_threadSwitch) until the
 * per-resume deltas from fn_800D3088 add up to argument 0.  The VM state byte
 * (+0x04) is 2 while waiting; an abort request (4 -> 3) leaves early with 0.
 */
s32 cms_op36_yield(GSVMCtx* ctx)
{
    s32 wait;
    s32 elapsed;
    GSVMValue* args;

    GSvmEnterNative(ctx);
    args = &ctx->stack[ctx->frame];
    wait = args[0].s;
    ctx->status = 2;
    for (elapsed = 0; elapsed < wait; elapsed += GSgetFrameStep()) {
        if (ctx->status == 4) {
            ctx->status = 3;
        }
        if (ctx->status == 3) {
            GSvmReturn(ctx);
            return 0;
        }
        _threadSwitch();
    }
    ctx->status = 1;
    GSvmReturn(ctx);
    return 1;
}

/* 0x800F16C0 | 0x34C */
/*
 * Native "print" call: expands the script's printf-style format string
 * (argument 0) with the following arguments into a 0x100-byte buffer and
 * writes it to the log.  Supports %d/%x/%c/%f/%s conversions (with any
 * flags copied through to sprintf) and the two-character escape "\n".
 * A %-conversion cut off by the end of the string adds whatever len held
 * before, as retail does.
 */
s32 cms_op35_printf(GSVMCtx* ctx)
{
    s32 len;
    s32 argIdx;
    s8* fmt;
    u8* out;
    s32 n;
    GSVMValue* args;
    GSVMValue value;

    GSvmEnterNative(ctx);
    out = lbl_80401AB8;
    argIdx = 1;
    args = &ctx->stack[ctx->frame];
    value = args[0];
    fmt = (s8*)value.p;
    while (*fmt != 0) {
        if (*fmt == '%') {
            value = ctx->stack[ctx->frame + argIdx++];
            n = 0;
            for (;;) {
                lbl_80401A78[n++] = *fmt;
                if (*fmt == 'd' || *fmt == 'x' || *fmt == 'c') {
                    lbl_80401A78[n] = 0;
                    len = sprintf(out, (const char*)lbl_80401A78, value.s);
                    break;
                } else if (*fmt == 'f') {
                    lbl_80401A78[n] = 0;
                    len = sprintf(out, (const char*)lbl_80401A78, value.f);
                    break;
                } else if (*fmt == 's') {
                    lbl_80401A78[n] = 0;
                    len = sprintf(out, (const char*)lbl_80401A78, value.p);
                    break;
                } else if (*fmt == 0) {
                    break;
                }
                fmt++;
            }
            out += len;
        } else if (*fmt == '\\') {
            switch (fmt[1]) {
            case 'n':
                len = sprintf(out, (const char*)lbl_8047CCB8);
                fmt++;
                out += len;
                break;
            default:
                *out++ = *fmt;
                break;
            }
        } else {
            *out++ = *fmt;
        }
        fmt++;
        if (out >= lbl_80401AB8 + 0xFF) {
            break;
        }
    }
    *out = 0;
    GSlogWrite((const char*)lbl_80401AB8);
    GSvmReturn(ctx);
    return 1;
}

#endif /* GS_VM_WHOLE_UNIT */

/* 0x800F1A0C | 0x42C */
/* Script VM logical OR: the first descriptor names the right-hand operand
 * (top of stack), the second the left; the result (0/1) is pushed. */
s32 cms_op34_orl(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s || rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s || rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f || rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f || rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F1E38 | 0x42C */
/* Script VM logical AND; same operand handling as fn_800F1A0C. */
s32 cms_op33_andl(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s && rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s && rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f && rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f && rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}



/* 0x800F2264 | 0x290 */
/* Script VM struct assignment (_codeStructAssign): copies `count` words
 * from the source variable block (or the top of the stack) to the
 * destination variable block and pushes the copied words. */
s32 cms_op32_copyStruct(GSVMCtx* ctx)
{
    u8 srcDesc;
    u8 dstDesc;
    u16 count;
    GSVMValue srcIdx;
    GSVMValue dstIdx;
    GSVMValue* src;
    GSVMValue* dst;
    s32 i;

    srcDesc = *ctx->ip++;
    dstDesc = *ctx->ip++;
    count = *(u16*)ctx->ip;
    ctx->ip += 2;
    if (dstDesc & 0x80) {
        GSlogWritef("_codeStructAssign:  エラー：定数に数値のセットはできません\n");
        return 1;
    }
    if (srcDesc & 0x80) {
        ctx->stackCount -= count;
        src = &ctx->stack[ctx->stackCount];
    } else {
        srcIdx = GSvmPop(ctx);
        if (srcDesc & 0x40) {
            src = &ctx->stack[ctx->frame + srcIdx.s];
        } else {
            src = &ctx->globals[srcIdx.s];
        }
    }
    dstIdx = GSvmPop(ctx);
    if (dstDesc & 0x40) {
        dst = &ctx->stack[ctx->frame + dstIdx.s];
    } else {
        dst = &ctx->globals[dstIdx.s];
    }
    for (i = 0; i < count; i++) {
        dst[i] = src[i];
    }
    for (i = 0; i < count; i++) {
        GSvmPushValue(ctx, dst[i]);
    }
    return 1;
}

/* setValue */
static inline void setValue(GSVMCtx* ctx, u16 desc, GSVMValue value)
{
    GSVMValue idx;

    if (desc == 0) {
        GSlogWritef("変数の型が未定義:[%02x]\n", desc);
    } else if (desc & 0x80) {
        GSlogWritef("setValue 例外エラー：定数に数値のセットはできません\n");
    } else {
        idx = GSvmPop(ctx);
        if (desc & 0x40) {
            ctx->stack[ctx->frame + idx.s] = value;
        } else {
            ctx->globals[idx.s] = value;
        }
    }
}

/* 0x800F24F4 | 0x2E0 */
/* Script VM assignment (setValue): converts the source operand to the
 * destination's type, stores it and pushes it. */
s32 cms_op31_setVar(GSVMCtx* ctx)
{
    u8 dstDesc;
    u8 srcDesc;
    GSVMValue value;

    srcDesc = *ctx->ip++;
    dstDesc = *ctx->ip++;
    value = GSvmGetOperand(ctx, srcDesc);
    if ((dstDesc & 0x3F) != (srcDesc & 0x3F)) {
        if ((srcDesc & 0x3F) == 2) {
            value.f = value.s;
        } else {
            value.s = value.f;
        }
    }
    setValue(ctx, dstDesc, value);
    GSvmPushValue(ctx, value);
    return 1;
}

/* 0x800F27D4: script VM "lhs != rhs" (operands as in fn_800F1A0C). */
s32 cms_op30_notEqual(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s != rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s != rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f != rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f != rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F2BE8: script VM "lhs == rhs" (operands as in fn_800F1A0C). */
s32 cms_op29_equal(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s == rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s == rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f == rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f == rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F2FF8: script VM "lhs >= rhs" (operands as in fn_800F1A0C). */
s32 cms_op28_greaterThanEqual(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s >= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s >= rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f >= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f >= rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F3418: script VM "lhs > rhs" (operands as in fn_800F1A0C). */
s32 cms_op27_greaterThan(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s > rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s > rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f > rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f > rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F3830: script VM "lhs <= rhs" (operands as in fn_800F1A0C). */
s32 cms_op26_lessThanEqual(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s <= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s <= rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f <= rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f <= rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F3C50: script VM "lhs < rhs" (operands as in fn_800F1A0C). */
s32 cms_op25_lessThan(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.s < rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.s < rhs.f) ? 1 : 0;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (lhs.f < rhs.s) ? 1 : 0;
        } else {
            result.s = (lhs.f < rhs.f) ? 1 : 0;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4068: script VM "lhs - rhs" (operands as in fn_800F1A0C). */
s32 cms_op24_subtract(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s - rhs.s;
        } else {
            result.f = lhs.s - rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.f = lhs.f - rhs.s;
        } else {
            result.f = lhs.f - rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4440: script VM "lhs + rhs" (operands as in fn_800F1A0C). */
s32 cms_op23_add(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s + rhs.s;
        } else {
            result.f = lhs.s + rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.f = lhs.f + rhs.s;
        } else {
            result.f = lhs.f + rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4818: script VM "lhs % (fmod of the integer lhs when a float is involved) rhs" (operands as in fn_800F1A0C). */
s32 cms_op22_mod(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if (rhs.s != 0) {
        if ((lhsDesc & 0x3F) == 2U) {
            if ((rhsDesc & 0x3F) == 2U) {
                result.s = lhs.s % rhs.s;
            } else {
                result.f = fmod(lhs.s, rhs.f);
            }
        } else {
            if ((rhsDesc & 0x3F) == 2U) {
                result.f = fmod(lhs.s, rhs.f);
            } else {
                result.f = fmod(lhs.s, rhs.f);
            }
        }
    } else {
        GSlogWritef("０で除算しようとしました\n");
        result.u = 0;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F4C38: script VM "lhs / rhs" (operands as in fn_800F1A0C). */
s32 cms_op21_divide(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if (rhs.s != 0) {
        if ((lhsDesc & 0x3F) == 2U) {
            if ((rhsDesc & 0x3F) == 2U) {
                result.s = lhs.s / rhs.s;
            } else {
                result.f = lhs.s / rhs.f;
            }
        } else {
            if ((rhsDesc & 0x3F) == 2U) {
                result.f = lhs.f / rhs.s;
            } else {
                result.f = lhs.f / rhs.f;
            }
        }
    } else {
        GSlogWritef("０で除算しようとしました\n");
        result.u = 0;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F502C: script VM "lhs * rhs" (operands as in fn_800F1A0C). */
s32 cms_op20_multiply(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s * rhs.s;
        } else {
            result.f = lhs.s * rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.f = lhs.f * rhs.s;
        } else {
            result.f = lhs.f * rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}


/* 0x800F5404 | 0x1D8: push an operand. */
s32 cms_op19_pushVar2(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue value;

    desc = *ctx->ip++;
    value = GSvmGetOperand(ctx, desc);
    GSvmPushValue(ctx, value);
    return 1;
}

/* 0x800F55DC | 0x214: "-operand". */
s32 cms_op18_negate(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue value;
    GSVMValue result;

    desc = *ctx->ip++;
    value = GSvmGetOperand(ctx, desc);
    if ((desc & 0x3F) == 2) {
        result.s = -value.s;
    } else {
        result.f = -value.f;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F57F0 | 0x24C: "!operand". */
s32 cms_op17_not(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue value;
    GSVMValue result;

    desc = *ctx->ip++;
    value = GSvmGetOperand(ctx, desc);
    if ((desc & 0x3F) == 2) {
        result.s = !value.s;
    } else {
        result.f = !value.f;
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F5A3C | 0x264: push `count` consecutive variables, each preceded by
 * its index, starting at the popped index. */
s32 cms_op16_pushMultiVar(GSVMCtx* ctx)
{
    GSVMValue start;
    GSVMValue value;
    u32 index;
    s32 i;
    u16 count;
    u8 desc;

    desc = *ctx->ip++;
    count = *(u16*)ctx->ip;
    ctx->ip += 2;
    start = GSvmPop(ctx);
    for (i = 0; i < count; i++) {
        /* The variable index is computed in its own statement: retail adds
         * start + i (add r4,r30,r28) before the push's overflow guard, with
         * start kept in r30. Passed straight to GSvmPush, the sum is
         * computed inside the store branch after the guard and start is
         * reloaded from its slot each iteration. */
        index = start.u + i;
        GSvmPush(ctx, index);
        value = GSvmGetOperand(ctx, desc);
        GSvmPushValue(ctx, value);
    }
    return 1;
}

/* 0x800F5CA0 | 0x24C: keep the popped index and push the address of the
 * variable it names. */
s32 cms_op15_pushVarRef(GSVMCtx* ctx)
{
    u8 desc;
    GSVMValue index;
    GSVMValue value;

    desc = *ctx->ip++;
    index = GSvmPop(ctx);
    GSvmPush(ctx, index.u);
    value = GSvmGetOperand(ctx, desc | 0x100);
    GSvmPushValue(ctx, value);
    return 1;
}

/* 0x800F5EEC: script VM "lhs | (bitwise) rhs" (operands as in fn_800F1A0C). */
s32 cms_op14_or(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s | rhs.s;
        } else {
            result.s = lhs.s | (s32)rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (s32)lhs.f | rhs.s;
        } else {
            result.s = (s32)lhs.f | (s32)rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F62BC: script VM "lhs & (bitwise) rhs" (operands as in fn_800F1A0C). */
s32 cms_op13_and(GSVMCtx* ctx) {
    u8 lhsDesc;
    u8 rhsDesc;
    GSVMValue lhs;
    GSVMValue rhs;
    GSVMValue result;

    rhsDesc = *ctx->ip++;
    lhsDesc = *ctx->ip++;
    rhs = GSvmGetOperand(ctx, rhsDesc);
    lhs = GSvmGetOperand(ctx, lhsDesc);
    if ((lhsDesc & 0x3F) == 2U) {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = lhs.s & rhs.s;
        } else {
            result.s = lhs.s & (s32)rhs.f;
        }
    } else {
        if ((rhsDesc & 0x3F) == 2U) {
            result.s = (s32)lhs.f & rhs.s;
        } else {
            result.s = (s32)lhs.f & (s32)rhs.f;
        }
    }
    GSvmPushValue(ctx, result);
    return 1;
}

/* 0x800F668C | 0x80: push the immediate word that follows the type byte. */
s32 cms_op08_pushImmediate(GSVMCtx* ctx)
{
    ctx->ip++;
    GSvmPush(ctx, *(u32*)ctx->ip);
    ctx->ip += 4;
    return 1;
}

/* 0x800F670C | 0xA0: branch to the bank offset that follows when the
 * popped value is zero. */
s32 cms_op07_jumpFalse(GSVMCtx* ctx)
{
    GSVMValue cond;

    cond = GSvmPop(ctx);
    if (cond.s != 0) {
        ctx->ip += 4;
    } else {
        ctx->ip = (u8*)ctx->script + *(u32*)ctx->ip;
    }
    return 1;
}

/* 0x800F67AC | 0x1C: branch to the bank offset that follows. */
s32 cms_op06_jump(GSVMCtx* ctx)
{
    ctx->ip = (u8*)ctx->script + *(u32*)ctx->ip;
    return 1;
}

/* 0x800F67C8 | 0x184: return from a script function. */
s32 cms_op05_return(GSVMCtx* ctx)
{
    GSVMValue argc;
    GSVMValue frame;
    GSVMValue ret;
    s32 i;

    argc = GSvmPop(ctx);
    frame = GSvmPop(ctx);
    ctx->frame = frame.s;
    ret = GSvmPop(ctx);
    ctx->ip = (u8*)ret.p;
    for (i = 0; i < argc.s; i++) {
        GSvmPop(ctx);
    }
    if (ctx->ip == NULL) {
        ctx->status = 3;
        return 0;
    }
    return 1;
}

/* 0x800F694C | 0x168: call a script function. */
s32 cms_op04_call(GSVMCtx* ctx)
{
    u32 func;
    u16 argc;
    u16 locals;
    GSVMScript* script;

    func = *(u32*)ctx->ip;
    ctx->ip += 4;
    argc = *(u16*)ctx->ip;
    ctx->ip += 2;
    locals = *(u16*)ctx->ip;
    ctx->ip += 2;
    GSvmPush(ctx, (u32)ctx->ip);
    GSvmPush(ctx, ctx->frame);
    GSvmPush(ctx, locals);
    ctx->frame = ctx->stackCount - (argc + locals + 3);
    script = ctx->script;
    if (func >= script->funcCount) {
        GSlogWritef("設定する関数のインデックス[%d]が不正です\n", func);
    } else {
        ctx->ip = (u8*)script + script->funcOffsets[func];
    }
    return 1;
}

/* 0x800F6AB4 | 0xA0: drop `count` stack values. */
s32 cms_op03_pop(GSVMCtx* ctx)
{
    u16 count;
    s32 i;

    ctx->ip++;
    count = *(u16*)ctx->ip;
    ctx->ip += 2;
    for (i = 0; i < count; i++) {
        GSvmPop(ctx);
    }
    return 1;
}

/* 0x800F6B54 | 0x58: push -1. */
s32 cms_op02_pushMinus1(GSVMCtx* ctx)
{
    GSvmPush(ctx, -1);
    return 1;
}

/* 0x800F6BAC | 0x10: stop the script. */
s32 cms_op01_exit(GSVMCtx* ctx)
{
    ctx->status = 3;
    return 0;
}

/* 0x800F6BBC | 0x8: no operation. */
s32 cms_op00_nop(GSVMCtx* ctx)
{
    return 1;
}

#ifdef GS_VM_WHOLE_UNIT
/* Opcode handlers, indexed by the opcode byte. */
s32 (*lbl_803155D0[0x26])(GSVMCtx* ctx) = {
    cms_op00_nop, cms_op01_exit, cms_op02_pushMinus1, cms_op03_pop,
    cms_op04_call, cms_op05_return, cms_op06_jump, cms_op07_jumpFalse,
    cms_op08_pushImmediate, NULL,        NULL,        NULL,
    NULL,        cms_op13_and, cms_op14_or, cms_op15_pushVarRef,
    cms_op16_pushMultiVar, cms_op17_not, cms_op18_negate, cms_op19_pushVar2,
    cms_op20_multiply, cms_op21_divide, cms_op22_mod, cms_op23_add,
    cms_op24_subtract, cms_op25_lessThan, cms_op26_lessThanEqual, cms_op27_greaterThan,
    cms_op28_greaterThanEqual, cms_op29_equal, cms_op30_notEqual, cms_op31_setVar,
    cms_op32_copyStruct, cms_op33_andl, cms_op34_orl, cms_op35_printf,
    cms_op36_yield, cms_op37_callStandard,
};
#else
/* Opcode handlers, indexed by the opcode byte (.data, outside the
 * linked range: game/data/data_803155D0.c). */
extern s32 (*lbl_803155D0[0x26])(GSVMCtx* ctx);
#endif

/* 0x800F6BC4 | 0x154: run a script until it stops; returns (and passes
 * to the completion callback) the value left on the stack. */
u32 cms_ScriptExecute(GSVMCtx* ctx)
{
    u8 op;
    s32 (*handler)(GSVMCtx*);
    s32 i;
    GSVMValue result;

    for (;;) {
        if (ctx->status == 0) {
            GSlogWrite("script %08X terminated by status == STANDBY\n", ctx->scriptId);
            break;
        }
        if (ctx->status == 3) {
            ctx->status = 0;
            break;
        }
        op = *ctx->ip++;
        if ((s32)op >= 0x26) {
            GSlogWritef("サポートされていないコードです:%02x\n", op);
        } else {
            handler = lbl_803155D0[op];
            if (handler != NULL) {
                handler(ctx);
            }
        }
        if (ctx->stackCount > 0) {
            for (i = ctx->stackCount - 1; i >= 0; i--) {
            }
        }
    }
    if (ctx->status == 4) {
        ctx->status = 0;
    }
    result = GSvmPop(ctx);
    if (ctx->callback != NULL) {
        ctx->callback(ctx, result.u);
    }
    return result.u;
}

/* 0x800F6D18 | 0x350: set up a context to run function `scriptId & 0xFFFF`
 * of script `scriptId >> 16` with `argc` arguments. */
GSVMCtx* cms_ScriptCreateThread(u32 scriptId, u32 argc, va_list args)
{
    GSVMPool* pool;
    GSVMCtx* ctx;
    GSVMScript* script;
    u32 i;
    u16 count;
    u32 key;
    u32 func;

    for (i = 0; i < lbl_80478B00->count; i++) {
        ctx = &lbl_80478B00->contexts[i];
        if (ctx->status == 4) {
            ctx->status = 0;
        }
        if (ctx->status == 3) {
            ctx->status = 0;
        }
    }
    pool = lbl_80478B00;
    count = pool->count;
    ctx = pool->contexts;
    for (i = 0; i < count; i++) {
        ctx = &pool->contexts[i];
        if (ctx->status == 0) {
            break;
        }
    }
    if (i == count) {
        GSlogWritef("スクリプトの起動に失敗しました:[%08x] タスクが足りません\n", scriptId);
        return NULL;
    }
    memset(ctx, 0, sizeof(GSVMCtx));
    for (script = lbl_80478B00->scripts; script != NULL; script = script->next) {
        if (script->id == (scriptId >> 16)) {
            ctx->script = script;
            ctx->globals = (GSVMValue*)((u8*)script + script->globalsOffset);
            break;
        }
    }
    if (script == NULL) {
        GSlogWritef("スクリプトの起動に失敗しました:[%08x] ＩＤが見つかりません\n", scriptId);
        return NULL;
    }
    /* The key is kept in a word and wrapped to 16 bits as it advances:
     * retail computes the next key into r0 (addi, clrlwi) and copies it
     * back (mr r29,r0). A u16 key incremented in place (key++, key += 1,
     * key = key + 1) keeps the unwrapped sum in r29 instead.
     * The wrap test reads the manager's lastKey again rather than a copy:
     * MWCC hoists that read into a temporary created after the manager
     * pointer's, which is what gives retail's lhz r6 / lwz r7 pairing; a
     * declared `lastKey` (or a `pool` local here) is numbered before the
     * pointer and swaps the two registers. */
    key = lbl_80478B00->lastKey;
    for (;;) {
        key = (u16)(key + 1);
        if (key == lbl_80478B00->lastKey) {
            GSlogWritef("スクリプトの起動に失敗しました:[%08x] ハンドルの確保に失敗\n", scriptId);
            return NULL;
        }
        if (key == 0) {
            continue;
        }
        if (GSvmFindByKey(key) == NULL) {
            break;
        }
    }
    lbl_80478B00->lastKey = key;
    func = scriptId & 0xFFFF;
    script = ctx->script;
    if (func >= script->funcCount) {
        GSlogWritef("設定する関数のインデックス[%d]が不正です\n", func);
    } else {
        ctx->ip = (u8*)script + script->funcOffsets[func];
    }
    ctx->status = 1;
    ctx->scriptId = scriptId;
    ctx->key = key;
    GSvmPush(ctx, 0);
    for (i = 0; i < argc; i++) {
        GSvmPush(ctx, va_arg(args, u32));
    }
    GSvmPush(ctx, 0);
    GSvmPush(ctx, 0);
    GSvmPush(ctx, argc);
    return ctx;
}

/* RULE-EXCEPTION(title-path): pool literals given global names - see
 * docs/RULE_EXCEPTIONS.md. The last three strings of the pool are
 * printed by fn_800F7318, fn_800F760C and fn_800F7758, which lie
 * outside the linked range and link to them by name. */
const char lbl_80271294[] = "[%s] スレッドのさくせいにしっぱい\n";
const char lbl_802712B8[] = "スクリプトのアンマップに失敗しました:[%d]\n";
const char lbl_802712E4[] = "GSvmの初期化に失敗しました\n";

/* 0x800F7068 | 0xA0: is the script with `key` still running?  With `wait`,
 * yields until it is not. */
s32 fn_800F7068(u16 key, u8 wait)
{
    for (;;) {
        if (GSvmFindByKey(key) == NULL) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
        } else {
            return 1;
        }
    }
}

/* 0x800F7108 | 0x64: the thread running the script with `key`. */
GSThread* fn_800F7108(u16 key)
{
    GSVMCtx* ctx;

    ctx = GSvmFindByKey(key);
    if (ctx == NULL) {
        return NULL;
    }
    return ctx->thread;
}

/* 0x800F716C | 0x108: stop every script running on a thread of `group`
 * (the thread's +0x0C field, read by fn_800F0374; floor teardown passes
 * the floor id).
 * The thread's group is read into its own variable before the compare:
 * retail compares "cmplw r3,r30" (the group read, then the argument).
 * MWCC (GC/1.3 through 2.7, -O4,p/-O4,s/-O3/-O2) turns an integer equality
 * with a call on either side, fn_800F0374(t) == group or
 * group == fn_800F0374(t), into "cmplw r30,r3"; the call-first order only
 * comes out when the call's value is a named variable (or when both sides
 * are pointers, as in fight_target.c's (void*)fightTrainerGetStatus(...)
 * == (void*)target, which is not the case for a floor id). */
s32 fn_800F716C(u32 group)
{
    GSVMCtx* ctx;
    u32 threadGroup;
    s32 i;

    for (i = 0; i < lbl_80478B00->count; i++) {
        ctx = &lbl_80478B00->contexts[i];
        if (ctx->status != 0 && ctx->thread != NULL) {
            threadGroup = fn_800F0374(ctx->thread);
            if (threadGroup == group) {
                GSvmStopByKey(ctx->key);
            }
        }
    }
    return 0;
}

/* 0x800F7274 | 0xA4: stop the script with `key`. */
s32 floorTerminateScriptThreads(u16 key)
{
    return GSvmStopByKey(key);
}

/* 0x800F7318 | 0x11C */
/*
 * Allocates a script task for `script` (fn_800F6D18 pushes argc variadic
 * arguments onto its stack), then runs it on a new GS thread whose entry is
 * the script executor fn_800F6BC4. `done` is called with the task and its
 * result when the script ends. Returns the task key, or 0.
 * The failure message prints the function's own name from the retail .data
 * table. The linked .text range references that existing data symbol.
 */
/* RULE-EXCEPTION(title-path): named stand-in for this TU's function-name
 * string in retail .data; see docs/RULE_EXCEPTIONS.md. */
extern const char lbl_80315668[];
u16 fn_800F7318(u32 affinity, u32 script, u32 stackSize, u32 autoStart,
                void* done, u32 argc, ...)
{
    va_list args;
    GSVMCtx* ctx;
    u32 priority;
    GSThread* thread;

    va_start(args, argc);
    ctx = cms_ScriptCreateThread(script, argc, args);
    priority = floorGetCurrentContext();
    if (ctx == NULL) {
        return 0;
    }
    thread = GSthreadCreate(affinity, priority, stackSize, 1, autoStart, cms_ScriptExecute);
    if (thread != NULL) {
        ctx->thread = thread;
        ctx->callback = (void (*)(GSVMCtx*, u32))done;
        GSthreadSetArgs(thread, 1, ctx);
    } else {
        GSlogWrite(lbl_80271294, lbl_80315668);
    }
    return ctx->key;
}

/* 0x800F7434 | 0x1C8: run a script to completion on the calling thread. */
u32 cmsRunSync(u32 script, u32 argc, ...)
{
    va_list args;
    GSVMCtx* ctx;

    va_start(args, argc);
    ctx = cms_ScriptCreateThread(script, argc, args);
    if (ctx == NULL) {
        return 0;
    }
    return cms_ScriptExecute(ctx);
}

/* 0x800F75FC | 0x10: install the native-function table. */
#ifdef GS_VM_WHOLE_UNIT
s32 fn_800F75FC(void* natives)
{
    lbl_80478B00->natives = natives;
    return 0;
}

/* 0x800F760C | 0xD8: unlink `script` from the loaded-script list, then
 * mark every context running it for termination (state 3). */
s32 fn_800F760C(GSVMScript* script)
{
    GSVMPool* pool = lbl_80478B00;
    GSVMScript* prev;
    GSVMScript* cur;
    u32 offset;
    GSVMCtx* ctx;
    s32 i;
    u8 state;

    cur = pool->scripts;
    if (cur == script) {
        pool->scripts = cur->next;
    } else {
        prev = cur;
        while ((cur = prev->next) != NULL) {
            if (cur == script) {
                prev->next = cur->next;
                goto scan;
            }
            prev = cur;
        }
        if (cur == NULL) {
            GSlogWritef(lbl_802712B8, script);
            return -1;
        }
    }

scan:
    i = 0;
    offset = 0;
    state = 3;
    while (i < (s32)lbl_80478B00->count) {
        ctx = (GSVMCtx*)((u8*)lbl_80478B00->contexts + offset);
        if (ctx->status != 0) {
            if (script->id == (u16)(ctx->scriptId >> 16)) {
                ctx->status = state;
            }
        }
        offset += sizeof(GSVMCtx);
        i++;
    }
    return 0;
}

/* 0x800F76E4 | 0x74: link a loaded script into the list and relocate it. */
void fn_800F76E4(GSVMScript* script)
{
    GSVMScript* head;
    u32 off;
    u32* tbl;
    s32 i;

    script->next = NULL;
    head = lbl_80478B00->scripts;
    if (head == NULL) {
        lbl_80478B00->scripts = script;
    } else {
        lbl_80478B00->scripts = script;
        script->next = head;
    }
    if (script->relocated != 0) {
        return;
    }
    tbl = (u32*)((u8*)script + script->relocOffset);
    i = 0;
    while (i < (s32)script->relocCount) {
        off = *tbl;
        tbl += 1;
        i += 1;
        *(u32*)((u8*)script + off) += (u32)script;
    }
    script->relocated = 1;
}

/* 0x800F7758 | 0x14C: allocate `count` script VM contexts and reset the
 * manager. */
s32 fn_800F7758(u16 count)
{
    GSVMCtx* ctx;
    s32 i;

    memset(&lbl_80401BF8, 0, sizeof(GSVMPool));
    lbl_80478B00 = &lbl_80401BF8;
    lbl_80478B00->handle = _toolentryAlloc__FUl(count * sizeof(GSVMCtx));
    if (lbl_80478B00->handle == 0) {
        GSlogWritef(lbl_802712E4);
        return -1;
    }

    lbl_80478B00->contexts = (GSVMCtx*)fn_800E27B0(lbl_80478B00->handle);
    ctx = lbl_80478B00->contexts;
    for (i = 0; i < count; i++) {
        ctx->script = NULL;
        ctx->status = 0;
        ctx++;
    }
    lbl_80478B00->count = count;
    return 0;
}

#endif /* GS_VM_WHOLE_UNIT */
