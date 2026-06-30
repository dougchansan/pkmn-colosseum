# Real Scene Slice 2

## Goal

Advance from the first visible real repository-content-backed slice to the
first multi-object or material-backed scene slice by translating exactly one
additional real `scene_data` branch element that deepens the current visible
result without broad engine boot.

## Ranked Targets

1. One real material slice for the already visible object:
   `dobjdesc 0x5598 -> mobjdesc 0x3A98 -> material 0x3A84 -> pedesc 0x39E4`
   - Best next step.
   - This object is already visible through the existing real camera + real
     joint + real `PObj` path.
   - The host bridge already has blend, Z, and alpha-compare knobs, so this
     only needed a narrow translation layer plus a minimal per-vertex alpha
     modulation hook.

2. One additional real visible object under the same branch:
   `joint 0x70E8 -> dobjdesc 0x6F98 -> pobjdesc 0x6F80`
   - Smaller than texture bring-up, but not a good next increment.
   - Direct inspection showed it reuses the same display list bytes and vertex
     descriptor block as the current visible object, and its enclosing joint
     path collapses to the same effective transform.
   - That made it unlikely to deepen the visible result honestly.

3. One broader scene bundle through the front textured branch:
   `joint 0x6FE8 -> dobjdesc 0x3B98 -> mobjdesc 0x3900 -> pobjdesc 0x3B80`
   - This is the next larger path.
   - It would require the first real texture-backed object path, which means
     real `TObj` / `ImageDesc` handling and texture upload, not just material
     state.

## Chosen Target

I chose option 1: the real `MObjDesc` on the already visible real repository
object.

Why this was the best gradient increase:

- it stayed on the already verified visible object path
- it was smaller than real texture bring-up
- it was more likely to produce a visible difference than the sibling object
  path, which inspection showed to be structurally duplicative
- it let the additional state still flow through the existing game-owned
  `fn_800DAD10` dispatch path via the stubbed pipeline hooks

## What Additional Real Scene / Material Slice Was Translated

The translated additional real slice was:

- `scene_data -> ... -> joint 0x7068 -> dobjdesc 0x5598 -> mobjdesc 0x3A98`

Translated from that real `MObjDesc`:

- `rendermode = 0x60006011`
- real `HSD_Material` at `0x3A84`
  - ambient `0x808080FF`
  - diffuse `0xFFFFFFFF`
  - specular `0xFFFFFFFF`
  - alpha `0.3`
  - shininess `50.0`
- real `HSD_PEDesc` at `0x39E4`
  - `type = 1`
  - `src_factor = 4`
  - `dst_factor = 5`
  - `z_comp = 3`
  - `alpha_comp0 = 7`
  - `alpha_op = 0`
  - `alpha_comp1 = 7`

This remained stacked above the already translated real content path:

- real camera: `scene_data -> 0x2DC88 -> 0x2DC38`
- real joint chain: `0x6FA8 -> 0x7028 -> 0x7068`
- real `PObjDesc`: `0x5580`

## What Changed

`src/pcport/real_content_host.h`

- added `PCPortTranslatedMaterial`
- added `PCPort_TranslateMaterialFromArchiveBE()`

`src/pcport/real_content_host.c`

- added narrow big-endian translation for serialized `HSD_MObjDesc`
- added narrow translation of the attached `HSD_Material`
- added narrow translation of the attached `HSD_PEDesc`

`src/game/gs_render.c`

- added a minimal host pipeline-state table keyed by `pipelineId`
- upgraded the `PCPORT` `fn_800D892C` path so `fn_800DAD10` can apply:
  - translated blend state
  - translated Z state
  - translated alpha-compare state
  - a translated material alpha scale

`src/pcport/gx_shim.h`

- added `GXHostSetVertexAlphaScale()`

`src/pcport/gx_shim.c`

- added narrow host alpha modulation for submitted vertex colors
- kept it scoped to the existing primitive submission path

`src/pcport/pcport_main.c`

- added `--real-scene-slice-2-smoke`
- translated the real `MObjDesc` hanging off the already visible object
- rendered the same real content twice:
  - current opaque baseline path
  - material-backed path through `pipelineId = 1`
- compared the two framebuffer results directly

## What Was Verified

Verified directly:

1. The real `MObjDesc` / `HSD_Material` / `HSD_PEDesc` slice translated from
   repository content.
2. That translated state was applied through the existing game-owned
   `fn_800DAD10` draw-dispatch path.
3. The material-backed render produced a visible framebuffer delta relative to
   the current opaque real-content slice.
4. Existing regressions still passed.

Primary smoke output:

```text
[pcport_bootstrap] Real scene slice 2 smoke passed (scene=0x2DCC4 camera=0x2DC38 joint=0x7068 dobj=0x5598 mobj=0x3A98 pobj=0x5580 diffPixels=307200 opaque=178,178,178,255 material=87,202,119,201 alpha=0.300 blend=1/4/5 z=3 update=0 submitted=1309 expanded=1309 prim=0x98)
```

What that proves:

- the additional real scene element was not synthetic
- it changed the already visible repository-content-backed render materially
- the visible result is still backed by real repository content
- the extra state reached the framebuffer through the same existing real camera
  + real joint + real `PObj` + game-owned draw bridge

## Whether the Visible Result Is Materially Richer And Still Backed By Real Repository Content

Yes.

The previous center sample was:

- opaque baseline: `178,178,178,255`

The material-backed pass changed that same sample to:

- material-backed: `87,202,119,201`

That change came from a real repository `MObj` / `PEDesc` slice, not from
hardcoded content.

## Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-2-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-translation-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

## Results

What compiled and linked:

- `src/pcport/real_content_host.c`
- `src/pcport/gx_shim.c`
- `src/game/gs_render.c`
- `src/pcport/pcport_main.c`
- the existing `pcport_bootstrap` target

What ran:

- `--real-scene-slice-2-smoke`
- the real-content parser smoke
- the real scene camera smoke
- the existing visible regressions

What succeeded:

- one additional real material slice was translated from the same real
  `scene_data` branch
- that translated material state produced a materially richer visible result
  while staying on the existing repository-content-backed path

## Exact Remaining Blockers

This is still not a title / menu / game slice.

The main blockers now are:

- no additional distinct real object path is visible yet
- the obvious sibling object candidate under the same branch is not a useful
  next increment because it resolves to the same effective transform and
  geometry
- the next real branch that is likely to deepen the scene honestly is the
  textured front object path, which requires the first real texture-backed
  `TObj` / `ImageDesc` bridge
- TEV, texture upload, and broader material behavior are still partial
- no engine-owned menu / scene update path is driving the content yet

## Is Engine Boot Smaller Now?

No.

The next smaller path is still content- and render-facing: one real textured
object path or one broader real material / texture slice under the same
`scene_data` branch.

## Next Smallest Milestone

Bring up the front textured object under the same `scene_data` branch:

- `joint 0x6FE8 -> dobjdesc 0x3B98 -> mobjdesc 0x3900 -> pobjdesc 0x3B80`

That is the next honest step toward a title/menu-capable slice, because it is
the first additional branch element that is both visually distinct and blocked
mainly by texture support rather than by broader scene or engine boot.
