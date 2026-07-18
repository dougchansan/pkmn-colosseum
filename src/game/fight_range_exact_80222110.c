/**
 * @file fight_range_exact_80222110.c
 * @brief Exact pure-C fight-sequence island, 0x80222110 - 0x80222BD8.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_8047B614;
extern u8 lbl_80478D78[1];
extern u32 lbl_8047B618;
extern void* lbl_8047B62C;
extern u8 lbl_80379F58[];
extern void fn_80211B94(void*, void*, u8);
extern void* fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(
    u32, u8, u32, u32, u32);

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
u8 fn_80222110(u8 a, u8 b) {
    u8 flag;
    u8 r = a;

    if (a == 0xf || a == 0x27) {
        flag = 1;
    } else if (a == 0x16 || a == 0x2e) {
        flag = 0;
    }

    if (b == 1 || b == 4) {
        if (flag == 1) {
            r = 0x26;
        } else {
            r = 0x27;
        }
    }
    if (b == 2 || b == 5 || b == 7) {
        if (flag == 1) {
            r = 0x28;
        } else {
            r = 0x29;
        }
    }
    if (b == 3) {
        if (flag == 1) {
            r = 0x2a;
        } else {
            r = 0x2b;
        }
    }
    if (b == 6) {
        if (flag == 1) {
            r = 0x2c;
        } else {
            r = 0x3c;
        }
    }
    return r;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F54A4 fightFloorGetStatus
#define fn_8012640C pokemonGetStatus
void fn_802221EC(u32 a, u32 b, u32 c, u32 d) {
    extern u16 fn_801F54A4();
    extern u32 fn_8012640C();
    extern u8  fn_801DDD28(u32, u32, u32, u32);
    extern void fn_801DA9E8(u32, u32, u32);
    extern void fn_80265598(u32, u16, u32);
    extern u32 fn_801DA94C(u32, u16, u32);
    extern void _threadSwitch();
    extern void fn_801DA8C4(u32, u16, u32);
    extern void fn_8026532C(u32, u16, u32);
    u32 obj;
    u16 partyCount;

    partyCount = (u16)fn_801F54A4(0, 0, 0x14, 0);
    obj = (u32)fn_8012640C(b, 0, 0xee, 0);

    if (obj == 0) {
        return;
    }
    if ((u8)fn_801DDD28(obj, a, 4, 0) == 0) {
        return;
    }
    fn_801DA9E8(obj, a, 4);

    if ((u8)d == 1) {
        fn_80265598(b, partyCount, 1);
    }
    if ((u8)c != 1) {
        return;
    }

    c = (u16)a;
    for (;;) {
        if ((u8)fn_801DA94C(obj, c, 4) == 0) {
            break;
        }
        _threadSwitch();
    }
    fn_801DA8C4(obj, c, 4);

    if ((u8)d == 1) {
        fn_8026532C(b, partyCount, 0);
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void fn_802222F4(void)

{
    extern void fn_801F025C();
    extern void fn_80221104();
  u8* pc;
  void* ptr1;
  u8 uVar1;
  u16 uVar3;

  fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  pc = lbl_8047B610;
  ptr1 = *(void**)(pc + 2);
  if (ptr1 != (void *)0) {
    uVar1 = *(u8*)ptr1;
  }
  else {
    uVar1 = 0;
  }
  if (*(u16**)(pc + 6) != (void *)0) {
    uVar3 = **(u16**)(pc + 6);
  }
  else {
    uVar3 = 0;
  }
  fn_80221104(*(u8 *)(pc + 1),uVar1,uVar3);
  lbl_8047B610 += 10;
  return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
void fn_80222370(void)

{
    extern void fn_801F025C();
    extern void fn_80221104();
  u16 uVar1;
  volatile u8* pc;
  u16* value;
  u32 arg2;

  fn_801F025C(*(u8 *)(lbl_8047B610 + 1),0);
  pc = lbl_8047B610;
  value = *(u16 **)(pc + 3);
  if (value != (void *)0) {
    uVar1 = *value;
  }
  else {
    uVar1 = 0;
  }
  arg2 = pc[2];
  fn_80221104(pc[1],arg2,uVar1);
  lbl_8047B610 += 7;
  return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_ALERTEND(void) { lbl_8047B614 = 2; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_802223E0(void) {
    if (fightFloorGetFightOutPokemonPtrAryPokemonTokuseiDataIdFirst(0, lbl_8047B610[1], 0, 0, 0) != NULL) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 2);
    } else {
        lbl_8047B610 += 6;
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_80207AE0 fightOutPokemonIsZokuseiDataId
void fn_80222438(void) {
    extern void* fn_801F025C();
    extern u8 fn_80207AE0();
    void* obj = (void*)fn_801F025C(lbl_8047B610[1], 0);
    if (fn_80207AE0(obj, lbl_8047B610[2]) == 1) {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 3);
    } else {
        lbl_8047B610 += 7;
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80222494(void) {
    fn_80211B94(lbl_8047B62C, *(void**)(lbl_8047B610 + 1), 0);
    lbl_8047B610 += 5;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_SPEABIEND(void) { lbl_8047B614 = 2; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_SEQEND(void) { lbl_8047B614 = 1; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_WAZAEND(void) { lbl_8047B614 = 1; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void WS_SEQRET(void) { lbl_8047B614 = 2; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
u8* fn_80222500(void) {
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 2;
    return pc;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
u8* fn_80222510(void) { return lbl_8047B610++; }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F000C fightMainWaitFrame
void fn_80222520(void) {
    extern void fn_801F000C();
    fn_801F000C(*(u16*)(lbl_8047B610 + 1));
    lbl_8047B610 += 3;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma opt_propagation off
void fn_80222554(void)
{
    u8* t = lbl_8047B610;
    u32 mask = 0xffffffff;
    u32 val = *(u32 *)(t + 5);
    u32 old;
    u32 *ptr = *(u32 **)(t + 1);
    old = *ptr;
    mask = val ^ mask;
    mask = old & mask;
    *ptr = mask;
    lbl_8047B610 += 9;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80222584(void) {
    u16* dst = *(u16**)(lbl_8047B610 + 1);
    *dst &= *(u16*)(lbl_8047B610 + 5) ^ 0xFFFF;
    lbl_8047B610 += 7;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_802225B0(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst &= *(u8*)(lbl_8047B610 + 5) ^ 0xFF;
    lbl_8047B610 += 6;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_802225DC(void) {
    u32* dst = *(u32**)(lbl_8047B610 + 1);
    *dst |= *(u32*)(lbl_8047B610 + 5);
    lbl_8047B610 += 9;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80222604(void) {
    u16* dst = *(u16**)(lbl_8047B610 + 1);
    *dst |= *(u16*)(lbl_8047B610 + 5);
    lbl_8047B610 += 7;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_8022262C(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst |= *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_80222654(void)
{
    struct {
        u8* value;
    } dst;
    int i;
    u8* src;
    u8* offset;
    int count;

    count = lbl_8047B610[13];
    dst.value = *(u8**)(lbl_8047B610 + 1);
    src = *(u8**)(lbl_8047B610 + 5);
    offset = *(u8**)(lbl_8047B610 + 9);

    for (i = 0; i < count; i++) {
        *dst.value++ = src[i + offset[0]];
    }
    lbl_8047B610 += 14;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma opt_propagation off
void fn_802226A4(void)
{
    u8* pc = lbl_8047B610;
    int count;
    u32 rawDst;
    u32 rawSrc;
    struct {
        u8* value;
    } dst;
    struct {
        u8* value;
    } src;
    rawDst = *(u32*)(pc + 1);
    rawSrc = *(u32*)(pc + 5);
    count = pc[9];
    dst.value = (u8*)rawDst;
    src.value = (u8*)rawSrc;

    while (count-- > 0) {
        *dst.value++ = *src.value++;
    }
    lbl_8047B610 += 10;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_802226EC(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst -= *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80222714(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst += *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_8022273C(void) {
    u8* dst = *(u8**)(lbl_8047B610 + 1);
    *dst = *(u8*)(lbl_8047B610 + 5);
    lbl_8047B610 += 6;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_8022275C(void)
{
#pragma optimization_level 3
    u8 count;
    u8* pc;
    u8* a;
    u8* b;
    u32 next;
    u8 i;
    u8 matches;

    count = lbl_8047B610[9];
    pc = lbl_8047B610;
    matches = 0;
    i = 0;
    a = *(u8**)(pc + 1);
    b = *(u8**)(pc + 5);
    next = *(u32*)(pc + 10);

    while (i < count) {
        if (*a == *b) {
            matches++;
        }
        a++;
        b++;
        i++;
    }
    if (matches != count) {
        lbl_8047B610 = (u8*)next;
    } else {
        lbl_8047B610 += 14;
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimize_for_size on
#pragma optimization_level 3
#pragma opt_propagation off
void fn_802227D4(void)
{
    u8* pc = lbl_8047B610;
    u32 rawCount = pc[9];
    u8* a = *(u8**)(pc + 1);
    u8* b = *(u8**)(pc + 5);
    u8 count = (u8)rawCount;
    u32 next = *(u32*)(pc + 10);
    u8 i = 0;

    while (i < count) {
        if (*a != *b) {
            lbl_8047B610 += 14;
            break;
        }
        a++;
        b++;
        i++;
    }
    if ((u8)(s8)i != (u8)(s8)rawCount) {
        return;
    }
    lbl_8047B610 = (u8*)next;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80222844(void* ctx, u32 param1, u32 param2, u32 param3) {
#pragma optimization_level 4
    int sel;
    u32 *base;
    u32 *pa;
    u32 vb;
    u32 nxt;

    base = *(u32**)&lbl_8047B610;
    sel = *(u8*)((u8*)base + 0x1);
    pa = *(u32**)((u8*)base + 0x2);
    vb = *(u32*)((u8*)base + 0x6);
    nxt = *(u32*)((u8*)base + 0xA);
    *(u32*)&lbl_8047B610 = (u32)base + 0xe;
    switch (sel) {
    case 0:
        if (*pa != vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 1:
        if (*pa == vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 2:
        if (*pa <= vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 3:
        if (*pa >= vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 4:
        if ((*pa & vb) == 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 5:
        if ((*pa & vb) != 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    }
    return;
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimization_level 4
#pragma opt_propagation off
void fn_8022290C(void) {
    int sel;
    u32 vb;
    u8* base;
    u32 pa;
    u8* nxt;

    base = lbl_8047B610;
    sel = base[1];
    pa = *(u32*)(base + 2);
    vb = *(u16*)(base + 6);
    nxt = *(u8**)(base + 8);
    lbl_8047B610 = base + 12;
    switch (sel) {
    case 0:
        if (*(u16*)pa != (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 1:
        if (*(u16*)pa == (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 2:
        if (*(u16*)pa <= (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 3:
        if (*(u16*)pa >= (u16)(s16)vb) return;
        lbl_8047B610 = nxt;
        return;
    case 4:
        if ((*(u16*)pa & (u16)(s16)vb) == 0) return;
        lbl_8047B610 = nxt;
        return;
    case 5:
        if ((*(u16*)pa & (u16)(s16)vb) != 0) return;
        lbl_8047B610 = nxt;
        return;
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimization_level 4
#pragma opt_propagation off
void fn_802229EC(void) {
    int sel;
    u32 vb;
    u32 base;
    u32 pa;
    u32 nxt;

    base = *(u32*)&lbl_8047B610;
    sel = *(u8*)(base + 1);
    pa = *(u32*)(base + 2);
    vb = *(u8*)(base + 6);
    nxt = *(u32*)(base + 7);
    *(u32*)&lbl_8047B610 = base + 11;
    switch (sel) {
    case 0:
        if (*(u8*)pa != (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 1:
        if (*(u8*)pa == (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 2:
        if (*(u8*)pa <= (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 3:
        if (*(u8*)pa >= (u8)(s8)vb) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 4:
        if ((*(u8*)pa & (u8)(s8)vb) == 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    case 5:
        if ((*(u8*)pa & (u8)(s8)vb) != 0) return;
        *(u32*)&lbl_8047B610 = nxt;
        return;
    }
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
void fn_80222ACC(void) { lbl_8047B610 = *(u8**)(lbl_8047B610 + 1); }

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#pragma optimize_for_size on
void fn_80222ADC(void) {
    extern u32 fightTargetGetPtrAsNowFightType();
    extern u32 pokemonGetStatus();
    extern s32 wazaGetStatus();
    extern void wazaSetStatus();
    u32 ctx1 = fightTargetGetPtrAsNowFightType(0x11, 0);
    u32 fieldD9 = pokemonGetStatus(ctx1, 0, 0xD9, 0);
    s16 val = (u8)wazaGetStatus(fieldD9, 0, 0x31, 0);

    val--;

    if (val < 0) {
        val = 0;
    }
    if (val == 0) {
        lbl_8047B610 = lbl_8047B610 + 5;
    } else {
        lbl_8047B610 = *(u8**)(lbl_8047B610 + 1);
    }
    wazaSetStatus(fieldD9, 0, 0x31, 0, val);
}

#pragma optimize_for_size reset
#pragma dont_inline reset
#pragma optimization_level reset
#pragma opt_propagation reset
#define fn_801F025C fightTargetGetPtrAsNowFightType
#define fn_8012640C pokemonGetStatus
#define fn_8011BBD8 wazaSetStatus
void fn_80222B7C(void) {
    extern u32 fn_801F025C();
    extern u32 fn_8012640C();
    extern void fn_8011BBD8();
    u32 ctx1 = fn_801F025C(0x11, 0);
    u32 fieldD9 = fn_8012640C(ctx1, 0, 0xd9, 0);
    fn_8011BBD8(fieldD9, 0, 0x31, 0, *(u8*)(lbl_8047B610 + 1));
    lbl_8047B610 = lbl_8047B610 + 2;
}
