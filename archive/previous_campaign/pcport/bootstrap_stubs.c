#include "dolphin/types.h"
#include "real_content_host.h"

#include <string.h>

typedef struct GLFWwindow GLFWwindow;

typedef struct PCPortTHP PCPortTHP;

int PCPortBGM_Init(void) {
    return 1;
}

int PCPortBGM_PlayFromFsys(const char* fsysPath, const char* member) {
    (void)fsysPath;
    (void)member;
    return 1;
}

void PCPortBGM_Update(void) {
}

void PCPortBGM_Stop(void) {
}

int PCPortBGM_IsPlaying(void) {
    return 0;
}

int MusyX_PlayWave(const char* fsysPath, const char* sdirMember,
                   const char* poolMember, const char* sampleMember) {
    (void)fsysPath;
    (void)sdirMember;
    (void)poolMember;
    (void)sampleMember;
    return 1;
}

void MusyX_StopWave(void) {
}

void fn_801AA568(void* pobj) {
    (void)pobj;
}

#ifndef PCPORT_BOOTSTRAP_REAL_CONTENT
BOOL PCPort_LoadFsysMember(const char* fsysPath, const char* memberName,
                           u8** outData, u32* outSize) {
    (void)fsysPath;
    (void)memberName;
    if (outData != NULL) {
        *outData = NULL;
    }
    if (outSize != NULL) {
        *outSize = 0u;
    }
    return 0;
}

BOOL PCPort_LoadFsysSceneMember(const char* fsysPath, u8** outData, u32* outSize) {
    return PCPort_LoadFsysMember(fsysPath, NULL, outData, outSize);
}

BOOL PCPort_LoadFsysWZXMember(const char* fsysPath, u8** outData, u32* outSize) {
    return PCPort_LoadFsysMember(fsysPath, NULL, outData, outSize);
}

BOOL PCPort_LoadFsysExitData(const char* fsysPath, u8** outData, u32* outCount) {
    (void)fsysPath;
    if (outData != NULL) {
        *outData = NULL;
    }
    if (outCount != NULL) {
        *outCount = 0u;
    }
    return 0;
}

void PCPort_FreeBuffer(void* buffer) {
    (void)buffer;
}

void PCPort_FsysListMembers(const char* fsysPath) {
    (void)fsysPath;
}

BOOL PCPort_HSDArchiveParseBE(PCPortHSDArchive* archive,
                              const void* data, u32 size) {
    if (archive != NULL) {
        memset(archive, 0, sizeof(*archive));
    }
    (void)data;
    (void)size;
    return 0;
}

const void* PCPort_HSDArchiveGetPublicAddress(const PCPortHSDArchive* archive,
                                              const char* name,
                                              u32* outArchiveOffset) {
    (void)archive;
    (void)name;
    if (outArchiveOffset != NULL) {
        *outArchiveOffset = 0u;
    }
    return NULL;
}

void PCPort_HSDArchiveDestroy(PCPortHSDArchive* archive) {
    (void)archive;
}

u32 PCPort_ReadBigEndianU32(const void* data) {
    const u8* b = (const u8*)data;
    return ((u32)b[0] << 24) | ((u32)b[1] << 16) | ((u32)b[2] << 8) | (u32)b[3];
}

BOOL PCPort_TranslatePObjFromArchiveBE(const PCPortHSDArchive* archive,
                                       u32 pobjArchiveOffset,
                                       PCPortTranslatedPObj* outPObj) {
    (void)archive;
    (void)pobjArchiveOffset;
    if (outPObj != NULL) {
        memset(outPObj, 0, sizeof(*outPObj));
    }
    return 0;
}

void PCPort_HSDSwizzleSmoke(const char* fsysPath, const char* memberName) {
    (void)fsysPath;
    (void)memberName;
}

void PCPort_DestroyTranslatedPObj(PCPortTranslatedPObj* pobj) {
    (void)pobj;
}

BOOL PCPort_TranslateJointChainToMatrixBE(const PCPortHSDArchive* archive,
                                          u32 rootJointArchiveOffset,
                                          u32 targetJointArchiveOffset,
                                          PCPortTranslatedJointTransform* outTransform) {
    (void)archive;
    (void)rootJointArchiveOffset;
    (void)targetJointArchiveOffset;
    if (outTransform != NULL) {
        memset(outTransform, 0, sizeof(*outTransform));
    }
    return 0;
}

