
/* Forward declarations for converted functions */

/**
 * @file hsd_render.c
 * @brief HSD internal functions (0x80197344-0x80197A64).
 *
 * Stub coverage for 7 functions.
 */

#include "dolphin/types.h"
#include "hsd/hsd_debug.h"

u32 fn_80197650(u32, s32, u32);

/* 0x80197344 | 0xBC */
void fn_80197344(u32 obj) {
#pragma optimization_level 1
    extern void (*lbl_8047B240)(u32, u32, u32, u32);
    extern void fn_80197784(u32);
    u32 base;
    u32 node;
    u32 flags;
    u32 flags2;

    base = obj;
    if (base == 0) {
        return;
    }
    if (((u32)__cntlzw(*(u32*)(base + 0x14) & 0x4020) >> 5) != 0) {
        fn_80197784(base);
        return;
    }
    if ((*(u32*)(base + 0x14) & 0x20) == 0) {
        return;
    }
    if (lbl_8047B240 == 0) {
        return;
    }

    node = *(u32*)(base + 0x18);
    while (node != 0) {
        flags = *(u32*)(node + 4);
        if ((flags & 0x80000000) != 0) {
            flags = *(u32*)(node + 4);
            flags2 = *(u32*)(node + 4);
            lbl_8047B240(0, flags2 & 0x3F, (flags >> 6) & 0x00FFFFFF, base);
        }
        *(u32*)(node + 4) = *(u32*)(node + 4) & 0x7FFFFFFF;
        node = *(u32*)node;
    }
}

/* 0x80197400 | 0xA8 */
void fn_80197400(void) {
    extern u8 lbl_80465348[];
    extern u32 lbl_80478C64;
    extern u32 lbl_80478C68;
    extern u32 lbl_80478C6C;
    extern u8 lbl_8047B24C[4];
    extern u8 lbl_8047B250[4];
    extern u32 lbl_8047B254;
    extern u8 lbl_8047B258[4];
    extern u32 lbl_8047B25C;
    extern void HSD_MtxFree();
    extern void HSD_ObjFree();
    u8 sp[0x10];
    u32 tmp = 0;
    u32 r3 = 0;
    u32 r4 = 0;
    u32 r5 = 0;
    u32 r6 = 0;
    u32 r7 = 0;
    u32 r8 = 0;
    u32 r9 = 0;
    u32 r30 = 0;
    u32 r31 = 0;

    r31 = *(u32*)lbl_8047B24C;
    while (r31 != 0) {

        tmp = *(u32*)((u8*)r31 + 0x30);
        r30 = *(u32*)((u8*)r31 + 0x44);
        if (tmp != 0) {
            r3 = *(u32*)((u8*)r31 + 0x30);
            HSD_MtxFree();
        }
        r3 = (u32)lbl_80465348;
        r4 = r31;
        r3 = (u32)lbl_80465348;
        HSD_ObjFree();
        r31 = r30;

    }
    r9 = 0x0;
    r8 = (u32)lbl_8047B24C;
    r7 = 0x0;
    r6 = (u32)lbl_8047B250;
    r5 = 0x0;
    r4 = 0x0;
    r3 = (u32)lbl_8047B258;
    tmp = 0x0;
    *(u32*)lbl_8047B24C = r9;
    lbl_80478C64 = r8;
    *(u32*)lbl_8047B250 = r7;
    lbl_80478C68 = r6;
    lbl_8047B254 = r5;
    *(u32*)lbl_8047B258 = r4;
    lbl_80478C6C = r3;
    lbl_8047B25C = tmp;
    return;
}

