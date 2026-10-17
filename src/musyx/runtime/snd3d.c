/**
 * @file snd3d.c
 * @brief MusyX 3D emitters, listeners and rooms, 0x8015DEC0 - 0x8015FE88.
 *
 * Follows the reference MusyX runtime's snd3d.c (AxioDL/musyx) in its
 * 2.0.0 form (rooms and doors, SND_PARAMETER with a u8 controller). The
 * game never calls the room/door setup API, sndUpdateEmitter,
 * sndRemoveEmitter, the other sndAddEmitter* variants or
 * sndGet3DParameters, so the linker strips them and they are omitted;
 * sndAddListenerEx is kept because sndAddListener expands it (its own
 * copy is then unreferenced and dead-stripped, as are the static helpers
 * MWCC inlines: UpdateRoomDistances, the listener/room helpers,
 * CheckDoorStatus, SetFXParameters, EmitterShutdown, MakeListenerMatrix,
 * ClearStartList and AddRunningEmitter).
 *
 * The TU owns its statics, laid out by MWCC exactly as retail: .bss
 * 0x80448540 - 0x80449390 (AddEmitter's tmp_em, startGroup, runList,
 * startListNum), .sbss 0x8047B030 - 0x8047B050, and the literal pool
 * .sdata2 0x8047D468 - 0x8047D4B8.
 *
 * Built with -fp_contract off: retail keeps multiplies and adds separate
 * (e.g. CalcEmitter's distance and pan sums); CheckRoomStatus and
 * CalcEmitter drop to 94% and 75% with contraction on, and all twelve
 * functions are exact with that one unit-wide setting.
 */
#include "dolphin/types.h"
#include "musyx/runtime/hw_dspctrl.h"

typedef u32 SND_VOICEID;
typedef u16 SND_FXID;

typedef struct SND_FVECTOR {
    f32 x;
    f32 y;
    f32 z;
} SND_FVECTOR;

typedef struct SND_FMATRIX {
    f32 m[3][3];
    f32 t[3];
} SND_FMATRIX;

typedef struct SND_PARAMETER {
    u8 ctrl;
    union _paraData {
        u8 value7;
        u16 value14;
    } paraData;
} SND_PARAMETER;

typedef struct SND_PARAMETER_INFO {
    u8 numPara;
    SND_PARAMETER* paraArray;
} SND_PARAMETER_INFO;

typedef struct SND_ROOM {
    struct SND_ROOM* next;
    struct SND_ROOM* prev;
    u32 flags;
    SND_FVECTOR pos;
    f32 distance;
    u8 studio;
    void (*activateReverb)(u8 studio, void* para);
    void (*deActivateReverb)(u8 studio);
    void* user;
    u32 curMVol;
} SND_ROOM;

typedef struct SND_DOOR {
    struct SND_DOOR* next;
    struct SND_DOOR* prev;
    SND_FVECTOR pos;
    f32 open;
    f32 dampen;
    u8 fxVol;
    u8 destStudio;
    SND_ROOM* a;
    SND_ROOM* b;
    u32 flags;
    s16 filterCoef[4];
    SND_STUDIO_INPUT input;
} SND_DOOR;

typedef struct SND_LISTENER {
    struct SND_LISTENER* next;
    struct SND_LISTENER* prev;
    SND_ROOM* room;
    u32 flags;
    SND_FVECTOR pos;
    f32 volPosOff;
    SND_FVECTOR dir;
    SND_FVECTOR heading;
    SND_FVECTOR right;
    SND_FVECTOR up;
    SND_FMATRIX mat;
    f32 surroundDisFront;
    f32 surroundDisBack;
    f32 soundSpeed;
    f32 vol;
} SND_LISTENER;

typedef struct SND_EMITTER {
    struct SND_EMITTER* next;
    struct SND_EMITTER* prev;
    SND_ROOM* room;
    SND_PARAMETER_INFO* paraInfo;
    u32 flags;
    SND_FVECTOR pos;
    SND_FVECTOR dir;
    f32 maxDis;
    f32 maxVol;
    f32 minVol;
    f32 volPush;
    SND_VOICEID vid;
    u32 group;
    SND_FXID fxid;
    u8 studio;
    u8 maxVoices;
    u16 VolLevelCnt;
    f32 fade;
} SND_EMITTER;