BOOL PCPort_TranslatePerspectiveCameraFromArchiveBE(const PCPortHSDArchive* archive,
                                                    u32 cameraArchiveOffset,
                                                    PCPortTranslatedCamera* outCamera) {
    (void)archive;
    (void)cameraArchiveOffset;
    if (outCamera != NULL) {
        memset(outCamera, 0, sizeof(*outCamera));
    }
    return 0;
}

BOOL PCPort_TranslateMaterialFromArchiveBE(const PCPortHSDArchive* archive,
                                           u32 mobjArchiveOffset,
                                           PCPortTranslatedMaterial* outMaterial) {
    (void)archive;
    (void)mobjArchiveOffset;
    if (outMaterial != NULL) {
        memset(outMaterial, 0, sizeof(*outMaterial));
    }
    return 0;
}

BOOL PCPort_TranslateTextureFromArchiveBE(const PCPortHSDArchive* archive,
                                          u32 tobjArchiveOffset,
                                          PCPortTranslatedTexture* outTexture) {
    (void)archive;
    (void)tobjArchiveOffset;
    if (outTexture != NULL) {
        memset(outTexture, 0, sizeof(*outTexture));
    }
    return 0;
}

BOOL PCPort_TranslateTextureNodeFromArchiveBE(const PCPortHSDArchive* archive,
                                              u32 tobjArchiveOffset,
                                              PCPortTranslatedTexture* outTexture) {
    return PCPort_TranslateTextureFromArchiveBE(archive, tobjArchiveOffset, outTexture);
}

BOOL PCPort_ParseTextureNodeChainFromArchiveBE(const PCPortHSDArchive* archive,
                                               u32 tobjArchiveOffset,
                                               u32 maxNodes,
                                               PCPortParsedTextureNodeChain* outChain) {
    (void)archive;
    (void)tobjArchiveOffset;
    (void)maxNodes;
    if (outChain != NULL) {
        memset(outChain, 0, sizeof(*outChain));
    }
    return 0;
}

BOOL PCPort_TranslateTextureExpFromArchiveBE(const PCPortHSDArchive* archive,
                                             u32 tobjArchiveOffset,
                                             PCPortTranslatedTextureExp* outExp) {
    (void)archive;
    (void)tobjArchiveOffset;
    if (outExp != NULL) {
        memset(outExp, 0, sizeof(*outExp));
    }
    return 0;
}

BOOL PCPort_TranslateTextureChainFromArchiveBE(const PCPortHSDArchive* archive,
                                               u32 tobjArchiveOffset,
                                               PCPortTranslatedTextureChain* outChain) {
    (void)archive;
    (void)tobjArchiveOffset;
    if (outChain != NULL) {
        memset(outChain, 0, sizeof(*outChain));
    }
    return 0;
}

BOOL PCPort_BakeTextureRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                         const PCPortTranslatedTexture* texture,
                                         u8** outPixels,
                                         u32* outSize) {
    (void)archive;
    (void)texture;
    if (outPixels != NULL) {
        *outPixels = NULL;
    }
    if (outSize != NULL) {
        *outSize = 0u;
    }
    return 0;
}

BOOL PCPort_BakeTextureExpRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                            const PCPortTranslatedTextureExp* exp,
                                            u8** outPixels,
                                            u32* outSize) {
    (void)archive;
    (void)exp;
    if (outPixels != NULL) {
        *outPixels = NULL;
    }
    if (outSize != NULL) {
        *outSize = 0u;
    }
    return 0;
}

BOOL PCPort_BakeTextureChainRGBAFromArchiveBE(const PCPortHSDArchive* archive,
                                              const PCPortTranslatedTextureChain* chain,
                                              u8** outPixels,
                                              u32* outSize) {
    (void)archive;
    (void)chain;
    if (outPixels != NULL) {
        *outPixels = NULL;
    }
    if (outSize != NULL) {
        *outSize = 0u;
    }
    return 0;
}

void PCPort_CharAnimProbe(const char* fsysPath, const char* memberName, int frames) {
    (void)fsysPath; (void)memberName; (void)frames;
}

int PCPort_CharAnimSuggestLocomotionMap(const char* fsysPath,
                                        const char* memberName,
                                        int* outIdle,
                                        int* outWalk,
                                        int* outRun) {
    (void)fsysPath; (void)memberName;
    if (outIdle != NULL) *outIdle = -1;
    if (outWalk != NULL) *outWalk = -1;
    if (outRun != NULL) *outRun = -1;
    return 0;
}

