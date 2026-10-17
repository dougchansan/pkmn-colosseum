#ifndef GAME_GBA_H
#define GAME_GBA_H

#include "dolphin/types.h"

/**
 * @file gba.h
 * @brief GBA Link Cable communication and Pokemon data conversion API.
 *
 * Pokemon Colosseum communicates with GBA Pokemon games (Ruby, Sapphire,
 * FireRed, LeafGreen, Emerald) via the GCN-GBA Link Cable using the
 * SI (Serial Interface) peripheral. The communication protocol transfers
 * Pokemon data between the GBA and GCN formats.
 *
 * Source files:
 *   gbaCommunication.c (0x80092C90 - 0x800937F4, 6+ functions)
 *   pokeconv.c          (0x80089048 - 0x800895A4, 2 functions)
 *
 * The GBA communication uses a 0x44A0-byte work buffer per channel,
 * allocated via GSmem. Up to 4 simultaneous connections are supported
 * (channels 0-3, one per controller port).
 *
 * Key data flow:
 *   1. Channel is opened (allocates work buffer, creates SI thread)
 *   2. Pokemon data is converted between GBA and GCN formats (pokeconv)
 *   3. Data is transferred over SI in 0x4000-byte blocks
 *   4. Ribbon data is logged ("Ribbon Index: %d")
 *   5. Channel is closed (frees work buffer)
 *
 * The bonus disc (Pokemon Colosseum Bonus Disc) uses this system to
 * trade Jirachi to GBA games. Menu code references GBA thumb code
 * binaries: bg0thumbcode.bin, bg1thumbcode.bin, bg2thumbcode.bin
 *
 * The game also loads colbtl.bin for link battle communication.
 *
 * Related FSYS archives:
 *   menuGBAC.c - GBA communication menu
 *   menuExDiscShrine.c - Bonus disc shrine (Celebi/Jirachi distribution)
 *   menuExDiscCoupon.c - Bonus disc coupon exchange
 */

/* =========================================================================
 * Constants
 * ========================================================================= */

/* GBA communication channels (one per controller port) */
#define GBA_CHANNEL_MIN     0
#define GBA_CHANNEL_MAX     3
#define GBA_CHANNEL_COUNT   4

/* GBA communication work buffer sizes */
#define GBA_WORK_SIZE       0x44A0     /* Total work buffer per channel */
#define GBA_WORK_CLEAR_SIZE 0x4490     /* Size to clear (minus header) */
#define GBA_DATA_OFFSET     0x20       /* Offset to SI thread data */
#define GBA_TRANSFER_SIZE   0x4000     /* SI transfer block size */
#define GBA_POKEMON_OFFSET  0x4344     /* Offset to Pokemon data in work buffer */

/* GBA work buffer state offsets */
#define GBA_STATE_PORT      0x4338     /* Port/channel number (u32) */
#define GBA_STATE_TIMEOUT   0x433C     /* Timeout counter (u32) */
#define GBA_STATE_PHASE     0x4340     /* Communication phase (u32) */
#define GBA_STATE_DATA      0x4344     /* Start of Pokemon data */

/* Communication phases */
#define GBA_PHASE_IDLE           0
#define GBA_PHASE_SEND_POKEMON   0x0C  /* Phase for sending Pokemon to GBA */
#define GBA_PHASE_RECV_POKEMON   0x0B  /* Phase for receiving Pokemon from GBA */

/* Timeout values */
#define GBA_TIMEOUT_SEND    0x0003000C /* Timeout for send operations */
#define GBA_TIMEOUT_RECV    0x0003000B /* Timeout for receive operations */

/* GBA Pokemon data format (from pokeconv.c) */
#define GBA_POKEMON_DATA_SIZE  12      /* Size of GBA Pokemon substructure data */
#define GBA_PID_SIZE            4      /* Personality value size */

/* PC item count validation (from pokeconv.c assert) */
#define GBA_PC_ITEMS_50        50      /* Ruby/Sapphire PC item count */
#define GBA_PC_ITEMS_30        30      /* FireRed/LeafGreen PC item count */

/* Ribbon data */
#define GBA_RIBBON_COUNT       12      /* Number of ribbon types to transfer */

/* SI thread configuration */
#define GBA_SI_PRIORITY        0x8     /* SI thread priority */

/* =========================================================================
 * GBA Pokemon data structure (as stored in GBA games)
 * ========================================================================= */

/**
 * GBA-format Pokemon data.
 * The GBA stores Pokemon in a 100-byte structure with 4 encrypted
 * 12-byte substructures. The order of substructures depends on the
 * personality value (PID % 24).
 *
 * The conversion functions (pokeconv.c) handle byte-swapping between
 * GBA little-endian and GCN big-endian formats.
 */
