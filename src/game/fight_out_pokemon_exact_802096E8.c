/**
 * @file fight_out_pokemon_exact_802096E8.c
 * @brief Exact fightWaza/fightItem accessor island, address range
 *        0x802096E8-0x8020A8E0, 64 functions.
 *
 * OutPokemon/Pokemon field accessors, sequence/status writers, and
 * damage-calc support the seq/waza layers call into (statusGetStatus,
 * fadeEffectGetRandom callers, etc). Corresponds to XD's
 * fight.cpp fightOutPokemon+fightPokemon cluster (0x80200644-0x80208288).
 */

#include "game/colosseum.h"
#include "game/trainer.h"
#include "game/pokemon.h"

typedef struct ColosseumEventRow6 {
    u8 mode;
    u8 field_01;
    u16 eventIndex;
    u16 nextIndex;
} ColosseumEventRow6;

typedef struct ColosseumEventSubRow {
    u8 valueMode;
    u8 scaleMode;
    s16 scaleNumerator;
    s16 scaleDenominator;
    u16 minValue;
    u16 maxValue;
} ColosseumEventSubRow;

typedef struct ColosseumEventPairRow {
    u8 resultFuncId;
    u8 field_01;
    u16 firstLinkIndex;
    ColosseumEventSubRow slots[2];
} ColosseumEventPairRow;

typedef struct StatusIdTable7 {
    u16 id[7];
} StatusIdTable7;

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* pokemonGetStatus();
extern u32   pokemonSetStatus();
extern void  pokemonGrowBasisStatus();
extern u32   itemGetStatus();
extern void  fn_80119ED0(void);
extern void  fn_80121ADC(void);
extern void  fn_8011B67C(void);
extern void  pokemonGetSoubiItemDataId(void);
extern void* fightActionGetPri(void* p);
extern void  wazaGetStatus(void);

/* SDA table pointers for event data arrays */
extern u32 lbl_80478D38;   /* Event table count */
extern ColosseumEventRow6 lbl_80478D30[]; /* Event table base (6 bytes per entry) */
extern u32 lbl_80478D28; /* Pair-row table count */
extern ColosseumEventPairRow lbl_80375A08[]; /* 0x18-byte pair rows */

/* Retail marks every function in this physical island active. */
#pragma force_active on

/* Address: 0x802096E8 | Size: 0xe0 | Ghidra import */
#pragma push
#pragma peephole off
u32 fightWazaIsHit(void* ctx)
{
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
  u16 sVar2;
  u32 uVar1;
  u8 cVar3;

  sVar2 = fn_80119ED0(0x40);
  if (sVar2 != 0x2a) {
    cVar3 = 0;
  }
  else {
    cVar3 = fn_8011B67C(ctx,0x40);
  }
  if (cVar3 == 1) {
    uVar1 = 0;
  }
  else {
    sVar2 = fn_80119ED0(0x43);
    if (sVar2 != 0x2a) {
      cVar3 = 0;
    }
    else {
      cVar3 = fn_8011B67C(ctx,0x43);
    }
    if (cVar3 == 1) {
      uVar1 = 0;
    }
    else {
      sVar2 = fn_80119ED0(0x45);
      if (sVar2 != 0x2a) {
        cVar3 = 0;
      }
      else {
        cVar3 = fn_8011B67C(ctx,0x45);
      }
      if (cVar3 == 1) {
        uVar1 = 0;
      }
      else {
        uVar1 = 1;
      }
    }
  }
  return uVar1;
}
#pragma pop

/* 0x802097C8 | size: 0x54 | small */
#pragma push
#pragma peephole on
void fightWazaWriteJoutaiDataId(u32 param_1, u32 param_2, u32 param_3) {
    extern u32 fn_80119ED0(u32);
    extern void fn_8011B2C0(u32, u32, u32);
    if ((fn_80119ED0(param_2) & 0xFFFF) == 0x2A) {
        fn_8011B2C0(param_1, param_2, param_3);
    }
}
#pragma pop

/* Address: 0x8020981C | Size: 0x54 | Ghidra import */
#pragma push
#pragma scheduling on
#pragma peephole on
u32 fightWazaCheckWriteJoutaiDataId(void* ctx, u32 param)

{
    extern u16 fn_80119ED0();
    extern u32 fn_8011B444();
  u16 sVar2;
  u32 uVar1;

  sVar2 = fn_80119ED0(param);
  if (sVar2 != 0x2a) {
    uVar1 = 0;
  }
  else {
    uVar1 = fn_8011B444(ctx,param);
  }
  return uVar1;
}
#pragma pop

