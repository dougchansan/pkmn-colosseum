#include "dolphin/types.h"

typedef struct GSLogVaList {
    u8 gpr;
    u8 fpr;
    u16 padding;
    u32* overflow_arg_area;
    u32* reg_save_area;
} GSLogVaList;

extern void fn_800DE128(char* output, u32 capacity, const char* format,
                        void* arguments);

void fn_800DE09C(char* dst, u32 size, const char* format, ...)
{
    GSLogVaList args;

    __builtin_va_info(&args);
    fn_800DE128(dst, size, format, &args);
}
