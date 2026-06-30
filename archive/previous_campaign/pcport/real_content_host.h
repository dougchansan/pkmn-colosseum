#ifndef PCPORT_REAL_CONTENT_HOST_H
#define PCPORT_REAL_CONTENT_HOST_H

#include "dolphin/types.h"
#include "hsd/hsd_pobj.h"
#include "hsd/hsd_fobj.h"
#include "hsd/hsd_jobj.h"

/* Host kickoff for the FObj keyframe interpreter (hsd_fobj_host.c): put an
 * FObj list into the "load next packet" start state (flags low-nibble = 2),
 * since the adapted src/hsd/hsd_fobj.c zeroes flags. */
void PCPort_FObjStartAnim(HSD_FObj* fobj, f32 startframe);

/* Walk a live HSD_JObj tree (after HSD_JObjAddAnimAll) and kick every attached
 * AObj's FObj chain into the interpreter start state via PCPort_FObjStartAnim.
 * Covers both joint SRT anim (jobj->aobj) and material/texture anim
 * (dobj->mobj->{aobj, tobj->aobj}). Defined in hsd_host.c. */
void PCPort_HSDStartAnimAll(HSD_JObj* root);
void PCPort_HSDJObjAnimJointOnlyAll(HSD_JObj* root);

typedef struct {
    u8* storage;
    u32 storageSize;
    u32 dataSize;
    u32 relocCount;
    u32 publicCount;
    u32 externCount;
    u32 dataOffset;
    u32 relocOffset;
    u32 publicOffset;
    u32 externOffset;
    u32 stringOffset;
} PCPortHSDArchive;

typedef struct {
    PCPortHSDArchive archive;
    u8* memberData;
    u32 rootOff;
    u32 animArrOff;
    u32 motionOffs[64];
    u32 motionCount;
} PCPortHostMotionBank;

BOOL PCPort_HostMotionBankLoad(const char* fsysPath,
                               const char* memberName,
                               PCPortHostMotionBank* bank,
                               int verbose);
void PCPort_HostMotionBankRelease(PCPortHostMotionBank* bank);
HSD_JObj* PCPort_HostMotionCreateRoot(PCPortHostMotionBank* bank,
                                      u32 motionIdx,
                                      f32* outEndFrame);
void PCPort_HostMotionRestart(HSD_JObj* root);
void PCPort_HostMotionStepAndApply(HSD_JObj* root,
                                   PCPortHSDArchive* beArchive,
                                   u32 beRootJoint,
                                   f32* timeInOut,
                                   f32 loopLen,
                                   BOOL applyRootTranslate);
f32 PCPort_HostMotionSRTChecksum(HSD_JObj* root);

typedef struct {
    HSD_PObj pobj;
    HSD_VtxDescList* verts;
    u8* displayList;
    void* positionData;
    void* colorData;
    void* texcoordData;
    void* texcoord1Data;
    void* normalData;
    u32 sourceArchiveOffset;
    u32 totalSubmittedVertices;
    u32 totalPrimitiveCommands;
    f32 minPosition[3];
    f32 maxPosition[3];
} PCPortTranslatedPObj;

typedef struct {
    u32 rootArchiveOffset;
    u32 jointArchiveOffset;
    f32 modelMatrix[3][4];
} PCPortTranslatedJointTransform;

typedef struct {
    u32 cameraArchiveOffset;
    u32 eyeArchiveOffset;
    u32 interestArchiveOffset;
    u16 viewportLeft;
    u16 viewportRight;
    u16 viewportTop;
    u16 viewportBottom;
    u16 scissorLeft;
    u16 scissorRight;
    u16 scissorTop;
    u16 scissorBottom;
    f32 eye[3];
    f32 interest[3];
    f32 up[3];
    f32 nearZ;
    f32 farZ;
    f32 fov;
    f32 aspect;
    f32 viewMatrix[3][4];
    f32 projectionMatrix[4][4];
} PCPortTranslatedCamera;

