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
import json
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
    "--binutils",
    metavar="DIR",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
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
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
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

# dtk, objdiff-cli and the Metrowerks compilers are all left unset so project.py
# downloads the pinned, PLATFORM-APPROPRIATE binaries (dtk_tag / objdiff_tag /
# compilers_tag) into build/ — the canonical dtk-template behavior, and what makes
# the Linux CI work from the same config as Windows. --dtk/--objdiff/--compilers
# override with a local copy.
config.binutils_path = args.binutils
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.compilers_path = args.compilers
config.sjiswrap_path = args.sjiswrap

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.force_active_symbols["main"] = [
    # Preserve retail zero-initialized bytes after the EXI control block,
    # now that EXIGetID owns that block in its exact source unit.
    "lbl_803FB488",
    # game/data/rodata_80267EA8.c: the matching menu code addresses this
    # verified retail pool through its base plus byte offsets. Retain each
    # typed table after moving the pool into its own source unit.
    "lbl_80267EA8",
    "lbl_80267F68",
    "lbl_80267FE8",
    "lbl_80268070",
    "lbl_802680C0",
    "lbl_802680D8",
    "lbl_802680E4",
    "lbl_80268120",
    "lbl_80268178",
    "lbl_80268184",
    "lbl_802681B4",
    "lbl_80268234",
    "lbl_80268424",
    "lbl_8026847C",
    "lbl_8026849C",
    "lbl_802684AC",
    "lbl_802684C4",
    "lbl_802684D0",
    "lbl_80268540",
    "lbl_80268550",
    "lbl_80268560",
    "lbl_80268574",
    "lbl_8026858C",
    "lbl_8026860C",
    "lbl_8026864C",
    "lbl_80268674",
    "lbl_80268680",
    "lbl_802686D0",
    # game/data/{bss,sdata,sbss}_*.c: retail variables that nothing in
    # main.dol references; compiled from source they would be dead-stripped
    # and the rest of the section would move.
    "lbl_803FC3D0",
    "lbl_803FCE04",
    "lbl_8047899C",
    "lbl_80478A64",
    "lbl_8047A6FC",
    "lbl_8047A834",
    "lbl_8047A8E4",
    "lbl_8047A8EC",
    "lbl_8047A8F4",
    "lbl_8047A8FC",
    "lbl_8047A904",
    "lbl_8047B1A1",
    # Present in retail, but its caller is not yet linked from source.
    "fn_800093D0",
    # game/gba/gba_conv_candidate_80088F58.c: debug-tool callbacks with no
    # reference in main.dol; compiled from source they would be dead-stripped.
    "fn_80088F58",
    "fn_80088F74",
    "fn_80088F88",
    "dbgMenuGBAAddCoupon",
    "dbgToolBattleDebugSetAGBConnectionMode",
    # game/menu/menu_debug_exact_8005DBC4.c / 8005DC24.c: callbacks referenced
    # only from unlinked debug-menu data.
    "dbgMenuLogChangeDisp",
    "dbgMenuFieldCameraChangeDisp",
    # game/data/rodata_802798F0.c: retail padding before lbl_802799C8 is
    # otherwise dead-stripped from the standalone matching data unit.
    "gap_04_802799C4_rodata",
    # game/menu/menu_r56b_800714C8_suffix.c: fn_8007169C has no reference
    # in main.dol but is present in retail; compiled from source it would be
    # dead-stripped.
    "fn_8007169C",
    # game/menu/menu_candidate_80075390.c: fn_80075390 has no reference in
    # main.dol but is present in retail.
    "fn_80075390",
    # dolphin/sdk_candidate_8009ED70.c: OSLinkFixed has no reference in
    # main.dol but is present in retail.
    "OSLinkFixed",
    # game/dbgMenu_candidate_80133250.c / 801334A8.c: debug-menu callbacks
    # referenced only from unlinked data.
    "dbgMenuGSmemDispMap",
    "debugMenuShadowBorderDisp",
    # game/dbgMenu_r61_middle_8013327C.c: the same, for the GSmem check,
    # frame-rate and mail entries.
    "dbgMenuGSmemCheck",
    "dbgMenuFrameRate20",
    "dbgMenuFrameRate30",
    "dbgMenuSendAllMail",
    "dbgMenuSendMail",
    # game/dbgMenu_r61_middle_80133510.c: the same, for the no-draw area,
    # party member and memory-info entries.
    "debugMenuNodrawArea",
    "fn_8013356C",
    "debugMenuChangeMemInfo",
    # game/dbgMenu_r61_prefix_80132C6C.cpp: menu callbacks referenced only from
    # unlinked data; compiled from source they would be dead-stripped.
    "fn_80132F7C",
    "dbgMenuMovieTest",
    "dbgMenuColisionDisp",
    "fn_80133050",
    "fn_8013308C",
    "fn_801330C8",
    "dbgMenuGSmemOptimize",
    "debugMenuColorBarDisp",
    "fn_801334DC",
    "fn_80133630",
    "fn_80006724",
    "fn_8000677C",
    "fn_800067D4",
    "fn_8000682C",
    "sprintf",
    "vsprintf",
    "vprintf",
    "printf",
    # game/movie.c: the movie scene callbacks have no reference in main.dol
    # but are present in retail; compiled from source they would be
    # dead-stripped.
    "fn_80035DD4",
    "fn_80035E04",
    "movieOpeningStart",
    "fn_80035F34",
    "fn_80035F64",
    "staffrollStart",
    "fn_80036210",
    "fn_80036240",
    "movieStartAttract",
    "fn_800363B0",
    "fn_800363B4",
    "fn_800363B8",
    "fn_800363BC",
    "movieGsLogoWait",
    "movieGsLogoStart",
    "fn_800365B0",
    "movieTpcWait",
    "movieTpcStart",
    "fn_8003669C",
    "fn_800366A0",
    "fn_800366A4",
    # game/gs_range_8000D290.c: fn_8000D290 has no reference in main.dol but
    # is present in retail; compiled from source it would be dead-stripped.
    "fn_8000D290",
    # game/gs_range_800096B4.c: the ten functions after dbgMenuPokemonEdit have no
    # reference in main.dol but are present in retail; compiled from source
    # they would be dead-stripped.
    "fn_8000BA94",
    "fn_8000BAB8",
    "fn_8000BB00",
    "dbgMenuHeroPokemonAdd",
    "dbgMenuPokemonEditSlot6",
    "fn_8000BC58",
    "fn_8000BCC4",
    "fn_8000BD30",
    "fn_8000BD9C",
    "fn_8000BE08",
    # trk/targsupp.c: MetroTRK file-I/O trap stubs with no reference in
    # main.dol (only TRKAccessFile is called); present in retail.
    "TRKOpenFile",
    "TRKCloseFile",
    "TRKPositionFile",
]
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
# The retail DOL matches when linked with the project linker-script override and
# no extra mwldeppc flags.
config.ldflags = []
if args.map:
    config.ldflags.append("-mapunused")

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []
# Retail callable exports must survive even without REL consumers.
# Do not retain arbitrary source-only helpers or force unrelated data.
export_manifest = Path("config") / config.version / "dol_exports.txt"
config.reconfig_deps.append(export_manifest)
config.force_active_symbols["main"].extend(
    line.strip() for line in export_manifest.read_text(encoding="utf-8").splitlines()
    if line.strip() and not line.lstrip().startswith("#")
)

# Reconstructed data can be addressed through a pool base plus an offset.
# Preserve its independently checked storage even without a direct relocation.
source_data_exports = Path("config") / config.version / "source_data_exports.txt"
config.reconfig_deps.append(source_data_exports)
config.force_active_symbols["main"].extend(
    line.strip() for line in source_data_exports.read_text(encoding="utf-8").splitlines()
    if line.strip() and not line.lstrip().startswith("#")
)

# REL modules live inside FSYS archives (docs/REL_MODULES.md). dtk reads them
# as loose files from orig/<version>/files, so the split step depends on an
# extraction step: tools/fsys_extract.py unpacks the modules listed in
# config.yml from their FSYS archives. The extracted .rel files stay
# gitignored game data; see docs/REL_MODULES.md for required archives.
fsys_modules = {
    "common.fsys": ["common_rel", "mail"],
    "pocket_menu.fsys": ["pocket_menu"],
    "s1_out.fsys": ["S1_out"],
    "world_map.fsys": ["worldmap_menu_rel"],
    "S1_shop_1F.fsys": ["S1_shop_1F"],
    "S1_out_bf.fsys": ["S1_out_bf"],
    "pda_menu.fsys": ["pda_menu"],
    "M1_out.fsys": ["M1_out"],
    "M1_pc_1F.fsys": ["M1_pc_1F"],
    "M1_pc_B1.fsys": ["M1_pc_B1"],
    "M1_houseA_1F.fsys": ["M1_houseA_1F"],
    "M1_houseA_2F.fsys": ["M1_houseA_2F"],
    "M1_houseB_1F.fsys": ["M1_houseB_1F"],
    "M1_houseC_1F.fsys": ["M1_houseC_1F"],
    "M1_shop_1F.fsys": ["M1_shop_1F"],
    "M1_shop_2F.fsys": ["M1_shop_2F"],
    "M1_gym_1F.fsys": ["M1_gym_1F"],
    "M1_gym_B1.fsys": ["M1_gym_B1"],
    "M1_stadium_1F.fsys": ["M1_stadium_1F"],
    "M1_carde_1.fsys": ["M1_carde_1"],
    "M1_out_bf.fsys": ["M1_out_bf"],
    "M1_gym_1F_bf.fsys": ["M1_gym_1F_bf"],
    "M1_houseA_1F_bf.fsys": ["M1_houseA_1F_bf"],
    "M1_water_colo.fsys": ["M1_water_colo"],
}
config.custom_build_rules = [
    {
        "name": "fsys_extract",
        "command": "$python tools/fsys_extract.py extract $in $members -o $out_dir",
        "description": "FSYS $in -> $members",
    },
]
# All owners now use source. The vector template uses the normal GNU assembly
# path; its synthetic target marker remains comparison-only metadata.
config.fallback_link_overrides = {}
# Accept validated loose RELs as an alternative to retaining entire FSYS
# archives. An available archive stays a Ninja dependency and is authoritative.
from tools.fsys_inputs import extraction_steps

config.custom_build_steps = {
    "pre-split": extraction_steps(config.version, fsys_modules),
}

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

# The people TU's one flag set (src/game/people/people.c header).
PEOPLE_TU_CFLAGS = [
    "-inline auto,deferred",
    "-use_lmw_stmw on",
    "-sdata 8",
    "-sdata2 8",
    "-str reuse,readonly",
]

Matching = True                   # Object matches and should be linked
NonMatching = False               # Object does not match and should not be linked
Equivalent = config.non_matching  # Linked only when configured with --non-matching
DataCandidate = NonMatching       # Compared by objdiff, but not linked yet
CodeCandidate = NonMatching       # Compared by objdiff, but not linked yet


# REL modules were built with the SN Systems ProDG toolchain (GCC 2.95, SN's
# assembler and GNU-ld-based linker), not CodeWarrior: see
# docs/REL_MODULES.md. -G0 keeps small data out of the module.
config.gnu_ld_modules = [
    "common_rel",
    "mail",
    "pocket_menu",
    "S1_out",
    "worldmap_menu_rel",
    "S1_shop_1F",
    "S1_out_bf",
    "pda_menu",
    "M1_out",
    "M1_pc_1F",
    "M1_pc_B1",
    "M1_houseA_1F",
    "M1_houseA_2F",
    "M1_houseB_1F",
    "M1_houseC_1F",
    "M1_shop_1F",
    "M1_shop_2F",
    "M1_gym_1F",
    "M1_gym_B1",
    "M1_stadium_1F",
    "M1_carde_1",
    "M1_out_bf",
    "M1_gym_1F_bf",
    "M1_houseA_1F_bf",
    "M1_water_colo",
]
cflags_rel = [
    "-O0",
    "-G0",
    "-I include",
    f"-I build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
]


# menuCB_Hook.c's .rodata pool (retail lbl_80267EA8). The units whose code
# addresses it off one base define it as static tables and redirect it here.
MENU_CB_HOOK_POOL_IMPORT = {
    "symbol": "lbl_80267EA8",
    "section": ".rodata",
    "offset": 0,
    "expected": (
        "0126012c019d01a301310137013c014201520159016c01730147014d015f01660179018001ca01ce00003c1400003c1500003c1700003c1900003c1b00003c1d"
        "00003c2400003c2500003c26000041fc00003c2700003bf900003be600003be700003bf900003dc200003dc300003dc400003dc500003bf900003beb00003bf0"
        "00003bf300003bf500003bf700003bf800003c6100003c6100003c6100003c2d00003c2f00003bf900003d4d00003d4e00003d4f000043f9000043fa00003bf9"
        "080a000000000000089b000000000000093e0000000000000937000000000001093800000000000109390000000000010ef60000000000010ef7000000000001"
        "0ef80000000000010ef900000000000108080000000000010809000000000001093a000000000002093b000000000002093c000000000002093d000000000002"
        "088b000000000003088c000000000004088d000000000005088e000000000006088f000000000007089000000000000308910000000000040892000000000005"
        "089300000000000608940000000000070fc00000000000030fc10000000000040fc20000000000050fc30000000000050fc40000000000060fc5000000000006"
        "0fc6000000000007089c000000003d960970000000003da3096f000000003da5096e000000003da7096d000000003da8096c000000003da9096a000000003dc0"
        "096b000000003d790963000000003dac0962000000003dad000002700000026f0000026e0000026d000002710000026d095b0fcc095a0959095800000fce0fcf"
        "0944089c0969094d0fcd094309700968094b094c0942096f09670949094a0941096e0966094709480940096d09650945094608a9096c09640957000000000006"
        "0fd2000000000006094e00000000000609630000000000060962000000000006095c000000000006095d000000000006095e000000000006095f000000000006"
        "0960000000000006096100000000000609600961095f095d095e095c097b000000000003097a0000000000030979000000000003097800000000000409770000"
        "0000000408a100000000000409b600000000000409b700000000000409b800000000000409c200000000000409c100000000000409c000000000000409bf0000"
        "0000000409be00000000000409bd00000000000409b200000000000609b300000000000609b400000000000609b500000000000609bb00000000000609b90000"
        "0000000209bc00000000000209c2000000003db609c1000000003db709c0000000003db809bf000000003db909be000000003dba09bd000000003dbb09bc0000"
        "00003db509bb000000003dc00ee80ee90eea0eeb0eec0eed0eee0eef0d81000000003d560d82000000003d570d83000000003d580d84000000003d590d850000"
        "00003d5a0d86000000003d5b0d87000000003d5c0d88000000003d600d9b000000003d630d9c0000000041f909d3000000003d630d9a000000003d6409d40000"
        "00003d6509d5000000003d6609d6000000003d6709d7000000003d6809d8000000003d6f09d9000000003d7009da000000003d6809db000000003d6f09dc0000"
        "00003d7109dd000000003d7209de000000003d7109df000000003d7209e0000000003d7509e1000000003d760d8d0d8e0d8f0d900d910d920d930d940d950e29"
        "0e2a0e2b0e2c0e2d0e2e0e2f0e300e3109c90000000000000d7f0000000000000ef10ef20ef30ef40ef500000a10000000003d5d0a11000000003d5e0a120000"
        "00003d5f0a13000000003d610a14000000003d620a16000000003d7b0a03000000003d730a00000000003d740a04000000003d750a01000000003d760a050000"
        "00003d750a02000000003d7611a0000000003d7711a1000000003d6509fe000000003d7809ff000000003d6509f60000000000020d7e0000000000020a1d0a33"
        "0a280e420a1e0a340a290e430a1f0a350a2a0e440a200a360a2b0e450a210a370a2c0e460a220a380a2d0e470a230a390a2e0e480a240a3a0a2f0e490a250a3b"
        "0a300e4a0a260a3c0a310e4b0a270a3d0a320a3e000001000001010000010100000001000001010101000001010101010000000108a309940995099609970998"
        "0999000000003db600003db700003db800003db900003dba00003dbb00003d2700003d2800003d2b0a420a470a4b0a4f0a530a570a5b0a5f0a630a670a6b0a6f"
        "0a730a770a440a480a4c0a500a540a580a5c0a600a640a680a6c0a700a740a780a450a490a4d0a510a550a590a5d0a610a650a690a6d0a710a750a790a460a4a"
        "0a4e0a520a560a5a0a5e0a620a660a6a0a6e0a720a760a4300000003000000000000000200000006004580000058260058000000585600000da40da50da60da7"
        "0da80da90daa0dab0dac0dad0eac00000ead00000eae01000eaf01000eb002000eb102000e560e620e6e0e680e570e630e6f0e690e580e640e700e6a0e590e65"
        "0e710e6b0e5a0e660e720e6c0e5b0e670e730e6d0f260f320f3e0f380f270f330f3f0f390f280f340f400f3a0f290f350f410f3b0f2a0f360f420f3c0f2b0f37"
        "0f430f3d0e4e0e4f0e500e510e520e530e540e550f1e0f1f0f200f210f220f230f240f250e860000000000040e870000000000070e880000000000040e890000"
        "000000040e8a0000000000070e8b0000000000070e8c0000000000040e8d00000000000408fb000000003c6208fc000000003d6908fd000000003d6a08fe0000"
        "00003d6b08ff000000003d6cffffffff213a44f211272bf26d656e7543425f486f6f6b2e6300000030203c3d207072756c652d3e6d5f6e426174746c6554696d"
        "654c696d6974000030203c3d207072756c652d3e6d5f6e436f6d6d616e6454696d654c696d69740049443a25316425316425316425316425316400005f4c454e"
        "475448285f43422e6d5f61547261696e65724f7267293e6a00"
    ),
}

def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "ProDG/3.5",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


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
# menu_middle's .rodata pool (0x80267EA8-0x80268708, menuCB_Hook.c's), owned by
# game/menu/menu_middle_exact_8006E798.c. Score-only menu_middle units that
# define the tables locally point their pool back at it (TEMPORARY
# constant_import until menu_middle is one unit again).
MENU_MIDDLE_RODATA_POOL = (
    "0126012c019d01a301310137013c014201520159016c01730147014d015f01660179018001ca01ce00003c1400003c15"
    "00003c1700003c1900003c1b00003c1d00003c2400003c2500003c26000041fc00003c2700003bf900003be600003be7"
    "00003bf900003dc200003dc300003dc400003dc500003bf900003beb00003bf000003bf300003bf500003bf700003bf8"
    "00003c6100003c6100003c6100003c2d00003c2f00003bf900003d4d00003d4e00003d4f000043f9000043fa00003bf9"
    "080a000000000000089b000000000000093e000000000000093700000000000109380000000000010939000000000001"
    "0ef60000000000010ef70000000000010ef80000000000010ef900000000000108080000000000010809000000000001"
    "093a000000000002093b000000000002093c000000000002093d000000000002088b000000000003088c000000000004"
    "088d000000000005088e000000000006088f000000000007089000000000000308910000000000040892000000000005"
    "089300000000000608940000000000070fc00000000000030fc10000000000040fc20000000000050fc3000000000005"
    "0fc40000000000060fc50000000000060fc6000000000007089c000000003d960970000000003da3096f000000003da5"
    "096e000000003da7096d000000003da8096c000000003da9096a000000003dc0096b000000003d790963000000003dac"
    "0962000000003dad000002700000026f0000026e0000026d000002710000026d095b0fcc095a0959095800000fce0fcf"
    "0944089c0969094d0fcd094309700968094b094c0942096f09670949094a0941096e0966094709480940096d09650945"
    "094608a9096c096409570000000000060fd2000000000006094e00000000000609630000000000060962000000000006"
    "095c000000000006095d000000000006095e000000000006095f00000000000609600000000000060961000000000006"
    "09600961095f095d095e095c097b000000000003097a0000000000030979000000000003097800000000000409770000"
    "0000000408a100000000000409b600000000000409b700000000000409b800000000000409c200000000000409c10000"
    "0000000409c000000000000409bf00000000000409be00000000000409bd00000000000409b200000000000609b30000"
    "0000000609b400000000000609b500000000000609bb00000000000609b900000000000209bc00000000000209c20000"
    "00003db609c1000000003db709c0000000003db809bf000000003db909be000000003dba09bd000000003dbb09bc0000"
    "00003db509bb000000003dc00ee80ee90eea0eeb0eec0eed0eee0eef0d81000000003d560d82000000003d570d830000"
    "00003d580d84000000003d590d85000000003d5a0d86000000003d5b0d87000000003d5c0d88000000003d600d9b0000"
    "00003d630d9c0000000041f909d3000000003d630d9a000000003d6409d4000000003d6509d5000000003d6609d60000"
    "00003d6709d7000000003d6809d8000000003d6f09d9000000003d7009da000000003d6809db000000003d6f09dc0000"
    "00003d7109dd000000003d7209de000000003d7109df000000003d7209e0000000003d7509e1000000003d760d8d0d8e"
    "0d8f0d900d910d920d930d940d950e290e2a0e2b0e2c0e2d0e2e0e2f0e300e3109c90000000000000d7f000000000000"
    "0ef10ef20ef30ef40ef500000a10000000003d5d0a11000000003d5e0a12000000003d5f0a13000000003d610a140000"
    "00003d620a16000000003d7b0a03000000003d730a00000000003d740a04000000003d750a01000000003d760a050000"
    "00003d750a02000000003d7611a0000000003d7711a1000000003d6509fe000000003d7809ff000000003d6509f60000"
    "000000020d7e0000000000020a1d0a330a280e420a1e0a340a290e430a1f0a350a2a0e440a200a360a2b0e450a210a37"
    "0a2c0e460a220a380a2d0e470a230a390a2e0e480a240a3a0a2f0e490a250a3b0a300e4a0a260a3c0a310e4b0a270a3d"
    "0a320a3e000001000001010000010100000001000001010101000001010101010000000108a309940995099609970998"
    "0999000000003db600003db700003db800003db900003dba00003dbb00003d2700003d2800003d2b0a420a470a4b0a4f"
    "0a530a570a5b0a5f0a630a670a6b0a6f0a730a770a440a480a4c0a500a540a580a5c0a600a640a680a6c0a700a740a78"
    "0a450a490a4d0a510a550a590a5d0a610a650a690a6d0a710a750a790a460a4a0a4e0a520a560a5a0a5e0a620a660a6a"
    "0a6e0a720a760a4300000003000000000000000200000006004580000058260058000000585600000da40da50da60da7"
    "0da80da90daa0dab0dac0dad0eac00000ead00000eae01000eaf01000eb002000eb102000e560e620e6e0e680e570e63"
    "0e6f0e690e580e640e700e6a0e590e650e710e6b0e5a0e660e720e6c0e5b0e670e730e6d0f260f320f3e0f380f270f33"
    "0f3f0f390f280f340f400f3a0f290f350f410f3b0f2a0f360f420f3c0f2b0f370f430f3d0e4e0e4f0e500e510e520e53"
    "0e540e550f1e0f1f0f200f210f220f230f240f250e860000000000040e870000000000070e880000000000040e890000"
    "000000040e8a0000000000070e8b0000000000070e8c0000000000040e8d00000000000408fb000000003c6208fc0000"
    "00003d6908fd000000003d6a08fe000000003d6b08ff000000003d6cffffffff213a44f211272bf26d656e7543425f48"
    "6f6f6b2e6300000030203c3d207072756c652d3e6d5f6e426174746c6554696d654c696d6974000030203c3d20707275"
    "6c652d3e6d5f6e436f6d6d616e6454696d654c696d69740049443a25316425316425316425316425316400005f4c454e"
    "475448285f43422e6d5f61547261696e65724f7267293e6a0000000000000000"
)

# Battle-script sources keep the original data-only owner identities. Their
# compiler derives widths and branch labels from the interpreted data language.
BATTLE_SCRIPT_ADDRESSES = (
    "80375E60", "80375F9C", "80376104", "80377088", "80377D90", "80378538",
    "80378724", "80378798", "80378968", "80378AA0", "80378B44", "80378D7C",
    "8037939C", "80379478", "803795D8", "80379808", "80379B0C",
)
battle_catalog_path = Path("src/game/battle_scripts/catalog.json")
battle_catalog = json.loads(battle_catalog_path.read_text(encoding="utf-8"))
config.reconfig_deps.append(battle_catalog_path)
battle_source_dependencies = [
    "tools/battle_script_format.py",
    "config/GC6E01/symbols.txt",
    str(battle_catalog_path),
    *[f"src/game/battle_scripts/scripts_{address}.battle.json"
      for address in BATTLE_SCRIPT_ADDRESSES],
    *sorted({entry["source"] for entry in battle_catalog["external_entries"]}),
]

