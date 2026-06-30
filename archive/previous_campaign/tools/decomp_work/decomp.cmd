@echo off
REM decomp.cmd - WSL-free wrapper for decomp.sh.
REM
REM In a cmd.exe pane (the Codex/agent panes) bare `bash` resolves to
REM C:\Windows\System32\bash.exe -- the WSL launcher, which is frequently broken or
REM absent and blocks the whole band workflow. This wrapper pins Git Bash instead so
REM `tools\decomp_work\decomp.cmd band init <tag> <file>` always works without WSL.
REM
REM NOTE: the band workflow underneath is pure Windows Python. The most robust path
REM (no bash at all) is to call band.py directly, e.g.:
REM     python tools\decomp_work\band.py init  <tag> <src\file.c>
REM     python tools\decomp_work\band.py check <tag> [fn ...]
REM     python tools\decomp_work\band.py save  <tag> <fn> [fn ...]
setlocal
set "GITBASH=C:\Program Files\Git\usr\bin\bash.exe"
if not exist "%GITBASH%" set "GITBASH=C:\Program Files\Git\bin\bash.exe"
if not exist "%GITBASH%" (
  echo decomp.cmd: Git Bash not found. For the band workflow, call Python directly: 1>&2
  echo     python tools\decomp_work\band.py init^|check^|save ^<tag^> ... 1>&2
  exit /b 1
)
"%GITBASH%" "%~dp0decomp.sh" %*
exit /b %ERRORLEVEL%