typedef enum {
    SND_STUDIO_TYPE_STD = 0,
} SND_STUDIO_TYPE;

extern u8 lbl_8047AF18; /* sndActive */

extern void synthSendKeyOff(SND_VOICEID vid);
extern void synthActivateStudio(u8 studio, u32 isMaster, SND_STUDIO_TYPE type); /* synthActivateStudio */
extern void synthDeactivateStudio(u8 studio);                                   /* synthDeactivateStudio */
extern u32 synthAddStudioInput(u8 studio, SND_STUDIO_INPUT* in_desc);        /* synthAddStudioInput */
extern u32 synthRemoveStudioInput(u8 studio, SND_STUDIO_INPUT* in_desc);        /* synthRemoveStudioInput */
extern SND_VOICEID synthFXStart(u16 fid, u8 vol, u8 pan, u8 studio, u32 itd);
extern u32 synthFXSetCtrl(SND_VOICEID vid, u8 ctrl, u8 value);
extern u32 synthFXSetCtrl14(SND_VOICEID vid, u8 ctrl, u16 value);
extern u8 synthFXGetMaxVoices(u16 fid);
extern SND_VOICEID sndFXCheck(SND_VOICEID vid);
extern void hwDisableIrq(void);
extern void hwEnableIrq(void);
extern void salApplyMatrix(const SND_FMATRIX* mat, const SND_FVECTOR* in, SND_FVECTOR* out);
extern f32 salNormalizeVector(SND_FVECTOR* vec);
extern void salCrossProduct(SND_FVECTOR* out, const SND_FVECTOR* a, const SND_FVECTOR* b);
extern void salInvertMatrix(SND_FMATRIX* out, const SND_FMATRIX* in);
extern f64 __frsqrte(f64 value);

