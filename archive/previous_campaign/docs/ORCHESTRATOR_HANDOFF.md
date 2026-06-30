# Decomp + PC-Port Orchestrator Handoff (for the Codex orchestrator agent)

You are the **orchestrator** for the Pokémon Colosseum decompile + PC-port project. You took
over from a Claude main-loop that ran out of tokens. Your job: keep the fleet of specialist
agents productive autonomously (drive them, verify their work, re-feed them when idle) until
the human returns. Operate from your tmux pane using bash + tmux + PowerShell.

## THE FLEET (tmux session 0, window 0)
| Pane | Lane | Role |
|---|---|---|
| %0 | claude (orig orchestrator) | paused/dead — ignore |
| %1 | quantum_dash.py | annealer dashboard |
| %2 | pipeline_dash.py | pipeline dashboard |
| %3 | **decomp Codex** (gpt-5.5) | src/game decomp: gs_render/gs_npc_interact/colosseum_battle |
| %4 | **GLM-5.1** (opencode "Sisyphus") | src/game decomp: gs_worldmap/gs_pokemon_summary/gs_npc_event |
| %5 | research_log_view.py | research log dashboard |
| %6 | **DeepSeek** worker (queue_attack.py loop) | cloud queue cracker (NOT winning — see TODO) |
| %7 | activity-matrix log (was dead qwen-3090) | live RE-agent activity tail |
| %8 | **PC-port Codex** (gpt-5.5) | animation/battle work in pcport/ |
| (you) | orchestrator Codex | drives all the above |

## HOW TO DRIVE AN AGENT (tmux)
- Capture a pane: `tmux capture-pane -t %8 -p | tail -20`
- Is it idle? Working agents show `esc to interrupt`. If that's ABSENT for ~40s+, it's idle → re-steer.
- Send a message to a Codex/GLM pane (CLEAR composer first, double-Enter for long msgs):
  `tmux send-keys -t %8 C-u; sleep 1; tmux send-keys -t %8 -l "YOUR MESSAGE"; sleep 1; tmux send-keys -t %8 Enter; sleep 2; tmux send-keys -t %8 Enter`
  Then verify it submitted: capture and confirm `Working` appears. Long pastes need the 2nd Enter.
- NEVER leave %3/%4/%6/%8 idle — re-dispatch the moment they finish (give the next function/task).

## ⚠ CRITICAL RULE: run Windows .exe via NATIVE POWERSHELL, not WSL
Post-crash, WSL→Windows interop HANGS when launching freshly-built exes (even hello-world).
Native PowerShell `Start-Process` works. To run a probe/game exe:
1. Edit `build_pc/_runbatch.ps1` (batch) or `_runprobe.ps1` (single) — set `$exe` to the newest
   `build_pc/pcport_motion_probe_headless_*.exe`, set `$env:PCPORT_*` vars.
