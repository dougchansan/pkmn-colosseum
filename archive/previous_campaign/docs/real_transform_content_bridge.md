# Real Transform Content Bridge

## Goal

Advance from the current real-content-backed but invisible `PObj` path to the
first visible real repository-content-backed slice by translating the smallest
enclosing real transform-facing chain above the already translated `PObj`.

## Ranked Transform-Facing Targets

1. One narrower real joint-chain-to-matrix helper above the translated `PObj`
   - Best next step.
   - This is smaller than a full host `HSD_Joint` / `HSD_DObj` object graph.
   - It tests the exact remaining question from the previous step:
     is the enclosing real joint transform alone enough to make the already
     translated real `PObj` visible?

2. One full real `HSD_Joint + DObjDesc + PObjDesc` chain
   - Larger than option 1.
   - It preserves more structure, but the current bridge only needs a real
     transform matrix plus the already translated real `PObj`.

3. One real scene / camera / projection slice
   - This was the fallback only if the real joint-chain translation proved
     insufficient.
   - It is broader than the joint-chain matrix step, so it was not the first
     choice.

## Chosen Target

I chose option 1: translate the real joint chain above the already translated
`PObj` into a host matrix, then apply that real matrix through the same
existing `fn_801AA568 -> fn_800DAD10` path.

Why this was the best gradient increase:

- it is the smallest real transform-facing slice above the translated object
- it avoids broad engine boot
- it keeps the same content-backed render bridge and changes only the missing
  transform input
- it cleanly distinguishes “missing joint transform” from “missing
  scene/camera/projection”

## What Real Transform / Content Slice Was Translated

The translated real chain was:

- `scene_data -> 0x2DC88 -> 0x2DC78 -> 0x6FA8 -> 0x7028 -> 0x7068 -> 0x5598 -> 0x5580`

Used directly in the smoke path:

- root joint: `0x6FA8`
- target joint: `0x7068`
- `dobjdesc`: `0x5598`
- `pobjdesc`: `0x5580`

The already translated real `PObj` from the previous step was preserved. This
step added only the real joint-chain matrix translation above it.

## What Changed

`src/pcport/real_content_host.h`

- added `PCPortTranslatedJointTransform`
- added `PCPort_TranslateJointChainToMatrixBE()`

`src/pcport/real_content_host.c`

- added affine matrix helpers
- added real joint local-matrix construction from serialized archive data
- added real joint path resolution from root joint to target joint
- added host matrix translation for the chosen real joint chain

`src/pcport/pcport_main.c`

- updated the existing `--real-content-translation-smoke` path to:
  - translate the real joint chain above the chosen `PObj`
  - load that real matrix into GX before the existing real-content draw
  - report transformed bounds as well as the existing framebuffer delta result

## What Was Verified

Verified directly:

1. The real joint-chain translation succeeded.
2. The real-content-backed smoke path still reached the existing game-owned
   render bridge.
3. The translated real content still submitted geometry:
   - submitted vertices: `1309`
   - expanded vertices: `1309`
   - last primitive: `0x98` (`GX_TRIANGLESTRIP`)
4. The transformed bounds were identical to the local bounds:
   - local:
     - `[-9.010, -7.020, -9.000]`
     - `[ 9.010,  7.020,  0.000]`
   - world:
     - `[-9.010, -7.020, -9.000]`
     - `[ 9.010,  7.020,  0.000]`
5. The existing regressions still passed unchanged:
   - `--real-content-parser-smoke`
   - `--gsgfx-pobj-smoke`
   - `--gsgfx-scene-like-smoke`
   - `--gsgfx-visible-smoke`
   - `--gsgfx-scissor-retry`

Primary smoke output:

```text
[pcport_bootstrap] Real content transform bridge reached fn_800DAD10 but changed no framebuffer pixels (scene=0x2DCC4 joint=0x7068 dobj=0x5598 pobj=0x5580 submitted=1309 expanded=1309 prim=0x98 local=[-9.010,-7.020,-9.000]-[9.010,7.020,0.000] world=[-9.010,-7.020,-9.000]-[9.010,7.020,0.000])
```

## Whether Visible Pixels Are Now Backed By Real Repository Content

No.

The real repository-backed object path plus the real joint-chain matrix path
both execute, but they still produce no visible framebuffer delta in the
current host-visible slice.

## Exact Remaining Blocker

This step changed the exact blocker:

- it is no longer “maybe the real joint transform is missing”
- it is now “the translated real joint chain above this `PObj` is not
  sufficient to move it into the visible range”

That conclusion is direct, not speculative:

- the joint-chain matrix translation succeeded
- the content still reached `fn_800DAD10`
- the geometry still submitted
- the world bounds remained identical to the local bounds
- framebuffer diff stayed zero

So the next smallest blocker is now a real scene / camera / projection slice,
not broader engine boot.

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

- the updated `--real-content-translation-smoke`
- the parser smoke
- all existing visible regressions

What succeeded:

- one real transform-facing slice above the translated `PObj` was translated
- that real transform was applied through the existing game-owned render bridge

What failed:

- visible real repository-content-backed pixels still did not appear

## Is Engine Boot Smaller Now?

No.

The next smaller step is now one real scene / camera / projection slice. Engine
boot would still have to solve that same visibility gap, so it is still larger
than the next direct translation step.

## Next Smallest Milestone

Translate exactly one real scene / camera / projection slice from `scene_data`,
then rerun the same real-content-backed smoke path with:

1. the existing translated real `PObj`
2. the existing translated real joint-chain matrix
3. one real scene/camera/projection input above them

That is now the honest critical path to a first visible real
repository-content-backed slice.
