/**
 * @file sdk_range_8009E7B0.c
 * @brief dolphin-sdk code, 0x8009E7B0 - 0x8009F1B8 (6 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"

typedef struct OSModuleInfo OSModuleInfo;
typedef struct OSModuleLink {
    OSModuleInfo* next;
    OSModuleInfo* prev;
} OSModuleLink;

struct OSModuleInfo {
    u32 id;
    OSModuleLink link;
    u32 numSections;
    u32 sectionInfoOffset;
    u32 nameOffset;
    u32 nameSize;
    u32 moduleType;
};

typedef struct {
    u32 offset;
    u32 size;
} OSSectionInfo;

typedef struct {
    u32 id;
    u32 offset;
} OSImportInfo;

typedef struct {
    u16 offset;
    u8 type;
    u8 section;
    u32 addend;
} OSRel;

typedef struct {
    OSModuleInfo info;
    u32 bssSize;
    u32 relOffset;
    u32 impOffset;
    u32 impSize;
    u8 prologSection;
    u8 epilogSection;
    u8 unresolvedSection;
    u8 bssSection;
    u32 prolog;
    u32 epilog;
    u32 unresolved;
    u32 align;
    u32 bssAlign;
    u32 fixSize;
} OSModuleHeader;

typedef struct {
    OSModuleInfo* head;
    OSModuleInfo* tail;
} OSModuleQueue;

extern OSModuleQueue __OSModuleInfoList : (0x800030C8);
extern const void* __OSStringTable : (0x800030D0);
extern void DCFlushRange(void* address, u32 length);
extern void ICInvalidateRange(void* address, u32 length);
extern void OSReport(const char* format, ...);
extern void fn_8009E7A8(OSModuleInfo* module);
extern void* memset(void* destination, s32 value, u32 size);

#define SECTION_INFO(module) \
    ((OSSectionInfo*)((OSModuleInfo*)(module))->sectionInfoOffset)
#define SECTION_OFFSET(offset) ((offset) & ~1)

BOOL OSLink(OSModuleHeader* newModule, OSModuleHeader* module)
{
    u32 idNew;
    OSImportInfo* imp;
    OSRel* rel;
    OSSectionInfo* si;
    OSSectionInfo* siFlush;
    u32* p;
    u32 offset;
    u32 x;

    idNew = newModule != NULL ? newModule->info.id : 0;
    for (imp = (OSImportInfo*)module->impOffset;
         imp < (OSImportInfo*)(module->impOffset + module->impSize); imp++)
    {
        if (imp->id == idNew) {
            goto found;
        }
    }
    return FALSE;

found:
    siFlush = NULL;
    for (rel = (OSRel*)imp->offset; rel->type != 203; rel++) {
        (u8*)p += rel->offset;
        if (idNew != 0) {
            si = &SECTION_INFO(newModule)[rel->section];
            offset = SECTION_OFFSET(si->offset);
        } else {
            offset = 0;
        }
        switch (rel->type) {
        case 0:
            break;
        case 1:
            x = offset + rel->addend;
            *p = x;
            break;
        case 2:
            x = offset + rel->addend;
            *p = (*p & ~0x03FFFFFC) | (x & 0x03FFFFFC);
            break;
        case 3:
            x = offset + rel->addend;
            *(u16*)p = x & 0xFFFF;
            break;
        case 4:
            x = offset + rel->addend;
            *(u16*)p = x & 0xFFFF;
            break;
        case 5:
            x = offset + rel->addend;
            *(u16*)p = (x >> 16) & 0xFFFF;
            break;
        case 6:
            x = offset + rel->addend;
            *(u16*)p = ((x >> 16) + ((x & 0x8000) ? 1 : 0)) & 0xFFFF;
            break;
        case 7:
        case 8:
        case 9:
            x = offset + rel->addend;
            *p = (*p & ~0x0000FFFC) | (x & 0x0000FFFC);
            break;
        case 10:
            x = offset + rel->addend - (u32)p;
            *p = (*p & ~0x03FFFFFC) | (x & 0x03FFFFFC);
            break;
        case 11:
        case 12:
        case 13:
            x = offset + rel->addend - (u32)p;
            *p = (*p & ~0x0000FFFC) | (x & 0x0000FFFC);
            break;
        case 201:
            break;
        case 202:
            si = &SECTION_INFO(module)[rel->section];
            p = (u32*)SECTION_OFFSET(si->offset);
            if (siFlush != NULL) {
                offset = SECTION_OFFSET(siFlush->offset);
                DCFlushRange((void*)offset, siFlush->size);
                ICInvalidateRange((void*)offset, siFlush->size);
            }
            siFlush = (si->offset & 1) ? si : NULL;
            break;
        default:
            OSReport("OSLink: unknown relocation type %3d\n", rel->type);
            break;
        }
    }
    if (siFlush != NULL) {
        offset = SECTION_OFFSET(siFlush->offset);
        DCFlushRange((void*)offset, siFlush->size);
        ICInvalidateRange((void*)offset, siFlush->size);
    }
    return TRUE;
}

BOOL Link(OSModuleInfo* newModule, void* bss, BOOL fixed)
{
    OSModuleHeader* moduleHeader;
    OSModuleInfo* moduleInfo;
    OSImportInfo* imp;
    OSSectionInfo* si;
    OSModuleInfo* previous;
    u32 i;

    moduleHeader = (OSModuleHeader*)newModule;
    moduleHeader->bssSection = 0;
    if (newModule->moduleType > 3 ||
        (newModule->moduleType >= 2 &&
         ((moduleHeader->align != 0 &&
           (u32)newModule % moduleHeader->align != 0) ||
          (moduleHeader->bssAlign != 0 &&
           (u32)bss % moduleHeader->bssAlign != 0))))
    {
        return FALSE;
    }
    previous = __OSModuleInfoList.tail;
    if (previous == NULL) {
        __OSModuleInfoList.head = newModule;
    } else {
        previous->link.next = newModule;
    }
    newModule->link.prev = previous;
    newModule->link.next = NULL;
    __OSModuleInfoList.tail = newModule;

    newModule->sectionInfoOffset += (u32)moduleHeader;
    moduleHeader->relOffset += (u32)moduleHeader;
    moduleHeader->impOffset += (u32)moduleHeader;
    if (newModule->moduleType >= 3) {
        moduleHeader->fixSize += (u32)moduleHeader;
    }
    for (i = 1; i < newModule->numSections; i++) {
        si = &SECTION_INFO(newModule)[i];
        if (si->offset != 0) {
            si->offset += (u32)moduleHeader;
        } else if (si->size != 0) {
            moduleHeader->bssSection = i;
            si->offset = (u32)bss;
            bss = (void*)((u32)bss + si->size);
        }
    }
    for (imp = (OSImportInfo*)moduleHeader->impOffset;
         imp < (OSImportInfo*)(moduleHeader->impOffset + moduleHeader->impSize);
         imp++)
    {
        imp->offset += (u32)moduleHeader;
    }
    if (moduleHeader->prologSection != 0) {
        moduleHeader->prolog += SECTION_OFFSET(
            SECTION_INFO(newModule)[moduleHeader->prologSection].offset);
    }
    if (moduleHeader->epilogSection != 0) {
        moduleHeader->epilog += SECTION_OFFSET(
            SECTION_INFO(newModule)[moduleHeader->epilogSection].offset);
    }
    if (moduleHeader->unresolvedSection != 0) {
        moduleHeader->unresolved += SECTION_OFFSET(
            SECTION_INFO(newModule)[moduleHeader->unresolvedSection].offset);
    }
    if (__OSStringTable != NULL) {
        newModule->nameOffset += (u32)__OSStringTable;
    }
    OSLink(NULL, moduleHeader);
    for (moduleInfo = __OSModuleInfoList.head; moduleInfo != NULL;
         moduleInfo = moduleInfo->link.next)
    {
        OSLink(moduleHeader, (OSModuleHeader*)moduleInfo);
        if (moduleInfo != newModule) {
            OSLink((OSModuleHeader*)moduleInfo, moduleHeader);
        }
    }
    if (fixed) {
        for (imp = (OSImportInfo*)moduleHeader->impOffset;
             imp <
             (OSImportInfo*)(moduleHeader->impOffset + moduleHeader->impSize);
             imp++)
        {
            if (imp->id == 0 || imp->id == newModule->id) {
                moduleHeader->impSize =
                    (u32)((u8*)imp - (u8*)moduleHeader->impOffset);
                break;
            }
        }
    }
    memset(bss, 0, moduleHeader->bssSize);
    fn_8009E7A8(newModule);
    return TRUE;
}

BOOL fn_8009ED4C(OSModuleInfo* module, void* data) {
    return Link(module, data, 0);
}

BOOL OSLinkFixed(OSModuleInfo* module, void* data) {
    if (module->moduleType > 3 || module->moduleType < 3) {
        return FALSE;
    }
    return Link(module, data, 1);
}
