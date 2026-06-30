# Reference GCN/Wii decomp projects with documented CW codegen techniques

These public byte-match decomp projects use the same Metrowerks CodeWarrior
compiler family (CW 1.x / 2.x). They are the highest-signal external reference
for "what C source produces this asm under CW." Cite or imitate their idioms
when you are stuck on a CW-specific optimization shape.

## doldecomp/melee — Super Smash Bros. Melee (CW 2.x, GCN)
- `src/melee/mn/mnmain.c` — single-case `switch` paired with `#pragma dont_inline on/off` inside `#pragma push/pop` to control inlining at boundaries.
- `src/melee/mn/mncharsel.c` — `(s8)byte < 0` sign-bit test pattern (works on CW 2.x; CW 1.3 sometimes regresses, prefer `((u32)x >> 7) & 1`).
- `src/melee/gr/grlib.c` — `s16` typed struct field stores; required to elide `extsh` before `sth`.

## zeldaret/tww — The Wind Waker (CW 1.2+, GCN)
- `docs/regalloc.md` — exhaustive catalog of register-allocation techniques: declaration order, actor-base extraction, `const` qualifiers, cast-operator type choice. **The single best reference for CW reg-alloc tricks.**
- `docs/decompiling.md` — switch comparison-tree compilation. Single-case `switch` with `case >= 1` produces the canonical 5-instruction `cmpwi N; beq; bge; cmpwi 0; b` form.
- `include/JSystem/...` — Dolphin SDK headers; type definitions for system structs that often appear unchanged across GCN games.

## doldecomp/pikmin2 — Pikmin 2 (CW 1.x — closest to our CW 1.3, GCN)
- `src/plugProjectEbisawaU/ebiScreenTitleMenu.cpp` — 2-case state-machine switches for menu screens.
- `src/plugProjectYamashitaU/pikiState.cpp` — linked-list traversal with switch over event types.
- Pikmin 2 is the **closest public reference** to our compiler version. When idioms differ, trust pikmin2 over melee/tww.

## PrimeDecomp/prime — Metroid Prime (CW 2.x, GCN)
- `src/MetroidPrime/CStateManager.cpp` — `#pragma inline_max_size(N)` to constrain inlining heuristics.
- `src/Kyoto/...` — Kyoto engine math/container patterns useful for Pokémon Colosseum's HSD pendant.

## doldecomp/sms — Super Mario Sunshine (CW 1.x, GCN)
- Strong HSD/Dolphin SDK overlap. Patterns for nested function-call chaining without non-volatile spill are well-documented.

## zeldaret/tp — Twilight Princess (CW 2.x, Wii/GCN)
- `docs/` — diff workflow + match-floor methodology mirrors ours. Useful for "what does ≥90% really mean" sanity.

## doldecomp/dolphinsdk — Raw Dolphin SDK (CW 1.2.5)
- `src/PAD/PADClamp.c`, `src/OS/OSContext.c`, etc. — unchanged across all GCN games. **Pokémon Colosseum reuses these almost verbatim**, so when an HSD/PAD/OS function in our project looks like a thunk, this repo's exact source likely matches.

## Confirmed cross-project techniques (do these)
1. **Inline CTR-loop count into `for(;;)` init** — never extract to a local first.
2. **`u32` counters → `cmplwi`**, `s32` → `cmpwi`. Type drives instruction.
3. **Block-scope `{ }` to defeat CSE** when target loads the same SDA symbol twice.
4. **Use existing externs** — never invent new ones. Grep the target `.c` file first.
5. **`#pragma push / peephole off / pop`** for branchless ternary suppression.
6. **`#pragma fp_contract on`** before `a*b+c` patterns that need `fmadds`.
7. **No `register` keyword, no `inline`**. CW ignores both at -O4.

## Confirmed traps (don't do these)
1. Float literals (`0.0f`) — always use `extern f32 lbl_XXXX;` for sdata2 floats.
2. Hoisting CTR loop count into a local variable + outer `if (n != 0)` guard.
3. `extern T (*fn(void))(A, B);` — CW rejects. Use `typedef T (*Fp)(A, B); extern Fp fn(void);`.
4. Adding "harmless" extern duplicates — CW errors on differing-signature redeclarations.
5. Trusting any subagent's match% claim without re-running `match_scan_file.py` twice.
