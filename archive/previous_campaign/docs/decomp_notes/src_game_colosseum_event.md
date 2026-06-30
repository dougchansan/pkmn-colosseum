# Decomp notes: src/game/colosseum_event.c

## Status snapshot
5/5 matched this session (2026-05-17); 22 functions remain in near-miss band (88-99%)

## Blocked near-misses

- **fn_80206C94** @ 92.94% — stmw-emission
  - Symptom: target uses stmw/lmw; TU emits stw/lwz
  - Next leads: TU split
  - Last attempt: 2026-05-17 by executor

- **fn_802099AC** @ 89.88% — stmw-emission
  - Symptom: target uses stmw/lmw; TU emits stw/lwz
  - Next leads: TU split
  - Last attempt: 2026-05-17 by executor

- **fn_80206AEC** @ 89.42% — stmw-emission
  - Symptom: target uses stmw/lmw; TU emits stw/lwz
  - Next leads: TU split
  - Last attempt: 2026-05-17 by executor

## Blocked near-misses (new, from this session)

- **fn_8020DF10, fn_8020DF50, fn_8020E020, fn_8020E068** @ 96.25% each — clrlslwi-cse
  - Symptom: target emits `clrlslwi r4, r4, 24, 3` (mask+shift on param reg); decomp emits `slwi r4, r0, 3` (reuses r0 from earlier clrlwi for comparison)
  - Root cause: CW CSE's `clrlwi r0, r4, 24` from the `slot >= 4` check and reuses r0 for the multiply. Target recomputes from r4.
  - Tried: no `(u32)` cast, explicit `(u8)` cast, optimization_level 2/3, restructured condition
  - Next leads: possibly `-opt nopeephole` or different if-structure to kill r0 liveness before multiply
  - Last attempt: 2026-05-17 by executor

- **fn_8020E204, fn_8020E488** @ 93.64% — clrlslwi-cse (u16 variant)
  - Symptom: target emits `clrlslwi r0, r3, 16, N`; decomp emits `slwi r0, r5, N`
  - Root cause: CW CSE's `clrlwi r5, r3, 16` from the `index > *count` comparison and reuses r5 for the shift.
  - Tried: inline deref, different cast forms
  - Next leads: change comparison to not produce r5 clrlwi result (e.g. use u32 param type)
  - Last attempt: 2026-05-17 by executor

- **fn_8020E614** @ 88.64% — booleanize-idiom
  - Symptom: target uses `neg r3, r3; subic r0, r3, 0x1; subfe r3, r0, r3` (3 instrs); decomp uses `neg r0, r3; or r0, r0, r3; srwi r3, r0, 31` (3 instrs)
  - Root cause: CW emits different booleanize idiom for `!= 0` check
  - Tried: `s32` vs `u32` return type, `!!`, local variable
  - Next leads: unclear — may require different compiler version or flag
  - Last attempt: 2026-05-17 by executor

- **fn_80209CB4** @ 93.09% — mr.-cw-signed + booleanize-idiom
  - Symptom: `mr.`/`cmpwi` pattern (signed, needs peephole) + `subic/subfe` vs `neg/or/srwi` booleanize
  - Tried: not yet attempted (Ghidra stub structure complicates fix)
  - Next leads: convert from Ghidra stub to proper function signature, then apply peephole + fix condition
  - Last attempt: 2026-05-17 by executor

## Recently landed

- **fn_80202C1C** — 85.9 -> **89.1%** (2026-06-10, commit 1e5f5eae). (1) `fn_801EF634`
  + result `sVar8` retyped short -> u16 (extsh/cmpwi -> clrlwi/cmplwi on `==1`);
  (2) outer `do { if (bound<=i) return; ... i++; } while(1)` -> `for(i=0;
  (i&0xffff)<(uVar2&0xffff);i++)` for the entry-`b`-to-bottom-condition shape.
  Residual: W1 r21-r31 rename + W4 null-check booleanize in the deep nest.
- **fn_80204854** — 90.1 -> **91.3%** (2026-06-10, commit a2750603). BODY BYTE-EXACT.
  (1) De-Morgan-inverted the `||`-chain-to-uVar1=0 precondition so uVar1=1 (return 1)
  lays out first (fixes swapped li r3,0x1/li r3,0x0 epilogue); (2) moved `iVar2` decl
  ahead of `sVar3` to flip the r30<->r31 saved-band home. Sole residual = W3 stmw
  threshold (2 saved regs). **LESSON: the De Morgan return-order inversion + a single
  decl reorder can take a fn to body-byte-exact, leaving only a pure W3 prologue.**

