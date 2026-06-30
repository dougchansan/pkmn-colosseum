"""Subprocess helpers that keep Windows tool runs headless."""

import os
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
TOOLS_DIR = ROOT / "tools"
_HEADLESS_TOOL_NAMES = {
    "objdump",
    "objdump.exe",
    "llvm-objdump",
    "llvm-objdump.exe",
    "powerpc-eabi-objdump",
    "powerpc-eabi-objdump.exe",
    "ppc-objdump",
    "ppc-objdump.exe",
}


def _hidden_startupinfo():
    if os.name != "nt":
        return None
    startupinfo = subprocess.STARTUPINFO()
    startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startupinfo.wShowWindow = subprocess.SW_HIDE
    return startupinfo


def _headless_creationflags(existing=0):
    if os.name != "nt" or os.environ.get("PKMN_ALLOW_WINDOW_POPUPS") == "1":
        return existing or 0

    flags = existing or 0
    flags &= ~getattr(subprocess, "CREATE_NEW_CONSOLE", 0)
    flags |= getattr(subprocess, "CREATE_NO_WINDOW", 0)
    flags |= getattr(subprocess, "DETACHED_PROCESS", 0)
    return flags


def run(cmd, **kwargs):
    """Run subprocess.run with no console/window popups on Windows."""
    if os.name == "nt" and isinstance(cmd, (list, tuple)) and cmd:
        exe = Path(str(cmd[0])).name.lower()
        local = TOOLS_DIR / exe
        if exe in _HEADLESS_TOOL_NAMES and local.exists():
            cmd = [str(local), *cmd[1:]]

    if os.name == "nt":
        kwargs["creationflags"] = _headless_creationflags(
            kwargs.get("creationflags", 0)
        )
        kwargs.setdefault("startupinfo", _hidden_startupinfo())
        kwargs.setdefault("stdin", subprocess.DEVNULL)

    env = dict(os.environ)
    env.update(kwargs.pop("env", {}) or {})
    if os.name == "nt":
        path = env.get("PATH", "")
        tools = str(TOOLS_DIR)
        if not path.lower().startswith(tools.lower() + os.pathsep):
            env["PATH"] = tools + os.pathsep + path
    env.setdefault("PKMN_ALLOW_INTERACTIVE_OBJD", "0")
    env.setdefault("PKMN_ALLOW_WINDOW_POPUPS", "0")
    kwargs["env"] = env
    return subprocess.run(cmd, **kwargs)
