# fleet_down.ps1 — stop the decomp fleet cleanly.
# Kills both loops (by command line), clears pidfiles, the tmux mutex, and any
# unsent dispatch requests so a later fleet_up starts from a clean slate.
# Does NOT touch the agent panes/TUIs or the tmux server — only the driver loops.
# Usage:  powershell -ExecutionPolicy Bypass -File tools\decomp_work\fleet_down.ps1
$repo = 'C:\Users\douglaswhittingham\pkmn-colosseum'
$killed = 0
Get-CimInstance Win32_Process | Where-Object {
  $_.Name -eq 'bash.exe' -and $_.CommandLine -match 'pane_io\.sh|fleet_driver\.sh'
} | ForEach-Object { try { Stop-Process -Id $_.ProcessId -Force; $script:killed++ } catch {} }
Remove-Item "$repo\build\.pane_io.pid","$repo\build\.fleet_driver.pid" -Force -EA SilentlyContinue
Remove-Item "$repo\build\.tmux.lockd" -Recurse -Force -EA SilentlyContinue
Remove-Item "$repo\build\dispatch\*.req" -Force -EA SilentlyContinue
Write-Host "[fleet_down] stopped $killed loop process(es); pidfiles/lock/pending-reqs cleared"
