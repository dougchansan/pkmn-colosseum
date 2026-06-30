#!/usr/bin/env python3
"""pcport_gen.py — generate PC-compilable copies of decomp .c files.

For the PC port (functional C, NOT byte-match), `#if 1 asm { #include .inc } #else
<C> #endif` wrappers must use the C branch — clang/MSVC can't assemble PowerPC .inc.
Every such wrapper already carries an inactive `#else` C reference body. This tool
emits a copy with each *asm-active* (`#if 1`) wrapper replaced by its `#else` C, so
the file compiles for x86. The ORIGINAL source is untouched (the CodeWarrior matching
build keeps the asm); only the generated copy is compiled by the PC build.

`#if 0` asm wrappers already select their C via the preprocessor and pass through.

Usage:
  python tools/pcport_gen.py src/hsd/hsd_dobj.c            -> build_pc/gen/hsd/hsd_dobj.c
  python tools/pcport_gen.py --out-dir build_pc/gen src/hsd/hsd_dobj.c src/hsd/hsd_jobj.c
"""
import argparse, re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Per-file PC-only preamble: targeted decls/includes some decompiled TUs need to
# compile on a strict compiler (the CodeWarrior matching build never sees these,
# since only the generated copy carries them). Keeps originals pristine.
PREAMBLE = {
    "hsd/hsd_render.c":       "#include <string.h>\n",
    # OSReport + the CW-builtin varargs placeholder `__va_arg()` left behind by the
    # decomp; K&R protos accept the no-arg placeholder calls. -Wimplicit-function-
    # declaration is a default ERROR in modern clang, so a decl is mandatory.
    "hsd/hsd_util.c":         "void OSReport();\nvoid __va_arg();\n",
    # fn_80196E10/fn_8019D9DC/fn_801AA498 are implicit calls needing a decl. memset needs a
    # real prototype for its valid 3-arg call (clang errors on the undeclared library fn);
    # the lone 0-arg `memset();` in dead post-return code is removed by TEXT_FIXUPS below,
    # so pulling in <string.h>'s real 3-arg proto is now conflict-free.
    "hsd/hsd_jobj_display.c": ("#include <string.h>\n"
                               "extern void fn_80196E10();\nextern void fn_8019D9DC();\n"
                               "extern void fn_801AA498();\n"),
    "hsd/hsd_memory_ext.c":   "extern void fn_80196E10();\n",
    # Track-D engine TUs (parallel-analysis workflow, 2026-06-02): forward-decls for
    # functions used before their in-TU macro/decl, or referenced-but-not-declared.
    # GSmemSplitBlock is called (lines 336/412) before its `#define ->fn_800E2DB0`
    # appears (line 460); K&R forward decl, return u16 matches its assignment.
    "game/gs_mem.c":          "unsigned short GSmemSplitBlock();\n",
    # DVDInit + memset called without a decl (pcport_compat.h omits <string.h>).
    "game/gs_dvd.c":          "void DVDInit();\nvoid* memset(void*, int, unsigned int);\n",
    # fn_80129280 referenced but not declared in this TU; void* matches the
    # cast-to-void* + pointer-arith call sites (same form as battle_main.c).
    "game/battle/battle_waza.c": "void* fn_80129280();\n",
    # Campaign Phase 1 (workflow wf_de8c288a): forward-decls for referenced-but-
    # -undeclared functions (K&R, used-before-decl or no in-TU decl).
    "game/gs_model.c":        "void fn_801074D4();\n",
    "game/menu/menu_precine.c": "void fn_800FB680();\nvoid fn_800FBB34();\nvoid fn_80132A38();\nvoid fn_801C41C8();\n",
    "game/script/psinterpret.c": "void fn_80196E10();\nvoid psSetCameraTracking();\nvoid fn_801A05EC();\nvoid fn_80172840();\nvoid fn_80172790();\nvoid fn_801726E0();\nvoid fn_80172630();\n",
    "game/script/pslist.c":   "void* HSD_MemAlloc();\nvoid HSD_MemFree();\nvoid fn_80196E10();\n#include <string.h>\n",
    "game/sound/sound_se.c":  "void fn_80164C40();\nvoid fn_80164DD0();\n",
    # Campaign Phase 2 chunk 2a (workflow wf_ad563b5e): asm-bearing TUs.
    "game/pokemon/poke_detail.c": "void fn_800F0308();\n",
    "game/gs_event_exec.c":   "extern int fn_8005D9E4();\nextern unsigned int fn_80102568();\n",
}

# Per-file extern-unification: CodeWarrior tolerated the same data label being
# block-scoped `extern`-declared with DIFFERENT types in different functions of one
# TU (e.g. `u8 lbl_X[]` in one fn and `HSD_CObjInfo lbl_X` in another). clang rejects
# this with "redeclaration ... with a different type". Since the link is type-agnostic,
# we rewrite EVERY block-scoped / file-scope `extern <type> lbl_X[..];` declaration of a
# conflicting label to one canonical form, so clang sees a single consistent type per
# symbol within the TU. The canonical form is chosen so all *usages* still compile
# (decay vs &-of for arrays/scalars); residual pointer/array arg mismatches are warnings
# silenced by the build's -w. Applied only to the generated copy; originals stay pristine.
# Map: rel-path -> { label_name: canonical_extern_decl_without_trailing_semicolon }
EXTERN_UNIFY = {
    "game/colosseum_battle.c": {
        # f32 decls 2:1 over u32; the lone u32 read converts (flagged for runtime QA).
        "lbl_8047E6D8": "extern f32 lbl_8047E6D8",
    },
    "game/colosseum_script.c": {
        # u8-scalar decls dominate 20:1; the lone array-decl site decays fine via &.
        "lbl_80478D78": "extern u8 lbl_80478D78",
        # u8-scalar decls dominate; the lone u32 read converts (flagged for runtime QA).
        "lbl_80478D7D": "extern u8 lbl_80478D7D",
    },
    "game/gs_field_world.c": {
        # u8-scalar vs u8[] decls; all uses are `&lbl` — array form keeps both legal.
        "lbl_80426BD0": "extern u8 lbl_80426BD0[]",
    },
    "hsd/hsd_cobj.c": {
        # Array form: bare-decay usages (arith/compare/cast) dominate; the lone
        # `&lbl_8036C678` (HSD_CLASS_INFO cast) accepts an array address fine.
        "lbl_8036C678": "extern u8 lbl_8036C678[]",
        # Array form: the bare-decay site (line 412, passed as const char*) needs the
        # array->ptr decay; the `&lbl` sites become char(*)[] -> const char*, a mere
        # -Wincompatible-pointer-types warning (silenced by -w), NOT the default-ERROR
        # -Wint-conversion that the scalar form would trigger on the bare-decay site
        # under modern clang.
        "lbl_8047D958": "extern char lbl_8047D958[]",
        "lbl_8047D960": "extern char lbl_8047D960[]",
    },
}

