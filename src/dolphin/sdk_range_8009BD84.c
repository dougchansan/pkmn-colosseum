/**
 * @file sdk_range_8009BD84.c
 * @brief dolphin-sdk code, 0x8009BD84 - 0x8009DF88 (21 fns).
 *
 * Range unit assigned from the propagated subsystem map
 * (tools/subsystem_propagation.py, >=80% single-label dominance;
 * campaign 2026-07-01). All functions asm-only until matched; the
 * range name stays honest until internal TU structure is proven.
 */
#include "dolphin/types.h"
#include "dolphin/db/DB.h"
#include "dolphin/gx/GX.h"
#include "dolphin/os/OS.h"
#include "dolphin/os/OSContext.h"
#include "dolphin/os/OSThread.h"
#include "dolphin/si/SI.h"
#include "dolphin/vi/VI.h"

#define OS_FPUCONTEXT (*(OSContext* volatile*)0x800000D8)

/* OSContext.c (0x8009B914 - 0x8009C2E0) links as its own file:
 * src/dolphin/os/OSContext.c. */

/* OSError.c (OSReport .. __OSUnhandledException, 0x8009C2E0 - 0x8009C860)
 * links as its own file: src/dolphin/os/OSError.c. */

typedef struct OSFontHeader {
    u16 fontType;
    u16 firstChar;
    u16 lastChar;
    u16 invalChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
    u16 cellWidth;
    u16 cellHeight;
    u32 sheetSize;
    u16 sheetFormat;
    u16 sheetColumn;
    u16 sheetRow;
    u16 sheetWidth;
    u16 sheetHeight;
    u16 widthTable;
    u32 sheetImage;
    u32 sheetFullSize;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;
} OSFontHeader;

/**
 * OSFatal's parameter block, at the head of the 0x2E8-byte lbl_803FB538
 * object: FatalParam (0xC, padded to 0x10) followed by FatalContext (0x2D8).
 */
typedef struct OSFatalParam {
    /* 0x00 */ GXColor fg;
    /* 0x04 */ GXColor bg;
    /* 0x08 */ const char* msg;
} OSFatalParam;

#if !defined(SDK_8009BD84_BANK_ACTIVE) || \
    defined(SDK_EXACT_8009C860_8009CD38)
void ScreenReport(void* xfb, u16 xfbW, u16 xfbH, GXColor yuv, s32 x, s32 y,
                  s32 leading, const char* string) {
    extern char* OSGetFontTexel(const char* string, void* image, s32 pos,
                             s32 stride, s32* width);
    u8* ptr;
    s32 width;
    u32 i;
    u32 j;
    u32 image[72];
    u32 k;
    u32 l;
    u8 Y;
    u32 pixel;
    s32 col;

loop_1:
    if (xfbH - 24 >= y) {
        ptr = (u8*)xfb + ((x + (y * xfbW)) * 2);
        col = x;

        while ((s8)*string != 0) {
            if ((s8)*string == '\n') {
                string++;
                y += leading;
                goto loop_1;
            }

            if (xfbW - 48 < col) {
                y += leading;
                goto loop_1;
            }

            for (i = 0; i < 24; i++) {
                j = (i & 7) + ((i >> 3) * 24);
                image[j + 0] = 0;
                image[j + 8] = 0;
                image[j + 16] = 0;
            }

            string = OSGetFontTexel(string, image, 0, 6, &width);

            for (i = 0; i < 24; i++) {
                j = (i & 7) + ((i >> 3) * 24);

                for (k = 0; k < 24; k++) {
                    l = j + (k & 0xFFFFFFF8);

                    Y = (image[l] >> ((7 - (k & 7)) * 4)) & 0xF;
                    if (Y != 0) {
                        Y = (((yuv.r * (Y * 0xEF)) / 255) / 15) + 0x10;
                        pixel = k + (i * xfbW);
                        ptr[pixel * 2] = Y;

                        if ((col + k) & 1) {
                            ptr[(pixel * 2) - 1] = yuv.g;
                            ptr[(pixel * 2) + 1] = yuv.b;
                        } else {
                            ptr[(pixel * 2) - 1] = yuv.b;
                            ptr[(pixel * 2) + 1] = yuv.g;
                        }
                    }
                }
            }

            ptr += width * 2;
            col += width;
        }
    }
}

