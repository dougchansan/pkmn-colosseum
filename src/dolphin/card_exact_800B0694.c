/**
 * @file card_exact_800B0694.c
 * @brief CARDUnlock.c body, 0x800B0694 - 0x800B1788: ReadArrayUnlock,
 * DummyLen, __CARDUnlock, InitCallback and DoneCallback.
 *
 * The TU's first function, bitrev (0x800B0528), is linked in
 * sdk_exact_800B01AC.c. The unlock microcode (CardData, lbl_80312800) and
 * the rand state (next, lbl_80478A50) stay in their data units. Layout and
 * helpers follow the Dolphin SDK's CARDUnlock.c as decompiled in XD:
 * https://github.com/TeamOrre/xd-decomp/blob/4989794e6c6430684e033bc56f4bb97c9a921e73/src/dolphin/card/CARDUnlock.c
 *
 * RULE-EXCEPTION(user-approved): per-unit -inline noauto (see
 * docs/RULE_EXCEPTIONS.md). Built with -inline noauto and explicit inline helpers: under auto-inlining
 * InitCallback's frame is 8 bytes smaller than retail. DummyLen and
 * __CARDUnlock keep 8 more frame bytes than XD's form produces; the
 * volatile pad locals reproduce that (the earlier range carve needed the same).
 */

#define SDK_EXACT_800B0694_800B1788
#include "src/dolphin/sdk_range_800AE3F0.c"

extern u32 bitrev(u32 data);
extern s32 EXIImmEx(s32 chan, void* buf, s32 len, u32 mode); /* EXIImmEx */
extern u32 fn_800AE794(void);                                   /* DSPCheckMailToDSP */
extern void DSPSendMailToDSP(u32 mail);
extern u32 OSGetTick(void);
extern void __CARDMountCallback(s32 chan, s32 result);
extern s32 __CARDReadStatus(s32 chan, u8* status);
extern void DCFlushRange(void* addr, u32 length);
extern void DCInvalidateRange(void* addr, u32 length);
extern DSPTaskInfo* DSPAddTask(DSPTaskInfo* task);
void InitCallback(void* _task);
void DoneCallback(void* _task);

static inline int CARDRand(void)
{
    lbl_80478A50 = lbl_80478A50 * 0x41C64E6D + 0x3039;
    return (lbl_80478A50 >> 16) & 0x7FFF;
}

static inline void CARDSrand(u32 seed)
{
    lbl_80478A50 = seed;
}

static inline u32 exnor_1st(u32 data, u32 rshift)
{
    u32 wk;
    u32 work;
    u32 i;

    work = data;
    for (i = 0; i < rshift; i++) {
        wk = ~(work ^ (work >> 7) ^ (work >> 15) ^ (work >> 23));
        work = (work >> 1) | ((wk << 30) & 0x40000000);
    }
    return work;
}

static inline u32 exnor(u32 data, u32 lshift)
{
    u32 wk;
    u32 work;
    u32 i;

    work = data;
    for (i = 0; i < lshift; i++) {
        wk = ~(work ^ (work << 7) ^ (work << 15) ^ (work << 23));
        work = (work << 1) | ((wk >> 30) & 2);
    }
    return work;
}

s32 ReadArrayUnlock(s32 chan, u32 data, void* rbuf, s32 rlen, s32 mode)
{
    CARDControl* card;
    BOOL err;
    u8 cmd[5];

    card = &lbl_803FC620[chan];
    if (!EXISelect(chan, 0, 4)) {
        return -3;
    }

    data &= 0xFFFFF000;
    memset(cmd, 0, sizeof(cmd));
    cmd[0] = 0x52;
    if (mode == 0) {
        cmd[1] = (data >> 29) & 3;
        cmd[2] = (data >> 21) & 0xFF;
        cmd[3] = (data >> 19) & 3;
        cmd[4] = (data >> 12) & 0x7F;
    } else {
        cmd[1] = data >> 24;
        cmd[2] = data >> 16;
    }

    err = FALSE;
    err |= !EXIImmEx(chan, cmd, sizeof(cmd), 1);
    err |= !EXIImmEx(chan, (u8*)card->workArea + sizeof(CARDID),
                        *(u32*)((u8*)card + 0x14), 1);
    err |= !EXIImmEx(chan, rbuf, rlen, 0);
    err |= !EXIDeselect(chan);
    return err ? -3 : 0;
}

