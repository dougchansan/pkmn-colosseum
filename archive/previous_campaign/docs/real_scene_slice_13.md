# Goal

Advance exactly one bounded milestone beyond slice 12 toward a broader shared
real `TObj` / `TExp` parser/translation layer, while staying strictly below
engine boot or update-flow work.

## Why this step was chosen

Slice 12 introduced a canonical shared whole-expression `TExp` kind for the
currently exercised families, but that family reasoning still was not attached
to the shared parsed-node-chain boundary itself.

There was still one small duplicated reasoning surface left:

- `PCPort_ParseTextureNodeChainFromArchiveBE(...)` produced only structural
  parsed nodes and coord ids
- `PCPort_TranslateTextureExpFromArchiveBE(...)` then classified the parsed
  chain into a whole-expression kind
- `PCPort_TranslateTextureChainFromArchiveBE(...)` classified that same parsed
  chain again before mapping back to legacy chain kinds

So the smallest next shared-layer milestone was not a farther family-specific
branch and not broader TEV work. It was to make the parsed node-chain boundary
itself carry the shared whole-expression kind so downstream translators could
reuse already classified parser output directly.

That is a clean shared parser/classification boundary improvement because the
linked real `TObj` parse step now returns both:

- structural parsed-node data
- the currently exercised shared family classification

## Exact files changed

- `src/pcport/real_content_host.h`
- `src/pcport/real_content_host.c`
- `docs/real_scene_slice_13.md`

## Exact functions / boundaries changed

### `src/pcport/real_content_host.h`

Broadened the parsed-chain boundary:

- `PCPortParsedTextureNodeChain`
  - added `kind`

This means the shared parsed `TObj` chain result now carries additive
whole-expression family metadata, not just nodeCount / coordIds / node payloads.

### `src/pcport/real_content_host.c`

#### `PCPort_ParseTextureNodeChainFromArchiveBE(...)`

Now sets `outChain->kind` after the linked `TObj` chain has been parsed.

That makes the parsed-chain boundary itself responsible for returning the
currently exercised shared whole-expression family meaning.

#### `PCPort_TranslateTextureExpFromArchiveBE(...)`

Now reuses `parsedChain.kind` directly instead of re-classifying the parsed
chain locally.

#### `PCPort_TranslateTextureChainFromArchiveBE(...)`

Now also reuses `parsedChain.kind` directly instead of re-classifying the parsed
chain a second time before mapping to the legacy narrow chain kinds.

## What semantics became shared that were not shared before

Before this slice:

- the shared parser returned parsed nodes and resolved coord meanings
- the shared whole-expression family existed, but only after an extra
  translation-layer classification step
- both the `TExp` translator and the legacy chain translator still had to apply
  whole-expression family reasoning on top of the parsed result

After this slice:

- the shared parsed-node-chain boundary itself carries the currently exercised
  whole-expression family as additive metadata
- downstream translators reuse that classified parser result directly
- the remaining duplicated whole-expression family reasoning between the two
  translators is removed

This is the bounded shared-layer milestone for slice 13.

## What remained narrow or family-specific

Still narrow or family-specific after this slice:

- the parsed-chain `kind` still only recognizes the currently exercised
  families:
  - direct sample
  - `I8` ramp
  - `I8` ramp + mask
- stage translation remains authoritative and still carries per-stage kinds
- TEV interpretation remains narrow / family-specific
- `texCoordSrc` support remains limited to currently verified meanings
- TLUT / CI and broader material-lighting behavior are still unsupported
- engine-owned title/menu/game boot and update-flow work remain entirely out of
  scope

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

1. The parsed-chain shared `kind` still only covers the currently exercised
   families.
2. No new shared whole-expression family has been added yet beyond the current
   direct sample / `I8` ramp / `I8` ramp+mask set.
3. Stage translation still carries family-specific per-stage reasoning.
4. `texCoordSrc` support remains narrow beyond the current verified meanings.
5. TEV interpretation, TLUT / CI, and broader material-lighting behavior remain
   narrow / unsupported.
6. Engine-owned title/menu/game boot and update-flow work remain out of scope
   and untouched.

## Why engine boot is still not the next step

Engine boot is still larger and less direct than the remaining shared
parser/classification debt on the current working path.

The next useful steps are still inside the shared real content bridge:

- broaden parsed-chain/shared classification to the next reachable family
- broaden shared `TObj` / `TExp` parsing only when that new family forces it
- widen `texCoordSrc` or TEV interpretation only when directly required by that
  next shared family milestone

So engine boot is still not the next step because the smaller, verifiable
shared parser/classification path is not exhausted yet.

## Git / checkpoint note

The workspace is still dirty outside this slice, so this slice should not force
a commit unless the tree is cleaned first. A verified documented stop is safer
than a mixed commit.
