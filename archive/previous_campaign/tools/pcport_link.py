#!/usr/bin/env python3
"""pcport_link.py — drive the PC-port bootstrap to LINK and RUN.

Compiles the bootstrap + HSD sources (generated copies where asm-blocked) with
clang -m32, links against the CMake-built shim/glfw/glad libs, then auto-generates
stub definitions for every unresolved symbol (fn_* -> no-op function, lbl_* -> data
bytes, others best-effort) and re-links. This gets a launching exe before every TU
is ported; stubs are replaced by real code incrementally.

Run:  python tools/pcport_link.py            # build + link + (on success) run --window-smoke
"""
import subprocess, re, sys, os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CLANG = r"C:\Program Files\LLVM\bin\clang.exe"
OBJ = ROOT / "build_pc" / "obj"
GEN = ROOT / "build_pc" / "gen"
OBJ.mkdir(parents=True, exist_ok=True)
INC = ["-Iinclude", "-Iinclude/dolphin", "-Iinclude/game", "-Iinclude/hsd",
       "-Ipcport", "-Ithird_party", "-Ithird_party/glad/include",
       "-Ibuild_pc/_deps/glfw-src/include"]   # GLFW headers (FetchContent) + stb_image (third_party)
COMPAT = ["-include", "pcport/pcport_compat.h"]
# -fms-compatibility/-fms-extensions: emulate MSVC leniency (e.g. u32 = long-vs-int
# typedef redefinitions the decomp+SDK headers disagree on) so the same TUs that
# the CMake/MSVC build compiles also compile here.
CFLAGS = ["-m32", "-c", "-DPCPORT=1", "-w", "-O1",
          "-fms-compatibility", "-fms-extensions",
          # default-error diagnostics the 32-bit decomp violates by design
          # (u32<->pointer interchange, CW value-use-of-void artifacts):
          "-Wno-error=int-conversion", "-Wno-error=incompatible-pointer-types",
          "-Wno-error=implicit-function-declaration",
          "-Wno-error=return-mismatch"] + COMPAT + INC
LIBS = [
    # project libs by full path (not in standard search dirs)
    str(ROOT / "build_pc/Debug/pcport_shim.lib"),
    str(ROOT / "build_pc/Debug/glad.lib"),
    str(ROOT / "build_pc/_deps/glfw-build/src/Debug/glfw3.lib"),
    # system libs via -l (clang/lld-link resolves against the auto-detected SDK paths)
    "-lopengl32", "-luser32", "-lgdi32", "-lshell32", "-lkernel32",
    "-lwinmm",  # waveOut (THP boot-movie audio sink)
]

# bootstrap sources (== CMake add_executable list)
BOOT = ["pcport/pcport_main.c", "pcport/gs_gfx_host_support.c",
        "pcport/real_content_host.c", "pcport/hsd_host.c",
        "pcport/thp_player.c",
        "pcport/thp_audio.c", "pcport/waveout_sink.c",
        "pcport/bgm_host.c", "pcport/musyx_wave.c",
        "pcport/os_thread_host.c", "pcport/engine_host.c",
        "pcport/engine_spike.c", "pcport/gs_sched_host.c",
        "pcport/engine_boot.c", "pcport/field_collision.c",
        "pcport/hsd_fobj_host.c",
        "src/dolphin/vi/VI.c",
        "src/dolphin/os/OSStateFlags.c", "src/trk/TRKUtil.c",
        "src/hsd/hsd_pobj_disp.c", "src/game/gs_gfx.c", "src/game/gs_render.c"]
HSD_CLEAN = ["hsd_fobj", "hsd_mobj_ext", "hsd_mtx", "hsd_pobj", "hsd_pobj_ext",
             "hsd_robj", "hsd_shadow", "hsd_state", "hsd_tev", "hsd_texp",
             "hsd_tobj", "hsd_tobj_ext"]
HSD_GEN = ["hsd_dobj", "hsd_jobj", "hsd_mobj", "hsd_aobj", "hsd_lobj", "hsd_object",
           "hsd_class", "hsd_initialize", "hsd_wobj", "hsd_displayfunc",
           "hsd_render", "hsd_memory_ext", "hsd_util", "hsd_jobj_display", "hsd_cobj"]
