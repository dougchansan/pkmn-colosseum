/**
 * @file gs_party_access.c
 * @brief GSpartyAccess -- Pokemon party data accessor functions.
 *
 * Address range: 0x8000BA94 - 0x8000D290 (~60 functions)
 *
 * This module provides a large set of accessor functions for reading and
 * writing fields of the player's Pokemon party data. The functions come
 * in several clusters based on their size signature:
 *
 *   0x24 bytes: Simple 32-bit field read (lwz + blr)
 *   0x48 bytes: Indexed field read with bounds check
 *   0x6C bytes: Field read/write with validation callback
 *   0xA4 bytes: Multi-field read with struct offset calculation
 *   0xCC bytes: Complex accessor with linked list traversal
 *
 * The party data structure appears to be an array where each Pokemon
 * occupies 0x8C bytes. Key offsets within each Pokemon entry:
 *   +0x00: Species ID (u16)
 *   +0x02: Current HP (u16)
 *   +0x04: Max HP (u16)
 *   +0x06: Level (u8)
 *   +0x08: Status condition (u32)
 *   +0x0C: Held item (u16)
 *   +0x10: Move 1 ID (u16)
 *   +0x14: Move 2 ID (u16)
 *   +0x18: Move 3 ID (u16)
 *   +0x1C: Move 4 ID (u16)
 *   +0x20: Attack stat (u16)
 *   +0x22: Defense stat (u16)
 *   +0x24: Sp.Atk stat (u16)
 *   +0x26: Sp.Def stat (u16)
 *   +0x28: Speed stat (u16)
 *
 * Key functions:
 *   fn_8000BA94  GSparty_GetFieldPtr         -- return base pointer to party array
 *   fn_8000BAB8  GSparty_GetSpecies           -- get species ID for slot N
 *   fn_8000BB00  GSparty_GetLevel             -- get level for slot N
 *   fn_8000BB48  GSparty_GetMoveCount         -- count non-zero moves for slot N
 *   fn_8000BBEC  GSparty_GetSlot0_HP          -- direct HP accessor, slot 0
 *   fn_8000BC58  GSparty_GetSlot1_HP          -- direct HP accessor, slot 1
 *   fn_8000BCC4  GSparty_GetSlot2_HP          -- direct HP accessor, slot 2
 *   fn_8000BD30  GSparty_GetSlot3_HP          -- direct HP accessor, slot 3
 *   fn_8000BD9C  GSparty_GetSlot4_HP          -- direct HP accessor, slot 4
 *   fn_8000BE08  GSparty_GetSlot5_HP          -- direct HP accessor, slot 5
 *   fn_8000BE74  GSparty_GetStatBlock         -- 0x12C bytes, read all 6 stats
 *   fn_8000BFA0  GSparty_SetStatBlock         -- 0xCC bytes, write all 6 stats
 *   fn_8000C06C  GSparty_GetType1             -- get primary type
 *   fn_8000C0DC  GSparty_GetType2             -- get secondary type
 *   fn_8000C144  GSparty_GetAbility           -- get ability ID
 *   fn_8000C1A8  GSparty_GetNature            -- get nature ID
 *   fn_8000C210  GSparty_GetIV_HP             -- get HP IV
 *   fn_8000C234  GSparty_GetIV_Atk            -- get Attack IV
 *   fn_8000C258  GSparty_GetIV_Def            -- get Defense IV
 *   fn_8000C27C  GSparty_GetIV_Spd            -- get Speed IV
 *   fn_8000C2A0  GSparty_GetEV_HP             -- get HP EV (0x3C bytes)
 *   fn_8000C2DC  GSparty_GetEV_Atk            -- get Attack EV
 *   fn_8000C318  GSparty_GetGender            -- 8 bytes, return gender byte
 *   fn_8000C320  GSparty_GetShiny             -- 8 bytes, return shiny flag
 *   fn_8000C328  GSparty_GetFriendship        -- get friendship value
 *   fn_8000C358  GSparty_GetExperience        -- get current EXP
 *   fn_8000C3A4  GSparty_GetExpToNext         -- get EXP to next level
 *   fn_8000C3D4  GSparty_GetMoveData          -- 0xCC bytes, get all move data
 *   fn_8000C4A0  GSparty_SetMove              -- set move at index
 *   fn_8000C518  GSparty_GetMovePP            -- get PP for move N
 *   fn_8000C588  GSparty_GetMovePPMax         -- get max PP for move N
 *   fn_8000C624  GSparty_GetHeldItem          -- get held item ID
 *   fn_8000C688  GSparty_SetHeldItem          -- set held item ID
 *   fn_8000C6EC  GSparty_GetCondition         -- get condition value (contest stat)
 *   fn_8000C788  GSparty_SetCondition         -- set condition value
 *   fn_8000C824  GSparty_GetRibbon            -- 0x108 bytes, get ribbon bit
 *   fn_8000C92C  GSparty_SetRibbon            -- 0x108 bytes, set ribbon bit
 *   fn_8000CA34  GSparty_GetMarkings          -- get marking bits
 *   fn_8000CAA4  GSparty_GetPokerus           -- get Pokerus strain
 *   fn_8000CAD0  GSparty_GetPokerusDays       -- get Pokerus days remaining
 *   fn_8000CAFC  GSparty_GetBall              -- get Pokeball type
 *   fn_8000CB28  GSparty_GetOTGender          -- get original trainer gender
 *   fn_8000CB54  GSparty_IsShadow             -- check if Shadow Pokemon
 *   fn_8000CB74  GSparty_GetShadowGauge       -- 0xC8 bytes, get heart gauge value
 *   fn_8000CC3C  GSparty_GetOTName            -- get OT name pointer
 *   fn_8000CC60  GSparty_GetNickname          -- get nickname pointer
 *   fn_8000CC84  GSparty_GetOTID              -- get OT trainer ID
 *   fn_8000CCA8  GSparty_GetOTSID             -- get OT secret ID
 *   fn_8000CCD0  GSparty_GetPersonality       -- get personality value (PID)
 *   fn_8000CCF8  GSparty_GetEncryptionKey     -- get encryption key
 *   fn_8000CD20  GSparty_GetFormData          -- get form/cosmetic data
 *   fn_8000CD50  GSparty_GetStatusFull        -- 0xC8 bytes, get full status struct
 *   fn_8000CE18  GSparty_CureStatus           -- cure status condition
 *   fn_8000CE5C  GSparty_ApplyDamage          -- apply damage to HP
 *   fn_8000CED0  GSparty_IsAlive              -- check if HP > 0
 *   fn_8000CEF8  GSparty_FullHeal             -- restore HP to max
 *   fn_8000CF68  GSparty_GetBattleStats       -- 0xF4 bytes, get computed battle stats
 *   fn_8000D05C  GSparty_CalcStatModifiers    -- 0xC0 bytes, apply stat stage modifiers
 *   fn_8000D11C  GSparty_GetCritRate          -- get critical hit rate
 *   fn_8000D154  GSparty_GetAccuracy          -- get accuracy modifier
 *   fn_8000D1C4  GSparty_GetEvasion           -- get evasion modifier
 *   fn_8000D234  GSparty_ResetStatStages      -- reset all stat stages to 0
 *   fn_8000D290  GSparty_Nop                  -- 8 bytes, return void
 *
 * SDA globals:
 *   Many of these accessors load base pointers from SDA globals in the
 *   0x80478F00-0x80478F50 range, which are initialized during game boot
 *   to point into the save data structure.
 */

