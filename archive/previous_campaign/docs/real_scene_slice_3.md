# Real Scene Slice 3

## Goal

Advance from the current real textured repository-content-backed slice to the
first richer multi-object real scene slice under the same `scene_data` branch,
without broad engine boot.

## Ranked Targets

1. One additional real visible sibling object under the same branch:
   `joint 0x70E8 -> dobjdesc 0x6F98 -> mobjdesc 0x39CC -> tobjdesc 0x395C -> pobjdesc 0x6F80`
   - Best next step.
   - It stays under the same already working `scene_data` branch.
   - It only needed one extra texture format (`GX_TF_I8`) and no broader boot
     or scene parser work.
   - It is a distinct real branch element even though it reuses some lower
     asset payloads.

2. One second real textured/material combination on the existing visible base
   object:
   `joint 0x7068 -> dobjdesc 0x5598 -> mobjdesc 0x3A98 -> tobjdesc 0x3A28`
   - Similar host requirement (`GX_TF_I8`) but weaker for the user goal.
   - It deepens one object, not the scene structure.

3. A broader scene bundle only if the smaller object targets failed
   - Not chosen.
   - Larger than necessary while the sibling object path was still available.

## Chosen Target

Chosen target: the sibling object branch at:

- `joint 0x70E8`
- `dobjdesc 0x6F98`
- `mobjdesc 0x39CC`
- `tobjdesc 0x395C`
- `imagedesc 0x3924`
- `image data 0x2CBC0`
- `pobjdesc 0x6F80`

Why this was the fastest path toward a multi-object scene slice:

- it adds a real additional object under the already working `scene_data` tree
- it reuses the existing camera/projection path and the existing game-owned
  `fn_801AA568 -> fn_800DAD10` bridge
- it did not require broader TEV expansion, only a narrow `GX_TF_I8` texture
  decode plus acceptance of a real non-null `TObj->tev` pointer during
  translation

## What Additional Real Slice Was Translated

Translated real slices:

- sibling `PObjDesc` at `0x6F80`
- sibling joint-chain target at `0x70E8`
- sibling `MObjDesc` at `0x39CC`
- sibling `TObjDesc` at `0x395C`
- sibling `ImageDesc` at `0x3924`
- sibling image data at `0x2CBC0`

Important repository fact:

- this sibling branch reuses the same underlying image data pointer as the base
  object (`0x2CBC0`)
- it also reuses the same display-list hash and vertex-desc block
- but it is still a distinct real scene branch element with its own
  `joint/dobj/mobj/tobj/pobj` descriptors and a different real joint transform

## What Changed

### Narrow texture support

`src/pcport/gx_texture.c`

- implemented narrow `GX_TF_I8` decode to RGBA8 for the host bridge

This was the only additional GX/texture behavior needed for the chosen second
object.

### Narrow translation tolerance

`src/pcport/real_content_host.h`
`src/pcport/real_content_host.c`

- added `tevArchiveOffset` to `PCPortTranslatedTexture`
- relaxed `PCPort_TranslateTextureFromArchiveBE()` so one real non-null
  `TObj->tev` pointer can cross the translation boundary
- still kept TLUT, LOD, and texture-chain expansion out of scope

This is an explicit limitation:

- the sibling object’s real `TObjDesc` has `tev != 0`
- this step does not parse or execute that TEV descriptor
- the current host path still uses the already supported stage-0 TEV preset
  route derived from the texture flags

### Verification smoke

`src/pcport/pcport_main.c`

- added `--real-scene-slice-3-smoke`

That smoke renders:

1. the current real textured scene baseline:
   - front textured object `0x6FE8 -> 0x3B98 -> 0x3900 -> 0x3890 -> 0x3B80`
   - base material-backed object `0x7068 -> 0x5598 -> 0x3A98 -> 0x5580`
2. then the same scene plus the sibling textured object:
   - `0x70E8 -> 0x6F98 -> 0x39CC -> 0x395C -> 0x6F80`

All three still go through the existing game-owned render path.

## Exact Commands Run

Build:

```powershell
cmd /d /s /c '"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul && "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build build_pcbootstrap --target pcport_bootstrap -v'
```

Primary verification:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
```

Regression checks:

```powershell
.\build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-2-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --real-content-translation-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-pobj-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scene-like-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
.\build_pcbootstrap\pcport_bootstrap.exe --gsgfx-scissor-retry
```

## Results

Primary smoke output:

```text
[pcport_bootstrap] Real scene slice 3 smoke passed (scene=0x2DCC4 camera=0x2DC38 frontJoint=0x6FE8 frontDObj=0x3B98 frontMObj=0x3900 frontTObj=0x3890 frontPObj=0x3B80 baseJoint=0x7068 baseDObj=0x5598 baseMObj=0x3A98 basePObj=0x5580 siblingJoint=0x70E8 siblingDObj=0x6F98 siblingMObj=0x39CC siblingTObj=0x395C siblingImage=0x3924 siblingData=0x2CBC0 siblingPObj=0x6F80 diffPixels=307200 baselineCenter=96,96,137,201 multiCenter=86,86,114,164 siblingFmt=1 siblingSize=64x64 submitted=1309 expanded=1309 prim=0x98)
```

Verified directly:

1. The additional real sibling object branch translated successfully.
2. The sibling branch reached the framebuffer through the existing
   game-owned `fn_801AA568 -> fn_800DAD10` path.
3. The scene changed visibly relative to the current textured baseline.
4. Existing visible/content regressions still passed.

## Whether The Result Is Now A Real Multi-Object Scene Slice

Yes.

This smoke now renders three real repository-backed scene elements from the same
`scene_data` branch:

- front textured object
- base material-backed object
- sibling textured object

The result is still narrow and bootstrap-driven, but it is now a real
multi-object repository-content-backed scene slice.

## Exact Remaining Blockers

This is still not a title/menu/game slice.

The blockers now are:

1. broader real `TObj` / TEV behavior beyond the current preset-only route
   - the sibling branch already surfaced a real non-null `TObj->tev` pointer
   - this step translated that boundary but did not interpret the TEV payload

2. additional real scene objects may require more texture formats, texture
   chains, or broader material behavior

3. no engine-owned scene update, title/menu logic, or boot flow is driving
   these objects yet

## Next Smallest Milestone

Translate one more real scene branch element only if it still fits the current
or very slightly expanded texture/material bridge. If the next candidate needs
real TEV descriptor interpretation rather than another narrow texture format,
then broader `TObj/TEV` support has become the smaller next step, not engine
boot.