# Real game TUs compiled through pcport_gen (conflict fixes) — rel path under src/
# (and build_pc/gen/), no .c. Track-D: host-link real engine code under the scheduler.
GAME_GEN = ["game/battle/battle_main",
            # Track-D parallel-analysis workflow (2026-06-02): 10 fully-C-active (0-asm)
            # engine TUs made host-linkable. 7 compile clean; gs_mem/gs_dvd/battle_waza
            # need the pcport_gen PREAMBLE/TEXT_FIXUPS entries added alongside.
            "game/gs_task_util", "game/gs_render_util", "game/gs_mem", "game/gs_dvd",
            "game/gs_flag", "game/gs_battle_setup", "game/battle/battle_waza",
            "game/gs_colsys", "game/gs_field_resource", "game/gs_floor_data",
            # Campaign Phase 1 (2026-06-02): all remaining 0-asm game TUs (47). The
            # compile step filters; failures get pcport_gen fix-entries via workflow.
            "game/battle/battle_grid", "game/data/common_rel",
            "game/data/move_data", "game/data/pokemon_data", "game/effect/generator",
            "game/effect/gs_effect", "game/effect/tracefx", "game/fsys/fsys_decomp",
            "game/gba/gba_comm", "game/gba/gba_comm_ext", "game/gba/gba_conv",
            "game/gba/gba_conv2", "game/gba/gba_misc", "game/gba/pokeconv",
            "game/gs_field_colquery", "game/gs_floor", "game/gs_model", "game/late_game",
            "game/menu/menu_battle", "game/menu/menu_carde", "game/menu/menu_carde_main",
            "game/menu/menu_carde_matrix", "game/menu/menu_common", "game/menu/menu_common_ext",
            "game/menu/menu_exdisc", "game/menu/menu_exdisc2", "game/menu/menu_middle",
            "game/menu/menu_pokecoupon", "game/menu/menu_precine", "game/menu/menu_rule",
            "game/menu/menu_tool", "game/menu/menu_tool2", "game/save/save",
            "game/save/save_carde", "game/save/save_crypto", "game/save/save_data",
            "game/script/psdisp", "game/script/pshelper", "game/script/psinterpret",
            "game/script/pslist", "game/shadow/shadow", "game/shadow/shadow_data",
            "game/sound/sound", "game/sound/sound_bgm", "game/sound/sound_se",
            # NOTE: colosseum_script (165 retype + 79 ordered text-fixups, non-functional
            # pseudo-register paths) + trainer (workflow couldn't resolve) DEFERRED —
            # they stay auto-stubbed (= baseline, no loss). Off the title/boot path.
            #
            # Campaign Phase 2 (2026-06-02): asm-bearing TUs — pcport_gen flips their
            # #if1-asm wrappers to the #else C (real decomp body -> real C; TODO -> stub).
            # EXCLUDED (collide with host shims; would duplicate-define): main (two main()),
            # gs_thread/gs_task (real GStask/GSthread vs gs_sched_host), gs_texture
            # (fn_800F0308 + GX-FIFO). Host shims win via BOOT-before-GAME_GEN link order,
            # but excluding avoids pulling their hardware closures.
            # --- chunk 2a (asm-bearing): 7 host-linked (2 compiled free + 5 workflow-
            # fixed). DEFERRED (pathological/unfixable, stay auto-stubbed = baseline):
            # colosseum_event (124 fixes, pseudo-register), pokemon (111), colosseum_battle
            # (73), + 8 the workflow couldn't resolve (colosseum_ui, menu_bag/msgbox/pokemon/
            # shop/status, evolution, script_callback). The asm-bearing tail is real
            # functional-decomp work, not link-flipping — see docs §6g.
            "game/gs_particle", "game/movie", "game/pokemon/poke_detail",
            "game/menu/menu_dialog", "game/gs_event_exec", "game/fsys/fsys_load",
            "game/input/input",
            # Step-0 flip-harvest (2026-06-10): pcport_gen auto-unify + FLIP_AS_STUB/
            # STUB_BODY made these asm-bearing TUs compile; broken pseudo-register
            # bodies are stubbed (= auto-stub baseline), real #else C + already-active
            # C goes live (~300 real fns incl. 40 newly-flipped field_world bodies).
            "game/gs_field_world", "game/gs_worldmap",
            "game/effect/effect_util", "game/battle/battle_scene",
            # Step-0 wave 2 (2026-06-10): harvest driver (pcport_harvest.py +
            # pcport_stub_tables.json) + kr_def/join-split-sigs passes cracked the
            # campaign's deferred TUs, incl. the three "pathological" pseudo-register
            # walls (their Ghidra-import bodies are functional C).
            "game/colosseum_event", "game/colosseum_battle", "game/colosseum_script",
            "game/colosseum_ui", "game/pokemon", "game/trainer",
            "game/menu/menu_bag", "game/menu/menu_msgbox", "game/menu/menu_pokemon",
            "game/menu/menu_shop", "game/menu/menu_status",
            "game/pokemon/evolution", "game/script/script_callback",
            ]


