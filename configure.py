#!/usr/bin/env python3

###
# Pokémon Colosseum (GC6E01) — dtk-template build configuration.
#
# Generates build.ninja and objdiff.json from the project configuration via the
# canonical decomp-toolkit pipeline (tools/project.py). The build splits the DOL
# into relocatable objects with dtk, links them back (substituting matching C
# objects where declared), and verifies the result against config/.../build.sha1.
#
# Usage:
#   python configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "GC6E01",  # 0 — NTSC-U Rev 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (default: tools/mwcc_compiler)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (default: tools/dtk.exe)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (default: tools/objdiff-cli.exe)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.generate_map = args.map
config.non_matching = args.non_matching
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions (used as download fallbacks when the local path is absent).
config.binutils_tag = "2.42-2"
config.dtk_tag = "v1.8.3"
config.compilers_tag = "20251118"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Prefer the binaries already vendored in tools/ over downloading.
def _local(path: Path) -> Path:
    return path if path.exists() else None

# dtk, objdiff-cli and the Metrowerks compilers are all left unset so project.py
# downloads the pinned, PLATFORM-APPROPRIATE binaries (dtk_tag / objdiff_tag /
# compilers_tag) into build/ — the canonical dtk-template behavior, and what makes
# the Linux CI work from the same config as Windows. --dtk/--objdiff/--compilers
# override with a local copy.
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.compilers_path = args.compilers

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
# Match the proven byte-match link (tools/decomp_work/build_dol.sh): a bare
# `mwldeppc -o main.elf -lcf <ldscript> <objs>` with no extra flags. Adding
# -fp/-nodefaults perturbs the output away from the original DOL.
config.ldflags = []
if args.map:
    config.ldflags.append("-mapunused")

# The original DOL uses _db_stack_addr = _stack_addr + 0x8000 (dtk's generated
# ldscript defaults to 0x2000). project.py applies config.ldscript_template if set;
# otherwise the dtk-generated ldscript is patched post-split (see tools/project.py
# integration / docs). Tracked so the byte-match link stays correct.

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Base CodeWarrior flags, common to most GC games. Per-object overrides live in
# the libs/objects below. The byte-match linker is GC/1.2.5n (see ra/mwldeppc.exe).
config.linker_version = "GC/1.2.5n"

cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hard",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",
    "-i include",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]

if args.debug:
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Linked only when configured with --non-matching


def GameLib(lib_name: str, mw_version: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": mw_version,
        "cflags": cflags_base,
        "progress_category": "game",
        "objects": objects,
    }


# Undeclared units link the dtk-extracted object as-is, reproducing the original
# DOL. Matching source objects are declared here and substituted in.
config.warn_missing_config = False
config.warn_missing_source = False
config.libs = [
    GameLib(
        "Runtime.PPCEABI.H",
        "GC/1.2.5n",
        [
            Object(
                Matching,
                "__init_cpp_exceptions.cpp",
                source="crt_data/__init_cpp_exceptions.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "crt/sdata2_math.c",
                source="crt_data/sdata2_math.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/data/rodata_80267060.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266BD8.c",
                progress_category="game",
                postprocess={
                    "set_section_alignments": [".rodata=4"],
                    "metadata_from": "build/GC6E01/obj/game/data/rodata_80266BD8.o",
                },
            ),
            Object(
                Matching,
                "game/data/rodata_80266C7C.c",
                progress_category="game",
                postprocess={
                    "set_section_alignments": [".rodata=4"],
                    "metadata_from": "build/GC6E01/obj/game/data/rodata_80266C7C.o",
                },
            ),
            Object(
                Matching,
                "game/data/rodata_80267350.c",
                progress_category="game",
                postprocess={
                    "set_section_alignments": [".rodata=4"],
                    "metadata_from": "build/GC6E01/obj/game/data/rodata_80267350.o",
                },
            ),
            Object(
                Matching,
                "game/data/rodata_80268424.c",
                progress_category="game",
                postprocess={
                    "set_section_alignments": [".rodata=4"],
                    "metadata_from": "build/GC6E01/obj/game/data/rodata_80268424.o",
                },
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B6A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B7A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B8A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B9A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BAA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BBA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BCA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BDA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BEA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BFA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C0A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C1A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C2A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C3A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "crt/sdata2_math_8047C8A0.c",
                source="crt_data/sdata2_math_8047C8A0.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/gs_render_util_sdata2.c",
                source="game/gs_render_util_sdata2.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C9A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CAA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CBE0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CC98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_sdata2_8047CD98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CE98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CF98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D098.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_sdata2_8047D198.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_sdata2_8047D298.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D398.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D498.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D690.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_sdata2_8047D790.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D890.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047D990.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DA90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DB90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DC90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DD90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DE90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047DF90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047E090.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047E190.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_waza_sdata2_8047E290.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E390.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/colosseum_battle_sdata2.c",
                progress_category="game",
            ),
        ],
    ),
]

config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose

if args.mode == "configure":
    generate_build(config)
elif args.mode == "progress":
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
