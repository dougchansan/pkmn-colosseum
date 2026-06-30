#!/usr/bin/env bash
# Pull the private ROM-derived artifacts (main.dol + target objects) from the
# renaming dashboard over the user's private Tailscale network, into the right
# local paths so a remote env can run the full objdiff / asm-review workflow.
#
# Usage:  bash tools/decomp_work/fetch_artifacts.sh [host[:port]]
#   host defaults to the DASH_HOST env var, else 127.0.0.1:8792
#
# These are the user's OWN ROM over their OWN private tailnet. Do not redistribute.
set -euo pipefail

HOST="${1:-${DASH_HOST:-127.0.0.1:8792}}"
BASE="http://${HOST}/artifacts"

# Resolve repo root from this script's location (tools/decomp_work/..).
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
cd "$ROOT"

echo "[fetch_artifacts] host=${HOST} root=${ROOT}"

mkdir -p orig/GC6E01/sys build/GC6E01

echo "[fetch_artifacts] main.dol -> orig/GC6E01/sys/main.dol"
curl -fL --progress-bar "${BASE}/main.dol" -o orig/GC6E01/sys/main.dol

echo "[fetch_artifacts] target-objects.zip -> build/GC6E01/obj/"
tmpzip="$(mktemp /tmp/target-objects.XXXXXX.zip)"
curl -fL --progress-bar "${BASE}/target-objects.zip" -o "$tmpzip"
# Archive entries are prefixed obj/... so extract at build/GC6E01/.
unzip -o "$tmpzip" -d build/GC6E01/ >/dev/null
rm -f "$tmpzip"

echo "[fetch_artifacts] done:"
ls -la orig/GC6E01/sys/main.dol
echo "  build/GC6E01/obj: $(ls build/GC6E01/obj 2>/dev/null | wc -l) files"
