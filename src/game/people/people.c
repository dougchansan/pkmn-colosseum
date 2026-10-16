/**
 * @file people.c
 * @brief The people (NPC) translation unit: per-frame update, movement,
 *        turning, motions, look-at, interaction queries and open/close.
 *
 * Extent (from the retail sections):
 *   .text    0x801812C4 - 0x8018F470
 *   .rodata  0x80273F90 - 0x802741F8  (initializer images, then the log
 *                                      strings; the TU addresses the block
 *                                      from its base, so it is one object)
 *   .data    0x8036C4E8 - 0x8036C53C  (__FUNCTION__ names: peopleOpenSub,
 *                                      peopleWaitSyncMotion(Blend),
 *                                      peopleMoveCheck)
 *   .sbss    0x8047B1F0 - 0x8047B1F8  (the two shadow lights)
 *   .sdata2  0x8047D798 - 0x8047D8A8  (the literal pool; it ends with
 *                                      2500.0f plus padding)
 *
 * The TU ends at fn_8018F30C. 0x8018F470-0x8018FE30 (peopleInit, the entry
 * accessors, the info-bios getters and the save/load hooks) is a separate
 * unit: it has its own .sdata2 pool (0x8047D8A8-0x8047D8B8, repeating 0.0f,
 * 1.0f and the degree factor this pool already holds), its own .sbss
 * (0x8047B1F8-0x8047B208) and its own switch table, and this TU calls even
 * its eight-byte getters (peopleGetMaxCount) instead of inlining them.
 *
 * Build (one flag set for the whole unit): GC/2.0 -O4,p with
 * -inline auto,deferred and -str reuse,readonly (since 2026-09-28, lane
 * P30d; before that -inline noauto,deferred, see below). The evidence:
 *   - GC/2.0 rather than GC/1.3 (checked 2026-09-28): every function that
 *     is exact under GC/1.3 stays exact, fn_8018D7D0 becomes exact (under
 *     GC/1.3 the masked index and the 0x7FFF0000 constant swap r0/r3; its
 *     linked carve people_candidate_8018D7D0_gc20.c was already GC/2.0),
 *     and fn_8018CD08's register differences shrink (56 -> 48 lines);
 *     GC/2.5-2.7 give the same code, GC/2.0p1 breaks 36 functions.
 *   - .text runs in the reverse of the .rodata initializer-image order and
 *     of the __FUNCTION__ order in .data, the layout MWCC's deferred mode
 *     gives a file written top-down; so the functions below appear in
 *     descending address order.
 *   - auto-inlining: retail expands the global peopleWaitSyncMotion into
 *     fn_801821B8 with that function's own __FUNCTION__ object, which only
 *     -inline auto gives (see the re-examination below). fn_80189990 still
 *     calls the 40-byte fn_80188F78 out of line under -inline auto: GC/2.0
 *     never auto-inlines a function whose body keeps a call to an in-TU
 *     candidate it could not expand (fn_80188F78 -> fn_80188FA0,
 *     fn_80185AAC -> fn_80188214; checked with one-line wrappers,
 *     2026-09-28);
 *   - the log formats are in .rodata (readonly strings) while the
 *     __FUNCTION__ names are in .data.
 * The lookups and other routines the TU expands at many call sites are
 * static inline below (see game/people/people_inline.h for the evidence).
 *
 * Built this way the .sdata2 pool pairs constant for constant with retail
 * and the .rodata strings and images follow in retail order. The two data
 * items below are closed by RULE-EXCEPTION(title-path) forms (user
 * directive, 2026-09-28): the unused zero GSvec local in fn_801845E4 and
 * the block-scope extern of fn_8018F30C in fn_80181850.
 *
 * Linked (2026-09-28, lane P30r): every function is exact, and the whole TU
 * is one Matching object with the .rodata/.data/.sbss/.sdata2 ranges above
 * (splits.txt `game/people/people.c`; main.dol SHA1 OK). It replaced the
 * score-instrumentation chunks and the carves that used to cover
 * 0x801812C4-0x8018F470 (people_candidate_*, people_r4x/r51_*,
 * people_exact_80183958...8018E920, people_candidate_8018D7D0_gc20), the
 * data units people_sdata2_8047D798.c and data_8036C52C.c, and the people
 * parts of rodata_80273A00.c and sdata2_8047D890.c (now rodata_80273A00.c
 * up to 0x80273F90, rodata_802741F8.c, and sdata2_8047D8A8.c). Title-path
 * rule exceptions in this TU (tagged RULE-EXCEPTION(title-path)): the zero
 * GSvec local in fn_801845E4, the block-scope extern in fn_80181850 (lane
 * P30d) and the peopleMoveAlongAngle helper of fn_80188214 (lane P30r).
 * objdiff scores the unit's data low (9%) only because the compiler's
 * @-literals do not pair by name with retail's lbl_ labels; the bytes are
 * identical (SHA1), and .rodata/.sdata2 differ only in their tail padding.
 *
 * Why 0x801812C4-0x80181EB0 (fn_801812C4, fn_801812E8, fn_80181478,
 * fn_80181850; all four 100%) cannot be linked as a standalone carve like
 * the people_exact_*.c pieces (checked 2026-09-28): compiled on their own
 * with this TU's flags, the four functions (with the XD-named
 * peopleMoveTypeRandomRot/peopleUpdateShadows inlines and the
 * people_inline.h lookups) give .text 0xBEC in retail order, and an .sdata2
 * of 0x40 bytes that is exactly retail 0x8047D798-0x8047D7D8: the head of
 * this TU's one deduplicated literal pool (75.0f, 1.0f, 0.0f, 0.5f, pi,
 * pi/2, 2pi as double and float, 2.0f, then the signed and unsigned
 * int-to-float magic doubles). The pieces carved so far read pool literals
 * by symbol (lbl_8047D7A0, lbl_8047D828) and emit no .sdata2, but
 * fn_80181850 converts fn_800D37CC()'s and fn_800D3088()'s results to
 * float (the random-rotation wait and the per-tick step), and MWCC emits the
 * magic constants of such conversions itself; C cannot name
 * lbl_8047D7C8/lbl_8047D7D0 for them. The object therefore has its own
 * pool. Giving the carve 0x8047D798-0x8047D7D8 does not work either: those
 * constants are local @-literals there, while the rest of the TU reads the
 * same bytes by name (people_exact_80188984.c's lbl_8047D7A0 and the
 * unlinked pieces' asm, which reference lbl_8047D798-lbl_8047D7D0), so the
 * link would lose those globals; only aliases or named stand-in constants
 * could keep them, and both are rejected. The range links with the whole
 * TU (.text 0x801812C4-0x8018F470 plus the .rodata/.data/.sbss/.sdata2
 * above as one object), as it now does (above). (peopleOpen,
 * peopleOpenSub and fn_8018CD08, which differed only in the 12-byte
 * .rodata image above, are exact since the image is in place.) Lane B30x
 * fixed peopleOpenSub's r19/r20 swap and fn_8018CD08's FPR colouring
 * (see those functions); the peopleTurnTo form that fixed the latter also
 * made fn_8018524C (99.34%) exact.
 * Pokemon XD offers no admissible helper for them under the sister-title
 * clause: no window of their wall regions scores 0.45 against
 * trevor403/xd-asm (b1087f18) with tools/find_inline_expansions.py;
 * fn_8018524C's counterpart _peopleMoveTypeList__FP13tagPeopleWorkb
 * (0x8029A0C8) was rewritten in XD; fn_80188214 is XD's peopleMoveForward
 * (0x8029C6C4) with peopleMoveAlongAngle (0x8029C194) expanded, but XD's
 * peopleMoveAlongAngle makes different calls (sin/cos,
 * GScolsys2HumanGetWalkHeight instead of fn_800E0718/GSvecTransformQuat);
 * fn_80188214 is exact through that helper form anyway, as a
 * RULE-EXCEPTION(title-path) (see fn_80188214; lane P30r, 2026-09-28).
 * (fn_80186B5C and fn_80189990 became exact on 2026-09-28, lane B30r;
 * fn_8018ECEC on 2026-09-28, lane P30r, through MSL's atan2f.)
 *
 * The two data items, re-examined 2026-09-28 (lane U1):
 *   - The sixth .rodata image (0x80273FCC). The images belong to
 *     fn_8018E920 (0x80273F90, the three bios callbacks), fn_8018CD08
 *     (0x80273F9C, read as base+0xC), fn_80188214 (0x80273FA8),
 *     fn_80186B5C (0x80273FB4) and fn_80184D80 (0x80273FC0); nothing in the
 *     TU reads 0x80273FCC, by symbol or as an offset from another base.
 *     GC/1.3 creates a local's initializer image when it parses the
 *     definition, so the images follow source order, and it keeps the
 *     image when the local is never read: controlled tests give an
 *     unreferenced image for a local that is never used, for one that is
 *     only written, and for a static inline that is never called; any
 *     read, even a conditional one, copies the image. So retail has one
 *     such local (or uncalled inline, or dead-stripped function) somewhere
 *     after fn_80184D80 in the source, and the code cannot say where.
 *     Pokemon XD does not help: its versions of these functions
 *     (_peopleMoveTypeRandomWalk 0x80299E54 and the rest of the TU) were
 *     rewritten without Vec images (xd-asm b1087f18), and no XD disc is
 *     available for its .rodata. An invented unused local or stand-in
 *     function is rejected, so the image stays open.
 *     Re-examined 2026-09-28 (lane B30d):
 *     . An unused local keeps no stack slot and changes no instruction
 *       (controlled GC/1.3 and GC/2.0 tests, also inside an expanded static
 *       inline), so no retail frame or code can point at its function.
 *     . The Pokemon XD JP demo's linker map (NXXJ01.map,
 *       StarsMmd/Colo-XD-PBR-symbol-maps 6b51d3af) lists people.o's live
 *       and stripped ("UNUSED") functions in object order, and its live
 *       ones run in exactly this TU's address order (_peopleDoNeckUpdate =
 *       fn_801812C4, peopleWalkPauseTalkMode = fn_801812E8,
 *       peopleWalkPause = fn_80181478, peopleDaemon = fn_80181850,
 *       peopleDispMark = fn_80181EB0, peopleFallAppear = fn_801821B8, ...,
 *       _peopleMoveTypeRandomWalk = fn_80184D80, _peopleMoveTypeList =
 *       fn_8018524C, _peopleMoveTypeLinear = fn_801858C4, _peopleMoveSub =
 *       fn_80185AAC, peopleUpdateAnimation = fn_80185B90). The image must
 *       come from below fn_80184D80 in that order. The stripped functions
 *       there are peopleSetHeroMove (8 bytes, before _peopleDoNeckUpdate)
 *       and peopleRandomWalkPause (0xA4, between peopleWalkPause and
 *       peopleUpdateShadow), which makes peopleRandomWalkPause the likeliest
 *       owner. The map's other stripped movers (_peopleWillMove,
 *       peopleMoveVector, peopleGetPosXYZ) sit above
 *       _peopleMoveTypeRandomWalk, so their images would come before
 *       fn_80184D80's. peopleRandomWalkPause has no body anywhere: it is
 *       UNUSED in the demo and absent from XD retail (TeamOrre/xd-decomp
 *       GXXE01 symbols.txt, trevor403/xd-asm). XD retail's
 *       peopleUpdateShadow (0x80298768) and peopleSetSpeedRate (0x802996B0),
 *       which Colosseum lacks in the same range, have no Vec local. So there
 *       is a likely owner but no body to reconstruct.
 *     . Not applied (a judgement call): an unused `GSvec offset = {0.0f,
 *       0.0f, 0.0f};` in peopleMoveTypeRandomRot, whose waiting code is
 *       fn_80184D80's (which has that local), puts the image at 0x80273FCC.
 *       fn_80181850 stays exact, peopleOpen becomes exact and the string
 *       offsets in fn_8018CD08/peopleOpenSub pair. But nothing shows that
 *       RandomRot had that local, and the NXXJ01.map ordering points at
 *       another function.
 *   - peopleWaitSyncMotion's __FUNCTION__ (.data 0x8036C4F8) is read by
 *     both peopleWaitSyncMotion (0x8018B1DC) and fn_801821B8 (0x80182D58),
 *     so retail expands the global function itself inside fn_801821B8.
 *     fn_8018D998 and peopleSearchID are likewise the out-of-line copies
 *     of the peopleFindSelf/peopleFindBySelf expansions. GC/1.3-2.6 in C
 *     mode give that shape (one __FUNCTION__ object, global function plus
 *     an expansion) only with -inline auto, and -inline auto,deferred (or
 *     all,deferred) on this TU leaves peopleWaitSyncMotion out of line in
 *     fn_801821B8 (0xD28 bytes, retail 0xE60) while wrongly expanding
 *     fn_80185AAC in fn_80184D80/fn_801858C4 and fn_801845E4 in
 *     fn_80181EB0. An explicit `inline` definition is expanded but not
 *     emitted out of line (its __FUNCTION__ becomes a weak
 *     __FUNCTION__$localstatic1$ object) unless its address is taken, and
 *     nothing in the TU takes it. No flag set or natural source found
 *     gives both.
 *     Re-examined 2026-09-28 (lane B30d): the GC/1.3 result above is
 *     specific to GC/1.3's auto-inline size limit. GC/1.3.2, GC/2.0 and
 *     later use a different size measure: about 30 simple statements
 *     against about 15 for GC/1.3, and it is measured before the callee's
 *     own explicit inlines expand. It has no per-caller budget and no
 *     definition-order effect under deferred. With that measure, and with
 *     fn_801821B8 calling the global peopleWaitSyncMotion (the static inline
 *     peopleSyncMotionWait removed), GC/2.0 -inline auto,deferred (the
 *     other flags unchanged) reproduces retail's decisions: fn_801821B8 is
 *     identical to retail (0xE60, peopleWaitSyncMotion expanded), .data
 *     holds exactly four __FUNCTION__ objects in retail order (peopleOpenSub,
 *     peopleWaitSyncMotion, ...Blend, peopleMoveCheck), and fn_80185AAC,
 *     fn_80188F78 and fn_80188FA0 stay out of line as in retail. Every
 *     other function is unchanged except two expansions retail does not
 *     make: fn_801845E4 into fn_80181EB0 (both calls) and fn_8018F30C into
 *     fn_80181850 (0x774 bytes, retail 0x660). Both are just under the
 *     limit. One extra store statement puts fn_801845E4 over it, two extra
 *     calls do, and three extra calls put fn_8018F30C over it. Nothing
 *     evidences a larger source form for either, and restating them only to
 *     cross the limit would be shaping. `-inline level=2` keeps fn_801845E4
 *     out of line but refuses depth-3 expansions retail makes, and cannot
 *     stop fn_8018F30C (depth 2, as deep as peopleWaitSyncMotion's).
 *     -inline auto therefore stays off. Its evidence is recorded here: it
 *     is the only mechanism found for the shared __FUNCTION__ object.
 *     Applied 2026-09-28 (lane P30d, title-path rule exceptions): the unit
 *     now builds with -inline auto,deferred; fn_801821B8 calls the global
 *     peopleWaitSyncMotion (the static inline twin is gone). An unused
 *     `GSvec offset = {0}` in fn_801845E4 (the orphan image's likeliest
 *     owner by position: fn_801845E4 comes after fn_80184D80 in source and
 *     no image-bearing function follows it) both places the sixth image at
 *     0x80273FCC and puts fn_801845E4 over the auto limit, so one construct
 *     explains both retail artifacts; fn_8018F30C is kept out of
 *     fn_80181850 by a block-scope extern. Every other function keeps its
 *     code, and peopleOpen, peopleOpenSub and fn_8018CD08 become exact.
 *     GC/2.0's -inline auto has two limits (probes 2026-09-28): the
 *     callee's own lowered statement count (29 plain statements; an `if`
 *     costs 2, a `for` about 6) and its size with its explicit inlines
 *     expanded (about 72 calls' worth); fn_8018F30C passes both, and a
 *     natural -inline auto source that calls the global twins instead of
 *     the static inlines loses retail's expansions of fn_8018B76C (over the
 *     statement limit), so the static inline twins stay.
 * fn_8018D7D0 was not exact under GC/1.3 (retail computes
 * (index & 0x7FFF0000) in r3 and the constant in r0, GC/1.3 swaps them);
 * it is exact since the unit moved to GC/2.0 (above).
 */
#include "dolphin/types.h"
#include "game/people/people.h"
#include "crt/math_ppc.h"

typedef struct GSvec {
    f32 x;
    f32 y;
    f32 z;
} GSvec;

/* One floor height hit from fn_8010E138. */
typedef struct PeopleFloorHit {
    f32 height;
    f32 field_04;
    f32 field_08;
} PeopleFloorHit;

/* A person's cylinder as registered with the human-collision system (fn_80110084). */
typedef struct PeopleHumanCollision {
    u32 groupId;
    u32 index;
    f32 radius;
    f32 height;
} PeopleHumanCollision;

/*
 * Floor save/restore handler triple (load, save, size), the layout
 * fn_800FF4D4 copies into its GSFloorResHandler table.
 */
typedef struct PeopleFloorResFuncs {
    void* func[3];
} PeopleFloorResFuncs;

/* ===== Engine ===== */
extern void GSlogWrite(const char* fmt, ...);
extern void GSlogWritef(const char* fmt, ...);
extern void* memset(void* dst, int val, u32 size);
extern void _threadSwitch(void);
extern void GSthreadBlock(void* thread);
extern void GSthreadUnblock(void* thread);
extern u32 GSgetFrameStep(void);
extern s32 fn_800D37CC(void);
extern f32 fn_800E0BA0(void);
extern f32 fn_800E0BE4(void);

