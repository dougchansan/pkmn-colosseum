# CW 1.3 (mwcceppc, GC/1.3) Register-Allocator Model — empirically derived

Compiler: `tools/mwcc_compiler/GC/1.3/mwcceppc.exe`
Flags: `-O4,p -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn off -use_lmw_stmw on -sdata 8 -sdata2 8`
Harness: `tools/decomp_work/ra/racexp.py <file.c> t`  (compiles `-c`, disassembles via WSL `powerpc-linux-gnu-objdump`)
Batch: `racbatch.py` (uses `-S`, same result); experiment sources are `e_*.c`.

NOTE on environment: mwcc + WSL objdump run fine but their stdout is heavily
DELAYED in the agent sandbox. Run with `dangerouslyDisableSandbox: true`, write
results to a file in-tree, and Read the file (do not rely on task stdout).

## THE CORE RULE (controlled, one-factor-at-a-time)

For values that must survive a call, CW assigns the non-volatile (saved) GPRs
**r14..r31** in a single contiguous band whose bottom is chosen so the band size
== number of simultaneously-call-live values (this drives the `stmw rN`/`lmw`
base). Within that band the assignment order is:

  **STRICTLY the order of FIRST DEFINITION (first assignment / first time the
  value is produced), ascending from the bottom of the band upward.**

Incoming parameters count as "defined at entry, left-to-right" (p0 before p1...).

It is NOT declaration order, NOT use order, NOT last-use order. Proven:

| exp | source factor varied | result |
|-----|----------------------|--------|
| e_decl5 | define v0..v4 in order, use v0..v4 | a=r27 v0=r28 v1=r29 v2=r30 v3=r31 v4=r3 |
| e_declrev5 | DECLARE v4..v0 reversed, DEFINE v0..v4 | IDENTICAL to e_decl5 -> decl order irrelevant |
| e_rev5 | define v0..v4, USE reversed v4..v0 | IDENTICAL map -> use order irrelevant |
| e_usefirst | DEFINE scrambled v2,v0,v4,v1,v3 | r28=v2 r29=v0 r30=v4 r31=v1 r3=v3 -> follows FIRST-DEF order exactly |
| e_params | 3 params across a call | p0=r29 p1=r30 p2=r31 (ascending by param position) |

## SECONDARY RULES

1. **Last-defined-value-with-no-following-call stays VOLATILE (r3).**
   In every "all live to end" case the final produced value is left in r3 and
   never assigned a saved reg, because it never has to survive a call. This is
   why an N-local function uses only N-1 (or N, counting the param) saved regs.

2. **No call => no saved regs at all.** e_nocall: all of v0..v4 live in
   volatile r0,r3,r4,r5,r6. Saved registers appear ONLY to carry a value across
   a `bl`. Register pressure alone (without a call) does not force r14..r31.

3. **stmw threshold = 5.** With `-use_lmw_stmw on`, CW emits `stmw rBottom`/`lmw`
   when the saved band has **>= 5** registers; for **<= 4** it emits individual
   `stw r31.. / stw r30..` (and matching `lwz`). Confirmed:
   - 2 saved (e_liverange): individual stw r31,r30
   - 4 saved (e_longfirst): individual stw r31,r30,r29,r28
   - 5 saved (e_decl5):     stmw r27
   - 6/7/8 saved (sparse/n6/n7): stmw r26/r26/r25
   This is the same "stmw wall" noted in memory feedback_stmw_threshold_wall:
   if the TARGET saved 2-4 regs but your C makes the band >=5 (or vice-versa),
   the prologue diverges. The band size = number of values live across a call,
   so it is controllable by changing how many values must survive (see rule 1).

4. **Band bottom = r(32 - count).**
   - 5 live saved (a+v0..v3): stmw r27  (32-5=27)
   - 8 live saved (a+v0..v6): stmw r24  (32-8=24)
   - 13 live (a+v0..v10):     stmw r20  (32-12=20)
   So the bottom of the band is deterministic from the live-across-call count.

## LIVE-RANGE / LINEAR-SCAN REFINEMENTS (round-2 + real-file corroboration)

The allocator is **linear-scan-like, not Chaitin-Briggs graph coloring**. Three
behaviours prove a live-range sweep rather than pure decl/def coloring:

A. **Short-lived values never get a saved reg.** A value produced and consumed
   before the next `bl` stays in volatile r3. (e_liverange: s0/s1/s2 vanish into
   r3; e_shortfirst: vfirst defined FIRST but used immediately -> stays r3, the
   four later survivors take r29..r31+r3.) => Only call-surviving values compete
   for r14..r31. First-definition order is the tiebreak AMONG survivors only.

B. **Register recycling when a range ends.** stmw_sparse.c (real harness file):
   `a` is live only until the 5th call, so its reg (r30) is RECLAIMED by v4 after
   `a` dies. v0..v3 (longer ranges) take the low stable regs r26..r29; the
   accumulator `w` takes r31. This is classic linear-scan slot reuse.

C. **Fewer survivors => survivor lands HIGH.** When only one long value survives
   many calls it gets r31 (e_liverange vlong->r31). When several survive they
   fill ascending and the first-defined is near the bottom (e_longfirst
   vlong->r29). So "first def" maps to the BOTTOM of the survivor band, and the
   band top is the value with the longest forward reach.

THRESHOLD for the clean ascending rule: n = 5,6,7,8 survivors are all clean
ascending-by-first-def (e_decl5/n6/n7/press8). The first-value-lifts-to-top
anomaly below appears only at very high pressure (n=12). Real wall functions
almost always sit in the clean regime, so:

  >>> PRACTICAL RULE: among the values that survive >=1 call, CW assigns
      r(32-k)..r31 in first-definition order ascending; short-lived values stay
      volatile; a register freed by a dead value is reused by the next survivor.

## HIGH-PRESSURE ANOMALY (only n>=~12, rarely hit)

e_press12 (12 locals + param a = 13 live): band is r20..r31.
  a   -> r20  (bottom)
  v0  -> r31  (TOP  <-- jumps to top instead of r21)
  v1  -> r21
  v2  -> r22  ... v10 -> r30
  v11 -> r3   (last, volatile)

So at high pressure the first long-lived local (v0, live across the most
remaining calls) is lifted to r31 while the param sits at the bottom r20 and the
rest fill r21 upward. This is the one place "first-def ascending" breaks and a
live-range/affinity heuristic shows through. The n=5/n=8 cases do NOT show it
(clean ascending), so it kicks in past a pressure threshold — round-2 e_n6/e_n7
locate the transition.

## WHY THIS MATTERS FOR CRACKING WALLS (inversion strategy)

The near-miss walls are register-COLORING mismatches that cascade. Because the
coloring is a deterministic function of FIRST-DEFINITION ORDER, the source-level
knob to INVERT the allocator is: **reorder the statements that first produce each
value so their first-definition sequence matches the target's saved-register
ascending order.** Read the target asm's `mr rK,r3` / saved-reg-consuming
sequence, list the saved regs in ascending order, and that is the exact order in
which your C must FIRST ASSIGN each corresponding value. Declaration order and
use order are free to stay readable; only first-definition order is load-bearing.

For the high-pressure case, additionally hoist/sink the one value that target
puts in r31 so that it is the first local defined (matching the v0->r31 lift).
