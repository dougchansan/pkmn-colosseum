<#
  launch-decomp.ps1 - build the "decomp" tmux (psmux) cockpit:

     +-----------+-----------+-----------+
     | col1      |           | OPUS | SON  |
     | proxy     | OPUS      +------+------+
     | permuter  | ORCH      | C3cx | C4cx |
     | glm       |           |      |      |
     +-----------+-----------+-----------+

  Fleet (2026-06-24: C1/C2 retired; right grid is now OPUS+SONNET over 2 Codex lanes):
  - pane    proxy    - local router (tools/llm-proxy/proxy.js); only GLM routes through it.
  - C3-C4   lanes    - 2x Codex (gpt-5.5), reverted to Codex when limits reset.
  - pane    glm      - Claude Code via the proxy, pinned glm-5.2[1m] (isolated config + key).
  - sonnet           - Claude Code Sonnet (DIRECT; Max bypasses the proxy - see note); top-right of grid.
  - worker (OPUS)    - a 2nd Opus the orchestrator dispatches hard structural work to; top-left of grid.
  - orch.            - the orchestrator Opus; the pane you're dropped into on attach (middle, full height).

  NOTE (auth): Claude Max-subscription traffic ignores ANTHROPIC_BASE_URL and goes
  straight to claude.ai, so the Opus/Sonnet panes CANNOT be proxied and run DIRECT.
  Only the GLM pane is proxied (it uses an API key, not Max OAuth). Live usage on the
  dashboard is therefore: GLM via the proxy; Claude/Codex via tools/decomp_work/agent_limits.json.

  - All Claude/Codex panes run --dangerously-skip-permissions for full autonomy.
  - After build, writes tools/decomp_work/tmux_control/panes.env so control.sh
    resolves codex/codex2..4/glm/sonnet/worker to the right panes (claude = orchestrator = self).
  - Panes are cmd shells (the reliable psmux default on this machine).

  GLM key resolution order: $env:GLM_API_KEY  ->  tools/llm-proxy/.glm_key file.

  Usage:
    powershell -ExecutionPolicy Bypass -File launch-decomp.ps1
    powershell -ExecutionPolicy Bypass -File launch-decomp.ps1 -NoAttach
  (or just double-click launch-decomp.bat)
#>
param(
  [switch]$NoAttach,
  [switch]$DryRun,          # build the layout with harmless echo placeholders
  [int]$Port = 8788,        # proxy listen port (8787 is taken by openbb-workspace-mcp)
  [string]$Session = "decomp",
  [switch]$NoDashboard,     # skip auto-starting the renaming/symbolmap web dashboard
  [switch]$NoCadence        # skip auto-starting the report_cadence publish loop
)

$ErrorActionPreference = "Continue"

# --- paths ---
$repo  = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$tm    = Join-Path $env:USERPROFILE "bin\tmux.exe"
if (-not (Test-Path $tm)) { $tm = "tmux" }

# --- GLM key ---
# Resolve a key from $env or an existing .glm_key, then persist it to .glm_key
# (gitignored, written BOM-free). The proxy reads the file itself, so the secret
# never appears in the pane command line, scrollback, or process args.
$keyFile = Join-Path $PSScriptRoot ".glm_key"
$glmKey = $env:GLM_API_KEY
if (-not $glmKey -and (Test-Path $keyFile)) { $glmKey = (Get-Content $keyFile -Raw).Trim() }
if ($glmKey) {
  [System.IO.File]::WriteAllText($keyFile, $glmKey.Trim())   # UTF-8, no BOM
} else {
  Write-Host "WARNING: no GLM key found (set `$env:GLM_API_KEY or create tools\llm-proxy\.glm_key)." -ForegroundColor Yellow
  Write-Host "         The proxy will still start, but GLM requests will be rejected until a key is set." -ForegroundColor Yellow
}

