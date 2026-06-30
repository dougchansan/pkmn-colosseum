/**
 * @file gs_worldmap.c
 * @brief GSWorldmap -- World map UI and location navigation.
 *
 * Address range: 0x80026000 - 0x80030170 (~90 functions)
 *
 * This module implements the world map interface where the player selects
 * a destination to travel to. It handles:
 *   - Map rendering with location markers
 *   - Cursor movement and location selection
 *   - Location unlock/discovery tracking
 *   - Travel confirmation dialog
 *   - Camera panning and zoom on the map
 *   - Location name and description display
 *   - Transition animation to selected location
 *
 * The world map in Pokemon Colosseum features a 3D map of the Orre region
 * with selectable locations (Phenac City, Pyrite Town, The Under, etc.).
 * Locations are unlocked as the story progresses.
 *
 * Key functions (first cluster: 0x80026000-0x80027740):
 *   fn_80026370  GSmap_GetLocationCount      -- 0x20 bytes
 *   fn_80026390  GSmap_GetUnlockedCount      -- 0x20 bytes
 *   fn_800263B0  GSmap_IsLocationUnlocked    -- 0x6C bytes
 *   fn_8002641C  GSmap_UnlockLocation        -- 0x5C bytes
 *   fn_80026478  GSmap_GetLocationData       -- 0xA4 bytes, get name/position
 *   fn_8002651C  GSmap_GetLocationPosition   -- 0xA4 bytes, get 3D coordinates
 *   fn_800265C0  GSmap_SetCursorPos          -- 0x40 bytes
 *   fn_80026600  GSmap_GetCursorPos          -- 0x40 bytes
 *   fn_80026640  GSmap_GetSelectedLocation   -- 0x40 bytes
 *   fn_80026680  GSmap_SetSelectedLocation   -- 0x40 bytes
 *   fn_800266C0  GSmap_GetTravelState        -- 0x40 bytes
 *   fn_80026700  GSmap_SetTravelState        -- 0x40 bytes
 *   fn_80026740  GSmap_MoveCursor            -- 0x90 bytes, cursor with bounds
 *   fn_800267D0  GSmap_AnimateCursor         -- 0x90 bytes, cursor bob animation
 *   fn_80026860  GSmap_AnimateMarker         -- 0x90 bytes, location marker pulse
 *   fn_800268F0  GSmap_DrawLocation0         -- 0x254 bytes, draw location icon type 0
 *   fn_80026B44  GSmap_DrawLocation1         -- 0x254 bytes, draw location icon type 1
 *   fn_80026D98  GSmap_DrawLocation2         -- 0x254 bytes, draw location icon type 2
 *   fn_80026FEC  GSmap_DrawRoute0            -- 0x190 bytes, draw route line type 0
 *   fn_8002717C  GSmap_DrawRoute1            -- 0x190 bytes, draw route line type 1
 *   fn_8002730C  GSmap_DrawRoute2            -- 0x190 bytes, draw route line type 2
 *   fn_8002749C  GSmap_DrawMapBackground     -- 0x158 bytes, terrain/map texture
 *   fn_800275F4  GSmap_DrawLocationName      -- 0x14C bytes, text overlay
 *   fn_80027740  GSmap_GetRouteCount         -- 0x3C bytes
 *   fn_8002777C  GSmap_GetRouteData          -- 0x3C bytes
 *   fn_800277B8  GSmap_IsRouteUnlocked       -- 0x3C bytes
 *
 * Key functions (second cluster: 0x800277F4-0x8002A618):
 *   fn_800277F4  GSmap_ProcessInput          -- 0xB0 bytes, input handling
 *   fn_800278A4  GSmap_ConfirmTravel         -- 0xBC bytes, "Travel to X?" dialog
 *   fn_80027960  GSmap_StartTravel           -- 0x144 bytes, begin travel sequence
 *   fn_80027AA4  GSmap_TravelAnimation       -- 0x2B4 bytes, flight/drive animation
 *   fn_80027D58  GSmap_ArrivalSequence       -- 0x3A4 bytes, arrive at destination
 *   fn_800280FC  GSmap_UpdateCamera          -- 0xF4 bytes, map camera control
 *   fn_800281F0  GSmap_Nop                   -- 4 bytes, no-op
 *   fn_800281F4  GSmap_Init                  -- 0x250 bytes, full initialization
 *   fn_80028444  GSmap_DrawCityA             -- 0x50 bytes, specific city renderer
 *   fn_80028494  GSmap_DrawCityB             -- 0x50 bytes
 *   fn_800284E4  GSmap_DrawCityC             -- 0x50 bytes
 *   fn_80028534  GSmap_DrawCityD             -- 0x54 bytes
 *   fn_80028588  GSmap_DrawAllCities         -- 0x98 bytes
 *   fn_80028620  GSmap_DrawLandmarks         -- 0x108 bytes
 *   fn_80028728  GSmap_DrawPaths             -- 0x108 bytes
 *   fn_80028830  GSmap_DrawOverlays          -- 0x118 bytes
 *   fn_80028948  GSmap_MainRenderFrame       -- 0x674 bytes, main render function
 *   fn_80028FBC  GSmap_MainUpdate            -- 0x59C bytes, main update loop
 *   fn_80029558  GSmap_ExitMap               -- 0xE0 bytes, close map UI
 *   fn_80029638  GSmap_GetExitResult         -- 0x28 bytes
 *   fn_80029660  GSmap_DrawDescription       -- 0x100 bytes, location description text
 *   fn_80029760  GSmap_DrawMinimap           -- 0xF0 bytes, minimap overlay
 *   fn_80029850  GSmap_DrawCompass           -- 0x8C bytes, compass indicator
 *
 * Key functions (third cluster: 0x800298DC-0x8002FC58+):
 *   fn_800298DC  GSmap_SceneCallback0        -- 0x1EC bytes, scene-specific callback
 *   fn_80029AC8  GSmap_SceneCallback1        -- 0x1F8 bytes
 *   fn_80029CC0  GSmap_SceneCallback2        -- 0x234 bytes
 *   fn_80029EF4  GSmap_ValidateDestination   -- 0xB8 bytes
 *   fn_80029FAC  GSmap_FormatText0           -- 0x10C bytes, format location string
 *   fn_8002A0B8  GSmap_FormatText1           -- 0x10C bytes (this is also called externally)
 *   fn_8002A1C4  GSmap_FormatText2           -- 0x108 bytes
 *   fn_8002A2CC  GSmap_FormatText3           -- 0x108 bytes
 *   fn_8002A3D4  GSmap_GetFormatCount        -- 0x2C bytes
 *   fn_8002A400  GSmap_BuildLocationList     -- 0x8C bytes
 *   fn_8002A48C  GSmap_SortLocations         -- 0x124 bytes
 *   fn_8002A5B0  GSmap_GetNearestLocation    -- 0x68 bytes
 *   fn_8002A618  GSmap_ComputeRoutes         -- 0x450 bytes, pathfinding
 *   fn_8002AA68  GSmap_GetRouteLength        -- 0x98 bytes
 *   fn_8002AB00  GSmap_IsDirectRoute         -- 0x40 bytes
 *   fn_8002AB40  GSmap_InterpolateRoute      -- 0x178 bytes
 *   fn_8002ACB8  GSmap_DrawRoutePreview      -- 0x18C bytes
 *   fn_8002AE44  GSmap_SetZoom               -- 0x24 bytes
 *   fn_8002AE68  GSmap_GetZoom               -- 0x34 bytes
 *   fn_8002AE9C  GSmap_AnimateZoom           -- 0x5C bytes
 *   fn_8002AEF8  GSmap_CameraLookAt          -- 0x144 bytes
 *   fn_8002B03C  GSmap_GetCameraTarget       -- 0x4C bytes
 *   fn_8002B088  GSmap_SetCameraTarget       -- 0x34 bytes
 *   fn_8002B0BC  GSmap_CameraPan             -- 0x78 bytes
 *   fn_8002B134  GSmap_CameraRotate          -- 0x6C bytes
 *   fn_8002B1A0  GSmap_DrawWeatherOverlay    -- 0x26C bytes
 *   fn_8002B40C  GSmap_DrawTimeOverlay       -- 0x188 bytes
 *   fn_8002B594  GSmap_DrawPartyIcons        -- 0x2EC bytes
 *   fn_8002B880  GSmap_DrawInfoPanel         -- 0x468 bytes
 *   fn_8002BCE8  GSmap_DrawTravelProgress    -- 0x120 bytes
 *   fn_8002BE08  GSmap_DrawTransition        -- 0x20C bytes
 *   fn_8002C014  GSmap_FadeToBlack           -- 0xD0 bytes
 *   fn_8002C0E4  GSmap_FadeFromBlack         -- 0x1A0 bytes
 *   fn_8002C284  GSmap_ShowTravelDialog      -- 0x184 bytes
 *   fn_8002C408  GSmap_DialogStateMachine    -- 0xA64 bytes, dialog handler
 *   fn_8002CE6C  GSmap_ProcessChoice         -- 0x2E8 bytes
 *   fn_8002D154  GSmap_ConfirmSequence       -- 0x480 bytes
 *   fn_8002D5D4  GSmap_CancelTravel         -- 0x348 bytes
 *   fn_8002D91C  GSmap_ArrivalDialog         -- 0x350 bytes
 *   fn_8002DC6C  GSmap_SetStoryFlag          -- 0xB8 bytes
 *   fn_8002DD24  GSmap_CheckStoryState       -- 0x1EC bytes
 *   fn_8002DF10  GSmap_UpdateAvailability    -- 0x35C bytes
 *   fn_8002E26C  GSmap_RefreshDisplay        -- 0x1F4 bytes
 *   fn_8002E460  GSmap_DrawFullFrame         -- 0x5FC bytes
 *   fn_8002EA5C  GSmap_HandleSceneChange     -- 0x418 bytes
 *   fn_8002EE74  GSmap_TransitionToScene     -- 0x410 bytes
 *   fn_8002F284  GSmap_LoadDestination       -- 0x518 bytes
 *   fn_8002F79C  GSmap_PrepareArrival        -- 0x4BC bytes
 *   fn_8002FC58  GSmap_FinalizeTransition    -- 0x518 bytes
 *
 * SDA globals:
 *   lbl_8047A360-A3A0: Map state variables (cursor pos, zoom, camera, etc.)
 *   lbl_80478DD8: Location data table pointer
 *   lbl_80478898: Camera float parameter
 *
 * BSS globals:
 *   lbl_803A2058: Map camera position vector
 *   lbl_803A204C: Map camera target vector
 *
 * Sdata2 float constants:
 *   lbl_8047B8B8: 0x4330000080000000 (int-to-float conversion constant)
 *   lbl_8047B8E0: Map viewport center X (float)
 *   lbl_8047B9D4: Map scale factor (float)
 *   lbl_8047B9F0: Map Y offset (float)
 */

#include "dolphin/types.h"

/* ===== Phase 2 recovery stubs ===== */

/* fn_80026370 - 0x80026370 | size: 0x20 */
#if 0
asm void fn_80026370(void) {
#include "src/game/gs_worldmap_fn_80026370.inc"
}
#else
#pragma optimization_level 4
s32 fn_80026370(void* r3, u8* r4) {
    r4[0x64] = 0;
    r4[0x65] = 0x35;
    r4[0x66] = 0x3c;
    return 0;
}
#endif

/* fn_80026390 - 0x80026390 | size: 0x20 */
#if 0
asm void fn_80026390(void) {
#include "src/game/gs_worldmap_fn_80026390.inc"
}
#else
#pragma optimization_level 4
s32 fn_80026390(void* r3, u8* r4) {
    r4[0x64] = 0;
    r4[0x65] = 0x35;
    r4[0x66] = 0x3c;
    return 0;
}
#endif

/* fn_800263B0 - 0x800263B0 | size: 0x6c | WALL 92.6%: li vs mr pattern */
extern u8 lbl_80266E18[];
#if 0
asm void fn_800263B0(void) {
#include "src/game/gs_worldmap_fn_800263B0.inc"
}
#else
#pragma optimization_level 4
s32 fn_800263B0(void* r3, u8* r4) {
    void* ctx;
    s32 idx;
    u8 r0, r5, r6;
    ctx = *(void**)((u8*)(*(void**)((u8*)r3 + 0x60)) + 0x24);
    idx = *(s32*)ctx + 1;
    if (idx >= 2) idx -= 2;
    if (idx < 0 || idx >= 2) {
        r0 = 0xff; r5 = 0xff; r6 = 0xff;
    } else {
        r0 = lbl_80266E18[idx * 0x18 + 0];
        r5 = lbl_80266E18[idx * 0x18 + 1];
        r6 = lbl_80266E18[idx * 0x18 + 2];
    }
    r4[0x64] = r0;
    r4[0x65] = r5;
    r4[0x66] = r6;
    return 0;
}
#endif

/* fn_8002641C - 0x8002641C | size: 0x5c | WALL 91.3%: li vs mr pattern */
#if 0
asm void fn_8002641C(void) {
#include "src/game/gs_worldmap_fn_8002641C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002641C(void* r3, u8* r4) {
    void* ctx;
    s32 idx;
    u8 r0, r5, r6;
    ctx = *(void**)((u8*)(*(void**)((u8*)r3 + 0x60)) + 0x24);
    idx = *(s32*)ctx;
    if (idx < 0 || idx >= 2) {
        r0 = 0xff; r5 = 0xff; r6 = 0xff;
    } else {
        r0 = lbl_80266E18[idx * 0x18 + 0];
        r5 = lbl_80266E18[idx * 0x18 + 1];
        r6 = lbl_80266E18[idx * 0x18 + 2];
    }
    r4[0x64] = r0;
    r4[0x65] = r5;
    r4[0x66] = r6;
    return 0;
}
#endif

/* fn_80026478 - 0x80026478 | size: 0xa4 */
extern void* fn_8012A5B0(s32, s32, u32);
extern u8 fn_80123FBC(void);
extern u8 fn_801231A4(void*);
#if 0
asm void fn_80026478(void) {
#include "src/game/gs_worldmap_fn_80026478.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026478(void* r3, u8* r4) {
    void* ctx;
    void* r31;
    u8 r30;
    ctx = *(void**)((u8*)r3 + 0x60);
    r30 = 0;
    if (*(s32*)((u8*)ctx + 0x1c) != 2) {
        r4[0x67] = 0;
        return 0;
    }
    r31 = fn_8012A5B0(0, 3, (u16)*(u32*)((u8*)ctx + 0x20));
    if ((u8)fn_80123FBC() == 0) goto L_done;
    if ((u32)(fn_801231A4(r31) & 0xff) != 1) goto L_done;
    r30 = 0xff;
L_done:
    r4[0x67] = r30;
    return 0;
}
#pragma pop
#endif

/* fn_8002651C - 0x8002651C | size: 0xa4 */
#if 0
asm void fn_8002651C(void) {
#include "src/game/gs_worldmap_fn_8002651C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002651C(void* r3, u8* r4) {
    void* ctx;
    void* r31;
    u8 r30;
    ctx = *(void**)((u8*)r3 + 0x60);
    r30 = 0;
    if (*(s32*)((u8*)ctx + 0x1c) != 2) {
        r4[0x67] = 0;
        return 0;
    }
    r31 = fn_8012A5B0(0, 3, (u16)*(u32*)((u8*)ctx + 0x20));
    if ((u8)fn_80123FBC() == 0) goto L_done2;
    if ((u32)(fn_801231A4(r31) & 0xff) != 0) goto L_done2;
    r30 = 0xff;
L_done2:
    r4[0x67] = r30;
    return 0;
}
#pragma pop
#endif

/* fn_800265C0 - 0x800265C0 | size: 0x40 */
extern u8 lbl_80266DD8[];
#if 0
asm void fn_800265C0(void) {
#include "src/game/gs_worldmap_fn_800265C0.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_800265C0(void* r3, u8* r4) {
    r3 = *(void**)((u8*)r3 + 0x60);
    r3 = (void*)(lbl_80266DD8 + (*(s32*)((u8*)r3 + 0x1c) << 4));
    if (*(s32*)((u8*)r3 + 0x4) != 7) r4[0x67] = 0;
    else r4[0x67] = 0xff;
    return 0;
}
#pragma pop
#endif

/* fn_80026600 - 0x80026600 | size: 0x40 */
#if 0
asm void fn_80026600(void) {
#include "src/game/gs_worldmap_fn_80026600.inc"
}
#else
#pragma optimization_level 4
s32 fn_80026600(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    entry = (s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4));
    if (entry[1] != 8) r4[0x67] = 0;
    else r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026640 - 0x80026640 | size: 0x40 */
#if 0
asm void fn_80026640(void) {
#include "src/game/gs_worldmap_fn_80026640.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026640(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    entry = (s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4));
    if (entry[1] == 0xa) goto L_40_then;
    r4[0x67] = 0;
    goto L_40_end;
L_40_then:
    r4[0x67] = 0xff;
L_40_end:
    return 0;
}
#pragma pop
#endif

/* fn_80026680 - 0x80026680 | size: 0x40 */
#if 0
asm void fn_80026680(void) {
#include "src/game/gs_worldmap_fn_80026680.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026680(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    entry = (s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4));
    if (entry[1] == 7) goto L_80_then;
    r4[0x67] = 0;
    goto L_80_end;
L_80_then:
    r4[0x67] = 0xff;
L_80_end:
    return 0;
}
#pragma pop
#endif

/* fn_800266C0 - 0x800266C0 | size: 0x40 */
#if 0
asm void fn_800266C0(void) {
#include "src/game/gs_worldmap_fn_800266C0.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_800266C0(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    entry = (s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4));
    if (entry[1] == 8) goto L_C0_then;
    r4[0x67] = 0;
    goto L_C0_end;
L_C0_then:
    r4[0x67] = 0xff;
L_C0_end:
    return 0;
}
#pragma pop
#endif

/* fn_80026700 - 0x80026700 | size: 0x40 */
#if 0
asm void fn_80026700(void) {
#include "src/game/gs_worldmap_fn_80026700.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_80026700(void* r3, u8* r4) {
    void* ctx;
    s32* entry;
    ctx = *(void**)((u8*)r3 + 0x60);
    entry = (s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4));
    if (entry[1] == 0xa) goto L_700_then;
    r4[0x67] = 0;
    goto L_700_end;
L_700_then:
    r4[0x67] = 0xff;
L_700_end:
    return 0;
}
#pragma pop
#endif

/* fn_80026740 - 0x80026740 | size: 0x90 | WALL 85.69% (band w_wmap, 2026-06-18)
 *
 * The #else C below is FAITHFUL, byte-correct logic and measures 85.69% — but
 * the asm wrapper (#if 1, this is the byte-exact ROM) measures 96.67% under
 * objdiff (the 96.67 is a pure numeric-vs-named float-reloc disassembler
 * artifact: the .inc emits `lfs f2,-0x7d68(r2)` while the target object carries
 * `lbl_8047B938@sda21`; both resolve to the same sdata2 address). Since the C
 * cannot beat the asm's measured ceiling, the asm stays active.
 *
 * Levers that DID land (took the draft 72% -> 85.69%):
 *   - `#pragma fp_contract on`  -> `1.0 - 255.0*x` fuses to a single fnmsubs
 *   - declaring `state`/`index` BEFORE `ctx` reserves r5 for `state`, pinning
 *     ctx to r6 to match the target's whole-function allocation (+11%)
 *   - if(state!=7){0}else{...} single trailing `return 0` -> one epilogue li r3,0
 *   - dropping the (u8)/(s32) cast on the r4[0x67] store -> stb truncates with
 *     no redundant clrlwi (peephole off keeps the (s16) extsh on the X store)
 *   - `#pragma scheduling on` (off regresses to 71%)
 *
 * Residual WALL (3 CW reg-alloc / scheduler ties, not source-controllable):
 *   1. table base/index scratch rotation: target keeps base in r3 + idx in r0
 *      (`add r3,r3,r0`); CW emits base->r0 + idx->r3 (`add r3,r0,r3`).
 *   2. X multiply result: target reuses freed r5 (`mulli r5,r0,0x1a`); CW keeps
 *      it in r0 (`mulli r0,r0,0x1a`).
 *   3. float const-load schedule: target interleaves lfs f2/f0 into the X-mul
 *      load-delay slots; CW won't hoist them across the sth store.
 * -> permuter territory. Same wall applies to siblings fn_800267D0/fn_80026860.
 */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
#if 1
asm void fn_80026740(void) {
#include "src/game/gs_worldmap_fn_80026740.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma fp_contract on
#pragma optimization_level 4
s32 fn_80026740(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];   /* state-machine entry table: each entry 16 bytes */
    s32  state;
    s32  index;
    void* ctx;

    ctx = *(void**)((u8*)r3 + 0x60);
    state = *(s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4) + 4);
    if (state != 7) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        *(s16*)(r4 + 0x50) = (s16)(index * 0x1a + *(s32*)(*(s32**)((u8*)ctx + 0x48)));
        r4[0x67] = lbl_8047B934 - lbl_8047B938 * *(f32*)(*(f32**)((u8*)ctx + 0x30));
    }
    return 0;
}
#pragma pop
#endif

/* fn_800267D0 - 0x800267D0 | size: 0x90 | WALL ~85.7% — sibling of fn_80026740.
 * selector=8, base ptr ctx+0x44. asm (#if 1) is the byte-exact ROM (96.67% is
 * the numeric-vs-named float-reloc artifact). See fn_80026740 for the full lever
 * analysis + residual reg-alloc/scheduler ties. The #else is the cloned shape,
 * staged for a future permuter attack. */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
#if 1
asm void fn_800267D0(void) {
#include "src/game/gs_worldmap_fn_800267D0.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma fp_contract on
#pragma optimization_level 4
s32 fn_800267D0(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    void* ctx;

    ctx = *(void**)((u8*)r3 + 0x60);
    state = *(s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4) + 4);
    if (state != 8) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        *(s16*)(r4 + 0x50) = (s16)(index * 0x1a + *(s32*)(*(s32**)((u8*)ctx + 0x44)));
        r4[0x67] = lbl_8047B934 - lbl_8047B938 * *(f32*)(*(f32**)((u8*)ctx + 0x30));
    }
    return 0;
}
#pragma pop
#endif

/* fn_80026860 - 0x80026860 | size: 0x90 */
/* fn_80026860 - 0x80026860 | size: 0x90 | WALL ~85.7% — sibling of fn_80026740.
 * selector=0xa, base ptr ctx+0x40. asm (#if 1) is the byte-exact ROM (96.67% is
 * the numeric-vs-named float-reloc artifact). See fn_80026740 for the full lever
 * analysis + residual reg-alloc/scheduler ties. The #else is the cloned shape,
 * staged for a future permuter attack. */
extern f32 lbl_8047B938;
extern f32 lbl_8047B934;
#if 1
asm void fn_80026860(void) {
#include "src/game/gs_worldmap_fn_80026860.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
#pragma fp_contract on
#pragma optimization_level 4
s32 fn_80026860(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];
    s32  state;
    s32  index;
    void* ctx;

    ctx = *(void**)((u8*)r3 + 0x60);
    state = *(s32*)(lbl_80266DD8 + (*(s32*)((u8*)ctx + 0x1c) << 4) + 4);
    if (state != 0xa) {
        r4[0x67] = 0;
    } else {
        index = *(s32*)(*(s32**)((u8*)ctx + 0x34));
        if (index >= state) {
            index = state - 1;
        }
        *(s16*)(r4 + 0x50) = (s16)(index * 0x1a + *(s32*)(*(s32**)((u8*)ctx + 0x40)));
        r4[0x67] = lbl_8047B934 - lbl_8047B938 * *(f32*)(*(f32**)((u8*)ctx + 0x30));
    }
    return 0;
}
#pragma pop
#endif

/* fn_800268F0 - 0x800268F0 | size: 0x254 */
extern void fn_80132A38(s32, void*);
extern u32 fn_800FA444(u32);
extern void fn_800FB680(s32, s32, s32, u32);
extern s32 fn_800FA314(void*);
extern void* fn_800FA280(u32);
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 1
asm void fn_800268F0(void) {
#include "src/game/gs_worldmap_fn_800268F0.inc"
}
#else
/*
 * GSmap_DrawLocation0 -- 0x800268F0, size 0x254
 *
 * Draws a world-map location icon of type 0 onto the draw-context canvas.
 * r3 = self (world-map object); r4 = draw-context (canvas u8*).
 *
 * Control flow:
 *   1. Look up state-entry word[1] for the current index in lbl_80266DD8.
 *      If it != 7 → clear r4[0x67] and return.
 *   2. Walk the u16 item list at ctx->0x18, emitting each icon with
 *      fn_80132A38 + fn_800FB680.
 *   3. If fewer items were drawn than the entry capacity AND r3[0x98]==0:
 *      look up a "next" item through lbl_80266E18[row][col].item_list,
 *      validate it, optionally draw a special computed-X icon.
 *   4. Always set r4[0x67] = 0xff on the way out of the "state==7" branch.
 */
s32 fn_800268F0(void* r3, u8* r4)
{
    extern u8  lbl_80266DD8[];   /* 16-byte-entry state table */
    extern u8  lbl_80266E18[];   /* 0x18-byte-row, 4-word-col item handle table */
    extern f32 lbl_8047B934;     /* map center X (float) */
    extern f32 lbl_8047B938;     /* map X scale (float) */

    extern void fn_80132A38(s32 cmd, void* buf);
    extern u32  fn_800FA444(u32 id);
    extern void fn_800FB680(s32 x, s32 y, s32 palette, u32 cmd);
    extern s32  fn_800FA314(void* handle);
    extern void* fn_800FA280(u32 id);

    u8  *self  = (u8*)r3;
    u8  *ctx   = *(u8**)(self + 0x60);

    /* entry = lbl_80266DD8 + state_index * 16; word[1] = entry[4] as u32 */
    s32  state_idx  = *(s32*)(ctx + 0x1c);
    u32  *entry_w1_base = (u32*)(lbl_80266DD8 + 4);  /* points at word[1] of entry[0] */
    u32   entry_cap = entry_w1_base[state_idx * 4];   /* stride 16 bytes = 4 u32 words */

    if (entry_cap != 7) {
        r4[0x67] = 0;
        return 0;
    }

    /* --- Phase 1: draw existing items ---------------------------------- */
    u16  *item_ptr = (u16*)(*(u8**)(ctx + 0x18)); /* walking u16 list */
    s32   item_count = 0;
    s32   y_cursor   = 0;
    u16   buf[2];

    while (*item_ptr != 0) {
        u16  item_id  = *item_ptr;
        s32  palette  = (s32)(self[0x8b]) | (s32)(-0x100);

        buf[0] = item_id;
        buf[1] = 0;
        fn_80132A38(0x37, buf);

        /* compute X offset: signed upper-16 of fn_800FA444(0xce), floor-div */
        u32  raw_x   = fn_800FA444(0xce);
        s32  pos_x   = (s16)(raw_x >> 16);         /* extsh of upper half */
        s32  delta   = 0x1b - pos_x;
        s32  x_off   = (delta + (s32)((u32)delta >> 31)) >> 1; /* arithmetic floor */

        fn_800FB680(y_cursor + x_off, 0, palette, 0xce);

        y_cursor   += 0x1a;
        item_count += 1;
        item_ptr   += 1;   /* advance by one u16 word */
    }

    /* --- Phase 2: optionally draw "next" item -------------------------- */
    /* Re-read cap (entry[1] for current state) */
    state_idx = *(s32*)(ctx + 0x1c);
    u32  cap2 = entry_w1_base[state_idx * 4];

    if ((u32)item_count >= cap2) goto L_done;
    if (self[0x98] != 0)         goto L_done;

    {
        /* Navigate lbl_80266E18[row][col] to find an item handle */
        s32  map_row  = *(s32*)*(u32**)(ctx + 0x24);  /* **(s32**)(ctx+0x24) */
        s32  item_idx = *(s32*)*(u32**)(ctx + 0x28);  /* **(s32**)(ctx+0x28) */
        s32  map_col  = *(s32*)*(u32**)(ctx + 0x2c);  /* **(s32**)(ctx+0x2c) */

        /* lbl_80266E18: row stride 0x18 bytes (6 u32s?), col stride 4 bytes;
           handle is at +8 within each col cell */
        u32  handle = 0;

        if (map_row < 0 || map_row >= 2) goto L_check_handle;
        if (map_col < 0 || map_col >= 4) goto L_check_handle;

        handle = *(u32*)(lbl_80266E18 + map_row * 0x18 + map_col * 4 + 8);

        {
            s32 list_count = fn_800FA314((void*)handle);
            if (item_idx < 0 || item_idx >= list_count) {
                handle = 0;
                goto L_check_handle;
            }
            /* read u16 at item_idx from the handle's array */
            u8  *arr = (u8*)fn_800FA280(handle);
            handle = (u32)*(u16*)(arr + item_idx * 2);
        }

    L_check_handle:
        if ((u16)handle == 0) goto L_done;

        /* Compare against special list 0x2efc */
        {
            u8   *special_arr = (u8*)fn_800FA280(0x2efc);
            u16   special_id  = *(u16*)(special_arr + 0);
            u32   palette_off;

            if ((u16)handle == special_id) {
                palette_off = 0;
            } else {
                palette_off = 6;
            }

            if (palette_off != 6) goto L_done;

            /* Compute the special icon draw */
            f32  *pos_fptr = *(f32**)(ctx + 0x30);
            f32   map_pos  = pos_fptr[0];
            f32   cx       = lbl_8047B934;
            f32   scale    = lbl_8047B938;
            /* computed X byte: truncate-to-int of scale*(center - map_pos) */
            s32   ix       = (s32)(scale * (cx - map_pos)); /* fctiwz = truncate */
            u8    x_byte   = (u8)(u32)ix;                   /* clrlwi 24 */
            /* palette word: 0x00FF0000 | x_byte -- ENDIAN-QA: big-endian
               halfword where high byte = 0xff, low byte = computed x */
            u32   pal_word = 0x00FF0000u | x_byte;          /* oris rN,r0,0xff */

            u16  draw_buf[2];
            draw_buf[0] = (u16)handle;                      /* sth r23, sp+8  */
            draw_buf[1] = 0;                                 /* sth r0, sp+0xa */
            fn_80132A38(0x37, draw_buf);

            /* X offset computation (same arithmetic as Phase 1) */
            {
                u32  raw_x2  = fn_800FA444(0xce);
                s32  pos_x2  = (s16)(raw_x2 >> 16);
                s32  delta2  = 0x1b - pos_x2;
                s32  x_off2  = (delta2 + (s32)((u32)delta2 >> 31)) >> 1;
                s32  y2      = item_count * 0x1a + x_off2;  /* mulli + add */
                fn_800FB680(y2, 0, (s32)pal_word, 0xce);
            }
        }
    }

L_done:
    r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026B44 - 0x80026B44 | size: 0x254 */
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 1
asm void fn_80026B44(void) {
#include "src/game/gs_worldmap_fn_80026B44.inc"
}
#else
/*
 * GSmap_DrawLocation1 -- Draw world-map location icon type 1 (state-8 entry).
 * Identical structure to GSmap_DrawLocation0 (fn_800268F0) except the entry
 * word at lbl_80266DD8[idx*16 + 4] is tested for 0x8 instead of 0x7.
 *
 * r3 = self object pointer
 * r4 = draw-context / output u8 buffer (byte 0x67 = visibility flag)
 *
 * Flow:
 *  1. If the table entry is not 8 → write 0 to r4[0x67] and return.
 *  2. Walk the name string (u16 array at ctx+0x18), rendering each glyph.
 *  3. If char-count < table-count AND object not flagged at +0x98:
 *       look up a special glyph via lbl_80266E18 + sub-tables,
 *       and if it differs from the "current" glyph (0x2efc pool),
 *       draw it at the next x position with an animated alpha byte derived
 *       from a float countdown at ctx+0x30.
 *  4. Write 0xFF to r4[0x67] on the success path.
 */
s32 fn_80026B44(void *r3, u8 *r4)
{
    extern u8  lbl_80266DD8[];   /* state table: 16-byte entries; word at +4 = type */
    extern u8  lbl_80266E18[];   /* glyph-set table: 0x18-byte stride, ptr at +8    */
    extern f32 lbl_8047B934;     /* float constant: animation reference value        */
    extern f32 lbl_8047B938;     /* float constant: animation scale                  */

    extern void   fn_80132A38(s32 cmd, void *buf); /* set glyph draw param */
    extern u32    fn_800FA444(u32 pool);            /* query pool metrics   */
    extern void   fn_800FB680(s32 x, s32 y, s32 color, u32 glyph_id); /* draw glyph */
    extern s32    fn_800FA314(void *tbl);           /* get entry count      */
    extern void  *fn_800FA280(u32 key);             /* get base pointer     */

    void  *ctx;       /* r30: *(void**)(r3 + 0x60) */
    u16   *name_ptr;  /* r24: pointer into u16 name string */
    s32    char_count;/* r31: number of chars drawn in loop */
    s32    x_acc;     /* r29: accumulated x position */
    s32    color;     /* r23: packed color/alpha word */
    u32    entry_word;/* word from lbl_80266DD8 entry[1] (offset +4 in 16-byte entry) */
    u16    buf[2];    /* r28 -> stack buf at sp+0xc: {glyph_id, 0} */
    u32    special;   /* r23 after glyph-set lookup: glyph handle / u16 id */
    void  *tbl_ptr;   /* pointer into lbl_80266E18 sub-table */
    s32    sub_idx0;  /* r5: first index into lbl_80266E18 rows */
    s32    sub_idx1;  /* r0: second index (column) */
    s32    half_w;    /* width correction: (0x1b - measured_width + 1) / 2 rounded */
    s32    measured;  /* signed half-word from fn_800FA444 high half */
    u8    *arr;
    s32    count;
    u16    glyph_id;
    u8     anim_byte;
    f32    anim_val;
    u32    cur_word;

    ctx = *(void **)((u8 *)r3 + 0x60);

    /* lbl_80266DD8 is a flat array of 16-byte entries; word at [idx*16 + 4] = type */
    {
        s32 idx = *(s32 *)((u8 *)ctx + 0x1c);
        entry_word = *(u32 *)(lbl_80266DD8 + (u32)(idx << 4) + 4);
    }

    if (entry_word != 0x8) {
        r4[0x67] = 0;
        return 0;
    }

    /* --- Name-string render loop ---------------------------------------- */
    char_count = 0;
    x_acc      = 0;
    name_ptr   = (u16 *)*(u32 *)((u8 *)ctx + 0x18);

    while (*name_ptr != 0) {
        glyph_id = *name_ptr;

        /* color: top 24 bits = 0xFFFFFF, bottom 8 bits = alpha from +0x8B */
        color = (s32)((u8 *)r3)[0x8b] | (s32)(-0x100);

        buf[0] = glyph_id;
        buf[1] = 0;
        fn_80132A38(0x37, buf);

        /* measured width: upper 16 bits of fn_800FA444 return, sign-extended */
        measured = (s32)(s16)(u16)((u32)fn_800FA444(0xce) >> 16);

        /* center the glyph: half of (0x1b - measured), rounding toward zero */
        {
            s32 diff = 0x1b - measured;
            half_w   = (diff + ((u32)diff >> 31)) >> 1;
        }

        fn_800FB680(x_acc + half_w, 0, color, 0xce);

        x_acc += 0x1a;
        char_count++;
        name_ptr++;
    }

    /* Re-read entry_word (compiler reloads from table) */
    {
        s32 idx = *(s32 *)((u8 *)ctx + 0x1c);
        entry_word = *(u32 *)(lbl_80266DD8 + (u32)(idx << 4) + 4);
    }

    if ((s32)char_count >= (s32)entry_word) goto L_set_visible;
    if (((u8 *)r3)[0x98] != 0)              goto L_set_visible;

    /* --- Special-glyph lookup ------------------------------------------- */
    /* Three indirection layers from ctx+0x24, +0x28, +0x2c               */
    {
        void **p_a = *(void ***)((u8 *)ctx + 0x24); /* ptr-to-ptr */
        void **p_b = *(void ***)((u8 *)ctx + 0x28);
        void **p_c = *(void ***)((u8 *)ctx + 0x2c);
        sub_idx0 = *(s32 *)p_a;   /* r5 */
        sub_idx1 = *(s32 *)p_c;   /* r0 */
        /* r25 used later as second range value from p_b */
        {
            s32 val_b = *(s32 *)p_b; /* r25 = *p_b[0] */

            if (sub_idx0 < 0 || sub_idx0 >= 2) {
                special = 0;
                goto L_check_special;
            }
            if (sub_idx1 < 0 || sub_idx1 >= 4) {
                special = 0;
                goto L_check_special;
            }

            /* lbl_80266E18: stride 0x18; column stride 4; pointer at +8 in each cell */
            tbl_ptr = *(void **)(lbl_80266E18 + (u32)sub_idx0 * 0x18 + (u32)sub_idx1 * 4 + 8);
            special = (u32)(uintptr_t)tbl_ptr; /* r23 = the raw pointer / handle */

            count = fn_800FA314(tbl_ptr);

            if (val_b < 0 || val_b >= count) {
                special = 0;
                goto L_check_special;
            }

            arr    = (u8 *)fn_800FA280((u32)(uintptr_t)tbl_ptr);
            special = (u32)*(u16 *)(arr + (u32)val_b * 2); /* lhzx, zero-extended */

        L_check_special:
            /* clrlwi r0, r23, 16 → zero upper 16 bits → treat as u16 */
            if ((u16)special == 0) goto L_set_visible;

            /* Compare against "current" glyph from pool 0x2efc */
            {
                u16 *cur_base = (u16 *)fn_800FA280(0x2efc);
                u16  cur_glyph = cur_base[0]; /* lhz r0, 0x0(r3) */
                s32  flag;

                if ((u16)special == cur_glyph)
                    flag = 0;
                else
                    flag = 6;

                if (flag != 6) goto L_set_visible;
            }

            /* --- Animated-alpha final glyph ----------------------------- */
            /* f0 = (lbl_8047B934 - *(f32*)(ctx+0x30)) * lbl_8047B938      */
            /* anim_byte = (u8)(s32)f0 (truncate-toward-zero via fctiwz)   */
            /* fctiwz stores in low 32 bits of the 64-bit FP reg (stfd idiom) */
            {
                f32 *fp_countdown = *(f32 **)((u8 *)ctx + 0x30);
                f32  f0 = (lbl_8047B934 - fp_countdown[0]) * lbl_8047B938;
                s32  int_val = (s32)f0; /* fctiwz = truncate toward zero    */
                anim_byte = (u8)(int_val & 0xFF); /* clrlwi r0,r0,24 */
            }

            /* r23 = anim_byte | 0x00FF0000  (oris r23, r0, 0xff)         */
            /* This packs: bits[23:16]=0xFF, bits[7:0]=anim_byte           */
            /* ENDIAN-QA: On PPC big-endian this is a 32-bit color word where
             * byte[1] (G channel or 2nd byte) = 0xFF and byte[3] = anim_byte.
             * fn_800FB680 treats its 3rd arg as a packed color; preserve the
             * bit pattern exactly.                                          */
            {
                u32 packed_color = ((u32)anim_byte) | 0x00FF0000u; /* ENDIAN-QA */

                buf[0] = (u16)special;
                buf[1] = 0;
                fn_80132A38(0x37, buf);

                measured = (s32)(s16)(u16)((u32)fn_800FA444(0xce) >> 16);
                {
                    s32 diff = 0x1b - measured;
                    /* final glyph: x = char_count*0x1a + half_w             */
                    /* In PPC: add r3,r0,r3 then mulli r0,r31,0x1a then add  */
                    /* Both orderings are commutative; reconstruct r29 = r31*0x1a */
                    s32 x_pos = (s32)char_count * 0x1a + (diff + ((u32)diff >> 31)) / 2;
                    fn_800FB680(x_pos, 0, (s32)packed_color, 0xce);
                }
            }
        }
    }

L_set_visible:
    r4[0x67] = 0xFF;
    return 0;
}
#endif

/* fn_80026D98 - 0x80026D98 | size: 0x254 */
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 1
asm void fn_80026D98(void) {
#include "src/game/gs_worldmap_fn_80026D98.inc"
}
#else
/*
 * fn_80026D98  GSmap_DrawLocation2  0x80026D98 | 0x254 bytes
 *
 * Draws a world-map location icon of type 2 (entry[1] == 0xA).
 * Iterates the null-terminated u16 name-string stored at ctx+0x18,
 * centre-draws each character glyph, then optionally draws a
 * species-coloured highlight icon at the accumulated row position.
 *
 * Parameters:
 *   r3  – self (GS object; *(void**)(self+0x60) is the map entry ctx)
 *   r4  – output/render buffer whose byte at +0x67 is updated
 *
 * Returns 0.
 */
s32 fn_80026D98(void* r3, u8* r4)
{
    /* block-scope externs (TU convention) */
    extern u8  lbl_80266DD8[];          /* map entry table; each entry 16 bytes */
    extern u8  lbl_80266E18[];          /* species-colour table; each row 0x18 bytes */
    extern f32 lbl_8047B934;            /* float const: base animation phase    */
    extern f32 lbl_8047B938;            /* float const: animation scale factor  */
    extern void  fn_80132A38(s32 cmd, void* buf);
    extern u32   fn_800FA444(u32 id);
    extern void  fn_800FB680(s32 x, s32 y, s32 color, u32 id);
    extern s32   fn_800FA314(void* ptr);
    extern void* fn_800FA280(u32 id);

    /* saved registers r23-r31 */
    u32  r23;     /* working value / species entry id / packed color word */
    u16* r24;     /* pointer advancing through the u16 name string        */
    u8*  r25;     /* self pointer (r3), reused as s32 index after loop    */
    u8*  r26;     /* output buffer (r4)                                   */
    u8*  r27;     /* &lbl_80266DD8[4] — word-1 base of entry table        */
    u8*  r28;     /* sp+0xc — inline u16[2] text buffer                  */
    s32  r29;     /* accumulated x position                               */
    u32  r30;     /* ctx pointer (*(void**)(self+0x60))                   */
    s32  r31;     /* character counter                                     */

    /* temporaries */
    s32  r0;
    u16  r6;

    /* scratch buffer on the (virtual) stack; two u16s written at sp+0xc/0xe */
    u16  text_buf[2];

    /* frame-local double storage for fctiwz idiom */
    union { f64 d; struct { u32 hi; u32 lo; } w; } fconv;

    r25 = (u8*)r3;
    r26 = r4;
    r30 = (u32)(uintptr_t)(*(void**)((u8*)r25 + 0x60));

    /*
     * Load entry[1] for the current map-table slot.
     * lbl_80266DD8 is a flat array of 16-byte entries; ctx+0x1c holds
     * the current entry index.  entry[1] is at byte offset 4.
     */
    r27 = lbl_80266DD8 + 4;             /* r27 = base of entry[1] words */
    r0  = (s32)( *(u32*)((u8*)(uintptr_t)r30 + 0x1c) ) << 4;
    r0  = (s32)*(u32*)(r27 + (u32)r0);  /* entry[1] for this slot */

    /* DrawLocation2 only operates when the slot type is 0xA */
    if (r0 != 0xa) {
        r26[0x67] = 0;
        return 0;
    }

    /* ---------------------------------------------------------------
     * Phase 1: walk the null-terminated u16 name string at ctx+0x18,
     * centre-drawing each character glyph.
     * --------------------------------------------------------------- */
    r31 = 0;                            /* character counter            */
    r24 = (u16*)(*(u32*)((u8*)(uintptr_t)r30 + 0x18));   /* name string ptr */
    r29 = 0;                            /* x accumulator                */
    r28 = (u8*)text_buf;               /* &sp+0xc text buffer           */

    goto loop_check;

loop_body:
    /* pack colour: self+0x8b byte OR'd with 0xFFFFFF00 = sign-extend to s32 */
    r0  = (s32)(u8)(r25[0x8b]) | (s32)(-0x100);
    r23 = (u32)r0;

    /* set up the two-element u16 text buffer: {r6, 0} */
    text_buf[0] = r6;
    text_buf[1] = 0;

    fn_80132A38(0x37, r28);

    /* get half-width of the glyph: upper s16 of fn_800FA444(0xce) */
    r0 = (s32)(s16)(u16)( fn_800FA444(0xce) >> 16 );

    /* x offset = r29 + floor((0x1b - half_width) / 2)             */
    r0 = 0x1b - r0;
    /* arithmetic divide-by-2: (r0 + (r0 >> 31)) >> 1 */
    r0 = (r0 + (s32)((u32)r0 >> 31)) >> 1;
    r0 = r29 + r0;

    fn_800FB680(r0, 0, (s32)r23, 0xce);

    r29 += 0x1a;
    r31 += 1;
    r24 += 1;     /* advance u16* by one element (+2 bytes) */

loop_check:
    r6 = *r24;
    if (r6 != 0)
        goto loop_body;

    /* ---------------------------------------------------------------
     * Phase 2: check if we need to draw the species highlight icon.
     * Conditions:
     *   (a) character count r31 < entry[1] for this slot
     *   (b) self+0x98 == 0 (not suppressed)
     * --------------------------------------------------------------- */
    r0 = (s32)( *(u32*)((u8*)(uintptr_t)r30 + 0x1c) ) << 4;
    r0 = (s32)*(u32*)(r27 + (u32)r0);   /* re-read entry[1] */

    if (r31 >= r0)
        goto done_ff;
    if ((u8)r25[0x98] != 0)
        goto done_ff;

    /* ---------------------------------------------------------------
     * Phase 3: resolve species/variant entry id (r23).
     * Uses three pointer-indirected values from ctx+0x24/0x28/0x2c.
     * val_a = **(u32**)(ctx+0x24)   must be [0,2)
     * r25   = **(u32**)(ctx+0x28)   used as index later
     * val_c = **(u32**)(ctx+0x2c)   must be [0,4)
     * species_id = lbl_80266E18[val_a*0x18 + val_c*4 + 8] as u32
     * --------------------------------------------------------------- */
    {
        u32* ptr_a  = *(u32**)((u8*)(uintptr_t)r30 + 0x24);
        u32* ptr_b  = *(u32**)((u8*)(uintptr_t)r30 + 0x28);
        u32* ptr_c  = *(u32**)((u8*)(uintptr_t)r30 + 0x2c);
        s32  val_a  = (s32)*ptr_a;
        s32  val_b  = (s32)*ptr_b;
        s32  val_c  = (s32)*ptr_c;
        s32  count;
        void* arr;

        /* r25 is repurposed to hold val_b from here */
        r25 = (u8*)(uintptr_t)(u32)val_b;

        if (val_a < 0 || val_a >= 2) {
            r23 = 0;
            goto after_species;
        }
        if (val_c < 0 || val_c >= 4) {
            r23 = 0;
            goto after_species;
        }

        /*
         * Index into lbl_80266E18:
         *   row = lbl_80266E18 + val_a * 0x18
         *   element = *(u32*)(row + val_c*4 + 8)
         */
        r23 = *(u32*)(lbl_80266E18 + (u32)val_a * 0x18 + (u32)val_c * 4 + 8);

        /* bounds-check val_b against the array length */
        count = fn_800FA314((void*)(uintptr_t)r23);
        if (val_b < 0 || val_b >= count) {
            r23 = 0;
            goto after_species;
        }

        /* r23 = array[val_b] as u16 */
        arr  = fn_800FA280(r23);
        r23  = (u32)*(u16*)((u8*)arr + (u32)(u32)val_b * 2);
    }

after_species:
    /* Skip if species id resolved to 0 */
    if ((u16)r23 == 0)
        goto done_ff;

    /* ---------------------------------------------------------------
     * Phase 4: compare against "currently displayed" species id.
     * fn_800FA280(0x2efc) → array, lhz [0] = active species u16.
     * If r23 matches → r0=0 (same species); else r0=6 (different).
     * Only draw when r0==6 (NOT the active species).
     * --------------------------------------------------------------- */
    {
        void* cur_arr = fn_800FA280(0x2efc);
        u16   cur_id  = *(u16*)((u8*)cur_arr);
        if ((u16)r23 == cur_id)
            r0 = 0;
        else
            r0 = 6;
    }

    if (r0 != 6)
        goto done_ff;

    /* ---------------------------------------------------------------
     * Phase 5: compute the animated x position and draw the icon.
     *
     * float at *(f32*)(ctx+0x30):  animation phase
     * packed color: (u8)(lbl_8047B938 * (lbl_8047B934 - phase)) | 0x00FF0000
     * The u16 species id r23 goes into text_buf[0]; text_buf[1] = 0.
     * x = r31 * 0x1a + floor((0x1b - half_width) / 2)
     * --------------------------------------------------------------- */
    {
        f32* fptr = *(f32**)((u8*)(uintptr_t)r30 + 0x30);
        f32  f0, f1, f2;
        u32  int_val;

        f1 = lbl_8047B934;
        f0 = *fptr;
        f2 = lbl_8047B938;

        f0 = f2 * (f1 - f0);           /* fsubs then fmuls */

        /* fctiwz → store as f64, pick up integer from low word (big-endian +4) */
        fconv.d = (f64)f0;              /* fctiwz truncation: use (s32) cast    */
        int_val = (u32)(s32)f0;         /* ENDIAN-QA: stfd/lwz+4 = lower 32 bits */

        /* clrlwi 24 = mask to 8 bits */
        r0  = (s32)((u8)int_val);
        /* oris r23, r0, 0xff: r23 = r0 | 0x00FF0000 */
        r23 = (u32)r0 | 0x00FF0000u;

        /* set up text buffer with species id */
        text_buf[0] = (u16)(r23 & 0xFFFF);  /* sth r23, 0x8(r1) — only low 16 written */
        text_buf[1] = 0;                      /* sth r0=0, 0xa(r1)                      */

        fn_80132A38(0x37, r28);

        /* half-width centering */
        r0 = (s32)(s16)(u16)(fn_800FA444(0xce) >> 16);
        r0 = 0x1b - r0;
        r0 = (r0 + (s32)((u32)r0 >> 31)) >> 1;

        /* x = r31 * 0x1a + centered_offset */
        r0 = (s32)r31 * 0x1a + r0;

        fn_800FB680(r0, 0, (s32)r23, 0xce);
    }

done_ff:
    r26[0x67] = 0xff;
    return 0;
}
#endif

/* fn_80026FEC - 0x80026FEC | size: 0x190 */
extern u32 lbl_8047B928;
extern u32 lbl_8047B92C;
extern u8 lbl_802EF0A8[];
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 1
asm void fn_80026FEC(void) {
#include "src/game/gs_worldmap_fn_80026FEC.inc"
}
#else
/*
 * fn_80026FEC  GSmap_DrawRoute0  (0x80026FEC, size 0x190)
 *
 * Callback for drawing a world-map route segment of type 0.
 *
 * r3  = callback-object ptr  (ctx ptr at +0x60)
 * r4  = node/sprite struct   (s16 key at +0x6; output s16s at +0x50/+0x52/+0x54/+0x56;
 *                             output u8 alpha at +0x67)
 *
 * The function selects a u16 discriminant from a two-element table built from
 * lbl_8047B928/lbl_8047B92C using indices derived from two s32 fields of the
 * context.  If the discriminant matches node->field_06 the route vertex data
 * is unpacked, transformed with a scale-factor + offset, and written back.
 * Otherwise the node's alpha byte is zeroed.
 *
 * Big-endian s16->f32 trick normalised to plain casts. ENDIAN-QA on all four
 * entry fields read via lha + xoris+0x8000 sequences.
 */
s32 fn_80026FEC(void* r3, u8* r4)
{
    /* ---- SDA globals (block-scope externs, TU convention) ---- */
    extern u32 lbl_8047B928;        /* u16 table slot 0+1 packed as u32 */
    extern u32 lbl_8047B92C;        /* u16 table slot 2+3 packed as u32 */
    extern u8  lbl_802EF0A8[];      /* route-vertex data array, stride 0x1c */
    extern f64 lbl_8047B948;        /* 0x4330000080000000 s16->f64 bias constant */
    extern f32 lbl_8047B93C;        /* route scale coefficient A */
    extern f32 lbl_8047B940;        /* route scale coefficient B */
    extern f32 lbl_8047B934;        /* map half-width / max coordinate */
    extern f32 lbl_8047B938;        /* alpha scale factor */

    /* ---- locals ---- */
    u16   sp_table[4];   /* two SDA u32s overlay a 4-element u16 lookup table */
    u16   sel;
    void* ctx;
    s32   val_a;         /* s32 from *(u32**)(ctx+0x28)[0] */
    s32   val_b;         /* s32 from *(u32**)(ctx+0x2c)[0] */
    u8*   entry;         /* pointer into lbl_802EF0A8 at stride 0x1c */
    f32*  scale_ptr;     /* *(f32**)(ctx+0x30), game-state scale float */
    f32   f10;           /* runtime scale = *scale_ptr */
    f32   e2, e4, e6, e8;/* s16 fields from route entry at +0x2, +0x4, +0x6, +0x8 */
    f32   f2, f7, f6, f1, f0; /* intermediate FP */
    f32   f3, f5;
    f32   alpha_f;
    s32   x0, y0, x1, y1;
    s32   alpha_int;
    s16   cmp_val;

    /*
     * Build the 4-entry u16 lookup table from the two SDA u32 words.
     * On big-endian PPC a 32-bit store places the high half at the lower address,
     * so slot 0 = lbl_8047B928 >> 16, slot 1 = lbl_8047B928 & 0xffff, etc.
     * On the x86 host we replicate the same layout explicitly. ENDIAN-QA
     */
    sp_table[0] = (u16)(lbl_8047B928 >> 16); /* ENDIAN-QA */
    sp_table[1] = (u16)(lbl_8047B928 & 0xffff);
    sp_table[2] = (u16)(lbl_8047B92C >> 16);
    sp_table[3] = (u16)(lbl_8047B92C & 0xffff);

    /* Unpack context */
    ctx    = *(void**)((u8*)r3 + 0x60);
    val_a  = *(s32*)(*(u8**)(ctx + 0x28)); /* *ptr_A */
    val_b  = *(s32*)(*(u8**)(ctx + 0x2c)); /* *ptr_B */

    /* Select discriminant u16 from the table.
     * Original condition: val_a >= 0xf means the outer check passes.
     * Then val_b must be in [0,3] to index the table; otherwise 0xFFFF sentinel. */
    if (val_a < 0xf) {
        sel = 0xffff;
    } else if (val_b < 0 || val_b >= 4) {
        sel = 0xffff;
    } else {
        sel = sp_table[val_b]; /* lhzx: zero-extended u16 */
    }

    /* Compare against the node's s16 key field (+0x6).
     * lha sign-extends; clrlwi zero-extends sel: comparison is unsigned u16 vs u16. */
    cmp_val = *(s16*)(r4 + 0x6);
    if ((u16)sel != (u16)(u32)cmp_val) {
        /* No match: mark node invisible */
        r4[0x67] = 0;
        return 0;
    }

    /* ---- Matching path: compute and write route vertex data ---- */

    /* Route entry in the vertex array; entry index = cmp_val, stride = 0x1c */
    entry = lbl_802EF0A8 + (s32)cmp_val * 0x1c;

    /* Runtime map scale from the context's float pointer */
    scale_ptr = *(f32**)(ctx + 0x30);
    f10       = *scale_ptr;          /* lfs f10, 0x0(r5) */

    /*
     * Unpack four s16 fields from the route entry via the big-endian
     * xoris+0x8000 / stfd / lfd / fsubs trick, normalised to plain casts.
     * ENDIAN-QA: the game reads native big-endian s16 fields with lha.
     */
    e6 = (f32)(s32)*(s16*)(entry + 0x6); /* ENDIAN-QA */
    e8 = (f32)(s32)*(s16*)(entry + 0x8); /* ENDIAN-QA */
    e2 = (f32)(s32)*(s16*)(entry + 0x2); /* ENDIAN-QA */
    e4 = (f32)(s32)*(s16*)(entry + 0x4); /* ENDIAN-QA */

    /*
     * Scale pass:
     *   f2 = e6 * f10
     *   f1_tmp = e8 * f10
     *   f6 = lbl_8047B93C * f2
     *   f7 = lbl_8047B93C * f1_tmp
     */
    f2  = e6 * f10;
    f1  = e8 * f10;
    f6  = lbl_8047B93C * f2;
    f7  = lbl_8047B93C * f1;

    /*
     * Compute output coordinates:
     *   x1 = e6 + lbl_8047B93C * e6 * f10
     *      = fadds(f9, f6)   where f9 = e6   [f1 register in asm]
     *   x0 = e2 - lbl_8047B93C * lbl_8047B940 * e6 * f10
     *      = fnmsubs(f6, lbl_8047B940, e2)   [f2 register in asm]
     *   y1 = e8 + lbl_8047B93C * e8 * f10
     *      = fadds(f8, f7)                   [f0 register in asm]
     *   y0 = e4 - lbl_8047B93C * lbl_8047B940 * e8 * f10
     *      = fnmsubs(f7, lbl_8047B940, e4)   [second f2 in asm]
     */
    x1 = (s32)(e6  + f6);
    f0 = e2  - f6  * lbl_8047B940;   /* x0 float */
    x0 = (s32)f0;
    y1 = (s32)(e8  + f7);
    f3 = e4  - f7  * lbl_8047B940;   /* y0 float */
    y0 = (s32)f3;

    /*
     * Alpha scalar:
     *   f5 = lbl_8047B934 - f10           (fsubs f5, f3, f10 in asm, where f3=lbl_8047B934)
     *   alpha = lbl_8047B938 * f5          (fmuls f3, f6, f5 in asm, where f6=lbl_8047B938)
     *   fctiwz -> truncate to int
     */
    f5       = lbl_8047B934 - f10;
    alpha_f  = lbl_8047B938 * f5;
    alpha_int = (s32)alpha_f;         /* fctiwz */

    /* Write vertex outputs */
    *(s16*)(r4 + 0x50) = (s16)x0;
    *(s16*)(r4 + 0x52) = (s16)y0;
    *(s16*)(r4 + 0x54) = (s16)x1;
    *(s16*)(r4 + 0x56) = (s16)y1;

    /* Write alpha byte (lower byte of the integer conversion) */
    r4[0x67] = (u8)(u32)alpha_int;

    return 0;
}
#endif

/* fn_8002717C - 0x8002717C | size: 0x190 */
extern u32 lbl_8047B928;
extern u32 lbl_8047B92C;
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 1
asm void fn_8002717C(void) {
#include "src/game/gs_worldmap_fn_8002717C.inc"
}
#else
/*
 * GSmap_DrawRoute1  (0x8002717C, 0x190 bytes)
 * Checks whether a world-map route entry matches the sprite's expected route ID,
 * then projects the route's four s16 data fields into screen coordinates stored
 * at r4[0x50..0x57].  r4[0x67] receives the projected "alpha/depth" byte or 0.
 *
 * Parameters:
 *   r3 = self / GSmap object (void*)
 *   r4 = sprite / output record (u8*)
 *
 * Big-endian note: lbl_8047B928 and lbl_8047B92C are each a u32 that the PPC
 * code treats as two packed big-endian u16 values on the stack.  On the x86
 * host we reconstruct the same four u16 slots via explicit bit-shifts.
 * Marked ENDIAN-QA below.
 */
s32 fn_8002717C(void* r3, u8* r4)
{
    extern u32 lbl_8047B928;   /* route-ID table word 0  (2 packed u16 big-endian) */
    extern u32 lbl_8047B92C;   /* route-ID table word 1  (2 packed u16 big-endian) */
    extern u8  lbl_802EF0A8[]; /* world-map entry table, stride 0x1c */
    extern f64 lbl_8047B948;   /* int->float bias constant (0x4330000080000000) */
    extern f32 lbl_8047B93C;   /* projection constant K1 */
    extern f32 lbl_8047B940;   /* projection constant K2 */
    extern f32 lbl_8047B934;   /* projection constant K3 */
    extern f32 lbl_8047B938;   /* projection constant K4 */

    u8*  ctx;       /* *(u8**)(r3+0x60) */
    u8*  ptr28;     /* *(u8**)(ctx+0x28) */
    u8*  ptr2c;     /* *(u8**)(ctx+0x2c) */
    s32  val28;     /* *(s32*) ptr28      - first  gating value */
    s32  val2c;     /* *(s32*) ptr2c      - route-slot index  */
    u16  sel;       /* selected route-ID from packed table     */
    s32  loc_id;    /* (s16)(r4+0x6)     - sprite's route ID  */
    u8*  entry;     /* &lbl_802EF0A8[loc_id * 0x1c]           */
    f32* scale_ptr; /* *(f32**)(ctx+0x30)                     */
    f32  scale;     /* *scale_ptr                              */
    f32  e2, e4, e6, e8; /* s16 fields at entry+2,+4,+6,+8   */
    f32  f9, f8;         /* e6*scale, e8*scale before K1      */
    f32  f6, f7;         /* K1*f9*scale,  K1*f8*scale         */
    f32  r_x1, r_y1;     /* projected coordinates             */
    f32  r_x2, r_y2;     /* second pair                       */
    f32  f_depth;        /* K4*(K3-scale)                     */
    s32  i_x1, i_y1, i_x2, i_y2, i_depth;

    ctx    = *(u8**)((u8*)r3 + 0x60);
    ptr28  = *(u8**)(ctx + 0x28);
    val28  = *(s32*)ptr28;
    ptr2c  = *(u8**)(ctx + 0x2c);
    val2c  = *(s32*)ptr2c;

    /* Build the 4-slot u16 table from the two packed u32 globals.
     * PPC stores them big-endian on the stack then reads u16 pairs:
     *   slot 0 = high u16 of lbl_8047B928
     *   slot 1 = low  u16 of lbl_8047B928
     *   slot 2 = high u16 of lbl_8047B92C
     *   slot 3 = low  u16 of lbl_8047B92C
     * ENDIAN-QA: extraction is endian-normalised for x86 host. */
    {
        u16 tab[4];
        tab[0] = (u16)(lbl_8047B928 >> 16);
        tab[1] = (u16)(lbl_8047B928 & 0xFFFF);
        tab[2] = (u16)(lbl_8047B92C >> 16);
        tab[3] = (u16)(lbl_8047B92C & 0xFFFF);

        /* Gate 1: val28 must be >= 15 for the route slot to be meaningful */
        if (val28 < 15) {
            sel = 0xFFFF;
        } else if (val2c < 0 || val2c >= 4) {
            sel = 0xFFFF;
        } else {
            sel = tab[val2c];
        }
    }

    loc_id = (s32)(s16)(*(u16*)(r4 + 0x6));

    if ((u16)sel != (u16)loc_id) {
        r4[0x67] = 0;
        return 0;
    }

    /* Locate the entry record */
    entry     = lbl_802EF0A8 + (u32)((s32)loc_id * 0x1c);
    scale_ptr = *(f32**)(ctx + 0x30);
    scale     = *scale_ptr;

    /* Convert four s16 fields to float (ENDIAN-QA: xoris/lfd trick -> plain cast) */
    e6 = (f32)(s32)(s16)*(s16*)(entry + 0x6); /* ENDIAN-QA */
    e8 = (f32)(s32)(s16)*(s16*)(entry + 0x8); /* ENDIAN-QA */
    e2 = (f32)(s32)(s16)*(s16*)(entry + 0x2); /* ENDIAN-QA */
    e4 = (f32)(s32)(s16)*(s16*)(entry + 0x4); /* ENDIAN-QA */

    /* f9 = e6 * scale, f8 = e8 * scale */
    f9 = e6 * scale;
    f8 = e8 * scale;

    /* f6 = K1 * f9,  f7 = K1 * f8 */
    f6 = lbl_8047B93C * f9;
    f7 = lbl_8047B93C * f8;

    /* Projected coordinates (fnmsubs = -(a*b) + c) */
    r_x1  = e6  + f6;                              /* fadds f1 = f9 + f6 */
    r_x2  = e2  - f6 * lbl_8047B940;               /* fnmsubs f2 = -(f6*K2) + e2 */
    r_y2  = e4  - f7 * lbl_8047B940;               /* fnmsubs f2 = -(f7*K2) + e4 */
    r_y1  = e8  + f7;                               /* fadds f0 = f8 + f7 */
    f_depth = lbl_8047B938 * (lbl_8047B934 - scale); /* fmuls f3 = K4*(K3-scale) */

    /* Truncate to integer */
    i_x2    = (s32)r_x2;   /* fctiwz -> truncate toward zero */
    i_x1    = (s32)r_x1;
    i_y2    = (s32)r_y2;
    i_y1    = (s32)r_y1;
    i_depth = (s32)f_depth;

    /* Store four s16 screen coordinates to sprite record */
    *(s16*)(r4 + 0x50) = (s16)i_x2;
    *(s16*)(r4 + 0x52) = (s16)i_y2;
    *(s16*)(r4 + 0x54) = (s16)i_x1;
    *(s16*)(r4 + 0x56) = (s16)i_y1;

    /* Store depth/alpha byte */
    r4[0x67] = (u8)(s32)i_depth;

    return 0;
}
#endif

/* fn_8002730C - 0x8002730C | size: 0x190 */
extern u32 lbl_8047B928;
extern u32 lbl_8047B92C;
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B940;
extern f32 lbl_8047B934;
extern f32 lbl_8047B938;
#if 1
asm void fn_8002730C(void) {
#include "src/game/gs_worldmap_fn_8002730C.inc"
}
#else
/*
 * GSmap_DrawRoute2 -- Draw route line type 2
 * 0x8002730C | size: 0x190
 *
 * Args:  r3 = self/callback context pointer
 *        r4 = render packet / output buffer (u8*)
 *
 * The function selects a route entry from lbl_802EF0A8[] (stride 0x1c)
 * by matching the packet's route ID field (r4[6..7] as s16) against a
 * 4-entry u16 lookup table packed from two SDA globals.  On a match it
 * projects four s16 coordinate fields through a scale + affine transform
 * and writes the results as four s16 pairs into r4[0x50..0x57], then
 * stores a derived byte into r4[0x67].  Always returns 0.
 *
 * Big-endian int->float idiom (0x43300000 / xoris / lfd / fsubs) is
 * rewritten as plain (f32)(s32) casts -- ENDIAN-QA
 */
s32 fn_8002730C(void* r3, u8* r4)
{
    /* SDA globals -- block-scope as per TU convention */
    extern u32 lbl_8047B928;   /* packed u16[2]: table entries [0],[1] */
    extern u32 lbl_8047B92C;   /* packed u16[2]: table entries [2],[3] */
    extern f64 lbl_8047B948;   /* int->float magic const (not used after rewrite) */
    extern f32 lbl_8047B93C;   /* affine scale coefficient A */
    extern f32 lbl_8047B940;   /* affine scale coefficient B */
    extern f32 lbl_8047B934;   /* base value for derived byte computation */
    extern f32 lbl_8047B938;   /* multiplier for derived byte computation */
    /* Route data table: entries are 0x1c (28) bytes each */
    extern u8 lbl_802EF0A8[];

    /* Stack-local u16 lookup table packed from two SDA u32 globals.
     * On big-endian PPC: lbl_8047B928 = { u16[0], u16[1] }
     *                    lbl_8047B92C = { u16[2], u16[3] }
     * On the x86 host those u32s are stored LE, so the packed bytes
     * are byte-swapped relative to PPC.  The original code reads them
     * with lhzx on the raw stack words -- which works on PPC because
     * it accesses the big-endian bytes.  On x86 we must byte-swap
     * each u16 when building the table.  *  ENDIAN-QA * 
     * We expose the swap here so host callers see the same logical
     * values the PPC code intended.
     */
    u16 route_table[4];
    {
        u32 w0 = lbl_8047B928;
        u32 w1 = lbl_8047B92C;
        /* PPC big-endian packing: high halfword = entry[0], low = entry[1] */
        route_table[0] = (u16)(w0 >> 16);
        route_table[1] = (u16)(w0 & 0xFFFF);
        route_table[2] = (u16)(w1 >> 16);
        route_table[3] = (u16)(w1 & 0xFFFF);
    }

    void*  ctx  = *(void**)((u8*)r3 + 0x60);
    u32*   ptr_a = *(u32**)((u8*)ctx + 0x28);
    u32    val_a = ptr_a[0];           /* deref of ctx->ptr_0x28 */
    u32*   ptr_b = *(u32**)((u8*)ctx + 0x2c);
    u32    val_b = ptr_b[0];           /* deref of ctx->ptr_0x2c -- route slot index */

    /* Determine which route ID to match against r4[6..7].
     * If val_a < 15 the slot is considered invalid -> sentinel 0xFFFF.
     * Otherwise use val_b as an index into route_table[0..3];
     * any out-of-range val_b also yields 0xFFFF. */
    u16 table_val;
    if ((s32)val_a < 0xf) {
        table_val = 0xFFFF;
    } else if ((s32)val_b < 0 || (s32)val_b >= 4) {
        table_val = 0xFFFF;
    } else {
        table_val = route_table[val_b];
    }

    /* r4[6..7] holds the route ID as a signed 16-bit integer.
     * The PPC code compares the zero-extended table_val (clrlwi, 16)
     * against the sign-extended field value (lha) as a full word.
     * For a valid table entry (0x0000..0xFFFE) this only matches a
     * non-negative s16; 0xFFFF never matches a valid s16 (it would
     * equal (s32)-1 as s16, but the sentinel is treated as u16=65535
     * and the s16 field is sign-extended -- so cmpw compares 65535 vs
     * a sign-extended value, effectively always a mismatch for 0xFFFF). */
    s16  route_id = *(s16*)(r4 + 0x6);

    if ((s32)(u32)table_val != (s32)route_id) {
        /* mismatch */
        r4[0x67] = 0;
        return 0;
    }

    /* Matched: compute projected coordinates for this route entry. */

    /* Route table entry base */
    u8* entry = lbl_802EF0A8 + (s32)route_id * 0x1c;

    /* s16 coordinate fields from the route entry -- ENDIAN-QA (big-endian s16 on PPC) */
    f32 e2 = (f32)(s16)(*(s16*)(entry + 0x2));   /* entry->x0 */
    f32 e4 = (f32)(s16)(*(s16*)(entry + 0x4));   /* entry->y0 */
    f32 e6 = (f32)(s16)(*(s16*)(entry + 0x6));   /* entry->x1 */
    f32 e8 = (f32)(s16)(*(s16*)(entry + 0x8));   /* entry->y1 */

    /* Scale factor: a float pointer stored at ctx+0x30 */
    f32 scale = *(f32*)(*(u8**)((u8*)ctx + 0x30));   /* **(f32**)(ctx+0x30) */

    /* Intermediate scaled values */
    f32 sx1 = e6 * scale;   /* field_6 * scale */
    f32 sx2 = e8 * scale;   /* field_8 * scale */

    f32 coeff_a = lbl_8047B93C;
    f32 coeff_b = lbl_8047B940;

    f32 fx6 = coeff_a * sx1;   /* A * (field_6 * scale) */
    f32 fx7 = coeff_a * sx2;   /* A * (field_8 * scale) */

    /* Affine projected coordinates (fnmsubs = -(a*b) + c = c - a*b) */
    f32 proj_x0 = e2  - fx6 * coeff_b;          /* field_2  - A*sx1*B */
    f32 proj_y0 = e4  - fx7 * coeff_b;          /* field_4  - A*sx2*B */
    f32 proj_x1 = e6  + fx6;                    /* field_6  + A*sx1  = field_6*(1+A*scale) */
    f32 proj_y1 = e8  + fx7;                    /* field_8  + A*sx2  = field_8*(1+A*scale) */

    /* Derived byte value */
    f32 base_val = lbl_8047B934;
    f32 mul_val  = lbl_8047B938;
    f32 derived_f = mul_val * (base_val - scale);

    /* Write projected coordinates as s16 pairs into the output packet.
     * fctiwz = round-toward-zero (truncate) then take low 32 bits as s32. */
    *(s16*)(r4 + 0x50) = (s16)(s32)proj_x0;
    *(s16*)(r4 + 0x52) = (s16)(s32)proj_y0;
    *(s16*)(r4 + 0x54) = (s16)(s32)proj_x1;
    *(s16*)(r4 + 0x56) = (s16)(s32)proj_y1;

    /* r4[0x67] = low byte of truncated derived float */
    r4[0x67] = (u8)(s32)derived_f;

    return 0;
}
#endif

/* fn_8002749C - 0x8002749C | size: 0x158 */
extern f64 lbl_8047B948;
extern f32 lbl_8047B93C;
extern f32 lbl_8047B934;
extern f32 lbl_8047B940;
extern f32 lbl_8047B938;
#if 1
asm void fn_8002749C(void) {
#include "src/game/gs_worldmap_fn_8002749C.inc"
}
#else
/*
 * GSmap_DrawMapBackground -- 0x8002749C | size: 0x158
 *
 * Computes screen position and texture UV coordinates for a world-map
 * background tile and writes them into the caller-supplied output buffer.
 *
 * Parameters:
 *   self -- GS object whose +0x60 word is the context/state pointer
 *   r4   -- per-sprite output buffer; r4[6..7] (s16) carries the entry
 *           index on input; on output receives:
 *             r4[0x50..0x51] s16  texture U left edge
 *             r4[0x52..0x53] s16  texture V top  edge
 *             r4[0x54..0x55] s16  screen X
 *             r4[0x56..0x57] s16  screen Y
 *             r4[0x67]       u8   texture column-count (or 0 = skip draw)
 *
 * Endian note: the 0x4330/xoris/lfd/fsubs sequences are the standard
 * big-endian s16->f32 and s32->f32 idioms; all converted to plain C casts
 * below. *  ENDIAN-QA * 
 */
s32 fn_8002749C(void *self, u8 *r4)
{
    /* ---- sdata2 globals (r2-relative) ---- */
    extern f64 lbl_8047B948;  /* int->float bias constant (0x4330000080000000) */
    extern f32 lbl_8047B93C;  /* map half-pixel / uv prescale multiplier        */
    extern f32 lbl_8047B934;  /* texture reference size                         */
    extern f32 lbl_8047B940;  /* uv correction scale                            */
    extern f32 lbl_8047B938;  /* column-count scale                             */

    /* ---- ROM data table (r3-relative) ---- */
    extern u8 lbl_802EF0A8[];  /* canonical; per-site reinterpret cast */



    /* ---------- local variables ---------- */
    u8  *ctx;          /* GS context/state block (ctx = *(u8**)(self+0x60))    */
    s16  entry_idx;    /* array index read from r4[6]                          */
    s16 *entry;        /* pointer into ((s16*)lbl_802EF0A8) for this entry             */
    u32  tile_id;      /* *(u32*)*(ctx+0x28) -- tile/map ID to range-check     */
    u32  tile_row;     /* *(u32*)*(ctx+0x2c) -- texture row index              */
    f32  scale;        /* *(f32*)*(ctx+0x30) -- map zoom/scale float           */
    f32  entry_x;      /* screen-space x from entry table (as f32)             */
    f32  entry_y;      /* screen-space y from entry table (as f32)             */
    f32  sx;           /* entry_x * scale                                      */
    f32  sy;           /* entry_y * scale                                      */
    f32  hsx;          /* lbl_8047B93C * sx  (half-texel pre-biased component) */
    f32  hsy;          /* lbl_8047B93C * sy                                    */
    s32  tex_u_base;   /* integer texture U origin  (*ptrs_38 + tile_id  * 27) */
    s32  tex_v_base;   /* integer texture V origin  (*ptrs_3c + tile_row * 35) */
    f32  fu_base;      /* (f32)tex_u_base                                      */
    f32  fv_base;      /* (f32)tex_v_base                                      */
    f32  tu;           /* final texture U (float, before truncate)             */
    f32  tv;           /* final texture V (float, before truncate)             */
    f32  screen_x_f;   /* final screen X (float, before truncate)              */
    f32  screen_y_f;   /* final screen Y (float, before truncate)              */
    s32  col_count;    /* (s32)(lbl_8047B938*(lbl_8047B934-scale)) -- #columns */

    /* ---- decode parameters ---- */
    ctx       = *(u8 **)((u8 *)self + 0x60);
    entry_idx = *(s16 *)(r4 + 6);                        /* lha r0, 0x6(r4)   */
    entry     = (s16 *)((u8 *)((s16*)lbl_802EF0A8) + (s32)entry_idx * 0x1c);

    scale     = *(f32 *)(*(u8 **)(ctx + 0x30));          /* lfs f8, 0x0(r3) after lwz r3,0x30(r9) */
    tile_id   = *(u32 *)(*(u8 **)(ctx + 0x28));          /* lwz r7, 0x0(r8) */
    tile_row  = *(u32 *)(*(u8 **)(ctx + 0x2c));          /* lwz r8, 0x0(r5) */

    /* convert s16 entry fields to f32 -- ENDIAN-QA:
       entry bytes 6..7 = x, bytes 8..9 = y (big-endian s16 pair) */
    entry_x = (f32)(s32)(s16)(entry[3]);  /* lha r5,0x6(r7); ... fsubs f3,f1,f7 */
    entry_y = (f32)(s32)(s16)(entry[4]);  /* lha r0,0x8(r7); ... fsubs f4,f0,f7 */

    sx  = entry_x * scale;               /* fmuls f1, f3, f8 */
    sy  = entry_y * scale;               /* fmuls f0, f4, f8 */
    hsx = lbl_8047B93C * sx;             /* fmuls f9, f2, f1 */
    hsy = lbl_8047B93C * sy;             /* fmuls f10, f2, f0 */

    /* range-check: tile_id must be < 15 to produce output */
    if (tile_id >= 0xf) {
        /* @L_800275E0 */
        r4[0x67] = 0;
        return 0;
    }

    /* ---- screen position (bilinear map-to-screen mapping) ---- */
    screen_x_f = entry_x + hsx;          /* fadds f1, f3, f9 */
    screen_y_f = entry_y + hsy;          /* fadds f0, f4, f10 */

    /* ---- texture UV origin (integer coords converted to f32) ---- */
    tex_u_base = (s32)(*(u32 *)(*(u8 **)(ctx + 0x38))) + (s32)tile_id  * 0x1b;  /* add r0,r3,r0 (r7*27) -- ENDIAN-QA */
    tex_v_base = (s32)(*(u32 *)(*(u8 **)(ctx + 0x3c))) + (s32)tile_row * 0x23;  /* add r0,r5,r3 (r8*35) -- ENDIAN-QA */

    fu_base = (f32)tex_u_base;           /* xoris+lfd+fsubs chain, s32->f32 */
    fv_base = (f32)tex_v_base;           /* xoris+lfd+fsubs chain, s32->f32 */

    /* texture UV: offset from base minus UV-correction term */
    tu = fu_base - hsx * lbl_8047B940;  /* fnmsubs f3, f9,  f4, f2 */
    tv = fv_base - hsy * lbl_8047B940;  /* fnmsubs f0, f10, f4, f1 */

    /* texture column count: width of the background tile in texels */
    col_count = (s32)(lbl_8047B938 * (lbl_8047B934 - scale)); /* fmuls f5,f6,f5; fctiwz f3,f5 */

    /* ---- write outputs ---- */
    *(s16 *)(r4 + 0x50) = (s16)(s32)tu;           /* sth r5, 0x50(r4) */
    *(s16 *)(r4 + 0x52) = (s16)(s32)tv;           /* sth r5, 0x52(r4) */
    *(s16 *)(r4 + 0x54) = (s16)(s32)screen_x_f;   /* sth r3, 0x54(r4) */
    *(s16 *)(r4 + 0x56) = (s16)(s32)screen_y_f;   /* sth r0, 0x56(r4) */
    r4[0x67] = (u8)(s8)col_count;                  /* stb r6, 0x67(r4) */

    return 0;
}
#endif

/* fn_800275F4 - 0x800275F4 | size: 0x14c */
#if 0
asm void fn_800275F4(void) {
#include "src/game/gs_worldmap_fn_800275F4.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 fn_800275F4(void* r3) {
    s32 r31;     /* x_acc */
    s32 r30;     /* byte_off */
    u8* r29;     /* inner_ptr */
    s32 r28;     /* y_acc */
    u8* r27;     /* base_ptr */
    u16* r26;    /* pbuf */
    s32 r25;     /* item */
    s32 r24;     /* outer_ctr */
    s32 r23;     /* loc_idx */
    void* r22;   /* self */
    void* r21;   /* entry_ptr */
    u8* ctx;
    u8* arr;
    u16 buf[2];
    u16 r6;
    s32 r0;
    r22 = r3;
    ctx = *(u8**)((u8*)r22 + 0x60);
    r23 = *(s32*)(*(u8**)((u8*)ctx + 0x24));
    r24 = 0;
    r28 = 0;
    r27 = lbl_80266E18 + r23 * 0x18;
    r26 = buf;
    while (r24 < 4) {
        r31 = 0;
        r29 = r27 + 8;
        r30 = 0;
        r25 = 0;
        r6 = 0;
        goto inner_check;
        while (r6 != 0) {
            r26[0] = r6; r26[1] = 0;
            fn_80132A38(0x37, r26);
            r0 = (s32)(s16)(u16)((u32)fn_800FA444(0xce) >> 16);
            r0 = (0x1b - r0);
            r0 = (r0 + (s32)((u32)r0 >> 31)) >> 1;
            fn_800FB680(r31 + r0, r28, (s32)((u8*)r22)[0x8b] | (s32)(-0x100), 0xce);
            r31 += 0x1b;
            r30 += 2;
            r25++;
        inner_check:
            if (r23 < 0 || r23 >= 2) { r6 = 0; continue; }
            if (r24 < 0 || r24 >= 4) { r6 = 0; continue; }
            r21 = *(void**)r29;
            r0 = fn_800FA314(r21);
            if (r25 < 0 || r25 >= r0) { r6 = 0; continue; }
            arr = (u8*)fn_800FA280((u32)r21);
            r6 = *(u16*)(arr + r30);
        }
        r28 += 0x23;
        r27 += 4;
        r24++;
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8002777C - 0x8002777C | size: 0x3c */
#if 0
asm void fn_8002777C(void) {
#include "src/game/gs_worldmap_fn_8002777C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002777C(void* r3) {
    fn_800FB680(0, 0, (s32)(((u8*)r3)[0x8b]) | (s32)(-0x100), 0x2ef3);
    return 0;
}
#endif

/* fn_800277B8 - 0x800277B8 | size: 0x3c */
#if 0
asm void fn_800277B8(void) {
#include "src/game/gs_worldmap_fn_800277B8.inc"
}
#else
#pragma optimization_level 4
s32 fn_800277B8(void* r3) {
    fn_800FB680(0, 0, (s32)(((u8*)r3)[0x8b]) | (s32)(-0x100), 0x2ef4);
    return 0;
}
#endif

/* fn_800278A4 - 0x800278A4 | size: 0xbc */
extern s32 fn_8011F5C8(void*);
#if 0
asm void fn_800278A4(void) {
#include "src/game/gs_worldmap_fn_800278A4.inc"
}
#else
#pragma optimization_level 4
s32 fn_800278A4(void* r3) {
    u8* r29;
    void* r31;
    void* r30;
    s32 r4;
    u32* entry;
    r29 = (u8*)r3;
    r31 = *(void**)(r29 + 0x60);
    if (*(s32*)r31 == 2) {
        r30 = fn_8012A5B0(0, 3, (u16)*(u32*)((u8*)r31 + 0x4));
        if ((fn_80123FBC() & 0xff) == 0) {
            r4 = fn_8011F5C8(r30);
        } else {
            r4 = 1;
        }
        fn_80132A38(0x4e, (void*)(u32)(u16)r4);
    }
    entry = (u32*)(lbl_80266DD8 + (*(s32*)r31 << 4));
    fn_800FB680(0, 0, (s32)r29[0x8b] | (s32)(-0x100), entry[0]);
    return 0;
}
#endif

/* fn_80027960 - 0x80027960 | size: 0x144 */
extern u32 lbl_8047B920;
#if 1
asm void fn_80027960(void) {
#include "src/game/gs_worldmap_fn_80027960.inc"
}
#else
u16 fn_80027960_TODO(u16 r26, s32 r27) {
    u32* r29;
    u16* r5;
    s32 r28, r30, r31, r4;
    r28 = 0;
    r31 = 0;
    do {
        u32 ptr;
        r29 = (u32*)lbl_8047B920;
        ptr = r29[r28];
        if (ptr == 0) goto next;
        r30 = fn_800FA314((void*)ptr);
        r5 = (u16*)((u8*)fn_800FA280(r29[r28]) + 2);
        r4 = 1;
        if (r30 > 1) {
            s32 ctr = r30 >> 1;
            do {
                if (*r5 == r26) break;
                r4 += 2;
                r5 += 2;
            } while (--ctr);
        }
        if (r4 < r30) goto found;
    next:
        r28++;
        r31 += 4;
    } while (r28 < 2);

found:
    if (r28 >= 2) r28 = 0;
    if (r28 == r27) return r26;
    if (r28 == 1) {
        r26 = *(r5 - 1);
    }
    if (r27 == 0) return r26;
    r30 = r27 << 2;
    r29 = (u32*)lbl_8047B920;
    r31 = fn_800FA314((void*)r29[r27]);
    r5 = (u16*)fn_800FA280(r29[r27]);
    r4 = 0;
    if (r31 > 0) {
        s32 ctr = (r31 + 1) >> 1;
        do {
            if (*r5 == r26) break;
            r4 += 2;
            r5 += 2;
        } while (--ctr);
    }
    if (r4 < r31) return *(r5 + 1);
    return 0;
}
#endif

/* fn_80027AA4 - 0x80027AA4 | size: 0x2b4 */
extern void fn_80166A28(void);
#if 1
asm void fn_80027AA4(void) {
#include "src/game/gs_worldmap_fn_80027AA4.inc"
}
#else
/*
 * fn_80027AA4 -- GSmap_TravelAnimation (0x80027AA4, size 0x2B4)
 *
 * World-map travel / UI state machine tick.
 * Reads two indirect sub-state counters from the context object,
 * decides which action to perform (0-6), then optionally fires a
 * scheduler/sound event via fn_80166A28 before returning a flag.
 *
 * param r3  - pointer to the world-map controller object ("self")
 * return    - 1 if a "complete / next-scene" transition was triggered,
 *             0 otherwise
 */
s32 fn_80027AA4(void* r3) {
    extern s32  fn_800FA314(void*);
    extern void* fn_800FA280(u32);
    extern u16  fn_80027960(u16, s32);
    extern void fn_80166A28(u32);
    extern u8   lbl_80266DD8[];   /* array of 16-byte entries: [u32 id, u32 limit, ...] */
    extern u8   lbl_80266E18[];   /* 2D array: [2][0x18], each 0x18 entry has sub-arrays of void* at +8 */

    u8*   self;          /* r28 -- self pointer (reused as byte-offset after case 1/2) */
    u32   r29;           /* event code sent to fn_80166A28 (0 = no call) */
    s32   r30;           /* return value */
    u16   r31;           /* current travel-target ID */
    s32   r27;           /* animation sub-counter (from self+0x28 indirect) */
    s32   r5;            /* state phase counter (from self+0x2c indirect) */
    void* ptr_28;        /* *(void**)(self+0x28) */
    void* ptr_2c;        /* *(void**)(self+0x2c) */
    s32   action;        /* the computed action code 0-6 */

    self  = (u8*)r3;
    r30   = 0;
    r29   = 0;

    ptr_28 = *(void**)(self + 0x28);
    ptr_2c = *(void**)(self + 0x2c);
    r27 = *(s32*)((u8*)ptr_28 + 0x0);   /* animation sub-counter */
    r5  = *(s32*)((u8*)ptr_2c + 0x0);   /* state phase */

    /* ---------------------------------------------------------------
     * Determine action code
     * --------------------------------------------------------------- */
    if (r27 < 0xf) {
        /* Look up the travel-target ID from a 2-D table indexed by
         * sub-state (0-1) and phase (0-3). */
        s32   sub_state;  /* 0 or 1 -- from self+0x24 indirect */
        s32   phase;      /* 0..3   -- r5 clamped */
        void* entry_ptr;
        void* list_obj;
        s32   list_count;
        void* list_data;
        u16   looked_up;
        u16   current;

        sub_state = *(s32*)(*(u8**)(self + 0x24));
        /* out-of-range sub_state → return 0 */
        if (sub_state < 0 || sub_state >= 2)
            return 0;
        phase = r5;
        /* out-of-range phase → return 0 */
        if (phase < 0 || phase >= 4)
            return 0;

        /* table entry at lbl_80266E18[sub_state * 0x18 + phase * 4]:
         * the pointer-to-list-object lives at +8 within the entry */
        entry_ptr = (void*)(lbl_80266E18 + (s32)(sub_state * 0x18) + (s32)(phase * 4));
        list_obj  = *(void**)((u8*)entry_ptr + 0x8);

        /* validate r27 (the animation counter) as index into the list */
        list_count = fn_800FA314(list_obj);
        if (r27 < 0 || r27 >= list_count)
            return 0;

        list_data = fn_800FA280((u32)list_obj);
        looked_up = *(u16*)((u8*)list_data + (u32)r27 * 2);
        r31 = looked_up;

        /* if the looked-up ID is zero, use a hard-coded default */
        if ((u16)r31 == 0) {
            void* def_ptr = fn_800FA280(0x2ef9u);
            r31 = *(u16*)def_ptr;
        }

        /* compare against the "current" reference value */
        current = *(u16*)fn_800FA280(0x2efcu);
        if ((u16)r31 == current)
            action = 0;
        else
            action = 6;
    } else {
        /* r27 >= 0xf: derive action purely from the phase counter */
        r31 = 0; /* r31 not used by these action codes */
        if (r5 == 0)
            action = 3;
        else if (r5 == 3)
            action = 5;
        else
            action = 4; /* r5 == 1, 2, or > 3 */
    }

    /* ---------------------------------------------------------------
     * Execute action
     * --------------------------------------------------------------- */
    switch (action) {
    case 0: {
        /* action 0: also used as the fall-through entry for case 6.
         * Load the default target ID then fall into the append logic. */
        void* def_ptr = fn_800FA280(0x2ef9u);
        r31 = *(u16*)def_ptr;
        /* FALL THROUGH */
    }
    /* no break -- falls into case 6 */
    case 6:
    default: {
        /* Append r31 into the travel-list at the current count position,
         * clamping if needed, and advance the count pointer. */
        u32   field_1c;
        u32*  entry;
        u32** count_ptr;
        s32   cur_count;
        s32   insert_idx;
        s32   limit;
        u16*  array;

        field_1c  = *(u32*)(self + 0x1c);
        entry     = (u32*)(lbl_80266DD8 + (s32)(field_1c * 16u));
        count_ptr = *(u32***)(self + 0x34);        /* ptr to the mutable count word */
        cur_count = *(s32*)count_ptr;
        limit     = (s32)entry[1];

        if (cur_count >= limit)
            insert_idx = limit - 1;    /* clamp */
        else
            insert_idx = cur_count;

        array = *(u16**)(self + 0x18);
        array[insert_idx]     = r31;
        array[insert_idx + 1] = 0;

        /* advance count, but not past limit */
        {
            s32 new_count = insert_idx + 1;
            if (new_count >= limit + 1)   /* i.e. new_count > limit */
                new_count = limit;
            *(u32*)count_ptr = (u32)new_count;
        }
        r29 = 0x24u;
        break;
    }
    case 1:
    case 2: {
        /* Advance to the previous entry in the travel list (wrap around). */
        u32** count_ptr;
        s32   cur_count;
        s32   idx;
        u16*  array;
        u16   candidate;

        count_ptr = *(u32***)(self + 0x34);
        cur_count = *(s32*)count_ptr;
        idx = cur_count - 1;
        if (idx >= 0) {
            s32 byte_off = (s32)((u32)idx * 2u);
            array     = *(u16**)(self + 0x18);
            candidate = array[byte_off / 2];            /* lhzx r3, r29, r28 */
            candidate = fn_80027960(candidate, 1);      /* look up alternate mapping */
            if ((u16)candidate != 0) {
                array[byte_off / 2] = candidate;
            }
        }
        r29 = 0x24u;
        break;
    }
    case 3: {
        /* Toggle sub-state (0→1→0). */
        u8**  sub_ptr;
        s32   cur;
        sub_ptr = (u8**)(self + 0x24);
        cur = *(s32*)*sub_ptr;
        cur++;
        if (cur >= 2)
            cur = 0;
        *(s32*)*sub_ptr = cur;
        r29 = 0x27u;
        break;
    }
    case 4: {
        /* Remove the last entry from the travel list. */
        u32** count_ptr;
        s32   cur_count;
        u8    did_remove;

        count_ptr = *(u32***)(self + 0x34);
        cur_count = *(s32*)count_ptr;
        if (cur_count > 0) {
            s32   new_count  = cur_count - 1;
            u16*  array      = *(u16**)(self + 0x18);
            array[new_count] = 0;                       /* zero the slot */
            *(u32*)count_ptr = (u32)new_count;
            did_remove = 1;
        } else {
            did_remove = 0;
        }
        /* clrlwi r0, r0, 24 -- mask to u8 */
        if ((u8)did_remove != 0)
            r29 = 0x25u;
        break;
    }
    case 5:
        /* Signal "next scene / done". */
        r30 = 1;
        break;
    }

    /* Fire the scheduler/sound event if a code was set. */
    if (r29 != 0)
        fn_80166A28(r29);

    return r30;
}
#endif

/* fn_80027D58 - 0x80027D58 | size: 0x3a4 */
extern u16* fn_80105624(void);
#if 1
asm void fn_80027D58(void) {
#include "src/game/gs_worldmap_fn_80027D58.inc"
}
#else
/* fn_80027D58 - GSmap_ArrivalSequence (0x80027D58, 0x3A4 bytes)
 * Overworld map party-slot edit dispatcher. Reads the global input-state
 * object (fn_80105624), then dispatches on its two bitfields:
 *   state->f4 (u16) bits select one mutually-exclusive action (each returns):
 *     bit 0x40 - cycle a per-slot toggle 0..1 and play SE 0x27
 *     bit 0x10 - commit/leave; if fn_80027AA4 says "exit" set actor[0x98],
 *                else range-check the route list and set status fields
 *     bit 0x20 - delete the current list entry, play SE 0x25
 *     bit 0x400 - swap the selected slot's species across the two
 *                 reference lists via fn_80027960, play SE 0x24
 *     bit 0x800 - request exit (set actor[0x98])
 *   state->f6 (u16) bits adjust the two cursor counters (NOT exclusive):
 *     bit 0x8/0x4 - inc/dec the column cursor (ctx->f28), clamp 0..15
 *     bit 0x2/0x1 - inc/dec the row cursor (ctx->f2c), clamp 0..3,
 *                   with a +2/-2 step when the column cursor is at 15
 * Returns 0 in all paths. */
s32 fn_80027D58(void* actor) {
    extern u16* fn_80105624(void);
    extern void fn_80166A28(s32 se);
    extern s32  fn_80027AA4(void* ctx);
    extern u16  fn_80027960(u16 value, s32 listIndex);
    extern s32  fn_800FA314(u32 list);
    extern void* fn_800FA280(u32 list);
    extern u8 lbl_80266DD8[];  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047B920;  /* canonical; per-site reinterpret cast */

    u16* state;
    u8*  ctx;
    u16  flags4;
    u16  flags6;

    state = fn_80105624();
    ctx   = *(u8**)((u8*)actor + 0x60);
    flags4 = state[2];              /* *(u16*)(state + 0x4) */

    if (flags4 & 0x40) {
        s32* cell = *(s32**)(ctx + 0x24);
        s32  v = *cell + 1;
        if (v >= 2) v = 0;
        *cell = v;
        fn_80166A28(0x27);
        return 0;
    }

    if (flags4 & 0x10) {
        if (fn_80027AA4(ctx) != 0) {
            *(u8*)((u8*)actor + 0x98) = 1;
            return 0;
        }
        {
            u16* list = *(u16**)(ctx + 0x18);
            s32  count = 0;
            s32* entry;
            while (*list != 0) {
                list++;
                count++;
            }
            entry = (s32*)((u8*)((s32*)lbl_80266DD8) + (*(s32*)(ctx + 0x1c) << 4));
            if (count >= entry[1]) {           /* entry+0x4 */
                *(*(s32**)(ctx + 0x28)) = 0xf;
                *(*(s32**)(ctx + 0x2c)) = 3;
            }
        }
        return 0;
    }

    if (flags4 & 0x20) {
        s32  n = *(*(s32**)(ctx + 0x34));
        u8   played = 0;
        if (n > 0) {
            s32  newN = n - 1;
            u16* base = *(u16**)(ctx + 0x18);
            base[newN] = 0;
            *(*(s32**)(ctx + 0x34)) = newN;
            played = 1;
        }
        if (played) {
            fn_80166A28(0x25);
        }
        return 0;
    }

    if (flags4 & 0x400) {
        s32 idx = *(*(s32**)(ctx + 0x34)) - 1;
        if (idx >= 0) {
            u16* slots = *(u16**)(ctx + 0x18);
            u16  cur = slots[idx];
            s32  found = 0;
            s32  li;
            /* find which of the two reference lists contains the current value */
            for (li = 0; li < 2; li++) {
                u32 list = ((u32*)&lbl_8047B920)[li];
                s32 count;
                u16* data;
                s32 r4;
                s32 ctr;
                if (list == 0) {
                    continue;
                }
                count = fn_800FA314(list);
                data  = (u16*)((u8*)fn_800FA280(list) + 2);  /* value half of key/value pairs */
                r4    = 1;
                if (count > 1) {
                    ctr = count >> 1;
                    do {
                        if (*data == cur) {
                            break;
                        }
                        r4   += 2;
                        data += 2;        /* skip to next pair (2 u16 = 4 bytes) */
                    } while (--ctr);
                }
                if (r4 < count) {
                    found = li;
                    break;
                }
            }
            if (found >= 2) {
                found = 0;
            }
            /* cycle the slot's value through the alternate list(s) until a
             * non-zero conversion is produced */
            {
                s32 dst = found;
                for (;;) {
                    u16 converted;
                    dst++;
                    if (dst >= 2) {
                        dst = 0;
                    }
                    converted = fn_80027960(slots[idx], dst);
                    if ((u16)converted != 0) {
                        slots[idx] = converted;
                        break;
                    }
                }
            }
        }
        fn_80166A28(0x24);
        return 0;
    }

    if (flags4 & 0x800) {
        *(u8*)((u8*)actor + 0x98) = 1;
        return 0;
    }

    /* --- non-exclusive cursor adjustments on state->f6 --- */
    flags6 = state[3];             /* *(u16*)(state + 0x6) */

    if (flags6 & 0x8) {            /* column cursor + */
        s32* colP = *(s32**)(ctx + 0x28);
        s32  col  = *colP + 1;
        if (col >= 0x10) {
            col = 0xf;
        } else {
            fn_80166A28(0x23);
        }
        *colP = col;
    }

    if (flags6 & 0x4) {            /* column cursor - */
        s32* colP = *(s32**)(ctx + 0x28);
        s32  col  = *colP - 1;
        if (col < 0) {
            col = 0;
        } else {
            fn_80166A28(0x23);
        }
        *colP = col;
    }

    if (flags6 & 0x2) {           /* row cursor + (double-step on last column) */
        s32* colP = *(s32**)(ctx + 0x28);
        s32* rowP = *(s32**)(ctx + 0x2c);
        s32  col  = *colP;
        s32  row  = *rowP;
        if (col >= 0xf) {
            if (row == 1) {
                row += 2;
            } else {
                row += 1;
            }
        } else {
            row += 1;
        }
        if (row >= 4) {
            row = 3;
        } else {
            fn_80166A28(0x23);
        }
        *rowP = row;
    }

    if (flags6 & 0x1) {           /* row cursor - (double-step on last column) */
        s32* colP = *(s32**)(ctx + 0x28);
        s32* rowP = *(s32**)(ctx + 0x2c);
        s32  col  = *colP;
        s32  row  = *rowP;
        if (col >= 0xf) {
            if (row == 2) {
                row -= 2;
            } else {
                row -= 1;
            }
        } else {
            row -= 1;
        }
        if (row < 0) {
            row = 0;
        } else {
            fn_80166A28(0x23);
        }
        *rowP = row;
    }

    return 0;
}
#endif

/* fn_800280FC - 0x800280FC | size: 0xf4 */
extern void fn_801080CC(void*, s32);
extern f32 lbl_8047B930;
extern f32 lbl_8047B950;
extern f32 lbl_8047B934;
#if 0
asm void fn_800280FC(void) {
#include "src/game/gs_worldmap_fn_800280FC.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 fn_800280FC(void* r3) {
    u8* r30;
    u8* r31;
    f32* fptr;
    f32 f0;
    f32 f1;
    f32 f2;
    s32 state;
    s32 flag;
    u8 one;

    r30 = (u8*)r3;
    state = (s32)(s8)*(volatile u8*)(r30 + 1);
    r31 = *(u8**)(r30 + 0x60);
    switch (state) {
    case 0:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            fn_801080CC(*(void**)(r30 + 4), 0x56);
            one = 1;
            **(f32**)(r31 + 0x30) = lbl_8047B930;
            r30[2] = one;
        }
        break;
    case 2:
        fptr = *(f32**)(r31 + 0x30);
        f0 = lbl_8047B950;
        f2 = *(volatile f32*)fptr;
        f1 = lbl_8047B934;
        f0 = f2 + f0;
        *fptr = f0;
        if (f0 >= f1) {
            fptr = *(f32**)(r31 + 0x30);
            *fptr = *fptr - f1;
        }
        break;
    case 3:
        flag = (s32)(s8)*(volatile u8*)(r30 + 2);
        if (flag == 0) {
            fn_801080CC(*(void**)(r30 + 4), 0x5a);
            r30[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

/* fn_800281F0 - 0x800281F0 | size: 0x4 */
#if 0
asm void fn_800281F0(void) {
#include "src/game/gs_worldmap_fn_800281F0.inc"
}
#else
#pragma optimization_level 4
void fn_800281F0(void) { }
#endif

/* fn_800281F4 - 0x800281F4 | size: 0x250 */
extern void fn_800F9E70(void*, u8*);
extern void fn_801337A8(void);
extern void fn_801046B8(void);
extern void fn_801026A4(void);
extern void fn_80106D3C(void);
extern void fn_8001E074(void);
extern void fn_801069FC(s32);
extern void fn_80102510(void);
extern void menuCloseSync(void);
extern void menuSubOpenYesNo(void);
extern void pcboxSetPokemonBoxName(void);
extern void GScharCmp(void);
extern void menuModelSetMotion(void);
extern void cameraWaitSyncAnime(void);
extern f32 sin(f32);
extern f32 cos(f32);
extern f32 lbl_8047B930;
extern u8 lbl_8047A3D4[4];
extern u8 lbl_8047A3D0[4];
extern u8 lbl_8047A3CC[4];
extern u8 lbl_8047A3C8[4];
extern u8 lbl_8047A3C4[4];
extern u32 lbl_8047A3C0;
extern u32 lbl_8047A3BC;
extern u32 lbl_8047A3B8;
extern u32 lbl_8047A3B4;
extern u32 lbl_8047A3B0;
extern u8 lbl_803A2068[];
#if 1
asm void fn_800281F4(void) {
#include "src/game/gs_worldmap_fn_800281F4.inc"
}
#else
/*
 * GSmap_Init (fn_800281F4) -- World-map name-entry dialog.
 *
 * Opens the GS name-input scene (0x6e), lets the player type a name,
 * and returns 1 if a new name was accepted or 0 if the dialog was cancelled.
 *
 * Parameters (derived from CW EABI register use before write):
 *   r3  existing_name  -- pointer to the current UTF-16 name (u16[]), may be empty
 *   r4  name_buf_in    -- source name buffer passed to fn_800F9E70 for the stack copy
 *   r5  arg2           -- extra context ptr stored in the GS name-input descriptor
 *   r6  arg3           -- extra context ptr stored in the GS name-input descriptor
 *   r7  allow_cancel   -- if 0, YES/CANCEL both exit without saving; if non-zero,
 *                         player must explicitly answer YES to save
 *
 * Returns 1 if the player accepted a new name, 0 otherwise.
 */
s32 fn_800281F4(u16 *existing_name, u8 *name_buf_in, void *arg2, void *arg3, s32 allow_cancel)
{
    /* --- block-scope extern declarations (TU convention) --- */
    extern u8  *fn_800F9E70(u8 *dst, u8 *src);        /* GS string copy                  */
    extern void fn_801337A8(s32 mode);                 /* set VSync mode (0=off, 1=on)    */
    extern u32  fn_801046B8(void);                     /* get current scene handle         */
    extern void fn_801026A4(s32 sceneId, u32 handle, s32 a, s32 b, s32 c, s32 d, ...); /* open GS scene with descriptor */
    extern void fn_80166A28(u32 arg);                  /* audio/effect trigger             */
    extern void fn_80132A38(s32 effect, void *param);  /* UI effect dispatcher             */
    extern void fn_80106D3C(s32 slot, s32 msgId, s32 p3, s32 p4); /* open dialog message */
    extern s8 menuSubOpenYesNo(s32 max, s32 a, s32 b, s32 initial); /* blocking yes/no picker */
    extern void fn_801069FC(s32 slot);                 /* close dialog slot               */
    extern void fn_80102510(s32 sceneId);              /* close GS scene                  */
    extern void menuCloseSync(s32 sceneId, s32 flag);  /* sync-close menu                 */

    /* lbl_* globals accessed in this function */
    extern f32  lbl_8047B930;       /* float constant 0.0f (stfs source)            */
    extern u8 lbl_8047A3D4[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3D0[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3CC[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3C8[4];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_8047A3C4[4];  /* canonical; per-site reinterpret cast */
    extern u32  lbl_8047A3C0;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3BC;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3B8;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3B4;       /* GS name-input descriptor field               */
    extern u32  lbl_8047A3B0;       /* GS name-input descriptor field               */
    extern u8   lbl_803A2068[];     /* committed name output buffer                 */

    /*
     * Stack-local name-input descriptor block.
     * sp+0x08 : name buffer (GS Unicode string, dest of fn_800F9E70)
     * sp+0x20 : saved existing_name ptr
     * sp+0x24 : saved arg2
     * sp+0x28 : saved arg3
     * sp+0x2c..0x50 : addresses of the nine lbl_8047A3xx globals (pointer table)
     *
     * We model sp+0x08 as a local u8 array large enough for a GS name
     * (0x20 - 0x08 = 0x18 bytes precede the saved-pointer area, but the
     * GS name buffer conventionally sits at this slot and may be longer;
     * 0x58 bytes = 0x60 save-area start - 0x08 buf start).
     */
    u8   name_buf[0x58];      /* sp+0x08: working name buffer                     */
    u16 *name_ptr;            /* sp+0x20 shadow: pointer to existing_name         */
    /* sp+0x24/0x28: arg2/arg3 -- accessed only through the descriptor above      */

    s32  done;                /* r29: loop-exit flag                              */
    s32  confirmed;           /* r30: 1 = player typed YES, 0 = NO/cancel        */
    u16 *scan;                /* r3/r24 reused: scan ptr through existing name    */
    s32  existing_len;        /* r25 reused: UTF-16 code-unit count               */
    u16 *loaded;              /* r3 after fn_800FA280: resource name ptr          */
    s32  match_count;         /* r5: matched code units                           */
    s32  names_differ;        /* r0: 1 = names differ, 0 = same                  */
    u16 *name_to_use;         /* r31: pointer to the name we will commit          */
    s8   yn_result;           /* r24 reused: yes/no answer                        */
    s32  i;                   /* loop counter                                     */

    extern void *fn_800FA280(u32 id);   /* resource lookup by ID                  */

    /* -----------------------------------------------------------------
     * Prologue: copy the caller's name into the local stack buffer,
     * zero the name-input descriptor globals, and initialise the
     * pointer table in the stack frame.
     * ----------------------------------------------------------------- */
    fn_800F9E70(name_buf, name_buf_in);

    /* Zero out the five descriptor globals that are explicitly cleared   */
    (*(u32*)lbl_8047A3D4) = 0;
    (*(u32*)lbl_8047A3D0) = 0;
    (*(u32*)lbl_8047A3CC) = 0;
    (*(f32*)lbl_8047A3C8) = lbl_8047B930;   /* 0.0f */
    (*(u32*)lbl_8047A3C4) = 0;

    /* Null the u16 at existing_name[0] (clear first code unit)           */
    existing_name[0] = 0;

    /* Turn off VSync while the name-input UI is open                     */
    fn_801337A8(0);

    name_ptr = existing_name;
    done      = 0;
    confirmed = 0;   /* FUNCTIONAL-TODO: r30 is written only later; set 0 for safety */

    /* -----------------------------------------------------------------
     * Main loop: keep the name-input scene open until the player makes
     * a final choice (done != 0).
     * ----------------------------------------------------------------- */
    do {
        /* Open the GS name-input scene (scene ID 0x6e).
         * r9 = name_buf (sp+8) is the 7th integer arg; crxor clears
         * cr1.eq so no float args are signalled.                         */
        fn_801026A4(0x6e, fn_801046B8(),
                    0, 0, 1, 1,
                    name_buf);      /* r9 = 7th arg = working name buffer */

        /* ---------------------------------------------------------------
         * Count UTF-16 code units in the original existing_name.
         * existing_name[0] was zeroed above, so if it was already empty
         * this loop exits immediately with existing_len = 0.
         * ------------------------------------------------------------- */
        scan         = name_ptr;  /* reload from sp+0x20 shadow */
        existing_len = 0;
        while (scan[0] != 0) {
            scan++;
            existing_len++;
        }

        /* ---------------------------------------------------------------
         * Determine whether the player entered the same name that was
         * already stored (names_differ = 0 → same, 1 → different).
         * If the original name was empty, treat as "same" (r0=0).
         * ------------------------------------------------------------- */
        if (existing_len == 0) {
            names_differ = 0;
        } else {
            /* Load the resource for ID 0x2ef9 (current box name data).   */
            loaded      = (u16 *)fn_800FA280(0x2ef9);
            match_count = 0;

            /* Compare up to existing_len code units.                     */
            scan = name_ptr;
            for (i = 0; i < existing_len; i++) {
                if (scan[0] != loaded[0]) {
                    break;
                }
                match_count++;
                scan++;
                /* loaded advances implicitly via bdnz; model as pointer: */
                /* loaded++ -- but in asm loaded (r3) is NOT incremented  */
                /* every iteration, only when bdnz fires.                  */
                /* FUNCTIONAL-TODO: The asm uses r24 for scan and r3 for  */
                /* loaded but only advances r24 on match; r3 seems to be  */
                /* the same resource base re-indexed. Conservative: break  */
                /* on mismatch (already done above) or count matches.     */
            }

            /* If all code units matched, names are the same.             */
            names_differ = (match_count < existing_len) ? 1 : 0;
        }

        /* ---------------------------------------------------------------
         * Select which name pointer to pass to the dialog:
         *   names_differ == 0 → use the newly-entered buffer (sp+0x08)
         *   names_differ != 0 → use the original name (existing_name)
         * ------------------------------------------------------------- */
        if (names_differ != 0) {
            name_to_use = name_ptr;             /* existing name differs: show it */
        } else {
            name_to_use = (u16 *)name_buf;      /* same / new: use entered buf    */
        }

        /* ---------------------------------------------------------------
         * Show the "Are you sure?" dialog (message 0x2ef6).
         * ------------------------------------------------------------- */
        fn_80166A28(0x440);
        fn_80132A38(0x4d, name_to_use);
        fn_80106D3C(2, 0x2ef6, 1, 0);
        yn_result = menuSubOpenYesNo(0, -1, -1, 0);
        fn_801069FC(1);

        /* ---------------------------------------------------------------
         * Interpret the yes/no answer.
         *   yn_result ==  1 → YES  → confirmed = 0 (use the name)
         *   yn_result == -1 → BACK → confirmed = 0 (treat same as yes)
         *   otherwise       → NO   → confirmed = 1 (do NOT save)
         * ------------------------------------------------------------- */
        if (yn_result == 1 || yn_result == -1) {
            confirmed = 0;
        } else {
            confirmed = 1;
        }

        /* ---------------------------------------------------------------
         * Decide whether to exit the loop.
         *   allow_cancel == 0 : always exit after first answer
         *   allow_cancel != 0 : exit only if player said YES (confirmed=0)
         * ------------------------------------------------------------- */
        if (allow_cancel == 0) {
            done = 1;
        } else if (confirmed == 0) {
            done = 1;
        }
        /* else: confirmed==1 and allow_cancel!=0 → loop again            */

    } while (done == 0);

    /* -----------------------------------------------------------------
     * Cleanup: restore VSync and close the name-input scene.
     * ----------------------------------------------------------------- */
    fn_801337A8(1);
    fn_80102510(0x6e);
    menuCloseSync(0x6e, 1);

    /* -----------------------------------------------------------------
     * Commit the result.
     * If the player accepted (confirmed==0), copy name_to_use into the
     * output buffer lbl_803A2068 and return 1.
     * Otherwise return 0.
     * ----------------------------------------------------------------- */
    if (confirmed != 0) {
        return 0;
    }
    fn_800F9E70(lbl_803A2068, (u8 *)name_to_use);
    return 1;
}
#endif

/* fn_80028444 - 0x80028444 | size: 0x50 */
#if 0
asm void fn_80028444(void) {
#include "src/game/gs_worldmap_fn_80028444.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
s32 fn_80028444(void* r3, u8* r4) {
    u8* r31;
    void* ctx;
    void* sub;
    void* p;
    r31 = r4;
    ctx = *(void**)((u8*)r3 + 0x60);
    sub = *(void**)ctx;
    p = *(void**)((u8*)sub + 0x8);
    fn_80132A38(0x37, fn_800FA280((u32)p));
    *(u32*)(r31 + 0x4c) = 0xcf;
    return 0;
}
#endif

/* fn_80028494 - 0x80028494 | size: 0x50 */
#if 0
asm void fn_80028494(void) {
#include "src/game/gs_worldmap_fn_80028494.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
s32 fn_80028494(void* r3, u8* r4) {
    u8* r31;
    void* ctx;
    void* sub;
    void* p;
    r31 = r4;
    ctx = *(void**)((u8*)r3 + 0x60);
    sub = *(void**)ctx;
    p = *(void**)((u8*)sub + 0x4);
    fn_80132A38(0x37, fn_800FA280((u32)p));
    *(u32*)(r31 + 0x4c) = 0xcf;
    return 0;
}
#endif

/* fn_800284E4 - 0x800284E4 | size: 0x50 */
#if 0
asm void fn_800284E4(void) {
#include "src/game/gs_worldmap_fn_800284E4.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
s32 fn_800284E4(void* r3, u8* r4) {
    u8* r31;
    void* ctx;
    void* sub;
    void* p;
    r31 = r4;
    ctx = *(void**)((u8*)r3 + 0x60);
    sub = *(void**)ctx;
    p = *(void**)sub;
    fn_80132A38(0x37, fn_800FA280((u32)p));
    *(u32*)(r31 + 0x4c) = 0xcf;
    return 0;
}
#endif

/* fn_80028534 - 0x80028534 | size: 0x54 */
#if 0
asm void fn_80028534(void) {
#include "src/game/gs_worldmap_fn_80028534.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
void fn_80028534(void* r3) {
    u8* r31;
    u16* pad;
    r31 = (u8*)r3;
    pad = fn_80105624();
    if (!(pad[0] & 0x20)) {
        if (pad[2] & 0x10) {
            r31[0x98] = 1;
        }
    }
}
#endif

/* fn_80028588 - 0x80028588 | size: 0x98 */
#if 0
asm void fn_80028588(void) {
#include "src/game/gs_worldmap_fn_80028588.inc"
}
#else
#pragma optimization_level 4
s32 fn_80028588(void* r3) {
    u8* r31;
    s8 state;
    r31 = (u8*)r3;
    state = (s8)r31[1];
    switch (state) {
    case 0:
        if ((s8)r31[2] == 0) {
            fn_801080CC(*(void**)(r31 + 4), 0x56);
            r31[2] = 1;
        }
        break;
    case 3:
        if ((s8)r31[2] == 0) {
            fn_801080CC(*(void**)(r31 + 4), 0x5a);
            r31[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

/* fn_80028620 - 0x80028620 | size: 0x108 */
extern void* fn_80109934(void*);
extern void fn_800D888C(s32);
extern void fn_800D88DC(s32);
extern void fn_800D7820(void*);
extern void fn_800D85D4(s32, void*);
extern void fn_800D6A00(s32);
extern void fn_800D67BC(s32);
extern void fn_800D61E4(s32, s32);
extern void fn_800D5CB8(s32, s32, s32, s32, s32);
extern void fn_800D59B8(s32, f32, f32);
extern void fn_800D6728(void);
extern u8 lbl_803A2094[];
extern u8 lbl_80314F98[];
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;
#if 0
asm void fn_80028620(void) {
#include "src/game/gs_worldmap_fn_80028620.inc"
}
#else
#pragma peephole off
#pragma scheduling on
#pragma optimization_level 4
s32 fn_80028620(void* r3, u8* r4) {
    void* r31;
    u8* r30;
    r30 = r4;
    r3 = *(void**)((u8*)r3 + 0x60);
    if (*(s32*)r3 == 2) {
        return 0;
    }
    r31 = fn_80109934(lbl_803A2094);
    if (r31 != (void*)0) {
        fn_800D888C(4);
        fn_800D88DC(3);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, r31);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B930, lbl_8047B930);
        fn_800D61E4((s32)*(s16*)(r30 + 0x54), (s32)*(s16*)(r30 + 0x56));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B934, lbl_8047B934);
        fn_800D6728();
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_80028728 - 0x80028728 | size: 0x108 */
extern f32 lbl_8047B930;
extern f32 lbl_8047B934;
#if 0
asm void fn_80028728(void) {
#include "src/game/gs_worldmap_fn_80028728.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_80028728(void* r3, u8* r4) {
    void* r31;
    u8* r30;
    r30 = r4;
    r3 = *(void**)((u8*)r3 + 0x60);
    if (*(s32*)r3 != 2) {
        return 0;
    }
    r31 = fn_80109934(lbl_803A2094);
    if (r31 != (void*)0) {
        fn_800D888C(4);
        fn_800D88DC(3);
        fn_800D7820(lbl_80314F98);
        fn_800D85D4(0, r31);
        fn_800D6A00(7);
        fn_800D67BC(2);
        fn_800D61E4(0, 0);
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B930, lbl_8047B930);
        fn_800D61E4((s32)*(s16*)(r30 + 0x54), (s32)*(s16*)(r30 + 0x56));
        fn_800D5CB8(0, 0xff, 0xff, 0xff, 0xff);
        fn_800D59B8(0, lbl_8047B934, lbl_8047B934);
        fn_800D6728();
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_80028830 - 0x80028830 | size: 0x118 */
extern void* fn_8005D858(s32);
extern void fn_80104160(s32, s32, s32, s32, u32, void*, s32, s32);
extern f64 lbl_8047B948;
extern f32 lbl_8047B940;
typedef struct WorldMapOverlay {
    s32 active;
    f32 x;
    f32 y;
    f32 scale;
    f32 unused10;
    u32 color;
    u8 alpha;
    u8 pad19[3];
    f32 timer;
    f32 lifetime;
} WorldMapOverlay;
extern WorldMapOverlay lbl_803A20DC[];
#if 0
asm void fn_80028830(void) {
#include "src/game/gs_worldmap_fn_80028830.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
#pragma fp_contract on
s32 fn_80028830(void* r3) {
    void* r27;
    register s32 r29;
    register s32 r28;
    register WorldMapOverlay* r31;
    register s32 r30;
    f32 sx;
    f32 sy;
    s32 x0;
    s32 y0;
    s32 x1;
    s32 y1;

    r27 = r3;
    r29 = *(s16*)((u8*)fn_8005D858(0x98) + 0xc);
    r28 = *(s16*)((u8*)fn_8005D858(0x98) + 0xe);
    r31 = lbl_803A20DC;
    r30 = 0;
    while (r30 < 0x1e) {
        if (r31->active != 0) {
            sx = (f32)r29 * r31->scale;
            sy = (f32)r28 * r31->scale;
            x1 = (s32)(lbl_8047B940 + sx);
            x0 = (s32)(lbl_8047B940 + (r31->x - sx * lbl_8047B940));
            y1 = (s32)(lbl_8047B940 + sy);
            y0 = (s32)(lbl_8047B940 + (r31->y - sy * lbl_8047B940));
            fn_80104160(x0, y0, x1, y1, r31->color | r31->alpha, r27, 0x98, 0);
        }
        r31++;
        r30++;
    }
    return 0;
}
#pragma pop
#endif

/* fn_80028948 - 0x80028948 | size: 0x674 */
extern f32 fn_800E0BE4(void);
extern f32 lbl_8047B958;
extern f32 lbl_8047B95C;
extern f32 lbl_8047B960;
extern f32 lbl_8047B964;
extern f32 lbl_8047B968;
extern f32 lbl_8047B930;
extern f32 lbl_8047B970;
extern f32 lbl_8047B96C;
extern f32 lbl_8047B934;
extern f32 lbl_8047B954;
#if 1
asm void fn_80028948(void) {
#include "src/game/gs_worldmap_fn_80028948.inc"
}
#else
/*
 * fn_80028948 - GSmap_MainRenderFrame (0x80028948, size 0x674)
 *
 * Particle/overlay state machine that drives the 30-entry overlay table
 * (lbl_803A20DC, WorldMapOverlay[30]) consumed by the renderer fn_80028830.
 *
 * Dispatches on the 1-byte mode field at offset 0x01 of the controller
 * object (signed):
 *   mode 0 : prime - clear all 30 overlays, then run 600 spawn/update ticks,
 *            finally latch the "primed" flag at offset 0x02.
 *   mode 2 : per-frame - probabilistically spawn one overlay, then update all.
 *   mode 3 : latch the flag at offset 0x02 (one-shot) and return.
 *   else   : no-op.
 *
 * Each tick: fn_800E0BE4() returns a random f32; if it is <= the spawn
 * threshold (lbl_8047B958) a new overlay is allocated in the first inactive
 * slot (if any of the 30 are free). The update pass ages every active
 * overlay, retires it when its timer reaches its lifetime, and recomputes
 * its per-frame scale and alpha.
 *
 * Big-endian note: the original advanced its timer / treated lbl_8047B934 as
 * both the per-tick increment and the constant 1.0 used in the alpha fade;
 * the same symbol is reused here so the value stays identical to the ROM.
 * The fctiwz->stb on the alpha is a truncate-to-int then low-byte store.
 */
s32 fn_80028948(void* r3)
{
    extern f32 fn_800E0BE4(void);            /* random f32 source (-> f1) */
    extern f32 lbl_8047B958;                 /* spawn threshold           */
    extern f32 lbl_8047B95C;                 /* x scale                   */
    extern f32 lbl_8047B960;                 /* y scale                   */
    extern f32 lbl_8047B964;                 /* scale base / init scale   */
    extern f32 lbl_8047B968;                 /* scale rand coeff          */
    extern f32 lbl_8047B96C;                 /* lifetime base             */
    extern f32 lbl_8047B970;                 /* lifetime rand coeff       */
    extern f32 lbl_8047B930;                 /* initial timer             */
    extern f32 lbl_8047B934;                 /* timer increment / 1.0     */
    extern f32 lbl_8047B954;                 /* alpha scale               */

    u8* ctl;
    s32 mode;
    s32 i;
    s32 slot;
    WorldMapOverlay* ov;

    ctl = (u8*)r3;
    mode = (s32)(s8)ctl[1];

    if (mode == 0) {
        if ((s32)(s8)ctl[2] != 0) {
            return 0;
        }

        /* clear all 30 overlays */
        for (i = 0; i < 30; i++) {
            lbl_803A20DC[i].active = 0;
        }

        /* 600 priming ticks */
        for (i = 0; i < 0x258; i++) {
            if (fn_800E0BE4() <= lbl_8047B958) {
                /* find first inactive slot */
                for (slot = 0; slot < 30; slot++) {
                    if (lbl_803A20DC[slot].active == 0) {
                        break;
                    }
                }
                if (slot < 30) {
                    ov = &lbl_803A20DC[slot];
                    ov->active = 1;
                    ov->x = lbl_8047B95C * fn_800E0BE4();
                    ov->y = lbl_8047B960 * fn_800E0BE4();
                    ov->scale = lbl_8047B964;
                    ov->unused10 = lbl_8047B968 * fn_800E0BE4() + lbl_8047B964;
                    ov->color = 0xFFFFFF00u;
                    ov->alpha = 0x80;
                    ov->timer = lbl_8047B930;
                    ov->lifetime = lbl_8047B970 * fn_800E0BE4() + lbl_8047B96C;
                }
            }

            /* update pass over all 30 overlays */
            for (slot = 0; slot < 30; slot++) {
                ov = &lbl_803A20DC[slot];
                if (ov->active != 0) {
                    f32 ratio;
                    ov->timer = ov->timer + lbl_8047B934;
                    if (ov->timer >= ov->lifetime) {
                        ov->active = 0;
                    }
                    ratio = ov->timer / ov->lifetime;
                    ov->scale = ov->unused10 * ratio;
                    ov->alpha = (u8)(s32)(lbl_8047B954 * (lbl_8047B934 - ratio));
                }
            }
        }

        ctl[2] = 1;
        return 0;
    }

    if (mode == 2) {
        /* probabilistic single spawn */
        if (fn_800E0BE4() <= lbl_8047B958) {
            for (slot = 0; slot < 30; slot++) {
                if (lbl_803A20DC[slot].active == 0) {
                    break;
                }
            }
            if (slot < 30) {
                ov = &lbl_803A20DC[slot];
                ov->active = 1;
                ov->x = lbl_8047B95C * fn_800E0BE4();
                ov->y = lbl_8047B960 * fn_800E0BE4();
                ov->scale = lbl_8047B964;
                ov->unused10 = lbl_8047B968 * fn_800E0BE4() + lbl_8047B964;
                ov->color = 0xFFFFFF00u;
                ov->alpha = 0x80;
                ov->timer = lbl_8047B930;
                ov->lifetime = lbl_8047B970 * fn_800E0BE4() + lbl_8047B96C;
            }
        }

        /* update pass over all 30 overlays */
        for (slot = 0; slot < 30; slot++) {
            ov = &lbl_803A20DC[slot];
            if (ov->active != 0) {
                f32 ratio;
                ov->timer = ov->timer + lbl_8047B934;
                if (ov->timer >= ov->lifetime) {
                    ov->active = 0;
                }
                ratio = ov->timer / ov->lifetime;
                ov->scale = ov->unused10 * ratio;
                ov->alpha = (u8)(s32)(lbl_8047B954 * (lbl_8047B934 - ratio));
            }
        }

        return 0;
    }

    if (mode == 3) {
        if ((s32)(s8)ctl[2] == 0) {
            ctl[2] = 1;
        }
        return 0;
    }

    return 0;
}
#endif

/* fn_80028FBC - 0x80028FBC | size: 0x59c */
extern void fn_8011F4F0(void);
extern void fn_80134A98(void);
extern void fn_8005D934(void);
extern void fn_8010A5BC(void);
extern void fn_8010A010(void);
extern void fn_8018F6F4(void);
extern void fn_8018F4C8(void);
extern void menuModelSetMotion(void);
extern void fn_80109C88(void);
extern void fn_80109B90(void);
extern void fn_801C41C8(void);
extern void fn_801C40F0(void);
extern void fn_8010A420(void);
extern void fn_8012A450(void);
extern void fn_8011DEE4(void);
extern void fn_801349DC(void);
extern void fn_800F9EE4(void);
extern void fn_800FF660(void);
extern void fn_8011288C(s32, u32);
extern u32 lbl_804788A0;
extern u8 lbl_80266DC0[];
extern f32 lbl_8047B940;
#if 1
asm void fn_80028FBC(void) {
#include "src/game/gs_worldmap_fn_80028FBC.inc"
}
#else
/* fn_80028FBC - GSmap_MainUpdate (0x80028FBC, size 0x59C)
 *
 * World-map main update loop. Operates entirely on module globals:
 *   - lbl_803A2068 : the GSmap context block. Layout used here:
 *         +0x00 u16   (cleared by callers)
 *         +0x18 s32   mode      (selector category, range 0..3)
 *         +0x1c s32   subIndex
 *         +0x20 s32   result    (written here, read by callers)
 *         +0x24 s32   flag24
 *         +0x28 s32   asyncMode (1 => render/finalize path active)
 *   - lbl_80266DC0 : map data blob. Header field +0x00 (ptr) and +0x0C (ptr),
 *         followed at +0x18 by an array of 0x10-byte entries; each entry has
 *         a payload pointer at +0x08 and an element count at +0x0C.
 *   - lbl_802EF0A8 : large read-only data blob (cross-TU); five s16 fields are
 *         latched into the lbl_8047A3xx scratch globals on the first frame.
 *   - lbl_804788A0 : "first frame / needs-latch" flag.
 *   - lbl_803A2094 : the world-map menu-model handle.
 *
 * Byte-match is irrelevant; this reproduces the x86 semantics of the loop.
 */
void fn_80028FBC(void) {
    /* --- module globals (block-scope typed externs, TU convention) --- */
    extern u8  lbl_803A2068[];     /* GSmap context block            */
    extern u8  lbl_80266DC0[];     /* map data blob                  */
    extern u8  lbl_802EF0A8[];     /* far read-only data blob        */
    extern u8  lbl_803A2094[];     /* menu-model handle              */
    extern u32 lbl_804788A0;       /* first-frame latch flag         */
    extern u32 lbl_8047A3C0;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3BC;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B4;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3B0;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047B940;       /* 0.0f constant                  */

    /* --- callees (minimal real signatures inferred from each bl site) --- */
    extern u32  fn_800FA280(u32 id);                       /* id -> resource ptr        */
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx);
    extern u32  fn_80123FBC(void);                         /* returns u8 status         */
    extern u32  fn_8011F4F0(u32 a);
    extern u32  fn_80134A98(s32 a, s32 b);
    extern void fn_800F9E70(void* dst, u8* src);           /* copy/build name struct    */
    extern void* fn_8005D934(s32 id);                      /* returns struct ptr        */
    extern void fn_8010A5BC(void* handle, s16 a, s16 b);
    extern void fn_8010A010(void* handle, s32 v);
    extern void fn_8018F6F4(s32 v);
    extern void fn_8018F4C8(s32 a, s32* outA, s32* outB);
    extern void menuModelSetMotion(void* handle, s32 motion);
    extern void fn_80109C88(void* handle, u32 v);
    extern void fn_80109B90(void* handle, s32 v);
    extern void fn_8010A420(void* handle);
    extern void fn_801C41C8(s32 mode, f32 v);
    extern void fn_801C40F0(s32 v);
    extern u32  fn_801046B8(void);                         /* returns context handle    */
    extern s32  fn_801026A4(s32 id, u32 ctx, s32 a, s32 b, s32 c, s32 d, void* arg);
    extern u32  fn_80166A28(s32 size);
    extern void fn_80132A38(s32 id, u32 name);
    extern void fn_80106D3C(s32 a, s32 b, s32 c, s32 d);
    extern s32  menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern void fn_801069FC(s32 a);
    extern void fn_80102510(s32 id);
    extern void menuCloseSync(s32 id, s32 a);
    extern s32  fn_800281F4(u8* ctx, void* nameBuf, s32 mode, s32 subIndex, s32 last);
    extern void fn_8012A450(s32 a, s32 b, u8* ctx);
    extern void fn_8011DEE4(u32 v, u8* ctx);
    extern void pcboxSetPokemonBoxName(s32 a, s32 name, u8* ctx);
    extern s32  GScharCmp(u8* ctx, void* nameBuf);
    extern void fn_800FF660(void);
    extern void fn_8011288C(s32 a, u32 b);

    u8* ctx;        /* lbl_803A2068 context block          */
    u8* data;       /* lbl_80266DC0 map data blob          */
    s32 mode;       /* ctx +0x18                           */
    s32 subIndex;   /* ctx +0x1c                           */
    s32 r0;         /* generic selection result            */
    u32 sel;        /* selection / pokemon handle          */
    u8  ok;
    s32 nameBuf[8]; /* sp+0x30 local name buffer (was sp[]) */
    s32 entryBuf[4];/* sp+0x20: copy of map header fields   */
    void* mdl;      /* struct ptr from fn_8005D934          */
    s32 motOut;     /* fn_8018F4C8 out word @ sp+0xc        */
    s32 motTmp;     /* fn_8018F4C8 out word @ sp+0x8        */

    data = lbl_80266DC0;

    /* First-frame latch: copy fixed display params out of the big data blob. */
    if (lbl_804788A0 != 0) {
        lbl_804788A0 = 0;
        (*(s32*)&lbl_8047A3C0) = (s32)*(s16*)(lbl_802EF0A8 + 0x6fca);
        (*(s32*)&lbl_8047A3BC) = (s32)*(s16*)(lbl_802EF0A8 + 0x6fcc);
        (*(s32*)&lbl_8047A3B8) = (s32)*(s16*)(lbl_802EF0A8 + 0x20ec2);
        (*(s32*)&lbl_8047A3B4) = (s32)*(s16*)(lbl_802EF0A8 + 0x73ba);
        (*(s32*)&lbl_8047A3B0) = (s32)*(s16*)(lbl_802EF0A8 + 0x70fe);
    }

    ctx = lbl_803A2068;
    mode = *(s32*)(ctx + 0x18);
    subIndex = *(s32*)(ctx + 0x1c);

    /* --- Resolve the current selection (r3) from the mode switch. --- */
    sel = 0;
    switch (mode) {
    case 0:
        sel = fn_800FA280(*(u32*)(data + 0x0));
        break;
    case 1:
        sel = fn_800FA280(*(u32*)(data + 0xc));
        break;
    case 2:
        r0 = (s32)fn_8012A5B0(0, 3, (u16)subIndex);
        if ((fn_80123FBC() & 0xff) == 0) {
            sel = 0;
        } else {
            sel = fn_8011F4F0((u32)r0);
        }
        break;
    case 3:
        sel = fn_80134A98(0, (s32)(s8)subIndex);
        break;
    default:
        sel = 0;
        break;
    }

    /* Build the name buffer from the selection; if none, zero the head. */
    if (sel != 0) {
        fn_800F9E70(nameBuf, (u8*)sel);
        ok = 1;
    } else {
        ok = 0;
    }
    if (ok == 0) {
        *(u16*)nameBuf = 0;
    }

    /* Snapshot four words of the data header into a scratch frame array
       (these are indexed by 'mode' below). */
    mode = *(s32*)(ctx + 0x18);
    entryBuf[0] = *(s32*)(data + 0x88);
    entryBuf[1] = *(s32*)(data + 0x8c);
    entryBuf[2] = *(s32*)(data + 0x90);
    entryBuf[3] = *(s32*)(data + 0x94);
    subIndex = *(s32*)(ctx + 0x1c);

    /* Fetch the model descriptor (0xd3a for mode 2, else 0xd39) and pose it. */
    mdl = fn_8005D934((mode == 2) ? 0xd3a : 0xd39);
    fn_8010A5BC(lbl_803A2094, *(s16*)((u8*)mdl + 0x6), *(s16*)((u8*)mdl + 0x8));

    /* --- Per-mode model setup. --- */
    switch (mode) {
    case 0:
    case 1:
    case 3: {
        s32 v = entryBuf[mode];
        if (mode == 3) {
            fn_8010A010(lbl_803A2094, v);
        } else {
            fn_8010A010(lbl_803A2094, v);
            fn_8018F6F4(v);
            fn_8018F4C8(1, &motOut, &motTmp);
            menuModelSetMotion(lbl_803A2094, motOut);
        }
        break;
    }
    case 2:
        sel = fn_8012A5B0(0, 3, (u16)subIndex);
        if ((fn_80123FBC() & 0xff) != 0) {
            fn_80109C88(lbl_803A2094, sel);
        }
        break;
    default:
        break;
    }
    fn_80109B90(lbl_803A2094, 1);

    /* --- Open the primary menu window (id 0x6f) seeded with the context. --- */
    nameBuf[6] = *(s32*)(ctx + 0x18);   /* sp+0x18 */
    nameBuf[7] = *(s32*)(ctx + 0x1c);   /* sp+0x1c */
    fn_801026A4(0x6f, fn_801046B8(), 0, 0, 1, 1, &nameBuf[6]);

    /* On async/render frames, prime the transition. */
    ctx = lbl_803A2068;
    if (*(s32*)(ctx + 0x28) == 0) {
        fn_801C41C8(2, lbl_8047B940);
        fn_801C40F0(1);
    }

    /* --- List/confirm loop over the current mode's entry array. --- */
    {
        s32 idx = *(s32*)(ctx + 0x18);
        u8* entry = (data + 0x18) + idx * 0x10;
        s32 count = *(s32*)(entry + 0xc);
        s32 lastFlag;

        if (count > 0) {
            s32 done = 0;
            while (done == 0) {
                idx = *(s32*)(ctx + 0x18);
                entry = (data + 0x18) + idx * 0x10;
                count = *(s32*)(entry + 0xc);
                {
                    s32* listPtr = *(s32**)(entry + 0x8);
                    s32 accepted = 0;
                    for (;;) {
                        s32 pick;
                        nameBuf[4] = (s32)listPtr; /* sp+0x10 */
                        nameBuf[5] = count;        /* sp+0x14 */
                        pick = fn_801026A4(0x70, fn_801046B8(), 0, 0, 1, 1, &nameBuf[4]);
                        if (pick == 0) {
                            fn_80166A28(0x24);
                            accepted = 0;
                            break;
                        }
                        if (pick == -1) {
                            continue;
                        }
                        {
                            u32 choiceName = (u32)fn_800FA280((u32)listPtr[pick - 1]);
                            s32 ans;
                            fn_80166A28(0x440);
                            fn_80132A38(0x4d, choiceName);
                            fn_80106D3C(2, 0x2ef6, 1, 0);
                            ans = (s32)(s8)menuSubOpenYesNo(0, -1, -1, 0);
                            fn_801069FC(1);
                            if (ans == 1 || ans == -1) {
                                /* yes/cancel sentinel -> not accepted, retry */
                                continue;
                            }
                            /* accepted */
                            fn_80102510(0x70);
                            menuCloseSync(0x70, 1);
                            accepted = 1;
                            fn_800F9E70(lbl_803A2068, (u8*)choiceName);
                            done = 1;
                            goto after_inner; /* accepted path completes the list loop */
                        }
                    }
                    /* not accepted: close the sub-window and continue/abort. */
                    fn_80102510(0x70);
                    menuCloseSync(0x70, 1);
                after_inner:;
                    if (done != 0) {
                        break;
                    }
                    /* Re-run init for the next page; if it reports terminal, stop. */
                    if (fn_800281F4(lbl_803A2068, nameBuf,
                                    *(s32*)(ctx + 0x18), *(s32*)(ctx + 0x1c), 0) != 0) {
                        break;
                    }
                }
            }
        } else {
            /* Empty list: single terminal init pass. */
            fn_800281F4(lbl_803A2068, nameBuf, 0 /*unused*/, *(s32*)(ctx + 0x1c), 1);
        }
        (void)lastFlag;
    }

    /* --- Tear down the menu model and finalize the selection by mode. --- */
    fn_801C41C8(3, lbl_8047B940);
    fn_801C40F0(1);
    fn_8010A420(lbl_803A2094);
    fn_80102510(0x6f);
    menuCloseSync(0x6f, 1);

    mode = *(s32*)(ctx + 0x18);
    subIndex = *(s32*)(ctx + 0x1c);
    switch (mode) {
    case 0:
        break;
    case 1:
        fn_8012A450(0, 0x17, lbl_803A2068);
        break;
    case 2:
        sel = fn_8012A5B0(0, 3, (u16)subIndex);
        if ((fn_80123FBC() & 0xff) != 0) {
            fn_8011DEE4(sel, lbl_803A2068);
        }
        break;
    case 3:
        pcboxSetPokemonBoxName(0, (s32)(s8)subIndex, lbl_803A2068);
        break;
    default:
        break;
    }

    /* result = (name changed) ? 1 : 0 */
    if (GScharCmp(lbl_803A2068, nameBuf) == 0) {
        *(s32*)(lbl_803A2068 + 0x20) = 0;
    } else {
        *(s32*)(lbl_803A2068 + 0x20) = 1;
    }

    /* Async finalize: post the appropriate completion event. */
    if (*(s32*)(ctx + 0x28) != 0) {
        fn_800FF660();
        if (*(s32*)(lbl_803A2068 + 0x24) != 0) {
            fn_8011288C(0, 0x05960008);
        } else {
            fn_8011288C(0, 0);
        }
    }
}
#endif

/* fn_80029558 - 0x80029558 | size: 0xe0 */
extern u8 lbl_803A2068[];
#if 0
asm void fn_80029558(void) {
#include "src/game/gs_worldmap_fn_80029558.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 fn_80029558(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _558_eq2;
    if (r29 >= 2) goto _558_ge3;
    if (r29 >= 0) goto _558_done;
    goto _558_fail;
    _558_ge3:
    if (r29 >= 4) goto _558_fail;
    goto _558_r30;
    _558_eq2:
    fn_8012A5B0(0, 3, (u16)r30);
    if ((u8)fn_80123FBC() == 0) r31 = 0;
    goto _558_done;
    _558_r30:
    if (r30 < 0) goto _558_r30_fail;
    if (r30 < 3) goto _558_done;
    _558_r30_fail:
    r31 = 0;
    goto _558_done;
    _558_fail:
    r31 = 0;
    _558_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x28) = 0;
    fn_80028FBC();
    return *(s32*)(ctx + 0x20);
}
#pragma peephole on
#endif

/* fn_80029638 - 0x80029638 | size: 0x28 */
#if 0
asm void fn_80029638(void) {
#include "src/game/gs_worldmap_fn_80029638.inc"
}
#else
#pragma scheduling off
#pragma optimization_level 4
void fn_80029638(void* r3) {
    fn_800F9E70(r3, lbl_803A2068);
}
#pragma scheduling on
#endif

/* fn_80029660 - 0x80029660 | size: 0x100 */
extern void fn_800FF730(s32);
extern void _threadSwitch(void);
extern void fn_8011288C(s32, u32);
#if 0
asm void fn_80029660(void) {
#include "src/game/gs_worldmap_fn_80029660.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 fn_80029660(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _660_eq2;
    if (r29 >= 2) goto _660_ge3;
    if (r29 >= 0) goto _660_done;
    goto _660_fail;
    _660_ge3:
    if (r29 >= 4) goto _660_fail;
    goto _660_r30;
    _660_eq2:
    fn_8012A5B0(0, 3, (u16)r30);
    if ((u8)fn_80123FBC() == 0) r31 = 0;
    goto _660_done;
    _660_r30:
    if (r30 < 0) goto _660_r30_fail;
    if (r30 < 3) goto _660_done;
    _660_r30_fail:
    r31 = 0;
    goto _660_done;
    _660_fail:
    r31 = 0;
    _660_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x24) = 0;
    *(s32*)(ctx + 0x28) = 1;
    fn_800FF730(0x390);
    fn_8011288C(0, 0x05960008);
    _threadSwitch();
    return *(s32*)(ctx + 0x20);
}
#endif

/* fn_80029760 - 0x80029760 | size: 0xf0 */
#if 0
asm void fn_80029760(void) {
#include "src/game/gs_worldmap_fn_80029760.inc"
}
#else
#pragma optimization_level 4
#pragma peephole off
s32 fn_80029760(s32 r3, s32 r4) {
    s32 r29;
    s32 r30;
    s32 r31;
    u8* ctx;
    r29 = r3;
    r30 = r4;
    r31 = 1;
    if (r29 == 2) goto _760_eq2;
    if (r29 >= 2) goto _760_ge3;
    if (r29 >= 0) goto _760_done;
    goto _760_fail;
    _760_ge3:
    if (r29 >= 4) goto _760_fail;
    goto _760_r30;
    _760_eq2:
    fn_8012A5B0(0, 3, (u16)r30);
    if ((u8)fn_80123FBC() == 0) r31 = 0;
    goto _760_done;
    _760_r30:
    if (r30 < 0) goto _760_r30_fail;
    if (r30 < 3) goto _760_done;
    _760_r30_fail:
    r31 = 0;
    goto _760_done;
    _760_fail:
    r31 = 0;
    _760_done:;
    if (r31 == 0) return 0;
    ctx = lbl_803A2068;
    *(u16*)ctx = 0;
    *(s32*)(ctx + 0x18) = r29;
    *(s32*)(ctx + 0x1c) = r30;
    *(s32*)(ctx + 0x20) = 0;
    *(s32*)(ctx + 0x24) = 1;
    *(s32*)(ctx + 0x28) = 1;
    fn_800FF730(0x390);
    _threadSwitch();
    return *(s32*)(ctx + 0x20);
}
#endif

/* fn_80029850 - 0x80029850 | size: 0x8c */
extern u16 fn_80143C68(void*);
extern u16 fn_80143C50(void*);
#if 0
asm void fn_80029850(void) {
#include "src/game/gs_worldmap_fn_80029850.inc"
}
#else
#pragma optimization_level 4
u32 fn_80029850(u8* r3, u16 r4, u16 r5, u16 r6) {
    register u32 r28;
    register s32 r27;
    register u8* r26;
    u16 r30;
    u16 r29;
    u16 r31;
    u16 val;
    u16 cur;
    r26 = r3;
    r30 = r6;
    r29 = r4;
    r28 = 0;
    r27 = 0;
    r31 = r5;
    while (r27 < r29) {
        val = fn_80143C68(r26);
        if (val == r31) {
            cur = fn_80143C50(r26);
            r28 += (u16)(r30 - cur);
        } else if (val == 0) {
            r28 += r30;
        }
        r27++;
        r26 += 4;
    }
    return r28;
}
#endif

/* fn_80029AC8 - 0x80029AC8 | size: 0x1f8 */
extern void fn_80143B80(void*, u16);
extern void fn_80143B70(void*, u16);
#if 0
asm void fn_80029AC8(void) {
#include "src/game/gs_worldmap_fn_80029AC8.inc"
}
#else
#pragma optimization_level 4
void fn_80029AC8(s32 r3, s32 r4, s32 r5, void* r6) {
    s32 r29;
    u16 r30;
    u16 r26;
    s16 r28;
    s16 r27;
    u8* r31;
    u16 r24;
    u16 r25;
    r29 = r3;
    r30 = r4;
    r26 = r5;
    r31 = (u8*)r6;
    if (!r31) return;
    r28 = *(s16*)((u8*)r31 + 0x768);
    if (r28 > -1) {
        r24 = r26;
        r27 = 0;
        while (r27 < r28 && r24) {
            s16 i = r27;
            if (i >= 0 && i < r28) {
                void* slot = (void*)(r31 + ((s32)i << 2));
                u16 v = fn_80143C68(slot);
                if (v == r30 || v == 0) {
                    u16 cur;
                    u16 delta;
                    u16 give;
                    if (v == 0) {
                        fn_80143B80(slot, r30);
                        cur = 0;
                    } else {
                        cur = fn_80143C50(slot);
                    }
                    delta = (u16)(0x3e7 - cur);
                    give = (delta >= r24) ? r24 : delta;
                    fn_80143B70(slot, (u16)(cur + give));
                    r24 = (u16)(r24 - give);
                }
            }
            r27++;
        }
        r26 = r24;
    }
    if (r28 > -1) {
        r25 = r26;
        r27 = 0;
        while (r27 < r28 && r25) {
            s16 i = r27;
            if (i >= 0 && i < r28) {
                void* slot = (void*)((u8*)r31 + 0x3ac + ((s32)i << 2));
                u16 v = fn_80143C68(slot);
                if (v == r30 || v == 0) {
                    u16 cur;
                    u16 delta;
                    u16 give;
                    if (v == 0) {
                        fn_80143B80(slot, r30);
                        cur = 0;
                    } else {
                        cur = fn_80143C50(slot);
                    }
                    delta = (u16)(0x3e7 - cur);
                    give = (delta >= r25) ? r25 : delta;
                    fn_80143B70(slot, (u16)(cur + give));
                    r25 = (u16)(r25 - give);
                }
            }
            r27++;
        }
    }
    *(s32*)(r31 + 0x758) -= r29;
    *(u8*)(r31 + 0x760) = 1;
}
#endif

/* fn_80029CC0 - 0x80029CC0 | size: 0x234 */
extern void fn_80142A88(void*, s32);
extern s32 fn_800849B4(s32, s32, s32, void*);
typedef struct WorldMapEntry {
    u16 id;
    u16 qty;
} WorldMapEntry;
typedef struct WorldMapBuf {
    u32 a;
    u32 b;
    u32 c;
    u16 d;
    u16 count;
    WorldMapEntry items[48];
} WorldMapBuf;
#if 0
asm void fn_80029CC0(void) {
#include "src/game/gs_worldmap_fn_80029CC0.inc"
}
#else
#pragma push
#pragma optimization_level 4
#pragma peephole off
#pragma scheduling on
s32 fn_80029CC0(u8* r30) {
    WorldMapBuf buf;
    s32 i;
    s16 idx;
    u16 cnt;
    u16 id;
    u16 qty;
    void* slot;
    u16 v;
    u16 cur;
    u16 delta;
    u16 give;
    s32 j;
    s16 jj;

    fn_80142A88(r30, 0xeb);
    fn_80142A88(r30 + 0x3ac, 0xeb);
    *(u32*)(r30 + 0x758) = 0;
    *(u32*)(r30 + 0x75c) = 0;
    if (fn_800849B4(0, 0x40, 0, &buf) < 0) {
        return 0;
    }
    for (i = 0; i < buf.count; i++) {
        id = buf.items[i].id;
        if (id == 0) continue;
        qty = buf.items[i].qty;
        idx = (s16)i;
        cnt = buf.count;
        if (idx < -1) continue;
        if (idx >= (s32)cnt) continue;
        if (idx != -1) {
            if (idx >= 0 && idx < (s32)cnt) {
                slot = (void*)(r30 + ((s32)idx << 2));
                v = (u16)fn_80143C68(slot);
                if (v != id && v != 0) continue;
                if (v == 0) {
                    fn_80143B80(slot, id);
                    cur = 0;
                } else {
                    cur = (u16)fn_80143C50(slot);
                }
                delta = (u16)(0x3e7 - cur);
                give = (delta >= qty) ? qty : delta;
                fn_80143B70(slot, (u16)(cur + give));
            }
        } else {
            for (j = 0; j < (s32)cnt && qty != 0; j++) {
                jj = (s16)j;
                qty = (u16)qty;
                if (jj >= 0 && jj < (s32)cnt) {
                    slot = (void*)(r30 + ((s32)jj << 2));
                    v = (u16)fn_80143C68(slot);
                    if (v != id && v != 0) continue;
                    if (v == 0) {
                        fn_80143B80(slot, id);
                        cur = 0;
                    } else {
                        cur = (u16)fn_80143C50(slot);
                    }
                    delta = (u16)(0x3e7 - cur);
                    give = (delta >= qty) ? qty : delta;
                    fn_80143B70(slot, (u16)(cur + give));
                    qty = (u16)(qty - give);
                }
            }
        }
    }
    *(u32*)(r30 + 0x758) = *(u32*)((u8*)&buf + 0);
    *(u32*)(r30 + 0x75c) = *(u32*)((u8*)&buf + 4);
    *(u8*)(r30 + 0x760) = 0;
    *(u32*)(r30 + 0x764) = *(u32*)((u8*)&buf + 8);
    *(u16*)(r30 + 0x768) = buf.count;
    return 1;
}
#pragma pop
#endif

/* fn_80029EF4 - 0x80029EF4 | size: 0xb8 */
extern void fn_80129384(s32, void*);
extern void fn_8013467C(s32, s32, u16);
extern void fn_80129A78(s32, s32, u16, s32);
#if 0
asm void fn_80029EF4(void) {
#include "src/game/gs_worldmap_fn_80029EF4.inc"
}
#else
#pragma optimization_level 4
void fn_80029EF4(void* r3, s32 r4, s32 r5, u8 r6, void* r7) {
    s32 r29, r30;
    void* r31;
    r29 = r4; r30 = r5; r31 = r7;
    switch ((u8)r6) {
    case 2:
        fn_80129384(0, r3);
        fn_8013467C(0, r29, (u16)r30);
        if (r31 != 0) { ((u8*)r31)[0x760] = 1; }
        break;
    case 3:
        fn_80029AC8((s32)(u32)r3, r4, r5, r31);
        break;
    default:
        fn_80129384(0, r3);
        fn_80129A78(0, r29, (u16)r30, -1);
        break;
    }
}
#endif

/* fn_80029FAC - 0x80029FAC | size: 0x10c | WALL 97%: slwi scheduling */
extern void* __va_arg(void*, s32);
extern u32 lbl_80478E54;
extern u32 lbl_80478E4C;
typedef struct WorldMapVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} WorldMapVaList;
typedef WorldMapVaList WorldMapVaListArray[1];
#if 0
asm void fn_80029FAC(void) {
#include "src/game/gs_worldmap_fn_80029FAC.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
u32 fn_80029FAC(u8* r3, s32 r4, s32 r5, s32 r6, ...) {
    WorldMapVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    u8* table;
    s32 idx;
    s32 offset;

    *(u32*)list = 0x04000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = r4 << 2;
    map = (u8*)lbl_80478E54;
    r31 = r5 << 2;
    table = (u8*)lbl_80478E4C;
    r30 = 1;
    offset = map[idx] * 0x4c;
    *r3 = table[offset];
    r28 = (u8*)*(volatile u32*)&lbl_80478E4C + offset + 4;
    while (r6 >= 0) {
        if (r30 != 0) {
            r29 = r6;
            r30 = 0;
        } else {
            r30 = 1;
            fn_80132A38(r29, (void*)r6);
        }
        r6 = *(s32*)__va_arg(list, 1);
    }
    return *(u32*)(r28 + r31);
}
#endif

/* fn_8002A0B8 - 0x8002A0B8 | size: 0x10c | WALL 97%: slwi scheduling */
extern u32 lbl_80478E54;
extern u32 lbl_80478E3C;
#if 0
asm void fn_8002A0B8(void) {
#include "src/game/gs_worldmap_fn_8002A0B8.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
u32 fn_8002A0B8(u8* r3, s32 r4, s32 r5, s32 r6, ...) {
    WorldMapVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    u8* table;
    s32 idx;
    s32 offset;

    *(u32*)list = 0x04000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = r4 << 2;
    map = (u8*)lbl_80478E54;
    r31 = r5 << 2;
    table = (u8*)lbl_80478E3C;
    r30 = 1;
    offset = map[idx] * 0x3c;
    *r3 = table[offset];
    r28 = (u8*)*(volatile u32*)&lbl_80478E3C + offset + 4;
    while (r6 >= 0) {
        if (r30 != 0) {
            r29 = r6;
            r30 = 0;
        } else {
            r30 = 1;
            fn_80132A38(r29, (void*)r6);
        }
        r6 = *(s32*)__va_arg(list, 1);
    }
    return *(u32*)(r28 + r31);
}
#endif

/* fn_8002A1C4 - 0x8002A1C4 | size: 0x108 | WALL 97%: slwi scheduling */
extern void fn_80106ADC(s32, u32, s32, s32, u8);
extern u32 lbl_80478E54;
extern u32 lbl_80478E4C;
#if 0
asm void fn_8002A1C4(void) {
#include "src/game/gs_worldmap_fn_8002A1C4.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void fn_8002A1C4(u8* r3, s32 r4, s32 r5, ...) {
    WorldMapVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    s32 idx;
    u8 r27;

    *(u32*)list = 0x03000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = (s32)r3 << 2;
    map = (u8*)lbl_80478E54;
    r31 = r4 << 2;
    r3 = (u8*)lbl_80478E4C + map[idx] * 0x4c;
    r27 = r3[0];
    r28 = r3 + 4;
    r30 = 1;
    while (r5 >= 0) {
        if (r30 != 0) {
            r29 = r5;
            r30 = 0;
        } else {
            r30 = 1;
            fn_80132A38(r29, (void*)r5);
        }
        r5 = *(s32*)__va_arg(list, 1);
    }
    fn_80106ADC(2, *(u32*)(r28 + r31), 1, 0, r27);
    fn_801069FC(1);
}
#endif

/* fn_8002A2CC - 0x8002A2CC | size: 0x108 | WALL 97%: slwi scheduling */
extern u32 lbl_80478E54;
extern u32 lbl_80478E3C;
#if 0
asm void fn_8002A2CC(void) {
#include "src/game/gs_worldmap_fn_8002A2CC.inc"
}
#else
#pragma optimization_level 4
#pragma scheduling on
#pragma peephole off
void fn_8002A2CC(u8* r3, s32 r4, s32 r5, ...) {
    WorldMapVaListArray list;
    s32 r31;
    s32 r30;
    s32 r29;
    u8* r28;
    u8* map;
    s32 idx;
    u8 r27;

    *(u32*)list = 0x03000000;
    list[0].overflow_arg_area = (u32*)((u8*)list + 0x30);
    list[0].reg_save_area = (u32*)((u8*)list - 0x60);
    idx = (s32)r3 << 2;
    map = (u8*)lbl_80478E54;
    r31 = r4 << 2;
    r3 = (u8*)lbl_80478E3C + map[idx] * 0x3c;
    r27 = r3[0];
    r28 = r3 + 4;
    r30 = 1;
    while (r5 >= 0) {
        if (r30 != 0) {
            r29 = r5;
            r30 = 0;
        } else {
            r30 = 1;
            fn_80132A38(r29, (void*)r5);
        }
        r5 = *(s32*)__va_arg(list, 1);
    }
    fn_80106ADC(2, *(u32*)(r28 + r31), 1, 0, r27);
    fn_801069FC(1);
}
#endif

/* fn_8002A3D4 - 0x8002A3D4 | size: 0x2c */
#if 0
asm void fn_8002A3D4(void) {
#include "src/game/gs_worldmap_fn_8002A3D4.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002A3D4(void* r3, u8* r4) {
    void* ctx;
    ctx = *(void**)((u8*)r3 + 0x60);
    r4[0x64] = ((u8*)ctx)[0x10];
    r4[0x65] = ((u8*)ctx)[0x11];
    r4[0x66] = ((u8*)ctx)[0x12];
    r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_8002A400 - 0x8002A400 | size: 0x8c */
#if 0
asm void fn_8002A400(void) {
#include "src/game/gs_worldmap_fn_8002A400.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002A400(void* r3, u8* r4) {
    u8* r30;
    void* r31;
    u32 id;
    u32 ret;
    r30 = r4;
    r31 = *(void**)((u8*)r3 + 0x60);
    fn_80132A38(0x50, (void*)(*(s32*)((u8*)r31 + 0x8) * *(s32*)(*(u32*)((u8*)r31 + 0xc))));
    if (*(s32*)((u8*)r31 + 0x14) != 0) {
        id = 0x153;
    } else {
        id = 0x151;
    }
    ret = fn_800FA444(id);
    fn_800FB680((s32)*(s16*)(r30 + 0x54) - (s32)(ret >> 16), 0, -1, id);
    return 0;
}
#endif

/* fn_8002A48C - 0x8002A48C | size: 0x124 */
extern void fn_800FB8C8(s32, s32, s16, s16, s32, s32);
extern u8 lbl_80266E58[];
#if 0
asm void fn_8002A48C(void) {
#include "src/game/gs_worldmap_fn_8002A48C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002A48C(void* r3, u8* r4) {
    u8* r31;
    void* r6;
    s32 r5;
    s32 r8;
    s32 r7;
    s32 r4v;
    s32 r0;
    s32 r3v;
    u8* r4p;
    r31 = r4;
    r6 = *(void**)((u8*)r3 + 0x60);
    r5 = 0;
    r3v = *(s16*)(r31 + 0x6);
    r4p = lbl_80266E58;
    if (*(s32*)r4p == r3v) goto _sentinel_done;
    r5 = 1;
    r4p = r4p + 0xc;
    if (*(s32*)r4p == r3v) goto _sentinel_done;
    r5 = 2;
    _sentinel_done:;
    if (r5 >= 2) { return 0; }
    r5 = 1 - r5;
    r8 = 1;
    r7 = 0;
    if (r5 > 0) {
        if (r5 > 8) {
            r0 = (r5 - 8 + 7) >> 3;
            if (r5 - 8 > 0) {
                do { r8 = r8 * 100000000; r7 += 8; r0--; } while (r0 != 0);
            }
        }
        r0 = r5 - r7;
        if (r7 < r5) {
            do { r8 = r8 * 10; r0--; } while (r0 != 0);
        }
    }
    r4v = *(s32*)(*(u32*)((u8*)r6 + 0xc));
    r4v = r4v / r8;
    r0 = r4v / 10 * 10;
    r4v = r4v - r0;
    fn_80132A38(0x34, (void*)r4v);
    fn_800FB8C8(0, 0, *(s16*)(r31 + 0x54), *(s16*)(r31 + 0x56), -1, 0xc9);
    return 0;
}
#endif

/* fn_8002A5B0 - 0x8002A5B0 | size: 0x68 */
#if 0
asm void fn_8002A5B0(void) {
#include "src/game/gs_worldmap_fn_8002A5B0.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002A5B0(void* r3, u8* r4) {
    s8 idx;
    s16 val;
    idx = (s8)((u8*)r3)[0x95];
    if (idx < 0 || idx >= 2) { return 0; }
    val = *(s16*)(r4 + 0x6);
    if (*(s32*)(lbl_80266E58 + (s32)idx * 0xc + 0x4) == val) {
        r4[0x67] = 0xff;
    } else if (*(s32*)(lbl_80266E58 + (s32)idx * 0xc + 0x8) == val) {
        r4[0x67] = 0xff;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AA68 - 0x8002AA68 | size: 0x98 */
#if 0
asm void fn_8002AA68(void) {
#include "src/game/gs_worldmap_fn_8002AA68.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AA68(void* r3) {
    u8* r31;
    s8 state;
    r31 = (u8*)r3;
    state = (s8)r31[1];
    switch (state) {
    case 0:
        if ((s8)r31[2] == 0) {
            fn_801080CC((void*)0x61, 0x7e);
            r31[2] = 1;
        }
        break;
    case 3:
        if ((s8)r31[2] == 0) {
            fn_801080CC((void*)0x61, 0x82);
            r31[2] = 1;
        }
        break;
    }
    return 0;
}
#endif

/* fn_8002AB00 - 0x8002AB00 | size: 0x40 */
extern u8 lbl_80266E70[];
#if 0
asm void fn_8002AB00(void) {
#include "src/game/gs_worldmap_fn_8002AB00.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AB00(void* r3, u8* r4) {
    void* ctx;
    u8* base;
    u8 v;
    s32 off;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    off = (s32)v * 3;
    base = lbl_80266E70;
    r4[0x64] = base[off];
    base = base + off;
    r4[0x65] = base[1];
    r4[0x66] = base[2];
    r4[0x67] = 0xff;
    return 0;
}
#endif

/* fn_8002AB40 - 0x8002AB40 | size: 0x178 */
extern u8 lbl_80266E80[];
extern u32 lbl_804788F0;
extern u8 lbl_802E61D8[];
#if 0
asm void fn_8002AB40(void) {
#include "src/game/gs_worldmap_fn_8002AB40.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
#pragma scheduling on
s32 fn_8002AB40(void* r3, u8* r4) {
    u8* ctx;
    u32 table[4];
    u32 value;
    s32 idx;
    u32* limit;

    ctx = *(u8**)((u8*)r3 + 0x60);
    table[0] = *(u32*)(lbl_80266E80 + 0x0);
    table[1] = *(u32*)(lbl_80266E80 + 0x4);
    table[2] = *(u32*)(lbl_80266E80 + 0x8);
    table[3] = *(u32*)(lbl_80266E80 + 0xC);

    if ((ctx[0x1D] & 1) != 0) {
        r4[0x67] = 0;
        return 0;
    }

    if (ctx[0x1C] == 0 || ctx[0x1C] == 1) {
        r4[0x67] = 0;
        return 0;
    }

    switch ((s32)(u32)ctx[0x1C]) {
    case 2:
        value = (u32)fn_8012A5B0(0, 0xE, 0);
        break;
    case 3:
        if (ctx + 0x20 != NULL) {
            value = *(u32*)(ctx + 0x77C);
        } else {
            value = 0;
        }
        break;
    default:
        value = (u32)fn_8012A5B0(0, 0xE, 0);
        break;
    }

    idx = lbl_804788F0 - 1;
    limit = (u32*)(lbl_802E61D8 + idx * 4);
    while (idx >= 0) {
        if (*limit <= value) {
            break;
        }
        limit--;
        idx--;
    }
    if (idx < 0) {
        idx = 0;
    }
    if (idx >= 4) {
        idx = 3;
    }

    if ((s32)*(s16*)(r4 + 0x6) == (s32)table[idx]) {
        r4[0x67] = 0xFF;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#pragma pop
#endif

/* fn_8002ACB8 - 0x8002ACB8 | size: 0x18c */
extern u32 lbl_8047A660;
extern u32 lbl_8047A664;
#if 0
asm void fn_8002ACB8(void) {
#include "src/game/gs_worldmap_fn_8002ACB8.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002ACB8(void* r3, u8* r4) {
    u8* ctx;
    u32 value;
    u32 text_id;
    s32 x;

    ctx = *(u8**)((u8*)r3 + 0x60);

    if ((ctx[0x1D] & 1) != 0) {
        r4[0x67] = 0;
        return 0;
    }

    if (ctx[0x1C] == 0 || ctx[0x1C] == 1) {
        fn_80132A38(0x50, fn_8012A5B0(0, 0xC, 0));
        text_id = 0x151;
        x = (s32)*(s16*)(r4 + 0x54) - (s32)(s16)(fn_800FA444(text_id) >> 16);
        fn_800FB680(x, 0, -1, text_id);
        goto done;
    }

    switch ((s32)(u32)ctx[0x1C]) {
    case 2:
        value = (u32)fn_8012A5B0(0, 0xD, 0);
        break;
    case 3:
        if (ctx + 0x20 != NULL) {
            if ((s32)*(volatile u32*)&lbl_8047A660 > 0) {
                *(u32*)(ctx + 0x778) += *(volatile u32*)&lbl_8047A660;
                *(u32*)(ctx + 0x77C) += *(volatile u32*)&lbl_8047A660;
                lbl_8047A660 = 0;
            }
            if ((s32)lbl_8047A664 > 0) {
                *(u32*)(ctx + 0x778) = 0;
                *(u32*)(ctx + 0x77C) = 0;
                lbl_8047A664 = 0;
            }
            value = *(u32*)(ctx + 0x778);
        } else {
            value = 0;
        }
        break;
    default:
        value = (u32)fn_8012A5B0(0, 0xD, 0);
        break;
    }

    fn_80132A38(0x50, (void*)value);
    text_id = 0x153;
    x = (s32)*(s16*)(r4 + 0x54) - (s32)(s16)(fn_800FA444(text_id) >> 16);
    fn_800FB680(x + 6, 0, -1, text_id);
done:
    return 0;
}
#endif

/* fn_8002AE44 - 0x8002AE44 | size: 0x24 */
#if 0
asm void fn_8002AE44(void) {
#include "src/game/gs_worldmap_fn_8002AE44.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AE44(void* r3, u8* r4) {
    void* ctx;
    ctx = *(void**)((u8*)r3 + 0x60);
    if (((u8*)ctx)[0x1d] & 1) {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AE68 - 0x8002AE68 | size: 0x34 */
#if 0
asm void fn_8002AE68(void) {
#include "src/game/gs_worldmap_fn_8002AE68.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AE68(void* r3, u8* r4) {
    void* ctx;
    u8 v;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        r4[0x67] = 0xcc;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AE9C - 0x8002AE9C | size: 0x5c */
extern u8 lbl_80266E70[];
#if 0
asm void fn_8002AE9C(void) {
#include "src/game/gs_worldmap_fn_8002AE9C.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002AE9C(void* r3, u8* r4) {
    void* ctx;
    u8 v;
    u8* base;
    s32 off;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        off = (s32)v * 3;
        base = lbl_80266E70;
        r4[0x64] = base[off];
        base = base + off;
        r4[0x65] = base[1];
        r4[0x66] = base[2];
        r4[0x67] = 0xff;
    } else {
        r4[0x67] = 0;
    }
    return 0;
}
#endif

/* fn_8002AEF8 - 0x8002AEF8 | size: 0x144 | WALL 83.7%: regalloc + scheduling */
extern void fn_801440A0(u32);
extern u32 fn_80144014(void);
extern u32 fn_80129BC8(s32, u32, u16*, s32, s32, s32, s32);
extern u32 itemGetStatus(u32, s32, s32, s32);
#if 0
asm void fn_8002AEF8(void) {
#include "src/game/gs_worldmap_fn_8002AEF8.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002AEF8(void* r3, u8* r4) {
    void* r5;
    u32 r30;
    s32 r29;
    s32 r28;
    u32 r27;
    u32 r31;
    s32 idx;
    u16 stack;
    r5 = *(void**)((u8*)r3 + 0x60);
    if (((u8*)r5)[0x1c] != 0 && ((u8*)r5)[0x1c] != 1) { return 0; }
    idx = (s8)((u8*)r3)[0x95] + (s8)((u8*)r3)[0x94];
    if (idx < 0 || idx >= (s32)*(u32*)((u8*)r5 + 0x8)) {
        r30 = 0;
    } else {
        r30 = (u16)*(u16*)(*(u32*)((u8*)r5 + 0x4) + idx * 2);
    }
    r29 = 0;
    if ((u16)r30 != 0) {
        r31 = r30;
        fn_801440A0(r30);
        r27 = fn_80129BC8(0, fn_80144014(), &stack, 0, 0, 0, 0);
        r28 = 0;
        while (r28 < (s32)stack) {
            if ((u16)itemGetStatus(r27, 0, 0x1b, 0) == (u16)r31) {
                r29 += (s32)itemGetStatus(r27, 0, 0x1c, 0);
            }
            r28++;
            r27 += 4;
        }
    }
    fn_80132A38(0x2d, (void*)(u32)(u16)r30);
    fn_80132A38(0x34, (void*)r29);
    fn_800FB680(0, 0, -1, 0x2b2f);
    return 0;
}
#pragma peephole on
#endif

/* fn_8002B03C - 0x8002B03C | size: 0x4c */
#if 0
asm void fn_8002B03C(void) {
#include "src/game/gs_worldmap_fn_8002B03C.inc"
}
#else
#pragma push
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B03C(void* r3) {
    void* ctx;
    u8 v;
    ctx = *(void**)((u8*)r3 + 0x60);
    v = ((u8*)ctx)[0x1c];
    if (v == 0 || v == 1) {
        fn_800FB680(0, 0, -1, 0x2b2e);
    }
    return 0;
}
#pragma pop
#endif

/* fn_8002B088 - 0x8002B088 | size: 0x34 */
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002B088(void) {
#include "src/game/gs_worldmap_fn_8002B088.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B088(void) {
    fn_800FB680(0, 0, -1, lbl_8047A3E4);
    return 0;
}
#pragma peephole on
#endif

/* fn_8002B0BC - 0x8002B0BC | size: 0x78 */
extern f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern f32 lbl_8047B978;
#if 0
asm void fn_8002B0BC(void) {
#include "src/game/gs_worldmap_fn_8002B0BC.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B0BC(void* r3, u8* r4) {
    u16 hv;
    u8* ctx;
    u8 pad[8];
    hv = *(u16*)((u8*)r3 + 0x94);
    ctx = *(u8**)((u8*)r3 + 0x60);
    *(u16*)pad = hv;
    if ((s8)pad[0] + 0xa < *(s32*)(ctx + 0x8) + 1) {
        if (*(u16*)(*(void**)ctx) == 0) {
            r4[0x67] = (lbl_8047B97C - lbl_8047A3E8) * lbl_8047B978;
            goto end;
        }
    }
    r4[0x67] = 0;
end:
    return 0;
}
#pragma peephole on
#endif

/* fn_8002B134 - 0x8002B134 | size: 0x6c */
#pragma scheduling on
extern f32 lbl_8047B97C;
extern f32 lbl_8047A3E8;
extern f32 lbl_8047B978;
#if 0
asm void fn_8002B134(void) {
#include "src/game/gs_worldmap_fn_8002B134.inc"
}
#else
#pragma peephole off
#pragma optimization_level 4
s32 fn_8002B134(void* r3, u8* r4) {
    u16 hv;
    void* ctx;
    u8 pad[8];
    hv = *(u16*)((u8*)r3 + 0x94);
    ctx = *(void**)((u8*)r3 + 0x60);
    *(u16*)pad = hv;
    if ((s8)pad[0] > 0) {
        if (*(u16*)(*(void**)ctx) == 0) {
            r4[0x67] = (lbl_8047B97C - lbl_8047A3E8) * lbl_8047B978;
            goto end;
        }
    }
    r4[0x67] = 0;
end:
    return 0;
}
#pragma peephole on
#endif

/* fn_8002B1A0 - 0x8002B1A0 | size: 0x26c */
extern void fn_800FE38C(void);
extern u32 fn_80144088(void);
extern u32 fn_80143FFC(void);
extern u32 fn_80143FE4(void);
extern void fn_800FE35C(void);
extern f32 lbl_8047B980;
#if 1
asm void fn_8002B1A0(void) {
#include "src/game/gs_worldmap_fn_8002B1A0.inc"
}
#else
/*
 * GSmap_DrawWeatherOverlay  0x8002B1A0  size: 0x26C
 *
 * Draws the scrollable slot list overlay on the world-map screen.
 * arg0 = UI/map object (this), arg1 = sprite descriptor for layout info.
 * Iterates up to r23 visual columns, skipping slots outside [0, ctx->count),
 * drawing a header icon and either a normal or alternate text string per slot,
 * then draws a trailing "add" or "locked" button if visual space remains.
 */
void fn_8002B1A0(void* arg0, u8* arg1)
{
    extern void fn_800FE38C(s32, s32, s32, s32);
    extern u32 fn_80144088(void);
    extern u32 fn_80143FFC(void);
    extern u32 fn_80143FE4(void);
    extern void fn_800FE35C(void);
    extern f32  lbl_8047B980;

    extern void fn_80132A38(s32, void*);
    extern u32  fn_800FA444(u32);
    extern void fn_800FB680(s32, s32, s32, u32);
    extern void fn_801440A0(u32);

    extern u8   lbl_802EF0A8[];

    s16 key;
    u8* tbl;
    u8* entry;
    s16 ref_x1;
    s16 ref_y1;
    s16 ref_x2;
    s16 ref_y2;
    s16 entry_x;
    s16 entry_y;
    u8* ctx;
    s32 slot_count;
    s8 slot_i;
    s32 loop_lim;
    s32 dir_off;
    s32 scroll_px;
    s32 w_0xdb;
    s32 w_0x14f;
    s32 y_base;
    s32 x_mid;
    f32 scroll_f;
    u32* scroll_flag;
    s32 x_acc;
    s32 loop_i;
    s32 trailing_tx;
    u32 btn_id;

    /* ---- decode layout rect from the fixed entries in lbl_802EF0A8 ---- */
    key        = *(s16*)(arg1 + 0x6);
    tbl        = lbl_802EF0A8;
    entry      = tbl + (s32)key * 0x1c;   /* indexed element */

    /* fixed reference rect corners stored at absolute byte offsets in the table */
    ref_x1     = *(s16*)(tbl + 0x492e);   /* reference left   */
    ref_y1     = *(s16*)(tbl + 0x4930);   /* reference top    */
    ref_x2     = *(s16*)(tbl + 0x4932);   /* reference right  */
    ref_y2     = *(s16*)(tbl + 0x4934);   /* reference bottom */

    entry_x    = *(s16*)(entry + 0x2);    /* per-entry x adjustment */
    entry_y    = *(s16*)(entry + 0x4);    /* per-entry y adjustment */

    /* context block hanging off arg0+0x60 (same layout as all sibling fns) */
    ctx        = *(u8**)(( u8*)arg0 + 0x60);
    slot_count = *(s32*)(ctx + 0x8);      /* total number of slots */

    /* ---- draw the border rect ---- */
    fn_800FE38C((s32)(ref_x1 - entry_x),
                (s32)(ref_y1 - entry_y),
                (s32)ref_x2,
                (s32)ref_y2);

    /* ---- set up per-frame counters ---- */
    slot_i  = (s8)(( u8*)arg0)[0x94];     /* starting slot index (signed) */
    loop_lim = 10;                         /* r23: visual column limit */
    dir_off  = 0;                          /* r24: scroll direction bias (-1/0) */
    scroll_px = 0;                         /* r22: scroll pixel offset */

    /* ---- format-print arg: max possible value (0x270f = 9999) ---- */
    fn_80132A38(0x50, (void*)0x270f);

    /* ---- measure two reference strings to compute the text x-centre ---- */
    w_0xdb  = (s32)(fn_800FA444(0xdb)  >> 16);
    w_0x14f = (s32)(fn_800FA444(0x14f) >> 16);

    /* y-base for item text: from sprite descriptor */
    y_base = (s32)*(s16*)(arg1 + 0x54);
    x_mid  = (y_base - w_0xdb) - w_0x14f;  /* r28, used as text x-anchor */

    /* ---- scroll / animation state ---- */
    scroll_f = *(f32*)(*(u32*)(ctx + 0xc)); /* current scroll float */

    if (scroll_f != lbl_8047B980) {              /* != 0.0f: scrolling active */
        scroll_flag = *(u32**)(ctx + 0x14);
        if (scroll_flag != (u32*)0 && *scroll_flag != 0) {
            if (scroll_f < lbl_8047B980) {       /* < 0.0f: scrolling left */
                slot_i  -= 1;
                dir_off  = -1;
            } else {                             /* > 0.0f: scrolling right */
                loop_lim = 11;
            }
            /* ENDIAN-QA: fctiwz+stfd+lwz integer extraction = (s32)scroll_f */
            scroll_px = (s32)scroll_f;
        }
    }

    /* ---- loop setup ---- */
    x_acc  = dir_off * 0x1f;   /* r30: pixel x accumulator (31 px/slot) */
    loop_i = dir_off;          /* r26: visual column index */

    /* loop: render one visual column per iteration */
    while (loop_i < loop_lim && slot_i < slot_count) {
        if (slot_i < 0) {
            /* slot index out of range on the low side: skip to increment */
            goto next_slot;
        }

        {
            /* r24 = adjusted x position for this column */
            s32  x_pos = x_acc - scroll_px;
            u32  slot_id;

            /* bounds check: if adjusted x is negative OR slot_i >= count */
            if (x_pos < 0 || slot_i >= slot_count) {
                slot_id = 0;
            } else {
                /* load the u16 slot ID from the packed array */
                slot_id = (u32)((u16*)(*(u32*)(ctx + 0x4)))[slot_i];
            }

            /* draw the icon for this slot (if any) */
            fn_801440A0(slot_id);
            {
                u32 icon_h = (u32)fn_80144088();
                if (icon_h != 0) {
                    fn_800FB680(0, x_pos, -1, icon_h);
                }
            }

            /* draw the text label: mode determined by ctx[0x1c] */
            {
                u8 mode = ctx[0x1c];
                if (mode == 0 || mode == 1) {
                    /* normal mode: draw background string at y_base, then value */
                    fn_800FB680(x_mid, x_pos, -1, 0x14f);
                    fn_801440A0(slot_id);
                    {
                        u32 val = fn_80143FFC();
                        fn_80132A38(0x50, (void*)(u32)(u16)val);
                    }
                    {
                        s32 tw = (s32)(fn_800FA444(0xdb) >> 16);
                        s32 tx = (s32)*(s16*)(arg1 + 0x54) - tw;
                        fn_800FB680(tx, x_pos, -1, 0xdb);
                    }
                } else {
                    /* alternate mode: draw different value string */
                    fn_801440A0(slot_id);
                    {
                        u32 val = fn_80143FE4();
                        fn_80132A38(0x50, (void*)(u32)(u16)val);
                    }
                    {
                        s32 tw = (s32)(fn_800FA444(0x153) >> 16);
                        s32 tx = (s32)*(s16*)(arg1 + 0x54) - tw;
                        fn_800FB680(tx, x_pos, -1, 0x153);
                    }
                }
            }
        }

next_slot:
        x_acc  += 0x1f;
        loop_i += 1;
        slot_i += 1;
    }

    /* ---- trailing "add/locked" button if visual columns remain ---- */
    if (loop_i < loop_lim) {
        trailing_tx = (loop_i * 0x1f) - scroll_px;
        btn_id = (ctx[0x1d] & 1) ? 0x2b47u : 0x2b2cu;
        fn_800FB680(0, trailing_tx, -1, btn_id);
    }

    fn_800FE35C();
}
#endif

/* fn_8002B40C - 0x8002B40C | size: 0x188 */
extern u8 lbl_802E4F68[];
extern f64 lbl_8047B998;
extern f32 lbl_8047B984;
extern f32 lbl_8047B988;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B990;
#if 1
asm void fn_8002B40C(void) {
#include "src/game/gs_worldmap_fn_8002B40C.inc"
}
#else
/*
 * GSmap_DrawTimeOverlay - 0x8002B40C | size: 0x188
 *
 * Looks up the current scene entry in the lbl_802E4F68 table by the s16 key
 * at r4[0x6], computes a time-overlay angle from the sprite-ID byte halves
 * plus an optional float-position offset, multiplies by a scale constant,
 * clamps the result to [lbl_8047B990, lbl_8047B98C] by wrapping, then stores
 * it as a float at r4[0x70].
 *
 * r3 = scene/object context pointer
 * r4 = sprite/render descriptor (u8* base)
 */
s32 fn_8002B40C(void* r3, u8* r4) {
    extern u8  lbl_802E4F68[];  /* table of {s32 key, s16 base_val, ...}[5], stride 8 */
    extern f64 lbl_8047B998;    /* int->float magic constant: 4503601774854144.0 */
    extern f32 lbl_8047B984;    /* angle scale multiplier */
    extern f32 lbl_8047B988;    /* wrap step (subtract or add to clamp) */
    extern f32 lbl_8047B98C;    /* upper clamp bound */
    extern f32 lbl_8047B990;    /* lower clamp bound */

    u16 sprite_id;
    u8 *ctx;
    u8 *tab;
    u8 *entry;
    s16 key;
    s32 idx;
    s8  sprite_hi; /* *  ENDIAN-QA *  high byte of sprite_id (big-endian byte[0]) */
    s8  sprite_lo; /* *  ENDIAN-QA *  low byte of sprite_id (big-endian byte[1])  */
    s32 pos_val;
    s32 sum;
    f32 f_pos;
    f32 f2;

    /* Load the two-byte sprite/timer ID from r3[0x94].
       In big-endian memory: byte[0] = high, byte[1] = low.        *  ENDIAN-QA * 
       Both halves are later sign-extended (extsb) before arithmetic.  */
    sprite_id = *(u16*)((u8*)r3 + 0x94);
    sprite_hi  = (s8)((sprite_id >> 8) & 0xff);  /* big-endian high byte -> extsb */
    sprite_lo  = (s8)( sprite_id        & 0xff);  /* big-endian low byte  -> extsb */

    ctx = (u8*)*(void**)((u8*)r3 + 0x60);
    tab = lbl_802E4F68;

    /* Linear search: find which of the 5 table entries matches r4[0x6].
       Each entry is 8 bytes: word[0]=s32 key, halfword[4]=s16 base_val.
       If nothing matches, idx stays 5 (sentinel = not found).           */
    key = *(s16*)(r4 + 0x6);
    idx = 5;
    if      (key == *(s32*)(tab + 0x00)) idx = 0;
    else if (key == *(s32*)(tab + 0x08)) idx = 1;
    else if (key == *(s32*)(tab + 0x10)) idx = 2;
    else if (key == *(s32*)(tab + 0x18)) idx = 3;
    else if (key == *(s32*)(tab + 0x20)) idx = 4;

    if (idx >= 5) {
        return 0;
    }

    entry = tab + (u32)idx * 8;

    /* Compute base position into r4[0x52] from the table entry and sprite_lo. */
    pos_val = (s32)*(s16*)(entry + 4) + (s32)sprite_lo * 0x1f;
    pos_val = (s16)pos_val;  /* extsh */
    *(s16*)(r4 + 0x52) = (s16)pos_val;

    /* If the dereference-chain ctx[0x14]->word[0] is zero, fold in the
       float-position value from ctx[0xc] (converted to int, round-toward-zero). */
    {
        u32 *state_ptr = *(u32**)(ctx + 0x14);
        if (*state_ptr == 0) {
            f_pos = *(f32*)(*(u32*)(ctx + 0xc));
            *(s16*)(r4 + 0x52) = (s16)((s32)*(s16*)(r4 + 0x52) + (s32)(s32)f_pos);
        }
    }

    /* Build the float angle:
       f_pos_int = (s32)(*(f32*)(ctx[0xc] deref))  -- round toward zero
       sum       = f_pos_int + (sprite_lo + sprite_hi) * 31
       f2        = lbl_8047B984 * (f32)sum
       The xoris/fsubs sequence is CW's int->f64->f32 cast; normalised here
       to a plain (f32)(s32) cast.                                          */
    f_pos = *(f32*)(*(u32*)(ctx + 0xc));
    sum   = (s32)f_pos + ((s32)sprite_lo + (s32)sprite_hi) * 0x1f;
    f2    = lbl_8047B984 * (f32)sum;

    /* Clamp down: subtract lbl_8047B988 until f2 <= lbl_8047B98C */
    while (f2 > lbl_8047B98C) {
        f2 -= lbl_8047B988;
    }

    /* Clamp up: add lbl_8047B988 until f2 >= lbl_8047B990 */
    while (f2 < lbl_8047B990) {
        f2 += lbl_8047B988;
    }

    *(f32*)(r4 + 0x70) = f2;
    return 0;
}
#endif

/* fn_8002B594 - 0x8002B594 | size: 0x2ec */
extern void fn_800CDBE0(void);
extern void fn_800CE148(void);
extern f32 lbl_8047B980;
extern f32 lbl_8047B97C;
extern f64 lbl_8047B998;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B9A0;
extern f32 lbl_8047B9A4;
extern f32 lbl_8047B9A8;
#if 1
asm void fn_8002B594(void) {
#include "src/game/gs_worldmap_fn_8002B594.inc"
}
#else
/*
 * GSmap_DrawPartyIcons  0x8002B594 | 0x2EC bytes
 *
 * Draws a party-icon sprite along a piecewise parametric curve.
 * The curve is divided into 5 segments defined by 5 break-point thresholds
 * computed from the two s16 fields (at +0x54/+0x56) in the data packet.
 * For each segment a normalised fractional parameter f31 is computed and
 * segment-specific x/y screen positions are derived (some use cos/sin arcs).
 * Finally fn_80104160 is called to emit the actual sprite draw call.
 *
 * Parameters (CW EABI):
 *   ctx        r3  – sprite/render context pointer passed through to fn_80104160
 *   data       r4  – sprite data packet; s16 at +0x56 = vert coord, s16 at +0x54 = horiz coord
 *   sprite_id  r5  – sprite index; lower 16 bits passed to fn_80104160
 *   color_byte r6  – low 8 bits = alpha/colour value; OR-merged with 0xFFFFFF00 to form r7
 *   pos        f1  – continuous position parameter along the curve
 *
 * ENDIAN-QA: the asm uses the classic CW big-endian 0x4330/xoris double-word trick
 * to convert the two s16 fields to f32.  On x86 this is a plain (f32)(s16) cast.
 */
void fn_8002B594(void *ctx, u8 *data, u32 sprite_id, u32 color_byte, f32 pos)
{
    extern f32 cos(f32);
    extern f32 sin(f32);
    extern void fn_80104160(s32, s32, s32, s32, s32, void *, u32, s32);

    /* sdata2 / r2-relative float constants */
    extern f32 lbl_8047B980;   /* threshold[0]: curve start */
    extern f32 lbl_8047B97C;   /* threshold[4]: curve end   */
    extern f32 lbl_8047B98C;   /* scale / angular coefficient */
    extern f32 lbl_8047B9A0;   /* radial scale y             */
    extern f32 lbl_8047B9A4;   /* radial offset              */
    extern f32 lbl_8047B9A8;   /* angular offset             */

    /* ----------------------------------------------------------------
     * Convert the two s16 fields in the data packet to f32.
     * ENDIAN-QA: CW used the 0x4330/xoris big-endian double trick here.
     * On x86 a plain signed-short cast is semantically identical. */
    f32 f29 = (f32)(s16)(*(s16 *)(data + 0x56)); /* ENDIAN-QA */
    f32 f30 = (f32)(s16)(*(s16 *)(data + 0x54)); /* ENDIAN-QA */

    /* ----------------------------------------------------------------
     * Build the 5 break-point threshold array (indices 0..4).
     * The normalising formula maps [0, f29] onto a set of segment widths
     * using lbl_98C / lbl_9A4 coefficients. */
    f32 t_scale  = lbl_8047B98C * f29;            /* lbl_98C * f29 */
    f32 t_tmp    = t_scale * lbl_8047B9A0;         /* * lbl_9A0 */
    f32 t_sum    = f30 + t_tmp;                    /* f30 + lbl_98C*f29*lbl_9A0 */
    f32 t_ma     = lbl_8047B9A4 * f30 + t_tmp;    /* lbl_9A4*f30 + lbl_98C*f29*lbl_9A0 */
    f32 t_denom  = lbl_8047B9A4 * t_sum;           /* lbl_9A4 * t_sum */
    f32 thresh1  = f30   / t_denom;
    f32 thresh2  = t_sum / t_denom;
    f32 thresh3  = t_ma  / t_denom;

    /* threshold array (stored on stack in the original): */
    /* [0] = lbl_B980, [1]=thresh1, [2]=thresh2, [3]=thresh3, [4]=lbl_B97C */

    /* ----------------------------------------------------------------
     * Find which segment pos falls in (r31 = 0..4). */
    s32 seg = 0;

    if (lbl_8047B980 > pos) {
        /* pos < lbl_B980 → already past the first break from below – enter seg 1 */
        seg = 1;
    } else {
        /* lbl_B980 <= pos */
        if (thresh1 > pos) {
            seg = 0;
            goto seg_found;
        }
        /* thresh1 <= pos → fall into seg 1 code */
        seg = 1;
    }

    /* seg=1 reached: check whether pos is in [thresh1, thresh2) */
    if (thresh1 > pos) {
        seg = 2;
    } else {
        if (thresh2 > pos) {
            seg = 1;
            goto seg_found;
        }
        seg = 2;
    }

    /* seg=2 check */
    if (thresh2 > pos) {
        seg = 3;
    } else {
        if (thresh3 > pos) {
            seg = 2;
            goto seg_found;
        }
        seg = 3;
    }

    /* seg=3 check */
    if (thresh3 > pos) {
        seg = 4;
    } else {
        if (lbl_8047B97C > pos) {
            seg = 3;
            goto seg_found;
        }
        seg = 4;
    }

seg_found:;

    /* ----------------------------------------------------------------
     * Compute the normalised fractional parameter within the segment.
     *   lower = thresholds[seg], upper = thresholds[seg+1]
     *   f31 = (pos - lower) / (upper - lower) */
    static const f32 *thresh_arr_table; /* unused – expand inline */
    f32 lower, upper;
    switch (seg) {
        case 0:  lower = lbl_8047B980; upper = thresh1;       break;
        case 1:  lower = thresh1;      upper = thresh2;       break;
        case 2:  lower = thresh2;      upper = thresh3;       break;
        case 3:  lower = thresh3;      upper = lbl_8047B97C;  break;
        default: lower = lbl_8047B97C; upper = lbl_8047B97C;  break; /* seg=4: edge/undefined */
    }
    f32 f31 = (pos - lower) / (upper - lower);

    /* ----------------------------------------------------------------
     * Segment-specific x/y screen position computation.
     * r30 → final x argument, r4_out → final y argument to fn_80104160. */
    s32 x_out = 0;  /* r30 in asm; passed as fn_80104160 arg0 (r3 at call site) */
    s32 y_out = 0;  /* r4  in asm; passed as fn_80104160 arg1 (r4 at call site) */

    if (seg == 0) {
        /* segment 0: linear vertical ramp */
        x_out = (s32)(f31 * f30);
        y_out = 0;
    }

    if (seg == 1) {
        /* segment 1: cosine arc for x (horizontal), sine arc for y (vertical) */
        f32 angle = lbl_8047B98C * f31 - lbl_8047B9A8;   /* fmsubs: lbl_98C*f31 - lbl_9A8 */

        f32 cos_a = cos(angle);
        f32 radial = f29 - lbl_8047B9A4;                  /* f29 - lbl_9A4 */
        x_out = (s32)(radial * cos_a * lbl_8047B9A0 + f30); /* fmadds: radial*cos*lbl_9A0 + f30 */

        f32 sin_a = sin(angle);
        y_out = (s32)(f29 * lbl_8047B9A0 +
                      (f29 - lbl_8047B9A4) * sin_a * lbl_8047B9A0); /* fmadds: f29*lbl_9A0 + radial*sin*lbl_9A0 */
    }

    if (seg == 2) {
        /* segment 2: linear horizontal + linear vertical */
        x_out = (s32)(f29 - lbl_8047B9A4);
        y_out = (s32)((lbl_8047B97C - f31) * f30);
    }

    if (seg == 3) {
        /* segment 3: cosine arc for x, sine arc for y (mirror of seg 1) */
        f32 angle = lbl_8047B98C * f31 + lbl_8047B9A8;   /* fmadds: lbl_98C*f31 + lbl_9A8 */

        f32 cos_a = cos(angle);
        f32 radial = f29 - lbl_8047B9A4;
        x_out = (s32)(radial * cos_a * lbl_8047B9A0);    /* fmuls: radial*cos*lbl_9A0 (no +f30 in seg3) */

        f32 sin_a = sin(angle);
        y_out = (s32)(f29 * lbl_8047B9A0 +
                      (f29 - lbl_8047B9A4) * sin_a * lbl_8047B9A0);
    }

    /* ----------------------------------------------------------------
     * Emit the sprite draw call.
     *   r3 = x_out (r30), r4 = y_out (r4), r5=2, r6=2,
     *   r7 = (u8)color_byte | 0xFFFFFF00,
     *   r8 = ctx (r27), r9 = (u16)sprite_id (r28), r10 = 0 */
    s32 color_arg = (s32)((color_byte & 0xFF) | 0xFFFFFF00u);
    fn_80104160(x_out, y_out, 2, 2, color_arg, ctx, (u32)(u16)sprite_id, 0);
}
#endif

/* fn_8002B880 - 0x8002B880 | size: 0x468 */
extern void fn_800FE6D0(void);
extern void fn_800FE4D4(void);
extern f64 lbl_8047B998;
extern f32 lbl_8047B98C;
extern f32 lbl_8047B9A0;
extern f32 lbl_8047B9A4;
extern f32 lbl_8047A3F0;
extern f32 lbl_8047B978;
extern f32 lbl_8047B9AC;
extern f32 lbl_8047B97C;
extern f32 lbl_8047B9B0;
extern f32 lbl_8047B9B4;
#if 1
asm void fn_8002B880(void) {
#include "src/game/gs_worldmap_fn_8002B880.inc"
}
#else
/* fn_8002B880  GSmap_DrawInfoPanel - 0x8002B880, size 0x468
 *
 * Worldmap info-panel draw. r3 = worldmap state object (its +0x60 is the
 * panel context), r4 = the draw/sprite entity. Only runs when the context's
 * primary flag word (*(u16*)*(void**)ctx) is zero. It first computes a sprite
 * column/alpha exactly like fn_8002BCE8 (5-entry lbl_802E4F68 lookup table,
 * keyed on entity->0x6), positions the panel via fn_800FE6D0/fn_800FE4D4, then
 * draws four 45-step radial rings of icons by calling fn_8002B594 in a loop,
 * each ring starting from a different phase offset and advancing by a fixed
 * per-step increment that wraps at lbl_8047B97C.
 *
 * ENDIAN-QA: all 0x43300000 / 0x8000-xor double-word int->float idioms in the
 * original asm are normalized here to plain signed casts on the full value.
 */
s32 fn_8002B880(void* r3, u8* r4)
{
    /* Cross-TU callees (real arg lists inferred from register state at each bl). */
    extern void fn_800FE6D0(s32 x, s32 y);
    extern void fn_800FE4D4(void);
    extern void fn_8002B594(void* panel, u8* entity, u32 mode, s32 step, f32 phase);

    /* r2-relative read-only float constants. */
    extern f64 lbl_8047B998; /* int->float bias double (folds into the casts) */
    extern f32 lbl_8047B98C;
    extern f32 lbl_8047B9A0;
    extern f32 lbl_8047B9A4;
    extern f32 lbl_8047B978;
    extern f32 lbl_8047B9AC;
    extern f32 lbl_8047B97C; /* phase wrap limit */
    extern f32 lbl_8047B9B0;
    extern f32 lbl_8047B9B4;
    /* r13-relative small-data float (running phase base). */
    extern f32 lbl_8047A3F0;
    /* Sprite-column lookup table: 5 entries of 8 bytes; key s32 @ +0, value s16 @ +4. */
    extern u8 lbl_802E4F68[];

    u8* state = (u8*)r3;
    u8* entity = r4;
    u8* ctx = *(u8**)(state + 0x60);

    f32 phase;
    f32 incr;
    f32 denom;
    f32 fx;
    f32 fy;
    s32 i;

    /* Bail unless the context's primary flag word is clear. */
    if (*(u16*)(*(void**)ctx) != 0) {
        return 0;
    }

    /* ---- Sprite column / alpha (mirrors fn_8002BCE8) ---- */
    {
        u16 sprite_id = *(u16*)(state + 0x94);
        s16 key = *(s16*)(entity + 0x6);
        u8* tab = lbl_802E4F68;
        s8 low_byte = (s8)(sprite_id & 0xff);
        s32 idx;
        u8* entry;

        idx = 5;
        if (key == *(s32*)(tab + 0x0)) idx = 0;
        else if (key == *(s32*)(tab + 0x8)) idx = 1;
        else if (key == *(s32*)(tab + 0x10)) idx = 2;
        else if (key == *(s32*)(tab + 0x18)) idx = 3;
        else if (key == *(s32*)(tab + 0x20)) idx = 4;

        entry = (idx < 5) ? (tab + (u32)idx * 8) : (u8*)0;

        if (entry != (u8*)0) {
            s32 val = (s32)*(s16*)(entry + 0x4) + (s32)low_byte * 0x1f;

            /* Add the float-derived bias only when the indirect flag is zero. */
            if (*(s32*)(*(u32*)(ctx + 0x14)) == 0) {
                val += (s32)*(f32*)(*(u32*)(ctx + 0xc));
            }

            *(s16*)(entity + 0x52) = (s16)val;
            entity[0x67] = (*(u16*)(*(void**)ctx) == 0) ? 0x72 : 0xff;
        }
    }

    /* ---- Position the panel ---- */
    fn_800FE6D0((s32)(s16)(*(s16*)(state + 0x84) + *(s16*)(entity + 0x50)),
                (s32)(s16)(*(s16*)(state + 0x86) + *(s16*)(entity + 0x52)));
    fn_800FE4D4();

    /* ---- Fixed per-step phase increment ---- */
    fx = (f32)(s16)*(s16*)(entity + 0x54);
    fy = (f32)(s16)*(s16*)(entity + 0x56);
    denom = lbl_8047B9A4 * (fx + (lbl_8047B98C * fy) * lbl_8047B9A0);
    incr = lbl_8047B9A4 / denom;

    /* ---- Ring 1: phase starts at the live r13 base ---- */
    phase = lbl_8047A3F0;
    for (i = 0; i < 0x2d; i++) {
        fn_8002B594(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    /* ---- Ring 2: phase starts at base + B9B0 ---- */
    phase = lbl_8047B9B0 + lbl_8047A3F0;
    if (phase > lbl_8047B97C) {
        phase -= lbl_8047B97C;
    }
    for (i = 0; i < 0x2d; i++) {
        fn_8002B594(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    /* ---- Ring 3: phase starts at base + B9A0 ---- */
    phase = lbl_8047B9A0 + lbl_8047A3F0;
    if (phase > lbl_8047B97C) {
        phase -= lbl_8047B97C;
    }
    for (i = 0; i < 0x2d; i++) {
        fn_8002B594(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    /* ---- Ring 4: phase starts at base + B9B4 ---- */
    phase = lbl_8047B9B4 + lbl_8047A3F0;
    if (phase > lbl_8047B97C) {
        phase -= lbl_8047B97C;
    }
    for (i = 0; i < 0x2d; i++) {
        fn_8002B594(state, entity, 0xd1,
                    (s32)(lbl_8047B978 * ((f32)i / lbl_8047B9AC)), phase);
        phase += incr;
        if (phase >= lbl_8047B97C) {
            phase -= lbl_8047B97C;
        }
    }

    return 0;
}
#endif

/* fn_8002BCE8 - 0x8002BCE8 | size: 0x120 */
extern u8 lbl_802E4F68[];
#if 0
asm void fn_8002BCE8(void) {
#include "src/game/gs_worldmap_fn_8002BCE8.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002BCE8(void* r3, u8* r4) {
    u16 sprite_id;
    s16 key;
    s32 r5;
    u8* entry;
    u8* tab;
    u8* ctx;
    s32 idx;
    s8 low_byte;
    sprite_id = *(u16*)((u8*)r3 + 0x94);
    ctx = (u8*)*(void**)((u8*)r3 + 0x60);
    tab = lbl_802E4F68;
    key = *(s16*)(r4 + 0x6);
    idx = 5;
    if (key == *(s32*)(tab + 0x0)) idx = 0;
    else if (key == *(s32*)(tab + 0x8)) idx = 1;
    else if (key == *(s32*)(tab + 0x10)) idx = 2;
    else if (key == *(s32*)(tab + 0x18)) idx = 3;
    else if (key == *(s32*)(tab + 0x20)) idx = 4;
    if (idx >= 5) return 0;
    entry = tab + (u32)idx * 8;
    low_byte = (s8)(sprite_id & 0xff);
    r5 = (s32)*(s16*)(entry + 4) + (s32)low_byte * 0x1f;
    if (*(u32*)(ctx + 0x14) != 0) {
        r5 += (s32)*(f32*)(*(u32*)(ctx + 0xc));
    }
    {
        u16 v = *(u16*)(*(u32*)ctx);
        u8 alpha = (v == 0) ? 0x72 : 0xff;
        *(s16*)(r4 + 0x52) = (s16)r5;
        r4[0x67] = alpha;
    }
    return 0;
}
#endif

/* fn_8002BE08 - 0x8002BE08 | size: 0x20c | WALL 86.5%: regalloc + scheduling */
extern void fn_80143F84(void);
extern f32 lbl_8047B9B8;
extern f32 lbl_8047B9BC;
extern u32 lbl_8047A3E4;
#if 0
asm void fn_8002BE08(void) {
#include "src/game/gs_worldmap_fn_8002BE08.inc"
}
#else
#pragma push
#pragma peephole off
#pragma scheduling on
u32 fn_8002BE08(u8* arg0) {
    u8* ctx;
    u16* state;
    s32 sum;
    u32 r3val;
    s32 limit;

    ctx = *(u8**)(arg0 + 0x60);
    state = fn_80105624();
    if (lbl_8047B980 != *(f32*)(*(u32*)(ctx + 0xc))) {
        return 0;
    }
    limit = *(s32*)(ctx + 0x8) + 1;
    if ((state[2] | state[4]) & 0x2) {
        ++arg0[0x95];
        if ((s32)((s8)arg0[0x95] + (s8)arg0[0x94]) >= limit) {
            --arg0[0x95];
        } else {
            if ((s8)arg0[0x95] >= 0xa) {
                ++arg0[0x94];
                --arg0[0x95];
                *(s32*)(*(u32*)(ctx + 0x14)) = 1;
            } else {
                *(s32*)(*(u32*)(ctx + 0x14)) = 0;
            }
            *(f32*)(*(u32*)(ctx + 0xc)) = lbl_8047B9B8;
        }
    }
    if ((state[2] | state[4]) & 0x1) {
        if ((s8)arg0[0x95] > 0 || (s8)arg0[0x94] > 0) {
            --arg0[0x95];
            {
                s32 t = (s8)arg0[0x95];
                if (t < 0) {
                    arg0[0x95] = 0;
                    --arg0[0x94];
                    *(s32*)(*(u32*)(ctx + 0x14)) = 1;
                } else {
                    *(s32*)(*(u32*)(ctx + 0x14)) = 0;
                }
            }
            *(f32*)(*(u32*)(ctx + 0xc)) = lbl_8047B9BC;
        }
    }
    sum = (s32)(s8)arg0[0x94] + (s32)(s8)arg0[0x95];
    if (sum < 0 || sum >= *(s32*)(ctx + 0x8)) {
        r3val = 0;
    } else {
        r3val = ((u16*)(*(u32*)(ctx + 0x4)))[sum];
    }
    if ((u16)r3val != 0) {
        fn_801440A0((u16)r3val);
        fn_80143F84();
    } else {
        u8 b = ctx[0x1c];
        if (b == 0 || b == 1) {
            r3val = 0x2b2d;
        } else if (ctx[0x1d] & 1) {
            r3val = 0x2b46;
        } else {
            r3val = 0x2b37;
        }
    }
    lbl_8047A3E4 = r3val;
    return 0;
}
#pragma pop
#endif

/* fn_8002C014 - 0x8002C014 | size: 0xd0 */
extern void fn_80102ED4(void*);
#if 0
asm void fn_8002C014(void) {
#include "src/game/gs_worldmap_fn_8002C014.inc"
}
#else
#pragma optimization_level 4
s32 fn_8002C014(void* r3) {
    u8* r31;
    u8* r30;
    u16* pad;
    s32 r4;
    u16 r3val;
    r31 = (u8*)r3;
    r30 = (u8*)*(void**)((u8*)r3 + 0x60);
    pad = fn_80105624();
    if (pad[2] & 0x10) {
        s32 a = (s8)r31[0x94];
        s32 b = (s8)r31[0x95];
        r4 = a + b;
        if (r4 < 0 || r4 >= (s32)*(u32*)(r30 + 0x8)) {
            r3val = 0;
        } else {
            r3val = ((u16*)(*(u32*)(r30 + 0x4)))[r4];
        }
        if ((r30[0x1d] & 1) && (r3val != 0)) {
            return 0;
        }
    } else {
        r3val = 0;
    }
    if (r3val != 0) {
        *(u16*)(*(u32*)r30) = r3val;
        fn_80102ED4(r31);
    }
    return 0;
}
#endif

/* fn_8002C0E4 - 0x8002C0E4 | size: 0x1a0 */
extern f32 lbl_8047B980;
extern f32 lbl_8047B9C0;
extern f32 lbl_8047B9C4;
extern f32 lbl_8047B97C;
extern f32 lbl_8047B9C8;
#if 1
asm void fn_8002C0E4(void) {
#include "src/game/gs_worldmap_fn_8002C0E4.inc"
}
#else
/*
 * GSmap_FadeFromBlack  (0x8002C0E4, 0x1A0 bytes)
 *
 * Drives the "fade from black" sequence on the world-map screen.
 * self->byte[0x1]  = current phase (0 = init, 2 = animate, 3 = finish)
 * self->byte[0x2]  = one-shot flag (0 = not yet triggered, 1 = done)
 * self->ptr[0x60]  = inner context block; its fields are indirect float/int cells:
 *     ctx+0x0C = ptr to f32 : horizontal pan offset  (driven toward 0 in phase 2)
 *     ctx+0x10 = ptr to f32 : wrap counter A         (incremented by lbl_8047B9C8 mod lbl_8047B97C)
 *     ctx+0x14 = ptr to u32 : integer flag / counter (cleared to 0 in phase 0)
 *     ctx+0x18 = ptr to f32 : wrap counter B         (incremented by lbl_8047B9C4 mod lbl_8047B97C)
 */
s32 fn_8002C0E4(u8 *self)
{
    extern void fn_801080CC(s32 param, u32 key);

    /* lbl_ float constants declared block-scope per TU convention */
    extern f32 lbl_8047B980;   /* 0.0f - zero reference                     */
    extern f32 lbl_8047B9C0;   /* step magnitude for pan offset convergence  */
    extern f32 lbl_8047B9C4;   /* step for wrap counter B                    */
    extern f32 lbl_8047B97C;   /* 1.0f (or wrap period) - upper bound        */
    extern f32 lbl_8047B9C8;   /* step for wrap counter A                    */

    s8  phase;
    u8 *ctx;
    f32 val, step;

    phase = (s8)self[0x1];
    ctx   = *(u8 **)(self + 0x60);

    if (phase == 2) {
        goto phase2;
    } else if (phase > 2) {
        if (phase >= 4) {
            return 0;   /* phase 4+ : no-op */
        }
        /* phase == 3 */
        goto phase3;
    } else if (phase == 0) {
        goto phase0;
    }
    /* phase == 1 (or anything else unmapped) : fall through */
    return 0;

phase0:
    /* One-shot init: arm the fade-from-black animation */
    if ((s8)self[0x2] != 0) {
        return 0;
    }
    fn_801080CC(0x60, 0x76);

    /* Zero all animated fields */
    *(f32 *)(*(u32 *)(ctx + 0x0C)) = lbl_8047B980;   /* pan offset = 0 */
    *(u32 *)(*(u32 *)(ctx + 0x14)) = 0;              /* integer flag = 0 */
    *(f32 *)(*(u32 *)(ctx + 0x18)) = lbl_8047B980;   /* wrap B = 0 */
    *(f32 *)(*(u32 *)(ctx + 0x10)) = lbl_8047B980;   /* wrap A = 0 */

    self[0x2] = 1;
    return 0;

phase2:
    /* Per-frame animation: converge pan offset toward 0, advance wrap counters */

    /* --- Converge pan offset (ctx+0x0C) toward 0.0 from the positive side --- */
    {
        f32 *pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
        val = *pan_ptr;
        if (val > lbl_8047B980) {               /* val > 0 */
            f32 nv = val - lbl_8047B9C0;
            *pan_ptr = nv;
            if (nv < lbl_8047B980) {            /* undershot: clamp to 0 */
                pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
                *pan_ptr = lbl_8047B980;
            }
        }
    }

    /* --- Converge pan offset toward 0.0 from the negative side --- */
    {
        f32 *pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
        val = *pan_ptr;
        if (val < lbl_8047B980) {               /* val < 0 */
            f32 nv = val + lbl_8047B9C0;
            *pan_ptr = nv;
            if (nv > lbl_8047B980) {            /* overshot: clamp to 0 */
                pan_ptr = (f32 *)(*(u32 *)(ctx + 0x0C));
                *pan_ptr = lbl_8047B980;
            }
        }
    }

    /* --- Advance wrap counter B (ctx+0x18), wrap at lbl_8047B97C back to 0 --- */
    {
        f32 *wb_ptr = (f32 *)(*(u32 *)(ctx + 0x18));
        step = lbl_8047B9C4;
        val  = *wb_ptr;
        val  = val + step;
        *wb_ptr = val;

        wb_ptr = (f32 *)(*(u32 *)(ctx + 0x18));
        val = *wb_ptr;
        if (val > lbl_8047B97C) {               /* exceeded period: wrap to 0 */
            *wb_ptr = lbl_8047B980;
        }
    }

    /* --- Advance wrap counter A (ctx+0x10), wrap at lbl_8047B97C back to (val - period) --- */
    {
        f32 *wa_ptr = (f32 *)(*(u32 *)(ctx + 0x10));
        step = lbl_8047B9C8;
        val  = *wa_ptr;
        val  = val + step;
        *wa_ptr = val;

        /* cror eq,gt,eq  =>  cr0.eq = cr0.gt | cr0.eq  =>  true when val >= lbl_8047B97C */
        if (val >= lbl_8047B97C) {
            wa_ptr = (f32 *)(*(u32 *)(ctx + 0x10));
            val    = *wa_ptr;
            *wa_ptr = val - lbl_8047B97C;       /* subtract one period (sawtooth) */
        }
    }

    return 0;

phase3:
    /* One-shot finish: signal end of fade sequence */
    if ((s8)self[0x2] != 0) {
        return 0;
    }
    fn_801080CC(0x60, 0x7a);
    self[0x2] = 1;
    return 0;
}
#endif

/* fn_8002C284 - 0x8002C284 | size: 0x184 */
extern void fn_80102568(void);
extern u32 lbl_804788A8;
extern u16 lbl_8047A3F8;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
extern u16 lbl_8047A3F4;
extern u16 lbl_8047A3EC;
#if 1
asm void fn_8002C284(void) {
#include "src/game/gs_worldmap_fn_8002C284.inc"
}
#else
/*
 * fn_8002C284  GSmap_ShowTravelDialog  0x8002C284 | 0x184 bytes
 *
 * Shows the "Travel to <location>?" confirmation dialog for the world map.
 *
 * loc_idx: world-map location index (indexes into lbl_80478E54 table)
 * mode:    dialog mode; low byte 0x02 or 0x03 = skip the format-text preamble call
 *
 * Functional C for x86 host build — byte-match irrelevant.
 */
void fn_8002C284(u32 loc_idx, u32 mode)
{
    extern void   fn_8002A1C4(u8* r3, s32 r4, s32 r5, ...); /* GSmap_FormatText2 */
    extern void*  fn_801046B8(void);                          /* get current scene context */
    extern void   fn_801026A4(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...); /* scene event dispatch */
    extern void   fn_80102568(u32 slot, u32 p1, u32 p2);     /* scene event release */

    /* lbl_802E4F68: array of 5 entries at stride 8: { u32 key; s16 val; u16 pad; } */
    extern u8     lbl_802E4F68[];
    /* lbl_802EF0A8: array of structs at stride 0x1c; s16 at offset +4 within each */
    extern u8     lbl_802EF0A8[];

    /* SDA globals */
    extern u32    lbl_804788A8;  /* "update pending" flag */
    extern u32    lbl_80478E54;  /* pointer to location-index map (array of u32 entries) */
    extern u32    lbl_80478E44;  /* pointer to destination u16 table */
    extern u16    lbl_8047A3F8;  /* output: zero count / first-free index */
    extern u16    lbl_8047A3F4;  /* output field for dialog params */
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16    lbl_8047A3EC;  /* output field for dialog params */
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */

    u8 mode_low;
    u8 *tab;
    u8 *ef0a8_base;
    u32 *map_entry;
    u16 *u16_base;
    u16 *u16_ptr;
    u16  zero_count;
    void *ctx;

    /* Dialog params struct built on the (conceptual) stack, passed as vararg to fn_801026A4 */
    struct {
        u16  *p_a3f8;     /* +0x00 */
        u16  *u16_base_p; /* +0x04 */
        u32   count;      /* +0x08 */
        u16  *p_a3f4;     /* +0x0C */
        u16  *p_a3f0;     /* +0x10 */
        u16  *p_a3ec;     /* +0x14 */
        u16  *p_a3e8;     /* +0x18 */
        u8    mode_byte;  /* +0x1C */
        u8    flag1;      /* +0x1D */
    } sp_data;

    /* If mode low byte is 2 or 3, skip the text-format preamble */
    mode_low = (u8)(mode & 0xFF);
    if (mode_low != 0x03 && mode_low != 0x02) {
        /* fn_8002A1C4(loc_idx_as_ptr, 0xa, -1) with no float arg (crxor 6,6,6) */
        fn_8002A1C4((u8*)(u32)loc_idx, 0xa, -1);
    }

    /* If the "update pending" flag is set, refresh the lbl_802E4F68 s16 values
     * from the lbl_802EF0A8 table (5 entries, stride 0x1c, s16 at +4) */
    if (lbl_804788A8 != 0) {
        tab      = lbl_802E4F68;        /* base of the 5-entry stride-8 table */
        ef0a8_base = lbl_802EF0A8 + 4; /* s16 values start at offset +4 */

        /* Entry 0 */
        *(s16*)(tab + 0x4) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x0) * 0x1c);
        /* Entry 1 */
        *(s16*)(tab + 0xC) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x8) * 0x1c);
        /* Entry 2 */
        *(s16*)(tab + 0x14) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x10) * 0x1c);
        /* Entry 3 */
        *(s16*)(tab + 0x1C) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x18) * 0x1c);
        /* Entry 4 */
        *(s16*)(tab + 0x24) = *(s16*)(ef0a8_base + *(u32*)(tab + 0x20) * 0x1c);

        lbl_804788A8 = 0;
    }

    /* Find the first zero u16 entry in lbl_80478E44 at the sub-table selected
     * by lbl_80478E54[loc_idx].  The sub-table base offset uses the u16 at
     * offset +2 of the loc_idx map entry (multiplied by 2 for u16 stride). */
    map_entry = (u32 *)((u8 *)lbl_80478E54 + loc_idx * 4);
    /* lbl_8047A3F8 is reset to 0 before the scan */
    lbl_8047A3F8 = 0;
    {
        u16 sub_idx = *(u16 *)((u8 *)map_entry + 2); /* u16 at offset +2 of map entry */
        u16_base = (u16 *)((u8 *)lbl_80478E44 + (u32)sub_idx * 2);
    }
    u16_ptr   = u16_base;
    zero_count = 0;
    while (*u16_ptr != 0) {
        u16_ptr++;
        zero_count++;
    }

    /* Build the parameter block for the scene event dispatcher */
    sp_data.p_a3f8     = &lbl_8047A3F8;
    sp_data.u16_base_p = u16_base;
    sp_data.count      = zero_count;
    sp_data.p_a3f4     = &lbl_8047A3F4;
    sp_data.p_a3f0     = &(*(u16*)&lbl_8047A3F0);
    sp_data.p_a3ec     = &lbl_8047A3EC;
    sp_data.p_a3e8     = &(*(u16*)&lbl_8047A3E8);
    sp_data.mode_byte  = mode_low;
    sp_data.flag1      = 1;

    /* Open scene dialog 0x60 with the parameter block */
    ctx = fn_801046B8();
    fn_801026A4((void*)0x60, (u32)ctx, 0, 0, (void*)1, 1, &sp_data);

    /* Release / wait for scene dialog 0x60 */
    fn_80102568(0x60, 0, 1);
}
#endif

/* fn_8002C408 - 0x8002C408 | size: 0xa64 */
extern void fn_80129280(void);
extern void fn_80134420(void);
extern void fn_801298B8(void);
extern void fn_80166AB8(void);
extern void fn_80093574(void);
extern void fn_80092C90(void);
extern void fn_80093610(void);
extern void fn_80093698(void);
extern void fn_801D0748(void);
extern void* memcpy(void* dst, const void* src, u32 n);
extern void* memset(void* dst, int val, u32 n);
extern u32 lbl_8047A3DC;
extern u32 lbl_8047A3D8;
extern u32 lbl_8047A660;
extern u32 lbl_8047A664;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
extern u32 lbl_804788A8;
extern u32 lbl_8047A3E4;
extern u32 lbl_8047A3E0;
extern u32 lbl_80478E4C;
#if 1
asm void fn_8002C408(void) {
#include "src/game/gs_worldmap_fn_8002C408.inc"
}
#else
/*
 * fn_8002C408  GSmap_DialogStateMachine  (0x8002C408, 0xA64)
 *
 * World-map travel/shop dialog driver. Reconstructed for x86 host (clang -m32);
 * byte-match irrelevant, semantics preserved. CW EABI: r3=mapIdx (param0, index
 * into the location tables at lbl_80478E54/E44), r29=mode (param1, u8 dialog
 * type 2/3/4). Returns void (no consistent r3 set across blr paths).
 *
 * Control flow mirrors the asm CFG via labels (retry-loop back-edge ->
 * L_628; early outs -> done). The 0x8e0 frame holds one real working buffer
 * (WorldMapBuf, shared with fn_80029CC0) at +0x128 plus stack menu descriptors.
 *
 * ENDIAN-QA: all multi-byte reads use natural-width loads; no big-endian
 * half/word splitting is required (the asm uses width-correct lhz/lwz/lbz).
 */
void fn_8002C408(s32 mapIdx, u32 mode)
{
    /* ---- cross-TU callees (block-scope typed externs, TU convention) ---- */
    extern void  fn_80142A88(void* buf, s32 v);            /* clear/init work buffer */
    extern s32   fn_80029CC0(u8* buf);                     /* scene callback 2 (mode 3 init) */
    extern u32   fn_80129280(u8* obj, u16 sel);            /* object/property accessor */
    extern u32   fn_8012A5B0(u8* ptr, u32 selector, u32 idx); /* state/interaction getter */
    extern void  fn_8012A450(u8* ptr, u32 selector, u32 value); /* state setter */
    extern void* fn_801440A0(u16 speciesId);              /* select species/item entry */
    extern u32   fn_80143FE4(void);                        /* read selected entry value (u16) */
    extern u16   fn_80143C68(void* slot);                  /* item/species id at slot */
    extern u16   fn_80143C50(void* slot);                  /* quantity at slot */
    extern u16   fn_80134420(s32 a, u16 species);          /* owned-count query (mode 2) */
    extern s32   fn_801298B8(u8* ptr, u32 species);        /* owned-count query (default) */
    extern void  fn_80029EF4(void* a, s32 b, s32 c, u8 d, void* e); /* commit purchase */
    extern void  fn_8002A1C4(u8* idx, s32 msgId, s32 term, ...);    /* show message line */
    extern u32   fn_80029FAC(u8* idx, s32 a, s32 b, s32 c, ...);    /* format text -> string ptr */
    extern void  fn_80106ADC(s32 a, u32 str, s32 c, s32 d, u8 alpha);/* display formatted string */
    extern u8    menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);      /* yes/no prompt */
    extern u32   fn_801046B8(void);                        /* current menu owner handle */
    extern s32   fn_801026A4(void* p, u32 owner, s32 c, s32 d, void* e, s32 f, ...); /* open list menu */
    extern void  fn_80102510(s32 slot);                    /* refresh menu slot */
    extern s32   menuCloseSync(void* slot, u8 flag);       /* close menu (sync) */
    extern void  fn_801069FC(s32 slot);                    /* close message box */
    extern s32   fn_80102568(void* slot, u32 m, u8 wait);  /* close menu group */
    extern void  fn_80166AB8(s32 soundId, s32 p2, s32 p3); /* play SE */
    extern s32   fn_801D0748(u32 a, u32 b, u32 c);         /* confirm-state query */
    extern void  fn_80093574(s32 a);                       /* inventory list ops */
    extern void  fn_80092C90(s32 a, void* list, s32 c);
    extern s32   fn_80093610(s32 a);
    extern void  fn_80093698(s32 a);
    extern void* memcpy(void* dst, const void* src, u32 n);
    extern void* memset(void* dst, s32 v, u32 n);

    /* ---- small-data / read-only globals ---- */
    extern u32 lbl_8047A3DC;   /* saved object snapshot dst */
    extern u32 lbl_8047A3D8;   /* saved interaction handle */
    extern u32 lbl_8047A3E4;   /* formatted string ptr A */
    extern u32 lbl_8047A3E0;   /* selected quantity result */
    extern u32 lbl_8047A660;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A664;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_804788A8;   /* "rebuild list" flag */
    extern u16 lbl_8047A3F8;   /* menu: selected id out */
    extern u16 lbl_8047A3F4;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16 lbl_8047A3EC;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_80478E54;   /* location header table base */
    extern u32 lbl_80478E44;   /* name/entry list table base */
    extern u32 lbl_80478E4C;   /* per-location descriptor table base */
    extern u8  lbl_80266E70[]; /* per-mode 3-byte RGB color table */
    extern u8  lbl_802E4F68[]; /* source struct for list-rebuild shuffle */
    extern u8  lbl_802EF0A8[]; /* indexed records (0x1c stride) for shuffle */

    /* ---- real working buffer (shared layout with fn_80029CC0) ---- */
    struct WorldMapBuf {
        u8   head[0x758];      /* item slots region init'd by fn_80142A88 */
        u32  credit0;          /* +0x758 */
        u32  credit1;          /* +0x75c */
        u8   exitFlag;         /* +0x760 */
        u8   pad761[3];
        u32  field764;         /* +0x764 */
        u16  count;            /* +0x768 */
    } buf;

    /* list-menu descriptor (frame +0x108), 7 ptr fields + 2 trailing bytes */
    struct ListDesc {
        void* selOut;          /* &lbl_8047A3F8 */
        void* entryList;       /* table + nameIdx*2 */
        u32   entryCount;
        void* f114;            /* &(*(u32*)&lbl_8047A3F4) */
        void* f118;            /* &(*(u32*)&lbl_8047A3F0) */
        void* f11c;            /* &(*(u32*)&lbl_8047A3EC) */
        void* f120;            /* &(*(u32*)&lbl_8047A3E8) */
        u8    modeByte;        /* +0x124 */
        u8    f125;            /* +0x125 */
    } desc1;

    /* quantity-selector descriptor (frame +0x14/+0x18) */
    struct QtyDesc {
        u32   enable;          /* +0x18 = 1 */
        s32   maxQty;          /* +0x1c = r16 (max affordable) */
        s32   unitPrice;       /* +0x20 = r17 */
        void* total;           /* +0x24 = &lbl_8047A3E0 */
        u8    colR;            /* +0x28 */
        u8    colG;            /* +0x29 */
        u8    colB;            /* +0x2a */
        u8    pad2b;
        u32   f2c;             /* +0x2c = 1 */
    } qty;
    s32 qtyTitle;              /* frame +0x14 = 1 (title flag passed via r5) */

    /* inventory snapshot used in the mode-3 commit path (frame +0x30) */
    struct InvHdr {
        u32 a;                 /* +0x30 = credit0 */
        u32 b;                 /* +0x34 = credit1 */
        u32 c;                 /* +0x38 = field764 */
        u16 zero;              /* +0x3c */
        u16 n;                 /* +0x3e = count */
        struct { u16 id; u16 qty; } items[51]; /* +0x40.. */
    } inv;

    u8  msgBuf[4];             /* small format scratch (frame +0x10/+0x11) */

    s32 r25;                   /* loop/return state accumulator */
    s32 r15_have;             /* available currency for mode-4 affordability pre-check */
    s32 minCost;               /* min entry value across the location list */
    u16* listStart;            /* u16 entry list for the location */
    u8* colorEntry;            /* RGB triple for this mode */
    u8 modeLow;                /* mode & 0xff */
    s32 idxX4;                 /* mapIdx << 2 */
    s32 ok;
    s32 sel;                   /* selected entry id (r25 inner) */
    s32 maxAfford;             /* r16 */
    s32 unitPrice;             /* r17 */
    s32 species;               /* r18 */
    s32 chosenQty;             /* r22 */
    s32 totalCost;             /* r21 */
    s32 ownedRoom;             /* r17 reused: owned/room count */
    s32 yn;                    /* r16 reused: yes/no result */
    s32 i;
    s32 next;                  /* r0 -> r25 in CB40 */

    modeLow = (u8)(mode & 0xff);
    r25 = 0;

    /* ===== Phase A: per-mode init ===== */
    if (modeLow == 3) {
        ok = fn_80029CC0((u8*)&buf);
    } else if (modeLow == 2) {
        fn_80142A88((u8*)&buf, 0xeb);
        fn_80142A88((u8*)&buf + 0x3ac, 0xeb);
        buf.credit0 = 0;
        buf.credit1 = 0;
        buf.exitFlag = 0;
        {
            u32 snap = fn_80129280((u8*)0, 3);
            memcpy((void*)lbl_8047A3DC, (const void*)snap, 0x7198);
        }
        lbl_8047A3D8 = fn_8012A5B0((u8*)0, 0xd, 0);
        ok = 1;
    } else {
        ok = 1;
    }
    if (ok == 0) {
        goto done;
    }

    /* ===== Phase B: mode-4 affordability gate ===== */
    if ((mode & 0xff) == 4) {
        /* modeLow is provably 4 here -> available currency via interaction getter */
        r15_have = (s32)fn_8012A5B0((u8*)0, 0xd, 0);

        /* min entry value across this location's list */
        listStart = (u16*)((u8*)lbl_80478E44 +
                    (u32)(*(u16*)((u8*)lbl_80478E54 + (u32)mapIdx * 4 + 2)) * 2);
        minCost = 0x98967F; /* 9999999 sentinel */
        {
            u16* p = listStart;
            while (*p != 0) {
                fn_801440A0(*p);
                {
                    s32 v = (s32)(u16)fn_80143FE4();
                    if (v < minCost) minCost = v;
                }
                p++;
            }
        }
        if (r15_have < minCost) {
            fn_8002A1C4((u8*)mapIdx, 4, -1);   /* "can't afford anything" */
            goto done;
        }
    }

    /* ===== main retry loop setup (L_8002C600) ===== */
    modeLow = (u8)(mode & 0xff);
    colorEntry = &lbl_80266E70[(u32)modeLow * 3];
    idxX4 = mapIdx << 2;

    /* do { body } while (r25 == 0)  -- entry jumps straight to the test */
    goto loop_test;

L_628:
    /* (1) optional list rebuild from lbl_802E4F68 / lbl_802EF0A8 records */
    if (lbl_804788A8 != 0) {
        s16* dst = (s16*)lbl_802E4F68;          /* fields at +4,+0xc,+0x14,+0x1c,+0x24 */
        u8*  recs = lbl_802EF0A8;               /* 0x1c-stride records */
        u32* sel5 = (u32*)lbl_802E4F68;         /* selector indices at +0,+8,+0x10,+0x18,+0x20 */
        lbl_804788A8 = 0;
        dst[2]  = *(s16*)(recs + sel5[0] * 0x1c + 4);
        dst[6]  = *(s16*)(recs + sel5[2] * 0x1c + 4);
        dst[10] = *(s16*)(recs + sel5[4] * 0x1c + 4);
        dst[14] = *(s16*)(recs + sel5[6] * 0x1c + 4);
        dst[18] = *(s16*)(recs + sel5[8] * 0x1c + 4);
    }

    /* (2) build the list-menu descriptor and open it */
    lbl_8047A3F8 = 0;
    {
        u16* p = (u16*)((u8*)lbl_80478E44 +
                 (u32)(*(u16*)((u8*)lbl_80478E54 + (u32)idxX4 + 2)) * 2);
        u32 cnt = 0;
        desc1.entryList = p;
        while (*p != 0) { p++; cnt++; }
        desc1.selOut    = &lbl_8047A3F8;
        desc1.entryCount = cnt;
        desc1.f114      = &(*(u32*)&lbl_8047A3F4);
        desc1.f118      = &(*(u32*)&lbl_8047A3F0);
        desc1.f11c      = &(*(u32*)&lbl_8047A3EC);
        desc1.f120      = &(*(u32*)&lbl_8047A3E8);
        desc1.modeByte  = (u8)mode;
        desc1.f125      = 0;
    }
    {
        s32 r = fn_801026A4((void*)0x60, fn_801046B8(), 0, 0, (void*)1, 1, &desc1);
        if (r == -1) r25 = 0;
        else         r25 = (s32)*(u16*)desc1.selOut;
    }

    sel = (s32)(u16)r25;
    if (sel == 0) {
        goto L_CB40;
    }

    /* (3) affordability for the chosen entry -> max quantity */
    fn_801440A0((u16)sel);
    maxAfford = (s32)(u16)fn_80143FE4();
    if (maxAfford > 0) {
        s32 have;
        /* modeLow is 4 in this loop; preserve the per-mode currency fetch */
        have = (s32)fn_8012A5B0((u8*)0, 0xd, 0);
        maxAfford = have / maxAfford;
        if (maxAfford > 0x63) maxAfford = 0x63;
    } else {
        maxAfford = 0x63;
    }
    if (maxAfford <= 0) {
        fn_8002A1C4((u8*)mapIdx, 8, -1);   /* sold out / cannot buy */
        goto L_628;
    }

    /* (4) format header line, open quantity selector */
    fn_801440A0((u16)sel);
    unitPrice = (s32)(u16)fn_80143FE4();
    species   = (s32)(u16)sel;
    fn_80029FAC(&msgBuf[1], mapIdx, 0xc, 0x2d, species, -1);
    lbl_8047A3E4 = 0; /* result captured by fn_80029FAC via &msgBuf[1] path */

    if (maxAfford >= 1) {
        qty.enable    = 1;
        qty.maxQty    = maxAfford;
        qty.unitPrice = unitPrice;
        lbl_8047A3E0  = 1;
        qty.total     = &lbl_8047A3E0;
        qty.colR      = colorEntry[0];
        qty.colG      = colorEntry[1];
        qty.colB      = colorEntry[2];
        qty.f2c       = 1;
        qtyTitle      = 1;
        yn = fn_801026A4((void*)0x61, fn_801046B8(), 0, 1, &qtyTitle, (s32)(uintptr_t)&qty);
        fn_80102510(0x61);
        menuCloseSync((void*)0x61, 1);
        if (yn == -1) chosenQty = -1;
        else          chosenQty = (s32)lbl_8047A3E0;
    } else {
        chosenQty = 0;
    }
    if (chosenQty < 0) {
        goto L_628;
    }

    /* (5) total cost, confirm yes/no */
    totalCost = chosenQty * unitPrice;
    fn_80029FAC(&msgBuf[1], mapIdx, 5, 0x2d, species, 0x2f, totalCost, -1, chosenQty, 0x4b);
    fn_80106ADC(2, lbl_8047A3E4, 1, 0, msgBuf[1]);
    yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
    fn_801069FC(1);
    if (yn == 1) goto L_628;     /* cancel */
    if (yn == -1) goto L_628;    /* aborted */

    /* (6) compute owned count / room left for this species */
    if (modeLow == 2) {
        ownedRoom = (s32)(u16)fn_80134420(0, (u16)sel);
    } else if (modeLow == 3) {
        ownedRoom = 0;
        {
            u8* base = (u8*)&buf;
            u16 n = buf.count;
            for (i = 0; i < (s32)n; i++) {
                void* slot = base + i * 4;
                u16 id = fn_80143C68(slot);
                if (id == (u16)sel) {
                    ownedRoom += (s32)(u16)(0x3e7 - fn_80143C50(slot));
                } else if (id == 0) {
                    ownedRoom += 0x3e7;
                }
            }
        }
    } else {
        ownedRoom = fn_801298B8((u8*)0, (u32)(u16)sel);
    }

    if (ownedRoom < chosenQty) {
        fn_8002A1C4((u8*)mapIdx, 9, -1);   /* no room */
        goto L_628;
    }

    /* (7) commit purchase */
    fn_8002A1C4((u8*)mapIdx, 6, -1);       /* "thank you" */
    fn_80029EF4((void*)(u32)totalCost, sel, chosenQty, (u8)mode, (void*)&buf);
    fn_80166AB8(0x3cc, 0, 0);              /* purchase SE */

    if ((mode & 0xff) == 4) {
        /* mode-4 "buy another?" loop */
        fn_80029FAC(&msgBuf[1], mapIdx, 7, -1);
        fn_80106ADC(2, lbl_8047A3E4, 1, 0, msgBuf[1]);
        yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
        fn_801069FC(1);
        if (yn == -1) goto L_CB40;
        if (yn != 1)  goto L_628;
    }

L_CB40:
    /* ===== finalize: only modes 2 and 3 run the close-out dialogs ===== */
    if (modeLow != 2 && modeLow != 3) {
        r25 = 1;
        goto loop_test;
    }

    if (buf.exitFlag == 0) {
        fn_80029FAC(&msgBuf[0], mapIdx, 0xd, -1);
        fn_80106ADC(2, lbl_8047A3E4, 1, 0, msgBuf[0]);
        yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
        fn_801069FC(1);
        if (yn == 1 || yn == -1) {
            next = 0;
        } else {
            if ((mode & 0xff) == 3) {
                fn_8002A1C4((u8*)mapIdx, 0xe, -1);
            }
            next = 1;
        }
        r25 = next;
        goto loop_test;
    } else {
        fn_80029FAC(&msgBuf[0], mapIdx, 0xf, -1);
        fn_80106ADC(2, lbl_8047A3E4, 1, 0, msgBuf[0]);
        yn = (s8)menuSubOpenYesNo(0, -1, -1, 0);
        fn_801069FC(1);
        if (yn == 1 || yn == -1) {
            next = 0;
            r25 = next;
            goto loop_test;
        }
        if ((mode & 0xff) != 3) {
            /* default-mode close path: confirm-state query then optional restore */
            if (fn_801D0748(4, 2, 0) != 4) {
                u32 snap = fn_80129280((u8*)0, 3);
                memcpy((void*)lbl_8047A3DC, (const void*)snap, 0x7198);
                fn_8012A450((u8*)0, 0xd, lbl_8047A3D8);
            }
            r25 = 1;
            goto loop_test;
        }
        /* mode 3: rebuild inventory snapshot, run sell/keep sub-flow */
        {
            u8  itemType;
            u8* recBase = (u8*)lbl_80478E4C +
                          (u32)(*(u8*)((u8*)lbl_80478E54 + (u32)idxX4)) * 0x4c;
            itemType = recBase[0];
            fn_80106ADC(2, 0x3d83, 0, 0, itemType);

            memset(&inv, 0, 0xd8);
            inv.a = buf.credit0;
            inv.b = buf.credit1;
            inv.c = buf.field764;
            inv.zero = 0;
            inv.n = buf.count;

            {
                u8* base = (u8*)&buf;
                s32 j;
                for (j = 0; j < (s32)inv.n; j++) {
                    u16 q = 0;
                    u16 id;
                    void* slot = base + j * 4;
                    if (j < 0 || j > (s32)buf.count) {
                        id = 0;
                    } else {
                        id = fn_80143C68(slot);
                        if (id != 0) {
                            inv.items[j].qty = fn_80143C50(slot);
                        }
                    }
                    inv.items[j].id = id;
                    if (id == 0) {
                        inv.items[j].id = 0;  /* terminate */
                        break;
                    }
                    (void)q;
                }
            }

            fn_80093574(1);
            fn_80092C90(1, &inv, 0);
            fn_80093574(1);
            if (fn_80093610(1) == 0xc) {
                fn_80093698(1);
                fn_80106ADC(2, 0x3d84, 1, 0, itemType);
                fn_801069FC(1);
            } else {
                fn_80093698(1);
                fn_80106ADC(2, 0x3d85, 1, 0, itemType);
                fn_801069FC(1);
            }
            fn_8002A1C4((u8*)mapIdx, 0xe, -1);
        }
        r25 = 1;
        goto loop_test;
    }

loop_test:
    if (r25 == 0) {
        goto L_628;
    }

    /* selection made -> close menu group and return */
    fn_80102568((void*)0x60, 0, 1);

done:
    return;
}
#endif

/* fn_8002CE6C - 0x8002CE6C | size: 0x2e8 */
extern void fn_800D3088(void);
extern void fn_80129474(void);
extern u32 lbl_804788A8;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
#if 1
asm void fn_8002CE6C(void) {
#include "src/game/gs_worldmap_fn_8002CE6C.inc"
}
#else
/*
 * fn_8002CE6C  GSmap_ProcessChoice  0x8002CE6C  size: 0x2E8
 *
 * Shows a trade/interaction menu for the NPC identified by `slot`.
 * `obj` is the worldmap context/player-object pointer.
 * `slot` (r4, u8) selects which NPC slot's item list to display.
 *
 * Flow:
 *  1. Open display engine, get nearest trade partner location,
 *     spin-yield until >= 30 frames have accumulated.
 *  2. If a pending key-remap flag is set, refresh the 5 dialog-key
 *     entries from the lbl_802EF0A8 lookup table.
 *  3. Count available items, build a menu descriptor on the stack,
 *     call fn_801046B8 + fn_801026A4 to show the selection dialog.
 *  4. On cancel (-1) → post "cancel" message and return.
 *  5. On selection:
 *       - look up species data (fn_801440A0) and get trade count (fn_80143FFC).
 *       - if count == 0 or ratio > 99 → post "no stock" message and return.
 *       - if trade precondition fails (fn_801298B8 < 1) → post "busy" message and return.
 *       - otherwise: play sound, deduct from party (fn_80129474),
 *         execute trade (fn_80129A78), then loop back to show updated menu.
 *  6. Close display engine and return.
 */
void fn_8002CE6C(u8* obj, u8 slot) {
    extern u32  fn_80102568(u32 a, u32 b, u32 c);    /* display engine open/close */
    extern void fn_8002A2CC(u8* obj, s32 msgId, s32 arg2, ...); /* post format message */
    extern void _threadSwitch(void);                     /* GSthread yield */
    extern void fn_800D3088(void);                     /* GSgfx tick / frame advance */
    extern u32  fn_8012A5B0(u8* ptr, u32 sel, u32 idx); /* interaction getter */
    extern u32  fn_801046B8(void);                    /* get display context handle */
    extern s32 fn_801026A4(u32 sceneId, u32 a, u32 b, u32 c, u32 d, u32 e, ...); /* show menu dialog */
    extern void fn_801440A0(u32 speciesId);            /* load species data */
    extern u16  fn_80143FFC(void);                    /* get trade/field count */
    extern s32  fn_801298B8(u8* ptr, u32 itemId);     /* check trade precondition */
    extern void fn_80166AB8(u32 soundId, u32 a, u32 b); /* play sound */
    extern void fn_80129474(u8* ptr, u32 offset);     /* deduct from party (give Pokemon) */
    extern s32  fn_80129A78(u8* ptr, u32 itemId, u32 qty, u32 flags); /* execute trade/receive */

    /* lbl_802E4F68: table of 5 dialog key records, each record has:
     *   +0x00  s32  key value (used for match)
     *   +0x04  s16  display field (written back from lbl_802EF0A8 lookup)
     *   record stride = 0x8 bytes
     * Five entries at offsets 0, 8, 0x10, 0x18, 0x20
     * The s16 at +4 within each entry maps to sth/lha target fields.
     */
    extern u8  lbl_802E4F68[];   /* dialog key table: 5 entries * 0x8 bytes (keys at +0, s16 at +4) */
    /* lbl_802EF0A8: lookup table, each record is 0x1c bytes, s16 at +4 */
    extern u8  lbl_802EF0A8[];

    extern u32 lbl_804788A8;     /* pending key-remap flag */
    extern u32 lbl_80478E54;     /* pointer to location/map table (u8*) */
    extern u32 lbl_80478E44;     /* pointer to NPC item list table (u16*) */

    /* Stack-local dialog descriptor (matches fn_8002C284 convention):
     *   sp+0x08  ptr  pointer to dialog key list (written at line 75)
     *   sp+0x0c  ptr  ptr to start of the matching item list entry (r8)
     *   sp+0x10  u32  item count (r7)
     *   sp+0x14  ptr  &lbl_8047A3F4
     *   sp+0x18  ptr  &(*(u16*)&lbl_8047A3F0)
     *   sp+0x1c  ptr  &lbl_8047A3EC
     *   sp+0x20  ptr  &(*(u16*)&lbl_8047A3E8)
     *   sp+0x24  u8   slot (r30)
     *   sp+0x25  u8   zero
     */
    extern u16 lbl_8047A3F8;    /* dialog key list head (u16, zeroed each iteration) */
    extern u16 lbl_8047A3F4;
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16 lbl_8047A3EC;
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */

    /* Saved registers */
    u32  obj_idx;     /* r31 = obj * 4, used as byte-offset into u16 location table */
    u32  frames;      /* r28, frame accumulator for the spin-yield */
    u16  selection;   /* r28 after dialog, selected item key */
    u16  species;     /* r27 = fn_801440A0 result (first call) */
    s32  trade_count; /* r26 = fn_80143FFC result (second call) */
    s32  ratio;       /* r0, computed trade ratio */

    /* --- derive the byte-offset index from the obj pointer integer value --- */
    /* r29 = (u32)obj, r31 = r29 << 2 = (u32)obj * 4 */
    /* FUNCTIONAL-TODO: obj is treated as both a pointer (for fn_8002A2CC) and
     * as a small integer index (slwi r31,r29,2 then lhzx into location table).
     * This pattern matches other scene-callback fns in this TU where the first
     * arg doubles as a scene-slot index.  Cast preserved below. */
    obj_idx = (u32)(u32)obj << 2;   /* ENDIAN-QA: r29 integer value * 4 */

_loop_top:
    /* 1. Open display engine, get nearest interaction partner, start message */
    fn_80102568(0x60, 0, 1);
    {
        u32 nearest = fn_8012A5B0(NULL, 0xc, 0);
        fn_8002A2CC(obj, 0, 0x4b, (s32)nearest, -1);
    }

    /* 2. Spin-yield for 30 frames */
    frames = 0;
    while (frames < 0x1e) {
        _threadSwitch();
        fn_800D3088();
        frames += 1; /* fn_800D3088 returns frame delta in r3 per usage elsewhere;
                      * here the asm does: bl fn_800D3088; add r28,r28,r3 →
                      * accumulate whatever fn_800D3088 returns (typically 1) */
        /* FUNCTIONAL-TODO: original accumulates fn_800D3088's return value.
         * Since fn_800D3088 is a gfx tick that returns 1 per frame, this is
         * functionally equivalent to counting frames. */
    }

    /* 3. If pending key-remap flag: refresh 5 dialog-key s16 fields */
    if (lbl_804788A8 != 0) {
        /* The asm loads 5 indices from lbl_802E4F68[0..4 * 8] at offset +0,
         * multiplies each by 0x1c to index lbl_802EF0A8, reads s16 at +4,
         * and stores back into lbl_802E4F68[entry * 8 + 4].
         * Entry offsets within lbl_802E4F68: 0x0, 0x8, 0x10, 0x18, 0x20.
         * Reading order from asm (r4/r6/r5/r4/r3 = entries 0,2,4,3,1):
         *   entry0 idx at +0x00, entry2 idx at +0x08 (lhzx offset 0x8 from base),
         *   etc.  The indices are loaded in parallel then written sequentially.
         * Preserve exact logic: for each of 5 entries, read the s32 index at
         * the entry's +0 field, look up lbl_802EF0A8[index*0x1c]+4 (s16),
         * store back to entry's +4 field. */
        {
            s32 idx0 = *(s32*)(lbl_802E4F68 + 0x00);
            s32 idx1 = *(s32*)(lbl_802E4F68 + 0x08); /* ENDIAN-QA */
            s32 idx2 = *(s32*)(lbl_802E4F68 + 0x10); /* ENDIAN-QA */
            s32 idx3 = *(s32*)(lbl_802E4F68 + 0x18); /* ENDIAN-QA */
            s32 idx4 = *(s32*)(lbl_802E4F68 + 0x20); /* ENDIAN-QA */
            lbl_804788A8 = 0;
            *(s16*)(lbl_802E4F68 + 0x04) = *(s16*)(lbl_802EF0A8 + idx0 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x0c) = *(s16*)(lbl_802EF0A8 + idx1 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x14) = *(s16*)(lbl_802EF0A8 + idx2 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x1c) = *(s16*)(lbl_802EF0A8 + idx3 * 0x1c + 0x4);
            *(s16*)(lbl_802E4F68 + 0x24) = *(s16*)(lbl_802EF0A8 + idx4 * 0x1c + 0x4);
        }
    }

    /* 4. Build dialog descriptor and show selection menu */
    {
        /* Locate start of this slot's item list in the NPC item table.
         * lbl_80478E54 points to a per-location u16 offset table; index by
         * obj_idx (== (u32)obj * 4) to get a u16 into lbl_80478E44's item list.
         * Advance r8 until a zero-terminator u16 is found, counting entries. */
        u16 *item_list;
        u16 *cur;
        u32  item_count;
        u16 *dialog_key_list;
        u32  menu_result;
        u32  disp_handle;

        lbl_8047A3F8 = 0;
        dialog_key_list = &lbl_8047A3F8;

        {
            /* r0 = *(u8*)lbl_80478E54 + 2 as a base, then lhzx with obj_idx */
            u8  *loc_table = (u8*)lbl_80478E54;
            u16  entry_offset = *(u16*)(loc_table + 2 + obj_idx); /* lhzx r0,r31,r0 */
            u16 *item_base    = (u16*)((u8*)lbl_80478E44 + (u32)entry_offset * 2);
            item_list = item_base;
        }

        /* Count items until zero terminator */
        cur = item_list;
        item_count = 0;
        while (*cur != 0) {
            cur++;
            item_count++;
        }

        /* Build stack descriptor (7 words + 2 bytes matching the asm layout):
         *   [0] = dialog_key_list ptr   (sp+0x08)
         *   [1] = item_list ptr         (sp+0x0c)
         *   [2] = item_count            (sp+0x10)
         *   [3] = &lbl_8047A3F4         (sp+0x14)
         *   [4] = &(*(u16*)&lbl_8047A3F0)         (sp+0x18)
         *   [5] = &lbl_8047A3EC         (sp+0x1c)
         *   [6] = &(*(u16*)&lbl_8047A3E8)         (sp+0x20)
         *   [7] = slot (u8, sp+0x24), zero (u8, sp+0x25)
         */
        struct {
            void *key_list;
            void *items;
            u32   count;
            void *f_a3f4;
            void *f_a3f0;
            void *f_a3ec;
            void *f_a3e8;
            u8    npc_slot;
            u8    _pad;
        } desc;
        desc.key_list = dialog_key_list;
        desc.items    = item_list;
        desc.count    = item_count;
        desc.f_a3f4   = &lbl_8047A3F4;
        desc.f_a3f0   = &(*(u16*)&lbl_8047A3F0);
        desc.f_a3ec   = &lbl_8047A3EC;
        desc.f_a3e8   = &(*(u16*)&lbl_8047A3E8);
        desc.npc_slot = slot;
        desc._pad     = 0;

        disp_handle = fn_801046B8();
        menu_result = (u32)fn_801026A4(0x60, disp_handle, 0, 0, 1, 1, &desc);

        /* 5a. Cancelled */
        if ((s32)menu_result == -1) {
            fn_8002A2CC(obj, 2, -1);
            goto _done;
        }

        /* 5b. Read the selected u16 item key from the dialog result pointer */
        selection = *(u16*)desc.key_list;

        if ((selection & 0xffff) == 0) {
            /* Zero selection = cancel path */
            fn_8002A2CC(obj, 2, -1);
            goto _done;
        }

        /* 6. Look up species and count for selected item */
        fn_801440A0((u32)selection);
        species = (u16)fn_80143FFC();

        fn_801440A0((u32)selection);
        trade_count = (s16)fn_80143FFC();

        /* 7. Compute stock ratio and check >= 1 */
        if (trade_count > 0) {
            u32 stock = fn_8012A5B0(NULL, 0xc, 0);
            ratio = (s32)stock / trade_count;
            if (ratio > 0x63) ratio = 0x63;
        } else {
            ratio = 0x63;
        }

        if (ratio <= 0) {
            fn_8002A2CC(obj, 5, -1);
            goto _done;
        }

        /* 8. Check trade precondition */
        if (fn_801298B8(NULL, (u32)selection) < 1) {
            fn_8002A2CC(obj, 6, -1);
            goto _done;
        }

        /* 9. Execute trade: play sound, deduct, trade, then loop */
        fn_80166AB8(0x3cb, 0, 0);
        fn_80129474(NULL, (u32)species);   /* give/deduct partner Pokemon by species offset */
        fn_80129A78(NULL, (u32)selection, 1, -1); /* execute trade/receive */
        fn_8002A2CC(obj, 4, 0x2d, (s32)(selection & 0xffff), -1);
        goto _loop_top;
    }

_done:
    /* 10. Close display engine */
    fn_80102568(0x60, 0, 1);
}
#endif

/* fn_8002D154 - 0x8002D154 | size: 0x480 */
extern void fn_80129B2C(void);
extern u32 lbl_804788A8;
extern u32 lbl_80478E54;
extern u32 lbl_80478E44;
extern u32 lbl_8047A3E4;
#if 1
asm void fn_8002D154(void) {
#include "src/game/gs_worldmap_fn_8002D154.inc"
}
#else
/*
 * fn_8002D154  GSmap_ConfirmSequence  (0x8002D154, size 0x480)
 *
 * World-map "shop / confirm purchase" interaction loop.
 *   mapIndex    (r3) : map slot index; selects the destination/shop entry list
 *                      (used as <<2 word index and passed to the text formatters).
 *   colorIndex  (r4) : low byte selects an RGB triple in lbl_80266E70[index*3]
 *                      used to tint the quantity menu.
 *
 * Flow: (1) optionally refresh a 5-entry stat-mirror struct, (2) open the item
 * list menu, (3) on a chosen item validate category/affordability, (4) open a
 * quantity menu, (5) confirm with a Yes/No prompt and commit the purchase, then
 * loop until the player backs out (empty selection) which closes the menu.
 */
void fn_8002D154(s32 mapIndex, u8 colorIndex)
{
    /* ---- block-scope typed externs (TU convention) ---- */
    extern u32  lbl_804788A8;        /* "stats dirty" flag           */
    extern u32  lbl_80478E54;        /* map -> list-index base table */
    extern u32  lbl_80478E44;        /* destination/shop list table  */
    extern u32  lbl_8047A3E4;        /* formatted-string handle out  */

    extern u16  lbl_8047A3F8;        /* item-menu result (selected item id) */
    extern u16  lbl_8047A3F4;
    extern f32 lbl_8047A3F0;  /* canonical; per-site reinterpret cast */
    extern u16  lbl_8047A3EC;
    extern f32 lbl_8047A3E8;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A3E0;  /* canonical; per-site reinterpret cast */

    extern u8   lbl_80266E70[];      /* RGB color triples, 3 bytes/entry */

    /* stat-mirror source/dest blocks */
    struct StatMirror {
        u32 src0;  u16 dst0;  u16 _p0;   /* +0x00 idx, +0x04 out */
        u32 src1;  u16 dst1;  u16 _p1;   /* +0x08 idx, +0x0c out */
        u32 src2;  u16 dst2;  u16 _p2;   /* +0x10 idx, +0x14 out */
        u32 src3;  u16 dst3;  u16 _p3;   /* +0x18 idx, +0x1c out */
        u32 src4;  u16 dst4;  u16 _p4;   /* +0x20 idx, +0x24 out */
    };
    extern u8 lbl_802E4F68[];  /* canonical; per-site reinterpret cast */
    extern u8 lbl_802EF0A8[];  /* canonical; per-site reinterpret cast */

    /* callees */
    extern u32  fn_801046B8(void);                                  /* menu ctx handle */
    extern s32 fn_801026A4(s32 a, u32 b, void* c, s32 d, s32 e, s32 f, void* desc); /* open list menu */
    extern void fn_80102510(s32 menuId);                           /* menu post-step  */
    extern void menuCloseSync(s32 menuId, s32 flag);
    extern void menuSubOpenYesNo(s32 a, s32 b, s32 c, s32 d);
    extern void fn_801069FC(s32 a);
    extern void fn_80102568(s32 a, s32 b, s32 c);                  /* close/release   */
    extern u32  fn_801440A0(u32 itemId);                           /* select item     */
    extern u32  fn_80144014(void);                                 /* item category   */
    extern u32  fn_80143FFC(void);                                 /* item price/value*/
    extern s8   fn_80129B2C(s32 a, s32 b);                         /* story/flag query*/
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx);       /* money getter    */
    extern u32  fn_801298B8(s32 a, u32 item);                      /* inventory count */
    extern void fn_80129474(s32 a, s32 amount);                    /* spend money     */
    extern void fn_80129A78(s32 a, s32 item, u16 qty, s32 d);      /* add item        */
    extern void fn_80166AB8(s32 sfx, s32 b, s32 c);               /* play sound      */
    extern void fn_80106ADC(s32 a, u32 b, s32 c, s32 d, u8 e);
    extern u32  fn_8002A0B8(u8* buf, s32 idx, s32 a, s32 b, ...);  /* format text     */
    extern void fn_8002A2CC(u8* idx, s32 a, s32 b, ...);          /* format/print txt*/

    /* RGB tint for this color index */
    u8*  rgb = &lbl_80266E70[(colorIndex & 0xff) * 3];

    /* item-list menu descriptor (consumed by fn_801026A4 via the pointer block) */
    struct {
        u16* result;        /* +0x00 -> lbl_8047A3F8        */
        u16* list;          /* +0x04 destination/shop list  */
        s32  count;         /* +0x08 entry count            */
        u16* slot1;         /* +0x0c -> lbl_8047A3F4        */
        u16* slot2;         /* +0x10 -> (*(u16*)&lbl_8047A3F0)        */
        u16* slot3;         /* +0x14 -> lbl_8047A3EC        */
        u16* slot4;         /* +0x18 -> (*(u16*)&lbl_8047A3E8)        */
        u8   color;         /* +0x1c                        */
        u8   flag;          /* +0x1d                        */
    } itemDesc;

    /* quantity-menu descriptor */
    struct {
        s32  enabled;       /* +0x00 (=1)                   */
        s32  maxQty;        /* +0x04                        */
        s32  unitPrice;     /* +0x08                        */
        s32* result;        /* +0x0c -> (*(s32*)&lbl_8047A3E0)        */
        u8   r;             /* +0x10                        */
        u8   g;             /* +0x11                        */
        u8   b;             /* +0x12                        */
        u8   _pad;          /* +0x13                        */
        u32  trailer;       /* +0x14 (=0)                   */
    } qtyDesc;
    s32 qtyHeader;          /* the separate +0x14 word passed as r5 (=1) */

    u16  selectedItem;
    s32  maxAffordable;
    u32  itemHandle;
    u32  unitPrice;
    s32  chosenQty;
    s32  totalCost;
    s8   yesNo;
    u32  ctx;
    s32  menuRet;
    u8   fmtBuf[0x20];      /* scratch buffer filled by fn_8002A0B8 */

    for (;;) {
        /* (1) refresh the 5-entry stat mirror when flagged dirty */
        if (lbl_804788A8 != 0) {
            struct StatMirror* m = &(*(struct StatMirror*)lbl_802E4F68);
            u32 i0 = m->src0, i1 = m->src1, i2 = m->src2, i3 = m->src3, i4 = m->src4;
            lbl_804788A8 = 0;
            /* each record is 0x1c bytes; field at +4 (s16). ((s16*)lbl_802EF0A8) is s16[]
               so record i's +4 field is element index i*(0x1c/2) + 2 == i*14 + 2 */
            m->dst0 = (u16)((s16*)lbl_802EF0A8)[i0 * 14 + 2];   /* ENDIAN-QA */
            m->dst1 = (u16)((s16*)lbl_802EF0A8)[i1 * 14 + 2];   /* ENDIAN-QA */
            m->dst2 = (u16)((s16*)lbl_802EF0A8)[i2 * 14 + 2];   /* ENDIAN-QA */
            m->dst3 = (u16)((s16*)lbl_802EF0A8)[i3 * 14 + 2];   /* ENDIAN-QA */
            m->dst4 = (u16)((s16*)lbl_802EF0A8)[i4 * 14 + 2];   /* ENDIAN-QA */
        }

        /* (2) build the destination/shop list and open the item menu */
        lbl_8047A3F8 = 0;
        {
            u16* base = (u16*)(lbl_80478E44 +
                        (u32)(*(u16*)((u8*)lbl_80478E54 + 2 + mapIndex * 4)) * 2);
            s32  count = 0;
            u16* p = base;
            while (*p != 0) { p++; count++; }

            itemDesc.result = &lbl_8047A3F8;
            itemDesc.list   = base;
            itemDesc.count  = count;
            itemDesc.slot1  = &lbl_8047A3F4;
            itemDesc.slot2  = &(*(u16*)&lbl_8047A3F0);
            itemDesc.slot3  = &lbl_8047A3EC;
            itemDesc.slot4  = &(*(u16*)&lbl_8047A3E8);
            itemDesc.color  = colorIndex;
            itemDesc.flag   = 0;
        }

        ctx     = fn_801046B8();
        menuRet = fn_801026A4(0x60, ctx, (void*)0, 0, 1, 1, &itemDesc);
        if (menuRet == -1) {
            selectedItem = 0;
        } else {
            selectedItem = lbl_8047A3F8;
        }

        /* (3) empty selection -> back out and close the menu */
        if ((selectedItem & 0xffff) == 0) {
            fn_80102568(0x60, 0, 1);
            return;
        }

        /* validate the item exists */
        if (fn_801440A0(selectedItem) == 0) {
            continue;
        }

        /* special-case: "key item" category (6) gated by a story flag */
        if ((fn_80144014() & 0xff) == 6) {
            if ((fn_80129B2C(0, 0x21e) & 0xff) == 0) {
                fn_8002A2CC((u8*)mapIndex, 8, -1);
                continue;
            }
        }

        /* (3b) compute how many the player can afford (capped at 99) */
        fn_801440A0(selectedItem);
        maxAffordable = (s32)(fn_80143FFC() & 0xffff);
        if (maxAffordable > 0) {
            maxAffordable = (s32)fn_8012A5B0((u8*)0, 0xc, 0) / maxAffordable;
            if (maxAffordable > 0x63) {
                maxAffordable = 0x63;
            }
        } else {
            maxAffordable = 0x63;
        }
        if (maxAffordable <= 0) {
            fn_8002A2CC((u8*)mapIndex, 5, -1);
            continue;
        }

        /* (4) format the item header and open the quantity selector */
        fn_801440A0(selectedItem);
        unitPrice = fn_80143FFC() & 0xffff;
        {
            u16 itemId16 = selectedItem & 0xffff;
            lbl_8047A3E4 = fn_8002A0B8(fmtBuf, mapIndex, 0xc, 0x2d, itemId16, -1);

            if (maxAffordable < 1) {
                chosenQty = 0;
            } else {
                qtyDesc.enabled   = 1;
                qtyDesc.maxQty    = maxAffordable;
                qtyDesc.unitPrice = (s32)unitPrice;
                qtyDesc.result    = &(*(s32*)&lbl_8047A3E0);
                qtyDesc.r         = rgb[0];
                qtyDesc.g         = rgb[1];
                qtyDesc.b         = rgb[2];
                qtyDesc._pad      = 0;
                qtyDesc.trailer   = 0;
                qtyHeader         = 1;
                (*(s32*)&lbl_8047A3E0)      = 1;

                ctx     = fn_801046B8();
                menuRet = fn_801026A4(0x61, ctx, &qtyHeader, 0, 1, 1, &qtyDesc);
                fn_80102510(0x61);
                menuCloseSync(0x61, 1);
                if (menuRet == -1) {
                    chosenQty = -1;
                } else {
                    chosenQty = (*(s32*)&lbl_8047A3E0);
                }
            }

            if (chosenQty < 0) {
                continue;
            }

            totalCost = chosenQty * (s32)unitPrice;

            /* (5) build the confirmation string and prompt Yes/No */
            menuRet = fn_8002A0B8(fmtBuf, mapIndex, 3, 0x2d,
                                  itemId16, 0x2f, chosenQty, 0x4b, totalCost, -1);
            fn_80106ADC(2, (u32)menuRet, 1, 0, fmtBuf[0]);
            yesNo = (s8)menuSubOpenYesNo(0, -1, -1, 0);
            fn_801069FC(1);
            if (yesNo == 1 || yesNo == -1) {
                continue;
            }

            /* committed: verify capacity, charge money, grant items */
            if ((s32)fn_801298B8(0, selectedItem) < chosenQty) {
                fn_8002A2CC((u8*)mapIndex, 6, -1);
                continue;
            }
            fn_80166AB8(0x3cb, 0, 0);
            fn_80129474(0, totalCost);
            fn_80129A78(0, selectedItem, (u16)chosenQty, -1);
            fn_8002A2CC((u8*)mapIndex, 4, -1);

            /* bonus item: buying >=10 of item 4 grants a premier-ball style extra */
            if ((selectedItem & 0xffff) == 4 && chosenQty >= 0xa &&
                (s32)fn_801298B8(0, 0xc) >= 1) {
                fn_8002A2CC((u8*)mapIndex, 7, -1);
                fn_80129A78(0, 0xc, 1, -1);
            }
        }
    }
}
#endif

/* fn_8002D5D4 - 0x8002D5D4 | size: 0x348 */
extern void fn_80018F54(void);
extern void fn_8010264C(void);
extern void fn_800E3534(void);
extern void fn_800E27B0(void);
extern void fn_800E24B0(void);
extern void fn_800E209C(void);
extern u32 lbl_8047A3FC;
extern u32 lbl_80478E54;
extern u32 lbl_8047A3DC;
#if 1
asm void fn_8002D5D4(void) {
#include "src/game/gs_worldmap_fn_8002D5D4.inc"
}
#else
/*
 * fn_8002D5D4  GSmap_CancelTravel  0x8002D5D4 | size: 0x348
 *
 * Implements the "cancel travel" / travel-confirmation state machine for the
 * world map.  Dispatches on the NPC-state byte at lbl_80478E54[lbl_8047A3FC*4 + 1]:
 *   0 -> show initial location name dialog (menu 0x62), loop until confirmed/cancelled
 *   1 -> hand off to fn_8002CE6C (alternate confirm sequence)
 *   else -> alloc a GSmem block for an extended dialog, run menu 0x83 loop
 *
 * No parameters (the state index lives in lbl_8047A3FC).
 * On exit, frees the GSmem block and optionally fires a story event.
 *
 * Callee conventions used here:
 *   fn_8002A0B8 / fn_80029FAC : vararg text formatters
 *       (u8* outBuf, s32 locIdx, s32 p2, s32 first_va, ..., -1 terminator)
 *   fn_80106ADC               : (s32 kind, u32 tableVal, s32 p3, s32 p4, u8 fmtId)
 *   fn_8010264C               : (s32 menuId, s32 flag) -> s32 result
 *   fn_80102510               : (s32 menuId)
 *   menuCloseSync             : (s32 menuId, s32 flag)
 *   fn_800E3534 / GSmemAllocRaw : (u32 size) -> u16 handle
 *   fn_800E27B0 / GSmemGetPtr   : (u16 handle) -> void*
 *   fn_800E24B0 / GSmemLock     : (u16 handle)
 *   fn_800E209C / GSmemFree     : (u16 handle)
 */
void fn_8002D5D4(void)
{
    /* block-scope externs: TU convention */
    extern u32  lbl_8047A3FC;          /* current location index */
    extern u32  lbl_80478E54;          /* pointer to location map array */
    extern u32  lbl_8047A3DC;          /* scratch pointer for GSmemGetPtr result */
    extern void fn_801069FC(s32);
    extern void fn_80106ADC(s32, u32, s32, s32, u8);
    extern s32  fn_8010264C(s32, s32);
    extern void fn_80102510(s32);
    extern s32  menuCloseSync(s32, s32);
    extern void fn_80018F54(u32, u32, u32);
    extern u16  fn_800E3534(u32);       /* GSmemAllocRaw */
    extern void* fn_800E27B0(u16);      /* GSmemGetPtr */
    extern void fn_800E24B0(u16);       /* GSmemLock */
    extern void fn_800E209C(u16);       /* GSmemFree */
    extern void fn_800FF660(void);
    extern void fn_8011288C(s32, u32);
    extern u32  fn_8012A5B0(u8*, u32, u32);  /* GSmap_GetNearestLocation */
    extern u32  fn_8002A0B8(u8*, s32, s32, s32, ...); /* GSmap_FormatText1 */
    extern void fn_8002A1C4(u8*, s32, s32, ...); /* GSmap_FormatText2 */
    extern void fn_8002A2CC(u8*, s32, s32, ...); /* GSmap_FormatText3 */
    extern u32  fn_80029FAC(u8*, s32, s32, s32, ...); /* GSmap_FormatText0 */
    extern void fn_8002D154(s32, u8);   /* GSmap_ConfirmSequence */
    extern void fn_8002CE6C(s32, u8);   /* GSmap_ProcessChoice */
    extern void fn_8002C408(s32, u8);   /* GSmap_DialogStateMachine */
    extern void fn_8002C284(s32, u8);   /* GSmap_ShowTravelDialog */

    s32   locIdx;        /* r31: lbl_8047A3FC - location index */
    u8    npcState;      /* r30: byte at lbl_80478E54[locIdx*4 + 1] */
    s32   menuResult;    /* r29 */
    s32   choice;        /* r27 */
    u16   memHandle;     /* r28 */
    u8    fmtId;         /* stack byte: sp[8] or sp[9] */
    u32   fmtTableVal;   /* return from fn_8002A0B8 / fn_80029FAC */

    locIdx   = (s32)lbl_8047A3FC;
    npcState = ((u8*)lbl_80478E54)[locIdx * 4 + 1];

    if (npcState == 1) {
        /* --- path 1: hand off to ProcessChoice then exit --- */
        fn_8002CE6C(locIdx, npcState);
        goto _epilogue;
    }
    if (npcState == 0) {
        /* --- path 0: initial location-name dialog (menu 0x62) --- */
        {
            u32 nearLoc = fn_8012A5B0(NULL, 0xc, 0);
            /* format text: first_va = 0x4b, then nearLoc, then -1 */
            fmtTableVal = fn_8002A0B8(&fmtId, locIdx, 0, 0x4b, (s32)nearLoc, -1);
            fn_80106ADC(2, fmtTableVal, 1, 0, fmtId);
        }
        /* menu 0x62 loop */
        for (;;) {
            menuResult = fn_8010264C(0x62, 1);
            fn_80102510(0x62);
            menuCloseSync(0x62, 1);

            if (menuResult == -1 || menuResult == 2) {
                choice = 2;
            } else if (menuResult == 0) {
                choice = 0;
            } else {
                choice = 1;
            }

            if (choice == 2) {
                /* cancelled: send format-text3, exit */
                fn_8002A2CC((u8*)&locIdx, 2, -1);
                goto _epilogue;
            }

            /* choice 0 or 1: process, then re-show dialog */
            fn_801069FC(1);
            if (choice == 0) {
                fn_8002D154(locIdx, npcState);
            } else if (choice == 1) {
                fn_80018F54(3, (u32)locIdx, 0);
            }
            /* choice >= 2 already handled above */

            /* reformat and re-open dialog */
            fmtTableVal = fn_8002A0B8(&fmtId, locIdx, 1, -1);
            fn_80106ADC(2, fmtTableVal, 1, 0, fmtId);
        }
        /* unreachable */
    }

    /* --- path 2+: allocate GSmem block, run extended location dialog (menu 0x83) --- */
    {
        choice     = 0;
        memHandle  = fn_800E3534(0x7198);
        lbl_8047A3DC = (u32)fn_800E27B0(memHandle);

        /* initial format and open */
        fmtTableVal = fn_80029FAC(&fmtId, locIdx, 0, -1);
        fn_80106ADC(2, fmtTableVal, 1, 0, fmtId);

        /* menu 0x83 loop */
        for (;;) {
            if (choice != 0)
                goto _after_reformat;

            /* only re-format on entry and after a "no-op" choice */
            fmtTableVal = fn_80029FAC(&fmtId, locIdx, 1, -1);
            fn_80106ADC(2, fmtTableVal, 1, 0, fmtId);

        _after_reformat:
            menuResult = fn_8010264C(0x83, 1);
            fn_80102510(0x83);
            menuCloseSync(0x83, 1);

            /* normalise menuResult into 0..3 */
            if (menuResult == 0) {
                menuResult = 0;
            } else if (menuResult == 1) {
                menuResult = 1;
            } else if (menuResult == 2) {
                menuResult = 2;
            } else if (menuResult == 3) {
                menuResult = 3; /* FUNCTIONAL-TODO: 3==keep-looping if !=3 exit; see below */
            } else {
                menuResult = 3;
            }

            if (menuResult == 3)
                break; /* exit loop */

            fn_801069FC(1);

            if (menuResult == 0) {
                fn_8002C408(locIdx, npcState);
            } else if (menuResult == 1) {
                fn_8002C284(locIdx, npcState);
            } else if (menuResult == 2) {
                fn_8002A1C4((u8*)&locIdx, 0xb, -1);
            } else {
                /* menuResult == 3 originally maps to r27=1, loop-continue */
                choice = 1;
                continue;
            }

            choice = 0;
            /* loop */
        }
        /* loop exit: choice != 0 (r27 != 0) breaks out */

        /* L_8002D8A8: decide how to close */
        if (npcState == 2 || npcState == 3) {
            fn_801069FC(1);
        } else {
            fn_8002A1C4((u8*)&locIdx, 2, -1);
        }

        fn_800E24B0(memHandle);
        fn_800E209C(memHandle);
    }

_epilogue:
    /* if lbl_8047A3FC[+4] is nonzero, fire story event */
    if (*(u32*)((u8*)&lbl_8047A3FC + 4) != 0) {
        fn_800FF660();
        fn_8011288C(0, 0);
    }
}
#endif

/* fn_8002D91C - 0x8002D91C | size: 0x350 */
extern u32 lbl_80478E54;
extern u32 lbl_8047A3DC;
#if 1
asm void fn_8002D91C(void) {
#include "src/game/gs_worldmap_fn_8002D91C.inc"
}
#else
/*
 * fn_8002D91C  GSmap_ArrivalDialog
 * 0x8002D91C | size: 0x350
 *
 * Drives the post-travel arrival dialog sequence.  Behaviour branches on
 * the "type" byte stored at lbl_80478E54[arg0*4 + 1]:
 *   0  -- name-entry / confirm-or-cancel dialog (menu 0x62)
 *   1  -- direct hand-off to fn_8002CE6C
 *   >=2 -- location-arrival dialog (menu 0x83), sub-type from r28 drives
 *          which sub-dialogs are opened while waiting for user choice.
 *
 * arg0: location/context index passed in by the caller (e.g. 0xB or 0xC
 *       from ui_core.c).
 */
void fn_8002D91C(u32 arg0)
{
    /* block-scope externs in TU convention */
    extern u32  lbl_80478E54;  /* pointer to location-map table base  */
    extern u32  lbl_8047A3DC;  /* GSmem pointer for arrival scratch    */
    extern u32  lbl_8047A3FC;  /* persisted arg0 for this session      */
    extern u32  lbl_8047A400;  /* word at lbl_8047A3FC+4, flag checked at tail */

    extern s32  fn_8010264C(u32 sceneId, u32 p1);  /* scene/menu query */
    extern void fn_80102510(u32 sceneId);           /* scene unload     */
    extern void menuCloseSync(u32 sceneId, u32 p1); /* sync-close menu  */
    extern void fn_801069FC(s32 p);                 /* yield / wait frame */
    extern void fn_80106ADC(s32 a, u32 b, s32 c, s32 d, u8 e); /* text display helper */

    extern u32  fn_8012A5B0(u8 *ptr, u32 selector, u32 idx); /* interaction getter */

    /* format-text helpers (vararg: last s32 arg is -1 terminator) */
    extern u32  fn_8002A0B8(u8 *buf, s32 locIdx, s32 field, s32 p6, ...);
    extern u32  fn_80029FAC(u8 *buf, s32 locIdx, s32 field, s32 p6, ...);
    extern void fn_8002A1C4(u8 *buf, s32 p4, s32 p5, ...);
    extern void fn_8002A2CC(u8 *buf, s32 p4, s32 p5, ...);

    /* sub-dialog launchers */
    extern void fn_8002CE6C(u32 ctx, u32 type); /* process-choice     */
    extern void fn_8002D154(u32 ctx, u32 type); /* confirm-sequence   */
    extern void fn_8002C408(u32 ctx, u32 type); /* dialog-state-machine */
    extern void fn_8002C284(u32 ctx, u32 type); /* show-travel-dialog */

    /* storage / set-box helper */
    extern void fn_80018F54(u32 a, u32 b, u32 c); /* GSpcbox_SetCurrentBox */

    /* GSmem helpers */
    extern u32   fn_800E3534(u32 size);          /* GSmemAllocRaw -> handle */
    extern void *fn_800E27B0(u32 handle);         /* GSmemGetPtr            */
    extern void  fn_800E24B0(u32 handle);         /* GSmemFree (step 1)     */
    extern void  fn_800E209C(u32 handle);         /* GSmemFree (step 2)     */

    /* exit-path helpers */
    extern void fn_800FF660(void);
    extern void fn_8011288C(s32 a, u32 b);

    /* ---- local variables ------------------------------------------------ */
    u8  type_byte;  /* r28: lbl_80478E54[arg0*4 + 1]                        */
    u32 mem_handle; /* r27: GSmemAllocRaw result, only used in >=2 path      */
    s32 loop_state; /* r26: inner-loop exit condition flag                   */
    s32 menu_res;   /* r29: raw menu query result, normalised to 0-3         */
    u8  text_buf0;  /* sp+8: first byte written by fn_80029FAC / fn_8002A0B8 */
    u8  text_buf1;  /* sp+9: first byte written by fn_8002A0B8               */
    u32 interact;   /* r7 scratch for fn_8012A5B0 result                     */

    /* --------------------------------------------------------------------- */
    /* Save arg0 for this dialog session and clear the flag word that follows */
    lbl_8047A3FC = arg0;
    lbl_8047A400 = 0;  /* *(r30+4) = 0 */

    /* Read the type byte from the location table:
     * table base = *(u32*)lbl_80478E54, entry at arg0*4, byte offset 1      */
    type_byte = ((u8 *)lbl_80478E54)[arg0 * 4 + 1];

    /* ================================================================
     * PATH A: type_byte == 0 -- name-entry / menu-0x62 flow
     * ================================================================ */
    if (type_byte == 0) {

        /* Fetch the interaction entry and format the first text line */
        interact = fn_8012A5B0((u8 *)0, 0xc, 0);
        {
            u32 text_entry = fn_8002A0B8(&text_buf1, (s32)arg0, 0, 0x4b,
                                         (s32)interact, (s32)-1);
            fn_80106ADC(2, text_entry, 1, 0, text_buf1);
        }

        /* ---- menu-0x62 wait-loop ---- */
        loop_state = 0;  /* initialise loop exit flag */
        do {
            /* Wait one frame before re-querying */
            fn_801069FC(1);

            /* Branch on previous normalised menu result */
            if (loop_state == 0) {
                fn_8002D154(arg0, (u32)type_byte);
            } else if (loop_state == 1) {
                /* r26 == 1: set current box (args: 3, arg0, 0) */
                fn_80018F54(3, arg0, 0);
            }
            /* r26 > 1: skip both calls */

            /* Re-format the text line with update flag = 1 */
            {
                u32 text_entry = fn_8002A0B8(&text_buf1, (s32)arg0, 1, (s32)-1);
                /* FUNCTIONAL-TODO: The va_arg terminator is -1 in r6; r7
                 * coming from fn_8012A5B0 at entry is gone by this second
                 * call site -- the asm does NOT pass r7 here.             */
                fn_80106ADC(2, text_entry, 1, 0, text_buf1);
            }

            /* Query menu 0x62, then unload/close it */
            menu_res = fn_8010264C(0x62, 1);
            fn_80102510(0x62);
            menuCloseSync(0x62, 1);

            /* Normalise raw menu result -> loop_state in {0, 1, 2} */
            if (menu_res == -1 || menu_res == 2) {
                loop_state = 2;
            } else if (menu_res == 0) {
                loop_state = 0;
            } else {
                loop_state = 1;
            }

        } while (loop_state != 2);

        /* On confirmed exit, push to format-text-3 and jump to common tail */
        fn_8002A2CC((u8 *)(u32)arg0, 2, (s32)-1);
        /* fall through to common tail */

    /* ================================================================
     * PATH B: type_byte == 1 -- direct hand-off to ProcessChoice
     * ================================================================ */
    } else if (type_byte == 1) {

        fn_8002CE6C(arg0, (u32)type_byte);
        /* fall through to common tail */

    /* ================================================================
     * PATH C: type_byte >= 2 -- location-arrival dialog (menu 0x83)
     * ================================================================ */
    } else {

        /* Allocate scratch GSmem block (0x7198 bytes) */
        mem_handle = fn_800E3534(0x7198);
        lbl_8047A3DC = (u32)fn_800E27B0(mem_handle);

        /* Format the initial "arrival at" text line */
        {
            u32 text_entry = fn_80029FAC(&text_buf0, (s32)arg0, 0, (s32)-1);
            fn_80106ADC(2, text_entry, 1, 0, text_buf0);
        }

        /* ---- menu-0x83 wait-loop ---- */
        loop_state = 0;
        do {
            /* Wait one frame before re-querying */
            fn_801069FC(1);

            /* Dispatch on the normalised menu result from the previous
             * iteration (first time through we skip straight to the menu
             * open since loop_state==0 and we jump into the loop tail).   */
            if (loop_state == 0) {
                /* Open the main arrival dialog state machine */
                fn_8002C408(arg0, (u32)type_byte);
            } else if (loop_state == 1) {
                /* Show the secondary travel dialog */
                fn_8002C284(arg0, (u32)type_byte);
            } else if (loop_state == 2) {
                /* Open "format text 2" sub-dialog */
                fn_8002A1C4((u8 *)(u32)arg0, 0xb, (s32)-1);
            } else if (loop_state == 3) {
                /* Set the done flag to exit after cleanup */
                /* loop_state==3 means "exit" -- handled after menu query */
            }
            /* loop_state >= 4 or < 0: no-op */

            if (loop_state != 0) {
                /* loop_state already set; skip to menu query */
                goto query_menu_83;
            }

            /* Re-format the arrival text (update pass, field=1) */
            {
                u32 text_entry = fn_80029FAC(&text_buf0, (s32)arg0, 1, (s32)-1);
                fn_80106ADC(2, text_entry, 1, 0, text_buf0);
            }

        query_menu_83:
            /* Query menu 0x83, then unload/close it */
            menu_res = fn_8010264C(0x83, 1);
            fn_80102510(0x83);
            menuCloseSync(0x83, 1);

            /* Normalise raw result -> 0/1/2/3 */
            if (menu_res == 0) {
                menu_res = 0;
            } else if (menu_res == 1) {
                menu_res = 1;
            } else if (menu_res >= 2 && menu_res < 3) {
                menu_res = 2;
            } else {
                menu_res = 3; /* <0 or >=3 */
            }

            loop_state = menu_res;

        } while (loop_state != 3);

        /* Post-loop: close sub-dialogs based on type_byte */
        if (type_byte == 2 || type_byte == 3) {
            fn_801069FC(1);
        } else {
            fn_8002A1C4((u8 *)(u32)arg0, 2, (s32)-1);
        }

        /* Release the GSmem scratch block */
        fn_800E24B0(mem_handle);
        fn_800E209C(mem_handle);
    }

    /* ================================================================
     * COMMON TAIL: if the flag word at lbl_8047A3FC+4 is non-zero,
     * run the global exit sequence.
     * ================================================================ */
    if (lbl_8047A400 != 0) {
        fn_800FF660();
        fn_8011288C(0, 0);
    }
}
#endif

/* fn_8002DC6C - 0x8002DC6C | size: 0xb8 | WALL 71%: fsub/fdiv double vs fsubs/fdivs single + sda21 store */
extern void fn_801D23C0(void);
extern u32 fn_800D37CC(void);
extern void fn_8010206C(f32);
extern void fn_8019075C(s32, s32);
extern void fn_80102038(f32);
extern f64 lbl_8047B998;
extern f32 lbl_8047B9CC;
#if 1
asm void fn_8002DC6C(void) {
#include "src/game/gs_worldmap_fn_8002DC6C.inc"
}
#else
/*
 * fn_8002DC6C  GSmap_SetStoryFlag  0x8002DC6C  size: 0xB8
 *
 * Sets a story-progression flag on the worldmap state, kicks the scene
 * fade/timer system, issues a wait-for-dialog yield, then samples the
 * scene timer a second time to feed the post-yield fade curve.
 *
 * Parameters:
 *   flag  -- story/destination flag value stored to lbl_8047A3FC (r3 -> r31)
 *
 * int-to-float pattern:
 *   xoris r3,r3,0x8000 + lis r0,0x4330 stacked into a f64 then
 *   fsubs lbl_8047B998(r2) bias => plain (f32)(s32)fn_800D37CC()
 */
void fn_8002DC6C(u32 flag)
{
    extern void fn_801D23C0(void);
    extern u32  fn_800D37CC(void);
    extern void fn_8010206C(f32);
    extern void fn_8019075C(s32, s32);
    extern void fn_800FF730(s32);
    extern void fn_8011288C(s32, u32);
    extern void _threadSwitch(void);
    extern void fn_80102038(f32);

    /* lbl_8047A3FC: two consecutive u32 words in SDA (flag word, active word) */
    extern u32 lbl_8047A3FC;

    /* lbl_8047B9CC / lbl_8047B998: r2-relative float/double constants used for
       the int->float bias conversion.  We bypass the bias trick with a direct
       cast - ENDIAN-QA: xoris+0x4330 bias is identical to (f32)(s32)x */
    f32 t;

    fn_801D23C0();

    /* pre-yield timer sample -> fade-in parameter */
    t = (f32)(s32)fn_800D37CC(); /* ENDIAN-QA: lbl_8047B9CC / (bias_cvt(D37CC())) */
    {
        extern f32 lbl_8047B9CC;
        t = lbl_8047B9CC / t;
    }
    fn_8010206C(t);

    /* store flag and mark slot active */
    lbl_8047A3FC = flag;
    *(&lbl_8047A3FC + 1) = 1u;   /* lbl_8047A3FC+4 */

    fn_8019075C(1, 2);
    fn_800FF730(0x38f);
    fn_8011288C(0, 0);
    _threadSwitch();   /* GSthreadYield / vsync yield */

    /* post-yield timer sample -> fade-out parameter */
    t = (f32)(s32)fn_800D37CC();
    {
        extern f32 lbl_8047B9CC;
        t = lbl_8047B9CC / t;
    }
    fn_80102038(t);
}
#endif

/* fn_8002DD24 - 0x8002DD24 | size: 0x1ec */
extern void fn_80089E20(void);
extern void fn_801D055C(void);
extern void fn_801D04D0(void);
extern void fn_80089D98(void);
extern void fn_801D046C(void);
extern void fn_801D04F4(void);
extern void fn_8008ABE4(void);
extern void fn_801D039C(void);
extern void fn_8001D7E4(void);
extern void fn_800E0C04(void);
extern u32 lbl_8047A424;
extern u8 lbl_803A2518[];
extern u32 lbl_8047A420;
extern u32 lbl_8047A40C;
extern u32 lbl_804788B0;
extern u32 lbl_8047A42C;
#if 1
asm void fn_8002DD24(void) {
#include "src/game/gs_worldmap_fn_8002DD24.inc"
}
#else
/*
 * fn_8002DD24  GSmap_CheckStoryState  0x8002DD24 | size: 0x1EC
 *
 * Checks whether a GBA encounter/trade handshake is ready, runs the
 * associated scene UI loop, and either commits the save-data block (fast
 * path) or plays a short delay sequence (slow path / confirmed trade).
 *
 * Parameters
 *   arg  - pointer to a caller-owned save-data buffer that is block-copied
 *           into the game's live save region on the fast path.
 */
void fn_8002DD24(void *arg)
{
    /* ---- block-scope externs (TU convention) ---- */
    extern u8  lbl_803A2518[];          /* base of live save-data array */
    extern u32 lbl_8047A424;            /* slot-A index (u16 used) */
    extern u32 lbl_8047A420;            /* slot-B index (u16 used) */
    extern u32 lbl_8047A40C;            /* encounter flags */
    extern u32 lbl_804788B0;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;            /* worldmap state machine token */

    /* Interaction getter: fn_8012A5B0(u8* base, u32 selector, u32 idx) */
    extern void *fn_8012A5B0(u8 *ptr, u32 selector, u32 idx);

    /* Message/dialog trigger:
       fn_80106D3C(s32 slot, s32 msgId, s32 p3, s32 p4)             */
    extern void fn_80106D3C(s32 slot, s32 msgId, s32 p3, s32 p4);

    /* GBA encounter / state checkers */
    extern s32  fn_80089E20(s32 mode, void *pkm, u32 slotB, u32 flags);
    extern s32  fn_80089D98(s32 slot);

    /* Scene model-pool helpers */
    extern void fn_801D055C(s32 a, s32 b, s32 c); /* batch update / open */
    extern void fn_801D04D0(void);                  /* pool poll – returns u8 */
    extern void fn_801D046C(s32 flag);              /* pool set-count flag  */
    extern void fn_801D04F4(void);                  /* pool event poll – returns s32 result code */
    extern void fn_801D039C(void);                  /* pool step update */

    /* Threading / render helpers */
    extern void _threadSwitch(void);   /* GSthread yield (one frame) */
    extern void fn_8008ABE4(s32 mode, s32 flag); /* abort / cancel scene */

    /* Save-data helpers */
    extern void *fn_80129280(s32 side, s32 slotType); /* get party/save ptr */
    extern void  fn_8001D7E4(void);                    /* save commit flush  */
    extern void  fn_801069FC(s32 slot);                /* save slot finalize */
    extern void  fn_8010A420(u8 *ptr);                 /* archive release    */

    /* Timer helper: fn_800E0C04(s32 frames) -> u32 countdown value */
    extern u32   fn_800E0C04(s32 frames);

    /* ---- locals ---- */
    u8   *save_base;   /* r30: base of live save-data */
    void *pkm_b;       /* r31: Pokémon-B object pointer */
    u8    did_action;  /* r29: flag – GBA action was processed this loop */
    u8    confirmed;   /* r28: flag – trade/save was confirmed (result==4) */
    s32   event_result;/* r31 reused: event poll return value */
    u32   timer;       /* r28 reused: countdown for delay path */

    save_base = lbl_803A2518;

    /* Warm up the interaction table for slot-A (result not used here) */
    fn_8012A5B0(NULL, 3, (u32)(u16)lbl_8047A424);

    /* Get the Pokémon object for slot-B */
    pkm_b = fn_8012A5B0(save_base + 0x170, 3, (u32)(u16)lbl_8047A420);

    did_action = 0;
    confirmed  = 0;

    /* Notify the message system that a new encounter dialog is starting */
    fn_80106D3C(2, 0x44d7, 1, 1);

    /* Check whether the GBA encounter is already ready */
    if (fn_80089E20(2, pkm_b, lbl_8047A420, lbl_8047A40C) != 0) {
        /* Encounter already in progress – jump straight to the path
           decision (same as falling off the loop with did_action==0,
           confirmed==0).  We skip the scene open and loop entirely. */
        goto L_path_decision;
    }

    /* Open the waiting scene */
    fn_801D055C(8, 2, 0);

    /* ---- yield loop: wait for the GBA event result ---- */
L_loop:
    if (!did_action) {
        /* Poll scene pool for a pending entry */
        s32 pool_entry = (u8)((u32(*)(void))fn_801D04D0)();
        if (pool_entry != 0) {
            /* Check GBA slot-2 state */
            s32 gba_state = fn_80089D98(2);
            if (gba_state >= 0) {
                if (gba_state == 0) {
                    fn_801D046C(0);
                } else {
                    fn_801D046C(1);
                }
                did_action = 1;
            }
        }
    }

    /* Yield one frame, then poll the scene for an event result */
    _threadSwitch();
    event_result = ((s32(*)(void))fn_801D04F4)();
    if (event_result == 0) {
        goto L_loop;
    }

    /* Scene is done – tear down and check the result code */
    fn_8008ABE4(2, 1);
    fn_801D039C();

    if (event_result == 4) {
        confirmed = 1;
    }

L_path_decision:
    /* If the GBA action was NOT processed (did_action==0) OR if it was
       processed AND confirmed (did_action!=0 && confirmed!=0): copy save. */
    if (!did_action || confirmed) {
        goto L_save_copy;
    }
    /* did_action && !confirmed → delay path */
    goto L_delay;

L_save_copy:
    {
        /* Block-copy the caller's buffer into the live save region.
         * fn_80129280(0,0) returns a pointer to the destination; the
         * source is the 'arg' parameter passed to this function.
         * The CW loop copies 2 words × 15354 iterations = 122832 bytes.
         * We use a plain memcpy equivalent for x86 semantics.
         */
        u8 *dst = (u8 *)fn_80129280(0, 0);
        u8 *src = (u8 *)arg;
        u32 i;
        for (i = 0; i < 15354; i++) {
            /* Each bdnz iteration copies two words (8 bytes) */
            ((u32 *)dst)[0] = ((u32 *)src)[0];
            ((u32 *)dst)[1] = ((u32 *)src)[1];
            dst += 8;
            src += 8;
        }

        fn_80106D3C(2, 0x44d6, 1, 0);
        fn_8001D7E4();
        fn_801069FC(1);
        fn_8010A420(save_base + 0xd18);
        fn_8010A420(save_base + 0xcd0);
        (*(u8*)&lbl_804788B0) = 0;
        lbl_8047A42C = 0;
        return;
    }

L_delay:
    {
        /* Play a ~60-frame delay sequence then signal a different outcome */
        timer = fn_800E0C04(0x3c);
        while (timer != 0) {
            _threadSwitch();
            timer--;
        }
        fn_80106D3C(2, 0x44d5, 1, 0);
        fn_8001D7E4();
        fn_801069FC(1);
        lbl_8047A42C = 0x13;
    }
}
#endif

/* fn_8002DF10 - 0x8002DF10 | size: 0x35c */
extern void fn_80128A64(void);
extern void fn_801CB9D8(void);
extern void fn_80112260(void);
extern void fn_8012805C(void);
extern void fn_80176E0C(void);
extern void fn_80113F48(void);
extern void fn_801CBA0C(void);
extern void fn_800F9318(void);
extern void GSscene_SetMode(void);
extern void fn_800E4014(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A40C;
extern u32 lbl_8047A420;
extern f32 lbl_8047B9D0;
extern u32 lbl_8047A41C;
extern u32 lbl_8047A418;
extern f32 lbl_8047B9D4;
extern u32 lbl_8047A408;
extern u32 lbl_8047A414;
extern u32 lbl_8047A42C;
#if 1
asm void fn_8002DF10(void) {
#include "src/game/gs_worldmap_fn_8002DF10.inc"
}
#else
/*
 * fn_8002DF10  GSmap_UpdateAvailability  0x8002DF10  size:0x35c
 *
 * No-arg world-map state machine: checks whether the two active NPC slots
 * (lbl_8047A424, lbl_8047A420) are in a valid encounter state, and if so
 * runs the full encounter setup sequence:
 *   1. Fade/audio transition (fn_801C41C8/fn_801C40F0)
 *   2. Hide marker objects (fn_8010A420)
 *   3. Wait for trainer anim (fn_801CB9D8)
 *   4. Yield one frame (_threadSwitch)
 *   5. Spawn/configure encounter objects (fn_8012805C)
 *   6. Re-anchor NPC handles (fn_80109C88, fn_8010A5BC)
 *   7. Scene-load BGM/scene (fn_80176E0C, fn_801CBA0C, fn_800F9318)
 *   8. Restore people state (GSscene_SetMode)
 *   9. Mark availability flag and advance state (lbl_8047A42C = 0x12)
 */
void fn_8002DF10(void)
{
    extern u32   lbl_8047A424;
    extern u32   lbl_8047A420;
    extern u32   lbl_8047A40C;
    extern u8    lbl_803A2518[];
    extern f32   lbl_8047B9D0;
    extern f32   lbl_8047B9D4;
    extern u32 lbl_8047A41C;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A408;  /* canonical; per-site reinterpret cast */
    extern u32   lbl_8047A418;
    extern u32   lbl_8047A414;
    extern u32   lbl_8047A42C;

    /* fn_8012A5B0 - interaction getter:  (u8* base, u32 mode, u16 idx) -> u8* NPC handle */
    extern u8*  fn_8012A5B0(u8 *ptr, u32 selector, u32 idx);
    /* fn_80128A64 - get NPC key/type at location:
         (u8* world, u32 mode, u16 key, u16* key_out, u8* type_out) -> u32 npc_handle (0/0xffff=invalid) */
    extern u32  fn_80128A64(u8 *arg0, u32 arg1, u16 arg2, u16 *arg3, u8 *arg4);
    /* fn_8011F5C8 - get Pokemon location / status field from NPC ptr */
    extern u16  fn_8011F5C8(void *ptr);
    /* fn_801C41C8 - BGM fade  (f32 vol, s32 mode) */
    extern void fn_801C41C8(f32 vol, s32 mode);
    /* fn_801C40F0 - BGM enable  (s32 flag) */
    extern void fn_801C40F0(s32 flag);
    /* fn_80102510 - menu show  (s32 id) */
    extern void fn_80102510(s32 id);
    /* fn_8010A420 - model marker hide  (u8* obj) */
    extern void fn_8010A420(u8 *obj);
    /* fn_801CB9D8 - scene trainer anim state  (u32 handle) */
    extern void fn_801CB9D8(u32 handle);
    /* fn_80112260 - field collision query reset  (s32 flag) */
    extern void fn_80112260(s32 flag);
    /* _threadSwitch - GS vsync yield */
    extern void _threadSwitch(void);
    /* fn_8012805C - encounter trigger dispatcher */
    extern s32 fn_8012805C(u8 *world, u32 npc, u16 key, u8 *type_out, u8 *team, s32 memo, s32 arg6, s32 audio);
    /* fn_8010A5BC - model set bounds (u8* obj, s32 w, s32 h) */
    extern void fn_8010A5BC(u8 *obj, s32 w, s32 h);
    /* fn_80109C88 - model set NPC handle (u8* obj, u8* npc_handle) */
    extern void fn_80109C88(u8 *obj, u8 *npc_handle);
    /* fn_80176E0C - scene render/BGM start (s32 scene_id, u32 color_key, s32 a, s32 b) */
    extern void fn_80176E0C(s32 scene_id, u32 color_key, s32 a, s32 b);
    /* fn_80113F48 - get current scene/resource handle */
    extern u32  fn_80113F48(void);
    /* fn_801CBA0C - scene transition fade-out: (u32 color_key) -> u32 fade_handle */
    extern u32  fn_801CBA0C(u32 color_key);
    /* fn_800F9318 - resolve resource pointer from handle (u32 handle) -> void* */
    extern void* fn_800F9318(u32 handle);
    /* GSscene_SetMode - restore people state (s32 mode) */
    extern void GSscene_SetMode(s32 mode);
    /* fn_800E4014 - enable/disable field object (void* obj, s32 flag) */
    extern void fn_800E4014(void *obj, s32 flag);
    /* fn_8010264C - menu close sync (s32 id, s32 flag) -> s32 */
    extern s32  fn_8010264C(s32 id, s32 flag);

    u8  *base;          /* r31: lbl_803A2518 base pointer */
    u8  *npc_a;         /* r28: NPC handle for slot A (lbl_8047A424) */
    u8  *npc_b;         /* r30: NPC handle for slot B (lbl_8047A420) */
    u8   need_update;   /* r29: set to 1 if either slot is valid */
    u32  scene_handle;  /* r28 reused: result of fn_80113F48 */
    u32  npc_result;    /* return of fn_80128A64 (cast to u16 for validity check) */
    s32  enc_result;    /* return of fn_8012805C */

    /* Stack temporaries for fn_80128A64 and fn_8012805C output buffers */
    u16  key_a;         /* sp+0xc: key output for initial A-slot check */
    u8   type_a;        /* sp+0x18: type output for initial A-slot check */
    u16  key_b;         /* -- sp+0xc reused for B-slot check */
    /* sp+0xa / sp+0x14 for encounter call buffers */
    u16  enc_key_a;     /* sp+0xa */
    u8   enc_type_a;    /* sp+0x14 */
    u16  enc_key_b;     /* sp+0x8 */
    u8   enc_type_b;    /* sp+0x10 */

    base = lbl_803A2518;
    lbl_8047A40C = 0;
    need_update = 0;

    /* Check slot A (lbl_8047A424): get NPC handle at index 3 */
    npc_a = fn_8012A5B0(NULL, 3, (u16)lbl_8047A424);
    /* Check slot B (lbl_8047A420): get NPC handle at index 3 from base+0x170 */
    npc_b = fn_8012A5B0(base + 0x170, 3, (u16)lbl_8047A420);

    /* Check slot A availability */
    npc_result = fn_80128A64(npc_a, 2, 0, &key_a, &type_a);
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        need_update = 1;
    }

    /* Check slot B availability */
    npc_result = fn_80128A64(npc_b, 2, 0, &key_a, &type_a); /* reuses same stack slots */
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        need_update = 1;
        /* Also update lbl_8047A40C from slot B's status field */
        lbl_8047A40C = (u16)fn_8011F5C8(npc_b);
    }

    if (need_update != 1) {
        /* No valid encounters -- just advance state counter */
        lbl_8047A42C = 0x12;
        return;
    }

    /* --- Valid encounter found: run full setup sequence --- */

    (*(u8*)&lbl_8047A41C) = 0;

    fn_801C41C8(lbl_8047B9D0, 3);
    fn_801C40F0(1);

    fn_80102510(0xde);

    fn_8010A420(base + 0xd18);
    fn_8010A420(base + 0xcd0);

    fn_801CB9D8(lbl_8047A418);

    fn_80112260(0);
    _threadSwitch();

    fn_801C41C8(lbl_8047B9D4, 2);
    fn_801C40F0(1);

    /* --- Slot A encounter trigger --- */
    npc_result = fn_80128A64(npc_a, 2, 0, &enc_key_a, &enc_type_a);
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        (*(u8*)&lbl_8047A408) = 1;
        enc_result = fn_8012805C(npc_a, (u32)(u16)npc_result, 0,
                                  &enc_type_a, NULL, 1, 0, 1);
        if (enc_result == 2) {
            (*(u8*)&lbl_8047A408) = 0;
        }
        /* Unconditional clear (compiler-emitted redundant store) */
        (*(u8*)&lbl_8047A408) = 0;
    }

    /* --- Slot B encounter trigger --- */
    npc_result = fn_80128A64(npc_b, 2, 0, &enc_key_b, &enc_type_b);
    if ((u16)npc_result != 0 && (u16)npc_result != 0xffff) {
        (*(u8*)&lbl_8047A408) = 1;
        enc_result = fn_8012805C(npc_b, (u32)(u16)npc_result, 0,
                                  &enc_type_b, NULL, 0, 0, 1);
        if (enc_result == 2) {
            (*(u8*)&lbl_8047A408) = 0;
        }
        /* Unconditional clear */
        (*(u8*)&lbl_8047A408) = 0;
    }

    /* --- Post-encounter: fade back in and re-anchor scene objects --- */

    fn_801C41C8(lbl_8047B9D4, 3);
    fn_801C40F0(1);

    /* Refresh NPC handles after encounter (slots may have changed) */
    npc_b = fn_8012A5B0(NULL, 3, (u16)lbl_8047A424);          /* r30 */
    /* FUNCTIONAL-TODO: asm re-uses r29 for slot-B after refresh */
    {
        u8 *npc_b2 = fn_8012A5B0(base + 0x170, 3, (u16)lbl_8047A420); /* r29 */

        fn_8010A5BC(base + 0xd18, 0xe8, 0x11c);
        fn_8010A5BC(base + 0xcd0, 0xe8, 0x11c);

        fn_80109C88(base + 0xd18, npc_b);
        fn_80109C88(base + 0xcd0, npc_b2);
    }

    fn_80176E0C(0x37c, 0x0fff1800, 0, 1);

    scene_handle = fn_80113F48();
    lbl_8047A418 = fn_801CBA0C(0x0ffe1000);
    lbl_8047A414 = (u32)fn_800F9318(scene_handle);

    fn_80176E0C(0x37c, 0x0fff1800, 0, 1);

    GSscene_SetMode(4);

    fn_800E4014((void*)lbl_8047A414, 1);

    fn_80112260(0);

    fn_8010264C(0xde, 1);

    fn_801C41C8(lbl_8047B9D0, 2);
    fn_801C40F0(1);

    (*(u8*)&lbl_8047A41C) = 1;

    /* Fall through to state advance */
    lbl_8047A42C = 0x12;
}
#endif

/* fn_8002E26C - 0x8002E26C | size: 0x1f4 */
extern void fn_80124A60(void);
extern void fn_8011F5FC(void);
extern void fn_801024E8(void);
extern void fn_801CB834(void);
extern void fn_80176B48(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern u32 lbl_8047A414;
extern u32 lbl_8047A41C;
extern f32 lbl_8047B9D0;
extern f32 lbl_8047B9D8;
extern u32 lbl_8047A42C;
#if 1
asm void fn_8002E26C(void) {
#include "src/game/gs_worldmap_fn_8002E26C.inc"
}
#else
/*
 * fn_8002E26C - GSmap_RefreshDisplay
 * 0x8002E26C | size: 0x1f4
 *
 * Refreshes the world-map display: sets up interaction objects, plays UI
 * sounds, initiates camera animations, shows/hides the map UI layer, loads
 * the trainer model, and writes the next state-machine step into lbl_8047A42C.
 *
 * No parameters (no r3..r10 reads before first write).
 */
void fn_8002E26C(void)
{
    /* --- block-scope externs (TU convention) --- */
    extern u8  lbl_803A2518[];            /* worldmap context base (BSS)          */
    extern u32 lbl_8047A424;              /* slot/index A (r13-relative)           */
    extern u32 lbl_8047A420;              /* slot/index B (r13-relative)           */
    extern u32 lbl_8047A414;             /* render handle (r13-relative)          */
    extern u32 lbl_8047A41C;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;             /* state-machine step (r13-relative)     */
    extern f32 lbl_8047B9D0;             /* camera duration constant 0 (r2-rel)   */
    extern f32 lbl_8047B9D8;             /* camera duration constant 1 (r2-rel)   */

    /* interaction/sound helpers */
    extern void fn_80124A60(u8 *ctx);
    extern void fn_8011F5FC(void *a, void *b);
    extern void fn_80102510(s32 sound_id);
    extern void fn_8010A420(void *widget);
    extern void fn_800E4014(u32 handle, s32 flag);
    extern void fn_801C41C8(f32 duration, s32 mode);
    extern void fn_801C40F0(s32 flag);
    extern void fn_801024E8(s32 arg);
    extern u32  fn_80113F48(void);
    extern void fn_80176E0C(s32 handle, u32 flags, s32 a3, s32 a4);
    extern void _threadSwitch(void);
    extern void fn_80166AB8(s32 a1, s32 a2, s32 a3);
    extern void fn_80112260(s32 flag);
    extern void fn_801CB834(u32 a1, s32 a2, s32 a3, s32 a4);
    extern void cameraWaitSyncAnime(s32 arg);
    extern void fn_8010A5BC(void *widget, s32 x, s32 y);
    extern void fn_80109C88(void *widget, void *obj);
    extern void fn_8010264C(s32 id, s32 flag);
    extern u32  fn_8012A5B0(u8 *ptr, u32 selector, u32 idx);

    u8  *base  = lbl_803A2518;
    void *obj_a;   /* r30: result of first  fn_8012A5B0 */
    void *obj_b;   /* r29: result of second fn_8012A5B0 */
    u32   handle;  /* r3 after fn_80113F48              */

    /* --- Initialise interaction objects --- */
    fn_80124A60(base);

    obj_a = (void *)fn_8012A5B0((u8 *)0, 3, (u16)lbl_8047A424);
    obj_b = (void *)fn_8012A5B0(base + 0x170, 3, (u16)lbl_8047A420);

    /* Cross-link the three objects */
    fn_8011F5FC(base,  obj_b);
    fn_8011F5FC(obj_b, obj_a);
    fn_8011F5FC(obj_a, base);

    /* --- Play worldmap entry sound, hide UI widgets --- */
    fn_80102510(0xde);
    fn_8010A420(base + 0xd18);
    fn_8010A420(base + 0xcd0);

    /* --- Clear display-active flag, disable render handle, start camera --- */
    (*(u8*)&lbl_8047A41C) = 0;
    fn_800E4014(lbl_8047A414, 0);

    fn_801C41C8(lbl_8047B9D0, 3);
    fn_801C40F0(1);
    fn_801024E8(1);

    /* --- Set up field model and audio, yield one VBlank --- */
    handle = fn_80113F48();
    fn_80176E0C((s32)handle, 0x10b71800, 0, 0);

    _threadSwitch();   /* vsync yield */

    fn_80166AB8(0x4c8, 0, 0);

    /* --- Show UI layer, load trainer model --- */
    fn_80112260(1);
    fn_801CB834(0x10b11000, 0, 0, 0);

    /* --- Second camera fade, wait for sync, third camera fade --- */
    fn_801C41C8(lbl_8047B9D0, 2);
    fn_801C40F0(1);
    cameraWaitSyncAnime(1);

    fn_801C41C8(lbl_8047B9D8, 3);
    fn_801C40F0(1);
    fn_80112260(0);

    /* --- Position and bind the UI widget slots --- */
    fn_8010A5BC(base + 0xd18, 0xe8, 0x11c);
    fn_8010A5BC(base + 0xcd0, 0xe8, 0x11c);

    fn_80109C88(base + 0xd18, obj_a);
    fn_80109C88(base + 0xcd0, obj_b);

    /* --- Re-enable sound and render handle --- */
    fn_80176E0C(0x37c, 0x0fff1800, 0, 1);

    fn_800E4014(lbl_8047A414, 1);
    fn_8010264C(0xde, 1);

    /* --- Final camera fade and set display-active --- */
    (*(u8*)&lbl_8047A41C) = 1;

    fn_801C41C8(lbl_8047B9D8, 2);
    fn_801C40F0(1);

    /* --- Advance state machine --- */
    lbl_8047A42C = 0x10;
}
#endif

/* fn_8002E460 - 0x8002E460 | size: 0x5fc */
extern void fn_80104704(void);
extern void fn_801046C8(void);
extern void fn_80109220(void);
extern void fn_80073A44(void);
extern void fn_8017B1AC(void);
extern u32 lbl_8047A424;
extern u32 lbl_8047A420;
extern f32 lbl_8047B9D0;
extern u32 lbl_8047A42C;
extern u32 lbl_8047A428;
extern u32 lbl_8047A410;
#if 1
asm void fn_8002E460(void) {
#include "src/game/gs_worldmap_fn_8002E460.inc"
}
#else
/*
 * fn_8002E460 / GSmap_DrawFullFrame (0x8002E460, size 0x5FC)
 *
 * Two-Pokemon "swap / register" worldmap sub-flow. Resolves two storage-box
 * Pokemon handles from the current map-state selectors (lbl_8047A424 / A420),
 * binds them into two list widgets (state+0xD18, state+0xCD0), primes a fixed
 * set of UI menu fields (object 0xDB), then runs a frame-driven loop that polls
 * directional input (fn_80073A44) and a status query (fn_8017B1AC). Each tick it
 * toggles two confirmation flags (r30/r29) on newly-pressed input bits, plays a
 * select/cancel SE, and refreshes the menu fields. The loop ends when both flags
 * are set (commit), when a cancel bit is hit (abort), or when the input query
 * returns a hard-cancel (status==0 path). On commit it tears down the widgets,
 * deep-copies the staged record returned by fn_80129280(0,0) into the caller's
 * mapCtx buffer (0x3BFA word-pairs = 0x1DFD0 bytes), advances the map state
 * machine to 0xD, and re-shows the dialog camera. On abort it resets the three
 * selector globals to -1 and advances to state 2.
 *
 * Real param: r3 (=r27) is the caller's destination record buffer (mapCtx),
 * passed from the dispatcher in gs_npc_event.c (return of fn_801D036C).
 */
void fn_8002E460(void* mapCtx)
{
    extern u8  lbl_803A2518[];          /* worldmap UI/state block base */
    extern u32 lbl_8047A424;            /* selector A (16-bit index in low half) */
    extern u32 lbl_8047A420;            /* selector B (16-bit index in low half) */
    extern u32 lbl_8047A428;            /* selector C (reset on abort) */
    extern u32 lbl_8047A42C;            /* map state-machine state */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern f32 lbl_8047B9D0;            /* camera transition param (0.0f) */

    extern u8*  fn_8012A5B0(void* obj, u32 selector, u32 idx);   /* interaction getter -> handle */
    extern void fn_8010A5BC(void* widget, s32 a, s32 b);         /* list-widget init */
    extern void fn_80109C88(void* widget, void* item);          /* bind item to widget */
    extern void fn_8010A420(void* widget);                      /* destroy widget */
    extern void fn_8010264C(s32 menuId, s32 flag);              /* menu open */
    extern void fn_80102510(s32 menuId);                        /* menu close */
    extern u8*  fn_80104704(s32 menuId);                        /* select menu object */
    extern u8*  fn_801046C8(void* menuObj, s32 fieldId);        /* select field -> handle */
    extern void fn_80109220(void* fieldHandle, s32 value);     /* set field value */
    extern void fn_801C41C8(f32 value, s32 mode);               /* camera anim init */
    extern void fn_801C40F0(s32 flag);                          /* camera anim start */
    extern u8*  fn_80105624(void);                              /* current input/state base */
    extern s32  fn_80073A44(s32 mode, u16* outFlags);          /* poll input -> status, writes flags */
    extern u32  fn_8017B1AC(void);                              /* input-mode status query */
    extern void fn_80166AB8(s32 soundId, s32 p2, s32 p3);       /* play SE */
    extern void fn_80106D3C(s32 a, s32 b, s32 c, s32 d);        /* dialog/sound event */
    extern void _threadSwitch(void);                             /* vsync / scheduler yield */
    extern void* fn_80129280(s32 a, s32 sel);                  /* staged-record getter */

    u8* state = lbl_803A2518;
    u8* handleA;
    u8* handleB;
    s32 doneA;          /* r30: first selection committed */
    s32 doneB;          /* r29: second selection committed */
    u16 prevFlags;      /* r28: previous raw input-flag word */
    u16 rawFlags;       /* r26: per-tick raw flags from fn_80105624 */
    u16 newPress;       /* r25: newly set bits (rawFlags & ~prevFlags) */
    u16 pollFlags;      /* sp+0x8: flag word written by fn_80073A44 */
    s32 status;
    s32 i;

    /* field-id groups refreshed every tick: index 0 carries the flag itself,
       the remaining four carry its logical negation (lit/unlit state). */
    static const s32 idsA[5] = { 0x11A8, 0x0F9B, 0x0F9A, 0x0FA3, 0x0FA5 };
    static const s32 idsB[5] = { 0x11A9, 0x0F99, 0x0F98, 0x0FA4, 0x0FA6 };

    handleA = fn_8012A5B0((void*)0, 3, lbl_8047A424 & 0xFFFF);
    handleB = fn_8012A5B0(state + 0x170, 3, lbl_8047A420 & 0xFFFF);

    fn_8010A5BC(state + 0xD18, 0xE4, 0x8F);
    fn_8010A5BC(state + 0xCD0, 0xE4, 0x8F);
    fn_80109C88(state + 0xD18, handleA);
    fn_80109C88(state + 0xCD0, handleB);

    fn_8010264C(0xDB, 0);

    /* initial menu-field priming: first of each group = 0, the rest = 1 */
    {
        u8* mo; u8* fh;
        mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsA[0]); fn_80109220(fh, 0);
        for (i = 1; i < 5; i++) { mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsA[i]); fn_80109220(fh, 1); }
        mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsB[0]); fn_80109220(fh, 0);
        for (i = 1; i < 5; i++) { mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsB[i]); fn_80109220(fh, 1); }
    }

    fn_801C41C8(lbl_8047B9D0, 2);
    fn_801C40F0(1);

    doneA = 0;
    doneB = 0;
    prevFlags = 0;

    /* main confirmation loop: runs while NOT (doneA && doneB) */
    for (;;) {
        s32 abortLoop = 0;

        if ((doneA & 0xFF) != 0 && (doneB & 0xFF) != 0) {
            break;  /* both confirmed -> finalize */
        }

        /* ---- loop body (L_8002E604) ---- */
        rawFlags = *(u16*)(fn_80105624() + 0x4);

        status = fn_80073A44(1, &pollFlags);
        if (status != 0) {
            /* hard cancel via input system */
            fn_80106D3C(2, 0x4448, 1, 0);
            fn_80102510(0xDB);
            fn_8010A420(state + 0xD18);
            fn_8010A420(state + 0xCD0);
            lbl_8047A42C = 0;
            return;
        }

        if (fn_8017B1AC() != 5) {
            newPress = (u16)(pollFlags & ~prevFlags);
            prevFlags = pollFlags;

            if ((rawFlags & 0x10) != 0) {
                /* select bit for slot A */
                if ((doneA & 0xFF) == 0) {
                    fn_80166AB8(0x24, 0, 0);
                }
                doneA = 1;
            } else if ((rawFlags & 0x20) != 0) {
                /* cancel bit for slot A */
                fn_80166AB8(0x25, 0, 0);
                if ((doneA & 0xFF) == 0) {
                    doneA = 0;
                    abortLoop = 1;   /* L_8002E6D4 -> finalize */
                } else {
                    doneA = 0;
                }
            }

            if (!abortLoop) {
                /* L_8002E6EC: react to newly pressed bits for slot B */
                if ((newPress & 0x1) != 0) {
                    if ((doneB & 0xFF) == 0) {
                        fn_80166AB8(0x24, 0, 0);
                    }
                    doneB = 1;
                } else if ((newPress & 0x2) != 0) {
                    fn_80166AB8(0x25, 0, 0);
                    if ((doneB & 0xFF) == 0) {
                        doneB = 0;
                        abortLoop = 1;   /* L_8002E74C -> finalize */
                    } else {
                        doneB = 0;
                    }
                }
            }
        }

        if (abortLoop) {
            break;  /* jump straight to finalize block (L_8002E888) */
        }

        /* ---- L_8002E764: vsync then refresh menu fields with live flags ---- */
        _threadSwitch();
        {
            u8* mo; u8* fh;
            s32 notA = ((doneA & 0xFF) == 0) ? 1 : 0;
            s32 notB = ((doneB & 0xFF) == 0) ? 1 : 0;
            mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsA[0]); fn_80109220(fh, doneA);
            for (i = 1; i < 5; i++) { mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsA[i]); fn_80109220(fh, notA); }
            mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsB[0]); fn_80109220(fh, doneB);
            for (i = 1; i < 5; i++) { mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsB[i]); fn_80109220(fh, notB); }
        }
    }

    /* ---- finalize (L_8002E888): one last field refresh ---- */
    {
        u8* mo; u8* fh;
        s32 notA = ((doneA & 0xFF) == 0) ? 1 : 0;
        s32 notB = ((doneB & 0xFF) == 0) ? 1 : 0;
        mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsA[0]); fn_80109220(fh, doneA);
        for (i = 1; i < 5; i++) { mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsA[i]); fn_80109220(fh, notA); }
        mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsB[0]); fn_80109220(fh, doneB);
        for (i = 1; i < 5; i++) { mo = fn_80104704(0xDB); fh = fn_801046C8(mo, idsB[i]); fn_80109220(fh, notB); }
    }

    if ((doneA & 0xFF) != 0 && (doneB & 0xFF) != 0) {
        /* ---- commit path (L_8002E9E4) ---- */
        u32* src;
        u32* dst;
        u32 lo;
        u32 hi;

        fn_8010A420(state + 0xD18);
        fn_8010A420(state + 0xCD0);

        src = (u32*)fn_80129280(0, 0);
        dst = (u32*)mapCtx;
        /* deep-copy 0x3BFA word-pairs (0x1DFD0 bytes) from staged record into caller buffer */
        for (i = 0; i < 0x3BFA; i++) {
            lo = src[0];
            hi = src[1];
            dst[0] = lo;
            dst[1] = hi;
            src += 2;
            dst += 2;
        }

        lbl_8047A42C = 0xD;
        fn_801C41C8(lbl_8047B9D0, 3);
        fn_801C40F0(1);
        fn_80102510(0xDB);
    } else {
        /* ---- abort path (L_8002E9A8) ---- */
        fn_80102510(0xDB);
        lbl_8047A428 = (u32)-1;
        lbl_8047A424 = (u32)-1;
        lbl_8047A420 = (u32)-1;
        fn_8010A420(state + 0xD18);
        fn_8010A420(state + 0xCD0);
        (*(u8*)&lbl_8047A410) = 1;
        lbl_8047A42C = 2;
    }
}
#endif

/* fn_8002EA5C - 0x8002EA5C | size: 0x418 */
extern void fn_8011F1A0(void);
extern void fn_80144064(void);
extern void fn_801021F8(void);
extern void fn_8012AC08(void);
extern void fn_8011E850(void);
extern void fn_80075FEC(void);
extern void fn_8011E8DC(void);
extern void fn_8012640C(void);
extern u32 lbl_8047A428;
extern u8 lbl_803A2688[];
extern f32 lbl_8047B9D4;
extern f64 lbl_8047B9E0;
extern f64 lbl_8047B9E8;
extern f32 lbl_8047B9DC;
extern u32 lbl_8047A42C;
extern u32 lbl_8047A420;
extern f32 lbl_8047B9D0;
#if 1
asm void fn_8002EA5C(void) {
#include "src/game/gs_worldmap_fn_8002EA5C.inc"
}
#else
/*
 * fn_8002EA5C  (GSmap_HandleSceneChange) - 0x8002EA5C, size 0x418
 *
 * Worldmap scene-change handler. Takes no parameters (reads the small-data
 * global lbl_8047A428 = the current/selected map id before touching any
 * argument register). Returns void.
 *
 * Flow:
 *   1. Query the selected map entry; gate on availability (fn_8011F1A0 +
 *      fn_80144064). If unavailable, run the "blocked" timeout: flash the
 *      0xD9 list element (marker 0x43DD), spin a frame-wait, clear it, then
 *      set state lbl_8047A42C = 7 and bail.
 *   2. Otherwise scan party-member slots 0..5 (skipping the active map slot)
 *      for a member that satisfies the carry/usable predicate chain and owns
 *      species property 0x83. If none qualifies, run the "no-match" timeout
 *      (identical shape, marker 0x44E8) and set state 7.
 *   3. If a member qualifies, commit the destination (lbl_8047A420 =
 *      lbl_8047A428), run the transition frame-wait, kick the field
 *      camera/fade (fn_801C41C8/fn_801C40F0) and sound (fn_80102510), and
 *      advance state lbl_8047A42C = 0xC.
 *
 * The triple frame-wait loop is the engine's time-integration idiom:
 *   accum += (f32)fn_800D3088() / (f32)(s32)fn_800D37CC()  per vsync yield
 * (the 0x43300000 double-word int->float magic in the asm is normalized to
 * plain casts here). fn_800D3088 = elapsed frame ticks (unsigned numerator),
 * fn_800D37CC = ticks-per-unit (signed denominator).
 */
void fn_8002EA5C(void)
{
    /* small-data globals */
    extern u32 lbl_8047A428;     /* selected/source map id            */
    extern u32 lbl_8047A420;     /* committed destination map id      */
    extern u32 lbl_8047A42C;     /* worldmap state machine selector   */
    extern u8  lbl_803A2688[];   /* worldmap object/context base      */
    /* read-only float consts (r2-relative) */
    extern f32 lbl_8047B9D4;     /* frame-wait accumulator init (0.0) */
    extern f32 lbl_8047B9DC;     /* frame-wait limit, timeout loops   */
    extern f32 lbl_8047B9D0;     /* frame-wait limit, transition loop */

    /* cross-TU callees (block-scope typed externs, TU convention) */
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx);   /* interaction getter   */
    extern u16  fn_8011F1A0(u8* ptr);                          /* map availability     */
    extern void fn_801440A0(void);                             /* effect/handle helper */
    extern u8   fn_80144064(void);                             /* gate result          */
    extern void fn_801021F8(u32 id, u32 flag);                 /* list show/hide       */
    extern s32  fn_8011F4F0(u32 ref);                          /* get species          */
    extern void fn_80132A38(u32 prop, u32 value);              /* set display property */
    extern u8*  fn_80104704(s32 key);                          /* find list/window     */
    extern u8*  fn_801046C8(u8* head, s32 key);                /* find child element   */
    extern void fn_80109220(u8* elem, u32 flag);              /* set element flag      */
    extern void fn_80166AB8(s32 soundId, s32 p2, s32 p3);     /* play SE              */
    extern void _threadSwitch(void);                             /* vsync yield          */
    extern u32  fn_800D3088(void);                             /* elapsed frame ticks  */
    extern s32  fn_800D37CC(void);                             /* ticks per unit       */
    extern u8*  fn_8012AC08(u8* base, u32 idx);                /* party slot getter    */
    extern u8   fn_8011E850(u8* obj);                          /* slot predicate A     */
    extern u8   fn_80123FBC(u8* obj);                          /* slot predicate B     */
    extern u8   fn_80075FEC(u8* obj);                          /* slot predicate C     */
    extern u8   fn_8011E8DC(u8* obj);                          /* slot predicate D     */
    extern u32  fn_8012640C(u8* obj, u32 id, u32 selector, u32 d); /* property getter  */
    extern void fn_801C41C8(f32 target, s32 mode);            /* camera/fade target   */
    extern void fn_801C40F0(s32 flag);                        /* camera/fade enable   */
    extern void fn_80102510(u32 id);                          /* sound/window kick    */

    u8*  mapRef;
    u8*  window;
    u8*  element;
    f32  accum;
    f32  num;
    f32  den;
    u32  i;
    u32  mapId;
    u8   available;
    u8   found;

    mapId  = lbl_8047A428;
    mapRef = (u8*)(u32)fn_8012A5B0(lbl_803A2688, 3, (u16)mapId);

    if ((u16)fn_8011F1A0((u8*)mapRef) != 0) {
        fn_801440A0();
        available = fn_80144064();
    } else {
        available = 1;
    }

    if ((u8)available == 0) {
        /* ---- blocked: map not selectable ---------------------------- */
        fn_801021F8(0xD9, 0);
        fn_80132A38(0x32, (u32)fn_8011F4F0((u32)mapRef));

        window  = fn_80104704(0xD9);
        element = fn_801046C8(window, 0x10B2);
        if (window != 0 && element != 0) {
            fn_80109220(element, 1);
            *(u32*)(element + 0x4C) = 0x43DD;
        }

        fn_80166AB8(0x26, 0, 0);

        accum = lbl_8047B9D4;
        while (accum < lbl_8047B9DC) {
            _threadSwitch();
            den = (f32)(s32)fn_800D37CC();
            num = (f32)(u32)fn_800D3088();
            accum = accum + num / den;
        }

        window  = fn_80104704(0xD9);
        element = fn_801046C8(window, 0x10B2);
        if (window != 0 && element != 0) {
            *(u32*)(element + 0x4C) = 0;
            fn_80109220(element, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 7;
        return;
    }

    /* ---- map selectable: scan party slots for an eligible member ---- */
    found = 0;
    for (i = 0; (u16)i < 6; i++) {
        u8* slot;

        if ((u16)i == (u16)mapId) {
            continue;
        }
        slot = fn_8012AC08(lbl_803A2688, i);
        if ((u8)fn_8011E850(slot) != 0) {
            continue;
        }
        if ((u8)fn_80123FBC(slot) == 0) {
            continue;
        }
        if ((u8)fn_80075FEC(slot) != 1) {
            continue;
        }
        if ((u8)fn_8011E8DC(slot) != 0) {
            continue;
        }
        if ((u16)fn_8012640C(slot, 0, 0x83, 0) == 0) {
            continue;
        }
        found = 1;
    }

    if ((u8)found == 0) {
        /* ---- no eligible member: timeout (marker 0x44E8) ------------ */
        fn_801021F8(0xD9, 0);
        fn_80132A38(0x32, (u32)fn_8011F4F0((u32)mapRef));

        window  = fn_80104704(0xD9);
        element = fn_801046C8(window, 0x10B2);
        if (window != 0 && element != 0) {
            fn_80109220(element, 1);
            *(u32*)(element + 0x4C) = 0x44E8;
        }

        fn_80166AB8(0x26, 0, 0);

        accum = lbl_8047B9D4;
        while (accum < lbl_8047B9DC) {
            _threadSwitch();
            den = (f32)(s32)fn_800D37CC();
            num = (f32)(u32)fn_800D3088();
            accum = accum + num / den;
        }

        window  = fn_80104704(0xD9);
        element = fn_801046C8(window, 0x10B2);
        if (window != 0 && element != 0) {
            *(u32*)(element + 0x4C) = 0;
            fn_80109220(element, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 7;
        return;
    }

    /* ---- success: commit destination and begin transition ---------- */
    lbl_8047A420 = lbl_8047A428;

    accum = lbl_8047B9D4;
    while (accum < lbl_8047B9D0) {
        _threadSwitch();
        den = (f32)(s32)fn_800D37CC();
        num = (f32)(u32)fn_800D3088();
        accum = accum + num / den;
    }

    fn_801C41C8(lbl_8047B9D0, 3);
    fn_801C40F0(1);
    fn_80102510(0xD9);
    lbl_8047A42C = 0xC;
}
#endif

/* fn_8002EE74 - 0x8002EE74 | size: 0x410 */
extern void fn_80103CC0(void);
extern void fn_801045A8(void);
extern void fn_801043A4(void);
extern void fn_801023E4(void);
extern void fn_80102004(void);
extern u32 lbl_8047A428;
extern f32 lbl_8047B9D4;
extern f64 lbl_8047B9E0;
extern f64 lbl_8047B9E8;
extern f32 lbl_8047B9DC;
extern u32 lbl_8047A42C;
#if 1
asm void fn_8002EE74(void) {
#include "src/game/gs_worldmap_fn_8002EE74.inc"
}
#else
/* fn_8002EE74 - GSmap_TransitionToScene (0x8002EE74, size 0x410)
 *
 * Worldmap scene-transition driver. Looks up the current map object via the
 * interaction getter, then takes one of three paths:
 *   (A) object is present + entering-state==1 + flag set  -> play a "depart"
 *       SE node animation (anim id 0x43e1) on scene-model 0xd9, frame-pace a
 *       short delay via the host timer, then stop the anim; next state = 7.
 *   (B) object's alt-state flag set -> same as (A) but anim id 0x44be; state=7.
 *   (C) otherwise -> normal scene swap: pump model 0xe3, kick anims 0x43e4/
 *       0x43e5, query the destination slot/branch result and dispatch the next
 *       worldmap state (0xa/0xb/7) or, on the "joint-count==1" special case,
 *       fire effect 0x4448 and go to state 0.
 *
 * EABI: void(void); all inputs come from r13-relative SDA state globals.
 * The 0x4330_0000 double-word int->float magic in the timer loops is
 * normalized to plain (f64)(s32) casts.  *  ENDIAN-QA * 
 */
void fn_8002EE74(void)
{
    /* --- cross-TU callee decls (block-scope, TU convention) --- */
    extern u32   fn_8012A5B0(u8* ptr, u32 selector, u32 idx);   /* interaction getter */
    extern void  fn_801021F8(void* p, u32 val);                 /* enable/disable node subtree */
    extern u8    fn_80123FBC(void* obj);
    extern u8    fn_80075FEC(void* obj);
    extern u8    fn_8011E8DC(void* obj);
    extern u8    fn_8011E850(void* obj);
    extern void* fn_80104704(s32 key);                          /* scene node by slot id */
    extern void* fn_801046C8(void* head, s32 subkey);           /* child node by sub-key */
    extern void  fn_80109220(void* node, u32 enable);           /* enable/disable a node */
    extern void  fn_80166AB8(s32 soundId, s32 p2, s32 p3);      /* play SE */
    extern void  _threadSwitch(void);                             /* host vsync yield (GSthreadYield) */
    extern s32   fn_800D37CC(void);                             /* timer read A */
    extern u32   fn_800D3088(void);                             /* timer read B (tick) */
    extern void  fn_80103CC0(s32 mode);
    extern void  fn_801026A4(void* p, u32 r4, s32 r5, s32 r6, void* r7, s32 r8, ...);
    extern u32   fn_801046B8(void);
    extern void  fn_801045A8(void* p, u8 flags);
    extern s32   fn_801043A4(s32 key);
    extern s32   fn_801023E4(void* p);
    extern void  fn_80102510(s32 p);
    extern u32   fn_80102004(void);
    extern void  fn_80106D3C(s32 a, s32 b, s32 c, s32 d);

    /* --- SDA / sdata2 data (block-scope typed externs) --- */
    extern u32 lbl_8047A428;     /* current map index / state */
    extern u32 lbl_8047A42C;     /* next worldmap state machine value */
    extern u8  lbl_803A2688[];   /* interaction context base */
    extern f32 lbl_8047B9D4;     /* timer accumulator start value */
    extern f32 lbl_8047B9DC;     /* timer accumulator threshold */
    extern f64 lbl_8047B9E0;     /* timer-A baseline offset */
    extern f64 lbl_8047B9E8;     /* timer-B baseline offset */

    void* obj;
    void* node;
    void* child;
    f32   acc;

    obj = (void*)fn_8012A5B0(lbl_803A2688, 3, (u16)lbl_8047A428);
    fn_801021F8((void*)0xd9, 0);

    if (fn_80123FBC(obj) != 0 &&
        fn_80075FEC(obj) == 1 &&
        fn_8011E8DC(obj) != 0) {
        /* ---- Branch A: depart-animation transition ---- */
        node  = fn_80104704(0xd9);
        child = fn_801046C8(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            fn_80109220(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x43e1;
        }
        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 dtA, dtB;
            _threadSwitch();
            dtA = (f64)(s32)fn_800D37CC() - lbl_8047B9E0;   /* ENDIAN-QA */
            dtB = (f64)(s32)fn_800D3088() - lbl_8047B9E8;   /* ENDIAN-QA */
            acc = acc + (f32)(dtB / dtA);
        }

        node  = fn_80104704(0xd9);
        child = fn_801046C8(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            *(u32*)((u8*)child + 0x4c) = 0;
            fn_80109220(child, 0);
        }
        lbl_8047A42C = 7;
        return;
    }

    if (fn_8011E850(obj) != 0) {
        /* ---- Branch B: alternate depart-animation transition ---- */
        node  = fn_80104704(0xd9);
        child = fn_801046C8(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            fn_80109220(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x44be;
        }
        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 dtA, dtB;
            _threadSwitch();
            dtA = (f64)(s32)fn_800D37CC() - lbl_8047B9E0;   /* ENDIAN-QA */
            dtB = (f64)(s32)fn_800D3088() - lbl_8047B9E8;   /* ENDIAN-QA */
            acc = acc + (f32)(dtB / dtA);
        }

        node  = fn_80104704(0xd9);
        child = fn_801046C8(node, 0x10b2);
        if (node != (void*)0 && child != (void*)0) {
            *(u32*)((u8*)child + 0x4c) = 0;
            fn_80109220(child, 0);
        }
        lbl_8047A42C = 7;
        return;
    }

    /* ---- Branch C: normal destination swap ---- */
    {
        s32 branchResult;
        s32 destResult;
        s32 flag = 1;

        fn_80103CC0(2);
        fn_801026A4((void*)0xe3, fn_801046B8(), (s32)&flag, 0, (void*)0, 0);

        node  = fn_80104704(0xe3);
        child = fn_801046C8(node, 0x102a);
        if (node != (void*)0 && child != (void*)0) {
            fn_80109220(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x43e4;
        }

        node  = fn_80104704(0xe3);
        child = fn_801046C8(node, 0x1029);
        if (node != (void*)0 && child != (void*)0) {
            fn_80109220(child, 1);
            *(u32*)((u8*)child + 0x4c) = 0x43e5;
        }

        fn_801045A8((void*)0xe3, 1);
        branchResult = fn_801043A4(0xe3);
        destResult   = fn_801023E4((void*)0xe3);
        fn_80102510(0xe3);
        if (branchResult == -1) {
            destResult = -1;
        }

        fn_80103CC0(1);
        if ((s32)fn_80102004() == 1) {
            fn_80106D3C(2, 0x4448, 1, 0);
            fn_80102510(0xd9);
            lbl_8047A42C = 0;
            return;
        }

        if (destResult == 0) {
            lbl_8047A42C = 0xb;
        } else if (destResult > 0) {
            if (destResult < 2) {       /* destResult == 1 */
                lbl_8047A42C = 0xa;
            }
            /* destResult >= 2: leave state unchanged */
        } else {                        /* destResult < 0 */
            if (destResult == -1) {
                lbl_8047A42C = 7;
            }
            /* destResult < -1: leave state unchanged */
        }
    }
}
#endif

/* fn_8002F284 - 0x8002F284 | size: 0x518 */
extern void fn_8005D8F8(void);
extern void fn_80102138(void);
extern void fn_801022B8(void);
extern u32 lbl_8047A410;
extern u32 lbl_8047A42C;
extern const u8 lbl_80266E90[];
extern u32 lbl_8047A428;
#if 1
#pragma peephole off
void fn_8002F284(void)
{
    /* --- UI item-enable dispatcher: fn_8005D8F8(u32 elementId, u32 val) --- */
    extern void fn_8005D8F8(u32 id, u32 val);
    /* --- party-collection accessor + per-member predicates --- */
    extern void* fn_8012AC08(u8* base, u16 idx);   /* idx-th party member object */
    extern u32   fn_8011E850(u8* mon);             /* eligibility predicate A */
    extern u32   fn_80123FBC(u8* mon);             /* eligibility predicate B */
    extern u32   fn_80075FEC(u8* mon);             /* global-state gate (==1) */
    /* --- scene/object (id 0xD9) management (gs_model.c family) --- */
    extern s32   fn_801023E4(void* p);             /* present? (>=0) / -1 absent */
    extern s32   fn_80102138(void* p, u32 param);  /* lazy load -> handle/result */
    extern void  fn_801021F8(void* p, u32 val);    /* set visibility on subtree */
    extern u8    fn_80103CC0(u8 mode);             /* push render mode, ret old */
    extern u32   fn_801046B8(void);                /* current context handle */
    extern void  fn_801026A4(void* p, u32 a, ...); /* submit/build */
    extern void* fn_80104704(s32 p);               /* resolve node by id */
    extern void* fn_801046C8(void* head, s32 key); /* find child node by key */
    extern void  fn_80109220(void* node, u32 enable); /* enable flag on node */
    extern void  fn_801045A8(void* p, u8 flags);   /* show/commit object */
    extern s32   fn_80102004(void);                /* arrival/joint-count query */
    extern void  fn_80106D3C(s32 a, s32 b, s32 c, s32 d); /* trigger arrival fx */
    extern void  fn_80102510(s32 p);               /* unload/release object */
    extern s32   fn_801043A4(s32 param);           /* dest id */
    extern s32   fn_801022B8(s32 p);               /* map key */

    /* --- small-data globals --- */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A428;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;  /* canonical; per-site reinterpret cast */
    /* --- party base + destination table --- */
    extern u8  lbl_803A2688[]; /* party / context base block */
    extern const u8 lbl_80266E90[]; /* destination table, 12 * 0x12-byte entries */

    u8*  partyBase;
    void* mon;
    void* lastMon;
    s32  eligible;
    s32  local8;          /* frame local at 0x8(sp); fn_80102138 result / flag */
    void* node;
    void* child;
    s32  destId;          /* fn_801043A4 result (treated as s32) */
    s32  mapKey;          /* fn_801022B8 result (table lookup key) */
    s32  resolvedMapId;   /* table-scan result, default 0 */
    s32  stateValue;
    u32  predicate;
    register const u8* ent;
    s32  e;

    local8 = 0;
    partyBase = lbl_803A2688;

    /* Re-enable the six fixed world-map menu element IDs. */
    fn_8005D8F8(0x1005, 0);
    fn_8005D8F8(0x1002, 0);
    fn_8005D8F8(0x1004, 0);
    fn_8005D8F8(0x1001, 0);
    fn_8005D8F8(0x1003, 0);
    fn_8005D8F8(0x1000, 0);

    /* Slot 0: eligibility -> menu element 0xFFF. */
    mon = fn_8012AC08(partyBase, 0);
    predicate = (u8)fn_8011E850((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)fn_80123FBC((u8*)mon);
        if (predicate > 0U && (u8)fn_80075FEC((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    fn_8005D8F8(0xFFF, (u32)eligible);

    /* Slot 1 -> menu element 0xFFC. */
    mon = fn_8012AC08(partyBase, 1);
    predicate = (u8)fn_8011E850((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)fn_80123FBC((u8*)mon);
        if (predicate > 0U && (u8)fn_80075FEC((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    fn_8005D8F8(0xFFC, (u32)eligible);

    /* Slot 2 -> menu element 0xFFE. */
    mon = fn_8012AC08(partyBase, 2);
    predicate = (u8)fn_8011E850((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)fn_80123FBC((u8*)mon);
        if (predicate > 0U && (u8)fn_80075FEC((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    fn_8005D8F8(0xFFE, (u32)eligible);

    /* Slot 3 -> menu element 0xFFB. */
    mon = fn_8012AC08(partyBase, 3);
    predicate = (u8)fn_8011E850((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)fn_80123FBC((u8*)mon);
        if (predicate > 0U && (u8)fn_80075FEC((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    fn_8005D8F8(0xFFB, (u32)eligible);

    /* Slot 4 -> menu element 0xFFD. */
    mon = fn_8012AC08(partyBase, 4);
    predicate = (u8)fn_8011E850((u8*)mon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)fn_80123FBC((u8*)mon);
        if (predicate > 0U && (u8)fn_80075FEC((u8*)mon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    fn_8005D8F8(0xFFD, (u32)eligible);

    /* Slot 5 -> menu element 0xFFA. */
    lastMon = fn_8012AC08(partyBase, 5);
    predicate = (u8)fn_8011E850((u8*)lastMon);
    if (predicate > 0U) {
        eligible = 1;
    } else {
        predicate = (u8)fn_80123FBC((u8*)lastMon);
        if (predicate > 0U && (u8)fn_80075FEC((u8*)lastMon) == 1) {
            eligible = 1;
        } else {
            eligible = 0;
        }
    }
    fn_8005D8F8(0xFFA, (u32)eligible);

    /* Ensure destination object 0xD9 is loaded.  Lazy-load when either the
     * "already initialized" flag is set, or the object is not yet present. */
    if ((*(u8*)&lbl_8047A410) != 0 || fn_801023E4((void*)0xD9) == 0) {
        local8 = fn_80102138((void*)0xD9, 0xFFF);
        (*(u8*)&lbl_8047A410) = 0;
    }

    fn_801021F8((void*)0xD9, 1);
    fn_80103CC0(2);

    /* Build/submit the object; the nonzero-local path passes &local8. */
    if (local8 != 0) {
        fn_801026A4((void*)0xD9, fn_801046B8(), &local8, 0, (void*)0, 0);
    } else {
        fn_801026A4((void*)0xD9, fn_801046B8(), (void*)0, 0, (void*)0, 0);
    }

    /* Resolve the object node and poke its child (key 0x10B2). */
    node = fn_80104704(0xD9);
    child = fn_801046C8(node, 0x10B2);
    if (node != (void*)0 && child != (void*)0) {
        fn_80109220(child, 1);
        *(u32*)((u8*)child + 0x4C) = 0x43D9;
    }

    fn_801045A8((void*)0xD9, 1);
    fn_80103CC0(1);

    /* Early "already arrived" branch. */
    if (fn_80102004() == 1) {
        fn_80106D3C(2, 0x4448, 1, 0);
        fn_80102510(0xD9);
        (*(s32*)&lbl_8047A42C) = 0;
        return;
    }

    /* Otherwise resolve the chosen destination id. */
    destId = fn_801043A4(0xD9);
    resolvedMapId = 0;

    /* Scan the 12-entry destination table for the map key returned by
     * fn_801022B8(0xD9).  Each entry is 0x12 bytes: key halfword at +0x10,
     * resolved map id byte at +0x01. */
    mapKey = fn_801022B8(0xD9);
    for (e = 0; e < 12; e++) {
        if (mapKey == (s32)*(u16*)(lbl_80266E90 + (e * 0x12) + 0x10)) {  /* ENDIAN-QA */
            resolvedMapId = (s32)*(u8*)(lbl_80266E90 + (e * 0x12) + 0x01);
        }
    }

    /* Special-case: travel key 0xFF9 maps to internal id 0x3E8. */
    if (fn_801022B8(0xD9) == 0xFF9) {
        resolvedMapId = 0x3E8;
    }

    /* destId == -1 forces the "invalid" sentinel result. */
    stateValue = resolvedMapId;
    if (destId == -1) {
        stateValue = -1;
    }

    (*(s32*)&lbl_8047A428) = -1;

    if (stateValue == 0x3E8) {
        goto high_sentinel;
    }
    if (stateValue >= 0x3E8) {
        goto valid_destination;
    }
    switch (stateValue) {
    case -1:
        goto invalid_sentinel;
    default:
        break;
    }
    goto valid_destination;

invalid_sentinel:
    (*(s32*)&lbl_8047A42C) = 9;
    return;

high_sentinel:
    (*(s32*)&lbl_8047A42C) = 9;
    return;

valid_destination:
    (*(s32*)&lbl_8047A428) = stateValue;
    (*(s32*)&lbl_8047A42C) = 8;
}
#pragma peephole on
#else
/* fn_8002F284 - GSmap_LoadDestination (0x8002F284, 0x518 bytes)
 *
 * World-map "load destination" handler. Re-enables the six map menu item
 * slots, then re-evaluates each of the six party-member portrait slots
 * (slots 0..5) deciding whether each is grayed out, by running the same
 * 3-predicate eligibility test used in gs_npc_event.c:208-216:
 *     eligible = fn_8011E850(mon) ||
 *                (fn_80123FBC(mon) && fn_80075FEC() == 1)
 * The per-slot result (0/1) is fed to UI dispatcher fn_8005D8F8 under the
 * corresponding menu element ID.
 *
 * It then ensures scene/object 0xD9 is loaded (lazy-load via fn_80102138 if
 * not already present), shows it, looks up child node 0x10B2 and pokes a tag,
 * and finally resolves the chosen travel destination either through an early
 * "already arrived" path (fn_80102004()==1 -> sets state (*(s32*)&lbl_8047A42C)=0) or by
 * scanning the 12-entry destination table lbl_80266E90 (stride 0x12, key at
 * +0x10, map id at +0x01) and writing the resolved id / UI state code into the
 * (*(s32*)&lbl_8047A428) / (*(s32*)&lbl_8047A42C) small-data globals.
 *
 * Wrapper reads no incoming registers before first write -> takes no params.
 */
void fn_8002F284(void)
{
    /* --- UI item-enable dispatcher: fn_8005D8F8(u32 elementId, u32 val) --- */
    extern void fn_8005D8F8(u32 id, u32 val);
    /* --- party-collection accessor + per-member predicates --- */
    extern void* fn_8012AC08(u8* base, u16 idx);   /* idx-th party member object */
    extern u8    fn_8011E850(u8* mon);             /* eligibility predicate A */
    extern u32   fn_80123FBC(u8* mon);             /* eligibility predicate B */
    extern u8    fn_80075FEC(void);                /* global-state gate (==1) */
    /* --- scene/object (id 0xD9) management (gs_model.c family) --- */
    extern s32   fn_801023E4(void* p);             /* present? (>=0) / -1 absent */
    extern s32   fn_80102138(void* p, u32 param);  /* lazy load -> handle/result */
    extern void  fn_801021F8(void* p, u32 val);    /* set visibility on subtree */
    extern u8    fn_80103CC0(u8 mode);             /* push render mode, ret old */
    extern u32   fn_801046B8(void);                /* current context handle */
    extern void  fn_801026A4(void* p, u32 a, void* b, s32 c, void* d, s32 e); /* submit/build */
    extern void* fn_80104704(s32 p);               /* resolve node by id */
    extern void* fn_801046C8(void* head, s32 key); /* find child node by key */
    extern void  fn_80109220(void* node, u32 enable); /* enable flag on node */
    extern void  fn_801045A8(void* p, u8 flags);   /* show/commit object */
    extern s32   fn_80102004(void);                /* arrival/joint-count query */
    extern void  fn_80106D3C(s32 a, s32 b, s32 c, s32 d); /* trigger arrival fx */
    extern void  fn_80102510(s32 p);               /* unload/release object */
    extern void* fn_801043A4(s32 param);           /* (here used as s32 dest id) */
    extern void* fn_801022B8(void* p, u32 target); /* (here used as s32 map key) */

    /* --- small-data globals --- */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A428;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A42C;  /* canonical; per-site reinterpret cast */
    /* --- party base + destination table --- */
    extern u8  lbl_803A2688[]; /* party / context base block */
    extern u8  lbl_80266E90[]; /* destination table, 12 * 0x12-byte entries */

    u8*  partyBase;
    void* mon;
    s32  eligible;
    s32  i;
    s32  local8;          /* frame local at 0x8(sp); fn_80102138 result / flag */
    void* node;
    void* child;
    s32  destId;          /* fn_801043A4 result (treated as s32) */
    s32  mapKey;          /* fn_801022B8 result (table lookup key) */
    s32  resolvedMapId;   /* table-scan result, default 0 */
    u8*  ent;
    s32  e;

    partyBase = lbl_803A2688;
    local8 = 0;

    /* Re-enable the six fixed world-map menu element IDs. */
    fn_8005D8F8(0x1005, 0);
    fn_8005D8F8(0x1002, 0);
    fn_8005D8F8(0x1004, 0);
    fn_8005D8F8(0x1001, 0);
    fn_8005D8F8(0x1003, 0);
    fn_8005D8F8(0x1000, 0);

    /* Slot 0: eligibility -> menu element 0xFFF. */
    mon = fn_8012AC08(partyBase, 0);
    if (fn_8011E850((u8*)mon) != 0) {
        eligible = 1;
    } else if (fn_80123FBC((u8*)mon) != 0 && fn_80075FEC() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    fn_8005D8F8(0xFFF, (u32)eligible);

    /* Slot 1 -> menu element 0xFFC. */
    mon = fn_8012AC08(partyBase, 1);
    if (fn_8011E850((u8*)mon) != 0) {
        eligible = 1;
    } else if (fn_80123FBC((u8*)mon) != 0 && fn_80075FEC() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    fn_8005D8F8(0xFFC, (u32)eligible);

    /* Slot 2 -> menu element 0xFFE. */
    mon = fn_8012AC08(partyBase, 2);
    if (fn_8011E850((u8*)mon) != 0) {
        eligible = 1;
    } else if (fn_80123FBC((u8*)mon) != 0 && fn_80075FEC() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    fn_8005D8F8(0xFFE, (u32)eligible);

    /* Slot 3 -> menu element 0xFFB. */
    mon = fn_8012AC08(partyBase, 3);
    if (fn_8011E850((u8*)mon) != 0) {
        eligible = 1;
    } else if (fn_80123FBC((u8*)mon) != 0 && fn_80075FEC() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    fn_8005D8F8(0xFFB, (u32)eligible);

    /* Slot 4 -> menu element 0xFFD. */
    mon = fn_8012AC08(partyBase, 4);
    if (fn_8011E850((u8*)mon) != 0) {
        eligible = 1;
    } else if (fn_80123FBC((u8*)mon) != 0 && fn_80075FEC() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    fn_8005D8F8(0xFFD, (u32)eligible);

    /* Slot 5 -> menu element 0xFFA. */
    mon = fn_8012AC08(partyBase, 5);
    if (fn_8011E850((u8*)mon) != 0) {
        eligible = 1;
    } else if (fn_80123FBC((u8*)mon) != 0 && fn_80075FEC() == 1) {
        eligible = 1;
    } else {
        eligible = 0;
    }
    fn_8005D8F8(0xFFA, (u32)eligible);

    /* Ensure destination object 0xD9 is loaded.  Lazy-load when either the
     * "already initialized" flag is set, or the object is not yet present. */
    if ((*(u8*)&lbl_8047A410) != 0 || fn_801023E4((void*)0xD9) == 0) {
        local8 = fn_80102138((void*)0xD9, 0xFFF);
        (*(u8*)&lbl_8047A410) = 0;
    }

    fn_801021F8((void*)0xD9, 1);
    fn_80103CC0(2);

    /* Build/submit the object; the nonzero-local path passes &local8. */
    if (local8 != 0) {
        fn_801026A4((void*)0xD9, fn_801046B8(), &local8, 0, (void*)0, 0);
    } else {
        fn_801026A4((void*)0xD9, fn_801046B8(), (void*)0, 0, (void*)0, 0);
    }

    /* Resolve the object node and poke its child (key 0x10B2). */
    node = fn_80104704(0xD9);
    child = fn_801046C8(node, 0x10B2);
    if (node != (void*)0 && child != (void*)0) {
        fn_80109220(child, 1);
        *(u32*)((u8*)child + 0x4C) = 0x43D9;
    }

    fn_801045A8((void*)0xD9, 1);
    fn_80103CC0(1);

    /* Early "already arrived" branch. */
    if (fn_80102004() == 1) {
        fn_80106D3C(2, 0x4448, 1, 0);
        fn_80102510(0xD9);
        (*(s32*)&lbl_8047A42C) = 0;
        return;
    }

    /* Otherwise resolve the chosen destination id. */
    destId = (s32)fn_801043A4(0xD9);
    resolvedMapId = 0;

    /* Scan the 12-entry destination table for the map key returned by
     * fn_801022B8(0xD9).  Each entry is 0x12 bytes: key halfword at +0x10,
     * resolved map id byte at +0x01. */
    mapKey = (s32)fn_801022B8((void*)0xD9, 0);  /* selector 0 (li r5,0) */
    ent = lbl_80266E90;
    for (e = 0; e < 12; e++) {
        if (mapKey == (s32)*(s16*)(ent + 0x10)) {  /* ENDIAN-QA */
            resolvedMapId = (s32)*(u8*)(ent + 0x01);
        }
        ent += 0x12;
    }

    /* Special-case: travel key 0xFF9 maps to internal id 0x3E8. */
    if (fn_801022B8((void*)0xD9, 0) == (void*)0xFF9) {
        resolvedMapId = 0x3E8;
    }

    /* destId == -1 forces the "invalid" sentinel result. */
    if (destId == -1) {
        resolvedMapId = -1;
    }

    (*(s32*)&lbl_8047A428) = -1;

    if (resolvedMapId == 0x3E8) {
        /* High sentinel: "arrived elsewhere" UI state. */
        (*(s32*)&lbl_8047A42C) = 9;
    } else if (resolvedMapId > 0x3E8) {
        /* Out-of-range high -> normal destination state, store id. */
        (*(s32*)&lbl_8047A428) = resolvedMapId;
        (*(s32*)&lbl_8047A42C) = 8;
    } else if (resolvedMapId == -1) {
        /* Invalid -> same UI state as 0x3E8 sentinel. */
        (*(s32*)&lbl_8047A42C) = 9;
    } else {
        /* Valid in-range destination -> store id, normal state. */
        (*(s32*)&lbl_8047A428) = resolvedMapId;
        (*(s32*)&lbl_8047A42C) = 8;
    }
}
#endif

/* fn_8002F79C - 0x8002F79C | size: 0x4bc */
extern void fn_8014402C(void);
extern void fn_8011ED68(void);
extern u32 lbl_8047A428;
extern f32 lbl_8047B9D4;
extern f64 lbl_8047B9E0;
extern f64 lbl_8047B9E8;
extern f32 lbl_8047B9DC;
extern u32 lbl_8047A42C;
extern u32 lbl_8047A410;
extern u32 lbl_8047A424;
#if 1
asm void fn_8002F79C(void) {
#include "src/game/gs_worldmap_fn_8002F79C.inc"
}
#else
/* fn_8002F79C - GSmap_PrepareArrival (0x8002F79C, 0x4BC)
 *
 * Overworld "prepare arrival / start-encounter check" step of the worldmap
 * state machine. Takes no parameters (CW: r3/r4 are loaded with literals
 * before any read). Drives the SDA state vars:
 *   lbl_8047A428 (u32) = current map/area index (input)
 *   lbl_8047A42C (u32) = worldmap step/state (output: 2 = abort/redo, 7 = arrive)
 *   (*(u8*)&lbl_8047A410) (u8)  = "arrival ready" flag (output)
 *   lbl_8047A424 (u32) = committed/arrival area index (output)
 *
 * Three outcomes:
 *   (A) party not ready            -> play abort cue, spin a frame-timed delay, state=2
 *   (B) interaction reports busy   -> suppress UI, play abort cue, spin delay, state=2
 *   (C) at least one party member is a valid wild/usable mon at a different
 *       slot than the current area -> normal "loading" cue, spin delay, state=2
 *   (D) otherwise commit the arrival: latch the area index and set state=7.
 *
 * The CW 0x43300000 store/lfd/fsub sequences are the standard big-endian
 * int->double conversion magic; normalized here to plain casts. The inner
 * loop is a vsync-paced timing spin that accumulates
 *   acc += (f64)(s32)GSgfx_tick() / (f64)(u32)GSrandom_Get()
 * until it crosses an f32 threshold, yielding to the scheduler each frame.
 */
void fn_8002F79C(void) {
    /* ---- cross-TU callees (block-scope typed externs, TU convention) ---- */
    extern u8*  fn_80129280(s32 side, s32 slotType);      /* get party/group handle */
    extern u32  fn_8012A5B0(u8* ptr, u32 selector, u32 idx); /* interaction getter */
    extern u16  fn_8011F1A0(u8* obj);                     /* read interaction field */
    extern u8   fn_801440A0(u16 handle);                  /* effect/UI helper */
    extern u8   fn_8014402C(void);                        /* arrival-ready query */
    extern s32  fn_8011F4F0(s32 pokemon);                 /* get species/id */
    extern void fn_80132A38(s32 msgType, s32 species);    /* show message */
    extern void* fn_80104704(s32 key);                    /* lookup effect object */
    extern void* fn_801046C8(void* obj, s32 sub);         /* sub-object lookup */
    extern void fn_80109220(void* elem, u32 flag);        /* activate effect element */
    extern void fn_80166AB8(u32 a, u32 b, u32 c);         /* play sound/cue */
    extern void fn_801021F8(s32 id, s32 flag);            /* set UI visibility */
    extern u32  fn_8011ED68(u8* obj);                     /* interaction busy query */
    extern u8*  fn_8012AC08(u8* party, u32 slot);         /* get party member at slot */
    extern u8   fn_8011E850(u8* mon);                     /* flag query */
    extern u8   fn_80123FBC(u8* mon);                     /* validity check */
    extern u8   fn_80075FEC(u8* mon);                     /* usable-state query */
    extern u8   fn_8011E8DC(u8* mon);                     /* flag query */
    extern u32  fn_8012640C(u8* obj, u32 id, u32 selector, u32 d); /* mon prop getter */
    extern void _threadSwitch(void);                        /* vsync / scheduler yield */
    extern u32  fn_800D37CC(void);                        /* GSrandom_Get */
    extern s32  fn_800D3088(void);                        /* GSgfx tick */

    /* ---- SDA state globals (block-scope typed externs) ---- */
    extern u32 lbl_8047A428;   /* current map/area index */
    extern u32 lbl_8047A42C;   /* worldmap step/state */
    extern u32 lbl_8047A410;  /* canonical; per-site reinterpret cast */
    extern u32 lbl_8047A424;   /* committed area index */

    /* ---- timing-loop constants (sdata2). f32 accumulator start/threshold ---- */
    extern f32 lbl_8047B9D4;   /* loop accumulator start */
    extern f32 lbl_8047B9DC;   /* loop accumulator threshold */

    u8* party;
    u8* interact;
    u16 field;
    u8  ready;
    s32 species;
    void* effRoot;
    void* effElem;
    f32 acc;
    u32 slot;
    u8  foundWild;

    party = fn_80129280(0, 2);
    interact = (u8*)fn_8012A5B0(0, 3, (u16)lbl_8047A428);

    field = fn_8011F1A0(interact);
    if (field != 0) {
        fn_801440A0(field);
        ready = fn_8014402C();
    } else {
        ready = 1;
    }

    if (ready == 0) {
        /* ---- (A) party not ready: abort cue + delay, redo this step ---- */
        species = fn_8011F4F0((s32)interact);
        fn_80132A38(0x32, species);

        effRoot = fn_80104704(0xD9);
        effElem = fn_801046C8(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            fn_80109220(effElem, 1);
            *(u32*)((u8*)effElem + 0x4C) = 0x43DD;
        }

        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 r;
            _threadSwitch();
            r = (f64)(u32)fn_800D37CC();          /* ENDIAN-QA: unsigned int->double */
            acc += (f32)((f64)(s32)fn_800D3088() / r); /* ENDIAN-QA: signed int->double */
        }

        effRoot = fn_80104704(0xD9);
        effElem = fn_801046C8(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            *(u32*)((u8*)effElem + 0x4C) = 0;
            fn_80109220(effElem, 0);
        }

        lbl_8047A42C = 2;
        return;
    }

    /* ---- ready != 0 ---- */
    if ((u8)fn_8011ED68(interact) == 1) {
        /* ---- (B) interaction busy: hide UI, abort cue + delay, redo ---- */
        fn_801021F8(0xD9, 0);

        species = fn_8011F4F0((s32)interact);
        fn_80132A38(0x32, species);

        effRoot = fn_80104704(0xD9);
        effElem = fn_801046C8(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            fn_80109220(effElem, 1);
            *(u32*)((u8*)effElem + 0x4C) = 0x43DF;
        }

        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 r;
            _threadSwitch();
            r = (f64)(u32)fn_800D37CC();
            acc += (f32)((f64)(s32)fn_800D3088() / r);
        }

        effRoot = fn_80104704(0xD9);
        effElem = fn_801046C8(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            *(u32*)((u8*)effElem + 0x4C) = 0;
            fn_80109220(effElem, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 2;
        return;
    }

    /* ---- scan the 6 party slots for a valid wild/usable mon at a slot
     *      other than the current area index ---- */
    foundWild = 0;
    for (slot = 0; slot < 6; slot++) {
        u8* mon;
        if ((u16)slot == (u16)lbl_8047A428) {
            continue;
        }
        mon = fn_8012AC08(party, slot);
        if (fn_8011E850(mon) != 0) {
            continue;
        }
        if (fn_80123FBC(mon) == 0) {
            continue;
        }
        if (fn_80075FEC(mon) != 1) {
            continue;
        }
        if (fn_8011E8DC(mon) != 0) {
            continue;
        }
        if ((u16)fn_8012640C(mon, 0, 0x83, 0) != 0) {
            foundWild = 1;
        }
    }

    if (foundWild == 0) {
        /* ---- (C) no usable wild mon: loading cue + delay, redo ---- */
        fn_801021F8(0xD9, 0);

        species = fn_8011F4F0((s32)interact);
        fn_80132A38(0x32, species);

        effRoot = fn_80104704(0xD9);
        effElem = fn_801046C8(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            fn_80109220(effElem, 1);
            *(u32*)((u8*)effElem + 0x4C) = 0x44E8;
        }

        fn_80166AB8(0x26, 0, 0);

        acc = lbl_8047B9D4;
        while (acc < lbl_8047B9DC) {
            f64 r;
            _threadSwitch();
            r = (f64)(u32)fn_800D37CC();
            acc += (f32)((f64)(s32)fn_800D3088() / r);
        }

        effRoot = fn_80104704(0xD9);
        effElem = fn_801046C8(effRoot, 0x10B2);
        if (effRoot != 0 && effElem != 0) {
            *(u32*)((u8*)effElem + 0x4C) = 0;
            fn_80109220(effElem, 0);
        }

        fn_801021F8(0xD9, 1);
        lbl_8047A42C = 2;
        return;
    }

    /* ---- (D) commit arrival ---- */
    (*(u8*)&lbl_8047A410) = 1;
    lbl_8047A424 = lbl_8047A428;
    lbl_8047A42C = 7;
}
#endif
