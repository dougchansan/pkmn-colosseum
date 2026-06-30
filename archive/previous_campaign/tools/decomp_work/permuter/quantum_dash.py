#!/usr/bin/env python3
"""quantum_dash.py — quantum-mechanics-themed live view of the decomp-permuter
simulated-annealing search. Reads .omc/permuter_state.json (grind2.py swarm).
ASCII-art equation (renders everywhere), a rotating 3D |psi|^2 probability cloud,
energy-descent, temperature, queue, and a MATCH-FOUND celebration animation.
Run with WSL python3. Ctrl-C to exit."""
import json, os, sys, time, math, random, shutil

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
STATE = os.path.join(REPO, ".omc", "permuter_state.json")

def fg(n): return f"\x1b[38;5;{n}m"
R = "\x1b[0m"; BD = "\x1b[1m"
CY = fg(51); MG = fg(201); GR = fg(46); YL = fg(226); OR = fg(208)
RD = fg(196); DM = fg(240); WH = fg(255); TEAL = fg(45); PINK = fg(213)
QGRAD = [fg(17), fg(19), fg(57), fg(93), fg(129), fg(165), fg(201), fg(207), fg(213), fg(219)]
ACOLORS = [fg(46), fg(201), fg(51), fg(226), fg(208), fg(135), fg(118), fg(45), fg(199), fg(214)]
SPARK = " .:-=+*#%@"
DEPTH = " .:-=co0O@"

import subprocess as _sub
_last_best = [None]
_snd = ("--sound" in sys.argv) or ("--mute" not in sys.argv)


def _beep(freq, dur=70):
    if not _snd:
        return
    try:
        _sub.Popen(["powershell.exe", "-NoProfile", "-c", f"[console]::beep({int(freq)},{int(dur)})"],
                   stdout=_sub.DEVNULL, stderr=_sub.DEVNULL)
    except Exception:
        pass


def _fanfare():
    for f in (523, 659, 784, 1047):
        _beep(f, 110)

# pre-built 3D point cloud sampling a |psi|^2 = two-lobe orbital + s-shell
_CLOUD = []
random.seed(7)
for _ in range(260):
    # mixture: central gaussian + two p-lobes along z
    r = random.random()
    if r < 0.5:
        x, y, z = (random.gauss(0, 0.5), random.gauss(0, 0.5), random.gauss(0, 0.5))
    else:
        sgn = 1 if random.random() < 0.5 else -1
        z = sgn * (0.9 + abs(random.gauss(0, 0.4)))
        x, y = random.gauss(0, 0.35), random.gauss(0, 0.35)
    _CLOUD.append((x, y, z))
# faint wireframe sphere (equator + 2 meridians) gives the cloud rotating 3D structure
for _t in range(0, 360, 9):
    _a = math.radians(_t)
    _CLOUD.append((1.35 * math.cos(_a), 1.35 * math.sin(_a), 0.0))
    _CLOUD.append((1.35 * math.cos(_a), 0.0, 1.35 * math.sin(_a)))
    _CLOUD.append((0.0, 1.35 * math.cos(_a), 1.35 * math.sin(_a)))


def load():
    try:
        return json.load(open(STATE))
    except Exception:
        return {}


import re as _re
_LOGDIR = os.path.join(REPO, "tools", "decomp_work", "permuter", "logs")
_pbest = {}   # per-annealer best energy (for sound + vibration)
_ehist = {}   # per-annealer accumulated energy-over-time (for the growing graph)