/* 0x801974A8 | 0x154 */
void fn_801974A8(void) {
    extern u8 lbl_80465348[];
    extern u32 lbl_80478C64;
    extern u32 lbl_80478C68;
    extern u32 lbl_80478C6C;
    extern u32 lbl_8047B24C;
    extern u32 lbl_8047B250;
    extern u32 lbl_8047B254;
    extern u32 lbl_8047B258;
    extern u32 lbl_8047B25C;
    extern u32 HSD_CObjGetCurrent(void);
    extern void HSD_MtxFree(u32);
    extern void HSD_ObjFree(void*, u32);
    u32 cobj;
    u32 mtxBase;
    u32 defaultMtx;
    u32 entry;

    cobj = HSD_CObjGetCurrent();
    mtxBase = cobj + 0x54;
    entry = lbl_8047B250;
    defaultMtx = mtxBase;
    while (entry != 0) {
        u32 obj;
        u32 mtx;
        u32 check;
        u32 entryArg;
        u32 flagsArg;
        u32 kind;
        u32 callbackObj;
        u32 vtbl;
        void (*callback)(u32, u32, u32, u32, u32);

        check = *(u32*)(entry + 0x30);
        obj = *(u32*)(entry + 0x34);
        if (check != 0) {
            mtx = *(u32*)(entry + 0x30);
        } else {
            mtx = defaultMtx;
        }
        entryArg = entry;
        flagsArg = *(u32*)(entry + 0x38);
        kind = 4;
        callbackObj = *(u32*)(entry + 0x34);
        vtbl = *(u32*)callbackObj;
        callback = *(void (**)(u32, u32, u32, u32, u32))(vtbl + 0x48);
        callback(obj, mtx, entryArg, kind, flagsArg);
        entry = *(u32*)(entry + 0x3C);
    }

    entry = lbl_8047B258;
    while (entry != 0) {
        u32 obj;
        u32 mtx;
        u32 check;
        u32 entryArg;
        u32 flagsArg;
        u32 kind;
        u32 callbackObj;
        u32 vtbl;
        void (*callback)(u32, u32, u32, u32, u32);

        check = *(u32*)(entry + 0x30);
        obj = *(u32*)(entry + 0x34);
        if (check != 0) {
            mtx = *(u32*)(entry + 0x30);
        } else {
            mtx = defaultMtx;
        }
        entryArg = entry;
        flagsArg = *(u32*)(entry + 0x38);
        kind = 2;
        callbackObj = *(u32*)(entry + 0x34);
        vtbl = *(u32*)callbackObj;
        callback = *(void (**)(u32, u32, u32, u32, u32))(vtbl + 0x48);
        callback(obj, mtx, entryArg, kind, flagsArg);
        entry = *(u32*)(entry + 0x40);
    }

    entry = lbl_8047B24C;
    while (entry != 0) {
        u32 next;

        next = *(u32*)(entry + 0x44);
        if (*(u32*)(entry + 0x30) != 0) {
            HSD_MtxFree(*(u32*)(entry + 0x30));
        }
        HSD_ObjFree(lbl_80465348, entry);
        entry = next;
    }

    lbl_8047B24C = 0;
    lbl_80478C64 = (u32)&lbl_8047B24C;
    lbl_8047B250 = 0;
    lbl_80478C68 = (u32)&lbl_8047B250;
    lbl_8047B254 = 0;
    lbl_8047B258 = 0;
    lbl_80478C6C = (u32)&lbl_8047B258;
    lbl_8047B25C = 0;
}

/* 0x54 | fn_801975FC | two_call_arg_check */
void fn_801975FC(void) {
#pragma optimization_level 1
    extern s32 lbl_8047B248;
    extern u32 lbl_8047B250;
    extern u32 lbl_8047B254;
    extern u32 lbl_8047B258;
    extern u32 lbl_8047B25C;

    if (lbl_8047B248 == 0) {
        return;
    }
    lbl_8047B250 = fn_80197650(lbl_8047B250, lbl_8047B254, 0x3C);
    lbl_8047B258 = fn_80197650(lbl_8047B258, lbl_8047B25C, 0x40);
}

/* 0x80197650 | 0x134 */
u32 fn_80197650(u32 head, s32 count, u32 nextOffset) {
    u32 result;
    u32 front;
    u32 back;
    s32 backCount;
    s32 frontCount;
    s32 i;
    u32* tail;

    result = head;
    if ((s32)count <= 1) {
        if (result != 0) {
            *(u32*)(result + nextOffset) = 0;
        }
        return result;
    }

    frontCount = count / 2;
    backCount = count - frontCount;
    i = 0;
    back = result;
    while (i < frontCount) {
        back = *(u32*)(back + nextOffset);
        i++;
    }

    front = fn_80197650(result, frontCount, nextOffset);
    back = fn_80197650(back, backCount, nextOffset);

    result = 0;
    tail = &result;
    while (front != 0 && back != 0) {
        if (*(f32*)(front + 0x2C) <= *(f32*)(back + 0x2C)) {
            *tail = front;
            front = *(u32*)(front + nextOffset);
        } else {
            *tail = back;
            back = *(u32*)(back + nextOffset);
        }
        tail = (u32*)(*tail + nextOffset);
    }

    if (front != 0) {
        *tail = front;
    } else if (back != 0) {
        *tail = back;
    }

    return result;
}

