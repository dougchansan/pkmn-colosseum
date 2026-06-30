/* resolver_probe.c — self-contained native probe for the HSD archive resolver.
 *
 * Verifies the P1 input+parse+relocate pipeline on REAL game data without the
 * full GL render link: FSYS member table -> LZSS decompress -> HSD archive
 * header -> relocation fixup -> public-symbol (root) resolution. Mirrors
 * pcport/real_content_host.c (PCPort_LoadFsysMember / DecompressLZSS /
 * PCPort_HSDArchiveParseBE / PCPort_HSDArchiveGetPublicAddress) exactly.
 *
 * Build (32-bit, matches the port's pointer model):
 *   clang -m32 -O1 tools/pcport_probe/resolver_probe.c -o build_pc/resolver_probe.exe
 * Run:
 *   build_pc\resolver_probe.exe orig/GC6E01/disc/files/topmenu.fsys [member]
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define FSYS_MAGIC        0x46535953u
#define LZSS_MAGIC        0x4C5A5353u
#define LZSS_HEADER_SIZE  0x10u
#define LZSS_WINDOW_SIZE  0x1000u
#define LZSS_WINDOW_START 0x0FEEu

static u32 be32(const u8* p) { return ((u32)p[0]<<24)|((u32)p[1]<<16)|((u32)p[2]<<8)|p[3]; }
static u16 be16(const u8* p) { return (u16)(((u16)p[0]<<8)|p[1]); }
static void wbe32(u8* p, u32 v){ p[0]=(u8)(v>>24);p[1]=(u8)(v>>16);p[2]=(u8)(v>>8);p[3]=(u8)v; }

static u8* load_file(const char* path, u32* outSize) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END); long n = ftell(f); fseek(f, 0, SEEK_SET);
    if (n <= 0) { fclose(f); return NULL; }
    u8* d = (u8*)malloc((size_t)n);
    if (!d) { fclose(f); return NULL; }
    if (fread(d, 1, (size_t)n, f) != (size_t)n) { free(d); fclose(f); return NULL; }
    fclose(f); *outSize = (u32)n; return d;
}

/* classic 4KB-window LZSS, identical to real_content_host.c:DecompressLZSS */
static int lzss(const u8* src, u32 srcSize, u8* dst, u32 dstSize) {
    u8 win[LZSS_WINDOW_SIZE]; u32 sp = LZSS_HEADER_SIZE, dp = 0, wp = LZSS_WINDOW_START, flags = 0;
    if (srcSize < LZSS_HEADER_SIZE) return 0;
    memset(win, 0, sizeof(win));
    while (sp < srcSize && dp < dstSize) {
        flags >>= 1;
        if ((flags & 0x100) == 0) { if (sp >= srcSize) break; flags = (u32)src[sp++] | 0xFF00u; }
        if (flags & 1) {
            if (sp >= srcSize) break;
            u8 c = src[sp++]; dst[dp++] = c; win[wp] = c; wp = (wp+1)&0x0FFFu;
        } else {
            if (sp + 1 >= srcSize) break;
            u8 b1 = src[sp++], b2 = src[sp++];
            u32 off = (u32)b1 | (((u32)b2 & 0xF0u) << 4), len = ((u32)b2 & 0x0Fu) + 2u, j;
            for (j = 0; j <= len && dp < dstSize; ++j) { u8 c = win[(off+j)&0x0FFFu]; dst[dp++]=c; win[wp]=c; wp=(wp+1)&0x0FFFu; }
        }
    }
    return dp == dstSize;
}

/* FSYS member load: find by name, decompress if LZSS. Mirrors PCPort_LoadFsysMember. */
static u8* load_member(const u8* fsys, u32 fsysSize, const char* name, u32* outSize) {
    if (fsysSize < 0x20 || be32(fsys) != FSYS_MAGIC) return NULL;
    u32 entryCount = be32(fsys + 0x08);
    u32 strTab = be32(fsys + 0x18);
    if (strTab + 4 > fsysSize) return NULL;
    u32 entryTab = be32(fsys + strTab);
    if (entryTab >= fsysSize) return NULL;
    for (u32 i = 0; i < entryCount; ++i) {
        u32 eo = be32(fsys + entryTab + i*4);
        if (eo + 0x28 > fsysSize) continue;
        u32 no = be32(fsys + eo + 0x24);
        if (no >= fsysSize) continue;
        if (strcmp((const char*)(fsys + no), name) != 0) continue;
        u32 dataOff = be32(fsys + eo + 0x04), comp = be32(fsys + eo + 0x08);
        if (dataOff >= fsysSize || dataOff + comp > fsysSize) return NULL;
        if (comp >= LZSS_HEADER_SIZE && be32(fsys + dataOff) == LZSS_MAGIC) {
            u32 outN = be32(fsys + dataOff + 0x04), inN = be32(fsys + dataOff + 0x08);
            if (!outN || inN > comp || inN < LZSS_HEADER_SIZE) return NULL;
            u8* o = (u8*)malloc(outN);
            if (!o || !lzss(fsys + dataOff, inN, o, outN)) { free(o); return NULL; }
            *outSize = outN; return o;
        }
        u8* o = (u8*)malloc(comp ? comp : 1); memcpy(o, fsys + dataOff, comp); *outSize = comp; return o;
    }
    return NULL;
}