- **fn_802078F0** — `#pragma peephole on` + `if ((ctx = r3) != NULL)` → `mr. r31, r3` (2026-05-17)
- **fn_80209D90** — `#pragma peephole on` + `if ((ctx = r3) == NULL)` → `mr. r31, r3` (2026-05-17)
- **fn_8020A478** — `#pragma peephole on` + `if ((ctx = r3) != NULL)` → `mr. r31, r3` (2026-05-17)
- **fn_8020FC70** — `#pragma peephole on` + `mode != 0` condition + inline `fn_801F0204` call in arg (2026-05-17)
- **fn_80211040** — same pattern as fn_8020FC70 (2026-05-17)

## Matched (100%)
(From this session — move to matched list after confirming stable)
fn_802078F0, fn_80209D90, fn_8020A478, fn_8020FC70, fn_80211040

## Session log

### 2026-06-10 (session 2) — executor (Sonnet 4.6)

**fn_80211A78** at 99.15% (already committed with peephole on). Attempted:
- `switch ((u8)result) { case 1: }` shape (same 99.15%)
- `#pragma peephole off/on` around switch (same 99.15% — `cmpwi` is the switch codegen itself)
- `switch ((u32)(u8)result)` (same 99.15%)
- Both `peephole on` and `peephole off` produce `cmpwi r0, 0x1` for switch case compares
- Wall: CW 1.3 switch codegen always emits `cmpwi` for small constants; target uses `cmplwi`.
  Not a peephole issue. Hard skip.

Completed full 80-100% band scan (39 functions). All remaining near-misses blocked by W1/W3/W4/W2.
See "Full 80-100% band scan" section above. Session complete for this TU.

### 2026-05-17 — executor (Sonnet 4.6)

**Goal:** push HINT near-misses in 88-99% band to 100%.

**Technique discoveries:**

1. **mr./clrlwi. pattern** (`peephole-combine`): Functions with `if (ctx == NULL) return` where ctx is saved to a callee-save register needed `#pragma peephole on` + assignment-in-condition form `if ((ctx = r3) == NULL)` to get CW to emit `mr. r31, r3` instead of `mr r31, r3; cmplwi r31, 0`. Without the pragma, CW 1.3 at -O4 does NOT emit `mr.` even with the combined form.

2. **clrlwi. pattern** for u16: same peephole fix, but condition needs to be `mode != 0` (not `mode != 1`) to get `clrlwi. r0, r30, 16; beq` — the target branches when mode==0, not mode==1. Original condition was wrong semantically.

3. **inline-fn-arg reg-alloc**: When `tablePtr = fn_801F0204(...)` is stored in a local and then passed as 5th arg, CW goes through r0. Inlining as `fn_802085C4(p3, p1, 2, 0, fn_801F0204(fn_801F0234(0x12)))` avoids the r0 intermediate and matches `mr r7, r3` directly.

**Unresolved patterns:**

- `clrlslwi r4, r4, 24, 3` vs `slwi r4, r0, 3`: CW CSE's the u8/u16 mask from an earlier comparison into a scratch register and reuses it for the multiply. No source change found to prevent this. Possibly needs `-opt nopeephole` global flag.
- `neg+subic+subfe` vs `neg+or+srwi`: different booleanize idiom. Unknown trigger.

**Functions moved to 100%:** 5 (fn_802078F0, fn_80209D90, fn_8020A478, fn_8020FC70, fn_80211040)
**Functions attempted but not improved:** fn_8020E614, fn_80209CB4, fn_8020DF10, fn_8020DF50, fn_8020E020, fn_8020E068, fn_8020E204, fn_8020E488

## Untouched near-misses
(Pull from `python tools/near_miss_report.py` for full list)
fn_80210998 (now 100%), fn_80211810 (now 100%), fn_80210D04 (now 100%), fn_8020ECE0 (now 100%),
fn_803103E8 (NA in this TU), fn_8020B330 (99.70% — W1 r23↔r26 rename, hard skip)

## Full 80-100% band scan (2026-06-10 session 2)

All 39 functions in 80-100% band scanned. Classification:
- **fn_8020341C** 99.62% — W2 scheduler: target saves fn_8012640C result to r29/r28 on
  null path (`li r29, 0x0; b; cmplwi r29, 0x0`); CW uses r3 directly. Not C-controllable.
