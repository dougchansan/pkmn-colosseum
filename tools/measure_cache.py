"""measure_cache.py - fingerprint cache for objdiff runs.

progress.py and repeated sweeps re-run objdiff on every base .o even when
the object hasn't changed since the last scan. This caches the reduced
per-function result keyed on a fast fingerprint of (target .o, base .o):
size + mtime_ns. A changed base object misses and re-runs; an unchanged
one is a dict lookup.

Cache lives at build/.measure_cache.json (gitignored under build/).
Cuts a full-project progress scan from ~hundreds of objdiff spawns to
only the objects that actually changed.
"""

import os
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
_EXE = ".exe" if os.name == "nt" else ""


def _first_existing(cands):
    for c in cands:
        if c and Path(c).exists():
            return Path(c)
    return None


OBJDIFF = _first_existing([
    ROOT / "tools" / f"objdiff-cli{_EXE}",
    ROOT / "build" / "tools" / f"objdiff-cli{_EXE}",
]) or (ROOT / "tools" / f"objdiff-cli{_EXE}")
CACHE = ROOT / "build" / ".measure_cache.json"
sys.path.insert(0, str(ROOT / "tools"))
from headless_subprocess import run as run_tool  # noqa: E402

_MEM = None
_BASE_INDEX = None


def _load():
    global _MEM
    if _MEM is None:
        try:
            _MEM = json.loads(CACHE.read_text(encoding="utf-8"))
        except (OSError, ValueError):
            _MEM = {}
    return _MEM


def _index_by_base(mem):
    """Return base-fingerprint -> cached funcs.

    The unified target object is rebuilt often during local decomp work even
    though its ROM-derived code bytes are stable. Reuse prior entries when the
    base object fingerprint is unchanged so report generation does not throw
    away a warm cache just because the target .o mtime changed.
    """
    global _BASE_INDEX
    if _BASE_INDEX is None:
        idx = {}
        for old_key, value in mem.items():
            parts = old_key.split("|", 1)
            if len(parts) == 2 and parts[1] not in idx:
                idx[parts[1]] = value
        _BASE_INDEX = idx
    return _BASE_INDEX


def _fp(p: Path) -> str:
    st = p.stat()
    return f"{st.st_size}:{st.st_mtime_ns}"


def flush():
    if _MEM is not None:
        CACHE.parent.mkdir(parents=True, exist_ok=True)
        CACHE.write_text(json.dumps(_MEM), encoding="utf-8")


def _run_objdiff(target_o: Path, base_o: Path):
    out = run_tool(
        [str(OBJDIFF), "diff", "-1", str(target_o), "-2", str(base_o),
         "-o", "-", "--format", "json",
         "-c", "ppc.calculatePoolRelocations=false"],
        capture_output=True, text=True,
    )
    if out.returncode != 0:
        return None
    try:
        return json.loads(out.stdout)
    except ValueError:
        return None


def diff_funcs(target_o: Path, base_o: Path):
    """Return [{name, match, size}] for fn_ symbols, cached on
    (target,base) fingerprint. Only the *reduced* per-function data is
    cached (a few KB), never the multi-hundred-MB raw objdiff JSON --
    caching the raw JSON made the warm path slower than recomputing.
    Returns None on objdiff failure (not cached)."""
    mem = _load()
    try:
        base_fp = _fp(Path(base_o))
        key = f"{_fp(Path(target_o))}|{base_fp}"
    except OSError:
        key = None
        base_fp = None
    if key and key in mem:
        return mem[key]
    if key and base_fp:
        cached = _index_by_base(mem).get(base_fp)
        if cached is not None:
            mem[key] = cached
            return cached
    j = _run_objdiff(target_o, base_o)
    if j is None:
        return None
    funcs = []
    for s in j.get("right", {}).get("symbols", []):
        kind = s.get("kind")
        if kind is not None and kind != "SYMBOL_FUNCTION":
            continue
        name = s.get("name", "")
        if not name.startswith("fn_"):
            continue
        pct = s.get("match_percent")
        sz = s.get("size", 0)
        try:
            sz = int(sz, 0) if isinstance(sz, str) else int(sz)
        except (TypeError, ValueError):
            sz = 0
        if pct is not None:
            funcs.append({"name": name, "match": pct, "size": sz})
    if key:
        mem[key] = funcs
        if base_fp:
            _index_by_base(mem).setdefault(base_fp, funcs)
    return funcs


def diff_json(target_o: Path, base_o: Path):
    """Uncached full objdiff JSON (for callers needing instruction
    streams, e.g. diffclass). Not cached — too large to store."""
    return _run_objdiff(target_o, base_o)
