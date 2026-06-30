# Real Textured Scene Slice

## Goal

Advance from the real material-backed scene slice to the first real textured repository-content-backed object path under the same `scene_data` branch, without jumping to broad engine boot.

## Ranked Targets

1. One real `TObjDesc + ImageDesc` slice on the known front object branch
2. One narrower texture helper around the same branch only if the direct `TObj` slice proved larger
3. Broader TEV/material expansion only if the direct `TObj` slice could not reach the framebuffer

## Chosen Path

Chosen target: the direct real `TObjDesc + ImageDesc` slice on the known front object branch:

- `joint 0x6FE8`
- `dobjdesc 0x3B98`
- `mobjdesc 0x3900`
- `tobjdesc 0x3890`
- `imagedesc 0x3878`
- `image data 0x73C0`
- `pobjdesc 0x3B80`

Why this was the fastest honest next step:

- The current bridge already translated and rendered a real `PObj`, joint chain, camera, and material.
- The chosen branch already fits the current indexed attribute subset.
- The remaining missing work was strictly texture-object translation and one narrow host texture path.
- It avoided broad engine boot and avoided broad TEV expansion.

## What Changed

### Host-side translation

Added a narrow real texture translation layer in:

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`

It resolves one real `TObjDesc` and `ImageDesc` from the parsed big-endian archive into:

- texture object archive offset
- image descriptor offset
- image data offset
- width / height / format
- wrap state
- TEV preset mode

### Host GX path

Added the minimum texture support needed in:

- `src/pcport/gx_texture.c`
- `src/pcport/gx_shim.h`
- `src/pcport/gx_shim.c`

Scope:

- CMPR (`GX_TF_CMPR`) decode to RGBA8 for this host path
- one narrow `GXTexObj` upload path in `GXInitTexObj`
- one narrow texture bind path in `GXLoadTexObj`
- one host-only reset helper `GXHostClearTextureBinding`
- one stage-0 fixed-function TEV mapping for `MODULATE` / `REPLACE` / `PASSCLR`
- texture enable/bind inside the existing immediate/display-list submit path

No broad TEV shader work was added.

### Game-owned pipeline bridge

Extended the existing `PCPORT` `gs_render.c` pipeline table just enough to carry one texture through the existing game-owned draw path:

- `GSgfxHostSetPipelineTexture(...)`
- `fn_800D892C(...)` now clears texture state by default, then applies one translated texture when the pipeline requests it

### Verification smoke

Added a focused bootstrap path in `src/pcport/pcport_main.c`:

- `--real-textured-scene-slice-smoke`

It renders:

1. the already working real material-backed object as the baseline
2. then the same scene plus the translated real textured front branch through the existing game-owned `fn_801AA568 -> fn_800DAD10` path

## Exact Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-translation-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-2-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

## Results

Primary result:

- `--real-textured-scene-slice-smoke` passed

Observed verified output:

- `scene=0x2DCC4`
- `camera=0x2DC38`
- baseline object: `joint=0x7068 dobj=0x5598 mobj=0x3A98 pobj=0x5580`
- textured branch: `joint=0x6FE8 dobj=0x3B98 mobj=0x3900 tobj=0x3890 image=0x3878 data=0x73C0 pobj=0x3B80`
- `diffPixels=307200`
- baseline outside sample: `87,202,119,201`
- textured outside sample: `126,131,178,201`
- textured center sample: `96,96,137,201`
- translated texture metadata: `tev=0 size=640x480 format=14`
- textured draw submission: `submitted=4 expanded=4 prim=0x98`

## What Was Directly Verified

- One real `TObjDesc + ImageDesc` slice from `topmenu.fsys:menu_bg00` was translated.
- The translated real texture data was uploaded by the host GX shim.
- The translated front textured object reached the framebuffer through the existing game-owned `fn_801AA568 -> fn_800DAD10` path.
- The visible result is now materially richer than the previous material-only slice and is still backed by real repository content.
- Existing visible/content regressions still passed.

## What Remains Unproven

- This is still a narrow one-texture path, not a general `TObj` stack.
- No real title/menu logic or engine-owned scene update is running.
- No claim is made that the title/menu is bootable or playable.

## Remaining Blockers

The next blockers to a first title/menu/game slice are now:

1. broader real `TObj` / material behavior beyond this one stage-0 path
2. real texture-object chains or additional scene objects under `scene_data`
3. broader TEV/material/lighting coverage for real content
4. eventually an engine-owned boot/update path that drives scene selection and menu state

## Next Smallest Milestone

Translate exactly one more real textured or sibling visible object under the same `scene_data` branch, or prove that the next smaller step is a second real `TObj` / TEV-material combination rather than broad engine boot.
