## Goal

Advance from the current shared narrow `TObj` / `TExp` stage layer to the next
smallest broader shared texture-expression milestone by unlocking exactly one
additional reachable stage family only if it exposes a new shared semantic;
otherwise move up to the next parser layer.

## Reachable-Branch Check

I first checked the nearby reachable `scene_data` roots already exercised by the
native bridge:

- `topmenu.fsys:menu_bg00`
- `pda_menu.fsys:pda2_bg`

The scan walked each reachable joint tree and enumerated the `DObj -> MObj ->
TObj` texture branches under those roots.

What that scan found:

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
    - two-stage I8 ramp + mask chain

So the reachable nearby branches only exposed the three families already on the
bridge:

- one-stage I8 ramp sample
- two-stage I8 ramp + mask
- one-stage no-TEV CMPR direct sample

No fourth nearby family showed up cheaply enough to justify another
family-specific extension.

## Cost Comparison

### Option 1: add one more nearby family

Why it lost:

- the nearby reachable branches collapsed to the three already-integrated
  families
- adding another family would have meant going farther afield instead of
  reducing the remaining structural debt at the shared texture-expression layer

### Option 2: move up to the next parser / translation layer

Why it won:

- there was no cheap fourth family in the currently reachable branches
- one existing rendered branch still bypassed the broader shared expression
  baker even though it already used the shared stage translator
- lifting that branch onto the shared bake path reduced branch-specific texture
  semantics further without jumping to engine boot

### Option 3: engine boot

Why it was still rejected:

- still larger and less direct than crossing the next shared parser /
  translation layer already exposed by the working render path

## Chosen Path

I chose option 2: the next parser / translation layer.

Concretely, I lifted the already rendered front textured branch off its raw
`GXInitTexObj` bypass and onto the shared texture-expression bake path.

That branch is:

- `topmenu.fsys:menu_bg00`
- `joint 0x6FE8`
- `dobj 0x3B98`
- `mobj 0x3900`
- `tobj 0x3890`
- `image 0x3878`
- `pobj 0x3B80`

## Parser-Layer Work Implemented

### 1. Shared texture decode helper for stage-0 direct samples

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1684):
  added `DecodeTextureToRGBA(...)`

This uses the existing repository host decoder layer (`gx_texture_decode`) to
turn raw archive texture data into RGBA8 for the shared expression baker.

### 2. Broadened shared texture baking

- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1768):
  broadened `PCPort_BakeTextureRGBAFromArchiveBE(...)`
- [real_content_host.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\real_content_host.c#L1819):
  broadened `PCPort_BakeTextureExpRGBAFromArchiveBE(...)`

Before this step:

- the shared expression baker only accepted stage-0 I8 ramp samples
- the front CMPR direct-sample branch still bypassed that layer

After this step:

- stage-0 direct samples with no serialized TEV payload can also flow through
  the shared expression baker
- the existing stage-1 mask-modulate path remains unchanged

### 3. Existing rendered branch moved onto the broader shared layer

- [pcport_main.c](C:\Users\douglaswhittingham\pkmn-colosseum\src\pcport\pcport_main.c#L1507):
  `RunRealTexturedSceneSliceSmoke`

That smoke now:

- translates the front branch through `PCPort_TranslateTextureExpFromArchiveBE`
- verifies `stage0 == PCPORT_TEXP_STAGE_DIRECT_SAMPLE`
- bakes it through `PCPort_BakeTextureExpRGBAFromArchiveBE`
- uploads the baked RGBA via `GXHostInitTexObjRGBA8`

So the front branch no longer relies on a raw archive texture upload path.

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
expression bake layer:

```text
[pcport_bootstrap] Real textured scene slice smoke passed (... texTObj=0x3890 ... stage0=3 stages=1 diffPixels=307200 ... format=14 baked=1228800 submitted=4 expanded=4 prim=0x98)
```

That directly proves:

- there was no cheaper fourth nearby family in the currently reachable scene
  roots
- an existing rendered branch now uses a broader shared parser / translation
  layer than before
- the game-owned `fn_801AA568 -> fn_800DAD10` draw bridge is unchanged

### Whether the result is structurally cleaner or visually richer

Structurally cleaner.

The visible result stayed working, but the meaningful improvement is that the
front textured branch no longer bypasses the shared expression baker.

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

- the nearby reachable branches only exposed the three already-integrated
  families
- the front direct-sample branch now uses the shared expression bake path
- the rendered result stayed correct
- the regression set stayed green

### Still intentionally narrow

- this is still not a general `TObj` / `TExp` parser
- the broader bake path now covers the current direct-sample stage-0 case, but
  only for the actually exercised no-TEV texture shape

## Exact Blockers Still Remaining

1. There is still no general shared `TObj` / `TExp` parser.
2. The shared expression baker still only understands the currently exercised
   stage families.
3. `texCoordSrc` support is still narrow to the meanings already exercised.
4. TEV interpretation is still family-specific.
5. TLUT / CI paths, broader material-lighting behavior, and engine-owned
   title/menu boot or update flow are still outside the current bridge.

## Conclusion

No fourth nearby family was reachable cheaply enough to justify another
family-specific step.

So the smaller next move was the next parser / translation layer, not engine
boot, and that step succeeded: the front textured branch now uses the broader
shared expression bake layer instead of a raw texture upload bypass.

## Next Smallest Milestone

The next smaller step is still not engine boot.

It is either:

- one more nearby branch only if it exposes a genuinely new stage semantic
  beyond the three already on the bridge
- or the next broader real `TObj` / `TExp` parser layer above the current
  narrow shared stage and bake semantics
