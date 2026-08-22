#include "dolphin/types.h"

s32 itemParamGetRecoverType(u8* itemParam)
{
    s32 selectedType;
    s32 selectedCount;
    s32 returnType;
    u32 bitValue;
    u8 firstGate;
    u8 statusNibble;
    u8 byteFlag;

    if (itemParam == NULL) {
        return 0x16;
    }

    if (itemParam == NULL) {
        firstGate = 0;
    } else {
        bitValue = (itemParam[0] >> 5) & 1;
        firstGate = ((-bitValue | bitValue) >> 31);
    }
    if (firstGate) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = (itemParam[0] >> 1) & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = (itemParam[1] >> 4) & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = itemParam[1] & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = (itemParam[2] >> 4) & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        statusNibble = 0;
    } else {
        statusNibble = itemParam[2] & 0xF;
    }
    if (statusNibble) {
        goto returnZero;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 7) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (!byteFlag) {
        goto afterReturnZero;
    }
returnZero:
    return 0;
afterReturnZero:

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[0] >> 6) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 1;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 6) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 2;
    }

    selectedCount = 0;

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 5) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 3;
        selectedCount = 1;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 4) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 4;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 3) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 5;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 2) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 6;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[3] >> 1) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 7;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = itemParam[3] & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 8;
        selectedCount++;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[0] >> 7) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        selectedType = 9;
        selectedCount++;
    }

    if (selectedCount > 0) {
        returnType = selectedType;
        if (selectedCount > 1) {
            returnType = 0xA;
        }
        return returnType;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xA];
    }
    if (byteFlag) {
        return 0xB;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[9];
    }
    if (byteFlag) {
        return 0xC;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[8];
    }
    if (byteFlag) {
        return 0xD;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xF];
    }
    if (byteFlag) {
        return 0xE;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xE];
    }
    if (byteFlag) {
        return 0xF;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xD];
    }
    if (byteFlag) {
        return 0x10;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xC];
    }
    if (byteFlag) {
        return 0x11;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 4) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 0x12;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 7) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 0x13;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 3) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        return 0x14;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        bitValue = (itemParam[4] >> 5) & 1;
        byteFlag = ((-bitValue | bitValue) >> 31);
    }
    if (byteFlag) {
        goto returnFifteen;
    }

    if (itemParam == NULL) {
        byteFlag = 0;
    } else {
        byteFlag = itemParam[0xB];
    }
    if (!byteFlag) {
        goto returnSixteen;
    }

returnFifteen:
    return 0x15;
returnSixteen:
    return 0x16;
}
