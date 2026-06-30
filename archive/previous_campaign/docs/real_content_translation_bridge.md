# Real Content Translation Bridge

## Goal

Cross the current real-content blocker by translating the smallest real
serialized archive object slice reachable from `scene_data` into host-native
`HSD_*` descriptor / object structures consumable by the existing
`fn_800DAD10` render path.

## Ranked Translation Targets

1. One real `HSD_PObjDesc` slice
   - Best next step.
   - The first renderable slice reachable from `scene_data` is:
     `scene_data -> 0x2DC88 -> 0x2DC78 -> 0x6FA8 -> 0x7028 -> 0x7068 -> 0x5598 -> 0x5580`
   - `0x5580` is the narrowest real object slice that already matches the
     current host GX subset:
     - `GX_VA_POS`
     - `GX_VA_CLR0`
     - `GX_VA_TEX0`
   - It can be fed into the existing `fn_801AA568` + `fn_800DAD10` bridge
     without pulling in material, texture, or engine boot work first.

2. One real `HSD_DObjDesc + child PObjDesc` slice
   - Larger than option 1.
   - `0x5598 -> 0x5580` would preserve the real display-object link, but the
     current bridge does not yet need `MObj` state for this first content test.

3. One real `HSD_Joint + DObjDesc + PObjDesc` slice
   - This is the most likely next blocker if option 1 reaches draw submission
     but still produces no visible pixels.
   - It is still smaller than engine boot, but it is larger than a direct
     `PObj` translation.

## Chosen Target

I chose option 1: translate exactly one real `HSD_PObjDesc` slice, reached from
the real `scene_data` graph, into a host-native `HSD_PObj` plus host-native
vertex arrays.

Why this was the fastest honest step:

- it is the smallest real object slice already reachable from `scene_data`
- it matches the currently supported indexed GX attribute subset
- it can use the already verified `fn_801AA568` -> `fn_800DAD10` render path
- it tests the translation boundary directly instead of hiding it behind engine
  boot

## What Changed

`src/pcport/real_content_host.h`

- added `PCPortTranslatedPObj`
- added `PCPort_TranslatePObjFromArchiveBE()`
- added `PCPort_DestroyTranslatedPObj()`

`src/pcport/real_content_host.c`

- added host-native translation for one serialized `HSD_PObjDesc` slice
- added serialized `HSD_VtxDescList` parsing
- added big-endian float conversion for position / texcoord arrays
- added real display-list scanning so the host bridge derives the true command
  span instead of assuming the serialized `n_display` field is a byte size

`src/pcport/pcport_main.c`

- added `--real-content-translation-smoke`
- added a narrow resolver that walks the real `scene_data` joint tree until it
  finds the first renderable `PObjDesc`
- added a full-frame before/after framebuffer diff check so visibility is
  measured against the existing `GSgfx_BeginFrame` baseline instead of by one
  guessed sample pixel

## What Serialized Slice Was Translated

Translated directly:

- archive: `orig/GC6E01/disc/files/topmenu.fsys`
- member: `menu_bg00`
- public root: `scene_data`
- chosen real object slice:
  - `joint = 0x7068`
  - `dobjdesc = 0x5598`
  - `pobjdesc = 0x5580`

The translated host-native slice contains:

- one host `HSD_PObj`
- one host `HSD_VtxDescList[]`
- converted host position array
- copied host color array
- converted host texcoord array
- copied real display-list byte stream

## What Was Verified

Verified directly:

1. The existing parser bridge still resolves the real `scene_data` root from
   `topmenu.fsys:menu_bg00`.
2. The smoke harness can walk the real `scene_data` graph to the first
   renderable `PObjDesc`.
3. The chosen real serialized `PObjDesc` at `0x5580` can be translated into
   host-native `HSD_PObj` / `HSD_VtxDescList` state.
4. The existing render bridge can consume that translated object:
   - `fn_801AA568(&translatedPObj.pobj)`
   - `fn_800DAD10(&drawObject)`
5. The translated real object reaches real GX submission:
   - submitted vertices: `1309`
   - expanded vertices: `1309`
   - last primitive: `0x98` (`GX_TRIANGLESTRIP`)

Smoke output:

```text
[pcport_bootstrap] Real content PObj translation reached fn_800DAD10 but changed no framebuffer pixels (scene=0x2DCC4 joint=0x7068 dobj=0x5598 pobj=0x5580 submitted=1309 expanded=1309 prim=0x98 bounds=[-9.010,-7.020,-9.000]-[9.010,7.020,0.000])
```

## Whether The Visible Result Is Now Backed By Real Repository Content

Not yet.

The real repository-backed object path is now translated and consumed by the
existing game-owned draw bridge, but it still produces no visible framebuffer
delta under the current `GSgfx_BeginFrame` visible slice.

Important boundary:

- the content is now real
- the host object translation is now real
- the game-owned dispatch path is now real
- the visible result is still not real repository content yet

## Why The Visible Result Still Failed

Directly verified:

- the translated object changed no framebuffer pixels
- the translated object still reached `fn_800DAD10`
- GX still submitted the real object display-list stream
- the translated object bounds were:
  - min: `[-9.010, -7.020, -9.000]`
  - max: `[ 9.010,  7.020,  0.000]`

Inference from the verified bounds plus the current render path:

- the current `PCPORT` `GSgfx_BeginFrame` visible slice still uses a narrow
  clip-space-style setup
- the translated real object is not yet being paired with the enclosing real
  transform / camera / projection slice needed to move that content into the
  visible range

So the translation blocker is crossed, but the next visibility blocker is now
at least one enclosing real transform-facing slice, not the `PObj` translation
layer anymore.

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

- the new `--real-content-translation-smoke`
- the parser smoke
- all existing visible regressions listed above

What succeeded:

- one real serialized archive object slice was translated into host-native HSD
  object state
- the existing render bridge consumed that translated object

What failed:

- the translated real object still produced no visible framebuffer delta

## Exact Remaining Blockers

1. The smallest real `PObj` translation layer now works, so the blocker is no
   longer raw archive parsing or raw `PObj` translation.
2. The current visible slice still lacks the enclosing real transform-facing
   slice that positions this object into the current host-visible range.
3. The next smallest honest step is at least one real parent transform or
   scene/camera/projection slice reachable from `scene_data`.
4. Broader parser bring-up is not yet smaller than that next transform-facing
   step.
5. Engine boot is still not smaller than that next transform-facing step,
   because boot would still need the same real transform / camera translation
   before this content becomes visible.

## Next Smallest Milestone

Translate exactly one enclosing real transform-facing slice above the chosen
`PObj`, then rerun the same content-backed `fn_800DAD10` smoke:

1. first candidate: one real `HSD_Joint + DObjDesc + PObjDesc` chain rooted at
   `0x7068 -> 0x5598 -> 0x5580`
2. if that still does not produce visible pixels, the next exact prerequisite
   is one real scene/camera/projection slice from `scene_data`

That is still smaller than engine boot and is now the honest critical path to a
first visible real repository-content-backed slice.
