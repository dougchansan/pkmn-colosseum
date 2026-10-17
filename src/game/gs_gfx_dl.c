/**
 * @file gs_gfx_dl.c
 * @brief GSgfx display lists, the whole TU: parse a GX display list back
 * into vertex callbacks, record (GSgfxDLBegin / GSgfxDLEnd, fn_800DB098,
 * fn_800DB758) or replay (GSgfxDLDraw) captured lists.
 * .text 0x800DA578 - 0x800DB890, .data 0x803152B8 - 0x80315384.
 *
 * XD source unit: game/pxdvs/GSAPI/GSgfxM/dl.cpp (C++; the parse helpers
 * keep their mangled names). The TU's .data is its four switch tables,
 * laid out contiguously from the 8-aligned start: _dlParseSurface's
 * (0x803152B8), _dlParseVertex's two (0x80315320, 0x80315340; MWCC emits
 * a function's tables last switch first) and fn_800DB758's (0x80315364).
 * fn_800DB758's table starts at a 4-aligned address, so the TU cannot be
 * linked as function carves once _dlParseVertex's tables are compiled; it
 * links whole. Its data (the GSgfx state pointer lbl_8047AA80 and the
 * capture table lbl_8047AAD4 / lbl_8047AAD8, in .sbss) stays extern.
 * GC/1.3 -O4,p.
 */
#include "game/gs_gfx_layer.h"

typedef struct GSgfxVtxDescList {
    s32 attr;
    s32 attr_type;
    s32 comp_cnt;
    s32 comp_type;
    u8 frac;
    u16 stride;
    void* vertex;
} GSgfxVtxDescList;

typedef struct GSgfxParseCallbackList {
    void (*begin)(s32 prim, u16 count, u32 attrs, void* user);
    void (*beginVertex)(void* user);
    void (*vertexAttr)(u32 attr, void* data, void* user);
    void (*endVertex)(void* user);
    void (*end)(void* user);
} GSgfxParseCallbackList;

extern void DCFlushRange(void* addr, u32 size);
extern void GXCallDisplayList(void* list, u32 size);
extern void gfxRecordCommand(u32, u32, ...);
extern void gfxAddDrawStats(u32 verts, u32 prims);
extern void fn_800D6A80(u16 vertCount, s32 type, u32* totalVerts, u32* totalPrims);
extern void gfxLoadVtxDesc(GSVtxDesc* desc);
extern void gfxSetupChannelsTev(GSVtxDesc* desc);
extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern void* GSmemLock(u16 handle);
extern void fn_800E2AF8(u16 handle, u32 size);
extern u16 GSmemAlloc(u32 size, u32 align);

extern GSgfxDLCapture* lbl_8047AAD4;
extern u32 lbl_8047AAD8;

u8* _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(s32 prim, GSgfxVtxDescList* desc, u8* ptr, u16 count, GSgfxParseCallbackList* callbacks, void* user);
u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user);

extern void _gfxEmitTexCoord1x8(u32);
extern void _gfxEmitTexCoord1x16(u32);
extern void _gfxEmitTexCoord2s8(u32);
extern void _gfxEmitTexCoord2u8(u32);
extern void _gfxEmitTexCoord2s16(u32);
extern void _gfxEmitTexCoord2u16(u32);
extern void _gfxEmitTexCoord2f32(u32);
extern void _gfxEmitColor1x8(u32);
extern void _gfxEmitColor1x16(u32);
extern void _gfxEmitColor1u16(u32);
extern void _gfxEmitColor1u32(u32);
extern void _gfxEmitColor3u8(u32);
extern void _gfxEmitColor4u8(u32);
extern void _gfxEmitNormal1x8(u32);
extern void _gfxEmitNormal1x16(u32);
extern void _gfxEmitNormal3s8(u32);
extern void _gfxEmitNormal3s16(u32);
extern void _gfxEmitNormal3f32(u32);
extern void _gfxEmitPos1x8(u32);
extern void _gfxEmitPos1x16(u32);
extern void _gfxEmitPos2s8(u32);
extern void _gfxEmitPos2u8(u32);
extern void _gfxEmitPos2s16(u32);
extern void _gfxEmitPos2u16(u32);
extern void _gfxEmitPos2f32(u32);
extern void _gfxEmitPos3s8(u32);
extern void _gfxEmitPos3u8(u32);
extern void _gfxEmitPos3s16(u32);
extern void _gfxEmitPos3u16(u32);
extern void _gfxEmitPos3f32(u32);