void ConfigureVideo(u16 fbWidth, u16 xfbHeight) {
    extern void VIConfigure(GXRenderModeObj* mode);
    extern void VIConfigurePan(u16 xOrigin, u16 yOrigin, u16 width, u16 height);
    GXRenderModeObj mode;

    mode.fbWidth = fbWidth;
    mode.efbHeight = 480;
    mode.xfbHeight = xfbHeight;
    mode.viXOrigin = 40;
    mode.viWidth = 640;
    mode.viHeight = xfbHeight;

    switch (VIGetTvFormat()) {
    case VI_NTSC:
    case VI_MPAL:
        if (*(volatile u16*)0xCC00206C & 1) {
            mode.viTVmode = 2;
            mode.viYOrigin = 0;
            mode.xfbMode = 0;
        } else {
            mode.viTVmode = 0;
            mode.viYOrigin = 0;
            mode.xfbMode = 1;
        }
        break;
    case VI_EUR60:
        mode.viTVmode = 20;
        mode.viYOrigin = 0;
        mode.xfbMode = 1;
        break;
    case VI_PAL:
        mode.viTVmode = 4;
        mode.viYOrigin = 47;
        mode.xfbMode = 1;
        break;
    }

    VIConfigure(&mode);
    VIConfigurePan(0, 0, 640, 480);
}
#endif

#if !defined(SDK_8009BD84_BANK_ACTIVE) || \
    defined(SDK_CANDIDATE_8009CD38_8009D510)
/* Inlined into Halt (Halt); emits no standalone code. */
static GXColor RGB2YUV(GXColor rgb) {
    f32 Y;
    f32 Cb;
    f32 Cr;
    GXColor yuv;

    Y = 0.5f + (16.0f + ((0.098f * (f32)rgb.b) +
                         ((0.257f * (f32)rgb.r) + (0.504f * (f32)rgb.g))));
    Cb = 0.5f + (128.0f + ((0.439f * (f32)rgb.b) +
                           ((-0.148f * (f32)rgb.r) - (0.291f * (f32)rgb.g))));
    Cr = 0.5f + (128.0f + (((0.439f * (f32)rgb.r) - (0.368f * (f32)rgb.g)) -
                           (0.071f * (f32)rgb.b)));

    yuv.r = (Y > 235.0f) ? 235.0f : (Y < 16.0f) ? 16.0f : Y;
    yuv.g = (Cb > 240.0f) ? 240.0f : (Cb < 16.0f) ? 16.0f : Cb;
    yuv.b = (Cr > 240.0f) ? 240.0f : (Cr < 16.0f) ? 16.0f : Cr;
    yuv.a = 0;

    return yuv;
}
#endif

/*
 * The bus clock as the SDK declares it for MWCC: an address-bound variable
 * (TeamOrre/xd-decomp 4989794e include/dolphin/os.h, `u32 __OSBusClock
 * AT_ADDRESS(OS_BASE_CACHED | 0x00F8)`), not SI.h's pointer macro. OSFatal's
 * one-second timeout is written in the loop condition as in XD's matching
 * OSFatal.c (src/dolphin/os/OSFatal.c there); MWCC hoists the address-bound
 * read out of the loop, which the pointer macro does not allow, and the
 * hoisted timeout then takes retail's registers (r25/r26, after t).
 */
extern u32 __OSBusClock : 0x800000F8;

/**
 * OSFatal. Halts the machine, then hands off to Halt (Halt) on a fresh
 * fiber stack at the top of the arena to draw the message to the screen.
 */
