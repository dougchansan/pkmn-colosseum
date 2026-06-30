#include "dolphin/types.h"

/*
 * exit.c - MetroWerks CRT exit handling.
 *
 * Implements C library exit(), the static constructor/destructor
 * initialization, and program termination support.
 */

/* Linker-defined constructor/destructor tables */
typedef void (*FuncPtr)(void);
extern FuncPtr _ctors[];
extern FuncPtr _dtors[];

/* SDA-relative globals */
extern s32 __aborting;
extern FuncPtr __stdio_exit;
extern FuncPtr __console_exit;
extern s32 __atexit_curr_func;
extern FuncPtr __atexit_funcs[];

extern void __begin_critical_region(s32 region);
extern void __end_critical_region(s32 region);
extern void __kill_critical_regions(void);
extern void __destroy_global_chain(void);
extern void _ExitProcess(void);
extern void PPCHalt(void);

/*
 * __init_cpp - Call all static constructors.
 *
 * Iterates the _ctors table (NULL-terminated array of function
 * pointers) and calls each constructor in order.
 */
static void __init_cpp(void) {
    FuncPtr* p;

    for (p = _ctors; *p != NULL; p++) {
        (*p)();
    }
}

/*
 * __init_user - Initialize user-level CRT state.
 * Calls __init_cpp to run static constructors.
 */
void __init_user(void) {
    __init_cpp();
}

/*
 * _ExitProcess - Halt the processor.
 * Final termination point; never returns.
 */
void _ExitProcess(void) {
    PPCHalt();
}

/*
 * exit - Standard C library exit function.
 *
 * Performs orderly shutdown:
 *   1. If not aborting, call destructor chain and static destructors
 *   2. Call __stdio_exit if registered
 *   3. Call atexit-registered functions in reverse order
 *   4. Call __console_exit if registered
 *   5. Halt the processor via _ExitProcess
 */
void exit(int status) {
    if (__aborting == 0) {
        /* Call begin/end critical region pair (bookkeeping) */
        __begin_critical_region(0);
        __end_critical_region(0);

        /* Run global destructors */
        __destroy_global_chain();

        /* Call static destructors from _dtors table */
        {
            FuncPtr* p = _dtors;
            while (*p != NULL) {
                (*p)();
                p++;
            }
        }

        /* Call stdio exit handler if registered */
        if (__stdio_exit != NULL) {
            __stdio_exit();
            __stdio_exit = NULL;
        }
    }

    /* Call atexit functions in reverse order */
    __begin_critical_region(0);
    while (__atexit_curr_func > 0) {
        __atexit_curr_func--;
        __atexit_funcs[__atexit_curr_func]();
    }
    __end_critical_region(0);

    __kill_critical_regions();

    /* Call console exit handler if registered */
    if (__console_exit != NULL) {
        __console_exit();
        __console_exit = NULL;
    }

    _ExitProcess();
}

/*
 * MWTRACE - MetroWerks trace/debug output function.
 *
 * Variadic no-op stub in release builds. The register spill
 * is required for ABI compliance with varargs on PPC.
 */
asm void MWTRACE(s32 level, const char* fmt, ...) {
    nofralloc
    stwu   r1, -0x70(r1)
    bne    cr1, @skip_fp
    stfd   f1, 0x28(r1)
    stfd   f2, 0x30(r1)
    stfd   f3, 0x38(r1)
    stfd   f4, 0x40(r1)
    stfd   f5, 0x48(r1)
    stfd   f6, 0x50(r1)
    stfd   f7, 0x58(r1)
    stfd   f8, 0x60(r1)
@skip_fp:
    stw    r3, 0x08(r1)
    stw    r4, 0x0c(r1)
    stw    r5, 0x10(r1)
    stw    r6, 0x14(r1)
    stw    r7, 0x18(r1)
    stw    r8, 0x1c(r1)
    stw    r9, 0x20(r1)
    stw    r10, 0x24(r1)
    addi   r1, r1, 0x70
    blr
}

/*
 * usr_put_initialize - Initialize user put (trace output) system.
 * No-op on GameCube.
 */
void usr_put_initialize(void) {
    /* empty */
}

/* ========================================================== */
/* Stub functions for coverage - TODO: decompile              */
/* ========================================================== */