static inline f32 sqrtf(f32 x)
{
    volatile f32 y;

    if (x > 0.0f) {
        f64 guess = __frsqrte((f64) x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        y = (f32) (x * guess);
        return y;
    }
    return x;
}

static u8 s3dCallCnt;
static SND_EMITTER* s3dEmitterRoot;
static SND_LISTENER* s3dListenerRoot;
static SND_ROOM* s3dRoomRoot;
static SND_DOOR* s3dDoorRoot;
static u32 snd_used_studios;
static u8 snd_base_studio;
static u8 snd_max_studios;
static u8 s3dUseMaxVoices;

static void UpdateRoomDistances(void)
{
    SND_ROOM* r;
    SND_LISTENER* li;
    f32 distance;
    u32 n;
    SND_FVECTOR d;

    for (n = 0, li = s3dListenerRoot; li != NULL; li = li->next, ++n)
        ;

    if (n != 0) {
        for (r = s3dRoomRoot; r != NULL; r = r->next) {
            if (r->studio != 0xFF) {
                distance = 0.f;
                for (li = s3dListenerRoot; li != NULL; li = li->next) {
                    d.x = r->pos.x - li->pos.x;
                    d.y = r->pos.y - li->pos.y;
                    d.z = r->pos.z - li->pos.z;
                    distance += d.x * d.x + d.y * d.y + d.z * d.z;
                }

                r->distance = distance / n;
            }
        }
    }
}

void fn_8015DEC0(void) /* CheckRoomStatus */
{
    SND_LISTENER* li;
    SND_EMITTER* em;
    SND_ROOM* r;
    SND_ROOM* max_room;
    SND_ROOM* room;
    SND_FVECTOR d;
    f32 distance;
    f32 maxDis;
    u32 li_num;
    u32 i;
    u32 mask;
    u8 has_listener;

    UpdateRoomDistances();

    for (li_num = 0, li = s3dListenerRoot; li != NULL; li = li->next, ++li_num)
        ;

    if (li_num != 0) {
        for (room = s3dRoomRoot; room != NULL; room = room->next) {
            if (room->studio == 0xFF) {
                distance = 0.f;
                for (li = s3dListenerRoot; li != NULL; li = li->next) {
                    d.x = room->pos.x - li->pos.x;
                    d.y = room->pos.y - li->pos.y;
                    d.z = room->pos.z - li->pos.z;
                    distance += d.x * d.x + d.y * d.y + d.z * d.z;
                }

                distance = distance / li_num;

                has_listener = FALSE;
                for (li = s3dListenerRoot; li != NULL; li = li->next) {
                    if (li->room == room) {
                        has_listener = TRUE;
                        break;
                    }
                }

                mask = ~(-1 << snd_max_studios);

                if (mask != (snd_used_studios & mask)) {
                    for (i = 0; i < snd_max_studios; ++i) {
                        if (!(snd_used_studios & (1 << i))) {
                            break;
                        }
                    }

                    snd_used_studios |= (1 << i);
                    room->studio = i + snd_base_studio;
                } else {
                    maxDis = -1.f;

                    for (r = s3dRoomRoot; r != NULL; r = r->next) {
                        if (r->studio != 0xFF && maxDis < r->distance) {
                            maxDis = r->distance;
                            max_room = r;
                        }
                    }

                    if (has_listener || maxDis > distance) {
                        for (em = s3dEmitterRoot; em != NULL; em = em->next) {
                            if (em->room == max_room) {
                                synthSendKeyOff(em->vid);
                                em->flags |= 0x80000;
                                em->vid = -1;
                            }
                        }

                        if (max_room->deActivateReverb != NULL) {
                            max_room->deActivateReverb(max_room->studio);
                        }

                        synthDeactivateStudio(max_room->studio);
                        room->studio = max_room->studio;
                        max_room->studio = 0xFF;
                        max_room->flags = 0;
                    } else {
                        continue;
                    }
                }
                room->distance = distance;
                room->curMVol = has_listener ? 0x7F0000 : 0;

                if (room->curMVol * 1.2014794e-07f >= 0.5) {
                    synthActivateStudio(room->studio, TRUE, SND_STUDIO_TYPE_STD);
                } else {
                    synthActivateStudio(room->studio, FALSE, SND_STUDIO_TYPE_STD);
                }

                if (room->activateReverb != NULL) {
                    room->activateReverb(room->studio, room->user);
                }
            } else {
                if (room->flags & 0x80000000) {
                    room->curMVol += 0x40000;
                    if (room->curMVol >= 0x7F0000) {
                        room->curMVol = 0x7F0000;
                        room->flags &= ~0x80000000;
                    }

                    if (room->curMVol * 1.2014794e-07f >= 0.5) {
                        synthActivateStudio(room->studio, TRUE, SND_STUDIO_TYPE_STD);
                    } else {
                        synthActivateStudio(room->studio, FALSE, SND_STUDIO_TYPE_STD);
                    }
                }

                if ((room->flags & 0x40000000) != 0) {
                    room->curMVol = room->curMVol - 0x40000;
                    if ((int)room->curMVol >= 0) {
                        room->curMVol = 0;
                        room->flags &= ~0x40000000;
                    }
                    if (room->curMVol * 1.2014794e-07f >= 0.5) {
                        synthActivateStudio(room->studio, TRUE, SND_STUDIO_TYPE_STD);
                    } else {
                        synthActivateStudio(room->studio, FALSE, SND_STUDIO_TYPE_STD);
                    }
                }
            }
        }
    }
}

static void AddListener2Room(SND_ROOM* room)
{
    if (room->flags & 0x80000000) {
        return;
    }

    if (room->curMVol != 0) {
        return;
    }

    room->flags |= 0x80000000;
}

static void RemoveListenerFromRoom(SND_ROOM* room)
{
    u32 n;
    SND_LISTENER* li;

    for (n = 0, li = s3dListenerRoot; li != NULL; li = li->next) {
        if (li->room == room) {
            ++n;
        }
    }

    if (n == 1) {
        room->flags &= ~0x80000000;
        room->flags |= 0x40000000;
    }
}

static void CalcDoorParameters(SND_DOOR* door)
{
    f32 f;
    f32 v;

    v = door->open;
    f = (1.f - door->open) * door->dampen;
    door->input.volA = door->fxVol * v;
    door->input.volB = 0;
    door->input.vol = v * 127.f;
}

static void CheckDoorStatus(void)
{
    SND_DOOR* door;

    for (door = s3dDoorRoot; door != NULL; door = door->next) {
        if (!(door->flags & 0x80000000)) {
            if (door->a->studio != 0xFF) {
                if (door->b->studio != 0xFF) {
                    CalcDoorParameters(door);
                    if (door->flags & 1) {
                        door->input.srcStudio = door->b->studio;
                        synthAddStudioInput(door->a->studio, &door->input);
                    } else {
                        door->input.srcStudio = door->a->studio;
                        synthAddStudioInput(door->b->studio, &door->input);
                    }

                    door->flags |= 0x80000000;
                }
            }
        } else if (door->a->studio == 0xFF || door->b->studio == 0xFF) {
            if ((door->a->studio != 0xFF && door->a->studio == door->destStudio) ||
                (door->b->studio != 0xFF && door->b->studio == door->destStudio)) {
                synthRemoveStudioInput(door->destStudio, &door->input);
            }

            door->flags &= ~0x80000000;
        } else {
            CalcDoorParameters(door);
        }
    }
}

typedef struct START_LIST {
    struct START_LIST* next;
    f32 vol;
    f32 xPan;
    f32 yPan;
    f32 zPan;
    f32 pitch;
    SND_EMITTER* em;
} START_LIST;

typedef struct RUN_LIST {
    struct RUN_LIST* next;
    f32 vol;
    SND_EMITTER* em;
} RUN_LIST;

typedef struct START_GROUP {
    u32 id;
    struct START_LIST* list;
    struct RUN_LIST* running;
    u16 numRunning;
} START_GROUP;

static START_GROUP startGroup[64];
static u8 startGroupNum;
static START_LIST startListNum[64];
static u8 startListNumnum;
static RUN_LIST runList[64];
static u8 runListNum;

void fn_8015E374(SND_EMITTER* em, f32* vol, f32* doppler, f32* xPan, f32* yPan, f32* zPan) /* CalcEmitter */
{
    SND_LISTENER* li;
    SND_FVECTOR d;
    SND_FVECTOR v;
    SND_FVECTOR p;
    f32 relspeed;
    f32 distance;
    f32 new_distance;
    f32 ft;
    f32 vd;
    SND_FVECTOR pan;
    u32 n;

    ft = 1.f / 60.f;
    *vol = 0.f;
    *doppler = 1.f;

    pan.x = pan.y = pan.z = 0.f;

    for (n = 0, li = s3dListenerRoot; li != NULL; li = li->next, ++n) {
        d.x = em->pos.x - (li->pos.x + li->heading.x * li->volPosOff);
        d.y = em->pos.y - (li->pos.y + li->heading.y * li->volPosOff);
        d.z = em->pos.z - (li->pos.z + li->heading.z * li->volPosOff);

        distance = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);

        if (em->maxDis >= distance) {
            vd = distance / em->maxDis;

            if (em->volPush >= 0.f) {
                *vol += li->vol *
                        (em->minVol + (em->maxVol - em->minVol) *
                                          (1.f - ((1.f - em->volPush) * vd + em->volPush * vd * vd)));
            } else {
                *vol += li->vol *
                        (em->minVol + (em->maxVol - em->minVol) *
                                          (1.f - ((em->volPush + 1.f) * vd -
                                                  em->volPush * (1.f - (1.f - vd) * (1.f - vd)))));
            }

            if (em->flags & 0x80000) {
                continue;
            }
            if ((em->flags & 0x8) || (li->flags & 1)) {
                v.x = li->dir.x - em->dir.x;
                v.y = li->dir.y - em->dir.y;
                v.z = li->dir.z - em->dir.z;
                relspeed = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

                if (relspeed > 0.f) {
                    d.x = (em->pos.x + em->dir.x * ft) - (li->pos.x + li->dir.x * ft);
                    d.y = (em->pos.y + em->dir.y * ft) - (li->pos.y + li->dir.y * ft);
                    d.z = (em->pos.z + em->dir.z * ft) - (li->pos.z + li->dir.z * ft);

                    new_distance = sqrtf(d.x * d.x + d.y * d.y + d.z * d.z);

                    if (new_distance < distance) {
                        *doppler = li->soundSpeed / (li->soundSpeed - relspeed);
                    } else {
                        *doppler = li->soundSpeed / (li->soundSpeed + relspeed);
                    }
                }
            }

            if (distance != 0.f) {
                salApplyMatrix(&li->mat, &em->pos, &p);

                if (p.z <= 0.f) {
                    pan.z += -li->surroundDisFront < p.z ? -p.z / li->surroundDisFront : 1.f;
                } else {
                    pan.z += li->surroundDisBack > p.z ? -p.z / li->surroundDisBack : -1.f;
                }

                if (p.x != 0.f || p.y != 0.f || p.z != 0.f) {
                    salNormalizeVector(&p);
                }

                pan.x += p.x;
                pan.y -= p.y;
            }
        }
    }

    if (n != 0) {
        *xPan = pan.x / n;
        *yPan = pan.y / n;
        *zPan = pan.z / n;
    }
}

