# Historical MusyX and HSD candidates

These 18 C files were retired from `src/` after checking the complete
source-owner report at `0275f31498f43ad807d4c91fa2da95ede5f5291b` and recursively
following quoted source/header includes from every active owner. None is an
active build input, directly or through another source file. Their contents
are preserved unchanged as research history; old includes and comments retain
the paths used by that historical campaign.

The six MusyX wrappers and `musyx_range_80157280.c` belonged to the former
centralized candidate implementation. The active pitch helpers that prompted
this cleanup are in [`src/musyx/runtime/synth_ac.c`](../../../src/musyx/runtime/synth_ac.c),
which owns both functions, its semitone tables and its small constant pool.
Other current implementations are selected individually by `configure.py`.

The HSD files here are superseded candidates, pools and older aggregate
implementations. They are distinct from the active `jobj.c`, `video.c`,
`shadow.c`, `texpdag.c`, and other owners in `src/hsd/`. Historical symbol-map
proposals, the old root report, and old campaign worklists may still mention
their former paths. Those records do not define the current build.

Use `configure.py` and `build/GC6E01/report.json` to identify active sources.
An address-based filename alone is not a reason to archive a file: some such
files remain genuine, complete source owners until their whole-unit
replacement has passed the matching checks.

See [the library cleanup record](../../../docs/LIBRARY_CLEANUP.md).