static inline u32 GetInitVal(void)
{
    u32 tmp;
    u32 tick;

    tick = OSGetTick();
    CARDSrand(tick);
    tmp = 0x7FEC8000;
    tmp |= CARDRand();
    tmp &= 0xFFFFF000;
    return tmp;
}

u32 DummyLen(void)
{
    u32 tick;
    u32 wk;
    s32 tmp;
    u32 max;
    /* RULE-EXCEPTION(user-approved): artificial local for frame size - see docs/RULE_EXCEPTIONS.md */
    volatile u32 pad[2];

    wk = 1;
    max = 0;
    tick = OSGetTick();
    CARDSrand(tick);

    tmp = CARDRand();
    tmp &= 0x1F;
    tmp += 1;
    while ((tmp < 4) && (max < 10)) {
        tick = OSGetTick();
        tmp = (s32)(tick << wk);
        wk++;
        if (wk > 16) {
            wk = 1;
        }
        CARDSrand((u32)tmp);
        tmp = CARDRand();
        tmp &= 0x1F;
        tmp += 1;
        max++;
    }

    if (tmp < 4) {
        tmp = 4;
    }
    return tmp;
}

s32 __CARDUnlock(s32 chan, u8 flashID[12])
{
    u32 init_val;
    u32 data;
    s32 dummy;
    s32 rlen;
    u32 rshift;
    u8 fsts;
    u32 wk, wk1;
    u32 Ans1 = 0;
    u32 Ans2 = 0;
    u32* dp;
    u8 rbuf[64];
    /* RULE-EXCEPTION(user-approved): artificial local for frame size - see docs/RULE_EXCEPTIONS.md */
    volatile u32 pad[1];
    u32 para1A = 0;
    u32 para1B = 0;
    u32 para2A = 0;
    u32 para2B = 0;
    CARDControl* card;
    DSPTaskInfo* task;
    CARDDecParam* param;
    u8* input;
    u8* output;

    card = &lbl_803FC620[chan];
    task = (DSPTaskInfo*)card->task;
    param = (CARDDecParam*)card->workArea;
    input = (u8*)((u8*)param + sizeof(CARDDecParam));
    input = (u8*)(((u32)input + 31) & ~31);
    output = input + 32;

    fsts = 0;
    init_val = GetInitVal();

    dummy = DummyLen();
    rlen = dummy;
    if (ReadArrayUnlock(chan, init_val, rbuf, rlen, 0) < 0) {
        return -3;
    }

    rshift = (u32)(dummy * 8 + 1);
    wk = exnor_1st(init_val, rshift);
    wk1 = ~(wk ^ (wk >> 7) ^ (wk >> 15) ^ (wk >> 23));
    card->scramble = (wk | ((wk1 << 31) & 0x80000000));
    card->scramble = bitrev(card->scramble);
    dummy = DummyLen();
    rlen = 20 + dummy;
    data = 0;
    if (ReadArrayUnlock(chan, data, rbuf, rlen, 1) < 0) {
        return -3;
    }

    dp = (u32*)rbuf;
    para1A = *dp++;
    para1B = *dp++;
    Ans1 = *dp++;
    para2A = *dp++;
    para2B = *dp++;
    para1A = (para1A ^ card->scramble);
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    para1B = (para1B ^ card->scramble);
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    Ans1 ^= card->scramble;
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    para2A = (para2A ^ card->scramble);
    rshift = 32;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    para2B = (para2B ^ card->scramble);
    rshift = (u32)(dummy * 8);
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    rshift = 32 + 1;
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    *(u32*)&input[0] = para2A;
    *(u32*)&input[4] = para2B;

    param->inputAddr = input;
    param->inputLength = 8;
    param->outputAddr = output;
    param->aramAddr = 0;

    DCFlushRange(input, 8);
    DCInvalidateRange(output, 4);
    DCFlushRange(param, sizeof(CARDDecParam));

    task->priority = 255;
    task->iram_mmem_addr = (u16*)((u32)lbl_80312800 - 0x80000000);
    task->iram_length = 0x160;
    task->iram_addr = 0;
    task->dsp_init_vector = 0x10;
    task->init_cb = InitCallback;
    task->res_cb = NULL;
    task->done_cb = DoneCallback;
    task->req_cb = NULL;
    DSPAddTask(task);

    dp = (u32*)flashID;
    *dp++ = para1A;
    *dp++ = para1B;
    *dp = Ans1;

    return 0;
}

