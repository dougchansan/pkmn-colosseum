/**
 * @file gs_range_8000BE74.c
 * @brief gs-engine, 0x8000BE74 - 0x8000CA34.
 */
#include "dolphin/types.h"

typedef struct FloorLinkEntry {
    s32 triggerId;
    u32 floorId;
} FloorLinkEntry;

typedef struct FloorDebugData {
    u8 pad_00[0x34];
    u32 field_34;
} FloorDebugData;

extern FloorLinkEntry lbl_802E28F0[];
extern u32 lbl_802666B0[];
extern const f32 lbl_8047B6E0;

extern u8 lbl_8047A298;
extern u8 lbl_8047A299;

extern u32* lbl_80478E10;
extern u16* lbl_80478E14;
extern u32* lbl_80478E18;
extern u16* lbl_80478E1C;
extern u32* lbl_80478E20;
extern u16* lbl_80478E24;
extern u32* lbl_80478E28;
extern u16* lbl_80478E2C;

extern u32 fn_800F7BC4(u32 flags);
extern u8 menuSelectInput(u32 index, u32* out);
extern void floorLink(u32 floorId, u32 arg);

extern u8 fn_800FF52C(void);
extern void soundStartSE(u32 id);
extern void GSmsgPrintf(u32 x, u32 y, s32 color, ...);
extern void _threadSwitch(void);

extern u8 menuIsCheck(s32 menuId);
extern void menuClose(s32 menuId);
extern s32 menuOpenCustom(s32 menuId, ...);
extern void menuSetPosition(s32 menuId, s32 x, s32 y);
extern s32 menuOpen(s32 menuId, s32 arg);

extern u8 fn_80175FFC(void);
extern void GSgfxCaptureMovieStop(void);
extern void GSgfxCaptureMovieStart(u32 mode);

extern u8 vtrGetDisp(void);
extern void vtrSetDisp(u8 index, u8 active);
extern void vtrSetMode4(void);
extern void vtrSetMode3(void);
extern void vtrSetMode1(void);
extern void vtrSetMode2(void);
extern u8 vtrIsActive(void);
extern void vtrActivate(void);
extern void vtrDeactivate(void);

extern void fn_801D0748(u32 a, u32 b, u32 c);
extern void winMsgOpen(u32 a, u32 msgId, u32 c, u32 d);
extern void winMsgClose(u32 id);
extern void fadeSet(u32 mode, f32 frames);
extern void fadeCheck(u32 mode);
extern void winMsgOpenError(u32 msgId, u32 a, u32 b);
extern void winMsgCloseFight(u32 id);
extern void winMsgCloseError(u32 id);
extern void msgctrlSetValue(u32 id, u32 value);
extern void winMsgOpenField(u32 msgId, u32 a, u32 b);
extern void menuSubKeyWait(void);
extern void winMsgCloseField(u32 id);

extern u32 std092_getCurrentRoomID(void);
extern FloorDebugData* floorDataBiosGetPtr(u32 id);
extern u8 fn_801174C4(void);
extern void fn_80117500(void);
extern void fn_800FEC34(u32 id);
extern void std136_fieldClearFlag_8047ad71(void);
extern void fn_800FECB8(u32 id);

extern void fn_80166D18(u32 a, u32 b, u32 c, u32 d);
extern s32 GSsndGetStatus(u16 entry);
extern void soundKill(u16 entry);
extern void std105_soundPlayEffect(u16 entry, u32 a, u32 b);
extern void std104_soundSetBackgroundMusic(u16 entry, u32 a, u32 b);

/* Retail retains this otherwise-unreferenced debug-menu entry-point group. */
#pragma force_active on

s32 dbgMenuWarpToRoom(s32 arg) {
    u32 linkArg = 0;
    s32 i;

    if ((fn_800F7BC4(1) & 0x20) != 0) {
        if (menuSelectInput(0, &linkArg) == 0) {
            return -1;
        }
    }

    for (i = 0; i < 0x99; i++) {
        if (arg == lbl_802E28F0[i].triggerId) {
            switch (arg) {
            case 0x63:
                linkArg = 0xB;
                break;
            case 0x64:
                linkArg = 0xA;
                break;
            case 0x65:
                linkArg = 0x8;
                break;
            case 0x66:
                linkArg = 0xD;
                break;
            case 0x67:
                linkArg = 0;
                break;
            case 0x68:
                linkArg = 0;
                break;
            default:
                break;
            }

            floorLink(lbl_802E28F0[i].floorId, linkArg);
            break;
        }
    }

    return 0;
}

s32 dbgMenuWarpSelect(void) {
    u32 floorId;
    s32 i;

    if (fn_800FF52C() != 0) {
        soundStartSE(0x26);
        i = 0;
        do {
            GSmsgPrintf(0xC8, 0xF0, -1, lbl_802666B0);
            _threadSwitch();
            i++;
        } while (i < 0xF);

        return 0;
    }

    if ((fn_800F7BC4(1) & 0x20) != 0) {
        if (menuSelectInput(0, &floorId) == 0) {
            return -1;
        }

        floorLink(floorId, 0);
        return 0;
    }

    return 1;
}

