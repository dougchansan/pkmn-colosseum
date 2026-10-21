/**
 * @file people_candidate_8018F4C8.c
 * @brief peopleInfoGetMotion, 0x8018F4C8 - 0x8018F5B4, with its switch jump table
 *        (.data 0x8036C540 - 0x8036C564).
 *
 * Standalone source for the split range; the body is the one previously
 * reached through the people.c include wrapper. Looks up the motion node
 * for a motion slot in a people-info entry and reports whether it loops.
 */
#include "dolphin/types.h"

void peopleInfoGetMotion(void* entry, u8 param, s32* outNode, u8* outResult) {
    s8* data = entry;

    if (data == NULL) {
        return;
    }
    switch (param) {
    case 1:
        *outNode = data[1];
        *outResult = 1;
        break;
    case 2:
        *outNode = data[2];
        *outResult = 1;
        break;
    case 3:
        *outNode = data[3];
        *outResult = 1;
        break;
    case 4:
        *outNode = data[4];
        *outResult = 0;
        break;
    case 5:
        *outNode = data[1];
        *outResult = 1;
        break;
    case 6:
        *outNode = data[6];
        *outResult = 0;
        break;
    case 7:
        *outNode = data[7];
        *outResult = 1;
        break;
    case 8:
        *outNode = data[8];
        *outResult = 0;
        break;
    }
}
