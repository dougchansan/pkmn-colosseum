/* pcport_compat.h — CodeWarrior/PowerPC intrinsics + SDK decls used by the
 * decompiled game/HSD C, mapped to host equivalents for the PC (x86) build.
 *
 * Force-included (clang/MSVC `-include`) only in the PCPORT build so the
 * decompiled C compiles natively. The CodeWarrior matching build never sees it.
 */
#ifndef PCPORT_COMPAT_H
#define PCPORT_COMPAT_H
#ifdef PCPORT

#include <math.h>
#include <stdarg.h>
/* NOTE: do NOT force-include <string.h>/<stdio.h> — several decompiled TUs declare
 * their own memcpy/memset/OSReport with (slightly different) signatures, which a
 * standard header conflicts with. Such TUs need a targeted per-file include/decl. */

/* --- PowerPC / CodeWarrior intrinsics --- */
static double  __fabs(double x)        { return fabs(x); }
static float   __fabsf(float x)        { return (float)fabs(x); }
static int     __cntlzw(unsigned int x){ return x ? __builtin_clz(x) : 32; }

/* NOTE: do NOT declare OSReport or fn_ helpers here — the individual TUs declare
 * them with their own (sometimes varying) signatures; a generic decl conflicts.
 * The handful of TUs that lack a local decl get a targeted one instead. */

#endif /* PCPORT */
#endif /* PCPORT_COMPAT_H */
