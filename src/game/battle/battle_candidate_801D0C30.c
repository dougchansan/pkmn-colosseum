/**
 * Battle intro (party-ball send-out), 0x801D0C30 - 0x801D1338, with the TU's
 * .data tables 0x8036E030 - 0x8036E0E0 and .sdata2 pool 0x8047E180 - 0x8047E1B0.
 */
#include "dolphin/types.h"

extern u8 fn_801D0AA0(u32 index);
extern u16 fn_801D0AFC(s32 mode);

typedef struct BattleIntroColor {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} BattleIntroColor;

/* Retail keeps this ahead of fn_801D0C30's pooled float at 0x8047E180. */
static const BattleIntroColor battleIntroColor = {0x80, 0x80, 0x80, 0xFF};

void fn_801D0C30(void)
{
    extern void _threadSwitch(void);
    extern void fadeSet(f32 duration, s32 mode);
    extern s8 fadeCheck(s32 wait);
    extern u32 soundPauseMusic(void);
    extern void soundResumeMusic(u32 handle);
    extern void soundPlayFanfare(s32 id, s32 fade, s32 volume);
    s32 running;
    s32 state;
    s32 timer;
    u32 handle;

    running = 1;
    state = 0;
    timer = 0;
    do {
        switch (state) {
        case 0:
            timer = 0;
            fn_801D0AFC(1);
            state = 10;
            break;
        case 10:
            fadeSet(1.5f, 3);
            state = 12;
            handle = soundPauseMusic();
            break;
        case 12:
            if (fadeCheck(0) == 0) {
                state = 6;
            } else {
                _threadSwitch();
            }
            break;
        case 6:
            timer++;
            if (timer >= 10) {
                state = 8;
                soundPlayFanfare(0x19, 2000, 0xFF);
                timer = 0;
            } else {
                _threadSwitch();
            }
            break;
        case 8:
            timer++;
            if (timer >= 10) {
                state = 9;
            } else {
                _threadSwitch();
            }
            break;
        case 9:
            soundResumeMusic(handle);
            fadeSet(1.5f, 2);
            state = 11;
            break;
        case 11:
            if (fadeCheck(0) == 0) {
                state = 1000;
            } else {
                _threadSwitch();
            }
            break;
        case 1000:
            running = 0;
            break;
        }
    } while (running != 0);
}

typedef struct BattleIntroVec {
    f32 x;
    f32 y;
    f32 z;
} BattleIntroVec;

typedef struct BattleIntroModelMap {
    s32 fieldId;
    u32 modelResId;
} BattleIntroModelMap;

/* Retail keeps these four tables at 0x8036E030 (.data), in this order. */
static u32 battleIntroObjectIds[13] = {
    0x03640400, 0x03650400, 0x03640400, 0x036B0400, 0x03360400,
    0x036A0400, 0x03660400, 0x03400400, 0x03670400, 0x03690400,
    0x036C0400, 0x03630400, 0x03680400,
};
static u32 battleIntroPartIds[6] = {9, 10, 11, 12, 13, 14};
static s16 battleIntroAnims[6] = {1, 2, 3, 4, 5, 6};
static BattleIntroModelMap battleIntroModels[11] = {
    {0x20, 0x00651003},
    {0x37, 0x00A31009},
    {0x6E, 0x002B1004},
    {0x89, 0x00511004},
    {0x94, 0x02371003},
    {0x41, 0x00C21005},
    {0x49, 0x01DC1003},
    {0x5E, 0x02E41006},
    {0x5F, 0x02E51005},
    {0x3B, 0x00931003},
    {0x633, 0x10BB1005},
};
extern void* lbl_80467378[6];
extern f32 lbl_8047B3E0;
extern BattleIntroVec lbl_80279320;
extern BattleIntroVec lbl_8027932C;

extern s32 std093_getCurrentGroupID(void);
extern void* fn_800F92D4(u32 id);
extern void* peopleIsInGroup(s32 group, s32 id);
extern void* peopleSearchID(void* id);
extern BattleIntroVec* peopleGetPosition(void* person);
extern void* peopleGetPositionPtr(void* person);
extern void GSvecSub(BattleIntroVec* dst, BattleIntroVec* a, void* b);
extern void peopleStartTurn(s32 group, s32 id, f32 angle, f32 speed);
extern double atan2(double y, double x);
extern void peopleMoveCheck(s32 group, s32 id, s32 wait);
extern void* GSmodelGetPart(void* model, u32 part);
extern void GSpartGetTransform(void* part, BattleIntroVec* pos, void* rot,
                               void* scale);
extern void GSpartFree(void* part);
extern void* floorOpenObject(u32 id);
extern void GSmodelSetPosition(void* model, BattleIntroVec* pos);
extern void GSmodelSetScale(void* model, BattleIntroVec* scale);
extern void GSmodelEnableModulation(void* model, BattleIntroColor* color);
extern void GSmodelSetAnimIndex(void* model, s32 index);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelSetAnimType(void* model, s32 type);
extern void GSmodelStartAnimation(void* model);
extern u8 GSmodelIsAnimating(void* model);
extern void GSmodelSetVisibility(void* model, s32 visible);
extern void GSmodelFree(void* model);
extern u32 fn_801662E8(u32 arg0, u32 arg1);
extern void soundPauseSong(u32 handle, u32 volume, u32 arg2);
extern void soundResumeMusic(u32 handle);
extern void soundPlayFanfare(s32 id, s32 fade, s32 volume);
extern void soundStartRaw(s32 id, s32 pan, s32 volume);
extern s32 fn_800D37CC(void);
extern u32 GSgetFrameStep(void);
extern void _threadSwitch(void);