# Per-file function-prototype neutralization to K&R empty-parens `extern <ret> fn_X();`.
# Use ONLY when a function's argument types are the sole conflict and its return type is
# consistent everywhere AND no real (prototyped) DEFINITION with promotion-affected params
# (float/char/short) exists in the TU — otherwise `()` is itself incompatible with that
# definition. The `()` form has unspecified parameters, so any argument shape at a call
# site is accepted. `-Wstrict-prototypes` on `()` is a warning, silenced by -w.
# Map: rel-path -> set of function names to neutralize to K&R prototypes.
FUNC_PROTO_KR = {
    # Campaign Phase 1 (workflow wf_de8c288a): arg-only conflicts on extern protos.
    "game/menu/menu_exdisc2.c": { "fn_8007B114", "fn_8007B6D8" },
    "game/menu/menu_middle.c":  { "fn_80070D84" },
    # Campaign Phase 2 chunk 2a (workflow wf_ad563b5e):
    "game/fsys/fsys_load.c": { "fn_801808E4" },
    "game/input/input.c": { "fn_800DD38C", "fn_800ABCF4", "fn_800AB4FC", "fn_800AAE34", "fn_800ABF5C" },
    "game/battle/battle_main.c": {
        # Block-scoped decls re-type the args (u32,u32,u32) vs the file-scope
        # prototypes' (s32,s32,s32); same void return. K&R `()` is compatible with
        # the surviving (comment-bearing, regex-skipped) file-scope prototype, and
        # accepts the 3-arg call sites. (sndPlay-with-fade / sndFade.)
        "fn_801659FC", "fn_80165A20",
    },
    "hsd/hsd_cobj.c": {
        # fn_80196E10 is the assert helper: callers pass the file/expr labels
        # inconsistently as `&lbl` (char(*)[]) and bare `lbl` (char*). No single data
        # type makes both clean against a `(const char*,...)` proto, so neutralize the
        # proto to K&R `()` — accepts every argument shape. Its definition's params
        # (const char*, u32, const char*) are promotion-safe, so `()` stays compatible.
        "fn_80196E10",
    },
}

# Per-file removal of redundant arg-less STUB prototypes. The generator emits placeholder
# `extern void fn_X(void);` / `extern <ret> fn_X();` stubs at file scope, but these same
# functions ALSO have a real typed declaration (block-scoped, with args) or a real typed
# DEFINITION elsewhere in the TU. clang errors "conflicting types" when the stub's return
# type (`void`) or arg-less shape clashes with the real one — and K&R can't bridge a
# differing return type or a float/short/char-param definition. Dropping the redundant
# stub lines leaves exactly the real typed decl/definition, which clang accepts. Listed
# names are verified to have a surviving typed decl/definition in the same TU.
# Map: rel-path -> set of function names whose arg-less stub prototypes are dropped.
FUNC_STUB_DROP = {
    # Campaign Phase 1 (workflow wf_de8c288a): redundant stub vs real in-TU def.
    "game/menu/menu_tool.c": { "fn_80075638" },
    "game/battle/battle_main.c": {
        # fn_801EF634 has a real DEFINITION `u16 fn_801EF634(void)` (line 431) that
        # precedes this redundant block-scoped `extern void fn_801EF634();` stub
        # (line 828) and all its call sites; the void-return stub clashes with the
        # u16 definition. Dropping it leaves the in-scope definition.
        "fn_801EF634",
    },
    "hsd/hsd_cobj.c": {
        "fn_801C25E4", "fn_801C2670", "fn_80191DCC", "fn_80191E88",
        "fn_800BD640", "fn_800BD744",
    },
}

# Per-file prototype RETYPE: rewrite a function's stub/forward prototype to an exact
# canonical signature. Use when the stub is genuinely NEEDED as a forward declaration
# (the symbol is referenced before its definition) AND its real definition has a
# prototyped signature that neither K&R `()` nor a dropped stub can satisfy (e.g. float
# params, which default-promotion makes incompatible with `()`). The canonical text MUST
# match the real definition's signature so the forward decl and definition agree.
# Map: rel-path -> { fn_name: canonical_prototype_without_trailing_semicolon }.
FUNC_PROTO_RETYPE = {
    # Step-0 harvest: fn_8012640C result is masked (`& 0xFF`) here; the TU's void*
    # majority decl breaks the arithmetic. Real return is u32 (see gs_field_world def).
    "game/colosseum_event.c": {
        "fn_8012640C": "u32 fn_8012640C()",
    },
    "game/colosseum_battle.c": {
        "fn_8012640C": "u32 fn_8012640C()",
    },
    # Campaign Phase 2 chunk 2a (workflow wf_ad563b5e):
    "game/fsys/fsys_load.c": {
        "fn_8017F6B4": "void* fn_8017F6B4()",
    },
    # Campaign Phase 1 (workflow wf_de8c288a): unify return-type-conflicting protos.
    "game/gba/gba_misc.c": {
        "fn_80083BF8": "s32 fn_80083BF8()",
        "fn_8008A99C": "s32 fn_8008A99C()",
        "fn_801906A0": "s32 fn_801906A0()",
    },
    "game/menu/menu_carde_matrix.c": {
        "fn_80104704": "void* fn_80104704()",
        "fn_801040A0": "u32* fn_801040A0()",
    },
    "game/menu/menu_exdisc.c": {
        "fn_8006B420": "u8* fn_8006B420()",
        "fn_800C80D0": "s32 fn_800C80D0()",
    },
    "game/menu/menu_exdisc2.c": {
        "fn_801C40F0": "s32 fn_801C40F0()",
        "fn_800A7BCC": "void* fn_800A7BCC()",
    },
    "game/menu/menu_middle.c": {
        "fn_8006AFC4": "s32 fn_8006AFC4()",
        "fn_80129280": "u8 *fn_80129280()",
    },
    "game/menu/menu_precine.c": {
        "fn_80132A38": "void fn_80132A38()",
        "fn_801C41C8": "void fn_801C41C8()",
    },
    "game/menu/menu_tool.c": {
        "fn_80102620": "s32 fn_80102620()",
    },
    "game/menu/menu_tool2.c": {
        "fn_80190528": "s32 fn_80190528()",
        "fn_801902E0": "s32 fn_801902E0()",
        "fn_80102510": "s32 fn_80102510()",
        "fn_80165A20": "s32 fn_80165A20()",
        "fn_80075F4C": "s32 fn_80075F4C()",
    },
    "game/battle/battle_main.c": {
        # Two block-scoped decls disagree on BOTH return and args: `void
        # fn_80129280()` (line 485, called 0-arg) vs `void* fn_80129280(u32,u32)`
        # (line 543, called as fn_80129280(0,0xF) with the result used). Retype both
        # to the K&R-args `void* fn_80129280()`: a single consistent return (void*)
        # with unspecified params, so BOTH the 0-arg and 2-arg call sites compile.
        "fn_80129280": "void* fn_80129280()",
    },
    "hsd/hsd_cobj.c": {
        # Referenced at file scope as `(u8*)fn_80196C54` before its definition; the def is
        # `void fn_80196C54(int, f32, f32, f32, f32, f32, f32)`. The `(void)` stub clashed
        # with the float-param definition, so retype the forward decl to match it exactly.
        "fn_80196C54": "void fn_80196C54(int, f32, f32, f32, f32, f32, f32)",
    },
}

