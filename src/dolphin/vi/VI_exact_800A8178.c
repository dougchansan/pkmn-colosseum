/** Exact DVD identity/fatal-message helpers, 0x800A8178 - 0x800A839C. */
#include "dolphin/dvd/dvd.h"
#include "dolphin/gx/GX.h"

BOOL DVDCompareDiskID(const DVDDiskID* id1, const DVDDiskID* id2)
{
    extern s32 strncmp(const char* str1, const char* str2, u32 length);

    if (id1->gameName[0] != '\0' && id2->gameName[0] != '\0' &&
        strncmp(id1->gameName, id2->gameName, 4) != 0) {
        return FALSE;
    }
    if (id1->company[0] == '\0' || id2->company[0] == '\0' ||
        strncmp(id1->company, id2->company, 2) != 0) {
        return FALSE;
    }
    if (id1->diskNumber != 0xFF && id2->diskNumber != 0xFF &&
        id1->diskNumber != id2->diskNumber) {
        return FALSE;
    }
    if (id1->gameVersion != 0xFF && id2->gameVersion != 0xFF &&
        id1->gameVersion != id2->gameVersion) {
        return FALSE;
    }
    return TRUE;
}

void ShowMessage(void)
{
    extern const GXColor lbl_8047C2D8;
    extern const GXColor lbl_8047C2DC;
    extern const char* lbl_804789E0;
    extern const char* lbl_804789E4;
    extern const char* lbl_8026F5F8[6];
    extern u32 VIGetTvFormat(void);
    extern u16 OSGetFontEncode(void);
    extern u8 OSGetLanguage(void);
    extern void fn_8009CD38(GXColor foreground, GXColor background,
                            const char* message);
    GXColor background = lbl_8047C2D8;
    GXColor foreground = lbl_8047C2DC;
    const char* message;

    if (VIGetTvFormat() == 0) {
        if (OSGetFontEncode() == 1) {
            message = lbl_804789E0;
        } else {
            message = lbl_804789E4;
        }
    } else {
        message = lbl_8026F5F8[OSGetLanguage()];
    }
    fn_8009CD38(foreground, background, message);
}

extern void (*FatalFunc_8047A830)(void);

BOOL DVDSetAutoFatalMessaging(BOOL enable)
{
    extern BOOL OSDisableInterrupts(void);
    extern BOOL OSRestoreInterrupts(BOOL level);
    BOOL enabled;
    BOOL previous;

    enabled = OSDisableInterrupts();
    if (FatalFunc_8047A830 != 0) {
        previous = TRUE;
    } else {
        previous = FALSE;
    }
    FatalFunc_8047A830 = enable ? ShowMessage : 0;
    OSRestoreInterrupts(enabled);
    return previous;
}

void __DVDPrintFatalMessage(void)
{
    if (FatalFunc_8047A830 != 0) {
        FatalFunc_8047A830();
    }
}
