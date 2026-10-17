/**
 * @file fight_range_exact_802136A4.c
 * @brief Strict fight-sequence bytecode/status island, 0x802136A4 - 0x80213A78.
 */
#include "dolphin/types.h"

extern u8* lbl_8047B610;
extern u8 lbl_8047B614;

extern void fightOutPokemonInitOneSelfTurn(void*);
extern void statusSetStatus(u8, u32, u32, u16, u32, u32);
extern u32 statusGetStatus(u8, u32, u32, u16, u32);

s32 _fightOutPokemonInitOneSelfTurnSub(void* p)
{
    fightOutPokemonInitOneSelfTurn(p);
    return 1;
}

void WS_STATUS_SET32(void)
{
    int pc = (int)lbl_8047B610;
    u32* p2 = *(u32**)(pc + 2);
    u8 id = *(u8*)(pc + 1);
    u16 value = *(u16*)(pc + 0xa);
    u8* p6 = *(u8**)(pc + 6);
    u8* pC = *(u8**)(pc + 0xc);
    u32* p10 = *(u32**)(pc + 0x10);
    u32 a2 = p2 == NULL ? 0 : *p2;
    u32 a3 = p6 == NULL ? 0 : *p6;
    u32 a5 = pC == NULL ? 0 : *pC;

    statusSetStatus(id, a2, a3, value, a5, *p10);
    lbl_8047B610 += 0x14;
}

void WS_STATUS_SET16(void)
{
    int pc = (int)lbl_8047B610;
    u32* p2 = *(u32**)(pc + 2);
    u8 id = *(u8*)(pc + 1);
    u16 value = *(u16*)(pc + 0xa);
    u8* p6 = *(u8**)(pc + 6);
    u8* pC = *(u8**)(pc + 0xc);
    u16* p10 = *(u16**)(pc + 0x10);
    u32 a2 = p2 == NULL ? 0 : *p2;
    u32 a3 = p6 == NULL ? 0 : *p6;
    u32 a5 = pC == NULL ? 0 : *pC;

    statusSetStatus(id, a2, a3, value, a5, *p10);
    lbl_8047B610 += 0x14;
}

void WS_STATUS_SET8(void)
{
    int pc = (int)lbl_8047B610;
    u32* p2 = *(u32**)(pc + 2);
    u8 id = *(u8*)(pc + 1);
    u16 value = *(u16*)(pc + 0xa);
    u8* p6 = *(u8**)(pc + 6);
    u8* pC = *(u8**)(pc + 0xc);
    u8* p10 = *(u8**)(pc + 0x10);
    u32 a2 = p2 == NULL ? 0 : *p2;
    u32 a3 = p6 == NULL ? 0 : *p6;
    u32 a5 = pC == NULL ? 0 : *pC;

    statusSetStatus(id, a2, a3, value, a5, *p10);
    lbl_8047B610 += 0x14;
}

void WS_STATUS_GET32(void)
{
    int pc = (int)lbl_8047B610;
    u32* p2 = *(u32**)(pc + 2);
    u8 id = *(u8*)(pc + 1);
    u16 value = *(u16*)(pc + 0xa);
    u8* p6 = *(u8**)(pc + 6);
    u8* pC = *(u8**)(pc + 0xc);
    u32* p10 = *(u32**)(pc + 0x10);
    u32 a2 = p2 == NULL ? 0 : *p2;
    u32 a3 = p6 == NULL ? 0 : *p6;
    u32 a5 = pC == NULL ? 0 : *pC;

    *p10 = statusGetStatus(id, a2, a3, value, a5);
    lbl_8047B610 += 0x14;
}

void WS_STATUS_GET16(void)
{
    int pc = (int)lbl_8047B610;
    u32* p2 = *(u32**)(pc + 2);
    u8 id = *(u8*)(pc + 1);
    u16 value = *(u16*)(pc + 0xa);
    u8* p6 = *(u8**)(pc + 6);
    u8* pC = *(u8**)(pc + 0xc);
    u16* p10 = *(u16**)(pc + 0x10);
    u32 a2 = p2 == NULL ? 0 : *p2;
    u32 a3 = p6 == NULL ? 0 : *p6;
    u32 a5 = pC == NULL ? 0 : *pC;

    *p10 = (u16)statusGetStatus(id, a2, a3, value, a5);
    lbl_8047B610 += 0x14;
}

void WS_STATUS_GET8(void)
{
    int pc = (int)lbl_8047B610;
    u32* p2 = *(u32**)(pc + 2);
    u8 id = *(u8*)(pc + 1);
    u16 value = *(u16*)(pc + 0xa);
    u8* p6 = *(u8**)(pc + 6);
    u8* pC = *(u8**)(pc + 0xc);
    u8* p10 = *(u8**)(pc + 0x10);
    u32 a2 = p2 == NULL ? 0 : *p2;
    u32 a3 = p6 == NULL ? 0 : *p6;
    u32 a5 = pC == NULL ? 0 : *pC;

    *p10 = (u8)statusGetStatus(id, a2, a3, value, a5);
    lbl_8047B610 += 0x14;
}

void WS_GETEND(void)
{
    lbl_8047B614 = 1;
}

void WS_ITEMEND(void)
{
    lbl_8047B614 = 1;
}

u8* wsCmdF5(void)
{
    return lbl_8047B610++;
}

u8* wsCmdF4(void)
{
    return lbl_8047B610++;
}

u8* wsCmdF3(void)
{
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}

u8* wsCmdF2(void)
{
    return lbl_8047B610++;
}

u8* wsCmdF1(void)
{
    u8* pc = lbl_8047B610;
    lbl_8047B610 = pc + 5;
    return pc;
}
