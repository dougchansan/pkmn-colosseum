#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

INTERVAL="${METRICS_INTERVAL_SECONDS:-86400}"

while true; do
  python3 tools/decomp_work/snapshot_metrics.py
  sleep "$INTERVAL"
done
