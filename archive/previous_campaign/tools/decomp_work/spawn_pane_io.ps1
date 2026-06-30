# spawn_pane_io.ps1 — relaunch pane_io.sh fully detached (NOT a child of the caller).
# Used by the fleet_driver wedge-watchdog to recover a stuck pane_io, and usable by hand.
# Mirrors the launch in fleet_up.ps1 but touches ONLY pane_io (never the driver), so the
# watchdog can restart the capture loop without disturbing itself.
$ErrorActionPreference = 'SilentlyContinue'
# repo root = two levels up from this script (tools/decomp_work/ -> repo)
$repo = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$bash = 'C:\Program Files\Git\bin\bash.exe'
$arg  = "-l -c `"cd '$repo' && exec bash tools/decomp_work/pane_io.sh >> build/logs/pane_io.log 2>&1`""
Start-Process -FilePath $bash -WindowStyle Hidden -ArgumentList $arg
Write-Output "[spawn_pane_io] launched"
