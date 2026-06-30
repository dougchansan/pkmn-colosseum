#!/usr/bin/env python3
"""research_log_view.py — live colored viewer for the research activity log
(.omc/research_log.jsonl, written by research_daemon.py). Newest at the bottom,
fixed-width columns (time | stage | fn | message), level-colored, so you can
watch the triage / oracle / inversion / swarm work as it happens.
Run with WSL python3. Ctrl-C to exit.

  python3 research_log_view.py            # live tail
  python3 research_log_view.py --once     # one frame (for capture)"""
import json, os, re, sys, time, shutil

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
LOG = os.path.join(REPO, ".omc", "research_log.jsonl")


def fg(n): return f"\x1b[38;5;{n}m"
R = "\x1b[0m"; BD = "\x1b[1m"
DM = fg(240); WH = fg(255); CY = fg(51); GR = fg(46); RD = fg(196)
STAGE_COL = {
    "DAEMON": fg(245), "TRIAGE": fg(255), "ORACLE": fg(199), "INVERT": fg(214),
    "SWARM": fg(51), "VERIFY": fg(46), "PREDICT": fg(226),
}
LEVEL_COL = {"win": fg(46), "best": fg(118), "invert": fg(214), "route": fg(213),
             "warn": fg(196), "info": fg(245)}

_ANSI_RE = re.compile(r"\x1b\[[0-9;?]*[A-Za-z]")


def vlen(s):
    """Visible length (ANSI escapes stripped)."""
    return len(_ANSI_RE.sub("", s))


def vtrunc(s, n):
    """Hard-truncate to n visible columns, ANSI-safe, with dim ellipsis."""
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


def load_events():
    try:
        lines = open(LOG, errors="replace").read().splitlines()
    except OSError:
        return []
    out = []
    for ln in lines:
        try:
            out.append(json.loads(ln))
        except Exception:
            pass
    return out


def fmt_time(ts):
    try:
        return time.strftime("%H:%M:%S", time.localtime(ts))
    except Exception:
        return "--:--:--"


def msg_color(level, msg):
    """Design language: win=green bold, FAIL=red, NEW BEST=cyan,
    start/finish chatter=dim; otherwise level-colored."""
    mu = msg.upper()
    if level == "win" or "WIN" in mu.split():
        return GR + BD
    if "FAIL" in mu or level == "warn":
        return RD
    if level == "best" or "NEW BEST" in mu:
        return CY
    if any(k in mu for k in ("START", "FINISH", "LAUNCH", "SPAWN", "BOOT")):
        return DM
    return LEVEL_COL.get(level, WH)


def render(evs, tw, th):
    out = []
    out.append("")
    out.append("  " + CY + BD + "RESEARCH ACTIVITY LOG" + R + "  " + DM
               + "triage -> oracle -> inversion -> swarm -> verify" + R)
    # one-line sticky column header
    msg_w = max(8, tw - 33)   # 2 + 8(time) +1+ 6(stage) +1+ 13(fn) +1+ msg
    out.append("  " + DM + BD + f"{'TIME':<8} {'STAGE':<6} {'FN':<13} "
               + f"{'MESSAGE':<{msg_w}}" + R)
    body_h = max(4, th - len(out) - 1)
    for ev in evs[-body_h:]:
        level = ev.get("level", "info")
        msg = (ev.get("msg", "") or "").replace("\n", " ")
        scol = STAGE_COL.get(ev.get("stage", ""), DM)
        lcol = msg_color(level, msg)
        stage = ev.get("stage", "?")[:6]
        fn = (ev.get("fn") or "")[:13]
        if len(msg) > msg_w:
            msg = msg[:max(0, msg_w - 1)] + "…"
        star = "  "
        if level == "win":
            star = GR + BD + "**" + R
        elif level in ("best", "invert"):
            star = lcol + "> " + R
        out.append("  " + DM + fmt_time(ev.get("ts", 0)) + R + " "
                   + scol + BD + f"{stage:<6}" + R + " "
                   + fg(45) + f"{fn:<13}" + R + star
                   + lcol + msg + R)
    return out


def main():
    once = "--once" in sys.argv
    sys.stdout.write("\x1b[?25l")
    try:
        while True:
            # re-read size every frame; honors COLUMNS/LINES env for testing
            tw, th = shutil.get_terminal_size((110, 40))
            evs = load_events()
            safe_w = max(20, tw - 1)
            lines = [vtrunc(ln, safe_w) for ln in render(evs, tw, th)]
            sys.stdout.write("\x1b[?2026h\x1b[H" + "".join(ln + R + "\x1b[K\n" for ln in lines[:th]) + "\x1b[J\x1b[?2026l")
            sys.stdout.flush()
            if once:
                break
            time.sleep(0.5)
    except KeyboardInterrupt:
        pass
    finally:
        sys.stdout.write("\x1b[?25h" + R + "\n"); sys.stdout.flush()


if __name__ == "__main__":
    main()