# Per-file exact-substring fixups applied to the FINAL generated text (last pass). For
# the rare decomp artifact that no declaration-level rewrite can fix — e.g. a call with
# the wrong arity in dead (post-`return`) code that clang still type-checks. Each entry is
# (old_exact_substring -> new_substring); must match verbatim. Applied only to the
# generated copy; the original byte-match source is never touched.
# Map: rel-path -> list of (old, new) pairs.
TEXT_FIXUPS = {
    # Campaign Phase 2 chunk 2a (workflow wf_ad563b5e): RAW asm blocks (not the
    # #if1/#else form, so pcport_gen's flipper can't reach them) -> empty stubs;
    # + one callback cast. These functions become stubs; the TU's real C links.
    "game/pokemon/poke_detail.c": [
        ('asm void fn_8004C120(void) { nofralloc\n    #include "asm/GC6E01/nonmatching/poke_detail/fn_8004C120.s"\n}', 'void fn_8004C120(void) {}'),
    ],
    "game/menu/menu_dialog.c": [
        ('asm void fn_80057B34(void) { nofralloc\n    #include "asm/GC6E01/nonmatching/menu_dialog/fn_80057B34.s"\n}', 'void fn_80057B34(void) { }'),
        ('asm void fn_80059034(void) { nofralloc\n    #include "asm/GC6E01/nonmatching/menu_dialog/fn_80059034.s"\n}', 'void fn_80059034(void) { }'),
    ],
    "game/fsys/fsys_load.c": [
        ('        entry->callback(entry->flag34, entry->callbackArg);', '        ((void(*)(u32, u32))entry->callback)(entry->flag34, entry->callbackArg);'),
    ],
    # Campaign Phase 1 (workflow wf_de8c288a): neutralize non-extern forward decls
    # the proto regexes (extern-only) miss, + one return-value cast.
    "game/gs_model.c": [
        ("extern void* fn_8005DA18(void);    /* linked list head */",
         "extern void* fn_8005DA18();    /* linked list head */"),
        ("extern void* fn_8005DA18(void*);", "extern void* fn_8005DA18();"),
    ],
    "game/late_game.c": [
        ("void fn_80093F64(void);", "void fn_80093F64();"),
    ],
    "game/menu/menu_exdisc2.c": [
        ("void fn_8007B114(void);", "void fn_8007B114();"),
        ("void fn_8007B6D8(void);", "void fn_8007B6D8();"),
    ],
    "game/menu/menu_middle.c": [
        ("void fn_80070D84(void);", "void fn_80070D84();"),
        ("    return fn_80129280(0x0, 0xe);", "    return (s32)fn_80129280(0x0, 0xe);"),
    ],
    "game/menu/menu_tool.c": [
        ("void fn_800756C8(void);", "void fn_800756C8();"),
    ],
    # gs_dvd.c: the decompiled GSDVD_CloseHandle / GSDVD_Open DEFINITIONS are typed
    # s32 but their bodies have bare `return;` (dropped r3) — clang -Wreturn-mismatch
    # is a hard error. They are definitions (not extern decls) so the proto rewriters
    # don't reach them; fix the return type textually (void is correct — they return
    # no value; cross-TU callers ignore return type at link).
    "game/gs_dvd.c": [
        ("s32 GSDVD_CloseHandle(u32 handleIndex, u32 mode) {",
         "void GSDVD_CloseHandle(u32 handleIndex, u32 mode) {"),
        ("s32 GSDVD_Open(u32 slotIndex, u32 resId, void* callback, u32 param1, u32 param2, u32 param3, u32 param4, u32 param5) {",
         "void GSDVD_Open(u32 slotIndex, u32 resId, void* callback, u32 param1, u32 param2, u32 param3, u32 param4, u32 param5) {"),
    ],
    "game/battle/battle_main.c": [
        # The file-scope `fn_800D3088` decl (line 79) has a void return but its
        # block-scoped re-decl (line 1223) + call site (`i += fn_800D3088()`) use a
        # u32 return. Line 79's trailing comment makes it regex-invisible to RETYPE,
        # so fix the return here. Unique prefix (line 1223 is `extern u32 ...`).
        ("extern void fn_800D3088(void);", "extern u32 fn_800D3088(void);"),
        # The scene render table is a real data OBJECT: the code uses both
        # `&lbl_80375CC8` (the table address) and `(u32)lbl_80375CC8`. The array
        # form makes both correct (lbl == &lbl == table addr); the decomp's lone
        # `void*` decl (line 324) was wrong. Its trailing comment makes it
        # regex-invisible to EXTERN_UNIFY, so fix it textually (the block-scoped
        # `extern u8 lbl_80375CC8[];` at line 817 already agrees).
        ("extern void* lbl_80375CC8;", "extern u8 lbl_80375CC8[];"),
    ],
    "hsd/hsd_jobj_display.c": [
        # Dead 0-arg call (after an unconditional `return`) to the clang builtin `memset`,
        # whose fixed 3-arg proto rejects it ("too few arguments"). Neutralize to a no-op.
        ("    memset();\n", "    /* pcport: dead 0-arg memset() removed */;\n"),
    ],
}

# Files that get the generic conflicting-prototype auto-unifier (see auto_unify()).
# Step-0 flip-harvest TUs (2026-06-10): each has 100s of block-scope extern decls that
# disagree across declaring functions — the class that got campaign TUs deferred.
AUTO_UNIFY_FILES = {
    "game/gs_field_world.c",
    "game/gs_worldmap.c",
    "game/effect/effect_util.c",
    "game/battle/battle_scene.c",
    "game/menu/menu_middle.c",   # pre-existing fn_8006B1C0 u32(s32)-vs-void() conflict
}

