#include "dolphin/types.h"
#include "game/gs_model.h"

typedef struct GSjobjClass {
    u8 pad_00[0x8];
    u16 flags;
    u16 pad_0A;
} GSjobjClass;

typedef struct GSlight {
    u8 pad_00[0xC];
    GSjobjClass* classObject;
} GSlight;

typedef struct GSmodel {
    u8 pad_00[0x158];
    u32 shadowVertexCount;
    void* shadowVertexBuffer;
    GSlight* shadowLight;
    u16 shadowVertexHandle;
} GSmodel;

extern void fn_800E24B0(u16 handle);
extern void GSmemFree(u16 handle);
extern u16 _toolentryAlloc__FUl(u32 size);
extern void* GSmemLock(u16 handle);
extern void* memcpy(void* dst, const void* src, u32 size);

void GSmodelSetShadowLight(GSmodel* model, GSlight* light)
{
    if (light != NULL && (light->classObject->flags & 0x3) == 0) {
        light = NULL;
    }
    model->shadowLight = light;
}

void GSmodelSetShadowSurface(GSmodel* model, s32 count, const void* data)
{
    if (count != model->shadowVertexCount) {
        if (model->shadowVertexHandle != 0) {
            fn_800E24B0(model->shadowVertexHandle);
            GSmemFree(model->shadowVertexHandle);
            model->shadowVertexHandle = 0;
            model->shadowVertexCount = 0;
            model->shadowVertexBuffer = NULL;
        }
        if (count == 0 || data == NULL) {
            return;
        }
        model->shadowVertexCount = count;
        model->shadowVertexHandle =
            _toolentryAlloc__FUl(model->shadowVertexCount * 4);
        model->shadowVertexBuffer = GSmemLock(model->shadowVertexHandle);
    }
    memcpy(model->shadowVertexBuffer, data, model->shadowVertexCount * 4);
}
