# Goal

Advance exactly one bounded milestone beyond the prior verified shared textured
parser/translation boundary toward a broader shared real `TObj` / `TExp`
parser layer, while staying strictly below engine boot or update-flow work.

# Why this step was chosen

The current codebase already had a broader shared parsed-node-chain shape on the
real content path, but the currently exercised `texCoordSrc` meaning still was
not shared all the way through that boundary:

- `TranslateTextureFromArchiveCommon(...)` still exposed only the raw
  `texCoordSrc` word on `PCPortTranslatedTexture`
- `PCPort_ParseTextureNodeChainFromArchiveBE(...)` still had to resolve that raw
  word again while walking the chain
- `pcport_main.c` still had its own local raw `texCoordSrc -> coordId` helper for
  textured pipeline setup

So the smallest correct next move was not engine boot, not TEV broadening, and
not another farther family-specific branch. It was to make the already
exercised `texCoordSrc` semantic itself part of the shared translated texture /
parsed-node-chain boundary.

That is a real shared parser-layer improvement because the resolved coordinate
meaning now crosses the shared translation boundary once and is then reused by
both the shared node-chain parser and the downstream textured pipeline setup.

# Exact files changed

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`
- `src/pcport/pcport_main.c`
- `docs/real_scene_slice_11.md`

# Exact functions / boundaries changed

## `src/pcport/real_content_host.h`

- `PCPortTranslatedTexture`
  - added `coordId`
  - added `hasCoordId`

This broadens the translated texture payload so the currently exercised
`texCoordSrc` meaning is preserved as shared translated data instead of being
re-decoded by each consumer.

## `src/pcport/real_content_host.c`

- `TranslateTextureFromArchiveCommon(...)` at `src/pcport/real_content_host.c:1467`
  - now resolves the currently supported raw `texCoordSrc` values during shared
    texture translation and stores the result on `PCPortTranslatedTexture`
- `PCPort_ParseTextureNodeChainFromArchiveBE(...)` at
  `src/pcport/real_content_host.c:260`
  - now reuses `texture.hasCoordId / texture.coordId` from the shared texture
    translation result instead of re-decoding raw `texCoordSrc` locally
- `PCPort_TranslateTextureExpFromArchiveBE(...)` at
  `src/pcport/real_content_host.c:1558`
  - continues to classify stages on top of the shared parsed-node-chain output,
    now backed by textures that already carry shared coord semantics
- `PCPort_TranslateTextureChainFromArchiveBE(...)` at
  `src/pcport/real_content_host.c:1593`
  - continues to build the narrow chain classification on top of the same shared
    parsed-node-chain output

## `src/pcport/pcport_main.c`

- `ConfigureTranslatedTexturedPipeline(...)` at `src/pcport/pcport_main.c:936`
  - now consumes `texture.hasCoordId / texture.coordId` directly instead of
    maintaining a duplicate local raw-source decoder
- the sibling TEV textured upload path at `src/pcport/pcport_main.c:2651`
  - now also consumes the shared resolved coord semantic from
    `translatedSiblingTexture`

# What semantics became shared that were not shared before

Before this slice:

- the currently exercised `texCoordSrc` meanings were only partially shared
- the translated texture object exposed the raw `texCoordSrc` field, but not the
  resolved coord semantic
- the parsed node-chain layer and `pcport_main.c` both had to reinterpret raw
  `texCoordSrc` locally

After this slice:

- the currently exercised `texCoordSrc -> coordId` mapping is now part of the
  shared translated texture output itself
- the shared parsed `TObj` node-chain boundary reuses that resolved semantic
  rather than re-decoding raw fields
- downstream textured pipeline setup reuses the same resolved semantic

This is still intentionally narrow to the already verified coord meanings, but
those meanings are now shared more honestly across the parser/translation path.

# What remained family-specific

Still family-specific or intentionally narrow after this slice:

- stage-kind classification in `PCPort_TranslateTextureExpFromArchiveBE(...)`
  still only recognizes the currently exercised families
  - `I8` ramp sample
  - `I8` mask modulate
  - no-TEV direct sample
- TEV interpretation is still narrow / family-specific
- TLUT / CI paths are still unsupported
- broader lighting/material behavior is still not generalized
- engine-owned title/menu/game boot or update-flow work is still entirely out of
  scope

# Build and verification commands run

## Build

```powershell
cmd /c 'call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" && cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap'
```

## Required smoke tests

```powershell
build_pcbootstrap\pcport_bootstrap.exe --real-textured-scene-slice-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-scene-slice-4-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-3-smoke
build_pcbootstrap\pcport_bootstrap.exe --real-tev-scene-slice-2-smoke
build_pcbootstrap\pcport_bootstrap.exe --gsgfx-visible-smoke
```

# Exact observed pass/fail results

All required commands passed.

## Observed build result

- `cmake --build build_pcbootstrap --config Debug --target pcport_bootstrap`
  - passed (`ninja: no work to do.` after the successful rebuild)

## Observed smoke results

- `--real-textured-scene-slice-smoke`
  - passed
  - `stage0=3 stages=1 baked=1228800`
- `--real-scene-slice-3-smoke`
  - passed
- `--real-scene-slice-4-smoke`
  - passed
  - `rawSrc0=4 coord0=0 rawSrc1=5 coord1=1`
- `--real-tev-scene-slice-3-smoke`
  - passed
  - `stage0=1 stages=1`
- `--real-tev-scene-slice-2-smoke`
  - passed
- `--gsgfx-visible-smoke`
  - passed

# Remaining blockers after this change

1. There is still no general shared `TObj` / `TExp` parser.
2. Shared expression classification still only understands the currently
   exercised families.
3. `texCoordSrc` support is still narrow beyond the currently verified meanings.
4. TEV interpretation is still family-specific.
5. TLUT / CI and broader material-lighting behavior are still missing.
6. Engine-owned title/menu/game boot and update-flow work are still out of
   scope and untouched.

# Why engine boot is still not the next step

Engine boot is still larger and less direct than the remaining shared
parser/translation debt that is already on the current working path.

The next useful milestones are still inside the shared content bridge:

- broader shared `TObj` / `TExp` parsing
- broader shared expression-classification support
- `texCoordSrc` broadening only when a newly exercised shared semantic requires
  it
- TEV broadening only when directly forced by the next parser milestone

So this slice keeps the project on the smaller, verifiable shared parser path
instead of jumping into engine lifecycle integration.