# Per-file: flipped #else bodies that are pseudo-register-broken (their CALL SITES omit
# arguments the callee's real prototype requires — on PPC the values were live in
# r3/r4, on x86 they'd be stack garbage). Emit a neutral stub instead of the broken C;
# identical semantics to the auto-stub link baseline. Populated iteratively from
# `too few arguments` compile errors. Map: rel-path -> set of fn names.
FLIP_AS_STUB = {
    # gs_field_world TODO-stub #else bodies: `(void)`-signature one-liners whose real
    # callers pass args (pseudo-register dispatchers). K&R-parens stubs accept every
    # call shape. These four are the TU's real decomp backlog (incl. the floor-
    # transition boss fn_8012640C and the object-update dispatcher fn_801254B4).
    "game/gs_field_world.c": {
        # (fn_8012640C, fn_801254B4 removed 2026-06-10 — functional #else decomps)
        # 10 batch-2 fns -> functional #else decomps 2026-06-10 (incl. fn_8012CA84)
        # arity-erroring flipped bodies (0-arg pseudo-register call sites):
        # (fn_8011BEB4, fn_8012A5B0 removed 2026-06-10 — functional #else decomps)
        # 0-arg residue call to the now-prototyped fn_8012A5B0:
        # 0-arg residue calls to the now-prototyped fn_801254B4:
        },
    "game/effect/effect_util.c": {
        "fn_801338A4", "fn_80135030", "fn_80136078",
    },
    # TODO-stub #else with (void) sig but real (jobj, joint) callers — K&R stub.
    "hsd/hsd_jobj.c": {
        "fn_801A0744",
    },
}

# Per-file: ACTIVE (non-wrapper) definitions whose bodies are pseudo-register
# transcriptions — byte-matched on PPC (implicit r3/r4 dataflow) but functionally
# broken on x86 (0-arg calls / lvalue casts / computed-goto jumptables). Their gen-copy
# bodies are replaced with a neutral K&R stub; identical to the auto-stub baseline.
# Populated from arity-class compile errors (tools/pcport_map_errors.py).
# Map: rel-path -> set of function names.
STUB_BODY = {
    # arity-class errors (0-arg pseudo-register calls / computed-goto jumptables) in
    # ACTIVE byte-matched bodies, + one CW lvalue-cast (fn_8011538C).
    "game/gs_field_world.c": {
        "GSfield_ObjectBatchUpdate", "GSfield_RenderPass",
        "GSfield_TransitionStateMachine",
        "fn_80119BD0", "fn_80123C54", "fn_80129280", "fn_80129840",
        "fn_80130660", "fn_80130770", "fn_80130890", "fn_801309A0",
        "fn_80130A88", "fn_80130BB0", "fn_8011538C",
    },
    "game/effect/effect_util.c": {
        "fn_80135D10", "_koukaOneExec__FUlPvPvPl",
    },
    # pseudo-register-param callees (u32 r3) only ever called 0-arg; K&R stub accepts
    # the residue-calls and drops the residue-dependent behavior (== PPC accident).
    "game/menu/menu_middle.c": {
        "fn_8006A7E8", "fn_8006A814",
    },
}

# Per-file: swap a definition's whole body in the GENERATED copy with a replacement
# file containing a complete new definition (real typed signature included). This is
# how functional decompilations of ACTIVE byte-matched transcription bodies land in
# the PC build without ever editing the byte-match source (mirrors STUB_BODY
# consumption). Names listed here are automatically EXCLUDED from STUB_BODY/KR_DEF at
# gen time, so stale stub-table entries cannot re-stub a function that has a real
# body. `asm` sibling definitions (the #if0-dead branch of a wrapper) are skipped.
# Map: rel-path -> { fn_name: replacement-body file path relative to repo ROOT }.
REPLACE_BODY = {}

# Per-file: definitions to convert to K&R parameter style — `ret f(u32 a, u8* b) {`
# becomes `ret f(a, b) u32 a; u8* b; {`. Keeps the REAL body (often a functional
# Ghidra import) while making the function accept any call arity (residue callers
# pass extra args; cdecl caller-pops makes that harmless on x86).
KR_DEF = {
    # active 1-param defs the (asm-faithful) fn_801254B4 dispatcher calls with 2 args;
    # K&R def accepts the extra arg (ignored — same as the PPC register behavior).
    "game/gs_field_world.c": {"fn_8011CF44", "fn_8011CF70"},
}

# External table overlay: tools/pcport_stub_tables.json (written by the harvest driver
# tools/pcport_harvest.py) merges INTO the in-file tables above. Format:
#   { "auto_unify": ["rel.c", ...],
#     "flip_as_stub": {"rel.c": ["fn", ...]},
#     "stub_body":    {"rel.c": ["fn", ...]} }
_TBL = ROOT / "tools" / "pcport_stub_tables.json"
if _TBL.exists():
    import json as _json
    _ext = _json.loads(_TBL.read_text())
    AUTO_UNIFY_FILES.update(_ext.get("auto_unify", []))
    def _merge(table, key):
        # An in-file entry emptied to comment-only braces parses as an empty DICT,
        # which .update(list) rejects — coerce every entry to a set before merging.
        for _k, _v in _ext.get(key, {}).items():
            cur = table.get(_k)
            if not isinstance(cur, set):
                table[_k] = cur = set(cur or ())
            cur.update(_v)
    _merge(FLIP_AS_STUB, "flip_as_stub")
    _merge(STUB_BODY, "stub_body")
    _merge(KR_DEF, "kr_def")
    for _k, _v in _ext.get("replace_body", {}).items():
        REPLACE_BODY.setdefault(_k, {}).update(_v)

# A standalone function PROTOTYPE line (declaration, not definition): begins with
# `extern`, names fn_X, has a parameter list, and ends with `;` (no body brace).
_FUNC_PROTO_RE = re.compile(
    r'^(?P<indent>\s*)extern\s+(?P<ret>[^;{}]*?)\b(?P<fn>fn_[0-9A-Fa-f]+)\s*\([^;{}]*\)\s*;\s*$'
)

# A standalone arg-less STUB prototype: `extern <ret> fn_X(void);` or `extern <ret> fn_X();`
# (empty or `void`-only parameter list). Used to drop redundant placeholder stubs when a
# real typed decl/definition exists elsewhere in the TU.
_FUNC_STUB_RE = re.compile(
    r'^\s*extern\s+[^;{}]*?\b(?P<fn>fn_[0-9A-Fa-f]+)\s*\(\s*(?:void\s*)?\)\s*;\s*$'
)

