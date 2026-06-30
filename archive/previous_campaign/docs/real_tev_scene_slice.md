# Real TEV Scene Slice

## Goal

Advance from the real multi-object scene slice to the first real TEV-interpreted
scene slice, using the smallest honest interpretation of one real `TObj->tev`
payload already present on an existing visible object.

## Ranked TEV Targets

1. Sibling textured object TEV payload
   - Path: `scene_data -> joint 0x70E8 -> dobj 0x6F98 -> mobj 0x39CC -> tobj 0x395C -> tev 0x393C`
   - Pros:
     - already on the current visible multi-object path
     - real non-null `TObj->tev`
     - uses `GX_TF_I8`, which is a plausible fit for a narrow color-ramp interpretation
     - does not require broad engine boot
2. Base object TEV payload
   - Path: `scene_data -> joint 0x7068 -> dobj 0x5598 -> mobj 0x3A98 -> tobj 0x3A28 -> tev 0x3A08`
   - Pros:
     - same real TEV payload bytes as the sibling object
   - Cons:
     - the currently verified visible path for this object is material-backed, not its textured branch
3. Broader TEV/material expansion
   - Examples:
     - general `TObj->tev` / `TExp` parsing
     - multi-stage TEV state emission
     - broader `GXSetTev*` shader/runtime support
   - Rejected for this step because it is larger than the first payload-specific bridge.

## Chosen Target

Chosen target: the sibling object's real `TObj->tev` payload at `0x393C`.

Why this was the best next step:

- It was already attached to a visible real repository-content-backed object.
- It avoided reopening scene/camera/object translation that was already working.
- The sibling and base payloads are byte-identical, so a successful narrow
  interpretation on the sibling path also proves the base branch is using the
  same TEV content shape.
- The object uses a real `GX_TF_I8` texture. Within the repository context,
  that made a narrow intensity-to-color interpretation the smallest credible
  path to a visible TEV-backed change.

## Repository Evidence Used

Verified facts from the current bridge:

- The sibling `TObj` is real repository content:
  - `tobj = 0x395C`
  - `image = 0x3924`
  - `image data = 0x2CBC0`
  - `format = 1` (`GX_TF_I8`)
- The sibling `TObj->tev` pointer is non-null:
  - `tev = 0x393C`
- The base branch has the same payload shape:
  - `base tev = 0x3A08`
  - payload bytes match the sibling payload

The interpreted payload words were:

```text
00000000 00000101 8580080F 07070707
7FB2E500 19334C00 00000000 40000077
00000000 00000000 00000000 00000004
00000000 00000000 00000000 3F800000
```

The narrow interpretation used here was:

- support exactly this real `0x40`-byte payload signature on `GX_TF_I8`
  textures
- interpret the two packed RGB-like words
  - `0x7FB2E500` -> light endpoint `127,178,229`
  - `0x19334C00` -> dark endpoint `25,51,76`
- bake the `I8` intensity texture to RGBA by linearly interpolating between the
  real dark/light endpoints

Important boundary:

- The repository clearly proves the payload is real and the endpoint colors come
  from real content.
- The exact meaning of this payload as a full general HSD/TEV/TExp structure is
  still inferred, not fully decompiled.
- This step intentionally does **not** claim general TEV support.

## Code Changes

### 1. Real content translation

Files:

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`

Changes:

- extended `PCPortTranslatedTexture` with narrow TEV metadata
  - `hasTevPayload`
  - `hasI8ColorRampTev`
  - `tevRampLight`
  - `tevRampDark`
- added a narrow payload recognizer for the real `0x393C` / `0x3A08` TEV shape
- added `PCPort_BakeTextureRGBAFromArchiveBE(...)`
  - currently only supports the verified `GX_TF_I8` + known TEV ramp path

### 2. Host texture upload support

Files:

- `src/pcport/gx_shim.h`
- `src/pcport/gx_shim.c`

Changes:

- added `GXHostInitTexObjRGBA8(...)`
- this lets the smoke path upload a CPU-interpreted RGBA texture while leaving
  the rest of the bridge unchanged

### 3. Verification smoke

File:

- `src/pcport/pcport_main.c`

Changes:

- added `--real-tev-scene-slice-smoke`
- the smoke:
  1. builds the existing real multi-object scene slice baseline
  2. re-renders the same scene with the sibling object's real TEV payload
     interpreted into an RGBA texture
  3. compares framebuffer output

## Exact Commands Attempted

Build:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

Primary verification:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-smoke
```

Regression subset:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-content-translation-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### Build

Succeeded.

### Primary TEV Smoke

Succeeded.

Observed output:

```text
[pcport_bootstrap] Real TEV scene slice smoke passed
  scene=0x2DCC4
  camera=0x2DC38
  siblingJoint=0x70E8
  siblingDObj=0x6F98
  siblingMObj=0x39CC
  siblingTObj=0x395C
  siblingTev=0x393C
  siblingImage=0x3924
  siblingData=0x2CBC0
  diffPixels=307200
  baselineCenter=86,86,114,164
  tevCenter=80,87,123,164
  light=127,178,229,255
  dark=25,51,76,255
  bakedBytes=16384
  submitted=1309
  expanded=1309
  prim=0x98
```

What this verifies:

- a real `TObj->tev` payload on a real visible repository object is now being
  interpreted, not merely tolerated
- the visible result changes because of that real payload
- the draw still runs through the existing game-owned
  `fn_801AA568 -> fn_800DAD10` render bridge

### Regression Results

All rerun regressions passed:

- `--real-scene-slice-3-smoke`
- `--real-textured-scene-slice-smoke`
- `--real-content-translation-smoke`
- `--real-content-parser-smoke`
- `--gsgfx-visible-smoke`

## What This Does And Does Not Prove

### Proven

- The bridge now supports one real TEV-backed content path, not just texture
  presence plus preset stage-0 routing.
- The interpreted path is still attached to real repository content and the
  existing game-owned draw path.

### Not Proven

- This is **not** general TEV support.
- This does **not** prove that the full `TObj->tev` / `TExp` structure is
  understood.
- This does **not** prove multi-stage TEV, TEV alpha rules, TEV konst/regs, or
  shader-accurate material behavior.
- This still does **not** make the title/menu/game boot path run.

## Exact Blockers Remaining

1. The current TEV bridge is signature-specific.
   - It only recognizes one verified real `0x40` payload shape on `GX_TF_I8`.
2. The payload's full structure is still unresolved.
   - The RGB ramp interpretation is repository-grounded but still inferred.
3. Alpha semantics remain unresolved.
   - The current narrow bridge applies the real RGB endpoints and leaves alpha
     on the existing host/material path.
4. Broader TEV/material behavior is still missing.
   - no general `TObj->tev` parser
   - no multi-stage TEV execution
   - no broader shader/runtime TEV combiner coverage
5. Engine ownership is still missing.
   - no title/menu update flow
   - no engine boot path driving these scene branches

## Next Smallest Milestone

The next smaller step is still broader TEV support, not engine boot:

- identify whether another real visible object under the same `scene_data`
  branch uses a second distinct `TObj->tev` payload shape
- if it does, add exactly one more narrow payload interpretation
- if it does not, the next honest step is exposing the smallest real
  `TObj->tev` / `TExp` translation boundary rather than jumping to engine boot