# --- GLM isolated CLAUDE config dir ---
# The GLM pane runs Claude Code in API-key mode pointed at the proxy. If it shares
# the normal ~/.claude config it ALSO sees the claude.ai OAuth login, and Claude
# warns "Both claude.ai and ANTHROPIC_API_KEY set - auth may not work" then throws
# 400 Authentication Failed (the dual-auth conflict, which also triggers a retry
# storm that trips z.ai's rate limit). Fix: give GLM its OWN config dir with NO
# .credentials.json, so it has a distinct identity and uses only the proxy key.
$glmConfigDir = Join-Path $env:USERPROFILE ".claude-glm"
if (-not (Test-Path $glmConfigDir)) { New-Item -ItemType Directory -Force -Path $glmConfigDir | Out-Null }
$glmJson = Join-Path $glmConfigDir ".claude.json"
if (-not (Test-Path $glmJson)) {
  # Seed onboarding-complete + pre-approved proxy key so the pane boots non-interactively.
  $seed = '{"hasCompletedOnboarding":true,"lastOnboardingVersion":"2.1.179","numStartups":200,"theme":"dark","autoUpdates":false,"customApiKeyResponses":{"approved":["zai-proxy ","zai-proxy"],"rejected":[]},"bypassPermissionsModeAccepted":true,"projects":{}}'
  [System.IO.File]::WriteAllText($glmJson, $seed)
}
# Never let the claude.ai login leak into the isolated dir.
$glmCreds = Join-Path $glmConfigDir ".credentials.json"
if (Test-Path $glmCreds) { Remove-Item -Force $glmCreds }

# --- per-pane cmd command lines ---
# No secret is embedded: the proxy reads the key from .glm_key on its own.
$cdRepo = 'cd /d "' + $repo + '"'

$proxyCmd  = $cdRepo + ' && set "PORT=' + $Port + '" && node "tools\llm-proxy\proxy.js"'
# 2026-06-24: right grid = OPUS worker + SONNET (top row) over 2 Codex lanes C3/C4
# (bottom row). C1/C2 were retired. $laneCmd (opus worker lane) is kept for reference.
# codex.cmd is NOT on the cmd PATH in a fresh pane, so launch it by full npm-global path.
$laneCmd   = $cdRepo + ' && claude --model "opus[1m]" --dangerously-skip-permissions'
$codexCmd  = $cdRepo + ' && "%APPDATA%\npm\codex.cmd"'
$glmCmd    = $cdRepo + ' && set "CLAUDE_CONFIG_DIR=' + $glmConfigDir + '" && set "ANTHROPIC_BASE_URL=http://127.0.0.1:' + $Port + '" && set "ANTHROPIC_API_KEY=zai-proxy" && claude --model "glm-5.2[1m]" --dangerously-skip-permissions'
# Sonnet + Opus run DIRECT (Max OAuth ignores the proxy; routing them through it is
# pointless and risks the dual-auth conflict). Default ~/.claude config = Max login.
$sonnetCmd = $cdRepo + ' && claude --model "sonnet" --dangerously-skip-permissions'   # NOT sonnet[1m] - the 1m beta throws an API error on this account
$workerCmd = $cdRepo + ' && claude --model "opus[1m]" --dangerously-skip-permissions'   # worker Opus (dispatched)
$orchCmd   = $cdRepo + ' && claude --model "opus[1m]" --dangerously-skip-permissions'   # orchestrator Opus (you type here)
# Stage-C permuter: continuous annealer swarm in WSL (native mwcceppc). Auto-refills
# from the closest winnable near-misses; writes .omc/permuter_state.json -> dashboard
# quantum panel. Runs under WSL because mwcc needs the Linux toolchain via interop.
$repoWsl   = '/mnt/' + $repo.Substring(0,1).ToLower() + ($repo.Substring(2) -replace '\\','/')
$permCmd   = 'wsl.exe bash -lc "bash ' + $repoWsl + '/tools/decomp_work/permuter/anneal_supervisor.sh"'

if ($DryRun) {
  $proxyCmd  = 'echo [DRYRUN] pane0 = NODE PROXY'
  $laneCmd   = 'echo [DRYRUN] pane = OPUS LANE'
  $codexCmd  = 'echo [DRYRUN] pane = CODEX'
  $glmCmd    = 'echo [DRYRUN] pane5 = CLAUDE GLM glm-5.2[1m]'
  $sonnetCmd = 'echo [DRYRUN] pane6 = CLAUDE SONNET'
  $workerCmd = 'echo [DRYRUN] pane7 = CLAUDE OPUS worker'
  $orchCmd   = 'echo [DRYRUN] pane8 = CLAUDE OPUS orchestrator'
  $permCmd   = 'echo [DRYRUN] pane9 = STAGE-C PERMUTER (WSL annealer swarm)'
}

