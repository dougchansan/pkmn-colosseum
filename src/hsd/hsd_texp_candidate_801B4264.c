#include "dolphin/types.h"
#include "hsd/hsd_objalloc.h"

typedef struct HSD_RenderAllocDataGroup {
    HSD_ObjAllocData chan;
    HSD_ObjAllocData tevReg;
    HSD_ObjAllocData render;
} HSD_RenderAllocDataGroup;

extern u8 lbl_80465728[];
extern void HSD_ObjAllocInit(void* list, u32 size, u32 alignment);

void HSD_RenderInitAllocData(void)
{
    u8* base;

    base = lbl_80465728;
    HSD_ObjAllocInit(base + 0x58, 0x1C, 4);
    HSD_ObjAllocInit(base + 0x2C, 0x14, 4);
    HSD_ObjAllocInit(base, 0x30, 4);
}