int PCPort_CharAnimSuggestLocomotionMapEx(const char* fsysPath,
                                          const char* memberName,
                                          int frames,
                                          PCPortLocomotionSuggestion* out) {
    (void)fsysPath; (void)memberName; (void)frames;
    if (out != NULL) {
        memset(out, 0, sizeof(*out));
    }
    return 0;
}

void PCPort_MotionBatchProbe(int frames) {
    (void)frames;
}

void PCPort_HeadlessMotionBatchProbe(int frames) {
    (void)frames;
}

void PCPort_BattleProbe(int frames) {
    (void)frames;
}

int PCPort_CharAnimSetup(const char* fsysPath, const char* memberName) {
    (void)fsysPath; (void)memberName; return 0;
}

void PCPort_CharAnimStepAndApply(PCPortHSDArchive* beArchive, u32 beRootJoint,
                                 f32 frameStep) {
    (void)beArchive; (void)beRootJoint; (void)frameStep;
}

int PCPort_CharAnimReady(void) {
    return 0;
}

int PCPort_CharAnimSetMotion(int motionIdx) {
    (void)motionIdx;
    return 0;
}

void PCPort_HeadlessMotionProbe(const char* fsysPath, const char* memberName,
                                int frames) {
    (void)fsysPath; (void)memberName; (void)frames;
}

void PCPort_AnimDump(const char* fsysPath, const char* memberName,
                     int motionIdx, int frames) {
    (void)fsysPath; (void)memberName; (void)motionIdx; (void)frames;
}

void PCPort_MeshDump(const char* fsysPath, const char* memberName,
                     int motionIdx, int frames) {
    (void)fsysPath; (void)memberName; (void)motionIdx; (void)frames;
}

void PCPort_MotionProbe(const char* fsysPath, const char* memberName,
                        int frames) {
    (void)fsysPath;
    (void)memberName;
    (void)frames;
}

void PCPort_TitleAnimTick(void) {
}

int PCPort_TitleAnimSetup(const char* fsysPath, const char* memberName) {
    (void)fsysPath; (void)memberName; return 0;
}
#endif /* PCPORT_BOOTSTRAP_REAL_CONTENT */

PCPortTHP* PCPortTHP_Open(const char* path) {
    (void)path;
    return NULL;
}

void PCPortTHP_Close(PCPortTHP* thp) {
    (void)thp;
}

int PCPortTHP_Width(const PCPortTHP* thp) {
    (void)thp;
    return 0;
}

int PCPortTHP_Height(const PCPortTHP* thp) {
    (void)thp;
    return 0;
}

int PCPortTHP_FrameCount(const PCPortTHP* thp) {
    (void)thp;
    return 0;
}

float PCPortTHP_Fps(const PCPortTHP* thp) {
    (void)thp;
    return 0.0f;
}

int PCPortTHP_NextFrameRGBA(PCPortTHP* thp, const unsigned char** outRGBA) {
    (void)thp;
    if (outRGBA != NULL) {
        *outRGBA = NULL;
    }
    return 0;
}

int PCPortTHP_HasAudio(const PCPortTHP* thp) {
    (void)thp;
    return 0;
}

unsigned PCPortTHP_AudioSampleRate(const PCPortTHP* thp) {
    (void)thp;
    return 0u;
}

int PCPortTHP_AudioChannels(const PCPortTHP* thp) {
    (void)thp;
    return 0;
}

unsigned PCPortTHP_AudioTotalSamples(const PCPortTHP* thp) {
    (void)thp;
    return 0u;
}

int PCPortTHP_NextFrameAudioPCM(PCPortTHP* thp, const short** outPCM,
                                unsigned* outNumFrames) {
    (void)thp;
    if (outPCM != NULL) {
        *outPCM = NULL;
    }
    if (outNumFrames != NULL) {
        *outNumFrames = 0u;
    }
    return 0;
}

int RunFibreSelfTest(void) {
    return 1;
}

int RunEngineSpike(GLFWwindow* window) {
    (void)window;
    return 1;
}

int RunSchedTest(void) {
    return 1;
}

int RunEngineBoot(GLFWwindow* window) {
    (void)window;
    return 1;
}