static u8 clip127(u8 v)
{
    if (v > 0x7F) {
        return 0x7F;
    }
    return v;
}

static u16 clip3FFF(u32 v)
{
    if (v > 0x3FFF) {
        return 0x3FFF;
    }
    return v;
}

static void SetFXParameters(SND_EMITTER* const em, f32 vol, f32 xPan, f32 yPan, f32 zPan, f32 doppler)
{
    SND_VOICEID vid;
    u8 i;
    SND_PARAMETER* pPtr;

    vid = em->vid;
    if ((em->flags & 0x100000) != 0) {
        synthFXSetCtrl(vid, 7, clip127((em->fade * vol) * 127.f));
    } else {
        synthFXSetCtrl(vid, 7, clip127(vol * 127.f));
    }

    synthFXSetCtrl(vid, 10, clip127((1.f + xPan) * 64.f));
    synthFXSetCtrl(vid, 131, clip127((1.f - zPan) * 64.f));
    synthFXSetCtrl14(vid, 132, clip3FFF(doppler * 8192.0f));

    if (em->paraInfo != NULL) {
        pPtr = em->paraInfo->paraArray;
        for (i = 0; i < em->paraInfo->numPara; ++pPtr, ++i) {
            if (pPtr->ctrl < 0x40 || pPtr->ctrl == 0x80 || pPtr->ctrl == 0x84) {
                synthFXSetCtrl14(vid, pPtr->ctrl, (pPtr->paraData).value14);
            } else {
                synthFXSetCtrl(vid, pPtr->ctrl, (pPtr->paraData).value7);
            }
        }
    }
}

