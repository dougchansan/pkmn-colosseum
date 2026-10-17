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

typedef struct SavedataPayload {
    u8 bytes[0x1DFD0];
} SavedataPayload;

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
extern s32 GSinputGetLeftStickXData(s32 pad, s32 axis);
extern s32 GSinputGetLeftStickYData(s32 pad, s32 axis);
extern void fn_800F7068(s32 id, s32 value);
extern void floorTerminateScriptThreads(s32 id);
extern void* GSthreadGetCurrentThread(void);
extern s32 fn_800F036C(void);
extern void GSlogWrite(const void* format, const void* text, ...);
extern s32 _vmThreadCreate(s32 task, void* callback, s32 stack_size, s32 arg3, s32 arg4, s32 arg5, ...);
extern void cmsRunSync(void* callback, s32 arg, ...);
extern u32 std093_getCurrentGroupID(void);
extern void* GSresGetResource(u32 group, u32 resource);
extern void gsParticleKill(void* resource, u32 arg);
extern void GSmodelLinkToGSparticleBank(void* model, void* particle_bank);
extern void GSmodelSetGSparticleLinkAttachMode(void* model, s32 mode);
extern void* peopleIsInGroup(u32 group, u32 resource);
extern void std168_characterDetachFromCharacter(u32 group, u32 resource);
extern void std070_characterAttachToCharacter(u32 group, u32 resource, u32 part_group, u32 part_resource, s32 part);
extern void peopleStopAnimation(u32 group, u32 resource);
extern void std154_characterPlayAnimation(u32 group, u32 resource, u32 anim_index, s32 frame, u8 loop);
extern void GSmodelStopAnimation(void* model);
extern void peopleCloseByID(u32 group, u32 resource);
extern void peopleSetVisible(u32 group, u32 resource, u8 visible);
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
extern void* peopleOpen(u32 group, s32 people_id, void* param);
extern void* peopleSearchID(void* people);
extern void* peopleGetModel(void* people);
extern void GSmodelSetBoundCheck(void* model, s32 enabled);
extern s32 floorRequestChange(s32 msg_id);
extern void winMsgOpen(s32 id, s32 msg_id, s32 arg2, s32 arg3);
extern s32 std011_dialogDisplayYesNoMenu(void);
extern s32 menuSaveLoadSaveGame(s32 arg);
extern void* GSmemGetHandleByPtr(void* ptr);
extern void fn_800E24B0(void);
extern s32 GSmemFree(void* ptr);
extern void GSmemAlloc(s32 size, s32 align);
extern void* GSmemLock(void);
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
extern s32 mainSetPauseFlag();
extern s32 fn_800056D4();
extern s32 GSgetVersionMinor();
extern s32 GSgetVersionMajor(void);
extern s32 fn_80072A00();
extern s32 fn_80089D74();
extern s32 fn_8008ABA0();
extern s32 fn_800B01C4();
extern void* fn_800B01AC(s32 channel);
extern s32 CARDOpen();
extern s32 fn_800B4C7C();
extern s32 CARDGetStatus();
extern s32 fn_800B5BE4();
void* savedataGetStatus(void* data, s32 index);
s32 gamedatasaveGetStatus(void* data, s32 index);
void gamedatasaveSetStatus(void* data, s32 index, s32 value);
void savedataCreate(void* data, s32 index);
void* gamedatasaveBiosGetPtr(void* data);
u64 gamedatasaveBiosGetMemcardID(u32* data);
void gamedatasaveBiosSetMemcardID(void* data, u64 memcard_id);
u16 fn_8006A718(void* data);
void savedataBattleSetSavedHero(void* data, void* value);
void fn_801CBE44(void* data, u32 size, void* hash, u32 offset);
u16 GSrand(void);
void* heroGetStatus(void* data, s32 status, s32 index);
void heroSetStatus(void* data, s32 status, void* value);
extern char* strcpy(char*, const char*);
extern s32 strcmp(const char*, const char*);
extern const char lbl_802758E8[];
extern const char lbl_802792E8[];
extern u32 lbl_8047E168;
extern u16 lbl_8047E170;
extern u32 lbl_8047E174;
extern void memcardRequestCancel(void);
extern s32 memcardSaveSetup(void);
extern s32 fn_801CF568(void);
extern s32 fn_801CF7E4(void);
extern s32 fn_801CF9C8(void);
extern s32 fn_801CFD08(void);

#if !defined(FIELD_801CB180_SPLIT) || defined(FIELD_801CB180_RANGE_801CB180)

s32 scriptIsTrigerPush(void)
{
    s32 pushed = 0;

    if ((fn_800F7BC4(1) & 0x1F70) != 0) {
        pushed = 1;
    }

    return pushed;
}