#include "dolphin/types.h"

/* =========================================================================
 * Stub implementations for key accessor patterns
 * ========================================================================= */

/* Pattern 1: Simple 0x24-byte accessor (e.g., fn_8000BA94)
 *
 * s32 GSparty_GetFieldPtr(void) {
 *     return *(s32*)gPartyBasePtr;
 * }
 */

/* Pattern 2: 0x48-byte indexed accessor (e.g., fn_8000BAB8)
 *
 * u16 GSparty_GetSpecies(s32 slot) {
 *     if (slot >= MAX_PARTY_SIZE) return 0;
 *     return gPartyArray[slot].species;
 * }
 */

/* Pattern 3: 0x6C-byte accessor with validation (e.g., fn_8000BBEC)
 *
 * u16 GSparty_GetSlotN_HP(s32 n) {
 *     void* pokemon = &gPartyArray[n];
 *     if (pokemon == NULL) return 0;
 *     return pokemon->currentHP;
 * }
 */

/* ===================================================================
 * AUTO-GENERATED accessor functions
 * Generated by tools/gen_accessors.py
 * 2 functions matched
 * =================================================================== */

/* Address: 0x8000C318 | Size: 0x8 | Pattern: return_constant */
u32 fn_8000C318(void) { return 0; }

/* Address: 0x8000C320 | Size: 0x8 | Pattern: return_constant */
u32 fn_8000C320(void) { return 0; }

/* ===== Phase 2 recovery stubs ===== */