/* GSvec */
extern void GSvecCopy(void* dst, void* src);
extern void set__5GSvecFfff(void* vec, f32 x, f32 y, f32 z);
extern void GSvecAdd(void* dst, void* a, void* b);
extern void GSvecSub(void* dst, void* a, void* b);
extern void vecScale(void* dst, void* src, f32 scale);
extern f32 vecMag(void* vec);
extern void vecNormalize(void* dst, void* src, f32 length);
extern f32 fn_800E0000(void* a, void* b);
extern void fn_800E0060(void* dst, void* src);
extern void fn_800E0718(void* quat, void* axis, f32 angle);
extern void GSvecTransformQuat(void* dst, void* quat, void* src);
extern f32 GSvecDistance(void* a, void* b);
extern void PSVECAdd(void* a, void* b, void* out);
extern void PSVECSubtract(void* a, void* b, void* out);
extern f32 PSVECSquareDistance(void* a, void* b);
extern GSvec lbl_8031554C; /* the Y axis */

/* Memory */
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* fn_800E27B0(u32 handle);
extern void fn_800E24B0(u32 handle);
extern void fn_800E209C(u32 handle);

/* Models and parts */
extern void* GSresGetResource(u32 group, u32 id);
extern void GSresRegisterResource(void* resource, u32 group, u32 id, u32 flags);
extern void* floorOpenObject(s32 objectId);
extern void GSmodelFree(void* model);
extern void* GSmodelGetPositionPtr(void* model);
extern u8 GSmodelGetVisibility(void* model);
extern u8 GSmodelHasAnimationEnded(void* model);
extern u8 GSmodelIsAnimating(void* model);
extern u8 GSmodelIsBlending(void* model);
extern void GSmodelGetAnimIndex(void* model, s32* current, s32* secondary);
extern void GSmodelSetAnimIndex(void* model, s32 index);
extern void GSmodelSetAnimFrame(void* model, f32 frame);
extern void GSmodelSetAnimRate(void* model, f32 rate);
extern void GSmodelSetAnimType(void* model, s32 type);
extern void GSmodelSetTexAnimIndex(void* model, s32 index);
extern void GSmodelSetTexAnimFrame(void* model, f32 frame);
extern void GSmodelSetTexAnimRate(void* model, f32 rate);
extern void GSmodelStartAnimation(void* model);
extern void GSmodelStopAnimation(void* model);
extern void GSmodelGetFrameCount(void* model, f32* start, f32* end);
extern void GSmodelSetAnimBlend(void* model, s32 from, s32 to);
extern void GSmodelSetBlendFactor(void* model, f32 factor);
extern void GSmodelSetBlendAnimFrameForce(void* model, f32 frame, f32 blendFrame);
extern void GSmodelEnableAnimBlend(void* model);
extern void GSmodelSetBoundCheck(void* model, u32 enable);
extern void GSmodelSetShadowFlags(void* model, u32 flags);
extern void GSmodelClearShadowFlags(void* model, u32 flags);
extern void GSmodelSetShadowLight(void* model, void* light);
extern void GSmodelSetShadowSurface(void* model, s32 count, void* surfaces);
extern void* GSmodelGetPart(void* model, s32 index);
extern void GSmodelAttachToGSpart(void* model, void* part, s32 a, s32 b, s32 c);
extern void GSmodelDetachFromGSpart(void* model, s32 index);
extern void GSpartFree(void* part);
extern void GSpartGetTransform(void* part, void* out, void* a, void* b);
extern void GSpartRegisterRotation(void* part, void* rotation, s32 order);
extern void fn_800E3CC8(void* model, s32 mode);
extern void fn_800EE288(void* part);

/* Lights */
extern void* GSlightCreate(void);
extern void GSlightSetType(void* light, s32 type);
extern void GSlightSetActive(void* light, u8 active);
extern void GSlightSetTarget(void* light, void* target);
extern void GSlightSetPosition(void* light, void* position);

/* Pad, flags, scripts, messages */
extern s8 fn_800F7A7C(s32 pad, s32 stick);
extern s8 fn_800F7A08(s32 pad, s32 stick);
extern u32 fn_800F7BC4(s32 pad);
extern void* fn_800F7108(u16 id);
extern u16 fn_800F7318(u32 type, u32 scriptId, u32 stackSize, u32 a, u32 b, s32 argc, ...);
extern void fn_800F9210(u32 groupId, u32 index);
extern void fn_800FF4D4(void* funcs, u8 typeId);
extern void fn_80101B90(u32 color);
extern void soundStartSE(u32 id);
extern u8 std081_flagCheckTrue(u32 flag);
extern void msgctrlSetValue(u32 id, u32 value);
extern void winMsgClose(s32 mode);
extern s32 winMsgCheckField(void);
extern void winMsgOpenFieldWithSE(s32 messageId, s32 a, s32 b, s32 value);
extern u32 std112_modelGetResourceID(u32 id);
extern void std115_mapObjectPlayAnimation(u32 index, u32 a, u32 b, u32 c);

/* Camera, hero, floor, collision */
extern void* cameraGetActive(void);
extern f32 cameraGetRotY(void);
extern void _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID(void);
extern u8 heroMoveIsMember(s32 member);
extern u32 heroMoveGetResID(u32* group, u32* index, s32 member);
extern void heroMoveSetEventList(u8 kind, void* events, s32 count);
extern void heroMoveSetLockFrame(s32 frames);
extern u8* fn_801170A4(u32 groupId, u32 index);
extern u32 floorCharacterBiosGetNameID(u8* character);
extern u8 floorCharacterBiosGetTalkStartType(u8* character);
extern u8 floorCharacterBiosGetTalkWallThrough(u8* character);
extern u32 floorCharacterBiosGetMoveSctID(u8* character);
extern u32 floorCharacterBiosGetTalkSctID(u8* character);
extern u16 charNameBiosGetHearFlag(u32 nameId);
extern u32 charNameBiosGetNameID(u32 nameId);
extern void* floorEventGetTresureList(u32 index);
extern void* floorDataBiosGetCurrentPtr(void);
extern u32 floorDataBiosGetShadowReciveNum(void* floor);
extern u32 floorDataBiosGetShadowReciveID(void* floor, u32 index);
extern u32 floorDataBiosGetShadowLightID(void* floor);
extern u32 std093_getCurrentGroupID(void);
extern u8 GScolsys2WalkGetLayer(void* position, u8* layer, u8* subLayer);
extern s32 GScolsys2ThruGetEventID(void* from, void* to, void* events, f32 radius);
extern s32 GScolsys2HumanCollision(s32 id, void* from, void* to, void* hit);
extern s32 fn_8010E138(void* position, PeopleFloorHit* hits);
extern s32 fn_8010F188(void* from, void* to, void* hit, f32 radius);
extern s32 colsysWallSweep(void* from, void* to, void* hit, f32 radius);
extern s32 fn_80110084(s32* result, void* query);
extern s32 colsysPassGetEventID(void* from, void* to, void* events);
extern f32 GScolsy2UtilGetSidePlanePoint(void* normal, void* verts, void* point);
extern void GScolsy2UtilGetCpPlanePoint(void* out, void* normal, void* verts, void* point);
extern s32 GScolsy2UtilChkInTri(void* point, void* verts, void* normal);
extern void GScolsy2UtilGetPointExtentionLine(void* out, void* from, void* to, f32 length);

/* Debug drawing */
extern void fn_800D258C(void);
extern void gfxSetCullMode(s32 a);
extern void gfxSetVtxDesc(void* a);
extern void gfxSetFlags(s32 a);
extern void gfxClearFlags(s32 a);
extern void gfxSetBlendMode(s32 a, s32 b, s32 c);
extern void fn_800D9ED8(s32 a);
extern void gfxSetPrimitive(s32 a);
extern void gfxBegin(s32 a);
extern void gfxVtxPos(f32 x, f32 y, f32 z);
extern void fn_800D5CB8(s32 a, s32 b, s32 c, s32 d, s32 e);
extern void gfxEnd(void);
extern u8 lbl_80314638[];

/* ===== The people core unit (0x8018F470-0x8018FE30) ===== */
extern u32 fn_8018F490(const PeopleInfoBiosEntry* info);
extern u32 characterModelGetTalkSE(const PeopleInfoBiosEntry* info);
extern void fn_8018F4C8(void* info, u8 motion, s32* outAnim, u8* outLoop);
extern f32 fn_8018F5B4(const PeopleInfoBiosEntry* info);
extern f32 fn_8018F5CC(const PeopleInfoBiosEntry* info);
extern f32 fn_8018F5E4(const PeopleInfoBiosEntry* info);

extern f32 charModelGetHeadPitchMax(const PeopleInfoBiosEntry* info);
extern f32 charModelGetHeadPitchMin(const PeopleInfoBiosEntry* info);
extern f32 charModelGetHeadYawMax(const PeopleInfoBiosEntry* info);
extern f32 charModelGetHeadYawMin(const PeopleInfoBiosEntry* info);
extern s8 charModelGetHeadPart(const PeopleInfoBiosEntry* info);
extern void* peopleInfoBiosGetPtr(void* scriptObj);
extern void peopleBiosPopData(u8* src, u32 size);
extern void peopleBiosPushData(u8* dst, u32 size);
extern u32 peopleBiosGetPushDataSize(void);
extern void peopleSetHumanCollision(PeopleEntry* entry, u8 animId);
extern void fn_8018FB60(PeopleEntry* entry, u8 animId);
extern void fn_8018FC08(PeopleEntry* entry, void* rotation);
extern void peopleGetRotation(PeopleEntry* entry, void* rotation);
extern GSvec* peopleGetPosition(PeopleEntry* entry);
extern void fn_8018FC74(PeopleEntry* entry, void* position);
extern void fn_8018FC98(PeopleEntry* entry, void* position);
extern void* peopleGetPositionPtr(PeopleEntry* entry);
extern PeopleEntry* peopleCreate(void);

/* ===== This unit ===== */
void* lbl_8047B1F0[2]; /* the shadow lights: player characters, others */
























/* ===== Functions of this unit (retail order is the reverse) ===== */
void fn_801812C4(PeopleEntry* entry);
s32 peopleTalkPause(u32 groupId, u32 index, u8 doInteract);
s32 std231_characterSetPathPaused(u32 groupId, u32 index, u8 doSetup);
void peopleMain(void);
void std131_characterSurpriseAnimation(u32 groupId, u32 index);
void std178_characterPlayAnimationAndWait(u32 groupId, u32 index);
void std077_characterEventFlagsSet(u32 groupId, u32 index);
void std076_characterEventFlagsClear(u32 groupId, u32 index);
s32 fn_80183688(PeopleEntry* self);
s32 fn_80183730(PeopleEntry* self);
s32 fn_801837D8(u32 groupId, u32 index, u32 flagId, u32 param1, u32 param2);
u32 fn_80183958(u32 groupId, u32 index);
u32 fn_8018397C(u32 groupId, u32 index);
s32 fn_801839A0(u32 groupId, u32 index, f32 field88, f32 field8C);
s32 fn_80183B44(u32 groupId, u32 index, f32 field80);
s32 std075_characterAllocPathAlt(u32 groupId, u32 index);
BOOL peopleStartWalkList(u32 groupId, u32 index, u32 loop);
s32 peopleAddWalkList(u32 groupId, u32 index, f32 x, f32 y, f32 z);
u8 peopleReserveWalkList(u32 groupId, u32 index, u16 count);
void std071_threadYield(void);
void std168_characterDetachFromCharacter(u32 groupId, u32 index);
void std070_characterAttachToCharacter(u32 groupId, u32 index, s32 group, s32 id, s32 partIndex);
void fn_801848D0(void* model, s32 group, s32 id, s32 partIndex);
void peopleSetWalkSpeed(u32 groupId, u32 index, f32 speed);
void peopleTurnUpdate(PeopleEntry* entry);
void fn_80184D80(PeopleEntry* entry);
void fn_8018524C(PeopleEntry* entry, u8 loop);
void fn_801858C4(PeopleEntry* entry);
s32 fn_80185AAC(PeopleEntry* entry);
void fn_80185B90(PeopleEntry* entry, f32 speed);
void std061_characterMoveToPosition(u32 groupId, u32 index, u8 keepFacing, f32 x, f32 y, f32 z);
void peopleSetRotationDeg(u32 groupId, u32 index, f32 x, f32 y, f32 z);
void std059_characterSetPosition(u32 groupId, u32 index, f32 x, f32 y, f32 z);
u8 peopleGazeHeroCheck(u32 groupId, u32 index);
u8 fn_80186284(u32 groupId, u32 index, f32 range, u32 targetGroupId, u32 targetIndex, f32 fov);
u8 std170_cameraFocusOnCharacter(u32 groupId, u32 index, u8 push, f32 x0, f32 z0, f32 x1, f32 z1);
GSvec fn_80186B5C(u32 groupId, u32 index);
u8 fn_801870E8(GSvec* position, GSvec* point, GSvec* start, GSvec* end, void* normal, f32 reach);
u8 std058_characterCheckWithinBounds(u32 groupId, u32 index, f32 x0, f32 z0, f32 x1, f32 z1);
void peopleStopTurn(u32 groupId, u32 index);
void std165_characterFaceCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex, f32 speed);
void peopleTurnToPoint(u32 groupId, u32 index, f32 x, f32 y, f32 z, f32 speed);
void peopleStartTurn(u32 groupId, u32 index, f32 yaw, f32 speed);
u8 peopleSetSpeedFromAmount(u32 groupId, u32 index, f32 speed);
void peopleMoveByVector(u32 groupId, u32 index, GSvec* offset, u8 face);
f32 peopleAmountFromMove(u32 groupId, u32 index, void* param3);
BOOL std155_characterWaitForLookAt(u32 groupId, u32 index, u8 wait);
void std067_characterStopLookAt(u32 groupId, u32 index);
void std066_characterLookAtPosition(u32 groupId, u32 index, s32 x, s32 y, s32 z);
void std065_characterLookAtPlayer(u32 groupId, u32 index);
void std181_characterTurnHeadTowardsCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex);
u8 fn_80189328(u32 groupId, u32 index, u8 enable);
void std055_characterEndSpeech(u32 groupId, u32 index);
void std054_characterTalk(u32 groupId, u32 index, s32 messageId);
BOOL peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag);
void std068_characterTurnToAngle(u32 groupId, u32 index, f32 amount);
void std177_characterMoveToCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex, u8 keepFacing, f32 distance);
void fn_8018AACC(u32 groupId, u32 index, u8 keepFacing, GSvec* target);
u8 peopleWaitSyncMotionBlend(u32 groupId, u32 index, u8 wait);
BOOL peopleWaitSyncMotion(u32 groupId, u32 index, u8 wait);
void fn_8018B220(u32 groupId, u32 index);
void std154_characterPlayAnimation(u32 groupId, u32 index, s32 animIndex, s32 frame, u8 looping);
u8 fn_8018B558(u32 groupId, u32 index, s32 blendAnimation, s32 animation, u32 frames);
u8 std051_characterSetAnimation(u32 groupId, u32 index, s32 animIndex, s32 frame, u8 loop);
void fn_8018BA04(u32 groupId, u32 index, GSvec* position);
void peopleGetPositionByIndex(u32 groupId, u32 index, s32 partIndex, GSvec* position);
void std172_characterGetPosition(u32 groupId, u32 index, GSvec* position);
void peopleSetRotation(u32 groupId, u32 index, GSvec* rotation);
void peopleSetPositionByIndex(u32 groupId, u32 index, void* position);
void peopleSetVisible(u32 groupId, u32 index, u8 visible);
u8 fn_8018C424(u32 groupId, u32 index, u32 mask);
u32 fn_8018C558(u32 groupId, u32 index);
void peopleClearFlagsByIndex(u32 groupId, u32 index, u32 mask);
void peopleSetFlagsByIndex(u32 groupId, u32 index, u32 mask);
void fn_8018C8F4(u32 groupId, u32 index, u32 flags);
void std151_characterFaceDirection(u32 groupId, u32 index, u8 visible);
void fn_8018CB5C(u32 groupId, u32 index);
PeopleEntry* peopleFindTalkTarget(u32 groupId, u32 index, f32 range, f32 fov);
u8 fn_8018D680(GSvec* a, GSvec* b, GSvec* point, f32 width);
u8 fn_8018D7D0(u32 groupId, u32 index);
PeopleEntry* peopleSearchID(PeopleEntry* self);
PeopleEntry* peopleIsInGroup(u32 groupId, u32 index);
void fn_8018DA88(void);
void fn_8018DB04(u8 releaseWalkList);
void fn_8018DB68(u32 groupId, u32 index);
void fn_8018DCA8(PeopleEntry* entry, u8 releaseWalkList);
void* peopleOpen(u32 groupId, u32 index, s32 objectId);
u8 peopleOpenSub(PeopleEntry* entry, u32 groupId, u32 index, s32 objectId);
void fn_8018E920(u32 maxPeople);
u8 peopleMoveCommit(PeopleEntry* entry, GSvec* position, GSvec* transform);
void peopleHeadLookUpdate(PeopleEntry* entry, f32 step);
void fn_8018F08C(PeopleEntry* entry, s32 motionIndex);
void fn_8018F30C(void);
/* ===== end prototypes ===== */

/*
 * ===== Routines the TU expands inline =====
 *
 * Each is also a function of this TU (named in its comment) whose body
 * retail expands, with the same instruction sequence, at many call sites;
 * see game/people/people_inline.h, which carries the same bodies for the
 * units carved out of this TU.
 */

/*
 * fn_8018D998: resolve (groupId, index) to a person's self pointer; if no
 * person of that group has the index, fall back to any group and warn.
 */
static inline PeopleEntry* peopleFindSelf(u32 groupId, u32 index)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        return entry->selfPtr;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite("Warining: people[%d,%d] group is different!!\n", groupId, index);
        return entry->selfPtr;
    }
    return NULL;
}

/* peopleSearchID: the active person whose self pointer is `found`. */
static inline PeopleEntry* peopleFindBySelf(PeopleEntry* found)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->selfPtr != found) continue;
        return entry;
    }
    return NULL;
}

