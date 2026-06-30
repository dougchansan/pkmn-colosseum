#!/usr/bin/env bash
# txk.sh — the ONLY safe way to run a psmux (native-PE Windows tmux) client.
# Source this, then call `txk <tmux-args...>`.
#
# Why this exists (see reference_psmux_windows_tmux.md + the 2026-06-20 stall):
#   * tmux here is a PE32+ native Windows console binary. A client that wedges
#     (concurrent access / console-handle contention) will NOT die from SIGTERM —
#     MSYS cannot deliver a Unix SIGTERM to a native PE process. `timeout 8 tmux`
#     therefore could not kill it, the driver loop blocked forever, and restarts
#     piled up zombie bash+tmux clients until the machine had to be rebooted.
#   * txk fixes both halves:
#       1. `timeout -s KILL` — SIGKILL maps to TerminateProcess, which DOES reap a
#          native PE. A wedged client dies within TXK_T seconds, guaranteed.
#       2. A process-wide mutex (atomic mkdir; flock is unavailable in Git Bash) so
#          only ONE psmux client runs at a time. Concurrent clients are what wedge
#          the per-console server in the first place; serializing removes the cause.
#   * Stale lock (holder SIGKILLed mid-hold) self-heals: the holder PID is recorded
#     and a dead holder's lock is stolen, so a killed capture never deadlocks the next.

# --- resolve the tmux binary (prefer env, then PATH, then the known-good path) ---
if [ -z "${TMUX_BIN:-}" ]; then
  if command -v tmux >/dev/null 2>&1; then TMUX_BIN="$(command -v tmux)"
  elif [ -x /c/Users/douglaswhittingham/bin/tmux ]; then TMUX_BIN=/c/Users/douglaswhittingham/bin/tmux
  else echo "txk: FATAL tmux binary not found" >&2; return 1 2>/dev/null || exit 1; fi
fi
export TMUX_BIN

# repo-root-anchored lock dir so every process shares one mutex
_TXK_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TXK_ROOT="${TXK_ROOT:-$(cd "$_TXK_DIR/../../.." && pwd)}"
TXK_LOCKD="${TXK_LOCKD:-$TXK_ROOT/build/.tmux.lockd}"
TXK_T="${TXK_T:-6}"          # SIGKILL deadline per tmux call (seconds)
TXK_WAIT="${TXK_WAIT:-150}"  # max 0.1s spins waiting for the mutex (15s) before stealing

mkdir -p "$TXK_ROOT/build" 2>/dev/null

_txk_acquire() {
  local holder spins=0
  until mkdir "$TXK_LOCKD" 2>/dev/null; do
    holder=$(cat "$TXK_LOCKD/pid" 2>/dev/null)
    # holder process gone (SIGKILLed mid-hold) -> steal immediately
    if [ -n "$holder" ] && ! kill -0 "$holder" 2>/dev/null; then
      rm -rf "$TXK_LOCKD" 2>/dev/null; continue
    fi
    sleep 0.1; spins=$((spins+1))
    if [ "$spins" -ge "$TXK_WAIT" ]; then     # hard backstop: never block forever
      rm -rf "$TXK_LOCKD" 2>/dev/null; continue
    fi
  done
  echo $$ > "$TXK_LOCKD/pid" 2>/dev/null
}
_txk_release() { rm -rf "$TXK_LOCKD" 2>/dev/null; }

# txk <tmux args...> : serialized + SIGKILL-bounded tmux call. Returns tmux's exit
# code, or 124 if it was killed on timeout. Never blocks the caller indefinitely.
txk() {
  _txk_acquire
  local rc
  timeout -s KILL "$TXK_T" "$TMUX_BIN" "$@"
  rc=$?
  _txk_release
  return $rc
}