typedef struct {
    u32 mobjArchiveOffset;
    u32 materialArchiveOffset;
    u32 pedescArchiveOffset;
    u32 rendermode;
    u32 ambient;
    u32 diffuse;
    u32 specular;
    f32 alpha;
    f32 shininess;
    BOOL hasPEDesc;
    u8 peFlags;
    u8 peRef0;
    u8 peRef1;
    u8 peDstAlpha;
    u8 peType;
    u8 peSrcFactor;
    u8 peDstFactor;
    u8 peLogicOp;
    u8 peZComp;
    u8 peAlphaComp0;
    u8 peAlphaOp;
    u8 peAlphaComp1;
} PCPortTranslatedMaterial;

typedef enum {
    PCPORT_TRANSLATED_TEV_NONE = 0,
    PCPORT_TRANSLATED_TEV_I8_COLOR_RAMP = 1
} PCPortTranslatedTevKind;

typedef struct {
    u32 archiveOffset;
    u32 rawWords[16];
    u8 rawWordCount;
    u8 kind;
    u8 rampLight[4];
    u8 rampDark[4];
} PCPortTranslatedTev;

typedef struct {
    u32 tobjArchiveOffset;
    u32 imageArchiveOffset;
    u32 imageDataArchiveOffset;
    u32 tevArchiveOffset;
    u32 flags;
    u32 texCoordSrc;
    u8 coordId;
    u8 hasCoordId;
    u32 wrapS;
    u32 wrapT;
    u32 magFilter;
    u32 format;
    u16 width;
    u16 height;
    u8 repeatS;
    u8 repeatT;
    u8 mipmap;
    u8 tevMode;
    PCPortTranslatedTev tev;
    f32 blending;
    u32 tlutArchiveOffset; /* archive offset of the palette (lut) data, 0 if none */
    u32 tlutFmt;           /* GXTlutFmt of the palette entries */
    u16 tlutEntries;       /* number of palette entries */
} PCPortTranslatedTexture;

typedef enum {
    PCPORT_TEXP_STAGE_NONE = 0,
    PCPORT_TEXP_STAGE_I8_RAMP_SAMPLE = 1,
    PCPORT_TEXP_STAGE_I8_MASK_MODULATE = 2,
    PCPORT_TEXP_STAGE_DIRECT_SAMPLE = 3
} PCPortTranslatedTextureExpStageKind;

#define PCPORT_TEXP_STAGE_MAX 4

typedef struct {
    u32 headArchiveOffset;
    u8 kind;
    u8 nodeCount;
    u8 coordIds[PCPORT_TEXP_STAGE_MAX];
    u8 stageKinds[PCPORT_TEXP_STAGE_MAX];
    PCPortTranslatedTexture nodes[PCPORT_TEXP_STAGE_MAX];
} PCPortParsedTextureNodeChain;

typedef struct {
    u8 kind;
    u8 coordId;
    PCPortTranslatedTexture texture;
} PCPortTranslatedTextureExpStage;

typedef enum {
    PCPORT_TEXTURE_EXP_KIND_NONE = 0,
    PCPORT_TEXTURE_EXP_KIND_I8_RAMP = 1,
    PCPORT_TEXTURE_EXP_KIND_I8_RAMP_MASK = 2,
    PCPORT_TEXTURE_EXP_KIND_DIRECT_SAMPLE = 3
} PCPortTranslatedTextureExpKind;

typedef struct {
    u32 headArchiveOffset;
    u8 kind;
    u8 stageCount;
    PCPortTranslatedTextureExpStage stages[PCPORT_TEXP_STAGE_MAX];
} PCPortTranslatedTextureExp;

typedef enum {
    PCPORT_TEXTURE_CHAIN_NONE = 0,
    PCPORT_TEXTURE_CHAIN_I8_RAMP = 1,
    PCPORT_TEXTURE_CHAIN_I8_RAMP_MASK = 2
} PCPortTranslatedTextureChainKind;

#define PCPORT_TEXTURE_CHAIN_MAX_NODES 2