/* Address: 0x80209870 | Size: 0x9c | Ghidra import */
u32 fightWazaIsJoutaiSousai(void* ctx)

{
    extern u16 fn_80119ED0();
    extern u8 fn_8011B67C();
  u16 sVar1;
  u8 cVar2;

  sVar1 = fn_80119ED0(0x41);
  if (sVar1 != 0x2a) {
    cVar2 = 0;
  }
  else {
    cVar2 = fn_8011B67C(ctx,0x41);
  }
  if (cVar2 == 1) {
    sVar1 = fn_80119ED0(0x42);
    if (sVar1 != 0x2a) {
      cVar2 = 0;
    }
    else {
      cVar2 = fn_8011B67C(ctx,0x42);
    }
    if (cVar2 == 1) {
      return 1;
    }
  }
  return 0;
}

/* 0x8020990C | size: 0x54 */
#pragma push
#pragma peephole on
#pragma scheduling on
u32 fightWazaIsJoutaiDataId(void* ctx, u32 param) {
    extern u32 fn_80119ED0();
    extern u32 fn_8011B67C();
    if ((fn_80119ED0(param) & 0xFFFF) != 0x2A) {
        return 0;
    }
    return fn_8011B67C(ctx, param);
}
#pragma pop

/* 0x80209960 | size: 0x4C | small */
#pragma push
#pragma scheduling on
#pragma peephole on
void fightWazaInitJoutaiDataId(void* ctx, u32 param) {
    extern u32 fn_80119ED0();
    extern void fn_8011B788();
    if ((fn_80119ED0(param) & 0xFFFF) == 0x2a) {
        fn_8011B788(ctx, param);
    }
}
#pragma pop

/* Address: 0x802099AC | Size: 0x270 | Ghidra import */
#pragma push
#pragma peephole on
void fightWazaCreate(void* p1, s8 p2, u32 p3, u16 p4, u8 p5) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    u16 sVar2;

    if (p1 != 0) {
        if (p1) {
            wazaSetStatus(p1, 0, 0x26, 0, (void*)0xffffffff);
            wazaSetStatus(p1, 0, 0x27, 0, 0);
            wazaSetStatus(p1, 0, 0x28, 0, 0);
            wazaSetStatus(p1, 0, 0x29, 0, 0);
            fn_8011B950(wazaGetStatus(p1, 0, 0x2a, 0), 9);
            sVar2 = fn_80119ED0(0x3f);
            if (sVar2 == 0x2a) {
                fn_8011B2C0(p1, 0x3f, 0);
            }
            wazaSetStatus(p1, 0, 0x2b, 0, 1);
            wazaSetStatus(p1, 0, 0x2c, 0, 1);
            wazaSetStatus(p1, 0, 0x2d, 0, 0);
            wazaSetStatus(p1, 0, 0x2e, 0, 0);
            wazaSetStatus(p1, 0, 0x2f, 0, 0);
            wazaSetStatus(p1, 0, 0x30, 0, 9);
            wazaSetStatus(p1, 0, 0x31, 0, 0);
            wazaSetStatus(p1, 0, 0x32, 0, 0);
        }
        wazaSetStatus(p1, 0, 0x26, 0, (s32)p2);
        wazaSetStatus(p1, 0, 0x29, 0, p4);
        wazaSetStatus(p1, 0, 0x27, 0, p3 & 0xffff);
        wazaSetStatus(p1, 0, 0x28, 0, p3 & 0xffff);
        wazaSetStatus(p1, 0, 0x2f, 0, (u16)wazaGetStatus(0, p3, 7, 0));
        wazaSetStatus(p1, 0, 0x30, 0, (u16)wazaGetStatus(0, p3, 3, 0));
        wazaSetStatus(p1, 0, 0x32, 0, p5);
    }
}
#pragma pop

/* Address: 0x80209C1C | Size: 0x98 | Ghidra import */
#pragma push
#pragma peephole on
#pragma scheduling on
void fightWazaSetUseWazaStatus(u32 r3, u32 r4)

{
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
  u16 uVar1;

  wazaSetStatus(r3,0,0x28,0,r4 & 0xffff);
  uVar1 = (u16)wazaGetStatus(0,r4,7,0);
  wazaSetStatus(r3,0,0x2f,0,uVar1);
  uVar1 = (u16)wazaGetStatus(0,r4,3,0);
  wazaSetStatus(r3,0,0x30,0,uVar1);
  return;
}
#pragma pop