# Matches a line whose entire (trimmed) content is a single extern data-object
# declaration of one label, capturing leading indentation and the label name:
#   <indent>extern <type...> lbl_NAME;     or   <indent>extern <type...> lbl_NAME[];
# Deliberately excludes function decls (the label is immediately followed by `;` or
# `[..];`, never `(`).
_EXTERN_DECL_RE = re.compile(
    r'^(?P<indent>\s*)extern\s+[^;{}()]*?\b(?P<label>lbl_[0-9A-Fa-f]+)\s*(?:\[\s*\])?\s*;\s*$'
)


def unify_externs(lines, mapping):
    """Rewrite every standalone `extern ... lbl_X ...;` line whose label is in `mapping`
    to that label's canonical declaration, leaving all other lines (and all *uses* of the
    label inside expressions) untouched. Returns (lines, rewritten_count)."""
    out, rewrites = [], 0
    for ln in lines:
        m = _EXTERN_DECL_RE.match(ln)
        if m and m.group('label') in mapping:
            canon = mapping[m.group('label')]
            new = f"{m.group('indent')}{canon};"
            if new != ln:
                rewrites += 1
            out.append(new)
        else:
            out.append(ln)
    return out, rewrites


def neutralize_func_protos(lines, names):
    """Rewrite every standalone prototype `extern <ret> fn_X(<args>);` whose name is in
    `names` to the K&R empty-parens form `extern <ret> fn_X();`. Definitions (lines with a
    body `{`) and call sites are untouched. Returns (lines, rewritten_count)."""
    out, rewrites = [], 0
    for ln in lines:
        m = _FUNC_PROTO_RE.match(ln)
        if m and m.group('fn') in names:
            new = f"{m.group('indent')}extern {m.group('ret').strip()} {m.group('fn')}();"
            if new != ln:
                rewrites += 1
            out.append(new)
        else:
            out.append(ln)
    return out, rewrites


def retype_func_protos(lines, mapping):
    """Rewrite every standalone prototype line for a function in `mapping` to the supplied
    canonical signature. Only declaration lines (ending `;`, no body `{`) are touched;
    definitions are matched separately and skipped. Returns (lines, rewritten_count)."""
    out, rewrites = [], 0
    for ln in lines:
        m = _FUNC_PROTO_RE.match(ln)
        if m and m.group('fn') in mapping:
            indent = m.group('indent')
            new = f"{indent}extern {mapping[m.group('fn')]};"
            if new != ln:
                rewrites += 1
            out.append(new)
        else:
            out.append(ln)
    return out, rewrites


def kr_definitions(lines, names):
    """Convert the named functions' definitions to K&R parameter style."""
    changed = 0
    for idx, ln in enumerate(lines):
        if not ln or ln[0].isspace() or ln.rstrip().endswith(';'):
            continue
        m = _DEF_SIG_RE.match(ln)
        if not m or m.group('fn') not in names:
            continue
        if m.group('trail') is None and not _brace_follows(lines, idx):
            continue
        args = m.group('args').strip()
        if args in ('', 'void'):
            # no params: plain K&R parens
            lines[idx] = re.sub(r'\((?:\s*void\s*)?\)', '()', ln, count=1)
            changed += 1
            continue
        # split top-level commas, take the last identifier of each param as its name
        parts, depth, cur = [], 0, ''
        for ch in args:
            if ch == ',' and depth == 0:
                parts.append(cur); cur = ''
            else:
                if ch == '(':
                    depth += 1
                elif ch == ')':
                    depth -= 1
                cur += ch
        parts.append(cur)
        names_only, decls = [], []
        ok = True
        for p in parts:
            pm = re.search(r'([A-Za-z_]\w*)\s*(?:\[\s*\d*\s*\])?\s*$', p)
            if not pm:
                ok = False
                break
            names_only.append(pm.group(1))
            decls.append(p.strip() + ';')
        if not ok:
            continue
        trailer = ln[m.end('args'):].lstrip()  # ')' onward — preserve trailing '{'
        brace = ' {' if trailer.rstrip().endswith('{') else ''
        lines[idx] = (f"{m.group('ret')}{m.group('fn')}({', '.join(names_only)}) "
                      f"{' '.join(decls)}{brace}")
        changed += 1
    return lines, changed


_TYPE_ONLY_RE = re.compile(r'^(?:static\s+)?(?:const\s+)?[A-Za-z_][\w]*(?:\s*\*+)?\s*$')
_SIG_START_RE = re.compile(r'^[A-Za-z_]\w*\s*\(')


def join_split_sigs(lines):
    """Ghidra imports sometimes split a definition signature across two lines
    (`u32` newline `fn_X(void)`). Join them so the line-based passes see them."""
    out, i, joined = [], 0, 0
    while i < len(lines):
        if (lines[i] and not lines[i][0].isspace() and _TYPE_ONLY_RE.match(lines[i])
                and i + 1 < len(lines) and _SIG_START_RE.match(lines[i + 1])):
            out.append(lines[i].rstrip() + ' ' + lines[i + 1])
            joined += 1
            i += 2
            continue
        out.append(lines[i])
        i += 1
    return out, joined


def stub_bodies(lines, names):
    """Replace the DEFINITION body of each named function with a neutral K&R stub
    (`<ret> name() { [return 0;] }`). The definition is matched at file scope; its
    brace-balanced body is dropped. Returns (lines, stubbed_count)."""
    out, i, n, stubbed = [], 0, len(lines), 0
    while i < n:
        ln = lines[i]
        m = None
        if ln and not ln[0].isspace() and not ln.rstrip().endswith(';'):
            m = _DEF_SIG_RE.match(ln)
        if m and m.group('fn') in names and m.group('fn') not in _C_KEYWORDS and \
           (m.group('trail') is not None or _brace_follows(lines, i)):
            ret = ' '.join(m.group('ret').replace('static', '').split())
            name = m.group('fn')
            # advance to the opening brace, then to its balanced close
            j, depth, opened = i, 0, False
            while j < n:
                for ch in lines[j]:
                    if ch == '{':
                        depth += 1; opened = True
                    elif ch == '}':
                        depth -= 1
                if opened and depth == 0:
                    break
                j += 1
            out.append(f"{ret} {name}() {{")
            out.append("    /* pcport_gen STUB_BODY: pseudo-register transcription "
                       "body (x86-broken); neutral stub == auto-stub baseline */")
            out.append("}" if ret == "void" else "    return 0; }")
            stubbed += 1
            i = j + 1
            continue
        out.append(ln)
        i += 1
    return out, stubbed


