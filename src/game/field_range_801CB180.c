/**
 * @file field_range_801CB180.c
 * @brief field/hero, 0x801CB180 - 0x801D0AA0.
 *
 * Boundary evidence-verified from asm (sdata clusters, callee families,
 * static linkage, call chains) - mixed-block split pass, 2026-07-01.
 * All functions asm-only until matched.
 */
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

extern u8 lbl_8047B3C8;
extern u8 lbl_8047B3D0;
extern MemcardTaskState* lbl_8047B3D4;
extern u8 lbl_80467168[];
extern u32 lbl_804670E8[];
extern const u8 lbl_802758C8[];
extern const u8 lbl_8036DCA8[];

extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, s32 value, u32 size);
extern const u8 lbl_8047E160[];
extern const u8 lbl_8047E164[];

extern u32 fn_800F7BC4(s32 pad);
extern s32 fn_800F7A7C(s32 pad, s32 axis);
extern s32 fn_800F7A08(s32 pad, s32 axis);
extern void fn_800F7068(s32 id, s32 value);
extern void fn_800F7274(s32 id);
extern void* GSthreadGetCurrentThread(void);
extern s32 fn_800F036C(void);
extern void GSlogWrite(const void* format, const void* text, ...);
extern s32 fn_800F7318(s32 task, void* callback, s32 stack_size, s32 arg3, s32 arg4, s32 arg5, ...);
extern void fn_800F7434(void* callback, s32 arg, ...);
extern u32 fn_80113F48(void);
extern void* GSresGetResource(u32 group, u32 resource);
extern void fn_80118874(void* resource, u32 arg);
extern void GSmodelLinkToGSparticleBank(void* model, void* particle_bank);
extern void GSmodelSetGSparticleLinkAttachMode(void* model, s32 mode);
extern void* fn_8018D998(u32 group, u32 resource);
extern void fn_80184470(u32 group, u32 resource);
extern void fn_801845E4(u32 group, u32 resource, u32 part_group, u32 part_resource, s32 part);
extern void fn_8018B220(u32 group, u32 resource);
extern void fn_8018B368(u32 group, u32 resource, u32 anim_index, s32 frame, u8 loop);
extern void GSmodelStopAnimation(void* model);
extern void fn_8018DB68(u32 group, u32 resource);
extern void fn_8018C1E8(u32 group, u32 resource, u8 visible);
extern void GSmodelSetVisibility(void* model, u8 visible);
extern void GSmodelDetachFromGSpart(void* model, s32 arg);
extern void* GSmodelGetPart(void* model, s32 part);
extern void GSmodelAttachToGSpart(void* model, void* part, s32 arg2, s32 arg3, s32 arg4);
extern void GSpartFree(void* part);
extern u32 peopleWaitSyncMotion(u32 group, u32 resource, u8 wait);
extern u32 GSmodelHasAnimationEnded(void* model);
extern void _threadSwitch(void);
extern void GSmodelSetAnimIndex(void* model, u32 index);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelSetTexAnimIndex(void* model, u32 index);
extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
extern void GSmodelSetTexAnimRate(void* model, f32 rate);
extern void GSmodelSetAnimType(void* model, u32 type);
extern void GSmodelStartAnimation(void* model);
extern void* fn_8018E050(u32 group, s32 people_id, void* param);
extern void* peopleSearchID(void* people);
extern void* peopleGetModel(void* people);
extern void GSmodelSetBoundCheck(void* model, s32 enabled);
extern s32 fn_800FF58C(s32 msg_id);
extern void winMsgOpen(s32 id, s32 msg_id, s32 arg2, s32 arg3);
extern s32 fn_8001E184(void);
extern s32 fn_800889E4(s32 arg);
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
extern s32 CARDProbeEx();
extern s32 CARDGetResultCode();
extern s32 CARDMountAsync();
extern s32 CARDCheckAsync();
extern s32 CARDFormatAsync();
extern s32 CARDDeleteAsync();
extern s32 CARDCreateAsync();
extern s32 CARDWriteAsync();
extern s32 CARDSetStatusAsync();
extern s32 CARDGetAttributes();
extern s32 CARDGetSerialNo();
extern s32 CARDFreeBlocks();
extern s32 CARDCancel();
extern s32 CARDClose();
extern s32 CARDUnmount();
extern s32 fn_800056C4();
extern s32 fn_800056D4();
extern s32 fn_800057A8();
extern s32 fn_800057A0(void);
extern s32 fn_80072A00();
extern s32 fn_80089D74();
extern s32 fn_8008ABA0();
extern s32 fn_800B01C4();
extern void* fn_800B01AC(s32 channel);
extern s32 fn_800B4488();
extern s32 fn_800B4C7C();
extern s32 fn_800B5530();
extern s32 fn_800B5BE4();
void* savedataGetStatus(void* data, s32 index);
s32 gamedatasaveGetStatus(void* data, s32 index);
void gamedatasaveSetStatus(void* data, s32 index, s32 value);
void* gamedatasaveBiosGetPtr(void* data);
void gamedatasaveBiosSetMemcardID(void* data, u64 memcard_id);
u16 fn_8006A718(void* data);
void fn_8006AF44(void* data, void* value);
void fn_801CBE44(void* data, u32 size, void* hash, u32 offset);
u16 fn_800E0C54(void);
void* heroGetStatus(void* data, s32 status, s32 index);
void heroSetStatus(void* data, s32 status, void* value);
extern char* strcpy(char*, const char*);
extern s32 strcmp(const char*, const char*);
extern const char lbl_802758E8[];
extern const char lbl_802792E8[];
extern u32 lbl_8047E168;
extern u16 lbl_8047E170;
extern u32 lbl_8047E174;
extern void fn_801D0080(void);
extern s32 fn_801CF320(void);
extern s32 fn_801CF568(void);
extern s32 fn_801CF7E4(void);
extern s32 fn_801CF9C8(void);
extern s32 fn_801CFD08(void);