s32 scriptIsAnyInput(void)
{
    s32 pushed = 0;
    s32 axis;

    if ((fn_800F7BC4(1) & 0xF) != 0) {
        pushed = 1;
    }

    axis = (s8)GSinputGetLeftStickXData(1, 0);
    if (axis > 0) {
        axis = (s8)GSinputGetLeftStickXData(1, 0);
    } else {
        axis = -(s8)GSinputGetLeftStickXData(1, 0);
    }
    if (axis > 2) {
        pushed = 1;
    }

    axis = (s8)GSinputGetLeftStickYData(1, 0);
    if (axis > 0) {
        axis = (s8)GSinputGetLeftStickYData(1, 0);
    } else {
        axis = -(s8)GSinputGetLeftStickYData(1, 0);
    }
    if (axis > 2) {
        pushed = 1;
    }

    if ((fn_800F7BC4(1) & 0x1F70) != 0) {
        pushed = 1;
    }

    return pushed;
}

s32 scriptIsMoveButtonPush(void)
{
    s32 pushed = 0;
    s32 axis;

    if ((fn_800F7BC4(1) & 0xF) != 0) {
        pushed = 1;
    }

    axis = (s8)GSinputGetLeftStickXData(1, 0);
    if (axis > 0) {
        axis = (s8)GSinputGetLeftStickXData(1, 0);
    } else {
        axis = -(s8)GSinputGetLeftStickXData(1, 0);
    }
    if (axis > 2) {
        pushed = 1;
    }

    axis = (s8)GSinputGetLeftStickYData(1, 0);
    if (axis > 0) {
        axis = (s8)GSinputGetLeftStickYData(1, 0);
    } else {
        axis = -(s8)GSinputGetLeftStickYData(1, 0);
    }
    if (axis > 2) {
        pushed = 1;
    }

    return pushed;
}

void std005_threadIsRunning(s32 id)
{
    fn_800F7068(id, 0);
}

void std006_threadWaitForCompletion(s32 id)
{
    fn_800F7068(id, 1);
}

void fn_801CB3DC(s32 id)
{
    floorTerminateScriptThreads(id);
}

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

    return _vmThreadCreate(task, callback, 0x1000, 1, 0, 4, arg2, arg3, arg4, arg5);
}

void std209_threadRunScriptMode4(void* callback, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    cmsRunSync(callback, 4, arg1, arg2, arg3, arg4);
}

void fn_801CB4E8(u32 resource, u32 arg)
{
    void* object = GSresGetResource(std093_getCurrentGroupID(), resource);

    gsParticleKill(object, arg);
}

void std221_mapObjectLinkParticles(u32 model_id, u32 particle_bank_id)
{
    void* model = GSresGetResource(std093_getCurrentGroupID(), model_id);
    void* particle_bank = GSresGetResource(std093_getCurrentGroupID(), particle_bank_id);

    GSmodelLinkToGSparticleBank(model, particle_bank);
    GSmodelSetGSparticleLinkAttachMode(model, 4);
}

#endif

#if defined(FIELD_801CB180_RANGE_801CB59C)
s32 scriptCharacterDetach(u32 resource)
{
    u32 group = std093_getCurrentGroupID();
    void* model;

    if (peopleIsInGroup(group, resource) != NULL) {
        std168_characterDetachFromCharacter(group, resource);
    } else {
        model = GSresGetResource(group, resource);
        if (model == NULL) {
            return 0;
        }
        GSmodelDetachFromGSpart(model, 1);
    }

    return 1;
}
#endif

#if defined(FIELD_801CB180_RANGE_801CB61C)
s32 std166_mapObjectAttachToObject(u32 resource, u32 part_resource, s32 part)
{
    void* model;
    void* part_model;
    void* gs_part;
    u32 group = std093_getCurrentGroupID();

    if (peopleIsInGroup(group, resource) != NULL) {
        std070_characterAttachToCharacter(group, resource, group, part_resource, part);
    } else {
        model = GSresGetResource(group, resource);
        if (model == NULL) {
            return 0;
        }

        part_model = GSresGetResource(group, part_resource);
        if (part_model == NULL) {
            return 0;
        }

        gs_part = GSmodelGetPart(part_model, part);
        GSmodelAttachToGSpart(model, gs_part, 7, 0, 1);
        GSpartFree(gs_part);
    }

    return 1;
}

