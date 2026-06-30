#!/bin/bash
# fleet_driver.sh — keep the named lanes busy and harvest wins. Each cycle:
#   1. regenerate the bucket queue (gen_bucket_queue.py)
#   2. refill idle lanes (auto_rebatch.sh, scoped to the live lane list)
#   3. every GATE_EVERY cycles, gate+commit accumulated band wins (auto_gate.sh)
# Emits one status line only on a meaningful event (rebatch or commit) so it is a
# clean Monitor stream. The lane list is read from build/fleet_lanes.txt EACH cycle,
# so lanes can be added/removed live (e.g. bring Codex up at reset) without a restart.
# Default lanes "OPUS SON"; GLM stays out (out of commission).
#
# TMUX-FREE (2026-06-20): this loop never runs a psmux client. Idle detection and
# prompt delivery are owned by pane_io.sh (the sole tmux owner) via build/hb/*.state
# and build/dispatch/*.req. REQUIRES pane_io.sh to be running — launch both detached
# with tools/decomp_work/fleet_up.ps1, stop with fleet_down.ps1. Every python/git call
# below is timeout-guarded so a stuck subprocess can never freeze the loop.
cd "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)/../.." || exit 1
export MSYS_NO_PATHCONV=1
# SINGLETON GUARD: TaskStop kills the Monitor's grep but NOT this bash loop, so naive
# restarts pile up zombie drivers that all dispatch at once. Refuse to start a 2nd
# instance. Stale pidfile (after SIGKILL) is harmless — kill -0 on a dead pid is false.
PIDF="build/.fleet_driver.pid"
if [ -f "$PIDF" ] && kill -0 "$(cat "$PIDF" 2>/dev/null)" 2>/dev/null; then
  echo "[fleet_driver] another instance ($(cat "$PIDF")) already running — exiting"; exit 0
