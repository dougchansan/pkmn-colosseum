#!/usr/bin/env python3
"""
Pokédex Decomp Dashboard — Horizontal HUD with 3D rotating Poké Ball.

Left panel:  Animated Poké Ball sphere with holo sparkles
Right panel: Dex completion stats, per-file progress bars, activity

The sphere renderer uses parametric sampling (phi/theta), perspective
projection, a z-buffer, and surface-normal dot-product shading —
adapted from the general ideas in ASCII donut/sphere demos
(github.com/sherwinvishesh/ASCII-Donut-Animation). The Poké Ball
geometry, color zones, and holo sparkle system are original.

Usage: python pokedex_dashboard.py [--once] [--no-color] [--reduced-motion]
"""

import os, sys, io, math, time, json, subprocess, random, shutil, threading

# Sound support (Windows only)
try:
    import winsound
    HAS_SOUND = True
except ImportError:
    HAS_SOUND = False

if sys.platform == "win32":
    sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding="utf-8", errors="replace")
    sys.stderr = io.TextIOWrapper(sys.stderr.buffer, encoding="utf-8", errors="replace")

# ─── Tuning ──────────────────────────────────────────────────────────────────

BALL_RADIUS    = int(os.environ.get("POKDEX_BALL_SCALE", "14"))
SPARKLE_DENSITY = float(os.environ.get("POKDEX_SPARKLE_DENSITY", "0.008"))
ROT_SPEED      = float(os.environ.get("POKDEX_ROT_SPEED", "0.03"))
REFRESH        = float(os.environ.get("POKDEX_REFRESH", "0.12"))
STATS_EVERY    = int(os.environ.get("POKDEX_STATS_REFRESH", "30"))
NO_COLOR       = "--no-color" in sys.argv or os.environ.get("POKDEX_NO_COLOR") == "1"
REDUCED        = "--reduced-motion" in sys.argv or os.environ.get("POKDEX_REDUCED_MOTION") == "1"

REPO       = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
COORD_DIR  = os.path.join(os.path.dirname(__file__), "coordination")
REVIEW_DIR = os.path.join(os.path.dirname(__file__), "review")
APPLIED_DIR = os.path.join(os.path.dirname(__file__), "applied")
TMUX_DIR   = os.path.join(os.path.dirname(__file__), "tmux_control")
MILE_FILE  = os.path.join(TMUX_DIR, "logs", "milestones.json")

# ─── ANSI ────────────────────────────────────────────────────────────────────

RST = "\033[0m"
BLD = "\033[1m"
DM  = "\033[2m"

def _c(t, *codes):
    return ("".join(codes) + t + RST) if not NO_COLOR else t

# Color codes
R91 = "\033[91m"; R31 = "\033[31m"; G92 = "\033[92m"; Y93 = "\033[93m"
B94 = "\033[94m"; M95 = "\033[95m"; C96 = "\033[96m"; W97 = "\033[97m"
GR37 = "\033[37m"

# ─── Sphere renderer ────────────────────────────────────────────────────────
#
# Parametric sphere: sample phi (0..pi) and theta (0..2pi).
# For each sample, compute 3D point, rotate around X and Z axes,
# perspective-project to 2D, shade via normal-dot-light, and write
# to a z-buffered screen buffer. The equator (phi ~ pi/2) splits
# the ball into red (upper) and white (lower) halves.

SHADE = " .`':;~=+*#%@"   # 13 chars, dark to bright — longer for smoother transitions

SPARKLE_CH = ["*", "+", ".", "'", "~", "x", "o", "*", "+", ".", "'", "~"]
HOLO_C = [Y93, C96, M95, W97, B94, G92, R91]

