/**
 * @file gs_range_8017FA5C_exact.c
 * @brief Exact free-space walk, 0x8017FA5C - 0x8017FB08.
 *
 * Carved from the gs small-block heap's level-0 retail translation unit.
 * The following gsSmallHeapFree remains a CodeCandidate in its own object.
 */
#include "game/gs_range_8017FA5C_shared.h"

u32 fn_8017FA5C(void)
{
    GsRangeMemNode* node;
    u32 sum;
    s32 count;
    GsRangeMemNode* head;

    sum = 0;
    /* RULE-EXCEPTION(title-path): dead store for retail's first li r28,0; see docs/RULE_EXCEPTIONS.md */
    count = 0;
    head = lbl_8047B1D0;
    /* RULE-EXCEPTION(title-path): self-assignment for register priority; see docs/RULE_EXCEPTIONS.md */
    head = head;
    if (!lbl_8047B1D0) {
        sum = lbl_80455048.remaining;
    } else {
        count = 0;
        node = head->next;
        for (;;) {
            count++;
            if ((u32)node <= 0x80000000) {
                return sum;
            }
            if (node) {
                sum += node->size;
            }
            if (node == lbl_8047B1D0) {
                break;
            }
            node = node->next;
        }
        sum += lbl_80455048.remaining;
    }
    return sum;
}