/* fn_8018805C: start turning toward a yaw in the model's current revolution. */
static inline void peopleStartTurnInline(u32 groupId, u32 index, f32 yaw, f32 speed)
{
    PeopleEntry* entry;
    GSvec rotation;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleGetRotation(entry, &rotation);
        yaw += 6.2831855f * (s32)(rotation.y / 6.2831855f);
        entry->pad22 = 1;
        entry->field_40 = yaw;
        entry->field_44 = speed;
    }
}

/*
 * fn_801887D8: proximity ratio of a displacement against a person's
 * near/far distances (field_34/field_38): 0..1 inside near, 1..2 between
 * near and far, 2 beyond.
 */
static inline f32 peopleCalcRange(u32 groupId, u32 index, void* delta)
{
    PeopleEntry* entry;
    f32 result;
    f32 t;

    result = 0.0f;
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0.0f;
    }
    t = vecMag(delta);
    if (entry->field_38 <= t) {
        result = 2.0f;
    } else if (entry->field_34 <= t) {
        if (entry->field_38 != 0.0f) {
            result = 1.0f + (t - entry->field_34) / (entry->field_38 - entry->field_34);
        }
    } else if (entry->field_34 != 0.0f) {
        result = t / entry->field_34;
    }
    return result;
}

/*
 * fn_8018B76C: start a person's body/texture animation unless it already
 * plays that motion unblended, then set the loop mode.
 */
static inline u8 peopleSetMotion(u32 groupId, u32 index, s32 animIndex, s32 frame, u8 loop)
{
    PeopleEntry* entry;
    void* model;
    s32 current;
    s32 secondary;
    u8 restart = 0;

    if (animIndex < 0) {
        return 0;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return 0;
    }
    if (GSmodelHasAnimationEnded(model)) {
        restart = 1;
    } else if (!GSmodelIsAnimating(model)) {
        restart = 1;
    } else {
        GSmodelGetAnimIndex(model, &current, &secondary);
        if (current != animIndex || secondary != -1) {
            restart = 1;
        }
    }
    if (restart) {
        entry->walkTargetNode = animIndex;
        entry->walkAnimRate = 0.0f;
        GSmodelSetAnimIndex(model, animIndex);
        GSmodelSetAnimFrame(model, frame);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelSetTexAnimIndex(model, animIndex);
        GSmodelSetTexAnimFrame(model, frame);
        GSmodelSetTexAnimRate(model, 0.5f);
        if (loop) {
            GSmodelSetAnimType(model, 1);
        } else {
            GSmodelSetAnimType(model, 0);
        }
        GSmodelStartAnimation(model);
    }
    if (loop) {
        GSmodelSetAnimType(model, 1);
    } else {
        GSmodelSetAnimType(model, 0);
    }
    return restart;
}

/* fn_8018F08C: select a person's motion slot and play the animation it maps to. */
static inline void peopleSetMotionIndex(PeopleEntry* entry, s32 motionIndex)
{
    PeopleInfoBiosEntry* info;
    s32 animIndex;
    u8 loop;

    entry->motionIndex = motionIndex;
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    fn_8018F4C8(info, (u8)entry->motionIndex, &animIndex, &loop);
    if (animIndex == -1) {
        return;
    }
    peopleSetMotion(entry->groupId, entry->index, animIndex, 0, loop);
}

/* fn_8018C0A8: move a person's model to `position` and record it as the transform. */
static inline void peoplePlaceAt(u32 groupId, u32 index, void* position)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018FC74(entry, position);
        peopleSetTransform(entry, position);
    }
}

/* fn_8018CA20: show or hide a person's shadow; it stays hidden while the model is. */
static inline void peopleSetShadowVisible(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        if (entry->animId == 0) {
            visible = 0;
        }
        peopleSetHumanCollision(entry, visible);
    }
}

/*
 * Point a person's head part at `position` (kept in threadHandle).
 * fn_80188CA0 and fn_80188FA0 expand this same sequence.
 */
static inline void peopleSetLookTarget(u32 groupId, u32 index, void* position)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    void* model;
    void* part;
    s8 partIndex;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        model = peopleGetModel(entry);
        if (model != NULL) {
            info = peopleInfoBiosGetPtr(entry->scriptRef);
            if (info != NULL) {
                partIndex = charModelGetHeadPart(info);
                if (partIndex >= 0) {
                    entry->threadHandle = position;
                    part = GSmodelGetPart(model, partIndex);
                    GSpartRegisterRotation(part, entry->headRotation, 3);
                    GSpartFree(part);
                }
            }
        }
    }
}

/*
 * The player characters are people 100 and 101 of group 0. Both shadow-light
 * users (peopleOpenSub and fn_8018F30C) expand this test identically before
 * turning it into a light number with `!= TRUE`.
 */
static inline u8 peopleIsHero(u32 groupId, u32 index)
{
    if (groupId == 0 && (index == 100 || index == 101)) {
        return TRUE;
    }
    return FALSE;
}

/*
 * Turn from `facing` to the direction from `from` to `to` in the XZ plane,
 * wrapped into [-pi, pi] (fmod by a full turn after adding one, then fold the
 * halves). Expanded wherever the TU aims a person (fn_80184D80, fn_8018524C
 * twice, fn_80186284, fn_801885C4, fn_8018CD08 three times, fn_8018D680,
 * fn_8018ECEC); `facing` is read before the subtraction's call, as retail
 * loads it ahead of fn_800E0168.
 *
 * The heading (atan2's result) is stored in `angle` in its own statement
 * before the turn is taken from it (lane B30x, 2026-09-28). Every expansion
 * gives the same instructions either way, but the one-expression form left
 * two register walls: fn_8018CD08 (99.59%) and fn_8018524C (99.34%). The
 * GC/2.6 replay of fn_8018CD08 (the same code as GC/2.0) showed why: the
 * `width` argument of its peopleIsBetween expansion, which is live across
 * the nested expansion here, had one interference too many to be removed
 * in the allocator's first simplify pass, so it was coloured before the
 * loop's long-lived values and took f31 where retail has f24; a replay of
 * the dumped graph reaches retail's colours with any one of its
 * interferences removed, and with no renumbering alone. With the
 * two-statement form the replay gives `width` 32 interferences instead of
 * 33, it leaves the first pass there, and the colours are retail's. The
 * form makes both functions exact and leaves every other expansion
 * (fn_80184D80, fn_80186284, fn_801885C4, fn_8018D680, fn_8018ECEC)
 * unchanged. fn_8018AACC writes the same heading statement.
 */
static inline f32 peopleTurnTo(void* to, void* from, f32 facing)
{
    GSvec delta;
    f32 angle;

    GSvecSub(&delta, to, from);
    angle = (f32)atan2(delta.x, delta.z);
    angle = fmod(6.283185307179586 + (angle - facing), 6.283185307179586);
    if (angle > 3.141592653589793) {
        angle -= 6.283185307179586;
    } else if (angle < -3.141592653589793) {
        angle = 6.283185307179586 + angle;
    }
    return angle;
}

/*
 * Bring a yaw into [0, 2pi) by whole turns. fn_80184A90 expands it for the
 * model's yaw and for the target yaw.
 */
static inline f32 peopleNormalizeYaw(f32 yaw)
{
    if (yaw < 0.0f) {
        while (yaw < 0.0f) {
            yaw += 6.283185307179586;
        }
    } else {
        while (yaw >= 6.283185307179586) {
            yaw -= 6.283185307179586;
        }
    }
    return yaw;
}

/*
 * fn_8018D680: whether `point` lies within `width` of the segment a..b and
 * no farther along it than its half length (measured from the midpoint in
 * the segment's frame). fn_8018CD08 expands it for the party member check.
 * `point` is moved into the midpoint's frame.
 */
static inline u8 peopleIsBetween(GSvec* a, GSvec* b, GSvec* point, f32 width)
{
    GSvec midpoint;
    GSvec rotated;
    u8 rotation[16];
    f32 halfLength;

    GSvecAdd(&midpoint, a, b);
    vecScale(&midpoint, &midpoint, 0.5f);
    fn_800E0718(rotation, &lbl_8031554C, peopleTurnTo(b, a, 0.0f));
    GSvecSub(point, point, &midpoint);
    GSvecTransformQuat(&rotated, rotation, point);
    halfLength = GSvecDistance(&midpoint, b);
    if (width >= fabs(rotated.x) && halfLength >= fabs(rotated.z)) {
        return TRUE;
    }
    return FALSE;
}

/* fn_8018C558: a person's flag word (0 for a missing person). */
static inline u32 peopleGetFlagsByID(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    return entry->flags;
}

/* fn_80189328: set or clear a person's talkable flag; returns its previous state. */
static inline u8 peopleSetTalkable(u32 groupId, u32 index, u8 enable)
{
    PeopleEntry* entry;
    u8 wasTalkable;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    wasTalkable = peopleTestFlags(entry, PEOPLE_FLAG_TALKABLE);
    if (enable) {
        peopleSetFlags(entry, PEOPLE_FLAG_TALKABLE);
    } else {
        peopleClearFlags(entry, PEOPLE_FLAG_TALKABLE);
    }
    return wasTalkable;
}

/*
 * fn_80188984: whether a person's head is still turning toward its target.
 * With `wait`, yield until the turn has finished and return FALSE.
 */
static inline BOOL peopleIsHeadTurning(u32 groupId, u32 index, u8 wait)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    for (;;) {
        if (entry->headRotation[0] == entry->headTarget[0] &&
            entry->headRotation[1] == entry->headTarget[1]) {
            return FALSE;
        }
        if (wait) {
            _threadSwitch();
            continue;
        }
        return TRUE;
    }
}

/*
 * Pokemon XD peopleRotateCheck (.text 0x8029D188, size 0x70, global;
 * TeamOrre/xd-decomp config/GXXE01/symbols.txt @ 4989794e, body in
 * trevor403/xd-asm @ b1087f18, code/func_FUN_8029d188.s): whether a person
 * is still turning (pad22). With `wait`, yield until the turn has finished
 * and return FALSE. XD's body is peopleGet(groupId, index), then a loop on
 * the turning flag that calls GSthreadSwitch; XD's peopleTalkMsg
 * (0x802A3258), the counterpart of fn_80189990, calls it right after
 * peopleRotateToTarget (fn_80187A60 here). Colosseum expands it there,
 * with peopleGet's lookup written as the peopleFindSelf/peopleFindBySelf
 * pair.
 */
static inline BOOL peopleRotateCheck(u32 groupId, u32 index, u8 wait)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    for (;;) {
        if (!entry->pad22) {
            return FALSE;
        }
        if (wait) {
            _threadSwitch();
            continue;
        }
        return TRUE;
    }
}

/* fn_80187A60: turn a person toward the model of resource (targetGroupId, targetIndex). */
static inline void peopleTurnToModel(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex, f32 speed)
{
    PeopleEntry* entry;
    void* model;
    GSvec delta;
    GSvec targetPosition;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = GSresGetResource(targetGroupId, targetIndex);
    if (model == NULL) {
        return;
    }
    GSvecCopy(&targetPosition, GSmodelGetPositionPtr(model));
    GSvecSub(&delta, &targetPosition, peopleGetPositionPtr(entry));
    peopleStartTurnInline(groupId, index, atan2f(delta.x, delta.z), speed);
}

/*
 * Whether `point` touches the triangle `verts` (facing `normal`) within
 * `reach`: in front of its plane, the closest plane point near enough and
 * inside the triangle. On a hit `result` receives that point. fn_801870E8
 * expands it for each of the wall quad's four triangles.
 */
static inline BOOL peopleTouchTriangle(GSvec* result, void* normal, GSvec* verts, GSvec* point, f32 reach)
{
    GSvec closest;

    if (GScolsy2UtilGetSidePlanePoint(normal, verts, point) < 0.0f) {
        return FALSE;
    }
    GScolsy2UtilGetCpPlanePoint(&closest, normal, verts, point);
    if (PSVECSquareDistance(&closest, point) >= reach * reach) {
        return FALSE;
    }
    if (!GScolsy2UtilChkInTri(&closest, verts, normal)) {
        return FALSE;
    }
    *result = closest;
    return TRUE;
}

/* fn_8018BDF4: copy a person's model position into `position` (if given). */
static inline void peopleGetPositionByID(u32 groupId, u32 index, GSvec* position)
{
    PeopleEntry* entry;

    if (position == NULL) {
        return;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018FC98(entry, position);
    }
}

/* fn_8018C1E8: show or hide a person; its shadow follows. */
static inline void peopleSetVisibleInline(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    fn_8018FB60(entry, visible);
    peopleSetShadowVisible(groupId, index, visible);
}

/* fn_80183CE0: release a person's walk list (if any) and stop it walking. */
static inline s32 peopleFreeWalkList(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (entry->walkListHandle != 0) {
        fn_800E24B0(entry->walkListHandle);
        fn_800E209C(entry->walkListHandle);
        entry->walkList = NULL;
        entry->walkListHandle = 0;
        entry->state = 0;
        entry->walkListCapacity = 0;
        entry->walkListCount = 0;
        entry->subState = 0;
    }
    return 1;
}

/* fn_80184470: detach whatever a person holds (walk nodes A-C) from its model. */
static inline void peopleDetachHeld(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    void* model;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = GSresGetResource(groupId, index);
    if (model != NULL && entry->walkNodeC >= 0) {
        entry->walkNodeA = -1;
        entry->walkNodeB = -1;
        entry->walkNodeC = -1;
        GSmodelDetachFromGSpart(model, 1);
    }
}

/* fn_801848D0: attach part `partIndex` of the model of resource (group, id) to `model`. */
static inline void peopleAttachPart(void* model, s32 group, s32 id, s32 partIndex)
{
    void* resource;
    void* part;

    resource = GSresGetResource(group, id);
    if (resource != NULL) {
        part = GSmodelGetPart(resource, partIndex);
        GSmodelAttachToGSpart(model, part, 7, 0, 1);
        GSpartFree(part);
    }
}

/*
 * fn_8018BC88: the world position of part `partIndex` of a person's model,
 * or of the model itself for a negative part.
 */
static inline void peopleGetPartPosition(u32 groupId, u32 index, s32 partIndex, GSvec* position)
{
    PeopleEntry* entry;
    void* part;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    if (partIndex >= 0) {
        part = GSmodelGetPart(entry->modelHandle, partIndex);
        GSpartGetTransform(part, position, NULL, NULL);
        GSpartFree(part);
    } else {
        GSvecCopy(position, peopleGetPositionPtr(entry));
    }
}

/* fn_8018C8F4: overwrite a person's flag word. */
static inline void peopleWriteFlagsByID(u32 groupId, u32 index, u32 flags)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleWriteFlags(entry, flags);
    }
}

/* fn_8018B368: (re)start a person's body/texture animation from `frame`. */
static inline void peoplePlayMotion(u32 groupId, u32 index, s32 animIndex, s32 frame, u8 loop)
{
    PeopleEntry* entry;
    void* model;

    if (animIndex < 0) {
        return;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return;
    }
    entry->walkTargetNode = animIndex;
    entry->walkAnimRate = 0.0f;
    GSmodelSetAnimIndex(model, animIndex);
    GSmodelSetAnimFrame(model, frame);
    GSmodelSetAnimRate(model, 0.5f);
    GSmodelSetTexAnimIndex(model, animIndex);
    GSmodelSetTexAnimFrame(model, frame);
    GSmodelSetTexAnimRate(model, 0.5f);
    if (loop) {
        GSmodelSetAnimType(model, 1);
    } else {
        GSmodelSetAnimType(model, 0);
    }
    GSmodelStartAnimation(model);
}

/*
 * fn_8018BA04: the position of a person's head part (its model position
 * without one) at the height of its feet.
 */
static inline void peopleGetHeadPosition(u32 groupId, u32 index, GSvec* position)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    s8 partIndex;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    partIndex = charModelGetHeadPart(info);
    peopleGetPartPosition(groupId, index, partIndex, position);
    position->y = ((GSvec*)peopleGetPositionPtr(entry))->y;
}

/* Move `*value` toward `target` by `step` without overshooting. */
static inline void peopleApproach(f32* value, f32 target, f32 step)
{
    if (*value > target) {
        *value -= step;
        if (*value < target) {
            *value = target;
        }
    } else if (*value < target) {
        *value += step;
        if (*value > target) {
            *value = target;
        }
    }
}

/*
 * Aim the two shadow lights at the first visible person of each kind (light
 * 0: the player characters, light 1: everyone else) from 2500 units above.
 */
void fn_8018F30C(void)
{
    s32 lightIndex;
    s32 i;
    PeopleEntry* entry;
    void* model;
    GSvec position;
    u8 hero;

    if (peopleGetMaxCount() == 0) {
        return;
    }
    for (lightIndex = 0; lightIndex < 2; lightIndex++) {
        set__5GSvecFfff(&position, 0.0f, 0.0f, 0.0f);
        for (i = 0; i < peopleGetMaxCount(); i++) {
            entry = peopleGetEntry(i);
            if (!entry->active) {
                continue;
            }
            model = peopleGetModel(entry);
            if (model == NULL || !GSmodelGetVisibility(model)) {
                continue;
            }
            hero = peopleIsHero(entry->groupId, entry->index);
            if ((hero != TRUE) != lightIndex) {
                continue;
            }
            GSvecCopy(&position, peopleGetPositionPtr(entry));
            break;
        }
        position.y = 0.0f;
        GSlightSetTarget(lbl_8047B1F0[lightIndex], &position);
        position.y = 2500.0f;
        GSlightSetPosition(lbl_8047B1F0[lightIndex], &position);
    }
}

/* Select a person's motion slot and play the animation its info maps it to. */
void fn_8018F08C(PeopleEntry* entry, s32 motionIndex)
{
    PeopleInfoBiosEntry* info;
    s32 animIndex;
    u8 loop;

    entry->motionIndex = motionIndex;
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    fn_8018F4C8(info, (u8)entry->motionIndex, &animIndex, &loop);
    if (animIndex == -1) {
        return;
    }
    peopleSetMotion(entry->groupId, entry->index, animIndex, 0, loop);
}