static void EmitterShutdown(SND_EMITTER* em)
{
    if (em->next != NULL) {
        em->next->prev = em->prev;
    }

    if (em->prev != NULL) {
        em->prev->next = em->next;
    } else {
        s3dEmitterRoot = em->next;
    }

    em->flags &= 0xFFFF;
    if (em->vid != -1) {
        synthSendKeyOff(em->vid);
    }
}

u32 fn_8015E890(SND_EMITTER* em) /* sndCheckEmitter */
{
    if (lbl_8047AF18) {
        return (em->flags & 0x10000) != 0;
    }
    return FALSE;
}

SND_VOICEID fn_8015E8B0(SND_EMITTER* em_buffer, SND_FVECTOR* pos, SND_FVECTOR* dir, f32 maxDis,
                        f32 comp, u32 flags, u16 fxid, u32 groupid, u8 maxVol, u8 minVol,
                        SND_ROOM* room, SND_PARAMETER_INFO* para, u8 studio) /* AddEmitter */
{
    static SND_EMITTER tmp_em;
    SND_EMITTER* em;
    f32 xPan;
    f32 yPan;
    f32 zPan;
    f32 cvol;
    f32 pitch;

    hwDisableIrq();
    em = em_buffer == NULL ? &tmp_em : em_buffer;

    em->flags = flags;
    em->pos = *pos;
    em->dir = *dir;
    em->maxDis = maxDis;
    em->fxid = fxid;
    em->maxVol = maxVol * (1.f / 127.f);
    em->minVol = minVol * (1.f / 127.f);
    em->volPush = comp;
    em->group = groupid;
    em->room = room;
    em->studio = studio;

    if (em_buffer == NULL) {
        if (em->room != NULL && em->room->studio == 0xFF) {
            hwEnableIrq();
            return -1;
        }
        fn_8015E374(em, &cvol, &pitch, &xPan, &yPan, &zPan);

        if (cvol == 0.f) {
            hwEnableIrq();
            return -1;
        } else {
            em->vid = synthFXStart(em->fxid, 127, 64, em->room != NULL ? em->room->studio : em->studio,
                                   (em->flags & 0x10) != 0);
            if (em->vid == -1) {
                hwEnableIrq();
                return -1;
            }
            SetFXParameters(em, cvol, xPan, yPan, zPan, pitch);
            hwEnableIrq();
            return em->vid;
        }
    } else {
        if ((em->next = s3dEmitterRoot) != NULL) {
            s3dEmitterRoot->prev = em;
        }

        em->prev = NULL;
        s3dEmitterRoot = em;
        em->paraInfo = para;
        em->vid = -1;
        em->VolLevelCnt = 0;
        em->flags |= 0x30000;
        em->maxVoices = synthFXGetMaxVoices(em->fxid);
    }

    hwEnableIrq();
    return -1;
}