#if !defined(SDK_8009BD84_BANK_ACTIVE) || \
    defined(SDK_CANDIDATE_8009CD38_8009D510)
void fn_8009CD38(GXColor fg, GXColor bg, const char* msg) {
    extern void __OSStopAudioSystem(void);
    extern void AISetStreamVolLeft(u8 vol);
    extern void AISetStreamVolRight(u8 vol);
    extern BOOL __OSCallResetFunctions(BOOL final); /* __OSCallResetFunctions */
    extern u32 VIGetRetraceCount(void);        /* VIGetRetraceCount */
    extern void GXAbortFrame(void);       /* GXAbortFrame */
    extern void VIInit(void);
    extern OSTime OSGetTime(void);
    extern void VISetBlack(BOOL black);
    extern void VIFlush(void);
    extern u8 lbl_803FB538[];            /* FatalParam + FatalContext */
    extern void Halt(void);       /* Halt */
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSEnableInterrupts(void);

    OSFatalParam* fp = (OSFatalParam*)lbl_803FB538;
    OSBootInfo* bootInfo = (OSBootInfo*)0x80000000;
    u32 count;
    OSTime t;

    OSDisableInterrupts();
    OSDisableScheduler();
    OSClearContext((OSContext*)&lbl_803FB538[0x10]);
    OSSetCurrentContext((OSContext*)((u8*)(fp + 1) + 4));
    __OSStopAudioSystem();
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    VIInit();
    VISetBlack(TRUE);
    VIFlush();
    OSEnableInterrupts();

    count = VIGetRetraceCount();
    do {
    } while ((s32)(VIGetRetraceCount() - count) < 1);

    t = OSGetTime();
    while (!__OSCallResetFunctions(FALSE) &&
           OSGetTime() - t < 1000 * (__OSBusClock / 4 / 1000)) {
    }

    OSDisableInterrupts();
    __OSCallResetFunctions(TRUE);
    __OSSetExceptionHandler(8, OSDefaultExceptionHandler);
    GXAbortFrame();
    OSSetArenaLo((void*)0x81200000);
    OSSetArenaHi((void*)bootInfo->FSTLocation);

    fp->fg = fg;
    fp->bg = bg;
    fp->msg = msg;
    OSSwitchFiber((u32)Halt, (u32)OSGetArenaHi());
}
#endif

/** EXI channel register file; [3] is the channel-0 control register. */
volatile u32 __EXIRegs[15] : 0xCC006800;

#if !defined(SDK_8009BD84_BANK_ACTIVE) || \
    defined(SDK_CANDIDATE_8009CD38_8009D510)
/* Inlined into Halt (Halt); emits no standalone code. */
static void ScreenClear(void* xfb, u16 xfbW, u16 xfbH, GXColor yuv) {
    int i;
    int j;
    u8* ptr;

    ptr = xfb;
    for (i = 0; i < xfbH; i++) {
        for (j = 0; j < xfbW; j += 2) {
            *ptr++ = yuv.r;
            *ptr++ = yuv.g;
            *ptr++ = yuv.r;
            *ptr++ = yuv.b;
        }
    }
}

/**
 * Halt: OSFatal's fiber entry point. Copies the message into the fresh arena,
 * quiesces EXI, loads the SDK font, clears the XFB to the background colour
 * and prints the message, then stops the CPU.
 *
 * ScreenClear and RGB2YUV above are both inlined here.
 */
