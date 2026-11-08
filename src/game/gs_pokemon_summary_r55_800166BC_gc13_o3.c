/* RULE-EXCEPTION(user-approved): constant_import (temporary) — remove when gs_pokemon_summary.c links as one unit — see docs/RULE_EXCEPTIONS.md */
/*
 * Exact gs_pokemon_summary source island 0x800166BC - 0x800167D0
 * (bagMenuDrawCursor5). Its float literals (2*pi, pi, -pi) and the signed
 * int-to-float bias are the TU's shared .sdata2 literals at
 * lbl_8047B758/75C/760/768 (src/game/data/sdata2_8047B740.c); the
 * temporary constant_import build step redirects the island's private
 * copies to them.
 */
#define GS_POKEMON_SUMMARY_800166BC_ONLY
#include "src/game/gs_pokemon_summary.c"