def replace_bodies(lines, mapping):
    """Replace each named function's whole DEFINITION (signature line through the
    balanced closing brace) with the content of its replacement-body file, which
    carries its own — possibly different — signature. Skips `asm` definitions (the
    #if0-dead asm sibling of a wrapper) so the function is emitted exactly once.
    Returns (lines, replaced_count)."""
    out, i, n, replaced = [], 0, len(lines), 0
    done = set()
    while i < n:
        ln = lines[i]
        m = None
        if ln and not ln[0].isspace() and not ln.rstrip().endswith(';') \
           and not re.match(r'\s*asm\b', ln):
            m = _DEF_SIG_RE.match(ln)
        if m and m.group('fn') in mapping and m.group('fn') not in done and \
           m.group('fn') not in _C_KEYWORDS and \
           (m.group('trail') is not None or _brace_follows(lines, i)):
            name = m.group('fn')
            j, depth, opened = i, 0, False
            while j < n:
                for ch in lines[j]:
                    if ch == '{':
                        depth += 1; opened = True
                    elif ch == '}':
                        depth -= 1
                if opened and depth == 0:
                    break
                j += 1
            body_path = ROOT / mapping[name]
            out.append(f"/* pcport_gen REPLACE_BODY: functional decomp from "
                       f"{mapping[name]} replaces the pseudo-register transcription "
                       f"(generated copy only) */")
            out.extend(body_path.read_text(errors="replace").rstrip("\n").splitlines())
            done.add(name)
            replaced += 1
            i = j + 1
            continue
        out.append(ln)
        i += 1
    missing = set(mapping) - done
    if missing:
        print(f"  WARNING: REPLACE_BODY found no definition for: {sorted(missing)}")
    return out, replaced


def drop_func_stubs(lines, names):
    """Drop every standalone arg-less STUB prototype (`extern <ret> fn_X(void);` or
    `... fn_X();`) whose name is in `names`, replacing it with an explanatory comment so
    line-for-line readability is preserved. A real typed decl/definition for each name is
    expected to survive elsewhere in the TU. Returns (lines, dropped_count)."""
    out, dropped = [], 0
    for ln in lines:
        m = _FUNC_STUB_RE.match(ln)
        if m and m.group('fn') in names:
            indent = ln[:len(ln) - len(ln.lstrip())]
            out.append(f"{indent}/* pcport: redundant {m.group('fn')} stub proto dropped "
                       f"(real typed decl/def in TU) */")
            dropped += 1
        else:
            out.append(ln)
    return out, dropped


ASM_RE   = re.compile(r'^\s*asm\s')
ASM_FN_RE = re.compile(r'^\s*asm\s+[\w\s\*]*?\b(fn_[0-9A-Fa-f]+)\s*\(')
INC_RE   = re.compile(r'#\s*include\s+"[^"]*\.inc"')
IF1_RE   = re.compile(r'^\s*#\s*if\s+1\b')
ELSE_RE  = re.compile(r'^\s*#\s*else\b')
ENDIF_RE = re.compile(r'^\s*#\s*endif\b')
IF_RE    = re.compile(r'^\s*#\s*if')          # #if / #ifdef / #ifndef

# A C function-definition signature line: `<ret-tokens> name(args) {` (brace on this
# or the next line, never a trailing `;`). Used by FLIP_AS_STUB and auto-unify.
_DEF_SIG_RE = re.compile(
    r'^(?P<ret>(?:static\s+)?(?:const\s+)?[A-Za-z_][\w\s]*?[\s\*]+\**)'
    r'(?P<fn>[A-Za-z_]\w*)\s*\((?P<args>(?:[^;{}()]|\([^;{}()]*\))*)\)\s*(?P<trail>\{.*)?$')

_C_KEYWORDS = {"return", "if", "while", "for", "switch", "goto", "else", "do",
               "case", "sizeof", "typedef", "break", "continue"}


_RAW_ASM_RE = re.compile(r'^\s*asm\s+(?P<ret>[\w\s\*]+?)\s*\b(?P<fn>[A-Za-z_]\w*)\s*\([^)]*\)\s*\{')


def transform(lines, stub_fns=None):
    """Flip `#if 1 asm {...} #else <C> #endif` wrappers to their #else C. Wrappers whose
    fn name is in `stub_fns` (pseudo-register-broken #else bodies: call sites missing
    args) are emitted as a neutral stub built from the #else C signature instead —
    functionally identical to the auto-stub link baseline, never worse.
    RAW asm blocks (no #if guard, preprocessor-live) are emitted as neutral K&R stubs —
    clang can't assemble PPC; the stub equals the auto-stub link baseline."""
    stub_fns = stub_fns or set()
    out, i, n, flipped, stubbed = [], 0, len(lines), 0, 0
    pp = [True]  # preprocessor active-state stack for raw (non-wrapper) lines
    while i < n:
        s = lines[i].lstrip()
        if s.startswith('#if'):
            cond_active = not re.match(r'^#\s*if\s+0\b', s)
            pp.append(pp[-1] and cond_active)
        elif s.startswith('#else') and len(pp) > 1:
            pp[-1] = pp[-2] and not pp[-1]
        elif s.startswith('#endif') and len(pp) > 1:
            pp.pop()
        if pp[-1] and not IF1_RE.match(lines[i]):
            rm = _RAW_ASM_RE.match(lines[i])
            if rm:
                # consume the brace-balanced asm block, emit a neutral K&R stub
                j, depth, opened = i, 0, False
                while j < n:
                    for ch in lines[j]:
                        if ch == '{':
                            depth += 1; opened = True
                        elif ch == '}':
                            depth -= 1
                    if opened and depth == 0:
                        break
                    j += 1
                ret = ' '.join(rm.group('ret').split())
                out.append(f"{ret} {rm.group('fn')}() {{")
                out.append("    /* pcport_gen: raw asm block (preprocessor-live, no "
                           "#else C) -> neutral stub == auto-stub baseline */")
                out.append("}" if ret == "void" else "    return 0; }")
                stubbed += 1
                i = j + 1
                continue
        if IF1_RE.match(lines[i]):
            # scan the #if-1 branch (to its #else at depth 1); is it an asm wrapper?
            k, d, else_idx, has_asm, has_inc = i + 1, 1, None, False, False
            asm_fn = None
            while k < n and d >= 1:
                if ELSE_RE.match(lines[k]) and d == 1:
                    else_idx = k; break
                if IF_RE.match(lines[k]): d += 1
                elif ENDIF_RE.match(lines[k]): d -= 1
                if d == 1:
                    am = ASM_FN_RE.match(lines[k])
                    if am:
                        asm_fn = am.group(1)
                    if ASM_RE.match(lines[k]): has_asm = True
                    if INC_RE.search(lines[k]): has_inc = True
                k += 1
            if else_idx is not None and has_asm and has_inc:
                if asm_fn in stub_fns:
                    # find the #else C signature, emit a stub, skip to the #endif
                    k, d, sig = else_idx + 1, 1, None
                    while k < n and d >= 1:
                        if IF_RE.match(lines[k]): d += 1
                        elif ENDIF_RE.match(lines[k]):
                            d -= 1
                            if d == 0:
                                break
                        elif sig is None:
                            m = _DEF_SIG_RE.match(lines[k])
                            if m and m.group('fn') not in _C_KEYWORDS:
                                sig = m
                        k += 1
                    ret = ' '.join(sig.group('ret').split()) if sig else 'void'
                    name = sig.group('fn') if sig else asm_fn
                    # K&R empty parens: unspecified params, accepts every call shape
                    # (pseudo-register callers pass varying arg counts). Signature on
                    # its own brace-terminated line so auto_unify's defs-pass sees it.
                    out.append(f"{ret} {name}() {{")
                    out.append(f"    /* pcport_gen FLIP_AS_STUB: #else body pseudo-"
                               f"register-broken; neutral stub == auto-stub baseline */")
                    out.append("}" if ret == "void" else "    return 0; }")
                    i = k + 1
                    stubbed += 1
                    continue
                # emit only the #else branch (drop #if1/asm and the matching #endif)
                k, d = else_idx + 1, 1
                while k < n and d >= 1:
                    if IF_RE.match(lines[k]):
                        d += 1; out.append(lines[k])
                    elif ENDIF_RE.match(lines[k]):
                        d -= 1
                        if d == 0:
                            break          # drop the wrapper's closing #endif
                        out.append(lines[k])
                    else:
                        out.append(lines[k])
                    k += 1
                i = k + 1
                flipped += 1
                continue
        out.append(lines[i]); i += 1
    return out, flipped, stubbed


