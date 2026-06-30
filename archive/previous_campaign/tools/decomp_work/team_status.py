#!/usr/bin/env python3
"""team_status.py — refresh .omc/agent_tokens.json + activity log from the LIVE
worktree team (codex / glm / deepseek / 3090-qwen). Run on a loop; the pokedex
dashboard reads agent_tokens.json for its agent readout and coordination/status.md
for the recent-activity feed.

Maps each agent by its tmux pane's WORKTREE PATH (stable — apps overwrite pane
titles, so title-matching is unreliable), and by its worktree branch for landed-
commit activity. Run with WSL python3."""
import json, os, subprocess, time

REPO = "/mnt/c/Users/douglaswhittingham/pkmn-colosseum"
COORD = os.path.join(REPO, "tools", "decomp_work", "coordination")
TOKENS = os.path.join(REPO, ".omc", "agent_tokens.json")
CURSOR = os.path.join(REPO, ".omc", "team_status_seen.json")

# name -> (model, branch, worktree-dir-basename, context-token-limit)
TEAM = {
    "codex":     ("gpt-5.5",         "decomp/codex",     "codex",     272_000),
    "glm":       ("glm-5.1",         "decomp/glm",       "glm",       200_000),
    "deepseek":  ("deepseek-v4-pro", "decomp/deepseek",  "deepseek",  128_000),
    "3090-qwen": ("qwen3:14b",       "decomp/bench3090", "bench3090",  40_000),
}
import re as _re


def parse_tokens(text, limit):
    """Best-effort context/token usage from a pane's TUI status line."""
    if not text:
        return 0
    m = _re.search(r'(\d+)\s*%\s*context\s*(used|left)?', text)
    if m:
        pct = int(m.group(1))
        if (m.group(2) or "used") == "left":
            pct = 100 - pct
        return int(pct / 100.0 * limit)
    # opencode shows a running total like "159.8K"
    m = _re.search(r'(\d+(?:\.\d+)?)\s*[kK]\b', text)
    if m:
        return int(float(m.group(1)) * 1000)
    m = _re.search(r'(\d+(?:\.\d+)?)\s*k\s*tokens', text)
    if m:
        return int(float(m.group(1)) * 1000)
    return 0
BUSY_MARKERS = ("esc to interrupt", "esc interrupt", "Working", "Thinking",
                "Pollinating", "Wrangling", "Generating", "Compiling", "Thought",
                "tokens", "Running", "Pondering", "Reticulating")


def tmux(*a):
    try:
        return subprocess.run(["tmux", *a], capture_output=True, text=True, timeout=6).stdout
    except Exception:
        return ""


def git(*a):
    try:
        return subprocess.run(["git", "-C", REPO, *a], capture_output=True, text=True, timeout=8).stdout
    except Exception:
        return ""


def panes_by_worktree():
    """Map worktree-dir-basename -> pane_id (robust to app-set pane titles)."""
    out = tmux("list-panes", "-t", "0", "-F", "#{pane_current_path}\t#{pane_id}")
    m = {}
    for l in out.strip().splitlines():
        if "\t" in l:
            path, pid = l.split("\t", 1)
            m[path.rstrip("/").split("/")[-1]] = pid
    return m


def is_busy(pid):
    c = tmux("capture-pane", "-t", pid, "-p")
    return any(mk in c for mk in BUSY_MARKERS)


def landed(branch):
    out = git("log", "--oneline", f"master..{branch}")
    return [l for l in out.strip().splitlines() if l]


def main():
    panes = panes_by_worktree()
    now = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
    agents = {}
    for name, (model, branch, wt, limit) in TEAM.items():
        pid = panes.get(wt)
        text = tmux("capture-pane", "-t", pid, "-p") if pid else ""
        busy = any(mk in text for mk in BUSY_MARKERS)
        used = parse_tokens(text, limit)
        lc = landed(branch)
        agents[name] = {
            "tokens_used": used, "limit": limit, "since": now,
            "status": "busy" if busy else "idle",
            "current_fn": ("working" if busy else (f"idle ({len(lc)} landed)" if lc else "idle")),
            "model": model, "branch": branch, "landed": len(lc),
            "online": pid is not None,
        }
    os.makedirs(os.path.dirname(TOKENS), exist_ok=True)
    json.dump({"agents": agents}, open(TOKENS, "w"), indent=2)

    # activity feed: append newly-seen branch commits to status.md
    seen = set()
    if os.path.exists(CURSOR):
        try: seen = set(json.load(open(CURSOR)))
        except Exception: seen = set()
    new_lines = []
    for name, (_m, branch, _wt, _lim) in TEAM.items():
        for line in landed(branch):
            sha = line.split()[0]
            if sha not in seen:
                seen.add(sha)
                new_lines.append(f"\n- **{now}** `{name}` — {line}")
    if new_lines:
        try:
            os.makedirs(COORD, exist_ok=True)
            with open(os.path.join(COORD, "status.md"), "a", encoding="utf-8") as f:
                f.write("".join(new_lines))
        except OSError:
            pass
    json.dump(sorted(seen), open(CURSOR, "w"))
    print({k: (v["status"], "online" if v["online"] else "offline", v["landed"]) for k, v in agents.items()})


if __name__ == "__main__":
    main()