void InitCallback(void* _task)
{
    s32 chan;
    CARDControl* card;
    DSPTaskInfo* task;
    CARDDecParam* param;

    task = _task;
    for (chan = 0; chan < 2; ++chan) {
        card = &lbl_803FC620[chan];
        if ((DSPTaskInfo*)card->task == task) {
            break;
        }
    }

    param = (CARDDecParam*)card->workArea;

    DSPSendMailToDSP(0xFF000000);
    while (fn_800AE794()) {
    }

    DSPSendMailToDSP((u32)param);
    while (fn_800AE794()) {
    }
}

void DoneCallback(void* _task)
{
    u8 rbuf[64];
    u32 data;
    s32 dummy;
    s32 rlen;
    u32 rshift;
    u8 unk;
    u32 wk, wk1;
    u32 Ans2;
    s32 chan;
    CARDControl* card;
    s32 result;
    DSPTaskInfo* task;
    CARDDecParam* param;
    u8* input;
    u8* output;

    task = _task;
    for (chan = 0; chan < 2; ++chan) {
        card = &lbl_803FC620[chan];
        if ((DSPTaskInfo*)card->task == task) {
            break;
        }
    }

    param = (CARDDecParam*)card->workArea;
    input = (u8*)((u8*)param + sizeof(CARDDecParam));
    input = (u8*)(((u32)input + 31) & ~31);
    output = input + 32;

    Ans2 = *(u32*)output;
    dummy = DummyLen();
    rlen = dummy;
    data = ((Ans2 ^ card->scramble) & 0xFFFF0000);
    if (ReadArrayUnlock(chan, data, rbuf, rlen, 1) < 0) {
        EXIUnlock(chan);
        __CARDMountCallback(chan, -3);
        return;
    }

    rshift = (u32)((dummy + 4 + *(u32*)((u8*)card + 0x14)) * 8 + 1);
    wk = exnor(card->scramble, rshift);
    wk1 = ~(wk ^ (wk << 7) ^ (wk << 15) ^ (wk << 23));
    card->scramble = (wk | ((wk1 >> 31) & 1));

    dummy = DummyLen();
    rlen = dummy;
    data = (((Ans2 << 16) ^ card->scramble) & 0xFFFF0000);
    if (ReadArrayUnlock(chan, data, rbuf, rlen, 1) < 0) {
        EXIUnlock(chan);
        __CARDMountCallback(chan, -3);
        return;
    }

    result = __CARDReadStatus(chan, &unk);
    if (!EXIProbe(chan)) {
        EXIUnlock(chan);
        __CARDMountCallback(chan, -3);
        return;
    }

    if (result == 0 && !(unk & 0x40)) {
        EXIUnlock(chan);
        result = -5;
    }

    __CARDMountCallback(chan, result);
}
