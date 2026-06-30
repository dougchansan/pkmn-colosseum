#include "dolphin/os/OS.h"
#include "dolphin/os/OSCache.h"
#include "dolphin/os/OSInterrupt.h"
#include "dolphin/db/DB.h"

extern void* memset(void* dest, int val, u32 n);
extern void* memcpy(void* dest, const void* src, u32 n);

/*
 * OSException.c - Exception vector installation and handlers.
 *
 * Installs exception vectors into the PowerPC exception table,
 * sets up the default exception handlers, and provides
 * __OSSetExceptionHandler / __OSGetExceptionHandler.
 *
 * Matches: 0x80099E1C - 0x8009A1E8
 */

/* The exception handler table pointer - stored at 0x80003000 */
static __OSExceptionHandler* OSExceptionTable;

/* External symbols for exception vector code */
extern u8  __OSEVStart[];
extern u8  __OSEVEnd[];
extern u32 __OSEVSetNumber[];
extern u8  __OSDBINTSTART[];
extern u8  __OSDBINTEND[];
extern u8  __OSDBJUMPEND[];
extern u32 __DBVECTOR[];

/* Exception vector offsets (indexed by exception number) */
static const u32 ExceptionVectorAddr[] = {
    0x00000100, /* System Reset */
    0x00000200, /* Machine Check */
    0x00000300, /* DSI */
    0x00000400, /* ISI */
    0x00000500, /* External Interrupt */
    0x00000600, /* Alignment */
    0x00000700, /* Program */
    0x00000800, /* Floating Point Unavailable */
    0x00000900, /* Decrementer */
    0x00000C00, /* System Call */
    0x00000D00, /* Trace */
    0x00000F00, /* Performance Monitor */
    0x00001300, /* IABR */
    0x00001400, /* SMI */
    0x00001700, /* Thermal */
};

void OSExceptionInit(void) {
    u32  exception;
    u32  vectorSize;
    u32  dbIntSize;
    u32  dbJumpSize;
    u32  savedSetNumber;
    u8*  vectorStart = __OSEVStart;
    u8*  vectorEnd   = __OSEVEnd;
    u8*  dbIntStart  = __OSDBINTSTART;
    u8*  dbIntEnd    = __OSDBINTEND;
    u8*  dbJumpEnd   = __OSDBJUMPEND;

    vectorSize  = (u32)(vectorEnd - vectorStart);
    savedSetNumber = *__OSEVSetNumber;
    dbIntSize   = (u32)(dbIntEnd - dbIntStart);
    dbJumpSize  = (u32)(dbJumpEnd - dbIntEnd);

    /* If the debug integrator is not installed, install it */
    if (*(volatile u32*)0x60 == 0) {
        DBPrintf("Installing DB integrator\n");
        memcpy((void*)0x60, dbIntStart, dbIntSize);
        DCFlushRangeNoSync((void*)0x60, dbIntSize);
        asm { sync }
        ICInvalidateRange((void*)0x60, dbIntSize);
    }

    /* Install each exception vector */
    for (exception = 0; exception < OS_EXCEPTION_MAX; exception++) {
        /* Check if this exception is marked for debugging */
        if (__DBIsExceptionMarked(exception) && *(volatile u32*)0x800000F4 != 0) {
            u32* debugFlag = (u32*)*(volatile u32*)0x800000F4;
            if (*debugFlag >= 2) {
                DBPrintf(">>> Exception %d is being debugged\n", exception);
                continue;
            }
        }

        /* Set exception number in the vector template */
        *__OSEVSetNumber = savedSetNumber | (u32)exception;

        /* Check if debug handler should be installed at this vector */
        if (__DBIsExceptionMarked(exception) && *(volatile u32*)0x800000F4 != 0) {
            u32* debugFlag = (u32*)*(volatile u32*)0x800000F4;
            if (*debugFlag >= 2) {
                DBPrintf(">>> Installing debug handler at vector 0x%x\n", exception);
                /* Copy the debug jump code */
                memcpy(__DBVECTOR, dbIntEnd, dbJumpSize);
            } else {
                /* Fill vector location with nops then copy the vector code */
                u32* dest = (u32*)__DBVECTOR;
                u32  numWords = (dbJumpSize + 3) / 4;
                u32  i;
                for (i = 0; i < numWords; i++) {
                    *dest++ = 0x60000000;  /* nop */
                }
            }
        } else {
            /* Fill vector location with nops then copy the vector code */
            u32* dest = (u32*)__DBVECTOR;
            u32  numWords = (dbJumpSize + 3) / 4;
            u32  i;
            for (i = 0; i < numWords; i++) {
                *dest++ = 0x60000000;  /* nop */
            }
        }

        /* Copy vector code to exception address */
        {
            void* destAddr = (void*)(ExceptionVectorAddr[exception] + 0x80000000);
            memcpy(destAddr, vectorStart, vectorSize);
            DCFlushRangeNoSync(destAddr, vectorSize);
            asm { sync }
            ICInvalidateRange(destAddr, vectorSize);
        }
    }

    /* Set up the exception handler table at 0x80003000 */
    OSExceptionTable = (__OSExceptionHandler*)0x80003000;

    /* Install default handlers for all exceptions */
    for (exception = 0; exception < OS_EXCEPTION_MAX; exception++) {
        __OSSetExceptionHandler(exception, OSDefaultExceptionHandler);
    }

    /* Restore the original set number */
    *__OSEVSetNumber = savedSetNumber;
    DBPrintf("Exceptions initialized...\n");
}

