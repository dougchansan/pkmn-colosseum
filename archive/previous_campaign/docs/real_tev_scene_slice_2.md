# Real TEV Scene Slice 2

## Goal

Advance from the first narrow real TEV-interpreted scene slice to the next
smallest broader TEV milestone by either:

- interpreting one second distinct real `TObj->tev` payload shape, or
- proving that the smaller next step is a narrow shared `TObj->tev` / `TExp`
  translation boundary.

## Ranked TEV Targets

1. One second distinct real `TObj->tev` payload shape on a reachable visible
   or nearly-visible object.
2. A narrow shared `TObj->tev` translation boundary if all reachable payloads
   collapse to the same shape.
3. Broader TEV/material expansion only if the two smaller targets proved
   larger than expected.

## Selection Evidence

I inspected the currently reachable `scene_data` textured branches under
`topmenu.fsys:menu_bg00` and enumerated the real `TObj->tev` payloads attached
to the visible objects:

- front branch: `joint 0x6FE8 -> mobj 0x3900 -> tobj 0x3890 -> tev 0x0`
- base branch: `joint 0x7068 -> mobj 0x3A98 -> tobj 0x3A28 -> tev 0x3A08`
- sibling branch: `joint 0x70E8 -> mobj 0x39CC -> tobj 0x395C -> tev 0x393C`

Directly verified result:

- there is only **one** unique non-null reachable TEV payload shape in the
  current visible branch set
- `0x3A08` and `0x393C` are byte-identical `0x40`-byte payloads
- the front textured object has no TEV payload at all

That ruled out “second distinct payload shape” as the next honest step.

## Chosen Target

Chosen target: a narrow shared `TObj->tev` translation boundary for the real
I8-ramp TEV family already present on the base and sibling objects.

Why this was the fastest path:

- it is smaller than inventing a second signature that does not exist on the
  current reachable branch
- it keeps the current real repository-content path and game-owned
  `fn_801AA568 -> fn_800DAD10` bridge intact
- it broadens the TEV step from “one object, one signature check” to “two real
  objects using a shared translated TEV payload type”

## What Changed

Files:

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`
- `src/pcport/pcport_main.c`

Changes:

1. Exposed a shared translated TEV payload boundary.
   - Added `PCPortTranslatedTev` and `PCPORT_TRANSLATED_TEV_*`.
   - `PCPort_TranslateTextureFromArchiveBE` now records the full raw 16-word
     serialized TEV payload and classifies it into a host-side TEV kind.
2. Kept the existing narrow interpretation, but moved it behind that boundary.
   - The currently supported kind remains the real `GX_TF_I8` color-ramp TEV
     family already seen at `0x3A08` and `0x393C`.
3. Added a broader verification smoke:
   - `--real-tev-scene-slice-2-smoke`
   - baseline: front textured object + base material-backed object + sibling
     TEV-backed object
   - broader pass: same scene, but with the base object switched onto the same
     shared translated TEV path as the sibling object

## Additional Real TEV Path Interpreted

The new additional real TEV-backed path is:

- `scene_data -> joint 0x7068 -> dobj 0x5598 -> mobj 0x3A98 -> tobj 0x3A28 -> tev 0x3A08`

This now goes through the same translated TEV payload boundary as the already
working sibling path:

- `scene_data -> joint 0x70E8 -> dobj 0x6F98 -> mobj 0x39CC -> tobj 0x395C -> tev 0x393C`

## Exact Commands Attempted

Build:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

Primary verification:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
```

Regression subset:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-content-translation-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-content-parser-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### Build

Succeeded.

### Primary Smoke

Succeeded.

Observed output:

```text
[pcport_bootstrap] Real TEV scene slice 2 smoke passed
  scene=0x2DCC4
  camera=0x2DC38
  baseJoint=0x7068
  baseDObj=0x5598
  baseMObj=0x3A98
  baseTObj=0x3A28
  baseTev=0x3A08
  siblingJoint=0x70E8
  siblingDObj=0x6F98
  siblingMObj=0x39CC
  siblingTObj=0x395C
  siblingTev=0x393C
  diffPixels=307200
  baselineCenter=80,87,123,164
  broaderCenter=46,57,96,164
  baseLight=127,178,229,255
  baseDark=25,51,76,255
  baseBakedBytes=16384
  siblingBakedBytes=16384
  submitted=1309
  expanded=1309
  prim=0x98
```

What this directly verifies:

- the currently reachable visible branch set does not contain a second distinct
  TEV payload shape
- the bridge now uses a shared translated TEV payload boundary for both real
  non-null `TObj->tev` payloads on the visible branch
- the visible result changes again when the base object is switched from the
  material-only route to that real shared TEV-backed route
- the draw still runs through the existing game-owned
  `fn_801AA568 -> fn_800DAD10` render bridge

### Regression Results

All rerun regressions passed:

- `--real-tev-scene-slice-smoke`
- `--real-scene-slice-3-smoke`
- `--real-textured-scene-slice-smoke`
- `--real-content-translation-smoke`
- `--real-content-parser-smoke`
- `--gsgfx-visible-smoke`

## What This Does And Does Not Prove

### Proven

- The visible result now reflects broader real TEV-backed behavior than the
  prior single-object TEV bridge.
- The bridge is no longer hard-wired only to one object path; it can translate
  the shared real TEV payload family and apply it to both reachable real
  objects that use it.

### Not Proven

- This still is **not** general TEV support.
- This still does **not** interpret a second distinct payload shape, because no
  second distinct reachable shape was found in the current branch.
- This still does **not** prove general `TObj->tev` / `TExp` parsing, multi-
  stage TEV execution, TEV alpha semantics, or title/menu boot.

## Exact Blockers Remaining

1. The current branch still exposes only one real non-null TEV payload family.
   - There is no second distinct reachable shape under the current visible
     `scene_data` branch.
2. The shared boundary is still narrow.
   - It records raw payload words and classifies one verified `GX_TF_I8`
     ramp-family kind.
3. General `TObj->tev` / `TExp` structure is still unresolved.
   - The bridge does not yet translate broader TEV node families.
4. TEV alpha semantics remain unresolved.
5. Multi-stage TEV, broader material/lighting behavior, and engine-owned
   title/menu flow are still missing.

## Next Smallest Milestone

The smaller next step is still broader TEV support, not engine boot:

- either find a new real scene branch whose `TObj->tev` resolves to a second
  distinct payload shape
- or expose the next smallest shared `TObj->tev` / `TExp` translation boundary
  beyond the current I8-ramp family
