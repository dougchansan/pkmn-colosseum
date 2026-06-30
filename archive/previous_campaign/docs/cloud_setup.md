# Claude cloud (and any Linux CI) — full byte-match decomp setup

The public repo deliberately **does not contain** `main.dol` (the copyrighted GameCube
ROM image), the generated `*.inc` target assembly, the objdiff target objects, or the
Metrowerks compiler — so a fresh clone in the **Claude cloud environment** can't run the
matching workflow. This is the "only local works" gap.

Good news: the project is a standard **dtk-template** decomp. `configure.py` auto-downloads
*everything* platform-appropriate (dtk, a **Linux** objdiff-cli, the Metrowerks compilers,
**wibo** to run them, binutils). The **only** thing it can't fetch is `main.dol`. So the cloud
just needs main.dol delivered privately, then one build command regenerates the rest.

`tools/decomp_work/band.py` is now platform-aware: on Linux it uses the
`configure.py`-downloaded `build/tools/objdiff-cli`, `build/compilers/GC/<ver>/mwcceppc.exe`,
and runs mwcc through `build/tools/wibo`. Windows behaviour is unchanged.

## One-time host setup (you, locally — needs your accounts)

1. **Create a PRIVATE GitHub repo** to hold the ROM, e.g. `dougchansan/pkmn-colosseum-rom`.
   (Private because main.dol is copyrighted — never put it in the public repo.)
2. **Publish main.dol as a release asset** on that private repo:
   ```bash
   # from your local pkmn-colosseum checkout, with the GitHub CLI authenticated:
   gh release create rom orig/GC6E01/sys/main.dol \
      --repo dougchansan/pkmn-colosseum-rom --title "GC6E01 ROM" --notes "main.dol"
   # (or upload main.dol to a release named tag `rom` via the web UI)
   ```
3. **Mint a fine-grained PAT** with *read-only* access (Contents: Read) scoped to ONLY that
   private ROM repo. Copy the token.

## Configure the Claude cloud environment

Set these as **environment secrets** in the cloud env:

| secret | value |
|---|---|
| `DECOMP_PAT` | the fine-grained PAT from step 3 |
| `DECOMP_DOL_REPO` | `dougchansan/pkmn-colosseum-rom` |
| `DECOMP_DOL_TAG` | `rom` (optional; default `rom`) |
| `DECOMP_DOL_ASSET` | `main.dol` (optional; default `main.dol`) |

Then make the environment's **setup/init command** run:

```bash
sudo apt-get update && sudo apt-get install -y ninja-build   # if not already present
bash tools/decomp_work/cloud_bootstrap.sh
```

`cloud_bootstrap.sh` will: fetch main.dol from the private release (via the PAT), run
`python3 configure.py && ninja` (which downloads the Linux toolchain and regenerates the
`*.inc` + objects from main.dol), and verify the band harness can measure a function.

## Verify (in the cloud env, after bootstrap)

```bash
python3 tools/decomp_work/band.py sections src/game/gs_render.c 1   # ranked real-C targets
python3 tools/decomp_work/band.py init t src/game/gs_render.c
python3 tools/decomp_work/band.py diff t fn_800D2AD4                # target vs ours asm
```
If `sections`/`diff` print, the **full byte-match workflow (objdiff %)** is live in the cloud,
and cloud agents can follow `AGENT_ONBOARDING.md` / `tools/decomp_work/HARNESS.md` exactly as
local ones do.

## Notes

- main.dol and all ROM-derived artifacts stay OUT of the public repo (git-ignored). They live
  only in your private release and are pulled at setup time over an authenticated channel.
- The Tailscale dashboard (`/api/asm`, `/artifacts/*`) is for machines **on your tailnet**; the
  Claude cloud env is not, which is why it uses this private-release bootstrap instead.
- Same script works for GitHub Actions / any Linux CI — set the same secrets and call it.
