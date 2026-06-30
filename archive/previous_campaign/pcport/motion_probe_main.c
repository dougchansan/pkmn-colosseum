#include "real_content_host.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    const char* batchFrames = getenv("PCPORT_MOTION_BATCH_PROBE");
    const char* motionFrames = getenv("PCPORT_MOTION_PROBE");
    const char* bankFrames = getenv("PCPORT_CHARANIM_BANK_PROBE");
    const char* charFrames = getenv("PCPORT_CHARANIM_PROBE");
    const char* animDump = getenv("PCPORT_ANIM_DUMP");
    const char* animDumpFrames = getenv("PCPORT_ANIM_DUMP_FRAMES");
    const char* meshDump = getenv("PCPORT_MESH_DUMP");
    const char* meshDumpFrames = getenv("PCPORT_MESH_DUMP_FRAMES");
    const char* fsys = getenv("PCPORT_SWIZ_ARCHIVE");
    const char* member = getenv("PCPORT_SWIZ_MEMBER");
    int dumpMotionIdx;
    int dumpFrames;

    (void)argc;
    (void)argv;

    if (fsys == NULL || fsys[0] == '\0') {
        fsys = "orig/GC6E01/disc/files/field_common.fsys";
    }
    if (member == NULL || member[0] == '\0') {
        member = "ken_b1";
    }

    printf("[headless-motion-main] start fsys=%s member=%s\n", fsys, member);
    fflush(stdout);

    if (batchFrames != NULL) {
        PCPort_HeadlessMotionBatchProbe(atoi(batchFrames));
        return 0;
    }
    if (animDump != NULL && animDump[0] != '\0') {
        dumpMotionIdx = atoi(animDump);
        dumpFrames = (animDumpFrames != NULL && animDumpFrames[0] != '\0') ?
                     atoi(animDumpFrames) : 24;
        PCPort_AnimDump(fsys, member, dumpMotionIdx, dumpFrames);
        return 0;
    }
    if (meshDump != NULL && meshDump[0] != '\0') {
        dumpMotionIdx = atoi(meshDump);
        dumpFrames = (meshDumpFrames != NULL && meshDumpFrames[0] != '\0') ?
                     atoi(meshDumpFrames) : 24;
        PCPort_MeshDump(fsys, member, dumpMotionIdx, dumpFrames);
        return 0;
    }
    if (motionFrames != NULL) {
        PCPort_HeadlessMotionProbe(fsys, member, atoi(motionFrames));
        return 0;
    }
    if (bankFrames != NULL) {
        PCPort_HeadlessMotionProbe(fsys, member, atoi(bankFrames));
        return 0;
    }
    if (charFrames != NULL) {
        PCPort_HeadlessMotionProbe(fsys, member, atoi(charFrames));
        return 0;
    }

    PCPort_HeadlessMotionProbe(fsys, member, 24);
    return 0;
}