s32 fn_8000C06C(void) {
    if (menuIsCheck(8) != 0) {
        menuClose(8);
    } else {
        menuOpenCustom(8, 0, 0, 0, 1, 0);
        menuSetPosition(8, 0x17C, 0x20);
    }

    return 0;
}

void dbgMovieCaptureUpdate(void) {
    if (lbl_8047A298 == 0) {
        return;
    }

    if (fn_80175FFC() == 1) {
        GSgfxCaptureMovieStop();
    } else if (lbl_8047A299 == 0) {
        GSgfxCaptureMovieStart(0);
    } else if (lbl_8047A299 == 1) {
        GSgfxCaptureMovieStart(1);
    }
}

s32 dbgMenuMovieCaptureToggle(void) {
    if (lbl_8047A298 == 0) {
        lbl_8047A299 = 1;
        lbl_8047A298 = 1;
    } else {
        lbl_8047A298 = 0;
        if (fn_80175FFC() == 1) {
            GSgfxCaptureMovieStop();
        }
    }

    vtrSetDisp(vtrGetDisp(), lbl_8047A298);
    return 0;
}

s32 fn_8000C1A8(void) {
    if (lbl_8047A298 == 0) {
        lbl_8047A299 = 0;
        lbl_8047A298 = 1;
    } else {
        lbl_8047A298 = 0;
        if (fn_80175FFC() == 1) {
            GSgfxCaptureMovieStop();
        }
    }

    vtrSetDisp(vtrGetDisp(), lbl_8047A298);
    return 0;
}

s32 dbgMenuVtrMode4(void) {
    vtrSetMode4();
    return 0;
}

s32 dbgMenuVtrMode3(void) {
    vtrSetMode3();
    return 0;
}

s32 dbgMenuVtrMode1(void) {
    vtrSetMode1();
    return 0;
}

s32 dbgMenuVtrMode2(void) {
    vtrSetMode2();
    return 0;
}

s32 dbgMenuGSvtrChangeDisp(void) {
    vtrSetDisp((u8)((u8)vtrGetDisp() == 0), lbl_8047A298);
    return 0;
}

s32 dbgMenuGSvtrChangeActive(void) {
    if (vtrIsActive() == 0) {
        vtrActivate();
    } else {
        vtrDeactivate();
    }

    return 0;
}

s32 fn_8000C318(void) {
    return 0;
}

s32 fn_8000C320(void) {
    return 0;
}

s32 fn_8000C328(void) {
    fn_801D0748(0xC, 2, 0);
    return 0;
}

s32 dbgMenuMemcardTask2(void) {
    fn_801D0748(2, 2, 0);
    winMsgOpen(2, 0x17A7, 1, 1);
    winMsgClose(1);
    return 0;
}

s32 fn_8000C3A4(void) {
    fn_801D0748(0xB, 2, 0);
    return 0;
}

s32 dbgMenuMsgTest(s32 unused, s32 type) {
    switch (type) {
    case 0:
        fadeSet(3, lbl_8047B6E0);
        fadeCheck(1);
        winMsgOpenError(0x44C5, 1, 0);
        winMsgClose(1);
        fadeSet(2, lbl_8047B6E0);
        fadeCheck(1);
        break;
    case 1:
        winMsgCloseFight(1);
        winMsgCloseError(1);
        break;
    case 2:
        _threadSwitch();
        _threadSwitch();
        _threadSwitch();
        winMsgCloseFight(0);
        _threadSwitch();
        _threadSwitch();
        break;
    case 3:
        msgctrlSetValue(0x31, 0x7DA);
        break;
    }

    return 0;
}

s32 dbgMenuMsgCheck(void) {
    u32 msgId;

    if ((fn_800F7BC4(1) & 0x20) != 0) {
        if (menuSelectInput(0, &msgId) == 0) {
            return -1;
        }

        winMsgOpenField(msgId, 1, 0);
        menuSubKeyWait();
        winMsgCloseField(1);
        return 0;
    }

    return 1;
}

s32 fn_8000C518(void) {
    if (menuIsCheck(0xC) != 0) {
        menuClose(0xC);
    } else {
        menuOpenCustom(0xC, 0, 0, 0, 1, 0);
        menuSetPosition(0xC, 0x190, 0x28);
    }

    return 0;
}

s32 fn_8000C588(void) {
    FloorDebugData* data;

    data = floorDataBiosGetPtr(std092_getCurrentRoomID());

    if (menuIsCheck(0xA) != 0) {
        if (fn_801174C4() != 0) {
            fn_80117500();
        }

        menuClose(0xA);
        fn_800FEC34(data->field_34);
    } else {
        std136_fieldClearFlag_8047ad71();
        menuOpenCustom(0xA, 0, 0, 0, 0, 0);
        fn_800FECB8(data->field_34);
    }

    return 0;
}