/*
 * Turn a person's head toward its look target (threadHandle), within the
 * yaw/pitch limits of its info; `step` is the distance within which it
 * looks at all (a negative step, or flag 2, always looks). The head then
 * moves toward the target angles at 0.04 rad per tick.
 *
 * The pitch goes through MSL's atan2f (crt/math_ppc.h, the float wrapper
 * fn_80187D48 also uses), not a cast of atan2 (lane P30r, 2026-09-28;
 * 99.42% -> 100%). Retail adds 13 to lookDelta.y, stores it and keeps the
 * sum in f2 for atan2's second argument. With `(f32)atan2(...)` the read
 * of lookDelta.y sits in the call's own statement, after the two abs()
 * diamonds the front end hoists out of it, so the backend (which forwards
 * a store to a load only within a block and its successors that have no
 * other predecessor) reloads it. Expanding atan2f assigns lookDelta.y to
 * the inline's `x` parameter before those diamonds, in the block of the
 * store, and the sum is reused. The `+=` form also matters: MWCC's parser
 * puts a float constant first in a plain `a + 13.0f` (every GC version
 * from 1.2.5 to 3.0a3), so only a compound assignment gives retail's
 * y + 13 operand order.
 * Pokemon XD's counterpart, _peopleUpdateNeck__FP13tagPeopleWorkf
 * (0x8029FA4C, 0x63C bytes), was rewritten; no other Colosseum function
 * repeats the block. Name leads (NXXJ01.map): peopleApproach below is
 * the shape of XD's stripped _peopleUpdateRotation__FPfff (0x48).
 */
void peopleHeadLookUpdate(PeopleEntry* entry, f32 step)
{
    PeopleInfoBiosEntry* info;
    void* model;
    void* part;
    GSvec partPosition;
    GSvec rotation;
    GSvec lookDelta;
    f32 yawMin;
    f32 yawMax;
    f32 pitchMin;
    f32 pitchMax;
    f32 yaw;
    f32 pitch;
    f32 turn;
    s8 partIndex;
    u8 inRange = TRUE;
    u8 look = FALSE;

    if (entry == NULL) {
        return;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return;
    }

    if (entry->threadHandle != NULL) {
        info = peopleInfoBiosGetPtr(entry->scriptRef);
        partIndex = charModelGetHeadPart(info);
        yawMin = charModelGetHeadYawMin(info);
        yawMax = charModelGetHeadYawMax(info);
        pitchMin = charModelGetHeadPitchMin(info);
        pitchMax = charModelGetHeadPitchMax(info);
        if (partIndex < 0) {
            return;
        }
        part = GSmodelGetPart(model, partIndex);
        GSpartGetTransform(part, &partPosition, NULL, NULL);
        GSpartFree(part);

        if (peopleTestFlags(entry, 2)) {
            look = TRUE;
        } else if (step < 0.0f) {
            look = TRUE;
        } else if (GSvecDistance(entry->threadHandle, &partPosition) <= step) {
            look = TRUE;
        }

        if (look) {
            peopleGetRotation(entry, &rotation);
            yaw = peopleTurnTo(entry->threadHandle, &partPosition, rotation.y);
            if (yaw < yawMin) {
                if (peopleTestFlags(entry, 2)) {
                    yaw = yawMin;
                } else {
                    inRange = FALSE;
                }
            } else if (yaw > yawMax) {
                if (peopleTestFlags(entry, 2)) {
                    yaw = yawMax;
                } else {
                    inRange = FALSE;
                }
            }
            if (inRange) {
                entry->headTarget[1] = yaw;
                GSvecSub(&lookDelta, entry->threadHandle, &partPosition);
                lookDelta.y += 13.0f;
                pitch = atan2f((lookDelta.x > 0.0f ? lookDelta.x : -lookDelta.x) +
                                   (lookDelta.z > 0.0f ? lookDelta.z : -lookDelta.z),
                               lookDelta.y) -
                        1.5707963267948966;
                if (pitch < pitchMin) {
                    pitch = pitchMin;
                } else if (pitch > pitchMax) {
                    pitch = pitchMax;
                }
                entry->headTarget[0] = pitch;
            } else {
                entry->headTarget[0] = 0.0f;
                entry->headTarget[1] = 0.0f;
            }
        } else {
            entry->headTarget[0] = 0.0f;
                entry->headTarget[1] = 0.0f;
        }
    }

    turn = 0.04f * (f32)GSgetFrameStep();
    peopleApproach(&entry->headRotation[0], entry->headTarget[0], turn);
    peopleApproach(&entry->headRotation[1], entry->headTarget[1], turn);
}

/*
 * Move a person from `transform` (its last position) to `position`, applying
 * the collision checks its flags ask for; `position` receives the result
 * and the model is placed there. Always returns 1.
 */
u8 peopleMoveCommit(PeopleEntry* entry, GSvec* position, GSvec* transform)
{
    u8 events[0xD0];
    PeopleFloorHit hits[8];
    GSvec last;
    GSvec result;
    GSvec from;
    GSvec to;
    GSvec hit;
    GSvec delta;
    PeopleInfoBiosEntry* info;
    u8* resource;
    f32 radius;
    f32 bestAny;
    f32 bestStep;
    s32 count;
    s32 i;
    BOOL foundStep;

    resource = GSresGetResource(0, 2);
    if (resource != NULL && resource[1] != 0 && (fn_800F7BC4(1) & 0x200)) {
        fn_8018FC74(entry, position);
        return 1;
    }

    fn_80101B90(0xFF);
    if (peopleTestFlags(entry, 0x700)) {
        result = *position;
        last = *transform;
        GSvecCopy(&from, &last);
        GSvecCopy(&to, &result);
        from.y += 8.5f;
        to.y += 8.5f;

        info = peopleInfoBiosGetPtr(entry->scriptRef);
        if (info == NULL) {
            radius = 3.0f;
        } else {
            radius = fn_8018F5E4(info);
        }

        if (peopleTestFlags(entry, 0x800)) {
            count = GScolsys2ThruGetEventID(&from, &to, events, radius);
            heroMoveSetEventList(2, events, count);
        }

        result = to;
        if (peopleTestFlags(entry, 0x400)) {
            if (GScolsys2HumanCollision(entry->shadowId, &last, &result, &hit) == 6) {
                result = hit;
            }
        }

        if (peopleTestFlags(entry, 0x100)) {
            if (colsysWallSweep(&from, &result, &hit, radius) != 0) {
                PSVECSubtract(&hit, &result, &delta);
                PSVECAdd(&result, &delta, &result);
            }
        }

        if (peopleTestFlags(entry, 0x800)) {
            count = colsysPassGetEventID(&from, &result, events);
            heroMoveSetEventList(1, events, count);
        }

        if (peopleTestFlags(entry, 0x200)) {
            count = fn_8010E138(&result, hits);
            if (count >= 2) {
                bestStep = -1000000.0f;
                bestAny = bestStep;
                foundStep = FALSE;
                for (i = 0; i < count; i++) {
                    if (bestAny < hits[i].height) {
                        bestAny = hits[i].height;
                    }
                    if (hits[i].height - result.y >= 10.0f) {
                        continue;
                    }
                    if (bestStep < hits[i].height) {
                        bestStep = hits[i].height;
                        foundStep = TRUE;
                    }
                }
                if (foundStep) {
                    result.y = bestStep;
                } else {
                    result.y = bestAny;
                }
            } else if (count > 0) {
                result.y = hits[0].height;
            } else {
                result.y = 0.0f;
            }
        }

        *position = result;
    }
    fn_80101B90(0xFF0000);
    fn_8018FC74(entry, position);
    return 1;
}

void fn_8018E920(u32 maxPeople)
{
    PeopleFloorResFuncs funcs = {peopleBiosPopData, peopleBiosPushData, peopleBiosGetPushDataSize};
    s32 i;
    void** light;

    peopleInit(maxPeople);
    for (i = 0, light = lbl_8047B1F0; i < 2; i++, light++) {
        *light = GSlightCreate();
        GSlightSetType(*light, 2);
        GSlightSetActive(*light, 0);
    }
    fn_800FF4D4(&funcs, 1);
}

/*
 * peopleOpenSub: open person (groupId, index) from floor object `objectId`
 * into `entry`: register its model, start its idle motion, derive its walk
 * and run speeds from the motions' lengths, and set up its shadow.
 *
 * Name (formerly fn_8018E1C4): both "ERROR! [%s]: ..." logs below pass the
 * .data string "peopleOpenSub" (0x8036C4E8), this function's __FUNCTION__
 * object, as the other __FUNCTION__ users of the TU do with theirs; Pokemon
 * XD keeps a global peopleOpenSub (0x8029F044, TeamOrre/xd-decomp
 * symbols.txt) next to peopleOpen.
 *
 * The shadow light (lane B30x, 2026-09-28): retail colours `other` before
 * `light`, so `other` takes r19 and `light` r20. MWCC numbers a function's
 * locals in reverse declaration order and colours the higher number first,
 * so `other` is declared before `light`. The choice between the two lights
 * is a conditional expression assigned to `light` itself: the equivalent
 * if/else is turned into a conditional assigned to a new compiler
 * temporary (the replay dump names it @5434) that replaces `light` from
 * there on, and temporaries are coloured before every declared local, which
 * put `light` back in r19 whatever the declaration order. XD's
 * peopleSetInfo (0x8029EBBC, which XD's peopleOpenSub calls; xd-asm
 * b1087f18) keeps the same order: `other` r28, the light r30.
 */
u8 peopleOpenSub(PeopleEntry* entry, u32 groupId, u32 index, s32 objectId)
{
    PeopleInfoBiosEntry* info;
    void* model;
    BOOL other;
    void* light;
    u8* character;
    f32 frames;
    s32 animIndex;
    u8 loop;

    model = floorOpenObject(objectId);
    if (model == NULL) {
        return FALSE;
    }
    GSresRegisterResource(model, groupId, index, 0);
    entry->modelHandle = model;
    entry->groupId = groupId;
    entry->index = index;
    entry->scriptRef = (void*)objectId;
    entry->visible = 1;
    entry->animId = 1;
    peopleSetMotionIndex(entry, 1);
    peopleWriteFlagsByID(groupId, index, PEOPLE_WALK_LIST_ACTIVE);
    entry->walkNodeA = -1;
    entry->walkNodeB = -1;
    entry->walkNodeC = -1;
    entry->moveType = PEOPLE_MOVE_NONE;

    info = peopleInfoBiosGetPtr((void*)objectId);
    if (info != NULL) {
        fn_8018F4C8(info, 2, &animIndex, &loop);
        if (animIndex != -1) {
            GSmodelSetAnimIndex(model, animIndex);
            GSmodelGetFrameCount(model, &frames, NULL);
            if (frames > 0.0f) {
                entry->field_34 = fn_8018F5CC(info) / frames;
            } else {
                GSlogWrite("ERROR! [%s]: People[%d,%d] WalkMotion[%d] is frame zero.\n",
                           __FUNCTION__, groupId, index, animIndex);
            }
        }
        fn_8018F4C8(info, 3, &animIndex, &loop);
        if (animIndex != -1) {
            GSmodelSetAnimIndex(model, animIndex);
            GSmodelGetFrameCount(model, &frames, NULL);
            if (frames > 0.0f) {
                entry->field_38 = fn_8018F5B4(info) / frames;
            } else {
                GSlogWrite("ERROR! [%s]: People[%d,%d] RunMotion[%d] is frame zero.\n",
                           __FUNCTION__, groupId, index, animIndex);
            }
        }
        fn_8018F4C8(info, 1, &animIndex, &loop);
        if (animIndex != -1) {
            peoplePlayMotion(groupId, index, animIndex, 0, TRUE);
        }
        GSmodelSetBoundCheck(model, fn_8018F490(info));
    }

    character = fn_801170A4(groupId, index);
    if (character != NULL && (((u32)character[0] >> 3) & 1) == 1) {
        GSmodelEnableAnimBlend(entry->modelHandle);
    }
    floorDataBiosGetCurrentPtr();
    other = peopleIsHero(groupId, index) != TRUE;
    light = (void*)floorDataBiosGetShadowLightID(floorDataBiosGetCurrentPtr());
    light = light != NULL ? GSresGetResource(std093_getCurrentGroupID(), (u32)light) : lbl_8047B1F0[other];
    GSmodelSetShadowFlags(model, 1);
    if (!other) {
        GSmodelSetShadowFlags(model, 4);
    }
    GSmodelSetShadowLight(model, light);
    return TRUE;
}

/*
 * peopleOpen: open person (groupId, index) from floor object `objectId` in
 * a free slot; returns its self pointer, or NULL (logged) if it is already
 * open, no slot is free or the open fails.
 *
 * Name (formerly fn_8018E050): all three of its logs (.rodata
 * 0x802740E4/0x80274114/0x80274148) name it "peopleOpen(%08x,%08x)", and
 * no other code reads those strings. Pokemon XD's peopleOpen (0x8029E818,
 * TeamOrre/xd-decomp GXXE01 symbols.txt; body in trevor403/xd-asm
 * b1087f18) has the same skeleton without the logs: it returns NULL when
 * the person is already open (XD's peopleGetHumanID lookup) or no work slot
 * is free (peopleBiosGetNewWork, which is fn_8018FCE0's role here), calls peopleOpenSub(entry, groupId, index, objectId) and clears
 * the whole work with memset on failure, and returns the entry's self
 * pointer (+4). XD adds its own field set-up between the open and the
 * return. The XD demo map (NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps
 * 6b51d3af) lists peopleOpen in people.o next to peopleOpenSub, as here.
 */
void* peopleOpen(u32 groupId, u32 index, s32 objectId)
{
    PeopleEntry* entry;

    if (peopleFindSelf(groupId, index) != NULL) {
        GSlogWrite("Warning： peopleOpen(%08x,%08x) ２重オープン\n", groupId, index);
        return NULL;
    }
    entry = peopleCreate();
    if (entry == NULL) {
        GSlogWrite("エラー： peopleOpen(%08x,%08x) ワークの確保に失敗\n", groupId, index);
        return NULL;
    }
    if (!peopleOpenSub(entry, groupId, index, objectId)) {
        GSlogWrite("エラー： peopleOpen(%08x,%08x) 人のオープンに失敗\n", groupId, index);
        memset(entry, 0, PEOPLE_ENTRY_SIZE);
        return NULL;
    }
    return entry->selfPtr;
}

/*
 * Close a person: hide it and its shadow, optionally release its walk list,
 * end its script, free its model and release the slot.
 */
void fn_8018DCA8(PeopleEntry* entry, u8 releaseWalkList)
{
    peopleSetVisibleInline(entry->groupId, entry->index, FALSE);
    if (releaseWalkList) {
        peopleFreeWalkList(entry->groupId, entry->index);
    }
    fn_800F9210(entry->groupId, entry->index);
    if (entry->modelHandle != NULL) {
        GSmodelFree(entry->modelHandle);
        entry->modelHandle = NULL;
    }
    peopleFree(entry);
}

/* Close the person (groupId, index). */
void fn_8018DB68(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018DCA8(entry, 1);
    }
}

/* Close every active person. */
void fn_8018DB04(u8 releaseWalkList)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        fn_8018DCA8(entry, releaseWalkList);
    }
}

/* Hide every active person and block its script thread. */
void fn_8018DA88(void)
{
    s32 i;
    PeopleEntry* entry;
    void* thread;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry == NULL) continue;
        entry->visible = 0;
        thread = fn_800F7108(entry->flagId);
        if (thread != NULL) {
            GSthreadBlock(thread);
        }
    }
}

/*
 * Resolve (groupId, index) to a person's self pointer; if no person of that
 * group has the index, fall back to any group and warn.
 */
PeopleEntry* peopleIsInGroup(u32 groupId, u32 index)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->groupId != groupId) continue;
        if (entry->index != index) continue;
        return entry->selfPtr;
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->index != index) continue;
        GSlogWrite("Warining: people[%d,%d] group is different!!\n", groupId, index);
        return entry->selfPtr;
    }
    return NULL;
}

/* Find the active person whose self pointer is `self`. */
PeopleEntry* peopleSearchID(PeopleEntry* self)
{
    s32 i;
    PeopleEntry* entry;

    for (i = 0; i < peopleGetMaxCount(); i++) {
        entry = peopleGetEntry(i);
        if (!entry->active) continue;
        if (entry->selfPtr != self) continue;
        return entry;
    }
    return NULL;
}

u8 fn_8018D7D0(u32 groupId, u32 index)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    return (entry->index & 0x7FFF0000) == 0x7FFF0000;
}

/*
 * Whether `point` lies within `width` of the segment a..b and no farther
 * along it than its half length (measured from the midpoint in the
 * segment's frame). `point` is moved into the midpoint's frame.
 */
u8 fn_8018D680(GSvec* a, GSvec* b, GSvec* point, f32 width)
{
    GSvec midpoint;
    GSvec rotated;
    u8 rotation[16];
    f32 halfLength;

    GSvecAdd(&midpoint, a, b);
    vecScale(&midpoint, &midpoint, 0.5f);
    fn_800E0718(rotation, &lbl_8031554C, peopleTurnTo(b, a, 0.0f));
    GSvecSub(point, point, &midpoint);
    GSvecTransformQuat(&rotated, rotation, point);
    halfLength = GSvecDistance(&midpoint, b);
    if (width >= fabs(rotated.x) && halfLength >= fabs(rotated.z)) {
        return TRUE;
    }
    return FALSE;
}

