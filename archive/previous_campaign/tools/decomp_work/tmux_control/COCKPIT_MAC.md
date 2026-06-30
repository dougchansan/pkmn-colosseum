# Mac/Linux cockpit (`cockpit.sh`)

A clean, self-contained multi-agent tmux cockpit for driving GLM + Codex on the
decomp pipeline. Replaces the Windows `control.sh`/`txk.sh`/psmux machinery (whose
worker scripts were never committed and which only existed to wrangle a native-PE
tmux). Uses real `tmux` — no SIGKILL wrapper, no registry file.

## Prerequisites
- `tmux`, `opencode`, and (for the codex pane) `codex` on PATH.
- A credentials file at `~/.config/decomp-keys/cockpit.env` (override with
  `DECOMP_COCKPIT_ENV`) that exports the keys, e.g.:
  ```sh
  export ZAI_API_KEY=...        # GLM coding plan (z.ai) — used by opencode model glm/glm-5.2
  ```
- opencode configured with the `glm` provider (z.ai coding endpoint) in
  `~/.config/opencode/opencode.json`. Verify with: `opencode run --model glm/glm-5.2 "hi"`.
- **Codex auth**: run `codex login` once (the ChatGPT OAuth token expires and must
  be refreshed interactively — `ask-codex` fails with a refresh-token error until you do).

## Two ways to drive an agent

**Headless dispatch (reliable — recommended for orchestration):**
```sh
cockpit.sh ask-glm   "rewrite fn_80012345 to match; show only the C"
cockpit.sh ask-codex "explain the regalloc wall in src/game/gs_field.c"   # needs codex login
```
These run `opencode run` / `codex exec` and print the agent's answer to stdout.
No tmux session required.

**Interactive panes (for a human at the keyboard):**
```sh
cockpit.sh bootstrap     # 2x2 grid: orchestrator | glm (opencode TUI) | codex | shell
cockpit.sh attach        # attach a real terminal — the TUIs render/persist while attached
```
> The opencode/codex TUIs need an attached client to stay alive; bootstrap then
> attach promptly. If a TUI pane shows a bare shell, run `cockpit.sh restart glm`.

## All commands
```
bootstrap | attach | list | status | kill
send <role> "text" | send-glm "text" | send-codex "text"   # type into the attached TUI
ask-glm "prompt" | ask-codex "prompt"                       # headless dispatch, prints answer
capture <role> [lines] | idle <role> | glm-idle | codex-idle
restart <glm|codex>
```
Roles: `orchestrator`, `glm`, `codex`, `shell` (tagged via the tmux `@role` pane option).

## Config (env overrides)
`DECOMP_SESSION` (default `decomp`) · `DECOMP_COCKPIT_ENV` · `DECOMP_GLM_MODEL`
(default `glm/glm-5.2`) · `DECOMP_REPO`.

## Notes
- Keys live OUTSIDE the repo (`~/.config/decomp-keys/`) and are never committed.
- If opencode errors with `no such column: name` after an opencode auto-update,
  its local DB schema is stale — reset it: `rm ~/.local/share/opencode/opencode.db*`
  (auth/config are separate and untouched).
