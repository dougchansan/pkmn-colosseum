#include "dolphin/types.h"

/* tolower - 0x800C7558 | size: 0x24 */
s32 tolower(s32 ch) {
    extern u8 lbl_80313C18[];

    if (ch == -1) {
        return -1;
    }
    return lbl_80313C18[(u8)ch];
}

enum file_kind {
    FILE_KIND_CLOSED,
    FILE_KIND_DISK,
    FILE_KIND_CONSOLE,
    FILE_KIND_UNAVAILABLE
};

enum io_state {
    IO_STATE_NEUTRAL,
    IO_STATE_WRITING,
    IO_STATE_READING,
    IO_STATE_REREADING
};

enum io_mode {
    IO_MODE_READ = 1,
    IO_MODE_WRITE = 2,
    IO_MODE_READ_WRITE = 3,
    IO_MODE_APPEND = 4
};

enum buffer_mode {
    BUFFER_MODE_NONE,
    BUFFER_MODE_LINE,
    BUFFER_MODE_FULL
};

enum io_result {
    IO_RESULT_OK,
    IO_RESULT_ERROR,
    IO_RESULT_EOF
};

typedef struct file_modes {
    u32 open_mode : 2;
    u32 io_mode : 3;
    u32 buffer_mode : 2;
    u32 file_kind : 3;
    u32 file_orientation : 2;
    u32 binary_io : 1;
} file_modes;

typedef struct file_states {
    u32 io_state : 3;
    u32 free_buffer : 1;
    u8 eof;
    u8 error;
} file_states;

typedef void (*MSLIdleFn)(void);
typedef s32 (*MSLPositionFn)(u32 handle, u32* position, s32 origin, MSLIdleFn idle_fn);
typedef s32 (*MSLCloseFn)(u32 handle);

typedef struct _MSL_FILE {
    /* 0x00 */ u32 handle;
    /* 0x04 */ file_modes file_mode;
    /* 0x08 */ file_states file_state;
    /* 0x0C */ u8 is_dynamically_allocated;
    /* 0x0D */ char char_buffer;
    /* 0x0E */ char char_buffer_overflow;
    /* 0x0F */ char ungetc_buffer[2];
    /* 0x12 */ u16 ungetc_wide_buffer[2];
    /* 0x18 */ u32 position;
    /* 0x1C */ u8* buffer;
    /* 0x20 */ u32 buffer_size;
    /* 0x24 */ u8* buffer_ptr;
    /* 0x28 */ u32 buffer_length;
    /* 0x2C */ u32 buffer_alignment;
    /* 0x30 */ u32 save_buffer_length;
    /* 0x34 */ u32 buffer_position;
    /* 0x38 */ MSLPositionFn position_fn;
    /* 0x3C */ void* read_fn;
    /* 0x40 */ void* write_fn;
    /* 0x44 */ MSLCloseFn close_fn;
    /* 0x48 */ MSLIdleFn idle_fn;
    /* 0x4C */ struct _MSL_FILE* next_file;
} MSL_FILE;

extern void __stdio_atexit(void);
extern void __prep_buffer(MSL_FILE* file);
extern s32 __flush_buffer(MSL_FILE* file, u32* bytes_flushed);
extern u32 __flush_all(void);
extern void* __memrchr(const void* buffer, s32 ch, u32 count);
extern void* memcpy(void* destination, const void* source, u32 count);
extern void free(void* pointer);
extern s32 fwide(MSL_FILE* file, s32 mode);
extern s32 fseek(MSL_FILE* file, s32 offset, s32 origin);
extern s32 ftell(MSL_FILE* file);
extern s32 fflush(MSL_FILE* file);

