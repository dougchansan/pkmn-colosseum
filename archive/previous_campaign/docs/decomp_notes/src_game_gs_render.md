# Decomp notes: src/game/gs_render.c

## Status snapshot
190/258 @ 100% (73.6%, as of 2026-05-13 commit cf60079)

## Recently landed (2026-05-13, agent w2)

Pattern A: **`*(u32*)` → `*(s32*)` for cmpwi**. 9 functions fixed by flipping the load type so CW emits a signed compare (`cmpwi`) matching the target.

- fn_800D56C0, fn_800D5724, fn_800D579C, fn_800D5A38, fn_800D5AB0, fn_800D5B28, fn_800D5BA0, fn_800D9ED8, fn_800DBAA4 — all 98% → 100%.

Other wins:

- **fn_800DB900** — same cast + separate `u32 off` to control add-operand order.
- **fn_800D7894** — explicit `cnt` var declared before `p` to fix SDA2 load order for `mtctr`.
- **fn_800E02E8 / fn_800E032C / fn_800E0370** — `u8 tmp[0x38]` → `u8 tmp[0x30]`. Smaller array forces the 0x40 frame size to match target.

## Session: Jun 10 2026 (Fable)

### GSmathInitCosTable / cos name port (commit 2ae11ec7, byte-neutral)
Renamed `fn_800E0D24` → `GSmathInitCosTable` (definition + extern + the caller
in fn_800E0790) and the local extern `fn_800CDBE0` → `cos`. Both names are in
config/GC6E01/symbols.build.txt at the same addresses; the target object uses
them, so the relocations now match.

- **fn_800E0790** 95.0 → 95.238 (bl GSmathInitCosTable reloc fixed).
  Residual: hand-written `__asm` LR-save `stw r0,0x14(r1)` collides with the
  compiler's own prologue LR save (it emits a second one). Target has one stw
  AFTER `li r3,0x4`; removing the asm stw drops to 90% (compiler puts li after
  stw). Toolchain-locked hand-asm/compiler-prologue collision.
- **GSmathInitCosTable** was unmeasurable under the fn_ name → 99.891 (bl cos
  reloc fixed). Residual: the int→float magic double pool `@2289` vs the named
  `lbl_8047CB40@sda21` (W6 magic-double pool lock, CW_QUIRKS line 41/143).
  Tried the manual `(f32)((f64)(((u64)0x43300000<<32)|(u32)i) - lbl_8047CB40)`
  reconstruction — it restructured the whole frame and regressed badly. Locked.

## W6 name-port lever (general)
When the only residual on a near-miss is a relocation ARG mismatch on a `bl`/`b`
to a same-address callee that has a real SDK/library name in
symbols.build.txt, rename the local extern + call sites (and the definition if
it lives in this TU) to that name. Byte-neutral, lands the match. See also the
GXProject port in gs_render_util.

## Blocked near-misses

- **fn_800D7468** @ 98.67% — `instr-scheduling-order` (backward `f2/f1` load
  order). Untouched this session.
- **The fn_800D7420..fn_800D75AC GX-FIFO emitter family** (96.4-96.7%) — W2
  identical-opcode permutation: target puts the SDA base pointer in the HIGH
  reg (r6) and a byte temp in r4; CW assigns base→r4. Reverse-offset decl,
  fifo-pointer var, and inlined-base all kept 96.67% or worsened. Allocator
  decision, not source-controllable. Skip.

## Asm-active (not C — leave alone)
- **fn_800DE128** is `#if 1` (asm wrapper active). Its 2 residual `li rN,
  lbl@sda21` vs `addi rN,r2,0` diffs are inside the wrapper. Not a C target.

## Untouched near-misses
~63 remain per recon.

## Session log

- **2026-05-13 (w2)** — 14 near-misses → 100% via `s32` cmpwi cast, mtctr load-order, and frame-size shrink.
- **2026-06-10 (Fable)** — GSmathInitCosTable/cos byte-neutral name port (fn_800E0790 +0.24, GSmathInitCosTable now measurable @99.89). GX-FIFO family and fn_800D7468 re-confirmed walled.
