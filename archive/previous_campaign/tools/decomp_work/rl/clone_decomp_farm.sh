#!/usr/bin/env bash
# Clone net-new GameCube CodeWarrior decomps for byte-match training data.
# Shallow clones onto the roomy / fs (NOT the tight /storage). Idempotent.
set -uo pipefail
DEST="/home/douglaswhittingham/decomp_farm"
mkdir -p "$DEST"; cd "$DEST"

clone() {  # clone <dir> <url> [extra git args...]
  local dir="$1"; shift; local url="$1"; shift
  if [ -d "$dir/.git" ]; then echo "== SKIP $dir (already cloned) =="; return; fi
  echo "== CLONE $dir =="
  git clone --depth 1 "$@" "$url" "$dir" && echo "   ok: $(du -sh "$dir" 2>/dev/null | cut -f1)" || echo "!! FAILED: $dir"
}

# --- ideal CW 1.2-1.3 band, plain C, high match (highest value) ---
clone ac-decomp    https://github.com/ACreTeam/ac-decomp.git
clone marioparty4  https://github.com/mariopartyrd/marioparty4.git
clone afe-decomp   https://github.com/ACreTeam/afe-decomp.git           --recurse-submodules
clone SFA-Decomp   https://github.com/zcanann/SFA-Decomp.git
clone drmario64-gc https://github.com/NewGBAXL/drmario64-gc.git

# --- large CW corpora, version/lang slightly off-target (volume) ---
clone prime        https://github.com/PrimeDecomp/prime.git
clone smstrikers   https://github.com/yannicksuter/smstrikers-decomp.git --recurse-submodules
clone tp           https://github.com/zeldaret/tp.git

echo "== DONE =="
du -sh "$DEST"/* 2>/dev/null | sort -h
df -h /home | awk 'NR==2{print $4" free on / after clones"}'
