# Decomp notes: src/game/gs_render_util.c

## Session log

- **2026-06-10 (Fable)** — fn_800D2DE8 (99.94→100.0) and fn_800D2F34 (99.93→100.0)
  via a W6 naming fix: the sole residual diff was `bl GXProject` (target) vs
  `bl fn_800BD16C` (ours) — a byte-identical `bl` to 0x800BD16C that objdiff
  flags as DIFF_ARG_MISMATCH because the target object names the callee
  `GXProject`. Renamed the local extern + 2 call sites
  `fn_800BD16C` → `GXProject` (the real GX SDK name, already in
  config/GC6E01/symbols.build.txt); the relocation now carries the matching
  name. fn_800BD16C is referenced only from this TU, so no cross-file impact.

## Lever recorded
- **W6 SDK-name port lands 100%** when the only residual is a relocation
  ARG mismatch on a `bl`/`b` to a same-address callee with a real SDK name in
  symbols.build.txt. Rename the local extern + callsites to that name.

## Near-misses still open (re-measured 2026-06-10)
| Function | % | class |
|---|---|---|
| fn_800D27FC | 95.67 | multi-diff: control-flow inversion (bne/b vs beq), a `cmplwi r0,0;blt` vs `cmpwi` quirk on the slot+0x110 `>=0` check (s32 cast swap didn't move it), and an else-block store-materialization (target `li r3,1;li r0,0` vs our `stb r5/r4` reuse). Several independent issues — not a single-lever win. |
| fn_800D2AD4 | 94.11 | TBD |
| fn_800D13C8 | 92.57 | TBD |
| fn_800D2150 | 89.33 | TBD |
| fn_800D2738 | 88.06 | TBD |