/* Address: 0x80209CB4 | Size: 0xdc | Ghidra import */
#pragma push
#pragma peephole on
u32 fightWazaCheckValid(void* ctx) {
    extern s32 wazaGetStatus(void* ctx, u32 p1, u32 p2, u32 p3);
    s32 iVar1;
    if (ctx == 0) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x27, 0);
    if (iVar1 == 0) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x27, 0);
    if (iVar1 == 0x163) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x28, 0);
    if (iVar1 == 0) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x28, 0);
    if (iVar1 == 0x163) {
        return 0;
    }
    iVar1 = wazaGetStatus(ctx, 0, 0x29, 0);
    return iVar1 != 0;
}
#pragma pop

/* 0x80209D90 | size: 0x188 */
#pragma push
#pragma peephole on
void fightWazaInit(void* r3) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void wazaSetStatus();
    extern void* wazaGetStatus();
    void* ctx;

    if ((ctx = r3) == NULL) { return; }
    wazaSetStatus(ctx, 0, 0x26, 0, (u32)-1);
    wazaSetStatus(ctx, 0, 0x27, 0, 0);
    wazaSetStatus(ctx, 0, 0x28, 0, 0);
    wazaSetStatus(ctx, 0, 0x29, 0, 0);
    fn_8011B950(wazaGetStatus(ctx, 0, 0x2A, 0), 9);
    if (fn_80119ED0(0x3F) == 0x2A) {
        fn_8011B2C0(ctx, 0x3F, 0);
    }
    wazaSetStatus(ctx, 0, 0x2B, 0, 1);
    wazaSetStatus(ctx, 0, 0x2C, 0, 1);
    wazaSetStatus(ctx, 0, 0x2D, 0, 0);
    wazaSetStatus(ctx, 0, 0x2E, 0, 0);
    wazaSetStatus(ctx, 0, 0x2F, 0, 0);
    wazaSetStatus(ctx, 0, 0x30, 0, 9);
    wazaSetStatus(ctx, 0, 0x31, 0, 0);
    wazaSetStatus(ctx, 0, 0x32, 0, 0);
}
#pragma pop

/* 0x80209F18 | size: 0xa8 */
#pragma push
#pragma peephole on
void fightWazaInitLoop(void* ctx) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern void fn_8011B950();
    extern void wazaSetStatus();
    extern u32 wazaGetStatus();
    u32 val;
    u16 typeId;
    val = wazaGetStatus(ctx, 0, 0x2a, 0);
    fn_8011B950(val, 9);
    typeId = fn_80119ED0(0x3f);
    if (typeId == 0x2a) {
        fn_8011B2C0(ctx, 0x3f, 0);
    }
    wazaSetStatus(ctx, 0, 0x2b, 0, 1);
    wazaSetStatus(ctx, 0, 0x2c, 0, 1);
}
#pragma pop

/* 0x80209FAC | size: 0x64 */
#pragma push
#pragma scheduling on
#pragma peephole on
void fightWazaInitJoutai(void* ctx) {
    extern u16 fn_80119ED0();
    extern void fn_8011B2C0();
    extern u32 fn_8011B950();
    extern u32 wazaGetStatus();
    u32 val = wazaGetStatus(ctx, 0, 0x2A, 0);
    fn_8011B950(val, 9);
    if (fn_80119ED0(0x3F) == 0x2A) {
        fn_8011B2C0(ctx, 0x3F, 0);
    }
}
#pragma pop

/* 0x8020A010 | size: 0x18 */
u32 fightWazaHitKakurituDataBiosGetWaru(u8* ptr) {
    if (ptr == NULL) { return 1; }
    return ptr[0x1];
}

/* 0x8020A028 | size: 0x18 */
u32 fightWazaHitKakurituDataBiosGetKake(u8* ptr) {
    if (ptr == NULL) { return 1; }
    return ptr[0x0];
}

/* fightWazaHitKakurituDataBiosGetPtr | Size: 0x28 | Look up 2-byte entry in table */
#pragma push
#pragma peephole on
u16* fightWazaHitKakurituDataBiosGetPtr(u16 index) {
    extern u8 lbl_80375DD0[];
    extern u32 lbl_80478D70;
    u16* result = (u16*)&lbl_80375DD0[index * 2];
    if (index < lbl_80478D70) {
        return result;
    }
    return NULL;
}
#pragma pop