/* HSD archive parse + in-place relocation, mirrors PCPort_HSDArchiveParseBE. */
typedef struct { u8* storage; u32 size, dataSize, relocCount, publicCount, externCount;
                 u32 dataOff, relocOff, publicOff, externOff, stringOff; } Archive;

static int hsd_parse(Archive* a, const u8* data, u32 size) {
    if (size < 0x20) return 0;
    memset(a, 0, sizeof(*a));
    u32 fileSize = be32(data+0x00), dataSize = be32(data+0x04);
    a->relocCount = be32(data+0x08); a->publicCount = be32(data+0x0C); a->externCount = be32(data+0x10);
    if (fileSize != size) return 0;                       /* the HSD-archive signature check */
    a->dataOff = 0x20; a->dataSize = dataSize;
    a->relocOff  = 0x20 + dataSize;
    a->publicOff = a->relocOff  + a->relocCount * 4u;
    a->externOff = a->publicOff + a->publicCount * 8u;
    a->stringOff = a->externOff + a->externCount * 8u;
    if (a->relocOff > size || a->publicOff > size || a->externOff > size || a->stringOff > size) return 0;
    a->storage = (u8*)malloc(size); if (!a->storage) return 0;
    memcpy(a->storage, data, size); a->size = size;
    for (u32 i = 0; i < a->relocCount; ++i) {
        u32 fieldOff = be32(a->storage + a->relocOff + i*4);
        u32 abs = a->dataOff + fieldOff;
        if (abs + 4 > size) { free(a->storage); return 0; }
        wbe32(a->storage + abs, be32(a->storage + abs) + a->dataOff);
    }
    return 1;
}

static u32 hsd_public(const Archive* a, const char* name) {
    for (u32 i = 0; i < a->publicCount; ++i) {
        const u8* p = a->storage + a->publicOff + i*8;
        u32 res = be32(p), key = be32(p+4);
        if (a->stringOff + key >= a->size) continue;
        if (strcmp((const char*)(a->storage + a->stringOff + key), name) == 0)
            return a->dataOff + res;
    }
    return 0;
}

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IONBF, 0);   /* unbuffered: output survives any early exit */
    printf("resolver_probe: ptr=%zu bytes (want 4 for the 32-bit GameCube pointer model)\n", sizeof(void*));
    if (argc < 2) { printf("usage: resolver_probe <file.fsys> [member]\n"); return 2; }
    u32 fsysSize = 0; u8* fsys = load_file(argv[1], &fsysSize);
    if (!fsys) { printf("cannot read %s\n", argv[1]); return 1; }
    if (fsysSize < 0x20 || be32(fsys) != FSYS_MAGIC) { printf("not an FSYS archive\n"); return 1; }

    u32 entryCount = be32(fsys + 0x08), strTab = be32(fsys + 0x18);
    if (strTab + 4 > fsysSize) { printf("bad stringTableOffset\n"); return 1; }
    u32 entryTab = be32(fsys + strTab);
    if (entryTab >= fsysSize) { printf("bad entryTableOffset\n"); return 1; }
    printf("FSYS %s: %u members (strTab=0x%X entryTab=0x%X)\n", argv[1], entryCount, strTab, entryTab);

    /* enumerate members, flag which are HSD archives (fileSize == decompressed size) */
    int hsdShown = 0;
    for (u32 i = 0; i < entryCount; ++i) {
        if (entryTab + i*4 + 4 > fsysSize) break;
        u32 eo = be32(fsys + entryTab + i*4);
        if (eo + 0x28 > fsysSize) continue;
        u32 nameOff = be32(fsys + eo + 0x24);
        if (nameOff >= fsysSize) continue;
        const char* nm = (const char*)(fsys + nameOff);
        if (argv[2] && strcmp(nm, argv[2]) != 0) continue;
        u32 sz = 0; u8* m = load_member(fsys, fsysSize, nm, &sz);
        if (!m) { printf("  [%2u] %-20s <decompress failed>\n", i, nm); continue; }
        int isHsd = (sz >= 0x20 && be32(m) == sz);
        printf("  [%2u] %-20s size=0x%-7X %s\n", i, nm, sz, isHsd ? "HSD-ARCHIVE" : "");
        if (isHsd && (argv[2] || hsdShown < 3)) {
            Archive a;
            if (hsd_parse(&a, m, sz)) {
                printf("        header: data=0x%X reloc=%u public=%u extern=%u  (relocated OK)\n",
                       a.dataSize, a.relocCount, a.publicCount, a.externCount);
                for (u32 k = 0; k < a.publicCount && k < 8; ++k) {
                    const u8* p = a.storage + a.publicOff + k*8;
                    u32 key = be32(p+4); const char* sym = (const char*)(a.storage + a.stringOff + key);
                    printf("        public[%u] = '%s' @ data+0x%X\n", k, sym, be32(p));
                }
                u32 scene = hsd_public(&a, "scene_data");
                if (scene) printf("        >>> scene_data root resolved @ storage+0x%X\n", scene);
                free(a.storage);
            } else {
                printf("        header parse FAILED\n");
            }
            hsdShown++;
        }
        free(m);
    }
    free(fsys);
    return 0;
}