# CRT-provided functions: the decomp redeclares these with GC signatures (`extern f32
# sin(f32);`, K&R `extern void cos();`) that clash with the host CRT prototypes already
# in scope via pcport_compat.h's includes. Drop the decomp decls; the CRT wins (f32
# arguments promote to double fine).
_CRT_FNS = {"sin", "cos", "tan", "sqrt", "atan", "atan2", "fabs", "floor", "ceil",
            "pow", "fmod", "exp", "log", "abs", "memset", "memcpy", "memmove",
            "strlen", "strcpy", "strncpy", "strcmp", "strncmp", "sprintf", "printf",
            "sinf", "cosf", "tanf", "sqrtf", "atanf", "atan2f", "fabsf", "floorf",
            "ceilf", "powf", "fmodf", "expf", "logf"}

# A standalone single-function declaration line, any symbol name (not just fn_*):
# optional `extern`, return tokens, name, parameter list, trailing `;`.
_ANY_DECL_RE = re.compile(
    r'^(?P<indent>\s*)(?P<ext>extern\s+)?'
    r'(?P<ret>(?:const\s+|unsigned\s+|signed\s+|struct\s+\w+\s+)?[A-Za-z_]\w*(?:\s+\w+)*[\s\*]+\**)'
    r'(?P<fn>[A-Za-z_]\w*)\s*\((?P<args>(?:[^;{}()]|\([^;{}()]*\))*)\)\s*;\s*(?:/\*.*\*/\s*|//.*)?$')


def _brace_follows(lines, idx):
    """True if the next non-blank line after `idx` starts with '{' (or line idx itself
    ends with '{'). Ghidra-import bodies put a blank line between signature and brace."""
    if lines[idx].rstrip().endswith('{'):
        return True
    j = idx + 1
    while j < len(lines) and not lines[j].strip():
        j += 1
    return j < len(lines) and lines[j].lstrip().startswith('{')


def _strip_comments(text):
    """Remove /*...*/ and //... comments and string literals so structural scans
    (value-use detection) never match prose or quoted text."""
    text = re.sub(r'/\*.*?\*/', ' ', text, flags=re.S)
    text = re.sub(r'//[^\n]*', ' ', text)
    text = re.sub(r'"(?:[^"\\\n]|\\.)*"', '""', text)
    return text


def _value_used(text, name):
    """True if a call to `name` appears in a value context anywhere in the TU text
    (assignment/initializer RHS, condition, argument, cast operand, return expr).
    `text` must be comment-stripped (_strip_comments)."""
    return re.search(
        r'(?:[=!<>+\-*/%&|^,?:(\[]|\breturn)\s*(?:\([\w\s\*]+\)\s*)*'
        + re.escape(name) + r'\s*\(', text) is not None