2. `powershell.exe -NoProfile -ExecutionPolicy Bypass -File C:\Users\douglaswhittingham\pkmn-colosseum\build_pc\_runbatch.ps1`
3. Read `build_pc/_batch_out.txt` (or `_probe_out.txt`).
Building with clang via WSL still works; only LAUNCHING new exes needs PowerShell.
Python for the toolchain: `$PYW=/mnt/c/Users/douglaswhittingham/AppData/Local/Programs/Python/Python312/python.exe` (NOT linux python3 — it can't launch the Windows clang).

## DASHBOARDS (WSL python3, in panes — read-only viewers)
quantum_dash.py (annealer), pipeline_dash.py (pipeline), research_log_view.py (research log).
Backends that feed them: `grind2.py` (annealer swarm, writes .omc/permuter_state.json) +
`research_daemon.py` (writes .omc/research_log.jsonl) + `anneal_supervisor.sh` (refill loop).

## cc-fleet / send-message
Vendor agents (GLM via opencode) are driven by tmux send-keys (above). There is also a
cc-fleet skill for spawning vendor teammates; the tmux_control library
(`tools/decomp_work/tmux_control/control.sh`) has `send-codex`, `capture`, `dashboard`, etc.
(its panes.env may be stale — prefer raw tmux send-keys to the %ids above).

## CURRENT STATE (2026-06-05)
- **Animation goal — nearly done.** All 149 character models (field_common 6 + people_archive
  154) confirmed animating headless (idle/walk/run by checksum). Pokémon: the pkx loader WORKS
  for single-member (Absol = 8 motions, animates=yes). The BATCH sweep is the last bug — it
  exits early (was dying after chara_small / at field_common because `isPkx` leaked into the
  non-pkx path). Fix in flight in %8: isPkx must be STRICTLY `pkx_*.fsys` filename; non-pkx
  archives use the original +0x08-count/no-wrapper path; pkx use +0x0c count + LZSS + HSD@+0x40.
  Pokémon confirm = `animates=yes` (their battle anims are one-shot, cyclic not required).
  Verify with the batch probe (above); expect 149 char rows + 527 pkx rows. See
  `.omc/pc_port_anim_status.md` for the full running log.
- **3090 is intentionally KILLED** (energy). Do NOT restart qwen-3090 (%7) or reload the 32B.
  Policy: 3090 only during peak-sun (~9am-3pm). The decomp workers that used it stay off.

## YOUR OPERATING LOOP (every few minutes)
1. Capture %3,%4,%6,%8 — re-dispatch any idle agent (give next function/task, "keep grinding").
2. If %8 (PC-port) rebuilt a probe exe, run the batch via PowerShell, paste results to %8.
3. Keep DeepSeek %6 fed: if "queue empty", requeue failed tasks:
   `python3 -c "import json;p='tools/decomp_work/coordination/tasks.json';d=json.load(open(p));n=0
   [d.__setitem__(i,(t.update(status='queued') or t)) for i,t in enumerate(d) if t.get('status')=='failed' and t.get('meta',{}).get('asm_lines',999)<=400];json.dump(d,open(p,'w'),indent=2)"`
   (or just flip failed<=400 to queued).
4. Commit checkpoints periodically; push to origin (github.com/dougchansan/pkmn-colosseum).
5. Update `.omc/pc_port_anim_status.md` as milestones land.

## OPEN TASKS (priority order)
1. **Finish the pkx batch sweep** → all 527 Pokémon confirm animates=yes. Then animation goal DONE.
2. **BATTLE COLOSSEUM** (the human's next big goal): full battle animations + battle text +
   player battles. Approach: research the online literature (GC double battles, Shadow Pokémon,
   turn/catch flow) + RE the game code (src/game/colosseum_battle.c, battle_logic.c,
   battle_waza moves, colosseum_event.c) + the env-gated-probe methodology (docs/RE_WORKFLOW.md).
   Wire battle flow/animations/text into pcport. Dispatch %8 (or a fresh agent) to scope+build.
3. **THE ANNEALER ISN'T WINNING** — permuter/wins/ are all `none (base)`, wins:[], done=NOWIN.
   The grind2 swarm verifies+swarms but produces no score-0 matches. TEST it with a KNOWN
   1-2-instruction near-miss (pick a function at ~99% in report.json whose only diff is a
   reg-alloc/scheduling nudge, seed it into .omc/permuter_queue.json, run grind2, confirm it
   reaches score 0 and saves wins/<fn>.c with a real `WIN <fn>` line). If it never wins,
   debug anneal_one.sh / the permuter invocation (is the compiler/scorer actually wired? is the
   objdiff score computed? is the base source a real-C near-miss vs an asm-wrapper?). Log real
   wins to .omc/research_log.jsonl (research_daemon emits SWARM events; add WIN events on score 0).
4. **DEEPSEEK FLASH GETS ZERO WINS** — review the prompt (tools/decomp_work/benchmark/
   attack_function.py build_per_fn_prompt + build_repair_prompt_with_diff) and retarget:
   (a) feed it SMALL near-miss functions (<=60 asm lines, already 80%+) not cold 400-line ones;
   (b) build an **iterative marginal-gains cracker**: for each function keep the best partial in a
   pool (.omc/permuter_pool/<fn>.c already exists for the annealer — reuse the pattern), each
   round give the model the current best + the exact remaining objdiff instruction diff and ask
   for a minimal edit, accept only if score improves, loop until 100% or N plateau rounds. This
   is brute-force-with-memory: compounding 1-instruction gains. Wire deepseek's queue_attack.py
   repair loop to persist + reload the best partial across runs (it currently restarts cold).
5. **Activity-matrix pane (%7)** — `bash tools/decomp_work/activity_matrix.sh` (created) tails
   all agent logs + git commits as a live colored "matrix" feed. Keep it running in %7.

## RESUME AFTER A CRASH
Run `bash tools/decomp_work/resume_fleet.sh` — it rebuilds all panes, dashboards, backends, and
re-dispatches the agents. Then read this doc + `.omc/pc_port_anim_status.md` to continue.

## DON'T
- Don't restart the 3090/qwen (energy). Don't run new exes via WSL/cmd (use PowerShell).
- Don't let `git add -A` sweep the junk untracked files into commits — `git add -- <specific paths>`.
- Don't count an asm-wrapper as a decomp win (real active-C 100% only).
- Don't lose the 149 confirmed character models when editing the batch probe.
