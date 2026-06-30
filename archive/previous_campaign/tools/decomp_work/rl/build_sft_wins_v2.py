#!/usr/bin/env python3
"""build_sft_wins_v2.py — v2 of the expert-iteration SFT dataset.

v1 (build_sft_wins.py) paired target CW-PPC asm -> byte-exact C, but with an
ASM-ONLY prompt and NO context. The SFT'd model then invented type/field names
(e.g. DTKEntry/entry->field_48 instead of the real FSYSSlot/slot->status), so its
output would not compile. v2 fixes this by prepending a CONTEXT header — the real
game types/structs/externs visible to the function's translation unit — to every
prompt, so the model is grounded in the actual declarations.

CONTEXT is derived purely from (asm + TU): we read the function's TU .c file,
follow its quoted #include "..." lines, and inline every header that resolves
under include/ and is NOT under include/dolphin/ (SDK boilerplate the model
already knows). Recursion follows the same rule, dedups by resolved path, and
avoids cycles. Pure-comment lines / docstrings are stripped to save tokens. The
answer C is NEVER consulted when building context (headers hold declarations,
not bodies, so the constraint is automatic).

Asm reuses v1's mechanism exactly: the one-time dtk disasm cache at
%TEMP%/auto_01_disasm.txt (parse `.fn <name>` ... `.endfn <name>` blocks), with
the src/game/*_<fn>.inc fallback. The cache already exists from the v1 run.
"""
import json, glob, re, subprocess, tempfile, statistics, os
from pathlib import Path

ROOT = Path("C:/Users/douglaswhittingham/pkmn-colosseum")
INCLUDE = ROOT / "include"
TARGET = ROOT / "build/GC6E01/obj/auto_01_800055E0_text.o"
DTK = ROOT / ("tools/dtk.exe" if os.name == "nt" else "tools/dtk")
CACHE = Path(tempfile.gettempdir()) / "auto_01_disasm.txt"
OUT = ROOT / "tools/decomp_work/rl/sft_wins_v2.jsonl"
OUT.parent.mkdir(parents=True, exist_ok=True)

# Same decompiler role as v1 (read from build_sft_wins.py SYS).
SYS = ("You are a GameCube PowerPC decompiler. Convert the target CodeWarrior 1.3 "
       "PPC assembly to byte-matching C89. Use extern labels for SDA globals, never "
       "float literals for sdata2 returns. Output only the C function with any needed externs.")

# Token budget: keep each example under ~7000 tokens (~28000 chars). Context alone
# capped at ~5000 tokens (~20000 chars); if larger we truncate whole headers off
# the tail (least-relevant: deepest/last-included) and note the count.
CHARS_PER_TOK = 4
CONTEXT_TOK_CAP = 5000
CONTEXT_CHAR_CAP = CONTEXT_TOK_CAP * CHARS_PER_TOK  # 20000

INC_RE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')


def strip_comments(text):
    """Remove C block /* ... */ and line // ... comments, then drop blank lines
    that result. Keeps real declarations; trims docstrings/comment-only lines.
    Operates on raw text (string/char literals in headers are declarations, not
    bodies, and don't contain comment delimiters in practice here)."""
    # Remove block comments (non-greedy, across newlines).
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    # Remove line comments.
    text = re.sub(r'//[^\n]*', '', text)
    # Collapse runs of blank lines and trailing whitespace.
    out = []
    blank = False
    for ln in text.splitlines():
        s = ln.rstrip()
        if not s.strip():
            if blank:
                continue
            blank = True
            out.append("")
        else:
            blank = False
            out.append(s)
    # Trim leading/trailing blank lines.
    while out and out[0] == "":
        out.pop(0)
    while out and out[-1] == "":
        out.pop()
    return "\n".join(out)


def resolve_include(inc):
    """Resolve a quoted include to a header under include/ that is NOT dolphin
    SDK boilerplate. Returns the resolved Path or None (None for dolphin/*,
    .inc/.s function-body includes, or anything not present under include/)."""
    if inc.endswith(".inc") or inc.endswith(".s"):
        return None
    if inc.startswith("dolphin/"):
        return None
    p = INCLUDE / inc
    if p.is_file():
        # Belt-and-suspenders: never inline anything inside include/dolphin/.
        try:
            rel = p.resolve().relative_to(INCLUDE.resolve())
        except ValueError:
            return None
        if str(rel).replace(os.sep, "/").startswith("dolphin/"):
            return None
        return p
    return None