/*
 * Pick the person (groupId, index) would talk to: the visible, talkable
 * person within `range` tenths of a unit (plus both radii) and inside the
 * `fov`-degree view, not behind a wall (unless its talk passes walls), not
 * a treasure facing away, and not screened by the party member. The best
 * scores lowest on distance^2 * |angle| / 2; ties go to the wider angle,
 * then to the left.
 *
 * Pokemon XD's version is peopleTalkCheck (0x802A3444, peopleTalk.o in the
 * NXXJ01 demo map; xd-asm b1087f18), with the same log format and the same
 * candidate filters, and peopleVecCalcRotY, peopleInsideCheck and
 * peopleGetNeckPos called out of line.
 *
 * Registers (lane B30x, 2026-09-28): retail colours the loop's long-lived
 * floats angle f31, distance f30, bestAngle f29, sourceRadius f28,
 * bestScore f27, fov f26, range f25, and the short-lived ones (the two
 * facings, peopleIsBetween's width, score) share f24. MWCC numbers a
 * function's locals in reverse declaration order and, when several are
 * left for the second simplify pass, colours the higher number first,
 * hence the declaration order below. sourceRadius is an if/else: the
 * conditional-expression form makes the front end replace it with a
 * compiler temporary, and temporaries are coloured before every declared
 * local. `angle` is replaced by peopleTurnTo's own result temporary, which
 * is why it still comes first. The width part needed peopleTurnTo's
 * two-statement heading (see there).
 */
PeopleEntry* peopleFindTalkTarget(u32 groupId, u32 index, f32 range, f32 fov)
{
    PeopleEntry* source;
    PeopleInfoBiosEntry* candidateInfo;
    PeopleEntry* candidate;
    PeopleEntry* best;
    PeopleInfoBiosEntry* info;
    u8* character;
    u8* treasure;
    GSvec candidatePosition;
    GSvec memberPosition = {0.0f, 0.0f, 0.0f};
    GSvec sourcePosition;
    GSvec rotation;
    f32 angle;
    f32 distance;
    f32 bestAngle; /* read before any assignment on an exact-score tie, as in retail */
    f32 sourceRadius;
    f32 bestScore;
    f32 score;
    s32 i;
    u32 hasMember;
    u8 checkWalls;
    u8 better;
    u32 memberGroup = 0;
    u32 memberIndex = 0;

    range = 10.0f * range;
    fov = 0.017453292f * fov;
    bestScore = 10000.0f;
    best = NULL;
    source = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (source == NULL) {
        return NULL;
    }
    GSvecCopy(&sourcePosition, peopleGetPositionPtr(source));
    info = peopleInfoBiosGetPtr(source->scriptRef);
    if (info != NULL) {
        sourceRadius = fn_8018F5E4(info);
    } else {
        sourceRadius = 4.0f;
    }
    range += sourceRadius;
    hasMember = heroMoveIsMember(1) != 0;
    heroMoveGetResID(&memberGroup, &memberIndex, 1);
    if (hasMember) {
        peopleGetPositionByID(memberGroup, memberIndex, &memberPosition);
    }

    for (i = 0; i < peopleGetMaxCount(); i++) {
        candidate = peopleGetEntry(i);
        if (!candidate->active) {
            continue;
        }
        if (source == candidate) {
            continue;
        }
        if (candidate->animId == 0) {
            continue;
        }
        if (peopleTestFlags(candidate, 1)) {
            continue;
        }
        character = fn_801170A4(candidate->groupId, candidate->index);
        if (character != NULL && floorCharacterBiosGetTalkStartType(character) == 3) {
            continue;
        }
        candidateInfo = peopleInfoBiosGetPtr(candidate->scriptRef);
        if (candidateInfo == NULL) {
            continue;
        }
        peopleGetHeadPosition(candidate->groupId, candidate->index, &candidatePosition);
        distance = GSvecDistance(&sourcePosition, &candidatePosition);
        if (distance > range + fn_8018F5E4(candidateInfo)) {
            continue;
        }
        angle = peopleTurnTo(&candidatePosition, &sourcePosition, source->field_40);
        if (fabs(angle) > fov) {
            continue;
        }
        if (character != NULL) {
            checkWalls = floorCharacterBiosGetTalkWallThrough(character) == 0;
        } else {
            treasure = floorEventGetTresureList(candidate->index);
            if (treasure != NULL && (((u32)treasure[0] >> 5) & 7) == 1) {
                peopleGetRotation(candidate, &rotation);
                if (fabs(peopleTurnTo(&sourcePosition, &candidatePosition, rotation.y)) > 1.0471976f) {
                    continue;
                }
            }
            checkWalls = FALSE;
        }
        if (checkWalls && colsysWallSweep(&sourcePosition, &candidatePosition, NULL, sourceRadius)) {
            continue;
        }
        if (hasMember && peopleIsBetween(&sourcePosition, &candidatePosition, &memberPosition,
                                         fn_8018F5E4(candidateInfo))) {
            continue;
        }
        score = distance * distance * fabs(angle) / 2.0;
        GSlogWritef("talk ->  people(%d,%d)  len =%.2f  ang =%.2f  area =%.2f\n",
                    candidate->groupId, candidate->index, distance, angle, score);
        better = FALSE;
        if (score < bestScore) {
            better = TRUE;
        } else if (score == bestScore) {
            if (fabs(bestAngle) < fabs(angle)) {
                better = TRUE;
            } else if (fabs(bestAngle) == fabs(angle) && angle > 0.0f) {
                better = TRUE;
            }
        }
        if (better) {
            bestScore = score;
            best = candidate;
            bestAngle = angle;
        }
    }
    return best;
}

/*
 * Register a person with the human-collision system (fn_80110084) if its
 * info asks for it (collision type 1) and it has no collision yet; the
 * cylinder has the info's radius and a height of 17.
 */
void fn_8018CB5C(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    PeopleHumanCollision collision;
    s32 id;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    if (entry->shadowId >= 0) {
        return;
    }
    if (fn_8018F5FC(info) != 1) {
        return;
    }
    collision.groupId = groupId;
    collision.index = index;
    collision.radius = fn_8018F5E4(info);
    collision.height = 17.0f;
    if (fn_80110084(&id, &collision) == 0) {
        entry->shadowId = id;
    } else {
        entry->shadowId = -1;
    }
}

/* Show or hide a person's shadow (peopleSetShadowVisible's body). */
void std151_characterFaceDirection(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        if (entry->animId == 0) {
            visible = 0;
        }
        peopleSetHumanCollision(entry, visible);
    }
}

void fn_8018C8F4(u32 groupId, u32 index, u32 flags)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleWriteFlags(entry, flags);
    }
}

void peopleSetFlagsByIndex(u32 groupId, u32 index, u32 mask)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleSetFlags(entry, mask);
    }
}

void peopleClearFlagsByIndex(u32 groupId, u32 index, u32 mask)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleClearFlags(entry, mask);
    }
}

/* A person's flag word (0 for a missing person). */
u32 fn_8018C558(u32 groupId, u32 index)
{
    return peopleGetFlagsByID(groupId, index);
}

u8 fn_8018C424(u32 groupId, u32 index, u32 mask)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    return peopleTestFlags(entry, mask);
}

/* Show or hide a person; its shadow follows (fn_8018CA20, expanded inline). */
void peopleSetVisible(u32 groupId, u32 index, u8 visible)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    fn_8018FB60(entry, visible);
    peopleSetShadowVisible(groupId, index, visible);
}

/* Move a person's model to `position` and record it as the transform. */
void peopleSetPositionByIndex(u32 groupId, u32 index, void* position)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018FC74(entry, position);
        peopleSetTransform(entry, position);
    }
}

/* Set a person's model rotation (angles wrapped to one turn) and facing. */
void peopleSetRotation(u32 groupId, u32 index, GSvec* rotation)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        rotation->x = fmod(6.283185307179586 + rotation->x, 6.283185307179586);
        rotation->y = fmod(6.283185307179586 + rotation->y, 6.283185307179586);
        rotation->z = fmod(6.283185307179586 + rotation->z, 6.283185307179586);
        fn_8018FC08(entry, rotation);
        entry->field_40 = rotation->y;
    }
}

/* Copy a person's model position into `position` (if given). */
void std172_characterGetPosition(u32 groupId, u32 index, GSvec* position)
{
    PeopleEntry* entry;

    if (position == NULL) {
        return;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        fn_8018FC98(entry, position);
    }
}

/*
 * The world position of part `partIndex` of a person's model, or of the
 * model itself for a negative part.
 */
void peopleGetPositionByIndex(u32 groupId, u32 index, s32 partIndex, GSvec* position)
{
    PeopleEntry* entry;
    void* part;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    if (partIndex >= 0) {
        part = GSmodelGetPart(entry->modelHandle, partIndex);
        GSpartGetTransform(part, position, NULL, NULL);
        GSpartFree(part);
    } else {
        GSvecCopy(position, peopleGetPositionPtr(entry));
    }
}

/*
 * The position of a person's head part (its model position without one)
 * at the height of its feet.
 */
void fn_8018BA04(u32 groupId, u32 index, GSvec* position)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    s8 partIndex;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    partIndex = charModelGetHeadPart(info);
    peopleGetPartPosition(groupId, index, partIndex, position);
    position->y = ((GSvec*)peopleGetPositionPtr(entry))->y;
}

u8 std051_characterSetAnimation(u32 groupId, u32 index, s32 animIndex, s32 frame, u8 loop) {
    PeopleEntry* entry;
    void* model;
    s32 current;
    s32 secondary;
    u8 restart = 0;

    if (animIndex < 0) {
        return 0;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return 0;
    }
    if (GSmodelHasAnimationEnded(model)) {
        restart = 1;
    } else if (!GSmodelIsAnimating(model)) {
        restart = 1;
    } else {
        GSmodelGetAnimIndex(model, &current, &secondary);
        if (current != animIndex || secondary != -1) {
            restart = 1;
        }
    }
    if (restart) {
        entry->walkTargetNode = animIndex;
        entry->walkAnimRate = 0.0f;
        GSmodelSetAnimIndex(model, animIndex);
        GSmodelSetAnimFrame(model, frame);
        GSmodelSetAnimRate(model, 0.5f);
        GSmodelSetTexAnimIndex(model, animIndex);
        GSmodelSetTexAnimFrame(model, frame);
        GSmodelSetTexAnimRate(model, 0.5f);
        if (loop) {
            GSmodelSetAnimType(model, 1);
        } else {
            GSmodelSetAnimType(model, 0);
        }
        GSmodelStartAnimation(model);
    }
    if (loop) {
        GSmodelSetAnimType(model, 1);
    } else {
        GSmodelSetAnimType(model, 0);
    }
    return restart;
}

u8 fn_8018B558(u32 groupId, u32 index, s32 blendAnimation,
                s32 animation, u32 frames) {
    PeopleEntry* entry;
    void* model;
    f32 frameCount;

    if (blendAnimation < 0 || animation < 0) {
        return 0;
    }
    if (frames < 1) {
        return 0;
    }
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return 0;
    }
    entry->walkTargetNode = animation;
    entry->walkAnimRate = 1.0f / (f32)frames;
    entry->syncMotion = 0.0f;
    GSmodelSetAnimBlend(model, blendAnimation, animation);
    GSmodelSetAnimFrame(model, 0.0f);
    GSmodelSetAnimRate(model, 0.0f);
    GSmodelSetBlendFactor(model, entry->syncMotion);
    GSmodelSetAnimType(model, 0);
    GSmodelStartAnimation(model);
    GSmodelGetFrameCount(model, &frameCount, 0);
    GSmodelSetBlendAnimFrameForce(model, frameCount - 1.5f,
                                  0.0f);
    return 1;
}

void std154_characterPlayAnimation(u32 groupId, u32 index, s32 animIndex, s32 frame,
                 u8 looping) {
    PeopleEntry* entry;
    void* model;

    if (animIndex < 0) {
        return;
    }

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }

    model = peopleGetModel(entry);
    if (model == NULL) {
        return;
    }

    entry->walkTargetNode = animIndex;
    entry->walkAnimRate = 0.0f;
    GSmodelSetAnimIndex(model, animIndex);
    GSmodelSetAnimFrame(model, (f32)frame);
    GSmodelSetAnimRate(model, 0.5f);
    GSmodelSetTexAnimIndex(model, animIndex);
    GSmodelSetTexAnimFrame(model, (f32)frame);
    GSmodelSetTexAnimRate(model, 0.5f);
    if (looping != 0) {
        GSmodelSetAnimType(model, 1);
    } else {
        GSmodelSetAnimType(model, 0);
    }
    GSmodelStartAnimation(model);
}

/* Stop a person's animation. */
void fn_8018B220(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    void* model;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        model = peopleGetModel(entry);
        if (model != NULL) {
            GSmodelStopAnimation(model);
        }
    }
}

/*
 * Whether a person's motion is still playing. With `wait`, yield until it
 * ends (logging and giving up if the motion loops) and return FALSE.
 */
BOOL peopleWaitSyncMotion(u32 groupId, u32 index, u8 wait)
{
    PeopleEntry* entry;
    void* model;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return FALSE;
    }
    for (;;) {
        if (GSmodelHasAnimationEnded(model)) {
            return FALSE;
        }
        if (!wait) {
            break;
        }
        if (*(s32*)((u8*)model + 0x8C) == 1) {
            GSlogWrite("[%s] people[%d,%d] ループモーションがおわるまでまとうとしました\n",
                       __FUNCTION__, groupId, index);
            return FALSE;
        }
        _threadSwitch();
    }
    return TRUE;
}

/*
 * Whether a person's motion blend (fn_8018B558) is still running. With
 * `wait`, yield until it completes (logging and giving up if the model's
 * motion loops) and return FALSE.
 */
u8 peopleWaitSyncMotionBlend(u32 groupId, u32 index, u8 wait)
{
    PeopleEntry* entry;
    void* model;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return FALSE;
    }
    for (;;) {
        if (1.0f == entry->syncMotion) {
            return FALSE;
        }
        if (!wait) {
            break;
        }
        if (*(s32*)((u8*)model + 0x8C) == 1) {
            GSlogWrite("[%s] people[%d,%d] ループモーションがおわるまでまとうとしました\n",
                       __FUNCTION__, groupId, index);
            return FALSE;
        }
        _threadSwitch();
    }
    return TRUE;
}

/*
 * Start a person walking to `target` (state 1) at walking speed, turning
 * toward it; without `keepFacing` the turn is dropped again.
 */
void fn_8018AACC(u32 groupId, u32 index, u8 keepFacing, GSvec* target)
{
    PeopleEntry* original;
    GSvec delta;
    PeopleEntry* entry;
    f32 oldSpeed;
    f32 angle;

    original = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (original == NULL) {
        return;
    }
    original->state = 1;
    GSvecCopy(original->field_5C, target);
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleSetFlags(entry, 8);
    }
    original->moveSpeed = 1.0f;
    original->pad97 = 0;
    GSvecSub(&delta, original->field_5C, peopleGetPositionPtr(original));
    angle = (f32)atan2(delta.x, delta.z);
    oldSpeed = original->moveSpeed;
    peopleStartTurnInline(groupId, index, angle, oldSpeed);
    if (keepFacing == 0) {
        original->pad22 = 0;
    }
}

/*
 * Walk a person up to `distance` tenths of a unit short of person
 * (targetGroupId, targetIndex); if it is already that close, stop it.
 */
void std177_characterMoveToCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex, u8 keepFacing, f32 distance)
{
    PeopleEntry* entry;
    GSvec delta;
    GSvec targetPosition;
    GSvec position;
    f32 length;
    f32 scale;

    distance = 10.0f * distance;
    peopleGetPositionByID(groupId, index, &position);
    peopleGetPositionByID(targetGroupId, targetIndex, &targetPosition);
    GSvecSub(&delta, &position, &targetPosition);
    length = vecMag(&delta);
    if (0.0f == length) {
        scale = 1.0f;
    } else {
        scale = distance / length;
    }
    if (scale >= 1.0f) {
        entry = peopleFindBySelf(peopleFindSelf(groupId, index));
        if (entry != NULL) {
            entry->state = 0;
            entry->pad22 = 0;
        }
    } else {
        vecScale(&delta, &delta, scale);
        GSvecAdd(&targetPosition, &targetPosition, &delta);
        fn_8018AACC(groupId, index, keepFacing, &targetPosition);
    }
}

void std068_characterTurnToAngle(u32 groupId, u32 index, f32 amount) {
    PeopleEntry* entry;
    f32 angle;
    f32 oldSpeed;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    oldSpeed = entry->moveSpeed;
    angle = 0.017453292f * amount;

    peopleStartTurnInline(groupId, index, angle, oldSpeed);
}

/* Find an entry by (groupId, index), then report whether its current movement
 * has completed. If waitFlag is set, yield until it reaches a terminal state. */
BOOL peopleMoveCheck(u32 groupId, u32 index, u8 waitFlag)
{
    PeopleEntry* entry;
    u8 isVisible;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));

    if (entry == NULL) {
        return FALSE;
    }
    for (;;) {
        if (entry->state == 0 && entry->pad22 == 0) {
            return FALSE;
        }
        if (entry->visible != 0) {
            isVisible = TRUE;
        } else if (fn_800F7108(entry->flagId) == NULL) {
            isVisible = TRUE;
        } else {
            isVisible = FALSE;
        }
        if (!isVisible && entry->pad22 == 0) {
            GSlogWrite("[%s]:  移動が終らないので強制的に終了しました\n", __FUNCTION__);
            return FALSE;
        }
        if (waitFlag) {
            _threadSwitch();
            continue;
        }
        return TRUE;
    }
}

/*
 * Open a field message for a person: face the player character (flag 0x20,
 * saving the facing) or look at and follow it (flag 0x10, saving the look
 * target and making the person talkable), then set the speaker's name and
 * open message `messageId`.
 */