/* fn_8000BAB8 - 0x8000BAB8 | size: 0x48 */
extern s32 fn_8010264C(u32 a, u32 b);
extern void fn_801293FC(u32 a, s32 b);
#if 0
asm void fn_8000BAB8(void) {
#include "src/game/gs_party_access_fn_8000BAB8.inc"
}
#else
#pragma peephole off
u32 fn_8000BAB8(void) {
    s32 val = fn_8010264C(2, 1);
    if (val == -1) { return 0; }
    fn_801293FC(0, val);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000BB00 - 0x8000BB00 | size: 0x48 */
extern void fn_801294C4(u32 a, s32 b);
#if 0
asm void fn_8000BB00(void) {
#include "src/game/gs_party_access_fn_8000BB00.inc"
}
#else
#pragma peephole off
u32 fn_8000BB00(void) {
    s32 val = fn_8010264C(2, 1);
    if (val == -1) { return 0; }
    fn_801294C4(0, val);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000BB48 - 0x8000BB48 | size: 0xa4 */
extern u32 fn_801EF63C(void);
extern void fn_80124A60(u8* a);
extern s32 fn_800096B4(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void fn_80129F20(u32 a, u8* b, u32 c, u32 d, u32 e);
extern u8 lbl_80478840;
extern u8 lbl_803A1A48[];
#if 0
asm void fn_8000BB48(void) {
#include "src/game/gs_party_access_fn_8000BB48.inc"
}
#else
s32 fn_8000BB48(void) {
    if ((u8)fn_801EF63C() == 1) { return -1; }
    if (lbl_80478840 != 0) {
        fn_80124A60(lbl_803A1A48);
        lbl_80478840 = 0;
    }
    if (fn_800096B4((u32)lbl_803A1A48, 0, 0, 0, 0, 0) < 0) { return -1; }
    fn_80129F20(0, lbl_803A1A48, 0, 4, 1);
    return -1;
}
#endif

/* fn_8000BBEC - 0x8000BBEC | size: 0x6c */
extern u32 fn_8012A5B0(u8* a, u32 b, u32 c);
#if 0
asm void fn_8000BBEC(void) {
#include "src/game/gs_party_access_fn_8000BBEC.inc"
}
#else
s32 fn_8000BBEC(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = fn_8012A5B0(NULL, 3, 5);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
#endif

/* fn_8000BC58 - 0x8000BC58 | size: 0x6c */
#if 0
asm void fn_8000BC58(void) {
#include "src/game/gs_party_access_fn_8000BC58.inc"
}
#else
s32 fn_8000BC58(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = fn_8012A5B0(NULL, 3, 4);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
#endif

/* fn_8000BCC4 - 0x8000BCC4 | size: 0x6c */
#if 0
asm void fn_8000BCC4(void) {
#include "src/game/gs_party_access_fn_8000BCC4.inc"
}
#else
s32 fn_8000BCC4(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = fn_8012A5B0(NULL, 3, 3);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
#endif

/* fn_8000BD30 - 0x8000BD30 | size: 0x6c */
#if 0
asm void fn_8000BD30(void) {
#include "src/game/gs_party_access_fn_8000BD30.inc"
}
#else
s32 fn_8000BD30(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = fn_8012A5B0(NULL, 3, 2);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
#endif

/* fn_8000BD9C - 0x8000BD9C | size: 0x6c */
#if 0
asm void fn_8000BD9C(void) {
#include "src/game/gs_party_access_fn_8000BD9C.inc"
}
#else
s32 fn_8000BD9C(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = fn_8012A5B0(NULL, 3, 1);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
#endif

/* fn_8000BE08 - 0x8000BE08 | size: 0x6c */
#if 0
asm void fn_8000BE08(void) {
#include "src/game/gs_party_access_fn_8000BE08.inc"
}
#else
s32 fn_8000BE08(void) {
    u32 val;
    if ((u8)fn_801EF63C() == 1) { return -1; }
    val = fn_8012A5B0(NULL, 3, 0);
    if (val == 0) { return -1; }
    return fn_800096B4(val, 0, 0, 0, 0, 0);
}
#endif

/* fn_8000BE74 - 0x8000BE74 | size: 0x12c */
extern u32 fn_800F7BC4(u32 a);
extern u32 fn_8001E3E0(u32 a, u32* b);
extern void fn_80113828(u32 a, u32 b);
extern u32 lbl_802E28F0[];
#if 0
asm void fn_8000BE74(void) {
#include "src/game/gs_party_access_fn_8000BE74.inc"
}
#else
#pragma peephole off
s32 fn_8000BE74(s32 arg) {
    u32 local;
    s32 idx;
    local = 0;
    if ((fn_800F7BC4(1) & 0x20) != 0) {
        if ((u8)fn_8001E3E0(0, &local) == 0) { return -1; }
    }
    for (idx = 0; idx < 0x99; idx++) {
        if (arg == (s32)lbl_802E28F0[idx * 2]) {
            switch (arg) {
                case 0x63: local = 0xb; break;
                case 0x64: local = 0xa; break;
                case 0x65: local = 0x8; break;
                case 0x66: local = 0xd; break;
                case 0x67: local = 0x0; break;
                case 0x68: local = 0x0; break;
                default: break;
            }
            fn_80113828(lbl_802E28F0[idx * 2 + 1], local);
            goto done;
        }
    }
done:
    return 0;
}
#pragma peephole on
#endif

/* fn_8000BFA0 - 0x8000BFA0 | size: 0xcc */
extern u8 fn_800FF52C(void);
extern void fn_80166A28(u32 a);
extern void fn_800FAEF8(u32 a, u32 b, s32 c, ...);
extern void _threadSwitch(void);
extern u32 lbl_802666B0[];
#if 0
asm void fn_8000BFA0(void) {
#include "src/game/gs_party_access_fn_8000BFA0.inc"
}
#else
#pragma peephole off
#pragma push
#pragma scheduling off
s32 fn_8000BFA0(void) {
    u32 local;
    u32 *rodata;
    s32 i;
    if (fn_800FF52C() != 0) {
        fn_80166A28(0x26);
        i = 0;
        rodata = lbl_802666B0;
        do {
            fn_800FAEF8(0xc8, 0xf0, -1, rodata);
            _threadSwitch();
            i++;
        } while (i < 0xf);
        return 0;
    }
    if ((fn_800F7BC4(1) & 0x20) != 0) {
        if ((u8)fn_8001E3E0(0, &local) == 0) { return -1; }
        fn_80113828(local, 0);
        return 0;
    }
    return 1;
}
#pragma pop
#pragma peephole on
#endif

/* fn_8000C06C - 0x8000C06C | size: 0x70 */
extern u32 fn_80102620(s32);
extern void fn_80102510(s32);
extern void fn_801026A4(s32, ...);
extern void fn_80102868(s32, s32, s32);
#if 0
asm void fn_8000C06C(void) {
#include "src/game/gs_party_access_fn_8000C06C.inc"
}
#else
#pragma peephole off
u32 fn_8000C06C(void) {
    extern u32 fn_80102620(u32 a);
    extern void fn_80102510(u32 a);
    extern void fn_801026A4(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, ...);
    extern void fn_80102868(u32 a, u32 b, u32 c);
    if ((u8)fn_80102620(8) != 0) {
        fn_80102510(8);
    } else {
        fn_801026A4(8, 0, 0, 0, 1, 0);
        fn_80102868(8, 0x17c, 0x20);
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C0DC - 0x8000C0DC | size: 0x68 */
extern u32 fn_80175FFC(void);
extern void fn_80176004(void);
extern void fn_80176030(u32 val);
extern u8 lbl_8047A298;
extern u8 lbl_8047A299;
#if 0
asm void fn_8000C0DC(void) {
#include "src/game/gs_party_access_fn_8000C0DC.inc"
}
#else
void fn_8000C0DC(void) {
    if (lbl_8047A298 == 0) { return; }
    if ((u8)fn_80175FFC() == 1) {
        fn_80176004();
        return;
    }
    if (lbl_8047A299 == 0) {
        fn_80176030(0);
        return;
    }
    if (lbl_8047A299 == 1) {
        fn_80176030(1);
    }
}
#endif

/* fn_8000C144 - 0x8000C144 | size: 0x64 */
extern u32 fn_801E11CC(void);
extern void fn_801E11D4(u32 a, u8 b);
extern u8 lbl_8047A298;
extern u8 lbl_8047A299;
#if 0
asm void fn_8000C144(void) {
#include "src/game/gs_party_access_fn_8000C144.inc"
}
#else
#pragma peephole off
u32 fn_8000C144(void) {
    extern u32 fn_80175FFC(void);
    extern void fn_80176004(void);
    extern u32 fn_801E11CC(void);
    extern void fn_801E11D4(u32 a, u8 b);
    extern u8 lbl_8047A298;
    extern u8 lbl_8047A299;
    if (lbl_8047A298 == 0) {
        lbl_8047A299 = 1;
        lbl_8047A298 = 1;
    } else {
        lbl_8047A298 = 0;
        if ((u8)fn_80175FFC() == 1) {
            fn_80176004();
        }
    }
    fn_801E11D4(fn_801E11CC(), lbl_8047A298);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C1A8 - 0x8000C1A8 | size: 0x68 */
extern u8 lbl_8047A298;
extern u8 lbl_8047A299;
#if 0
asm void fn_8000C1A8(void) {
#include "src/game/gs_party_access_fn_8000C1A8.inc"
}
#else
#pragma peephole off
u32 fn_8000C1A8(void) {
    extern u32 fn_80175FFC(void);
    extern void fn_80176004(void);
    extern u32 fn_801E11CC(void);
    extern void fn_801E11D4(u32 a, u8 b);
    extern u8 lbl_8047A298;
    extern u8 lbl_8047A299;
    if (lbl_8047A298 == 0) {
        lbl_8047A299 = 0;
        lbl_8047A298 = 1;
    } else {
        lbl_8047A298 = 0;
        if ((u8)fn_80175FFC() == 1) {
            fn_80176004();
        }
    }
    fn_801E11D4(fn_801E11CC(), lbl_8047A298);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C210 - 0x8000C210 | size: 0x24 */
extern void fn_801E1170(void);
#if 0
asm void fn_8000C210(void) {
#include "src/game/gs_party_access_fn_8000C210.inc"
}
#else
#pragma peephole off
u32 fn_8000C210(void) {
    fn_801E1170();
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C234 - 0x8000C234 | size: 0x24 */
extern void fn_801E118C(void);
#if 0
asm void fn_8000C234(void) {
#include "src/game/gs_party_access_fn_8000C234.inc"
}
#else
#pragma peephole off
u32 fn_8000C234(void) {
    fn_801E118C();
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C258 - 0x8000C258 | size: 0x24 */
extern void fn_801E11B0(void);
#if 0
asm void fn_8000C258(void) {
#include "src/game/gs_party_access_fn_8000C258.inc"
}
#else
#pragma peephole off
u32 fn_8000C258(void) {
    fn_801E11B0();
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C27C - 0x8000C27C | size: 0x24 */
extern void fn_801E119C(void);
#if 0
asm void fn_8000C27C(void) {
#include "src/game/gs_party_access_fn_8000C27C.inc"
}
#else
#pragma peephole off
u32 fn_8000C27C(void) {
    fn_801E119C();
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C2A0 - 0x8000C2A0 | size: 0x3c */
extern u32 fn_801E11CC(void);
extern void fn_801E11D4(u32 a, u8 b);
#if 0
asm void fn_8000C2A0(void) {
#include "src/game/gs_party_access_fn_8000C2A0.inc"
}
#else
#pragma peephole off
u32 fn_8000C2A0(void) {
    fn_801E11D4((u8)((u8)fn_801E11CC() == 0), lbl_8047A298);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C2DC - 0x8000C2DC | size: 0x3c */
extern u32 fn_801E11E8(void);
extern void fn_801E1258(void);
extern void fn_801E11F0(void);
#if 0
asm void fn_8000C2DC(void) {
#include "src/game/gs_party_access_fn_8000C2DC.inc"
}
#else
#pragma peephole off
u32 fn_8000C2DC(void) {
    if ((u8)fn_801E11E8() == 0) {
        fn_801E1258();
    } else {
        fn_801E11F0();
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C328 - 0x8000C328 | size: 0x30 */
extern void fn_801D0748(u32 a, u32 b, u32 c);
#if 0
asm void fn_8000C328(void) {
#include "src/game/gs_party_access_fn_8000C328.inc"
}
#else
#pragma peephole off
u32 fn_8000C328(void) {
    fn_801D0748(0xc, 2, 0);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C358 - 0x8000C358 | size: 0x4c */
extern void fn_80106D3C(u32 a, u32 b, u32 c, u32 d);
extern void fn_801069FC(u32 a);
#if 0
asm void fn_8000C358(void) {
#include "src/game/gs_party_access_fn_8000C358.inc"
}
#else
#pragma peephole off
u32 fn_8000C358(void) {
    fn_801D0748(2, 2, 0);
    fn_80106D3C(2, 0x17a7, 1, 1);
    fn_801069FC(1);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C3A4 - 0x8000C3A4 | size: 0x30 */
#if 0
asm void fn_8000C3A4(void) {
#include "src/game/gs_party_access_fn_8000C3A4.inc"
}
#else
#pragma peephole off
u32 fn_8000C3A4(void) {
    fn_801D0748(0xb, 2, 0);
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C3D4 - 0x8000C3D4 | size: 0xcc */
extern void fn_801C41C8(u32 a, f32 b);
extern void fn_801C40F0(u32 a);
extern void fn_80105FF8(u32 a, u32 b, u32 c);
extern void fn_80106080(u32 a);
extern void fn_80105FB0(u32 a);
extern void fn_80132A38(u32 a, u32 b);
extern f32 lbl_8047B6E0;
#if 0
asm void fn_8000C3D4(void) {
#include "src/game/gs_party_access_fn_8000C3D4.inc"
}
#else
#pragma peephole off
s32 fn_8000C3D4(u32 arg1, u32 type) {
    switch (type) {
    case 0:
        fn_801C41C8(3, lbl_8047B6E0);
        fn_801C40F0(1);
        fn_80105FF8(0x44c5, 1, 0);
        fn_801069FC(1);
        fn_801C41C8(2, lbl_8047B6E0);
        fn_801C40F0(1);
        break;
    case 1:
        fn_80106080(1);
        fn_80105FB0(1);
        break;
    case 2:
        _threadSwitch();
        _threadSwitch();
        _threadSwitch();
        fn_80106080(0);
        _threadSwitch();
        _threadSwitch();
        break;
    case 3:
        fn_80132A38(0x31, 0x7da);
        break;
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C4A0 - 0x8000C4A0 | size: 0x78 */
extern u32 fn_800F7BC4(u32 a);
extern u32 fn_8001E3E0(u32 a, u32* b);
extern void fn_801067E8(u32 a, u32 b, u32 c);
extern void fn_8001D7E4(void);
extern void fn_801065B8(u32 a);
#pragma peephole off
#if 0
asm void fn_8000C4A0(void) {
#include "src/game/gs_party_access_fn_8000C4A0.inc"
}
#else
s32 fn_8000C4A0(void) {
    u32 local;
    if ((fn_800F7BC4(1) & 0x20) == 0) { goto ret1; }
    if ((u8)fn_8001E3E0(0, &local) == 0) { return -1; }
    fn_801067E8(local, 1, 0);
    fn_8001D7E4();
    fn_801065B8(1);
    return 0;
ret1:
    return 1;
}
#endif
#pragma peephole on

/* fn_8000C518 - 0x8000C518 | size: 0x70 */
#if 0
asm void fn_8000C518(void) {
#include "src/game/gs_party_access_fn_8000C518.inc"
}
#else
#pragma peephole off
u32 fn_8000C518(void) {
    extern u32 fn_80102620(u32 a);
    extern void fn_80102510(u32 a);
    extern void fn_801026A4(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f, ...);
    extern void fn_80102868(u32 a, u32 b, u32 c);
    if ((u8)fn_80102620(0xc) != 0) {
        fn_80102510(0xc);
    } else {
        fn_801026A4(0xc, 0, 0, 0, 1, 0);
        fn_80102868(0xc, 0x190, 0x28);
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C588 - 0x8000C588 | size: 0x9c */
extern void* fn_800FF56C(void);
extern void* fn_80115C48(void* a);
extern u32 fn_801174C4(void);
extern void fn_80117500(void);
extern void fn_800FEC34(u32 a);
extern void fn_801174F4(void);
extern void fn_800FECB8(u32 a);
#pragma peephole off
#if 0
asm void fn_8000C588(void) {
#include "src/game/gs_party_access_fn_8000C588.inc"
}
#else
u32 fn_8000C588(void) {
    u8* obj = (u8*)fn_80115C48(fn_800FF56C());
    if ((u8)fn_80102620(0xa) != 0) {
        if ((u8)fn_801174C4() != 0) {
            fn_80117500();
        }
        fn_80102510(0xa);
        fn_800FEC34(*(u32*)(obj + 0x34));
    } else {
        fn_801174F4();
        fn_801026A4(0xa, 0, 0, 0, 0, 0);
        fn_800FECB8(*(u32*)(obj + 0x34));
    }
    return 0;
}
#endif
#pragma peephole on

/* fn_8000C624 - 0x8000C624 | size: 0x64 */
extern void fn_80166D18(u32 a, u32 b, u32 c, u32 d);
#if 0
asm void fn_8000C624(void) {
#include "src/game/gs_party_access_fn_8000C624.inc"
}
#else
#pragma peephole off
u32 fn_8000C624(s32 arg) {
    extern void fn_80166D18(u32 a, u32 b, u32 c, u32 d);
    switch (arg) {
    case 0xd3:
        fn_80166D18(0x7f, 0, 0, 1);
        break;
    case 0xd4:
        fn_80166D18(0, 0, 0, 1);
        break;
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C688 - 0x8000C688 | size: 0x64 */
#if 0
asm void fn_8000C688(void) {
#include "src/game/gs_party_access_fn_8000C688.inc"
}
#else
#pragma peephole off
u32 fn_8000C688(s32 arg) {
    extern void fn_80166D18(u32 a, u32 b, u32 c, u32 d);
    switch (arg) {
    case 0xd1:
        fn_80166D18(0x7f, 0, 1, 0);
        break;
    case 0xd2:
        fn_80166D18(0, 0, 1, 0);
        break;
    }
    return 0;
}
#pragma peephole on
#endif

/* fn_8000C6EC - 0x8000C6EC | size: 0x9c */
extern s32 fn_8010264C(u32 a, u32 b);
extern u32 fn_801666BC(u16 a);
extern void fn_80166B18(u16 a);
extern void fn_801659FC(u16 a, u32 b, u32 c);
extern u32 lbl_80478E18;
extern u32 lbl_80478E1C;
#pragma peephole off
#if 0
asm void fn_8000C6EC(void) {
#include "src/game/gs_party_access_fn_8000C6EC.inc"
}
#else
s32 fn_8000C6EC(void) {
    s32 val;
    u16 entry;
    s32 check;
    goto loop_check;
loop_body:
    if ((u32)val >= *(u32*)(u32)lbl_80478E18) { goto loop_check; }
    entry = *(u16*)(lbl_80478E1C + (u32)val * 2);
    check = (s32)fn_801666BC(entry);
    if (check == 0) { goto do_else; }
    if (check < 0) { goto do_else; }
    if (check >= 4) { goto do_else; }
    fn_80166B18(entry);
    goto loop_check;
do_else:
    fn_801659FC(entry, 0, 0x7f);
loop_check:
    val = fn_8010264C(2, 1);
    if (val != -1) { goto loop_body; }
    fn_80102510(2);
    return -1;
}
#endif
#pragma peephole on

/* ===== New externs for decompiled functions ===== */
extern void fn_80266320(void);
extern void fn_801EF02C(void);
extern void fn_801EF080(void);
extern void fn_801EF0D4(void);
extern void fn_8025FF18(u32);
extern void fn_8025FBCC(u32);
extern void fn_80190528(u32);
extern void fn_800884BC(u32, u32, u32);
extern void fn_800FF660(void);
extern u32 fn_800FF560(void);
extern u32 GSthreadCreate(s32, u32, s32, s32, s32, u32);
extern u32 fn_80129280(s32, s32);
extern u32 fn_8012AC08(u32, s32);
extern u32 fn_80123FBC(u32);
extern void fn_80097A38(u32, s32);
extern void fn_800FF730(u32);
extern void fn_8001BDF4(s32, s32, s32);
extern void fn_800F0654(u32, s32, ...);
extern void fn_8026132C(u32, u32, s32, u16*, s32, u32*);
extern void fn_8002DC6C(u32);
extern void fn_80029760(u32, s32);
extern void fn_80165A20(u32, s32, s32);
extern u8 lbl_804673F8[];
extern u32 lbl_80478E10;
extern u32 lbl_80478E14;
extern u32 lbl_80478E20;
extern u32 lbl_80478E24;
extern u32 lbl_80478E28;
extern u32 lbl_80478E2C;
extern u32 lbl_80266770[];
extern u32 lbl_80266700[];
extern u32 lbl_802666E0[];
extern f32 lbl_8047B6E8;

#pragma peephole off
/* fn_8000C788 - 0x8000C788 | size: 0x9c */
/* GSparty_SetCondition -- loop: get index, check bounds, check condition range, set */
#if 0
asm void fn_8000C788(void) {
#include "src/game/gs_party_access_fn_8000C788.inc"
}
#else
s32 fn_8000C788(void) {
    s32 val;
    u16 entry;
    s32 check;
    goto loop_check;
loop_body:
    if ((u32)val >= *(u32*)(u32)lbl_80478E10) { goto loop_check; }
    entry = *(u16*)((u32)lbl_80478E14 + (u32)val * 2);
    check = (s32)fn_801666BC(entry);
    if (check == 0) { goto do_else; }
    if (check < 0) { goto do_else; }
    if (check >= 4) { goto do_else; }
    fn_80166B18(entry);
    goto loop_check;
do_else:
    fn_801659FC(entry, 0, 0x7f);
loop_check:
    val = fn_8010264C(2, 1);
    if (val != -1) { goto loop_body; }
    fn_80102510(2);
    return -1;
}
#endif

/* fn_8000C824 - 0x8000C824 | size: 0x108 */
/* GSparty_GetRibbon -- loop with inner loop over ribbon array */
#if 0
asm void fn_8000C824(void) {
#include "src/game/gs_party_access_fn_8000C824.inc"
}
#else
s32 fn_8000C824(void) {
    s32 val;
    u16 entry;
    s32 check;
    u32 joff, j;
    goto loop_check;
loop_body:
    if ((u32)val >= *(u32*)(u32)lbl_80478E20) { goto loop_check; }
    j = 0; joff = 0;
    goto inner_check;
inner_body:
    if ((s32)val == (s32)j) { goto inner_next; }
    check = (s32)fn_801666BC(*(u16*)((u32)lbl_80478E24 + joff));
    if (check >= 4) { goto inner_next; }
    if (check >= 1) { goto do_call824; } else { goto inner_next; }
do_call824:
    fn_80166B18(*(u16*)((u32)lbl_80478E24 + joff));
inner_next:
    joff += 2; j++;
inner_check:
    if (j < *(u32*)(u32)lbl_80478E20) { goto inner_body; }
    entry = *(u16*)((u32)lbl_80478E24 + (u32)val * 2);
    check = (s32)fn_801666BC(entry);
    if (check == 0) { goto do_else; }
    if (check < 0) { goto do_else; }
    if (check >= 4) { goto do_else; }
    fn_80166B18(entry);
    goto loop_check;
do_else:
    fn_801659FC(entry, 0, 0x7f);
loop_check:
    val = fn_8010264C(2, 1);
    if (val != -1) { goto loop_body; }
    fn_80102510(2);
    return -1;
}
#endif

/* fn_8000C92C - 0x8000C92C | size: 0x108 */
/* GSparty_SetRibbon -- loop with inner loop, different SDA globals */
#if 0
asm void fn_8000C92C(void) {
#include "src/game/gs_party_access_fn_8000C92C.inc"
}
#else
s32 fn_8000C92C(void) {
    s32 val;
    u16 entry;
    s32 check;
    u32 joff, j;
    goto loop_check;
loop_body:
    if ((u32)val >= *(u32*)(u32)lbl_80478E28) { goto loop_check; }
    j = 0; joff = 0;
    goto inner_check;
inner_body:
    if ((s32)val == (s32)j) { goto inner_next; }
    check = (s32)fn_801666BC(*(u16*)((u32)lbl_80478E2C + joff));
    if (check >= 4) { goto inner_next; }
    if (check >= 1) { goto do_call92c; } else { goto inner_next; }
do_call92c:
    fn_80166B18(*(u16*)((u32)lbl_80478E2C + joff));
inner_next:
    joff += 2; j++;
inner_check:
    if (j < *(u32*)(u32)lbl_80478E28) { goto inner_body; }
    entry = *(u16*)((u32)lbl_80478E2C + (u32)val * 2);
    check = (s32)fn_801666BC(entry);
    if (check == 0) { goto do_else; }
    if (check < 0) { goto do_else; }
    if (check >= 4) { goto do_else; }
    fn_80166B18(entry);
    goto loop_check;
do_else:
    fn_80165A20(entry, 0, 0x7f);
loop_check:
    val = fn_8010264C(2, 1);
    if (val != -1) { goto loop_body; }
    fn_80102510(2);
    return -1;
}
#endif
#pragma peephole on

#pragma peephole off
/* fn_8000CA34 - 0x8000CA34 | size: 0x70 */
/* GSparty_GetMarkings */
s32 fn_8000CA34(void) {
    u32 r;
    r = fn_80102620(0x9);
    if ((u8)r != 0) {
        fn_80102510(0x9);
    } else {
        fn_801026A4(0x9, 0, 0, 0, 1, 0);
        fn_80102868(0x9, 0xC, 0xA);
    }
    return 0;
}

#ifndef PCPORT
typedef struct {
    s8 b80 : 1;
    s8 b40 : 1;
    s8 b20 : 1;
    s8 b10 : 1;
    s8 b08 : 1;
    s8 b04 : 1;
    s8 b02 : 1;
    s8 b01 : 1;
} PartyFlags8;
#endif

#pragma push
#pragma peephole off

/* fn_8000CAA4 - 0x8000CAA4 | size: 0x2c — toggles bit 3 (0x10) of lbl_804673F8[0x8] */
s32 fn_8000CAA4(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x10;
#else
    PartyFlags8* p = (PartyFlags8*)(lbl_804673F8 + 8);
    p->b10 ^= 1;
#endif
    return 0;
}

/* fn_8000CAD0 - 0x8000CAD0 | size: 0x2c — toggles bit 2 (0x20) */
s32 fn_8000CAD0(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x20;
#else
    PartyFlags8* p = (PartyFlags8*)(lbl_804673F8 + 8);
    p->b20 ^= 1;
#endif
    return 0;
}

/* fn_8000CAFC - 0x8000CAFC | size: 0x2c — toggles bit 1 (0x40) */
s32 fn_8000CAFC(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x40;
#else
    PartyFlags8* p = (PartyFlags8*)(lbl_804673F8 + 8);
    p->b40 ^= 1;
#endif
    return 0;
}

/* fn_8000CB28 - 0x8000CB28 | size: 0x2c — toggles bit 0 (0x80) */
s32 fn_8000CB28(void) {
#ifdef PCPORT
    lbl_804673F8[8] ^= 0x80;
#else
    PartyFlags8* p = (PartyFlags8*)(lbl_804673F8 + 8);
    p->b80 ^= 1;
#endif
    return 0;
}
#pragma pop

/* fn_8000CB54 - 0x8000CB54 | size: 0x20 */
/* GSparty_IsShadow -- tail call to fn_80266320 */
void fn_8000CB54(void) {
    fn_80266320();
}

/* fn_8000CB74 - 0x8000CB74 | size: 0xc8 */
/* GSparty_GetShadowGauge -- load table, search, call GSthreadCreate + fn_800F0654 */
#if 0
asm void fn_8000CB74(void) {
#include "src/game/gs_party_access_fn_8000CB74.inc"
}
#else
u32 fn_8000CB74(u32 arg) {
    u32 table[6];
    u32 idx = 0;
    u32 val;
    u32 r;
    u32 *p = lbl_80266770;
    table[0] = p[0];
    table[1] = p[1];
    table[2] = p[2];
    table[3] = p[3];
    table[4] = p[4];
    table[5] = p[5];
    if ((s32)arg == (s32)table[0]) { goto idx_done; }
    idx = 1;
    if ((s32)arg == (s32)table[2]) { goto idx_done; }
    idx = 2;
    if ((s32)arg == (s32)table[4]) { goto idx_done; }
    idx = 3;
idx_done:
    val = table[idx * 2 + 1];
    r = GSthreadCreate(1, fn_800FF560(), 0x4000, 1, 1, (u32)(void(*)(void))fn_8000CB54);
    fn_800F0654(r, 1, val);
    return 0;
}
#endif

/* fn_8000CC3C - 0x8000CC3C | size: 0x24 */
/* GSparty_GetOTName */
s32 fn_8000CC3C(void) {
    fn_801EF02C();
    return 1;
}

/* fn_8000CC60 - 0x8000CC60 | size: 0x24 */
/* GSparty_GetNickname */
s32 fn_8000CC60(void) {
    fn_801EF080();
    return 1;
}

/* fn_8000CC84 - 0x8000CC84 | size: 0x24 */
/* GSparty_GetOTID */
s32 fn_8000CC84(void) {
    fn_801EF0D4();
    return 1;
}

/* fn_8000CCA8 - 0x8000CCA8 | size: 0x28 */
/* GSparty_GetOTSID */
s32 fn_8000CCA8(void) {
    fn_8025FF18(0);
    return 1;
}

/* fn_8000CCD0 - 0x8000CCD0 | size: 0x28 */
/* GSparty_GetPersonality */
s32 fn_8000CCD0(void) {
    fn_8025FBCC(0);
    return 1;
}

/* fn_8000CCF8 - 0x8000CCF8 | size: 0x28 */
/* GSparty_GetEncryptionKey */
s32 fn_8000CCF8(void) {
    fn_80190528(0x3F0);
    return 1;
}

/* fn_8000CD20 - 0x8000CD20 | size: 0x30 */
/* GSparty_GetFormData */
s32 fn_8000CD20(void) {
    fn_800884BC(0x99, 0x9C, 0x9F);
    return 0;
}

/* fn_8000CD50 - 0x8000CD50 | size: 0xc8 */
/* GSparty_GetStatusFull -- complex multi-call with float arg */
#if 0
asm void fn_8000CD50(void) {
#include "src/game/gs_party_access_fn_8000CD50.inc"
}
#else
void testEvolution__Fv(void) {
    u32 val1, val2;
    struct { u32 buf[5]; u16 a; u16 b; } locals;

    val1 = fn_8012A5B0(NULL, 3, 0);
    if ((u8)fn_80123FBC(val1) == 0) { return; }
    val2 = fn_8012A5B0(NULL, 3, 1);
    if ((u8)fn_80123FBC(val2) == 0) { return; }
    fn_801C41C8(3, lbl_8047B6E8);
    fn_801C40F0(1);
    locals.a = 1;
    locals.b = 2;
    fn_8026132C(val1, val2, 1, &locals.a, 2, locals.buf);
    fn_801C41C8(2, lbl_8047B6E8);
    fn_801C40F0(1);
}
#endif

/* fn_8000CE18 - 0x8000CE18 | size: 0x44 */
/* GSparty_CureStatus -- call fn_800FF560, then GSthreadCreate with fn_8000CD50 as callback */
s32 fn_8000CE18(void) {
    u32 r;
    r = (u32)fn_800FF560();
    GSthreadCreate(1, r, 0x4000, 1, 1, (u32)testEvolution__Fv);
    return 0;
}

/* fn_8000CE5C - 0x8000CE5C | size: 0x74 */
/* GSparty_ApplyDamage -- get party slot, validate, call fn_80097A38 */
#pragma peephole off
#if 0
asm void fn_8000CE5C(void) {
#include "src/game/gs_party_access_fn_8000CE5C.inc"
}
#else
u32 fn_8000CE5C(void) {
    u32 val;
    val = fn_80129280(0, 2);
    if (val == 0) { return 0; }
    val = fn_8012AC08(val, 0);
    if ((u8)fn_80123FBC(val) == 0) { return 0; }
    fn_80097A38(val, 0x25);
    return 0;
}
#endif
#pragma peephole on

/* fn_8000CED0 - 0x8000CED0 | size: 0x28 */
/* GSparty_IsAlive */
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
#pragma peephole off
s32 fn_8000CED0(void) {
    fn_800FF730(0x391);
    return 0;
}
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on
#pragma peephole on

/* fn_8000CEF8 - 0x8000CEF8 | size: 0x70 */
/* GSparty_FullHeal -- switch on r3 input */
#pragma peephole off
#if 0
asm void fn_8000CEF8(void) {
#include "src/game/gs_party_access_fn_8000CEF8.inc"
}
#else
u32 fn_8000CEF8(s32 arg) {
    switch (arg) {
        case 0x110:
            fn_8001BDF4(1, 0, 0);
            break;
        case 0x111:
            fn_8001BDF4(2, 0, 0);
            break;
        default:
            fn_8001BDF4(2, 0, 0);
            break;
    }
    return 0;
}
#endif
#pragma peephole on

/* fn_8000CF68 - 0x8000CF68 | size: 0xf4 */
/* GSparty_GetBattleStats -- search table for match, call fn_8002DC6C */
#if 0
asm void fn_8000CF68(void) {
#include "src/game/gs_party_access_fn_8000CF68.inc"
}
#else
u32 fn_8000CF68(u32 arg) {
#pragma peephole off
    struct StatCopyBlk { u32 data[28]; };
    u32 local[28];
    u32 val;
    u32 *p;
    u32 idx;
    int i;
    int j;
    *(struct StatCopyBlk*)local = *(struct StatCopyBlk*)lbl_80266700;
    p = local;
    idx = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 6; j++) {
            if ((s32)arg == (s32)p[j * 2]) { goto idx_done; }
            idx++;
        }
        if ((s32)arg == (s32)p[12]) { goto idx_done; }
        p += 14;
        idx++;
    }
idx_done:
    if ((s32)idx >= 14) {
        return 0;
    }
    val = local[idx * 2 + 1];
    fn_8002DC6C(val);
    return 0;
}
#endif

/* fn_8000D05C - 0x8000D05C | size: 0xc0 */
/* GSparty_CalcStatModifiers -- table lookup with struct copy */
#if 0
asm void fn_8000D05C(void) {
#include "src/game/gs_party_access_fn_8000D05C.inc"
}
#else
#pragma peephole off
u32 fn_8000D05C(u32 arg) {
    u32 table[8];
    u32 idx = 0;
    u32 val;
    u32 *p = lbl_802666E0;
    table[0] = p[0];
    table[1] = p[1];
    table[2] = p[2];
    table[3] = p[3];
    table[4] = p[4];
    table[5] = p[5];
    table[6] = p[6];
    table[7] = p[7];
    if ((s32)arg == (s32)table[0]) { goto idx_done; }
    idx = 1;
    if ((s32)arg == (s32)table[2]) { goto idx_done; }
    idx = 2;
    if ((s32)arg == (s32)table[4]) { goto idx_done; }
    idx = 3;
    if ((s32)arg == (s32)table[6]) { goto idx_done; }
    idx = 4;
idx_done:
    if ((s32)idx >= 4) {
        return 0;
    }
    val = table[idx * 2 + 1];
    fn_80029760(val, 0);
    return 0;
}
#pragma peephole reset
#endif

/* fn_8000D11C - 0x8000D11C | size: 0x38 */
/* GSparty_GetCritRate -- conditional call based on global */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 fn_8000D11C(void) {
    s32 v = *(s32*)(lbl_804673F8 + 0x874);
    if (v != 0) {
        fn_800FF660();
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

/* fn_8000D154 - 0x8000D154 | size: 0x70 */
/* GSparty_GetAccuracy */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 fn_8000D154(void) {
    u32 r;
    r = fn_80102620(0xCF);
    if ((u8)r != 0) {
        fn_80102510(0xCF);
    } else {
        fn_801026A4(0xCF, 0, 0, 0, 1, 0);
        fn_80102868(0xCF, 0xC, 0xA);
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

/* fn_8000D1C4 - 0x8000D1C4 | size: 0x70 */
/* GSparty_GetEvasion */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 fn_8000D1C4(void) {
    u32 r;
    r = fn_80102620(0x78);
    if ((u8)r != 0) {
        fn_80102510(0x78);
    } else {
        fn_801026A4(0x78, 0, 0, 0, 1, 0);
        fn_80102868(0x78, 0xC, 0xA);
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

/* fn_8000D234 - 0x8000D234 | size: 0x5c */
/* GSparty_ResetStatStages */
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
#pragma push
#pragma peephole off
s32 fn_8000D234(void) {
    u32 r;
    r = fn_80102620(0x13);
    if ((u8)r != 0) {
        fn_80102510(0x13);
    }
    if (*(s32*)(lbl_804673F8 + 0x874) == 0) {
        fn_800FF730(0x3E7);
        _threadSwitch();
    }
    return 0;
}
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop
#pragma pop

