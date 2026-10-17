/**
 * @file wazaViewer_exact_801D53D8.c
 * @brief _wazaViewerInitialize, 0x801D53D8 - 0x801D5464.
 *
 * Function-boundary carve of the wazaViewer TU (see wazaViewer.c): set up
 * the waza system and the viewer, hide resource model 0x64, start the
 * viewer thread and record whether the game runs in viewer mode 2. No jump
 * table, no pooled constant; its data is the viewer's .bss state
 * lbl_804673F8, kept extern, and the address of wazaViewerThread. GC/1.3
 * -O4,p like the TU's linked carves, no pragmas.
 */
#include "game/battle/battle_waza_types.h"

extern struct GSmodel* GSresGetResource(u32 group, u32 handle);
extern s32 GSthreadCreate(s32, s32, s32, s32, s32, void*);
extern s32 GSgetVersionMinor(void);

void _wazaViewerInitialize(void) {
    struct GSmodel* model;

    wazaSequenceSysInit(8);
    fn_801D58E4();
    model = GSresGetResource(0, 0x64);
    if (model != 0) {
        GSmodelSetVisibility(model, 0);
    }
    *(s32*)(lbl_804673F8 + 0x66C) = GSthreadCreate(0x14, 0x58, 0x2000, 1, 0, wazaViewerThread);
    *(u32*)lbl_804673F8 = GSgetVersionMinor() != 2;
}
