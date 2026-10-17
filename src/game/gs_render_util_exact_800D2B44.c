#include "dolphin/types.h"
#include "hsd/hsd_aobj.h"

extern void __assert(const char* file, u32 line, const char* condition);
/* These retail string symbols live in .sdata2; the storage declaration keeps
 * their canonical SDA21 relocations instead of inventing private literals. */
extern const char lbl_8047C9DC[] __attribute__((section(".sdata2")));
extern const char lbl_8047C9E4[] __attribute__((section(".sdata2")));
extern f32 lbl_8047AA78;
extern f32 lbl_8047C990;

void _GScameraAnimCallback(HSD_AObj* aobj)
{
    if (aobj == NULL) {
        __assert(lbl_8047C9DC, 0xAB, lbl_8047C9E4);
    }
    lbl_8047AA78 = lbl_8047C990 + aobj->end_frame;
}