void std054_characterTalk(u32 groupId, u32 index, s32 messageId)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    u8* character;
    u32 flags;
    u32 nameMessageId;
    u32 nameId;
    u16 hearFlag;
    u32 messageValue;

    messageValue = 0;
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        flags = peopleGetFlagsByID(groupId, index);
        if (flags & 0x20) {
            if (!entry->field_94) {
                entry->field_98 = entry->field_40;
                entry->field_94 = 1;
            }
            peopleTurnToModel(groupId, index, 0, 100, 1.0f);
            peopleRotateCheck(groupId, index, TRUE);
        } else if (flags & 0x10) {
            if (!entry->field_94) {
                entry->nextLink = entry->threadHandle;
                entry->field_94 = 1;
            }
            std065_characterLookAtPlayer(groupId, index);
            entry->isTalkable = peopleSetTalkable(groupId, index, TRUE);
            peopleIsHeadTurning(groupId, index, TRUE);
        }
        info = peopleInfoBiosGetPtr(entry->scriptRef);
        if (info != NULL) {
            messageValue = characterModelGetTalkSE(info);
        }
        nameMessageId = 0xFA3;
        character = fn_801170A4(entry->groupId, entry->index);
        if (character != NULL) {
            nameId = floorCharacterBiosGetNameID(character);
            hearFlag = charNameBiosGetHearFlag(nameId);
            if (hearFlag == 0 || std081_flagCheckTrue(hearFlag) == 1) {
                nameMessageId = charNameBiosGetNameID(nameId);
            }
        }
        msgctrlSetValue(0x59, nameMessageId);
    }
    winMsgOpenFieldWithSE(messageId, 1, 0, messageValue);
}

/*
 * Close a person's field message and undo what fn_80189990 set up: turn back
 * to the saved facing (flags 0x20 with 0x40) or restore the look target and
 * talkable state (flag 0x10); then wait for the window to close.
 */
void std055_characterEndSpeech(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    u32 flags;

    winMsgClose(0);
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        flags = peopleGetFlagsByID(groupId, index);
        if (entry->field_94) {
            entry->field_94 = 0;
            if (flags & 0x20) {
                if (flags & 0x40) {
                    peopleStartTurnInline(groupId, index, entry->field_98, 1.0f);
                }
            } else if (flags & 0x10) {
                entry->threadHandle = entry->nextLink;
                peopleSetTalkable(groupId, index, entry->isTalkable);
            }
        }
    }
    while (winMsgCheckField() != -1) {
        _threadSwitch();
    }
}

/* Set or clear a person's talkable flag; returns its previous state. */
u8 fn_80189328(u32 groupId, u32 index, u8 enable)
{
    PeopleEntry* entry;
    u8 wasTalkable;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    wasTalkable = peopleTestFlags(entry, PEOPLE_FLAG_TALKABLE);
    if (enable) {
        peopleSetFlags(entry, PEOPLE_FLAG_TALKABLE);
    } else {
        peopleClearFlags(entry, PEOPLE_FLAG_TALKABLE);
    }
    return wasTalkable;
}

/* Make a person walk after another person, looking at it. */
void std181_characterTurnHeadTowardsCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex)
{
    PeopleEntry* entry;
    PeopleEntry* target;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    target = peopleFindBySelf(peopleFindSelf(targetGroupId, targetIndex));
    if (target == NULL) {
        return;
    }
    peopleSetLookTarget(groupId, index, peopleGetPositionPtr(target));
    entry->moveType = PEOPLE_MOVE_WALK_PATH;
    entry->walkPathId = targetGroupId;
    entry->walkPathParam = targetIndex;
}

/* Make a person follow person (0, 100). */
void std065_characterLookAtPlayer(u32 groupId, u32 index)
{
    std181_characterTurnHeadTowardsCharacter(groupId, index, 0, 100);
}

/* Make a person look at the point (x, y, z), kept in targetX..targetZ. */
void std066_characterLookAtPosition(u32 groupId, u32 index, s32 x, s32 y, s32 z)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    set__5GSvecFfff(&entry->targetX, x, y, z);
    peopleSetLookTarget(groupId, index, &entry->targetX);
    entry->moveType = PEOPLE_MOVE_WALK_POSITION;
}

/* Stop a person looking at anything: reset its head part and look target. */
void std067_characterStopLookAt(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    void* model;
    void* part;
    s8 partIndex;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return;
    }
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info == NULL) {
        return;
    }
    partIndex = charModelGetHeadPart(info);
    if (partIndex < 0) {
        return;
    }
    part = GSmodelGetPart(model, partIndex);
    fn_800EE288(part);
    GSpartFree(part);
    entry->threadHandle = NULL;
    set__5GSvecFfff(entry->headTarget, 0.0f, 0.0f, 0.0f);
    entry->moveType = PEOPLE_MOVE_NONE;
}

/*
 * Report whether a person's head is still turning toward its target. With
 * `wait`, yield until the turn has finished and return FALSE.
 */
BOOL std155_characterWaitForLookAt(u32 groupId, u32 index, u8 wait)
{
    return peopleIsHeadTurning(groupId, index, wait);
}

/* fn_801887D8 -- find a people entry by (groupId, index) and compute an
 * animation blend ratio against entry->field_34/field_38 (float time range)
 * for a caller-supplied time-source object (param3, fed to fn_800E008C). */
f32 peopleAmountFromMove(u32 groupId, u32 index, void* param3) {
    PeopleEntry* entry;
    f32 result;
    f32 t;
    f32 endTime;
    f32 startTime;

    result = 0.0f;
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0.0f;
    }
    t = vecMag(param3);
    endTime = entry->field_38;
    if (endTime <= t) {
        result = 2.0f;
    } else {
        startTime = entry->field_34;
        if (startTime <= t) {
            if (0.0f != endTime) {
                result = 1.0f + (t - startTime) / (endTime - startTime);
            }
        } else if (0.0f != startTime) {
            result = t / startTime;
        }
    }
    return result;
}

/*
 * Move a person by `offset` (XZ) through the collision checks and, with
 * `face`, face the direction of the offset.
 */
void peopleMoveByVector(u32 groupId, u32 index, GSvec* offset, u8 face)
{
    PeopleEntry* entry;
    GSvec position;
    GSvec origin = {0.0f, 0.0f, 0.0f};
    f32 angle;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    fn_8018FC98(entry, &position);
    position.x += offset->x;
    position.z += offset->z;
    peopleMoveCommit(entry, &position, peopleGetTransform(entry));
    if (face) {
        angle = peopleTurnTo(offset, &origin, 0.0f);
        entry->pad22 = 1;
        entry->field_40 = angle;
        entry->field_44 = 1.0f;
    }
}

/*
 * Step a person along `yaw` by one frame of walking at `speed` (0..1 walks,
 * above 1 runs; scaled by 1.2 at 50 Hz), placing it through the collision
 * checks. Returns FALSE when the person or its model is missing.
 *
 * RULE-EXCEPTION(title-path): single-use static inline helper backed only
 * by register colouring - see docs/RULE_EXCEPTIONS.md
 * Pokemon XD has this split: fn_80188214 is XD's peopleMoveForward
 * (0x8029C6C4; NXXJ01.map, StarsMmd/Colo-XD-PBR-symbol-maps 6b51d3af, lists
 * it next to peopleMoveAlongAngle), which finds the person and calls
 * peopleMoveAlongAngle (0x8029C194) with its facing. But XD's
 * peopleMoveAlongAngle makes different calls (GSgfxVideoGetVsyncRate,
 * timeGetLastFrameTime, sin/cos, GScolsys2HumanGetWalkHeight,
 * gimmickBoxOnBox, ...; trevor403/xd-asm b1087f18), so the sister-title
 * clause is not met; nothing else in Colosseum repeats the block
 * (tools/find_inline_expansions.py, best 0.585 in fn_80186B5C); and the
 * expansion leaves no written fingerprint. Its only evidence is the
 * colouring: retail gives the second lookup's entry r29 and the model r28,
 * which MWCC does only when `model` is an inline's local (coloured before
 * the lookup's return temporary). Written in place (the body below pasted
 * into fn_80188214, locals as function locals), `model` is coloured after
 * every inline temporary and takes r29 (99.72%, lanes B30r/P30r; no
 * declaration order, early-return form or C++ reference binding changes
 * that). A clean fix needs evidence for this helper: a second expansion or
 * an inline fingerprint. XD's own argument order (speed, yaw) leaves an
 * extra `fmr` of speed that retail does not have.
 */
static inline u8 peopleMoveAlongAngle(u32 groupId, u32 index, f32 yaw, f32 speed)
{
    PeopleEntry* entry;
    void* model;
    f32 ticks;
    f32 frameStart;
    f32 frameEnd;
    GSvec position;
    GSvec localStep;
    GSvec worldStep;
    u8 rotation[16];

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    model = peopleGetModel(entry);
    if (model == NULL) {
        return FALSE;
    }

    fn_8018FC98(entry, &position);
    GSmodelGetFrameCount(model, &frameStart, &frameEnd);
    if (fn_800D37CC() == 50) {
        speed *= 1.2f;
    }

    if (speed <= 0.4f) {
        speed = speed / 0.4f * (entry->field_34 / 2.0f);
    } else if (speed > 0.4f && speed <= 1.0f) {
        speed = (speed - 0.4f) / 0.6f * (entry->field_34 - entry->field_34 / 2.0f) +
                entry->field_34 / 2.0f;
    } else {
        speed = (speed - 1.0f) * (entry->field_38 - entry->field_34) + entry->field_34;
    }

    fn_800E0718(rotation, &lbl_8031554C, yaw);
    set__5GSvecFfff(&localStep, 0.0f, 0.0f, speed);
    GSvecTransformQuat(&worldStep, rotation, &localStep);
    ticks = (f32)GSgetFrameStep();
    position.x += worldStep.x * ticks;
    position.z += worldStep.z * ticks;
    return peopleMoveCommit(entry, &position, (GSvec*)entry->transform);
}

/* Step a person along its own facing (XD's peopleMoveForward). */
u8 peopleSetSpeedFromAmount(u32 groupId, u32 index, f32 speed)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    /* RULE-EXCEPTION(title-path): single-use static inline helper - see docs/RULE_EXCEPTIONS.md */
    return peopleMoveAlongAngle(groupId, index, entry->field_40, speed);
}

/* Start turning a person toward `yaw`, expressed in its model's current revolution. */
void peopleStartTurn(u32 groupId, u32 index, f32 yaw, f32 speed)
{
    PeopleEntry* entry;
    GSvec rotation;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleGetRotation(entry, &rotation);
        yaw += 6.2831855f * (s32)(rotation.y / 6.2831855f);
        entry->pad22 = 1;
        entry->field_40 = yaw;
        entry->field_44 = speed;
    }
}

/* Turn a person toward the point (x, y, z). */
void peopleTurnToPoint(u32 groupId, u32 index, f32 x, f32 y, f32 z, f32 speed)
{
    PeopleEntry* entry;
    GSvec delta;
    GSvec targetPosition;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    set__5GSvecFfff(&targetPosition, x, y, z);
    GSvecSub(&delta, &targetPosition, peopleGetPositionPtr(entry));
    peopleStartTurnInline(groupId, index, atan2f(delta.x, delta.z), speed);
}

/* Turn a person toward the model of resource (targetGroupId, targetIndex). */
void std165_characterFaceCharacter(u32 groupId, u32 index, u32 targetGroupId, u32 targetIndex, f32 speed)
{
    PeopleEntry* entry;
    void* model;
    GSvec delta;
    GSvec targetPosition;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = GSresGetResource(targetGroupId, targetIndex);
    if (model == NULL) {
        return;
    }
    GSvecCopy(&targetPosition, GSmodelGetPositionPtr(model));
    GSvecSub(&delta, &targetPosition, peopleGetPositionPtr(entry));
    peopleStartTurnInline(groupId, index, atan2f(delta.x, delta.z), speed);
}

/* Stop a person turning, keeping its current model yaw as its facing. */
void peopleStopTurn(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    GSvec* position;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        entry->pad22 = 0;
        position = peopleGetPosition(entry);
        entry->field_40 = position->y;
    }
}

/*
 * Whether a person stands inside the XZ rectangle spanned by (x0, z0) and
 * (x1, z1). With the debug display on (resource (0, 2)), the rectangle is
 * drawn as a 50-unit-high fence around the person.
 */
u8 std058_characterCheckWithinBounds(u32 groupId, u32 index, f32 x0, f32 z0, f32 x1, f32 z1)
{
    PeopleEntry* entry;
    u8* debug;
    GSvec position;
    f32 minX;
    f32 maxX;
    f32 minZ;
    f32 maxZ;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    GSvecCopy(&position, peopleGetPositionPtr(entry));

    debug = GSresGetResource(0, 2);
    if (debug != NULL && debug[0] != 0) {
        cameraGetActive();
        fn_800D258C();
        _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
        gfxSetCullMode(0);
        gfxSetVtxDesc(lbl_80314638);
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetBlendMode(1, 6, 7);
        fn_800D9ED8(0);
        gfxSetPrimitive(4);
        gfxBegin(10);
        gfxVtxPos(x0, position.y, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x0, position.y + 50.0f, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x0, position.y, z1);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x0, position.y + 50.0f, z1);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x1, position.y, z1);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x1, position.y + 50.0f, z1);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x1, position.y, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x1, position.y + 50.0f, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x0, position.y, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x0, position.y + 50.0f, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxEnd();
    }

    if (x0 < x1) {
        minX = x0;
        maxX = x1;
    } else {
        minX = x1;
        maxX = x0;
    }
    if (z0 < z1) {
        minZ = z0;
        maxZ = z1;
    } else {
        minZ = z1;
        maxZ = z0;
    }
    if (minX <= position.x && position.x <= maxX && minZ <= position.z && position.z <= maxZ) {
        return TRUE;
    }
    return FALSE;
}

/*
 * Whether `point` touches, within `reach`, the wall quad standing on the
 * segment start..end from 50 below to 50 above `position`'s height (tested
 * as four triangles). On a hit `position` receives the contact point.
 */
u8 fn_801870E8(GSvec* position, GSvec* point, GSvec* start, GSvec* end, void* normal, f32 reach)
{
    GSvec verts[3];

    verts[0].x = start->x;
    verts[0].y = position->y - 50.0f;
    verts[0].z = start->z;
    verts[1].x = end->x;
    verts[1].y = position->y - 50.0f;
    verts[1].z = end->z;
    verts[2].x = start->x;
    verts[2].y = position->y + 50.0f;
    verts[2].z = start->z;
    if (peopleTouchTriangle(position, normal, verts, point, reach)) {
        return TRUE;
    }

    verts[0].x = end->x;
    verts[0].y = position->y - 50.0f;
    verts[0].z = end->z;
    verts[1].x = end->x;
    verts[1].y = position->y + 50.0f;
    verts[1].z = end->z;
    if (peopleTouchTriangle(position, normal, verts, point, reach)) {
        return TRUE;
    }

    verts[0].x = end->x;
    verts[0].y = position->y - 50.0f;
    verts[0].z = end->z;
    verts[1].x = start->x;
    verts[1].y = position->y - 50.0f;
    verts[1].z = start->z;
    verts[2].x = start->x;
    verts[2].y = position->y + 50.0f;
    verts[2].z = start->z;
    if (peopleTouchTriangle(position, normal, verts, point, reach)) {
        return TRUE;
    }

    verts[1].x = start->x;
    verts[1].y = position->y + 50.0f;
    verts[1].z = start->z;
    verts[2].x = end->x;
    verts[2].y = position->y + 50.0f;
    verts[2].z = end->z;
    if (peopleTouchTriangle(position, normal, verts, point, reach)) {
        return TRUE;
    }
    return FALSE;
}

/*
 * One frame of pad-driven movement for a person: the control stick (or the
 * D-pad) picks a speed on the walk/run curve, and the direction comes from
 * the stick relative to the camera (or the current facing when the stick
 * is barely tilted). Returns the frame's displacement.
 *
 * The step length (the walk/run ramp over field_34/field_38, the same
 * sequence fn_80188214 computes) is written in place: retail keeps it in
 * f30, the register `heading` had, which MWCC gives a function local
 * (coloured last) but not an inline helper's result temporary (coloured
 * first, so it took tilt's f31). A static inline shared with fn_80188214
 * therefore did not reproduce this function.
 */
GSvec fn_80186B5C(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    GSvec result = {0.0f, 0.0f, 0.0f};
    GSvec localStep;
    GSvec worldStep;
    u8 rotation[16];
    s8 stickX;
    s8 stickY;
    s8 subStickX;
    s8 subStickY;
    f32 x;
    f32 z;
    f32 tilt;
    f32 angle;
    f32 direction;
    f32 heading;
    f32 speed;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return result;
    }
    stickX = fn_800F7A7C(1, 1);
    stickY = fn_800F7A08(1, 1);
    subStickX = fn_800F7A7C(1, 0);
    subStickY = fn_800F7A08(1, 0);
    if (stickX == 0 && stickY == 0) {
        if (fn_800F7BC4(1) & 8) {
            stickY = -56;
        }
        if (fn_800F7BC4(1) & 4) {
            stickY = 56;
        }
        if (fn_800F7BC4(1) & 1) {
            stickX = -56;
        }
        if (fn_800F7BC4(1) & 2) {
            stickX = 56;
        }
        subStickX = stickX;
        subStickY = stickY;
    }
    if (stickX != 0 || stickY != 0) {
        if (stickX > 56) {
            stickX = 56;
        } else if (stickX < -56) {
            stickX = -56;
        }
        if (stickY > 56) {
            stickY = 56;
        } else if (stickY < -56) {
            stickY = -56;
        }
        x = (stickX > 0 ? stickX : -stickX) / 28.0f;
        z = (stickY > 0 ? stickY : -stickY) / 28.0f;
        tilt = sqrtf(x * x + z * z);
        if (tilt > 2.0f) {
            tilt = 2.0f;
        }
        if (subStickX <= -2 || subStickX >= 2 || subStickY <= -2 || subStickY >= 2) {
            if (z < 0.001f) {
                direction = 1.5707964f;
            } else {
                direction = x / z;
                if (direction > 5.0f) {
                    direction = 5.0f;
                }
                direction = 1.5707964f * sinf(direction / 3.1830988f);
            }
            if (stickY >= 0) {
                heading = direction;
            } else {
                heading = 3.1415927f - direction;
            }
            if (stickX < 0) {
                if (stickY >= 0) {
                    heading = 3.1415927f + (3.1415927f - direction);
                } else {
                    heading = 3.1415927f + direction;
                }
            }
            angle = heading + cameraGetRotY();
        } else {
            angle = entry->field_40;
        }
        if (tilt <= 0.4f) {
            speed = tilt / 0.4f * (entry->field_34 / 2.0f);
        } else if (tilt > 0.4f && tilt <= 1.0f) {
            speed = (tilt - 0.4f) / 0.6f * (entry->field_34 - entry->field_34 / 2.0f) +
                    entry->field_34 / 2.0f;
        } else {
            speed = (tilt - 1.0f) * (entry->field_38 - entry->field_34) + entry->field_34;
        }
        fn_800E0718(rotation, &lbl_8031554C, angle);
        set__5GSvecFfff(&localStep, 0.0f, 0.0f, speed);
        GSvecTransformQuat(&worldStep, rotation, &localStep);
        vecScale(&result, &worldStep, (f32)GSgetFrameStep());
    }
    return result;
}