void GSgfxParseDisplayList(GSgfxVtxDescList* desc, u8* ptr, u32 size, GSgfxParseCallbackList* callbacks, void* user) {
    s32 prim;
    u8* end;
    u16 count;
    s32 cmd;

    end = ptr + size;
    while (ptr < end) {
        cmd = *ptr++ & 0xF8;
        switch (cmd) {
        case 0x00:
            break;
        case 0x80:
        case 0x90:
        case 0x98:
        case 0xA0:
        case 0xA8:
        case 0xB0:
        case 0xB8:
            count = *(u16*)ptr;
            ptr = (u8*)((u16*)ptr + 1);
            switch (cmd) {
            case 0xB8:
                prim = 0;
                break;
            case 0xA8:
                prim = 1;
                break;
            case 0xB0:
                prim = 2;
                break;
            case 0x90:
                prim = 3;
                break;
            case 0x98:
                prim = 4;
                break;
            case 0xA0:
                prim = 5;
                break;
            case 0x80:
                prim = 6;
                break;
            }
            ptr = _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(prim, desc, ptr, count, callbacks, user);
            break;
        case 0x61:
            ptr += 4;
            break;
        default:
            return;
        }
    }
}

u8* _dlParseSurface__F13GSgfxPrimTypeP16_HSD_VtxDescListPUcUsP22GSgfxParseCallbackListPv_802B1590(s32 prim, GSgfxVtxDescList* desc, u8* ptr, u16 count, GSgfxParseCallbackList* callbacks, void* user) {
    GSgfxVtxDescList* it;
    u32 mask;
    s32 attr;

    mask = 0;
    it = desc;
    while ((attr = it->attr) != 0xff) {
        switch (attr) {
        case 0:
            mask |= 1;
            break;
        case 1:
            mask |= 0x40;
            break;
        case 9:
            mask |= 2;
            break;
        case 10:
            mask |= 4;
            break;
        case 25:
            mask |= 8;
            break;
        case 11:
            mask |= 0x10;
            break;
        case 12:
            mask |= 0x20;
            break;
        case 13:
            mask |= 0x80;
            break;
        case 14:
            mask |= 0x100;
            break;
        case 15:
            mask |= 0x200;
            break;
        case 16:
            mask |= 0x400;
            break;
        case 17:
            mask |= 0x800;
            break;
        case 18:
            mask |= 0x1000;
            break;
        case 19:
            mask |= 0x2000;
            break;
        case 20:
            mask |= 0x4000;
            break;
        }
        it++;
    }

    if (callbacks->begin != 0) {
        callbacks->begin(prim, count, mask, user);
    }

    while (count-- != 0) {
        if (callbacks->beginVertex != 0) {
            callbacks->beginVertex(user);
        }
        it = desc;
        while (it->attr != 0xff) {
            ptr = _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(it, ptr, callbacks, user);
            it++;
        }
        if (callbacks->endVertex != 0) {
            callbacks->endVertex(user);
        }
    }

    if (callbacks->end != 0) {
        callbacks->end(user);
    }
    return ptr;
}

/*
 * Decodes one attribute of one vertex and hands it to the vertexAttr
 * callback as attr | component | type flags.
 *
 * The flags, the component count and the direct-data size are left unset
 * on the out-of-range paths (an attribute outside 0-20/25, an unknown
 * component type or format): retail has no initialising instruction on
 * those paths either and passes whatever the registers hold.
 *
 * Retail builds the colour attribute flag (attributes 11/12) and the
 * else-path component flag in r4, then copies them to their home
 * registers (mr r7,r4 / mr r8,r4). Both values go through one variable
 * that is r4 = data's register and is never live on entry: on the
 * component switch's default path the copy reads the incoming data
 * pointer (retail's r4 there is still the ptr argument). The mwcc-debugger
 * replay (GC/2.6, same code as GC/1.3 here) shows why a separate u32 flag
 * cannot give this: it is read uninitialised on that default path, so it
 * is live on entry, interferes with the argument registers and with data,
 * and is coloured r11. Reusing data (defined on entry by `data = ptr`)
 * gives retail's r4, and `opt_lifetimes off` keeps the colour path's
 * definition in the same web, so the mr r7,r4 copy survives. Without the
 * pragma the colour path's value is split off and built in r7 (99.4%);
 * a separate flag with the pragma is 99.7%.
 */
