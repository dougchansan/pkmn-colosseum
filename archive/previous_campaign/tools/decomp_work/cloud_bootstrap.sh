#!/usr/bin/env bash
# cloud_bootstrap.sh — provision the Claude cloud (or any Linux CI) environment
# for the FULL byte-match decomp workflow.
#
# The public repo is missing exactly ONE thing the toolchain can't auto-download:
# the original ROM image `main.dol` (copyrighted). Everything else — dtk, a Linux
# objdiff-cli, the Metrowerks compilers, wibo, binutils — is fetched by
# `configure.py` from public mirrors. So this script:
#   1. downloads main.dol from a PRIVATE GitHub release asset (via a PAT secret),
#   2. runs `configure.py` + `ninja` (which regenerates the *.inc target asm AND
#      the objdiff target objects from main.dol, and downloads the Linux toolchain),
#   3. verifies the band harness can measure a function.
#
# Required env (set these as secrets in the cloud environment):
#   DECOMP_PAT        fine-grained GitHub PAT with read access to the private ROM repo
#   DECOMP_DOL_REPO   owner/repo holding the release (e.g. dougchansan/pkmn-colosseum-rom)
#   DECOMP_DOL_TAG    release tag that has the asset (default: rom)
#   DECOMP_DOL_ASSET  asset file name (default: main.dol)
#
# Idempotent: re-running with main.dol already in place skips the download.
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."   # repo root

DOL_PATH="orig/GC6E01/sys/main.dol"
DOL_TAG="${DECOMP_DOL_TAG:-rom}"
DOL_ASSET="${DECOMP_DOL_ASSET:-main.dol}"
EXPECTED_SIZE=3779808            # bytes — Colosseum GC6E01 main.dol

log() { printf '\n=== %s ===\n' "$*"; }

have_dol() { [ -f "$DOL_PATH" ] && [ "$(wc -c <"$DOL_PATH")" = "$EXPECTED_SIZE" ]; }

if have_dol; then
  log "main.dol already present (${EXPECTED_SIZE} bytes) — skipping download"
else
  : "${DECOMP_PAT:?set DECOMP_PAT (GitHub PAT with read access to the private ROM repo)}"
  : "${DECOMP_DOL_REPO:?set DECOMP_DOL_REPO=owner/repo of the private release}"
  log "Fetching ${DOL_ASSET} from ${DECOMP_DOL_REPO}@${DOL_TAG} (private release)"
  mkdir -p "$(dirname "$DOL_PATH")"
  api="https://api.github.com/repos/${DECOMP_DOL_REPO}/releases/tags/${DOL_TAG}"
  # Resolve the asset id, then download the raw bytes (octet-stream) with the PAT.
  asset_id=$(curl -fsSL -H "Authorization: Bearer ${DECOMP_PAT}" \
                 -H "Accept: application/vnd.github+json" "$api" \
             | python3 -c "import sys,json;a=[x for x in json.load(sys.stdin).get('assets',[]) if x['name']=='${DOL_ASSET}'];print(a[0]['id'] if a else '')")
  [ -n "$asset_id" ] || { echo "ERROR: asset '${DOL_ASSET}' not found in ${DECOMP_DOL_REPO}@${DOL_TAG}"; exit 1; }
  curl -fSL -H "Authorization: Bearer ${DECOMP_PAT}" \
       -H "Accept: application/octet-stream" \
       "https://api.github.com/repos/${DECOMP_DOL_REPO}/releases/assets/${asset_id}" \
       -o "$DOL_PATH"
  have_dol || { echo "ERROR: downloaded main.dol is the wrong size ($(wc -c <"$DOL_PATH") bytes, expected ${EXPECTED_SIZE})"; exit 1; }
  log "main.dol OK (${EXPECTED_SIZE} bytes)"
fi

command -v ninja >/dev/null 2>&1 || { echo "ERROR: ninja not on PATH — install it (apt-get install -y ninja-build)"; exit 1; }
command -v python3 >/dev/null 2>&1 || { echo "ERROR: python3 required"; exit 1; }

log "configure.py (downloads dtk / objdiff-cli / compilers / wibo / binutils)"
python3 configure.py

log "ninja (splits main.dol -> *.inc, compiles -> objects, builds objdiff targets)"
ninja

log "Verify: band harness can measure a function on Linux"
python3 tools/decomp_work/band.py sections src/hsd/hsd_cobj.c 1 | tail -3 || {
  echo "WARN: band sections failed — check build/tools/objdiff-cli, build/tools/wibo, build/compilers/GC/*/mwcceppc.exe exist"; exit 1; }

log "BOOTSTRAP COMPLETE — full byte-match workflow is available (read AGENT_ONBOARDING.md / HARNESS.md)"
