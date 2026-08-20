#include "dolphin/types.h"

typedef struct SavedataBlock SavedataBlock;
typedef struct SavedataBody SavedataBody;
typedef struct MemcardWorkBuffer MemcardWorkBuffer;

typedef struct MemcardDiskId {
    u32 game_code;
    u16 company_code;
    u8 disk_number;
    u8 game_version;
    u8 streaming;
    u8 streaming_buffer_size;
    u8 padding[0x16];
} MemcardDiskId;

typedef struct MemcardTaskState {
    s32 task_kind;
    s32 error_code;
    s32 task_result;
    s32 state;
    s32 resume_state;
    s32 card_channel;
    s32 sector_size;
    s32 memory_size;
    s32 field_20;
    s32 retry_count;
    s32 card_result;
    s32 field_2c;
    u32 serial_hi;
    s32 random_delay;
    u8 field_38[4];
    u8 callback_finished;
    u8 field_3d;
    u8 dialog_result;
    u8 initial_dialog_result;
    u8 format_requested;
    u8 serial_check_enabled;
    u8 field_42[6];
    u32 card_serial[2];
    MemcardWorkBuffer* work_buffer;
    void* card_work_area;
    SavedataBody* savedata_status;
    s32 card_work_size;
    s32 next_state_after_delay;
    void* gapp;
    MemcardDiskId* disk_id;
    MemcardDiskId mounted_disk_id;
    struct {
        s32 chan;
        s32 file_no;
        u32 offset;
        u32 length;
        u16 start_block;
    } file_info;
} MemcardTaskState;

struct SavedataBlock {
    u8 field_0000[0xB328];
    u8 field_B328[0x16B4];
    u8 field_C9DC[0x1F8];
    u8 field_CBD4[0x58];
};

struct SavedataBody {
    u8 data[0x1DFD0];
};

struct MemcardWorkBuffer {
    u8 header[8];
    SavedataBody savedata;
};

typedef struct SavedataPayload {
    u8 bytes[0x1DFD0];
} SavedataPayload;

typedef union MemcardSaveHeader {
    struct {
        u8 valid;
        u8 initialized;
        u8 field_02;
        u8 field_03;
        s32 save_count;
        u8 savedata[0x18];
    } fields;
    u32 words[8];
} MemcardSaveHeader;

typedef struct MemcardSaveBuffer {
    MemcardSaveHeader header;
    u32 field_20;
    u8 field_24[0x1DFB4];
    u8 random[20];
    u8 hash[0x18];
} MemcardSaveBuffer;

extern MemcardTaskState* lbl_8047B3D4;
extern void* savedataGetStatus(void* data, s32 kind);
extern void* gamedatasaveBiosGetPtr(void* ptr);
extern void gamedatasaveBiosSetMemcardID(void* ptr, u64 id);
extern u16 fn_8006A718(void* data);
extern void fn_8006AF44(void* data, void* arg);
extern s32 gamedatasaveGetStatus(void* data, s32 kind);
extern void gamedatasaveSetStatus(void* data, s32 kind, s32 value);
extern void* heroGetStatus(void* hero, s32 kind, s32 index);
extern void heroSetStatus(void* hero, s32 kind, void* data);
extern void CARDGetSerialNo(s32 channel, u32 serial[2]);
extern u16 fn_800E0C54(void);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void fn_801CBE44(void* data, u32 size, void* hash, u32 offset);

static inline void memcardSetSaveId(void* data, u32 serial_hi, u32 serial_lo)
{
    u64 memcard_id = ((u64)serial_hi << 32) | serial_lo;

    gamedatasaveBiosSetMemcardID(
        gamedatasaveBiosGetPtr(savedataGetStatus(data, 1)), memcard_id);
}

static inline void memcardFillRandom(u8* random, s32 count)
{
    while (count-- != 0) {
        *random++ = fn_800E0C54();
    }
}

s32 fn_801CF320(void)
{
    u32 serial[2];
    MemcardSaveBuffer* save;
    void* data;
    s32 status;

    CARDGetSerialNo(lbl_8047B3D4->card_channel, serial);
    memcardSetSaveId(NULL, serial[0], serial[1]);

    lbl_8047B3D4->field_2c++;
    lbl_8047B3D4->field_20 = (lbl_8047B3D4->field_20 + 1) % 3;

    save = (MemcardSaveBuffer*)lbl_8047B3D4->work_buffer;
    save->header.fields.valid = 1;
    save->header.fields.initialized = 1;
    save->header.fields.field_02 = 0;
    save->header.fields.field_03 = 0;
    save->header.fields.save_count = lbl_8047B3D4->field_2c;

    memcardSetSaveId(save->header.fields.savedata, serial[0], serial[1]);

    switch (lbl_8047B3D4->task_kind) {
    case 9:
        data = savedataGetStatus(save->header.fields.savedata, 14);
        if (fn_8006A718(save->header.fields.savedata) == 1) {
            fn_8006AF44(data, NULL);
        }
        /* fallthrough */
    case 4:
    case 8:
    case 10:
    case 11:
        status = gamedatasaveGetStatus(
            savedataGetStatus(save->header.fields.savedata, 1), 4);
        gamedatasaveSetStatus(
            savedataGetStatus(save->header.fields.savedata, 1), 4,
            status + 1);
        break;
    }

    save->field_20 = 0;
    for (status = 0xC0; status != 0; status--) {
    }

    save->header.words[3] = 0;
    fn_801CBE44(save, 0x1DFD8, save->hash, sizeof(save->hash));

    status = save->header.words[0];
    status += save->header.words[1];
    status += save->header.words[2];
    status += save->header.words[3];
    status += save->header.words[4];
    status += save->header.words[5];
    status += save->header.words[6];
    status += save->header.words[7];
    save->header.words[3] = -status;

    memcardFillRandom(save->random, 20);

    if (lbl_8047B3D4->task_kind == 10) {
        save->header.words[6]++;
    }

    return 38;
}

s32 fn_801CF568(void)
{
    SavedataBlock temporary;
    SavedataBody* savedata = &lbl_8047B3D4->work_buffer->savedata;
    SavedataBlock* status1;
    SavedataBlock* status2;
    SavedataBlock* status14;
    SavedataBlock* default_status14;
    void* hero_status13;
    void* hero_status14;

    status1 = savedataGetStatus(savedata, 1);
    status2 = savedataGetStatus(savedata, 2);
    status14 = savedataGetStatus(savedata, 14);
    default_status14 = savedataGetStatus(NULL, 14);

    switch (lbl_8047B3D4->task_kind) {
    case 9:
        temporary = *status14;
        *savedata = *(SavedataBody*)savedataGetStatus(NULL, 0);
        *status14 = temporary;
        *lbl_8047B3D4->savedata_status = *savedata;
        break;
    case 5:
        fn_8006AF44(status14, default_status14->field_B328);
        break;
    case 13:
        memcpy(status14->field_C9DC, default_status14->field_C9DC, 0x1F8);
        break;
    case 6:
        hero_status13 = heroGetStatus(NULL, 13, 0);
        heroSetStatus(status2, 13, hero_status13);
        hero_status14 = heroGetStatus(NULL, 14, 0);
        heroSetStatus(status2, 14, hero_status14);
        *status14 = *(SavedataBlock*)savedataGetStatus(NULL, 14);
        break;
    }

    gamedatasaveSetStatus(status1, 9,
                          (u8)gamedatasaveGetStatus(NULL, 9));
    gamedatasaveSetStatus(status1, 10,
                          (u8)gamedatasaveGetStatus(NULL, 10));
    return 37;
}
