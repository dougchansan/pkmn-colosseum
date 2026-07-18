/**
 * @file musyx_hw_exact_80162858.c
 * @brief Exact MusyX hardware island, 0x80162858 - 0x80162FB0.
 */

#include "dolphin/types.h"

typedef struct PeopleFieldMoveSlot {
    u8 pad_00[0x1C];
    u32 field_1C;
    u8 pad_20[0x4];
    u32 flags_24[0x13];
    u16 field_70;
    u8 pad_72[0x1E];
    u8 field_90;
    u8 pad_91[0x3];
    u32 field_94;
    u32 field_98;
    u8 field_9C;
    u8 pad_9D[0x3];
    u8 field_A0;
    u8 pad_A1[0x2B];
    u16 field_CC;
    u16 field_CE;
    u16 field_D0;
    u16 field_D2;
    u8 field_D4;
    u8 pad_D5[0x13];
    u32 field_E8;
    u8 active;
    u8 field_ED;
    u8 field_EE;
    u8 pad_EF;
    u32 field_F0;
} PeopleFieldMoveSlot;

typedef union HwVolumeStudio {
    struct {
        u32 allocation;
        u8 pad_04[0x24];
        u32 auxAllocation;
        u8 pad_2C[0x24];
        u8 state;
        u8 isMaster;
        u8 numInputs;
        u8 pad_53;
        s32 type;
        u8 pad_58[0x64];
    } named;
    u8 bytes[0xBC];
} HwVolumeStudio;

typedef union HwVolumeInfo {
    struct {
        f32 volL;
        f32 volR;
        f32 volS;
        f32 volAuxAL;
        f32 volAuxAR;
        f32 volAuxAS;
        f32 volAuxBL;
        f32 volAuxBR;
        f32 volAuxBS;
    } named;
    f32 values[9];
} HwVolumeInfo;

typedef struct HwVolumeVoice {
    u8 pad_00[0x24];
    u32 changed[5];
    u32 pitch[5];
    u16 volL;
    u16 volR;
    u16 volS;
    u16 volLa;
    u16 volRa;
    u16 volSa;
    u16 volLb;
    u16 volRb;
    u16 volSb;
    u8 pad_5E[0x72];
    u16 itdShiftL;
    u16 itdShiftR;
    u8 pad_D4[0x10];
    struct {
        u8 pitch;
        u8 vol;
        u8 volA;
        u8 volB;
    } lastUpdate;
    u32 virtualSampleID;
    u8 state;
    u8 postBreak;
    u8 startupBreak;
    u8 studio;
    u32 flags;
} HwVolumeVoice;

typedef struct PeopleStudioState {
    u8 pad_00[0xAC];
    u32 field_AC;
    u32 field_B0;
    u32 field_B4;
    u32 field_B8;
} PeopleStudioState;

extern u32 lbl_8047B024;
extern u8 lbl_8047B050;
extern u16 lbl_80478BF8;
extern u16 lbl_80478C00;
extern volatile const u16 lbl_80273448[];
extern HwVolumeStudio lbl_80447E60[];
extern volatile const f32 lbl_8047D4D8;
extern f32 lbl_8047D4DC;

extern void salActivateVoice(u8* ptr, u8 studio);
extern void salDeactivateVoice(void* ptr);
extern void salActivateStudio(void);
extern void fn_8015AAA0(u32 studio);
extern u32 fn_8015D54C(void* studio, void* arg);
extern u32 fn_8015D5F4(void* studio, void* arg);
extern void salCalcVolume(u32 table, f32* out, f32 volume, u32 pan, u32 span,
                          f32 auxA, f32 auxB, u32 itd, u32 dpl2);
extern u32 aramGetStreamBufferAddress(u32 index, u32* out);
extern void aramUploadData(void* dst, void* src, u32 size, u32 type, u32 arg7,
                           u32 arg8);
extern void DCStoreRange(void* addr, u32 size);
extern void fn_80163CA8(void);
extern void aramFreeStreamBuffer(void);

#pragma push
#pragma optimization_level 2
void fn_80162858(u32 index, u32 val1, u32 val2) {
    u32 offset = index * 0xF4;
    {
        PeopleFieldMoveSlot* entry1 =
            (PeopleFieldMoveSlot*)((u8*)lbl_8047B024 + offset);
        entry1->field_94 = val1;
    }
    {
        PeopleFieldMoveSlot* entry2 =
            (PeopleFieldMoveSlot*)((u8*)lbl_8047B024 + offset);
        entry2->field_98 = val2;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 2
u8 fn_80162878(u32 index) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    return entries[index].field_9C;
}
#pragma pop

#pragma push
#pragma optimization_level 2
u8 fn_8016288C(u32 index) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    return entries[index].field_90;
}
#pragma pop