# Explicit positioned splits to match the cockpit diagram (NOT tiled). We capture
# each pane id at creation, so the registry is robust to psmux index shuffles.
# Send keys literally (-l) to a SPECIFIC pane id, then Enter.
function Send($target, $cmd) { & $tm send-keys -t $target -l $cmd; & $tm send-keys -t $target "Enter" }
function SplitH($target, $pct) { return ([string](& $tm split-window -h -p $pct -t $target -P -F '#{pane_id}' "cmd")).Trim() }
function SplitV($target, $pct) { return ([string](& $tm split-window -v -p $pct -t $target -P -F '#{pane_id}' "cmd")).Trim() }

# --- (re)build session ---
& $tm kill-session -t $Session 2>$null
& $tm new-session -d -s $Session "cmd"
Start-Sleep -Milliseconds 1300          # psmux server start race
& $tm set -t $Session pane-border-status top  2>$null
& $tm set -t $Session pane-border-format ' #{pane_title} ' 2>$null

# Initial pane becomes col1-top (GLM proxy) after we carve everything off it.
$P0 = ([string](& $tm list-panes -t $Session -F '#{pane_id}' | Select-Object -First 1)).Trim()

# Layout (matches the cockpit diagram):
#   col1:   GLM proxy (top) / Stage-C permuter (mid) / GLM agent (below)
#   middle: Opus orchestrator (full height)
#   right:  2x2 grid — OPUS worker | SONNET (top row), Codex C3 | C4 (bottom row)
$REST     = SplitH $P0 72      # P0 = left col (~28%), REST = right (~72%)
$RIGHT    = SplitH $REST 58    # REST = middle/orchestrator, RIGHT = 2x2 grid (~42%)
$ORCH     = $REST
$BELOW    = SplitV $P0 84      # P0 = GLM proxy (top ~16%), BELOW = rest of col1
$PROXY    = $P0
$GLMAGENT = SplitV $BELOW 55   # BELOW = permuter (top ~45%), GLMAGENT = GLM agent (bottom ~55%)
$PERMUTER = $BELOW
# Right 2x2: top row = OPUS worker (TL) + SONNET (TR); bottom row = Codex C3 (BL) + C4 (BR).
$BOTROW   = SplitV $RIGHT 50   # RIGHT = top row, BOTROW = bottom row
$SONNET   = SplitH $RIGHT 50   # RIGHT = OPUS worker (TL), SONNET = (TR)
$OPUS     = $RIGHT
$CODEX4   = SplitH $BOTROW 50  # BOTROW = C3 codex (BL), CODEX4 = C4 codex (BR)
$CODEX3   = $BOTROW
# C1/C2 lanes retired (2026-06-24): the old top row of opus worker lanes was replaced by
# the OPUS worker + SONNET agents; only C3/C4 remain as (Codex) lanes.
Start-Sleep -Milliseconds 400

# Launch each role in its captured pane.
Send $PROXY    $proxyCmd
Send $GLMAGENT $glmCmd
Send $OPUS     $workerCmd
Send $SONNET   $sonnetCmd
Send $ORCH     $orchCmd
Send $CODEX3   $codexCmd
Send $CODEX4   $codexCmd
Send $PERMUTER $permCmd

# --- registry from CAPTURED ids (robust to layout/index shuffles) ---
if (-not $DryRun) {
  $reg = Join-Path $repo 'tools\decomp_work\tmux_control\panes.env'
  $regBody = @(
    '# panes.env - decomp cockpit registry (written by launch-decomp.ps1 from captured pane ids).',
    '# claude=orchestrator Opus (self) | worker=Opus | sonnet=Sonnet | glm=GLM',
    '# Lane mix (2026-06-24): C3-C4 = Codex (gpt-5.5); C1-C2 retired (blanked so pane_io skips).',
    '# The right 2x2 top row is now the OPUS worker + SONNET agents, not lanes C1/C2.',
    ('CLAUDE_PANE="'  + $ORCH     + '"'),
    ('WORKER_PANE="'  + $OPUS     + '"'),
    ('SONNET_PANE="'  + $SONNET   + '"'),
    ('GLM_PANE="'     + $GLMAGENT + '"'),
    'CODEX_PANE=""',
    'CODEX2_PANE=""',
    ('CODEX3_PANE="'  + $CODEX3   + '"'),
    ('CODEX4_PANE="'  + $CODEX4   + '"'),
    'CODEX5_PANE=""',
    'CODEX6_PANE=""',
    'CODEX7_PANE=""',
    'CODEX8_PANE=""',
    ('PROXY_PANE="'   + $PROXY    + '"'),
    ('PERMUTER_PANE="' + $PERMUTER + '"')
  ) -join "`n"
  [System.IO.File]::WriteAllText($reg, $regBody + "`n")
  Write-Host "Wrote control registry: $reg" -ForegroundColor DarkGray

  # Seed the fleet lane list so the driver feeds OPUS + SONNET + GLM + the 2 Codex lanes
  # on boot. Without this, fleet_driver defaults to just "OPUS SON" and the rest sit idle.
  $lanesFile = Join-Path $repo 'build\fleet_lanes.txt'
  New-Item -ItemType Directory -Force -Path (Split-Path $lanesFile) | Out-Null
  [System.IO.File]::WriteAllText($lanesFile, "OPUS SON GLM C3 C4`n")
  Write-Host "Wrote fleet lanes: OPUS SON GLM C3 C4" -ForegroundColor DarkGray
}

