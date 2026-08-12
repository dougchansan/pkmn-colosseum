from __future__ import annotations

import datetime as dt
from pathlib import Path
from typing import Any

from .common import IntelError, read_json, write_json


STATES = ("planned", "executed", "capture_complete", "analysis_complete", "failed")
TRANSITIONS = {
    None: {"planned"},
    "planned": {"executed", "failed"},
    "executed": {"capture_complete", "failed"},
    "capture_complete": {"analysis_complete", "failed"},
    "analysis_complete": set(),
    "failed": set(),
}


def transition(path: Path, state: str, **updates: Any) -> dict[str, Any]:
    if state not in STATES:
        raise IntelError(f"invalid capture state: {state}")
    manifest = read_json(path) if path.is_file() else {}
    current = manifest.get("state")
    if state != current and state not in TRANSITIONS.get(current, set()):
        raise IntelError(f"invalid capture state transition: {current!r} -> {state!r}")
    manifest.update(updates)
    manifest["state"] = state
    manifest["updated_at"] = dt.datetime.now(dt.timezone.utc).isoformat()
    history = manifest.setdefault("state_history", [])
    if not history or history[-1].get("state") != state:
        history.append({"state": state, "at": manifest["updated_at"]})
    write_json(path, manifest)
    return manifest