void Halt(void) {
    extern void EXISetExiCallback(s32 chan, void* callback); /* EXISetExiCallback */
    extern BOOL EXILock(s32 chan, u32 dev, void* callback);
    extern BOOL EXIDeselect(s32 chan);
    extern BOOL EXIUnlock(s32 chan);
    extern u16 OSGetFontEncode(void);                        /* OSGetFontEncode */
    extern u32 OSLoadFont(OSFontHeader* fontData, void* tmp); /* OSLoadFont */
    extern u32 VIGetRetraceCount(void);                        /* VIGetRetraceCount */
    extern void* OSAllocFromArenaLo(u32 size, u32 align);
    extern void* memmove(void* dst, const void* src, u32 size);
    extern u32 strlen(const char* s);
    extern void VISetNextFrameBuffer(void* fb);
    extern void VISetBlack(BOOL black);
    extern void VIFlush(void);
    extern void DCFlushRange(void* addr, u32 nBytes);
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSEnableInterrupts(void);
    extern void OSReport(const char* format, ...);
    extern u8 lbl_803FB538[]; /* FatalParam + FatalContext */
    extern char lbl_80478998[] __attribute__((section(".sdata"))); /* "%s\n" */

    OSFatalParam* fp;
    u32 count;
    OSFontHeader* fontData;
    void* xfb;
    u32 len;

    OSEnableInterrupts();
    fp = (OSFatalParam*)lbl_803FB538;
    len = strlen(fp->msg) + 1;
    fp->msg = memmove(OSAllocFromArenaLo(len, 32), fp->msg, len);

    EXISetExiCallback(0, NULL);
    EXISetExiCallback(2, NULL);

    while (!EXILock(0, 1, NULL)) {
        while ((__EXIRegs[3] & 1) == 1) {
        }
        EXIDeselect(0);
        EXIUnlock(0);
    }
    EXIUnlock(0);

    while ((__EXIRegs[3] & 1) == 1) {
    }

    if (OSGetFontEncode() == 1) {
        fontData = OSAllocFromArenaLo(0x90EE4, 32);
    } else {
        fontData = OSAllocFromArenaLo(0x10120, 32);
    }
    OSLoadFont(fontData, OSGetArenaLo());

    xfb = OSAllocFromArenaLo(0x96000, 32);
    ScreenClear(xfb, 640, 480, RGB2YUV(fp->bg));
    VISetNextFrameBuffer(xfb);
    ConfigureVideo(640, 480);
    VIFlush();

    count = VIGetRetraceCount();
    do {
    } while ((s32)(VIGetRetraceCount() - count) < 2);

    ScreenReport(xfb, 640, 480, RGB2YUV(fp->fg), 48, 100, fontData->leading,
                 fp->msg);
    DCFlushRange(xfb, 0x96000);
    VISetBlack(FALSE);
    VIFlush();

    count = VIGetRetraceCount();
    do {
    } while ((s32)(VIGetRetraceCount() - count) < 1);

    OSDisableInterrupts();
    OSReport(lbl_80478998, fp->msg);
    PPCHalt();
}
#endif


/* 0x8009D820 | 0x58 */
#if !defined(SDK_8009BD84_BANK_ACTIVE) || \
    defined(SDK_EXACT_8009D510_8009DF3C)
static BOOL IsSjisTrailByte(u8 c) {
    return (0x40 <= c && c <= 0xFC) && (c != 0x7F);
}

/* Maps a character code to its index in the IPL font sheet. */
int GetFontCode(u16 code) {
    extern u16 lbl_80310CD8[]; /* HankakuToCode */
    extern u16 lbl_80310E58[]; /* Zenkaku2Code */
    extern u16 OSGetFontEncode(void);

    if (OSGetFontEncode() == 1) {
        if (code >= 0x20 && code <= 0xDF) {
            return lbl_80310CD8[code - 0x20];
        }

        if (code > 0x889E && code <= 0x9872) {
            int i = ((code >> 8) - 0x88) * 188;
            int j = (code & 0xFF);

            if (!IsSjisTrailByte(j)) {
                return 0;
            }

            j -= 0x40;
            if (j >= 0x40) {
                j--;
            }

            return (i + j + 0x2BE);
        }

        if (code >= 0x8140 && code < 0x879E) {
            int i = ((code >> 8) - 0x81) * 188;
            int j = (code & 0xFF);

            if (!IsSjisTrailByte(j)) {
                return 0;
            }

            j -= 0x40;
            if (j >= 0x40) {
                j--;
            }

            return lbl_80310E58[i + j];
        }
    } else if (code > 0x20 && code <= 0xFF) {
        return code - 0x20;
    }

    return 0;
}