/* Address: 0x8020A068 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaCriticalDataBiosGetBunbo(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* fightWazaCriticalDataBiosGetPtr | Size: 0x24 | Look up byte in table with bounds check */
#pragma push
#pragma peephole on
u8* fightWazaCriticalDataBiosGetPtr(u16 index) {
    extern u8 lbl_80478D58[];
    extern u32 lbl_80478D60;
    u8* result = &lbl_80478D58[index];
    if (index < lbl_80478D60) {
        return result;
    }
    return NULL;
}
#pragma pop

/* Address: 0x8020A0A4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetAutoMakeFlag(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA9]) = val;
}

/* Address: 0x8020A0B4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetKaisuu(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0xA8]) = val;
}

/* Address: 0x8020A0C4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetZokusei(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA6]) = val;
}

/* Address: 0x8020A0D4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetIryoku(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0xA4]) = val;
}

/* Address: 0x8020A0E4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetHitDamage(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0xA0]) = val;
}

/* Address: 0x8020A0F4 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetDamage(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x9C]) = val;
}

/* Address: 0x8020A104 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetDamageValue(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x99]) = val;
}

/* Address: 0x8020A114 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetCritical(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x98]) = val;
}

/* Address: 0x8020A124 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetTargetDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x6]) = val;
}

/* Address: 0x8020A134 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetUseWazaDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x4]) = val;
}

/* Address: 0x8020A144 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetMotoWazaDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x8020A154 | Size: 0x10 | Pattern: nullcheck_setter */
void fightWazaBiosSetWazaBanme(u8* ptr, u8 val) {
    if (ptr == NULL) { return; }
    *(u8*)(&ptr[0x0]) = val;
}

/* Address: 0x8020A164 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetAutoMakeFlag(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA9]);
}

/* Address: 0x8020A17C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetKaisuu(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0xA8]);
}

/* Address: 0x8020A194 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetZokusei(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA6]);
}

/* Address: 0x8020A1AC | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetIryoku(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0xA4]);
}

/* Address: 0x8020A1C4 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightWazaBiosGetHitDamage(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0xA0]);
}

/* Address: 0x8020A1DC | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightWazaBiosGetDamage(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x9C]);
}

/* Address: 0x8020A1F4 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetDamageValue(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x99]);
}

/* Address: 0x8020A20C | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetCritical(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x98]);
}

/* 0x8020A224 | size: 0x34 | small */
#pragma push
#pragma peephole on
void* fightWazaBiosGetJoutaiPtr(void* base, u16 index) {
    if (base == 0) return 0;
    if (index >= 9) return 0;
    return (u8*)base + 0x8 + index * 16;
}
#pragma pop

/* Address: 0x8020A258 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetTargetDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x6]);
}

/* Address: 0x8020A270 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetUseWazaDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x4]);
}

/* Address: 0x8020A288 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightWazaBiosGetMotoWazaDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020A2A0 | Size: 0x18 | Pattern: nullcheck_getter */
u8 fightWazaBiosGetWazaBanme(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u8*)(&ptr[0x0]);
}

/* 0x8020A2B8 | size: 0x40 -- copy 0xAC bytes (43 u32s) */
void fightWazaBiosCopy(u32* dst, u32* src) {
    struct CopyBlk8020A2B8 { u32 data[43]; };
    if (dst == 0) return;
    if (src == 0) return;
    *(struct CopyBlk8020A2B8*)dst = *(struct CopyBlk8020A2B8*)src;
}

/* Address: 0x8020A2F8 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetBuff(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x8]) = val;
}

/* Address: 0x8020A308 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetCount(u8* ptr, u32 val) {
    if (ptr == NULL) { return; }
    *(u32*)(&ptr[0x4]) = val;
}

/* Address: 0x8020A318 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetTargetDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x2]) = val;
}

/* Address: 0x8020A328 | Size: 0x10 | Pattern: nullcheck_setter */
void fightItemBiosSetItemDataId(u8* ptr, u16 val) {
    if (ptr == NULL) { return; }
    *(u16*)(&ptr[0x0]) = val;
}

/* Address: 0x8020A338 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightItemBiosGetBuff(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x8]);
}

/* Address: 0x8020A350 | Size: 0x18 | Pattern: nullcheck_getter */
u32 fightItemBiosGetCount(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u32*)(&ptr[0x4]);
}