def render_sphere(screen_w, screen_h, angle):
    """
    Render a Poké Ball sphere into a (screen_w x screen_h) cell grid.
    Returns list of lists: grid[y][x] = colored_string or None (transparent).

    Uses ray-casting: for each screen pixel, check if it falls inside the
    sphere. If so, compute the surface normal, apply Y-axis rotation to
    the texture lookup (hemisphere/button), and shade via dot-product
    lighting. This guarantees a clean, gap-free sphere on every frame.

    angle = Y-axis rotation. The equator stays horizontal; only the
    shading highlight and button position rotate.
    """
    grid = [[None] * screen_w for _ in range(screen_h)]

    R = BALL_RADIUS
    cx = screen_w / 2.0
    cy = screen_h / 2.0
    aspect = 2.1  # terminal chars are ~2x taller than wide

    cosY, sinY = math.cos(angle), math.sin(angle)

    # World-space light direction (fixed: upper-right-front)
    # We rotate the light INTO the ball's local frame so the highlight
    # sweeps across the surface as the ball spins.
    wlx, wly, wlz = 0.4, -0.45, 0.8
    lm = math.sqrt(wlx*wlx + wly*wly + wlz*wlz)
    wlx /= lm; wly /= lm; wlz /= lm
    # Rotate light by -angle (inverse of ball rotation) into ball's frame
    lx = wlx * cosY - wlz * sinY
    ly = wly
    lz = wlx * sinY + wlz * cosY

    for sy in range(screen_h):
        for sx in range(screen_w):
            # Map screen pixel to normalized sphere coords [-1, 1]
            nx = (sx - cx) / R
            ny = (sy - cy) / (R / aspect)

            r2 = nx * nx + ny * ny
            if r2 > 1.0:
                continue  # outside sphere

            # Front-facing surface normal
            nz = math.sqrt(1.0 - r2)

            # Diffuse lighting — light is rotated so highlight moves with spin
            diff = nx * lx + ny * ly + nz * lz
            diff = max(0.0, diff)

            # Specular: reflect light off surface, check alignment with view (0,0,1)
            dot_nl = nx * lx + ny * ly + nz * lz
            rx = 2 * dot_nl * nx - lx
            ry = 2 * dot_nl * ny - ly
            rz = 2 * dot_nl * nz - lz
            spec = max(0.0, rz) ** 10 * 0.5

            intensity = min(1.0, diff * 0.78 + spec + 0.1)

            # Edge darkening
            edge = 1.0 - r2
            if edge < 0.2:
                intensity *= edge / 0.2

            si = max(0, min(int(intensity * (len(SHADE) - 1)), len(SHADE) - 1))
            ch = SHADE[si]

            # Apply Y-axis rotation to get "world" position for texture lookup
            # This determines which part of the ball is facing us
            wx = nx * cosY + nz * sinY
            wy = ny  # Y unchanged — equator stays horizontal
            wz = -nx * sinY + nz * cosY

            # Hemisphere: wy < 0 = upper (red), wy > 0 = lower (white)
            # (screen Y is inverted: negative = top)
            seam_w = 0.07
            is_seam = abs(wy) < seam_w

            # Button: small circle at the front center of the seam
            is_button = r2 < 0.035 and abs(wy) < seam_w * 2.5 and wz > 0.5
            is_btn_ring = r2 < 0.07 and r2 >= 0.035 and abs(wy) < seam_w * 2 and wz > 0.5
            is_upper = wy < -seam_w

            if is_button:
                cell = _c("●" if intensity > 0.45 else "◎", W97, BLD)
            elif is_btn_ring:
                cell = _c("○" if intensity > 0.35 else "·", GR37, BLD)
            elif is_seam:
                cell = _c("═" if intensity > 0.25 else "─", W97 if spec > 0.1 else GR37, BLD)
            elif is_upper:
                # Red hemisphere
                if spec > 0.2:
                    cell = _c(ch, Y93, BLD)
                elif intensity > 0.55:
                    cell = _c(ch, R91, BLD)
                elif intensity > 0.3:
                    cell = _c(ch, R91)
                else:
                    cell = _c(ch, R31, DM)
            else:
                # White hemisphere
                if spec > 0.2:
                    cell = _c(ch, C96, BLD)
                elif intensity > 0.55:
                    cell = _c(ch, W97, BLD)
                elif intensity > 0.3:
                    cell = _c(ch, W97)
                else:
                    cell = _c(ch, GR37, DM)

            grid[sy][sx] = cell

    return grid