#pragma push
#pragma scheduling off
#pragma peephole off
s32 scriptIsTrigerPush(void)
{
    s32 pushed = 0;

    if ((fn_800F7BC4(1) & 0x1F70) != 0) {
        pushed = 1;
    }

#pragma scheduling on
    return pushed;
}
#pragma pop

#pragma push
#pragma scheduling off
#pragma peephole off
s32 fn_801CB1C4(void)
{
  s32 pushed = 0;
  s32 axis;
  s32 buttons;
  ;
  if ((fn_800F7BC4(1) & 0xF) != 0)
  {
    pushed = 1;
  }
  axis = (s8) fn_800F7A7C(1, 0);
  if (axis > (1 * 0))
  {
    axis = (s8) fn_800F7A7C(1, 0);
  }
  else
  {
    axis = -((s8) fn_800F7A7C(1, 0));
  }
  if (axis > 2)
  {
    pushed = 1;
  }
  axis = (s8) fn_800F7A08(1, 0);
  if (axis > 0)
  {
    axis = (s8) fn_800F7A08(1, 0);
  }
  else
  {
    axis = -((s8) fn_800F7A08(1, 0));
  }
  if (axis > 2)
  {
    pushed = 1;
  }
  if ((fn_800F7BC4(1) & 0x1F70) != 0)
  {
    pushed = 1;
  }
  #pragma scheduling on
  #pragma scheduling on
  return pushed;
}
#pragma pop

#pragma push
#pragma scheduling off
#pragma peephole off
s32 scriptIsMoveButtonPush(void)
{
    s32 pushed = 0;
    s32 axis;
    s32 buttons;

    if ((fn_800F7BC4(1) & 0xF) != 0) {
        pushed = 1;
    }

    axis = (s8)fn_800F7A7C(1, 0);
    if (axis > 0) {
        axis = (s8)fn_800F7A7C(1, 0);
    } else {
        axis = -(s8)fn_800F7A7C(1, 0);
    }
    if (axis > 2) {
        pushed = 1;
    }

    axis = (s8)fn_800F7A08(1, 0);
    if (axis > 0) {
        axis = (s8)fn_800F7A08(1, 0);
    } else {
        axis = -(s8)fn_800F7A08(1, 0);
    }
    if (axis > 2) {
        pushed = 1;
    }

#pragma scheduling on
    return pushed;
}
#pragma pop

#pragma push
#pragma scheduling off
void fn_801CB394(s32 id)
{
    fn_800F7068(id, 0);
}

void fn_801CB3B8(s32 id)
{
    fn_800F7068(id, 1);
}

void fn_801CB3DC(s32 id)
{
    fn_800F7274(id);
}
#pragma pop