/* Address: 0x8020A368 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightItemBiosGetTargetDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x2]);
}

/* Address: 0x8020A380 | Size: 0x18 | Pattern: nullcheck_getter */
u16 fightItemBiosGetItemDataId(u8* ptr) {
    if (ptr == NULL) { return 0; }
    return *(u16*)(&ptr[0x0]);
}

#pragma push
#pragma peephole on
static inline void fightItemInitInline(u8* item)
{
    extern void fn_80142B24();

    if (item != NULL) {
        fn_80142B24(item, 0, 0x1E, 0, 0);
        fn_80142B24(item, 0, 0x1F, 0, 0);
        fn_80142B24(item, 0, 0x20, 0, (void*)-1);
        fn_80142B24(item, 0, 0x21, 0, 0);
    }
}

/* Address: 0x8020A398 | Size: 0xe0 */
void fightItemCreate(u8* item, u16 itemDataId, u16 targetDataId, u32 count)
{
    extern void fn_80142B24();
    u8* ctx;

    if ((ctx = item) != NULL) {
        fightItemInitInline(ctx);
        fn_80142B24(ctx, 0, 0x1E, 0, itemDataId);
        fn_80142B24(ctx, 0, 0x1F, 0, targetDataId);
        fn_80142B24(ctx, 0, 0x20, 0, count);
    }
}
#pragma pop

/* Address: 0x8020A478 | Size: 0x88 | Ghidra import */
#pragma push
#pragma peephole on
void fightItemInit(void* r3)
{
    extern void fn_80142B24();
    void* ctx;
    if ((ctx = r3) != NULL) {
        fn_80142B24(ctx, 0, 0x1e, 0, 0);
        fn_80142B24(ctx, 0, 0x1f, 0, 0);
        fn_80142B24(ctx, 0, 0x20, 0, (void*)0xffffffff);
        fn_80142B24(ctx, 0, 0x21, 0, 0);
    }
}
#pragma pop

/* 0x8020A500 | size: 0x40 */
u32 fn_8020A500(u16 idx) {
    ColosseumEventRow6* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = &lbl_80478D30[idx];
    }
    if (entry == NULL) { return 0; }
    return entry->nextIndex;
}

/* 0x8020A540 | size: 0x40 */
u32 fn_8020A540(u16 idx) {
    ColosseumEventRow6* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = &lbl_80478D30[idx];
    }
    if (entry == NULL) { return 0; }
    return entry->eventIndex;
}

/* 0x8020A580 | size: 0x40 */
u32 fn_8020A580(u16 idx) {
    ColosseumEventRow6* entry;
    idx = idx;
    if (idx >= lbl_80478D38) {
        entry = NULL;
    } else {
        entry = &lbl_80478D30[idx];
    }
    if (entry == NULL) { return 0; }
    return entry->mode;
}

/* 0x8020A5C0 | size: 0x70 */
s16 fn_8020A5C0(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->scaleDenominator;
}

/* 0x8020A630 | size: 0x70 */
s16 fn_8020A630(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->scaleNumerator;
}

/* 0x8020A6A0 | size: 0x70 */
u8 fn_8020A6A0(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->scaleMode;
}

/* 0x8020A710 | size: 0x70 */
u16 fn_8020A710(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->maxValue;
}

/* 0x8020A780 | size: 0x70 */
u16 fn_8020A780(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->minValue;
}

/* 0x8020A7F0 | size: 0x70 */
u8 fn_8020A7F0(u16 index, u16 slot) {
    ColosseumEventPairRow* entry;
    ColosseumEventSubRow* sub;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        sub = NULL;
    } else if (slot >= 2) {
        sub = NULL;
    } else {
        sub = &entry->slots[slot];
    }
    if (sub == NULL) {
        return 0;
    }
    return sub->valueMode;
}

/* fn_8020A860 | Size: 0x40 | Look up u16 field at offset 2 in 0x18-byte table */
u16 fn_8020A860(u16 index) {
    ColosseumEventPairRow* entry;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry->firstLinkIndex;
}

/* fn_8020A8A0 | Size: 0x40 | Look up u8 field at offset 0 in 0x18-byte table */
u8 fn_8020A8A0(u16 index) {
    ColosseumEventPairRow* entry;
    if (index >= lbl_80478D28) {
        entry = NULL;
    } else {
        entry = &lbl_80375A08[index];
    }
    if (entry == NULL) {
        return 0;
    }
    return entry->resultFuncId;
}