fi
echo $$ > "$PIDF"; trap 'rm -f "$PIDF"' EXIT
INTERVAL="${INTERVAL:-15}"   # was 30: tighter cycle so a finished lane is refilled sooner
GATE_EVERY="${GATE_EVERY:-5}"
# Rebuild the wall-ledger (bucket membership/totals) every LEDGER_EVERY cycles so the
# campaign bucket counts on the dashboard stay current. The ledger's `attempted` overlay
# is recomputed live by the dashboard, but bucket MEMBERSHIP only refreshes on a full
# `wall_ledger.py build` — without this it freezes at the last manual build.
LEDGER_EVERY="${LEDGER_EVERY:-20}"
LANEFILE="build/fleet_lanes.txt"
[ -f "$LANEFILE" ] || echo "OPUS SON" > "$LANEFILE"
i=0; session=0
echo "[fleet_driver] up — lanes from $LANEFILE, interval ${INTERVAL}s, gate every ${GATE_EVERY}"
while true; do
  i=$((i+1))
  lanes=$(tr -d '\r' < "$LANEFILE" | tr '\n' ' ' | sed 's/  */ /g')
  [ -n "$(echo "$lanes" | tr -d ' ')" ] || lanes="OPUS SON"
  # --- pane_io wedge-watchdog --------------------------------------------------
  # pane_io can wedge under heavy host load: it keeps refreshing build/hb/.alive (so
  # a naive liveness check still passes) yet stops completing the capture/classify
  # loop, so every lane state freezes and idle lanes are never re-dispatched. Detect
  # it by lane-state STALENESS: if the freshest of ALL lane states is older than
  # WEDGE_STALE while .alive is fresh, pane_io is alive-but-stuck -> kill + relaunch
  # detached. Rate-limited to one restart per WEDGE_COOLDOWN so a slow first pass on
  # the fresh loop can't trigger a restart storm.
  WEDGE_STALE="${WEDGE_STALE:-75}"; WEDGE_COOLDOWN="${WEDGE_COOLDOWN:-120}"
  _now=$(date +%s); _alive=$(cat build/hb/.alive 2>/dev/null || echo 0); _fresh=0
  for _l in $lanes; do
    _ts=$(awk '{print $2}' "build/hb/$_l.state" 2>/dev/null || echo 0)
    [ "${_ts:-0}" -gt "$_fresh" ] && _fresh=$_ts
  done
  _lastwd=$(cat build/.pane_io_wd.ts 2>/dev/null || echo 0)
  if [ $(( _now - ${_alive:-0} )) -le 30 ] && [ "$_fresh" -gt 0 ] \
     && [ $(( _now - _fresh )) -gt "$WEDGE_STALE" ] \
     && [ $(( _now - ${_lastwd:-0} )) -gt "$WEDGE_COOLDOWN" ]; then
    echo "[$(date +%H:%M)] WATCHDOG: pane_io wedged (freshest lane state $(( _now - _fresh ))s old, .alive $(( _now - _alive ))s) — restarting pane_io"
    echo "$_now" > build/.pane_io_wd.ts
    _pid=$(cat build/.pane_io.pid 2>/dev/null); [ -n "$_pid" ] && kill -9 "$_pid" 2>/dev/null
    pkill -9 -f pane_io.sh 2>/dev/null; rm -f build/.pane_io.pid
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/decomp_work/spawn_pane_io.ps1 >/dev/null 2>&1
  fi
  timeout 40 python tools/decomp_work/gen_bucket_queue.py >/tmp/fleet_q.txt 2>&1
  bucket=$(grep -oE "ACTIVE-BUCKET=[A-Z]+ files=[0-9]+" /tmp/fleet_q.txt | head -1)
  # auto_rebatch is now TMUX-FREE (reads build/hb state, writes build/dispatch reqs);
  # the timeout is a belt-and-braces backstop on its python sub-calls only.
  rb=$(timeout 180 bash -c "ASM_LANES='$lanes' bash tools/decomp_work/auto_rebatch.sh" 2>/dev/null | grep -c "^REBATCH")
  gatemsg=""
  if [ $((i % GATE_EVERY)) -eq 0 ]; then
    # Detect commits by HEAD change — robust to auto_gate's output format. auto_gate
    # only gates files not currently band-locked, so a lane camping a file delays its
    # wins until the lock releases; gating every GATE_EVERY cycles catches them then.
    before=$(timeout 10 git rev-parse HEAD 2>/dev/null)
    g=$(timeout 240 bash tools/decomp_work/auto_gate.sh 2>&1)
    after=$(timeout 10 git rev-parse HEAD 2>/dev/null)
    nfraud=$(echo "$g" | grep -ciE "fraud|reject")
    if [ -n "$before" ] && [ "$before" != "$after" ]; then
      nc=$(timeout 10 git rev-list --count "$before..$after" 2>/dev/null)
      nbe=$(echo "$g" | grep -oE "\+[0-9]+ byte-exact" | grep -oE "[0-9]+" | awk '{s+=$1} END{if(s)print s}')
      session=$((session + ${nc:-1}))
      gatemsg=" | GATED ${nc} commit(s) ${nbe:+(+${nbe} byte-exact)}"
    fi
    [ "$nfraud" -gt 0 ] && gatemsg="$gatemsg | fraud $nfraud"
  fi
  # periodic wall-ledger rebuild (backgrounded + locked so it never blocks a dispatch
  # cycle and concurrent builds can't stack/corrupt the ledger).
  if [ $((i % LEDGER_EVERY)) -eq 0 ] && [ ! -f build/.ledger_building ]; then
    ( touch build/.ledger_building
      timeout 240 python tools/decomp_work/wall_ledger.py build >/tmp/fleet_ledger.txt 2>&1
      rm -f build/.ledger_building ) &
    gatemsg="$gatemsg | ledger-rebuild"
  fi
  ts=$(date +%H:%M)
  if [ "$rb" -gt 0 ] || [ -n "$gatemsg" ]; then
    echo "[$ts] ${bucket:-bucket=?} | lanes:[$lanes] rebatched $rb$gatemsg | session +$session"
  fi
  sleep "$INTERVAL"
done