# auto-start the decomp fleet driver once the agents have booted. fleet_up.ps1 brings up
# the wedge-proof tmux control (sole-owner pane_io + tmux-free driver); it only dispatches
# to panes that read as idle, so a ~75s delay lets the agent TUIs finish booting first.
if (-not $DryRun) {
  Start-Process powershell -WindowStyle Hidden -ArgumentList @(
    '-NoProfile','-ExecutionPolicy','Bypass','-Command',
    "Start-Sleep 75; & '$repo\tools\decomp_work\fleet_up.ps1'"
  )
  Write-Host "Scheduled fleet_up (decomp driver) to start in ~75s" -ForegroundColor Cyan
}

# Renaming/symbolmap web dashboard. open_renaming_dashboard.ps1 starts the python
# server hidden, health-checks it, and opens the browser. Port = proxy port + 1 (the
# proxy owns $Port) so it lands on 8789 without the collision-probe dance. Skip with
# -NoDashboard. Failure here is non-fatal — the cockpit is already up.
if (-not $DryRun -and -not $NoDashboard) {
  try {
    & (Join-Path $repo 'tools\symbolmap\open_renaming_dashboard.ps1') -Port ($Port + 1)
  } catch {
    Write-Host "WARNING: renaming dashboard failed to start: $_" -ForegroundColor Yellow
  }
}

# report_cadence: the hourly loop that recomputes the PUBLISHED report.json
# (compile_check --all + gen_decomp_report), syncs the README, and pushes — so decomp.dev
# and the README never freeze. fleet_up does NOT start this; without it the headline number
# stalls (it sat dead ~25h once while real matches kept landing). Launched via Git bash so
# python + git are on PATH (report_cadence shells out to both). Idempotent: kill any
# existing loop first so relaunches don't stack. Skip with -NoCadence.
if (-not $DryRun -and -not $NoCadence) {
  New-Item -ItemType Directory -Force -Path (Join-Path $repo 'build\logs') | Out-Null
  Get-CimInstance Win32_Process | Where-Object {
    $_.CommandLine -match 'report_cadence\.py'
  } | ForEach-Object { try { Stop-Process -Id $_.ProcessId -Force } catch {} }
  $bashExe = 'C:\Program Files\Git\bin\bash.exe'
  if (Test-Path $bashExe) {
    $cadArg = "-l -c `"cd '$repo' && exec python tools/decomp_work/report_cadence.py >> build/logs/report_cadence.log 2>&1`""
    Start-Process -FilePath $bashExe -WindowStyle Hidden -ArgumentList $cadArg
    Write-Host "Started report_cadence publish loop (hourly report.json/README + push)" -ForegroundColor Cyan
  } else {
    Write-Host "WARNING: git bash not found; report_cadence not started" -ForegroundColor Yellow
  }
}

# land the user in the orchestrator pane (captured id)
& $tm select-pane -t $ORCH 2>$null

Write-Host ""
Write-Host "decomp cockpit built - diagram layout (col1 glm-proxy/permuter/glm-agent | mid orchestrator | right 2x2: OPUS|SONNET / Codex C3|C4):" -ForegroundColor Green
& $tm list-panes -t $Session -F '  pane #{pane_index}: @#{pane_left},#{pane_top} #{pane_id} cmd=#{pane_current_command}'
Write-Host ""

if ($NoAttach) {
  Write-Host "Attach with:  tmux attach -t $Session" -ForegroundColor Cyan
} else {
  & $tm attach -t $Session
}