- **fn_8020EED4** 99.53% — W1 r28↔r29 + r30↔r5 rename throughout. Hard skip.
- **fn_80211A78** 99.15% — W2 switch cmpwi vs cmplwi (see above). Hard skip.
- **fn_8020E7AC** 96.44% — W1 r26-r31 band rotation. Hard skip (noted previously).
- **fn_80209CB4** 96.00% — W2 mr./cmpwi + W4 booleanize. Hard skip (noted previously).
- **fn_8020BFA0** 95.15% — W2 scheduler. Hard skip (noted previously).
- **fn_8020DAD0** 95.03% — W1 + DIV-IDIOM wall (srawi/addze). Hard skip (noted previously).
- **fn_8020B72C** 94.71% — W1 r25-r30 full band rotation. Hard skip.
- **fn_8020B910** 94.69% — W1 r29↔r31/r30 rotation. Hard skip.
- **fn_80206C94** 94.66% — W3 stmw (3 saved) + W1 r29↔r30. Hard skip.
- **fn_8020E204/fn_8020E488** 93.64% — clrlslwi-cse (noted previously). Hard skip.
- **fn_80206780** 92.26% — W4 booleanize + branch-shape diffs. Hard skip.
- **fn_80206AEC** 91.79% — W3 stmw (noted previously, was 89.42% now 91.79%). Hard skip.
- **fn_80204854** 91.32% — W3 stmw body-exact (noted previously). Hard skip.
- **fn_80206608** 91.06% — W4 booleanize + inverted condition. Hard skip.
- **fn_80209380** 89.85% — W3 stmw for 3 saved regs, body byte-exact. Hard skip.
- **fn_8020E95C** 89.63% — W1 r26-r31. Hard skip (noted previously).
- **fn_80202C1C** 89.09% — W1 r21-r31 + W4. Hard skip (noted previously).
- **fn_8020E614** 88.64% — W4 booleanize. Hard skip (noted previously).
- **fn_802032E4** 88.13% — W3 stmw (4 saved) + W1 r29-r31 rotation. Hard skip.
- **fn_802077D4** 87.89% — W3 stmw (noted previously). Hard skip.
- **fn_80202998** 87.57% — W3 stmw (2 saved regs, r30/r31). Hard skip.
- **fn_80210BF8** 86.15% — W3 stmw (4 saved) + W1 param home order. Hard skip.
- **fn_80210888** 85.61% — W3 stmw (4 saved) + W1 param home order. Hard skip.
- **fn_8020BA80** 85.17% (and 10+ others 80-85%) — W1+W3 combinations. Hard skip.
- All remaining 80-85% functions: W1 and/or W3 combinations. No improvable functions found.

**CONCLUSION**: colosseum_event.c 80-100% band is fully exhausted. All remaining near-misses
are blocked by W1 (register allocation), W3 (stmw threshold), W4 (booleanize idiom), or
W2 (scheduler) walls.

## Blocked near-misses (2026-06-10 session, Opus)

- **fn_80208028** @ 99.84% — W6 call-target NAME (`bl itemGetStatus` vs `bl fn_80142CF4`,
  same address). Byte-identical; symbol_mappings artifact. Hard skip.
- **fn_80211A78** @ 99.15% — single diff: `cmplwi r0, 0x1` (T) vs `cmpwi r0, 0x1` (M)
  at the switch case-1 compare. `#pragma peephole on/off` both emit `cmpwi` for small
  constants (this is a CW switch codegen wall, not peephole). `switch((u8)result){case 1:}`
  shape is correct; the compare instruction type is compiler-version-locked. Hard skip.
- **fn_8020E7AC** @ 96.44% / **fn_8020E95C** @ 89.6% — W1 saved-band rename (r26-r31).
  `u8 bVar1` fixes the `clrlwi.` combine but shifts the whole band -> net worse
  (E7AC 95.97). `matchVal==sVar3` operand-order flip inert (CW canonicalizes). Skip.
- **fn_8020DAD0** @ 95.0% — manual round-toward-zero `((s32)x>>1)+((s32)x<0 & x&1)`
  IS `(s32)x/2`; replacing -> +0.45 (3-instr srwi/add/srawi, still not target's
  2-instr srawi/addze DIV-IDIOM wall) + W1 r27-r31 rename. Reverted (sub-bar). Skip.
- **fn_8020BFA0** @ 95.15% — W2 scheduler call-arg slot interleave (target hoists
  `li r4/r5/r6` constant args into the lbl-address-load gap; CW groups them at the
  call) + `mr r0,r3; li r3,0` hop on a u16-return arg. Not per-spot controllable.
- **fn_802077D4** @ 87.9% — pure W3 stmw threshold (body matches). Skip.

NOTE: this TU's Ghidra imports share colosseum_script's bug classes — s8/short
mis-types on `==N` compares, and entry-`b` loop shapes written as `do{...}while(1)`
with a top-tested return. Both are pure correctness fixes; sweep them first.
