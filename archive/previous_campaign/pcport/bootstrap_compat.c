#include <stdio.h>
#include <stdlib.h>

#ifdef _MSC_VER
#include <intrin.h>
#endif

void __assert(const char* expr, const char* file, int line)
{
    fprintf(stderr, "[assert] %s:%d: %s\n",
            file != NULL ? file : "?", line, expr != NULL ? expr : "?");
    abort();
}

int __builtin_clz(unsigned int x)
{
    unsigned long idx;
    if (x == 0u) {
        return 32;
    }
    _BitScanReverse(&idx, x);
    return 31 - (int)idx;
}