def add_sparkles(grid, ball_w, ball_h, frame, radius=None):
    """
    Add holo sparkles to transparent (None) cells in the grid.
    Sparkles are confined to a ring zone: outside a margin around the
    sphere, but inside a border margin from the pane edges.
    """
    rng = random.Random(frame // 3)
    color_off = frame % len(HOLO_C)
    count = int(ball_w * ball_h * SPARKLE_DENSITY)

    cx = ball_w / 2.0
    cy = ball_h / 2.0
    aspect = 2.1
    R = radius if radius else min(ball_w // 2, int(ball_h * aspect))

    # Inner boundary: keep sparkles at least 3 chars away from sphere edge
    inner_margin = 3
    # Outer boundary: keep sparkles at least 2 chars from pane edges
    outer_margin = 2

    for _ in range(count):
        x = rng.randint(0, ball_w - 1)
        y = rng.randint(0, ball_h - 1)

        # Skip outer edge of pane
        if x < outer_margin or x >= ball_w - outer_margin:
            continue
        if y < outer_margin or y >= ball_h - outer_margin:
            continue

        # Distance from center in sphere-normalized coords
        nx = (x - cx) / R
        ny = (y - cy) / (R / aspect)
        dist = math.sqrt(nx * nx + ny * ny)

        # Skip if too close to sphere (inside sphere + margin zone)
        inner_r = 1.0 + inner_margin / R
        if dist < inner_r:
            continue

        if y < len(grid) and x < len(grid[y]) and grid[y][x] is None:
            ch = rng.choice(SPARKLE_CH)
            ci = (color_off + rng.randint(0, 3)) % len(HOLO_C)
            bold = BLD if rng.random() > 0.4 else ""
            grid[y][x] = _c(ch, HOLO_C[ci], bold)

    return grid


# ─── Data collection ─────────────────────────────────────────────────────────

_scan_cache = {}

def _extract_symbol_bytes(elf_path, sym_name):
    """Parse ELF32-BE .o and return (bytes, size) for sym_name in .text, else (None, 0)."""
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

def _is_c_block_matched(target_elf, base_elf, fn_name):
    tb, ts = _extract_symbol_bytes(target_elf, fn_name)
    cb, cs = _extract_symbol_bytes(base_elf, fn_name)
    if not tb or not cb or ts == 0 or cs == 0 or ts != cs:
        return False
    return tb == cb

def scan_sources():
    import re as _re
    results = {}
    target_elf = os.path.join(REPO, "build", "GC6E01", "obj", "auto_01_800055E0_text.o")
    for root, _, files in os.walk(os.path.join(REPO, "src")):
        for f in files:
            if not f.endswith(".c"): continue
            fp = os.path.join(root, f)
            rel = os.path.relpath(fp, REPO).replace("\\", "/")
            try: mtime = os.path.getmtime(fp)
            except OSError: continue
            # cache key includes compiled .o mtime so flips are visible
            rel_no_src = rel[4:] if rel.startswith("src/") else rel
            base_elf = os.path.join(REPO, "build", "GC6E01", "base", rel_no_src[:-2] + ".o")
            try: bmtime = os.path.getmtime(base_elf)
            except OSError: bmtime = 0
            cache_key = (fp, mtime, bmtime)
            cached = _scan_cache.get(rel)
            if cached and cached[0] == cache_key:
                results[rel] = cached[1]
                continue
            asm_active = 0        # #if 1 + asm = matched-by-construction
            c_matched = 0         # #if 0 + asm / #else C, and bytes match target
            c_unmatched = 0       # #if 0 block but bytes differ or compile missing
            try:
                with open(fp, "r", encoding="utf-8", errors="replace") as fh:
                    lines = fh.readlines()
            except OSError:
                continue
            i = 0
            while i < len(lines):
                s = lines[i].strip()
                if s == "#if 1" and i+1 < len(lines):
                    am = _re.match(r"\s*asm\s+\w+\s+(fn_\w+)", lines[i+1])
                    if am:
                        asm_active += 1
                        i += 2; continue
                elif s == "#if 0" and i+1 < len(lines):
                    am = _re.match(r"\s*asm\s+\w+\s+(fn_\w+)", lines[i+1])
                    if am:
                        fn = am.group(1)
                        if _is_c_block_matched(target_elf, base_elf, fn):
                            c_matched += 1
                        else:
                            c_unmatched += 1
                        i += 2; continue
                i += 1
            total = asm_active + c_matched + c_unmatched
            if total > 0:
                # Display contract: "matched" = asm_active + c_matched; "asm" = c_unmatched (remaining work)
                entry = {"asm": c_unmatched, "matched": asm_active + c_matched}
                _scan_cache[rel] = (cache_key, entry)
                results[rel] = entry
    return results

def count_json(d):
    try: return len([f for f in os.listdir(d) if f.endswith(".json")])
    except OSError: return 0

def get_coord():
    try:
        with open(os.path.join(COORD_DIR, "tasks.json")) as f:
            tasks = json.load(f)
        return (len([t for t in tasks if t["status"]=="queued"]),
                len([t for t in tasks if t["status"]=="claimed"]),
                len([t for t in tasks if t["status"]=="completed"]))
    except: return 0, 0, 0

def _utc_to_hst(ts_str):
    """Convert a UTC timestamp string to HST (UTC-10)."""
    import re as _re
    m = _re.match(r'(\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})Z?', ts_str)
    if not m:
        return ts_str
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
                    # Sanitize to ASCII
                    s = s.replace('\u2014', ' - ').replace('\u2013', ' - ')
                    s = s.replace('\u2018', "'").replace('\u2019', "'")
                    # Convert UTC timestamps to HST
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

# Milestones
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

def bar(ratio, w=28, caught=0, total=0):
    filled = int(ratio * w)
    if ratio >= 0.7: bc = G92 if ratio >= 1.0 else C96
    elif ratio >= 0.4: bc = Y93
    else: bc = R91
    return _c("█"*filled, bc) + _c("░"*(w-filled), DM)

# ─── Sparkle rain celebration ────────────────────────────────────────────────
#
# When a new match is detected (total_matched increases), spawn falling
# yellow sparkle particles that rain down the screen like Pokémon game
# catch celebrations. Particles have random x positions, fall at
# slightly different speeds, and fade after reaching the bottom.

SOUND_ON       = os.environ.get("POKDEX_SOUND", "1") != "0" and HAS_SOUND

# ─── Sound effects ───────────────────────────────────────────────────────────
#
# Pokémon-inspired level-up sound using winsound.Beep (Windows built-in).
# An ascending arpeggio that captures the spirit of the level-up jingle
# without using copyrighted audio. Runs in a background thread so it
# doesn't block the render loop.

def _play_levelup_sound():
    """Ascending arpeggio — Pokémon level-up inspired."""
    if not SOUND_ON:
        return
    def _play():
        try:
            # C5 → E5 → G5 → C6 ascending major arpeggio
            notes = [523, 659, 784, 1047]
            for freq in notes:
                winsound.Beep(freq, 100)
            # Final sustained high note
            winsound.Beep(1047, 200)
        except Exception:
            pass
    threading.Thread(target=_play, daemon=True).start()

def _play_milestone_sound():
    """Bigger fanfare for milestone achievements."""
    if not SOUND_ON:
        return
    def _play():
        try:
            # C5 → E5 → G5 → C6 → E6 → G6 with longer notes
            notes = [(523, 80), (659, 80), (784, 80), (1047, 120),
                     (1319, 120), (1568, 250)]
            for freq, dur in notes:
                winsound.Beep(freq, dur)
        except Exception:
            pass
    threading.Thread(target=_play, daemon=True).start()

RAIN_CHARS = ["*", "+", ".", "'", "~", "o", "x", "."]

def _bar_color_for_ratio(ratio):
    """Return the ANSI color code matching the progress bar for a given ratio."""
    if ratio >= 1.0: return G92
    elif ratio >= 0.7: return C96
    elif ratio >= 0.4: return Y93
    else: return R91

class SparkleRain:
    """Manages falling sparkle particles for match celebrations."""

    def __init__(self):
        self.particles = []   # list of [x, y_float, speed, char, color]
        self.active = False
        self.ttl = 0          # frames remaining

    def trigger(self, width, height, color=None):
        """Spawn a burst of falling particles in the given color."""
        self.active = True
        self.ttl = 40  # ~5 seconds at 8fps
        rain_color = color or Y93
        self._rain_color = rain_color
        # Mix: 70% main color, 30% white for shimmer
        colors = [rain_color] * 7 + [W97] * 3
        rng = random.Random()
        for _ in range(random.randint(30, 50)):
            x = rng.randint(0, width - 1)
            y = rng.uniform(-5.0, 0.0)
            speed = rng.uniform(0.3, 1.2)
            ch = rng.choice(RAIN_CHARS)
            c = rng.choice(colors)
            self.particles.append([x, y, speed, ch, c])

    def tick(self, height):
        """Advance particles one frame. Returns False when done."""
        if not self.active:
            return False
        self.ttl -= 1
        alive = []
        for p in self.particles:
            p[1] += p[2]  # y += speed
            if p[1] < height + 2:  # keep if still on screen
                alive.append(p)
        self.particles = alive
        if self.ttl <= 0 and not self.particles:
            self.active = False
            return False
        # Spawn a few more while ttl > 0 (continuous rain effect)
        if self.ttl > 0:
            rng = random.Random()
            color = self._rain_color if hasattr(self, '_rain_color') else Y93
            colors = [color] * 7 + [W97] * 3
            for _ in range(random.randint(2, 5)):
                x = rng.randint(0, self.last_width - 1) if hasattr(self, 'last_width') else rng.randint(0, 80)
                self.particles.append([
                    x, rng.uniform(-2.0, 0.0),
                    rng.uniform(0.3, 1.2),
                    rng.choice(RAIN_CHARS),
                    rng.choice(colors)
                ])
        return True

    def overlay(self, lines, tw):
        """Overlay particles onto the rendered frame lines (in-place)."""
        if not self.active:
            return
        self.last_width = tw
        for p in self.particles:
            x, y = int(p[0]), int(p[1])
            if 0 <= y < len(lines) and 0 <= x < tw:
                # Convert line to list for char replacement
                row = list(lines[y])
                # Only overlay on space characters (don't clobber content)
                if x < len(row) and row[x] == " ":
                    row[x] = _c(p[3], p[4], BLD)
                    lines[y] = "".join(row)

# Global rain instance
_rain = SparkleRain()

# ─── Battle scene ────────────────────────────────────────────────────────────
#
# Gen 1-style battle UI: enemy (top-right), our Pokemon (bottom-left),
# HP bars, level display, and text box. The user's original pixel art
# is traced into ASCII below.

# User's original creature "Yujula" (back sprite — facing away from us)
YUJULA_BACK = [
    "   @@@@     ",
    "  @    @>   ",
    " @  .  @==  ",
    " @      @   ",
    "  @@@@@@    ",
    "  @    @    ",
    " @%    %@   ",
    " @      @   ",
    "  @    @    ",
    "  =    =    ",
]

# User's original glitch creature "MissingNo" (front sprite)
GLITCH_FRONT = [
    "# .@##  %@. #  =",
    " @# .%@  #=  @% ",
    "=  ##@ .# %@#  .",
    "@ %# =  @.# =%@ ",
    " #@.  %# = @# . ",
    "= @# %  .@= #%  ",
    ".# = @%#  . =@# ",
    "@ .# =  @%# . = ",
]

# Map source files to "Pokemon" names for the battle display
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

class BattleScene:
    """Manages the Gen 1-style battle display state."""

    def __init__(self):
        self.active_file = None       # which file is "fighting"
        self.active_name = "YUJULA"   # Pokemon name
        self.our_level = 0            # match % as level
        self.enemy_hp_pct = 100.0     # overall remaining % (inverse of decomp progress)
        self.exp_matched = 0          # matched count for active file
        self.exp_total = 1            # total funcs for active file
        self.battle_text = "Wild MISSINGNO appeared!"
        self.text_ttl = 0             # frames to show attack text
        self.attack_frame = 0         # animation counter (12 frames for flashier)
        self.evolving = False         # evolution animation active
        self.evolve_ttl = 0           # evolution frames remaining

    def update(self, stats, matched_file=None, matched_delta=0):
        """Update battle state from current stats."""
        ta = stats["ta"]; tm = stats["tm"]; tot = ta + tm
        # Enemy HP = inverse of decomp progress (100% decomp = 0 HP)
        decomp_pct = (tm / tot * 100) if tot > 0 else 0
        self.enemy_hp_pct = 100.0 - decomp_pct

        if matched_file:
            # Find the file that matched
            for fpath, name in FILE_POKEMON.items():
                if matched_file and fpath.endswith(matched_file.split("(")[0].strip()):
                    self.active_name = name
                    self.active_file = fpath
                    break
            else:
                short = matched_file.split("(")[0].strip()
                self.active_name = short.upper()[:11]

            fdata = stats["files"].get(self.active_file)
            if fdata:
                ft = fdata["asm"] + fdata["matched"]
                self.our_level = int(fdata["matched"] / ft * 100) if ft > 0 else 0
                self.exp_matched = fdata["matched"]
                self.exp_total = ft
                # Check for evolution: file reached 100%
                if fdata["asm"] == 0 and ft > 0:
                    self.evolving = True
                    self.evolve_ttl = 20
            else:
                self.our_level = int(decomp_pct) if tot > 0 else 0
                self.exp_matched = tm
                self.exp_total = tot

            self.battle_text = f"{self.active_name} used DECOMP!"
            self.text_ttl = 30
            self.attack_frame = 12  # longer for flashier animation
        elif self.evolve_ttl > 0:
            self.evolve_ttl -= 1
            if self.evolve_ttl > 10:
                self.battle_text = f"{self.active_name} is evolving...!"
            elif self.evolve_ttl > 0:
                self.battle_text = f"{self.active_name} evolved! 100% MATCHED!"
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
            # Default: cycle through active files, prefer most-worked-on
            best_file = None
            best_matched = 0
            for fpath in FILE_POKEMON:
                fdata = stats["files"].get(fpath)
                if fdata and fdata["matched"] > best_matched:
                    best_matched = fdata["matched"]
                    best_file = fpath
            if best_file:
                self.active_file = best_file
                self.active_name = FILE_POKEMON.get(best_file, "YUJULA")
                fdata = stats["files"][best_file]
                ft = fdata["asm"] + fdata["matched"]
                self.our_level = int(fdata["matched"] / ft * 100) if ft > 0 else 0
                self.exp_matched = fdata["matched"]
                self.exp_total = ft

        if self.attack_frame > 0:
            self.attack_frame -= 1

    def render(self, width, height, frame):
        """Render the battle scene as a list of strings."""
        lines = []
        bw = max(2, width - 2)  # inner width for borders

        # Idle bobbing offsets (like old Pokemon games — slight up/down)
        enemy_bob = 1 if (frame // 6) % 2 == 0 else 0
        our_bob = 1 if (frame // 8) % 2 == 0 else 0
        # Slight horizontal sway
        enemy_sway = 1 if (frame // 10) % 3 == 1 else 0
        our_sway = 1 if (frame // 12) % 3 == 1 else 0

        # ─── Enemy info (top area, right-aligned) ───
        enemy_name = "MISSINGNO"
        enemy_hp = max(0, min(100, self.enemy_hp_pct))
        hp_bar_w = max(6, bw - 18)
        hp_filled = int((enemy_hp / 100) * hp_bar_w)
        hp_empty = hp_bar_w - hp_filled

        if enemy_hp > 50: hp_color = G92
        elif enemy_hp > 25: hp_color = Y93
        else: hp_color = R91

        # Right-align enemy info
        info1 = f"{enemy_name}   Lv??"
        info2 = f"HP:" + "=" * hp_filled + "-" * hp_empty + f" {enemy_hp:.0f}%"
        pad1 = " " * max(0, bw - len(info1))
        pad2 = " " * max(0, bw - len(info2))
        lines.append(pad1 + _c(enemy_name, W97, BLD) + _c("   Lv??", GR37))
        lines.append(pad2 + _c("HP:", GR37) + _c("=" * hp_filled, hp_color) + _c("-" * hp_empty, DM) + _c(f" {enemy_hp:.0f}%", GR37))

        # ─── Gap + enemy bob ───
        for _ in range(2 + enemy_bob):
            lines.append("")

        # ─── Enemy sprite (right side, with sway) ───
        glitch = list(GLITCH_FRONT)
        # Animate glitch during attack: randomize chars
        if self.attack_frame > 0 and self.attack_frame % 2 == 0:
            rng = random.Random(frame)
            glitch = []
            for row in GLITCH_FRONT:
                new_row = list(row)
                for i in range(len(new_row)):
                    if rng.random() < 0.4:
                        new_row[i] = rng.choice("#@%=+. ")
                glitch.append("".join(new_row))

        sprite_w = max(len(r) for r in glitch)
        for row in glitch:
            pad = " " * max(0, bw - sprite_w - 2 + enemy_sway)
            lines.append(pad + _c(row, GR37))

        # ─── Fireball attack animation (flashier, 12 frames) ───
        if self.attack_frame > 0:
            progress = 1.0 - (self.attack_frame / 12.0)  # 0.0 -> 1.0
            fb_x = int(2 + progress * (bw - 8))
            if progress < 0.3:
                # Charge-up phase
                fb = _c("  ~*~", Y93, BLD)
            elif progress < 0.8:
                # Traveling fireball with trail
                trail = _c("~", R91) + _c("~", Y93) + _c("=", R91, BLD)
                head = _c("*))>", Y93, BLD)
                fb = trail + head
            elif progress < 0.95:
                # Impact
                fb = _c("<<**BOOM**>>", Y93, BLD)
            else:
                fb = _c("  ~  .  ~  .", GR37, DM)
            lines.append(" " * fb_x + fb)
            # Second trail line for thickness
            if 0.3 < progress < 0.8:
                fb_x2 = max(0, fb_x - 1)
                lines.append(" " * fb_x2 + _c("  " + "~" * int(progress * 6), R91, DM))
            else:
                lines.append("")
        else:
            lines.append("")
            lines.append("")

        # ─── Spacing to push our pokemon down ───
        target_our_start = height - 16  # our pokemon area starts here
        while len(lines) < target_our_start + our_bob:
            lines.append("")

        # ─── Our Pokemon (left side, with sway) ───
        # Evolution animation: flash the sprite
        if self.evolving and self.evolve_ttl > 10:
            # Flashing sprite during evolution
            if frame % 3 == 0:
                for row in YUJULA_BACK:
                    lines.append(" " * (2 + our_sway) + _c(row, Y93, BLD))
            elif frame % 3 == 1:
                for row in YUJULA_BACK:
                    lines.append(" " * (2 + our_sway) + _c(row, W97, BLD))
            else:
                for _ in YUJULA_BACK:
                    lines.append(" " * (2 + our_sway) + _c("  * * * * * ", Y93, BLD))
        elif self.evolving and self.evolve_ttl > 0:
            # Post-evolution: bright green
            for row in YUJULA_BACK:
                lines.append(" " * (2 + our_sway) + _c(row, G92, BLD))
        else:
            for row in YUJULA_BACK:
                lines.append(" " * (2 + our_sway) + _c(row, W97))

        # ─── Our info + EXP bar ───
        sw = " " * (1 + our_sway)
        lines.append(sw + _c(f" {self.active_name}", W97, BLD) +
                     _c(f"  Lv{self.our_level}", GR37))

        # HP line
        our_fdata = stats_ref[0].get("files", {}).get(self.active_file, {}) if stats_ref[0] else {}
        if our_fdata:
            m = our_fdata.get("matched", 0)
            ft = our_fdata.get("asm", 0) + m
            lines.append(sw + _c(f" HP: {m}/{ft}", GR37))
        else:
            m = self.exp_matched
            ft = self.exp_total
            lines.append(sw + _c(f" HP: {m}/{ft}", GR37))

        # EXP bar — shows progress toward 100% for this file
        exp_bar_w = max(6, min(20, bw - 10))
        exp_ratio = m / ft if ft > 0 else 0
        exp_filled = int(exp_ratio * exp_bar_w)
        exp_empty = exp_bar_w - exp_filled
        if exp_ratio >= 1.0:
            exp_color = G92
        elif exp_ratio >= 0.7:
            exp_color = C96
        else:
            exp_color = B94
        lines.append(sw + _c(" EXP:", GR37) +
                     _c("=" * exp_filled, exp_color, BLD) +
                     _c("-" * exp_empty, DM) +
                     _c(f" {exp_ratio*100:.0f}%", GR37))

        # ─── Text box (bottom, with full border) ───
        border_top = "+" + "-" * bw + "+"
        border_bot = "+" + "-" * bw + "+"
        txt = f" {self.battle_text}"
        if len(txt) < bw:
            txt = txt + " " * (bw - len(txt))
        lines.append(_c(border_top, GR37))
        lines.append(_c("|", GR37) + _c(txt[:bw], W97) + _c("|", GR37))
        lines.append(_c(border_bot, GR37))

        # Pad to height
        while len(lines) < height:
            lines.append("")
        return lines[:height]

# Global battle state + stats reference for the render method
_battle = BattleScene()
stats_ref = [None]  # mutable ref so battle.render can access stats

# ─── Compose vertical layout ────────────────────────────────────────────────

def render_frame(stats, frame, tw, th):
    """
    Vertical layout: title → ball (top, centered) → stats table (bottom).
    Returns list of strings, one per terminal row.
    """
    out = []

    # ─── ASCII art title (Pokémon Colosseum style) ───
    for _ in range(5):
        out.append("")

    # Clean 3-row half-block font — reliable rendering, clear at any size
    margin = 4
    pokemon_art = [
        "█▀█ █▀█ █▀▄ █▀▀ █▀▄▀█ █▀█ █▀█",
        "█▀▀ █ █ █▀▄ ██▀ █ ▀ █ █ █ █ █",
        "▀   ▀▀▀ ▀ ▀ ▀▀▀ ▀   ▀ ▀▀▀ ▀ ▀",
    ]
    colosseum_art = [
        "█▀▀ █▀█ █   █▀█ █▀▀ █▀▀ █▀▀ █ █ █▀▄▀█",
        "█   █ █ █   █ █ ▀▀█ ▀▀█ ██▀ █ █ █ ▀ █",
        "▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀▀▀ ▀   ▀",
    ]

    # Center within margin zone
    usable = tw - margin * 2
    for line in pokemon_art:
        inner_pad = max(0, (usable - len(line)) // 2)
        out.append(" " * (margin + inner_pad) + _c(line, R91, BLD))
    for line in colosseum_art:
        inner_pad = max(0, (usable - len(line)) // 2)
        out.append(" " * (margin + inner_pad) + _c(line, C96, BLD))

    # ─── Ball + Battle (side by side) ───
    remaining_h = th - len(out) - 1
    ball_h = max(6, int(remaining_h * 0.45))
    ball_w = max(20, int(tw * 0.40))  # ball gets left 40%

    # Auto-compute radius to fit: radius must fit in both width and height
    max_r_w = (ball_w - 4) // 2        # horizontal (each char = 1 unit)
    max_r_h = int((ball_h - 2) * 2.1)  # vertical (corrected for aspect)
    auto_r = min(max_r_w, max_r_h, BALL_RADIUS)
    auto_r = max(4, auto_r)

    # Temporarily override radius for this frame
    saved_r = BALL_RADIUS
    angle = frame * ROT_SPEED if not REDUCED else 0.0

    # Render with auto-sized radius
    grid = [[None] * ball_w for _ in range(ball_h)]
    zbuf = [[0.0] * ball_w for _ in range(ball_h)]
    R = auto_r
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
            if r2 > 1.0:
                continue
            nz = math.sqrt(1.0 - r2)
            diff = max(0.0, nx * llx + ny * lly + nz * llz)
            dot_nl = nx * llx + ny * lly + nz * llz
            rz = 2 * dot_nl * nz - llz
            spec = max(0.0, rz) ** 10 * 0.5
            intensity = min(1.0, diff * 0.78 + spec + 0.1)
            edge = 1.0 - r2
            if edge < 0.2:
                intensity *= edge / 0.2
            si = max(0, min(int(intensity * (len(SHADE) - 1)), len(SHADE) - 1))
            ch = SHADE[si]
            wx = nx * cosYa + nz * sinYa
            wy = ny
            wz = -nx * sinYa + nz * cosYa
            seam_w = 0.07
            is_seam = abs(wy) < seam_w
            is_button = r2 < 0.035 and abs(wy) < seam_w * 2.5 and wz > 0.5
            is_btn_ring = r2 < 0.07 and r2 >= 0.035 and abs(wy) < seam_w * 2 and wz > 0.5
            is_upper = wy < -seam_w

            if is_button:
                cell = _c("●" if intensity > 0.45 else "◎", W97, BLD)
            elif is_btn_ring:
                cell = _c("○" if intensity > 0.35 else "·", GR37, BLD)
            elif is_seam:
                cell = _c("═" if intensity > 0.25 else "─", W97 if spec > 0.1 else GR37, BLD)
            elif is_upper:
                if spec > 0.2: cell = _c(ch, Y93, BLD)
                elif intensity > 0.55: cell = _c(ch, R91, BLD)
                elif intensity > 0.3: cell = _c(ch, R91)
                else: cell = _c(ch, R31, DM)
            else:
                if spec > 0.2: cell = _c(ch, C96, BLD)
                elif intensity > 0.55: cell = _c(ch, W97, BLD)
                elif intensity > 0.3: cell = _c(ch, W97)
                else: cell = _c(ch, GR37, DM)
            grid[sy][sx] = cell

    # Convert ball grid to strings
    ball_lines = []
    for row in grid:
        ball_lines.append("".join(cell if cell is not None else " " for cell in row))

    # Render battle scene for right side
    battle_w = tw - ball_w - 1
    stats_ref[0] = stats
    _battle.update(stats)
    battle_lines = _battle.render(battle_w, ball_h, frame)

    # Compose side by side: ball (left) | battle (right)
    for i in range(ball_h):
        left = ball_lines[i] if i < len(ball_lines) else " " * ball_w
        right = battle_lines[i] if i < len(battle_lines) else ""
        out.append(left + " " + right)

    # ─── "DECOMP" subtitle below ───
    decomp_art = [
        "█▀▄ █▀▀ █▀▀ █▀█ █▀▄▀█ █▀█",
        "█ █ ██▀ █   █ █ █ ▀ █ █▀▀",
        "▀▀  ▀▀▀ ▀▀▀ ▀▀▀ ▀   ▀ ▀  ",
    ]
    for line in decomp_art:
        dpad = max(0, (tw - len(line)) // 2)
        out.append(" " * dpad + _c(line, Y93, BLD))

    # ─── Stats (bottom section) ───
    ta = stats["ta"]; tm = stats["tm"]; tot = ta + tm
    ratio = tm / tot if tot > 0 else 0

    # Dex completion — compact multi-line box
    bar_w = max(12, tw - 30)
    main_b = bar(ratio, w=bar_w)
    pct = f"{ratio*100:.1f}%"
    rc = stats["review"]; ac = stats["applied"]
    q, c, d = stats["coord"]
    cs = stats.get("codex", "?")
    cs_fmt = _c(f"● {cs}", G92, BLD) if cs == "IDLE" else _c(f"◆ {cs}", Y93, BLD) if cs == "BUSY" else _c(f"○ {cs}", DM)

    # Phase milestones from decomp plan
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
    current_badge = ""
    next_target = ""
    for threshold, badge, desc in phase_milestones:
        pct_val = ratio * 100
        if pct_val >= threshold:
            current_badge = f"[{badge}] {desc}"
            break
    for threshold, badge, desc in reversed(phase_milestones):
        if ratio * 100 < threshold:
            funcs_needed = int(threshold / 100 * tot) - tm
            next_target = f"Next: {badge} in {funcs_needed} matches"
            break

    out.append(_c("--- DEX COMPLETION " + "-"*max(1, tw-20), Y93))
    out.append(f"  {_c('*',R91)} {main_b} {pct} {tm}/{tot}")
    out.append(f"  Caught:{_c(str(tm),G92,BLD)} Rem:{_c(str(ta),R91,BLD)}" +
        f" Rev:{_c(str(rc),Y93,BLD)} App:{_c(str(ac),C96,BLD)}" +
        f"  Q:{q} C:{c} D:{d} Codex:{cs_fmt}")
    if current_badge:
        out.append(f"  {_c(current_badge, Y93, BLD)}")
    if next_target:
        out.append(f"  {_c(next_target, DM)}")

    # Route table
    fs = stats["files"]
    sf = sorted([(f,d) for f,d in fs.items() if f in TOP],
                key=lambda x: x[1]["asm"], reverse=True)

    out.append(_c("--- POKEDEX BY ROUTE " + "-"*max(1, tw-22), M95))

    # How many files can we fit?
    max_files = min(len(sf), max(6, th - len(out) - 5))
    name_w = min(24, tw - 30)
    bar_w = max(8, tw - name_w - 20)

    for fp, data in sf[:max_files]:
        a, m = data["asm"], data["matched"]
        ft = a + m; fr = m/ft if ft > 0 else 0
        short = fp.replace("src/game/","").replace("src/","")
        if len(short) > name_w: short = short[-name_w:]
        b = bar(fr, w=bar_w)
        out.append(f"  {short:<{name_w}s} {b} {fr*100:5.1f}% {m:>3d}/{ft}")

    # Legend + status message
    msg = MSGS[(frame // 12) % len(MSGS)]
    out.append(f"  {_c('#',G92)} Caught {_c('.',DM)} Rem {_c('*',Y93)} Rev {_c('+',C96)} App" +
               f"  {_c(msg, DM)}")

    # Recent activity (if room) — show agent discoveries and match results
    sl = stats.get("status_lines", [])
    if sl and len(out) < th - 3:
        out.append(_c("  --- Recent Activity ---", GR37))
        for s in reversed(sl[-25:]):
            if len(out) >= th - 1: break
            clean = s.replace("**","").replace("- ","  ")
            if len(clean) > tw - 2: clean = clean[:tw-5] + "..."
            # Final ASCII safety net
            clean = clean.encode('ascii', 'ignore').decode('ascii')
            # Highlight match/claim/complete events
            if "Completed" in clean or "matched" in clean.lower():
                out.append(_c(clean, G92))
            elif "Claimed" in clean:
                out.append(_c(clean, C96))
            elif "Enqueued" in clean:
                out.append(_c(clean, Y93))
            else:
                out.append(_c(clean, GR37))

    # Milestone badge (handled by phase milestones in dex completion section)

    # Pad to terminal height
    while len(out) < th:
        out.append("")

    return out[:th]

# ─── Main loop ───────────────────────────────────────────────────────────────

def main():
    once = "--once" in sys.argv
    frame = 0
    stats = None
    ms = load_ms()
    prev_matched = None   # track previous total match count
    prev_file_stats = {}  # track per-file matched counts for color detection

    while True:
        try:
            t0 = time.monotonic()
            term = shutil.get_terminal_size((120, 40))
            tw, th = term.columns, term.lines

            if stats is None or frame % STATS_EVERY == 0:
                fs = scan_sources()
                ta = sum(d["asm"] for d in fs.values())
                tm = sum(d["matched"] for d in fs.values())
                stats = {
                    "ta": ta, "tm": tm, "files": fs,
                    "review": count_json(REVIEW_DIR),
                    "applied": count_json(APPLIED_DIR),
                    "coord": get_coord(),
                    "status_lines": get_status(25),
                    "codex": get_codex(),
                }
                # Milestone check
                tot = ta + tm
                pct = tm/tot*100 if tot > 0 else 0
                for t in sorted(MILESTONES.keys()):
                    if pct >= t and t not in ms.get("triggered",[]):
                        ms.setdefault("triggered",[]).append(t)
                        save_ms(ms)
                        _play_milestone_sound()

                # Celebration: trigger sparkle rain when matches increase
                # Detect which file changed and use its bar color
                if prev_matched is not None and tm > prev_matched:
                    rain_color = Y93  # default yellow
                    matched_file = "unknown"
                    matched_delta = tm - prev_matched
                    for fpath, fdata in fs.items():
                        prev_m = prev_file_stats.get(fpath, 0)
                        cur_m = fdata["matched"]
                        if cur_m > prev_m:
                            ftotal = fdata["asm"] + fdata["matched"]
                            fratio = cur_m / ftotal if ftotal > 0 else 0
                            rain_color = _bar_color_for_ratio(fratio)
                            short = fpath.replace("src/game/","").replace("src/","")
                            matched_file = f"{short} ({fratio*100:.0f}%)"
                            break
                    _rain.trigger(tw, th, color=rain_color)
                    _play_levelup_sound()
                    # Trigger battle attack animation
                    _battle.update(stats, matched_file=matched_file, matched_delta=matched_delta)
                    # Write match event to coordination status log
                    try:
                        from datetime import datetime
                        ts = datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%SZ')
                        with open(os.path.join(COORD_DIR, "status.md"), "a") as f:
                            f.write(f"\n- **{ts}** `dashboard` — MATCH! +{matched_delta} in {matched_file}")
                    except: pass
                prev_matched = tm
                prev_file_stats = {fp: d["matched"] for fp, d in fs.items()}

            output = render_frame(stats, frame, tw, th)

            # Tick sparkle rain and overlay particles on the frame
            _rain.tick(th)
            _rain.overlay(output, tw)

            # Single write — cursor home + all lines joined, each with clear-to-EOL
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
