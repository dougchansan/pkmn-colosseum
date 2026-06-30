/* bside_shim.h - minimal type/decl shim so REAL src/ function bodies compile
 * standalone in the oracle harness, with NO edits to src/.
 *
 * The harness #includes the verbatim source text of the target function and
 * its callees (extracted by line-range from src/, never retyped). This shim
 * supplies the types and the externs those bodies reference.
 */
#ifndef ORACLE_BSIDE_SHIM_H
#define ORACLE_BSIDE_SHIM_H

#include <stdint.h>
#include <stddef.h>

/* The harness includes DolRecomp's core/cpu.h FIRST, which already provides
 * u8/u16/u32/u64/s8/s16/s32/s64/f32/f64 via core/types.h. We deliberately do
 * NOT re-typedef them here (that conflicts on LP64, where u64==unsigned long).
 * This header only supplies what the src bodies need beyond those types. */

#ifndef NULL
#define NULL ((void*)0)
#endif

#endif /* ORACLE_BSIDE_SHIM_H */