s32 dbgMenuSoundVolume(s32 arg) {
    switch (arg) {
    case 0xD3:
        fn_80166D18(0x7F, 0, 0, 1);
        break;
    case 0xD4:
        fn_80166D18(0, 0, 0, 1);
        break;
    }

    return 0;
}

s32 fn_8000C688(s32 arg) {
    switch (arg) {
    case 0xD1:
        fn_80166D18(0x7F, 0, 1, 0);
        break;
    case 0xD2:
        fn_80166D18(0, 0, 1, 0);
        break;
    }

    return 0;
}

s32 dbgMenuSoundTestSE(void) {
    s32 selected;
    u16 entry;
    s32 status;

    goto loop_check;
loop_body:
    if ((u32)selected >= *lbl_80478E18) {
        goto loop_check;
    }

    entry = lbl_80478E1C[selected];
    status = GSsndGetStatus(entry);
    if (status == 0) {
        goto use_default;
    }
    if (status < 0) {
        goto use_default;
    }
    if (status >= 4) {
        goto use_default;
    }

    soundKill(entry);
    goto loop_check;
use_default:
    std105_soundPlayEffect(entry, 0, 0x7F);
loop_check:
    selected = menuOpen(2, 1);
    if (selected != -1) {
        goto loop_body;
    }

    menuClose(2);
    return -1;
}

s32 dbgMenuSoundTestSE2(void) {
    s32 selected;
    u16 entry;
    s32 status;

    goto loop_check;
loop_body:
    if ((u32)selected >= *lbl_80478E10) {
        goto loop_check;
    }

    entry = lbl_80478E14[selected];
    status = GSsndGetStatus(entry);
    if (status == 0) {
        goto use_default;
    }
    if (status < 0) {
        goto use_default;
    }
    if (status >= 4) {
        goto use_default;
    }

    soundKill(entry);
    goto loop_check;
use_default:
    std105_soundPlayEffect(entry, 0, 0x7F);
loop_check:
    selected = menuOpen(2, 1);
    if (selected != -1) {
        goto loop_body;
    }

    menuClose(2);
    return -1;
}

s32 dbgMenuSoundTestSEExclusive(void) {
    s32 selected;
    u16 entry;
    s32 status;
    u32 offset;
    u32 i;

    goto loop_check;
loop_body:
    if ((u32)selected >= *lbl_80478E20) {
        goto loop_check;
    }

    i = 0;
    offset = 0;
    goto inner_check;
inner_body:
    if (selected == (s32)i) {
        goto inner_next;
    }
    status = GSsndGetStatus(*(u16*)((u8*)lbl_80478E24 + offset));
    switch (status) {
    case 1:
    case 2:
    case 3:
        goto call_inner;
    default:
        goto inner_next;
    }
call_inner:
    soundKill(*(u16*)((u8*)lbl_80478E24 + offset));
inner_next:
    offset += sizeof(u16);
    i++;
inner_check:
    if (i < *lbl_80478E20) {
        goto inner_body;
    }

    entry = lbl_80478E24[selected];
    status = GSsndGetStatus(entry);
    if (status == 0) {
        goto use_default;
    }
    if (status < 0) {
        goto use_default;
    }
    if (status >= 4) {
        goto use_default;
    }

    soundKill(entry);
    goto loop_check;
use_default:
    std105_soundPlayEffect(entry, 0, 0x7F);
loop_check:
    selected = menuOpen(2, 1);
    if (selected != -1) {
        goto loop_body;
    }

    menuClose(2);
    return -1;
}

s32 fn_8000C92C(void) {
    s32 selected;
    u16 entry;
    s32 status;
    u32 offset;
    u32 i;

    goto loop_check;
loop_body:
    if ((u32)selected >= *lbl_80478E28) {
        goto loop_check;
    }

    i = 0;
    offset = 0;
    goto inner_check;
inner_body:
    if (selected == (s32)i) {
        goto inner_next;
    }
    status = GSsndGetStatus(*(u16*)((u8*)lbl_80478E2C + offset));
    switch (status) {
    case 1:
    case 2:
    case 3:
        goto call_inner;
    default:
        goto inner_next;
    }
call_inner:
    soundKill(*(u16*)((u8*)lbl_80478E2C + offset));
inner_next:
    offset += sizeof(u16);
    i++;
inner_check:
    if (i < *lbl_80478E28) {
        goto inner_body;
    }

    entry = lbl_80478E2C[selected];
    status = GSsndGetStatus(entry);
    if (status == 0) {
        goto use_default;
    }
    if (status < 0) {
        goto use_default;
    }
    if (status >= 4) {
        goto use_default;
    }

    soundKill(entry);
    goto loop_check;
use_default:
    std104_soundSetBackgroundMusic(entry, 0, 0x7F);
loop_check:
    selected = menuOpen(2, 1);
    if (selected != -1) {
        goto loop_body;
    }

    menuClose(2);
    return -1;
}

