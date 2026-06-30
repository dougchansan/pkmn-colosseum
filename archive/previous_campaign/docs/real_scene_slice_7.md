## Goal

Advance from the current fixed-size shared `TObj` chain bridge to the next
smallest real shared `TObj` / `TExp` translation boundary that reduces
branch-specific texture semantics further, without jumping to engine boot.

## Cost Comparison

### Option 1: one more object-specific chained-`TObj` family

Why it was considered:

- it could expand visible coverage quickly if a nearby branch exposed another
  cheap texture composition variant

Why it lost:

- it would still leave the bridge expressed as branch-specific chain kinds
- it would not reduce the current structural debt at the `TObj` / `TExp`
  boundary
- the already rendered branches were enough to expose the next shared
  abstraction directly

### Option 2: a narrow shared `TObj` / `TExp` stage boundary

Why it won:

- the currently rendered branches already expose a common per-stage structure:
  - one-node I8 ramp sample
  - two-node I8 ramp sample + mask modulate
- that made a per-stage translated boundary smaller than finding and
  validating another object-specific family
- it directly replaces the last branch-specific texture composition helper in
  the bootstrap path

### Option 3: engine boot

Why it was still rejected:

- larger and less direct than crossing the already-reachable texture-object
  abstraction layer

## Chosen Path

I chose option 2: a narrow shared `TObj` / `TExp` stage boundary.

The important distinction from the previous step is that the bridge is no
longer centered on fixed chain families such as "one-node ramp" or "two-node
ramp + mask". It is now centered on per-stage translated semantics:

- `PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE`
- `PCPORT_TEXP_STAGE_I8_MASK_MODULATE`

That is still narrow and repository-specific, but it is a real shared boundary
above the current fixed-size chain classifier.

## Shared Work Implemented

### 1. Shared translated `TObj` / `TExp` metadata

- `src/pcport/real_content_host.h`
  - added `PCPortTranslatedTextureExpStageKind`
  - added `PCPortTranslatedTextureExpStage`
  - added `PCPortTranslatedTextureExp`

This boundary is intentionally small:

- up to 4 stages
- only the stage families actually reached by the current rendered branches

### 2. Shared stage translation

- `src/pcport/real_content_host.c`
  - added `ClassifyTextureExpStageKind(...)`
  - added `PCPort_TranslateTextureExpFromArchiveBE(...)`

This translator walks the linked real `TObj` list, resolves the raw
`texCoordSrc` values already proven on the current branches, and classifies
each node into a narrow per-stage semantic kind.

Currently verified stage kinds:

- `PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE`
- `PCPORT_TEXP_STAGE_I8_MASK_MODULATE`

### 3. Shared stage baking

- `src/pcport/real_content_host.c`
  - added `PCPort_BakeTextureExpRGBAFromArchiveBE(...)`

This replaces the last object-specific texture composition logic in the smoke
path:

- stage 0 must currently be an I8 ramp sample
- later stages can currently apply the verified I8 mask modulation family

### 4. Existing rendered branches switched to the shared boundary

- `src/pcport/pcport_main.c`
  - `RunRealTevSceneSlice3Smoke` now uses
    `PCPort_TranslateTextureExpFromArchiveBE(...)` and
    `PCPort_BakeTextureExpRGBAFromArchiveBE(...)`
  - `RunRealSceneSlice4Smoke` now uses the same shared calls for both the
    one-node and two-node rendered branches

That is the critical structural result of this step: the object-0 branch no
longer relies on its old branch-specific chain bake path.

## Exact Steps Attempted

1. Verified the broader shared one-node stage boundary:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
```

2. Verified the broader shared two-node stage boundary:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
```

3. Re-ran existing regressions:

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Results

### Shared-boundary verification

The one-node rendered branch now reports shared per-stage metadata:

```text
[pcport_bootstrap] Real TEV scene slice 3 smoke passed (... obj1TObj=0x1A7C stage0=1 stages=1 obj1Tev=0x1A5C diffPixels=307200 baselineCenter=0,0,0,59 tevCenter=14,24,7,59 ...)
```

The two-node rendered branch now reports shared per-stage metadata instead of
only a fixed family kind:

```text
[pcport_bootstrap] Real scene slice 4 smoke passed (... obj0TObj0=0x19BC obj0TObj1=0x1918 stage00=1 stage01=2 stages0=2 stage10=1 stages1=1 obj0Tev0=0x199C obj0Tev1=0x18F8 rawSrc0=4 coord0=0 rawSrc1=5 coord1=1 ...)
```

That directly verifies:

- an existing rendered one-node branch now uses the broader shared `TObj` /
  `TExp` boundary
- an existing rendered two-node branch now uses the same broader shared
  boundary
- the game-owned `fn_801AA568 -> fn_800DAD10` render path remains the path
  that reaches the framebuffer

### Whether the result is visually richer or structurally cleaner

Structurally cleaner.

I did not claim a richer visible scene here. The visible outputs stayed
working, but the point of this step was to move texture semantics out of
branch-specific baking and into a shared translated stage layer.

### Regression status

These all still passed:

- `--real-tev-scene-slice-3-smoke`
- `--real-scene-slice-4-smoke`
- `--real-tev-scene-slice-2-smoke`
- `--real-scene-slice-3-smoke`
- `--real-textured-scene-slice-smoke`
- `--gsgfx-visible-smoke`

## Directly Verified Versus Inferred

### Directly verified

- the current one-node rendered branch uses shared stage kind `1`
- the current two-node rendered branch uses shared stage kinds `1` and `2`
- both rendered branches still reach the framebuffer through the existing
  game-owned bridge
- regressions stayed green

### Still inferred or intentionally narrow

- this is not yet a general `TObj` / `TExp` interpreter
- the current stage classifier is still limited to the reachable real families
- `texCoordSrc` support is still only as broad as the values exercised so far

## Exact Blockers Still Remaining

1. There is still no general chained `TObj` support beyond the current narrow
   staged families.
2. There is still no real shared general `TObj` / `TExp` parser.
3. `texCoordSrc` support is still narrow to the meanings exercised so far.
4. TEV interpretation is still family-specific.
5. TLUT / CI paths, broader material and lighting behavior, and engine-owned
   title/menu boot or update flow are still outside the current bridge.

## Conclusion

The smaller next step was the shared `TObj` / `TExp` boundary, not another
object-specific family and not engine boot.

That step succeeded: at least one existing rendered branch, and in practice
both current textured rendered branches, now use a broader shared translated
texture-expression boundary than the previous fixed-size chain bridge.

## Next Smallest Milestone

The next smaller step is still not engine boot.

It is either:

- one more nearby real stage family that fits under the same translated
  `TObj` / `TExp` boundary, if reachable cheaply
- or the next shared layer above this one: a broader real `TObj` / `TExp`
  parser that can describe more than the currently verified staged families
