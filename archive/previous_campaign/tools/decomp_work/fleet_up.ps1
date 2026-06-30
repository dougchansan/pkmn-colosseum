# fleet_up.ps1 — launch the decomp fleet as DETACHED standalone processes.
#
# 2026-06-20 rearchitecture. Two long-running loops, each in its own console
# (Start-Process, NOT a child of the Claude Bash tool / Monitor — that nested-child
# launch is exactly what wedged native-PE psmux per reference_psmux_windows_tmux.md):
#   1. pane_io.sh     — the SOLE tmux owner. Captures panes -> build/hb/*.state,
#                       drains build/dispatch/*.req -> send-keys. Every tmux call is
#                       SIGKILL-bounded + serialized (txk), so it can never hang/pile up.
#   2. fleet_driver.sh— tmux-FREE decision loop. Reads hb state, writes dispatch reqs,
#                       gates wins. Cannot wedge on tmux.
# Usage:  powershell -ExecutionPolicy Bypass -File tools\decomp_work\fleet_up.ps1
$ErrorActionPreference = 'Stop'
$repo = 'C:\Users\douglaswhittingham\pkmn-colosseum'
$bash = 'C:\Program Files\Git\bin\bash.exe'
if (-not (Test-Path $bash)) { throw "git bash not found at $bash" }
Set-Location $repo
New-Item -ItemType Directory -Force -Path "$repo\build\logs" | Out-Null

# --- idempotent: stop any existing loops first (match by command line) ---
Get-CimInstance Win32_Process | Where-Object {
  $_.Name -eq 'bash.exe' -and $_.CommandLine -match 'pane_io\.sh|fleet_driver\.sh'
} | ForEach-Object { try { Stop-Process -Id $_.ProcessId -Force } catch {} }
Remove-Item "$repo\build\.pane_io.pid","$repo\build\.fleet_driver.pid" -Force -EA SilentlyContinue
Remove-Item "$repo\build\.tmux.lockd" -Recurse -Force -EA SilentlyContinue
Start-Sleep 1

# --- 1. pane_io (sole tmux owner) ---
# NOTE: ArgumentList MUST be a single pre-quoted string. Passing an @('-lc', '...')
# array lets Start-Process space-split the command so bash never receives -c intact
# (silent no-op launch — the 2026-06-20 'empty log, process vanished' bug).
$argPio = "-l -c `"cd '$repo' && exec bash tools/decomp_work/pane_io.sh >> build/logs/pane_io.log 2>&1`""
Start-Process -FilePath $bash -WindowStyle Hidden -ArgumentList $argPio
Write-Host "[fleet_up] pane_io launched (sole tmux owner)"

# give pane_io a couple passes to seed build/hb/*.state before the driver trusts them
Start-Sleep 6

# --- 2. fleet_driver (tmux-free decision loop) ---
$argDrv = "-l -c `"cd '$repo' && exec bash tools/decomp_work/fleet_driver.sh >> build/logs/fleet_driver.log 2>&1`""
Start-Process -FilePath $bash -WindowStyle Hidden -ArgumentList $argDrv
Write-Host "[fleet_up] fleet_driver launched (tmux-free)"

Start-Sleep 2
$alive = (Get-CimInstance Win32_Process | Where-Object {
  $_.Name -eq 'bash.exe' -and $_.CommandLine -match 'pane_io\.sh|fleet_driver\.sh'
}).Count
Write-Host "[fleet_up] up: $alive/2 loops running. Logs: build/logs/{pane_io,fleet_driver}.log"
