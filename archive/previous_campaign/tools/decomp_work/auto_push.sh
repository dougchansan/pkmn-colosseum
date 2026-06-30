#!/bin/bash
# auto_push.sh — keep GitHub current. Every PUSH_INTERVAL seconds: sync the README
# headline numbers to report.json, commit that if it changed, then push master if
# local is ahead of origin. Master push requires ALLOW_MASTER_PUSH=1 (pre-push hook
# gate); the user has authorized auto-push of decomp progress. Remote is GitHub
# (origin = github.com/dougchansan/pkmn-colosseum), NOT GitLab.
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
export MSYS_NO_PATHCONV=1 ALLOW_MASTER_PUSH=1
INTERVAL="${PUSH_INTERVAL:-900}"
echo "[auto_push] up — README sync + master push every ${INTERVAL}s"
while true; do
  python tools/sync_progress_metadata.py --sync >/dev/null 2>&1
  if ! git diff --quiet -- README.md tools/decomp_work/progress.json 2>/dev/null; then
    git add README.md tools/decomp_work/progress.json &&
      git commit -q -m "progress: sync metadata [auto]" 2>/dev/null
  fi
  ahead=$(git rev-list --count origin/master..master 2>/dev/null)
  if [ "${ahead:-0}" -gt 0 ]; then
    if git push origin master >/tmp/auto_push.log 2>&1; then
      echo "[$(date +%H:%M)] pushed $ahead commit(s) to GitHub ($(git rev-parse --short HEAD))"
    else
      echo "[$(date +%H:%M)] PUSH FAILED: $(tail -2 /tmp/auto_push.log | tr '\n' ' ')"
    fi
  fi
  sleep "$INTERVAL"
done
