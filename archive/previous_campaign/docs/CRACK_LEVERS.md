# CRACK_LEVERS — CodeWarrior reg-alloc / shape lever catalog

Read this once; the lane prompts only send the header (file + fns + tag).

## Workflow (WSL-free, never bare `bash`)
```
python tools/decomp_work/band.py init  <TAG> <FILE>
python tools/decomp_work/band.py diff   <TAG> <fn>     # see the exact instr miss
# edit tools/decomp_work/scratch/band_<TAG>.c
python tools/decomp_work/band.py check  <TAG>
python tools/decomp_work/band.py save   <TAG> <fn>     # only persists >=100%
python tools/decomp_work/kg/kg.py record-crack <fn> <lever>   # after a win
python tools/decomp_work/kg/kg.py q lever-targets <fn>        # which lever fits
```
A SAVED body must be REAL C. `asm{}` / `asm void` / `#include "*.inc"` = INSTANT
gate rejection + logged as fraud. NO vault-sync skill.

## Diagnose — run the classifier FIRST, every time
```
python tools/decomp_work/classify_residual.py <TAG> <fn>
```
It reads `band.py diff` and tells you which class the residual is + the exact fix.
**You may NOT file `WALL` on a fn whose classifier verdict is REG-COLORING (exit 0).**
That verdict means the instructions are already correct and only the register MAP
differs — it is ALWAYS winnable with the levers below. Filing it as a wall is the
#1 missed-win (fn_80200E00 sat at 97.92% this way).

The classes:
- **REG-COLORING** (same instrs, different register numbers, maybe a spare `mr`/`li`)
  → keep grinding: NAMED locals + declaration-order lever. Never a wall.
- **SCHEDULING** (same instr SET, reordered) → one scheduling-pragma / operand-order
  try, else wall.
- **RELOC** (`@nnn`-vs-`@named`, SDA-numeric, conversion-literal) → wall.
- **SHAPE** (mnemonics differ / >15% of lines / fn <95%) → REWORK: run
  `python tools/decomp_work/m2c_draft.py <fn> <FILE>` for a faithful draft, fix the
  shape/types to match, THEN re-diff and apply levers.

## ⚠️ Raw-m2c register locals PIN the coloring — never leave them
A draft that declares the CPU registers as locals (`u32 r27, r28, r29, r30; ...
r28 = *(u16*)(r29 + r0);`) compiles and lands ~98%, but it *forces* CW into one
register map that is usually one permutation off the target — and you can't move it,
because you named the registers. The fix is ALWAYS to rewrite as faithful C with
**named** locals (`msgId`, `basePtr`, `entry`, `result`) so the allocator is free,
then steer it with declaration order. If your scratch has `rNN` locals, that is the
bug, not a wall.

## Reg-alloc levers
- **declaration ORDER** of co-surviving locals sets the r31-down register map — reorder them.
- **named TEMP** for a repeated subexpr swaps operand register order.
- **parameter-aliasing**: alias a param into a local to fix a base-pointer regswap.
- **nested call**: nest a call expr to keep its result in r3 and avoid a non-volatile spill.
- **no-temp read-modify-write**; **block-scope decl restructuring** (opt-level gated).

## Type levers
- **`int` (NOT `s32`)** enables loop-unrolling — free, zero-risk, try it.
- a **u16/u8 return retyped to u32** drops a `clrlwi` mask and cascades reg numbering.
- u8-vs-s8 / signedness at the compare site.

## Shape levers (TWW)
- constant-compare chains are a **`switch`** (sometimes needs a dead `case`/`default`).
- **avoid `goto`** (worsens O4); null-checks should **FALL THROUGH** to the null case.
- `?:` vs `if/else` codegen differs; cast-operator-KIND swap `(T*)x` vs `(T)x`.
- toggle `const` on a primitive param (omitted from the mangled name — try both).
- **direct-array-indexed-hoist** (scratch-only): use `lbl + indexed_offset` directly instead of a cached base pointer when target initializes induction first, then hoists `lis/addi`; before calling it done, retry as a typed array/struct access and gate that version.
- **split-array-scalar-stack-order**: split an addressed local array into scalar locals, then order declarations to match target stack offsets and avoid saved element-address registers.

## Pragmas / opt
- `#pragma peephole off/on`; `#pragma optimization_level N`; scheduling on/off;
  per-fn opt-level; `#pragma opt_common_subs off` (defeats cross-block CSE).

## FILE AS WALL IMMEDIATELY (not C-reachable — do not grind)
pure scheduler reorderings, `beq;b`-vs-`bne`, `@nnn`-vs-`@named` reloc,
SDA-numeric-vs-`@sda21` / conversion-literal artifacts. Report `WALL <fn> <%> <residual>`.
**Before you type WALL, run `classify_residual.py <TAG> <fn>` — if it exits 0
(REG-COLORING), you may not wall it.** Register-number differences are NOT in the
wall list above; they are the most common *winnable* residual.