__OSExceptionHandler __OSSetExceptionHandler(__OSException exception, __OSExceptionHandler handler) {
    __OSExceptionHandler old;

    old = OSExceptionTable[exception];
    OSExceptionTable[exception] = handler;
    return old;
}

__OSExceptionHandler __OSGetExceptionHandler(__OSException exception) {
    return OSExceptionTable[exception];
}

/* OSDefaultExceptionHandler - asm stub that saves remaining context and jumps
   to __OSUnhandledException */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
asm void OSDefaultExceptionHandler(register __OSException exception, register OSContext* context, register u32 dsisr, register u32 dar) {
    nofralloc
    stw     r0,   0x0000(r4)
    stw     r1,   0x0004(r4)
    stw     r2,   0x0008(r4)
    stmw    r6,   0x0018(r4)
    mfspr   r0, GQR1
    stw     r0, 0x01A8(r4)
    mfspr   r0, GQR2
    stw     r0, 0x01AC(r4)
    mfspr   r0, GQR3
    stw     r0, 0x01B0(r4)
    mfspr   r0, GQR4
    stw     r0, 0x01B4(r4)
    mfspr   r0, GQR5
    stw     r0, 0x01B8(r4)
    mfspr   r0, GQR6
    stw     r0, 0x01BC(r4)
    mfspr   r0, GQR7
    stw     r0, 0x01C0(r4)
    mfdsisr r5
    mfdar   r6
    stwu    r1, -0x0008(r1)
    b       __OSUnhandledException
}
#pragma pop

/* ===================================================================
 * Stub functions for coverage -- TODO: decompile
 * 1 function(s)
 * =================================================================== */

/* fn_8009A0C0 - 0x8009A0C0 | size: 0x4 */
#if 1
asm void fn_8009A0C0(void) {
#include "src/dolphin/os/OSException_fn_8009A0C0.inc"
}
#else
void fn_8009A0C0(void) {
    /* bla 0x60 */;
}
#endif

/* 0x8009A09C | 0x24 */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void __OSDBIntegrator(void) {
#include "src/dolphin/os/OSException___OSDBIntegrator.inc"
}
#else
void __OSDBIntegrator(void) {
    /* TODO: match -- 36 bytes at 0x8009A09C */
}
#endif
#pragma pop

/* 0x8009A0F4 | 0x9C */
#pragma push
#pragma optimization_level 0
#pragma optimizewithasm off
#if 1
asm void OSExceptionVector(void) {
#include "src/dolphin/os/OSException_OSExceptionVector.inc"
}
#else
void OSExceptionVector(void) {
    /* TODO: match -- 156 bytes at 0x8009A0F4 */
}
#endif
#pragma pop
