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

typedef struct MemcardFileStatus {
    char file_name[0x20];
    u32 length;
    u32 time;
    u32 game_code;
    u16 company_code;
    u8 banner_format;
    u8 padding;
    u32 icon_address;
    u16 icon_format;
    u16 icon_speed;
    u32 comment_address;
    u32 banner_offset;
    u32 banner_tlut_offset;
    u32 icon_offsets[8];
    u32 icon_tlut_offset;
    u32 data_offset;
} MemcardFileStatus;

extern MemcardTaskState* lbl_8047B3D4;
extern const char lbl_802792E8[];
extern u32 lbl_8047E168;
extern u16 lbl_8047E170;
extern u32 lbl_8047E174;
extern s32 CARDFreeBlocks(s32 chan, s32* byte_not_used, s32* files_not_used);
extern s32 fn_800B4488(s32 chan, s32 file_no, void* file_info);
extern s32 fn_800B5530(s32 chan, s32 file_no, MemcardFileStatus* status);
extern s32 fn_800057A0(void);
extern s32 fn_800057A8(void);
extern MemcardDiskId* fn_800B01AC(s32 chan);
extern void fn_800B01C4(s32 chan, MemcardDiskId* disk_id);
extern s32 strcmp(const char* lhs, const char* rhs);

s32 fn_801CFD08(void)
{
    s32 free_bytes;
    s32 free_file_count;
    MemcardFileStatus opened_status;
    MemcardFileStatus status;
    s32 file_no;
    s32 result;
    s32 channel;
    void* file_info;
    s32 error;
    u8 valid;

    CARDFreeBlocks(lbl_8047B3D4->card_channel, &free_bytes,
                   &free_file_count);

    channel = lbl_8047B3D4->card_channel;
    file_info = &lbl_8047B3D4->file_info;
    for (file_no = 0; file_no < 0x7F; file_no++) {
        result = fn_800B4488(channel, file_no, file_info);
        if (result == 0) {
            result = fn_800B5530(channel, file_no, &status);
            if (result != 0) {
                goto invalid_file;
            }
            switch (fn_800057A0()) {
            case 0:
                if (strcmp(status.file_name, lbl_802792E8) != 0 ||
                    status.game_code != lbl_8047E168 ||
                    status.company_code != lbl_8047E170)
                {
                    goto invalid_file;
                }
                valid = 1;
                goto file_checked;
            case 1:
                if (strcmp(status.file_name, lbl_802792E8) != 0 ||
                    status.game_code != lbl_8047E174 ||
                    status.company_code != lbl_8047E170)
                {
                    goto invalid_file;
                }
                valid = 1;
                goto file_checked;
            case 2:
                if (strcmp(status.file_name, lbl_802792E8) != 0 ||
                    status.game_code != lbl_8047E174 ||
                    status.company_code != lbl_8047E170)
                {
                    goto invalid_file;
                }
                valid = 1;
                goto file_checked;
            default:
            invalid_file:
                valid = 0;
                break;
            }
        file_checked:
            if (valid != 0) {
                result = 0;
                goto scan_done;
            }
        }
    }

    if (result == 0) {
        result = -4;
    }

scan_done:
    error = result;
    switch (error) {
    case 0:
        lbl_8047B3D4->dialog_result = 1;
        if (fn_800057A8() == 4) {
            lbl_8047B3D4->disk_id = fn_800B01AC(lbl_8047B3D4->card_channel);
            lbl_8047B3D4->mounted_disk_id = *lbl_8047B3D4->disk_id;
            lbl_8047B3D4->mounted_disk_id.game_code = lbl_8047E168;
            lbl_8047B3D4->mounted_disk_id.company_code = lbl_8047E170;
            fn_800B01C4(lbl_8047B3D4->card_channel,
                       &lbl_8047B3D4->mounted_disk_id);
        }

        error = fn_800B5530(lbl_8047B3D4->card_channel,
                           lbl_8047B3D4->file_info.file_no,
                           &opened_status);
        if (error != 0) {
            break;
        }
        if (opened_status.comment_address != 0xFFFFFFFF) {
            if (lbl_8047B3D4->task_kind == 12) {
                lbl_8047B3D4->error_code = 2;
                lbl_8047B3D4->resume_state = 11;
                return 0x30;
            }
            if (((volatile MemcardTaskState*)lbl_8047B3D4)->task_kind == 3) {
                return 0x1C;
            }
            return 0x19;
        }
        free_bytes += 0x60000;
        free_file_count++;
        error = -4;
        /* fallthrough */
    case -4:
        switch (lbl_8047B3D4->task_kind) {
        case 1:
            if (free_bytes < 0x60000) {
                error = -9;
                break;
            }
            if (free_file_count < 1) {
                error = -8;
                break;
            }
            lbl_8047B3D4->error_code = 6;
            lbl_8047B3D4->resume_state = 10;
            return 0x30;
        case 9:
            lbl_8047B3D4->error_code = 2;
            lbl_8047B3D4->resume_state = 11;
            return 0x30;
        }
        break;
    }

    lbl_8047B3D4->error_code = error;
    return 0x2B;
}
