## Goal

Advance from the current shared narrow `TObj` / `TExp` bake layer to the next
smallest broader real `TObj` / `TExp` parser layer that reduces
family-specific logic further, without jumping to engine boot.

## Reachable-Family Check

I re-checked the nearby real scene roots already exercised by the native bridge:

- `topmenu.fsys:menu_bg00`
- `pda_menu.fsys:pda2_bg`

The currently reachable textured branches under those roots still collapse to
the same three families already on the bridge:

- one-stage I8 ramp sample
- two-stage I8 ramp + mask
- one-stage no-TEV CMPR direct sample

Specifically:

- `topmenu.fsys:menu_bg00`
  - `joint 0x6FE8 -> tobj 0x3890`
    - one-stage no-TEV CMPR direct sample
  - `joint 0x7068 -> tobj 0x3A28`
    - one-stage I8 ramp sample
  - `joint 0x70E8 -> tobj 0x395C`
    - one-stage I8 ramp sample
- `pda_menu.fsys:pda2_bg`
  - `joint 0x2328 -> tobj 0x1A7C`
    - one-stage I8 ramp sample
  - `joint 0x22E8 -> tobj 0x19BC -> next 0x1918`
    - two-stage I8 ramp + mask

So there was still no cheap fourth family nearby that added a genuinely new
shared semantic.

## Cost Comparison

### Option 1: another farther family-specific branch

Why it lost:

- the already reachable roots still only expose the three current families
- going farther afield would add search surface without reducing the remaining
  structural debt at the shared texture-expression layer

### Option 2: the next broader parser / translation layer

Why it won:

- no cheap fourth family existed nearby
- one existing rendered branch still bypassed the broader shared expression
  baker even though it already used the shared stage translator
- lifting that branch onto the shared bake path reduces family-specific logic
  without broadening into engine boot

### Option 3: engine boot

Why it was still rejected:

- still larger and less direct than crossing the next shared parser /
  translation layer already exposed by the working render path

## Chosen Path

I chose option 2: the next broader parser / translation layer.

Concretely, I lifted the already rendered front textured branch off its raw
archive-texture upload bypass and onto the shared texture-expression bake path.

That branch is:

- `topmenu.fsys:menu_bg00`
- `joint 0x6FE8`
- `dobj 0x3B98`
- `mobj 0x3900`
- `tobj 0x3890`
- `image 0x3878`
- `pobj 0x3B80`

## Parser / Translation Work Implemented

### 1. Shared archive texture decode helper

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1684):
  added `DecodeTextureToRGBA(...)`

This uses the existing host-side texture decoder to turn raw archive texture
data into RGBA8 for the shared expression baker instead of relying on a branch-
specific raw upload path.

### 2. Broadened shared stage-0 bake support

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1768):
  broadened `PCPort_BakeTextureRGBAFromArchiveBE(...)`
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1819):
  broadened `PCPort_BakeTextureExpRGBAFromArchiveBE(...)`

Before this step:

- the shared expression baker only handled the already translated ramp-style
  cases cleanly
- the front direct-sample branch still used a raw archive-texture upload bypass

After this step:

- the shared bake path can also handle the already translated no-TEV direct
  sample case through the same broader parser boundary
- existing I8 ramp and ramp+mask behavior stays intact

### 3. Existing rendered branch moved onto the broader shared boundary

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L1507):
  `RunRealTexturedSceneSliceSmoke`

That smoke now:

- translates the front branch through the shared `TObj` / `TExp` boundary
- bakes it through `PCPort_BakeTextureExpRGBAFromArchiveBE(...)`
- uploads the baked RGBA texture via the host helper instead of the raw
  archive-backed `GXInitTexObj(...)` path

So the front branch no longer bypasses the shared expression baker.

## Exact Steps Attempted

1. Rebuilt:

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

2. Verified the lifted front-branch path:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
```

3. Re-ran current rendered regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### What was directly verified

The front textured branch still renders, but now through the broader shared
expression bake path:

```text
[pcport_bootstrap] Real textured scene slice smoke passed (scene=0x2DCC4 camera=0x2DC38 baseJoint=0x7068 baseDObj=0x5598 baseMObj=0x3A98 basePObj=0x5580 texJoint=0x6FE8 texDObj=0x3B98 texMObj=0x3900 texTObj=0x3890 texImage=0x3878 texData=0x73C0 texPObj=0x3B80 stage0=3 stages=1 diffPixels=307200 baseOutside=87,202,119,201 texturedOutside=126,131,178,201 center=96,96,137,201 tev=0 size=640x480 format=14 baked=1228800 submitted=4 expanded=4 prim=0x98)
```

That directly proves:

- there was no cheaper nearby fourth family with a new semantic
- an existing rendered branch now uses a broader real parser / translation
  boundary than the previous narrow family-specific route
- the game-owned `fn_801AA568 -> fn_800DAD10` draw bridge remains unchanged

### Whether the result is structurally cleaner or visually richer

Structurally cleaner.

The visible result did not become richer in this step. The improvement is that
the front textured branch no longer bypasses the shared bake layer.

### Regression status

These all still passed:

- `--real-textured-scene-slice-smoke`
- `--real-scene-slice-3-smoke`
- `--real-scene-slice-4-smoke`
- `--real-tev-scene-slice-3-smoke`
- `--real-tev-scene-slice-2-smoke`
- `--gsgfx-visible-smoke`

## Directly Verified Versus Inferred

### Directly verified

- the nearby reachable textured branches still expose only the three current
  shared families
- the front direct-sample branch now uses the broader shared bake path
- the rendered result stayed correct
- the current regression set stayed green

### Still intentionally narrow

- this is still not a general `TObj` / `TExp` parser
- the broader bake path now covers the already exercised direct-sample case,
  but only for the verified reachable no-TEV family

## Exact Blockers Still Remaining

1. There is still no general shared `TObj` / `TExp` parser.
2. The shared expression baker still only understands the currently exercised
   families.
3. `texCoordSrc` support is still narrow to the meanings already exercised.
4. TEV interpretation is still family-specific.
5. TLUT / CI paths, broader material-lighting behavior, and engine-owned
   title/menu boot or update flow are still outside the current bridge.

## Conclusion

Because there was no cheap fourth nearby family with a new shared semantic, the
smaller next step was the broader parser / translation layer, not another
family-specific branch and not engine boot.

That step succeeded: the front textured branch now uses the broader shared
expression bake layer instead of a raw archive-texture upload bypass.

## Next Smallest Milestone

The next smaller step is still not engine boot.

It is either:

- the next broader real `TObj` / `TExp` parser layer above the current shared
  families
- or one farther branch only if it exposes a genuinely new semantic that is
  cheaper than broadening that parser layer
