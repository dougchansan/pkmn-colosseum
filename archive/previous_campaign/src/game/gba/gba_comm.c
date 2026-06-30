/**
 * @file gba_comm.c
 * @brief GBA Link Cable communication for Pokemon Colosseum.
 *
 * This file corresponds to the original gbaCommunication.c source file,
 * confirmed by the string "gbaCommunication.c" at lbl_8026F5A8 in rodata.
 *
 * Address range: 0x80092C90 - 0x800937F4 (6+ functions)
 *
 * The GBA communication system enables data transfer between the
 * GameCube and GBA Pokemon games via the GCN-GBA Link Cable. The
 * cable connects to a GCN controller port and uses the Serial
 * Interface (SI) peripheral for bidirectional communication.
 *
 * Each of the 4 controller ports can host a GBA connection. The
 * system allocates a 0x44A0-byte work buffer per active channel
 * from GSmem, containing:
 *   - SI thread data and transfer buffer (0x0000 - 0x4337)
 *   - State variables (0x4338 - 0x4343)
 *   - Pokemon data buffer (0x4344 - 0x449F)
 *
 * Channel pointers are stored in BSS at lbl_803FB328 (array of 4 u32*).
 *
 * The communication flow:
 *   1. Allocate work buffer (GSmem_Alloc, 0x44A0 bytes, 32-byte aligned)
 *   2. Get usable pointer (GSmem_GetPtr)
 *   3. Clear buffer (memset, 0x4490 bytes)
 *   4. Store channel pointer in lbl_803FB328[channel]
 *   5. Register SI callback (fn_80093B04)
 *   6. Store port number at offset 0x4338
 *   7. Register with GSthread (fn_800716C8)
 *   8. Initialize SI state (fn_8009F77C, fn_8009F9C8)
 *   9. Create SI transfer thread (OSCreateThread):
 *      - Entry: fn_800937F4 (gbaCommunication_ThreadEntry)
 *      - Stack: at work + 0x20
 *      - Size: 0x4000
 *      - Priority: 8
 *   10. Resume thread (OSResumeThread)
 *   11. Begin transfer based on phase
 *
 * Related source files:
 *   menuGBAC.c      - GBA communication menu (lbl_8026F488: "menuGBAC.c")
 *   colbtl.bin       - Link battle communication binary
 *   bg0thumbcode.bin - GBA thumb code for background 0
 *   bg1thumbcode.bin - GBA thumb code for background 1
 *   bg2thumbcode.bin - GBA thumb code for background 2
 *
 * The "Ribbon Index: %d" string at lbl_8026F5E4 is used during
 * ribbon data transfer, logged via OSReport during debugging.
 *
 * Transfer data table at lbl_8026F5F8 (rodata, 6 entries of 4 bytes each):
 *   Points to data blocks in .data section (lbl_80311D38 through lbl_80311FEC)
 *   These likely define the transfer protocol configuration for
 *   different transfer modes (trade, battle, bonus disc).
 */

#include "dolphin/types.h"
#include "game/gba/gba.h"

/* =========================================================================
 * External declarations
 * ========================================================================= */

extern void* memset(void* dst, int val, u32 size);

/* GSmem allocator (fn_800E2C04, fn_800E27B0) */
extern void* fn_800E2C04(u32 size, u32 align);
extern void* fn_800E27B0(void* handle);

/* GSthread registration (fn_800716C8) */
extern void fn_800716C8(s32 channel, void* threadData);

/* SI functions */
extern void fn_8009F77C(void* ctx);       /* SI_Setup */
extern void fn_8009F9C8(void* cbData);    /* SI_SetupCallback */
extern void fn_800A19CC(void* thread, void* func, void* arg,
                         void* stackBase, u32 stackSize,
                         u32 priority, u32 detached); /* OSCreateThread */
extern void fn_800A1F94(void* thread);    /* OSResumeThread */
extern void fn_8009F7B4(void* ctx);       /* SI_BeginTransfer */
extern void fn_8009F890(void* ctx);       /* SI_EndTransfer */
extern void fn_800A257C(void* thread, u32 prio); /* SI_SetPriority */
extern void fn_8009FABC(void* cbData);    /* SI_TriggerCallback */

/* Assert function (__assert) */
extern void __assert(const char* file, u32 line, const char* msg);

/* Pokemon conversion (pokeconv.c) */
extern s32 fn_80089048(void* pDst, void* pSrc, void* pSaveCtx);

/* Thread entry and SI callback (forward declarations) */
extern void fn_800937F4(void);  /* gbaCommunication_ThreadEntry */
extern void fn_80093B04(void);  /* gbaCommunication_SICallback */

/* =========================================================================
 * File-scope string
 * ========================================================================= */

static const char sFile[] = "gbaCommunication.c";

/* SDA assert string reference */
extern const char lbl_8047C1E8[];  /* NULL allocation assert */

/* =========================================================================
 * Global state (BSS)
 * ========================================================================= */

/**
 * Channel pointer array at lbl_803FB328.
 * Holds pointers to the 0x44A0-byte work buffers for each of the
 * 4 controller ports. NULL if the channel is not active.
 */