/* Yay0 decompressor for the IPL font sheet. */
void Decode(u8* s, u8* d) {
    int i;
    int j;
    int k;
    int p;
    int q;
    int linkTable;
    int chunkSrc;
    int cnt;
    int os;
    unsigned int flag;
    unsigned int code;

    os = *(int*)(s + 0x4);
    linkTable = *(int*)(s + 0x8);
    chunkSrc = *(int*)(s + 0xC);

    q = 0;
    flag = 0;
    p = 16;

    do {
        if (flag == 0) {
            code = *(u32*)(s + p);
            p += sizeof(u32);
            flag = sizeof(u32) * 8;
        }

        if (code & 0x80000000) {
            d[q++] = s[chunkSrc++];
        } else {
            j = s[linkTable] << 8 | s[linkTable + 1];
            linkTable += sizeof(u16);

            k = q - (j & 0x0FFF);
            cnt = j >> 12;
            if (cnt == 0) {
                cnt = s[chunkSrc++] + 0x12;
            } else {
                cnt += 2;
            }

            for (i = 0; i < cnt; i++, q++, k++) {
                d[q] = d[k - 1];
            }
        }

        code <<= 1;
        flag--;
    } while (q < os);
}

u16 OSGetFontEncode(void) {
    extern u16 lbl_804789A0;

    if (lbl_804789A0 <= 1) {
        return lbl_804789A0;
    }

    switch (*(s32*)0x800000CC) {
    case 0:
        lbl_804789A0 = (*(volatile u16*)0xCC00206E & 2) ? 1 : 0;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        lbl_804789A0 = 0;
        break;
    }

    return lbl_804789A0;
}

void ReadROM(void* dest, s32 size, s32 offset) {
    extern BOOL __OSReadROM(void* dest, s32 size, s32 offset);

    while (size > 0) {
        s32 chunkSize = size <= 0x100 ? size : 0x100;
        size -= chunkSize;
        while (!__OSReadROM(dest, chunkSize, offset)) {
        }
        offset += chunkSize;
        dest = (u8*)dest + chunkSize;
    }
}

/* Font state: header, width table, and the cached sheetColumn*sheetRow. */
#define FontData lbl_8047A700
#define FontWidthTable lbl_8047A708
#define FontCharsPerSheet lbl_8047A70C

static inline u32 GetFontSize(u8* buf) {
    if (buf[0] == 'Y' && buf[1] == 'a' && buf[2] == 'y') {
        return *(u32*)(buf + 4);
    }

    return 0;
}

static inline u32 ReadFont(void* image) {
    if (OSGetFontEncode() == 1) {
        ReadROM(image, 0x4D000, 0x1AFF00);
    } else {
        ReadROM(image, 0x3000, 0x1FCF00);
    }

    return GetFontSize(image);
}

/*
 * Reads the font sheet out of IPL ROM, decompresses it, and caches the
 * header, width table and chars-per-sheet that OSGetFontTexel reads back.
 */