/* RULE-EXCEPTION(title-path): local compiler control and variable reuse - see docs/RULE_EXCEPTIONS.md */
#pragma push
#pragma opt_lifetimes off
u8* _dlParseVertex__FP16_HSD_VtxDescListPUcP22GSgfxParseCallbackListPv_802B1720(GSgfxVtxDescList* desc, u8* ptr, GSgfxParseCallbackList* callbacks, void* user) {
    u16 index;
    u8* data;
    s32 count;
    u32 attrFlag;
    u32 compFlag;
    u32 typeFlag;
    s32 attr;

    data = ptr;
    attr = desc->attr;
    switch (attr) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        compFlag = 0x10000;
        typeFlag = 0x10000000;
        switch (attr) {
        case 0:
            attrFlag = 1;
            break;
        case 1:
            attrFlag = 0x40;
            break;
        case 2:
            attrFlag = 0x40;
            break;
        case 3:
            attrFlag = 0x40;
            break;
        case 4:
            attrFlag = 0x40;
            break;
        case 5:
            attrFlag = 0x40;
            break;
        case 6:
            attrFlag = 0x40;
            break;
        case 7:
            attrFlag = 0x40;
            break;
        case 8:
            attrFlag = 0x40;
            break;
        }
        ptr += 1;
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 20:
    case 25:
        if (attr == 11 || attr == 12) {
            /* RULE-EXCEPTION(title-path): data reused for the flag values - see docs/RULE_EXCEPTIONS.md */
            data = (u8*)0x20;
            if (attr == 11) {
                data = (u8*)0x10;
            }
            attrFlag = (u32)data;
            typeFlag = 0x10000000;
            switch (desc->comp_type) {
            case 0:
                compFlag = 0x200000;
                break;
            case 1:
                compFlag = 0x400000;
                break;
            case 2:
                compFlag = 0x800000;
                break;
            case 3:
                compFlag = 0x1000000;
                break;
            case 4:
                compFlag = 0x2000000;
                break;
            case 5:
                compFlag = 0x4000000;
                break;
            }
        } else {
            switch (desc->comp_type) {
            case 0:
                data = (u8*)0x10000;
                break;
            case 1:
                data = (u8*)0x20000;
                break;
            case 2:
                data = (u8*)0x40000;
                break;
            case 3:
                data = (u8*)0x80000;
                break;
            case 4:
                data = (u8*)0x100000;
                break;
            }
            compFlag = (u32)data;
            switch (attr) {
            case 9:
                attrFlag = 2;
                if (desc->comp_cnt == 0) {
                    typeFlag = 0x20000000;
                } else {
                    typeFlag = 0x40000000;
                }
                break;
            case 10:
                attrFlag = 4;
                if (desc->comp_cnt == 0) {
                    typeFlag = 0x40000000;
                }
                break;
            case 25:
                attrFlag = 8;
                if (desc->comp_cnt == 1) {
                    typeFlag = 0x40000000;
                }
                break;
            case 13:
            case 14:
            case 15:
            case 16:
            case 17:
            case 18:
            case 19:
            case 20:
                switch (attr) {
                case 13:
                    attrFlag = 0x80;
                    break;
                case 14:
                    attrFlag = 0x100;
                    break;
                case 15:
                    attrFlag = 0x200;
                    break;
                case 16:
                    attrFlag = 0x400;
                    break;
                case 17:
                    attrFlag = 0x800;
                    break;
                case 18:
                    attrFlag = 0x1000;
                    break;
                case 19:
                    attrFlag = 0x2000;
                    break;
                case 20:
                    attrFlag = 0x4000;
                    break;
                }
                if (desc->comp_cnt == 0) {
                    typeFlag = 0x10000000;
                } else {
                    typeFlag = 0x20000000;
                }
                break;
            }
        }
        if (desc->attr_type == 1) {
            data = ptr;
            switch (compFlag) {
            case 0x10000:
            case 0x20000:
                count = 1;
                break;
            case 0x40000:
            case 0x80000:
            case 0x200000:
            case 0x1000000:
                count = 2;
                break;
            case 0x400000:
            case 0x2000000:
                count = 3;
                break;
            case 0x100000:
            case 0x800000:
            case 0x4000000:
                count = 4;
                break;
            }
            switch (typeFlag) {
            case 0x10000000:
                break;
            case 0x20000000:
                count <<= 1;
                break;
            case 0x40000000:
                count *= 3;
                break;
            case 0x80000000:
                count <<= 2;
                break;
            }
            ptr += count;
        } else {
            if (desc->attr_type == 2) {
                index = *ptr++;
            } else {
                index = *(u16*)ptr;
                ptr += 2;
            }
            data = (u8*)desc->vertex + index * desc->stride;
        }
        break;
    }

    if (callbacks->vertexAttr != 0) {
        callbacks->vertexAttr(attrFlag | compFlag | typeFlag, data, user);
    }
    return ptr;
}
#pragma pop

