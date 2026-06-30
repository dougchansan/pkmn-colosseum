# TU attribution from __FILE__ string xrefs

Ground truth: every function referencing a `*.c` string literal was compiled from that file (CodeWarrior `__FILE__`). Filename folding treats `cobj.c`≡`hsd_cobj.c`, `DVD.c`≡`dvd.c`, etc.

- ✅ **match** — string evidence agrees with the assigned source.
- 🔀 **relabel** — range assigned the wrong file; code is a different TU.
- ✂️ **split** — range spans 2+ source files; needs a split point.

## Range cross-check (existing splits vs. string evidence)

| range | status | assigned src | .c strings referenced | finding |
|---|---|---|---|---|
| `0x800055e0`-`0x80006630` | KNOWN | main.c | `error.c`×2 | 🔀 relabel (assigned main.c but code is error.c) |
| `0x80033278`-`0x80034280` | KNOWN | menu_carde.c | `menuCardE.c`×1 | ✅ match |
| `0x80069a60`-`0x80069c0c` | KNOWN | menu_battle.c | `menuCB_Battle.c`×1 | ✅ match |
| `0x8007109c`-`0x8007162c` | KNOWN | menu_common.c | `menuCB_Common.c`×5 | ✅ match |
| `0x8007581c`-`0x80075a34` | LIKELY | menu_tool_battle.c | `menuToolBattle.c`×1 | ✅ match |
| `0x800767b8`-`0x80077a5c` | LIKELY | menu_rule.c | `menuCB_Rule.c`×4 | ✅ match |
| `0x80077ed4`-`0x80078390` | LIKELY | menu_exdisc_shrine.c | `menuExDiscShrine.c`×1 | ✅ match |
| `0x800792d8`-`0x800798e8` | LIKELY | menu_exdisc_coupon.c | `menuExDiscCoupon.c`×1 | ✅ match |
| `0x8007c2c0`-`0x8007c300` | KNOWN | menu_pokecoupon.c | `menuPokeCoupon.c`×1 | ✅ match |
| `0x8007c300`-`0x8007fd64` | LIKELY | menu_carde_matrix.c | `menuCardE_Matrix.c`×4 | ✅ match |
| `0x80082650`-`0x80083af4` | KNOWN | save_data.c | `cardesavedata.c`×11 | ✅ match |
| `0x80089048`-`0x800895a4` | KNOWN | pokeconv.c | `pokeconv.c`×2 | ✅ match |
| `0x80092c90`-`0x800937f4` | KNOWN | gba_comm.c | `gbaCommunication.c`×6 | ✅ match |
| `0x800a4d28`-`0x800a5624` | GAP | ??? | `dvdfs.c`×4 |  |
| `0x800a5624`-`0x800a7820` | KNOWN | DVD.c | `dvd.c`×2 | ✅ match |
| `0x800a8178`-`0x800aa430` | GAP | ??? | `vi.c`×1 |  |
| `0x800e3604`-`0x800ee2c8` | GAP | ??? | `parse.c`×1 |  |
| `0x80167040`-`0x80168c64` | KNOWN | gs_dvd.c | `GSdvd.c`×1 | ✅ match |
| `0x80168c64`-`0x80169104` | KNOWN | pslist.c | `pslist.c`×2 | ✅ match |
| `0x8016f430`-`0x80172630` | KNOWN | psinterpret.c | `psinterpret.c`×1 | ✅ match |
| `0x8017424c`-`0x8017572c` | KNOWN | generator.c | `generator.c`×1 | ✅ match |
| `0x801914f4`-`0x801920e4` | KNOWN | hsd_wobj.c | `wobj.c`×3 | ✅ match |
| `0x801938fc`-`0x80193c24` | KNOWN | hsd_class.c | `class.c`×1 | ✅ match |
| `0x80193c24`-`0x80197a64` | KNOWN | hsd_cobj.c | `cobj.c`×18 | ✅ match |
| `0x80197a64`-`0x80198f7c` | KNOWN | hsd_displayfunc.c | `displayfunc.c`×2 | ✅ match |
| `0x80198f7c`-`0x8019b7c0` | KNOWN | hsd_dobj.c | `fobj.c`×4, `dobj.c`×2 | ✂️ split (also holds: fobj.c) |
| `0x8019b7c0`-`0x8019c690` | KNOWN | hsd_fog.c | `fog.c`×2, `hash.c`×1, `id.c`×1 | ✂️ split (also holds: hash.c, id.c) |
| `0x8019c690`-`0x8019ce50` | KNOWN | hsd_initialize.c | `initialize.c`×4 | ✅ match |
| `0x8019ce50`-`0x801a4000` | KNOWN | hsd_jobj.c | `jobj.c`×13, `list.c`×2 | ✂️ split (also holds: list.c) |
| `0x801a4000`-`0x801a69c0` | KNOWN | hsd_lobj.c | `lobj.c`×7 | ✅ match |
| `0x801a69c0`-`0x801a6a34` | GAP | ??? | `memory.c`×1 |  |
| `0x801a6a34`-`0x801aa608` | KNOWN | hsd_mobj.c | `mobj.c`×6, `mtx.c`×2, `perf.c`×1 | ✂️ split (also holds: mtx.c, perf.c) |
| `0x801aa608`-`0x801b019c` | KNOWN | hsd_pobj.c | `robj.c`×12, `pobj.c`×10 | ✂️ split (also holds: robj.c) |
| `0x801b019c`-`0x801b1730` | LIKELY | hsd_shadow.c | `shadow.c`×10 | ✅ match |
| `0x801b1730`-`0x801bbac8` | GAP | ??? | `texp.c`×15, `tev.c`×5 |  |
| `0x801bbac8`-`0x801bf138` | KNOWN | hsd_tobj.c | `tobj.c`×14, `util.c`×1 | ✂️ split (also holds: util.c) |
| `0x801bf138`-`0x801c0000` | LIKELY | hsd_robj.c | `video.c`×3 | 🔀 relabel (assigned hsd_robj.c but code is video.c) |
| `0x801c0000`-`0x801c3108` | GAP | ??? | `aobj.c`×3 |  |

## GAP-range attribution proposals

Functions in unattributed GAP regions that reference a `*.c` literal — these regions can be split to that TU.

- **`texp.c`** — 15 GAP function(s), `0x801b4300`..`0x801b7178` (e.g. fn_801B4300, fn_801B4614, fn_801B4B5C, fn_801B50C0, …)
- **`tev.c`** — 5 GAP function(s), `0x801b3338`..`0x801b3998` (e.g. fn_801B3338, fn_801B3408, fn_801B37A0, fn_801B38C0, …)
- **`dvdfs.c`** — 4 GAP function(s), `0x800a4d28`..`0x800a5558` (e.g. fn_800A4D28, fn_800A532C, fn_800A541C, fn_800A5558)
- **`aobj.c`** — 3 GAP function(s), `0x801c028c`..`0x801c2670` (e.g. fn_801C028C, fn_801C0F20, fn_801C2670)
- **`vi.c`** — 1 GAP function(s), `0x800a94ac`..`0x800a94ac` (e.g. fn_800A94AC)
- **`parse.c`** — 1 GAP function(s), `0x800eafe4`..`0x800eafe4` (e.g. fn_800EAFE4)
- **`memory.c`** — 1 GAP function(s), `0x801a69c0`..`0x801a69c0` (e.g. fn_801A69C0)