/* fn_800C4F34 - 0x800C4F34 | size: 0x70
 * atexit - Register an exit handler function.
 * On first call, initializes the handler table (0x34 bytes).
 * Then adds the function via fn_800C4FA4.
 */
void fn_800C4F34(void* handler) {
    extern u8 lbl_803FFBB8[];
    extern u8 lbl_8047AA08;
    extern void fn_800C4FA4(u8* table, void* handler);

    __begin_critical_region(1);

    if (lbl_8047AA08 == 0) {
        memset(lbl_803FFBB8, 0, 0x34);
        lbl_8047AA08 = 1;
    }

    fn_800C4FA4(lbl_803FFBB8, handler);
    __end_critical_region(1);
}

/* ----------------------------------------------------------------
 * MSL MSL_C alloc.c — variable / fixed pool deallocator.
 * Genuine MetroWerks Standard Library source, wired to Colosseum's
 * own symbol names (fn_800C4FA4 = __pool_free,
 * fn_800C4FFC = deallocate_from_fixed_pools,
 * fn_800C5154 = deallocate_from_var_pools, __sys_free = __sys_free).
 * ---------------------------------------------------------------- */

typedef struct Block {
    struct Block* prev;
    struct Block* next;
    unsigned long max_size;
    unsigned long size;
} Block;

typedef struct SubBlock {
    unsigned long size;
    Block* block;
    struct SubBlock* prev;
    struct SubBlock* next;
} SubBlock;

struct FixSubBlock;

typedef struct FixBlock {
    struct FixBlock* prev_;
    struct FixBlock* next_;
    unsigned long client_size_;
    struct FixSubBlock* start_;
    unsigned long n_allocated_;
} FixBlock;

typedef struct FixSubBlock {
    FixBlock* block_;
    struct FixSubBlock* next_;
} FixSubBlock;

typedef struct FixStart {
    FixBlock* tail_;
    FixBlock* head_;
} FixStart;

typedef struct __mem_pool_obj {
    Block* start_;
    FixStart fix_start[6];
} __mem_pool_obj;

/* fix_pool_sizes table lives at a fixed ROM data address (lbl_8026FEE8):
 * {4, 12, 20, 36, 52, 68}. Reference the genuine label so the relocation
 * targets the original data symbol rather than a fresh local copy. */
extern const unsigned long lbl_8026FEE8[];
#define fix_pool_sizes lbl_8026FEE8

extern void __sys_free(void* bp);    /* __sys_free */

#define SubBlock_size(ths) ((ths)->size & 0xFFFFFFF8)
#define SubBlock_block(ths) ((Block*)((unsigned long)((ths)->block) & ~0x1))
#define Block_size(ths) ((ths)->size & 0xFFFFFFF8)
#define Block_start(ths) (*(SubBlock**)((char*)(ths) + Block_size((ths)) - sizeof(unsigned long)))

#define SubBlock_set_free(ths)                                                                     \
    unsigned long this_size = SubBlock_size((ths));                                                \
    (ths)->size &= ~0x2;                                                                           \
    *(unsigned long*)((char*)(ths) + this_size) &= ~0x4;                                           \
    *(unsigned long*)((char*)(ths) + this_size - sizeof(unsigned long)) = this_size

#define SubBlock_is_free(ths) !((ths)->size & 2)
#define SubBlock_set_size(ths, sz)                                                                 \
    (ths)->size &= ~0xFFFFFFF8;                                                                    \
    (ths)->size |= (sz) & 0xFFFFFFF8;                                                              \
    if (SubBlock_is_free((ths)))                                                                   \
    *(unsigned long*)((char*)(ths) + (sz) - sizeof(unsigned long)) = (sz)

#define SubBlock_from_pointer(ptr) ((SubBlock*)((char*)(ptr)-8))
#define FixSubBlock_from_pointer(ptr) ((FixSubBlock*)((char*)(ptr)-4))

#define FixBlock_client_size(ths) ((ths)->client_size_)
#define FixSubBlock_size(ths) (FixBlock_client_size((ths)->block_))

#define classify(ptr) (*(unsigned long*)((char*)(ptr) - sizeof(unsigned long)) & 1)
#define __msize_inline(ptr)                                                                        \
    (!classify(ptr) ? FixSubBlock_size(FixSubBlock_from_pointer(ptr)) :                            \
                      SubBlock_size(SubBlock_from_pointer(ptr)) - 8)