void GSgfxDLFree(GSgfxDLCapture* capture) {
    if (lbl_8047AA80->captureDL != capture) {
        fn_800E24B0(capture->handle);
        GSmemFree(capture->handle);
        capture->active = 0;
    }
}

void GSgfxDLDraw(GSgfxDLCapture* capture) {
    if (lbl_8047AA80->recordMode == 1) {
        gfxRecordCommand(0x2a, 1, capture);
    } else if (lbl_8047AA80->layerDraw == lbl_8047AA80->layerCur &&
               (lbl_8047AA80->drawMask & lbl_8047AA80->drawEnable) &&
               capture->size != 0) {
        gfxLoadVtxDesc(capture->desc);
        gfxSetupChannelsTev(capture->desc);
        GXCallDisplayList(capture->data, capture->size);
        gfxAddDrawStats(capture->totalVerts, capture->totalPrims);
    }
}

GSgfxDLCapture* GSgfxDLEnd(void) {
    GSgfxDLCapture* capture;
    u8* end;
    u32 pad;

    if (!lbl_8047AA80->captureActive) {
        return NULL;
    }
    capture = lbl_8047AA80->captureDL;
    end = (u8*)(((u32)lbl_8047AA80->captureCursor + 0x1F) & ~0x1F);
    if ((u32)end > (u32)capture->data + capture->size || capture->overflow != 0) {
        fn_800E24B0(capture->handle);
        GSmemFree(capture->handle);
        return NULL;
    }
    for (pad = end - lbl_8047AA80->captureCursor; pad != 0; pad--) {
        *lbl_8047AA80->captureCursor++ = 0;
    }
    capture->size = end - (u8*)capture->data;
    fn_800E2AF8(capture->handle, capture->size);
    DCFlushRange(capture->data, capture->size);
    lbl_8047AA80->captureActive = 0;
    lbl_8047AA80->captureDL = NULL;
    lbl_8047AA80->captureCursor = NULL;
    return capture;
}

/* RULE-EXCEPTION(title-path): single-use helper with register-only evidence - see docs/RULE_EXCEPTIONS.md */
static inline GSgfxDLCapture* GSgfxFindFreeDLCapture(GSgfxDLCapture* capture) {
    u32 i;

    for (i = 0; i < lbl_8047AAD8; i++) {
        if (capture->active == 0) {
            return capture;
        }
        capture++;
    }
    return NULL;
}