def energy_graph(anns, tw, frame):
    """A multi-row time-series graph of objdiff score per chain (colored), that
    grows as each annealer accumulates history. Newest sample on the right."""
    H = 9
    W = clamp(tw - 10, 16, 120)   # leave 8 cols for the Y-axis tick labels + margin
    cell = [[None] * W for _ in range(H)]   # stores annealer idx per plotted cell
    series = [(a["idx"], _ehist.get(a["fn"], [])) for a in anns]
    allv = [v for _, h in series for v in h]
    if not allv:
        return ["  " + DM + "(energy graph warming up - chains accumulating history...)" + R]
    lo, hi = min(allv), max(allv)
    rng = max(1, hi - lo)
    for idx, h in series:
        hh = h[-W:]
        n = len(hh)
        py = None
        for xi, v in enumerate(hh):
            x = W - n + xi
            y = (H - 1) - int((v - lo) / rng * (H - 1))
            y = clamp(y, 0, H - 1)
            if 0 <= x < W:
                cell[y][x] = idx
                # vertical connector to the previous point (line feel)
                if py is not None and x - 1 >= 0:
                    a2, b2 = (y, py) if y <= py else (py, y)
                    for yy in range(a2, b2 + 1):
                        if cell[yy][x - 1] is None:
                            cell[yy][x - 1] = idx
                py = y
    out = []
    out.append("  " + DM + "E = objdiff cost  [instruction-diff units]   (0 = byte-exact match)" + R)
    for r in range(H):
        # Y-axis tick labels carry the unit (objdiff cost) on the top/bottom rows
        if r == 0:
            axis = YL + f"{hi:>6} " + DM + "┤" + R
        elif r == H - 1:
            axis = YL + f"{lo:>6} " + DM + "┤" + R
        else:
            axis = "       " + DM + "│" + R
        s = ""
        for c in range(W):
            ci = cell[r][c]
            if ci is None:
                s += DM + ("." if (c % 12 == 0) else " ") + R   # faint gridline
            else:
                s += ACOLORS[ci % len(ACOLORS)] + "•" + R
        out.append(axis + s)
    out.append("       " + DM + "└" + "─" * min(W, 56) + "→ iterations (anneal time)" + R)
    out.append("  " + DM + f"E: hi {hi}  →  lo {lo}  objdiff cost units   (0 = byte-exact match)" + R)
    return out


def read_annealers():
    """Live per-annealer data read straight from the permuter run logs (robust:
    bypasses any state-polling lag). Returns list of {fn,iter,score,best,idx}."""
    st = load()
    act = st.get("active", {})
    fns = []
    for v in (act.values() if isinstance(act, dict) else []):
        fn = v.get("fn")
        if fn and fn not in fns:
            fns.append(fn)
    res = []
    for i, fn in enumerate(fns):
        it = None; sc = None; scs = []
        try:
            txt = open(os.path.join(_LOGDIR, f"run_{fn}.log"), errors="replace").read()[-6000:]
            its = _re.findall(r'iteration (\d+)', txt)
            scs = _re.findall(r'score = (-?\d+)', txt)
            if its: it = int(its[-1])
            if scs: sc = int(scs[-1])
        except Exception:
            pass
        if sc is not None:
            b = _pbest.get(fn)
            if b is None or sc < b:
                _pbest[fn] = sc
            eh = _ehist.setdefault(fn, [])
            if not eh or eh[-1] != sc:
                eh.append(sc)
                del eh[:-160]   # keep a growing-but-bounded tail
        hist = [int(x) for x in scs[-26:]]
        temp = clamp(1.0 - (it or 0) / 40000.0, 0.02, 1.0)   # each chain cools on ITS own iteration
        res.append({"fn": fn, "iter": it or 0, "score": sc,
                    "best": _pbest.get(fn), "idx": i, "hist": hist, "temp": temp})
    return res, st


def clamp(v, a, b): return max(a, min(b, v))


_ANSI_RE = _re.compile(r"\x1b\[[0-9;?]*[A-Za-z]")


def vlen(s):
    """Visible length of a string (ANSI escapes stripped)."""
    return len(_ANSI_RE.sub("", s))


def vtrunc(s, n):
    """Hard-truncate to n visible columns, keeping ANSI sequences intact.
    Appends a dim ellipsis when something was cut."""
    if n <= 0:
        return ""
    if vlen(s) <= n:
        return s
    out, vis, i, budget = [], 0, 0, max(0, n - 1)
    while i < len(s) and vis < budget:
        m = _ANSI_RE.match(s, i)
        if m:
            out.append(m.group())
            i = m.end()
            continue
        out.append(s[i])
        vis += 1
        i += 1
    return "".join(out) + R + DM + "…" + R