u32 __fwrite(const void* buffer, u32 size, u32 count, MSL_FILE* file) {
    s32 always_buffer;
    s32 is_text_or_full_buffered;
    u8* write_ptr;
    u32 num_bytes;
    u32 bytes_to_go;
    u32 bytes_written;
    s32 ioresult;

    if (fwide(file, 0) == 0) {
        fwide(file, -1);
    }

    bytes_to_go = size * count;

    if (!bytes_to_go || file->file_state.error || file->file_mode.file_kind == FILE_KIND_CLOSED) {
        return 0;
    }

    if (file->file_mode.file_kind == FILE_KIND_CONSOLE) {
        __stdio_atexit();
    }

    always_buffer = 1;
    is_text_or_full_buffered =
        !file->file_mode.binary_io || file->file_mode.buffer_mode == BUFFER_MODE_FULL;
    if (!is_text_or_full_buffered && file->file_mode.buffer_mode != BUFFER_MODE_LINE) {
        always_buffer = 0;
    }

    if (file->file_state.io_state == IO_STATE_NEUTRAL) {
        if (file->file_mode.io_mode & IO_MODE_WRITE) {
            if (file->file_mode.io_mode & IO_MODE_APPEND) {
                if (fseek(file, 0, 2)) {
                    return 0;
                }
            }
            file->file_state.io_state = IO_STATE_WRITING;
            __prep_buffer(file);
        }
    }

    if (file->file_state.io_state != IO_STATE_WRITING) {
        file->file_state.error = 1;
        file->buffer_length = 0;
        return 0;
    }

    write_ptr = (u8*)buffer;
    bytes_written = 0;

    if (bytes_to_go && (file->buffer_ptr != file->buffer || always_buffer)) {
        file->buffer_length = file->buffer_size - (file->buffer_ptr - file->buffer);

        do {
            u8* newline = NULL;

            num_bytes = file->buffer_length;
            if (num_bytes > bytes_to_go) {
                num_bytes = bytes_to_go;
            }
            if (file->file_mode.buffer_mode == BUFFER_MODE_LINE && num_bytes) {
                if ((newline = (u8*)__memrchr(write_ptr, '\n', num_bytes)) != NULL) {
                    num_bytes = newline + 1 - write_ptr;
                }
            }

            if (num_bytes) {
                memcpy(file->buffer_ptr, write_ptr, num_bytes);
                write_ptr += num_bytes;
                bytes_written += num_bytes;
                bytes_to_go -= num_bytes;
                file->buffer_ptr += num_bytes;
                file->buffer_length -= num_bytes;
            }

            if (!file->buffer_length || newline != NULL || file->file_mode.buffer_mode == BUFFER_MODE_NONE) {
                ioresult = __flush_buffer(file, NULL);
                if (ioresult) {
                    file->file_state.error = 1;
                    file->buffer_length = 0;
                    bytes_to_go = 0;
                    break;
                }
            }
        } while (bytes_to_go && always_buffer);
    }

    if (bytes_to_go && !always_buffer) {
        u8* save_buffer = file->buffer;
        u32 save_size = file->buffer_size;

        file->buffer = write_ptr;
        file->buffer_size = bytes_to_go;
        file->buffer_ptr = write_ptr + bytes_to_go;

        if (__flush_buffer(file, &num_bytes) != IO_RESULT_OK) {
            file->file_state.error = 1;
            file->buffer_length = 0;
        }

        bytes_written += num_bytes;
        file->buffer = save_buffer;
        file->buffer_size = save_size;
        __prep_buffer(file);
        file->buffer_length = 0;
    }

    if (file->file_mode.buffer_mode != BUFFER_MODE_FULL) {
        file->buffer_length = 0;
    }

    return (bytes_written + size - 1) / size;
}

u32 fwrite(const void* ptr, u32 size, u32 count, MSL_FILE* stream) {
    extern void __begin_critical_region(s32 region);
    extern void __end_critical_region(s32 region);
    extern u32 __fwrite(const void* ptr, u32 size, u32 count, MSL_FILE* stream);
    u32 result;

    __begin_critical_region(2);
    result = __fwrite(ptr, size, count, stream);
    __end_critical_region(2);
    return result;
}

s32 fflush(MSL_FILE* file) {
    s32 position;

    if (file == NULL) {
        return __flush_all();
    }

    if (file->file_state.error || file->file_mode.file_kind == FILE_KIND_CLOSED) {
        return -1;
    }

    if (file->file_mode.io_mode == IO_MODE_READ) {
        return 0;
    }

    if (file->file_state.io_state >= IO_STATE_REREADING) {
        file->file_state.io_state = IO_STATE_READING;
    }

    if (file->file_state.io_state == IO_STATE_READING) {
        file->buffer_length = 0;
    }

    if (file->file_state.io_state != IO_STATE_WRITING) {
        file->file_state.io_state = IO_STATE_NEUTRAL;
        return 0;
    }

    if (file->file_mode.file_kind != FILE_KIND_DISK) {
        position = 0;
    } else {
        position = ftell(file);
    }

    if (__flush_buffer(file, NULL) != IO_RESULT_OK) {
        file->file_state.error = 1;
        file->buffer_length = 0;
        return -1;
    }

    file->file_state.io_state = IO_STATE_NEUTRAL;
    file->position = position;
    file->buffer_length = 0;
    return 0;
}

