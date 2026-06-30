# Worktree workflow for parallel decomp work

When multiple agents (Codex + Opus + Kimi etc.) are decompiling functions in
the same `.c` file simultaneously, use git worktrees to avoid edit conflicts.

## Setup a fresh worktree

```bash
# From the main repo
git worktree add ../pkmn-colosseum-mybranch -b decomp-mybranch
```

The worktree shares git state but has its own working tree. Edits in one
worktree don't conflict with edits in another.

## Required artifact copies (gitignored, not in worktree)

The `tools/mwcc_compiler/`, `tools/objdiff-cli.exe`, and `build/` artifacts are
gitignored. The worktree gets a clean checkout WITHOUT them. Copy them over:

```bash
cp -r tools/mwcc_compiler ../pkmn-colosseum-mybranch/tools/
cp tools/objdiff-cli.exe ../pkmn-colosseum-mybranch/tools/
mkdir -p ../pkmn-colosseum-mybranch/build/GC6E01/{obj,base/game}
cp build/GC6E01/obj/auto_01_800055E0_text.o ../pkmn-colosseum-mybranch/build/GC6E01/obj/
cp build/GC6E01/base/game/gs_title.o ../pkmn-colosseum-mybranch/build/GC6E01/base/game/
cp openrouterkey.txt ../pkmn-colosseum-mybranch/openrouterkey.txt
```

Also copy any untracked scripts you depend on (e.g., the benchmark/attack
infra files added during this session — they're untracked in main).

## Three workflow gotchas (learned the hard way)

### 1. Always Re-Read before Edit when iteration has run

`local_iterate.py` mutates `src/game/<stem>.c` on every run (swap-then-restore).
After ANY iteration, the file's mtime changes even if content is identical.
The `Edit` tool will refuse with "File modified since read" until you re-Read.

**Rule:** Right before each `Edit` on the .c file, do a fresh `Read` of the
specific line range you're editing. Don't rely on older Read cache.

### 2. Asm-wrapper signature conflicts cascade across the file

If you change a file-scope `extern void fn_X(void)` to match the function's
real signature (e.g., `extern s32 fn_X(u32, u32*)`), CW will error on the
asm-wrapper definition at the function's home position:
```
identifier 'fn_X(...)' redeclared
was declared as: 'unsigned long'
now declared as: 'int (...)'
```
This error spreads — the asm-wrapper for the next function in the file gets
parsed as C code, producing dozens of "declaration syntax error" lines that
look like they're from a totally different function.

**Rule:** If you must change a file-scope extern's signature, ALSO update the
asm-wrapper's signature OR keep the extern as `void` and use cast-fnptr at
call sites. The cast-fnptr approach is the standard idiom in this file.

**Diagnostic:** When compile errors mention asm content from a function you
weren't editing (e.g., `bl fn_8005D934` showing as syntax error), suspect a
signature conflict introduced earlier in the file.

### 3. 3090 reward calls share state across worktrees

The 3090 reward host has a single `/storage/finetune/pkmn-colosseum/` tree.
Each `remote_reward` call snapshots-mutates-restores the .c file under a
file lock. WITHIN a call this is atomic. BETWEEN calls, multiple worktrees
both `scp`-ing their version of `gs_title.c` will race.

**Rule:** When two agents are pushing to the 3090 reward, expect occasional
"compile_ok=False" results that are actually transient state-snapshot
issues, not real compile errors. Re-running usually succeeds.

**Mitigation:** `bench_compile_match.py` already scps the candidate's file
to a unique temp filename per call, so cross-worktree contention is bounded
to the .c-source-of-truth state, not the candidate body.

## Procedure to commit a worktree's work back to master

```bash
# In main repo
cd /c/Users/douglaswhittingham/pkmn-colosseum
git fetch  # picks up commits from worktree's branch (shared .git)
git merge --ff-only decomp-mybranch  # if linear history
# OR
git cherry-pick <sha>  # for individual commits
```

## When to use a worktree vs main repo

- **Use main repo** for: solo work, infra/tooling commits, tests that don't
  touch shared `.c` files.
- **Use worktree** for: iterating on a specific function while other agents
  edit the same file, or experimenting with multiple alternative
  implementations in parallel branches.