/*
 * Whether a person's movement this frame runs into the wall standing on the
 * segment (x0, z0)-(x1, z1), sweeping the step in radius-sized slices. The
 * pad-controlled mover (flag 0x40000000) is tested with its pad step. With
 * `push`, the person is stopped at the wall (pushed out of any collision)
 * and the hero's movement is locked for a frame.
 */
u8 std170_cameraFocusOnCharacter(u32 groupId, u32 index, u8 push, f32 x0, f32 z0, f32 x1, f32 z1)
{
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    u8* debug;
    GSvec testPoint;
    GSvec lineStart;
    GSvec lineEnd;
    GSvec transform;
    GSvec position;
    GSvec direction;
    GSvec scaledDirection;
    GSvec hit;
    GSvec correction;
    GSvec contact;
    GSvec projectedStart;
    GSvec projectedDelta;
    GSvec lineDirection;
    GSvec offset;
    f32 radius;
    f32 reach;
    f32 length;
    f32 step;
    f32 t;
    f32 next;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    if (peopleTestFlags(entry, 0x40000000)) {
        GSvecCopy(&direction, &entry->collisionX);
        vecScale(&scaledDirection, &direction, 0.1f);
        direction = fn_80186B5C(0, 100);
        GSvecCopy(&transform, peopleGetTransform(entry));
        GSvecAdd(&position, &transform, &direction);
    } else {
        GSvecCopy(&position, peopleGetPositionPtr(entry));
        GSvecCopy(&transform, peopleGetTransform(entry));
        GSvecSub(&direction, &position, &transform);
    }
    set__5GSvecFfff(&lineStart, x0, position.y, z0);
    set__5GSvecFfff(&lineEnd, x1, position.y, z1);

    debug = GSresGetResource(0, 2);
    if (debug != NULL && debug[0] != 0) {
        cameraGetActive();
        fn_800D258C();
        _cameraLoadCameraMatrix__FP9_GScamera12GSgfxLayerID();
        gfxSetCullMode(0);
        gfxSetVtxDesc(lbl_80314638);
        gfxSetFlags(1);
        gfxClearFlags(6);
        gfxSetBlendMode(1, 6, 7);
        fn_800D9ED8(0);
        gfxSetPrimitive(4);
        gfxBegin(4);
        gfxVtxPos(x0, position.y - 50.0f, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x0, position.y + 50.0f, z0);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x1, position.y - 50.0f, z1);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxVtxPos(x1, position.y + 50.0f, z1);
        fn_800D5CB8(0, 0, 0x80, 0xFF, 0xC0);
        gfxEnd();
    }

    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info != NULL) {
        radius = fn_8018F5E4(info);
    } else {
        radius = 4.0f;
    }
    reach = radius;
    length = vecMag(&direction);
    if (length > 0.0f) {
        step = radius / length;
        if (step > 1.0f) {
            step = 1.0f;
        }
    } else {
        step = 1.0f;
    }

    GSvecSub(&lineDirection, &lineEnd, &lineStart);
    if (vecMag(&lineDirection) < 0.0001f) {
        return FALSE;
    }
    fn_800E0060(&lineDirection, &lineDirection);
    GSvecSub(&offset, &transform, &lineStart);
    vecScale(&offset, &lineDirection, fn_800E0000(&offset, &lineDirection));
    GSvecAdd(&projectedStart, &offset, &lineStart);
    GSvecSub(&projectedDelta, &transform, &projectedStart);

    for (t = 0.0f; t < 1.0f; t += step) {
        next = t + step;
        if (next > 1.0f) {
            next = 1.0f;
        }
        vecScale(&testPoint, &direction, next);
        GSvecAdd(&testPoint, &transform, &testPoint);
        if (fn_801870E8(&projectedStart, &testPoint, &lineStart, &lineEnd, &projectedDelta, reach)) {
            if (push) {
                GScolsy2UtilGetPointExtentionLine(&contact, &projectedStart, &testPoint, reach + 0.0001f);
                if (colsysWallSweep(&transform, &contact, &hit, 4.0f)) {
                    GSvecSub(&correction, &hit, &contact);
                    GSvecAdd(&contact, &contact, &correction);
                }
                fn_8018FC74(entry, &contact);
                heroMoveSetLockFrame(1);
            }
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * Whether person (targetGroupId, targetIndex) is within `range` tenths of
 * a unit of a person, inside its field of view (`fov` degrees wide) and not
 * behind a wall.
 */
u8 fn_80186284(u32 groupId, u32 index, f32 range, u32 targetGroupId, u32 targetIndex, f32 fov)
{
    PeopleEntry* target;
    PeopleEntry* entry;
    PeopleInfoBiosEntry* info;
    GSvec position;
    GSvec targetPosition;
    GSvec rotation;
    GSvec delta;
    f32 angle;
    f32 radius = 4.0f;

    target = peopleFindBySelf(peopleFindSelf(targetGroupId, targetIndex));
    if (target == NULL) {
        return FALSE;
    }
    GSvecCopy(&targetPosition, peopleGetPositionPtr(target));
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    GSvecCopy(&position, peopleGetPositionPtr(entry));
    GSvecSub(&delta, &position, &targetPosition);
    range = 10.0f * range;
    if (vecMag(&delta) > range) {
        return FALSE;
    }
    peopleGetRotation(entry, &rotation);
    angle = peopleTurnTo(&targetPosition, &position, rotation.y);
    fov = 0.5f * (0.017453292f * fov);
    if (fabs(angle) > fov) {
        return FALSE;
    }
    info = peopleInfoBiosGetPtr(entry->scriptRef);
    if (info != NULL) {
        radius = fn_8018F5E4(info);
    }
    if (fn_8010F188(&position, &targetPosition, NULL, radius)) {
        return FALSE;
    }
    return TRUE;
}

/* Whether the player character (0, 100) is within 3 units and a 30-degree view of a person. */
u8 peopleGazeHeroCheck(u32 groupId, u32 index)
{
    return fn_80186284(groupId, index, 3.0f, 0, 100, 30.0f);
}

/* Place a person at (x, y, z). */
void std059_characterSetPosition(u32 groupId, u32 index, f32 x, f32 y, f32 z)
{
    GSvec position;

    set__5GSvecFfff(&position, x, y, z);
    peoplePlaceAt(groupId, index, &position);
}

void peopleSetRotationDeg(u32 groupId, u32 index, f32 x, f32 y, f32 z) {
    PeopleEntry* entry;
    GSvec rotation;
    f32 radians = 0.017453292f;

    rotation.x = radians * x;
    rotation.y = radians * y;
    rotation.z = radians * z;
    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        rotation.x = (f32)fmod(6.283185307179586 + rotation.x, 6.283185307179586);
        rotation.y = (f32)fmod(6.283185307179586 + rotation.y, 6.283185307179586);
        rotation.z = (f32)fmod(6.283185307179586 + rotation.z, 6.283185307179586);
        fn_8018FC08(entry, &rotation);
        entry->field_40 = rotation.y;
    }
}

/* Walk a person to (x, y, z) (fn_8018AACC). */
void std061_characterMoveToPosition(u32 groupId, u32 index, u8 keepFacing, f32 x, f32 y, f32 z)
{
    GSvec target;

    set__5GSvecFfff(&target, x, y, z);
    fn_8018AACC(groupId, index, keepFacing, &target);
}

/*
 * Per-frame motion upkeep: advance a pending motion blend, then (for people
 * that walk, flag 8) pick the idle/walk/run slot for `speed` (below 0.4,
 * below 1.5, above) and play it when it changes.
 */
void fn_80185B90(PeopleEntry* entry, f32 speed)
{
    void* model;
    s32 motion;

    model = peopleGetModel(entry);
    if (model == NULL) {
        return;
    }
    if (0.0f != entry->walkAnimRate) {
        entry->syncMotion += entry->walkAnimRate * (f32)GSgetFrameStep();
        if (entry->syncMotion > 1.0f) {
            entry->syncMotion = 1.0f;
            entry->walkAnimRate = 0.0f;
        }
        GSmodelSetBlendFactor(model, entry->syncMotion);
    }
    if (!peopleTestFlags(entry, 8)) {
        return;
    }
    if (speed < 0.4f) {
        motion = 1;
    } else if (speed >= 0.4f && speed < 1.5f) {
        motion = 2;
    } else {
        motion = 3;
    }
    if (entry->motionIndex == motion) {
        return;
    }
    peopleSetMotionIndex(entry, motion);
}

/*
 * Step a person toward field_5C. Returns 2 if the step failed, 1 if it
 * overshot (the person is then placed on the target), 0 otherwise.
 */
s32 fn_80185AAC(PeopleEntry* entry)
{
    GSvec delta;
    f32 oldLength;

    GSvecSub(&delta, entry->field_5C, peopleGetPositionPtr(entry));
    oldLength = vecMag(&delta);
    if (!peopleSetSpeedFromAmount(entry->groupId, entry->index, entry->moveSpeed)) {
        return 2;
    }
    GSvecSub(&delta, entry->field_5C, peopleGetPositionPtr(entry));
    if (vecMag(&delta) > oldLength) {
        fn_8018FC74(entry, entry->field_5C);
        peopleMoveCommit(entry, peopleGetPositionPtr(entry), peopleGetTransform(entry));
        return 1;
    }
    return 0;
}

/*
 * Walk toward field_5C: stop once a step reports arrival, and once the model
 * has not moved for 60 frames, snap it to the target and stop.
 */
void fn_801858C4(PeopleEntry* entry)
{
    void* position;
    void* transform;
    GSvec delta;

    if (fn_80185AAC(entry) != 0) {
        entry->state = 0;
    }
    if (entry == NULL) {
        return;
    }
    position = peopleGetPositionPtr(entry);
    transform = peopleGetTransform(entry);
    GSvecSub(&delta, position, transform);
    if (__fabs(delta.x) < 0.0001f && __fabs(delta.y) < 0.0001f &&
        __fabs(delta.z) < 0.0001f) {
        entry->pad97++;
        if (entry->pad97 > 60) {
            peoplePlaceAt(entry->groupId, entry->index, entry->field_5C);
            entry->state = 0;
            entry->pad97 = 0;
        }
    } else {
        entry->pad97 = 0;
    }
}

/*
 * Walk a person along its walk list (states 2 and 3; state 3 loops): face
 * the next point, then each frame step forward, passing as many points as
 * the step covers. Past the last point a looping walk starts over, and
 * otherwise the person is placed on it and stops. The per-frame update
 * passes whether the walk loops, but the loop test reads the state itself.
 *
 * Exact since peopleTurnTo computes the heading in its own statement (lane
 * B30x, 2026-09-28; see peopleTurnTo): that changes the vreg numbering of
 * the loop's expansion and gives retail's colouring. The analysis below
 * (lane B30r) is kept for the record.
 * Former wall (99.34%, lane B30r, 2026-09-28): FPR colouring in the walk
 * loop. Retail puts remaining in f31, the hoisted pi, 0.0f and 2pi of the
 * loop's peopleTurnTo in f30/f29/f28 and distance in f27; ours colours
 * 0.0f, distance, remaining, pi, 2pi in that order (f31..f27). The
 * instructions are otherwise identical, so the interference graph is the
 * same and only MWCC's vreg numbering differs. Replaying the dumped graph
 * (GC/2.6 replay faithful; simplify K=32, lowest-number scan) reaches
 * retail only when remaining is numbered below every other value and the
 * hoisted constants are numbered 2pi <= 0.0f <= pi; no single renumbering
 * or interference edge does it. Declaration orders, a while loop, moving
 * `remaining -= distance`, passing peopleTurnTo straight to
 * peopleStartTurn, and the tools/local_campaign.py rewrite search (114
 * forms) all fail. Pokemon XD's _peopleMoveTypeList__FP13tagPeopleWorkb
 * (0x8029A0C8) was rewritten (distanceBetween, adjustPIPI,
 * peopleRotateToAngle, peopleMoveAlongAngle), so it gives no helper.
 */
void fn_8018524C(PeopleEntry* entry, u8 loop)
{
    GSvec delta;
    f32 remaining;
    f32 distance;
    f32 angle;

    switch (entry->subState) {
    case 0:
        break;
    case 1:
        GSvecCopy(entry->field_5C, &entry->walkList[entry->walkListCount]);
        angle = peopleTurnTo(entry->field_5C, peopleGetPositionPtr(entry), 0.0f);
        peopleStartTurnInline(entry->groupId, entry->index, angle, 1.0f);
        entry->subState = 2;
        /* fallthrough */
    case 2:
        peopleSetSpeedFromAmount(entry->groupId, entry->index, entry->moveSpeed);
        GSvecSub(&delta, entry->transform, peopleGetPositionPtr(entry));
        remaining = vecMag(&delta);
        for (;;) {
            GSvecSub(&delta, entry->transform, entry->field_5C);
            distance = vecMag(&delta);
            if (!(distance < remaining)) {
                break;
            }
            GSvecCopy(entry->transform, entry->field_5C);
            entry->walkListCount++;
            if (entry->walkListCount >= entry->walkListCapacity) {
                if (entry->state == 3) {
                    entry->walkListCount = 0;
                } else {
                    peoplePlaceAt(entry->groupId, entry->index, entry->field_5C);
                    entry->state = 0;
                    return;
                }
            }
            GSvecCopy(entry->field_5C, &entry->walkList[entry->walkListCount]);
            angle = peopleTurnTo(entry->field_5C, entry->transform, 0.0f);
            peopleStartTurnInline(entry->groupId, entry->index, angle, 2.0f);
            remaining -= distance;
        }
        GSvecSub(&delta, entry->field_5C, entry->transform);
        vecScale(&delta, &delta, remaining / distance);
        GSvecAdd(&delta, entry->transform, &delta);
        peopleMoveCommit(entry, &delta, (GSvec*)entry->transform);
        break;
    }
}

/*
 * Wander around a point (state 4): wait out the pause, pick a random point
 * on the circle of radius field_80 around collisionX and turn toward it,
 * then walk there. A blocked step turns the person around; arriving starts
 * a new random pause (field_88 + field_8C * random).
 */
void fn_80184D80(PeopleEntry* entry)
{
    GSvec offset = {0.0f, 0.0f, 0.0f};
    f32 frameRate;
    f32 angle;
    s32 result;

    switch (entry->subState) {
    case 0:
        if (entry->animBlendFactor > 0.0f) {
            frameRate = (f32)fn_800D37CC();
            entry->animBlendFactor -= (f32)GSgetFrameStep() / frameRate;
            if (entry->animBlendFactor < 0.0f) {
                entry->animBlendFactor = 0.0f;
            }
            break;
        }
        entry->subState = 1;
        /* fallthrough */
    case 1:
        angle = 3.1415927f * (2.0f * fn_800E0BE4());
        offset.x = entry->field_80 * sin(angle);
        offset.z = entry->field_80 * cos(angle);
        GSvecAdd(entry->field_5C, &entry->collisionX, &offset);
        angle = peopleTurnTo(entry->field_5C, peopleGetPositionPtr(entry), 0.0f);
        peopleStartTurnInline(entry->groupId, entry->index, angle, 1.0f);
        entry->subState = 2;
        /* fallthrough */
    case 2:
        result = fn_80185AAC(entry);
        if (result == 2) {
            peopleStartTurnInline(entry->groupId, entry->index, 3.141592653589793 + entry->field_40, 1.0f);
            entry->subState = 2;
        } else if (result == 1) {
            entry->animBlendFactor = entry->field_8C * fn_800E0BA0() + entry->field_88;
            entry->subState = 0;
        }
        break;
    }
}

/*
 * Advance a person's turn toward field_40 (while pad22 is set). A free
 * turn (or one with flag 8) takes the shorter way at field_44 * pi/20 per
 * tick; a person following a point (flag 0x40000000) eases in, never slower
 * than one degree per tick.
 */
void peopleTurnUpdate(PeopleEntry* entry)
{
    GSvec rotation;
    f32 target;
    f32 step;
    f32 difference;
    u32 ticks;
    u8 turning;
    u8 shortest;

    shortest = TRUE;
    turning = entry->pad22;
    if (!turning) {
        return;
    }
    peopleGetRotation(entry, &rotation);
    if (peopleTestFlags(entry, 0x40000000)) {
        shortest = FALSE;
        if (peopleTestFlags(entry, 8)) {
            shortest = TRUE;
        }
    }
    if (shortest) {
        rotation.y = peopleNormalizeYaw(rotation.y);
        target = peopleNormalizeYaw(entry->field_40);
        step = 0.15707964f * entry->field_44;
        for (ticks = GSgetFrameStep(); ticks != 0; ticks--) {
            difference = target - rotation.y;
            if (fabs(difference) >= 3.141592653589793) {
                if (difference < 0.0f) {
                    difference = 6.283185307179586 + difference;
                } else {
                    difference = difference - 6.283185307179586;
                }
            }
            if (fabs(difference) <= step) {
                rotation.y = target;
                turning = FALSE;
                break;
            }
            if (difference > 0.0f) {
                rotation.y += step;
            } else {
                rotation.y -= step;
            }
        }
    } else {
        if (rotation.y - entry->field_40 > 3.2986722f) {
            rotation.y -= 6.2831855f;
        } else if (entry->field_40 - rotation.y > 3.2986722f) {
            rotation.y += 6.2831855f;
        }
        step = (entry->field_40 - rotation.y) / (10.0f * (2.6f - entry->field_44));
        if (fabs(step) <= 0.017453292f) {
            step = step < 0.0f ? -0.017453292f : 0.017453292f;
        }
        rotation.y += step * (f32)GSgetFrameStep();
        if (step < 0.0f) {
            if (rotation.y <= entry->field_40) {
                rotation.y = entry->field_40;
                turning = FALSE;
            }
        } else if (rotation.y >= entry->field_40) {
            rotation.y = entry->field_40;
            turning = FALSE;
        }
    }
    fn_8018FC08(entry, &rotation);
    entry->pad22 = turning;
}

void peopleSetWalkSpeed(u32 groupId, u32 index, f32 speed) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        entry->moveSpeed = speed;
    }
}