/* 0x80197784 | 0x214 */
void fn_80197784(u32 obj, u32 mtx, u32 mask, u32 arg) {
#pragma optimization_level 1
    extern u8 lbl_80465348[];
    extern u32 lbl_80478C64;
    extern u32 lbl_80478C68;
    extern u32 lbl_80478C6C;
    extern s32 lbl_8047B244;
    extern u32 lbl_8047B254;
    extern u32 lbl_8047B25C;
    extern u8 lbl_8047D9E0[7];
    extern u8 lbl_8047D9E8[5];
    extern void fn_800A2D64(u32, u32);
    extern u32 HSD_CObjGetCurrent(void);
    extern void __assert(u8*, u32, u8*);
    extern void fn_8019D9DC(u32);
    extern u32 HSD_MtxAlloc(void);
    extern u32 HSD_ObjAlloc(void*);
    u8 mtxBuf[0x30];
    u32 renderFlags;
    u32 vtbl;
    u32 entry;
    s32 shouldUpdate;
    void (*setup)(u32, u32, u32);
    void (*draw)(u32, u32, u32, u32, u32);

    if ((*(u32*)(obj + 0x14) & 0x10) != 0) {
        return;
    }

    renderFlags = *(u32*)(obj + 0x14) & (mask << 18);
    if (renderFlags == 0) {
        return;
    }

    if (obj != 0) {
        if (obj == 0) {
            __assert(lbl_8047D9E0, 0x25D, lbl_8047D9E8);
        }
        shouldUpdate = 0;
        if ((*(u32*)(obj + 0x14) & 0x00800000) == 0) {
            if ((*(u32*)(obj + 0x14) & 0x40) != 0) {
                shouldUpdate = 1;
            }
        }
        if (shouldUpdate != 0) {
            fn_8019D9DC(obj);
        }
    }

    if (mtx == 0) {
        u32 cobj;
        u32 mtxBase;

        cobj = HSD_CObjGetCurrent();
        mtxBase = cobj + 0x54;
        mtx = mtxBase;
    }

    vtbl = *(u32*)obj;
    setup = *(void (**)(u32, u32, u32))(vtbl + 0x44);
    setup(obj, mtx, (u32)mtxBuf);

    if ((renderFlags & 0x00040000) != 0) {
        (*(void (**)(u32, u32, u32, u32, u32))(*(u32*)obj + 0x48))(obj, mtx, (u32)mtxBuf, 1, arg);
    }

    if (lbl_8047B244 == 0) {
        if ((renderFlags & 0x00100000) != 0) {
            (*(void (**)(u32, u32, u32, u32, u32))(*(u32*)obj + 0x48))(obj, mtx, (u32)mtxBuf, 4, arg);
        }
        if ((renderFlags & 0x00080000) == 0) {
            return;
        }
        (*(void (**)(u32, u32, u32, u32, u32))(*(u32*)obj + 0x48))(obj, mtx, (u32)mtxBuf, 2, arg);
        return;
    }

    if ((renderFlags & 0x00180000) == 0) {
        return;
    }

    entry = HSD_ObjAlloc(lbl_80465348);
    memset((void*)(entry + 0x30), 0, 0x18);
    {
        u32 buf;

        buf = (u32)mtxBuf;
        fn_800A2D64(buf, entry);
    }
    if (mtx != 0) {
        u32 allocated;

        allocated = HSD_MtxAlloc();
        *(u32*)(entry + 0x30) = allocated;
        fn_800A2D64(mtx, *(u32*)(entry + 0x30));
    }
    *(u32*)(entry + 0x34) = obj;
    *(u32*)(entry + 0x38) = arg;
    *(u32*)lbl_80478C64 = entry;
    lbl_80478C64 = entry + 0x44;

    if ((renderFlags & 0x00100000) != 0) {
        *(u32*)lbl_80478C68 = entry;
        lbl_80478C68 = entry + 0x3C;
        lbl_8047B254++;
    }

    if ((renderFlags & 0x00080000) != 0) {
        *(u32*)lbl_80478C6C = entry;
        lbl_80478C6C = entry + 0x40;
        lbl_8047B25C++;
    }
}

/* 0x80197998 | 0xCC */
void fn_80197998(u32 obj, u32 arg1, u32 arg2, u32 mask, u32 flags) {
    extern void HSD_DObjSetCurrent(u32);
    extern void fn_8019F024(u32);
    extern void fn_801A5DCC(u32);
    extern void fn_801AB63C(u32, u32);
    u32 child;
    u32 drawMask;
    u32 vtbl;
    void (*callback)(u32, u32, u32, u32);

    fn_8019F024(obj);
    drawMask = mask << 1;
    if ((flags & 0x04000000) == 0) {
        if ((*(u32*)(obj + 0x14) & 0x00010000) != 0) {
            fn_801A5DCC(arg2);
        }
    }
    fn_801AB63C(0, 0);
    child = *(u32*)(obj + 0x18);
    while (child != 0) {
        if ((*(volatile u32*)(child + 0x14) & 1) == 0) {
            if ((*(volatile u32*)(child + 0x14) & drawMask) != 0) {
                HSD_DObjSetCurrent(child);
                vtbl = *(u32*)child;
                callback = *(void (**)(u32, u32, u32, u32))(vtbl + 0x3C);
                callback(child, arg1, arg2, flags);
            }
        }
        child = *(u32*)(child + 4);
    }
    HSD_DObjSetCurrent(0);
    fn_8019F024(0);
}