extern void* lbl_803FB328[GBA_CHANNEL_COUNT]; /* u32*[4] */

/* =========================================================================
 * fn_80092C90: gbaCommunication_Transfer1
 * Address: 0x80092C90, Size: 0x1A8
 *
 * Initialize GBA communication and send a Pokemon.
 *
 * Opens a channel, allocates work buffer, creates SI thread, converts
 * the Pokemon data from GBA to GCN format, and starts the transfer
 * with phase GBA_PHASE_SEND_POKEMON (0x0C).
 *
 * Decompiled from PPC disassembly.
 * ========================================================================= */
s32 gbaCommunication_Transfer1(s32 channel, void* pSrc, void* pSaveCtx) {
    void* workHandle;
    u8*   work;
    u8*   channelWork;
    u32   slotOffset;
    s32   result;
    s32   convertResult;

    /* Validate channel range (0-3) */
    if (channel < GBA_CHANNEL_MIN || channel > GBA_CHANNEL_MAX) {
        return 0;
    }

    /* Check if channel is already allocated */
    slotOffset = (u32)channel << 2; /* channel * 4 for array index */
    if (lbl_803FB328[channel] != NULL) {
        result = 1;
    } else {

    /* Allocate work buffer: 0x44A0 bytes, 32-byte aligned */
    workHandle = fn_800E2C04(GBA_WORK_SIZE, 0x20);

    /* Assert: allocation succeeded (line 0x1DD) */
    if (((u32)workHandle & 0xFFFF) == 0) {
        __assert(sFile, 0x1DD, lbl_8047C1E8);
    }

    /* Get usable pointer from handle */
    work = (u8*)fn_800E27B0(workHandle);

    /* Clear work buffer */
    memset(work, 0, GBA_WORK_CLEAR_SIZE);

    /* Store channel pointer */
    lbl_803FB328[channel] = work;

    /* Register SI callback */
    {
        u8* channelPtr = (u8*)lbl_803FB328[channel];

        /* Clear phase */
        *(u32*)(channelPtr + GBA_STATE_PHASE) = 0;

        /* Store port number */
        *(u32*)(channelPtr + GBA_STATE_PORT) = (u32)channel;

        /* Register with GSthread */
        fn_800716C8(channel, channelPtr + GBA_DATA_OFFSET);

        /* Setup SI state */
        fn_8009F77C(channelPtr);

        /* Setup SI callback data */
        fn_8009F9C8(channelPtr + 0x18);

        /* Create SI transfer thread */
        fn_800A19CC(
            channelPtr + GBA_DATA_OFFSET,   /* thread data */
            (void*)fn_800937F4,              /* entry: threadEntry */
            channelPtr,                       /* arg: work buffer */
            channelPtr + GBA_STATE_PORT,     /* stack info */
            GBA_TRANSFER_SIZE,               /* stack size: 0x4000 */
            GBA_SI_PRIORITY,                 /* priority: 8 */
            0                                 /* not detached */
        );

        /* Resume the thread */
        fn_800A1F94(channelPtr + GBA_DATA_OFFSET);

        result = 1;
    }
    }
    if (result == 0) {
        return 0;
    }

    /* Phase 2: Begin transfer */
    {
        u8* activeWork = (u8*)lbl_803FB328[channel];
        convertResult = 0;

        /* Begin SI transfer */
        fn_8009F7B4(activeWork);

        /* Check if ready for Pokemon conversion */
        if (*(u32*)(activeWork + GBA_STATE_PHASE) == 0) {
            /* Convert Pokemon from GBA to GCN format */
            convertResult = fn_80089048(
                activeWork + GBA_POKEMON_OFFSET,  /* dst: work + 0x4344 */
                pSrc,                              /* src: GBA Pokemon data */
                pSaveCtx                           /* save context */
            );

            if (convertResult != 0) {
                /* Set phase to send Pokemon */
                *(u32*)(activeWork + GBA_STATE_PHASE) = GBA_PHASE_SEND_POKEMON;

                /* Set timeout */
                *(u32*)(activeWork + GBA_STATE_TIMEOUT) = GBA_TIMEOUT_SEND;
            }
        }

        /* End SI transfer */
        fn_8009F890(activeWork);

        /* Set thread priority */
        fn_800A257C(activeWork + GBA_DATA_OFFSET, GBA_SI_PRIORITY);

        /* If conversion succeeded, trigger callback */
        if (convertResult != 0) {
            fn_8009FABC(activeWork + 0x18);
        }

        return convertResult;
    }
}

/* =========================================================================
 * fn_80092E38: gbaCommunication_Transfer2
 * Address: 0x80092E38, Size: 0x190
 *
 * Initialize GBA communication and receive a Pokemon.
 * Same structure as Transfer1, but sets phase to
 * GBA_PHASE_RECV_POKEMON (0x0B) and stores the destination buffer.
 *
 * Decompiled from PPC disassembly.
 * ========================================================================= */
