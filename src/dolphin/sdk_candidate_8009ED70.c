/**
 * OSLink.c from OSLinkFixed onwards (0x8009ED70 - 0x8009F1B8), linked with
 * OSUnlink's report string (.data 0x80311840 - 0x80311868).
 */
#include "dolphin/os/OSModule.h"

extern OSModuleQueue __OSModuleInfoList : (0x800030C8);
extern const void* __OSStringTable : (0x800030D0);
extern void DCFlushRange(void* address, u32 length);
extern void ICInvalidateRange(void* address, u32 length);
extern void OSReport(const char* format, ...);
extern BOOL Link(OSModuleInfo* newModule, void* bss, BOOL fixed);

#define SECTION_INFO(module) \
    ((OSSectionInfo*)((OSModuleInfo*)(module))->sectionInfoOffset)
#define SECTION_OFFSET(offset) OS_SECTIONINFO_OFFSET(offset)

BOOL OSLinkFixed(OSModuleInfo* module, void* data) {
    if (module->version > 3 || module->version < 3) {
        return FALSE;
    }
    return Link(module, data, 1);
}

extern void OSNotifyUnlink(OSModuleInfo* module);

BOOL OSUnlink(OSModuleHeader* newModule, OSModuleHeader* module)
{
    u32 idNew;
    OSImportInfo* imp;
    OSRel* rel;
    OSSectionInfo* si;
    OSSectionInfo* siFlush;
    u32* p;
    u32 offset;
    u32 x;
    u32 branchMask;

    branchMask = ~0x03FFFFFC;
    idNew = newModule->info.id;
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
        si = &SECTION_INFO(newModule)[rel->section];
        offset = SECTION_OFFSET(si->offset);
        x = 0;
        switch (rel->type) {
        case 0:
            break;
        case 1:
            *p = x;
            break;
        case 2:
            *p = (*p & branchMask) | (x & 0x03FFFFFC);
            break;
        case 3:
            *(u16*)p = x & 0xFFFF;
            break;
        case 4:
            *(u16*)p = x & 0xFFFF;
            break;
        case 5:
            *(u16*)p = (x >> 16) & 0xFFFF;
            break;
        case 6:
            *(u16*)p = ((x >> 16) + ((x & 0x8000) != 0)) & 0xFFFF;
            break;
        case 7:
        case 8:
        case 9:
            *p = (*p & ~0x0000FFFC) | (x & 0x0000FFFC);
            break;
        case 10:
            if (module->unresolvedSection != 0) {
                x = module->unresolved - (u32)p;
            }
            *p = (*p & branchMask) | (x & 0x03FFFFFC);
            break;
        case 11:
        case 12:
        case 13:
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
            OSReport("OSUnlink: unknown relocation type %3d\n", rel->type);
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

BOOL fn_8009EFE4(OSModuleInfo* oldModule)
{
    OSModuleHeader* moduleHeader;
    OSModuleInfo* moduleInfo;
    OSModuleInfo* next;
    OSModuleInfo* prev;
    OSSectionInfo* si;
    OSImportInfo* imp;
    u32 i;

    moduleHeader = (OSModuleHeader*)oldModule;
    next = oldModule->link.next;
    prev = oldModule->link.prev;
    if (next == NULL) {
        __OSModuleInfoList.tail = prev;
    } else {
        next->link.prev = prev;
    }
    if (prev == NULL) {
        __OSModuleInfoList.head = next;
    } else {
        prev->link.next = next;
    }

    for (moduleInfo = __OSModuleInfoList.head; moduleInfo != NULL;
         moduleInfo = moduleInfo->link.next)
    {
        OSUnlink(moduleHeader, (OSModuleHeader*)moduleInfo);
    }
    OSNotifyUnlink(oldModule);
    if (__OSStringTable != NULL) {
        oldModule->nameOffset -= (u32)__OSStringTable;
    }
    if (moduleHeader->prologSection != 0) {
        moduleHeader->prolog -= SECTION_OFFSET(
            SECTION_INFO(oldModule)[moduleHeader->prologSection].offset);
    }
    if (moduleHeader->epilogSection != 0) {
        moduleHeader->epilog -= SECTION_OFFSET(
            SECTION_INFO(oldModule)[moduleHeader->epilogSection].offset);
    }
    if (moduleHeader->unresolvedSection != 0) {
        moduleHeader->unresolved -= SECTION_OFFSET(
            SECTION_INFO(oldModule)[moduleHeader->unresolvedSection].offset);
    }
    for (imp = (OSImportInfo*)moduleHeader->impOffset;
         imp < (OSImportInfo*)(moduleHeader->impOffset + moduleHeader->impSize);
         imp++)
    {
        imp->offset -= (u32)moduleHeader;
    }
    for (i = 1; i < oldModule->numSections; i++) {
        si = &SECTION_INFO(oldModule)[i];
        if (i == moduleHeader->bssSection) {
            moduleHeader->bssSection = 0;
            si->offset = 0;
        } else if (si->offset != 0) {
            si->offset -= (u32)moduleHeader;
        }
    }
    moduleHeader->relOffset -= (u32)moduleHeader;
    moduleHeader->impOffset -= (u32)moduleHeader;
    oldModule->sectionInfoOffset -= (u32)moduleHeader;
    return TRUE;
}