SND_VOICEID sndAddEmitter(SND_EMITTER* em_buffer, SND_FVECTOR* pos, SND_FVECTOR* dir, f32 maxDis,
                          f32 comp, u32 flags, SND_FXID fxid, u8 maxVol, u8 minVol, SND_ROOM* room)
{
    if (lbl_8047AF18) {
        return fn_8015E8B0(em_buffer, pos, dir, maxDis, comp, flags, fxid, fxid | 0x80000000, maxVol,
                           minVol, room, NULL, 0);
    }

    return -1;
}

static void MakeListenerMatrix(SND_LISTENER* li)
{
    SND_FMATRIX mat;

    salCrossProduct(&li->right, &li->heading, &li->up);
    mat.m[0][0] = li->right.x;
    mat.m[1][0] = li->right.y;
    mat.m[2][0] = li->right.z;
    mat.m[0][1] = li->up.x;
    mat.m[1][1] = li->up.y;
    mat.m[2][1] = li->up.z;
    mat.m[0][2] = -li->heading.x;
    mat.m[1][2] = -li->heading.y;
    mat.m[2][2] = -li->heading.z;
    mat.t[0] = li->pos.x;
    mat.t[1] = li->pos.y;
    mat.t[2] = li->pos.z;
    salInvertMatrix(&li->mat, &mat);
}

u32 s3dUpdateListener(SND_LISTENER* li, SND_FVECTOR* pos, SND_FVECTOR* dir, SND_FVECTOR* heading,
                SND_FVECTOR* up, u8 vol, SND_ROOM* room) /* sndUpdateListener */
{
    if (lbl_8047AF18) {
        hwDisableIrq();
        li->pos = *pos;
        li->dir = *dir;
        li->heading = *heading;
        li->up = *up;

        MakeListenerMatrix(li);
        li->vol = vol / 127.f;

        if (room != li->room) {
            if (li->room != NULL) {
                RemoveListenerFromRoom(li->room);
            }

            li->room = room;
            if (room != NULL) {
                AddListener2Room(li->room);
            }
        }

        hwEnableIrq();
        return TRUE;
    }

    return FALSE;
}

u32 sndAddListenerEx(SND_LISTENER* li, SND_FVECTOR* pos, SND_FVECTOR* dir, SND_FVECTOR* heading,
                     SND_FVECTOR* up, f32 front_sur, f32 back_sur, f32 soundSpeed, f32 volPosOffset,
                     u32 flags, u8 vol, SND_ROOM* room)
{
    if (lbl_8047AF18) {
        hwDisableIrq();
        if ((li->next = s3dListenerRoot) != NULL) {
            s3dListenerRoot->prev = li;
        }

        li->prev = NULL;
        s3dListenerRoot = li;
        li->pos = *pos;
        li->dir = *dir;
        li->heading = *heading;
        li->up = *up;
        li->surroundDisFront = front_sur;
        li->surroundDisBack = back_sur;
        li->soundSpeed = soundSpeed;
        li->volPosOff = volPosOffset;
        MakeListenerMatrix(li);
        li->flags = flags;
        li->vol = vol / 127.f;
        li->room = room;
        if (room != NULL) {
            AddListener2Room(room);
        }
        hwEnableIrq();
        return TRUE;
    }

    return FALSE;
}

