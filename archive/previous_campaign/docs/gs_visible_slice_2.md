# GS Visible Slice 2

## Goal

Advance from the first visible game-owned flat quad to the next smallest richer visible operation using real `gs_render.c` code, without pulling in assets or broad engine boot.

## Candidate Ranking

1. `fn_800D9D68` (`GSgfx_SetScissor`)
   - Best gradient increase.
   - Already has a decompiled C implementation in `src/game/gs_render.c`.
   - Very small dependency surface.
   - Can make the current visible quad richer by clipping it into a smaller on-screen region.
   - Requires no new texture, TEV, material, or draw-dispatch work.

2. `fn_800D9B58`
   - Likely a projection / visible-region helper.
   - More interesting visually than scissor, but it depends on `fn_800E0698`, `fn_800BD2E0`, and `fn_800D834C`, which are not all host-ready.

3. `fn_800DA428` / `fn_800DA4C4`
   - Blend helpers from the render-state block.
   - Too indirect for the next step because a single opaque quad has little visible payoff from blend changes without layering or background interaction.

## Chosen Path

I chose `fn_800D9D68`.

Why:

- it is real `gs_render.c` code, not just adjacent host scaffolding
- it is one step richer than the current flat full-screen quad
- it should have been able to produce a centered clipped panel using the existing `GSgfx_BeginFrame` draw
- it was the lowest-risk path that still moved the visible slice forward

## Attempted Integration

I attempted the path in four steps:

1. Move the `fn_800D9D68` implementation onto the `gs_render.c` `PCPORT` side so the symbol is provided by game-owned render code rather than `gs_gfx_host_support.c`.
2. Add a narrow smoke path that:
   - initialized `GSgfx`
   - set a centered scissor rectangle
   - called `GSgfx_BeginFrame`
   - sampled one pixel inside the scissor region and one outside it
3. Reapply scissor state inside the `GSgfx_BeginFrame` `PCPORT` subset because the harness clear step disables scissor before draw.
4. Re-test with explicit `GXSetZMode(0, GX_ALWAYS, 0)` as the last cheap state delta.

## Results

The attempted richer path did not become visible.

Observed failure pattern:

- build still compiled and linked
- host primitive submission still passed
- `GSgfx_BeginFrame` still submitted a correct quad:
  - `submitted=4`
  - `expanded=6`
  - `prim=0x80`
- but both the baseline visible path and the scissored path read back black:
  - `rgba=0,0,0,255`

This is the key reason I did not keep the scissor integration in the tree.

## Directly Verified

Verified during the failed attempt:

- the candidate code compiled and linked
- the host GX primitive path remained healthy
- the game-owned draw still reached `GXBegin` / `GXEnd` and submitted the expected primitive
- adding scissor state on the game-owned side was enough to break even the previously working full-screen visible path

Verified after reverting the failed attempt:

- `.\build_pcbootstrap\pcport_bootstrap.exe --gx-primitive-smoke` passed again
- `.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-smoke` passed again
- the original visible flat-quad slice is preserved

## Exact Blocker Chain

The blocker is now narrower than “general rendering does not work.”

1. The current `GSgfx_BeginFrame` host subset can produce visible pixels as a flat full-screen quad.
2. The next richer candidate, `fn_800D9D68`, is small enough to integrate from `gs_render.c`.
3. Once the visible path is routed through that game-owned scissor helper, pixels disappear even though primitive submission still reports a correct quad.
4. Reapplying scissor state in `GSgfx_BeginFrame` and restoring explicit Z disable did not recover visible pixels.

Inference from those results:

- the next blocker is the interaction between the current host GX scissor path and the minimal game-owned `GSgfx_BeginFrame` subset
- it is no longer credible to claim that another tiny `gs_render.c` helper alone will produce a richer visible result without first isolating scissor behavior itself

## Additional GX Support Required

None was proven sufficient for the chosen path.

I did not keep any new GX support changes from this attempt because the richer slice did not verify cleanly.

## Final State Left in Tree

I reverted the failed scissor integration and preserved the last known-good visible path.

That means:

- the repository still has the previously verified visible game-owned flat quad
- no richer visible game-owned operation is currently left enabled from this task

## Next Smallest Milestone

Before retrying another `gs_render.c` richer visible helper, the next prerequisite is:

- isolate and verify `GXSetScissor` against a host-side immediate draw path

Only after that should `fn_800D9D68` be retried on the game-owned side.

If that host scissor diagnostic fails, the blocker is in the host GX scissor implementation.
If it passes, the blocker is in the way the current `GSgfx_BeginFrame` host subset hands render state through to the scissored draw.
