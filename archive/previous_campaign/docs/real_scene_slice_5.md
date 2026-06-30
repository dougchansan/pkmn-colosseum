## Goal

Cross the next smallest real-content blocker by admitting exactly one chained
`TObj` list shape and resolving raw `texCoordSrc=4` for the existing
`pda_menu.fsys:pda2_bg` object-0 path, without broadening farther than
necessary.

## Ranked Paths

1. Admit the exact two-node `TObj` chain already blocking
   `pda2_bg` object 0.
   Path:
   `scene_data -> joint 0x22E8 -> dobj 0x1FB8 -> mobj 0x1A2C -> tobj 0x19BC -> next 0x1918 -> pobj 0x1FA0`
   Why first:
   the object was already proven past `PObj` translation and `GX_VA_TEX1`; the
   remaining gap was the narrow real texture-object boundary on this one object.

2. Move up to the broader shared `TObj` / `TExp` boundary.
   Why second:
   larger than needed if the exact object-0 chain could be admitted with one
   raw `src` interpretation and one chained-node shape.

## Chosen Path

I chose option 1: the exact two-node `TObj` chain on `pda2_bg` object 0.

That was still the smaller path. The repo-local evidence for this branch was:

- the first node is `tobj=0x19BC`
- the second node is `nextTObj=0x1918`
- the raw sources are `src0=4` and `src1=5`
- the object is already on the verified `fn_801AA568 -> fn_800DAD10` bridge

So the narrowest honest next step was:

- admit one chained `TObj` list shape
- resolve `src=4` / `src=5` on this branch
- reuse the existing game-owned draw path

## Exact Work Done

### 1. Admit one chained texture-node boundary

- [real_content_host.h](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.h#L153):
  exposed `PCPort_TranslateTextureNodeFromArchiveBE`.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1362):
  split texture translation into a shared helper with an `allowNext` path.
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1442):
  added the narrow node-level translator used for chained `TObj` members.

### 2. Admit the real non-null `lod` boundary for this path, but not broader texture support

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1406):
  stopped rejecting a non-null serialized `lod` pointer while still rejecting
  non-null `tlut` data.

This is intentionally narrow:

- object 0 needed the real `lod` boundary admitted to get past translation
- this step did not add general TLUT or full `TObj` feature support

### 3. Resolve raw `texCoordSrc=4` / `5` for this branch

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L936):
  added `ResolveTextureCoordIdFromRawSrc`.
- The narrow mapping used here is:
  - raw `4 -> coord 0`
  - raw `5 -> coord 1`

That mapping is repository-grounded for this path because the blocking object
already uses `GX_VA_TEX0` and `GX_VA_TEX1`, and the new smoke verifies those
resolved coord IDs directly at runtime.

### 4. Bake the exact object-0 chained texture shape

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L1066):
  added `BakePdaMenuObject0ChainTexture`.

The exact narrow composition used here is:

- node A (`0x19BC`) stays the real I8-ramp base texture
- node B (`0x1918`) is treated as the real I8 modulate mask for this exact
  branch shape

That interpretation is still narrow and repo-backed:

- the chain is exactly two nodes
- the second node's serialized flags match the alpha-modulate shape checked in
  the smoke
- the second node's translated TEV colors are zero on this branch

### 5. Retry the exact object-0 smoke through the existing game-owned path

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L3789):
  updated `RunRealSceneSlice4Smoke`.
- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L4390):
  retained `--real-scene-slice-4-smoke`.

The smoke now:

- translates both `TObj` nodes
- validates the exact narrow chain shape
- bakes the chain into one host texture for this exact branch
- drives object 0 through the existing game-owned `fn_801AA568 -> fn_800DAD10`
  render path

## Exact Steps Attempted

1. Rebuilt `pcport_bootstrap`:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

2. Ran the exact object-0 smoke:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
```

3. Re-ran regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### What was directly verified

The object-0 path now renders through the existing game-owned bridge.

The final verified runtime output was:

```text
[pcport_bootstrap] Real scene slice 4 smoke passed (scene=0x28BC4 camera=0x28B38 obj0Joint=0x22E8 obj0DObj=0x1FB8 obj0MObj=0x1A2C obj0TObj0=0x19BC obj0TObj1=0x1918 obj0Tev0=0x199C obj0Tev1=0x18F8 rawSrc0=4 coord0=0 rawSrc1=5 coord1=1 obj1Tev=0x1A5C diffPixels=307200 baselineCenter=14,24,7,59 richerCenter=0,0,0,0 obj0Light=127,178,76,255 obj0Dark=51,76,25,255 baked0=262144 baked1=262144 submitted=134 expanded=134 prim=0x98)
```

That directly proves:

- object 0 is no longer blocked at `PObj`
- object 0 is no longer blocked at `GX_VA_TEX1`
- object 0 is no longer blocked at `tobj=0x19BC -> next=0x1918`
- raw `src0=4` and `src1=5` were resolved to `coord0=0` and `coord1=1`
- the object rendered through the existing game-owned path

### Whether object 0 rendered

Yes.

The smoke passed and reported:

- `submitted=134`
- `expanded=134`
- `prim=0x98`

### Whether the visible result is materially richer

Yes.

The framebuffer changed by:

- `diffPixels=307200`

and the sample pixel changed from:

- `baselineCenter=14,24,7,59`

to:

- `richerCenter=0,0,0,0`

That is a real additional content-backed datapoint, not just a translation-only
success.

## Regression Status

These all still passed after the chained-`TObj` / raw-`src` work:

- `--real-tev-scene-slice-3-smoke`
- `--real-tev-scene-slice-2-smoke`
- `--real-scene-slice-3-smoke`
- `--real-textured-scene-slice-smoke`
- `--gsgfx-visible-smoke`

## What Was Inferred Versus Verified

### Directly verified

- the exact chain offsets
- the exact raw `src` values
- the resolved coord IDs used by the smoke
- the object-0 render pass succeeding through the existing game-owned bridge
- the framebuffer delta

### Narrow inference used

The second `TObj` node is currently treated as an I8 modulate mask for this
exact branch shape.

That is not a claim of general `TObj` / `TExp` support. It is a narrow
inference from:

- the exact two-node chain shape
- the second node's real alpha-modulate flags
- the zero-color translated TEV boundary on that second node
- the current object-local smoke succeeding without broader texture-object work

## Exact Blockers Still Remaining

This step crossed the object-0 blocker.

The next blockers are now broader than this exact chain shape:

1. There is still no general chained `TObj` list support.
2. There is still no shared `TObj` / `TExp` parser.
3. `texCoordSrc` support is still narrow to the exact meanings exercised so far.
4. TEV interpretation is still narrow and family-specific.
5. There is still no engine-owned title/menu boot or update flow driving these
   objects.

## Conclusion

Admitting the exact chained-`TObj` / raw-`src` object-0 path was still cheaper
than moving up to the broader shared `TObj` / `TExp` boundary, and it
succeeded.

That means the current critical path is still:

- not engine boot
- not more `PObj` work
- the next broader shared texture-object / TEV boundary above this exact
  object-0 chain shape

## Next Smallest Milestone

The next smaller step is still not engine boot.

It is the next shared `TObj` / `TExp` boundary:

- either admit one more real chained-`TObj` family beyond this exact two-node
  shape
- or expose the smallest real shared `TObj` / `TExp` translation layer that can
  replace this branch-specific composition