u32 GSgfxDLBegin(GSVtxDesc* desc, u32 size) {
    GSgfxDLCapture* capture;

    if (lbl_8047AA80->captureActive == 1) {
        return 0;
    }
    if (lbl_8047AA80->vtxPending == 1) {
        return 0;
    }
    capture = GSgfxFindFreeDLCapture(lbl_8047AAD4);
    if (capture == NULL) {
        return 0;
    }
    capture->active = 1;
    capture->overflow = 0;
    capture->totalVerts = 0;
    capture->totalPrims = 0;
    capture->handle = GSmemAlloc(size, 0x20);
    if (capture->handle == 0) {
        return 0;
    }
    capture->size = size;
    capture->data = GSmemLock(capture->handle);
    if (capture->data == NULL) {
        GSmemFree(capture->handle);
        return 0;
    }
    capture->desc = desc;
    lbl_8047AA80->captureActive = 1;
    lbl_8047AA80->captureDL = capture;
    lbl_8047AA80->captureCursor = capture->data;
    return 1;
}

void fn_800DB098(void) {
    GSVtxEmitFn emit;
    s32 i;
    s32 n;

    if (lbl_8047AA80->captureCursor + 0x68 > (u8*)lbl_8047AA80->captureDL->data + lbl_8047AA80->captureDL->size) {
        lbl_8047AA80->captureDL->overflow = 1;
        return;
    }

    emit = lbl_8047AA80->emitPos;
    if (emit == _gfxEmitPos3f32) {
        u32* p = (u32*)lbl_8047AA80->captureCursor;
        *p++ = *(u32*)&lbl_8047AA80->pos.f[0];
        *p++ = *(u32*)&lbl_8047AA80->pos.f[1];
        *p++ = *(u32*)&lbl_8047AA80->pos.f[2];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos3u16 || emit == _gfxEmitPos3s16) {
        u16* p = (u16*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.s[0];
        *p++ = lbl_8047AA80->pos.s[1];
        *p++ = lbl_8047AA80->pos.s[2];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos3u8 || emit == _gfxEmitPos3s8) {
        u8* p = (u8*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.b[0];
        *p++ = lbl_8047AA80->pos.b[1];
        *p++ = lbl_8047AA80->pos.b[2];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos2f32) {
        u32* p = (u32*)lbl_8047AA80->captureCursor;
        *p++ = *(u32*)&lbl_8047AA80->pos.f[0];
        *p++ = *(u32*)&lbl_8047AA80->pos.f[1];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos2u16 || emit == _gfxEmitPos2s16) {
        u16* p = (u16*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.s[0];
        *p++ = lbl_8047AA80->pos.s[1];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos2u8 || emit == _gfxEmitPos2s8) {
        u8* p = (u8*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.b[0];
        *p++ = lbl_8047AA80->pos.b[1];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos1x16) {
        u16* p = (u16*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.s[0];
        lbl_8047AA80->captureCursor = (u8*)p;
    } else if (emit == _gfxEmitPos1x8) {
        u8* p = (u8*)lbl_8047AA80->captureCursor;
        *p++ = lbl_8047AA80->pos.b[0];
        lbl_8047AA80->captureCursor = (u8*)p;
    }

    emit = lbl_8047AA80->emitNrm;
    if (lbl_8047AA80->captureDL->desc->attr[2].enabled == 1) {
        if (emit == _gfxEmitNormal3f32) {
            u32* p = (u32*)lbl_8047AA80->captureCursor;
            *p++ = *(u32*)&lbl_8047AA80->nrm.f[0];
            *p++ = *(u32*)&lbl_8047AA80->nrm.f[1];
            *p++ = *(u32*)&lbl_8047AA80->nrm.f[2];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == _gfxEmitNormal3s16) {
            u16* p = (u16*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.s[0];
            *p++ = lbl_8047AA80->nrm.s[1];
            *p++ = lbl_8047AA80->nrm.s[2];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == _gfxEmitNormal3s8) {
            u8* p = (u8*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.b[0];
            *p++ = lbl_8047AA80->nrm.b[1];
            *p++ = lbl_8047AA80->nrm.b[2];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == _gfxEmitNormal1x16) {
            u16* p = (u16*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.s[0];
            lbl_8047AA80->captureCursor = (u8*)p;
        } else if (emit == _gfxEmitNormal1x8) {
            u8* p = (u8*)lbl_8047AA80->captureCursor;
            *p++ = lbl_8047AA80->nrm.b[0];
            lbl_8047AA80->captureCursor = (u8*)p;
        }
    }

    for (i = 4; i < 6; i++) {
        n = i - 4;
        emit = lbl_8047AA80->emitClr[n];
        if (lbl_8047AA80->captureDL->desc->attr[i].enabled == 1) {
            if (emit == _gfxEmitColor4u8) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].b[0];
                *p++ = lbl_8047AA80->clr[n].b[1];
                *p++ = lbl_8047AA80->clr[n].b[2];
                *p++ = lbl_8047AA80->clr[n].b[3];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitColor3u8) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].b[0];
                *p++ = lbl_8047AA80->clr[n].b[1];
                *p++ = lbl_8047AA80->clr[n].b[2];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitColor1u32) {
                u32* p = (u32*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].w;
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitColor1u16) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].s;
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitColor1x16) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].s;
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitColor1x8) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->clr[n].b[0];
                lbl_8047AA80->captureCursor = (u8*)p;
            }
        }
    }

    for (i = 6; i < 14; i++) {
        n = i - 6;
        emit = lbl_8047AA80->emitTex[n];
        if (lbl_8047AA80->captureDL->desc->attr[i].enabled == 1) {
            if (emit == _gfxEmitTexCoord2f32) {
                u32* p = (u32*)lbl_8047AA80->captureCursor;
                *p++ = *(u32*)&lbl_8047AA80->tex[n].f[0];
                *p++ = *(u32*)&lbl_8047AA80->tex[n].f[1];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitTexCoord2u16 || emit == _gfxEmitTexCoord2s16) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].s[0];
                *p++ = lbl_8047AA80->tex[n].s[1];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitTexCoord2u8 || emit == _gfxEmitTexCoord2s8) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].b[0];
                *p++ = lbl_8047AA80->tex[n].b[1];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitTexCoord1x16) {
                u16* p = (u16*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].s[0];
                lbl_8047AA80->captureCursor = (u8*)p;
            } else if (emit == _gfxEmitTexCoord1x8) {
                u8* p = (u8*)lbl_8047AA80->captureCursor;
                *p++ = lbl_8047AA80->tex[n].b[0];
                lbl_8047AA80->captureCursor = (u8*)p;
            }
        }
    }
}

