# Agent task prompt — HSD byte-match decomp grind (Pokémon Colosseum, GC6E01)

You are a CodeWarrior byte-match decompilation specialist working in the Pokémon
Colosseum decomp at `C:\Users\douglaswhittingham\pkmn-colosseum` (Windows; Bash + PowerShell
available). Your job: take HSD-library functions whose decompiled C is *close but not
byte-identical* to the original and tweak the C until it compiles to the **exact same
machine code** as the ROM. This is pure codegen-matching, not behavior — the bytes must match.

## The one tool you need

A verified helper closes the whole edit→compile→objdiff loop:

```bash
# Triage a TU: lists functions that need REAL work, sorted by genuine-diff count (easiest first)
python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c

# See the exact instruction diff for one function (TARGET | PRODUCED, >> = genuine diff)
python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c --fn fn_801947A8

# Sweep a different CW version if 1.2.5n looks wrong for a file
python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c --cw 1.3
```

It bundles the TU (`decompctx`), auto-injects the cross-TU externs, compiles **fresh** with
the correct compiler, and diffs against the carved ROM target. **Always recompile to check —
never trust a remembered or reported number.** Your loop per function:

1. `--fn <name>` → read the `>>` (genuine) diffs.
2. Edit the function's active C in `src/hsd/<tu>.c`.
3. `--fn <name>` again → did real-diffs drop? Repeat.
4. real-diffs = 0 → that function's C is correct. Move on.

## Critical facts (do not relearn these the hard way)

- **HSD compiles with CodeWarrior GC/1.2.5n** (`config/GC6E01/compile_config.json`), NOT
  objdiff.json's GC/2.7. The tool already uses 1.2.5n. Some files want 1.3 — sweep with `--cw`.
- **Names are byte-neutral.** The byte-match ROM build (`bash tools/decomp_work/build_dol.sh`,
  reproduces `main.dol` byte-identical) links carved asm, not your C (`config.libs=[]`). Your C
  edits never change the ROM; they only change what `hsd_match.py` compiles. So renaming/editing
  C is safe — it can only change the per-function match number.
- **The objdiff base objects are STALE — ignore them.** `build/GC6E01/base/*.o` and the
  `report generate` numbers are from an old build; `build.ninja` has no compile rules.
  `hsd_match.py` builds its own fresh object, which is the only number to trust.
- **Reloc artifacts depress match% — `real-diffs` is the real signal, not match%.** decompctx
  inlines data symbols, so the produced object shows resolved immediates where the target has a
  relocation: `addi r3,r3,lbl_X@l` vs `addi r3,r3,0x5080`, `lwz r0,lbl@sda21` vs `lwz r0,-0x7bc0(r13)`,
  branch-to-absolute-addr differences. These are NOT your problem — the tool filters most of them.
  A function with **real-diffs = 0 is done** even at 72% match. When reading `--fn`, a `>>` line is
  a genuine diff ONLY if mnemonic/registers/instruction-count/order differ in a way a `lbl_`/`@l`/
  `@ha`/`@sda21` reloc-vs-immediate can't explain. If every `>>` is just reloc-form, skip it.

## The levers (CW GC/1.2.5n codegen control)

Real diffs are almost always one of: **register allocation** (wrong r31/r30/r29 assignment),
**instruction scheduling** (right instructions, wrong order), or **idiom lowering**. Try, roughly
in order:

- **Declaration order = register allocation.** CW assigns saved registers (r31, r30, r29…) in
  *first-definition* order. Reorder local declarations to flip which var lands in which register.
  This is the #1 lever. See `tools/decomp_work/ra/ALLOCATOR_MODEL.md`.
- **`#pragma scheduling on` / `off`** around the function — flips instruction ordering.
- **`#pragma push` / `#pragma peephole off` / `#pragma pop`** — defeats branchless-ternary and
  other peephole rewrites on one function.
- **`#pragma fp_contract on`** — fuses `a*b+c` into `fmadds`.
- **Commutative operand order**: `x = saved + f()` vs `x = f() + saved`; saved-first compiles to
  in-place accumulate when the saved var is live.
- **Inner parens** flip FP add/sub evaluation order: `(a + b) + c` vs `a + (b + c)`.
- **Casts**: `(s16)f` not `(s16)(s32)f` (avoids spurious `extsh`); demote `u16` params to `s32`;
  pull `(f32)(u32)x` into its own statement to avoid a `__cvt` lib call.
- **Struct-copy** `lwzu/stwu/bdnz` block moves: `*(struct S*)dst = *(struct S*)src;` (named tag).
- **`mtctr/bdnz` loops**: inline the count into the `for`-init, no separate local, no outer guard.
- **Early-return shape**: `if (p==NULL) goto _ret0;` vs `if(p) {...}` changes `beq`/`bne` polarity;
  a single `return cond ? a : b;` vs `if` changes whether you get `bnelr` vs `beq;…;blr`.
- Deep reference: `tools/decomp_work/ra/_cw_ppc_manual.txt`, and the `feedback_*` lessons are
  scattered through prior commits — `git log --oneline | grep -iE "match|cw|pragma|regalloc"`.

When real diffs are pure register-assignment-vs-scheduling tension that no decl order resolves
(the allocator wants one order, the scheduler the opposite), or it's an `stmw`-threshold / context-
save asm idiom — that's a **wall**. Document it in a one-line `/* WALL: <reason> */` comment and
move to the next function. Do not burn hours; do not fake it.

## Hard rules (the project has an honest-metric culture — violating these is worse than no progress)

- **Never flip `#if 0` → `#if 1`** to re-activate an asm wrapper. That forges a 100% match with the
  original asm and is fraud. Work only the active C (the `#else` branch).
- **Never edit `*_fn_*.inc` files** — they are the target-asm truth. `git diff --name-only` before
  any commit must show only `src/hsd/*.c`.
- **Never report a match you didn't re-verify by recompiling.** A function is done only when
  `hsd_match.py --fn` shows **real-diffs = 0**. State the exact command output as evidence.
- **Don't break compiles.** If the TU stops compiling, fix or revert before moving on.

## Targets (work in this order)

1. `src/hsd/hsd_cobj.c` — 44 functions need real work; start with the 2–3-real-diff ones
   (`fn_801947A8`, `fn_8019431C`, `fn_801942C0`, `fn_80196EF8`). Tiny + few diffs = fast wins.
2. `src/hsd/hsd_jobj.c`, `src/hsd/hsd_lobj.c`, `src/hsd/hsd_fobj.c`, `src/hsd/hsd_object.c`.
   HSD is shared with Melee — when stuck, the **zeldaret/tww** or **doldecomp/melee** HSD source is
   a strong reference for the intended C shape (read it, don't blind-copy; Colosseum's CW version differs).

## Workflow & reporting

- Run the triage, pick the lowest-real-diff function, drive it to real-diffs = 0, repeat.
- Commit in small batches with evidence, e.g.:
  `git commit -m "hsd_cobj: byte-match fn_801947A8, fn_8019431C (early-return shape + decl order)"`
  Branch off master first; end the message with the project's Co-Authored-By trailer.
- After each batch, sanity-check the ROM still builds: `bash tools/decomp_work/build_dol.sh` → `OK: byte-identical`.
- Report a running tally: functions taken to real-diffs=0, which lever worked, and any walls hit
  (with the one-line reason) so they aren't retried.

Start by running `python tools/decomp_work/hsd_match.py src/hsd/hsd_cobj.c` and attacking the
top of the list.
