/**
 * @file field_exact_801D0314.c
 * @brief Exact field/memory-card task island, 0x801D0314 - 0x801D0AA0.
 */
#include "dolphin/types.h"

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

extern u8 lbl_8047B3D0;
extern MemcardTaskState* lbl_8047B3D4;
extern u8 lbl_80467168[];

extern void* fn_800E202C(void* ptr);
extern void fn_800E24B0(void);
extern s32 fn_800E209C(void* ptr);
extern void fn_800E2C04(s32 size, s32 align);
extern void* fn_800E27B0(void);
extern void winMsgClose(s32 id);
extern void GSgappTerminate(void* app);
extern void* GSgappCreate(s32 state, u8 priority, void* param, void* callback);
extern u32 _fadeEffectGetRandom__FUl(u32 limit);
extern s32 fn_801D0090(s32 error_code);
extern void CARDInit(void);
extern void _threadSwitch(void);
extern void fn_801CDB04(void);
extern void* savedataGetStatus(void* data, s32 index);

void* fn_801D0314(void* ptr)
{
    void* aligned = fn_800E202C(ptr);

    fn_800E24B0();
    if (fn_800E209C(aligned) == 0) {
        return NULL;
    }
    return ptr;
}

void fn_801D036C(void)
{
    fn_800E2C04(0x1DFD0, 0x20);
    fn_800E27B0();
}

void fn_801D039C(void)
{
    if (lbl_8047B3D4->task_kind != 0) {
        if (((volatile MemcardTaskState*)lbl_8047B3D4)->task_kind != 8) {
            winMsgClose(1);
        }

        GSgappTerminate(lbl_8047B3D4->gapp);

        lbl_8047B3D4->card_work_area = fn_801D0314(lbl_8047B3D4->card_work_area);
        lbl_8047B3D4->work_buffer = fn_801D0314(lbl_8047B3D4->work_buffer);
    }

    lbl_8047B3D4->task_kind = 0;
}

void fn_801D046C(u8 resume)
{
    if (lbl_8047B3D4->state == 0x31 && resume != 0) {
        lbl_8047B3D4->state = lbl_8047B3D4->resume_state;
        lbl_8047B3D4->random_delay = _fadeEffectGetRandom__FUl(0x3C);
    } else {
        lbl_8047B3D4->error_code = 0xF;
        lbl_8047B3D4->state = 0x2B;
    }
}

u8 fn_801D04D0(void)
{
    return lbl_8047B3D4->state == 0x31;
}

u8 fn_801D04E8(void)
{
    return lbl_8047B3D4->serial_check_enabled;
}

s32 memcardGetTaskResult(void)
{
    if (lbl_8047B3D4->state == 0x30) {
        lbl_8047B3D4->error_code = (u8)fn_801D0090(lbl_8047B3D4->error_code);
        lbl_8047B3D4->state = lbl_8047B3D4->resume_state;
    }

    if (lbl_8047B3D4->state != 0x32) {
        return 0;
    }
    return lbl_8047B3D4->task_result;
}

void fn_801D055C(s32 task_kind, s32 card_work_size, s32 card_channel)
{
    if (lbl_8047B3D4->task_kind != 0) {
        do {
            _threadSwitch();
        } while (memcardGetTaskResult() == 0);
        fn_801D039C();
    }

    if (card_work_size == 0) {
        card_work_size = 2;
    }

    lbl_8047B3D4->task_kind = task_kind;
    lbl_8047B3D4->card_channel = card_channel;
    lbl_8047B3D4->card_work_size = card_work_size;
    lbl_8047B3D4->task_result = 0;
    lbl_8047B3D4->state = 0;
    lbl_8047B3D4->error_code = 0;
    lbl_8047B3D4->dialog_result = 0;

    fn_800E2C04(0x1E600, 0x20);
    lbl_8047B3D4->work_buffer = fn_800E27B0();
    fn_800E2C04(0xA000, 0x20);
    lbl_8047B3D4->card_work_area = fn_800E27B0();
    lbl_8047B3D4->gapp = GSgappCreate(1, 0x17, NULL, fn_801CDB04);
    lbl_8047B3D4->savedata_status = savedataGetStatus(0, 0);
}

static inline void memcardStartTask(s32 task_kind, s32 card_work_size,
                                    s32 card_channel)
{
    if (lbl_8047B3D4->task_kind != 0) {
        do {
            _threadSwitch();
        } while (memcardGetTaskResult() == 0);
        fn_801D039C();
    }

    if (card_work_size == 0) {
        card_work_size = 2;
    }

    lbl_8047B3D4->task_kind = task_kind;
    lbl_8047B3D4->card_channel = card_channel;
    lbl_8047B3D4->card_work_size = card_work_size;
    lbl_8047B3D4->task_result = 0;
    lbl_8047B3D4->state = 0;
    lbl_8047B3D4->error_code = 0;
    lbl_8047B3D4->dialog_result = 0;

    fn_800E2C04(0x1E600, 0x20);
    lbl_8047B3D4->work_buffer = fn_800E27B0();
    fn_800E2C04(0xA000, 0x20);
    lbl_8047B3D4->card_work_area = fn_800E27B0();
    lbl_8047B3D4->gapp = GSgappCreate(1, 0x17, NULL, fn_801CDB04);
    lbl_8047B3D4->savedata_status = savedataGetStatus(0, 0);
}

s32 fn_801D0748(s32 task_kind, s32 card_work_size, s32 card_channel)
{
    s32 result;

    memcardStartTask(task_kind, card_work_size, card_channel);

    do {
        _threadSwitch();
        result = memcardGetTaskResult();
    } while (result == 0);

    fn_801D039C();
    return result;
}

void fn_801D0A30(void)
{
    u32 offset;

    if (lbl_8047B3D0 == 0) {
        CARDInit();
        offset = _fadeEffectGetRandom__FUl(0x100) & ~7;
        lbl_8047B3D4 = (MemcardTaskState*)&lbl_80467168[offset];
        lbl_8047B3D4->task_kind = 0;
        lbl_8047B3D4->work_buffer = NULL;
        lbl_8047B3D4->card_work_area = NULL;
        lbl_8047B3D4->field_2c = 0;
        lbl_8047B3D0 = 1;
    }
}