u32 s3dSetListener(SND_LISTENER* li, SND_FVECTOR* pos, SND_FVECTOR* dir, SND_FVECTOR* heading,
                SND_FVECTOR* up, f32 front_sur, f32 back_sur, f32 soundSpeed, u32 flags, u8 vol,
                SND_ROOM* room) /* sndAddListener */
{
    return sndAddListenerEx(li, pos, dir, heading, up, front_sur, back_sur, soundSpeed, 0.f, flags,
                            vol, room);
}

static void ClearStartList(void)
{
    startGroupNum = 0;
    startListNumnum = 0;
    runListNum = 0;
}

static void AddRunningEmitter(SND_EMITTER* em, f32 vol)
{
    s32 i;
    RUN_LIST* rl;
    RUN_LIST* lrl;

    for (i = 0; i < startGroupNum; ++i) {
        if (em->group == startGroup[i].id) {
            break;
        }
    }

    if (i == startGroupNum) {
        startGroup[i].list = NULL;
        startGroup[i].running = NULL;
        startGroup[i].numRunning = 0;
        startGroup[i].id = em->group;
        ++startGroupNum;
    }

    ++startGroup[i].numRunning;

    lrl = NULL;
    for (rl = startGroup[i].running; rl != NULL; rl = rl->next) {
        if (rl->vol > vol) {
            break;
        }
        lrl = rl;
    }

    if (lrl == NULL) {
        startGroup[i].running = &runList[runListNum];
    } else {
        lrl->next = &runList[runListNum];
    }

    runList[runListNum].next = rl;
    runList[runListNum].em = em;
    runList[runListNum++].vol = vol;
}

u32 fn_8015F124(SND_EMITTER* em, f32 vol, f32 xPan, f32 yPan, f32 zPan, f32 pitch) /* AddStartingEmitter */
{
    s32 i;
    START_LIST* sl;

    for (i = 0; i < startGroupNum; ++i) {
        if (em->group == startGroup[i].id) {
            break;
        }
    }

    if (i == startGroupNum) {
        if (startGroupNum == 64) {
            return FALSE;
        }

        startGroup[i].list = NULL;
        startGroup[i].running = NULL;
        startGroup[i].numRunning = 0;
        startGroup[i].id = em->group;
        ++startGroupNum;
    }

    if (startListNumnum == 64) {
        return FALSE;
    }

    sl = startGroup[i].list;

    if (sl != NULL) {
        for (; sl->next != NULL; sl = sl->next) {
            if (sl->vol < vol) {
                break;
            }
        }
        startListNum[startListNumnum].next = sl->next;
        sl->next = &startListNum[startListNumnum];
    } else {
        startListNum[startListNumnum].next = startGroup[i].list;
        startGroup[i].list = &startListNum[startListNumnum];
    }

    startListNum[startListNumnum].em = em;
    startListNum[startListNumnum].pitch = pitch;
    startListNum[startListNumnum].xPan = xPan;
    startListNum[startListNumnum].yPan = yPan;
    startListNum[startListNumnum].zPan = zPan;
    startListNum[startListNumnum++].vol = vol;

    return TRUE;
}

void fn_8015F270(void) /* StartContinousEmitters */
{
    s32 i;
    START_LIST* sl;
    SND_EMITTER* em;
    f32 dv;

    for (i = 0; i < startGroupNum; ++i) {
        for (sl = startGroup[i].list; sl != NULL; sl = sl->next) {
            if ((startGroup[i].running != NULL) &&
                !(((s3dUseMaxVoices != '\0' && ((startGroup[i].id & 0x80000000) != 0)) &&
                   (startGroup[i].numRunning < startGroup[i].list->em->maxVoices)))) {
                dv = sl->vol - (startGroup[i].running)->vol;
                if (dv <= 0.08f) {
                    continue;
                } else if (dv <= 0.15f) {
                    if (++sl->em->VolLevelCnt < 20) {
                        continue;
                    }
                } else {
                    sl->em->VolLevelCnt = 0;
                }
            }
            em = sl->em;

            if (em->room != NULL && em->room->studio == 0xFF) {
                goto set_flags;
            }
            if ((em->vid = synthFXStart(em->fxid, 127, 64,
                                        em->room != NULL ? em->room->studio : em->studio,
                                        (em->flags & 0x10) != 0)) == -1) {
            set_flags:
                if (!(em->flags & 0x2)) {
                    em->flags |= 0x40000;
                    em->flags &= ~0x20000;
                }
            } else {
                if (!(em->flags & 0x20)) {
                    em->flags |= 0x100000;
                    em->fade = 0.f;
                } else {
                    em->fade = 1.f;
                }
                SetFXParameters(em, sl->vol, sl->xPan, sl->yPan, sl->zPan, sl->pitch);
                em->flags &= ~0x20000;
                ++startGroup[i].numRunning;
                if (startGroup[i].running != NULL) {
                    startGroup[i].running = startGroup[i].running->next;
                }
            }
        }
    }
}