static inline u32 battleIntroFindModel(s32 fieldId, BattleIntroModelMap* map)
{
    s32 i;

    for (i = 0; i < 11; i++, map++) {
        if (map->fieldId == fieldId) {
            return map->modelResId;
        }
    }
    return 0;
}

void fn_801D0DB0(s32 peopleGroup, s32 peopleId)
{
    s32 count;
    void* model;
    u8 hadPerson;
    s32 state;
    s32 running;
    s32 i;
    u32 handle;
    BattleIntroVec pos;
    BattleIntroVec scale;
    BattleIntroVec personPos;
    BattleIntroVec delta;
    BattleIntroColor color;
    void* part;
    void* person;
    s32 j;
    s16 anim;
    f32 timer;
    f32 frameDelta;

    pos = lbl_80279320;
    scale = lbl_8027932C;
    color = battleIntroColor;
    running = 1;
    state = 0;
    timer = 0.0f;
    do {
        switch (state) {
        case 0:
            i = 0;
            model = fn_800F92D4(battleIntroFindModel(std093_getCurrentGroupID(), battleIntroModels));
            count = fn_801D0AFC(0);
            for (j = 0; j < 6; j++) {
                lbl_80467378[j] = NULL;
            }
            person = peopleSearchID(peopleIsInGroup(peopleGroup, peopleId));
            if (person != NULL) {
                hadPerson = 1;
                part = GSmodelGetPart(model, 0xE);
                GSpartGetTransform(part, &pos, NULL, NULL);
                GSpartFree(part);
                personPos = *peopleGetPosition(person);
                GSvecSub(&delta, &pos, peopleGetPositionPtr(person));
                peopleStartTurn(peopleGroup, peopleId, atan2(delta.x, delta.z),
                            2.0f);
                state = 1;
            } else {
                hadPerson = 0;
                state = 3;
            }
            break;
        case 1:
            peopleMoveCheck(peopleGroup, peopleId, 1);
            state = 3;
            break;
        case 3:
            {
                u32 se = fn_801662E8(0, 0x406);

                if (se != -1) {
                    soundPauseSong(se, 0x1068, 0);
                }
                handle = se;
            }
            timer = 0.0f;
            part = GSmodelGetPart(model, battleIntroPartIds[i]);
            GSpartGetTransform(part, &pos, NULL, NULL);
            GSpartFree(part);
            lbl_80467378[i] = floorOpenObject(battleIntroObjectIds[fn_801D0AA0((u16)i)]);
            GSmodelSetPosition(lbl_80467378[i], &pos);
            GSmodelSetScale(lbl_80467378[i], &scale);
            GSmodelEnableModulation(lbl_80467378[i], &color);
            soundStartRaw(0x3C3, 0, 0);
            state = 4;
            break;
        case 4:
            frameDelta = (f32)GSgetFrameStep() / (f32)fn_800D37CC();
            timer += frameDelta;
            lbl_8047B3E0 = frameDelta;
            if (timer >= 0.5f) {
                i++;
                if (i == count) {
                    state = 5;
                } else {
                    state = 3;
                }
            } else {
                _threadSwitch();
            }
            break;
        case 5:
            anim = battleIntroAnims[count - 1];
            if (model != NULL) {
                GSmodelSetAnimIndex(model, anim);
                GSmodelSetAnimFrame(model, 0.0f);
                GSmodelSetAnimRate(model, 1.0f);
                GSmodelSetAnimType(model, 0);
                GSmodelStartAnimation(model);
            }
            soundPlayFanfare(0x19, 2000, 0xFF);
            state = 6;
            break;
        case 6:
            while (GSmodelIsAnimating(model)) {
                _threadSwitch();
            }
            timer = 0.0f;
            while (timer < 0.7f) {
                frameDelta = (f32)GSgetFrameStep() / (f32)fn_800D37CC();
                timer += frameDelta;
                lbl_8047B3E0 = frameDelta;
                _threadSwitch();
            }
            state = 7;
            break;
        case 7:
            for (j = 0; j < 6; j++) {
                if (lbl_80467378[j] != NULL) {
                    GSmodelSetVisibility(lbl_80467378[j], 0);
                    GSmodelFree(lbl_80467378[j]);
                }
            }
            timer = 0.0f;
            while (timer < 0.7f) {
                frameDelta = (f32)GSgetFrameStep() / (f32)fn_800D37CC();
                timer += frameDelta;
                lbl_8047B3E0 = frameDelta;
                _threadSwitch();
            }
            soundResumeMusic(handle);
            state = 8;
            break;
        case 8:
            if (hadPerson) {
                peopleSearchID(peopleIsInGroup(peopleGroup, peopleId));
                peopleStartTurn(peopleGroup, peopleId, personPos.y, 2.0f);
                state = 2;
            } else {
                state = 1000;
            }
            break;
        case 2:
            peopleMoveCheck(peopleGroup, peopleId, 1);
            state = 1000;
            break;
        case 1000:
            running = 0;
            break;
        }
    } while (running);
}