config.libs = [
    GameLib(
        "Runtime.PPCEABI.H",
        "GC/1.2.5n",
        [
            *[
                Object(
                    Matching,
                    f"auto_05_{address}_data",
                    source=f"game/battle_scripts/scripts_{address}.battle.json",
                    source_generator={
                        "kind": "object", "tool": "tools/compile_battle_scripts.py",
                        "args": ["--symbols", "config/GC6E01/symbols.txt"],
                        "dependencies": battle_source_dependencies,
                        "assembler": True,
                    },
                    progress_category="game",
                )
                for address in BATTLE_SCRIPT_ADDRESSES
            ],
            # Typed task/menu/GBA defaults and small lookup tables.
            Object(
                Matching,
                "game/data/sdata_80478820.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                data_aliases="config/GC6E01/data_aliases/sdata_80478820.json",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_804788D8.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                progress_category="game",
            ),
            # Complete controller arrays retain their historical interior labels.
            Object(
                Matching,
                "game/data/sdata_80478950.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                data_aliases="config/GC6E01/data_aliases/sdata_80478950.json",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478958.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                progress_category="game",
            ),
            # Typed battle counts, status operands and purification sequence pairs.
            Object(
                Matching,
                "game/data/sdata_80478D08.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                progress_category="game",
            ),
            # The native eight-byte flag array keeps its full C object bounds.
            Object(
                Matching,
                "game/data/sdata_80478D78.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                data_aliases="config/GC6E01/data_aliases/sdata_80478D78.json",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478D80.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478D98.c",
                extra_cflags=["-sdata 8", "-sdata2 0"],
                progress_category="game",
            ),
            # Initialized zero storage with guarded original object extents.
            Object(
                Matching,
                "game/data/data_8038FF64.c",
                data_alignment=4,
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            # SDK source literals reconstructed from existing uses and build records.
            Object(
                Matching,
                "game/data/data_80311A24.c",
                data_alignment=4,
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/data/data_803124B8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/data/data_803125A0.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="sdk",
            ),
            # Source diagnostic names; private CI verifies their full section bytes.
            Object(
                Matching,
                "game/data/data_80315678.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B060.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B878.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B988.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BA10.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BA60.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            # Zero-init owners are candidates until symbol layouts and the full link pass.
            Object(
                Matching,
                "game/data/sbss2_8047E700.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_80478DC0.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A714.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A9A0.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803FED64.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047AA28.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047AA60.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047AB18.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss2_8047E718.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B050.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B318.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B398.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B3D8.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B5B8.c",
                extra_cflags=["-sdata 16", "-sdata2 8"],
                progress_category="game",
            ),
            # Preserve upstream source-backed data absent from the donor.
            Object(
                Matching,
                "game/data/data_802E4B78.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A6E8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478998.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803FB538.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803119B8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_804789B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A778.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_804789C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803FC360.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A7D8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311CB8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311FEC.c",
                extra_cflags=["-sdata 0"],
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311DB8.c",
                extra_cflags=["-sdata 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A8A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478A08.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803127A8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803FC840.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803FCDD8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047A9F8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478AC0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478AE0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_804001B0.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80400948.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_804011B8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478AF8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478B08.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B3F0.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B448.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B950.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047AF60.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047AFE8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478C08.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B1A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478C40.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B1F8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata_80478CA8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036DCA8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A690.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sbss_8047B678.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80478800.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A6C8.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803FB380.c",
                extra_cflags=["-sdata 0", "-sdata2 0"],
                progress_category="game",
            ),
            Object(Matching, "game/data/bss_803FB488.c", progress_category="game"),
            Object(Matching, "trk/ddh_cc_range_800C3E90.c", mw_version="GC/1.3", progress_category="runtime"),  # CALIB_TRK
            Object(
                Matching,
                "crt/stdio_range_800C7558.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # CALIB_CRT
            Object(
                Matching,
                "trk/ddh_cc_range_800C3C00.c",
                mw_version="GC/2.6",
                progress_category="runtime",
                # ddh_cc_read saves registers with stmw/lmw (was a source pragma).
                extra_cflags=["-rostr", "-use_lmw_stmw on"],
            ),  # BANK_TRK
            Object(
                Matching,
                "trk/gdev_cc_range_800C41AC.c",
                mw_version="GC/2.6",
                progress_category="runtime",
                # gdev_cc_read saves registers with stmw/lmw (was a source pragma).
                extra_cflags=["-rostr", "-use_lmw_stmw on"],
            ),  # BANK_TRK
            Object(Matching, "trk/gdev_cc_range_800C4444.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK
            # HAL sysdolphin mtx.c, the whole translation unit, built with the
            # library flags; it owns its .data (HSD_identityMtx), .bss (the
            # matrix/vector alloc data) and .sdata2 (strings and float pool).
            Object(
                Matching,
                "hsd/mtx.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/objalloc.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            Object(Matching, "hsd/hsd_obj_info_exact_801AA568.c", mw_version="GC/1.3", progress_category="hsd"),  # PR419 exact
            # HAL sysdolphin pobj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/pobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
  # BANK_HSD_POBJ
            # HAL sysdolphin quatlib.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .sdata2.
            Object(
                Matching,
                "hsd/quatlib.c",
                # CI-verified source symbol extents and relocation signatures.
                comparison_view="data_names",
                symbol_mappings={
                    'lbl_8047DD18': '@100',
                    'lbl_8047DD1C': '@101',
                    'lbl_8047DD20': '@243',
                    'lbl_8047DD28': '@244',
                    'lbl_8047DD30': '@245',
                    'lbl_8047DD38': '@246',
                },
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin robj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/robj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(Matching, "trk/TRKTarget_range_800C1310.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK2
            Object(
                Matching,
                "trk/TRKComm_range_800C3678.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKNub_exact_800BE47C.c",
                mw_version="GC/1.3",
                extra_cflags=["-rostr"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKNub_candidate_800BE6B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-sdata 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BE800.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKNub_candidate_800BE844.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BEBB0.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BEC18.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_exact_800BED14.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKNub_range_800BEE74.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKSerial_range_800BF088.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-rostr",
                    "-common off",
                    "-inline deferred",
                    "-char signed",
                    "-sdata 0",
                    "-sdata2 0",
                    "-sdatathreshold 0",
                ],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKBuffer.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatch_range_800BF53C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-str readonly", "-sdata 0", "-sdata2 0"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatch_r52_800C0504_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatch_r52_800C08C0_inline_noauto.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-inline noauto", "-rostr"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatch_range_800C0504_r41_800C0AA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-O4,s"],
                progress_category="runtime",
            ),
            *[
                Object(status, path, mw_version="GC/1.3", progress_category="runtime")
                for status, path in [
                    (Matching, "trk/TRKDispatch_exact_800C0CD8.c"),
                    (Matching, "trk/TRKDispatch_range_800C0CD8.c"),
                    (Matching, "trk/TRKDispatch_exact_800C0DA8.c"),
                    (Matching, "trk/TRKDispatch_candidate_800C0E60.c"),
                ]
            ],  # PR414_TRK_DISPATCH
            Object(
                Matching,
                "trk/TRKInterrupt.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTarget_range_800C1348.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKTarget_exact_800C1548.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKTarget_residual_800C17CC.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(Matching, "trk/TRKTarget_exact_800C195C.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKTarget_residual_800C1A08.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKTarget_residual_800C1A08_r41_800C1FB0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(Matching, "trk/TRKTarget_exact_800C24BC.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKTarget_residual_800C25FC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),  # BANK_TRK3
            # MetroTRK targsupp: four 8-byte trap stubs, 8-byte aligned in
            # retail (the zero word at 0x800C29EC is alignment padding).
            Object(
                Matching,
                "trk/targsupp.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-func_align 8"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKSaveState.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKInit.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-inline auto,deferred"],
                progress_category="runtime",
            ),  # PR414_TRK_INIT
            Object(
                Matching,
                "trk/TRKBoard_exact_800C33BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-rostr", "-sdata 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(Matching, "trk/TRKBoard_candidate_800C3414.c", mw_version="GC/1.3", progress_category="runtime"),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKBoard_exact_800C349C.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata2 0"],
                progress_category="runtime",
            ),  # BANK_TRK3
            Object(
                Matching,
                "trk/TRKBoard_range_800C3630.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata 0"],
                progress_category="runtime",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-char signed"]
                    + (
                        ["-use_lmw_stmw on"]
                        if path
                        in ["crt/printf.c", "crt/printf_residual_800C8864.c"]
                        else []
                    ),
                    progress_category="runtime",
                )
                for status, path in [
                    (Matching, "crt/printf.c"),
                    (Matching, "crt/printf_exact_800C87F8.c"),
                    (Matching, "crt/printf_residual_800C8864.c"),
                ]
            ],  # PR414_CRT_PRINTF
            Object(
                Matching,
                "__init_cpp_exceptions.cpp",
                source="crt_data/__init_cpp_exceptions.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/mem_range_800C811C.c",
                mw_version="GC/2.0",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/mem.c",
                mw_version="GC/2.0",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/string_range_800CA78C.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/string.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/wchar.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/wchar_range_800C7FB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/mwtrace_helpers.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/critical_regions.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/stdio_atexit.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A7CCC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDFs.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDQueue.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDError_ErrorCode2Num.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDError.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVDLow.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/exi/EXI2Stubs.c",
                progress_category="sdk",
            ),
            # The whole PAD TU (0x800AA4D4-0x800AC02C) with its .bss.
            Object(
                Matching,
                "dolphin/pad/PAD.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/exi/EXI2_range_800CEA3C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B71F0.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B770C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B7714.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800B856C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA198.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA414.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA424.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BA440.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BAE5C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BB2E4.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_fn_800BB2F8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/PPCArch.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSGetExceptionHandler.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSArena.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSInterruptHandlers.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSInterrupt.c",
                # Retail keeps the unfolded `b` chains of the SDK OS build,
                # so the TU was built with the peephole pass off (as OSTime).
                extra_cflags=["-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSEXI_fn_8009E7A8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSEXI_fn_8009E7AC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSState_fn_8009FAEC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                # Whole OSThread.c TU (0x800A128C-0x800A2778) with its .bss,
                # .sbss and .sdata.
                "dolphin/os/OSThread.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSTime_range_800A2778.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSTime.c",
                source="dolphin/os/calendar/OSTime.c",
                # OSTime.c: peephole off, GetDates not inlined (were source pragmas).
                extra_cflags=["-opt nopeephole", "-inline off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBInit.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBGetFirstCallback.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DB.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBIsExceptionMarked.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/db/DBPrintf.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SITypeDecode.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_fn_800D0F44.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_range_800D0F68.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/si/SI_fn_800D104C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/menu/menu_bag.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/sound/sound.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/gs_effect.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/sex.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # msgctrl.c emits only its head (0x80131588-0x80131690) unless a
            # candidate chunk defines MSGCTRL_WHOLE_TU. The TU was built with
            # the peephole pass off (see msgctrl_exact_80132A38.c).
            Object(
                Matching,
                "game/msgctrl.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_80131690.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_candidate_80131714.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_801317FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_r49_8013182C_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_r49_80131A34_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_80131BA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_candidate_80131BF8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_exact_80131F9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_candidate_80131FF4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/msgctrl_candidate_80132834.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            # msgctrl.c tail on the TU's flags: -O4,p with the peephole pass
            # off (see the source header for the evidence).
            Object(
                Matching,
                "game/msgctrl_exact_80132A38.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ]
                    + (
                        ["-schedule on"]
                        if path
                        in {
                            "game/dbgMenu_candidate_80133218.c",
                            "game/dbgMenu_candidate_80133250.c",
                            "game/dbgMenu_candidate_80133050.c",
                            "game/dbgMenu_candidate_8013308C.c",
                            "game/dbgMenu_candidate_801333AC.c",
                            "game/dbgMenu_candidate_80133450.c",
                            "game/dbgMenu_candidate_801334A8.c",
                            "game/dbgMenu_candidate_801334DC.c",
                            "game/dbgMenu_candidate_80133630.c",
                            "game/dbgMenu_candidate_801337E4.c",
                        }
                        else []
                    )
                    # dbgMenu keeps its string literals in .rodata.
                    + (
                        ["-str reuse,readonly"]
                        if path == "game/dbgMenu_r61_middle_8013327C.c"
                        else []
                    )
                    # The helpers follow _dbgMenuGetItemNo in retail and are
                    # still expanded into it: deferred inlining (which also
                    # emits the definitions in retail's order).
                    + (
                        ["-inline auto,deferred"]
                        if path == "game/dbgMenu_candidate_80133E6C.c"
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/dbgMenu_r61_prefix_80132C6C.cpp"),
                    (Matching, "game/dbgMenu_candidate_80133050.c"),
                    (Matching, "game/dbgMenu_candidate_8013308C.c"),
                    (Matching, "game/dbgMenu_r61_middle_801330C8.c"),
                    (Matching, "game/dbgMenu_candidate_80133218.c"),
                    (Matching, "game/dbgMenu_candidate_80133250.c"),
                    (Matching, "game/dbgMenu_r61_middle_8013327C.c"),
                    (Matching, "game/dbgMenu_candidate_801333AC.c"),
                    (Matching, "game/dbgMenu_candidate_80133450.c"),
                    (Matching, "game/dbgMenu_candidate_801334A8.c"),
                    (Matching, "game/dbgMenu_candidate_801334DC.c"),
                    (Matching, "game/dbgMenu_r61_middle_80133510.c"),
                    (Matching, "game/dbgMenu_candidate_80133630.c"),
                    (Matching, "game/dbgMenu_r61_suffix_80133664.c"),
                    (Matching, "game/dbgMenu_exact_801337A0.c"),
                    (Matching, "game/dbgMenu_exact_801337E4.c"),
                    (Matching, "game/dbgMenu_candidate_801337E4.c"),
                    (Matching, "game/dbgMenu_exact_80133BE4.c"),
                    (Matching, "game/dbgMenu_candidate_80133C3C.c"),
                    (Matching, "game/dbgMenu_exact_80133E1C.c"),
                    (Matching, "game/dbgMenu_candidate_80133E6C.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/pcbox_candidate_8013433C.c"),
                    (Matching, "game/pcbox_exact_801347D0.c"),
                    (Matching, "game/pcbox_exact_80135028.c"),
                ]
            ],
            Object(
                Matching,
                "game/pcbox_exact_801347E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pcbox_r54_80134E10_gc125_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pcbox_r54_80134EF0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedatasave_status.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedata.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedataBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gamedatasaveBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/status.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/koukaBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/kouka.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/tenkouBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/tikeiBios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/effect/effect_visual_r50_8013A1D4_suffix.c",
                constant_import=[
                    {"symbol": "lbl_8047D1B4", "offset": 0, "expected": "3e800000"},
                    {"symbol": "lbl_8047D190", "offset": 4, "expected": "00000000"},
                    {"symbol": "lbl_8047D1A8", "offset": 8, "expected": "3f800000"},
                    {"symbol": "lbl_8047D1AC", "offset": 12, "expected": "3f000000"},
                    {"symbol": "lbl_8047D1A0", "offset": 16, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/effect/effect_visual_r50_80139AC4_prefix.c",
                constant_import=[
                    {"symbol": "lbl_8047D198", "offset": 0, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047D1A0", "offset": 8, "expected": "4330000000000000"},
                    {"symbol": "lbl_8047D190", "offset": 16, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    f"game/effect/{name}.c",
                    mw_version="GC/1.3.2"
                    if name == "effect_visual_candidate_801386DC"
                    else "GC/1.3",
                    extra_cflags=(
                        [
                            "-O1",
                            "-use_lmw_stmw on",
                            "-sdata 8",
                            "-sdata2 8",
                        ]
                        if name == "effect_visual_candidate_8013E54C"
                        else ["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"]
                        if name == "effect_visual_candidate_801386DC"
                        else ["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    ),
                    progress_category="game",
                )
                for status, name in [
                    (Matching, "effect_visual_exact_801380D4"),
                    (Matching, "effect_visual_candidate_8013814C"),
                    (Matching, "effect_visual_exact_80138630"),
                    (Matching, "effect_visual_candidate_801386DC"),
                    (Matching, "effect_visual_exact_801387C0"),
                    (Matching, "effect_visual_exact_80138B00"),
                    (Matching, "effect_visual_r49_80138BBC_prefix"),
                    (Matching, "effect_visual_exact_80139820"),
                    (Matching, "effect_visual_exact_8013A42C"),
                    (Matching, "effect_visual_exact_8013AA8C"),
                    (Matching, "effect_visual_candidate_8013AB60"),
                    (Matching, "effect_visual_exact_8013B490"),
                    (Matching, "effect_visual_r49_8013B5E4_prefix"),
                    (Matching, "effect_visual_exact_8013C5A0"),
                    (Matching, "effect_visual_r51_8013C670_prefix"),
                    (Matching, "effect_visual_r51_8013CA48_suffix"),
                    (Matching, "effect_visual_exact_8013CE58"),
                    (Matching, "effect_visual_exact_8013D604"),
                    (Matching, "effect_visual_candidate_8013D984"),
                    (Matching, "effect_visual_exact_8013DB64"),
                    (Matching, "effect_visual_exact_8013E470"),
                    (Matching, "effect_visual_candidate_8013E54C"),
                    (Matching, "effect_visual_exact_8013E5AC"),
                    (Matching, "effect_visual_exact_8013F000"),
                    (Matching, "effect_visual_candidate_8013F344"),
                    (Matching, "effect_visual_exact_8013F410"),
                    (Matching, "effect_visual_candidate_8013F80C"),
                    (Matching, "effect_visual_exact_8013FBE0"),
                    (Matching, "effect_visual_candidate_8013FF0C"),
                    (Matching, "effect_visual_exact_80140138"),
                ]
            ],
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/effect/effect_visual_candidate_801402AC.c",
                constant_import=[
                    {"symbol": "lbl_8047D340", "offset": 0, "expected": "4059999a"},
                    {"symbol": "lbl_8047D348", "offset": 8, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_candidate_8013E8A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_r51_8013C718_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/effect/effect_visual_candidate_80138838.c",
                constant_import=[
                    {"symbol": "lbl_8047D15C", "offset": 0, "expected": "40c90fdb"},
                    {"symbol": "lbl_8047D154", "offset": 4, "expected": "00000000"},
                    {"symbol": "lbl_8047D14C", "offset": 8, "expected": "3f800000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_r51_8013D0A8_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/effect/effect_visual_r50_80139E80_o4s.c",
                constant_import=[
                    {"symbol": "lbl_8047D1A8", "offset": 0, "expected": "3f800000"},
                    {"symbol": "lbl_8047D190", "offset": 4, "expected": "00000000"},
                    {"symbol": "lbl_8047D1AC", "offset": 8, "expected": "3f000000"},
                    {"symbol": "lbl_8047D1B0", "offset": 12, "expected": "3a83126f"},
                    {"symbol": "lbl_8047D1A0", "offset": 16, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            # _leaffxMoveLeaf's 1.0f (the leaf's slerp-time decrement) is a literal;
            # retail shares it as lbl_8047D160 (effect_visual_sdata2_8047D160.c).
            Object(
                Matching,
                "game/effect/effect_visual_r49_80139074_suffix.c",
                constant_import=[
                    {"symbol": "lbl_8047D160", "offset": 0, "expected": "3f800000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            # _leaffxUpdate's 2.0f, 1.0f, 0.0f and int-to-float biases are
            # literals; retail shares them as lbl_8047D160..lbl_8047D178
            # (effect_visual_sdata2_8047D160.c).
            Object(
                Matching,
                "game/effect/effect_visual_r49_80138DE4_o2.c",
                constant_import=[
                    {"symbol": "lbl_8047D164", "offset": 0, "expected": "40000000"},
                    {"symbol": "lbl_8047D160", "offset": 4, "expected": "3f800000"},
                    {"symbol": "lbl_8047D168", "offset": 8, "expected": "00000000"},
                    {"symbol": "lbl_8047D170", "offset": 16, "expected": "4330000000000000"},
                    {"symbol": "lbl_8047D178", "offset": 24, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_candidate_8013C074_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/effect/effect_visual_candidate_8013A520_gc125n.c",
                constant_import=[
                    {"symbol": "lbl_8047D1B8", "offset": 0, "expected": "42800000"},
                    {"symbol": "lbl_8047D1BC", "offset": 4, "expected": "3f800000"},
                    {"symbol": "lbl_8047D1C0", "offset": 8, "expected": "437f0000"},
                    {"symbol": "lbl_8047D1C4", "offset": 12, "expected": "3dcccccd"},
                    {"symbol": "lbl_8047D1C8", "offset": 16, "expected": "3f000000"},
                    {"symbol": "lbl_8047D1CC", "offset": 20, "expected": "40c90fdb"},
                    {"symbol": "lbl_8047D1D0", "offset": 24, "expected": "40490fdb"},
                    {"symbol": "lbl_8047D1D8", "offset": 32, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_candidate_8013DE6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/item_range_80144574.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "musyx/runtime/seq.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/synth.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/seq_api.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/snd_synthapi.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            Object(
                Matching,
                "musyx/runtime/stream.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/synthdata.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/synthmacros.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-fp_contract off"]
                        if "_fp_contract_off" in path
                        or path == "musyx/runtime/snd_math.c"
                        or path == "musyx/runtime/snd3d.c"
                        else ["-inline noauto"]
                        if "_inline_noauto" in path
                        else []
                    ),
                    progress_category="musyx",
                )
                for status, path, version in [
                    (Matching, "musyx/runtime/synthvoice.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/synth_ac.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/synth_adsr.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/synth_vsamples.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/s_data.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/hw_dspctrl.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd3d.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd_init.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd_math.c", "GC/1.3.2"),
                    (Matching, "musyx/runtime/snd_midictrl.c", "GC/1.3.2"),
                ]
            ],
            # hw_volconv.c keeps every multiply and add separate (fmuls then
            # fadds, never fmadds) and salCalcVolume, its only function,
            # matches with -fp_contract off for the whole unit.
            Object(
                Matching,
                "musyx/runtime/hw_volconv.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/snd_service_exact_80162070.c"),
                ]
            ],
            Object(
                Matching,
                "musyx/runtime/hardware.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw off", "-sdata 8", "-sdata2 8"],
                    progress_category="musyx",
                )
                for status, path in [
                    (Matching, "musyx/runtime/hw_aramdma.c"),
                    (Matching, "musyx/runtime/hw_dolphin.c"),
                    (Matching, "musyx/musyx_candidate_801643D8.c"),
                    (Matching, "musyx/musyx_exact_80164488.c"),
                    (Matching, "musyx/musyx_r50_801644E0_prefix.c"),
                ]
            ],
            Object(
                Matching,
                "musyx/runtime/reverb_candidate_80164520.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw off",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                ],
                progress_category="musyx",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/people/people_data_exact_80140ACC.c"),
                    (Matching, "game/people/people_data_exact_80141308.c"),
                    (Matching, "game/people/people_data_exact_80142368.c"),
                    (Matching, "game/people/people_data_candidate_801425E8.c"),
                    (Matching, "game/people/people_field_lookup_exact_80142984.c"),
                    (Matching, "game/people/people_data_exact_80142A88.c"),
                    (Matching, "game/people/people_data_exact_80142B24.c"),
                    (Matching, "game/people/people_data_exact_80142CF4.c"),
                    (Matching, "game/people/people_data_r51_801431AC_suffix.c"),
                    (Matching, "game/people/people_item_friend_exact_8014369C.c"),
                    (Matching, "game/people/people_data_candidate_801436F0.c"),
                    (Matching, "game/people/people_item_effort_exact_80143718.c"),
                    (Matching, "game/people/people_data_candidate_80143778.c"),
                    (Matching, "game/people/people_item_ppup_exact_801437A0.c"),
                    (Matching, "game/people/people_data_candidate_801437B8.c"),
                    (Matching, "game/people/people_item_hpup_exact_801437E0.c"),
                    (Matching, "game/people/people_data_candidate_801437F8.c"),
                    (Matching, "game/people/people_item_battle_boost_exact_801439B8.c"),
                    (Matching, "game/people/people_data_candidate_80143A44.c"),
                    (Matching, "game/people/people_item_tables_setters_exact_80143A94.c"),
                ]
            ],
            Object(
                Matching,
                "game/people/people_data_r51_80142EF8_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_data_r49_80140588_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_data_r49_80140A9C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_item_getters_exact_80143C50.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_item_tail_exact_80144064.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/people/people_data_suffix_801441A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F7F80.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801F7F80.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F8424.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801F87CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801F87CC_r41_801F8A18.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F8D80.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_r53_801F9130_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_r53_801F93F8_gc20p1_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_r53_801F9600_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801F9790.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_r51_801F99C8_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_r51_801F9F78_gc20_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801FA4B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801FA524.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801FA634.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801FAA58.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_exact_801FB8F8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801FBAD4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_db_range_801FBD10.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_range_801FDB78.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                    "-DFIGHT_POKEMON_EXACT_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_candidate_801FE168_gc20.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_candidate_801FE3F8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_candidate_801FE3F8_r40_801FE91C_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                    # One-function unit: no IRO loop-invariant motion, so the
                    # backend hoists retail's sp+0x18 table base (see
                    # docs/RULE_EXCEPTIONS.md).
                    "-opt noloopinvariants",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    Matching
                    if path
                    in {
                        "game/fight_pokemon_r58_801FEC10_prefix.c",
                        "game/fight_pokemon_r58_801FED3C_o1.c",
                        "game/fight_pokemon_r58_801FEF74_middle.c",
                        "game/fight_pokemon_r58_801FF1BC_o1.c",
                        "game/fight_pokemon_r58_80200A5C_middle.c",
                        "game/fight_pokemon_r58_80200B10_o1.c",
                    }
                    else CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-O4,s",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                        *(
                            ["-schedule off"]
                            if path
                            in {
                                "game/fight_pokemon_r58_801FEF74_middle.c",
                            }
                            else []
                        ),
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/fight_pokemon_r58_801FEC10_prefix.c",
                    "game/fight_pokemon_r58_801FED3C_o1.c",
                    "game/fight_pokemon_r58_801FEF74_middle.c",
                    "game/fight_pokemon_r58_801FF1BC_o1.c",
                    "game/fight_pokemon_r58_80200A5C_middle.c",
                    "game/fight_pokemon_r58_80200B10_o1.c",
                ]
            ],
            Object(
                Matching,
                "game/fight_pokemon_candidate_80200E00_gc20.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_candidate_802010C8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_POKEMON_CANDIDATE_801FDB78_ONLY",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_pokemon_exact_80201248.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_target_exact_801F0058.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_target_801F0134.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_target_exact_801F0204.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fightActionFifoInit links as a data-free carve; the rest of the
            # range is scored from the whole-range candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_action_range_801F0898.c"),
                    (Matching, "game/fight_action_exact_801F108C.c"),
                    (Matching, "game/fight_action_range_candidate_801F1170.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_floor_exact_801F150C.c"),
                    (Matching, "game/fight_floor_candidate_801F1588.c"),
                    (Matching, "game/fight_floor_exact_801F1700.c"),
                    (Matching, "game/fight_floor_candidate_801F1990.c"),
                    (Matching, "game/fight_floor_exact_801F1A6C.c"),
                    (Matching, "game/fight_floor_candidate_801F1B14.c"),
                    (Matching, "game/fight_floor_exact_801F1F30.c"),
                    (Matching, "game/fight_floor_candidate_801F1F7C.c"),
                    (Matching, "game/fight_floor_exact_801F2020.c"),
                    (Matching, "game/fight_floor_candidate_801F2350.c"),
                    (Matching, "game/fight_floor_exact_801F2654.c"),
                    (Matching, "game/fight_floor_candidate_801F26A8.c"),
                    (Matching, "game/fight_floor_exact_801F27D4.c"),
                    (Matching, "game/fight_floor_candidate_801F2B5C.c"),
                    (Matching, "game/fight_floor_exact_801F32B0.c"),
                    (Matching, "game/fight_floor_candidate_801F32EC.c"),
                    (Matching, "game/fight_floor_exact_801F33E8.c"),
                    (Matching, "game/fight_floor_candidate_801F37B0.c"),
                    (Matching, "game/fight_floor_exact_801F3B24.c"),
                    (Matching, "game/fight_floor_candidate_801F3BB4.c"),
                    (Matching, "game/fight_floor_exact_801F4220.c"),
                    (Matching, "game/fight_floor_candidate_801F4354.c"),
                    (Matching, "game/fight_floor_exact_801F4460.c"),
                    (Matching, "game/fight_floor_exact_801F61BC.c"),
                    (Matching, "game/fight_floor_candidate_801F61EC.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_floor_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_side_exact_801F6B54.c"),
                    (Matching, "game/fight_side_candidate_801F6F38.c"),
                    (Matching, "game/fight_side_exact_801F7258.c"),
                    (Matching, "game/fight_side_candidate_801F72B0.c"),
                    (Matching, "game/fight_side_exact_801F7388.c"),
                    (Matching, "game/fight_side_exact_801F75F8.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_side_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_range_801F7954.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_candidate_80202810.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_8020355C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_candidate_802038A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_80203A6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_80206A04.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_suffix_80207C6C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_80207F5C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_exact_802096E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_out_pokemon_suffix_8020A8E0.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020AE30.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_action_exact_8020AED0.c"),
                    (Matching, "game/fight_action_exact_8020AF30.c"),
                    (Matching, "game/fight_action_exact_8020D784.c"),
                    (Matching, "game/fight_action_exact_8020D844.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_action_r51_8020B058_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020B058.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020B910.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_r51_8020CA98_o3.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020CFE0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_r51_8020CFE0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020D698.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_action_exact_8020D868.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_kouka_exact_8020D968.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_candidate_8020DA14.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-proc 603"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_candidate_8020DAD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020DD44.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020DF10.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020DF90.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020E020.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_encount_exact_8020E0B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_exact_8020E124.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_exact_8020E1A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_waza.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fightSeqGetNromalWazaDamage needs GC/1.3.2: GC/1.3 moves the
            # Lucky Punch / Stick `&&` values down into the crit sum, past
            # its calls. The other fight_seq functions match under both.
            Object(
                Matching,
                "game/fight_seq_candidate_80211170.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_seq_exact_802117FC.c"),
                    (Matching, "game/fight_seq_exact_802119D4.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_seq_r54_80211810_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_seq_r54_80211830_gc13_o4p.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_seq_r54_802118FC_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/mail_exact_801D1338.c"),
                    (Matching, "game/mail_exact_801D13E4.c"),
                    (Matching, "game/mail_candidate_801D142C.c"),
                    (Matching, "game/mail_exact_801D1470.c"),
                    (Matching, "game/mail_exact_801D167C.c"),
                    (Matching, "game/mail_exact_801D16F0.c"),
                    (Matching, "game/mail_candidate_801D1734.c"),
                    (Matching, "game/mail_exact_801D1A44.c"),
                    (Matching, "game/mail_exact_801D1B10.c"),
                    (Matching, "game/mail_exact_801D1E50.c"),
                    (Matching, "game/mail_candidate_801D1F0C.c"),
                ]
            ],
            # mailMain TU (Colosseum map: mailMainReceiveTerminate,
            # mailMainInit): .text 0x801D2080-0x801D2B4C with its .sdata2
            # pool 0x8047E1B0-0x8047E1D8 and chkMailSend's jump table
            # 0x8036E130-0x8036E14C.
            Object(
                Matching,
                "game/mailMain.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Waza camera TU: wazaSequenceCameraGetPattern (with its one pooled
            # 0.0f) and battleCameraWazaGetOwner/fn_801D2C74 link as carves; the rest of
            # wazaSequenceCamera.c, 0x801D2D28-0x801D5328, links whole with its
            # .sdata2 pool 0x8047E1E0-0x8047E2E0.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/wazaSequenceCamera_exact_801D2B4C.c"),
                    (Matching, "game/wazaSequenceCamera_exact_801D2C6C.c"),
                    (Matching, "game/wazaSequenceCamera_file_801D2D28.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    **(
                        {"constant_import": [{"symbol": "lbl_8047E328", "offset": 0, "expected": "4330000000000000"}]}
                        if path == "game/wazaViewer_exact_801D7464.c"
                        else {
                            # TEMPORARY constant_import: fn_801D7B94's float
                            # literals, pooled in source order; retail shares
                            # the named copies owned by the .sdata2 data unit.
                            "constant_import": [
                                {"symbol": "lbl_8047E318", "offset": 0x00, "expected": "3f000000"},
                                {"symbol": "lbl_8047E330", "offset": 0x04, "expected": "3f400000"},
                                {"symbol": "lbl_8047E30C", "offset": 0x08, "expected": "3f800000"},
                                {"symbol": "lbl_8047E334", "offset": 0x0C, "expected": "3faaa993"},
                                {"symbol": "lbl_8047E338", "offset": 0x10, "expected": "40000000"},
                                {"symbol": "lbl_8047E33C", "offset": 0x14, "expected": "40800000"},
                                {"symbol": "lbl_8047E340", "offset": 0x18, "expected": "43800000"},
                                {"symbol": "lbl_8047E344", "offset": 0x1C, "expected": "437f0000"},
                            ]
                        }
                        if path == "game/wazaViewer_candidate_801D7464.c"
                        else {}
                    ),
                    mw_version=(
                        "GC/1.3.2"
                        if path
                        in {
                            "game/wazaViewer_candidate_801D5464.c",
                            "game/wazaViewer_candidate_801D5A94.c",
                            "game/wazaViewer_candidate_801D56B0.c",
                            "game/wazaViewer_candidate_801D6A64.c",
                            "game/wazaViewer_exact_801D5DA0.c",
                            "game/wazaViewer_exact_801D56B0.c",
                            "game/wazaViewer_exact_801D624C.c",
                            "game/wazaViewer_exact_801D6E58.c",
                            "game/wazaViewer_exact_801D7464.c",
                        }
                        else "GC/1.3"
                    ),
                    # fn_801D603C and wazaViewerUpdatePlayMode keep retail's `li r4,0; mr r5,r4` key copy only below -O3.
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-O2,p"]
                        if path
                        in {
                            "game/wazaViewer_candidate_801D56B0.c",
                            "game/wazaViewer_candidate_801D6A64.c",
                        }
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/wazaViewer_candidate_801D5328.c"),
                    (Matching, "game/wazaViewer_exact_801D53D4.c"),
                    (Matching, "game/wazaViewer_exact_801D53D8.c"),
                    (Matching, "game/wazaViewer_candidate_801D5464.c"),
                    (Matching, "game/wazaViewer_exact_801D56B0.c"),
                    (Matching, "game/wazaViewer_candidate_801D5A94.c"),
                    (Matching, "game/wazaViewer_exact_801D5DA0.c"),
                    (Matching, "game/wazaViewer_candidate_801D56B0.c"),
                    (Matching, "game/wazaViewer_exact_801D624C.c"),
                    (Matching, "game/wazaViewer_candidate_801D6A64.c"),
                    (Matching, "game/wazaViewer_exact_801D6E58.c"),
                    (Matching, "game/wazaViewer_exact_801D744C.c"),
                    (Matching, "game/wazaViewer_exact_801D7464.c"),
                    (Matching, "game/wazaViewer_candidate_801D7464.c"),
                ]
            ],
            Object(
                Matching,
                "game/wazaSequenceEntry_r56_801D7E58_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceEntry_r57_801D81CC_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceEntry_candidate_801D87B0_gc125.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceEntry_particle_to_scale_801D8B38.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceEntry_candidate_801D9C1C_gc125.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceSys.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/1.3.2"
                        if path == "game/wazaSequenceSys_tail_candidate_801DB3F8.c"
                        else "GC/1.3"
                    ),
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(
                            ["-str reuse,readonly"]
                            if path == "game/wazaSequenceSys_tail_candidate_801DB3F8.c"
                            else []
                        ),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/wazaSequenceSys_r52_801DAC90_prefix.c"),
                    (Matching, "game/wazaSequenceSys_tail_exact_801DB060.c"),
                    (Matching, "game/wazaSequenceSys_tail_candidate_801DB288.c"),
                    (Matching, "game/wazaSequenceSys_tail_candidate_801DB3F8.c"),
                    (Matching, "game/wazaSequenceSys_tail_exact_801DB848.c"),
                ]
            ],
            Object(
                Matching,
                "game/wazaSequence_r52_801DC014_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nolifetimes"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequenceSys_reset.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/wazaSequence_candidate_801DB988.c"),
                    (Matching, "game/wazaSequence_exact_801DBB10.c"),
                    (Matching, "game/wazaSequence_candidate_801DBDDC.c"),
                    (Matching, "game/wazaSequence_exact_801DBFB0.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/sequence_exact_801DCDA8.c"),
                    (Matching, "game/sequence_exact_801DCF00.c"),
                    (Matching, "game/sequence_exact_801DCF84.c"),
                    (Matching, "game/sequence_candidate_801DD158.c"),
                    (Matching, "game/sequence_exact_801DD23C.c"),
                    (Matching, "game/sequence_candidate_801DD45C.c"),
                    (Matching, "game/sequence_exact_801DE164.c"),
                    (Matching, "game/sequence_r51_801DE190_prefix.c"),
                    (Matching, "game/sequence_r51_801DE598_suffix.c"),
                    (Matching, "game/sequence_exact_801DE654.c"),
                ]
            ],
            Object(
                Matching,
                "game/sequence_r51_801DE418_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/kaisuu.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gapp_exact_80101B34.c"),
                    # fn_80101B90, fn_80101D5C and fn_80101D8C share the
                    # TU's literal pool (.sdata2 0x8047CD80-0x8047CDC0).
                    (Matching, "game/gapp_exact_80101B90.c"),
                    (Matching, "game/gapp_exact_80101FB8.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu_get_last_error_exact_80102004.c"),
                ]
            ],
            # The retail menu TU (0x80102004-0x80103E68) was built as a whole
            # with GC/1.3.2, the peephole optimizer off and read-only string
            # literals (-rostr: the GSlogWrite format is in .rodata, the
            # __FUNCTION__ string in .data). With that single setting and no
            # local pragmas every function of src/game/menu.c matches except
            # menuCursorNormal (register assignment only) and
            # _menuUpdateKeyInfo. GC/1.3 cannot reproduce retail's scheduling
            # of menuInit and _menuUpdateKeyInfo (loads hoisted above the
            # port-state stores). Open conflict: under -inline auto GC/1.3.2
            # inlines _menuGetAgbKeyInfo into _menuUpdateKeyInfo, which retail
            # calls; -inline noauto fixes that function but loses the
            # menuCloseSync/menuCloseCustom/menuGetCursor expansions the
            # 0x80102014 range needs. The exact, text-only ranges are linked
            # as function-boundary carves; see src/game/menu.c.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-opt nopeephole",
                        "-rostr",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu_r50_80102014_prefix.c"),
                    (Matching, "game/menu_r50_80102F38_o3.c"),
                    (Matching, "game/menu_exact_80103484.c"),
                    (Matching, "game/menu_exact_80103614.c"),
                    (Matching, "game/menu_exact_801038F8.c"),
                    (Matching, "game/menu_exact_80103BA8.c"),
                ]
            ],
            Object(
                Matching,
                "game/cursor_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Window TU (0x80103FE4 - 0x801058CC): -O4,p with the peephole
            # pass off, built with GC/2.5 like the winMsg TU after it; see
            # window_exact_80104318.c for the evidence.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/window_exact_80103FE4.c"),
                    (Matching, "game/window.c"),
                    (Matching, "game/window_exact_801040A0.c"),
                    (Matching, "game/window_exact_801040F0.c"),
                    (Matching, "game/window_exact_80104160.c"),
                    (Matching, "game/window_exact_80104318.c"),
                    (Matching, "game/window_candidate_80104530.c"),
                    (Matching, "game/window_exact_801045A8.c"),
                    (Matching, "game/window_exact_801046B8.c"),
                    (Matching, "game/window_candidate_801046C8.c"),
                    (Matching, "game/window_exact_80104704.c"),
                    (Matching, "game/window_exact_8010474C.c"),
                    (Matching, "game/window_candidate_80104828.c"),
                    (Matching, "game/window_r50_80104A94_o2.c"),
                    # windowOpen reads the window TU's own 1.0f pool literal (0x8047CDEC,
                    # shared with windowDrawSprite2) through an extern stand-in.
                    # RULE-EXCEPTION(title-path): linked anyway, see docs/RULE_EXCEPTIONS.md.
                    (Matching, "game/window_r50_80104CA0_suffix.c"),
                    (Matching, "game/window_exact_80105410.c"),
                    (Matching, "game/window_exact_801054B8.c"),
                    (Matching, "game/window_exact_80105624.c"),
                    (Matching, "game/window_candidate_80105634.c"),
                ]
            ],
            # winMsg TU (0x801058CC - 0x80106F98): -O4,p with the peephole
            # pass off, built with GC/2.5; see win_msg_exact_80105A3C.c for
            # the evidence. winMsgDraw (win_msg.c) is text only and exact.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/win_msg.c"),
                    (Matching, "game/win_msg_exact_80105A3C.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/win_sequence.c"),
                    (Matching, "game/win_sequence_exact_801070F4.c"),
                ]
            ],
            # winSeq tail + winSprite TU (XD's winSprite.cpp) from
            # fn_80107170 to the menuOffScreen TU, with its literal pool,
            # jump table and .bss work area; GC/1.3.2, peephole off like the
            # winMsg TU, and -inline auto,deferred (functions are defined in
            # reverse address order). See the source header.
            Object(
                Matching,
                "game/win_sprite.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-inline auto,deferred",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu_offscreen.c",
                mw_version="GC/1.3",
                # All 12 functions match with the peephole pass off for the
                # whole TU (retail keeps the unfolded lbz/clrlwi and the
                # loop-entry branches); with it on, three of them do not.
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (["-opt nopeephole,nopropagation"] if path == "game/menu_model_render.c" else [])
                    + (["-opt nopeephole"] if path in ("game/menu_model_exact_setMotion.c", "game/menu_model_exact_check.c") else []),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu_model_exact_setMotion.c"),
                    (Matching, "game/menu_model_render.c"),
                    (Matching, "game/menu_model_exact_check.c"),
                    (Matching, "game/menu_model_exact_free.c"),
                ]
            ],
            Object(
                Matching,
                "game/floor_data.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # floorEventGetTresureList .. floorEventChangeTresure and
            # floorEventGetTresure link as data-free carves; the rest of the
            # TU is scored from the whole-TU candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/floor_event_exact_80115CB4.c"),
                    (Matching, "game/floor_event_exact_80115E6C.c"),
                    (Matching, "game/floor_event_candidate_80116164.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on", "-sdata 8", "-sdata2 8",
                        "-opt nopeephole,nopropagation"
                        if path == "game/floor_character_exact_80116D30.cpp"
                        else "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/floor_character_exact_80116D30.cpp"),
                    (Matching, "game/floor_character_exact_80116E6C.c"),
                    (Matching, "game/floor_character_exact_80116F68.c"),
                    (Matching, "game/floor_character_exact_80117038.c"),
                    (Matching, "game/floor_character_exact_80117070.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/field_camera_exact_8011711C.c"),
                    (Matching, "game/field_camera.c"),
                    (Matching, "game/field_camera_exact_801174C4.c"),
                    (Matching, "game/field_camera_exact_80117AD4.c"),
                    (Matching, "game/field_camera_exact_80117AE4.c"),
                ]
            ],
            # floorUpdateFieldCamera: linked carve on the field camera TU's
            # flags (-opt nopeephole, as game/field_camera.c).
            Object(
                Matching,
                "game/field_camera_r50_80117514_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # fn_801176C8 / fn_8011791C: the field camera TU's flags
            # (-opt nopeephole), plus -inline noauto so fieldCameraLoadList stays a
            # call as in retail. Each wrapper sets a FIELD_CAMERA_<addr>_ONLY
            # guard so it compiles just its own function.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-inline noauto",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/field_camera_r50_801176C8_o3.c"),
                    (Matching, "game/field_camera_r50_8011791C_suffix.c"),
                ]
            ],
            Object(
                Matching,
                "game/field_exact_80117E58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80118100.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_801183EC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80118874.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_801195AC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80119824.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80119BD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_80119D90.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_8011A0A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_8011A280.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_8011B2C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_8011B444.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/waza.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_data.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_8011F5FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_8011F77C.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80120B00.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_80121C18.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80122040.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_801226D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_801229F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_801237B8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80123C54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_exact_80123D58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80123E70.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_801240C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80124410.c",
                # RULE-EXCEPTION(title-path): GC/1.3.2, unlike the rest of the
                # pokemon TU (GC/1.3); see docs/RULE_EXCEPTIONS.md.
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_801248C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_set_status_exact_801254B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of pokemonGetStatus; owns its switch
            # table (jumptable_8035E4B0) like the pokemonSetStatus carve.
            Object(
                Matching,
                "game/pokemon_get_status_exact_8012640C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_8012795C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of pokemonEvolutionAll and
            # pokemonEvolutionCreateAddPokemon; owns their .rodata list
            # initialisers and the evolution code's only .sdata2 float.
            Object(
                Matching,
                "game/pokemon_evolution_exact_8012805C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_80128524.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_range_exact_80128CC0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_savedata_exact_80129280.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_bios_exact_8012A450.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # The whole hero_move TU with its .sdata2 literal pool. Deferred
            # inlining: functions are defined in reverse address order (see the
            # file header in src/game/hero_move.c).
            Object(
                Matching,
                "game/hero_move.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/hero_pokemon.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_8000D290.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80011EA4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_candidate_80012858.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80033278.c",
                constant_import=[
                    {"symbol": "lbl_8047BA08", "offset": 0, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047BA10", "offset": 8, "expected": "4330000000000000"},
                ],
                # Combined unit: .text 0x80033278-0x80035C48 and the Card-e .bss
                # 0x803A3278-0x803A3E58. The three switch tables are retail-owned by
                # gs_npc_event_candidate_800324A0 (.data +0x4C, +0x6C, +0xC4).
                jump_table_import={
                    "function": "fn_80034280",
                    "owner": "game/gs_npc_event_candidate_800324A0.c",
                    "tables": [
                        {"symbol": "jumptable_802E502C", "owner_offset": 0x4C,
                         "function": "_sysvarsProcessData__FP16sysvarsFuncEntryPc"},
                        {"symbol": "jumptable_802E504C", "owner_offset": 0x6C},
                        {"symbol": "jumptable_802E50A4", "owner_offset": 0xC4,
                         "function": "cardeBattleRun"},
                    ],
                },
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_exact_80035C48.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/movie.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-inline deferred",
                    "-rostr",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_range_80037158.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DPDA_RANGE_80037158_ONLY",
                    "-opt off",
                    "-O1",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80037174.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: these PDA callbacks share their 0.0f.
            Object(
                Matching,
                "game/menu/pda_exact_800376F8.c",
                constant_import=[
                    {"symbol": "lbl_8047BA58", "offset": 0, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_800379E8.c",
                constant_import=[
                    {"symbol": "lbl_8047BA60", "offset": 0, "expected": "40c90fdb"},
                    {"symbol": "lbl_8047BA58", "offset": 4, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_80037F40.c",
                constant_import=[
                    {"symbol": "lbl_8047BA60", "offset": 0, "expected": "40c90fdb"},
                    {"symbol": "lbl_8047BA58", "offset": 4, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80038124.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_80038170.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80038250.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_8003842C.cpp",
                # TEMPORARY constant_import: the merged driver/save unit
                # shares the original PDA conversion biases.
                constant_import=[{"symbol": "lbl_8047BA80", "offset": 0, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047BA88", "offset": 8, "expected": "4330000000000000"}],
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-pragma \"peephole off\""],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_80038E74.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80039004.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the slide literals and the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_candidate_80039128.c",
                constant_import=[
                    {"symbol": "lbl_8047BA90", "offset": 0, "expected": "41555555"},
                    {"symbol": "lbl_8047BA94", "offset": 4, "expected": "41055555"},
                    {"symbol": "lbl_8047BA98", "offset": 8, "expected": "437f0000"},
                    {"symbol": "lbl_8047BAA0", "offset": 16, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80039498.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80039644.c",
                mw_version="GC/1.3",
                extra_cflags=["-O2,p", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_800398D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80039A50.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_80039A50.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80039F44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: this isolated callback shares PDA's 0.0f.
            Object(
                Matching,
                "game/menu/pda_exact_80039F70.c",
                constant_import=[
                    {"symbol": "lbl_8047BAB0", "offset": 0, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_8003A10C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_range_r47_8003A520_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_range_r47_8003A6C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_range_r46_8003AFDC_o4s.c",
                mw_version="GC/2.0",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003B2D8.c",
                mw_version="GC/2.0",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003B478.c",
                mw_version="GC/2.0",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_candidate_8003B6D0_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003B814.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule off"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: the 0.0f compare literal and both int-to-float biases.
            Object(
                Matching,
                "game/menu/pda_exact_8003B85C.c",
                constant_import=[{"symbol": "lbl_8047BAC4", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BAE0", "offset": 8, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047BAE8", "offset": 16, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_8003BF54.c",
                constant_import=[
                    {"symbol": "lbl_8047BAE0", "offset": 0, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003C24C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: the row-layout float literals.
            Object(
                Matching,
                "game/menu/pda_exact_8003C2B8.c",
                constant_import=[
                    {"symbol": "lbl_8047BAC0", "offset": 0, "expected": "437f0000"},
                    {"symbol": "lbl_8047BAC8", "offset": 4, "expected": "3f800000"},
                    {"symbol": "lbl_8047BAF8", "offset": 8, "expected": "42860000"},
                    {"symbol": "lbl_8047BAFC", "offset": 12, "expected": "41000000"},
                    {"symbol": "lbl_8047BB00", "offset": 16, "expected": "40800000"},
                    {"symbol": "lbl_8047BAD8", "offset": 20, "expected": "41c00000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_8003C728.c",
                constant_import=[
                    {"symbol": "lbl_8047BAE0", "offset": 0, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_8003C7C0.c",
                constant_import=[
                    {"symbol": "lbl_8047BAE0", "offset": 0, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003CE1C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003CF38.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003D1FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003D4C8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003D818.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003D8CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003DC54.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003E394.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003F040.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8003F2DC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_8003F464.c",
                constant_import=[{"symbol": "lbl_8047BCA0", "offset": 0, "expected": "437f0000"},
                                 {"symbol": "lbl_8047BCA8", "offset": 4, "expected": "40000000"},
                                 {"symbol": "lbl_8047BCA4", "offset": 8, "expected": "40490fdb"},
                                 {"symbol": "lbl_8047BCAC", "offset": 12, "expected": "41e80000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 16, "expected": "4330000080000000"}],
                mw_version="GC/1.3.2",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80040018.c",
                constant_import=[{"symbol": "lbl_8047BCB8", "offset": 0, "expected": "3e800000"},
                                 {"symbol": "lbl_8047BCBC", "offset": 4, "expected": "3f800000"},
                                 {"symbol": "lbl_8047BC94", "offset": 8, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCC0", "offset": 12, "expected": "3f4ccccd"},
                                 {"symbol": "lbl_8047BCC4", "offset": 16, "expected": "41200000"},
                                 {"symbol": "lbl_8047BC98", "offset": 20, "expected": "41a00000"},
                                 {"symbol": "lbl_8047BCC8", "offset": 24, "expected": "c1a00000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80040308.c",
                constant_import=[{"symbol": "lbl_8047BCB8", "offset": 0, "expected": "3e800000"},
                                 {"symbol": "lbl_8047BCBC", "offset": 4, "expected": "3f800000"},
                                 {"symbol": "lbl_8047BC94", "offset": 8, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCC0", "offset": 12, "expected": "3f4ccccd"},
                                 {"symbol": "lbl_8047BCC4", "offset": 16, "expected": "41200000"},
                                 {"symbol": "lbl_8047BC98", "offset": 20, "expected": "41a00000"},
                                 {"symbol": "lbl_8047BCC8", "offset": 24, "expected": "c1a00000"},
                                 {"symbol": "lbl_8047BC9C", "offset": 28, "expected": "42200000"},
                                 {"symbol": "lbl_8047BCCC", "offset": 32, "expected": "43960000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 40, "expected": "4330000080000000"}],
                mw_version="GC/2.0",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80041114.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_800411FC.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80041B5C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80041E48.c",
                constant_import=[{"symbol": "lbl_8047BCBC", "offset": 0, "expected": "3f800000"},
                                 {"symbol": "lbl_8047BC94", "offset": 4, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCC4", "offset": 8, "expected": "41200000"},
                                 {"symbol": "lbl_8047BCD0", "offset": 12, "expected": "43730000"},
                                 {"symbol": "lbl_8047BCD4", "offset": 16, "expected": "40a00000"},
                                 {"symbol": "lbl_8047BCB8", "offset": 20, "expected": "3e800000"},
                                 {"symbol": "lbl_8047BCC0", "offset": 24, "expected": "3f4ccccd"},
                                 {"symbol": "lbl_8047BC98", "offset": 28, "expected": "41a00000"},
                                 {"symbol": "lbl_8047BCC8", "offset": 32, "expected": "c1a00000"},
                                 {"symbol": "lbl_8047BCAC", "offset": 36, "expected": "41e80000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 40, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047BCD8", "offset": 48, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80042658.c",
                constant_import=[{"symbol": "lbl_8047BCBC", "offset": 0, "expected": "3f800000"},
                                 {"symbol": "lbl_8047BC94", "offset": 4, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCC4", "offset": 8, "expected": "41200000"},
                                 {"symbol": "lbl_8047BCD0", "offset": 12, "expected": "43730000"},
                                 {"symbol": "lbl_8047BCD4", "offset": 16, "expected": "40a00000"},
                                 {"symbol": "lbl_8047BCB8", "offset": 20, "expected": "3e800000"},
                                 {"symbol": "lbl_8047BCC0", "offset": 24, "expected": "3f4ccccd"},
                                 {"symbol": "lbl_8047BC98", "offset": 28, "expected": "41a00000"},
                                 {"symbol": "lbl_8047BCC8", "offset": 32, "expected": "c1a00000"},
                                 {"symbol": "lbl_8047BCA4", "offset": 36, "expected": "40490fdb"},
                                 {"symbol": "lbl_8047BCE0", "offset": 40, "expected": "3c000000"},
                                 {"symbol": "lbl_8047BCE4", "offset": 44, "expected": "3fc90fdb"},
                                 {"symbol": "lbl_8047BCE8", "offset": 48, "expected": "bfc90fdb"},
                                 {"symbol": "lbl_8047BCEC", "offset": 52, "expected": "40400000"},
                                 {"symbol": "lbl_8047BC9C", "offset": 56, "expected": "42200000"},
                                 {"symbol": "lbl_8047BCA8", "offset": 60, "expected": "40000000"},
                                 {"symbol": "lbl_8047BCF0", "offset": 64, "expected": "3fc00000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 72, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047BCD8", "offset": 80, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80043728.c",
                constant_import=[{"symbol": "lbl_8047BCF4", "offset": 0, "expected": "41d80000"},
                                 {"symbol": "lbl_8047BCF8", "offset": 4, "expected": "43870000"},
                                 {"symbol": "lbl_8047BC94", "offset": 8, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 16, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_800439BC.c",
                constant_import=[{"symbol": "lbl_8047BC94", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCFC", "offset": 4, "expected": "3f75be0b"},
                                 {"symbol": "lbl_8047BD00", "offset": 8, "expected": "400ba058"},
                                 {"symbol": "lbl_8047BD04", "offset": 12, "expected": "3f1c61aa"},
                                 {"symbol": "lbl_8047BD08", "offset": 16, "expected": "4021f770"},
                                 {"symbol": "lbl_8047BCB0", "offset": 24, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_80043CD8.c",
                constant_import=[
                    {"symbol": "lbl_8047BCB0", "offset": 0, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80043DC0.c",
                constant_import=[{"symbol": "lbl_8047BCA4", "offset": 0, "expected": "40490fdb"},
                                 {"symbol": "lbl_8047BCA8", "offset": 4, "expected": "40000000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 8, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80043EC8.c",
                constant_import=[{"symbol": "lbl_8047BCB0", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_candidate_8003B814_r46_80043FA8.c",
                constant_import=[{"symbol": "lbl_8047BCA0", "offset": 0, "expected": "437f0000"},
                                 {"symbol": "lbl_8047BD10", "offset": 4, "expected": "43800000"},
                                 {"symbol": "lbl_8047BD14", "offset": 8, "expected": "43000000"},
                                 {"symbol": "lbl_8047BCA4", "offset": 12, "expected": "40490fdb"},
                                 {"symbol": "lbl_8047BCA8", "offset": 16, "expected": "40000000"},
                                 {"symbol": "lbl_8047BC94", "offset": 20, "expected": "00000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_80044378.c",
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail's 20.0f clamp and 0.0f compare literals.
            Object(
                Matching,
                "game/menu/pda_exact_80044630.c",
                constant_import=[{"symbol": "lbl_8047BC98", "offset": 0, "expected": "41a00000"},
                                 {"symbol": "lbl_8047BC94", "offset": 4, "expected": "00000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: the 0.0f compare literal and the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_80046168.c",
                constant_import=[{"symbol": "lbl_8047BC94", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 8, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_800472CC.c",
                constant_import=[{"symbol": "lbl_8047BCC4", "offset": 0, "expected": "41200000"},
                                 {"symbol": "lbl_8047BC94", "offset": 4, "expected": "00000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_candidate_8003B814_r46_800473E0.c",
                constant_import=[{"symbol": "lbl_8047BCBC", "offset": 0, "expected": "3f800000"},
                                 {"symbol": "lbl_8047BC94", "offset": 4, "expected": "00000000"},
                                 {"symbol": "lbl_8047BD20", "offset": 8, "expected": "43690000"},
                                 {"symbol": "lbl_8047BCA8", "offset": 12, "expected": "40000000"},
                                 {"symbol": "lbl_8047BD24", "offset": 16, "expected": "41c80000"},
                                 {"symbol": "lbl_8047BD28", "offset": 20, "expected": "42480000"},
                                 {"symbol": "lbl_8047BD2C", "offset": 24, "expected": "3e680000"},
                                 {"symbol": "lbl_8047BCD8", "offset": 32, "expected": "4330000000000000"},
                                 {"symbol": "lbl_8047BCB0", "offset": 40, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_800478B4.c",
                constant_import=[
                    {"symbol": "lbl_8047BCB0", "offset": 0, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_80047CC0.c",
                constant_import=[{"symbol": "lbl_8047BCB0", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_exact_800484A4.c",
                constant_import=[{"symbol": "lbl_8047BCB0", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/pda_candidate_8003B814_r46_800484A4.c",
                constant_import=[{"symbol": "lbl_8047BC94", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BCF0", "offset": 4, "expected": "3fc00000"},
                                 {"symbol": "lbl_8047BCBC", "offset": 8, "expected": "3f800000"},
                                 {"symbol": "lbl_8047BCCC", "offset": 12, "expected": "43960000"}],
                mw_version="GC/1.3",
                extra_cflags=["-O3", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_800492CC_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the mail-screen float literals and the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_800495C8.c",
                constant_import=[
                    {"symbol": "lbl_8047BDA8", "offset": 0, "expected": "3f000000"},
                    {"symbol": "lbl_8047BDA0", "offset": 4, "expected": "3f800000"},
                    {"symbol": "lbl_8047BDAC", "offset": 8, "expected": "00000000"},
                    {"symbol": "lbl_8047BDB0", "offset": 12, "expected": "437f0000"},
                    {"symbol": "lbl_8047BDB4", "offset": 16, "expected": "41400000"},
                    {"symbol": "lbl_8047BDB8", "offset": 20, "expected": "40800000"},
                    {"symbol": "lbl_8047BDA4", "offset": 24, "expected": "43f00000"},
                    {"symbol": "lbl_8047BDC0", "offset": 32, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3.2",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the mail-screen float literals and the int-to-float bias.
            Object(
                Matching,
                "game/menu/pda_exact_800499BC.c",
                constant_import=[
                    {"symbol": "lbl_8047BDB8", "offset": 0, "expected": "40800000"},
                    {"symbol": "lbl_8047BDC8", "offset": 4, "expected": "40490fdb"},
                    {"symbol": "lbl_8047BDCC", "offset": 8, "expected": "40000000"},
                    {"symbol": "lbl_8047BDC0", "offset": 16, "expected": "4330000080000000"},
                ],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8004A47C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the mail-screen float literals.
            Object(
                Matching,
                "game/menu/pda_exact_8004A7A8.c",
                constant_import=[
                    {"symbol": "lbl_8047BDB0", "offset": 0, "expected": "437f0000"},
                    {"symbol": "lbl_8047BDC8", "offset": 4, "expected": "40490fdb"},
                    {"symbol": "lbl_8047BDCC", "offset": 8, "expected": "40000000"},
                    {"symbol": "lbl_8047BDAC", "offset": 12, "expected": "00000000"},
                    {"symbol": "lbl_8047BDA0", "offset": 16, "expected": "3f800000"},
                    {"symbol": "lbl_8047BDA8", "offset": 20, "expected": "3f000000"},
                    {"symbol": "lbl_8047BDD0", "offset": 24, "expected": "43800000"},
                    {"symbol": "lbl_8047BDD4", "offset": 28, "expected": "40400000"},
                    {"symbol": "lbl_8047BDB8", "offset": 32, "expected": "40800000"},
                ],
                mw_version="GC/1.3.2",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: retail shares the mail-screen float literals and conversion biases.
            Object(
                Matching,
                "game/menu/pda_exact_8004B278.c",
                constant_import=[
                    {"symbol": "lbl_8047BDA8", "offset": 0, "expected": "3f000000"},
                    {"symbol": "lbl_8047BDA0", "offset": 4, "expected": "3f800000"},
                    {"symbol": "lbl_8047BDAC", "offset": 8, "expected": "00000000"},
                    {"symbol": "lbl_8047BDD8", "offset": 12, "expected": "41200000"},
                    {"symbol": "lbl_8047BDDC", "offset": 16, "expected": "41a00000"},
                    {"symbol": "lbl_8047BDE0", "offset": 20, "expected": "c1a00000"},
                    {"symbol": "lbl_8047BDE4", "offset": 24, "expected": "3b4ccccd"},
                    {"symbol": "lbl_8047BDC0", "offset": 32, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047BDE8", "offset": 40, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3.2",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/pda_exact_8004B598.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_r54b_8004B7EC_gc13_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_exact_8004BDB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O3,p", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_r54b_8004BDB8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_exact_8004C2D8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_candidate_8004C3E4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/menu_pda_mail_exact_8004C5B0.c"),
                    (Matching, "game/menu/menu_pda_mail_exact_8004C6C0.c"),
                    (Matching, "game/menu/menu_pda_mail_exact_8004CF78.c"),
                    (Matching, "game/menu/menu_pda_mail_candidate_8004D34C.c"),
                    (Matching, "game/menu/menu_pda_mail_candidate_8004DA64.c"),
                    (Matching, "game/menu/menu_pda_mail_exact_8004DB34.c"),
                    (Matching, "game/menu/menu_pda_mail_candidate_8004DDC0.c"),
                    (Matching, "game/menu/menu_pda_mail_exact_8004DF34.c"),
                    (Matching, "game/menu/menu_pda_mail_exact_8004DFCC.c"),
                ]
            ],
            Object(
                Matching,
                "game/menu/menu_pda_mail_candidate_8004E144.c",
                constant_import=[
                    {"symbol": "lbl_8047BE30", "offset": 0, "expected": "401921FB54442D18"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: the original mail TU shares this conversion bias.
            Object(
                Matching,
                "game/menu/menu_pda_mail_exact_8004E180.c",
                constant_import=[
                    {"symbol": "lbl_8047BE40", "offset": 0, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/menu_pda_mail_exact_8004E440.c"),
                    (Matching, "game/menu/menu_pda_mail_exact_8004E790.c"),
                    (Matching, "game/menu/menu_pda_mail_candidate_8004E9C0.c"),
                ]
            ],
            Object(
                Matching,
                "game/menu/menu_pda_mail_candidate_8004E510.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_exact_8004D590.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_exact_8004D6AC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_pda_mail_exact_8004D6F0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata_exact_8007FD64.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata.c",
                mw_version="GC/2.0",
                # TEMPORARY constant_import: fn_8007FDBC defines the
                # menuCardE_Matrix.c TU's .rodata pool (item-id tables, sprite
                # ids, assert file name) as its own objects, addressed off one
                # base like retail; the relocations go back to lbl_80268B88.
                constant_import=[
                    {"symbol": "lbl_80268B88", "section": ".rodata", "offset": 0, "expected": (
                        "07a007a107a207a3102e102f1030103110321033103410351036103710381039103a103b103c103d103e103f1040104110421043104410451046104710481049"
                        "104a104b104c104d112711281129112a112b112c112d112e112f1130113111321133113411351136113711381139113a113b113c113d113e113f114011411142"
                        "1143114411451146114711481149114a104e104f1050105110521053105410551056105710581059105a105b105c105d105e105f106010611062106310641065"
                        "1066106710681069106a106b106c106d106e106f10701071114b114c114d114e114f1150115111521153115411551156115711581159115a115b115c115d115e"
                        "115f1160116111621163116411651166116711681169116a116b116c116d116e10721073107410751076107710781079107a107b107c107d107e107f10801081"
                        "10821083108410851086108710881089108a108b108c108d108e108f109010911092109310941095116f1170117111721173117411751176117711781179117a"
                        "117b117c117d117e117f1180118111821183118411851186118711881189118a118b118c118d118e118f1190119111920000020a0000020c0000020e00000210"
                        "000002120000021400000216000002180000020b0000020d0000020f00000211000002130000021500000217000002196d656e7543617264455f4d6174726978"
                        "2e6300"
                    )},
                ],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # The cardedecode.c TU (0x80080310-0x80082650) with its .rodata
            # pool and cardeSaveApplyField's jump tables; GC/1.3.2 like the grid TU.
            Object(
                Matching,
                "game/menu/cardesavedata_decode_80080310.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-str reuse,readonly", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # The Card-e grid TU: fn_80082650 - fn_800836AC with its own
            # string pool (.rodata 0x8026F1C8, .sdata2 "series"/"lv").
            # Deferred inlining emits the functions in reverse source order
            # and lets cardeSaveRemoveCard/cardeSaveSetCard inline their later callees.
            Object(
                Matching,
                "game/menu/cardesavedata_grid_80082650.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline auto,deferred", "-str reuse,readonly", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # fn_80083CBC/fn_80083CFC link as a data-free carve on the TU's
            # flags; the rest of the suffix is scored from the candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/cardesavedata_r51_80083AF4_suffix.c"),
                    (Matching, "game/menu/cardesavedata_exact_80083CBC.c"),
                    (Matching, "game/menu/cardesavedata_exact_80084034.c"),
                ]
            ],
            Object(
                Matching,
                "game/menu/cardesavedata_r51_80083D30.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata_80084038.c",
                mw_version="GC/2.5",
                # TEMPORARY constant_import: fn_80084038 defines the
                # menuGBAC.c TU's .rodata pool (state sprite masks, item
                # messages, assert file name) as its own objects, addressed
                # off one base like retail; the relocations go back to
                # lbl_8026F2E8 (shared with cardeGbaTransfer's carve).
                constant_import=[
                    {"symbol": "lbl_8026F2E8", "section": ".rodata", "offset": 0, "expected": (
                        "00000000000000080000001b0000001f0000001f000000bb0000007b0000007b00000700000007000000170000000f0000000008000000010000000000000000"
                        "00000008000000010000000100000000000000080000000100000001000000010000000100000001000000010000000110d00126012c10d1013c014210d2013c"
                        "014210d30147014d10d40147014d10d5013c014210d6013c014210d7013c014210d80147014d10d90147014d10da013c014210db013c014210dc013c014210dd"
                        "0147014d10de0147014d10df013c014210e0013c014210e10147014d10e20147014d10e3013c014210e4013c014210e5013c014210e60147014d10e70147014d"
                        "10e8013c014210e9013c014210ea0147014d10eb0147014d10ec013c014210ed013c014210ee0147014d10ef0147014d10f0013c014210f1013c014210f2013c"
                        "014210f30147014d10f40147014d10f5013c014210f60147014d10f70147014d10f80147014d10f90147014d10fa0147014d10fb0147014d10fc0126012c10fd"
                        "0126012c6d656e75474241432e6300"
                    )},
                ],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata_exact_800849B4.c",
                mw_version="GC/2.5",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata_candidate_80084A8C_o3.c",
                # TEMPORARY constant_import and jump_table_import: the isolated
                # transfer routine shares Card-e's literal and switch pools.
                constant_import=[
                    {"symbol": "lbl_8026F2E8", "section": ".rodata", "offset": 0, "expected": (
                        "00000000000000080000001b0000001f0000001f000000bb0000007b0000007b00000700000007000000170000000f0000000008000000010000000000000000"
                        "00000008000000010000000100000000000000080000000100000001000000010000000100000001000000010000000110d00126012c10d1013c014210d2013c"
                        "014210d30147014d10d40147014d10d5013c014210d6013c014210d7013c014210d80147014d10d90147014d10da013c014210db013c014210dc013c014210dd"
                        "0147014d10de0147014d10df013c014210e0013c014210e10147014d10e20147014d10e3013c014210e4013c014210e5013c014210e60147014d10e70147014d"
                        "10e8013c014210e9013c014210ea0147014d10eb0147014d10ec013c014210ed013c014210ee0147014d10ef0147014d10f0013c014210f1013c014210f2013c"
                        "014210f30147014d10f40147014d10f5013c014210f60147014d10f70147014d10f80147014d10f90147014d10fa0147014d10fb0147014d10fc0126012c10fd"
                        "0126012c6d656e75474241432e630000636f6c62746c2e62696e000000000000"
                    )},
                    {"symbol": "lbl_8047C1A8", "offset": 0, "expected": "00000000"},
                    {"symbol": "lbl_8047C1AC", "offset": 4, "expected": "3f800000"},
                    {"symbol": "lbl_8047C1B0", "offset": 8, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047C1B8", "offset": 16, "expected": "4330000000000000"},
                ],
                jump_table_import={
                    "symbol": "jumptable_802EEB78",
                    "function": "cardeGbaTransfer",
                    "owner": "game/data/data_802EEB78.c",
                    "owner_offset": 0,
                },
                mw_version="GC/2.5",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/cardesavedata_exact_80087AE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import: the cardGridShowCount anims[4]
            # initialiser {0, 1, 2, 3} is retail's lbl_8047C1C0/lbl_8047C1C4.
            Object(
                Matching,
                "game/menu/cardesavedata_candidate_80087AE8.c",
                constant_import=[
                    {"symbol": "lbl_8047C1C0", "offset": 0, "expected": "00000001"},
                    {"symbol": "lbl_8047C1C4", "offset": 4, "expected": "00020003"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokeconv.c",
                mw_version="GC/2.6",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokeconv_exact_80089380.c",
                mw_version="GC/2.6",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_80090720.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_candidate_80091DA4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_80092FC8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_800937F4.c",
                constant_import=[
                    {"symbol": "lbl_8047C1F8", "offset": 0, "expected": "4330000000000000"},
                ],
                jump_table_import={
                    "symbol": "jumptable_802EECF0",
                    "function": "_gbaCommThread",
                    "owner": "game/data/data_802EEB78.c",
                    "owner_offset": 0x178,
                },
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_80093B04.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_candidate_80093B4C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_80093F2C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_candidate_80094650_gc125n.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_candidate_8009567C.c",
                # TEMPORARY constant_import and jump_table_import: the carved
                # renderer shares the file's conversion literals and switch pool.
                constant_import=[
                    {"symbol": "lbl_8047C220", "offset": 0, "expected": "4330000000000000"},
                    {"symbol": "lbl_8047C228", "offset": 8, "expected": "4330000080000000"},
                ],
                jump_table_import={
                    "symbol": "jumptable_802EF01C",
                    "function": "menuStatusDrawPokemonInfo",
                    "owner": "game/data/data_802EEB78.c",
                    "owner_offset": 0x4A4,
                },
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-fp_contract off"],
                progress_category="game",
            ),
            # fn_800965C8 carve: GC/2.0 -O3, peephole off.
            Object(
                Matching,
                "game/gbaCommunication_candidate_800965C8_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-O3", "-use_lmw_stmw off", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_candidate_80096C48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gbaCommunication_exact_800973EC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "dolphin/sdk_r48_80098108_prefix.c",
                mw_version="GC/1.2.5n",
                # EXIBios.c is built at optimization level 0 for space with
                # the scheduler off (formerly per-function pragmas).
                extra_cflags=["-opt level=0", "-opt space", "-schedule off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_r48_80099400_o2.c",
                mw_version="GC/1.2.5",
                cflags=["-O1" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-opt level=0"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_r48_80099790_suffix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_8009A0F4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSAlarm.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009A92C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_8009A9D8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009ABD0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_8009AC50.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009AFD0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSAudioSystem.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.2.5n",
                    progress_category="sdk",
                    # OSFatal.c's RGB2YUV is emitted with separate fmuls/fadds,
                    # so this SDK family predates the project-wide
                    # -fp_contract on.
                    extra_cflags=["-fp_contract off"],
                )
                for status, path in [
                    (Matching, "dolphin/os/OSError.c"),
                    (Matching, "dolphin/sdk_exact_8009C860.c"),
                    (Matching, "dolphin/sdk_candidate_8009CD38.c"),
                    (Matching, "dolphin/sdk_exact_8009D510.c"),
                    (Matching, "dolphin/sdk_candidate_8009DF3C.c"),
                ]
            ],
            *[
                Object(status, path, mw_version="GC/1.2.5n", progress_category="sdk")
                for status, path in [
                    (Matching, "dolphin/sdk_range_8009E7B0.c"),
                    (Matching, "dolphin/sdk_exact_8009ED4C.c"),
                    (Matching, "dolphin/sdk_candidate_8009ED70.c"),
                ]
            ],
            Object(
                Matching,
                "dolphin/sdk_range_8009F77C_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009F958.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_8009F9C8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_8009FADC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A03B4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            # SDK OSRtc.c head (0x800A064C-0x800A0D00) with its Scb .bss.
            Object(
                Matching,
                "dolphin/os/OSRtc.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A0D00.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A2B9C.c",
                mw_version="GC/1.2.5n",
                # __init_cpp is built with peephole off (was a source pragma).
                extra_cflags=["-opt nopeephole"],
                progress_category="sdk",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    cflags=(
                        ["-O2" if flag == "-O4,p" else flag for flag in cflags_base]
                        if use_o2
                        else None
                    ),
                    progress_category="sdk",
                )
                for status, path, version, use_o2 in [
                    (Matching, "dolphin/sdk_r58_800A2D38_prefix.c", "GC/1.2.5n", False),
                ]
            ],
            Object(
                Matching,
                "dolphin/sdk_r59_800A3194_prefix.c",
                mw_version="GC/1.2.5n",
                # PSMTXRotAxisRad is built with peephole off (was a source pragma).
                extra_cflags=["-opt nopeephole"],
                progress_category="sdk",
            ),
            # The O2 middle of the sdk_range_800A2D38 owner also wants
            # -opt nopeephole (90.11% -> 94.21% under the flag sweep).
            Object(
                Matching,
                "dolphin/sdk_r58_800A30E4_o2.c",
                mw_version="GC/1.1p1",
                cflags=["-O2,p" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_r59_800A33B4_suffix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_r59_800A335C_o2.c",
                mw_version="GC/1.1p1",
                cflags=["-O2,p" if flag == "-O4,p" else flag for flag in cflags_base],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3458.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off", "-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A35E4_suffix.c",
                mw_version="GC/1.2.5n",
                # C_MTXLightFrustum/C_MTXLightPerspective: peephole off (was a source pragma).
                extra_cflags=["-fp_contract off", "-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800A3744.c",
                mw_version="GC/1.2.5n",
                # C_MTXLightOrtho is built without fused multiply-add (was #pragma fp_contract off).
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800A37CC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800A3874.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3A78.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            # The vendor SDK separates vec.c from the quaternion source here.
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3C54.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3CB0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "dolphin/sdk_candidate_800A3D3C.c",
                constant_import=[{"symbol": "lbl_8047C2CC", "offset": 0, "expected": "3f800000"},
                                 {"symbol": "lbl_8047C2C8", "offset": 4, "expected": "00000000"},
                                 {"symbol": "lbl_8047C2D4", "offset": 8, "expected": "3f7fff58"}],
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/dvdfs_range_800A4D28.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800A7820.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A7880.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/data/data_80311C00.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "dolphin/vi/VI_exact_800A8178.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            # The whole fstload.c and vi.c TUs with their data.
            Object(
                Matching,
                "dolphin/dvd/fstload.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/vi/vi.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ai_exact_800AC02C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800AC440.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ai_exact_800AC5AC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800AC6D4.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_exact_800AC910.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_exact_800AC954.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_exact_800ACB44.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800ACBFC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/ar_dsp_exact_800AE3F0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dsp_task.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800AF280.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800AF35C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800AF474.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800AF8A0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B016C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B0174.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B01AC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800B0694.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800B1788.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B2070.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B2968.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B29F4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B2F84.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B3078.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B38DC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B3978.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/card_exact_800B3B68.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B4644.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B4DC4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B4E50.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B4FC0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B5070.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B5184.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_candidate_800B5228.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B5AE0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B5BA4.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                # Whole GXInit.c TU (0x800B5C5C-0x800B6FE0) with its .bss,
                # .sbss and .sdata2. Built with the peephole pass off.
                "dolphin/sdk_candidate_800B5C5C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B6FE0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B71FC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B771C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B771C_r40_800B7C18_gc11p1.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B771C_r40_800B7D3C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800B857C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800B884C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B8AE8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800B8DA8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B8E74.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800B8E98.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800B9578.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BA1B4.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BA44C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/gx/GX_exact_800BA4C8.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BA7C0.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BAE64.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BB30C_prefix.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BB780.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/sdk_range_800BB81C.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/gx/GXTev.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_candidate_800BC8F8.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BCEBC.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/sdk_candidate_800BCFDC.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(Matching, "dolphin/gx/GX_exact_800BD07C.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_candidate_800BD16C.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off"],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BD394.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_candidate_800BD454.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BD554.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_candidate_800BD58C.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-inline noauto", "-fp_contract off"],
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BD744.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_r52_800BD7A0_prefix.c",
                # CI-verified source symbol extents and relocation signatures.
                comparison_view="data_names",
                symbol_mappings={
                    'jumptable_80313628': '@146',
                    'jumptable_80313684': '@145',
                },
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(Matching, "dolphin/gx/GX_exact_800BE30C.c", mw_version="GC/1.2.5n", progress_category="sdk"),
            Object(
                Matching,
                "dolphin/sdk_candidate_800BE348.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata 0"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800BF33C.c",
                mw_version="GC/1.3.2",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "trk/trk_exact_800C3EBC.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/trk_range_800C3EBC.c",
                mw_version="GC/1.3",
                extra_cflags=["-DTRK_RANGE_800C3EBC_ONLY"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/trk_exact_800C4154.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/trk_range_800C4470.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "game/gs_range_800C45A0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "dolphin/sdk_r52_800C4D8C_suffix.c",
                mw_version="GC/1.3",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800C5458.c",
                # CI-verified source symbol extents and relocation signatures.
                comparison_view="data_names",
                symbol_mappings={
                    'lbl_8047C400': '@212',
                },
                mw_version="GC/2.5",
                extra_cflags=["-use_lmw_stmw on", "-str pool,readonly"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "crt/math_range_800CAA58.c",
                source="crt_matching/math_range_800CAA58.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_ieee754_atan2.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_range_800CB2B4.c",
                mw_version="GC/2.0",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_range_800CB4D8.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-DMATH_E_SPLIT", "-DMATH_E_FMOD"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_e_log_800CB814.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_e_pow_800CBA90.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_e_rem_pio2_800CC2C0.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_cos.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_rem_pio2.c",
                mw_version="GC/1.3",
                # __kernel_rem_pio2 saves registers with stmw/lmw (was a source pragma).
                extra_cflags=["-use_lmw_stmw on"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_sin.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_kernel_tan.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_atan.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_ceil.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_copysign.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_range_800CDBE0.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_exact_800CE04C.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_range_800CE378.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/math_exact_800CE59C.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "dolphin/sdk_range_800CE7DC.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CEB64.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_residual_800CED58.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CEF10.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CF254.c",
                mw_version="GC/1.2.5n",
                # This original EXIBios island alone was built without
                # instruction scheduling; adjacent Odemu/EXIUart code was not.
                extra_cflags=["-schedule off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/sdk_exact_800CF47C.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/gs_range_800D1070.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_mem.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_main.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_main_exact_800E3928.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_main_suffix_candidate_800E4AC0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4BF4.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4C98.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4D18.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4D3C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4DB0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E4E8C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E50A8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5188.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E51A4.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5550.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E563C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5790.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E584C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5978.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E59C8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5A74.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5B68.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E5BE0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5D40.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E5E34.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5FAC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E5FFC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E60F0.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E61BC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E638C.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E6478.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E65CC.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E66B8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6804.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E68D8.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E69C4.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6B20.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6BC8.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6DC0.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E6DCC.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E7290.c"),
                    (Matching, "game/gs_model_main_suffix_candidate_800E732C.c"),
                    (Matching, "game/gs_model_main_suffix_exact_800E85E8.c"),
                ]
            ],
            # modelShadowRender owns its .sdata2 literal pool (0x8047CBC0-0x8047CBE8)
            # and emits "shadow" there, so it needs -str reuse,readonly.
            Object(
                Matching,
                "game/gs_model_shadow_candidate_800E8684.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_shadow_exact_800E8EFC.c"),
                    (Matching, "game/gs_model_shadow_candidate_800E8F80.c"),
                    (Matching, "game/gs_model_shadow_exact_800E8FE8.c"),
                    (Matching, "game/gs_model_shadow_flags_exact_800E90C8.c"),
                    (Matching, "game/gs_model_shadow_candidate_800E9148.c"),
                    (Matching, "game/gs_model_shadow_exact_800E9288.c"),
                    (Matching, "game/gs_model_shadow_exact_800E92D8.c"),
                    (Matching, "game/gs_model_shadow_candidate_800E9358.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_state.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/1.3.2"
                        if path
                        in (
                            "game/gs_model_parse.c",
                            "game/gs_model_parse_exact_800EAFE4.c",
                        )
                        else "GC/1.3"
                    ),
                    # The parse TU is GC/1.3.2 -inline auto,deferred with
                    # strings in .rodata/.sdata2 (see gs_model_parse.c).
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-inline auto,deferred"]
                        if path == "game/gs_model_parse.c"
                        else []
                    )
                    + (
                        ["-str reuse,readonly"]
                        if path == "game/gs_model_parse_exact_800EAFE4.c"
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_parse.c"),
                    (Matching, "game/gs_model_parse_exact_800EAFE4.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_bound.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_bound_exact_800EB414.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_bound_exact_800EB464.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_bound_r55_800EB5A0_gc13_o2.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_bound_r55_800EB6E0_suffix.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_model_anim_candidate_800EBEEC.c"),
                    (Matching, "game/gs_model_anim_exact_800EC0E8.c"),
                    (Matching, "game/gs_model_anim_candidate_800EC35C.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_model_anim_exact_800EC4D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_anim_suffix_exact_800ED4D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_anim_suffix_candidate_800ED8C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_anim_suffix_exact_800EE044.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_part_exact_800EE150.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_part.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_part_exact_800EE6B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_scratch_800EE928.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_scratch_alloc_800EEC38.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_scratch_exact_800EEDF8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_range_800EEF48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80101910.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_8010C220 (an empty function) links as a carve; the rest of
            # the range is scored from the whole-range candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"] + (["-opt nopeephole"] if path in ("game/gs_range_80109C88.c", "game/gs_range_exact_8010A420.c", "game/gs_range_exact_menuModelInit.c") else []) + (["-O3,p", "-opt nopeephole"] if path == "game/gs_range_exact_8010B9E8.c" else []),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_range_80109C88.c"),
                    (Matching, "game/gs_range_exact_8010A420.c"),
                    (Matching, "game/gs_range_exact_menuModelInit.c"),
                    (Matching, "game/gs_range_exact_8010A88C.c"),
                    (Matching, "game/gs_range_exact_8010AB00.c"),
                    (Matching, "game/gs_range_exact_8010B560.c"),
                    (Matching, "game/gs_range_exact_8010B5C4.c"),
                    (Matching, "game/gs_range_exact_8010B718.c"),
                    (Matching, "game/gs_range_exact_8010B9E8.c"),
                    (Matching, "game/gs_range_exact_8010BCE4.c"),
                    (Matching, "game/gs_range_exact_8010C220.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_range_candidate_8010BD6C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_candidate_8010B16C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O3,p", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_exact_8010AE2C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_exact_8010B01C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O3,p", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_exact_8010BBB8.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O3,p", "-opt nopeephole"],
                progress_category="game",
            ),
            # menuFaceInit: same compiler, peephole off (see the file header).
            Object(
                Matching,
                "game/gs_range_exact_8010C224.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-O3"] if o3 else []),
                    ],
                    progress_category="game",
                )
                for status, path, o3 in [
                    (Matching, "game/gs_range_8010CBD0.c", True),
                    (Matching, "game/gs_colsys_exact_8010D170.c", True),
                    (Matching, "game/gs_colsys_exact_8010D20C.c", True),
                    (Matching, "game/gs_colsys_candidate_8010E53C.c", True),
                ]
            ],
            Object(
                Matching,
                "game/GScolsys2Walk.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Walk_exact_8010E138.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_801140DC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_80114AE0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_exact_80114CA8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_exact_80115250.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_801654E0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_candidate_80165D0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_80165DEC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_candidate_80165FDC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_8016604C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_candidate_80166D48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC_exact_80166E44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_801653CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # HAL's particle link lists (pslist.c, named by its assert
            # strings): owns its .rodata string pool and the three per-link
            # .bss arrays. GC/1.3.2 pools the arrays in particleSort (1.3
            # does not); -inline deferred gives retail's .bss order (reverse
            # definition order; first-reference order would put
            # activeParticle first); -str readonly puts the strings in
            # .rodata. See the file header.
            Object(
                Matching,
                "game/pslist.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/ps_app_srt_exact_8016A644.c"),
                    (Matching, "game/ps_candidate_8016A79C.c"),
                    (Matching, "game/ps_app_srt_exact_8016A93C.c"),
                    (Matching, "game/ps_exact_8016A9B4.c"),
                    (Matching, "game/ps_app_srt_exact_8016AAAC.c"),
                ]
            ],
            # HAL's particle module (particle.c) as one TU, 0x80169034 -
            # 0x8016A644, with its .rodata strings, psSetGeneratorAngleRadius-
            # Scale's jump table (.data), the per-bank tables and point JObjs
            # (pooled .bss) and its .sdata2 pool, on the particle library
            # flags (GC/1.3.2 pools the .bss tables; -inline deferred gives
            # the reverse definition order). See the file header.
            Object(
                Matching,
                "game/particle.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle command interpreter (psinterpret.c) as one TU with
            # its .rodata __FILE__, psInterpretParticle0's jump table (.data),
            # getFloat's .sbss scratch and its .sdata2 pool, on the particle
            # library flags, no local pragmas. See the file header.
            Object(
                Matching,
                "game/psinterpret.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle display (psdisp.c) as one TU, 0x8016AB94 -
            # 0x8016EC1C, with its .rodata, the quad texture coordinates
            # (.data), the display matrices (.bss), its .sbss state and its
            # .sdata2 pool, on the particle library flags. See the file header.
            Object(
                Matching,
                "game/psdisp.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle TEV set-up (psdisptev.c) as one TU with its .sbss,
            # on the particle library flags. See the file header.
            Object(
                Matching,
                "game/psdisptev.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            # HAL's particle generator module (generator.c) as one TU with its
            # data, on the pslist.c / psInitParticle flags. See the file header.
            Object(
                Matching,
                "game/generator.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-inline deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-opt level=0"] if path in (
                            "game/gs_range_8017A5FC_prefix.c",
                            "game/gs_range_8017A624_middle.c",
                            "game/gs_range_8017A814_suffix.c",
                            "game/fsys/fsys_system_8017AAA4.c",
                            "game/fsys/fsys_request_8017AF6C.c",
                        ) else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_range_8017A5FC_prefix.c"),
                    (Matching, "game/gs_range_8017A624_middle.c"),
                    (Matching, "game/gs_range_8017A814_suffix.c"),
                    (Matching, "game/fsys/fsys_system_8017AAA4.c"),
                    (Matching, "game/fsys/fsys_request_8017AF6C.c"),
                ]
            ],
            # 0x8017F2C4 - 0x80180C78 is optimisation-level-0 code (peephole
            # and scheduling still on): dead induction counters kept in
            # r30/r31, single-use locals and parameters stack-homed. The
            # exact middle island matches entirely under `-opt level=0` with
            # no local pragmas, as does the LZSS prefix; the suffix residuals
            # stay candidates built with the same flag.
            Object(
                Matching,
                "game/gs_range_8017F2C4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_8017F3F8_middle.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # The 0x8017FA5C - 0x801812C4 tail is one retail unit (the job
            # pool tasks after 0x80180C78 share its .sbss job pool
            # lbl_8047B1E0/E4/E8 and are started by fn_8018094C; people.c
            # starts at 0x801812C4); its exact
            # functions are carved into their own objects so they can link,
            # the rest stay candidates. All share `-opt level=0` and
            # `-inline deferred`. Evidence for deferred inlining: fsysCacheInit
            # addresses the unit's pooled .bss (queue +0, cache +0x20, arena
            # +0x1030). Normal inlining orders pooled .bss by first reference,
            # which puts the cache first (its pre-call store is the first
            # reference), while deferred inlining uses reverse definition
            # order, which gives retail's layout from plain definitions. Every
            # other exact function of the unit (fn_8017FDB0, fn_801808B4,
            # fn_801808E4, fn_80180B94) compiles byte-identically either way;
            # deferred inlining only reverses emission order, so
            # multi-function carves list their functions high address first.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0", "-inline deferred"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_range_8017FA5C_exact.c"),
                    (Matching, "game/gs_range_8017FA5C_suffix.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_8017FDB0.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_801800F8.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_80180320.c"),
                    (Matching, "game/gs_range_8017FA5C_residual_8018094C.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_80180B94.c"),
                    (Matching, "game/gs_range_8017FA5C_exact_80180C78.c"),
                ]
            ],
            # HAL's bytecode.c (HSD_ByteCodeEval) as one translation unit,
            # text, .rodata and .sdata2, with the sysdolphin library flags and
            # no local pragmas. See the file header.
            Object(
                Matching,
                "hsd/bytecode.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # hsdSearchClassInfo, hsdIsDescendantOf and hsdNew, carved from
            # HAL's class.c (the HSD base class): the whole TU with its
            # .rodata/.data/.sbss/.sdata2, exact with the HSD library flags and
            # no pragmas. See the file header.
            Object(
                Matching,
                "hsd/class.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL's mobj.c as one translation unit, text and data, with the
            # sysdolphin library flags and no local pragmas. See the file
            # header.
            Object(
                Matching,
                "hsd/mobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if use_o1
                        else None
                    ),
                    extra_cflags=(
                        ["-use_lmw_stmw on"]
                        if use_o1
                        else ["-O2", "-use_lmw_stmw on"]
                    ),
                    progress_category="hsd",
                )
                for path, use_o1 in [
                ]
            ],
            # HAL's tobj.c as one translation unit, text and data, with the
            # sysdolphin library flags and no local pragmas. See the file
            # header.
            Object(
                Matching,
                "hsd/tobj.c",
                # CI-verified source symbol extents and relocation signatures.
                comparison_view="data_names",
                symbol_mappings={
                    'jumptable_8036D5A4': '@827',
                    'jumptable_8036D5D0': '@918',
                    'jumptable_8036D5F0': '@1082',
                    'jumptable_8036D610': '@1081',
                    'jumptable_8036D630': '@1080',
                    'jumptable_8036D650': '@1079',
                    'jumptable_8036D670': '@1078',
                    'jumptable_8036D690': '@1077',
                    'jumptable_8036D6B0': '@1076',
                    'jumptable_8036D6D0': '@1075',
                    'jumptable_8036D6F0': '@1198',
                    'jumptable_8036D714': '@1281',
                    'jumptable_8036D734': '@1648',
                    'lbl_8036D43C': 'zero$288',
                    'lbl_8036D46C': 'func$323',
                    'lbl_8036D48C': 'tevdesc$355',
                    'lbl_8036D510': 'tevdesc$365',
                    'lbl_8036D594': 'default_lod$572',
                },
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "game/fight_range_80201764.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80265EC4_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80265F94_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80266250_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_80009178.c",
                mw_version="GC/1.3",
                extra_cflags=["-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_800096B4.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_8000BE74.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80051710.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_8005344C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_exact_80054914.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_suffix_800552D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_80055E38.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_80056C54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_80057B34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_candidate_80057C9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_80057DE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_r56_80057E70_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_r56_80057F94_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_candidate_80058DCC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_80058F08.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_pre_main.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_800599AC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_candidate_80059BDC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-str reuse,readonly"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_8005CCD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_8005CD88.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_exact_8005CEE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_exact_8005D26C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_candidate_8005D2E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_exact_8005D3D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_candidate_8005D5CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuColosseumBattle_tail_exact_8005D6A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_bios_range_8005D7F8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_debug_exact_8005DA48.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_debug_exact_8005DBC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_debug_exact_8005DC24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_debug_exact_8005DCC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_debug_exact_8005DEE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_BattleStart.c",
                constant_import=[{"symbol": "lbl_8047BF60", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BF80", "offset": 8, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047BF88", "offset": 16, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole",
                              "-DMENUCB_BATTLESTART_8005DFC8_ONLY"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_8005E690_inline_off.c",
                mw_version="GC/1.3",
                cflags=[
                    "-inline off" if flag == "-inline auto" else flag
                    for flag in cflags_base
                ],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_8005FFE4.c",
                # TEMPORARY constant_import: retail shares the TU's literal pool;
                # its {6,6,6,6} initialiser is labelled as two 4-byte words.
                constant_import=[
                    {"symbol": "lbl_8047BF58", "offset": 0, "expected": "00060006"},
                    {"symbol": "lbl_8047BF5C", "offset": 4, "expected": "00060006"},
                ],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_80060434.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_800609B4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_80060A28.c",
                # TEMPORARY constant_import: retail shares the TU's literal pool.
                constant_import=[
                    {"symbol": "lbl_8047BF60", "offset": 0, "expected": "00000000"},
                    {"symbol": "lbl_8047BF94", "offset": 4, "expected": "41200000"},
                    {"symbol": "lbl_8047BF98", "offset": 8, "expected": "41a00000"},
                    {"symbol": "lbl_8047BF9C", "offset": 12, "expected": "c1a00000"},
                    {"symbol": "lbl_8047BF90", "offset": 16, "expected": "3f800000"},
                ],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_80060D70.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_8006106C.c",
                constant_import=[{"symbol": "lbl_8047BF80", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_80061454.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_80061BBC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_80061D34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_80061F6C.c",
                # TEMPORARY constant_import: retail shares this conversion bias.
                constant_import=[
                    {"symbol": "lbl_8047BF88", "offset": 0, "expected": "4330000000000000"}
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_80062284.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_candidate_80062334.c",
                # TEMPORARY constant_import: the closing stores' 0.0f is retail's shared lbl_8047BF60.
                constant_import=[
                    {"symbol": "lbl_8047BF60", "offset": 0, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_800626CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_BattleStart_exact_80062834_gc125n.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_80062948.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_800638F4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-i src/game/menu"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_80063AD4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-i src/game/menu", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80063D10.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_range_80063D14.c",
                constant_import=[
                    {"symbol": "lbl_8047BFE8", "offset": 0, "expected": "00000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80064378.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_r57_800643D4_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_r57_80065628_o1.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_r57_80065730_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80065A48.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_range_800676EC.c",
                constant_import=[{"symbol": "lbl_8047BFF0", "offset": 0, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047BFE8", "offset": 8, "expected": "00000000"}],
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80068738.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_r55_80068794_gc13_o4s.c",
                constant_import=[{"symbol": "lbl_8047BFE8", "offset": 0, "expected": "00000000"}],
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_exact_800688C4.c",
                mw_version="GC/2.0",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_r55_800688C4_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80069048.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_r50_8006905C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_r50_80069220_o4s.c",
                constant_import=[{"symbol": "lbl_8047BFE8", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BFF0", "offset": 8, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047C020", "offset": 16, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menu/menuCB_r50_800693A4_suffix.c",
                constant_import=[{"symbol": "lbl_8047BFE8", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047BFF0", "offset": 8, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047C020", "offset": 16, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_800697C4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_800697F4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_range_exact_80069A08.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menuCB_Battle.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Linked carve 0x8007109C-0x80071398 (MENU_R56B_8007109C_ONLY).
            Object(
                Matching,
                "game/menu/menu_r56b_8007109C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Linked carve 0x80071398-0x800714C8 (MENU_R56B_80071398_ONLY).
            Object(
                Matching,
                "game/menu/menu_r56b_80071398_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # menuCB_Common.c tail carve: GC/1.3 -O4,p, peephole off.
            Object(
                Matching,
                "game/menu/menu_r56b_800714C8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # pkjb_uploader.c TU (0x800716C8 - 0x80075390): GC/2.5, -O4,p
            # with the peephole pass off; see pkjb_exact_80073990.c for the
            # evidence and include/game/menu/pkjb_uploader_shared.h for the
            # unit's extent. Carved at function boundaries; data stays extern.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/pkjb_exact_800716C8.c"),
                    (Matching, "game/menu/pkjb_candidate_80071AE4.c"),
                    (Matching, "game/menu/pkjb_exact_80071E34.c"),
                    (Matching, "game/menu/pkjb_candidate_80071EA4.c"),
                    (Matching, "game/menu/pkjb_exact_800722A0.c"),
                    (Matching, "game/menu/pkjb_candidate_80072A00.c"),
                    (Matching, "game/menu/pkjb_exact_80072C74.c"),
                    (Matching, "game/menu/pkjb_candidate_80072D58.c"),
                    (Matching, "game/menu/pkjb_exact_80073034.c"),
                    (Matching, "game/menu/pkjb_candidate_800733D0.c"),
                    (Matching, "game/menu/pkjb_exact_80073690.c"),
                    (Matching, "game/menu/pkjb_candidate_80073700.c"),
                    (Matching, "game/menu/pkjb_exact_80073990.c"),
                    (Matching, "game/menu/menu_candidate_80074324.c"),
                    (Matching, "game/menu/menu_candidate_8007480C_gc125n.c"),
                ]
            ],
            # pkjbPrepare reports through real string literals; retail pools
            # the unit's read-only strings off one base register (-rostr).
            # TEMPORARY constant_import: this carve emits only the first 101
            # bytes of the retail pool owned by rodata_8026860C.c.
            Object(
                Matching,
                "game/menu/menu_candidate_80073E8C_gc20.c",
                constant_import=[
                    {
                        "section": ".rodata",
                        "symbol": "lbl_80268780",
                        "offset": 0,
                        "expected": (
                            "706b6a625f75706c6f616465722e6300504b4a425f507265706172653a20"
                            "6e6f7420666f756e6400504b4a425f507265706172653a20656d707479206f"
                            "7220746f6f206269672073697a6500504b4a425f507265706172653a207265"
                            "6164206572726f7200"
                        ),
                    }
                ],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-rostr"],
                progress_category="game",
            ),
            # GC/1.3 -O4,p chunk of the menu range bucket, built with
            # unit-wide -opt nopeephole instead of local pragmas; the
            # menuCBRule tail (0x80077A5C-0x80077ED4) links as a carve.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    # Linked carve (MENU_80075390_ONLY) with its .sdata2 pool.
                    (Matching, "game/menu/menu_candidate_80075390.c"),
                    (Matching, "game/menu/menu_exact_80075A34.c"),
                    (Matching, "game/menu/menu_candidate_80075DC8.c"),
                    (Matching, "game/menu/menu_exact_80075EE0.c"),
                    (Matching, "game/menu/menu_exact_80076334.c"),
                    (Matching, "game/menu/menu_candidate_80076A8C.c"),
                    (Matching, "game/menu/menu_exact_80076F2C.c"),
                    (Matching, "game/menu/menu_candidate_800776E4.c"),
                    (Matching, "game/menu/menu_exact_80077A5C.c"),
                ]
            ],
            # Linked carve 0x80076054-0x80076334 (MENU_80076054_ONLY). GC/1.3.2
            # with read-only strings pools the menuCB_Rule.c tables and assert
            # strings off one base as retail does; the pool is retail
            # lbl_80268940 (rodata_8026860C.c), so the import checks its bytes
            # and retargets the relocations there.
            Object(
                Matching,
                "game/menu/menu_candidate_80076054.c",
                # TEMPORARY constant_import: the carve's read-only pool is
                # retail lbl_80268940 + 0x0..0x135 (rule presets, error table,
                # assert file and condition strings).
                constant_import=[
                    {"symbol": "lbl_80268940", "section": ".rodata", "offset": 0, "expected": (
                        "0001006402580001000000000101000001010001ffc4ffec00000000000000000000000000000000000000000000000000000000000000000000000000000000"
                        "000000000000000000000000000000000000000000010032012c0006000000000000000001010001ffc4ffec0000000000000000000000000000000000000000"
                        "000000000000000000000000000000000000000000000000000000000000000000000000000000000001006402580006000000000000000001010001ffc4ffec"
                        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000044254427"
                        "44284429442a44bf6d656e7543425f52756c652e63000000302026262273706563696669656420696e76616c6964206172672e2200"
                    )},
                ],
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-rostr"],
                progress_category="game",
            ),
            # Linked carve 0x80077ED4-0x800798E8 (MENU_R47_80077ED4_ONLY): owns
            # fn_80078D5C's switch table and the shared int->float biases.
            Object(
                Matching,
                "game/menu/menu_candidate_r47_80077ED4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_exact_800798E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Linked carve 0x80079C1C-0x8007B350 (MENU_R47_80079C1C_ONLY): owns
            # its .data string/switch tables and the .sdata2 int->float biases.
            Object(
                Matching,
                "game/menu/menu_candidate_r47_80079C1C.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_exact_8007B350.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_exact_8007B6A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_exact_8007B6D8.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_candidate_80075390_r46_8007C23C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_poke_coupon.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Whole SDK OSContext.c (0x8009B914-0x8009C2E0) with its .data;
            # its asm functions are admitted in docs/asm_evidence/oscontext.md.
            Object(
                Matching,
                "dolphin/os/OSContext.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-fp_contract off", "-opt nopeephole"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                # Whole SIBios.c TU (0x800CF708-0x800D0DF8) with its .data,
                # .sdata, .bss and .sbss.
                "dolphin/si/SI_range_800CF764.c",
                mw_version="GC/1.2.5n",
                # No automatic inlining: SIGetStatus stays a call (was #pragma dont_inline on).
                extra_cflags=["-inline noauto"],
                progress_category="sdk",
            ),
            # fn_8018FE30 alone (GS_RANGE_8018FE30_FN_8018FE30_ONLY): the
            # setters it inlines are static inline copies there.
            Object(
                Matching,
                "game/gs_range_8018FE30.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DGS_RANGE_8018FE30_FN_8018FE30_ONLY",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_flag_test_exact_801902E0.c"),
                    (Matching, "game/gs_range_8018FE30_suffix_801903B0.c"),
                    (Matching, "game/gs_flag_get_exact_801906A0.c"),
                    (Matching, "game/gs_range_8018FE30_suffix_8019075C.c"),
                    (Matching, "game/gs_flag_exact_801908D4.c"),
                ]
            ],
            Object(
                Matching,
                "game/effect/fade_range_801C6934.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_exact_801C766C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_candidate_801C7730.c",
                constant_import=[
                    {"symbol": "lbl_8047E100", "offset": 0, "expected": "3f800000"},
                    {"symbol": "lbl_8047E120", "offset": 8, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047E128", "offset": 16, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_exact_801C84FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # std205_getDNASampleSepcies and std200_gbaConnectEvent share one unit: their three switch tables are one
            # .data run (0x8036DD90-0x8036DE6C) and std200_gbaConnectEvent's starts 4-aligned at 0x8036DE1C.
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_exact_801C8834.c",
                constant_import=[{"symbol": "lbl_8047E120", "offset": 0, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047E128", "offset": 8, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_801CADA8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_exact_801C8DD0.c",
                constant_import=[{"symbol": "lbl_8047E120", "offset": 0, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047E128", "offset": 8, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"] + (["-opt nopeephole"] if path in ("game/field_range_801CA7EC_prefix.c",) else []),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/field_range_801CA7EC_prefix.c"),
                    (Matching, "game/field_exact_801CA9F0.c"),
                    (Matching, "game/field_exact_801CA9F8.c"),
                    (Matching, "game/field_exact_801CADA0.c"),
                    (Matching, "game/field_exact_801CAE80.c"),
                ]
            ],
            Object(
                Matching,
                "game/field_exact_801CAA08.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_candidate_801CAAF4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_801CAC6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_exact_801CAEA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_range_801CAF0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (["-opt nopeephole"] if path in ("game/field_range_801CB180.c", "game/field_candidate_801CB61C.c", "game/field_candidate_801CB834.c") else []),
                    progress_category="game",
                )
                for status, path, version in [
                    (Matching, "game/field_range_801CB180.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CB59C.c", "GC/1.3"),
                    (Matching, "game/field_candidate_801CB61C.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CB7C4.c", "GC/1.3"),
                    (Matching, "game/field_candidate_801CB834.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CB9D8.c", "GC/1.3"),
                    (Matching, "game/field_candidate_801CBA0C.c", "GC/1.3"),
                    (Matching, "game/field_exact_801CBA84.c", "GC/1.3"),
                ]
            ],
            Object(
                Matching,
                "game/field_exact_801CBA90.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O3,p", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/field_r55_801CBAB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O2,p", "-opt nopeephole"],
                progress_category="game",
            ),
            # Whole memory-card TU (src/game/memcard.c, .text 0x801CBBAC-0x801D0AA0
            # with its .rodata, .data switch tables, .bss, .sbss and .sdata2):
            # GC/2.5 -O4,p with read-only strings and deferred inlining; see that
            # file for the evidence. Replaces the former per-function carves.
            Object(
                Matching,
                "game/memcard.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-str reuse,readonly",
                    "-inline deferred",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/battle/battle_exact_801D0AA0.c"),
                ]
            ],
            Object(
                Matching,
                "game/battle/battle_candidate_801D0C30.c",
                mw_version="GC/1.3.2",
                # pokemonCenterBallMachineSequence calls heroGetPokemonBallId out of line in retail.
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline noauto"],
                progress_category="game",
            ),
            # The exact animation ranges link without moving their shared pool;
            # the preceding callbacks remain in the whole-range candidate.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_range_801DE698.c"),
                    (Matching, "game/gs_range_exact_801DEE14.c"),
                    (Matching, "game/gs_range_exact_801DF160.c"),
                    (Matching, "game/gs_range_candidate_801DF1D0.c"),
                ]
            ],
            # The whole etctool TU (0x801DF474-0x801E0FB4) with its .data and
            # .sdata2 pool; GC/1.3.2 addresses the pooled file statics as
            # retail does.
            Object(
                Matching,
                "game/etctool.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    # fn_801E0FB4 (XD _vtrUpdateFunc) opens the vtr unit and
                    # uses no pool constant, so it links as a data-free carve.
                    (Matching, "game/gs_exact_801E0FB4.c"),
                    (Matching, "game/gs_exact_801E1170.c"),
                    (Matching, "game/gs_candidate_801E11F0.c"),
                    (Matching, "game/gs_exact_801E1258.c"),
                    (Matching, "game/gs_candidate_801E1300.c"),
                    (Matching, "game/gs_exact_801E16D0.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_movie_801E189C.c",
                mw_version="GC/1.3.2",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-str reuse,readonly",
                ],
                progress_category="game",
            ),
            # Dolphin SDK THP player sample, as the game built it (the same
            # sample options doldecomp/sms uses: -inline auto + deferred).
            # Deferred inlining is evidenced by every TU's functions and .bss
            # objects being laid out in reverse source order, with the
            # sample's small queue helpers expanded into their callers; auto
            # inlining by THPVideoDecode.c (VideoDecoder only matches with it).
            # -O4,s is evidenced by two-register saves using stmw
            # (THPGXYuv2RgbSetup). THPPlayer.c builds with noauto: with auto,
            # its stream-start routine (0x801E34F0) is inlined into
            # THPPlayerPrepare, which the retail build did not do. On noauto
            # all 21 of its functions are exact except movieStreamFill (98.7%).
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.5",
                    cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                    extra_cflags=[
                        inline,
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-str reuse,readonly",
                    ],
                    progress_category="sdk",
                )
                for status, path, inline in [
                    (Matching, "dolphin/thp/THPRead.c", "-inline auto,deferred"),
                    (Matching, "dolphin/thp/THPDraw.c", "-inline auto,deferred"),
                    (Matching, "dolphin/thp/THPPlayer.c", "-inline noauto,deferred"),
                    (Matching, "dolphin/thp/THPAudioDecode.c", "-inline auto,deferred"),
                    (Matching, "dolphin/thp/THPVideoDecode.c", "-inline auto,deferred"),
                ]
            ],
            # THPDec.c as one unit (0x801E5548-0x801ECFE0) with its data.
            Object(
                Matching,
                "dolphin/thp/THPDec.c",
                mw_version="GC/1.2.5n",
                extra_cflags=["-i src/dolphin/thp"],
                progress_category="sdk",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    cflags=(
                        ["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base]
                        if path == "game/field_range_801ECFE0.c"
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/field_range_801ECFE0.c"),
                    (Matching, "game/field_exact_801ED218.c"),
                    (Matching, "game/field_candidate_801ED310.c"),
                    (Matching, "game/field_exact_801ED388.c"),
                    (Matching, "game/field_exact_801ED3B8.c"),
                ]
            ],
            Object(
                Matching,
                "game/battle/battle_range_exact_801ED640.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_candidate_801ED780.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EE034.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EE0BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_candidate_801EE958.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EEAD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_candidate_801EEB34.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EEC74.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_candidate_801EEE6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_range_exact_801EEEB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "crt/__start.c",
                mw_version="GC/1.2.5n",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/__start_mem_80003458.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/interrupt_vectors",
                source="crt/interrupt_vectors.s",
                extra_asflags=["-mregnames"],
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/__start_mem_800053E0.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "game/fight_range_80211A00.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-DFIGHT_RANGE_80211A00_ONLY", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80212840.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802128D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80213158.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802128D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802134D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80213558.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802136A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80213A78.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80213A78.c",
                constant_import=[
                    {"symbol": "lbl_8047E610", "offset": 0, "expected": "4330000000000000"},
                ],
                jump_table_import={
                    "symbol": "jumptable_80399FF4",
                    "function": "wsCmdEF",
                    "owner": "game/data/data_8038FFFC.c",
                    "owner_offset": 0x9FF8,
                },
                mw_version="GC/2.0",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80211A00_exact_80214450.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80211A00_suffix_80216A58.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80217018.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80217D34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80217E20.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80218BD4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80218D24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80218FDC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80218FDC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219270.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_candidate_802192B4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219804.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_candidate_80219838.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219D98.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80219E10.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80219FE4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021A054.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021A2C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8021A338.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021A6CC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fight_range_8021B910.c"),
                    (Matching, "game/fight_range_exact_8021C034.c"),
                ]
            ],
            Object(
                Matching,
                "game/fight_range_exact_8021C0F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8021CA00.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O2"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021CB58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8021D40C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8021D9C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8021FAD4.c",
                mw_version="GC/2.0",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80220778.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80220868.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80220B8C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022106C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80221104.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80222110.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80222BD8.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80222C44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802230BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80223A24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80224158.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80224740.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802247D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802249B8_to_8022BB84.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022BB84.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022BE2C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022D20C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022D6BC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022DCB8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8022F2F8.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_8022FE20.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80230568.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_802316FC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80232110.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80232D28.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80232FE4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802331F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_exact_80233DB0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80234A0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023565C_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802373B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023753C_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80238060.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_802381C4_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80238B0C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80238E30_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023A308.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nocse"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023A740.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s", "-opt nolifetimes"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_8023B498.c",
                mw_version="GC/2.5",
                extra_cflags=["-O4,s", "-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_range_80211A00_exact_8023CA9C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O4,s"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_80240BD0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_candidate_80241660.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_80241B70.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_middle.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_middle_exact_80244318.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_middle_suffix_8024498C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_802451C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_candidate_80245FC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_80247048.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024A664.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_residual_8024B474.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024BA44.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_residual_8024BFC0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024C5BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024D818.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_candidate_8024DC7C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_value_exact_8024DE8C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_irekae.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_irekae_candidate_8024E690_gc20.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    Matching,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-O4,s",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/fight_trainer_ai_irekae_r58b_8024F8B4_o1.c",
                    "game/fight_trainer_ai_irekae_r58b_80250070_suffix.c",
                ]
            ],
            Object(
                Matching,
                "game/fight_trainer_ai_irekae_r47_select_item_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_damage.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_exact_80253950.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_candidate_802546E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_exact_80254810.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_candidate_80256ED0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit_exact_802570D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai_waza_hit.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_exact_8025C5A4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_exact_8025C770.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_candidate_8025C808.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_trainer_ai2_exact_8025CAA8.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-O4,s",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-schedule on", "-DTOOLENTRY_DEBUG_ONLY"] if path == "game/toolentry.c" else []),
                        *(["-schedule on"] if path == "game/toolentry_candidate_8025D644.c" else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/toolentry_exact_8025CD64.c"),
                    (Matching, "game/toolentry.c"),
                    (Matching, "game/toolentry_exact_8025D06C.c"),
                    (Matching, "game/toolentry_exact_8025D28C.c"),
                    (Matching, "game/toolentry_exact_8025D560.c"),
                    (Matching, "game/toolentry_candidate_8025D644.c"),
                    (Matching, "game/toolentry_exact_8025D744.c"),
                    (Matching, "game/toolentry_candidate_8025D788.c"),
                    (Matching, "game/toolentry_exact_8025D914.c"),
                    (Matching, "game/toolentry_exact_8025D938.c"),
                    (Matching, "game/toolentry_exact_8025D9A8.c"),
                    (Matching, "game/toolentry_exact_8025DBB0.c"),
                ]
            ],
            Object(
                Matching,
                "game/toolentry_candidate_8025D164.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/toolentry_r55_8025D364_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/toolentry_r55_8025D3F4_gc13_o1.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-schedule on",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/table_res_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_before_particle.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_sequence_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,p", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_particle_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_exp_candidate.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_ceremony_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,p", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_after_particle.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,p", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_pre_main_exact.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O4,p", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_waitTime_exact.c",
                constant_import=[
                    {"symbol": "lbl_8047E668", "offset": 0, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_prefix_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,p", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/pokemon_relive_suffix_exact.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Dolphin SDK GBA library: GC/1.2.5n with the base SDK flags.
            *[
                Object(status, path, mw_version="GC/1.2.5n", progress_category="sdk")
                for status, path in [
                    (Matching, "game/gba/GBA.c"),
                    (Matching, "game/gba/GBARead.c"),
                    (Matching, "game/gba/GBAWrite.c"),
                    (Matching, "game/gba/GBAXfer.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/memo_exact_8025FEE4.c"),
                    (Matching, "game/memo_candidate_8025FF18.c"),
                ]
            ],
            Object(
                Matching,
                "game/memo_r57b_8025FA20_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/memo_r57b_8025FBCC_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/memo_r57b_8025FD34_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/evolution_exact_802600E4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/evolution_candidate_80260910.c",
                constant_import=[
                    {"symbol": "lbl_8047E6B8", "offset": 0, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/evolution_exact_80260EBC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/evolution_r57b_802612D0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/char_name_bios.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole,nocommonsubs"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_gsfloor.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                    "-DFIGHT_MENU_ONLY_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_candidate_8026316C_o2.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_exact_80263BC8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_candidate_80263BC8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_exact_80264ADC.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_candidate_80264D58.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-O4,s",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-DFIGHT_MENU_CANDIDATE_80261B68",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_menu_exact_8026503C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fight_timer.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/d2present.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_exact_801EF02C.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_residual_801EF4B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_exact_801EF5C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_candidate_801EFA08.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_main_exact_801EFFC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # battleGridHideModelsExcept builds best under GC/1.3.2 (flag sweep);
            # without BATTLE_CAMERA_FULL_TU the file builds only this unit's range.
            Object(
                Matching,
                "game/battle/battle_camera.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/battle/battle_camera_exact_801C2AE8.c"),
                    (Matching, "game/battle/battle_camera_exact_801C2D5C.c"),
                ]
            ],
            Object(
                Matching,
                "game/battle/battle_camera_r50_801C2D80_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_camera_r50_801C2F00_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_exact_801C3108.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_r56_801C3114_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_r56_801C3A64_o1.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_800E0DDC_r40_800E1544_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # menuFightOpenWaza: standalone data-free carve.
            Object(
                Matching,
                "game/menuFight_r40_800117BC_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # menuFightCloseTop / fn_80011A1C: standalone data-free carve.
            Object(
                Matching,
                "game/menuFight_exact_800119A8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Head of the title TU, 0x8001EF78-0x8002058C (the former
            # gs_pcbox carves and fn_80020328/800203B4/8002049C), with its
            # .sdata2 pool 0x8047B808-0x8047B868.
            Object(
                Matching,
                "game/gs_title_8001EF78.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequence_candidate_801DC014_r40_801DCAA0_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/wazaSequence_candidate_801DC014_r40_801DCBC8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_r40_8002B880_gc125n.c",
                constant_import=[
                    {"symbol": "lbl_8047B9A4", "offset": 0x00, "expected": "40000000"},
                    {"symbol": "lbl_8047B98C", "offset": 0x04, "expected": "40490fdb"},
                    {"symbol": "lbl_8047B9A0", "offset": 0x08, "expected": "3f000000"},
                    {"symbol": "lbl_8047B978", "offset": 0x0C, "expected": "437f0000"},
                    {"symbol": "lbl_8047B9AC", "offset": 0x10, "expected": "42340000"},
                    {"symbol": "lbl_8047B97C", "offset": 0x14, "expected": "3f800000"},
                    {"symbol": "lbl_8047B9B0", "offset": 0x18, "expected": "3e800000"},
                    {"symbol": "lbl_8047B9B4", "offset": 0x1C, "expected": "3f400000"},
                    {"symbol": "lbl_8047B998", "offset": 0x20, "expected": "4330000080000000"},
                ],
                # menuShopDrawMarqueeFrame only (MENUSHOP_R40_8002B880_ONLY carve), the same
                # compiler as menuShopDrawMarqueeDot; this function needs -fp_contract off.
                mw_version="GC/2.0",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-fp_contract off",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            # 0x8002BCE8-0x8002C284 and 0x8002CE6C-0x8002D5D4: exact runs linked as
            # menuShop.c islands; the residual pair stays a candidate.
            Object(
                Matching,
                "game/menuShop_island_8002BCE8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_candidate_8002C284.c",
                # fn_8002C284 only (MENUSHOP_CANDIDATE_8002C284_ONLY carve); the
                # unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_candidate_8002C408.c",
                # fn_8002C408 only (MENUSHOP_CANDIDATE_8002C408_ONLY carve); the
                # unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_island_8002CE6C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_8002D5D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_candidate_8002D91C.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nolifetimes", "-opt nopeephole", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_8002DC6C.c",
                constant_import=[{"symbol": "lbl_8047B998", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_residual_801C3114_r40_801C3B80_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_exact_801C3C98.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_residual_801C3E3C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle/battle_grid_exact_801C3F10.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/fade.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # fade_effect.o (XD fade_effect.cpp), linked whole: .text
            # 0x801C4814 - 0x801C6934 with its .sdata2 pool 0x8047DFD8 -
            # 0x8047E0A8. fade_effect.c includes fade_range_801C4CB8.c for
            # 0x801C4CB8 onwards. Built without the peephole pass
            # (docs/recon/fade_tu_d11.md).
            Object(
                Matching,
                "game/effect/fade_effect.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_party_access.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # gba_misc TU flag set: GC/2.0, -O4,p, "-opt nopeephole" (as the
            # fn_8008AC34 carve below); every exact function here matches under
            # it with no local pragmas.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/2.0",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gba/gba_misc_exact_800896B8.c"),
                    (Matching, "game/gba/gba_misc_r51_80089B8C_suffix.c"),
                    (Matching, "game/gba/gba_misc_exact_80089CA8.c"),
                    (Matching, "game/gba/gba_misc_exact_80089D30.c"),
                    (Matching, "game/gba/gba_misc_exact_80089D98.c"),
                    (Matching, "game/gba/gba_misc_candidate_80089E20.c"),
                    (Matching, "game/gba/gba_misc_exact_80089F58.c"),
                    (Matching, "game/gba/gba_misc_exact_8008A99C.c"),
                    (Matching, "game/gba/gba_misc_exact_8008A9AC.c"),
                    (Matching, "game/gba/gba_misc_candidate_8008A9E4.c"),
                    (Matching, "game/gba/gba_misc_exact_8008AB4C.c"),
                    (Matching, "game/gba/gba_misc_exact_8008ABE4.c"),
                ]
            ],
            Object(
                Matching,
                "game/gba/gba_misc_r51_800896E8_o2.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gba/gba_misc_r51_80089978_o2.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gba/gba_misc_candidate_8008ABE4_r42_8008AC34_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    Matching,
                    path,
                    mw_version=(
                        "GC/1.3"
                        if path.endswith("_8008C6FC_suffix.c")
                        else "GC/2.0"
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (
                        []
                        if path.endswith("_8008C6FC_suffix.c")
                        else ["-opt nopeephole"]
                    ),
                    jump_table_import=(
                        {
                            "function": "pokeconvPokemonToGBA",
                            "owner": "game/data/data_802EEB78.c",
                            "tables": [
                                {"symbol": "jumptable_802EEBB8", "owner_offset": 0x40},
                                {"symbol": "jumptable_802EEBE0", "owner_offset": 0x68},
                            ],
                        }
                        if path.endswith("_8008AE18_prefix.c")
                        else None
                    ),
                    progress_category="game",
                )
                for path in [
                    "game/gba/gba_misc_r58_8008AE18_prefix.c",
                    "game/gba/gba_misc_r58_8008C5D4_o1.c",
                    "game/gba/gba_misc_r58_8008C6FC_suffix.c",
                ]
            ],
            Object(
                Matching,
                "game/gba/gba_misc_exact_8008BBDC.c",
                jump_table_import={
                    "function": "pokeconvPokemonFromGBA",
                    "owner": "game/data/data_802EEB78.c",
                    "tables": [
                        {"symbol": "jumptable_802EEC10", "owner_offset": 0x98},
                        {"symbol": "jumptable_802EEC30", "owner_offset": 0xB8},
                    ],
                },
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gba/gba_misc_candidate_80089F78.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gba/gba_conv_exact_80088EA8.c"),
                    (Matching, "game/gba/gba_conv_candidate_80088F58.c"),
                    (Matching, "game/gba/gba_conv_exact_80089028.c"),
                    (Matching, "game/gba/gba_conv_candidate_80089030.c"),
                ]
            ],
            Object(
                Matching,
                "game/gba/gba_conv_r49_80088428_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gba/gba_conv_r49_800884BC_gc125_o2.c",
                mw_version="GC/2.6",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    cflags=(
                        [opt if flag == "-O4,p" else flag for flag in cflags_base]
                        if opt
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", *extra],
                    progress_category="game",
                )
                # menuSaveLoadSaveGame's assert strings are pooled in .rodata
                # (lbl_8026F4F8) and addressed as base+0: GC/1.3.2 + -rostr.
                for status, path, version, opt, extra in [
                    (Matching, "game/gba/gba_conv_r59_800886D0_o1.c", "GC/1.3", None, ["-opt nopeephole"]),
                    (Matching, "game/gba/gba_conv_r59_80088964_middle.c", "GC/1.3", None, ["-opt nopeephole"]),
                    (Matching, "game/gba/gba_conv_r59_800889E4_o1.c", "GC/1.3.2", None, ["-rostr", "-opt nopeephole"]),
                    (Matching, "game/gba/gba_conv_r59_80088C60_suffix.c", "GC/1.3", None, ["-opt nopeephole"]),
                ]
            ],
            Object(
                Matching,
                "game/GScolsys2Util.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Human_range_8010FAF4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Human_exact_8010FFC4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Thru.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Thru_r56_80111864_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GScolsys2Check.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # The GScolsys2Sun TU (.text 0x80111C24-0x80111DF8, .sdata2
            # 0x8047CF68-0x8047CF70), linked whole; the floor helpers after it
            # stay a candidate. Both build nopeephole, like floor.c.
            Object(
                Matching,
                "game/GScolsys2Sun.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/floor_range_80111DF8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/floor.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-str reuse,readonly"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_colsys.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O3,p", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_colsys_candidate_8010C46C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0", "-inline off"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_colsys_exact_8010C3FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/GScolsys2Util_exact_8010C77C.c"),
                    (Matching, "game/gs_colsys_obj_enable_exact_8010C7BC.c"),
                    (Matching, "game/gs_colsys_exact_8010C8D0.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_field_resource.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_render_util_exact_800D13C4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Carve of GScameraGetInvMatrixPtr/GScameraGetMatrixPtr with the cameraBuildMatrix
            # inline (repeated expansion; XD name); text only.
            Object(
                Matching,
                "game/gs_render_util_exact_800D1B3C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_render_util_exact_800D1EB8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_render_util_exact_800D1F58.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                    ]
                    + (
                        ["-DGS_RENDER_UTIL_SUFFIX_800D1B3C"]
                        if status is CodeCandidate
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_render_util_exact_800D207C.c"),
                    (Matching, "game/gs_render_util_exact_800D20CC.c"),
                    (Matching, "game/gs_render_util_candidate_800D2150.c"),
                    (Matching, "game/gs_render_util_exact_800D21C8.c"),
                    (Matching, "game/gs_render_util_exact_800D2248.c"),
                    (Matching, "game/gs_render_util_exact_800D2584.c"),
                    (Matching, "game/gs_render_util_candidate_800D258C.c"),
                    (Matching, "game/gs_render_util_exact_800D2738.c"),
                    (Matching, "game/gs_render_util_candidate_800D27FC.c"),
                    (Matching, "game/gs_render_util_exact_800D2B44.c"),
                ]
            ],
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + (
                        ["-opt nopeephole"]
                        if path
                        in (
                            "game/gs_event_exec_candidate_80014118.c",
                            "game/gs_event_exec_candidate_8001374C.c",
                            "game/gs_event_exec_r47_prefix.c",
                        )
                        else []
                    ),
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_event_exec_exact_80012D20.c"),
                    (Matching, "game/gs_event_exec_exact_80013744.c"),
                    (Matching, "game/gs_event_exec_candidate_8001374C.c"),
                    (Matching, "game/gs_event_exec_exact_800140FC.c"),
                    (Matching, "game/gs_event_exec_exact_80014110.c"),
                    (Matching, "game/gs_event_exec_candidate_80014118.c"),
                    (Matching, "game/gs_event_exec_exact_80014198.c"),
                    (Matching, "game/gs_event_exec_r47_prefix.c"),
                    (Matching, "game/gs_event_exec_r47_suffix.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_event_exec_r47_80014574_o3.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # menuFight prefix: fn_8000DAA8/fn_8000DAB0 and the timer window
            # functions (0x8000DC88-0x8000DE24), target button callback
            # (0x8000DFF0-0x8000E204), and secret-Pokemon draw callback
            # (0x8000EA10-0x8000ED34) link as data-free carves;
            # the remaining ranges use whole-TU candidate wrappers.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menuFight_exact_8000DAA8.c"),
                    (Matching, "game/menuFight_r51_8000DAA8_prefix.c"),
                    (Matching, "game/menuFight_exact_8000DC88.c"),
                    (Matching, "game/menuFight_exact_8000DE24.c"),
                    (Matching, "game/menuFight_r51_8000DE24.c"),
                    (Matching, "game/menuFight_exact_8000DFF0.c"),
                    (Matching, "game/menuFight_exact_8000E204.c"),
                    (Matching, "game/menuFight_r51_8000E290.c"),
                    (Matching, "game/menuFight_exact_8000EA10.c"),
                    (Matching, "game/menuFight_exact_8000F310.c"),
                    (Matching, "game/menuFight_exact_800100C0.c"),
                    (Matching, "game/menuFight_exact_80010294.c"),
                    (Matching, "game/menuFight_exact_80010588.c"),
                    (Matching, "game/menuFight_exact_80010C98.c"),
                ]
            ],
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuFight_candidate_8000ED34.c",
                constant_import=[
                    {"symbol": "lbl_8047B700", "offset": 0, "expected": "00000000"},
                    {"symbol": "lbl_8047B704", "offset": 4, "expected": "3f490fdb"},
                    {"symbol": "lbl_8047B708", "offset": 8, "expected": "4016cbe4"},
                    {"symbol": "lbl_8047B710", "offset": 16, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # menuFightDrawSecretWazaDoc: one-function carve, built without
            # copy propagation (unit flag instead of the old local pragma).
            Object(
                Matching,
                "game/menuFight_exact_8000F400.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole,nopropagation"],
                progress_category="game",
            ),
            # menuFightDrawSecretWazaSelect .. menuFightDrawBall: one carve so the
            # two adjacent switch tables (0x802E4CD8, 0x802E4D2C) come from one
            # object. TEMPORARY constant_import for the WazaTop literals.
            Object(
                Matching,
                "game/menuFight_exact_8000F768.c",
                constant_import=[{"symbol": "lbl_8047B700", "offset": 0, "expected": "00000000"},
                                 {"symbol": "lbl_8047B704", "offset": 4, "expected": "3f490fdb"},
                                 {"symbol": "lbl_8047B708", "offset": 8, "expected": "4016cbe4"},
                                 {"symbol": "lbl_8047B710", "offset": 16, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuFight_r51_80011288_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuFight_r51_800114A4_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuFightStatus.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_range_80069C0C.c",
                mw_version="GC/1.3.2",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # menuCB_Bios.c part of menu_middle (0x8006A65C - 0x8006B6B4: the
            # functions using its rodata, lbl_80267DD8 / the "menuCB_Bios.c"
            # asserts at 0x80267DE8 - 0x80267EA3): GC/1.3, -O4,p with the
            # peephole pass off; see menu_middle_range_8006AF44.c for the
            # evidence.
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-i src/game/menu",
                        "-opt nopeephole",
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menu/menu_middle_exact_8006A65C.c"),
                    (Matching, "game/menu/menu_middle_exact_8006A824.c"),
                    (Matching, "game/menu/menu_middle_range_8006A990.c"),
                    (Matching, "game/menu/menu_middle_exact_8006AC28.c"),
                    (Matching, "game/menu/menu_middle_range_8006ACCC.c"),
                    (Matching, "game/menu/menu_middle_exact_8006ADB4.c"),
                    (Matching, "game/menu/menu_middle_range_8006AE18.c"),
                    (Matching, "game/menu/menu_middle_exact_8006AEEC.c"),
                    (Matching, "game/menu/menu_middle_range_8006AF44.c"),
                    (Matching, "game/menu/menu_middle_exact_8006AFC4.c"),
                    (Matching, "game/menu/menu_middle_range_8006AFE4.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B09C.c"),
                    (Matching, "game/menu/menu_middle_range_8006B154.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B1C0.c"),
                    (Matching, "game/menu/menu_middle_range_8006B2A4.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B354.c"),
                    (Matching, "game/menu/menu_middle_range_8006B420.c"),
                    (Matching, "game/menu/menu_middle_exact_8006B4AC.c"),
                    (Matching, "game/menu/menu_middle_r48_8006B5D0_prefix.c"),
                ]
            ],
            Object(
                Matching,
                "game/menu/menu_middle_exact_8006B6B4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_8006B8E8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_range_8006B9B8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_range_8006B9B8_r41_8006BB34.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_candidate_8006DC28.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            # menuBattleModeRefreshRules addresses the menuCB_Hook pool off one base, like
            # fn_8006E338; the unit defines the pool and redirects it.
            Object(
                Matching,
                "game/menu/menu_middle_r59_8006E798_middle.c",
                mw_version="GC/2.0",
                # TEMPORARY constant_import: the unit's menuCB_Hook pool tables.
                constant_import=[MENU_CB_HOOK_POOL_IMPORT],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    (
                        Matching
                        if path in (
                            "game/menu/menu_middle_r59_8006FBFC_suffix.c",
                            "game/menu/menu_middle_r59_8006C5D8_suffix.c",
                            "game/menu/menu_middle_r59_8006E9A4_o1.c",
                            "game/menu/menu_middle_r59_8006D940_suffix.c",
                            "game/menu/menu_middle_r59_8006CCC0_o1.c",
                            "game/menu/menu_middle_r59_8006F720_o1.c",
                            "game/menu/menu_middle_r59_8006C164_o1.c",
                            "game/menu/menu_middle_r59_8006F284_prefix.c",
                        )
                        else CodeCandidate
                    ),
                    path,
                    mw_version="GC/1.3",
                    jump_table_import=(
                        {
                            "symbol": "jumptable_802EE06C",
                            "function": "fn_8006F720",
                            "owner": "game/menu/menu_middle_r59_8006CCC0_o1.c",
                            "owner_offset": 0xBC,
                        }
                        if path == "game/menu/menu_middle_r59_8006F720_o1.c"
                        else None
                    ),
                    extra_cflags=[
                        "-opt nopeephole",
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        "-i src/game/menu",
                    ],
                    progress_category="game",
                )
                for path in [
                    "game/menu/menu_middle_r59_8006C5D8_suffix.c",
                    "game/menu/menu_middle_r59_8006FBFC_suffix.c",
                    "game/menu/menu_middle_r59_8006D940_suffix.c",
                    "game/menu/menu_middle_r59_8006CCC0_o1.c",
                    "game/menu/menu_middle_r59_8006E9A4_o1.c",
                    "game/menu/menu_middle_r59_8006F720_o1.c",
                    "game/menu/menu_middle_r59_8006C164_o1.c",
                    "game/menu/menu_middle_r59_8006F284_prefix.c",
                ]
            ],
            # fn_8006E338 is GC/2.0 code that addresses menuCB_Hook.c's
            # .rodata pool off one base; the unit defines that pool and
            # TEMPORARY constant_import redirects it to retail lbl_80267EA8.
            Object(
                Matching,
                "game/menu/menu_middle_r59_8006E338_o1.c",
                # CI-verified source symbol extents and relocation signatures.
                comparison_view="data_names",
                symbol_mappings={
                    'lbl_8047E708': '@19',
                },
                mw_version="GC/2.0",
                constant_import=[
                    MENU_CB_HOOK_POOL_IMPORT,
                ],
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_r50_8006EE7C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_r50_8006EFF8_gc20_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_candidate_8006C7D4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_8006FCF8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_range_8006FEE4.c",
                mw_version="GC/2.0",
                # TEMPORARY constant_import: the unit's menuCB_Hook pool tables
                # and its float literals.
                constant_import=[
                    MENU_CB_HOOK_POOL_IMPORT,
                    {"symbol": "lbl_8047C078", "offset": 0, "expected": "3c8efa35"},
                    {"symbol": "lbl_8047C080", "offset": 8, "expected": "401921fb60000000"},
                    {"symbol": "lbl_8047C088", "offset": 16, "expected": "00000000"},
                    {"symbol": "lbl_8047C08C", "offset": 20, "expected": "40c90fdb"},
                ],
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_80070274.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_range_80070318.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_exact_800704A4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_r47_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_middle_r47_80070D84_o2.c",
                mw_version="GC/1.3",
                cflags=["-O3" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=[
                    "-schedule on",
                    "-opt nopeephole",
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-i src/game/menu",
                ],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_thread_exact_800F0030.c"),
                    (Matching, "game/gs_thread_exact_800F036C.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_thread_exact_800F0424.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_thread_exact_800F106C.c",
                mw_version="GC/1.3",
                extra_cflags=["-inline noauto", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # GS VM native-call opcodes (0x800F10E8-0x800F1A0C), carved from
            # the old cms_CallStdTrampoline candidate (cms_CallStdTrampoline itself is first-party
            # assembly in gs_thread_exact_800F106C.c). .text-only units. The GS VM TU is
            # GC/1.3.2 -O4,p: its pooled-string handlers (cms_op34_orl,
            # cms_op33_andl, cms_ScriptCreateThread) address the pool's first message as
            # "addi rX,r31,0", which GC/1.3 emits as "mr rX,r31".
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3.2",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_vm_exact_800F10E8.c"),
                    (Matching, "game/gs_vm_exact_800F13D0.c"),
                    (Matching, "game/gs_vm_exact_800F16C0.c"),
                ]
            ],
            # The GS VM translation unit (see include/game/gs_vm.h), linked
            # over 0x800F1A0C-0x800F75FC with the string pool and .sdata2
            # literals it owns. The .text-only units before and after it are
            # linked pieces of the same TU. -rostr puts the pool in .rodata.
            Object(
                Matching,
                "game/gs_vm.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_res.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # GSmsg (0x800F96E4-0x800FE35C) is one retail translation unit. It
            # builds with -O4,p and the peephole optimizer off: with -opt
            # nopeephole every one of its functions scores equal or higher, and
            # GSmsgInit, GScharCpy, GScharLenCpy, GScharCmp, GSmsgDaemon,
            # GSmsgFontOpen and winMsgDrawMessage only become exact with it.
            # GSmsg_800F9D04.c links 0x800F9D04-0x800FE35C with the TU's .bss
            # and .sdata2 pool. It needs GC/1.3.2: the TU's buffers are pooled
            # file statics, GC/1.3 folds their object offsets into store
            # displacements, and GC/1.3.2 keeps them out as retail does
            # (GSmsgPrintf's fill pointer; the rest of the TU is identical under
            # the two versions). GSmsgGetText links by itself, as do GScharToGBA
            # and GScharFromGBA together.
            Object(
                Matching,
                "game/gs_msg_exact_800F96E4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_msg_r56b_800F9AEC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/GSmsg_800F9D04.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Sprite screen-environment TU (.text 0x800FE35C-0x800FE6DC with
            # its .sdata2 pool 0x8047CD50-0x8047CD80), linked whole; see the
            # file header for the unit-wide -opt nopeephole evidence.
            Object(
                Matching,
                "game/gs_thread_hi_range_800FE35C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gapp.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_thread_hi_range_800FEC34.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_80006630.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_residual_80006724.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_80006884.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_residual_80006908.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_80006FAC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_residual_80007364.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_exact_8000765C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_task_residual_80008868.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # people.c, the whole people TU: .text 0x801812C4-0x8018F470 with
            # its .rodata, .data, .sbss and .sdata2 (see the file header for
            # the evidence of its one flag set: GC/2.0 -O4,p, deferred
            # auto-inlining and read-only strings). Every function is exact;
            # the TU links as one object.
            Object(
                Matching,
                "game/people/people.c",
                mw_version="GC/2.0",
                extra_cflags=PEOPLE_TU_CFLAGS,
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"]
                    + {
                        "game/gs_npc_event_candidate_8003037C_r40_8003042C_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80030574_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_800307A8_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_800308D4.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80030A44_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80030C14.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80030D34_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80030F0C_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80031228_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80031404_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_80031648_gc20.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_8003037C_r40_800318D8.c": ["-opt nopeephole"],
                        "game/gs_npc_event_candidate_800324A0.c": ["-opt nopeephole"],
                    }.get(path, []),
                    progress_category="game",
                )
                for status, path, version in [
                    (Matching, "game/gs_npc_event_exact_80030170.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_800301B0.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_exact_80030370.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_8003042C_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80030574_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_800307A8_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_800308D4.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80030A44_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80030C14.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80030D34_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80030F0C_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80031188.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80031228_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80031404_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_80031648_gc20.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_8003037C_r40_800318D8.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_800324A0.c", "GC/1.3"),
                    (Matching, "game/gs_npc_event_candidate_80032ED8.c", "GC/1.3"),
                ]
            ],
            # gbaTradeRun only (GS_NPC_EVENT_ONLY_80031B4C carve); its switch
            # table jumptable_802E4F90 stays owned by data_802E4DB0_prefix.
            Object(
                Matching,
                "game/gs_npc_event_candidate_80031B4C_o3.c",
                jump_table_import={
                    "symbol": "jumptable_802E4F90",
                    "function": "gbaTradeRun",
                    "owner": "game/data/data_802E4DB0_prefix.c",
                    "owner_offset": 0x1E0,
                },
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # cardeBattleSelectCard only (GS_NPC_EVENT_ONLY_800327FC carve); its float
            # literals are redirected to the retail-owned .sdata2 labels.
            Object(
                Matching,
                "game/gs_npc_event_candidate_800327FC_o4s.c",
                constant_import=[
                    {"symbol": "lbl_8047BA00", "offset": 0x00, "expected": "3e99999a"},
                    {"symbol": "lbl_8047B9F8", "offset": 0x04, "expected": "00000000"},
                    {"symbol": "lbl_8047BA18", "offset": 0x08, "expected": "3f800000"},
                    {"symbol": "lbl_8047BA08", "offset": 0x10, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047BA10", "offset": 0x18, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=version,
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path, version in [
                    (Matching, "game/gs_pokemon_summary_exact_800178EC.c", "GC/1.3"),
                ]
            ],
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_pokemon_summary_r41_800167D0_gc20.c",
                # menuPocketDrawMarqueeDot only (GS_POKEMON_SUMMARY_800167D0_ONLY island);
                # its float literals are the TU's shared .sdata2 (sdata2_8047B740).
                constant_import=[
                    {"symbol": "lbl_8047B75C", "offset": 0x00, "expected": "40490fdb"},
                    {"symbol": "lbl_8047B770", "offset": 0x04, "expected": "3f000000"},
                    {"symbol": "lbl_8047B774", "offset": 0x08, "expected": "40000000"},
                    {"symbol": "lbl_8047B748", "offset": 0x0C, "expected": "00000000"},
                    {"symbol": "lbl_8047B744", "offset": 0x10, "expected": "3f800000"},
                    {"symbol": "lbl_8047B778", "offset": 0x14, "expected": "3fc90fdb"},
                    {"symbol": "lbl_8047B768", "offset": 0x18, "expected": "4330000080000000"},
                ],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_pokemon_summary_r41_80016ABC.c",
                # menuPocketDrawMarqueeFrame's float literals, in the order MWCC pools them;
                # retail shares each with the TU's .sdata2 (sdata2_8047B740).
                constant_import=[
                    {"symbol": "lbl_8047B774", "offset": 0x00, "expected": "40000000"},
                    {"symbol": "lbl_8047B75C", "offset": 0x04, "expected": "40490fdb"},
                    {"symbol": "lbl_8047B770", "offset": 0x08, "expected": "3f000000"},
                    {"symbol": "lbl_8047B740", "offset": 0x0C, "expected": "437f0000"},
                    {"symbol": "lbl_8047B77C", "offset": 0x10, "expected": "42340000"},
                    {"symbol": "lbl_8047B744", "offset": 0x14, "expected": "3f800000"},
                    {"symbol": "lbl_8047B780", "offset": 0x18, "expected": "3e800000"},
                    {"symbol": "lbl_8047B784", "offset": 0x1C, "expected": "3f400000"},
                    {"symbol": "lbl_8047B768", "offset": 0x20, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-fp_contract off"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_8001501C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_800150E4_o4s.c",
                # fn_800150E4 only (GS_POKEMON_SUMMARY_800150E4_ONLY island);
                # the unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_80015374_o4s.c",
                # menuPocketDrawNameList only (GS_POKEMON_SUMMARY_80015374_ONLY island);
                # the unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_800155B0_o4s.c",
                # fn_800155B0 only (GS_POKEMON_SUMMARY_800155B0_ONLY island);
                # the unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_800159BC_middle.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_80015E3C_o4s.c",
                # menuPocketDrawItemList only (GS_POKEMON_SUMMARY_80015E3C_ONLY island);
                # the unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r57b_800161B0_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_pokemon_summary_r55_800166BC_gc13_o3.c",
                constant_import=[{"symbol": "lbl_8047B758", "offset": 0, "expected": "40c90fdb"},
                                 {"symbol": "lbl_8047B75C", "offset": 4, "expected": "40490fdb"},
                                 {"symbol": "lbl_8047B760", "offset": 8, "expected": "c0490fdb"},
                                 {"symbol": "lbl_8047B768", "offset": 16, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r55_8001793C_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r55_80017CB8_gc13_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-O4,s", "-opt nopeephole", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_pokemon_summary_r55_80017E8C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/main.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/main_tail.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-DGS_TEXTURE_800EF098_ONLY"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_exact_800EF1E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_getters_exact_800EF3E0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_texture_exact_800EF504.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_texture_exact_800EF548.c"),
                    (Matching, "game/gs_texture_exact_800EF578.c"),
                    (Matching, "game/gs_texture_exact_800EF5A4.c"),
                    (Matching, "game/gs_texture_candidate_800EF5FC.c"),
                    (Matching, "game/gs_texture_exact_800EFD14.c"),
                    (Matching, "game/gs_texture_exact_800EFFC0.c"),
                ]
            ],
            Object(
                Matching,
                "game/input/input_exact_800F75FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/input/input_candidate_800F760C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/input/input_exact_800F76E4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # One retail unit, 0x800F7758-0x800F915C. GC/1.3.2 because retail
            # keeps the pooled .bss `base + 0` add in GSinputInit/inputUpdateMotors
            # (GC/1.3 folds it into a register copy); -inline deferred because
            # retail's pooled .bss layout (slots, timers, commands) is reverse
            # definition order, not first-reference order. Every function of
            # the unit is exact with this single setting and no pragmas;
            # deferred inlining emits functions in reverse, so the source
            # lists them high address first.
            Object(
                Matching,
                "game/input/input.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline deferred"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_gfx_exact_800D377C.c"),
                ]
            ],
            # GS render/graphics span 0x800D2B90-0x800D377C (fog setters,
            # camera projections, GSgfx frame functions) with its .sdata2
            # literal pool 0x8047C9F0-0x8047CA10. GC/2.5: fn_800D36B4
            # schedules its by-value colour loads ahead of the frame, which
            # GC/1.0-2.0 do not emit. See the file header.
            Object(
                Matching,
                "game/gs_gfx_range_800D2B90.c",
                mw_version="GC/2.5",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_candidate_800D37D4.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),  # GC/1.3.2: pooled static render-mode tables (see source header)
            # Whole DObj owner, including its own literal pools and state.
            # The existing DObjLoad parameter-copy exception remains documented.
            Object(
                Matching,
                "hsd/dobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/fobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # sysdolphin library flags
            # Whole WObj owner; archive functionality has its own source below.
            Object(
                Matching,
                "hsd/wobj.c",
                mw_version="GC/1.3",
                extra_cflags=["-O1", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # Retain archive's proven GC/1.3 codegen for the first whole-owner build.
            Object(
                Matching,
                "hsd/archive.c",
                mw_version="GC/1.3",
                extra_cflags=["-O1", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin fog.c, built with the library flags; owns its
            # .rodata/.data/.sdata2 (its zero colour stays in dtk's .sbss2).
            Object(
                Matching,
                "hsd/fog.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/id.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin initialize.c, built with the library flags; it
            # owns its .rodata/.data/.bss/.sdata/.sbss/.sdata2/.sbss2.
            Object(
                Matching,
                "hsd/initialize.c",
                # CI-verified source symbol extents and relocation signatures.
                comparison_view="data_names",
                symbol_mappings={
                    'lbl_8047DAB0': '@150',
                    'lbl_8047DAB8': '@151',
                    'lbl_8047DAC0': '@152',
                    'lbl_8047DAC4': '@153',
                    'lbl_8047DACC': '@154',
                    'lbl_8047DAD4': '@155',
                    'lbl_8047DAD8': '@156',
                    'lbl_8047DADC': '@157',
                    'lbl_8047DAE4': '@158',
                    'lbl_8047DAEC': '@159',
                    'lbl_8047DAF4': '@160',
                    'lbl_8047DAFC': '@161',
                    'lbl_8047DB04': '@162',
                    'lbl_8047DB0C': '@296',
                    'lbl_8047DB14': '@316',
                    'lbl_8047DB18': '@317',
                },
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/hsd_object.c",
                mw_version="GC/1.3",
                progress_category="hsd",
            ),
            Object(
                Matching,
                "dolphin/os/OSMemory_exact_8009F1B8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSMemory_privileged.c",
                mw_version="GC/1.2.5n",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "game/gs_gfx_core.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # GSgfxDrawScene is its own TU (its .sdata2 literal pool
            # 0x8047CA20-0x8047CA30 sits between the 0x800D2DE8-0x800D3E4C
            # pool and gfxSetPointSize's), built without copy propagation:
            # retail re-tests bit 0 of the second-pass flags after masking it.
            # The possible TU neighbours gs_gfx_core, 45F8 and 4F98 are
            # byte-identical under this flag.
            # RULE-EXCEPTION(title-path): per-unit compiler flag chosen because it matches - see docs/RULE_EXCEPTIONS.md
            Object(
                Matching,
                "game/gs_gfx_candidate_800D3FA4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D45F8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # gfxPlayCommands owns its compiler-emitted switch table (.data
            # 0x80314188, formerly game/data/data_80314188.c).
            Object(
                Matching,
                "game/gs_gfx_candidate_800D461C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D4F98.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),  # GC/1.3.2 + -rostr: pooled .rodata request-buffer messages
            # gfxSetPointSize/gfxSetLineWidth own their literal pool (.sdata2
            # 0x8047CA30-0x8047CA40: 0.0f, 42.5f, 6.0f).
            Object(
                Matching,
                "game/gs_gfx_candidate_800D55D0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D56C0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of gfxBegin (no jump tables or pooled
            # constants; data extern), on the neighbouring gs_gfx flags.
            Object(
                Matching,
                "game/gs_gfx_candidate_800D67BC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D6A00.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # gfxFlushPendingVertex: GC/1.3.2 with its pooled save-slot .bss defined
            # in the unit (retail passes the first slot as "addi r3,r31,0x0");
            # -inline deferred gives retail's .bss order (reverse definition
            # order, as in pslist.c). See the source header.
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D6B00.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline deferred"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_gfx_layer_exact_800D7230.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D76A8.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7820.c"),
                    (Matching, "game/gs_gfx_layer_candidate_800D7894.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7940.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7A70.c"),
                    (Matching, "game/gs_gfx_layer_candidate_800D7D90.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D7E5C.c"),
                    (Matching, "game/gs_gfx_layer_candidate_800D85D4.c"),
                    (Matching, "game/gs_gfx_layer_exact_800D87AC.c"),
                ]
            ],
            # Byte-exact with tagged rule exceptions (docs/RULE_EXCEPTIONS.md).
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D892C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of _gfxFlushTevState (compare-tree switch, no
            # jump tables or pooled constants; data extern), plain gs_gfx flags.
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D923C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # Function-boundary carve of gfxSetTevOp (compare-tree switch, no
            # jump tables or pooled constants; data extern), plain gs_gfx flags.
            Object(
                Matching,
                "game/gs_gfx_layer_candidate_800D963C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_range_800D9AF0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_exact_800D9D68.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # The gs_gfx dl TU (0x800DA578 - 0x800DB890) linked whole with its
            # four switch tables (.data 0x803152B8 - 0x80315384): the last
            # table starts 4-aligned, so compiled tables cannot be carved.
            # _dlParseVertex uses opt_lifetimes off and GSgfxDLBegin a
            # single-use helper. RULE-EXCEPTION(title-path): see
            # docs/RULE_EXCEPTIONS.md.
            Object(
                Matching,
                "game/gs_gfx_dl.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_range_800DB890.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_gfx_backfb.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_light_exact_800DC560.c"),
                    (Matching, "game/gs_light_candidate_800DC6D8.c"),
                    (Matching, "game/gs_light_exact_800DC874.c"),
                    (Matching, "game/gs_light_candidate_800DC878.c"),
                    (Matching, "game/gs_light_exact_800DCA10.c"),
                    (Matching, "game/gs_light_exact_800DCC3C.c"),
                    (Matching, "game/gs_light_candidate_800DCC84.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_log.cpp",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_material_800DEFC8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_material.c"),
                    (Matching, "game/gs_material_exact_800DF11C.c"),
                ]
            ],
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/gs_material_candidate_800DF140.c",
                constant_import=[{"symbol": "lbl_8047CACC", "offset": 0, "expected": "437f0000"},
                                 {"symbol": "lbl_8047CAD0", "offset": 8, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_material_exact_800DF188.c"),
                    (Matching, "game/gs_material_exact_800DF1B8.c"),
                    (Matching, "game/gs_material_exact_800DF21C.c"),
                    (Matching, "game/gs_material_exact_800DF240.c"),
                    (Matching, "game/gs_material_candidate_800DF248.c"),
                    (Matching, "game/gs_material_exact_800DF470.c"),
                    (Matching, "game/gs_material_exact_800DF498.c"),
                    (Matching, "game/gs_material_candidate_800DFABC.c"),
                    (Matching, "game/gs_material_exact_800DFE98.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_math_vec.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_mtx.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_mtx44.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_quat.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_init.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_bezier.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_lerp.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_range_800E09E8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_exact_800E0BA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_candidate_800E0C04.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_exact_800E0C54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_random_exact_800E0CA0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_math_util_exact_800E0D24.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_range_800E0DDC_r58_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            # GSmemDumpMap's GSmem check messages are pooled .rodata literals
            # addressed off one base: GC/1.3.2 + -rostr. Retail shares that
            # pool as lbl_80270658 (0x55F bytes + 1 pad; gs_mem.c also
            # addresses it) and the 100.0f / unsigned-int bias as
            # lbl_8047CB48 / lbl_8047CB50 (sdata2_8047CB48.c).
            Object(
                Matching,
                "game/gs_range_800E0E14_r58_o1.c",
                constant_import=[
                    {
                        "section": ".rodata",
                        "symbol": "lbl_80270658",
                        "offset": 0,
                        "expected": (
                        "2d2d5b2047536d656d20737461746520636865636b205d2d2d2d2d2d2d2d2d2d"
                        "2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d0a000021212120666972737420626c"
                        "6f636b27732070726576696f757320706f696e746572206973206e6f74204e55"
                        "4c4c0a00212121204672656520626c6f636b206973206f757473696465206f66"
                        "2047536d656d20617265610a00000000212121204672656520626c6f636b2065"
                        "786973747320696e206d656d6f727920656e747279207461626c650a00000000"
                        "212121204672656520626c6f636b20696e636c7564657320737061636520696e"
                        "206d656d6f727920656e747279207461626c650a000000002121212046726565"
                        "20626c6f636b206c69737420706f696e7465727320696e636f6e73697374656e"
                        "740a0000212121204672656520626c6f636b732068617665206e6f7420626565"
                        "6e206d657267656420636f72726563746c790a00212121204672656520626c6f"
                        "636b206c69737420706f696e74657273206e6f74206c696e6561720a00000000"
                        "21212120426c6f636b2065786973747320696e20616e20696e76616c6964206d"
                        "656d6f727920617265610a002121212050616464696e67206279746573206f76"
                        "65727772697474656e20696e2068616e646c652025640a0021212120426c6f63"
                        "6b206d6f64696669656420776974686f7574206c6f636b20696e2068616e646c"
                        "652025640a0000004d656d6f7279206d61702064756d703a0a00000020202530"
                        "385868202d3e2025303858683a20616c6c6f636174656420626c6f636b202873"
                        "697a653d25642c2068616e646c653d25642c206c6f636b733d25642c20616c69"
                        "676e3d2564290a0020202530385868202d3e2025303858683a20667265652062"
                        "6c6f636b202873697a653d25642c20707265763d25303858682c206e6578743d"
                        "2530385868290a0021212120706f696e74657220617420253038586820646f65"
                        "73206e6f742061707065617220746f20626520612076616c6964206672656520"
                        "626c6f636b0a000021212120706f696e74657220617420253038586820686173"
                        "20696e76616c69642073697a652025303858680a0000000021212120456e6420"
                        "6f66206d656d6f7279206d617020646f6573206e6f74206c696e652075702077"
                        "697468206c61737420626c6f636b21205b6c6173743a2530385820213d20656e"
                        "643a253038585d0a00000000212121205573656420626c6f636b20636f756e74"
                        "20646f6573206e6f74206d6174636820696e7465726e616c20636f756e746572"
                        "0a00000047536d656d206d656d6f727920617265613a2020202530385868202d"
                        "3e2025303858680a0000000041637469766520616c6c6f636174696f6e733a20"
                        "2025640a00000000416374697665206c6f636b733a202020202020202025640a"
                        "00000000456e747279207461626c652073697a653a2020202025642062797465"
                        "730a00004c61726765737420667265652073706163653a202025642062797465"
                        "730a0000546f74616c20667265652073706163653a2020202025642062797465"
                        "730a0000546f74616c206672656520626c6f636b733a20202025640a00000000"
                        "546f74616c20616c6c6f636174696f6e733a20202025640a00000000546f7461"
                        "6c206c6f636b733a20202020202020202025640a00000000546f74616c20756e"
                        "6c6f636b733a2020202020202025640a00000000546f74616c2066726565733a"
                        "20202020202020202025640a00000000546f74616c20656e7472792072657369"
                        "7a65733a2025640a00000000467261676d656e746174696f6e3a202020202020"
                        "20252e316625250a000000002d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d"
                        "2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d2d0a00"
                        ),
                    },
                    {"symbol": "lbl_8047CB48", "offset": 0, "expected": "42c80000"},
                    {"symbol": "lbl_8047CB50", "offset": 8, "expected": "4330000000000000"},
                ],
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-rostr"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_r56_80026370_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80026478.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_800265C0.c",
                mw_version="GC/1.3",
                cflags=["-O4" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80026640.c",
                mw_version="GC/1.3",
                cflags=["-O4" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80026740.c",
                mw_version="GC/1.3",
                cflags=["-O4" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_r56_800268F0_gc20p1_o4s.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_candidate_80026B44_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuNameEntry_candidate_80026FEC.c",
                constant_import=[{"symbol": "lbl_8047B948", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole,nopropagation,nocommonsubs"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuNameEntry_exact_8002749C.c",
                constant_import=[{"symbol": "lbl_8047B948", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole,nocommonsubs"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_candidate_800275F4_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_r51_80027740_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_r51_80027AA4_o2.c",
                mw_version="GC/1.3",
                cflags=["-O2,p" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-O2", "-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80027D58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-opt nostrength"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_800280FC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_800281F4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80028444.c",
                mw_version="GC/1.3",
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80028588.c",
                mw_version="GC/1.3",
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80028620.c",
                mw_version="GC/1.3",
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80028728.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80028830.c",
                mw_version="GC/1.3",
                constant_export={"symbol": "lbl_8047B948", "offset": 0,
                                 "expected": "4330000080000000"},
                extra_cflags=["-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_candidate_80028830.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80028FBC.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuNameEntry_exact_80029558.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # 0x80029850-0x80029FAC: exact menuShop.c islands and carves.
            Object(
                Matching,
                "game/menuShop_island_80029850.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_80029AC8.c",
                # fn_80029AC8 only (MENUSHOP_EXACT_80029AC8_ONLY carve); the
                # unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_80029CC0.c",
                # fn_80029CC0 only (MENUSHOP_EXACT_80029CC0_ONLY carve); the
                # unit-level -opt nopeephole replaces the file's local pragma.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_island_80029EF4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_80029FAC.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_8002A3D4.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_8002A5B0.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-schedule on", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8",
                    "-opt nopeephole",
                ],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_candidate_8002A5B0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/menuShop_exact_8002AA68.c"),
                    (Matching, "game/menuShop_candidate_8002AB00.c"),
                    (Matching, "game/menuShop_exact_8002AB40.c"),
                    (Matching, "game/menuShop_candidate_8002AE9C.c"),
                    (Matching, "game/menuShop_exact_8002B03C.c"),
                ]
            ],
            Object(
                Matching,
                "game/menuShop_candidate_8002B1A0.c",
                # menuShopDrawListText only (MENUSHOP_CANDIDATE_8002B1A0_ONLY
                # carve); the unit-level -opt nopeephole replaces the file's
                # local pragma, and -opt nopropagation keeps the function's
                # single-use locals as their own registers, as retail does.
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole", "-opt nopropagation"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_exact_8002B40C.c",
                constant_import=[
                    {"symbol": "lbl_8047B988", "offset": 0, "expected": "40c90fdb"},
                    {"symbol": "lbl_8047B98C", "offset": 4, "expected": "40490fdb"},
                    {"symbol": "lbl_8047B990", "offset": 8, "expected": "c0490fdb"},
                    {"symbol": "lbl_8047B998", "offset": 16, "expected": "4330000080000000"},
                ],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuShop_candidate_8002B594.c",
                constant_import=[
                    {"symbol": "lbl_8047B998", "offset": 0, "expected": "4330000080000000"},
                ],
                # menuShopDrawMarqueeDot only (MENUSHOP_CANDIDATE_8002B594_ONLY carve). GC/2.0:
                # retail's `li r0,-0x100; or` colour merge is what 2.0+ emits for
                # `(u8)x | -0x100`; GC/1.3 folds it to oris/ori. Same compiler as its
                # twin menuPocketDrawMarqueeDot in gs_pokemon_summary.
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonChange_r51_8002DD24_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonChange_island_8002DF10.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonChange_r51_8002DF10_suffix.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-lang=c++", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonChange_r41_8002E460_gc125n.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuPokemonChange_r41_8002EA5C.c",
                constant_import=[{"symbol": "lbl_8047B9D0", "offset": 0, "expected": "3e99999a"},
                                 {"symbol": "lbl_8047B9E0", "offset": 8, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047B9E8", "offset": 16, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuPokemonChange_island_8002EE74.c",
                constant_import=[{"symbol": "lbl_8047B9E0", "offset": 0, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047B9E8", "offset": 8, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonChange_exact_8002F284.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuPokemonChange_island_8002F79C.c",
                constant_import=[{"symbol": "lbl_8047B9E0", "offset": 0, "expected": "4330000080000000"},
                                 {"symbol": "lbl_8047B9E8", "offset": 8, "expected": "4330000000000000"}],
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # fn_8002FC58 only (MENU_POKEMON_CHANGE_EXACT_8002FC58_ONLY carve).
            Object(
                Matching,
                "game/menuPokemonChange_candidate_8002FC58.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_title_candidate_800205C8.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt nopeephole",
                    "-O1",
                ],
                progress_category="game",
            ),
            # gs_title 0x80022E54-0x80023DA8 with its .sdata2 literal
            # 0x8047B8A4, compiled from gs_title.c.
            Object(
                Matching,
                "game/gs_title_80022E54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # gs_title 0x80020F54-0x800215C4 with its own .sdata2 pool
            # (0x8047B898-0x8047B8A0), compiled from gs_title.c.
            Object(
                Matching,
                "game/gs_title_80020F54.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # gs_title 0x80020618-0x80020E9C with its own .sdata2 pool
            # (0x8047B868-0x8047B898), compiled from gs_title.c.
            Object(
                Matching,
                "game/gs_title_80020618.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version=(
                        "GC/2.0"
                        if path == "game/gs_title_candidate_80020EA4.c"
                        else "GC/1.3"
                    ),
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_title_exact_8002058C.c"),
                    (Matching, "game/gs_title_exact_800205B8.c"),
                    (Matching, "game/gs_title_exact_8002060C.c"),
                    (Matching, "game/gs_title_exact_80020E9C.c"),
                    (Matching, "game/gs_title_candidate_80020EA4.c"),
                    (Matching, "game/gs_title_exact_800215C4.c"),
                    (Matching, "game/gs_title_exact_80021624.c"),
                    (Matching, "game/gs_title_exact_80021644.c"),
                    (Matching, "game/gs_title_exact_800216E0.c"),
                    (Matching, "game/gs_title_r51_800218BC_suffix.c"),
                    (Matching, "game/gs_title_exact_80021A9C.c"),
                    (Matching, "game/gs_title_candidate_80021B14.c"),
                    (Matching, "game/gs_title_exact_80022050.c"),
                ]
            ],
            Object(
                Matching,
                "game/gs_title_r51_800216E8_o4s.c",
                mw_version="GC/1.3",
                cflags=["-O4,s" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # gs_title 0x80023DA8-0x80026370 with its own .sdata2 pool
            # (0x8047B8A8-0x8047B920), compiled from gs_title.c.
            Object(
                Matching,
                "game/gs_title_80023DA8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_title_candidate_8002217C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_title_candidate_80022478.c",
                jump_table_import={
                    "symbol": "jumptable_802E4F00",
                    "function": "itemFieldUseParamItem",
                    "owner": "game/data/data_802E4DB0_prefix.c",
                    "owner_offset": 0x150,
                },
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_title_exact_80022720.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_title_candidate_80022834.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_xfb_capture.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_spline_exact_80176068.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_spline.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O2,p"],
                progress_category="game",
            ),
            # The camera TU's text up to cameraInit (0x801765F4-0x80179DFC)
            # with its .rodata initialiser image, .data and whole .sdata2
            # pool, compiled from the whole-TU source with the camera unit's
            # flags (see camera.c's header and camera_801765F4.c).
            Object(
                Matching,
                "game/camera_801765F4.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline auto,deferred", "-str reuse,readonly"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/camera_exact_80179DFC.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline auto,deferred", "-str reuse,readonly"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/camera_candidate_80179E04.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-inline auto,deferred", "-str reuse,readonly"],
                progress_category="game",
            ),
            # fn_80179F4C: level-0 code (parameter homed on the stack), exact
            # with the unit-wide `-opt level=0` and no local pragmas; carved
            # from the fsysInitTOC candidate below.
            Object(
                Matching,
                "game/gs_range_80179F4C_exact_80179F4C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # fsysInitTOC: level-0 like the rest of fsys, and built with GC/2.0
            # like the fsys read units after it (gs_range_8017A5FC_prefix
            # onward): GC/1.3 colours its DMA-setup temporaries differently,
            # while GC/1.3.2 through 2.7 all emit the retail bytes.
            Object(
                Matching,
                "game/gs_range_80179F4C.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=[
                        "-use_lmw_stmw on",
                        "-sdata 8",
                        "-sdata2 8",
                        *(["-opt level=0"] if path in (
                            "game/fsys/fsys_file.c",
                            "game/fsys/fsys_slot_8017B1CC.c",
                            "game/fsys/fsys_file_r48_8017B4BC_prefix.c",
                            "game/fsys/fsys_file_exact_8017B5C0.c",
                            "game/fsys/fsys_file_r48_8017B6B8_suffix.c",
                            "game/fsys/fsys_file_exact_8017BC90.c",
                            "game/fsys/fsys_file_r51_8017C5B8_prefix.c",
                            "game/fsys/fsys_file_candidate_8017C008.c",
                            "game/fsys/fsys_file_candidate_8017C074.c",
                            "game/fsys/fsys_file_candidate_8017C39C.c",
                            "game/fsys/fsys_file_exact_8017C1D8.c",
                            "game/fsys/fsys_file_candidate_8017C414.c",
                            "game/fsys/fsys_file_candidate_8017C894.c",
                            "game/fsys/fsys_file_candidate_8017C8C8.c",
                            "game/fsys/fsys_file_candidate_8017D3D4.c",
                        ) else []),
                    ],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/fsys/fsys_file.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017B1AC.c"),
                    (Matching, "game/fsys/fsys_slot_8017B1CC.c"),
                    (Matching, "game/fsys/fsys_file_r48_8017B4BC_prefix.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017B5C0.c"),
                    (Matching, "game/fsys/fsys_file_r48_8017B6B8_suffix.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017BC90.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017BFE8.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C008.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C074.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C1D8.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C394.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C39C.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C414.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C568.c"),
                    (Matching, "game/fsys/fsys_file_r51_8017C5B8_prefix.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C88C.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C894.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C8C0.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017C8C8.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017C8F4.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017CEC8.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017D3A0.c"),
                    (Matching, "game/fsys/fsys_file_candidate_8017D3D4.c"),
                    (Matching, "game/fsys/fsys_file_exact_8017D400.c"),
                ]
            ],
            Object(
                Matching,
                "game/fsys/fsys_file_r51_8017C6E0_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # fsysStatus100: level-0 code like the rest of fsys (it was scored
            # at -O4,s from a candidate include; exact at level 0).
            Object(
                Matching,
                "game/fsys/fsys_file_r49_8017C8FC_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # Status-2000 handler, level-0 code like the rest of fsys: exact
            # only with the unit-wide `-opt level=0` (73% without it).
            Object(
                Matching,
                "game/fsys/fsys_file_r49_8017CE7C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_r48_8017BD34_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # Like the rest of the fsys code, 0x8017D410 - 0x8017D960 is
            # optimisation-level-0 code: every function in the unit is exact
            # with the unit-wide `-opt level=0` and no local pragmas.
            Object(
                Matching,
                "game/fsys/fsys_file_r52_8017D410_prefix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_r52_8017D960_o4s_inline_noauto.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # Level-0 code like the rest of fsys: exact only with the
            # unit-wide `-opt level=0` (66% without it).
            Object(
                Matching,
                "game/fsys/fsys_file_r52_8017DAB8_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017DB74_gc20.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # 0x8017DEA4 - 0x8017E30C: exact under the same unit-wide
            # `-opt level=0` (parameters homed on the stack, no pragmas).
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017DEA4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # fsysLoadEntrySync: exact on the fsys level-0 flags. Its
            # `compressed = NULL` after the free is the unit's free-then-clear
            # idiom (see the file header for the evidence).
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017E30C_o2.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            # _fsysGetFilename on the fsys unit's level-0 flags; its "%s.fsys"
            # literal is in .sdata2, i.e. strings are read-only (the unit's
            # "gsfsys.toc" name is in .rodata too).
            Object(
                Matching,
                "game/fsys/fsys_file_exact_8017EB6C.c",
                mw_version="GC/1.3",
                extra_cflags=[
                    "-use_lmw_stmw on",
                    "-sdata 8",
                    "-sdata2 8",
                    "-opt level=0",
                    "-str reuse,readonly",
                ],
                progress_category="game",
            ),
            # DVD/ARQ completion callbacks, level-0 code like the rest of
            # fsys: both exact only with the unit-wide `-opt level=0`.
            Object(
                Matching,
                "game/fsys/fsys_file_r56_8017F108_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/fsys/fsys_file_candidate_8017CED8.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt level=0"],
                progress_category="game",
            ),
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/people/people_exact_8018F470.c"),
                    (Matching, "game/people/people_candidate_8018F4C8.c"),
                    (Matching, "game/people/people_exact_8018F5B4.c"),
                    (Matching, "game/people/people_candidate_8018F730.c"),
                    (Matching, "game/people/people_exact_8018FB2C.c"),
                    (Matching, "game/people/people_exact_8018FB94.c"),
                    (Matching, "game/people/people_exact_8018FBAC.c"),
                    (Matching, "game/people/people_exact_8018FC50.c"),
                    (Matching, "game/people/people_exact_8018FC74.c"),
                    (Matching, "game/people/people_candidate_8018FCE0.c"),
                    (Matching, "game/people/people_exact_8018FD88.c"),
                ]
            ],
            Object(
                Matching,
                "game/menuPokemon_r57_800181C4_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_island_80018594.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_80018A68.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # Exact menuPokemon.c runs 0x80018F30-0x800192A8 and 0x80019938-0x80019B48
            # link as _ONLY islands; the rest of the range stays candidates.
            Object(
                Matching,
                "game/menuPokemon_island_80018F30.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_800192A8.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_island_800194D0.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_800194E4.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_island_800195E0.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_80019754.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_island_80019938.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_r57_80019B48_gc20p1_o4s.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_r57_80019D5C_o1.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-O2,p", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuPokemon_r57_80019F6C_suffix.c",
                constant_import=[{"symbol": "lbl_8047B7B8", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonCtrl_exact.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonClose_exact.c",
                mw_version="GC/2.0",
                cflags=["-O4" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule off"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_8001B1EC.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # TEMPORARY constant_import
            Object(
                Matching,
                "game/menuPokemon_island_8001BAC4.c",
                constant_import=[{"symbol": "lbl_8047B7B8", "offset": 0, "expected": "4330000080000000"}],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonOpen_exact.c",
                mw_version="GC/2.0",
                cflags=["-O4" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemonMain_exact.c",
                mw_version="GC/2.0",
                cflags=["-O4" if flag == "-O4,p" else flag for flag in cflags_base],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-schedule off"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_8001BEBC.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_candidate_8001C064.c",
                mw_version="GC/2.0",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_r50_8001C7B8_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=["-inline auto,deferred", "-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_r50_8001D378_o3.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menuPokemon_r50_8001D624_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # menuSub TU (0x8001D718-0x8001EF78) with its .sdata2 pool, the
            # fn_8001E644 colour initializer and the msgWinDrawBackground work area.
            Object(
                Matching,
                "game/menuSub.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            # HAL sysdolphin cobj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.sdata/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/cobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin displayfunc.c, built with the library flags; it
            # owns its .rodata/.data/.bss/.sdata/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/displayfunc.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin jobj.c, the whole translation unit, built with the
            # library flags; it owns its .rodata (IK vectors and string pool),
            # .data (hsdJObj and JObjUpdateFunc's switch table), .sbss and
            # .sdata2.
            Object(
                Matching,
                "hsd/jobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin list.c, built with the library flags; it owns its
            # .bss/.sdata2.
            Object(
                Matching,
                "hsd/list.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin lobj.c, built with the library flags; it owns
            # its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/lobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            *[
                Object(
                    CodeCandidate,
                    path,
                    mw_version=version,
                    cflags=(
                        ["-O1" if flag == "-O4,p" else flag for flag in cflags_base]
                        if use_o1
                        else None
                    ),
                    extra_cflags=["-use_lmw_stmw on"],
                    progress_category="hsd",
                )
                for path, version, use_o1 in [
                ]
            ],
            Object(
                Matching,
                "hsd/memory.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),  # HAL sysdolphin memory.c, library flags; owns its .rodata/.bss
            *[
                Object(
                    status,
                    path,
                    mw_version="GC/1.3",
                    extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                    progress_category="game",
                )
                for status, path in [
                    (Matching, "game/gs_dvd_candidate_80167040.c"),
                    (Matching, "game/gs_dvd_exact_80167E54.c"),
                    (Matching, "game/gs_dvd_candidate_80167E64.c"),
                    (Matching, "game/gs_dvd_exact_80167FA4.c"),
                    (Matching, "game/gs_dvd_r47_prefix.c"),
                ]
            ],
            # Function-boundary carve of fn_80168638 (no jump tables or pooled
            # constants; data extern), same GC/1.3 flags as the prefix owner.
            Object(
                Matching,
                "game/gs_dvd_r47_80168638_o4s.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_exact_800FF0A0.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_residual_800FF178.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_exact_800FF4D4.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_residual_800FF58C.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_exact_800FF660.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor_data_residual_800FF730.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_carde_r48_8007C300_prefix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_carde_r48_8007C7EC_o2.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_carde_r48_8007CAB0_suffix.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_carde_matrix_candidate_8007CBB4.c",
                mw_version="GC/2.0",
                # TEMPORARY constant_import: the unit's Card-E matrix tables
                # (the TU's .rodata pool, addressed off one base).
                constant_import=[
                    {"symbol": "lbl_80268B88", "section": ".rodata", "offset": 0, "expected": (
                        "07a007a107a207a3102e102f1030103110321033103410351036103710381039103a103b103c103d103e103f1040104110421043104410451046104710481049"
                        "104a104b104c104d112711281129112a112b112c112d112e112f1130113111321133113411351136113711381139113a113b113c113d113e113f114011411142"
                        "1143114411451146114711481149114a104e104f1050105110521053105410551056105710581059105a105b105c105d105e105f106010611062106310641065"
                        "1066106710681069106a106b106c106d106e106f10701071114b114c114d114e114f1150115111521153115411551156115711581159115a115b115c115d115e"
                        "115f1160116111621163116411651166116711681169116a116b116c116d116e10721073107410751076107710781079107a107b107c107d107e107f10801081"
                        "10821083108410851086108710881089108a108b108c108d108e108f109010911092109310941095116f1170117111721173117411751176117711781179117a"
                        "117b117c117d117e117f1180118111821183118411851186118711881189118a118b118c118d118e118f1190119111920000020a0000020c0000020e00000210"
                        "000002120000021400000216000002180000020b0000020d0000020f0000021100000213000002150000021700000219"
                    )},
                ],
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_carde_matrix_candidate_8007D4FC_gc20.c",
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/menu/menu_carde_matrix_candidate_8007D978.c",
                constant_import=[
                    {"symbol": "lbl_8047C148", "offset": 0, "expected": "3f800000"},
                    {"symbol": "lbl_8047C14C", "offset": 4, "expected": "3d800000"},
                    {"symbol": "lbl_8047C150", "offset": 8, "expected": "00000000"},
                    {"symbol": "lbl_8047C154", "offset": 12, "expected": "42400000"},
                    {"symbol": "lbl_8047C158", "offset": 16, "expected": "42000000"},
                    {"symbol": "lbl_8047C15C", "offset": 20, "expected": "41200000"},
                    {"symbol": "lbl_8047C160", "offset": 24, "expected": "3d000000"},
                    {"symbol": "lbl_8047C168", "offset": 32, "expected": "4330000080000000"},
                    {"symbol": "lbl_8047C170", "offset": 40, "expected": "4330000000000000"},
                ],
                mw_version="GC/2.0",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8", "-opt nopeephole"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_floor.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/tracefx_r55_801364A8_gc13_o1.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/tracefx_r55_8013796C_suffix.c",
                mw_version="GC/1.3",
                extra_cflags=["-use_lmw_stmw on", "-sdata 8", "-sdata2 8"],
                progress_category="game",
            ),
            # HAL sysdolphin shadow.c, built with the library flags; it owns
            # its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/shadow.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin util.c and video.c, built with the library
            # flags; each owns its data.
            Object(
                Matching,
                "hsd/util.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL video.c owns its complete text and literal/BSS pools. The
            # locally tagged Melee-style dont_inline keeps the draw-done flag
            # accessor out of line for HSD_VICopyXFBAsync.
            Object(
                Matching,
                "hsd/video.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin spline.c (.text 0x801B1890-0x801B25C4, .sdata2
            # 0x8047DE00-0x8047DE50) on the library-wide HSD flags.
            Object(
                Matching,
                "hsd/spline.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin state.c, the whole translation unit, on the
            # library-wide HSD flags; it owns the channel set-ups (.data),
            # dark_matter (.sdata) and its float pool (.sdata2).
            Object(
                Matching,
                "hsd/state.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            # HAL sysdolphin tev.c, one retail TU on the library-wide HSD flags.
            Object(
                Matching,
                "hsd/tev.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            # HAL sysdolphin texp.c / texpdag.c, each one retail TU built with
            # the library-wide HSD flags (GC/2.5, optimizer level 1 with
            # scheduling, deferred auto-inlining, lmw/stmw, read-only strings).
            Object(
                Matching,
                "hsd/texp.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/texpdag.c",
                mw_version="GC/2.5",
                extra_cflags=[
                    "-O1",
                    "-inline auto,deferred",
                    "-use_lmw_stmw on",
                    "-str reuse,readonly",
                ],
                progress_category="hsd",
            ),
            # HAL sysdolphin aobj.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .rodata/.data/.bss/.sbss/.sdata2.
            Object(
                Matching,
                "hsd/aobj.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "dolphin/os/OSCache.c",
                # L2GlobalInvalidate is never inlined into __OSCacheInit (was #pragma dont_inline on).
                extra_cflags=["-inline off"],
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5624.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5784.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A5810.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A58BC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A58F0.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5918.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A59CC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5C60.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5CC8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5D60.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A5D88.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A5EE0.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A60D4.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A62CC.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A63C8.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A640C.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A6508.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A6578.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A6684.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A7484.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_exact_800A76E4.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/dvd/DVD_range_800A7728.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReset_candidate_prefix.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReset_prefix_exact.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReset_exact.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReset_data.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "dolphin/os/OSReset_candidate_suffix.c",
                progress_category="sdk",
            ),
            Object(
                Matching,
                "crt/exit.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/global_destructor_chain_exact_800C4668.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/runtime.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKConstructEvent.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKBufferReset.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKGetBuffer.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTerminateSerialHandler.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/usr_put_initialize.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatchInit.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatchConnected.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKDispatchMutex.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTargetState.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKTargetStopped.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKBoard.c",
                mw_version="GC/1.3",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKCommState.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/TRKComm_exact_800C39BC.c",
                mw_version="GC/1.3.2",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/udp_cc.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/ddh_cc_close.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/ddh_cc_shutdown.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/circle_buffer_count.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/gdev_cc_close.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "trk/gdev_cc_shutdown.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "game/data/rodata_80267060.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_802663A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_802666B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266BD8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266C30.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266C7C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80266D78.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267250.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267350.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267398.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267A5C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/rodata_80267DD8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80267EA8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80268708.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026F2E8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026F568.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026F640.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026F818.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026FA34.c",
                data_alignment=4,
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026FAE8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026FB94.c",
                data_alignment=4,
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026FD4C.c",
                data_alignment=4,
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026FE58.c",
                data_alignment=4,
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_8026FE70.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80270200.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80270350.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80270440.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80270528.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80270EE8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80271300.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80271E10.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80272200.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_802729C0.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80272B08.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80272C30.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80273548.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80273A00.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80273DC8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_802741F8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            # HAL sysdolphin debug.c from HSD_Panic on, built with the library
            # flags; HSD_SaveContext (0x80196CE0) is hand-written assembly in
            # HAL's source and stays in the generated assembly.
            Object(
                Matching,
                "hsd/debug.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            # HAL sysdolphin hash.c and id.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); each owns its own data.
            Object(
                Matching,
                "hsd/hash.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "game/data/rodata_80274EC8.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_802757F0.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80279320_prefix.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_802798F0.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/rodata_80279B78.c",
                progress_category="game",
                extra_cflags=["-sdata2 0"],
            ),
            Object(
                Matching,
                "game/data/bss_8039A700.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_8039E700.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803A1F88.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803A2040.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803A6498.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803A9E40.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803B6E40.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803D6E40.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_803F6E40.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80402518.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80404BF0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80408400_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80435FF8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80446F10.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_8044FB90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80452500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80452EC8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80455070.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80465378.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80465710.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80466DE8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80467378.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_80467CF8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_8046D630.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/bss_804787E0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8027A500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E28F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E4B98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E4CA8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E4DB0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E51C8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E61D8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802E6428.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802ED9F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EDE54.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EE458.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EE608.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EEB78.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_802EF0A8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80310CD8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311868.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803119F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311AD4.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311B48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311D38.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80311E54.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80312500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80312558.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803125E8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803127F0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80312C40_suffix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80313608.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "trk/TRKRestoreFlags.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "crt/ansi_files_data.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80313B18.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80313F48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80314350.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80315490.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803155D0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80315690.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B430.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B468.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B500.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B8A0.c",
                extra_cflags=["-sdata 0"],
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B904.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B96C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035B9F8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BA48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BB50.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035BBA8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035C430.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8035E940.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803635C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80363774.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80363B18.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80363CA8_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80367EF0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803692C8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80369D20.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036C2A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036C568.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036CC00.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036CFA8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036E0E0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8036E150.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80372700.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803751F0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80375230.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80375938.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80375F98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803760D8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80376FCC.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80377B0C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803784C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803786D0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80378794.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803788C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80378A7C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80378B00.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80378D14.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80379388.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8037943C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_803794CC.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80379714.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80379A3C.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_80379BF4.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8038FFFC.c",
                data_alignment=4,
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A648.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/data_8039A6A8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B6B8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B6E0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B700.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B740.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B7A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B920.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B950.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047B9A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BA58.c",
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
                "game/data/sdata2_8047BE18.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BE98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BF10.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BFA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047BFE8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C0C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C100.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C128.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C190.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C1A0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C280.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C2A0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C318.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C3E8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C408.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047C418.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "crt/sdata2_math_8047C910.c",
                source="crt_data/sdata2_math_8047C910.c",
                progress_category="runtime",
            ),
            Object(
                Matching,
                "crt/sdata2_math_8047C970.c",
                source="crt_data/sdata2_math_8047C970.c",
                progress_category="runtime",
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
                "game/data/sdata2_8047C9B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA10.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA50.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA70.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CA88.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CAC8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB10.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB48.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB60.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CB98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CBE0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CC90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CC98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/gs_model_sdata2_8047CDC0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CDE0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CE70.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CE98.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CF08.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CF60.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047CFD0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D028.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D0E8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D110.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_sdata2_8047D160.c",
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
                "game/effect/effect_visual_sdata2_8047D300.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/effect/effect_visual_sdata2_8047D328.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D3B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D3C0_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D528.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D560.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D5C0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047D8A8.c",
                progress_category="game",
            ),
            # HAL sysdolphin random.c, built with the library flags
            # (GC/1.3.2 -O4,p -O1 -inline auto,deferred -use_lmw_stmw on
            # -str reuse,readonly); it owns its .sdata and .sdata2.
            Object(
                Matching,
                "hsd/random.c",
                mw_version="GC/1.3.2",
                extra_cflags=["-O1", "-inline auto,deferred", "-use_lmw_stmw on", "-str reuse,readonly"],
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DCA0.c",
                progress_category="hsd",
            ),
            Object(
                Matching,
                "hsd/hsd_sdata2_8047DF58.c",
                progress_category="hsd",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047DF90.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047DFA0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_sdata2_8047E0A8.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_waza_sdata2_prefix_8047E290.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_waza_sdata2_middle_8047E350.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/battle_waza_sdata2_suffix_8047E388.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E390_prefix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E3B0.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E3C8_suffix.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E428.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E508.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E530.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E538.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/data/sdata2_8047E628.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/colosseum_battle_sdata2.c",
                progress_category="game",
            ),
            Object(
                Matching,
                "game/colosseum_battle_sdata2_8047E6F8.c",
                progress_category="game",
            ),
        ],
    ),
    # REL 125 (common_rel, common.fsys member 0)
    Rel(
        "common_rel",
        [
            Object(Matching, "rel/common_rel/common_rel.c"),
            Object(Matching, "rel/common_rel/const_data.c"),
            Object(Matching, "rel/common_rel/msg_face_data.c"),
            # Shared DOL resource definitions; each complete original span
            # was hash-equal in the private-input resource reuse diagnostic.
            # Source/native REL and canonical-output checks remain required.
            Object(Matching, "rel/common_rel/shared_font_data.c"),
            Object(Matching, "rel/common_rel/shared_message_data.c"),
            # Typed MessageGroup headers and indices retain the original bases;
            # adjacent source catalogues preserve every message-storage byte.
            Object(Matching, "rel/common_rel/message_index_58044.c"),
            Object(Matching, "rel/common_rel/message_index_76C94.c"),
            Object(Matching, "rel/common_rel/script_structure_89CFC.c"),
            Object(
                Matching,
                "rel/common_rel/common_script_0596.c",
                source="rel/common_rel/common_script_0596.json",
                source_generator={"kind": "c", "tool": "tools/gs_vm_script_source.py"},
            ),
            # Packed GSchar controls and counter labels compiled from semantic templates.
            *[
                Object(
                    Matching,
                    f"rel/common_rel/message_templates_{address}.c",
                    source="rel/common_rel/message_templates.json",
                    source_generator={
                        "kind": "object", "tool": "tools/message_template_source.py",
                        "args": ["--unit", f"message_templates_{address}"],
                        "dependencies": ["tools/source_object_metadata.py"],
                        "assembler": True,
                    },
                )
                for address in ("5BFF4", "61A12", "61A5C", "61D8B", "61E46", "63069",
                                "64102", "7B1FC", "80AB1", "811F1", "85FFD", "89788")
            ],
            # Maintainer-approved readable message content and typed controls.
            # Existing physical owners and the complete data scope are unchanged.
            *[
                Object(
                    Matching,
                    f"rel/common_rel/message_storage_{address}.c",
                    source="rel/common_rel/message_catalog.json",
                    source_generator={
                        "kind": "object", "tool": "tools/message_catalog_source.py",
                        "args": [
                            "--unit", f"message_storage_{address}",
                            "--symbols", "config/GC6E01/common_rel/symbols.txt",
                            "--index-source", "src/rel/common_rel/message_index_58044.c",
                            "--index-source", "src/rel/common_rel/message_index_76C94.c",
                        ],
                        "dependencies": [
                            "tools/source_object_metadata.py",
                            "tools/message_template_source.py",
                            "config/GC6E01/common_rel/symbols.txt",
                            "src/rel/common_rel/message_index_58044.c",
                            "src/rel/common_rel/message_index_76C94.c",
                        ],
                        "assembler": True,
                    },
                )
                for address in ("5BF64", "5C090", "61A40", "61A7B", "61D94", "61E77",
                                "6306E", "6410E", "7B268", "810E1", "81609", "86014", "899BC")
            ],
            Object(Matching, "rel/common_rel/table_res_data.c"),
            Object(Matching, "rel/common_rel/floor_fight_data.c"),
            Object(Matching, "rel/common_rel/fight_trainer_data.c"),
            Object(Matching, "rel/common_rel/snd_song_table.c", comparison_view="prodg_data"),
            Object(Matching, "rel/common_rel/gs_task_data.c"),
            Object(Matching, "rel/common_rel/fight_ai_data.c"),
            Object(Matching, "rel/common_rel/treasure_data.c"),
            Object(Matching, "rel/common_rel/waza_data.c"),
            Object(Matching, "rel/common_rel/pokemon_data.c"),
            Object(Matching, "rel/common_rel/people_data.c"),
            Object(Matching, "rel/common_rel/snd_volume_data.c"),
            Object(Matching, "rel/common_rel/snd_sample_table.c", comparison_view="prodg_data"),
            Object(Matching, "rel/common_rel/misc_data.c"),
        ],
    ),
    # REL 1 (mail, common.fsys member 1)
    Rel(
        "mail",
        [
            Object(Matching, "rel/mail/mail.c"),
        ],
    ),
    # REL 2 (pocket_menu, pocket_menu.fsys member 0)
    Rel("pocket_menu", [Object(Matching, "rel/pocket_menu/pocket_menu.c")]),
    Rel("S1_out", [Object(Matching, "rel/S1_out/S1_out.c")]),
    # REL 3 (worldmap_menu_rel, world_map.fsys member 0)
    Rel(
        "worldmap_menu_rel",
        [Object(Matching, "rel/worldmap_menu_rel/worldmap_menu_rel.c")],
    ),
    # REL 5 (S1_shop_1F, S1_shop_1F.fsys member 0)
    Rel("S1_shop_1F", [Object(Matching, "rel/S1_shop_1F/S1_shop_1F.c")]),
    # REL 129 (S1_out_bf, S1_out_bf.fsys member 0)
    Rel("S1_out_bf", [Object(Matching, "rel/S1_out_bf/S1_out_bf.c")]),
    # REL 167 (pda_menu, pda_menu.fsys member 0)
    Rel("pda_menu", [Object(Matching, "rel/pda_menu/pda_menu.c")]),
    # REL 15 (M1_out, M1_out.fsys member 0)
    Rel("M1_out", [Object(Matching, "rel/M1_out/M1_out.c")]),
    # REL 16 (M1_pc_1F, M1_pc_1F.fsys member 0)
    Rel("M1_pc_1F", [Object(Matching, "rel/M1_pc_1F/M1_pc_1F.c")]),
    # REL 17 (M1_pc_B1, M1_pc_B1.fsys member 0)
    Rel("M1_pc_B1", [Object(Matching, "rel/M1_pc_B1/M1_pc_B1.c")]),
    # REL 18 (M1_houseA_1F, M1_houseA_1F.fsys member 0)
    Rel("M1_houseA_1F", [Object(Matching, "rel/M1_houseA_1F/M1_houseA_1F.c")]),
    # REL 19 (M1_houseA_2F, M1_houseA_2F.fsys member 0)
    Rel("M1_houseA_2F", [Object(Matching, "rel/M1_houseA_2F/M1_houseA_2F.c")]),
    # REL 20 (M1_houseB_1F, M1_houseB_1F.fsys member 0)
    Rel("M1_houseB_1F", [Object(Matching, "rel/M1_houseB_1F/M1_houseB_1F.c")]),
    # REL 21 (M1_houseC_1F, M1_houseC_1F.fsys member 0)
    Rel("M1_houseC_1F", [Object(Matching, "rel/M1_houseC_1F/M1_houseC_1F.c")]),
    # REL 22 (M1_shop_1F, M1_shop_1F.fsys member 0)
    Rel("M1_shop_1F", [Object(Matching, "rel/M1_shop_1F/M1_shop_1F.c")]),
    # REL 23 (M1_shop_2F, M1_shop_2F.fsys member 0)
    Rel("M1_shop_2F", [Object(Matching, "rel/M1_shop_2F/M1_shop_2F.c")]),
    # REL 24 (M1_gym_1F, M1_gym_1F.fsys member 0)
    Rel("M1_gym_1F", [Object(Matching, "rel/M1_gym_1F/M1_gym_1F.c")]),
    # REL 25 (M1_gym_B1, M1_gym_B1.fsys member 0)
    Rel("M1_gym_B1", [Object(Matching, "rel/M1_gym_B1/M1_gym_B1.c")]),
    # REL 26 (M1_stadium_1F, M1_stadium_1F.fsys member 0)
    Rel("M1_stadium_1F", [Object(Matching, "rel/M1_stadium_1F/M1_stadium_1F.c")]),
    # REL 27 (M1_carde_1, M1_carde_1.fsys member 0)
    Rel("M1_carde_1", [Object(Matching, "rel/M1_carde_1/M1_carde_1.c")]),
    # REL 132 (M1_out_bf, M1_out_bf.fsys member 0)
    Rel("M1_out_bf", [Object(Matching, "rel/M1_out_bf/M1_out_bf.c")]),
    # REL 133 (M1_gym_1F_bf, M1_gym_1F_bf.fsys member 0)
    Rel("M1_gym_1F_bf", [Object(Matching, "rel/M1_gym_1F_bf/M1_gym_1F_bf.c")]),
    # REL 134 (M1_houseA_1F_bf, M1_houseA_1F_bf.fsys member 0)
    Rel("M1_houseA_1F_bf", [Object(Matching, "rel/M1_houseA_1F_bf/M1_houseA_1F_bf.c")]),
    # REL 157 (M1_water_colo, M1_water_colo.fsys member 0)
    Rel("M1_water_colo", [Object(Matching, "rel/M1_water_colo/M1_water_colo.c")]),
]

config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "Dolphin SDK Code"),
    ProgressCategory("hsd", "HSD/sysdolphin (Third Party)"),
    ProgressCategory("musyx", "MusyX (Third Party)"),
    ProgressCategory("runtime", "Gekko Runtime Code"),
]
config.progress_each_module = args.verbose

if args.mode == "configure":
    generate_build(config)
elif args.mode == "progress":
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