#define Block_empty(ths)                                                                           \
    (_sb = (SubBlock*)((char*)(ths) + 16)),                                                        \
        SubBlock_is_free(_sb) && SubBlock_size(_sb) == Block_size((ths)) - 24

static inline SubBlock* SubBlock_merge_prev(SubBlock* ths, SubBlock** start) {
    unsigned long prevsz;
    SubBlock* p;

    if (!(ths->size & 0x04)) {
        prevsz = *(unsigned long*)((char*)ths - sizeof(unsigned long));
        if (prevsz & 0x2)
            return ths;
        p = (SubBlock*)((char*)ths - prevsz);
        SubBlock_set_size(p, prevsz + SubBlock_size(ths));

        if (*start == ths)
            *start = (*start)->next;
        ths->next->prev = ths->prev;
        ths->next->prev->next = ths->next;
        return p;
    }
    return ths;
}

static inline void SubBlock_merge_next(SubBlock* pBlock, SubBlock** pStart) {
    SubBlock* next_sub_block;
    unsigned long this_cur_size;

    next_sub_block = (SubBlock*)((char*)pBlock + (pBlock->size & 0xFFFFFFF8));

    if (!(next_sub_block->size & 2)) {
        this_cur_size = (pBlock->size & 0xFFFFFFF8) + (next_sub_block->size & 0xFFFFFFF8);

        pBlock->size &= ~0xFFFFFFF8;
        pBlock->size |= this_cur_size & 0xFFFFFFF8;

        if (!(pBlock->size & 2)) {
            *(unsigned long*)((char*)(pBlock) + (this_cur_size)-4) = (this_cur_size);
        }

        if (!(pBlock->size & 2)) {
            *(unsigned long*)((char*)pBlock + this_cur_size) &= ~4;
        } else {
            *(unsigned long*)((char*)pBlock + this_cur_size) |= 4;
        }

        if (*pStart == next_sub_block) {
            *pStart = (*pStart)->next;
        }

        if (*pStart == next_sub_block) {
            *pStart = 0;
        }

        next_sub_block->next->prev = next_sub_block->prev;
        next_sub_block->prev->next = next_sub_block->next;
    }
}

static inline void Block_link(Block* ths, SubBlock* sb) {
    SubBlock** st;
    SubBlock_set_free(sb);
    st = &Block_start(ths);

    if (*st != 0) {
        sb->prev = (*st)->prev;
        sb->prev->next = sb;
        sb->next = *st;
        (*st)->prev = sb;
        *st = sb;
        *st = SubBlock_merge_prev(*st, st);
        SubBlock_merge_next(*st, st);
    } else {
        *st = sb;
        sb->prev = sb;
        sb->next = sb;
    }
    if (ths->max_size < SubBlock_size(*st))
        ths->max_size = SubBlock_size(*st);
}

static inline Block* __unlink(__mem_pool_obj* pool_obj, Block* bp) {
    Block* result = bp->next;
    if (result == bp) {
        result = 0;
    }

    if (pool_obj->start_ == bp) {
        pool_obj->start_ = result;
    }

    if (result != 0) {
        result->prev = bp->prev;
        result->prev->next = result;
    }

    bp->next = 0;
    bp->prev = 0;
    return result;
}

void fn_800C4FFC(__mem_pool_obj* pool_obj, void* ptr, unsigned long size);
void fn_800C5154(__mem_pool_obj* pool_obj, void* ptr);

/* fn_800C4FA4 - 0x800C4FA4 | size: 0x58 — __pool_free */
void fn_800C4FA4(void* pool, void* ptr) {
    __mem_pool_obj* pool_obj;
    unsigned long size;

    if (ptr == 0) {
        return;
    }

    pool_obj = (__mem_pool_obj*)pool;
    size = __msize_inline(ptr);

    if (size <= 68) {
        fn_800C4FFC(pool_obj, ptr, size);
    } else {
        fn_800C5154(pool_obj, ptr);
    }
}

