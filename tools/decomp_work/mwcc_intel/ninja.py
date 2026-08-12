from __future__ import annotations

import re
import shlex
from dataclasses import asdict, dataclass
from pathlib import Path

from .common import IntelError, display_path


@dataclass(frozen=True)
class CompileUnit:
    output: str
    rule: str
    source: str
    mw_version: str
    cflags_raw: str
    cflags: list[str]
    basedir: str
    basefile: str
    working_directory: str
    compiler: str
    wrapper: str | None
    target_object: str
    candidate_object: str

    def to_dict(self) -> dict:
        return asdict(self)


def _normalize(value: str) -> str:
    return value.replace("\\", "/")


def _unwrap(text: str) -> str:
    return re.sub(r"\$\r?\n\s*", "", text)


def _legacy_parse(path: Path) -> dict:
    """Reuse the established workunit parser as the flag/version authority."""
    try:
        from tools.decomp_work.permuter.gen_workunits import parse_ninja

        return parse_ninja(path)
    except (ImportError, OSError):
        return {}


def parse_compile_units(ninja_path: Path, repo: Path) -> dict[str, CompileUnit]:
    if not ninja_path.is_file():
        raise IntelError(f"missing build graph: {display_path(ninja_path, repo)}")
    text = _unwrap(ninja_path.read_text(encoding="utf-8"))
    legacy = {_normalize(key): value for key, value in _legacy_parse(ninja_path).items()}
    units: dict[str, CompileUnit] = {}
    current: dict[str, str] | None = None

    def finish() -> None:
        nonlocal current
        if current is None:
            return
        output = _normalize(current["output"])
        old = legacy.get(output, {})
        version = _normalize(current.get("mw_version", old.get("mw_version", "")))
        flags_raw = current.get("cflags", old.get("cflags", ""))
        if not version or not flags_raw:
            current = None
            return
        try:
            flags = shlex.split(flags_raw, posix=True)
        except ValueError as exc:
            raise IntelError(f"invalid cflags for {output}: {exc}") from exc
        source = _normalize(current["source"])
        rule = current["rule"]
        wrapper = "build/tools/sjiswrap.exe" if "sjis" in rule else None
        compiler = f"build/compilers/{version}/mwcceppc.exe"
        target = output.replace("/src/", "/obj/", 1)
        unit = CompileUnit(
            output=output,
            rule=rule,
            source=source,
            mw_version=version,
            cflags_raw=flags_raw,
            cflags=flags,
            basedir=_normalize(current.get("basedir", str(Path(output).parent))),
            basefile=_normalize(current.get("basefile", str(Path(output).with_suffix("")))),
            working_directory=str(repo.resolve()),
            compiler=compiler,
            wrapper=wrapper,
            target_object=target,
            candidate_object=output,
        )
        units[output] = unit
        current = None

    for line in text.splitlines():
        match = re.match(r"^build (\S+): (mwcc\S*) (\S+)", line)
        if match:
            finish()
            current = {"output": match.group(1), "rule": match.group(2), "source": match.group(3)}
            continue
        if current is None:
            continue
        variable = re.match(r"^\s+(mw_version|cflags|basedir|basefile) = (.*)$", line)
        if variable:
            current[variable.group(1)] = variable.group(2).strip()
        elif line and not line[0].isspace():
            finish()
    finish()
    return units


def find_unit_for_symbol(report: dict, units: dict[str, CompileUnit], symbol: str) -> tuple[dict, dict, CompileUnit]:
    for report_unit in report.get("units", []):
        for function in report_unit.get("functions", []):
            if function.get("name") != symbol:
                continue
            source = report_unit.get("metadata", {}).get("source_path")
            if not source:
                break
            normalized_source = _normalize(source)
            unit_name = _normalize(report_unit.get("name", ""))
            if unit_name.startswith("main/"):
                expected = "build/GC6E01/src/" + unit_name.split("/", 1)[1] + ".o"
                if expected in units:
                    return report_unit, function, units[expected]
            for unit in units.values():
                if unit.source == normalized_source:
                    return report_unit, function, unit
            expected = "build/GC6E01/" + normalized_source.rsplit(".", 1)[0] + ".o"
            if expected in units:
                return report_unit, function, units[expected]
            raise IntelError(f"source owner for {symbol} is absent from build.ninja: {source}")
    raise IntelError(f"symbol is absent from the current report: {symbol}")