#pragma push
#pragma optimization_level 2
u16 fn_801628A0(u32 index) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    return entries[index].field_70;
}
#pragma pop

#pragma push
#pragma optimization_level 2
void fn_801628B4(u32 index, u8 val) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    entries[index].field_A0 = val;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwStart(u32 index, u8 studio) {
#define HW_VOICES (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024)
    HW_VOICES[index].field_D4 = lbl_8047B050;
    salActivateVoice((u8*)&HW_VOICES[index], studio);
#undef HW_VOICES
}
#pragma pop

void hwKeyOff(u32 index) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    entries[index].flags_24[lbl_8047B050] |= 0x40;
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwSetPitch(u32 index, u16 value) {
    typedef struct {
        u8 pad_00[0x24];
        u32 words_24[0x30];
        u8 activeWordIndex;
        u8 pad_E5[0x0F];
    } PeopleFieldState;
    PeopleFieldState* entries =
        (*(PeopleFieldState* volatile*)&lbl_8047B024);
    PeopleFieldState* entry = &entries[index];
    u32 scaledValue;

    if ((u16)value >= 0x4000) {
        value = 0x3FFF;
    }
    if (entry->activeWordIndex != 0xFF) {
        scaledValue = (u16)value << 4;
        if (entry->words_24[5 + entry->activeWordIndex] == scaledValue) {
            return;
        }
    }
    scaledValue = (u16)value << 4;
    entry->words_24[5 + lbl_8047B050] = scaledValue;
    entry->words_24[lbl_8047B050] |= 8;
    entry->activeWordIndex = lbl_8047B050;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_801629A4(u32 index, u8 value) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    entries[index].field_CC = (&lbl_80478BF8)[(u8)value];
    entries[index].flags_24[0] |= 0x100;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_801629D0(u32 index, u8 value) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    entries[index].field_CE = (&lbl_80478C00)[(u8)value];
    entries[index].flags_24[0] |= 0x80;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwSetITDMode(u32 index, u8 flag) {
#define HW_VOICES (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024)
    if (flag == 0) {
        HW_VOICES[index].field_F0 |= 0x80000000;
        HW_VOICES[index].field_D0 = 0x10;
        HW_VOICES[index].field_D2 = 0x10;
    } else {
        HW_VOICES[index].field_F0 &= ~0x80000000u;
    }
#undef HW_VOICES
}
#pragma pop

static inline void hwSetupITD(HwVolumeVoice* voice, u8 pan) {
    voice->itdShiftL = lbl_80273448[pan];
    voice->itdShiftR = 32 - lbl_80273448[pan];
    voice->changed[0] |= 0x200;
}

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwSetVolume(u32 voice, u32 table, f32 volume, u32 pan, u32 span,
                 f32 auxA, f32 auxB) {
    HwVolumeInfo volumeInfo;
    u16 left;
    u16 right;
    u16 surround;
    HwVolumeVoice* dspVoice = (HwVolumeVoice*)lbl_8047B024;
    HwVolumeVoice* dspVoicePtr = &dspVoice[voice];

    {
        f32 one = lbl_8047D4D8;
        if (volume >= one) {
            volume = one;
        }
    }
    {
        f32 one = lbl_8047D4D8;
        if (auxA >= one) {
            auxA = one;
        }
    }
    {
        f32 one = lbl_8047D4D8;
        if (auxB >= one) {
            auxB = one;
        }
    }

    {
        u32 hasITD = (dspVoicePtr->flags & 0x80000000) != 0;
        u32 dpl2 = lbl_80447E60[dspVoicePtr->studio].named.type == 1;

        salCalcVolume(table, volumeInfo.values, volume, pan, span, auxA, auxB,
                      hasITD, dpl2);
    }

    left = lbl_8047D4DC * volumeInfo.named.volL;
    right = lbl_8047D4DC * volumeInfo.named.volR;
    surround = lbl_8047D4DC * volumeInfo.named.volS;
    if (dspVoicePtr->lastUpdate.vol == 0xFF || dspVoicePtr->volL != left ||
        dspVoicePtr->volR != right || dspVoicePtr->volS != surround) {
        dspVoicePtr->volL = left;
        dspVoicePtr->volR = right;
        dspVoicePtr->volS = surround;
        dspVoicePtr->changed[0] |= 1;
        dspVoicePtr->lastUpdate.vol = 0;
    }

    left = lbl_8047D4DC * volumeInfo.named.volAuxAL;
    right = lbl_8047D4DC * volumeInfo.named.volAuxAR;
    surround = lbl_8047D4DC * volumeInfo.named.volAuxAS;
    if (dspVoicePtr->lastUpdate.volA == 0xFF || dspVoicePtr->volLa != left ||
        dspVoicePtr->volRa != right || dspVoicePtr->volSa != surround) {
        dspVoicePtr->volLa = left;
        dspVoicePtr->volRa = right;
        dspVoicePtr->volSa = surround;
        dspVoicePtr->changed[0] |= 2;
        dspVoicePtr->lastUpdate.volA = 0;
    }

    left = lbl_8047D4DC * volumeInfo.named.volAuxBL;
    right = lbl_8047D4DC * volumeInfo.named.volAuxBR;
    surround = lbl_8047D4DC * volumeInfo.named.volAuxBS;
    if (dspVoicePtr->lastUpdate.volB == 0xFF || dspVoicePtr->volLb != left ||
        dspVoicePtr->volRb != right || dspVoicePtr->volSb != surround) {
        dspVoicePtr->volLb = left;
        dspVoicePtr->volRb = right;
        dspVoicePtr->volSb = surround;
        dspVoicePtr->changed[0] |= 4;
        dspVoicePtr->lastUpdate.volB = 0;
    }

    if (dspVoicePtr->flags & 0x80000000) {
        hwSetupITD(dspVoicePtr, pan >> 16);
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162D18(u32 index) {
    PeopleFieldMoveSlot* entries =
        (*(PeopleFieldMoveSlot* volatile*)&lbl_8047B024);
    salDeactivateVoice((u8*)&entries[index]);
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwSetAUXProcessingCallbacks(u8 index, u32 a, u32 b, u32 c, u32 d) {
    PeopleStudioState* entries = (PeopleStudioState*)lbl_80447E60;
    entries[(u8)index].field_AC = a;
    entries[(u8)index].field_B4 = b;
    entries[(u8)index].field_B0 = c;
    entries[(u8)index].field_B8 = d;
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162D6C(void) { salActivateStudio(); }
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162D8C(u32 studio) { fn_8015AAA0(studio); }
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162DAC(u8 index, u32 arg1) {
    PeopleStudioState* entries = (PeopleStudioState*)lbl_80447E60;
    fn_8015D54C((u8*)&entries[(u8)index], (void*)arg1);
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162DE0(u8 index, u32 arg1) {
    PeopleStudioState* entries = (PeopleStudioState*)lbl_80447E60;
    fn_8015D5F4((u8*)&entries[(u8)index], (void*)arg1);
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
u32 fn_80162E14(u32 idx) {
    typedef struct PeopleFieldEntry {
        u8 pad_00[0x20];
        u32 dim_20;
        u8 pad_24[0x54];
        u32 dim_78;
        u8 pad_7C[0x14];
        u8 kind_90;
        u8 pad_91[0x5B];
        u8 flag_EC;
        u8 pad_ED[0x07];
    } PeopleFieldEntry;
    PeopleFieldEntry* entries =
        (*(PeopleFieldEntry* volatile*)&lbl_8047B024);

    if (entries[idx].flag_EC != 2) {
        return 0;
    }
    switch (entries[idx].kind_90) {
    case 0:
    case 1:
    case 4:
    case 5: {
        PeopleFieldEntry* entry =
            (PeopleFieldEntry*)((u32)entries + idx * 0xF4);
        u32 small = entry->dim_78;
        u32 big = entry->dim_20;
        u32 value = ((big - (small << 1)) >> 4) * 0xE;
        u32 lo = big & 0xF;
        if (lo >= 2) {
            value = lo + value;
            value -= 2;
        }
        return value;
    }
    case 3:
        return entries[idx].dim_20 - entries[idx].dim_78;
    case 2:
        return entries[idx].dim_20 - (entries[idx].dim_78 >> 1);
    default:
        return idx;
    }
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void hwFlushStream(u8* dstBase, u32 srcOffset, u32 size, u32 streamIndex,
                   u32 arg7, u32 arg8) {
    u32 unusedOut;
    u8* srcBase =
        (u8*)aramGetStreamBufferAddress(streamIndex, &unusedOut);
    u8* dst;

    size += srcOffset & 0x1F;
    srcOffset &= ~0x1F;
    dst = dstBase + srcOffset;
    size = (size + 0x1F) & ~0x1F;
    DCStoreRange(dst, size);
    aramUploadData(dst, srcBase + srcOffset, size, 1, arg7, arg8);
}
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162F48(void) { fn_80163CA8(); }
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162F68(void) { aramFreeStreamBuffer(); }
#pragma pop

#pragma push
#pragma optimization_level 4
#pragma optimizewithasm off
void fn_80162F88(void* index) {
    aramGetStreamBufferAddress((u32)index, 0);
}
#pragma pop

#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
void fn_80162FAC(void) {}
#pragma pop