s32 scriptWaitSyncMotion(u32 resource, s32 wait)
{
    u32 group = std093_getCurrentGroupID();
    void* model;

    if (peopleIsInGroup(group, resource) != NULL) {
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
#endif

#if defined(FIELD_801CB180_RANGE_801CB7C4)
void std116_mapObjectStopAnimation(u32 resource)
{
    u32 group = std093_getCurrentGroupID();
    void* model;

    if (peopleIsInGroup(group, resource) != NULL) {
        peopleStopAnimation(group, resource);
    } else {
        model = GSresGetResource(group, resource);
        if (model != NULL) {
            GSmodelStopAnimation(model);
        }
    }
}
#endif

#if defined(FIELD_801CB180_RANGE_801CB834)
/* Built with the unit-wide -opt nopeephole (configure.py), like
 * field_range_801CB180.c. The unit owns its .sdata2 pool: the 0.5f rate
 * and the s32-to-float bias (0x8047E150-0x8047E160). */
void std115_mapObjectPlayAnimation(u32 resource, u32 anim_index, s32 frame, s32 loop)
{
    u32 group = std093_getCurrentGroupID();
    void* model;

    if (peopleIsInGroup(group, resource) != NULL) {
        std154_characterPlayAnimation(group, resource, anim_index, frame, (u8)loop);
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

void std114_mapObjectSetVisibility(u32 resource, s32 visible)
{
    u32 group = std093_getCurrentGroupID();
    void* model;

    if (peopleIsInGroup(group, resource) != NULL) {
        peopleSetVisible(group, resource, (u8)visible);
    } else {
        model = GSresGetResource(group, resource);
        if (model != NULL) {
            GSmodelSetVisibility(model, (u8)visible);
        }
    }
}
#endif

#if defined(FIELD_801CB180_RANGE_801CB9D8)
void std113_mapObjectUnk_8018db68(u32 resource)
{
    peopleCloseByID(std093_getCurrentGroupID(), resource);
}
#endif

#if defined(FIELD_801CB180_RANGE_801CBA0C)
s32 std112_modelGetResourceID(void* param)
{
    s32 people_id;
    u32 raw_id;
    void* people;

    raw_id = lbl_8047B3C8;
    lbl_8047B3C8 = raw_id + 1;
    people_id = (s8)raw_id | 0x7FFE0000;

    people = peopleOpen(std093_getCurrentGroupID(), people_id, param);
    if (people == NULL) {
        return 0;
    }

    GSmodelSetBoundCheck(peopleGetModel(peopleSearchID(people)), 0);
    return people_id;
}
#endif

#if defined(FIELD_801CB180_RANGE_801CBA84)
void fn_801CBA84(void)
{
    lbl_8047B3C8 = 0;
}
#endif

#if defined(FIELD_801CBA90_RANGE_PREFIX)
s32 std229_mapLoad395(void)
{
    floorRequestChange(0x395);
    return 0;
}

s32 std223_askYesNoThenWarp395(void)
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
            input = (s8)std011_dialogDisplayYesNoMenu();
            winMsgClose(1);
            if (input != 0) {
                done = 1;
            } else {
                state = 2;
            }
            break;
        case 2:
            if (menuSaveLoadSaveGame(1) == 0) {
                state = 3;
                result = 1;
            } else {
                state = 4;
            }
            break;
        case 3:
            floorRequestChange(0x395);
            state = 4;
            break;
        case 4:
            done = 1;
            break;
        }
    }

    return result;
}
#endif

/*
 * Save-data SHA-1 (0x801CBBAC - 0x801CDB04). sha1Calc, fn_801CBE44 and
 * fn_801CBF64 (SHA1Final) are linked from field_exact_801CBBAC.c,
 * field_exact_801CBE44.c and field_exact_801CBF64.c; sha1Transform
 * (SHA1Transform) from field_exact_801CC380.c.
 */
#include "game/save/savedata_sha1.h"

/* savedataDecryptCheck: see src/game/memcard.c (whole memory-card TU candidate). */




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

#if defined(FIELD_801CBA90_RANGE_801CC380)
void sha1Transform(u32 state[5], const u8 input[64])
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
#endif

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
/* memcardTaskMain: see src/game/memcard.c (whole memory-card TU candidate). */
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
        *random++ = GSrand();
    }
}

#if defined(FIELD_801CBA90_RANGE_801CF320)
s32 memcardSaveSetup(void)
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
            savedataBattleSetSavedHero(data, NULL);
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
        savedataBattleSetSavedHero(status14, default_status14->field_B328);
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
#endif

/* fn_801CF7E4 is linked from field_exact_801CF7E4.c. */
/* fn_801CF9C8: see src/game/memcard.c (whole memory-card TU candidate). */

#if defined(FIELD_801CBA90_RANGE_801CFD08)
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
        result = CARDOpen(channel, file_no, file_info);
        if (result == 0) {
            result = CARDGetStatus(channel, file_no, &status);
            if (result != 0) {
                goto invalid_file;
            }
            switch (GSgetVersionMajor()) {
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
        if (GSgetVersionMinor() == 4) {
            lbl_8047B3D4->disk_id = fn_800B01AC(lbl_8047B3D4->card_channel);
            lbl_8047B3D4->mounted_disk_id = *lbl_8047B3D4->disk_id;
            lbl_8047B3D4->mounted_disk_id.game_code = lbl_8047E168;
            lbl_8047B3D4->mounted_disk_id.company_code = lbl_8047E170;
            fn_800B01C4(lbl_8047B3D4->card_channel,
                       &lbl_8047B3D4->mounted_disk_id);
        }

        error = CARDGetStatus(lbl_8047B3D4->card_channel,
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
#endif

#if defined(FIELD_801CBA90_RANGE_801D0080)
void memcardRequestCancel(void)
{
    lbl_8047B3D4->callback_finished = 1;
}
#endif

/* fn_801D0090 is linked from field_exact_801D0090.c. */