u32 OSLoadFont(OSFontHeader* fontData, void* tmp) {
    extern OSFontHeader* FontData;
    extern u8* FontWidthTable;
    extern s32 FontCharsPerSheet;
    extern u32 lbl_8047A704;
    extern const u32 lbl_8047C280;
    extern const u32 lbl_8047C284;
    extern void ReadROM(void* dest, s32 size, s32 offset);
    extern void Decode(u8* s, u8* d);
    u32 size;

    lbl_8047A704 = 0;
    size = ReadFont(tmp);
    if (size != 0) {
        Decode(tmp, (u8*)fontData);
        FontData = fontData;
        FontWidthTable = (u8*)FontData + FontData->widthTable;
        FontCharsPerSheet = FontData->sheetColumn * FontData->sheetRow;

        if (OSGetFontEncode() == 1) {
            u8* image;
            u8* ptr;
            int fontCode;
            int sheet;
            int numChars;
            int row;
            int column;
            int x;
            int y;
            u16 glyph[4];

            *(u32*)&glyph[0] = lbl_8047C280;
            *(u32*)&glyph[2] = lbl_8047C284;

            fontCode = GetFontCode(0x54);
            sheet = fontCode / FontCharsPerSheet;
            numChars = fontCode - (sheet * FontCharsPerSheet);
            row = numChars / FontData->sheetColumn;
            column = numChars - (row * FontData->sheetColumn);
            row *= FontData->cellHeight;
            column *= FontData->cellWidth;
            image = (u8*)FontData + FontData->sheetImage;
            image += (sheet * FontData->sheetSize) / 2;

            for (y = 4; y < 8; y++) {
                x = 0;
                ptr = image + (((FontData->sheetWidth / 8) * 32) / 2) *
                                  ((row + y) / 8);
                ptr += ((column + x) / 8) * 16;
                ptr += ((row + y) % 8) * 2;
                ptr += ((column + x) % 8) / 4;

                *(u16*)ptr = glyph[y - 4];
            }
        }
    }

    return size;
}

static BOOL IsSjisLeadByte(u8 c) {
    return ((c >= 0x81) && (c <= 0x9F)) || ((c >= 0xE0) && (c <= 0xFC));
}

char* OSGetFontTexel(const char* string, void* image, s32 pos, s32 stride,
                  s32* width) {
    extern OSFontHeader* FontData;
    extern u8* FontWidthTable;
    extern s32 FontCharsPerSheet;
    extern int GetFontCode(u16 code);
    u16 encode;
    u16 code;
    u8* dst;
    s32 fontCode;
    s32 sheet;
    u8* src;
    s32 numChars;
    s32 row;
    s32 column;
    s32 x;
    s32 y;
    s32 offsetSrc;
    u8* colorIndex;
    u8* imageSrc;

    code = (u8)*string;
    if (code == 0) {
        return (char*)string;
    }
    string++;

    encode = OSGetFontEncode();
    if (encode == 1) {
        if (IsSjisLeadByte((u8)code) && (s8)*string != 0) {
            code = (code << 8) | (u8)*string;
            string++;
        }
    }

    colorIndex = &FontData->c0;
    fontCode = GetFontCode(code);

    sheet = fontCode / FontCharsPerSheet;
    numChars = fontCode - (sheet * FontCharsPerSheet);
    row = numChars / FontData->sheetColumn;
    column = numChars - (row * FontData->sheetColumn);
    row *= FontData->cellHeight;
    column *= FontData->cellWidth;

    imageSrc = (u8*)FontData + FontData->sheetImage;
    imageSrc += ((sheet * FontData->sheetSize) >> 1);

    for (y = 0; y < FontData->cellHeight; y++) {
        for (x = 0; x < FontData->cellWidth; x++) {
            src = imageSrc + (((FontData->sheetWidth / 8) * 32) / 2) *
                                 ((row + y) / 8);
            src += ((column + x) / 8) * 16;
            src += ((row + y) % 8) * 2;
            src += ((column + x) % 8) / 4;

            offsetSrc = (column + x) % 4;

            dst = (u8*)image + ((y / 8) * (((stride * 4) / 8) * 32));
            dst += (((pos + x) / 8) * 32);
            dst += ((y % 8) * 4);
            dst += ((pos + x) % 8) / 2;

            *dst |= colorIndex[*src >> (6 - (offsetSrc * 2)) & 3] &
                    (((pos + x) % 2 != 0) ? 0x0F : 0xF0);
        }
    }

    if (width != 0) {
        *width = FontWidthTable[fontCode];
    }

    return (char*)string;
}
#endif