def compile_one(src, name):
    o = OBJ / f"{name}.o"
    r = subprocess.run([CLANG, *CFLAGS, str(src), "-o", str(o)],
                       cwd=ROOT, capture_output=True, text=True)
    return (o if r.returncode == 0 else None), r.stderr


def gen_hsd():
    srcs = ([f"src/hsd/{n}.c" for n in HSD_GEN] +
            [f"src/{n}.c" for n in GAME_GEN])
    subprocess.run([sys.executable, "tools/pcport_gen.py", "--out-dir", "build_pc/gen", *srcs],
                   cwd=ROOT, capture_output=True)


def link(objs, exe, extra=None):
    cmd = [CLANG, "-m32", *[str(o) for o in objs]]
    if extra: cmd.append(str(extra))
    # /FORCE:MULTIPLE: gs_gfx_host_support.c had placeholder defs that the real HSD
    # TUs now provide — allow the duplicates (linker takes the first) so we get a
    # launching exe; the redundant host stubs get cleaned up next.
    cmd += LIBS + ["-o", str(exe), "-Xlinker", "/SUBSYSTEM:CONSOLE",
                   "-Xlinker", "/FORCE:MULTIPLE", "-Xlinker", "/IGNORE:4006,4088",
                   "-Xlinker", "/ERRORLIMIT:0"]   # report ALL undefined, not just 20
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)


UNDEF_RE = re.compile(r'undefined symbol:\s+_?([A-Za-z_]\w*)')


def parse_undef(stderr):
    return sorted(set(UNDEF_RE.findall(stderr)))


def make_stubs(symbols):
    lines = ["/* AUTO-GENERATED stubs for not-yet-ported symbols (pcport_link.py). */",
             "typedef unsigned int u32;"]
    for s in symbols:
        if s.startswith("fn_"):
            lines.append(f"int {s}(){{return 0;}}")
        elif s.startswith("lbl_"):
            lines.append(f"char {s}[0x4000];")
        else:
            # unknown: emit BOTH would conflict; guess data for known-data names else fn
            lines.append(f"int {s}(){{return 0;}}")
    (ROOT / "build_pc/gen/pcport_stubs.c").write_text("\n".join(lines) + "\n")
    return ROOT / "build_pc/gen/pcport_stubs.c"


def main():
    gen_hsd()
    objs, failed = [], []
    for src in BOOT:
        o, err = compile_one(ROOT / src, Path(src).stem)
        (objs if o else failed).append(o or (src, err))
    for n in HSD_CLEAN:
        o, err = compile_one(ROOT / f"src/hsd/{n}.c", n)
        (objs.append(o) if o else failed.append((n, err)))
    for n in HSD_GEN:
        o, err = compile_one(GEN / f"hsd/{n}.c", n)
        (objs.append(o) if o else failed.append((n, err)))
    for n in GAME_GEN:
        o, err = compile_one(GEN / f"{n}.c", Path(n).stem)
        (objs.append(o) if o else failed.append((n, err)))
    objs = [o for o in objs if o]
    print(f"compiled {len(objs)} objects; {len(failed)} failed to compile: "
          f"{[f[0] if isinstance(f, tuple) else f for f in failed]}")

    exe = ROOT / os.environ.get("PCPORT_LINK_EXE", "build_pc/pcport_bootstrap.exe")
    all_undef, so = set(), None
    for rnd in range(1, 9):
        if exe.exists():
            exe.unlink()
        r = link(objs + ([so] if so else []), exe)
        if r.returncode == 0 and exe.exists():
            print(f"round {rnd}: LINKED OK ({len(all_undef)} symbols stubbed)")
            print(f"\n=== BUILT {exe} ===")
            return
        undef = parse_undef(r.stderr)
        if not undef:
            print(f"round {rnd}: link failed with NO undefined symbols (other error):\n"
                  + (r.stderr or r.stdout)[-1500:])
            return
        new = set(undef) - all_undef
        print(f"round {rnd}: rc={r.returncode}, {len(undef)} undefined ({len(new)} new) -> stubbing")
        if not new:
            print("no new undefined but still failing; residual:", sorted(undef)[:20]); return
        all_undef |= set(undef)
        stub_src = make_stubs(sorted(all_undef))
        so, serr = compile_one(stub_src, "pcport_stubs")
        if not so:
            print("STUB COMPILE FAILED:\n" + serr[:1500]); return
    print("did not converge in 8 rounds; stubbed", len(all_undef))


if __name__ == "__main__":
    main()
