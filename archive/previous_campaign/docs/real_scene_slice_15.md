# Goal

Attempt to advance exactly one bounded milestone beyond slice 14 toward a broader
shared real `TObj` / `TExp` parser/translation/classification layer, while
staying strictly below engine boot or update-flow work.

## Outcome

Stopped with a precise blocker report.

I did **not** implement a new slice-15 parser/classification milestone because I
could not identify a clean new shared family or a smaller parser/classifier
extension that directly enables one without widening into broader TEV,
coord-source, or TLUT/CI work.

## Why this stop was chosen

Slice 14 already removed the smallest remaining duplicated classification work
inside the current shared family cluster. After re-checking the current shared
surfaces, the project still only recognizes one narrow family cluster:

### Shared stage kinds still limited to:

- direct sample
- `I8` ramp sample
- `I8` mask modulate

### Shared whole-expression kinds still limited to:

- direct sample
- `I8` ramp
- `I8` ramp + mask

The next possible families are blocked by at least one larger cross-cutting
surface that is still intentionally narrow:

- TEV signature interpretation
- `texCoordSrc` resolution
- TLUT / CI acceptance

Because none of those can be widened safely without concrete evidence for one
specific next reachable family, forcing another “shared-layer improvement” here
would be speculative scope growth, not the smallest successful milestone.

That violates the task’s decision rule, so the correct slice-15 result is a
verified blocker report.

## Exact files changed

- `docs/real_scene_slice_15.md`

No code files were changed for slice 15.

## Exact functions / boundaries changed

None.

The blocker is that the current boundaries are already as far as they can be
pushed without first identifying one concrete next reachable family.

Relevant unchanged boundaries that remain limiting:

- `TranslateTextureTevPayload(...)`
- `ResolveTextureCoordIdFromRawSrc(...)`
- `TranslateTextureFromArchiveCommon(...)`
- `ClassifyTextureExpStageKind(...)`
- `ClassifyTextureExpKindFromParsedChain(...)`

## What new family or enabling semantic became shared

None in slice 15.

This slice intentionally stops before speculative widening.

## What remained narrow or family-specific

Still narrow / limiting after slice 14 and unchanged in slice 15:

- shared stage-kind recognition still only covers the current three stage kinds
- shared whole-expression recognition still only covers the current three
  whole-expression families
- TEV parsing still only recognizes the current narrow `I8` ramp signature
- `texCoordSrc` resolution is still limited to the currently verified meanings
- TLUT / CI-backed textures are still rejected during translation
- downstream bake paths are still limited to the currently exercised family set
- engine-owned title/menu/game boot and update-flow work remain out of scope

## Exact verification commands run

### Build

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

### Required smokes

```powershell
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Exact observed results

All verification commands passed.

### Build

- `cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap`
  - passed
  - observed: `ninja: no work to do.`

### Smokes

- `--real-textured-scene-slice-smoke`
  - passed
  - observed: `kind=3 stage0=3 stages=1 baked=1228800`
- `--real-scene-slice-3-smoke`
  - passed
- `--real-scene-slice-4-smoke`
  - passed
  - observed: `kind0=2 stage00=1 stage01=2 stages0=2 kind1=1 stage10=1 stages1=1 rawSrc0=4 coord0=0 rawSrc1=5 coord1=1`
- `--real-tev-scene-slice-3-smoke`
  - passed
  - observed: `kind=1 stage0=1 stages=1`
- `--real-tev-scene-slice-2-smoke`
  - passed
- `--gsgfx-visible-smoke`
  - passed

## Remaining blockers

1. No new shared family is currently reachable with a small, verified delta from
   the current parser/classifier surfaces alone.
2. A new family would require at least one broader enabling surface, likely one
   of:
   - a new TEV signature meaning
   - a new `texCoordSrc` meaning
   - TLUT / CI acceptance
3. Those surfaces are cross-cutting enough that widening them without a concrete
   observed next family would violate the “smallest bounded milestone” rule.
4. The next safe progress step must therefore be evidence-led: identify one
   concrete next family from scene/archive data first, then widen only the one
   forced surface.

## Why engine boot is still not the next step

Engine boot is still larger and less direct than the unresolved shared
parser/classifier blocker.

The correct next progress step is still inside the shared real content bridge:

- identify the next concrete reachable family
- widen only the single forced shared surface for that family
- then land that family as one bounded shared-layer slice

Until that concrete family is identified, jumping to engine boot or lifecycle
integration would skip over the actual rendering-content blocker and widen scope
prematurely.

## Git / checkpoint note

The workspace remains dirty outside this slice, so there is no safe slice-15
commit to make here.

The correct stop is a documented blocker, not a speculative code change or a
mixed commit.