typedef struct GBAPokemonData {
    /* 0x00 */ u32  pid;                 /* Personality value */
    /* 0x04 */ u32  otID;                /* Original Trainer ID (TID | SID << 16) */
    /* 0x08 */ u8   nickname[10];        /* Nickname (GBA encoding) */
    /* 0x12 */ u16  pcItemsNum;          /* PC items count (used for validation) */
    /* 0x14 */ u8   subdata[48];         /* 4 encrypted substructures (12 bytes each) */
} GBAPokemonData;

/**
 * Converted Pokemon data (GCN format, intermediate representation).
 * Used during GBA<->GCN data transfer at work buffer offset 0x4344.
 * The first 0x10 bytes contain the core ID data (PID, OTID, etc.)
 * followed by converted substructure data.
 */
typedef struct ConvertedPokemon {
    /* 0x00 */ u32  data0;               /* Byte-swapped PID components */
    /* 0x04 */ u32  data1;               /* Byte-swapped OT ID components */
    /* 0x08 */ u32  data2;               /* Byte-swapped data field */
    /* 0x0C */ u32  data3;               /* Byte-swapped data field + item count */
    /* 0x10 */ u8   convertedData[256];  /* Converted substructure data */
} ConvertedPokemon;

/* =========================================================================
 * GBA communication state
 * ========================================================================= */

/**
 * Per-channel GBA communication state.
 * Allocated as a 0x44A0-byte block from GSmem. The block layout:
 *   0x0000 - 0x001F : Channel header
 *   0x0020 - 0x4337 : SI thread and transfer buffer
 *   0x4338 - 0x4343 : State variables (port, timeout, phase)
 *   0x4344 - 0x449F : Pokemon data buffer
 *
 * Stored in BSS at lbl_803FB328 (array of 4 pointers, one per channel).
 */

/* =========================================================================
 * Function declarations - Pokemon conversion (pokeconv.c)
 * ========================================================================= */

/**
 * fn_80089048: Convert a Pokemon from GBA format to GCN format.
 * Performs byte-swapping (GBA is little-endian, GCN is big-endian) on
 * the PID, OT ID, and substructure data. If a target context is provided,
 * also performs species validation and ribbon data extraction.
 *
 * The byte-swapping is performed by extracting each byte of a 32-bit
 * word and reassembling in reversed order:
 *   GBA: [b0][b1][b2][b3] -> GCN: [b3][b2][b1][b0]
 *
 * After conversion, validates the PC item count:
 *   assert("cp->pc_items_num == 50 || cp->pc_items_num == 30")
 *   (50 = Ruby/Sapphire, 30 = FireRed/LeafGreen)
 *
 * If the item count is valid, converts the remaining data items
 * in batches of 4 (unrolled loop), performing the same byte-swap
 * on each 32-bit word from the GBA source.
 *
 * When a save context (param 3) is provided, calls pokeconvPokemonToGBA to
 * extract additional data, then fills 11 ribbon bytes via exribbonGetNo.
 *
 * @param pDst     Destination buffer (GCN format, at work+0x4344)
 * @param pSrc     Source GBA Pokemon data
 * @param pSaveCtx Save context for ribbon extraction (may be NULL)
 * @return 1 on success, 0 if validation fails
 */
s32 pokeconv_ConvertPokemon(void* pDst, void* pSrc, void* pSaveCtx);

/**
 * fn_80089380: Convert a Pokemon from GCN format back to GBA format.
 * Reverse of pokeconv_ConvertPokemon. Performs big-endian to
 * little-endian byte-swapping and validates the PC item count.
 *
 * assert("cp->pc_items_num == 50 || cp->pc_items_num == 30")
 *   at line 0x6F in pokeconv.c
 *
 * After validation, converts data in batches and writes back to
 * the GBA-format destination buffer.
 *
 * @param pDst  Destination buffer (GBA format)
 * @param pSrc  Source GCN-format Pokemon data
 */
void pokeconv_ValidateItems(void* pDst, void* pSrc);

/* =========================================================================
 * Function declarations - GBA communication (gbaCommunication.c)
 * ========================================================================= */

/**
 * gbaCommStartSendPokemon: Initialize GBA communication and send a Pokemon.
 * Opens a channel (0-3), allocates a 0x44A0-byte work buffer via
 * GSmem, clears it, creates an SI transfer thread, and converts
 * the source Pokemon data from GBA to GCN format using
 * pokeconv_ConvertPokemon.
 *
 * Sets communication phase to GBA_PHASE_SEND_POKEMON (0x0C).
 *
 * @param channel  SI channel (0-3)
 * @param pSrc     Source GBA Pokemon data
 * @param pSaveCtx Save context for ribbon data
 * @return 1 on success, 0 on failure
 */
s32 gbaCommunication_Transfer1(s32 channel, void* pSrc, void* pSaveCtx);