typedef struct {
    u32 headArchiveOffset;
    u8 nodeCount;
    u8 kind;
    u8 coordIds[PCPORT_TEXTURE_CHAIN_MAX_NODES];
    PCPortTranslatedTexture nodes[PCPORT_TEXTURE_CHAIN_MAX_NODES];
} PCPortTranslatedTextureChain;

BOOL PCPort_LoadFsysMember(const char* fsysPath, const char* memberName,
                           u8** outData, u32* outSize);
/* Returns the largest HSD-archive member exposing a "scene_data" public symbol
 * (for field maps whose members share names; the title uses LoadFsysMember). */
BOOL PCPort_LoadFsysSceneMember(const char* fsysPath, u8** outData, u32* outSize);
/* Returns the WZX collision-mesh member of a field-map .fsys (matched by
 * content signature, since it shares the map's member name). See field_collision.c. */
BOOL PCPort_LoadFsysWZXMember(const char* fsysPath, u8** outData, u32* outSize);
/* Attempt to locate the per-room exit/door trigger records (RE'd 0x2C-byte
 * "people"-subsystem exit table) inside a field-map .fsys scene_data. The 0x2C
 * layout is RE-derived from runtime SDA r13-relative accessor fns and a probe
 * (tools/pcport_probe/probe_exits.py) did NOT find a coherent static table in
 * the archive -- the exit array is populated at runtime by the asm people
 * subsystem during floor load. This function therefore currently always returns
 * FALSE (exit data not statically locatable); the host warp path falls back to
 * a hand-specified exit list. Kept as the integration point for the future RE.
 * On success (when implemented) writes a malloc'd buffer of raw 0x2C records to
 * *outData with *outCount the number of records; caller frees via PCPort_FreeBuffer. */
BOOL PCPort_LoadFsysExitData(const char* fsysPath, u8** outData, u32* outCount);
void PCPort_FreeBuffer(void* buffer);
/* Enumerate .fsys members (name/size/compression) to stdout. Diagnostic. */
void PCPort_FsysListMembers(const char* fsysPath);

BOOL PCPort_HSDArchiveParseBE(PCPortHSDArchive* archive,
                              const void* data, u32 size);
/* Swizzle a parsed scene archive's joint graph BE->LE + relocate pointers to
 * native, in place, so the GAME'S OWN HSD_*LoadDesc can consume it. Pass the
 * storage offset of the scene root HSD_Joint; returns it as a native pointer
 * (cast to HSD_Joint*). Destroys the BE layout -- use only for the HSD pipeline. */
void* PCPort_SwizzleSceneForHSD(PCPortHSDArchive* archive, u32 jointListOffset);
/* Verify the swizzle math: load+resolve+swizzle a scene member, print sane-value
 * report (root joint SRT/flags, first material colors/alpha, first TObj). */
void PCPort_HSDSwizzleSmoke(const char* fsysPath, const char* memberName);

/* Build + arm a live animated HSD_JObj tree from a scene member (swizzle ->
 * LoadJoint -> AddAnimAll -> ReqAnimAll -> StartAnimAll). Returns 1 on success.
 * The archive storage is kept alive internally (the live tree points into it). */
int PCPort_TitleAnimSetup(const char* fsysPath, const char* memberName);
/* Advance the title HSD animation one frame (HSD_JObjAnimAll over the live tree
 * built by PCPort_TitleAnimSetup). No-op if setup was not run / failed. */
void PCPort_TitleAnimTick(void);

/* Field scene-ambient animation (signpost swing, etc.).
 * PCPort_FieldAnimSetup: load a fresh copy of the field map's scene archive
 *   and arm the animjoint tree.  Pass the .fsys path; memberName may be NULL
 *   to auto-select the largest scene_data member (same as PCPort_EngineFieldSetup).
 *   Returns 1 on success, 0 if no animjoint is present (static map).
 * PCPort_FieldAnimTick: advance the scene anim by frameStep game frames each
 *   frame (1.0 = one 60 Hz tick); loops via re-arm when end_frame is reached.
 * PCPort_FieldAnimRelease: reset state before loading a new map. */
