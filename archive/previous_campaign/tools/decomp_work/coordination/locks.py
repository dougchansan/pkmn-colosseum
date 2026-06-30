#!/usr/bin/env python3
"""locks.py — atomic file/function locks for the parallel decomp fleet.

Backend: one small JSON file per lock under build/locks/ (override DECOMP_LOCKS_DIR),
acquired with an atomic O_EXCL create. No sqlite, no WAL, no DB-corruption surface.

Why file-based (rewritten from the SQLite version): the lock LOGIC was never the
problem — but the hot-path `list` call ran on every dispatch cycle, and under the
Windows AV process-spawn tax a `python locks.py list` could take many seconds and get
killed by the caller's timeout, returning EMPTY → the fleet fell back to a file-grep
backstop and could double-assign. With one file per lock:
  * `list` is just a directory read — a bash caller can do it with `ls`/`cat` and skip
    spawning python entirely (no startup tax on the hot path);
  * acquire is a single atomic O_EXCL create (atomic across processes);
  * an expired lock is reclaimed lazily on the next acquire/list (TTL + heartbeat);
  * the store is plain JSON you can read/delete by hand.

Two lock SCOPES, both exclusive:
  * file  — whole-file ownership.  key = repo-relative path, e.g. src/game/gs_render.c
  * fn    — single-function ownership. key = fn_XXXXXXXX (optionally tagged --file).

Re-acquiring a lock you already own renews it (re-entrant, never self-deadlocks).

CLI (verbs print a one-line summary; add --json for machine output):
    python locks.py acquire-file <agent> src/game/gs_render.c [--ttl 1800] [--note ..]
    python locks.py release-file <agent> src/game/gs_render.c
    python locks.py acquire <agent> fn_800DF21C [--file src/game/gs_render.c] [--ttl 1800]
    python locks.py release <agent> fn_800DF21C
    python locks.py renew <agent> [fn_800DF21C] [--scope file|fn]
    python locks.py owner fn_800DF21C
    python locks.py list [--scope file|fn] [--file src/...] [--agent name] [--all]
    python locks.py gc

Exit codes: 0 = success / lock is yours, 1 = denied or not found. So a worker can gate:
    python locks.py acquire me fn_X && ...edit...
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import socket
import sys
import time
from pathlib import Path

COORD_DIR = Path(__file__).resolve().parent
ROOT = COORD_DIR.parents[1]                      # tools/decomp_work/coordination -> repo? no:
# parents: [coordination, decomp_work, tools, <repo>]  -> repo is parents[2]
ROOT = COORD_DIR.parents[2]
LOCK_DIR = Path(os.environ.get("DECOMP_LOCKS_DIR", ROOT / "build" / "locks"))
DEFAULT_TTL = int(os.environ.get("DECOMP_LOCK_TTL", "1800"))  # 30 min; renew to extend
SCOPES = ("file", "fn")


def _now() -> float:
    return time.time()


def connect(db_path=None):
    """Compat shim: the old API threaded a sqlite connection through every call.
    File-based locks need none; we just ensure the lock dir exists and return None,
    so callers that still pass conn=connect() keep working unchanged."""
    LOCK_DIR.mkdir(parents=True, exist_ok=True)
    return None


def _lock_path(scope: str, key: str) -> Path:
    h = hashlib.sha1(f"{scope}:{key}".encode("utf-8")).hexdigest()[:10]
    safe = re.sub(r"[^A-Za-z0-9._-]", "_", key)[:60]
    return LOCK_DIR / f"{scope}__{safe}__{h}.lock"


def _read(lp: Path) -> dict | None:
    try:
        return json.loads(lp.read_text(encoding="utf-8"))
    except (OSError, ValueError):
        return None


def _atomic_write(lp: Path, rec: dict) -> None:
    LOCK_DIR.mkdir(parents=True, exist_ok=True)
    tmp = lp.with_suffix(lp.suffix + f".tmp{os.getpid()}")
    tmp.write_text(json.dumps(rec), encoding="utf-8")
    os.replace(str(tmp), str(lp))   # atomic on POSIX + Windows


def _decorate(rec: dict, now: float) -> dict:
    d = dict(rec)
    exp = float(d.get("expires_at") or 0)
    d["expired"] = bool(exp and exp <= now)
    d["ttl_remaining"] = None if exp == 0 else max(0, round(exp - now))
    d["age"] = round(now - float(d.get("acquired_at") or now))
    return d


# --------------------------------------------------------------------------- #
# Core API (importable) — signatures match the old SQLite module               #
# --------------------------------------------------------------------------- #
def acquire(agent, key, scope="fn", ttl=DEFAULT_TTL, file=None, note=None, conn=None) -> dict:
    if scope not in SCOPES:
        raise ValueError(f"scope must be one of {SCOPES}, got {scope!r}")
    LOCK_DIR.mkdir(parents=True, exist_ok=True)
    now = _now()
    exp = 0.0 if ttl <= 0 else now + ttl
    lp = _lock_path(scope, key)
    rec = {"scope": scope, "key": key, "owner": agent, "file": file, "note": note,
           "host": socket.gethostname(), "pid": os.getpid(),
           "acquired_at": now, "renewed_at": now, "expires_at": exp}
    # fast path: atomic create-if-absent
    try:
        fd = os.open(str(lp), os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o644)
        with os.fdopen(fd, "w") as f:
            json.dump(rec, f)
        return {"ok": True, "action": "acquired", "owner": agent, "lock": _decorate(rec, now)}
    except FileExistsError:
        pass
    existing = _read(lp)
    if existing is None:                       # corrupt/partial -> reclaim
        _atomic_write(lp, rec)
        return {"ok": True, "action": "acquired", "owner": agent, "lock": _decorate(rec, now)}
    exp_e = float(existing.get("expires_at") or 0)
    expired = bool(exp_e and exp_e <= now)
    if existing.get("owner") == agent:         # re-entrant renew
        existing["renewed_at"] = now
        existing["expires_at"] = exp
        if file is not None:
            existing["file"] = file
        if note is not None:
            existing["note"] = note
        _atomic_write(lp, existing)
        return {"ok": True, "action": "renewed", "owner": agent, "lock": _decorate(existing, now)}
    if expired:                                # steal a dead lock
        _atomic_write(lp, rec)
        return {"ok": True, "action": "acquired", "owner": agent, "lock": _decorate(rec, now)}
    return {"ok": False, "action": "denied", "owner": existing.get("owner"),
            "lock": _decorate(existing, now)}


def release(agent, key, scope="fn", force=False, conn=None) -> dict:
    lp = _lock_path(scope, key)
    existing = _read(lp)
    if existing is None:
        if lp.exists():
            try:
                lp.unlink()
            except OSError:
                pass
        return {"ok": True, "action": "absent", "owner": None}
    if existing.get("owner") != agent and not force:
        return {"ok": False, "action": "denied", "owner": existing.get("owner")}
    try:
        lp.unlink()
    except OSError:
        pass
    return {"ok": True, "action": "released", "owner": agent}


def renew(agent, key=None, scope=None, ttl=DEFAULT_TTL, conn=None) -> dict:
    now = _now()
    exp = 0.0 if ttl <= 0 else now + ttl
    n = 0
    if key is not None:
        scopes = [scope] if scope else list(SCOPES)
        for sc in scopes:
            lp = _lock_path(sc, key)
            rec = _read(lp)
            if rec and rec.get("owner") == agent:
                rec["renewed_at"] = now
                rec["expires_at"] = exp
                _atomic_write(lp, rec)
                n += 1
    else:
        for lp in LOCK_DIR.glob("*.lock"):
            rec = _read(lp)
            if rec and rec.get("owner") == agent:
                rec["renewed_at"] = now
                rec["expires_at"] = exp
                _atomic_write(lp, rec)
                n += 1
    return {"ok": n > 0, "action": "renewed", "count": n}


def owner_of(key, scope="fn", conn=None) -> dict | None:
    now = _now()
    lp = _lock_path(scope, key)
    rec = _read(lp)
    if rec is None:
        return None
    exp = float(rec.get("expires_at") or 0)
    if exp and exp <= now:                     # lazily reclaim
        try:
            lp.unlink()
        except OSError:
            pass
        return None
    return _decorate(rec, now)


def list_locks(scope=None, agent=None, file=None, include_expired=False, conn=None) -> list[dict]:
    now = _now()
    out = []
    if not LOCK_DIR.exists():
        return out
    for lp in LOCK_DIR.glob("*.lock"):
        rec = _read(lp)
        if rec is None:
            continue
        d = _decorate(rec, now)
        if d["expired"] and not include_expired:
            try:
                lp.unlink()                    # GC on read
            except OSError:
                pass
            continue
        if scope and rec.get("scope") != scope:
            continue
        if agent and rec.get("owner") != agent:
            continue
        if file and not (rec.get("file") == file or
                         (rec.get("scope") == "file" and rec.get("key") == file)):
            continue
        out.append(d)
    out.sort(key=lambda r: (r.get("scope", ""), -float(r.get("acquired_at") or 0)))
    return out


def gc(conn=None) -> int:
    now = _now()
    n = 0
    if not LOCK_DIR.exists():
        return 0
    for lp in LOCK_DIR.glob("*.lock"):
        rec = _read(lp)
        exp = float((rec or {}).get("expires_at") or 0)
        if rec is None or (exp and exp <= now):
            try:
                lp.unlink()
                n += 1
            except OSError:
                pass
    return n


# --------------------------------------------------------------------------- #
# CLI — identical verbs / flags / exit codes / `list` line format             #
# --------------------------------------------------------------------------- #
def _emit(result, as_json: bool, human: str) -> None:
    print(json.dumps(result, indent=2) if as_json else human)


def main(argv=None) -> int:
    p = argparse.ArgumentParser(prog="locks.py", description=__doc__,
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--json", action="store_true", help="machine-readable output")
    p.add_argument("--db", help="(compat, ignored) old sqlite path; use DECOMP_LOCKS_DIR")
    p.add_argument("--dir", help="override lock directory (default build/locks)")
    sub = p.add_subparsers(dest="cmd", required=True)

    def add_common(sp):
        sp.add_argument("--ttl", type=int, default=DEFAULT_TTL,
                        help=f"seconds until auto-expiry (0=never; default {DEFAULT_TTL})")
        sp.add_argument("--note", default=None)

    sp = sub.add_parser("acquire", help="acquire a function lock")
    sp.add_argument("agent"); sp.add_argument("fn")
    sp.add_argument("--file", default=None, help="owning src file (for grouping)")
    add_common(sp)
    sp = sub.add_parser("acquire-file", help="acquire a whole-file lock")
    sp.add_argument("agent"); sp.add_argument("file"); add_common(sp)
    sp = sub.add_parser("release", help="release a function lock")
    sp.add_argument("agent"); sp.add_argument("fn")
    sp.add_argument("--force", action="store_true")
    sp = sub.add_parser("release-file", help="release a whole-file lock")
    sp.add_argument("agent"); sp.add_argument("file")
    sp.add_argument("--force", action="store_true")
    sp = sub.add_parser("renew", help="heartbeat: extend agent's lock(s)")
    sp.add_argument("agent"); sp.add_argument("key", nargs="?", default=None)
    sp.add_argument("--scope", choices=SCOPES, default=None)
    sp.add_argument("--ttl", type=int, default=DEFAULT_TTL)
    sp = sub.add_parser("owner", help="who holds a lock?")
    sp.add_argument("key"); sp.add_argument("--scope", choices=SCOPES, default="fn")
    sp = sub.add_parser("list", help="list active locks")
    sp.add_argument("--scope", choices=SCOPES, default=None)
    sp.add_argument("--agent", default=None)
    sp.add_argument("--file", default=None)
    sp.add_argument("--all", action="store_true", help="include expired rows")
    sub.add_parser("gc", help="purge expired locks")

    args = p.parse_args(argv)
    global LOCK_DIR
    if args.dir:
        LOCK_DIR = Path(args.dir)
    LOCK_DIR.mkdir(parents=True, exist_ok=True)

    if args.cmd in ("acquire", "acquire-file"):
        scope = "fn" if args.cmd == "acquire" else "file"
        key = args.fn if scope == "fn" else args.file
        file_tag = getattr(args, "file", None) if scope == "fn" else key
        r = acquire(args.agent, key, scope=scope, ttl=args.ttl, file=file_tag, note=args.note)
        if r["action"] == "denied":
            lk = r["lock"]
            human = (f"DENIED: {scope} '{key}' held by {r['owner']} "
                     f"(ttl {lk.get('ttl_remaining')}s, note={lk.get('note')!r})")
        else:
            human = f"{r['action'].upper()}: {scope} '{key}' -> {args.agent}"
        _emit(r, args.json, human)
        return 0 if r["ok"] else 1

    if args.cmd in ("release", "release-file"):
        scope = "fn" if args.cmd == "release" else "file"
        key = args.fn if scope == "fn" else args.file
        r = release(args.agent, key, scope=scope, force=args.force)
        human = (f"DENIED: {scope} '{key}' is held by {r['owner']}, not {args.agent} (use --force)"
                 if r["action"] == "denied" else f"{r['action'].upper()}: {scope} '{key}'")
        _emit(r, args.json, human)
        return 0 if r["ok"] else 1

    if args.cmd == "renew":
        r = renew(args.agent, key=args.key, scope=args.scope, ttl=args.ttl)
        _emit(r, args.json, f"renewed {r['count']} lock(s) for {args.agent}")
        return 0 if r["ok"] else 1

    if args.cmd == "owner":
        r = owner_of(args.key, scope=args.scope)
        if r is None:
            _emit({"owner": None, "free": True}, args.json, f"FREE: {args.scope} '{args.key}'")
            return 0
        _emit(r, args.json, f"{r['owner']} holds {args.scope} '{args.key}' "
                            f"(ttl {r.get('ttl_remaining')}s, file={r.get('file')})")
        return 0

    if args.cmd == "list":
        rows = list_locks(scope=args.scope, agent=args.agent, file=args.file,
                          include_expired=args.all)
        if args.json:
            print(json.dumps(rows, indent=2))
        elif not rows:
            print("(no active locks)")
        else:
            for r in rows:
                exp = "EXPIRED" if r["expired"] else f"{r['ttl_remaining']}s"
                tag = f" file={r['file']}" if r.get("file") and r["scope"] == "fn" else ""
                print(f"[{r['scope']:>4}] {r['key']:<28} {r['owner']:<16} "
                      f"ttl={exp:<8}{tag}  {r.get('note') or ''}")
        return 0

    if args.cmd == "gc":
        n = gc()
        _emit({"purged": n}, args.json, f"purged {n} expired lock(s)")
        return 0
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