def auto_unify(lines):
    """Generic conflicting-prototype unifier for campaign-tail TUs (hundreds of
    block-scope extern decls that disagree per declaring function; hand-tables don't
    scale). Policy: CRT names -> decl dropped; `jumptable_*` function decls -> dropped
    (they're data objects; computed-goto transcriptions referencing them get stubbed);
    symbol DEFINED in this TU -> decls unified to K&R with the definition's return
    (exact prototype when params are promotion-unsafe); undefined symbol -> K&R with
    majority non-void return; all-void undefined symbol whose call RESULT is used
    anywhere in the TU (CW tolerated value-use of void calls) -> retyped to u32.
    Returns (lines, rewritten_count)."""
    text = _strip_comments("\n".join(lines))
    # pass 1: in-TU definitions (file scope: no leading whitespace, brace-started body)
    defs = {}
    for idx, ln in enumerate(lines):
        if not ln or ln[0].isspace() or ln.rstrip().endswith(';'):
            continue
        m = _DEF_SIG_RE.match(ln)
        if not m or m.group('fn') in _C_KEYWORDS:
            continue
        if m.group('trail') is not None or _brace_follows(lines, idx):
            ret = ' '.join(m.group('ret').replace('static', '').split())
            defs[m.group('fn')] = (ret, m.group('args').strip(), idx)
    # pass 2: declarations per symbol
    decls = {}
    for idx, ln in enumerate(lines):
        m = _ANY_DECL_RE.match(ln)
        if not m or m.group('fn') in _C_KEYWORDS:
            continue
        if not m.group('ext') and m.group('indent'):
            continue  # indented non-extern: not a decl this pass should touch
        ret = ' '.join(m.group('ret').split())
        decls.setdefault(m.group('fn'), []).append(
            (idx, m.group('indent'), ret, m.group('args').strip()))
    changed = 0
    for name, ds in decls.items():
        if name in _CRT_FNS or name.startswith("jumptable_"):
            why = "CRT" if name in _CRT_FNS else "data-object jumptable"
            for idx, indent, _, _ in ds:
                lines[idx] = (f"{indent}/* pcport_gen auto-unify: {why} {name} "
                              f"fn-redeclaration dropped */")
                changed += 1
            continue
        sigs = {(r, a) for _, _, r, a in ds}
        if name in defs:
            dret, dargs, didx = defs[name]
            if dret == 'void' and _value_used(text, name):
                # CW dropped-return artifact: the matched definition is typed void but
                # callers read the call result (r3 residue on PPC). Retype the gen-copy
                # definition to u32 so value-use sites compile; missing return paths
                # leave garbage EAX — the same accident the PPC binary exhibits.
                dret = 'u32'
                lines[didx] = re.sub(r'^(static\s+)?void\b', r'\1u32', lines[didx], count=1)
                changed += 1
            # Promotion-unsafe params (float/char/short) make a K&R `()` decl
            # incompatible with the prototyped definition — must match it exactly.
            # Otherwise prefer K&R: the matched decomp deliberately leaves 0-arg
            # pseudo-register call sites (CW-legal through `()` decls); an exact
            # prototype would turn every one into an arity error.
            # Variadic definitions also require an exact prototype: a K&R `()`
            # declaration is incompatible with a `...` definition in C.
            unsafe = (re.search(r'\b(f32|f64|float|double|u8|s8|u16|s16|char|short)\b'
                                r'(?!\s*\*)', dargs) is not None
                      or '...' in dargs)
            canon = (f"extern {dret} {name}({dargs});" if unsafe
                     else f"extern {dret} {name}();")
            for idx, indent, _, _ in ds:
                new = f"{indent}{canon}"
                if lines[idx] != new:
                    lines[idx] = new
                    changed += 1
        else:
            rets = [r for _, _, r, _ in ds]
            nonvoid = [r for r in rets if r != 'void']
            if not nonvoid:
                # all decls say void: retype to u32 if the call RESULT is used
                # anywhere (CW tolerated value-use of void calls; clang errors).
                # A u32 return on a genuinely-void fn is harmless on x86 cdecl.
                if not _value_used(text, name):
                    if len(sigs) <= 1:
                        continue
                    ret = 'void'
                else:
                    ret = 'u32'
            else:
                if len(sigs) <= 1:
                    continue
                ret = max(set(nonvoid), key=nonvoid.count)
            for idx, indent, _, _ in ds:
                new = f"{indent}extern {ret} {name}();"
                if lines[idx] != new:
                    lines[idx] = new
                    changed += 1
    # defined void fns that are value-used but have no separate decls in the TU
    for name, (dret, dargs, didx) in defs.items():
        if dret == 'void' and name not in decls and _value_used(text, name) \
           and re.match(r'^(static\s+)?void\b', lines[didx]):
            lines[didx] = re.sub(r'^(static\s+)?void\b', r'\1u32', lines[didx], count=1)
            changed += 1
    return lines, changed


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("files", nargs="+")
    ap.add_argument("--out-dir", default="build_pc/gen")
    a = ap.parse_args()
    out_root = ROOT / a.out_dir
    for f in a.files:
        src = (ROOT / f) if not Path(f).is_absolute() else Path(f)
        rel = src.resolve().relative_to(ROOT / "src")
        lines = src.read_text(errors="replace").splitlines()
        gen, flipped, stubbed = transform(lines, FLIP_AS_STUB.get(rel.as_posix()))
        if rel.as_posix() in AUTO_UNIFY_FILES:
            gen, _j = join_split_sigs(gen)
        replace_map = REPLACE_BODY.get(rel.as_posix()) or {}
        replaced_bodies = 0
        if replace_map:
            gen, replaced_bodies = replace_bodies(gen, replace_map)
        body_names = STUB_BODY.get(rel.as_posix())
        if body_names:
            # functions with a real replacement body must never be re-stubbed by
            # stale stub-table entries
            body_names = set(body_names) - set(replace_map)
            gen, body_stubbed = stub_bodies(gen, body_names)
            stubbed += body_stubbed
        auto_uni = 0
        if rel.as_posix() in AUTO_UNIFY_FILES:
            gen, auto_uni = auto_unify(gen)
        # AFTER auto_unify: K&R-converted defs are invisible to its defs-pass, so the
        # decl unification must see the original prototyped definitions first.
        kr_names_def = KR_DEF.get(rel.as_posix())
        if kr_names_def:
            kr_names_def = set(kr_names_def) - set(replace_map)
            gen, _krd = kr_definitions(gen, kr_names_def)
        unify_map = EXTERN_UNIFY.get(rel.as_posix())
        rewrites = 0
        if unify_map:
            gen, rewrites = unify_externs(gen, unify_map)
        kr_names = FUNC_PROTO_KR.get(rel.as_posix())
        kr = 0
        if kr_names:
            gen, kr = neutralize_func_protos(gen, kr_names)
        retype_map = FUNC_PROTO_RETYPE.get(rel.as_posix())
        retyped = 0
        if retype_map:
            gen, retyped = retype_func_protos(gen, retype_map)
        drop_names = FUNC_STUB_DROP.get(rel.as_posix())
        dropped = 0
        if drop_names:
            gen, dropped = drop_func_stubs(gen, drop_names)
        dst = out_root / rel
        dst.parent.mkdir(parents=True, exist_ok=True)
        header = (f"/* GENERATED by tools/pcport_gen.py from src/{rel.as_posix()} — "
                  f"{flipped} asm wrapper(s) replaced with their #else C; "
                  f"{replaced_bodies} body(ies) replaced with functional decomps; "
                  f"{stubbed} broken body(ies) stubbed; {auto_uni} decl(s) auto-unified; "
                  f"{rewrites} conflicting data extern(s) unified; "
                  f"{kr} func proto(s) neutralized; {retyped} retyped; "
                  f"{dropped} stub proto(s) dropped. DO NOT EDIT. */\n")
        pre = PREAMBLE.get(rel.as_posix(), "")
        text = header + pre + "\n".join(gen) + "\n"
        fixes = 0
        for old, new in TEXT_FIXUPS.get(rel.as_posix(), []):
            if old in text:
                text = text.replace(old, new)
                fixes += 1
        dst.write_text(text)
        print(f"  {rel.as_posix():<28} flipped {flipped}, stubbed {stubbed}, "
              f"auto-unified {auto_uni}, "
              f"unified {rewrites} data extern(s), {kr} kr-proto(s), "
              f"{retyped} retype(s), {dropped} stub-drop(s), {fixes} text fixup(s) -> "
              f"{dst.relative_to(ROOT).as_posix()}")


if __name__ == "__main__":
    main()