void fn_800DB758(u16 vertCount)
{
    u32 state;
    u32 obj;
    u8* p;
    u16* p16;

    state = (u32)lbl_8047AA80;
    if (*(s32*)(state + 0x488) == 7) {
        vertCount = (vertCount & 0x7FFF) << 1;
    }

    obj = *(u32*)(state + 0x480);
    fn_800D6A80(vertCount, *(u32*)(state + 0x488),
                (u32*)(obj + 0x10), (u32*)(obj + 0x14));

    state = (u32)lbl_8047AA80;
    switch (*(u32*)(state + 0x488)) {
        case 0:
            *(u8*)*(u32*)(state + 0x484) = 0xB8;
            break;
        case 1:
            *(u8*)*(u32*)(state + 0x484) = 0xA8;
            break;
        case 2:
            *(u8*)*(u32*)(state + 0x484) = 0xB0;
            break;
        case 3:
            *(u8*)*(u32*)(state + 0x484) = 0x90;
            break;
        case 4:
            *(u8*)*(u32*)(state + 0x484) = 0x98;
            break;
        case 5:
            *(u8*)*(u32*)(state + 0x484) = 0xA0;
            break;
        case 6:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
        case 7:
            *(u8*)*(u32*)(state + 0x484) = 0x80;
            break;
    }

    state = (u32)lbl_8047AA80;
    p = *(u8**)(state + 0x484);
    *(u32*)(state + 0x484) = (u32)(p + 1);
    obj = *(u32*)(state + 0x480);
    *p = (u8)(*p | *(u32*)(*(u32*)(obj + 0xC) + 4));

    state = (u32)lbl_8047AA80;
    p16 = *(u16**)(state + 0x484);
    *p16++ = vertCount;
    state = (u32)lbl_8047AA80;
    *(u32*)(state + 0x484) = (u32)p16;
}