/* outAnimRootOff (may be NULL): receives the archive offset of the animated
 * slot's rootJoint so the caller can pass it to PCPort_FieldAnimSetRenderTarget. */
int  PCPort_FieldAnimSetup(const char* fsysPath, const char* memberName,
                           u32* outAnimRootOff);
void PCPort_FieldAnimTick(f32 frameStep);
void PCPort_FieldAnimRelease(void);
/* Register the render-side BE archive so PCPort_FieldAnimTick can write updated
 * SRT into the storage that RenderJointTree reads.  Call after EngineFieldSetup. */
void PCPort_FieldAnimSetRenderTarget(PCPortHSDArchive* renderArchive,
                                     u32 renderAnimRootOff);
/* Walk the live field-anim JObj tree + BE archive simultaneously and populate
 * the module-scope UV-offset table from every TObj whose TexAnim drove a
 * non-zero translate_x/translate_y this tick.  renderArchive is &g_engTitleArchive
 * and beRootJointOff is g_engTitleRootJoint (same args as SetRenderTarget). */
void PCPort_FieldAnimHarvestTexUV(PCPortHSDArchive* renderArchive,
                                   u32 beRootJointOff);
/* Lookup the UV offset for a given TObj archive offset.  Returns 1 and fills
 * *outU / *outV if a TexAnim entry was found; returns 0 for identity UV. */
int PCPort_FieldAnimGetTexUV(u32 tobjArchiveOffset, f32* outU, f32* outV);
/* Diagnostic: build a character's live animated tree and report how many joints
 * actually move over <frames> (proves whether the archive carries real motion). */
void PCPort_CharAnimProbe(const char* fsysPath, const char* memberName, int frames);
/* Diagnostic: enumerate a character's Resource+0x4 motion bank and step every
 * motion over real HSD data, printing moved-joint counts and SRT checksums. */
void PCPort_CharAnimBankProbe(const char* fsysPath, const char* memberName,
                              int frames);
/* Diagnostic: non-visual motion-bank proof. Steps each AnimJoint/FObj motion and
 * prints per-motion SRT checksum variation plus loop/cyclic classification. */
void PCPort_MotionProbe(const char* fsysPath, const char* memberName,
                        int frames);
void PCPort_HeadlessMotionProbe(const char* fsysPath, const char* memberName,
                                int frames);
void PCPort_AnimDump(const char* fsysPath, const char* memberName,
                     int motionIdx, int frames);
void PCPort_MeshDump(const char* fsysPath, const char* memberName,
                     int motionIdx, int frames);
typedef struct PCPortLocomotionSuggestion {
    int valid;
    int idle;
    int walk;
    int run;
    int idleConfirmed;
    int walkConfirmed;
    int runConfirmed;
    int allConfirmed;
    u32 motionCount;
    u32 cyclicCount;
    u32 varyingCyclicCount;
    f32 idleEnergy;
    f32 walkEnergy;
    f32 runEnergy;
} PCPortLocomotionSuggestion;
/* Data-derived locomotion map from the character's motion bank. The current
 * heuristic selects the lowest/middle/highest-energy cyclic motions. */
int PCPort_CharAnimSuggestLocomotionMap(const char* fsysPath,
                                        const char* memberName,
                                        int* outIdle,
                                        int* outWalk,
                                        int* outRun);
int PCPort_CharAnimSuggestLocomotionMapEx(const char* fsysPath,
                                          const char* memberName,
                                          int frames,
                                          PCPortLocomotionSuggestion* out);
/* Batch non-visual classifier over the field/character archives. It prints one
 * row for each member with a Resource+0x4 motion bank. */
void PCPort_MotionBatchProbe(int frames);
void PCPort_HeadlessMotionBatchProbe(int frames);
/* Headless Battle Colosseum vertical-slice probe. Loads four pkx Pokemon models,
 * chooses deterministic stance/attack/damage motions from their real motion
 * banks, prints fixed 2v2 placement, menu text, and a scripted one-turn flow. */
