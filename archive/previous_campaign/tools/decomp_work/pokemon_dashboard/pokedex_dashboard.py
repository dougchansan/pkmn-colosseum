#!/usr/bin/env python3
"""Pokédex Decomp Dashboard — Horizontal HUD with 3D rotating Poké Ball.

Left panel:  Animated Poké Ball sphere with holo sparkles
Right panel: Dex completion stats, per-file progress bars, activity

Usage: python pokedex_dashboard.py [--once] [--no-color] [--reduced-motion]
"""

import os, sys, io, math, time, json, subprocess, random, shutil, threading

try:
    import winsound
    HAS_SOUND = True
except ImportError:
    HAS_SOUND = False

if sys.platform == "win32":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding="utf-8", errors="replace")

# Allow importing sprite_data from this folder
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
try:
    from sprite_data import SPRITE_ROWS, SPRITE_W
except ImportError:
    SPRITE_ROWS = []
    SPRITE_W = 0

try:
    from enemy_sprite_data import ENEMY_ROWS, ENEMY_W
except ImportError:
    ENEMY_ROWS = []
    ENEMY_W = 0

# ─── Tuning ──────────────────────────────────────────────────────────────────

BALL_RADIUS    = int(os.environ.get("POKDEX_BALL_SCALE", "14"))
SPARKLE_DENSITY = float(os.environ.get("POKDEX_SPARKLE_DENSITY", "0.008"))
ROT_SPEED      = float(os.environ.get("POKDEX_ROT_SPEED", "0.03"))
REFRESH        = float(os.environ.get("POKDEX_REFRESH", "0.12"))
STATS_EVERY    = int(os.environ.get("POKDEX_STATS_REFRESH", "3000"))
NO_COLOR       = "--no-color" in sys.argv or os.environ.get("POKDEX_NO_COLOR") == "1"
REDUCED        = "--reduced-motion" in sys.argv or os.environ.get("POKDEX_REDUCED_MOTION") == "1"

HERE       = os.path.dirname(os.path.abspath(__file__))
REPO       = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
DECOMP_DIR = os.path.abspath(os.path.join(HERE, ".."))
COORD_DIR  = os.path.join(DECOMP_DIR, "coordination")
REVIEW_DIR = os.path.join(DECOMP_DIR, "review")
APPLIED_DIR = os.path.join(DECOMP_DIR, "applied")
TMUX_DIR   = os.path.join(DECOMP_DIR, "tmux_control")
MILE_FILE  = os.path.join(TMUX_DIR, "logs", "milestones.json")
MATCH_HISTORY_FILE = os.path.join(HERE, "match_history.json")
AGENT_TOKENS_FILE  = os.path.join(REPO, ".omc", "agent_tokens.json")
PROGRESS_FILE = os.path.join(DECOMP_DIR, "progress.json")
WORK_QUEUE_FILE = os.path.join(DECOMP_DIR, "work_queue.json")
REPORTS_DIR = os.path.join(DECOMP_DIR, "reports")
DASHBOARD_STATE_FILE = os.path.join(REPORTS_DIR, "dashboard_state.json")

# ─── ANSI ────────────────────────────────────────────────────────────────────

RST = "\033[0m"
BLD = "\033[1m"
DM  = "\033[2m"

def _c(t, *codes):
    return ("".join(codes) + t + RST) if not NO_COLOR else t

R91 = "\033[91m"; R31 = "\033[31m"; G92 = "\033[92m"; Y93 = "\033[93m"
B94 = "\033[94m"; M95 = "\033[95m"; C96 = "\033[96m"; W97 = "\033[97m"
GR37 = "\033[37m"

def _fg256(n):
    return f"\033[38;5;{n}m"

def _bg256(n):
    return f"\033[48;5;{n}m"


# Some terminals render 256-color cells at double-width. For the PIKAPI sprite
# we quantize to basic 16 colors (30-37, 90-97 — ANSI 4-bit) which all
# terminals render at normal 1-cell width.
_ANSI16 = [
    (0,   0,   0,   30),  # black
    (170, 0,   0,   31),  # red
    (0,   170, 0,   32),  # green
    (170, 85,  0,   33),  # yellow/brown
    (0,   0,   170, 34),  # blue
    (170, 0,   170, 35),  # magenta
    (0,   170, 170, 36),  # cyan
    (170, 170, 170, 37),  # white
    (85,  85,  85,  90),  # bright black (grey)
    (255, 85,  85,  91),  # bright red
    (85,  255, 85,  92),  # bright green
    (255, 255, 85,  93),  # bright yellow
    (85,  85,  255, 94),  # bright blue
    (255, 85,  255, 95),  # bright magenta
    (85,  255, 255, 96),  # bright cyan
    (255, 255, 255, 97),  # bright white
]