/* fn_800C4FFC - 0x800C4FFC | size: 0x158 — deallocate_from_fixed_pools */
void fn_800C4FFC(__mem_pool_obj* pool_obj, void* ptr, unsigned long size) {
    unsigned long i = 0;
    FixSubBlock* p;
    FixBlock* b;
    FixStart* fs;

    while (size > fix_pool_sizes[i]) {
        ++i;
    }

    fs = &pool_obj->fix_start[i];
    p = FixSubBlock_from_pointer(ptr);
    b = p->block_;

    if (b->start_ == 0 && fs->head_ != b) {
        if (fs->tail_ == b) {
            fs->head_ = fs->head_->prev_;
            fs->tail_ = fs->tail_->prev_;
        } else {
            b->prev_->next_ = b->next_;
            b->next_->prev_ = b->prev_;
            b->next_ = fs->head_;
            b->prev_ = b->next_->prev_;
            b->prev_->next_ = b;
            b->next_->prev_ = b;
            fs->head_ = b;
        }
    }

    p->next_ = b->start_;
    b->start_ = p;

    if (--b->n_allocated_ == 0) {
        if (fs->head_ == b) {
            fs->head_ = b->next_;
        }

        if (fs->tail_ == b) {
            fs->tail_ = b->prev_;
        }

        b->prev_->next_ = b->next_;
        b->next_->prev_ = b->prev_;

        if (fs->head_ == b) {
            fs->head_ = 0;
        }

        if (fs->tail_ == b) {
            fs->tail_ = 0;
        }

        fn_800C5154(pool_obj, b);
    }
}

/* fn_800C5154 - 0x800C5154 | size: 0x294 — deallocate_from_var_pools */
void fn_800C5154(__mem_pool_obj* pool_obj, void* ptr) {
    SubBlock* sb = SubBlock_from_pointer(ptr);
    SubBlock* _sb;

    Block* bp = SubBlock_block(sb);
    Block_link(bp, sb);

    if (Block_empty(bp)) {
        __unlink(pool_obj, bp);
        __sys_free(bp);
    }
}

/* ----------------------------------------------------------------
 * MSL MSL_C ansi_files.c — __flush_all.
 * Walks the __files FILE list and fflush()es every open file.
 * (fflush; FILE layout from MSL ansi_files.h.)
 * ---------------------------------------------------------------- */

enum __file_kinds {
    __closed_file,
    __disk_file,
    __console_file,
    __unavailable_file
};

typedef struct _file_modes {
    unsigned int open_mode : 2;
    unsigned int io_mode : 3;
    unsigned int buffer_mode : 2;
    unsigned int file_kind : 3;
    unsigned int file_orientation : 2;
    unsigned int binary_io : 1;
} file_modes;

typedef struct _MSL_FILE {
    /* 0x00 */ unsigned long handle;
    /* 0x04 */ file_modes file_mode;
    /* 0x08 */ unsigned long file_state;
    /* 0x0C */ unsigned char is_dynamically_allocated;
    /* 0x0D */ char char_buffer;
    /* 0x0E */ char char_buffer_overflow;
    /* 0x0F */ char ungetc_buffer[2];
    /* 0x12 */ unsigned short ungetc_wide_buffer[2];
    /* 0x18 */ unsigned long position;
    /* 0x1C */ unsigned char* buffer;
    /* 0x20 */ unsigned long buffer_size;
    /* 0x24 */ unsigned char* buffer_ptr;
    /* 0x28 */ unsigned long buffer_length;
    /* 0x2C */ unsigned long buffer_alignment;
    /* 0x30 */ unsigned long save_buffer_length;
    /* 0x34 */ unsigned long buffer_position;
    /* 0x38 */ void* position_fn;
    /* 0x3C */ void* read_fn;
    /* 0x40 */ void* write_fn;
    /* 0x44 */ void* close_fn;
    /* 0x48 */ void* idle_fn;
    /* 0x4C */ struct _MSL_FILE* next_file;
} MSL_FILE;

extern MSL_FILE __files;        /* &__files._stdin == &__files */
extern int fflush(MSL_FILE* file);

/* fn_800C53E8 - 0x800C53E8 | size: 0x70 — __flush_all */
unsigned int fn_800C53E8(void) {
    unsigned int ret = 0;
    MSL_FILE* file = &__files;

    while (file) {
        if (file->file_mode.file_kind != __closed_file && fflush(file)) {
            ret = -1;
        }
        file = file->next_file;
    }

    return ret;
}