/* Attach part `partIndex` of the model of resource (group, id) to `model`. */
void fn_801848D0(void* model, s32 group, s32 id, s32 partIndex)
{
    void* resource;
    void* part;

    resource = GSresGetResource(group, id);
    if (resource != NULL) {
        part = GSmodelGetPart(resource, partIndex);
        GSmodelAttachToGSpart(model, part, 7, 0, 1);
        GSpartFree(part);
    }
}

/*
 * Make a person hold part `partIndex` of the model of resource
 * (group, id), replacing anything it held; the held item is recorded in
 * walk nodes A-C.
 */
void std070_characterAttachToCharacter(u32 groupId, u32 index, s32 group, s32 id, s32 partIndex)
{
    /*
     * RULE-EXCEPTION(title-path): unused initialised local (invented dead
     * code) - see docs/RULE_EXCEPTIONS.md. It is never read, so it emits no
     * instruction; it gives the TU its sixth 12-byte zero .rodata image
     * (0x80273FCC, after fn_80184D80's) and adds the statement that puts
     * this function over GC/2.0's -inline auto size limit, so fn_80181EB0
     * calls it out of line as retail does. Nothing in retail shows the
     * local; a clean fix needs the real owner of the image.
     */
    GSvec offset = {0.0f, 0.0f, 0.0f};
    PeopleEntry* entry;
    void* model;
    s32 current;
    s32 secondary;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = GSresGetResource(groupId, index);
    if (model == NULL) {
        return;
    }
    peopleDetachHeld(groupId, index);
    entry->walkNodeA = group;
    entry->walkNodeB = id;
    entry->walkNodeC = partIndex;
    if (GSmodelIsBlending(model)) {
        GSmodelGetAnimIndex(model, &current, &secondary);
        GSmodelSetAnimIndex(model, secondary);
    }
    peopleAttachPart(model, group, id, partIndex);
}

/* Detach whatever a person holds (walk nodes A-C) from its model. */
void std168_characterDetachFromCharacter(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    void* model;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    model = GSresGetResource(groupId, index);
    if (model != NULL && entry->walkNodeC >= 0) {
        entry->walkNodeA = -1;
        entry->walkNodeB = -1;
        entry->walkNodeC = -1;
        GSmodelDetachFromGSpart(model, 1);
    }
}

void std071_threadYield(void) {
    _threadSwitch();
}

/* Give a person a new, empty walk list with room for `count` points. */
u8 peopleReserveWalkList(u32 groupId, u32 index, u16 count)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    peopleFreeWalkList(groupId, index);
    entry->walkListHandle = _toolentryAlloc__FUl(count * sizeof(GSvec));
    if (entry->walkListHandle == 0) {
        return FALSE;
    }
    entry->walkList = fn_800E27B0(entry->walkListHandle);
    if (entry->walkList == NULL) {
        return FALSE;
    }
    memset(entry->walkList, 0, count * sizeof(GSvec));
    entry->walkListCapacity = count;
    entry->walkListCount = 0;
    entry->subState = 0;
    return TRUE;
}

s32 peopleAddWalkList(u32 groupId, u32 index, f32 x, f32 y, f32 z) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (entry->walkList == NULL) {
        return 0;
    }
    if (entry->walkListCount >= entry->walkListCapacity) {
        GSlogWrite("peopleAddWalkList:  登録リストが多すぎます\n");
        return 0;
    }

    set__5GSvecFfff(&entry->walkList[entry->walkListCount], x, y, z);
    entry->walkListCount++;
    return 1;
}

/* Start a person walking its walk list from the first point (looping or once). */
BOOL peopleStartWalkList(u32 groupId, u32 index, u32 loop)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return FALSE;
    }
    entry->subState = 0;
    entry->walkListCount = 0;
    entry->moveSpeed = 1.0f;
    entry->subState = 1;
    if (loop) {
        entry->state = 3;
    } else {
        entry->state = 2;
    }
    return TRUE;
}

s32 std075_characterAllocPathAlt(u32 groupId, u32 index) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }

    if (entry->walkListHandle != 0) {
        fn_800E24B0(entry->walkListHandle);
        fn_800E209C(entry->walkListHandle);
        entry->walkList = NULL;
        entry->walkListHandle = 0;
        entry->state = 0;
        entry->walkListCapacity = 0;
        entry->walkListCount = 0;
        entry->subState = 0;
    }
    return 1;
}

s32 fn_80183B44(u32 groupId, u32 index, f32 field80) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }

    entry->state = PEOPLE_STATE_INTERACTING;
    entry->subState = 0;
    fn_8018FC98(entry, &entry->collisionX);
    entry->field_80 = field80;
    entry->animBlendFactor = 0.0f;
    entry->field_88 = 5.0f;
    entry->field_8C = 3.0f;
    entry->moveSpeed = 1.0f;
    return 1;
}

s32 fn_801839A0(u32 groupId, u32 index, f32 field88, f32 field8C) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }

    entry->state = PEOPLE_STATE_CUTSCENE;
    entry->subState = 0;
    fn_8018FC98(entry, &entry->collisionX);
    entry->animBlendFactor = 0.0f;
    entry->field_88 = field88;
    entry->field_8C = field8C;
    entry->moveSpeed = 1.0f;
    return 1;
}

/* The talk script of a person's floor-character record. */
u32 fn_8018397C(u32 groupId, u32 index)
{
    return floorCharacterBiosGetTalkSctID(fn_801170A4(groupId, index));
}

/* The move script of a person's floor-character record. */
u32 fn_80183958(u32 groupId, u32 index)
{
    return floorCharacterBiosGetMoveSctID(fn_801170A4(groupId, index));
}

s32 fn_801837D8(u32 groupId, u32 index, u32 flagId, u32 param1, u32 param2) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (flagId == 0) {
        return 0;
    }

    entry->flagId =
        fn_800F7318(15, flagId, 0x1000, 1, 0, 4, groupId, index,
                    param1, param2);
    return 1;
}

/* Hide the person whose self pointer is `self` and suspend its script thread. */
s32 fn_80183730(PeopleEntry* self)
{
    PeopleEntry* entry;
    void* thread;

    entry = peopleFindBySelf(self);
    if (entry == NULL) {
        return 0;
    }
    entry->visible = 0;
    thread = fn_800F7108(entry->flagId);
    if (thread == NULL) {
        return 0;
    }
    GSthreadBlock(thread);
    return 1;
}

/* Show the person whose self pointer is `self` and resume its script thread. */
s32 fn_80183688(PeopleEntry* self)
{
    PeopleEntry* entry;
    void* thread;

    entry = peopleFindBySelf(self);
    if (entry == NULL) {
        return 0;
    }
    entry->visible = 1;
    thread = fn_800F7108(entry->flagId);
    if (thread == NULL) {
        return 0;
    }
    GSthreadUnblock(thread);
    return 1;
}

/* Clear the transient interaction flags and restore the high movement flag. */
void std076_characterEventFlagsClear(u32 groupId, u32 index) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleClearFlags(entry, 0x100);
    }

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleClearFlags(entry, 0x400);
    }

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleSetFlags(entry, 0x80000000);
    }
}

/* Set the transient interaction flags and suspend the high movement flag. */
void std077_characterEventFlagsSet(u32 groupId, u32 index) {
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleSetFlags(entry, 0x100);
    }

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleSetFlags(entry, 0x400);
    }

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        peopleClearFlags(entry, 0x80000000);
    }
}

/*
 * Drop a person in from 40 units above its position: show it, play the
 * fall motion (7) while lowering it by 1.3 units a tick, then the landing
 * motion (8) to its end, then stand (1).
 */
void std178_characterPlayAnimationAndWait(u32 groupId, u32 index)
{
    PeopleEntry* entry;
    GSvec position;
    f32 ground;
    f32 height = 40.0f;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return;
    }
    peopleGetPositionByID(groupId, index, &position);
    ground = position.y;
    peopleSetVisibleInline(groupId, index, TRUE);
    peopleClearFlags(entry, 8);
    soundStartSE(0x49E);
    peopleSetMotionIndex(entry, 7);
    for (;;) {
        height -= 1.3 * GSgetFrameStep();
        if (height < 0.0f) {
            height = 0.0f;
        }
        position.y = ground + height;
        peoplePlaceAt(groupId, index, &position);
        if (height <= 0.0f) {
            break;
        }
        _threadSwitch();
    }
    peopleSetMotionIndex(entry, 8);
    peopleWaitSyncMotion(groupId, index, TRUE);
    peopleSetMotionIndex(entry, 1);
}

void std131_characterSurpriseAnimation(u32 groupId, u32 index) {
    PeopleEntry* entry;
    void* model;
    u32 modelGroup;
    u32 modelIndex;
    s8 attachmentIndex;

    attachmentIndex = 0;
    modelGroup = std093_getCurrentGroupID();
    modelIndex = std112_modelGetResourceID(0x0F850400);

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry != NULL) {
        PeopleInfoBiosEntry* info = peopleInfoBiosGetPtr(entry->scriptRef);
        if (info != NULL) {
            attachmentIndex = charModelGetHeadPart(info);
        }
    }

    if (attachmentIndex >= 0) {
        std070_characterAttachToCharacter(modelGroup, modelIndex, groupId, index,
                    attachmentIndex);
    } else {
        std070_characterAttachToCharacter(modelGroup, modelIndex, groupId, index, 0);
    }

    model = GSresGetResource(modelGroup, modelIndex);
    if (model != NULL) {
        fn_800E3CC8(model, 1);
        GSmodelClearShadowFlags(model, 1);
    }

    std115_mapObjectPlayAnimation(modelIndex, 0, 0, 0);
    soundStartSE(0x47);

    entry = peopleFindBySelf(peopleFindSelf(modelGroup, modelIndex));
    if (entry != NULL) {
        entry->flagId = fn_800F7318(0xF, 0x0596000E, 0x1000, 1, 0, 4,
                                    modelGroup, modelIndex, 0, 0);
    }
}

/*
 * Helpers admitted under the same-engine sister-title clause of docs/CAMPAIGN_OPERATIONS.md
 * (user decision, 2026-09-28; XD names from TeamOrre/xd-decomp symbols.txt at 4989794e,
 * XD code from trevor403/xd-asm at b1087f18; see docs/recon/title_walls_evidence.md). Pokemon XD keeps
 * both of fn_80181850's inline bodies as named statics:
 * _peopleMoveTypeRandomRot__FP13tagPeopleWork (XD 0x80299D30, dispatched from
 * peopleDaemon's move-type switch next to _peopleMoveTypeLinear/List/
 * RandomWalk, which are fn_801858C4/fn_8018524C/fn_80184D80 here) and
 * _peopleUpdateShadows__FP13tagPeopleWork (XD 0x802A0328: current floor,
 * floorGetCurrentGroupID, position, GScolsys2WalkGetLayer, receiver count,
 * one or two receiver surfaces, GSmodelSetShadowSurface). Colosseum has no
 * second expansion of either. With both helpers fn_80181850 is exact.
 */

/* Random look-around: wait, pick a heading, turn to it (XD's RandomRot). */
static inline void peopleMoveTypeRandomRot(PeopleEntry* entry)
{
    f32 frameCount;
    f32 angle;

    switch (entry->subState) {
    case 0:
        if (entry->animBlendFactor > 0.0f) {
            frameCount = (f32)fn_800D37CC();
            entry->animBlendFactor -= (f32)GSgetFrameStep() / frameCount;
            if (entry->animBlendFactor < 0.0f) {
                entry->animBlendFactor = 0.0f;
            }
            break;
        }
        entry->subState = 1;
        /* fallthrough */
    case 1:
        angle = 3.141592653589793 + entry->field_40 +
                1.5707963267948966 * fn_800E0BA0();
        angle = fmod(angle, 6.2831855f);
        peopleStartTurnInline(entry->groupId, entry->index, angle, 1.0f);
        entry->subState = 2;
        /* fallthrough */
    case 2:
        entry->animBlendFactor =
            entry->field_8C * fn_800E0BA0() + entry->field_88;
        entry->subState = 0;
        break;
    }
}

/* Point a person's shadow at the receiver surfaces under it. */
static inline void peopleUpdateShadows(PeopleEntry* entry)
{
    GSvec floorPosition;
    void* shadowSurfaces[2];
    u8 layer;
    u8 subLayer;
    void* floor;
    s32 receiverCount;
    s32 shadowCount;
    u32 group;

    floor = floorDataBiosGetCurrentPtr();
    if (floor != NULL) {
        group = std093_getCurrentGroupID();
        fn_8018FC98(entry, &floorPosition);
        if (!GScolsys2WalkGetLayer(&floorPosition, &layer, &subLayer)) {
            layer = 0;
            subLayer = 0;
        }

        receiverCount = floorDataBiosGetShadowReciveNum(floor);
        if (layer < receiverCount && subLayer < receiverCount) {
            shadowCount = 1;
            shadowSurfaces[0] = GSresGetResource(
                group, floorDataBiosGetShadowReciveID(floor, layer));
            if (layer != subLayer) {
                shadowCount = 2;
                shadowSurfaces[1] = GSresGetResource(
                    group, floorDataBiosGetShadowReciveID(floor, subLayer));
            }
            GSmodelSetShadowSurface(entry->modelHandle, shadowCount, shadowSurfaces);
        }
    }
}

void peopleMain(void)
{
    /*
     * RULE-EXCEPTION(title-path): block-scope extern used to stop an
     * inline - see docs/RULE_EXCEPTIONS.md. Under the TU's -inline auto,
     * fn_8018F30C (defined above) is just under the auto-inline limit and
     * would be expanded here; retail calls it. A clean fix needs the source
     * form that puts fn_8018F30C over the limit (or keeps it out of the TU's
     * auto-inline candidates) without dead code.
     */
    extern void fn_8018F30C(void);
    s32 i;
    PeopleEntry* entry;
    GSvec currentPosition;
    GSvec modelRotation;
    GSvec modelPosition;
    u8 visible;

    i = peopleGetMaxCount();
    while (i-- > 0) {
        entry = peopleGetEntry(i);
        if (!entry->active) {
            continue;
        }

        if (peopleTestFlags(entry, 0x40000000)) {
            GSvecCopy(&modelPosition, peopleGetPositionPtr(entry));
            GSvecCopy(&modelRotation, peopleGetTransform(entry));
            GSvecSub(&entry->collisionX, &modelPosition, &modelRotation);
        }

        fn_8018FC98(entry, &currentPosition);
        peopleSetTransform(entry, &currentPosition);
        entry->talkRange = 0.0f;

        if (entry->visible) {
            visible = TRUE;
        } else if (!fn_800F7108(entry->flagId)) {
            visible = TRUE;
        } else {
            visible = FALSE;
        }

        if (visible && !entry->talkLock) {
            switch (entry->state) {
            case 1:
                fn_801858C4(entry);
                break;
            case 2:
                fn_8018524C(entry, FALSE);
                break;
            case 3:
                fn_8018524C(entry, TRUE);
                break;
            case 4:
                fn_80184D80(entry);
                break;
            case 5:
                peopleMoveTypeRandomRot(entry);
                break;
            }

            if (GSgetFrameStep() != 0) {
                GSvecSub(&currentPosition, peopleGetPositionPtr(entry), &currentPosition);
                vecNormalize(&currentPosition, &currentPosition, (f32)GSgetFrameStep());
                entry->talkRange = peopleCalcRange(entry->groupId, entry->index,
                                                   &currentPosition);
            }
        }

        peopleTurnUpdate(entry);
        fn_80185B90(entry, entry->talkRange);
        peopleHeadLookUpdate(entry, 75.0f);

        peopleUpdateShadows(entry);
    }

    fn_8018F30C();
}

/* Lock a person for talking (playing motion slot 1), or release the lock. */
s32 std231_characterSetPathPaused(u32 groupId, u32 index, u8 doSetup)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (entry->state == 0) {
        return 1;
    }
    if (doSetup) {
        if (!entry->talkLock) {
            entry->talkLock = 1;
            peopleSetMotionIndex(entry, 1);
        }
        return 1;
    }
    if (entry->talkLock) {
        entry->talkLock = 0;
    }
    return 0;
}

/*
 * Suspend a person's scripted state (4 or 5) for an interaction, or restore
 * it afterwards.
 */
s32 peopleTalkPause(u32 groupId, u32 index, u8 doInteract)
{
    PeopleEntry* entry;

    entry = peopleFindBySelf(peopleFindSelf(groupId, index));
    if (entry == NULL) {
        return 0;
    }
    if (doInteract) {
        entry->prevState = entry->state;
        switch (entry->state) {
        case 4:
        case 5:
            entry->state = 0;
            break;
        }
    } else {
        switch (entry->prevState) {
        case 4:
        case 5:
            entry->state = entry->prevState;
            entry->subState = 0;
            entry->animBlendFactor = 1.0f;
            break;
        }
    }
    return 1;
}

/* The people movement step (auto-inlined into the per-frame update). */
void fn_801812C4(PeopleEntry* entry)
{
    peopleHeadLookUpdate(entry, 75.0f);
}
