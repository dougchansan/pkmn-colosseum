# Goal

Advance exactly one bounded milestone beyond slice 11 toward a broader shared
real `TObj` / `TExp` parser/translation layer, while staying strictly below
engine boot or update-flow work.

## Why this step was chosen

Slice 11 made the parsed linked-`TObj` walk and resolved `texCoordSrc` meaning
shared, but the currently exercised expression families were still not exposed
as one canonical shared whole-expression result.

The remaining gap was that the real-content path still had multiple reasoning
surfaces for the same exercised families:

- per-stage `TExp` stage kinds
- narrow legacy `TextureChain.kind`
- smoke expectations that still primarily thought in stage tuples

The smallest correct next step was therefore not another farther family-specific
branch and not TEV/boot work. It was to promote the already exercised whole
expression families into one explicit shared `TExp` classification contract and
make downstream verification consume that contract.

That is a real shared parser/translation milestone because the linked parsed
node chain is now classified once into a whole-expression family that can be
reused by both translators and by the current verified smokes.

## Exact files changed

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`
- `src/pcport/pcport_main.c`
- `docs/real_scene_slice_12.md`

## Exact functions / boundaries changed

### `src/pcport/real_content_host.h`

Added a shared whole-expression family enum and carried it on the translated
`TExp` result:

- `PCPortTranslatedTextureExpKind`
  - `PCPORT_TEXTURE_EXP_KIND_NONE`
  - `PCPORT_TEXTURE_EXP_KIND_I8_RAMP`
  - `PCPORT_TEXTURE_EXP_KIND_I8_RAMP_MASK`
  - `PCPORT_TEXTURE_EXP_KIND_DIRECT_SAMPLE`
- `PCPortTranslatedTextureExp.kind`

This broadens the shared translated `TExp` boundary from “a sequence of stage
nodes” to “a sequence of stage nodes plus one classified shared family meaning
for the exercised cases.”

### `src/pcport/real_content_host.c`

#### `ClassifyTextureExpKindFromParsedChain(...)`

New shared whole-expression classifier built directly on top of the parsed node
chain. It recognizes exactly the currently exercised families:

- one-node direct sample
- one-node `I8` ramp
- two-node `I8` ramp + mask

#### `PCPort_TranslateTextureExpFromArchiveBE(...)`

Now sets `outExp->kind` from `ClassifyTextureExpKindFromParsedChain(...)`
before filling the per-stage details.

This makes the translated `TExp` output itself carry a whole-expression family,
not just stage-by-stage tags.

This is additive metadata only: if a parseable chain falls outside the
currently exercised shared families, stage translation still remains the
authoritative boundary and `outExp->kind` simply stays
`PCPORT_TEXTURE_EXP_KIND_NONE`.

#### `PCPort_TranslateTextureChainFromArchiveBE(...)`

Now reuses the same shared whole-expression classifier when mapping back to the
older narrow chain kinds:

- `PCPORT_TEXTURE_CHAIN_I8_RAMP`
- `PCPORT_TEXTURE_CHAIN_I8_RAMP_MASK`

So the legacy chain wrapper now sits on top of the same shared whole-expression
classification boundary instead of keeping separate ad-hoc family recognition.

### `src/pcport/pcport_main.c`

The current smokes now assert the shared whole-expression family directly on the
translated `TExp` result:

- `RunRealTexturedSceneSliceSmoke(...)`
  - expects `PCPORT_TEXTURE_EXP_KIND_DIRECT_SAMPLE`
- `RunRealTevSceneSlice3Smoke(...)`
  - expects `PCPORT_TEXTURE_EXP_KIND_I8_RAMP`
- `RunRealSceneSlice4Smoke(...)`
  - expects object 0 `PCPORT_TEXTURE_EXP_KIND_I8_RAMP_MASK`
  - expects object 1 `PCPORT_TEXTURE_EXP_KIND_I8_RAMP`

The success/error output now prints those shared `kind=` values so the current
coverage verifies the broader shared expression-classification boundary instead
of only stage tuples.

## What semantics became shared that were not shared before

Before this slice:

- the shared path could parse linked `TObj` nodes and tag individual stages
- but there was still no canonical shared whole-expression family carried on the
  translated `TExp` output
- the exercised families were still reasoned about through separate stage-kind
  and legacy chain-kind surfaces

After this slice:

- the translated `TExp` result carries one shared whole-expression family for
  the currently exercised cases
- the parsed node chain is classified once into:
  - direct sample
  - `I8` ramp
  - `I8` ramp + mask
- the legacy narrow chain translator reuses that same family result
- the current smokes verify those shared family meanings directly

That is the new bounded shared semantic step beyond slice 11.

## What remained family-specific

Still family-specific or intentionally narrow after this slice:

- per-stage `ClassifyTextureExpStageKind(...)` details still exist for the
  currently exercised families
- the shared classifier still only understands the currently exercised
  expression families
- TEV interpretation remains narrow / family-specific
- TLUT / CI paths are still unsupported
- broader material-lighting behavior is still missing
- engine-owned title/menu/game boot and update flow remain fully out of scope

## Build and verification commands run

### Build

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

### Required smoke tests

```powershell
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\Debug\pcport_bootstrap.exe --gsgfx-visible-smoke
```

## Exact observed results

All required commands passed.

### Build

- `cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap`
  - passed

### Smoke results

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

1. Shared whole-expression classification still only covers the currently
   exercised families.
2. A broader general shared `TObj` / `TExp` parser/classifier for additional
   real families still does not exist.
3. `texCoordSrc` support is still narrow beyond the currently verified meanings.
4. TEV interpretation is still family-specific beyond the current ramp cases.
5. TLUT / CI and broader material-lighting behavior are still missing.
6. Engine-owned title/menu/game boot and update-flow work remain out of scope
   and untouched.

## Why engine boot is still not the next step

Engine boot is still larger and less direct than the remaining shared
parser/translation debt on the current working path.

The next useful steps are still inside the same shared content bridge:

- broader shared whole-expression classification for the next exercised family
- broader shared `TObj` / `TExp` parsing when a new family requires it
- `texCoordSrc` broadening only if directly forced by that next shared family
- TEV broadening only if directly forced by that parser/classifier milestone

So engine boot is still not the next step because the smaller, better-verified
shared parser/translation work has not been exhausted yet.

## Git / checkpoint note

The workspace is still dirty outside this slice, so this slice should not force
a commit unless the tree is cleaned first. The correct behavior here is to stop
with the verified slice documented instead of making a risky mixed commit.