def quoted_includes(text):
    out = []
    for ln in text.splitlines():
        m = INC_RE.match(ln)
        if m:
            out.append(m.group(1))
    return out


def gather_headers(tu_c):
    """Return the ordered list of resolved header Paths for a TU's context:
    each non-dolphin header reachable from the TU's quoted includes, recursively,
    deduped, with dependencies emitted before dependents (post-order), cycles
    avoided. The TU .c body is NEVER inlined (only its #include lines are read)."""
    try:
        tu_text = Path(tu_c).read_text(encoding="utf-8", errors="replace")
    except OSError:
        return []
    seen = set()
    order = []

    def visit(path):
        rp = str(path.resolve())
        if rp in seen:
            return
        seen.add(rp)
        try:
            txt = path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            return
        for inc in quoted_includes(txt):
            r = resolve_include(inc)
            if r is not None:
                visit(r)
        order.append(path)  # post-order: deps first

    for inc in quoted_includes(tu_text):
        r = resolve_include(inc)
        if r is not None:
            visit(r)
    return order


def build_context(tu_c):
    """Build the CONTEXT string for a TU plus a small stats dict.
    Returns (context_text, n_headers_used, n_headers_truncated)."""
    headers = gather_headers(tu_c)
    if not headers:
        return "", 0, 0
    blocks = []
    for h in headers:
        rel = os.path.relpath(h, INCLUDE).replace(os.sep, "/")
        body = strip_comments(h.read_text(encoding="utf-8", errors="replace"))
        if body.strip():
            blocks.append((rel, f"/* === {rel} === */\n{body}"))
    # Token-budget the context: keep whole headers from the front (the TU's own
    # primary headers come last in post-order, so put dependents-then-deps but
    # we present in include order; truncation drops least-relevant tail). We
    # accumulate until the char cap, counting how many headers we drop.
    full = "\n\n".join(b for _, b in blocks)
    if len(full) <= CONTEXT_CHAR_CAP:
        return full, len(blocks), 0
    kept = []
    total = 0
    truncated = 0
    for _, b in blocks:
        add = len(b) + 2  # +2 for the "\n\n" join
        if total + add <= CONTEXT_CHAR_CAP:
            kept.append(b)
            total += add
        else:
            truncated += 1
    ctx = "\n\n".join(kept)
    if truncated:
        ctx += f"\n\n/* ... {truncated} additional header(s) omitted for length ... */"
    return ctx, len(kept), truncated


# ----------------------------------------------------------------------------
# 1) collect unique win functions {fn: C} and the per-fn TU source path.
wins = {}
fn_src = {}
for f in glob.glob(str(ROOT / "build/band_wins/*.json")):
    try:
        d = json.load(open(f, encoding="utf-8"))
    except Exception:
        continue
    srcs = d.get("_srcs") or {}
    default_src = d.get("_src")
    for k, v in d.items():
        if k in {"_src", "_srcs", "_pct"}:
            continue
        if isinstance(v, str) and v.strip():
            if k not in wins:
                wins[k] = v
                fn_src[k] = srcs.get(k, default_src)
print(f"collected {len(wins)} unique byte-exact win functions")

# 2) ensure one-time disasm cache (reuse v1's; only rebuild if missing).
if not CACHE.exists() or CACHE.stat().st_size < 1000:
    print("disassembling target object once (dtk)...")
    subprocess.run([str(DTK), "elf", "disasm", str(TARGET), str(CACHE)], cwd=str(ROOT))
print(f"disasm cache: {CACHE} ({CACHE.stat().st_size if CACHE.exists() else 0} bytes)")

# 3) parse cache into {fn: asm-block}.
asm = {}
cur, buf = None, []
for ln in CACHE.read_text(errors="replace").splitlines():
    m = re.match(r"^\.fn\s+(\S+)", ln)
    if m:
        cur = m.group(1).rstrip(","); buf = [ln]; continue
    if cur:
        buf.append(ln)
        if re.match(rf"^\.endfn\s+{re.escape(cur)}\b", ln):
            asm[cur] = "\n".join(buf); cur = None
print(f"parsed asm for {len(asm)} functions from cache")


