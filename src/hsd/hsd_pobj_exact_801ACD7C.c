#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"

extern void fn_800B7D3C(void);
extern HSD_VtxDescList* lbl_8047B2FC;
extern HSD_VtxDescList* lbl_8047B300;

void HSD_ClearVtxDesc(void)
{
    fn_800B7D3C();
    lbl_8047B2FC = 0;
    lbl_8047B300 = NULL;
}
