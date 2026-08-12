# MWCC intelligence pipeline

This optional, local-only workflow connects the existing Pokémon Colosseum
build to an external `mwcc-decomp` checkout. It never changes ordinary builds
or public CI, never imports or vendors the external implementation, and keeps
all generated commands, captures, objects, logs, and reports under
`build/mwcc_intel/`.

The external project currently has no confirmed project-wide license. Treat it
as an executable analysis tool: configure its root with `--mwcc-root`,
`MWCC_DECOMP_ROOT`, or a sibling directory named `mwcc-decomp`. Do not copy its
source into this repository.

## Supported compiler identities

Capture addresses are enabled only for these exact SHA-256 values:

| Compiler | Capture target | SHA-256 |
| --- | --- | --- |
| GC/1.2.5 | `stock` | `0443b5c02b1aa7b575b61e0e24c4d5ad6bed8fd54cc42de5a2204a5216001914` |
| GC/1.2.5n | `ninji` | `ccf4b465cec73b5aae9c5c5543dcf8cda8a62aba246f89e2e0b200d742f2e55c` |

Directory names are not evidence. Every live capture rechecks the compiler
hash immediately before execution and rejects any other binary.

## Commands

Use placeholder paths in reusable documentation:

```sh
python3 -m tools.decomp_work.mwcc_intel doctor \
  --mwcc-root /path/to/mwcc-decomp --json build/mwcc_intel/doctor.json

python3 -m tools.decomp_work.mwcc_intel select \
  --minimum-match 90 --maximum-match 99.999 --limit 20 \
  --output build/mwcc_intel/candidates.json

python3 -m tools.decomp_work.mwcc_intel capture --symbol SYMBOL \
  --mwcc-root /path/to/mwcc-decomp --image mwcc-debugger:local --dry-run

python3 -m tools.decomp_work.mwcc_intel capture --symbol SYMBOL \
  --mwcc-root /path/to/mwcc-decomp --image mwcc-debugger:local --execute

python3 -m tools.decomp_work.mwcc_intel analyze CAPTURE_DIR

python3 -m tools.decomp_work.mwcc_intel compare \
  --baseline BASELINE_CAPTURE --candidate CANDIDATE_CAPTURE

python3 -m tools.decomp_work.mwcc_intel pilot --auto --dry-run
```

`capture --dry-run` resolves the authentic full owner translation unit,
preserves ordered and repeated flags from `build.ninja`, writes the exact host
and sandbox-mapped argument vectors, hashes every input, generates the GDB
command file, and executes nothing.

`capture --execute` uses a pre-existing image with `qemu-i386`,
`gdb-multiarch`, and the external capture script. The invocation uses
`--pull never`, no network, a read-only root, dropped capabilities,
`no-new-privileges`, PID/memory/CPU limits, a `noexec,nosuid,nodev` temporary
filesystem, read-only minimal input mounts, and one dedicated writable capture
directory. The host enforces a timeout. The compiler never runs through host
Wine, Wibo, or Windows execution.

## Interpretation limits

- Exact allocator replay is exact only for the modeled captured path.
- An inverse-order witness proves fixed-graph reachability, not source
  realizability.
- A pressure vector is a lower bound, not a concrete edit.
- A source-rank witness is constructive only inside its configured model.
- A sampled miss is not an impossibility proof.
- Ambiguous target-to-web mappings remain ambiguous.

The original compiler, `objdiff`, and the full linked DOL SHA-1 remain the
authorities. A function-level score alone cannot bank a source match.

## Publication boundary

Generated artifacts and proprietary inputs stay local. Before any public
commit or pull request, audit with `git status`, `git diff --check`, and
`git ls-files`; never publish compiler binaries, Wibo, objects, captures,
absolute workstation paths, or generated internal compiler state. On a public
remote, private review must happen in a separate private repository or through
a sanitized patch exchange.
