/**
 * @file archive.c
 * @brief HAL archive parser and public-symbol lookup, 0x80191ECC-0x801920E4.
 *
 * Separated from the Count-Sheep campaign's hsd_wobj.c into its original
 * library responsibility, with its diagnostic string owned here.
 * Archive layout and operations checked against doldecomp/melee,
 * src/sysdolphin/baselib/archive.c at
 * 598987cf347c125c7416517af18bf5c1dc3f15a1.
 * Colosseum's diagnostic has no file-size arguments; preserve that version.
 * The first integrated candidate retains the matching GC/1.3 -O1 build
 * and its non-deferred function order; readonly strings restore .rodata.
 */
#include "dolphin/types.h"
#include "hsd/hsd_archive.h"

extern int strcmp(const char* s1, const char* s2);
extern void OSReport(const char* fmt, ...);
extern void* memcpy(void* dst, const void* src, u32 size);
extern void* memset(void* dst, int val, u32 size);

void* HSD_ArchiveGetPublicAddress(HSD_Archive* archive, const char* symbols)
{
    u32 i;

    for (i = 0; i < archive->header.nb_public; i++) {
        int comparison =
            strcmp(archive->symbols + archive->public_info[i].symbol, symbols);

        if (comparison == 0) {
            return archive->data + archive->public_info[i].offset;
        }
    }

    return NULL;
}

static inline void Locate(HSD_Archive* archive)
{
    u32 i;
    u32* ptr;

    for (i = 0; i < archive->header.nb_reloc; i++) {
        ptr = (u32*) (archive->data + archive->reloc_info[i].offset);
        *ptr += (u32) archive->data;
    }
}

s32 HSD_ArchiveParse(HSD_Archive* archive, u8* src, u32 file_size)
{
    u32 offset = 0;

    if (archive == NULL) {
        return -1;
    }

    memset(archive, 0, sizeof(HSD_Archive));
    archive->flags = archive->flags | HSD_ARCHIVE_DONT_FREE;
    memcpy(archive, src, sizeof(HSD_ArchiveHeader));

    if (archive->header.file_size != file_size) {
        OSReport("HSD_ArchiveParse: byte-order mismatch! Please check data format\n");
        return -1;
    }

    offset += sizeof(HSD_ArchiveHeader);
    if (archive->header.data_size != 0) {
        archive->data = src + offset;
        offset += archive->header.data_size;
    }
    if (archive->header.nb_reloc != 0) {
        archive->reloc_info = (HSD_ArchiveRelocationInfo*) (src + offset);
        offset += archive->header.nb_reloc * sizeof(HSD_ArchiveRelocationInfo);
    }
    if (archive->header.nb_public != 0) {
        archive->public_info = (HSD_ArchivePublicInfo*) (src + offset);
        offset += archive->header.nb_public * sizeof(HSD_ArchivePublicInfo);
    }
    if (archive->header.nb_extern != 0) {
        archive->extern_info = (HSD_ArchiveExternInfo*) (src + offset);
        offset += archive->header.nb_extern * sizeof(HSD_ArchiveExternInfo);
    }
    if (offset < archive->header.file_size) {
        archive->symbols = (char*) (src + offset);
    }

    archive->top_ptr = src;
    Locate(archive);

    return 0;
}
