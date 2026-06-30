# Real Scene Camera Bridge

## Goal

Reach the first visible real repository-content-backed slice by translating the
smallest real scene / camera / projection slice from `scene_data` and applying
it above the already working real `PObj` + real joint-chain path.

## Ranked Scene / Camera / Projection Targets

1. One real `HSD_CObjDescPerspective` slice plus its `eyepos` / `interest`
   `HSD_WObjDesc` inputs
   - Best next step.
   - This is the smallest real visibility-owning slice directly reachable from
     `scene_data`.
   - It changes real viewport, scissor, view, and projection state without
     broad scene boot.

2. One narrower scene helper that only yields the camera pointer from
   `scene_data`
   - Smaller structurally, but not sufficient by itself.
   - The previous step already proved the blocker was missing camera /
     projection state, not just camera discovery.

3. One broader scene bundle around the `scene_data` branch
   - Larger than option 1.
   - This would only be justified if the direct real camera slice proved
     insufficient.

## Chosen Target

I chose option 1: translate the real perspective camera slice directly from the
resolved `scene_data` graph and apply that real camera state above the existing
real joint-chain + real `PObj` path.

Why this was the best path:

- it was the smallest real slice that could directly affect visibility
- it reused the existing game-owned `fn_801AA568 -> fn_800DAD10` render bridge
- it avoided broad engine boot
- it cleanly tested whether the remaining blocker was truly scene / camera /
  projection

## What Real Scene / Camera / Projection Slice Was Translated

The translated real path was:

- `scene_data` public root at `0x2DCC4`
- `scene_data + 0x00 -> 0x2DC88`
- `0x2DC88 + 0x08 -> camera desc 0x2DC38`

The chosen real camera slice at `0x2DC38` resolved to:

- projection type: `1` (`perspective`)
- viewport: `(0, 640, 0, 480)`
- scissor: `(0, 640, 0, 480)`
- near: `0.1`
- far: `32768.0`
- fov: `61.3726578`
- aspect: `1.27999997`
- `eyepos` `HSD_WObjDesc`: `0x2DC10`
- `interest` `HSD_WObjDesc`: `0x2DC24`
- eye: `(0.0, 0.0, 1.2049...)`
- interest: `(0.0, 0.0, 0.0)`

This step preserved the already translated real content path beneath it:

- root joint: `0x6FA8`
- target joint: `0x7068`
- `dobjdesc`: `0x5598`
- `pobjdesc`: `0x5580`

## What Changed

`src/pcport/real_content_host.h`

- added `PCPortTranslatedCamera`
- added `PCPort_TranslatePerspectiveCameraFromArchiveBE()`

`src/pcport/real_content_host.c`

- added big-endian camera-field parsing for the serialized perspective camera
  layout
- added serialized `HSD_WObjDesc` position loading for `eyepos` and `interest`
- added host look-at view-matrix construction
- added host perspective projection-matrix construction

`src/pcport/pcport_main.c`

- updated `--real-content-translation-smoke` to:
  - resolve the real camera desc from `scene_data`
  - translate that real camera slice
  - apply its real viewport and scissor
  - apply its real projection matrix
  - concatenate its real view matrix above the existing translated real
    joint-chain matrix
  - reuse the same existing `fn_801AA568 -> fn_800DAD10` draw path
  - report local, world, and view-space bounds plus framebuffer delta

## What Was Verified

Verified directly:

1. The real scene camera slice translated successfully from `scene_data`.
2. The translated real camera state applied cleanly to the existing real
   content-backed draw path.
3. The same real `PObj` and real joint-chain path that previously submitted
   invisible geometry now produced visible framebuffer changes.
4. Existing render regressions still passed unchanged.

Primary smoke output:

```text
[pcport_bootstrap] Real scene camera bridge smoke passed (scene=0x2DCC4 camera=0x2DC38 joint=0x7068 dobj=0x5598 pobj=0x5580 changedPixels=307200 submitted=1309 expanded=1309 prim=0x98 local=[-9.010,-7.020,-9.000]-[9.010,7.020,0.000] world=[-9.010,-7.020,-9.000]-[9.010,7.020,0.000] view=[-9.010,-7.020,-10.205]-[9.010,7.020,-1.205])
```

Interpretation:

- visible framebuffer pixels now appear from real repository content
- the prior invisibility blocker was the missing real scene / camera /
  projection slice
- the real object is now demonstrably in front of the translated camera in
  view space

## Whether Visible Pixels Are Now Backed By Real Repository Content

Yes.

This visible result is backed by:

- a real FSYS member from `orig/GC6E01/disc/files/topmenu.fsys`
- real serialized `scene_data`
- a real serialized perspective camera slice from that scene graph
- real serialized `HSD_WObjDesc` eye / interest inputs
- the previously translated real serialized joint-chain slice
- the previously translated real serialized `PObj` slice
- the existing game-owned `fn_801AA568 -> fn_800DAD10` render bridge

## Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-translation-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

## Results

What compiled and linked:

- `src/pcport/real_content_host.c`
- `src/pcport/pcport_main.c`
- the existing `pcport_bootstrap` target

What ran:

- `--real-content-translation-smoke`
- `--real-content-parser-smoke`
- `--gsgfx-pobj-smoke`
- `--gsgfx-scene-like-smoke`
- `--gsgfx-visible-smoke`
- `--gsgfx-scissor-retry`

What succeeded:

- one real scene / camera / projection slice was translated from `scene_data`
- that real camera state made the previously invisible real repository content
  visible through the existing game-owned render bridge

## Exact Remaining Blockers

This is still not a title / menu / game slice.

The remaining blockers are now:

- only one real camera slice and one real renderable object path are translated
- broader real scene content above / beside this object is not yet translated
- real material / texture / lighting / TEV coverage is still partial
- no engine-owned scene update / menu logic / boot flow is driving the content

## Is Engine Boot Smaller Now?

No.

The current blocker was crossed directly. The next smaller steps are still
content- and render-facing, not broad engine boot.

## Next Smallest Milestone

Translate one additional real scene object or real material / texture slice
that lives under the same `scene_data` branch and still fits the current narrow
host GX subset, then feed it through the same visible content-backed path.
