# Goal

Advance exactly one bounded milestone beyond slice 16 toward a broader shared
real `TObj` / `TExp` parser/translation/classification layer, while staying
strictly below engine boot or update-flow work.

## Why RGBA8 no-TEV direct sample was the chosen next family

Slice 16 identified one concrete next reachable family already present in real
scene/archive data:

- one-stage `RGBA8` no-TEV direct sample

Representative sample:

- archive: `orig/GC6E01/disc/files/title.fsys`
- member: `logo_demo`
- path:
  - `joint 0x26748`
  - `dobj 0x25F38`
  - `mobj 0x15758`
  - `pobj 0x25F20`
  - `tobj 0x156E8`
  - `image 0x156D0`

Its key facts were already favorable for a small shared-layer step:

- `format = GX_TF_RGBA8`
- `tlutOffset = 0`
- `tevOffset = 0`
- `raw texCoordSrc = 4 -> coordId 0`

So the only forced blocker was the direct-sample stage-family predicate itself.
This made RGBA8 no-TEV direct sample the smallest evidence-backed next family.

## Exact files changed

- `src/pcport/real_content_host.c`
- `docs/real_scene_slice_17.md`

## Exact functions / predicates changed

### `src/pcport/real_content_host.c`

Added a narrow helper:

- `IsNoTevDirectSampleFormat(...)`

Updated the direct-sample stage-family predicate in:

- `ClassifyTextureExpStageKind(...)`

Before this slice, direct sample required:

- `format == GX_TF_CMPR`
- `tevArchiveOffset == 0`
- `tev.kind == PCPORT_TRANSLATED_TEV_NONE`

After this slice, direct sample requires:

- `format` is one of:
  - existing `GX_TF_CMPR`
  - newly admitted `GX_TF_RGBA8`
- `tevArchiveOffset == 0`
- `tev.kind == PCPORT_TRANSLATED_TEV_NONE`

## Whether the widening is RGBA8-only or a broader safe rule

This is a **narrow two-format rule**, not a broad no-TEV format expansion.

It remains intentionally limited to:

- `GX_TF_CMPR` (already supported)
- `GX_TF_RGBA8` (newly admitted)

I did **not** widen direct sample to every non-CI no-TEV format, because that
would have been less safe in the current tree. Several other decode paths in
`gx_texture.c` are still placeholder/TODO implementations, so a broader rule
would have admitted formats that are not yet proven safe in the current shared
bake path.

So the chosen widening is broader than a one-off branch in shape, but still
narrow and evidence-driven in scope.

## What became shared that was not shared before

Before this slice:

- the shared stage classifier recognized no-TEV direct sample only for `CMPR`
- the concrete reachable `RGBA8` no-TEV family from `title.fsys:logo_demo`
  collapsed to `PCPORT_TEXP_STAGE_NONE`
- therefore the shared whole-expression classifier could not admit it as a
  direct-sample family either

After this slice:

- the shared stage classifier now recognizes `RGBA8` no-TEV direct sample
- the existing shared whole-expression direct-sample classification follows
  automatically from the current parsed-chain logic
- the slice-16 `logo_demo` sample now classifies through the shared path as
  direct sample instead of failing classification

## What remains narrow

Still intentionally narrow after this slice:

- direct sample is **not** widened to all no-TEV non-CI formats
- only `CMPR` and `RGBA8` are admitted by the direct-sample predicate
- TEV handling remains unchanged
- `texCoordSrc` handling remains unchanged
- TLUT / CI acceptance remains unchanged
- no new whole-expression families were added beyond the direct-sample family;
  this slice only broadens the admitted stage-family formats for that family
- broader material-lighting and engine boot/update work remain out of scope

## Exact verification commands run

### Build

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

### Required smokes

```powershell
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --gsgfx-visible-smoke
```

### Narrow classification proof

I also reran a read-only ad hoc Python archive probe against the exact
`title.fsys:logo_demo` sample from slice 16 and observed:

```text
logo_demo sample joint=0x26748 dobj=0x25F38 mobj=0x15758 tobj=0x156E8 image=0x156D0 fmt=6 tlut=0x0 tev=0x0 rawSrc=4 coord=0 stage=direct_sample
```

That proves the representative RGBA8 sample now classifies through the shared
stage-family boundary as direct sample.

## Exact observed results

All required verification commands passed.

### Build

- `cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap`
  - passed

### Smokes

- `--real-textured-scene-slice-smoke`
  - passed
  - observed: `kind=3 stage0=3 stages=1 baked=1228800`
- `--real-scene-slice-3-smoke`
  - passed
- `--real-scene-slice-4-smoke`
  - passed
- `--real-tev-scene-slice-3-smoke`
  - passed
- `--real-tev-scene-slice-2-smoke`
  - passed
- `--gsgfx-visible-smoke`
  - passed

## Remaining blockers

1. No broader non-CMPR direct-sample format set is admitted yet beyond
   `RGBA8`.
2. Other unsupported `title.fsys:logo_demo` texture-expression samples still
   appear to require TEV-surface work rather than another direct-sample
   predicate tweak.
3. `texCoordSrc` support remains narrow beyond current verified meanings.
4. TLUT / CI-backed textures remain rejected during translation.
5. Engine-owned title/menu/game boot and update-flow work remain out of scope.

## Why engine boot is still not the next step

Engine boot is still not the next step because the shared real content path just
admitted one more concrete reachable family without touching lifecycle work.

The next useful step is still in the shared parser/classifier bridge:

- either the next concrete TEV-backed family from `title.fsys:logo_demo`
- or another equally evidence-backed family from already parsed scene members

That remains smaller, safer, and more directly verifiable than any engine boot
or lifecycle integration step.

## Git / checkpoint note

The workspace remains dirty outside this slice, so slice 17 should not force a
commit unless the tree is cleaned first.
