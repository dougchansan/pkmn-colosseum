/** Candidate-only owner for 0x801D0C30 - 0x801D1338. */
#include "src/game/battle/battle_range_801D0AA0.c"

void fn_801D0DB0(s32 peopleGroup, s32 peopleId)
{
    typedef struct BattleIntroModelMap {
        u32 fieldId;
        u32 modelResId;
    } BattleIntroModelMap;
    extern u32 fn_80113F48(void);
    extern void* fn_80113D58(u32 id);
    extern void* fn_800F92D4(u32 id);
    extern void* fn_8018D998(s32 group, s32 id);
    extern void* peopleSearchID(void*);
    extern void* peopleGetPosition(void* person);
    extern void* fn_8018FCBC(void* person);
    extern void fn_800E0168(void* dst, const void* a, const void* b);
    extern void peopleMoveCheck(s32 group, s32 id, s32 wait);
    extern void fn_8018A280(s32 group, s32 id, s32 wait);
    extern void GSmodelSetAnimIndex(void* model, s32 index);
    extern void GSmodelSetAnimFrame(void* model, f32 frame);
    extern void GSmodelSetAnimRate(void* model, f32 rate);
    extern void GSmodelSetAnimType(void* model, s32 type);
    extern void GSmodelStartAnimation(void* model);
    extern u8 GSmodelIsAnimating(void* model);
    extern void GSmodelSetVisibility(void* model, s32 visible);
    extern void GSmodelFree(void* model);
    extern void fn_800ECCA8(void* model, s16 anim);
    extern void fn_800ECA78(void* model, f32 frame);
    extern void fn_800EC9DC(void* model, f32 rate);
    extern u8 fn_800EC960(void* model);
    extern void fn_800EC990(void* model);
    extern void fn_800ECB74(void* model, u32 type);
    extern void fn_800E43A4(void* model, void* pos, void* color);
    extern void fn_800E407C(void* model, void* scale);
    extern void fn_800E5BE0(void* model, void* color);
    extern void fn_800E4014(void* model, u8 visible);
    extern void fn_800E4BF4(void* model);
    extern s32 fn_801662E8(u32 arg0, u32 arg1);
    extern void fn_801668DC(u32 id, u32 volume, u32 arg2);
    extern void fn_80165548(u32 handle);
    extern void fn_801653CC(s32 id, s32 fade, s32 volume);
    extern void fn_80166AB8(s32 id, s32 pan, s32 volume);
    extern void _threadSwitch(void);
    extern f32 lbl_8047B3E0;
    extern f32 lbl_8047E188;
    extern f32 lbl_8047E18C;
    extern f32 lbl_8047E190;
    extern f32 lbl_8047E194;
    extern f32 lbl_8047E198;
    extern f64 lbl_8047E1A0;
    extern f64 lbl_8047E1A8;
    extern u32 lbl_8047E180;
    extern u8 lbl_80279320[12];
    extern u8 lbl_8027932C[12];
    extern u8 lbl_8036E030[176];
    extern void* lbl_80467378[6];
    BattleIntroModelMap* map;
    u32* partTable;
    u16* animTable;
    f32 pos[3];
    f32 personPosition[3];
    f32 delta[3];
    f32 scale[3];
    u32 color;
    void* personPos;
    void* person;
    void* effectModel;
    s32 handle;
    u16 count;
    u16 i;
    u8 hadPerson;
    s32 state;
    f32 timer;
    f32 frameDelta;

    map = (BattleIntroModelMap*)(lbl_8036E030 + 0x58);
    partTable = (u32*)(lbl_8036E030 + 0x34);
    animTable = (u16*)(lbl_8036E030 + 0x4C);
    memcpy(scale, lbl_8027932C, sizeof(scale));
    color = lbl_8047E180;
    effectModel = NULL;
    for (i = 0; i < 11; i++) {
        if (map[i].fieldId == fn_80113F48()) {
            effectModel = fn_800F92D4(map[i].modelResId);
            break;
        }
    }
    count = fn_801D0AFC(0);
    for (i = 0; i < 6; i++) {
        lbl_80467378[i] = NULL;
    }
    person = peopleSearchID(fn_8018D998(peopleGroup, peopleId));
    hadPerson = person != NULL;
    if (person != NULL) {
        void* part = (void*)fn_800EE150((u32)effectModel, 0xE);

        fn_800EE3BC(part, pos, 0, 0);
        fn_800EE828(part);
        personPos = peopleGetPosition(person);
        ((u32*)personPosition)[0] = ((u32*)personPos)[0];
        ((u32*)personPosition)[1] = ((u32*)personPos)[1];
        ((u32*)personPosition)[2] = ((u32*)personPos)[2];
        fn_800E0168(delta, pos, fn_8018FCBC(person));
        fn_8018805C(peopleGroup, peopleId,
                    (f32)fn_800CE2D8(delta[0], delta[2]),
                    lbl_8047E18C);
        state = 1;
    } else {
        state = 3;
    }

    timer = lbl_8047E188;
    handle = -1;
    i = 0;
    while (1) {
        switch (state) {
        case 1:
            fn_8018A280(peopleGroup, peopleId, 1);
            state = 3;
            break;
        case 3:
            handle = fn_801662E8(0, 0x406);
            if ((u32)(handle + 0x10000) != 0xFFFFFFFFU) {
                fn_801668DC(handle, 0x1068, 0);
            }
            if ((u32)i < 6) {
                void* part = (void*)fn_800EE150((u32)effectModel, (s32)partTable[i]);

                fn_800EE3BC(part, pos, 0, 0);
                fn_800EE828(part);
                lbl_80467378[i] = fn_80113D58(partTable[fn_801D0AA0(i)]);
                fn_800E43A4(lbl_80467378[i], pos, lbl_80467378);
                fn_800E407C(lbl_80467378[i], scale);
                fn_800E5BE0(lbl_80467378[i], &color);
                fn_80166AB8(0x3C3, 0, 0);
            }
            state = 4;
            break;
        case 4:
            frameDelta = (f32)fn_800D3088() / (f32)fn_800D37CC();
            timer += frameDelta;
            lbl_8047B3E0 = frameDelta;
            if (timer >= lbl_8047E190) {
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
            if (effectModel != NULL) {
                GSmodelSetAnimIndex(effectModel, animTable[count - 1]);
                GSmodelSetAnimFrame(effectModel, lbl_8047E188);
                GSmodelSetAnimRate(effectModel, lbl_8047E194);
                GSmodelSetAnimType(effectModel, 0);
                GSmodelStartAnimation(effectModel);
            }
            fn_801653CC(0x19, 2000, 0xFF);
            state = 6;
            break;
        case 6:
            while (fn_800EC960(effectModel) != 0) {
                _threadSwitch();
            }
            timer = lbl_8047E188;
            while (timer < lbl_8047E198) {
                frameDelta = (f32)fn_800D3088() / (f32)fn_800D37CC();
                timer += frameDelta;
                lbl_8047B3E0 = frameDelta;
                _threadSwitch();
            }
            state = 7;
            break;
        case 7:
            for (i = 0; i < 6; i++) {
                if (lbl_80467378[i] != NULL) {
                    fn_800E4014(lbl_80467378[i], 0);
                    fn_800E4BF4(lbl_80467378[i]);
                }
            }
            timer = lbl_8047E188;
            while (timer < lbl_8047E198) {
                frameDelta = (f32)fn_800D3088() / (f32)fn_800D37CC();
                timer += frameDelta;
                lbl_8047B3E0 = frameDelta;
                _threadSwitch();
            }
            fn_80165548(handle);
            state = 8;
            break;
        case 8:
            if (hadPerson != 0) {
                peopleSearchID(fn_8018D998(peopleGroup, peopleId));
                fn_8018805C(peopleGroup, peopleId, personPosition[1],
                            lbl_8047E18C);
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
            return;
        }
    }
}