s32 gbaCommunication_Transfer2(s32 channel, void* pDst) {
    void* workHandle;
    u8*   work;
    u32   result;

    /* Validate channel range */
    if (channel < GBA_CHANNEL_MIN || channel > GBA_CHANNEL_MAX) {
        return 0;
    }

    /* Check if channel already allocated */
    if (lbl_803FB328[channel] != NULL) {
        result = 1;
    } else {
        /* Allocate work buffer */
        workHandle = fn_800E2C04(GBA_WORK_SIZE, 0x20);

        if (((u32)workHandle & 0xFFFF) == 0) {
            __assert(sFile, 0x1DD, lbl_8047C1E8);
        }

        work = (u8*)fn_800E27B0(workHandle);
        memset(work, 0, GBA_WORK_CLEAR_SIZE);

        lbl_803FB328[channel] = work;

        {
            u8* channelPtr = (u8*)lbl_803FB328[channel];

            *(u32*)(channelPtr + GBA_STATE_PHASE) = 0;
            *(u32*)(channelPtr + GBA_STATE_PORT) = (u32)channel;

            fn_800716C8(channel, channelPtr + GBA_DATA_OFFSET);
            fn_8009F77C(channelPtr);
            fn_8009F9C8(channelPtr + 0x18);

            fn_800A19CC(
                channelPtr + GBA_DATA_OFFSET,
                (void*)fn_800937F4,
                channelPtr,
                channelPtr + GBA_STATE_PORT,
                GBA_TRANSFER_SIZE,
                GBA_SI_PRIORITY,
                0
            );

            fn_800A1F94(channelPtr + GBA_DATA_OFFSET);
            result = 1;
        }
    }
    if (result == 0) {
        return 0;
    }

    {
        u8* activeWork = (u8*)lbl_803FB328[channel];
        s32 started = 0;

        fn_8009F7B4(activeWork);

        if (*(u32*)(activeWork + GBA_STATE_PHASE) == 0) {
            /* Set phase to receive */
            *(u32*)(activeWork + GBA_STATE_PHASE) = GBA_PHASE_RECV_POKEMON;

            /* Set timeout */
            *(u32*)(activeWork + GBA_STATE_TIMEOUT) = GBA_TIMEOUT_RECV;

            started = 1;

            /* Store destination buffer pointer */
            *(u32*)(activeWork + GBA_STATE_DATA) = (u32)pDst;
        }

        fn_8009F890(activeWork);

        fn_800A257C(activeWork + GBA_DATA_OFFSET, GBA_SI_PRIORITY);

        if (started != 0) {
            fn_8009FABC(activeWork + 0x18);
        }

        return started;
    }
}

/* =========================================================================
 * fn_80092FC8: gbaCommunication_Transfer3
 * Address: 0x80092FC8, Size: 0x198
 *
 * Transfer Pokemon data with source and save context.
 * Structurally identical to Transfer1, follows the same allocation
 * and thread setup pattern. The specific difference is in the
 * transfer phase and timeout configuration.
 *
 * Pattern match from disassembly confirms identical structure to
 * Transfer1 except for different state values stored at phase/timeout.
 * ========================================================================= */
/* s32 gbaCommunication_Transfer3(s32 channel, void* pSrc, void* pSaveCtx) */
/* Implementation follows same pattern as Transfer1 */

/* =========================================================================
 * fn_80093160: gbaCommunication_Transfer4
 * Address: 0x80093160, Size: 0x190
 *
 * Another transfer variant. Same allocation/thread pattern.
 * ========================================================================= */
/* s32 gbaCommunication_Transfer4(s32 channel, void* pSrc, void* pSaveCtx) */

/* =========================================================================
 * fn_800932F0: gbaCommunication_Transfer5
 * Address: 0x800932F0, Size: 0x1F4
 *
 * Complex transfer with validation. Slightly larger than other
 * transfer functions due to additional validation logic.
 * ========================================================================= */
/* s32 gbaCommunication_Transfer5(s32 channel, void* pSrc, void* pSaveCtx) */

/* =========================================================================
 * fn_80093698: gbaCommunication_Transfer6
 * Address: 0x80093698, Size: 0x15C
 *
 * Finalize transfer and log ribbon data.
 * Uses OSReport with "Ribbon Index: %d" (lbl_8026F5E4) during
 * ribbon data extraction.
 * ========================================================================= */
/* s32 gbaCommunication_Transfer6(s32 channel, void* pDst) */

/* =========================================================================
 * fn_80093B04: gbaCommunication_SICallback
 * fn_800937F4: gbaCommunication_ThreadEntry
 *
 * These are the SI callback and thread entry registered during
 * channel setup. They implement the low-level SI protocol:
 *
 * The thread entry (fn_800937F4) runs a state machine that:
 *   1. Waits for the GBA to be ready (handshake)
 *   2. Sends/receives data in blocks
 *   3. Handles errors and timeouts
 *   4. Signals completion via the callback system
 *
 * The SI callback (fn_80093B04) is called by the SI hardware
 * when a transfer completes or an error occurs.
 * ========================================================================= */