def from_inc(fn):
    for p in (ROOT / "src/game").glob(f"*_{fn}.inc"):
        return p.read_text(errors="replace")
    return None


# 4) cache contexts per-TU (many fns share a TU).
ctx_cache = {}


def context_for_src(src):
    if not src:
        return ("", 0, 0)
    if src not in ctx_cache:
        path = src if os.path.isabs(src) else str(ROOT / src)
        ctx_cache[src] = build_context(path)
    return ctx_cache[src]


# 5) build ChatML examples.
examples, skipped = [], []
n_with_ctx = 0
n_empty_ctx = 0
n_truncated = 0
for fn, c in wins.items():
    a = asm.get(fn) or from_inc(fn)
    if not a:
        skipped.append(fn); continue
    ctx, n_hdr, n_trunc = context_for_src(fn_src.get(fn))
    if ctx.strip():
        n_with_ctx += 1
    else:
        n_empty_ctx += 1
    if n_trunc:
        n_truncated += 1
    if ctx.strip():
        user = (f"Available types, structs, and externs for this translation unit:\n\n"
                f"{ctx}\n\n"
                f"Decompile this function to byte-matching C:\n\n```\n{a.strip()}\n```")
    else:
        # No game context resolved (e.g. SDK-only TU): fall back to bare asm prompt.
        user = (f"Available types, structs, and externs for this translation unit:\n\n"
                f"(none beyond the standard GameCube SDK types)\n\n"
                f"Decompile this function to byte-matching C:\n\n```\n{a.strip()}\n```")
    examples.append({"messages": [
        {"role": "system", "content": SYS},
        {"role": "user", "content": user},
        {"role": "assistant", "content": f"```c\n{c.strip()}\n```"},
    ]})

with open(OUT, "w", encoding="utf-8") as f:
    for e in examples:
        f.write(json.dumps(e, ensure_ascii=False) + "\n")

# 6) stats.
tl = [(len(e["messages"][1]["content"]) + len(e["messages"][2]["content"])) // CHARS_PER_TOK
      for e in examples]
print(f"\nWROTE {len(examples)} examples -> {OUT}")
print(f"skipped (no asm found): {len(skipped)}  e.g. {skipped[:5]}")
print(f"non-empty game context: {n_with_ctx}   empty/fallback context: {n_empty_ctx}")
print(f"examples with a truncated context: {n_truncated}")
if tl:
    over = sum(1 for t in tl if t > 8192)
    print(f"approx token lengths: min {min(tl)} median {int(statistics.median(tl))} "
          f"max {max(tl)} | >8192: {over}")

# 7) FSYSSlot verification + print the full fn_8017D800 sample.
KEY = "fn_8017D800"
key_ex = None
for fn, c in wins.items():
    pass
# Find the example whose assistant content matches the KEY win body (or rebuild
# directly from the win for an exact sample).
for e in examples:
    if "FSYSSlot" in e["messages"][1]["content"] and "fn_8017D800" in e["messages"][2]["content"]:
        key_ex = e
        break
if KEY in wins:
    a = asm.get(KEY) or from_inc(KEY)
    ctx, _, _ = context_for_src(fn_src.get(KEY))
    has_fsysslot = "FSYSSlot" in ctx
    print(f"\nfn_8017D800 context contains 'FSYSSlot': {has_fsysslot}")
    # Rebuild the exact example for printing.
    if a:
        user = (f"Available types, structs, and externs for this translation unit:\n\n"
                f"{ctx}\n\n"
                f"Decompile this function to byte-matching C:\n\n```\n{a.strip()}\n```")
        sample = {"messages": [
            {"role": "system", "content": SYS},
            {"role": "user", "content": user},
            {"role": "assistant", "content": f"```c\n{wins[KEY].strip()}\n```"},
        ]}
        print("\n" + "=" * 78)
        print("FULL SAMPLE EXAMPLE: fn_8017D800")
        print("=" * 78)
        print("\n----- SYSTEM -----")
        print(sample["messages"][0]["content"])
        print("\n----- USER -----")
        print(sample["messages"][1]["content"])
        print("\n----- ASSISTANT -----")
        print(sample["messages"][2]["content"])
        print("=" * 78)
else:
    print("\nWARNING: fn_8017D800 not found among wins!")