s32 fclose(MSL_FILE* file) {
    s32 flush_result;
    s32 close_result;

    if (file == NULL) {
        return -1;
    }

    if (file->file_mode.file_kind == FILE_KIND_CLOSED) {
        return 0;
    }

    flush_result = fflush(file);
    close_result = file->close_fn(file->handle);
    file->file_mode.file_kind = FILE_KIND_CLOSED;
    file->handle = 0;

    if (file->file_state.free_buffer) {
        free(file->buffer);
    }

    return (flush_result || close_result) ? -1 : 0;
}

s32 fseek(MSL_FILE* stream, s32 offset, s32 origin) {
    extern void __begin_critical_region(s32 region);
    extern void __end_critical_region(s32 region);
    extern s32 _fseek(MSL_FILE* stream, s32 offset, s32 origin);
    s32 result;

    __begin_critical_region(2);
    result = _fseek(stream, offset, origin);
    __end_critical_region(2);
    return result;
}

inline s32 _ftell(MSL_FILE* file) {
    extern s32 lbl_8047AA10;
    s32 chars_in_undo_buffer = 0;
    s32 position;
    u8 kind = file->file_mode.file_kind;

    if (!((kind == FILE_KIND_DISK) || (kind == FILE_KIND_CONSOLE)) || file->file_state.error) {
        lbl_8047AA10 = 0x28;
        return -1;
    }

    if (file->file_state.io_state == IO_STATE_NEUTRAL) {
        return file->position;
    }

    position = file->buffer_position + (file->buffer_ptr - file->buffer);

    if (file->file_state.io_state >= IO_STATE_REREADING) {
        chars_in_undo_buffer = file->file_state.io_state - IO_STATE_REREADING + 1;
        position -= chars_in_undo_buffer;
    }

    if (!file->file_mode.binary_io) {
        s32 count = file->buffer_ptr - file->buffer - chars_in_undo_buffer;
        u8* ptr = file->buffer;

        while (count--) {
            if (*ptr++ == '\n') {
                position++;
            }
        }
    }

    return position;
}

s32 _fseek(MSL_FILE* file, u32 offset, s32 origin) {
    s32 position;

    if (file->file_mode.file_kind != FILE_KIND_DISK || file->file_state.error) {
        extern s32 lbl_8047AA10;
        lbl_8047AA10 = 0x28;
        return -1;
    }

    if (file->file_state.io_state == IO_STATE_WRITING) {
        if (__flush_buffer(file, NULL) != IO_RESULT_OK) {
            extern s32 lbl_8047AA10;
            file->file_state.error = 1;
            file->buffer_length = 0;
            lbl_8047AA10 = 0x28;
            return -1;
        }
    }

    if (origin == 1) {
        origin = 0;
        position = _ftell(file);
        offset += position;
    }

    if (origin != 2 && file->file_mode.io_mode != IO_MODE_READ_WRITE &&
        (file->file_state.io_state == IO_STATE_READING || file->file_state.io_state == IO_STATE_REREADING)) {
        if (offset >= file->position || offset < file->buffer_position) {
            file->file_state.io_state = IO_STATE_NEUTRAL;
        } else {
            file->buffer_ptr = file->buffer + (offset - file->buffer_position);
            file->buffer_length = file->position - offset;
            file->file_state.io_state = IO_STATE_READING;
        }
    } else {
        file->file_state.io_state = IO_STATE_NEUTRAL;
    }

    if (file->file_state.io_state == IO_STATE_NEUTRAL) {
        if (file->position_fn != NULL && file->position_fn(file->handle, &offset, origin, file->idle_fn)) {
            extern s32 lbl_8047AA10;
            file->file_state.error = 1;
            file->buffer_length = 0;
            lbl_8047AA10 = 0x28;
            return -1;
        }

        file->file_state.eof = 0;
        file->position = offset;
        file->buffer_length = 0;
    }

    return 0;
}

s32 ftell(MSL_FILE* stream) {
    extern void __begin_critical_region(s32 region);
    extern void __end_critical_region(s32 region);
    s32 result;

    __begin_critical_region(2);
    result = _ftell(stream);
    __end_critical_region(2);
    return result;
}