/**
 * gbaCommStartTransferA: Initialize GBA communication and receive a Pokemon.
 * Same setup as Transfer1, but sets phase to GBA_PHASE_RECV_POKEMON (0x0B)
 * and stores the destination buffer pointer at state offset 0x4344.
 *
 * @param channel  SI channel (0-3)
 * @param pDst     Destination buffer for received Pokemon data
 * @return 1 on success, 0 on failure
 */
s32 gbaCommunication_Transfer2(s32 channel, void* pDst);

/**
 * gbaCommStartTransferB: Transfer Pokemon data with source and save context.
 * Similar to Transfer1, combines send operation with additional
 * save context handling.
 *
 * @param channel  SI channel (0-3)
 * @param pSrc     Source Pokemon data
 * @param pSaveCtx Save context
 * @return 1 on success, 0 on failure
 */
s32 gbaCommunication_Transfer3(s32 channel, void* pSrc, void* pSaveCtx);

/**
 * gbaCommStartTransferC: Transfer Pokemon data with different phase setup.
 * Variant transfer function for specific trade scenarios.
 *
 * @param channel  SI channel (0-3)
 * @param pSrc     Source Pokemon data
 * @param pSaveCtx Save context
 * @return 1 on success, 0 on failure
 */
s32 gbaCommunication_Transfer4(s32 channel, void* pSrc, void* pSaveCtx);

/**
 * gbaCommStartTransferFile: Complex transfer with validation.
 * Extended transfer function that performs additional data
 * validation and handles multi-block transfers.
 *
 * @param channel  SI channel (0-3)
 * @param pSrc     Source Pokemon data
 * @param pSaveCtx Save context
 * @return 1 on success, 0 on failure
 */
s32 gbaCommunication_Transfer5(s32 channel, void* pSrc, void* pSaveCtx);

/**
 * gbaCommEnd: Finalize GBA transfer and print ribbon data.
 * Completes the data transfer, logs ribbon indices using
 * OSReport("Ribbon Index: %d"), and cleans up the channel.
 *
 * @param channel  SI channel (0-3)
 * @param pDst     Destination for results
 * @return 1 on success, 0 on failure
 */
s32 gbaCommunication_Transfer6(s32 channel, void* pDst);

/**
 * fn_80093B04: GBA SI callback.
 * Called by the SI system when a transfer completes or errors.
 * Registered during channel setup in Transfer1/Transfer2.
 */
void gbaCommunication_SICallback(void);

/**
 * _gbaCommThread: GBA communication thread entry.
 * The main processing thread for GBA link cable communication.
 * Runs the state machine that manages the data transfer protocol.
 * Created by OSCreateThread during channel initialization.
 */
void gbaCommunication_ThreadEntry(void);

/* =========================================================================
 * GBA communication init functions (called from main.c)
 * ========================================================================= */

/**
 * fn_801ED740: Initialize GBA communication system.
 * Called from GameInit(). Sets up the global GBA comm state.
 */
void gba_SystemInit(void);

/**
 * fn_801ED388: Initialize GBA link subsystem.
 * Called from GameMainLoop(). Prepares for GBA connections.
 */
void gba_LinkInit(void);

/* =========================================================================
 * External references used by GBA communication
 * ========================================================================= */

/* GSmem allocator */
/* GSmemAlloc */ extern void* GSmem_Alloc(u32 size, u32 align);
/* GSmemLock */ extern void* GSmem_GetPtr(void* handle);

/* SI / Thread functions */
/* fn_800716C8 */ extern void  GSthread_Register(s32 channel, void* threadData);
/* OSInitMutex */ extern void  SI_Setup(void* ctx);
/* OSInitCond */ extern void  SI_SetupCallback(void* callbackData);
/* OSCreateThread */ extern void  OSCreateThread(void* thread, void* func,
                                              void* arg, void* stackBase,
                                              u32 stackSize, u32 priority,
                                              u32 detached);
/* OSResumeThread */ extern void  OSResumeThread(void* thread);
/* OSLockMutex */ extern void  SI_BeginTransfer(void* ctx);
/* OSUnlockMutex */ extern void  SI_EndTransfer(void* ctx);
/* OSSetThreadPriority */ extern void  SI_SetPriority(void* thread, u32 priority);
/* OSSignalCond */ extern void  SI_TriggerCallback(void* callbackData);

/* Assert function */
/* __assert */ extern void  GS_Assert(const char* file, u32 line, const char* msg);

/* Pokemon validation */
/* pokemonCheckValid */ extern u8    pokemon_IsValid(void* pSaveCtx);
/* pokemonBiosGetPokemonDataId */ extern u16   pokemon_GetSpecies(void* pSaveCtx);

/* Data extraction */
/* pokeconvPokemonToGBA */ extern void  pokemon_ExtractData(void* pSaveCtx, void* pDst);
/* exribbonGetNo */ extern u8    ribbon_GetValue(s32 ribbonIdx);

#endif /* GAME_GBA_H */
