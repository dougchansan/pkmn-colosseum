## Goal

Advance from the second real TEV-family milestone to the next smallest render/content milestone by admitting exactly one real `GX_VA_TEX1` object path if that is cheaper than broadening TEV generically.

## Ranked Paths

1. Admit `pda_menu.fsys:pda2_bg` object 0 through the existing game-owned draw bridge.
   - Path: `scene_data -> joint 0x22E8 -> dobj 0x1FB8 -> mobj 0x1A2C -> tobj 0x19BC -> tev 0x199C -> pobj 0x1FA0`
   - Why first: the current bridge already handled the same general scene branch, camera path, and I8-ramp TEV family, so the cheapest missing gap was the real `PObj` attribute set on this one object.

2. Move directly to the next shared `TObj->tev` / `TExp` boundary.
   - Why second: this was larger at the start of the task because the concrete blocker was still the narrow host `PObj` bridge rejecting `GX_VA_TEX1`.

## Chosen Path

I chose the object-0 path first.

That was the correct cheaper probe because the repo now proves the object is no longer blocked at `PObj`/display-list translation. The blocker moved forward to its texture-object boundary instead.

## Minimum Work Implemented

### 1. Admit `GX_VA_TEX1` in the narrow translated `PObj` bridge

- [real_content_host.h](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.h#L28): added storage for a second translated texcoord array.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L24): track a second texcoord index range.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L298): scan real display lists for `GX_VA_TEX1`.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L401): translate `GX_VA_TEX1` arrays just like the current `GX_VA_TEX0` path.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1043): free the second texcoord array during teardown.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1107): allow `GX_VA_TEX1` in `PCPort_TranslatePObjFromArchiveBE`.

### 2. Carry one selected texcoord through the host draw path

- [gx_shim.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\gx_shim.c#L177): mark `GX_VA_TEX1` as a supported display-list attribute.
- [gx_shim.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\gx_shim.c#L184): select the active texcoord stream from TEV stage 0.
- [gx_shim.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\gx_shim.c#L298): decode `GX_VA_TEX1` into the same single host texcoord slot when that coord is active.
- [gx_shim.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\gx_shim.c#L1583): consume `GX_VA_TEX1` bytes during real display-list replay.

### 3. Thread texture-coordinate selection through the game-owned pipeline state

- [gs_render.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\game\gs_render.c#L141): added `textureCoordId` to the host pipeline state.
- [gs_render.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\game\gs_render.c#L331): `GSgfxHostSetPipelineTexture` now records the texture-coordinate selection.
- [gs_render.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\game\gs_render.c#L384): issue `GXSetTevOrder` before `GXLoadTexObj`.
- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L941): propagate `translatedTexture.texCoordSrc` into the host pipeline setup.

### 4. Add a focused smoke target for the exact object-0 path

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L3593): added `RunRealSceneSlice4Smoke`.
- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L4145): exposed `--real-scene-slice-4-smoke`.
- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L3743): localized the exact object-0 blocker report.

## Exact Steps Attempted

1. Rebuilt:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

2. Attempted the new object-0 smoke:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
```

3. Re-ran regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### What succeeded

- The new `GX_VA_TEX1` bridge work compiled and linked.
- Existing visible and real-content regressions still passed unchanged.
- The object-0 path is no longer blocked at `PObj` translation.

That is directly verified by the new smoke getting past the `PObj`, joint, and material steps and then failing later at the texture-object step.

### What failed

Object 0 did **not** render.

The exact failing runtime output was:

- `tobj=0x19BC`
- `nextTObj=0x1918`
- `texCoordSrc=4`

The smoke failed with:

```text
[pcport_bootstrap] pda2_bg object 0 texture translation failed (tobj=0x19BC nextTObj=0x1918 texCoordSrc=4)
```

## Exact Blocker Chain

1. The original narrow blocker was real: object 0’s `PObj` included `GX_VA_TEX1`, and the host `PObj`/display-list bridge rejected it.
2. That blocker is now crossed.
3. The next blocker is object 0’s real `TObj` shape:
   - the `TObj` is chained (`nextTObj=0x1918`)
   - its raw `texCoordSrc` is `4`, not the simple coord-0/coord-1 case used by the current narrow bridge
4. So object 0 now requires a broader texture-object boundary than “one extra vertex attribute.”

## Regression Status

These still passed after the `GX_VA_TEX1` work:

- `--real-tev-scene-slice-3-smoke`
- `--real-tev-scene-slice-2-smoke`
- `--real-scene-slice-3-smoke`
- `--gsgfx-visible-smoke`

## Conclusion

Admitting the one real `GX_VA_TEX1` object path was still the right smaller probe. It exposed the exact next boundary honestly:

- not engine boot
- not more `PObj` work
- the next shared `TObj->tev` / `TExp` texture-object boundary

## Next Smallest Milestone

The smaller next step is now the next shared `TObj->tev` / `TExp` boundary, not engine boot:

- admit one chained `TObj` list shape
- determine what raw `texCoordSrc=4` means on this branch
- then retry the same `pda2_bg` object-0 smoke without broadening further than that