def _ansi16_code_from_256(n):
    """Map an ANSI 256-color index to the nearest 16-color ANSI code.

    Sprites are pre-palettized at convert time (convert_sprite.py locks each
    sprite to its own color list). If `n` is already a valid 16-color code
    (30-37 or 90-97), it's used as-is. Otherwise, a generic nearest-match
    is performed for any lingering 256-color values in sprite data."""
    if 30 <= n <= 37 or 90 <= n <= 97:
        return n
    if n < 16:
        i = n
        if i in (5, 13): return 95
        if i in (1, 3, 9, 11): return 91
        if i in (15, 7): return 97
        return 30
    if n < 232:
        i = n - 16
        ri = i // 36
        gi = (i // 6) % 6
        bi = i % 6
        r, g, b = ri * 51, gi * 51, bi * 51
    else:
        gray = (n - 232) * 10 + 8
        r = g = b = gray
    # Decide between: dark (30), white (97), purple (95), orange (91).
    mx = max(r, g, b); mn = min(r, g, b)
    brightness = (r + g + b) / 3
    chroma = mx - mn
    # Low-chroma: black if dark, white if bright
    if chroma < 25:
        return 30 if brightness < 100 else 97
    # Chromatic: decide purple vs orange by where the hue leans.
    # Purple = high red+blue, low green.  Orange = high red, mid green, low blue.
    purple_score = (r + b) / 2 - g
    orange_score = r - b
    if purple_score > orange_score:
        return 95
    return 91

# ─── Sphere renderer ────────────────────────────────────────────────────────

# Filled-block shade ramp — gradient from empty → full block.
SHADE = " ░░▒▒▓▓████"

SPARKLE_CH = ["*", "+", ".", "'", "~", "x", "o", "*", "+", ".", "'", "~"]
HOLO_C = [Y93, C96, M95, W97, B94, G92, R91]


def add_sparkles(grid, ball_w, ball_h, frame, radius=None):
    rng = random.Random(frame // 3)
    color_off = frame % len(HOLO_C)
    count = int(ball_w * ball_h * SPARKLE_DENSITY)

    cx = ball_w / 2.0
    cy = ball_h / 2.0
    aspect = 2.1
    R = radius if radius else min(ball_w // 2, int(ball_h * aspect))
    inner_margin = 3
    outer_margin = 2

    for _ in range(count):
        x = rng.randint(0, ball_w - 1)
        y = rng.randint(0, ball_h - 1)
        if x < outer_margin or x >= ball_w - outer_margin: continue
        if y < outer_margin or y >= ball_h - outer_margin: continue
        nx = (x - cx) / R
        ny = (y - cy) / (R / aspect)
        dist = math.sqrt(nx * nx + ny * ny)
        inner_r = 1.0 + inner_margin / R
        if dist < inner_r: continue
        if y < len(grid) and x < len(grid[y]) and grid[y][x] is None:
            ch = rng.choice(SPARKLE_CH)
            ci = (color_off + rng.randint(0, 3)) % len(HOLO_C)
            bold = BLD if rng.random() > 0.4 else ""
            grid[y][x] = _c(ch, HOLO_C[ci], bold)
    return grid


# ─── Data collection ─────────────────────────────────────────────────────────

_scan_cache = {}

def _extract_symbol_bytes(elf_path, sym_name):
    import struct as _struct
    try:
        with open(elf_path, "rb") as fh:
            data = fh.read()
    except OSError:
        return None, 0
    if len(data) < 0x34 or data[:4] != b"\x7fELF":
        return None, 0
    e_shoff = _struct.unpack(">I", data[0x20:0x24])[0]
    e_shentsize = _struct.unpack(">H", data[0x2E:0x30])[0]
    e_shnum = _struct.unpack(">H", data[0x30:0x32])[0]
    e_shstrndx = _struct.unpack(">H", data[0x32:0x34])[0]
    sections = []
    for i in range(e_shnum):
        off = e_shoff + i * e_shentsize
        sections.append(_struct.unpack(">IIIIIIIIII", data[off:off+40]))
    shstr_off = sections[e_shstrndx][4]
    shstr = data[shstr_off:shstr_off+sections[e_shstrndx][5]]
    def sh_name(idx):
        end = shstr.index(b"\x00", idx)
        return shstr[idx:end].decode(errors="replace")
    symtab = strtab = text_sec = None
    for sh in sections:
        n = sh_name(sh[0])
        if n == ".symtab": symtab = sh
        elif n == ".strtab": strtab = sh
        elif n == ".text": text_sec = sh
    if not (symtab and strtab and text_sec):
        return None, 0
    strtab_data = data[strtab[4]:strtab[4]+strtab[5]]
    sym_off, sym_size, sym_ent = symtab[4], symtab[5], symtab[9]
    text_off = text_sec[4]
    for i in range(0, sym_size, sym_ent):
        st = _struct.unpack(">IIIBBH", data[sym_off+i:sym_off+i+16])
        end = strtab_data.index(b"\x00", st[0])
        name = strtab_data[st[0]:end].decode(errors="replace")
        if name == sym_name:
            return data[text_off+st[1]:text_off+st[1]+st[2]], st[2]
    return None, 0

_match_pct_cache = {}  # (target_path, base_path, target_mtime, base_mtime, fn) -> pct

# objdiff-cli path (same tool match_scan_file.py uses for accurate
# instruction-level match%, including weighted register/imm tolerances).
_OBJDIFF_CLI = os.path.join(REPO, 'tools', ('objdiff-cli.exe' if os.name == "nt" else 'objdiff-cli'))

def _c_block_match_percent(target_elf, base_elf, fn_name):
    """Count fn as 'matched' for dex if objdiff-cli reports ≥90% byte-match —
    the same bar match_scan_file.py uses (and our commit policy enforces).

    Falls back to fast 4-byte-chunk equality if objdiff-cli is unavailable.
    Cached per (target/base mtime, fn) so we only invoke the subprocess when
    a build artifact has actually changed.
    """
    try:
        tm = os.path.getmtime(target_elf)
        bm = os.path.getmtime(base_elf)
    except OSError:
        return 0.0
    key = (target_elf, base_elf, tm, bm, fn_name)
    if key in _match_pct_cache:
        return _match_pct_cache[key]

    pct = None
    if os.path.exists(_OBJDIFF_CLI):
        try:
            r = subprocess.run(
                [_OBJDIFF_CLI, 'diff', '-1', target_elf, '-2', base_elf,
                 '-o', '-', '--format', 'json',
                 '-c', 'ppc.calculatePoolRelocations=false', fn_name],
                capture_output=True, text=True, shell=False, timeout=10
            )
            if r.returncode == 0 and r.stdout.strip():
                d = json.loads(r.stdout)
                for side in ('right', 'left'):
                    for sym in d.get(side, {}).get('symbols', []):
                        if sym.get('name') == fn_name and sym.get('kind') == 'SYMBOL_FUNCTION':
                            mp = sym.get('match_percent')
                            if mp is not None:
                                pct = mp
                                break
                    if pct is not None:
                        break
        except (subprocess.TimeoutExpired, subprocess.SubprocessError, ValueError):
            pct = None

    if pct is None:
        # Fallback: 4-byte chunk equality (legacy fast path)
        tb, ts = _extract_symbol_bytes(target_elf, fn_name)
        cb, cs = _extract_symbol_bytes(base_elf, fn_name)
        if not tb or not cb or ts == 0 or cs == 0 or ts != cs:
            pct = 0.0
        elif tb == cb:
            pct = 100.0
        elif ts < 4:
            pct = 0.0
        else:
            matches = sum(1 for i in range(0, ts, 4) if tb[i:i+4] == cb[i:i+4])
            pct = 100.0 * matches / max(1, (ts // 4))

    _match_pct_cache[key] = pct
    # Cap cache size to prevent unbounded growth
    if len(_match_pct_cache) > 5000:
        _match_pct_cache.clear()
    return pct


def _is_c_block_matched(target_elf, base_elf, fn_name):
    """Compatibility wrapper: official completion means 100% match."""
    return _c_block_match_percent(target_elf, base_elf, fn_name) >= 100.0


_ASM_WRAPPER_RE = None
_catalog_cache = {"ts": 0, "catalog": {}}


def _asm_wrapper_symbol(line):
    import re as _re
    global _ASM_WRAPPER_RE
    if _ASM_WRAPPER_RE is None:
        _ASM_WRAPPER_RE = _re.compile(r"\s*asm\b(?:\s+\w+)?\s+([_A-Za-z]\w*)\s*\(")
    m = _ASM_WRAPPER_RE.match(line)
    return m.group(1) if m else None


def _load_json(path, default):
    try:
        with open(path, encoding="utf-8") as f:
            return json.load(f)
    except (OSError, ValueError):
        return default


def _latest_report_path(prefix):
    try:
        names = [
            os.path.join(REPORTS_DIR, n)
            for n in os.listdir(REPORTS_DIR)
            if n.startswith(prefix) and n.endswith(".json")
        ]
    except OSError:
        return None
    return max(names, key=os.path.getmtime) if names else None


def load_function_catalog(max_age_s=30):
    """Map known functions to files from queue/report artifacts."""
    now = time.time()
    if now - _catalog_cache["ts"] < max_age_s:
        return _catalog_cache["catalog"]

    catalog = {}
    sources = []
    queue = _load_json(WORK_QUEUE_FILE, [])
    if isinstance(queue, list):
        sources.extend(queue)
    latest_scan = _latest_report_path("work_queue_scan-")
    if latest_scan:
        report = _load_json(latest_scan, {})
        wrappers = report.get("wrappers", [])
        if isinstance(wrappers, list):
            sources.extend(wrappers)

    for item in sources:
        fn = item.get("function")
        fp = item.get("file")
        if not fn or not fp:
            continue
        catalog[fn] = {
            "file": fp.replace("\\", "/"),
            "tier": item.get("tier", "?"),
            "asm_lines": item.get("asm_lines"),
            "line_number": item.get("line_number"),
        }

    _catalog_cache["ts"] = now
    _catalog_cache["catalog"] = catalog
    return catalog

def scan_sources():
    results = {}
    matched_fns = set()       # set of fn names currently matched (for regression tracking)
    matched_by_file = {}
    function_rows = []
    seen_symbols = set()
    target_elf = os.path.join(REPO, "build", "GC6E01", "obj", "auto_01_800055E0_text.o")
    for root, _, files in os.walk(os.path.join(REPO, "src")):
        for f in files:
            if not f.endswith(".c"): continue
            fp = os.path.join(root, f)
            rel = os.path.relpath(fp, REPO).replace("\\", "/")
            try: mtime = os.path.getmtime(fp)
            except OSError: continue
            rel_no_src = rel[4:] if rel.startswith("src/") else rel
            base_elf = os.path.join(REPO, "build", "GC6E01", "base", rel_no_src[:-2] + ".o")
            try: bmtime = os.path.getmtime(base_elf)
            except OSError: bmtime = 0
            cache_key = (fp, mtime, bmtime)
            cached = _scan_cache.get(rel)
            if cached and cached[0] == cache_key:
                cached_entry, fns, rows = cached[1]
                entry = dict(cached_entry)
                results[rel] = entry
                matched_fns.update(fns)
                matched_by_file[rel] = list(fns)
                function_rows.extend(dict(row) for row in rows)
                seen_symbols.update(row["function"] for row in rows)
                continue
            asm_active = 0
            c_matched = 0
            c_near = 0
            c_unmatched = 0
            local_matched_fns = []
            local_rows = []
            try:
                with open(fp, "r", encoding="utf-8", errors="replace") as fh:
                    lines = fh.readlines()
            except OSError:
                continue
            i = 0
            while i < len(lines):
                s = lines[i].strip()
                if s == "#if 1" and i+1 < len(lines):
                    fn = _asm_wrapper_symbol(lines[i+1])
                    if fn:
                        asm_active += 1
                        local_rows.append({
                            "file": rel, "function": fn,
                            "status": "pending_asm",
                            "match_percent": None,
                        })
                        seen_symbols.add(fn)
                        i += 2; continue
                elif s == "#if 0" and i+1 < len(lines):
                    fn = _asm_wrapper_symbol(lines[i+1])
                    if fn:
                        pct = _c_block_match_percent(target_elf, base_elf, fn)
                        if pct >= 100.0:
                            c_matched += 1
                            local_matched_fns.append(fn)
                            status = "complete_100"
                        elif pct >= 90.0:
                            c_near += 1
                            status = "candidate_90_99"
                        else:
                            c_unmatched += 1
                            status = "active_c_unmatched"
                        local_rows.append({
                            "file": rel, "function": fn,
                            "status": status,
                            "match_percent": pct,
                        })
                        seen_symbols.add(fn)
                        i += 2; continue
                i += 1
            total = asm_active + c_matched + c_near + c_unmatched
            if total > 0:
                entry = {
                    "asm": asm_active + c_near + c_unmatched,
                    "matched": c_matched,
                    "near": c_near,
                    "pending_asm": asm_active,
                    "active_c_unmatched": c_unmatched,
                }
                _scan_cache[rel] = (
                    cache_key,
                    (dict(entry), list(local_matched_fns), [dict(row) for row in local_rows]),
                )
                results[rel] = entry
                matched_fns.update(local_matched_fns)
                matched_by_file[rel] = list(local_matched_fns)
                function_rows.extend(local_rows)

    progress = _load_json(PROGRESS_FILE, {"completed": [], "failed": [], "in_progress": []})
    catalog = load_function_catalog()
    for fn in progress.get("completed", []):
        if fn in seen_symbols:
            continue
        meta = catalog.get(fn)
        if not meta:
            continue
        rel = meta["file"]
        entry = results.setdefault(rel, {
            "asm": 0, "matched": 0, "near": 0,
            "pending_asm": 0, "active_c_unmatched": 0,
        })
        entry["matched"] += 1
        matched_fns.add(fn)
        matched_by_file.setdefault(rel, []).append(fn)
        function_rows.append({
            "file": rel, "function": fn,
            "status": "progress_completed",
            "match_percent": 100.0,
            "tier": meta.get("tier", "?"),
        })
        seen_symbols.add(fn)

    for key, status in (("failed", "failed"), ("in_progress", "in_progress")):
        for fn in progress.get(key, []):
            if fn in seen_symbols:
                continue
            meta = catalog.get(fn)
            if not meta:
                continue
            rel = meta["file"]
            entry = results.setdefault(rel, {
                "asm": 0, "matched": 0, "near": 0,
                "pending_asm": 0, "active_c_unmatched": 0,
            })
            entry["asm"] += 1
            function_rows.append({
                "file": rel, "function": fn,
                "status": status,
                "match_percent": None,
                "tier": meta.get("tier", "?"),
            })
            seen_symbols.add(fn)

    return results, matched_fns, matched_by_file, function_rows


def load_match_history():
    try:
        with open(MATCH_HISTORY_FILE) as f:
            return json.load(f)
    except (OSError, ValueError):
        return {"matched": [], "regressions": []}


def save_match_history(h):
    try:
        with open(MATCH_HISTORY_FILE, "w") as f:
            json.dump(h, f, indent=1)
    except OSError:
        pass


def save_dashboard_state(stats, function_rows, stable_set):
    """Write a complete machine-readable snapshot for agents and scans."""
    from datetime import datetime, timezone

    files = {}
    src_root = os.path.join(REPO, "src")
    for root, _, names in os.walk(src_root):
        for name in names:
            if not name.endswith(".c"):
                continue
            rel = os.path.relpath(os.path.join(root, name), REPO).replace("\\", "/")
            data = stats.get("files", {}).get(rel, {})
            matched = int(data.get("matched", 0))
            remaining = int(data.get("asm", 0))
            near = int(data.get("near", 0))
            files[rel] = {
                "matched": matched,
                "remaining": remaining,
                "near": near,
                "total": matched + remaining,
                "functions": [],
            }

    for row in function_rows:
        rel = row.get("file")
        fn = row.get("function")
        if not rel or not fn:
            continue
        display = row.get("status", "unknown")
        if display == "complete_100" and fn not in stable_set:
            display = "complete_pending_stability"
        item = dict(row)
        item["display_status"] = display
        files.setdefault(rel, {
            "matched": 0, "remaining": 0, "near": 0,
            "total": 0, "functions": [],
        })["functions"].append(item)

    queue = _load_json(WORK_QUEUE_FILE, [])
    queue_status = {}
    if isinstance(queue, list):
        for item in queue:
            status = item.get("status", "unknown")
            queue_status[status] = queue_status.get(status, 0) + 1

    progress = _load_json(PROGRESS_FILE, {"completed": [], "failed": [], "in_progress": []})
    state = {
        "generated_at": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "totals": {
            "source_files": len(files),
            "tracked_files": len([f for f in files.values() if f["total"] > 0]),
            "tracked_functions": sum(f["total"] for f in files.values()),
            "matched": stats.get("tm", 0),
            "remaining": stats.get("ta", 0),
            "near": stats.get("tn", 0),
        },
        "progress": {
            "completed": len(progress.get("completed", [])),
            "failed": len(progress.get("failed", [])),
            "in_progress": len(progress.get("in_progress", [])),
        },
        "queue": {
            "total": len(queue) if isinstance(queue, list) else 0,
            "by_status": queue_status,
        },
        "files": files,
    }

    try:
        os.makedirs(REPORTS_DIR, exist_ok=True)
        with open(DASHBOARD_STATE_FILE, "w", encoding="utf-8") as f:
            json.dump(state, f, indent=2, sort_keys=True)
    except OSError:
        pass


def load_dashboard_state_for_render(max_age_s=3600):
    try:
        age = time.time() - os.path.getmtime(DASHBOARD_STATE_FILE)
    except OSError:
        return None
    if age > max_age_s:
        return None
    state = _load_json(DASHBOARD_STATE_FILE, {})
    totals = state.get("totals", {})
    files = {}
    for rel, data in state.get("files", {}).items():
        total = int(data.get("total", 0))
        if total <= 0:
            continue
        files[rel] = {
            "asm": int(data.get("remaining", 0)),
            "matched": int(data.get("matched", 0)),
            "near": int(data.get("near", 0)),
        }
    _rm = _load_report_measures()
    _tm = _rm["matched"] if _rm else int(totals.get("matched", 0))
    _ta = max(0, _rm["total"] - _rm["matched"]) if _rm else int(totals.get("remaining", 0))
    return {
        "ta": _ta,
        "tm": _tm,
        "tn": int(totals.get("near", 0)),
        "files": files,
        "review": count_json(REVIEW_DIR),
        "applied": count_json(APPLIED_DIR),
        "coord": get_coord(),
        "status_lines": get_status(25),
        "codex": get_codex(),
        "regressions_recent": [],
        "stale_objs": 0,
        "missing_objs": 0,
        "tokens": load_agent_tokens(),
        "source_files": int(totals.get("source_files", count_source_files())),
        "tracked_files": int(totals.get("tracked_files", len(files))),
    }


def count_json(d):
    try: return len([f for f in os.listdir(d) if f.endswith(".json")])
    except OSError: return 0

def count_source_files():
    n = 0
    for _, _, files in os.walk(os.path.join(REPO, "src")):
        n += len([f for f in files if f.endswith(".c")])
    return n

def get_coord():
    try:
        with open(os.path.join(COORD_DIR, "tasks.json")) as f:
            tasks = json.load(f)
        return (len([t for t in tasks if t["status"]=="queued"]),
                len([t for t in tasks if t["status"]=="claimed"]),
                len([t for t in tasks if t["status"]=="completed"]))
    except: return 0, 0, 0

def _utc_to_hst(ts_str):
    import re as _re
    m = _re.match(r'(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})Z?', ts_str)
    if not m: return ts_str
    from datetime import datetime, timedelta
    dt = datetime(int(m[1]), int(m[2]), int(m[3]), int(m[4]), int(m[5]), int(m[6]))
    dt_hst = dt - timedelta(hours=10)
    return dt_hst.strftime('%m/%d %I:%M%p HST')

def get_status(n=3):
    try:
        import re as _re
        with open(os.path.join(COORD_DIR, "status.md"), encoding="utf-8", errors="replace") as f:
            lines = []
            for l in f:
                s = l.strip()
                if s.startswith("- **"):
                    s = s.replace('\u2014', ' - ').replace('\u2013', ' - ')
                    s = s.replace('\u2018', "'").replace('\u2019', "'")
                    ts_match = _re.search(r'\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z?', s)
                    if ts_match:
                        s = s.replace(ts_match.group(), _utc_to_hst(ts_match.group()))
                    lines.append(s)
            return lines[-n:]
    except: return []

def get_codex():
    try:
        s = os.path.join(TMUX_DIR, "codex_is_idle.sh")
        if not os.path.isfile(s): return "?"
        r = subprocess.run(["bash", s, "--quiet"], capture_output=True, timeout=3)
        return "IDLE" if r.returncode == 0 else "BUSY"
    except: return "?"


# ─── Stale-object detection (catches the "phantom regression" failure mode) ──
_stale_cache = {"ts": 0, "stale": [], "missing": []}
def find_stale_objects(max_age_s=10):
    """Return (stale_files, missing_files) where compiled .o is older than .c source.
    Cached for max_age_s seconds to avoid re-walking on every refresh tick."""
    now = time.time()
    if now - _stale_cache["ts"] < max_age_s:
        return _stale_cache["stale"], _stale_cache["missing"]
    stale, missing = [], []
    for root, _, files in os.walk(os.path.join(REPO, "src")):
        for f in files:
            if not f.endswith(".c"): continue
            sp = os.path.join(root, f)
            rel = os.path.relpath(sp, REPO).replace("\\", "/")
            rel_no_src = rel[4:] if rel.startswith("src/") else rel
            obj = os.path.join(REPO, "build", "GC6E01", "base", rel_no_src[:-2] + ".o")
            try:
                src_t = os.path.getmtime(sp)
            except OSError: continue
            if not os.path.exists(obj):
                missing.append(rel)
            else:
                if src_t > os.path.getmtime(obj):
                    stale.append(rel)
    _stale_cache["ts"] = now
    _stale_cache["stale"] = stale
    _stale_cache["missing"] = missing
    return stale, missing


# ─── Agent attribution via git log (maps fn name to last committer / message) ──
_attr_cache = {"ts": 0, "by_fn": {}}
def get_recent_attributions(max_age_s=30, n=300):
    """Parse recent git commits to map fn names -> agent label.
    Agent inferred from commit message keywords + author. Cached briefly."""
    now = time.time()
    if now - _attr_cache["ts"] < max_age_s:
        return _attr_cache["by_fn"]
    by_fn = {}
    try:
        r = subprocess.run(
            ["git", "log", f"-n{n}", "--pretty=format:%h|%an|%s", "--", "src/"],
            capture_output=True, text=True, timeout=5, cwd=REPO,
        )
        if r.returncode == 0:
            import re as _re
            for line in r.stdout.splitlines():
                parts = line.split("|", 2)
                if len(parts) != 3: continue
                sha, author, subj = parts
                # Extract fn name from subject like "gs_title: fn_80024160 96.5% byte-match"
                m = _re.search(r"fn_[0-9A-Fa-f]{8}", subj)
                if not m: continue
                fn = m.group(0)
                # Infer agent label from subject content. Order matters —
                # check more specific tags first. Subjects from this overnight
                # session always include "via <agent>" trailer for clean tagging.
                lower = subj.lower()
                if "via deepseek" in lower or " deepseek-v4" in lower or "v4-flash" in lower:
                    agent = "deepseek"
                elif "via codex" in lower or "codex" in lower or "improve" in lower:
                    agent = "codex"
                elif "via opus-sub" in lower or "opus-sub" in lower or "opus" in lower or "from-scratch" in lower:
                    agent = "opus-sub"
                elif "via kimi" in lower or "kimi" in lower or "sisyphus" in lower:
                    agent = "kimi"
                elif "via qwen" in lower or "qwen" in lower:
                    agent = "qwen-3090"
                else:
                    agent = author.split()[0].lower() if author else "?"
                # Extract route stem from subject ("gs_worldmap: fn_X 91% via codex")
                stem_m = _re.match(r"\s*([a-zA-Z0-9_]+)\s*[:.]", subj)
                route = stem_m.group(1) if stem_m else "?"
                if fn not in by_fn:
                    by_fn[fn] = (agent, sha, subj, route)
    except Exception: pass
    _attr_cache["ts"] = now
    _attr_cache["by_fn"] = by_fn
    return by_fn


def attribute_fn(fn):
    """Return agent label for a fn — git-attributed if committed, otherwise
    the SPECIFIC busy worker name (moonshot-kimi-1, qwen-3090, opus-sub-3, etc)
    so multi-instance workers are distinguishable in the activity log."""
    rec = get_recent_attributions().get(fn)
    if rec:
        return rec[0]  # Short tag from commit subject
    # Fallback: any agent currently working this fn — return FULL name
    tokens = load_agent_tokens()
    for name, a in tokens.get("agents", {}).items():
        cf = (a.get("current_fn", "") or "")
        if cf and cf.split()[0].split("(")[0].strip() == fn:
            return name
    return "?"


_route_cache = {"ts": 0, "by_fn": {}}
def attribute_route(fn):
    """Return file stem (e.g. 'gs_material') the fn lives in. Tries:
    1) Recent git commit subjects (fast)
    2) tasks.json meta.stem for the fn (covers in-progress fns)
    """
    rec = get_recent_attributions().get(fn)
    if rec and len(rec) >= 4 and rec[3] != "?":
        return rec[3]
    # Fallback: look up in tasks.json (cached briefly)
    now = time.time()
    if now - _route_cache["ts"] > 30:
        try:
            tasks = json.loads(open(
                os.path.join(REPO, "tools/decomp_work/coordination/tasks.json"),
                encoding="utf-8").read())
            _route_cache["by_fn"] = {
                t["function"]: t.get("meta", {}).get("stem", "?")
                for t in tasks if t.get("function")
            }
            _route_cache["ts"] = now
        except (OSError, ValueError):
            pass
    return _route_cache["by_fn"].get(fn, "?")


# ─── Agent token usage tracking (per-harness budgets) ─────────────────────────
_tokens_cache = {"ts": 0, "data": {}}
def load_agent_tokens(max_age_s=45):
    """Read .omc/agent_tokens.json with format:
    {"agents": {"name": {"tokens_used": N, "limit": M, "since": "ts", "current_fn": "fn_X", "status": "busy/idle"}}}
    """
    now = time.time()
    if now - _tokens_cache["ts"] < max_age_s:
        return _tokens_cache["data"]
    try:
        with open(AGENT_TOKENS_FILE) as f:
            data = json.load(f)
    except (OSError, ValueError):
        data = {"agents": {}}
    _tokens_cache["ts"] = now
    _tokens_cache["data"] = data
    return data


def fmt_tokens(n):
    """Format token count compactly: 1234 -> '1.2K', 1234567 -> '1.2M'."""
    if n >= 1_000_000: return f"{n/1_000_000:.1f}M"
    if n >= 1_000: return f"{n/1_000:.1f}K"
    return str(n)


# ─── CODEX + CLAUDE daily token usage (lazy/async, never blocks render) ──────
#
# CODEX: codex CLI rollout session files (~/.codex/sessions/YYYY/MM/DD/
# rollout-*.jsonl) carry "token_count" events with cumulative per-session
# total_token_usage AND rate-limit snapshots (primary=5h window,
# secondary=weekly). We tail-read the last such event per today's session.
#
# CLAUDE: Claude Code writes transcripts as JSONL under
# <home>/.claude/projects/<munged-repo-path>/; each assistant message has a
# usage object. We sum files whose mtime is today, deduping by message id.
#
# Both readers run in a background thread (or inline for --once) and the
# result is cached in-process + on disk (reports/) with a 60s TTL.

USAGE_TTL = 60.0
# Per-platform cache file: WSL and Windows see different sources (~/.codex
# only exists inside WSL), so they must not share a snapshot.
USAGE_CACHE_FILE = os.path.join(
    REPORTS_DIR, f"agent_usage_cache.{sys.platform}.json")
_usage_cache = {"ts": 0.0, "data": None, "thread": None}


def _last_json_line_with(path, needle, tail_bytes=262144):
    """Last JSON line in `path` containing `needle` (tail-read only)."""
    try:
        with open(path, "rb") as f:
            f.seek(0, 2)
            size = f.tell()
            f.seek(max(0, size - tail_bytes))
            data = f.read().decode("utf-8", "replace")
    except OSError:
        return None
    for line in reversed(data.splitlines()):
        if needle in line:
            try:
                return json.loads(line)
            except ValueError:
                continue
    return None


def _codex_sessions_dirs():
    cands = [os.path.expanduser("~/.codex/sessions")]
    if sys.platform == "win32":
        import glob as _glob
        try:
            cands += _glob.glob(r"\\wsl.localhost\*\home\*\.codex\sessions")
            cands += _glob.glob(r"\\wsl$\*\home\*\.codex\sessions")
        except Exception:
            pass
    return [c for c in cands if os.path.isdir(c)]


def _read_codex_usage():
    """Today's codex usage + latest weekly/5h rate-limit snapshot, or None."""
    from datetime import date
    today = date.today()
    out = {"in": 0, "out": 0, "sessions": 0,
           "weekly_pct": None, "p5h_pct": None}
    found = False
    for root in _codex_sessions_dirs():
        day_dir = os.path.join(
            root, f"{today.year:04d}", f"{today.month:02d}", f"{today.day:02d}")
        try:
            names = sorted(os.listdir(day_dir))
        except OSError:
            continue
        newest_mtime = -1.0
        for n in names:
            if not (n.startswith("rollout-") and n.endswith(".jsonl")):
                continue
            p = os.path.join(day_dir, n)
            last = _last_json_line_with(p, '"token_count"')
            if not last:
                continue
            payload = last.get("payload") or {}
            info = payload.get("info") or {}
            tot = info.get("total_token_usage") or {}
            out["in"] += int(tot.get("input_tokens", 0) or 0)
            out["out"] += int(tot.get("output_tokens", 0) or 0)
            out["sessions"] += 1
            found = True
            rl = payload.get("rate_limits") or {}
            try:
                mt = os.path.getmtime(p)
            except OSError:
                mt = 0.0
            if rl and mt >= newest_mtime:
                newest_mtime = mt
                pri = rl.get("primary") or {}
                sec = rl.get("secondary") or {}
                out["p5h_pct"] = pri.get("used_percent")
                out["weekly_pct"] = sec.get("used_percent")
        if found:
            return out
    return None


def _claude_project_dirs():
    """Claude Code transcript dirs for THIS repo. The JSONL lives on the
    Windows side; under WSL the repo path is already /mnt/c/... so deriving
    the home dir from REPO's parent works in both environments."""
    dirs = []
    repo_name = os.path.basename(REPO)
    bases = [
        os.path.join(os.path.dirname(REPO), ".claude", "projects"),
        os.path.expanduser("~/.claude/projects"),
    ]
    seen = set()
    for base in bases:
        try:
            entries = os.listdir(base)
        except OSError:
            continue
        for e in entries:
            if e.endswith("-" + repo_name):
                p = os.path.join(base, e)
                key = os.path.normcase(os.path.normpath(p))
                if key not in seen and os.path.isdir(p):
                    seen.add(key)
                    dirs.append(p)
    return dirs


def _read_claude_usage():
    """Sum usage objects from today's (mtime) transcript JSONLs, or None."""
    from datetime import date, datetime
    today = date.today()
    out = {"in": 0, "out": 0, "files": 0}
    found = False
    for d in _claude_project_dirs():
        try:
            names = os.listdir(d)
        except OSError:
            continue
        for n in names:
            if not n.endswith(".jsonl"):
                continue
            p = os.path.join(d, n)
            try:
                if datetime.fromtimestamp(os.path.getmtime(p)).date() != today:
                    continue
            except OSError:
                continue
            seen_ids = set()
            try:
                with open(p, encoding="utf-8", errors="replace") as f:
                    for line in f:
                        if '"usage"' not in line:
                            continue
                        try:
                            rec = json.loads(line)
                        except ValueError:
                            continue
                        msg = rec.get("message") or {}
                        usage = msg.get("usage")
                        if not isinstance(usage, dict):
                            continue
                        mid = msg.get("id") or rec.get("requestId") or rec.get("uuid")
                        if mid is not None:
                            if mid in seen_ids:
                                continue
                            seen_ids.add(mid)
                        out["in"] += int(usage.get("input_tokens", 0) or 0)
                        out["in"] += int(usage.get("cache_creation_input_tokens", 0) or 0)
                        out["in"] += int(usage.get("cache_read_input_tokens", 0) or 0)
                        out["out"] += int(usage.get("output_tokens", 0) or 0)
                        found = True
            except OSError:
                continue
            out["files"] += 1
    return out if found else None


def _collect_agent_usage():
    data = {
        "codex": _read_codex_usage(),
        "claude": _read_claude_usage(),
        "at": time.time(),
    }
    try:
        os.makedirs(REPORTS_DIR, exist_ok=True)
        with open(USAGE_CACHE_FILE, "w", encoding="utf-8") as f:
            json.dump(data, f)
    except OSError:
        pass
    return data


def get_agent_usage(block=False):
    """Cached codex+claude usage snapshot. Never blocks the render loop:
    stale/missing data kicks a background refresh and returns the last
    snapshot (or None for the 'measuring' placeholder). --once renders
    block inline so a single frame still has real data."""
    now = time.time()
    if _usage_cache["data"] is not None and now - _usage_cache["ts"] < USAGE_TTL:
        return _usage_cache["data"]
    if _usage_cache["data"] is None:
        # Cross-run disk cache (same TTL) so cold starts paint instantly.
        try:
            mt = os.path.getmtime(USAGE_CACHE_FILE)
            if now - mt < USAGE_TTL:
                d = _load_json(USAGE_CACHE_FILE, None)
                if isinstance(d, dict):
                    _usage_cache["data"] = d
                    _usage_cache["ts"] = mt
                    return d
        except OSError:
            pass
    if block:
        try:
            data = _collect_agent_usage()
        except Exception:
            data = {"codex": None, "claude": None}
        _usage_cache["data"] = data
        _usage_cache["ts"] = time.time()
        return data
    th = _usage_cache.get("thread")
    if th is None or not th.is_alive():
        def _bg():
            try:
                data = _collect_agent_usage()
            except Exception:
                data = {"codex": None, "claude": None}
            _usage_cache["data"] = data
            _usage_cache["ts"] = time.time()
        th = threading.Thread(target=_bg, daemon=True)
        _usage_cache["thread"] = th
        th.start()
    return _usage_cache["data"]  # possibly stale or None — never blocks

MILESTONES = {60:"BADGE 6",70:"BADGE 7",80:"BADGE 8",90:"CHAMPION"}
def load_ms():
    try:
        with open(MILE_FILE) as f: return json.load(f)
    except: return {"triggered":[]}
def save_ms(d):
    try:
        os.makedirs(os.path.dirname(MILE_FILE), exist_ok=True)
        with open(MILE_FILE,"w") as f: json.dump(d,f)
    except: pass

TOP = [
    "src/game/gs_field_world.c","src/game/scene_init.c",
    "src/game/gs_render.c","src/game/effect/effect_util.c",
    "src/game/people/people_field.c","src/game/gs_worldmap.c",
    "src/game/gs_title.c","src/game/gs_thread.c",
    "src/game/ui/ui_core.c","src/game/effect/effect_visual.c",
    "src/hsd/hsd_cobj.c","src/game/gs_party_access.c",
]

MSGS = [
    "Gotta decompile 'em all!",
    "A wild function appeared!",
    "Used DECOMP... It's super effective!",
    "Professor Oak: Your Pokédex grows...",
    "Trainer CLAUDE used MATCH ATTACK!",
    "Codex used CLAIM TASK!",
    "Critical hit on that function match!",
    "Wild ASM fainted! Gained C code!",
    "Your Pokédex is being updated...",
    "Elite Four of decomp: GX, OS, HSD, Game",
]

# ─── Right panel rendering ───────────────────────────────────────────────────
#
# Animated bars: when a match happens in a file, a spinning ball rolls left
# along the empty portion of its bar until it reaches the filled edge, where
# it lingers briefly before vanishing. Keyed by file path (or "__total__").

_bar_anims = {}  # key -> {"pos": float (ratio 0..1), "target": float, "ttl": int}

SPIN_CHARS = "oOo0"

def trigger_bar_anim(key, target_ratio):
    _bar_anims[key] = {"pos": 0.0, "target": max(0.01, target_ratio), "ttl": 10}

def tick_bar_anims():
    for k in list(_bar_anims):
        a = _bar_anims[k]
        if a["pos"] < a["target"]:
            a["pos"] += 0.04
            if a["pos"] >= a["target"]:
                a["pos"] = a["target"]
        else:
            a["ttl"] -= 1
            if a["ttl"] <= 0:
                del _bar_anims[k]

def bar(ratio, w=28, key=None, frame=0):
    filled = int(ratio * w)
    if ratio >= 0.7: bc = G92 if ratio >= 1.0 else C96
    elif ratio >= 0.4: bc = Y93
    else: bc = R91
    fill_str = "█" * filled
    empty_str = "░" * (w - filled)

    # All spans emitted with BLD. Colored-without-bold rows trigger
    # double-width rendering in tmux + Windows cmd.
    if key and key in _bar_anims:
        ax = int(_bar_anims[key]["pos"] * w)
        spin = SPIN_CHARS[frame % len(SPIN_CHARS)]
        if 0 <= ax < filled:
            return (_c(fill_str[:ax], bc, BLD) + _c(spin, Y93, BLD) +
                    _c(fill_str[ax+1:], bc, BLD) + _c(empty_str, GR37, BLD))
        elif filled <= ax < w:
            ei = ax - filled
            return (_c(fill_str, bc, BLD) + _c(empty_str[:ei], GR37, BLD) +
                    _c(spin, Y93, BLD) + _c(empty_str[ei+1:], GR37, BLD))
    return _c(fill_str, bc, BLD) + _c(empty_str, GR37, BLD)

SOUND_ON = os.environ.get("POKDEX_SOUND", "1") != "0" and HAS_SOUND

def _play_levelup_sound():
    if not SOUND_ON: return
    def _play():
        try:
            for freq in [523, 659, 784, 1047]:
                winsound.Beep(freq, 100)
            winsound.Beep(1047, 200)
        except Exception: pass
    threading.Thread(target=_play, daemon=True).start()

def _play_milestone_sound():
    if not SOUND_ON: return
    def _play():
        try:
            for freq, dur in [(523,80),(659,80),(784,80),(1047,120),(1319,120),(1568,250)]:
                winsound.Beep(freq, dur)
        except Exception: pass
    threading.Thread(target=_play, daemon=True).start()

def _play_regression_sound():
    """Sad descending tone for regressions."""
    if not SOUND_ON: return
    def _play():
        try:
            for freq in [523, 440, 330, 262]:
                winsound.Beep(freq, 120)
        except Exception: pass
    threading.Thread(target=_play, daemon=True).start()

RAIN_CHARS = ["*", "+", ".", "'", "~", "o", "x", "."]

def _bar_color_for_ratio(ratio):
    if ratio >= 1.0: return G92
    elif ratio >= 0.7: return C96
    elif ratio >= 0.4: return Y93
    else: return R91

class SparkleRain:
    def __init__(self):
        self.particles = []
        self.active = False
        self.ttl = 0

    def trigger(self, width, height, color=None):
        self.active = True
        self.ttl = 40
        rain_color = color or Y93
        self._rain_color = rain_color
        colors = [rain_color] * 7 + [W97] * 3
        rng = random.Random()
        for _ in range(random.randint(30, 50)):
            x = rng.randint(0, width - 1)
            y = rng.uniform(-5.0, 0.0)
            speed = rng.uniform(0.3, 1.2)
            self.particles.append([x, y, speed, rng.choice(RAIN_CHARS), rng.choice(colors)])

    def tick(self, height):
        if not self.active: return False
        self.ttl -= 1
        alive = []
        for p in self.particles:
            p[1] += p[2]
            if p[1] < height + 2: alive.append(p)
        self.particles = alive
        if self.ttl <= 0 and not self.particles:
            self.active = False
            return False
        if self.ttl > 0:
            rng = random.Random()
            color = getattr(self, '_rain_color', Y93)
            colors = [color] * 7 + [W97] * 3
            for _ in range(random.randint(2, 5)):
                x = rng.randint(0, getattr(self, 'last_width', 80) - 1)
                self.particles.append([x, rng.uniform(-2.0, 0.0),
                    rng.uniform(0.3, 1.2), rng.choice(RAIN_CHARS), rng.choice(colors)])
        return True

    def overlay(self, lines, tw):
        if not self.active: return
        self.last_width = tw
        for p in self.particles:
            x, y = int(p[0]), int(p[1])
            if 0 <= y < len(lines) and 0 <= x < tw:
                row = list(lines[y])
                if x < len(row) and row[x] == " ":
                    row[x] = _c(p[3], p[4], BLD)
                    lines[y] = "".join(row)

_rain = SparkleRain()

# ─── Battle scene ────────────────────────────────────────────────────────────

# User's own Pokemon "PIKAPI" — rendered from backsprite.png via convert_sprite.py.
# See sprite_data.py for SPRITE_ROWS: list of rows of (fg256, bg256) tuples.

# Simple fallback glitch creature if sprite_data missing
GLITCH_FRONT = [
    "# .## .@# #.",
    " #  @.  #+ #",
    "+ ##. .@# . ",
    "#@  +.# .+@#",
    " #. @.+ #.  ",
    "+ #.@ .#+.@ ",
]

FILE_POKEMON = {
    "src/game/gs_field_world.c":    "FIELDWALKER",
    "src/game/scene_init.c":        "SCENEBEAST",
    "src/game/gs_render.c":         "RENDERVOLT",
    "src/game/effect/effect_util.c":"FXWISP",
    "src/game/people/people_field.c":"PEOPLEMON",
    "src/game/gs_worldmap.c":       "MAPDRAKE",
    "src/game/gs_title.c":          "TITLORUS",
    "src/game/gs_thread.c":         "THREADER",
    "src/game/ui/ui_core.c":        "UIGEIST",
    "src/game/effect/effect_visual.c":"VIZFLAME",
    "src/hsd/hsd_cobj.c":           "CAMERON",
    "src/game/gs_party_access.c":   "PARTYCHU",
}

# Name of the trainer's Pokemon displayed on the left/bottom (our side)
OUR_POKEMON_NAME = "PIKAPI"


def _render_pixel_sprite(rows):
    """Shared pixel-sprite renderer. Takes SPRITE_ROWS-style data (list of
    rows of (top_color256, bot_color256) tuples, None for transparent).
    Returns list of colored strings — one per cell-row.

    Emits contiguous bold+color runs without per-cell reset. This matches
    the ANSI pattern that renders correctly in tmux + Windows cmd.
    """
    out = []
    for row in rows:
        parts = []
        run_chars = []
        run_code = None
        def flush():
            if not run_chars: return
            text = "".join(run_chars)
            run_chars.clear()
            if NO_COLOR or run_code is None:
                parts.append(text)
            else:
                parts.append(f"{BLD}\033[{run_code}m{text}{RST}")
        for top, bot in row:
            if top is None and bot is None:
                flush()
                run_code = None
                parts.append(" ")
                continue
            if top is None:
                code = _ansi16_code_from_256(bot); ch = "▄"
            elif bot is None:
                code = _ansi16_code_from_256(top); ch = "▀"
            else:
                code = _ansi16_code_from_256(top); ch = "█"
            if code != run_code:
                flush()
                run_code = code
            run_chars.append(ch)
        flush()
        out.append("".join(parts))
    return out


def render_pikapi_sprite():
    """Render PIKAPI from sprite_data as a list of colored strings."""
    if not SPRITE_ROWS:
        # Fallback: blocky PIKAPI
        return [_c(r, Y93, BLD) for r in [
            "    ▄▀▀▀▀▄     ",
            "   █      █    ",
            "  █  ●  ●  █   ",
            "  █    ▼    █  ",
            "   █▄▄▄▄▄▄█    ",
            "    ██████     ",
        ]]

    return _render_pixel_sprite(SPRITE_ROWS)


def render_enemy_sprite():
    """Render enemy (MissingNo) sprite from enemy_sprite_data."""
    if not ENEMY_ROWS:
        return None
    return _render_pixel_sprite(ENEMY_ROWS)


PIKAPI_SPRITE = render_pikapi_sprite()
PIKAPI_SPRITE_W = SPRITE_W if SPRITE_ROWS else 15
PIKAPI_SPRITE_H = len(PIKAPI_SPRITE)

ENEMY_SPRITE = render_enemy_sprite()
ENEMY_SPRITE_W = ENEMY_W if ENEMY_ROWS else 0
ENEMY_SPRITE_H = len(ENEMY_SPRITE) if ENEMY_SPRITE else 0


class BattleScene:
    def __init__(self):
        self.active_file = None
        self.active_name = OUR_POKEMON_NAME
        self.our_level = 0
        self.enemy_hp_pct = 100.0
        self.exp_matched = 0
        self.exp_total = 1
        self.battle_text = "Wild MISSINGNO appeared!"
        self.text_ttl = 0
        self.attack_frame = 0
        self.evolving = False
        self.evolve_ttl = 0
        self.shake_ttl = 0  # damage-taken shake (on regression)

    def update(self, stats, matched_file=None, matched_delta=0, regression_file=None):
        ta = stats["ta"]; tm = stats["tm"]; tot = ta + tm
        decomp_pct = (tm / tot * 100) if tot > 0 else 0
        self.enemy_hp_pct = 100.0 - decomp_pct

        if regression_file:
            # A previously matched function regressed — PIKAPI takes damage
            self.shake_ttl = 12
            self.battle_text = f"{OUR_POKEMON_NAME} lost progress in {regression_file}!"
            self.text_ttl = 30
        elif matched_file:
            for fpath, name in FILE_POKEMON.items():
                if matched_file and fpath.endswith(matched_file.split("(")[0].strip()):
                    self.active_file = fpath
                    break
            else:
                self.active_file = None

            fdata = stats["files"].get(self.active_file) if self.active_file else None
            if fdata:
                ft = fdata["asm"] + fdata["matched"]
                self.our_level = int(fdata["matched"] / ft * 100) if ft > 0 else 0
                self.exp_matched = fdata["matched"]
                self.exp_total = ft
                if fdata["asm"] == 0 and ft > 0:
                    self.evolving = True
                    self.evolve_ttl = 20
            else:
                self.our_level = int(decomp_pct) if tot > 0 else 0
                self.exp_matched = tm
                self.exp_total = tot

            self.battle_text = f"{OUR_POKEMON_NAME} used DECOMP!"
            self.text_ttl = 30
            self.attack_frame = 12
        elif self.evolve_ttl > 0:
            self.evolve_ttl -= 1
            if self.evolve_ttl > 10:
                self.battle_text = f"{OUR_POKEMON_NAME} is evolving...!"
            elif self.evolve_ttl > 0:
                self.battle_text = f"{OUR_POKEMON_NAME} evolved! 100% MATCHED!"
            else:
                self.evolving = False
                self.battle_text = "Wild MISSINGNO appeared!"
        elif self.text_ttl > 0:
            self.text_ttl -= 1
            if self.text_ttl == 20:
                self.battle_text = "It's super effective!"
            elif self.text_ttl == 10:
                self.battle_text = f"MISSINGNO lost {matched_delta} HP!"
            elif self.text_ttl <= 0:
                self.battle_text = "Wild MISSINGNO appeared!"
        else:
            best_file = None
            best_matched = 0
            for fpath in FILE_POKEMON:
                fdata = stats["files"].get(fpath)
                if fdata and fdata["matched"] > best_matched:
                    best_matched = fdata["matched"]
                    best_file = fpath
            if best_file:
                self.active_file = best_file
                fdata = stats["files"][best_file]
                ft = fdata["asm"] + fdata["matched"]
                self.our_level = int(fdata["matched"] / ft * 100) if ft > 0 else 0
                self.exp_matched = fdata["matched"]
                self.exp_total = ft

        if self.attack_frame > 0: self.attack_frame -= 1
        if self.shake_ttl > 0: self.shake_ttl -= 1

    def render(self, width, height, frame):
        """Render battle scene, framed by a border. Width/height include the border."""
        lines = []
        inner_w = max(10, width - 2)   # space inside the border
        inner_h = max(6, height - 2)   # rows inside the border

        # Build the inner contents first, then wrap with a border.
        inner = self._render_inner(inner_w, inner_h, frame)

        # Border — single-line characters (reliably 1 cell wide in all terminals)
        top    = "┌" + "─" * inner_w + "┐"
        bot    = "└" + "─" * inner_w + "┘"
        left   = "│"
        right  = "│"

        lines.append(_c(top, C96, BLD))
        for row in inner[:inner_h]:
            # Visible length check: strip ANSI to measure
            vis_len = _visible_len(row)
            if vis_len < inner_w:
                row = row + " " * (inner_w - vis_len)
            elif vis_len > inner_w:
                row = _truncate_visible(row, inner_w)
            lines.append(_c(left, C96, BLD) + row + _c(right, C96, BLD))
        while len(lines) < 1 + inner_h:
            lines.append(_c(left, C96, BLD) + " " * inner_w + _c(right, C96, BLD))
        lines.append(_c(bot, C96, BLD))

        while len(lines) < height:
            lines.append("")
        return lines[:height]

    def _render_inner(self, bw, bh, frame):
        """Render battle inside the frame. Condensed layout with parenthetical
        definitions next to every stat so someone unfamiliar can read it."""
        our_sway = 1 if (frame // 12) % 3 == 1 else 0
        shake_dx = (1 if self.shake_ttl % 2 == 0 else -1) if self.shake_ttl > 0 else 0

        # ─── Enemy info header (2 rows) ───
        enemy_name = "MISSINGNO"
        enemy_hp = max(0, min(100, self.enemy_hp_pct))
        hp_bar_w = max(6, bw - 22)
        hp_filled = int((enemy_hp / 100) * hp_bar_w)
        hp_empty = hp_bar_w - hp_filled
        if enemy_hp > 50: hp_color = G92
        elif enemy_hp > 25: hp_color = Y93
        else: hp_color = R91

        info1_plain = f"{enemy_name}  Lv?? (asm remaining)"
        pad1 = " " * max(0, bw - len(info1_plain) - 1)
        head_row1 = " " + pad1 + _c(enemy_name, W97, BLD) + _c("  Lv??", GR37) + _c(" (asm remaining)", DM)

        info2_core = f"HP:" + "=" * hp_filled + "-" * hp_empty + f" {enemy_hp:.0f}% (unmatched)"
        pad2 = " " * max(0, bw - len(info2_core) - 1)
        head_row2 = (" " + pad2 + _c("HP:", GR37) + _c("=" * hp_filled, hp_color) +
                     _c("-" * hp_empty, DM) + _c(f" {enemy_hp:.0f}%", GR37) +
                     _c(" (unmatched)", DM))

        # ─── PIKAPI info + text box (fixed rows at bottom) ───
        info_pad = " " * (1 + our_sway)
        m = self.exp_matched
        ft = self.exp_total

        pikapi_info = []
        pikapi_info.append(info_pad + _c(f"{OUR_POKEMON_NAME}", W97, BLD) +
                           _c(f"  Lv{self.our_level}", GR37) +
                           _c(f" (file match %)", DM))
        pikapi_info.append(info_pad + _c(f"HP: {m}/{ft}", GR37) +
                           _c(f" (fns matched / total)", DM))

        exp_bar_w = max(6, min(18, bw - 22))
        exp_ratio = m / ft if ft > 0 else 0
        exp_filled = int(exp_ratio * exp_bar_w)
        exp_empty = exp_bar_w - exp_filled
        exp_color = G92 if exp_ratio >= 1.0 else (C96 if exp_ratio >= 0.7 else B94)
        pikapi_info.append(info_pad + _c("EXP:", GR37) +
                           _c("=" * exp_filled, exp_color, BLD) +
                           _c("-" * exp_empty, DM) +
                           _c(f" {exp_ratio*100:.0f}%", GR37) +
                           _c(" (to 100%)", DM))

        txt = f" {self.battle_text}"
        if len(txt) > bw - 2: txt = txt[:bw - 5] + "..."
        pad_r = bw - 2 - len(txt)
        textbox = [
            _c("┌" + "─" * (bw - 2) + "┐", GR37),
            _c("│", GR37) + _c(txt + " " * max(0, pad_r), W97) + _c("│", GR37),
            _c("└" + "─" * (bw - 2) + "┘", GR37),
        ]

        # ─── Overlay section: enemy (right) + PIKAPI (left) share rows ───
        # PIKAPI sway/lunge: on match, lunge forward 2 cells then bounce back.
        attack_lunge = 0
        if self.attack_frame > 0:
            # Lunge forward (right) for first half, retreat for second
            if self.attack_frame >= 6:
                attack_lunge = 2
            elif self.attack_frame >= 3:
                attack_lunge = 1
        sprite_pad = 1 + max(0, our_sway + shake_dx + attack_lunge)

        pikapi_rows = list(PIKAPI_SPRITE)
        if self.evolving and self.evolve_ttl > 10 and frame % 3 != 0:
            pikapi_rows = [_c("▓" * PIKAPI_SPRITE_W, Y93, BLD)] * PIKAPI_SPRITE_H
        elif self.attack_frame > 0 and self.attack_frame % 2 == 0:
            # Yellow flash on match — Pikachu's thunderbolt aura
            pikapi_rows = [_c("⚡" + ("▓" * (PIKAPI_SPRITE_W - 1)), Y93, BLD)] + list(PIKAPI_SPRITE)[1:]

        if ENEMY_SPRITE:
            enemy_rows = list(ENEMY_SPRITE)
            enemy_w = ENEMY_SPRITE_W
            if self.attack_frame > 0 and self.attack_frame % 2 == 0 and ENEMY_ROWS:
                rng = random.Random(frame)
                glitched = [[(t, b) if rng.random() > 0.4 else (None, None)
                             for t, b in r] for r in ENEMY_ROWS]
                enemy_rows = _render_pixel_sprite(glitched)
            elif self.shake_ttl > 0 and ENEMY_ROWS:
                # Regression — MISSINGNO attacks: red-tinted glitch + shimmer
                rng = random.Random(frame * 7)
                glitched = [
                    [(91 if rng.random() > 0.5 else t, 91 if rng.random() > 0.7 else b)
                     for t, b in r]
                    for r in ENEMY_ROWS
                ]
                enemy_rows = _render_pixel_sprite(glitched)
        else:
            enemy_rows, enemy_w = [], 0

        # Stack both so their BOTTOMS align at the same row (just above info).
        overlay_h = max(len(pikapi_rows), len(enemy_rows))
        overlay = []
        for i in range(overlay_h):
            p_i = i - (overlay_h - len(pikapi_rows))
            e_i = i - (overlay_h - len(enemy_rows))

            p = (" " * sprite_pad + pikapi_rows[p_i]) if 0 <= p_i < len(pikapi_rows) else ""
            p_vis = (sprite_pad + PIKAPI_SPRITE_W) if 0 <= p_i < len(pikapi_rows) else 0

            e = enemy_rows[e_i] if 0 <= e_i < len(enemy_rows) else ""
            e_vis = enemy_w if 0 <= e_i < len(enemy_rows) else 0

            gap = max(1, bw - p_vis - e_vis - 1)
            overlay.append(p + " " * gap + e)

        # Compose: 2 header + overlay + 3 info + 3 textbox
        head = [head_row1, head_row2]
        tail = pikapi_info + textbox
        avail_overlay = bh - len(head) - len(tail)
        if avail_overlay < len(overlay):
            overlay = overlay[-avail_overlay:] if avail_overlay > 0 else []

        middle_blanks = max(0, bh - len(head) - len(overlay) - len(tail))
        lines = list(head) + list(overlay) + [""] * middle_blanks + list(tail)
        return lines[:bh]


def _visible_len(s):
    """Length of a string ignoring ANSI escape sequences."""
    import re
    return len(re.sub(r"\033\[[0-9;]*[a-zA-Z]", "", s))


def _truncate_visible(s, n):
    """Truncate s to visible length n, keeping ANSI sequences intact."""
    import re
    out = []
    vis = 0
    i = 0
    while i < len(s) and vis < n:
        if s[i] == "\033":
            m = re.match(r"\033\[[0-9;]*[a-zA-Z]", s[i:])
            if m:
                out.append(m.group())
                i += len(m.group())
                continue
        out.append(s[i])
        vis += 1
        i += 1
    return "".join(out) + RST


_battle = BattleScene()
stats_ref = [None]

# ─── Compose vertical layout ────────────────────────────────────────────────

POKEMON_ART = [
    "█▀█ █▀█ █▀▄ █▀▀ █▀▄▀█ █▀█ █▀█",
    "█▀▀ █ █ █▀▄ ██▀ █ ▀ █ █ █ █ █",
    "▀   ▀▀▀ ▀ ▀ ▀▀▀ ▀   ▀ ▀▀▀ ▀ ▀",
]
DECOMP_ART = [
    "█▀▄ █▀▀ █▀▀ █▀█ █▀▄▀█ █▀█",
    "█ █ ██▀ █   █ █ █ ▀ █ █▀▀",
    "▀▀  ▀▀▀ ▀▀▀ ▀▀▀ ▀   ▀ ▀  ",
]
COLOSSEUM_ART = [
    "█▀▀ █▀█ █   █▀█ █▀▀ █▀▀ █▀▀ █ █ █▀▄▀█",
    "█   █ █ █   █ █ ▀▀█ ▀▀█ ██▀ █ █ █ ▀ █",
    "▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀   ▀",
]


def _pad_left_col(s, w):
    vis = _visible_len(s)
    if vis < w: return s + " " * (w - vis)
    if vis > w: return _truncate_visible(s, w)
    return s


def _build_title_lines(col_w):
    """Return title art (POKEMON / DECOMP / COLOSSEUM) centered in col_w.
    Includes 3 leading blank rows so the top row doesn't clip when the
    terminal scrolls up on taller panes."""
    lines = ["", "", ""]
    for art, color in [(POKEMON_ART, R91), (DECOMP_ART, Y93), (COLOSSEUM_ART, C96)]:
        for line in art:
            pad = max(0, (col_w - len(line)) // 2)
            if len(line) > col_w:
                lines.append(_c(line[:col_w], color, BLD))
            else:
                lines.append(" " * pad + _c(line, color, BLD))
        lines.append("")
    return lines


def _render_ball(ball_w, ball_h, frame):
    """Render the Poké Ball. Each grid cell is (char, color_code_str, bold)
    instead of a pre-wrapped _c() string. We compose each row as one
    contiguous ANSI region — per-cell \\033[0m resets trigger double-width
    rendering in tmux + Windows console and break frame alignment."""
    angle = frame * ROT_SPEED if not REDUCED else 0.0
    max_r_w = (ball_w - 4) // 2
    max_r_h = int((ball_h - 2) * 2.1)
    R = max(4, min(max_r_w, max_r_h, BALL_RADIUS))

    # grid[y][x] = (char, color_str or "", bold: bool) or None for blank
    grid = [[None] * ball_w for _ in range(ball_h)]
    cx = ball_w / 2.0
    cy = ball_h / 2.0
    aspect = 2.1
    cosYa, sinYa = math.cos(angle), math.sin(angle)
    wlx, wly, wlz = 0.4, -0.45, 0.8
    lm = math.sqrt(wlx*wlx + wly*wly + wlz*wlz)
    wlx /= lm; wly /= lm; wlz /= lm
    llx = wlx * cosYa - wlz * sinYa
    lly = wly
    llz = wlx * sinYa + wlz * cosYa

    for sy in range(ball_h):
        for sx in range(ball_w):
            nx = (sx - cx) / R
            ny = (sy - cy) / (R / aspect)
            r2 = nx * nx + ny * ny
            if r2 > 1.0: continue
            nz = math.sqrt(1.0 - r2)
            diff = max(0.0, nx * llx + ny * lly + nz * llz)
            dot_nl = nx * llx + ny * lly + nz * llz
            rz = 2 * dot_nl * nz - llz
            spec = max(0.0, rz) ** 10 * 0.5
            intensity = min(1.0, diff * 0.78 + spec + 0.1)
            edge = 1.0 - r2
            if edge < 0.2: intensity *= edge / 0.2
            si = max(0, min(int(intensity * (len(SHADE) - 1)), len(SHADE) - 1))
            ch = SHADE[si]
            wy = ny
            wz = -nx * sinYa + nz * cosYa
            seam_w = 0.07
            is_seam = abs(wy) < seam_w
            is_button = r2 < 0.035 and abs(wy) < seam_w * 2.5 and wz > 0.5
            is_btn_ring = r2 < 0.07 and r2 >= 0.035 and abs(wy) < seam_w * 2 and wz > 0.5
            is_upper = wy < -seam_w

            # Intensity-based palette. All cells bold — colored-without-bold
            # triggers double-width rendering in tmux + Windows cmd.
            if is_button:
                grid[sy][sx] = ("●" if intensity > 0.45 else "◎", W97, True)
            elif is_btn_ring:
                grid[sy][sx] = ("○" if intensity > 0.35 else "·", GR37, True)
            elif is_seam:
                grid[sy][sx] = ("═" if intensity > 0.25 else "─",
                                W97 if spec > 0.1 else GR37, True)
            elif is_upper:
                if spec > 0.2: grid[sy][sx] = (ch, Y93, True)
                elif intensity > 0.5: grid[sy][sx] = (ch, R91, True)
                else: grid[sy][sx] = (ch, R31, True)
            else:
                if spec > 0.2: grid[sy][sx] = (ch, C96, True)
                elif intensity > 0.5: grid[sy][sx] = (ch, W97, True)
                else: grid[sy][sx] = (ch, GR37, True)

    # Sparkles — add as tuples too
    rng = random.Random(frame // 3)
    color_off = frame % len(HOLO_C)
    count = int(ball_w * ball_h * SPARKLE_DENSITY)
    cx_s = ball_w / 2.0; cy_s = ball_h / 2.0; aspect_s = 2.1
    inner_margin = 3; outer_margin = 2
    for _ in range(count):
        sx = rng.randint(0, ball_w - 1)
        sy = rng.randint(0, ball_h - 1)
        if sx < outer_margin or sx >= ball_w - outer_margin: continue
        if sy < outer_margin or sy >= ball_h - outer_margin: continue
        nx = (sx - cx_s) / R
        ny = (sy - cy_s) / (R / aspect_s)
        dist = math.sqrt(nx * nx + ny * ny)
        if dist < 1.0 + inner_margin / R: continue
        if grid[sy][sx] is None:
            ch = rng.choice(SPARKLE_CH)
            ci = (color_off + rng.randint(0, 3)) % len(HOLO_C)
            grid[sy][sx] = (ch, HOLO_C[ci], rng.random() > 0.4)

    # Compose each row — one bold+color wrapper per contiguous same-color run.
    # Matches the title's ANSI pattern which tmux+cmd renders correctly.
    out_rows = []
    for row in grid:
        parts = []
        cur_color = None
        run = []
        def flush():
            if not run:
                return
            text = "".join(run)
            run.clear()
            if NO_COLOR or cur_color is None:
                parts.append(text)
            else:
                parts.append(f"{BLD}{cur_color}{text}{RST}")
        for cell in row:
            if cell is None:
                flush()
                cur_color = None
                parts.append(" ")
            else:
                ch, color, _bold = cell
                if color != cur_color:
                    flush()
                    cur_color = color
                run.append(ch)
        flush()
        out_rows.append("".join(parts))
    return out_rows


def render_frame(stats, frame, tw, th):
    out = []
    tick_bar_anims()

    # ─── LEFT column (title on top + ball below) + RIGHT column (battle) ───
    # Title height: 3 blank + (3+1+3+1+3) art rows = ~11 rows
    title_h_est = 15
    # Battle needs: info(2) + max(enemy, PIKAPI) + info(3) + textbox(3) + border(2)
    # Enemy + PIKAPI share vertical space (overlay) to save rows.
    enemy_h = ENEMY_SPRITE_H if ENEMY_SPRITE else 6
    min_battle_h = max(PIKAPI_SPRITE_H, enemy_h) + 10
    min_ball_h = 12

    # Reserve some space for stats+activity below
    below_rows_min = 14
    top_section_h = max(title_h_est + min_ball_h, min_battle_h)
    top_section_h = min(top_section_h, th - below_rows_min)
    top_section_h = max(top_section_h, title_h_est + 8)

    # Reserve the last column — writing to the rightmost cell triggers
    # auto-wrap on Windows console + tmux, which collapses borders.
    usable_w = tw - 1
    left_col_w = max(38, int(usable_w * 0.38))
    right_col_w = usable_w - left_col_w - 1

    title_lines = _build_title_lines(left_col_w)
    ball_h = max(6, top_section_h - len(title_lines))
    ball_lines = _render_ball(left_col_w, ball_h, frame)

    stats_ref[0] = stats
    _battle.update(stats)
    battle_lines = _battle.render(right_col_w, top_section_h, frame)

    for i in range(top_section_h):
        if i < len(title_lines):
            left = title_lines[i]
        else:
            bi = i - len(title_lines)
            left = ball_lines[bi] if bi < len(ball_lines) else ""
        left = _pad_left_col(left, left_col_w)
        right = battle_lines[i] if i < len(battle_lines) else ""
        out.append(left + " " + right)

    # ─── Stats ───
    ta = stats["ta"]; tm = stats["tm"]; tn = stats.get("tn", 0); tot = ta + tm
    ratio = tm / tot if tot > 0 else 0

    bar_w_main = max(12, tw - 38)
    main_b = bar(ratio, w=bar_w_main, key="__total__", frame=frame)
    pct = f"{ratio*100:.1f}%"
    rc = stats["review"]; ac = stats["applied"]
    q, c, d = stats["coord"]
    cs = stats.get("codex", "?")
    cs_fmt = _c(f"o {cs}", G92, BLD) if cs == "IDLE" else _c(f"x {cs}", Y93, BLD) if cs == "BUSY" else _c(f". {cs}", DM)

    phase_milestones = [
        (100, "CHAMPION",  "All functions matched!"),
        ( 90, "ELITE FOUR","90%+ — Final push!"),
        ( 80, "BADGE 8",   "80%+ — HSD/GFX tier complete"),
        ( 70, "BADGE 7",   "70%+ — Core game logic matched"),
        ( 60, "BADGE 6",   "60%+ — Battle + scene systems done"),
        ( 50, "BADGE 5",   "50%+ — Halfway to full decomp"),
        ( 40, "BADGE 4",   "40%+ — Dolphin SDK matched"),
        ( 30, "BADGE 3",   "30%+ — Foundation systems done"),
    ]
    current_badge = ""; next_target = ""
    for threshold, badge, desc in phase_milestones:
        if ratio * 100 >= threshold:
            current_badge = f"[{badge}] {desc}"
            break
    for threshold, badge, desc in reversed(phase_milestones):
        if ratio * 100 < threshold:
            funcs_needed = int(threshold / 100 * tot) - tm
            next_target = f"Next: {badge} in {funcs_needed} matches"
            break

    out.append(_c("--- DECOMP PROGRESS " + "-"*max(1, tw-21), Y93))
    _dd = _load_report_measures()
    if _dd:
        out.append("  " + _c("decomp.dev  ", C96, BLD) +
                   _c("Code ", DM) + _c(f"{_dd['code_pct']:.2f}%", G92, BLD) +
                   _c("   Fuzzy ", DM) + _c(f"{_dd['fuzzy_pct']:.2f}%", Y93, BLD) +
                   _c("   Functions ", DM) + _c(f"{_dd['fn_pct']:.2f}%", C96, BLD) +
                   _c(f"  ({_dd['matched']}/{_dd['total']} fns)", DM))
    out.append(f"  {_c('*',R91)} {main_b} {pct} {tm}/{tot} " +
               _c("(matched / total funcs)", DM))
    out.append(f"  Caught:{_c(str(tm),G92,BLD)}{_c('(100%)',DM)}" +
        f" Near:{_c(str(tn),Y93,BLD)}{_c('(90-99)',DM)}" +
        f" Rem:{_c(str(ta),R91,BLD)}" +
        f" Rev:{_c(str(rc),Y93,BLD)} App:{_c(str(ac),C96,BLD)}" +
        f"  Q:{q} C:{c} D:{d} Codex:{cs_fmt}")
    out.append(f"  Files:{_c(str(stats.get('tracked_files', len(stats.get('files', {})))),C96,BLD)}"
               f"/{stats.get('source_files', '?')}{_c('(tracked/source)',DM)} "
               f"State:{_c(os.path.relpath(DASHBOARD_STATE_FILE, REPO).replace(chr(92), '/'), DM)}")
    if current_badge: out.append(f"  {_c(current_badge, Y93, BLD)}")
    if next_target: out.append(f"  {_c(next_target, DM)}")

    regressions = stats.get("regressions_recent", [])
    if regressions:
        # Attribute each regressed fn to its last-committing agent + route
        attr_str = ", ".join(
            f"{fn}({attribute_fn(fn)}/{attribute_route(fn)}.c)"
            for fn in regressions[:5]
        )
        out.append(_c(f"  !! REGRESSION: {len(regressions)} fn(s) lost match — {attr_str}", R91, BLD))

    # Stale-object warning (prevents the "phantom regression" failure mode)
    stale_n = stats.get("stale_objs", 0)
    missing_n = stats.get("missing_objs", 0)
    if stale_n or missing_n:
        out.append(_c(f"  !! STALE BUILDS: {stale_n} stale .o, {missing_n} missing .o — run `python rebuild_stale.py` to recompile and refresh dex count", Y93, BLD))

    # Agent token usage section — CODEX + CLAUDE daily usage, then any
    # active .omc/agent_tokens.json lanes.
    out.append(_c("--- AGENT TOKENS " + "-"*max(1, tw-19), C96))
    bar_w = max(12, min(28, tw - 60))
    usage = get_agent_usage(block=("--once" in sys.argv))

    def _limit_bar(pct):
        """Clamped fill bar in the section's green/yellow/red scheme."""
        color = G92 if pct < 60 else (Y93 if pct < 85 else R91)
        filled = min(bar_w, int((pct / 100) * bar_w))  # clamp at 100%
        return (_c("█" * filled, color, BLD) + _c("░" * (bar_w - filled), DM),
                color)

    if usage is None:
        out.append("  " + _c("measuring agent usage...", DM))
    else:
        cx = usage.get("codex")
        if cx:
            wk = cx.get("weekly_pct")
            if wk is not None:
                b, wc = _limit_bar(float(wk))
                p5 = cx.get("p5h_pct")
                p5_str = _c(f"  5h {p5:.0f}%", DM) if p5 is not None else ""
                out.append(f"  {_c('▶', G92, BLD)} {'codex gpt-5.5':<16s} "
                           f"{b} {_c(f'{float(wk):3.0f}%', wc, BLD)} "
                           f"{_c('weekly limit used', DM)}{p5_str}")
            else:
                out.append(f"  {_c('▶', G92, BLD)} {'codex gpt-5.5':<16s} "
                           + _c("no rate-limit snapshot", DM))
            out.append("    " + _c(
                f"today {fmt_tokens(cx['in'] + cx['out'])} tok "
                f"({fmt_tokens(cx['out'])} out, {cx['sessions']} sessions)", DM))
        else:
            out.append(f"  {_c('·', GR37)} {'codex gpt-5.5':<16s} "
                       + _c("n/a (source not found)", DM))
        cl = usage.get("claude")
        if cl:
            out.append(f"  {_c('▶', G92, BLD)} {'claude fable':<16s} "
                       + _c("today ", DM)
                       + _c(fmt_tokens(cl['in'] + cl['out']), W97, BLD)
                       + _c(" tok / ", DM)
                       + _c(fmt_tokens(cl['out']), C96, BLD)
                       + _c(f" out ({cl['files']} sessions)", DM))
        else:
            out.append(f"  {_c('·', GR37)} {'claude fable':<16s} "
                       + _c("n/a (source not found)", DM))

    tokens_data = stats.get("tokens", {})
    agents = tokens_data.get("agents", {}) if isinstance(tokens_data, dict) else {}
    active_lanes = [n for n in sorted(agents.keys())
                    if (agents[n].get("status", "") or "").lower().startswith("busy")]
    if active_lanes:
        # Animated fill bars: busy agents get a sliding "shimmer" pulse.
        for name in active_lanes:
            a = agents[name]
            used = int(a.get("tokens_used", 0))
            limit = int(a.get("limit", 0))
            cur_fn = a.get("current_fn", "")
            status = (a.get("status", "") or "").lower()
            pct = (used / limit * 100) if limit > 0 else 0
            color = G92 if pct < 60 else (Y93 if pct < 85 else R91)
            filled = min(bar_w, int((pct / 100) * bar_w))   # clamp: used can exceed limit
            empty = bar_w - filled
            # Animation: when busy, slide a brighter glyph through the empty
            # portion (1 cell per frame) to signal "actively iterating".
            bar_chars = ["█"] * filled + ["░"] * empty
            if status.startswith("busy") and empty > 0:
                shim_pos = filled + (frame % max(1, empty))
                if 0 <= shim_pos < bar_w:
                    bar_chars[shim_pos] = "▓"
            # Color: filled portion in agent-status color, shimmer in bright cyan
            bar_str = ""
            for i, ch in enumerate(bar_chars):
                if i < filled:
                    bar_str += _c(ch, color, BLD)
                elif ch == "▓":
                    bar_str += _c(ch, C96, BLD)
                else:
                    bar_str += _c(ch, DM)
            limit_str = fmt_tokens(limit) if limit > 0 else "?"
            usage_str = f"{fmt_tokens(used)}/{limit_str}"
            # Status icon: busy=pulse glyph (animates), idle=static dot
            icon = "▶" if status.startswith("busy") else ("⏸" if status.startswith("idle") else "·")
            icon_color = G92 if status.startswith("busy") else GR37
            # Pulse the busy icon
            if status.startswith("busy") and frame % 4 < 2:
                icon = "▷"
            extra = []
            if cur_fn and cur_fn != "idle":
                # Look up which file the current_fn lives in
                fn_route = ""
                fn_clean = cur_fn.split()[0].split("(")[0].strip()
                if fn_clean.startswith("fn_"):
                    route = attribute_route(fn_clean)
                    if route and route != "?":
                        fn_route = f" in {route}.c"
                extra.append(f"on {fn_clean}{fn_route}")
            extra_str = " (" + ", ".join(extra) + ")" if extra else ""
            out.append(
                f"  {_c(icon, icon_color, BLD)} {name:18s} "
                f"{bar_str} "
                f"{_c(usage_str, color):>16s} "
                f"{_c(f'{pct:>3.0f}%', color)}"
                f"{_c(extra_str, DM)}"
            )

    # ─── Per-file routes: show files-in-progress; condense 100% files ───
    fs = stats["files"]
    # Split: in-progress (has asm remaining) vs complete (0 asm remaining)
    in_progress = [(f, d) for f, d in fs.items() if d["asm"] > 0]
    complete    = [(f, d) for f, d in fs.items() if d["asm"] == 0 and d["matched"] > 0]
    # Order in-progress files WORST-FIRST (lowest match ratio at the top) so
    # the files needing the most work lead the list.
    in_progress.sort(key=lambda x: x[1]["matched"] / max(1, x[1]["matched"] + x[1]["asm"]))
    complete.sort(key=lambda x: x[0])

    out.append(_c("--- POKEDEX BY ROUTE " + "-"*max(1, tw-22), M95) +
               _c(" (in-progress files, worst-first)", DM))

    # Reserve rows for complete-files line + legend + activity
    activity_rows = max(6, th - len(out) - len(in_progress) - 6)
    max_files = max(6, th - len(out) - activity_rows - 6)
    name_w = max(8, min(28, tw - 32))
    bar_w_file = max(8, tw - name_w - 22)

    out.append("  " + _c(f"{'File':<{name_w}s} {'match progress':<{bar_w_file}s} {'%':>6s} {'fns':>7s}", C96, BLD))
    for fp, data in in_progress[:max_files]:
        a, m = data["asm"], data["matched"]
        near = data.get("near", 0)
        ft = a + m; fr = m/ft if ft > 0 else 0
        short = fp.replace("src/game/","").replace("src/","")
        if len(short) > name_w: short = short[-name_w:]
        b = bar(fr, w=bar_w_file, key=fp, frame=frame)
        near_s = f" ~{near}" if near else ""
        out.append(f"  {short:<{name_w}s} {b} {fr*100:5.1f}% {m:>3d}/{ft}{near_s}")

    if len(in_progress) > max_files:
        out.append(_c(f"  ... +{len(in_progress) - max_files} more in-progress files (raise terminal height to see all)", DM))

    # Compact list of 100% files — wrap to terminal width
    if complete:
        complete_total = sum(d["matched"] for _, d in complete)
        out.append(_c(f"--- CHAMPIONS ({len(complete)} files, {complete_total} fns) " +
                      "-"*max(1, tw-28-len(str(len(complete)))-len(str(complete_total))), G92, BLD))
        names = [f.replace("src/game/","").replace("src/","").replace(".c","") for f, _ in complete]
        line = "  "
        for name in names:
            piece = name + "  "
            if len(line) + len(piece) > tw - 2:
                out.append(_c(line, G92))
                line = "  " + piece
            else:
                line += piece
        if line.strip():
            out.append(_c(line, G92))

    msg = MSGS[(frame // 12) % len(MSGS)]
    out.append(f"  {_c('█',G92,BLD)} matched  {_c('░',GR37,BLD)} remaining  " +
               _c(msg, DM))

    # ─── Recent Activity (expanded) ───
    sl = stats.get("status_lines", [])
    if sl and len(out) < th - 2:
        out.append(_c("--- Recent Activity " + "-"*max(1, tw-22), GR37))
        max_activity = th - len(out) - 1
        for s in reversed(sl[-max_activity:]):
            if len(out) >= th - 1: break
            clean = s.replace("**","").replace("- ","  ")
            if len(clean) > tw - 2: clean = clean[:tw-5] + "..."
            clean = clean.encode('ascii', 'ignore').decode('ascii')
            if "REGRESSION" in clean or "regressed" in clean.lower():
                out.append(_c(clean, R91, BLD))
            elif "Completed" in clean or "matched" in clean.lower() or "MATCH" in clean:
                out.append(_c(clean, G92))
            elif "Claimed" in clean: out.append(_c(clean, C96))
            elif "Enqueued" in clean: out.append(_c(clean, Y93))
            else: out.append(_c(clean, GR37))

    while len(out) < th:
        out.append("")
    return out[:th]

# ─── Main loop ───────────────────────────────────────────────────────────────

def _check_new_commits(last_sha):
    """Return list of (sha, agent, fn, stem, subj) for commits since last_sha,
    or all-recent if last_sha is None. Used to surface direct commit events
    in the activity log so codex/opus-sub matches don't get drowned out."""
    try:
        if last_sha:
            r = subprocess.run(
                ["git", "log", f"{last_sha}..HEAD", "--pretty=format:%H|%an|%s", "--", "src/"],
                capture_output=True, text=True, timeout=5, cwd=REPO,
            )
        else:
            r = subprocess.run(
                ["git", "log", "-5", "--pretty=format:%H|%an|%s", "--", "src/"],
                capture_output=True, text=True, timeout=5, cwd=REPO,
            )
        if r.returncode != 0:
            return []
        out = []
        import re as _re
        for line in r.stdout.splitlines():
            parts = line.split("|", 2)
            if len(parts) != 3:
                continue
            sha, author, subj = parts
            fnm = _re.search(r"fn_[0-9A-Fa-f]{8}", subj)
            stm = _re.match(r"\s*([a-zA-Z0-9_]+)\s*[:.]", subj)
            if not fnm:
                continue
            lower = subj.lower()
            if "via deepseek" in lower or " deepseek-v4" in lower:
                agent = "deepseek-v4-flash"
            elif "via codex" in lower or "codex" in lower:
                agent = "codex"
            elif "via opus-sub" in lower or "opus-sub" in lower or "via opus" in lower:
                agent = "opus-sub"
            elif "via kimi" in lower or "kimi" in lower:
                agent = "kimi"
            elif "via qwen" in lower or "qwen" in lower:
                agent = "qwen-3090"
            else:
                agent = author.split()[0].lower() if author else "?"
            out.append((sha, agent, fnm.group(0), stm.group(1) if stm else "?", subj))
        return out
    except Exception:
        return []


def _load_report_measures():
    """Authoritative fn-match totals from the committed objdiff report.json
    (the decomp.dev metric via gen_decomp_report.py). The live source scan only
    covers a subset of TUs, so the headline completion % must use this instead,
    or it under-reports actual progress."""
    try:
        m = _load_json(os.path.join(REPO, "report.json"), {}).get("measures", {})
        tot = int(m.get("total_functions", 0))
        mat = int(m.get("matched_functions", 0))
        if tot > 0 and 0 <= mat <= tot:
            return {"total": tot, "matched": mat,
                    "code_pct": float(m.get("matched_code_percent", 0.0)),
                    "fuzzy_pct": float(m.get("fuzzy_match_percent", 0.0)),
                    "fn_pct": float(m.get("matched_functions_percent", 0.0))}
    except Exception:
        pass
    return None


def _print_loading_frame():
    """Immediate first paint — the full source scan (objdiff per fn) can take
    a long time on a cold cache; show SOMETHING the instant the pane opens."""
    term = shutil.get_terminal_size((120, 40))
    tw = term.columns
    lines = [
        "",
        "  " + _c("POKEDEX DECOMP DASHBOARD", C96, BLD),
        "  " + _c("measuring sources + build artifacts...", DM),
        "  " + _c("first full frame lands when the scan finishes", DM),
        "",
        "  " + _c("(cached snapshots render instantly on later launches)", DM),
    ]
    out = "\033[H\033[J" + "\n".join(
        _truncate_visible(l, tw - 1) if _visible_len(l) > tw - 1 else l
        for l in lines)
    sys.stdout.write(out + "\n")
    sys.stdout.flush()


def main():
    once = "--once" in sys.argv
    stats = load_dashboard_state_for_render()
    if stats is None:
        _print_loading_frame()   # cold start: paint immediately, then measure
    frame = 1 if stats is not None else 0
    ms = load_ms()
    history = load_match_history()
    prev_matched_set = set(history.get("matched", []))
    prev_matched = None
    prev_file_stats = {}
    last_git_sha = None  # tracks newest commit seen, for direct activity events

    while True:
        try:
            t0 = time.monotonic()
            term = shutil.get_terminal_size((120, 40))
            tw, th = term.columns, term.lines

            regressions_this_tick = []
            _stale_objs_now = []
            _missing_objs_now = []
            if stats is None or frame % STATS_EVERY == 0:
                fs, matched_fns_now, matched_by_file, function_rows = scan_sources()
                _stale_objs_now, _missing_objs_now = find_stale_objects()
                # Persistence dampener: only count fns matched in 2+ consecutive
                # scans toward the total, to prevent worker-iteration flicker
                # from making the dex completion oscillate up/down.
                stable_set = matched_fns_now & prev_matched_set
                # Stable set per file (for fs[fp]["matched"] count)
                stable_by_file = {}
                for fp, fns in matched_by_file.items():
                    stable_by_file[fp] = [f for f in fns if f in stable_set]
                # Adjust fs counts to use stable matches only
                for fp, data in fs.items():
                    persistent_match = len(stable_by_file.get(fp, []))
                    transient_match = data["matched"] - persistent_match
                    data["matched"] = persistent_match
                    data["asm"] = data["asm"] + transient_match  # transients go back to asm-left
                ta = sum(d["asm"] for d in fs.values())
                tm = sum(d["matched"] for d in fs.values())
                tn = sum(d.get("near", 0) for d in fs.values())
                # Headline completion uses the AUTHORITATIVE report.json totals
                # (whole binary: 8287 fns / 147 units). The per-file `fs` catalog
                # above only covers a subset, which under-reports the real %.
                _rm = _load_report_measures()
                if _rm:
                    tm = _rm["matched"]
                    ta = max(0, _rm["total"] - _rm["matched"])
                new_matches = stable_set - prev_matched_set
                # Note: prev_matched_set is updated below to matched_fns_now (raw),
                # not stable_set, so we keep tracking which fns are eligible for
                # next-tick stability promotion.

                # Regression detection: functions that were in history but
                # aren't in the current scan. Dampened to skip false positives
                # from workers actively iterating: a fn currently being worked
                # by any agent (per agent_tokens.json) is excluded.
                raw_regressed = prev_matched_set - matched_fns_now
                # Skip fns that are listed as the current_fn of any busy agent
                busy_fns = set()
                tokens_now = load_agent_tokens()
                for _name, _a in tokens_now.get("agents", {}).items():
                    cf = (_a.get("current_fn", "") or "")
                    if cf and cf.startswith("fn_"):
                        busy_fns.add(cf.split()[0].split("(")[0].strip())
                regressed_fns = raw_regressed - busy_fns
                # Persistence: require a fn to appear missing in TWO consecutive
                # scans before flagging. Tracks "tentative" misses in history.
                tentative = set(history.get("tentative_regressions", []))
                confirmed = regressed_fns & tentative  # missing 2x in a row
                history["tentative_regressions"] = sorted(regressed_fns)  # for next tick
                if prev_matched is not None and confirmed:
                    regressions_this_tick = sorted(confirmed)
                    for fn in regressions_this_tick:
                        history.setdefault("regressions", []).append({
                            "fn": fn, "at": time.time()
                        })
                    # Cap regression log
                    history["regressions"] = history["regressions"][-200:]

                history["matched"] = sorted(matched_fns_now)
                save_match_history(history)
                prev_matched_set = matched_fns_now

                # Direct commit watcher: writes one activity entry per new
                # commit so codex/opus-sub events always show up explicitly,
                # not just when match-detection runs ahead of git.
                try:
                    new_commits = _check_new_commits(last_git_sha)
                    if new_commits and last_git_sha is None:
                        # First scan: just record HEAD, don't backfill
                        last_git_sha = new_commits[0][0]
                    elif new_commits:
                        with open(os.path.join(COORD_DIR, "status.md"), "a") as f:
                            for sha, agent, fn, stem, subj in reversed(new_commits):
                                from datetime import datetime
                                ts = datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')
                                # Extract pct from subject
                                import re as _re
                                pmm = _re.search(r"([0-9.]+)%", subj)
                                pct_str = f" {pmm.group(1)}%" if pmm else ""
                                f.write(f"\n- **{ts}** `dashboard` — COMMIT {fn}{pct_str} in {stem}.c by {agent}")
                        last_git_sha = new_commits[0][0]
                except Exception: pass

                stats = {
                    "ta": ta, "tm": tm, "tn": tn, "files": fs,
                    "review": count_json(REVIEW_DIR),
                    "applied": count_json(APPLIED_DIR),
                    "coord": get_coord(),
                    "status_lines": get_status(25),
                    "codex": get_codex(),
                    "regressions_recent": regressions_this_tick,
                    "stale_objs": len(_stale_objs_now),
                    "missing_objs": len(_missing_objs_now),
                    "tokens": load_agent_tokens(),
                    "source_files": count_source_files(),
                    "tracked_files": len(fs),
                }
                save_dashboard_state(stats, function_rows, stable_set)

                tot = ta + tm
                pct = tm/tot*100 if tot > 0 else 0
                for t in sorted(MILESTONES.keys()):
                    if pct >= t and t not in ms.get("triggered",[]):
                        ms.setdefault("triggered",[]).append(t)
                        save_ms(ms)
                        _play_milestone_sound()

                # Regression sound + battle shake
                if regressions_this_tick:
                    _play_regression_sound()
                    regression_short = regressions_this_tick[0].replace("fn_", "")
                    _battle.update(stats, regression_file=regression_short)
                    try:
                        from datetime import datetime
                        ts = datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')
                        # Attribute each regressed fn to its last-committing agent
                        attr_pairs = [f"{fn}({attribute_fn(fn)}/{attribute_route(fn)}.c)" for fn in regressions_this_tick[:5]]
                        with open(os.path.join(COORD_DIR, "status.md"), "a") as f:
                            f.write(f"\n- **{ts}** `dashboard` — REGRESSION: {', '.join(attr_pairs)}")
                    except: pass

                if new_matches:
                    rain_color = Y93
                    matched_file = "unknown"
                    matched_delta = len(new_matches)
                    # Trigger animation on total bar
                    trigger_bar_anim("__total__", tm / max(1, ta + tm))
                    files_with_new = []
                    for fpath, fn_list in matched_by_file.items():
                        if any(fn in new_matches for fn in fn_list):
                            files_with_new.append(fpath)
                    for fpath in files_with_new:
                        fdata = fs.get(fpath)
                        if not fdata:
                            continue
                        ftotal = fdata["asm"] + fdata["matched"]
                        fratio = fdata["matched"] / ftotal if ftotal > 0 else 0
                        trigger_bar_anim(fpath, fratio)
                    if files_with_new:
                        first = files_with_new[0]
                        fdata = fs[first]
                        ftotal = fdata["asm"] + fdata["matched"]
                        fratio = fdata["matched"] / ftotal if ftotal > 0 else 0
                        rain_color = _bar_color_for_ratio(fratio)
                        short = first.replace("src/game/","").replace("src/","")
                        if len(files_with_new) == 1:
                            matched_file = f"{short} ({fratio*100:.0f}%)"
                        else:
                            matched_file = f"{short} +{len(files_with_new)-1} file(s)"
                    _rain.trigger(tw, th, color=rain_color)
                    _play_levelup_sound()
                    _battle.update(stats, matched_file=matched_file, matched_delta=matched_delta)
                    try:
                        from datetime import datetime
                        ts = datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')
                        # Attribute new matches: per-fn agent first, then fall
                        # back to any busy worker active IN the same file.
                        agent_tags = {attribute_fn(fn) for fn in new_matches if attribute_fn(fn) != "?"}
                        if not agent_tags and files_with_new:
                            # File-level fallback: any agent whose current_fn
                            # is hosted in one of the files_with_new
                            tokens_n = load_agent_tokens()
                            for ag_name, ag in tokens_n.get("agents", {}).items():
                                cf = (ag.get("current_fn", "") or "")
                                if not cf or not cf.startswith("fn_"):
                                    continue
                                cf_clean = cf.split()[0].split("(")[0].strip()
                                # Check if cf_clean is in any of the files_with_new
                                for fp in files_with_new:
                                    if cf_clean in matched_by_file.get(fp, []):
                                        agent_tags.add(ag_name); break
                                else:
                                    # Even broader: check the agent's file lane
                                    # heuristically via the file stem
                                    for fp in files_with_new:
                                        stem = os.path.splitext(os.path.basename(fp))[0]
                                        if any(stem in (ag.get("current_fn","") or "")
                                               for k in (ag_name,)):
                                            agent_tags.add(ag_name); break
                        agent_str = (" by " + "+".join(sorted(agent_tags))) if agent_tags else ""
                        with open(os.path.join(COORD_DIR, "status.md"), "a") as f:
                            f.write(f"\n- **{ts}** `dashboard` — MATCH! +{matched_delta} in {matched_file}{agent_str}")
                    except: pass

                prev_matched = tm
                prev_file_stats = {fp: d["matched"] for fp, d in fs.items()}

            output = render_frame(stats, frame, tw, th)

            _rain.tick(th)
            _rain.overlay(output, tw)

            # Safety: truncate any line that would overflow terminal width.
            # An overflowing line wraps to the next row which scrolls the top off.
            safe_w = tw - 1
            for idx, ln in enumerate(output):
                if _visible_len(ln) > safe_w:
                    output[idx] = _truncate_visible(ln, safe_w)

            buf = "\033[H" + "\033[K\n".join(output) + "\033[K"
            sys.stdout.write(buf)
            sys.stdout.flush()

            if once: break
            frame += 1
            elapsed = time.monotonic() - t0
            sleep = max(0, REFRESH - elapsed)
            if sleep > 0: time.sleep(sleep)

        except KeyboardInterrupt:
            sys.stdout.write(RST + "\n")
            break

if __name__ == "__main__":
    main()