void PCPort_BattleProbe(int frames);

/* Field-character animation. PCPort_CharAnimSetup builds a live animated HSD
 * tree from a swizzled copy of the character archive (once). Each frame,
 * PCPort_CharAnimStepAndApply advances it and writes the animated per-joint SRT
 * back into the renderer's BE archive (beArchive/beRootJoint) so the existing
 * skinning animates. PCPort_CharAnimReady reports whether setup succeeded. */
int  PCPort_CharAnimSetup(const char* fsysPath, const char* memberName);
/* frameStep: game-frame units to advance this call (1.0 = one 60 Hz tick).
 * Pass elapsed_seconds * 60.0f for real-time-locked animation. */
void PCPort_CharAnimStepAndApply(PCPortHSDArchive* beArchive, u32 beRootJoint,
                                 f32 frameStep);
int  PCPort_CharAnimReady(void);
/* Switch the active motion (idle/walk/run) by bank index; rebuilds the live
 * tree from the remembered archive. No-op if already on that motion. */
int  PCPort_CharAnimSetMotion(int motionIdx);
void PCPort_HSDArchiveDestroy(PCPortHSDArchive* archive);
const void* PCPort_HSDArchiveGetPublicAddress(const PCPortHSDArchive* archive,
                                              const char* name,
                                              u32* outArchiveOffset);
u32 PCPort_ReadBigEndianU32(const void* data);
BOOL PCPort_TranslatePObjFromArchiveBE(const PCPortHSDArchive* archive,
                                       u32 pobjArchiveOffset,
                                       PCPortTranslatedPObj* outPObj);
void PCPort_DestroyTranslatedPObj(PCPortTranslatedPObj* pobj);
BOOL PCPort_TranslateJointChainToMatrixBE(const PCPortHSDArchive* archive,
                                          u32 rootJointArchiveOffset,
                                          u32 targetJointArchiveOffset,
                                          PCPortTranslatedJointTransform* outTransform);
BOOL PCPort_TranslatePerspectiveCameraFromArchiveBE(const PCPortHSDArchive* archive,
                                                    u32 cameraArchiveOffset,
                                                    PCPortTranslatedCamera* outCamera);
BOOL PCPort_TranslateMaterialFromArchiveBE(const PCPortHSDArchive* archive,
                                           u32 mobjArchiveOffset,
                                           PCPortTranslatedMaterial* outMaterial);
BOOL PCPort_TranslateTextureFromArchiveBE(const PCPortHSDArchive* archive,
                                          u32 tobjArchiveOffset,
                                          PCPortTranslatedTexture* outTexture);
BOOL PCPort_TranslateTextureNodeFromArchiveBE(const PCPortHSDArchive* archive,
                                              u32 tobjArchiveOffset,
                                              PCPortTranslatedTexture* outTexture);
BOOL PCPort_ParseTextureNodeChainFromArchiveBE(const PCPortHSDArchive* archive,
                                               u32 tobjArchiveOffset,
                                               u32 maxNodes,
                                               PCPortParsedTextureNodeChain* outChain);
BOOL PCPort_TranslateTextureExpFromArchiveBE(const PCPortHSDArchive* archive,
                                             u32 tobjArchiveOffset,
                                             PCPortTranslatedTextureExp* outExp);
BOOL PCPort_TranslateTextureChainFromArchiveBE(const PCPortHSDArchive* archive,
                                               u32 tobjArchiveOffset,
                                               PCPortTranslatedTextureChain* outChain);
BOOL PCPort_BakeTextureRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                         const PCPortTranslatedTexture* texture,
                                         u8** outPixels,
                                         u32* outSize);
BOOL PCPort_BakeTextureExpRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                            const PCPortTranslatedTextureExp* exp,
                                            u8** outPixels,
                                            u32* outSize);
BOOL PCPort_BakeTextureChainRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                              const PCPortTranslatedTextureChain* chain,
                                              u8** outPixels,
                                              u32* outSize);

#endif