s32 scriptExecTask(void* callback, u32 priority, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
{
    s32 task;

    if (GSthreadGetCurrentThread() != NULL) {
        task = fn_800F036C();
    } else {
        GSlogWrite(lbl_802758C8, lbl_8036DCA8);
        task = 0x7F;
    }

    if ((u8)priority > 7) {
        priority = 7;
    }

    task += priority;

    return fn_800F7318(task, callback, 0x1000, 1, 0, 4, arg2, arg3, arg4, arg5);
}

#pragma push
#pragma peephole off
void fn_801CB4A8(void* callback, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    fn_800F7434(callback, 4, arg1, arg2, arg3, arg4);
}
#pragma pop

#pragma push
#pragma peephole off
void fn_801CB4E8(u32 resource, u32 arg)
{
    void* object = GSresGetResource(fn_80113F48(), resource);

    fn_80118874(object, arg);
}
#pragma pop

#pragma push
#pragma peephole off
void fn_801CB530(u32 model_id, u32 particle_bank_id)
{
    void* model = GSresGetResource(fn_80113F48(), model_id);
    void* particle_bank = GSresGetResource(fn_80113F48(), particle_bank_id);

    GSmodelLinkToGSparticleBank(model, particle_bank);
    GSmodelSetGSparticleLinkAttachMode(model, 4);
}
#pragma pop

s32 fn_801CB59C(u32 resource)
{
    u32 group = fn_80113F48();
    void* model;

    if (fn_8018D998(group, resource) != NULL) {
        fn_80184470(group, resource);
    } else {
        model = GSresGetResource(group, resource);
        if (model == NULL) {
            return 0;
        }
        GSmodelDetachFromGSpart(model, 1);
    }

    return 1;
}

#pragma push
#pragma scheduling off
#pragma peephole off
s32 fn_801CB61C(u32 resource, u32 part_resource, s32 part)
{
    u32 group = fn_80113F48();
    void* model;
    void* part_model;

    if (fn_8018D998(group, resource) != NULL) {
        fn_801845E4(group, resource, group, part_resource, part);
    } else {
        model = GSresGetResource(group, resource);
        if (model == NULL) {
            return 0;
        }

        part_model = GSresGetResource(group, part_resource);
        if (part_model == NULL) {
            return 0;
        }

        part_model = GSmodelGetPart(part_model, part);
        GSmodelAttachToGSpart(model, part_model, 7, 0, 1);
        GSpartFree(part_model);
    }

    return 1;
}
#pragma pop

#pragma push
#pragma peephole off
s32 scriptWaitSyncMotion(u32 resource, s32 wait)
{
    u32 group = fn_80113F48();
    void* model;

    if (fn_8018D998(group, resource) != NULL) {
        return (u8)peopleWaitSyncMotion(group, resource, wait);
    }

    model = GSresGetResource(group, resource);
    if (model == NULL) {
        return 0;
    }

    while (1) {
        if ((u8)GSmodelHasAnimationEnded(model) != 0) {
            return 0;
        }
        if (wait != 0) {
            _threadSwitch();
            continue;
        }
        return 1;
    }
}
#pragma pop

void fn_801CB7C4(u32 resource)
{
    u32 group = fn_80113F48();
    void* model;

    if (fn_8018D998(group, resource) != NULL) {
        fn_8018B220(group, resource);
    } else {
        model = GSresGetResource(group, resource);
        if (model != NULL) {
            GSmodelStopAnimation(model);
        }
    }
}

#pragma push
#pragma peephole off
void fn_801CB834(u32 resource, u32 anim_index, s32 frame, s32 loop)
{
    u32 group = fn_80113F48();
    void* model;

    if (fn_8018D998(group, resource) != NULL) {
        fn_8018B368(group, resource, anim_index, frame, (u8)loop);
        return;
    }

    model = GSresGetResource(group, resource);
    if (model != NULL) {
        GSmodelSetAnimIndex(model, anim_index);
        GSmodelSetAnimFrame(model, (f32)frame);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelSetTexAnimIndex(model, anim_index);
        GSmodelSetTexAnimFrame(model, (f32)frame);
        GSmodelSetTexAnimRate(model, 0.5f);
        if (loop != 0) {
            GSmodelSetAnimType(model, 1);
        } else {
            GSmodelSetAnimType(model, 0);
        }
        GSmodelStartAnimation(model);
    }
}
#pragma pop

#pragma push
#pragma peephole off
void fn_801CB954(u32 resource, s32 visible)
{
    u32 group = fn_80113F48();
    void* model;

    if (fn_8018D998(group, resource) != NULL) {
        fn_8018C1E8(group, resource, (u8)visible);
    } else {
        model = GSresGetResource(group, resource);
        if (model != NULL) {
            GSmodelSetVisibility(model, (u8)visible);
        }
    }
}
#pragma pop

void fn_801CB9D8(u32 resource)
{
    fn_8018DB68(fn_80113F48(), resource);
}

#pragma push
#pragma scheduling off
s32 fn_801CBA0C(void* param)
{
    s32 people_id;
    u32 raw_id;
    void* people;

    raw_id = lbl_8047B3C8;
    lbl_8047B3C8 = raw_id + 1;
    people_id = (s8)raw_id | 0x7FFE0000;

    people = fn_8018E050(fn_80113F48(), people_id, param);
    if (people == NULL) {
#pragma scheduling on
        return 0;
    }

    GSmodelSetBoundCheck(peopleGetModel(peopleSearchID(people)), 0);
#pragma scheduling on
    return people_id;
}
#pragma pop

void fn_801CBA84(void)
{
    lbl_8047B3C8 = 0;
}

#pragma push
#pragma scheduling off
s32 fn_801CBA90(void)
{
    fn_800FF58C(0x395);
    return 0;
}
#pragma pop

#pragma push
#pragma scheduling off
s32 fn_801CBAB8(void)
{
    s32 state;
    s32 done;
    s32 input;
    s32 result;

    done = state = result = 0;

    while (done == 0) {
        switch (state) {
        case 0:
            winMsgOpen(2, 0x3C46, 1, 1);
            input = (s8)fn_8001E184();
            winMsgClose(1);
            if (input != 0) {
                done = 1;
            } else {
                state = 2;
            }
            break;
        case 2:
            if (fn_800889E4(1) == 0) {
                state = 3;
                result = 1;
            } else {
                state = 4;
            }
            break;
        case 3:
            fn_800FF58C(0x395);
            state = 4;
            break;
        case 4:
            done = 1;
            break;
        }
    }

    return result;
}
#pragma scheduling on
#pragma pop

typedef struct FieldSha1Context {
    u32 state[5];
    u32 count[2];
    u8 buffer[64];
} FieldSha1Context;

void fn_801CC380(u32 state[5], const u8 input[64]);
void fn_801CBF64(u8 digest[20], FieldSha1Context* context);

static inline void fieldSha1Update(FieldSha1Context* context,
                                   const u8* input, u32 length)
{
    u32 index;
    u32 part_length;
    u32 i;

    index = (context->count[0] >> 3) & 0x3F;
    if ((context->count[0] += length << 3) < (length << 3)) {
        context->count[1]++;
    }
    context->count[1] += length >> 29;
    part_length = 64 - index;

    if (length >= part_length) {
        memcpy(&context->buffer[index], input, part_length);
        fn_801CC380(context->state, context->buffer);
        for (i = part_length; i + 63 < length; i += 64) {
            fn_801CC380(context->state, &input[i]);
        }
        index = 0;
    } else {
        i = 0;
    }
    memcpy(&context->buffer[index], &input[i], length - i);
}

void fn_801CBBAC(u8 digest[20], const u8* input, u32 length)
{
    FieldSha1Context context;
    u32 index;
    u32 part_length;
    u32 i;
    const u8* current_input;

    context.state[0] = 0x67452301;
    context.state[1] = 0xEFCDAB89;
    context.state[2] = 0x98BADCFE;
    context.state[3] = 0x10325476;
    context.state[4] = 0xC3D2E1F0;
    context.count[0] = 0;
    context.count[1] = 0;

    index = (context.count[0] >> 3) & 0x3F;
    if ((context.count[0] += length << 3) < (length << 3)) {
        context.count[1]++;
    }
    context.count[1] += length >> 29;

    if (index + length > 63) {
        part_length = 64 - index;
        memcpy(&context.buffer[index], input, part_length);
        fn_801CC380(context.state, context.buffer);
        current_input = &input[part_length];
        for (i = part_length; i + 63 < length; i += 64) {
            fn_801CC380(context.state, current_input);
            current_input += 64;
        }
        index = 0;
    } else {
        i = 0;
    }
    memcpy(&context.buffer[index], &input[i], length - i);
    fn_801CBF64(digest, &context);
}

u32 fn_801CBCDC(u8* data, u32 size, const u32 expected[5], u32 offset)
{
    u32* savedDigest;
    u32* digest;
    u32* key;
    u8* cursor;
    u32 processed;
    s32 i;

    savedDigest = &lbl_804670E8[16];
    digest = &lbl_804670E8[21];
    key = &lbl_804670E8[26];
    memcpy(savedDigest, expected, 20);
    key[0] = ~expected[0];
    key[1] = ~expected[1];
    key[2] = ~expected[2];
    key[3] = ~expected[3];
    key[4] = ~expected[4];

    cursor = data + offset;
    processed = offset;
    while (processed < size) {
        fn_801CBBAC((u8*)digest, cursor, 20);
        ((u32*)cursor)[0] ^= key[0];
        ((u32*)cursor)[1] ^= key[1];
        ((u32*)cursor)[2] ^= key[2];
        ((u32*)cursor)[3] ^= key[3];
        ((u32*)cursor)[4] ^= key[4];
        memcpy(key, digest, 20);
        cursor += 20;
        processed += 20;
    }

    fn_801CBBAC((u8*)key, data, size);
    for (i = 0; i < 5; i++) {
        if (key[i] != savedDigest[i]) {
            return 0;
        }
    }
    return 1;
}

void fn_801CBE44(void* dataArg, u32 size, void* outArg, u32 offset)
{
    extern u32 lbl_80467128[];
    extern u32 lbl_80467150[];
    u32* digest;
    u32* key;
    u8* cursor;
    u8* data;
    u8* outDigest;
    u32 processed;

    data = dataArg;
    outDigest = outArg;
    digest = lbl_80467128;
    key = lbl_80467150;
    fn_801CBBAC((u8*)digest, data, size);
    key[0] = ~digest[0];
    key[1] = ~digest[1];
    key[2] = ~digest[2];
    key[3] = ~digest[3];
    key[4] = ~digest[4];

    cursor = data + offset;
    processed = offset;
    while (processed < size) {
        ((u32*)cursor)[0] ^= key[0];
        ((u32*)cursor)[1] ^= key[1];
        ((u32*)cursor)[2] ^= key[2];
        ((u32*)cursor)[3] ^= key[3];
        ((u32*)cursor)[4] ^= key[4];
        fn_801CBBAC((u8*)key, cursor, 20);
        cursor += 20;
        processed += 20;
    }
    memcpy(outDigest, digest, 20);
}

void fn_801CBF64(u8 digest[20], FieldSha1Context* context)
{
    u8 bits[8];
    u32 i;

    for (i = 0; i < 8; i++) {
        bits[i] = context->count[(i >= 4) ? 0 : 1] >>
                  ((3 - (i & 3)) * 8);
    }

    fieldSha1Update(context, lbl_8047E160, 1);
    while ((context->count[0] & 0x1F8) != 0x1C0) {
        fieldSha1Update(context, lbl_8047E164, 1);
    }
    fieldSha1Update(context, bits, 8);

    for (i = 0; i < 20; i++) {
        digest[i] = context->state[i >> 2] >> ((3 - (i & 3)) * 8);
    }

    memset(context->buffer, 0, sizeof(context->buffer));
    memset(context->state, 0, sizeof(context->state));
    memset(context->count, 0, sizeof(context->count));
    memset(bits, 0, sizeof(bits));
    fn_801CC380(context->state, context->buffer);
}


/* SHA-1's 16-word circular message schedule. */
typedef union SHA1Block {
    u8 bytes[64];
    u32 words[16];
} SHA1Block;

#define SHA1_ROTL(value, bits) \
    (((value) << (bits)) | ((value) >> (32 - (bits))))
#define SHA1_BLK0(i) (block->words[(i)])
#define SHA1_BLK(i) \
    (block->words[(i) & 15] = SHA1_ROTL(block->words[((i) + 13) & 15] ^ \
                                             block->words[((i) + 8) & 15] ^ \
                                             block->words[((i) + 2) & 15] ^ \
                                             block->words[(i) & 15], 1))
#define SHA1_R0(v, w, x, y, z, i) \
    z += ((w & (x ^ y)) ^ y) + SHA1_BLK0(i) + 0x5A827999 + SHA1_ROTL(v, 5); \
    w = SHA1_ROTL(w, 30)
#define SHA1_R1(v, w, x, y, z, i) \
    z += ((w & (x ^ y)) ^ y) + SHA1_BLK(i) + 0x5A827999 + SHA1_ROTL(v, 5); \
    w = SHA1_ROTL(w, 30)
#define SHA1_R2(v, w, x, y, z, i) \
    z += (w ^ x ^ y) + SHA1_BLK(i) + 0x6ED9EBA1 + SHA1_ROTL(v, 5); \
    w = SHA1_ROTL(w, 30)
#define SHA1_R3(v, w, x, y, z, i) \
    z += (((w | x) & y) | (w & x)) + SHA1_BLK(i) + 0x8F1BBCDC + \
         SHA1_ROTL(v, 5); \
    w = SHA1_ROTL(w, 30)
#define SHA1_R4(v, w, x, y, z, i) \
    z += (w ^ x ^ y) + SHA1_BLK(i) + 0xCA62C1D6 + SHA1_ROTL(v, 5); \
    w = SHA1_ROTL(w, 30)

void fn_801CC380(u32 state[5], const u8 input[64])
{
    u32 a;
    u32 b;
    u32 c;
    u32 d;
    u32 e;
    SHA1Block* block = (SHA1Block*) lbl_804670E8;

    memcpy(block, input, 64);

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    SHA1_R0(a, b, c, d, e, 0);
    SHA1_R0(e, a, b, c, d, 1);
    SHA1_R0(d, e, a, b, c, 2);
    SHA1_R0(c, d, e, a, b, 3);
    SHA1_R0(b, c, d, e, a, 4);
    SHA1_R0(a, b, c, d, e, 5);
    SHA1_R0(e, a, b, c, d, 6);
    SHA1_R0(d, e, a, b, c, 7);
    SHA1_R0(c, d, e, a, b, 8);
    SHA1_R0(b, c, d, e, a, 9);
    SHA1_R0(a, b, c, d, e, 10);
    SHA1_R0(e, a, b, c, d, 11);
    SHA1_R0(d, e, a, b, c, 12);
    SHA1_R0(c, d, e, a, b, 13);
    SHA1_R0(b, c, d, e, a, 14);
    SHA1_R0(a, b, c, d, e, 15);
    SHA1_R1(e, a, b, c, d, 16);
    SHA1_R1(d, e, a, b, c, 17);
    SHA1_R1(c, d, e, a, b, 18);
    SHA1_R1(b, c, d, e, a, 19);
    SHA1_R2(a, b, c, d, e, 20);
    SHA1_R2(e, a, b, c, d, 21);
    SHA1_R2(d, e, a, b, c, 22);
    SHA1_R2(c, d, e, a, b, 23);
    SHA1_R2(b, c, d, e, a, 24);
    SHA1_R2(a, b, c, d, e, 25);
    SHA1_R2(e, a, b, c, d, 26);
    SHA1_R2(d, e, a, b, c, 27);
    SHA1_R2(c, d, e, a, b, 28);
    SHA1_R2(b, c, d, e, a, 29);
    SHA1_R2(a, b, c, d, e, 30);
    SHA1_R2(e, a, b, c, d, 31);
    SHA1_R2(d, e, a, b, c, 32);
    SHA1_R2(c, d, e, a, b, 33);
    SHA1_R2(b, c, d, e, a, 34);
    SHA1_R2(a, b, c, d, e, 35);
    SHA1_R2(e, a, b, c, d, 36);
    SHA1_R2(d, e, a, b, c, 37);
    SHA1_R2(c, d, e, a, b, 38);
    SHA1_R2(b, c, d, e, a, 39);
    SHA1_R3(a, b, c, d, e, 40);
    SHA1_R3(e, a, b, c, d, 41);
    SHA1_R3(d, e, a, b, c, 42);
    SHA1_R3(c, d, e, a, b, 43);
    SHA1_R3(b, c, d, e, a, 44);
    SHA1_R3(a, b, c, d, e, 45);
    SHA1_R3(e, a, b, c, d, 46);
    SHA1_R3(d, e, a, b, c, 47);
    SHA1_R3(c, d, e, a, b, 48);
    SHA1_R3(b, c, d, e, a, 49);
    SHA1_R3(a, b, c, d, e, 50);
    SHA1_R3(e, a, b, c, d, 51);
    SHA1_R3(d, e, a, b, c, 52);
    SHA1_R3(c, d, e, a, b, 53);
    SHA1_R3(b, c, d, e, a, 54);
    SHA1_R3(a, b, c, d, e, 55);
    SHA1_R3(e, a, b, c, d, 56);
    SHA1_R3(d, e, a, b, c, 57);
    SHA1_R3(c, d, e, a, b, 58);
    SHA1_R3(b, c, d, e, a, 59);
    SHA1_R4(a, b, c, d, e, 60);
    SHA1_R4(e, a, b, c, d, 61);
    SHA1_R4(d, e, a, b, c, 62);
    SHA1_R4(c, d, e, a, b, 63);
    SHA1_R4(b, c, d, e, a, 64);
    SHA1_R4(a, b, c, d, e, 65);
    SHA1_R4(e, a, b, c, d, 66);
    SHA1_R4(d, e, a, b, c, 67);
    SHA1_R4(c, d, e, a, b, 68);
    SHA1_R4(b, c, d, e, a, 69);
    SHA1_R4(a, b, c, d, e, 70);
    SHA1_R4(e, a, b, c, d, 71);
    SHA1_R4(d, e, a, b, c, 72);
    SHA1_R4(c, d, e, a, b, 73);
    SHA1_R4(b, c, d, e, a, 74);
    SHA1_R4(a, b, c, d, e, 75);
    SHA1_R4(e, a, b, c, d, 76);
    SHA1_R4(d, e, a, b, c, 77);
    SHA1_R4(c, d, e, a, b, 78);
    SHA1_R4(b, c, d, e, a, 79);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

#undef SHA1_R4
#undef SHA1_R3
#undef SHA1_R2
#undef SHA1_R1
#undef SHA1_R0
#undef SHA1_BLK
#undef SHA1_BLK0
#undef SHA1_ROTL

/*
 * Memory-card task controller.  Each asynchronous SDK operation advances to
 * a polling state; completed operations are routed either to the next phase,
 * the user-decision state (0x30), or the common error state (0x2B).
 */
/* Preserve direct active-task reads across callback-capable card calls. */
#define task lbl_8047B3D4
#define raw ((u8*)lbl_8047B3D4)
#define file_info ((void*)((u8*)lbl_8047B3D4 + 0x8C))
void fn_801CDB04(void)
{
    s32 result;
    s32 status;
    u32 serial[2];
    u8 attributes;

    do {
        switch (task->state) {
        case 0:
            task->card_result = 0;
            task->next_state_after_delay = 0x32;
            task->format_requested = 0;
            task->initial_dialog_result = (u8) fn_800056D4();
            task->callback_finished = 0;
            fn_800056C4(task->initial_dialog_result);
            if (task->task_kind >= 1 && task->task_kind <= 2) {
                task->serial_check_enabled = 0;
            } else if ((task->task_kind == 3 || task->task_kind == 4) &&
                       gamedatasaveGetStatus(0, 4) == 0)
            {
                task->task_kind = 9;
            }
            task->retry_count = 0;
            task->state = 1;
            break;

        case 1:
            result = CARDProbeEx(task->card_channel, &task->sector_size,
                                 &task->memory_size);
            if (result == 0) {
                task->card_result = CARDGetResultCode(task->card_channel);
                task->state = 2;
            } else if (result == -1) {
                task->card_result = result;
                task->state = 1;
            } else if (task->retry_count++ < 6) {
                task->card_result = -1;
                task->state = 1;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 2:
            result = CARDMountAsync(task->card_channel, task->card_work_area,
                                    fn_801D0080, 0);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 3;
            break;

        case 3:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 3;
            } else if (result == 0 || result == -6 || result == -13) {
                task->state = 4;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 4:
            result = CARDCheckAsync(task->card_channel, NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 5;
            break;

        case 5:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (task->memory_size != 0x2000) {
                result = -6;
            }
            if (result == -1) {
                task->state = 5;
            } else if (result == 0) {
                if (task->serial_check_enabled && task->task_kind != 3 &&
                    task->task_kind != 9 && task->task_kind != 11)
                {
                    CARDGetSerialNo(task->card_channel, serial);
                    if (serial[0] != task->card_serial[0] ||
                        serial[1] != task->card_serial[1])
                    {
                        task->error_code = 0x10;
                        task->state = 0x2B;
                        break;
                    }
                }
                task->state = 9;
            } else if ((result == -6 || result == -13) &&
                       (task->task_kind == 1 || task->task_kind == 9))
            {
                task->error_code = 7;
                task->resume_state = 6;
                task->state = 0x30;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 6:
            if ((u8) task->error_code != 0) {
                CARDFormatAsync(task->card_channel, NULL);
                if (task->callback_finished != 0) {
                    task->error_code = 0;
                    task->state = 0x2B;
                } else {
                    task->next_state_after_delay = 7;
                    task->error_code = 4;
                    task->resume_state = 7;
                    task->state = 0x30;
                }
            } else {
                task->error_code = 0;
                task->state = 0x2B;
            }
            break;

        case 7:
            fn_800056D4();
            result = CARDFormatAsync(task->card_channel, NULL);
            if (task->callback_finished != 0) {
                task->error_code = result;
                task->state = 0x2B;
            } else {
                task->card_result = result == 0
                                        ? CARDGetResultCode(task->card_channel)
                                        : result;
                task->state = 8;
            }
            break;

        case 8:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 8;
            } else if (result == 0) {
                fn_800056C4(task->initial_dialog_result);
                task->next_state_after_delay = 0x32;
                task->error_code = 9;
                task->resume_state = 9;
                task->state = 0x30;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 9:
            task->state = fn_801CFD08();
            break;

        case 10:
            if (task->error_code == 0) {
                task->error_code = 0;
                task->state = 0x2B;
            } else {
                task->error_code = 3;
                task->next_state_after_delay = 13;
                task->resume_state = 11;
                task->state = 0x30;
            }
            break;

        case 11:
            fn_800056D4();
            result = CARDDeleteAsync(task->card_channel,
                                     &lbl_802758E8[0x3A00], NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 12;
            break;

        case 12:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 12;
            } else if (result == 0 || result == -4) {
                task->state = 13;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 13:
            result = CARDCreateAsync(task->card_channel,
                                     &lbl_802758E8[0x3A00],
                                     task->card_work_size, file_info, NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 14;
            break;

        case 14:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 14;
            } else if (result == 0) {
                raw[0x3E] = 1;
                task->state = 15;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 15:
            attributes = 0;
            result = CARDGetAttributes(task->card_channel, raw[0x8C],
                                       &attributes);
            if (result == 0) {
                result = fn_800B5BE4(task->card_channel, raw[0x8C],
                                     attributes & ~1, fn_801D0080);
            }
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 16;
            break;

        case 16:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 16;
            } else if (result == 0) {
                task->state = 17;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 17:
            memset(task->work_buffer, 0, task->card_work_size);
            strcpy((char*) task->work_buffer, &lbl_802758E8[0x3A00]);
            strcpy((char*) task->work_buffer + 0x20,
                   &lbl_802758E8[0x3A20]);
            memcpy((u8*) task->work_buffer + 0x40, &lbl_802758E8[0x3A40],
                   0x20);
            memcpy((u8*) task->work_buffer + 0x60, &lbl_802758E8[0x3A60],
                   0x20);
            task->state = 18;
            break;

        case 18:
            result = CARDWriteAsync(file_info, task->work_buffer, 0x2000, 0,
                                    NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 19;
            break;

        case 19:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 19;
            } else if (result == 0) {
                task->state = 20;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 20:
            memset((u8*) task->work_buffer + 0x2000, 0, 0x2000);
            raw[0x80] = 1;
            *(u32*) &raw[0x8C] = 0;
            task->state = 21;
            break;

        case 21:
            result = CARDWriteAsync(file_info,
                                    (u8*) task->work_buffer + 0x2000,
                                    0x2000, 0x2000, NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 22;
            break;

        case 22:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 22;
            } else if (result == 0) {
                task->state = 23;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 23:
            fn_800B5530(task->card_channel, raw[0x8C], &raw[0xA8]);
            result = CARDSetStatusAsync(task->card_channel, raw[0x8C],
                                        &raw[0xA8], NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 24;
            break;

        case 24:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 24;
            } else if (result == 0) {
                savedataGetStatus(0, 1);
                task->error_code = 0;
                task->task_result = 1;
                task->state = 25;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 25:
            task->state = 26;
            break;

        case 26:
            result = fn_800B4C7C(file_info, task->work_buffer, 0x2000, 0,
                                 NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 27;
            break;

        case 27:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 27;
            } else if (result == 0) {
                task->format_requested = 0;
                task->state = 28;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 28:
            if (task->error_code != 0) {
                task->error_code = 5;
                task->resume_state = 29;
                task->state = 0x30;
            } else {
                task->error_code = 0;
                task->state = 0x2B;
            }
            break;

        case 29:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 29;
            } else if (result == 0) {
                task->state = 30;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 30:
            result = fn_800B4C7C(file_info,
                                 (u8*) task->work_buffer + 0x2000,
                                 0x2000, 0x2000, NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 31;
            break;

        case 31:
            task->state = fn_801CF9C8();
            break;
        case 32:
            task->state = 33;
            break;
        case 33:
            result = fn_800B4C7C(file_info, task->work_buffer, 0x2000, 0,
                                 NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->state = 34;
            break;
        case 34:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 34;
            } else if (result == 0) {
                task->state = 35;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;
        case 35:
            task->state = fn_801CF7E4();
            break;
        case 36:
            task->state = fn_801CF568();
            break;
        case 37:
            task->state = fn_801CF320();
            break;

        case 38:
            fn_800056D4();
            result = CARDWriteAsync(file_info, task->work_buffer, 0x2000,
                                    task->field_20 * 0x1E000 + 0x6000, NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->field_38[0] /= 14;
            task->state = 39;
            break;

        case 39:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
            }
            if (result == -1) {
                task->state = 39;
            } else if (result == 0) {
                status = gamedatasaveGetStatus(savedataGetStatus(0, 1), 4);
                gamedatasaveSetStatus(savedataGetStatus(0, 1), 4,
                                      status + 1);
                task->error_code = 0;
                task->task_result = 4;
                task->state = task->format_requested ? 17 : 44;
            } else {
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 40:
            if (task->random_delay-- != 0) {
                task->card_result = -1;
                task->state = 40;
            } else {
                task->card_result = 0;
                task->state = 41;
            }
            break;

        case 41:
            if (fn_8008ABA0(2) == 0) {
                task->state = 0x2B;
                break;
            }
            result = CARDWriteAsync(file_info,
                                    (u8*) task->work_buffer + 0x1C000,
                                    0x2000,
                                    task->field_20 * 0x1E000 + 0x1C000,
                                    NULL);
            task->card_result = result == 0
                                    ? CARDGetResultCode(task->card_channel)
                                    : result;
            task->field_38[0] /= 14;
            task->field_3d = 0;
            task->state = 42;
            break;

        case 42:
            result = task->card_result;
            if (result == -1) {
                result = CARDGetResultCode(task->card_channel);
                task->card_result = result;
                task->field_38[0]--;
                if (task->field_3d == 0) {
                    if (fn_8008ABA0(2) == 0 ||
                        (task->field_38[0] < 3 && fn_80089D74(2) != 0))
                    {
                        CARDCancel(file_info);
                        task->state = 0x2B;
                        break;
                    }
                    task->field_3d = 1;
                }
                task->state = 42;
            } else if (result == 0) {
                if (task->field_3d == 0) {
                    fn_80089D74(2);
                }
                status = gamedatasaveGetStatus(savedataGetStatus(0, 1), 4);
                gamedatasaveSetStatus(savedataGetStatus(0, 1), 4,
                                      status + 1);
                task->error_code = 0;
                task->task_result = 4;
                task->state = task->format_requested ? 17 : 44;
            } else {
                fn_80072A00(1);
                task->error_code = result;
                task->state = 0x2B;
            }
            break;

        case 43:
            task->task_result = -1;
            task->state = 44;
            break;

        case 44:
            if (raw[0x3E] != 0) {
                if (fn_800057A8() == 4) {
                    fn_800B01C4(task->card_channel, *(u32*) &raw[0x68]);
                }
                CARDGetSerialNo(task->card_channel, serial);
                CARDClose(file_info);
                raw[0x3E] = 0;
            }
            CARDUnmount(task->card_channel);
            fn_800056C4(task->initial_dialog_result);
            if (task->callback_finished != 0) {
                task->state = 0;
            } else if (task->task_kind == 3 || task->task_kind == 8) {
                task->state = 0x32;
            } else {
                if (task->task_result >= 1 && task->task_result <= 4) {
                    task->card_serial[0] = serial[0];
                    task->card_serial[1] = serial[1];
                    task->serial_check_enabled = 1;
                }
                task->resume_state = 45;
                task->state = 0x30;
            }
            break;

        case 45:
            switch (task->task_kind) {
            case 0:
                if (task->task_result != 4) {
                    task->error_code = 0xF;
                    task->resume_state = 0x32;
                    task->state = 0x30;
                } else {
                    task->state = 0x32;
                }
                break;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
                if (task->task_result == -1) {
                    if (task->next_state_after_delay == 7) {
                        task->error_code = 13;
                    } else if (task->next_state_after_delay == 13) {
                        task->error_code = 14;
                    }
                    task->resume_state = 46;
                    task->state = 0x30;
                } else {
                    task->state = 0x32;
                }
                break;
            default:
                task->state = 0x32;
                break;
            }
            break;

        case 46:
            task->error_code = 8;
            task->resume_state = 47;
            task->state = 0x30;
            break;
        case 47:
            if ((u8) task->error_code != 0) {
                task->task_result = 5;
            }
            task->state = 0x32;
            break;
        case 48:
        case 49:
        case 50:
        default:
            break;
        }
    } while (lbl_8047B3D4->card_result != -1);
}
#undef file_info
#undef raw
#undef task

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

s32 fn_801CF7E4(void)
{
    u8* task;
    u8* save;
    u8* destination;
    u32* header;
    u32 sum;
    s32 taskKind;

    task = (u8*)lbl_8047B3D4;
    save = *(u8**)(task + 0x50);
    *(u32*)(save + 0xC) = 0;
    if (fn_801CBCDC(save, 0x1DFD8, (u32*)(save + 0x1DFEC), 0x18) != 0) {
        taskKind = *(s32*)task;
        if (taskKind != 3) {
            *(u32*)(task + 0x2C) = *(u32*)(save + 4);
            if (*(s32*)(task + 0x30) > *(s32*)(task + 0x2C)) {
                *(u32*)(task + 0x2C) = *(u32*)(task + 0x30);
                *(u32*)(task + 4) = 5;
                if (taskKind >= 1 && taskKind < 3) {
                    destination = *(u8**)(task + 0x58);
                    memcpy(destination, save + 8, 0x1DFD0);
                    *(u32*)(task + 0x10) = 0x25;
                } else {
                    *(u32*)(task + 0x10) = 0x24;
                }
                return 0x30;
            }
        }

        if (taskKind >= 1 && taskKind < 4) {
            destination = *(u8**)(task + 0x58);
            memcpy(destination, save + 8, 0x1DFD0);
            *(u32*)(task + 4) = 12;
            *(u32*)(task + 8) = 3;
            if (task[0x40] != 0) {
                return 0x11;
            }
            return 0x2C;
        }
        return 0x24;
    }

    header = (u32*)(save + 0x1E000 + (*(u32*)(task + 0x20) << 9));
    header[1] = 0;
    header[3] = 0;
    sum = header[0] + header[1] + header[2] + header[3];
    sum += header[4] + header[5] + header[6] + header[7];
    header[3] = -sum;
    return 0x1F;
}

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


void fn_801D0080(void)
{
    lbl_8047B3D4->callback_finished = 1;
}

s32 fn_801D0090(s32 error)
{
    extern void winMsgOpen(s32 window, u32 message, s32 arg2, s32 arg3);
    extern s8 menuSubOpenYesNo(s32 port, s32 x, s32 y, s32 initial);
    extern void msgctrlSetValue(s32 slot, s32 value);
    u32 message;
    u8 ask;
    s32 initial;
    s32 taskKind;

    ask = 0;
    taskKind = lbl_8047B3D4->task_kind;
    switch (error) {
    case 0:
        return 0;
    case 1:
        message = 0x3C29;
        break;
    case 2:
        if ((taskKind >= 5 && taskKind < 8) || taskKind == 13) {
            message = 0x3C2C;
        } else {
            message = 0x3C04;
        }
        break;
    case 3:
        message = 0x3BFB;
        break;
    case 4:
        message = 0x3C55;
        break;
    case 5:
        message = 0x4421;
        break;
    case 6:
        message = 0x3BFA;
        ask = 1;
        initial = 0;
        break;
    case 7:
        message = 0x3C33;
        ask = 1;
        initial = 0;
        break;
    case 8:
        message = 0x3D45;
        ask = 1;
        initial = 1;
        break;
    case 9:
        message = 0x3C57;
        break;
    case 10:
        if ((taskKind >= 5 && taskKind < 8) || taskKind == 13) {
            message = 0x3C2E;
        } else {
            message = 0x3C06;
        }
        break;
    case 11:
        message = 0x3BFD;
        break;
    case 12:
        message = 0x44E5;
        break;
    case 13:
        message = 0x3D43;
        break;
    case 14:
        message = 0x44D8;
        break;
    case 15:
        if ((taskKind >= 5 && taskKind < 8) || taskKind == 13) {
            message = 0x44D8;
        } else {
            message = 0x4444;
        }
        break;
    case 16:
        message = 0x3C34;
        break;
    case 17:
        message = 0x4422;
        if (taskKind != 2) {
            winMsgOpen((s8)(u32)lbl_8047B3D4->gapp, message, 1, 1);
            message = 0x44E4;
        }
        break;
    case 18:
        message = 0x4423;
        break;
    case 19:
        message = 0x3C30;
        break;
    case -3:
        message = 0x3C32;
        break;
    case -4:
        message = 0x3C31;
        break;
    case -5:
        message = 0x3C5B;
        break;
    case -6:
    case -13:
        if (taskKind >= 1 && taskKind < 3 &&
            lbl_8047B3D4->disk_id == (MemcardDiskId*)0x32) {
            message = 0x4424;
        } else {
            message = 0x3C30;
        }
        break;
    case -8:
    case -9:
        message = 0x3C5F;
        msgctrlSetValue(0x2F, 0x30);
        break;
    default:
        message = 0x3C30;
        break;
    }

    winMsgOpen((s8)(u32)lbl_8047B3D4->gapp, message, 1, 1);
    if (ask != 0 && menuSubOpenYesNo(0, 0x3C, 0xAA, initial) != 0) {
        ask = 0;
    }
    return ask;
}