def eq_block(frame):
    glow = QGRAD[(frame // 2) % len(QGRAD)]
    g = lambda s: glow + BD + s + R
    return [
        "  " + g("                  /   -dE   \\"),
        "  " + g("  P(accept) = exp |  -------  |") + DM + "   Metropolis-Hastings" + R,
        "  " + g("                  \\  k_B * T /"),
        "  " + DM + "  T(t) = T0 * e^(-t/tau)    high T explores | low T exploits -> E=0" + R,
    ]


def cloud_block(frame, tw, busy, nann=1):
    h, w = 14, clamp(tw - 6, 20, 96)
    grid = [[" "] * w for _ in range(h)]
    zbuf = [[-9.0] * w for _ in range(h)]
    cbuf = [[0] * w for _ in range(h)]   # which annealer "owns" each cell (for multi-color)
    n = max(1, nann)
    ang = frame * 0.15           # faster rotation
    ca, sa = math.cos(ang), math.sin(ang)
    cb, sb = math.cos(ang * 0.6), math.sin(ang * 0.6)
    breath = 1.0 + (0.25 * math.sin(frame * 0.15) if busy else 0)
    for pi, (x0, y0, z0) in enumerate(_CLOUD):
        x, y, z = x0 * breath, y0 * breath, z0 * breath
        x, z = x * ca - z * sa, x * sa + z * ca
        y, z = y * cb - z * sb, y * sb + z * cb
        zp = z + 4.0
        if zp <= 0.1:
            continue
        f = 2.2 / zp
        sx = int(w / 2 + x * f * (w / 2.4))
        sy = int(h / 2 - y * f * (h / 1.4))
        if 0 <= sx < w and 0 <= sy < h and z > zbuf[sy][sx]:
            zbuf[sy][sx] = z
            di = clamp(int((z + 1.6) / 3.2 * (len(DEPTH) - 1)), 0, len(DEPTH) - 1)
            grid[sy][sx] = DEPTH[di]
            cbuf[sy][sx] = pi % n
    out = []
    for r_ in range(h):
        s = ""
        for c_ in range(w):
            ch = grid[r_][c_]
            if ch == " ":
                s += " "
            elif n > 1:
                s += ACOLORS[cbuf[r_][c_] % len(ACOLORS)] + ch + R
            else:
                col = QGRAD[clamp(int(DEPTH.index(ch) / (len(DEPTH) - 1) * (len(QGRAD) - 1)), 0, len(QGRAD) - 1)]
                s += col + ch + R
        out.append("    " + s)
    return out


def multi_cloud(frame, tw, anns):
    """Superimpose one rotating |psi|^2 cloud per annealer in the same 3D space,
    z-buffer composited, each its own color. Each cloud rotates at its own rate
    and VIBRATES (breath freq/amplitude) driven by its annealer's energy. 2x tall."""
    h, w = 26, clamp(tw - 6, 20, 100)
    grid = [[" "] * w for _ in range(h)]
    zbuf = [[-9.0] * w for _ in range(h)]
    cbuf = [[0] * w for _ in range(h)]
    clouds = anns if anns else [{"idx": 0, "score": None}]
    for ci, a in enumerate(clouds):
        temp = a.get("temp", 0.5)                           # this chain's OWN temperature
        vib = 0.10 + temp * 0.55                            # hotter chain -> vibrates faster/wilder
        rate = (0.05 + ci * 0.02) * (0.5 + temp)            # hotter chain -> rotates faster (distinct per cloud)
        ang = frame * rate + ci * 1.3
        ca, sa = math.cos(ang), math.sin(ang)
        cbb, sbb = math.cos(ang * 0.6), math.sin(ang * 0.6)
        breath = 1.0 + 0.30 * math.sin(frame * vib + ci)    # each vibrates differently
        for (x0, y0, z0) in _CLOUD:
            x, y, z = x0 * breath, y0 * breath, z0 * breath
            x, z = x * ca - z * sa, x * sa + z * ca
            y, z = y * cbb - z * sbb, y * sbb + z * cbb
            zp = z + 4.0
            if zp <= 0.1:
                continue
            f = 2.5 / zp
            sx = int(w / 2 + x * f * (w / 2.4))
            sy = int(h / 2 - y * f * (h / 2.3))
            if 0 <= sx < w and 0 <= sy < h and z > zbuf[sy][sx]:
                zbuf[sy][sx] = z
                di = clamp(int((z + 1.6) / 3.2 * (len(DEPTH) - 1)), 0, len(DEPTH) - 1)
                grid[sy][sx] = DEPTH[di]
                cbuf[sy][sx] = a.get("idx", ci)
    out = []
    for r_ in range(h):
        s = ""
        for c_ in range(w):
            ch = grid[r_][c_]
            s += " " if ch == " " else (ACOLORS[cbuf[r_][c_] % len(ACOLORS)] + ch + R)
        out.append("    " + s)
    return out


_sndlast = {}


def _sound_swarm(anns):
    # each annealer owns a distinct sound 'channel' (base pitch); chirps when it improves
    for a in anns:
        fn, sc = a.get("fn"), a.get("score")
        if sc is None:
            continue
        prev = _sndlast.get(fn)
        if prev is None:
            _sndlast[fn] = sc
        elif sc < prev:
            base = 262 + a.get("idx", 0) * 64
            _beep(base + clamp(int((prev - sc) / 10), 0, 280), 45)
            _sndlast[fn] = sc


def celebrate(fn, frame, tw, th):
    out = []
    cx, cy = tw // 2, 6
    R0 = (frame % 14)
    burst = QGRAD[frame % len(QGRAD)]
    out.append("")
    big = f"  ★ ★ ★   M A T C H   F O U N D   ★ ★ ★  "
    out.append(burst + BD + big.center(tw) + R)
    out.append(GR + BD + f"score 0 byte-exact :: {fn}".center(tw) + R)
    out.append("")
    for ry in range(9):
        line = [" "] * tw
        for ang in range(0, 360, 18):
            a = math.radians(ang + frame * 12)
            x = int(cx + math.cos(a) * R0 * 2.0)
            y = int(4 + math.sin(a) * R0)
            if 0 <= x < tw and 0 <= y < 9 and y == ry:
                line[x] = "*"
        s = "".join((QGRAD[(i + frame) % len(QGRAD)] + c + R) if c != " " else " " for i, c in enumerate(line))
        out.append(s)
    return out


_seen_wins = set()


def render(st, frame, tw, th):
    global _seen_wins
    out = []
    # width-adaptive title: spaced letters when roomy, compact when narrow
    title = " Q U A N T U M   A N N E A L E R " if tw >= 64 else " QUANTUM ANNEALER "
    sh = "".join(QGRAD[(i + frame) % len(QGRAD)] + c for i, c in enumerate(title))
    out.append("")
    out.append("  " + CY + BD + sh + R)
    out.append("  " + DM + "decomp-permuter * simulated annealing over CW codegen" + R)

    wins = st.get("wins") or []
    new = [w for w in wins if w not in _seen_wins]
    if new and (frame % 60) < 40:  # celebrate for a stretch
        if frame % 60 == 0:
            _fanfare()
        out += celebrate(new[-1], frame, tw, th)
        out.append("")
    if frame % 60 == 59:
        _seen_wins |= set(wins)

    out += eq_block(frame)
    out.append("")

    anns, _ = read_annealers()
    busy = bool(anns)
    _sound_swarm(anns)
    if anns:
        out.append("  " + GR + BD + "ACTIVE ANNEALERS " + R
                   + DM + f"({len(anns)} in parallel - each its own colour + sound channel; trace = energy log)" + R)
        for a in anns[:12]:
            col = ACOLORS[a["idx"] % len(ACOLORS)]
            e = "--" if a.get("score") is None else str(a.get("score"))
            b = "--" if a.get("best") is None else str(a.get("best"))
            spin = "◓◑◒◐"[(frame + a["idx"]) % 4]
            hist = a.get("hist") or []
            spk = ""
            if hist:
                lo, hi = min(hist), max(hist); rng = max(1, hi - lo)
                spk_w = clamp(tw - 48, 4, 16)   # sparkline shrinks with the pane
                spk = "".join(SPARK[clamp(int((v - lo) / rng * (len(SPARK) - 1)), 0, len(SPARK) - 1)]
                              for v in hist[-spk_w:])
            out.append("    " + col + BD + spin + " " + f"{a['fn']:<13}" + R
                       + DM + " it " + R + col + f"{a['iter']:>5,}" + R
                       + DM + " E=" + R + col + f"{str(e):<5}" + R
                       + DM + " best=" + R + col + BD + f"{str(b):<5}" + R
                       + "  " + col + spk + R)
    else:
        out.append("  " + DM + "swarm warming up / queue drained" + R)
    out.append("")

    out += multi_cloud(frame, tw, anns)
    out.append("  " + PINK + f"|psi(x)|^2 - {len(anns)} superimposed clouds" + R
               + DM + "  (one per annealer; each vibrates by its own energy)" + R)
    out.append("")

    # energy descent — a GROWING multi-chain time-series graph (one colored trace
    # per annealer, accumulating history; newest sample on the right)
    out.append("  " + YL + BD + "ENERGY DESCENT" + R
               + DM + "  (each chain's objdiff energy over time, growing; 0 = exact byte-match)" + R)
    out += energy_graph(anns, tw, frame)
    out.append("")

    # per-chain temperature — each function cools on ITS OWN iteration count
    out.append("  " + TEAL + BD + "PER-CHAIN TEMPERATURE" + R
               + DM + "  (high T explores | low T exploits; each cools independently)" + R)
    if anns:
        bw = clamp(tw - 30, 10, 48)
        for a in anns[:8]:
            T = a.get("temp", 0.5)
            col = ACOLORS[a["idx"] % len(ACOLORS)]
            filled = int(T * bw)
            bar = "".join((RD if i/bw > 0.6 else OR if i/bw > 0.3 else CY) + ("#" if i < filled else ".") + R
                          for i in range(bw))
            out.append("    " + col + f"{a['fn']:<13}" + R + " [" + bar + "] " + col + f"{T:0.3f}" + R)
    else:
        out.append("    " + DM + "no active chains" + R)
    out.append("")

    q = st.get("queue") or []
    nq = max(1, (tw - 16) // 14)               # how many fn names fit on one line
    qs = "  ".join(WH + fn + R for fn in q[:nq])
    if len(q) > nq:
        qs += DM + f"  +{len(q) - nq} more" + R
    out.append("  " + MG + BD + "QUEUE " + R + DM + f"({len(q)}) " + R + qs)
    done = st.get("done") or []
    if done:
        nd = max(1, (tw - 12) // 19)
        ds = "   ".join((GR + "[WIN] " + d["fn"] + R) if d.get("result") == "WIN"
                        else (DM + d["fn"] + f" E={d.get('score')}" + R) for d in done[-nd:])
        out.append("  " + GR + BD + "RESULTS " + R + ds)
    if wins:
        nw = max(1, (tw - 14) // 15)
        ws = ", ".join(wins[-nw:]) + (DM + f" +{len(wins) - nw} more" + R if len(wins) > nw else "")
        out.append("  " + GR + BD + "MATCHES: " + R + GR + ws + R)
    return out


def main():
    once = "--once" in sys.argv
    sys.stdout.write("\x1b[?25l")
    frame = 0
    try:
        while True:
            # re-read size every frame; honors COLUMNS/LINES env for testing
            tw, th = shutil.get_terminal_size((100, 44))
            lines = render(load(), frame, tw, th)
            # never emit a line wider than the pane: hard ANSI-aware truncation
            safe_w = max(20, tw - 1)
            lines = [vtrunc(ln, safe_w) for ln in lines]
            sys.stdout.write("\x1b[?2026h\x1b[H" + "".join(ln + R + "\x1b[K\n" for ln in lines[:th]) + "\x1b[J\x1b[?2026l")
            sys.stdout.flush()
            if once:
                break
            frame += 1
            time.sleep(0.22)
    except KeyboardInterrupt:
        pass
    finally:
        sys.stdout.write("\x1b[?25h" + R + "\n"); sys.stdout.flush()


if __name__ == "__main__":
    main()