void s3dHandle(void) /* s3dHandle */
{
    SND_EMITTER* em;
    SND_EMITTER* nem;
    f32 vol;
    f32 xPan;
    f32 yPan;
    f32 zPan;
    f32 pitch;

    if (s3dCallCnt != 0) {
        --s3dCallCnt;
        return;
    }
    s3dCallCnt = 3;
    ClearStartList();
    em = s3dEmitterRoot;
    for (; em != NULL; em = nem) {
        nem = em->next;
        if ((em->flags & 0x40000) != 0) {
            EmitterShutdown(em);
            continue;
        }
        if ((em->flags & 0x20001) != 0) {
            fn_8015E374(em, &vol, &pitch, &xPan, &yPan, &zPan);
        }

        if (!(em->flags & 0x80000)) {
            if (em->flags & 0x20000) {
                if (vol == 0.f && em->flags & 0x4) {
                    em->flags |= 0x80000;
                    em->flags &= ~0x20000;
                    goto found_emitter;
                } else if (vol == 0.f && em->flags & 0x40) {
                    EmitterShutdown(em);
                    continue;
                }

                if (em->flags & 1) {
                    if (fn_8015F124(em, vol, xPan, yPan, zPan, pitch)) {
                        continue;
                    }
                } else if (em->room == NULL || em->room->studio != 0xFF) {
                    if ((em->vid = synthFXStart(em->fxid, 127, 64,
                                                em->room != NULL ? em->room->studio : em->studio,
                                                (em->flags & 0x10) != 0)) == -1) {
                    derp:
                        if (!(em->flags & 2)) {
                            em->flags |= 0x40000;
                            em->flags &= ~0x20000;
                        } else {
                            continue;
                        }
                    }
                } else {
                    goto derp;
                }
            } else if ((em->vid = sndFXCheck(em->vid)) == -1) {
                if ((em->flags & 2)) {
                    em->flags |= 0x20000;
                } else {
                    em->flags |= 0x40000;
                }
            }

        found_emitter:
            if (em->vid != -1) {
                if ((em->flags & 1) != 0) {
                    AddRunningEmitter(em, vol);
                }
                if ((vol == 0.f) && ((em->flags & 4) != 0)) {
                    synthSendKeyOff(em->vid);
                    em->vid = 0xFFFFFFFF;
                    if ((em->flags & 2)) {
                        em->flags |= 0x80000;
                    } else {
                        em->flags |= 0x40000;
                    }
                } else {
                    SetFXParameters(em, vol, xPan, yPan, zPan, pitch);
                }
            }
            if ((em->flags & 0x100000) != 0) {
                em->fade += .3f;
                if (em->fade >= 1.f) {
                    em->flags &= ~0x100000;
                }
            }
        } else if ((em->room == NULL || (em->room != NULL && em->room->studio != 0xFF)) &&
                   vol != 0.f) {
            em->flags &= ~0x80000;
            em->flags |= 0x20000;
        }
    }
    fn_8015F270();
    fn_8015DEC0();
    CheckDoorStatus();
}

void s3dInit(u32 flags) /* s3dInit */
{
    s3dEmitterRoot = NULL;
    s3dListenerRoot = NULL;
    s3dRoomRoot = 0;
    s3dDoorRoot = 0;
    snd_used_studios = 0;
    snd_base_studio = 1;
    snd_max_studios = 3;
    s3dCallCnt = 0;
    s3dUseMaxVoices = ((flags & 2) != 0);
}

void s3dExit(void) /* s3dExit */
{
}
