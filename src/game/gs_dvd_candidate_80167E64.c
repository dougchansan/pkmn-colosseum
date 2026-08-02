/** DVD request helpers, 0x80167E64 - 0x80167FA4. */
#include "dolphin/types.h"

typedef struct GSDVDWork {
    u8 active;
    u8 started;
    u8 drawing;
    u8 _pad03;
    u8 fileInfo[0x3C];
    void (*callback)(s32 result, struct GSDVDWork* work);
} GSDVDWork;

extern void fn_80168164(u8* flag);
extern void DVDClose(void* fileInfo);
extern BOOL DVDReadAsync(void* fileInfo, void* addr, s32 length, s32 offset,
                         void (*callback)(s32 result, void* fileInfo),
                         s32 priority);
extern s32 DVDRead(void* fileInfo, void* addr, s32 length, s32 offset,
                   s32 priority);
extern s32 DVDConvertPathToEntrynum(const char* path);
extern GSDVDWork* fn_8016819C(void);
extern BOOL DVDOpen(const char* path, void* fileInfo);
extern void _AsyncCallback(s32 result, void* fileInfo);
extern void __assert(const char* file, u32 line, const char* condition);
extern const char lbl_8047D584[8];
extern const char lbl_8047D58C[8];

void fn_80167E64(u8* file)
{
    fn_80168164(file);
    DVDClose(file + 4);
}

u8 fn_80167E98(GSDVDWork* work, void* addr, s32 length, s32 offset,
                void (*callback)(s32 result, GSDVDWork* work))
{
    work->callback = callback;
    return DVDReadAsync(work->fileInfo, addr, length, offset, _AsyncCallback,
                        2);
}

s32 fn_80167ED0(GSDVDWork* work, void* addr, s32 length, s32 offset)
{
    return DVDRead(work->fileInfo, addr, length, offset, 2);
}

u32 fn_80167EF8(const char* path)
{
    return DVDConvertPathToEntrynum(path) != -1;
}

GSDVDWork* fn_80167F28(const char* path)
{
    GSDVDWork* work = fn_8016819C();

    if (work == NULL) {
        return NULL;
    }
    if (!DVDOpen(path, work->fileInfo)) {
        __assert(lbl_8047D584, 0x26A, lbl_8047D58C);
        fn_80168164((u8*)work);
        return NULL;
    }
    return work;
}
