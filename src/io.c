#define SCU_SHORT_ALIASES

#ifndef _POSIX_C_SOURCE
    #define _POSIX_C_SOURCE 200809L
#endif
#ifndef _FILE_OFFSET_BITS
    #define _FILE_OFFSET_BITS 64
#endif

#include <pthread.h>
#include <stdio.h>
#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/io.h"
#include "scu/memory.h"
#include "scu/string.h"

struct ScuFile {

    /** @brief The underlying file stream handle. */
    FILE* handle;

};

/** @brief The factor used when growing dynamically allocated buffers. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

/** @brief The chunk size used when reading from a file stream. */
static constexpr isize SCU_CHUNK_SIZE = 4096;

/**
 * @brief A flag to ensure the file stream associated with the standard input
 * stream is only initialized once.
 */
static pthread_once_t scuStdinInitialized = PTHREAD_ONCE_INIT;

/** @brief The file stream associated with the standard input stream. */
static ScuFile scuStdin;

/**
 * @brief A flag to ensure the file stream associated with the standard output
 * stream is only initialized once.
 */
static pthread_once_t scuStdoutInitialized = PTHREAD_ONCE_INIT;

/** @brief The file stream associated with the standard output stream. */
static ScuFile scuStdout;

/**
 * @brief A flag to ensure the file stream associated with the standard error
 * stream is only initialized once.
 */
static pthread_once_t scuStderrInitialized = PTHREAD_ONCE_INIT;

/** @brief The file stream associated with the standard error stream. */
static ScuFile scuStderr;

/**
 * @brief Initializes the file stream associated with the standard input stream.
 */
static void scu_init_stdin() {
    scuStdin.handle = stdin;
}

/**
 * @brief Initializes the file stream associated with the standard output
 * stream.
 */
static void scu_init_stdout() {
    scuStdout.handle = stdout;
}

/**
 * @brief Initializes the file stream associated with the standard error stream.
 */
static void scu_init_stderr() {
    scuStderr.handle = stderr;
}

ScuFile* scu_stdin() {
    pthread_once(&scuStdinInitialized, scu_init_stdin);
    return &scuStdin;
}

ScuFile* scu_stdout() {
    pthread_once(&scuStdoutInitialized, scu_init_stdout);
    return &scuStdout;
}

ScuFile* scu_stderr() {
    pthread_once(&scuStderrInitialized, scu_init_stderr);
    return &scuStderr;
}

ScuError scu_fopen(
    ScuFile* restrict* restrict file,
    const char* restrict name,
    const char* restrict mode
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(name != nullptr);
    SCU_ASSERT(mode != nullptr);
    FILE* handle = fopen(name, mode);
    if (handle == nullptr) {
        *file = nullptr;
        return SCU_ERROR_OPENING_FILE;
    }
    *file = scu_malloc(SCU_SIZEOF(ScuFile));
    if (*file == nullptr) {
        fclose(handle);
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    (*file)->handle = handle;
    return SCU_ERROR_NONE;
}

ScuError scu_fopentmp(ScuFile** file) {
    SCU_ASSERT(file != nullptr);
    FILE* handle = tmpfile();
    if (handle == nullptr) {
        *file = nullptr;
        return SCU_ERROR_OPENING_FILE;
    }
    *file = scu_malloc(SCU_SIZEOF(ScuFile));
    if (*file == nullptr) {
        fclose(handle);
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    (*file)->handle = handle;
    return SCU_ERROR_NONE;
}

ScuError scu_freopen(
    ScuFile* restrict file,
    const char* restrict name,
    const char* restrict mode
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(mode != nullptr);
    FILE* handle = freopen(name, mode, file->handle);
    if (handle == nullptr) {
        return SCU_ERROR_OPENING_FILE;
    }
    file->handle = handle;
    return SCU_ERROR_NONE;
}

ScuError scu_fclose(ScuFile* file) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    int result = fclose(file->handle);
    file->handle = nullptr;
    scu_free(file);
    return (result == EOF) ? SCU_ERROR_CLOSING_FILE : SCU_ERROR_NONE;
}

ScuError scu_fflush(ScuFile* file) {
    if (file == nullptr) {
        return (fflush(nullptr) == EOF)
            ? SCU_ERROR_FLUSHING_FILE
            : SCU_ERROR_NONE;
    }
    SCU_ASSERT(file->handle != nullptr);
    return (fflush(file->handle) == EOF)
        ? SCU_ERROR_FLUSHING_FILE
        : SCU_ERROR_NONE;
}

ScuError scu_fremove(const char* name) {
    SCU_ASSERT(name != nullptr);
    return (remove(name) != 0) ? SCU_ERROR_REMOVING_FILE : SCU_ERROR_NONE;
}

ScuError scu_frename(const char* oldName, const char* newName) {
    SCU_ASSERT(oldName != nullptr);
    SCU_ASSERT(newName != nullptr);
    return (rename(oldName, newName) != 0)
        ? SCU_ERROR_RENAMING_FILE
        : SCU_ERROR_NONE;
}

isize scu_fread(
    ScuFile* restrict file,
    void* restrict buffer,
    isize count,
    isize size
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(count >= 0);
    SCU_ASSERT(size >= 0);
    if ((count == 0) || (size == 0)) {
        return 0;
    }
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size <= (INT64_MAX / count));
    return (isize) fread(buffer, (usize) size, (usize) count, file->handle);
}

isize scu_read(void* buffer, isize count, isize size) {
    return scu_fread(SCU_STDIN, buffer, count, size);
}

isize scu_fwrite(
    ScuFile* restrict file,
    const void* restrict buffer,
    isize count,
    isize size
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(count >= 0);
    SCU_ASSERT(size >= 0);
    if ((count == 0) || (size == 0)) {
        return 0;
    }
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size <= (INT64_MAX / count));
    return (isize) fwrite(buffer, (usize) size, (usize) count, file->handle);
}

isize scu_write(const void* buffer, isize count, isize size) {
    return scu_fwrite(SCU_STDOUT, buffer, count, size);
}

ScuError scu_freadc(ScuFile* restrict file, char* restrict c) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(c != nullptr);
    int result = fgetc(file->handle);
    if (result == EOF) {
        *c = '\0';
        return feof(file->handle)
            ? SCU_ERROR_END_OF_FILE
            : SCU_ERROR_READING_FILE;
    }
    *c = (char) result;
    return SCU_ERROR_NONE;
}

ScuError scu_readc(char* c) {
    return scu_freadc(SCU_STDIN, c);
}

ScuError scu_fwritec(ScuFile* file, char c) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    return (fputc(c, file->handle) == EOF)
        ? SCU_ERROR_WRITING_FILE
        : SCU_ERROR_NONE;
}

ScuError scu_writec(char c) {
    return scu_fwritec(SCU_STDOUT, c);
}

ScuError scu_funreadc(ScuFile* file, char c) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    return (ungetc(c, file->handle) == EOF)
        ? SCU_ERROR_WRITING_FILE
        : SCU_ERROR_NONE;
}

ScuError scu_freads(ScuFile* restrict file, char* restrict buffer, isize size) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(size >= 0);
    if (size == 0) {
        return SCU_ERROR_NONE;
    }
    SCU_ASSERT(buffer != nullptr);
    isize read = (isize) fread(
        buffer,
        sizeof(char),
        (usize) (size - 1),
        file->handle
    );
    buffer[read] = '\0';
    if (read < (size - 1)) {
        if (feof(file->handle)) {
            return (read == 0) ? SCU_ERROR_END_OF_FILE : SCU_ERROR_NONE;
        }
        return SCU_ERROR_READING_FILE;
    }
    return SCU_ERROR_NONE;
}

ScuError scu_reads(char* buffer, isize size) {
    return scu_freads(SCU_STDIN, buffer, size);
}

ScuError scu_fwrites(ScuFile* restrict file, const char* restrict buffer) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(buffer != nullptr);
    isize n = scu_strlen(buffer);
    isize written = (isize) fwrite(
        buffer,
        sizeof(char),
        (usize) n,
        file->handle
    );
    return (written < n) ? SCU_ERROR_WRITING_FILE : SCU_ERROR_NONE;
}

ScuError scu_writes(const char* buffer) {
    return scu_fwrites(SCU_STDOUT, buffer);
}

/**
 * @brief Ensures that a specified buffer has at least a required size.
 *
 * This function checks if the buffer pointed to by `*buffer` has a size of at
 * least `requiredSize` bytes (determined by comparing against `*size`). If that
 * is not the case, it reallocates the buffer using `scu_realloc()` to ensure it
 * has at least the required size. The new buffer is then stored in `*buffer`,
 * and its new size is reflected in `*size`.
 *
 * @note If `*size` is zero, `*buffer` must be a `nullptr` (and vice versa). In
 * this case, the function allocates a buffer using `scu_realloc()` if
 * `requiredSize` is greater than zero.
 *
 * @warning If `*buffer` is not a `nullptr`, it must have been allocated using
 * `scu_malloc()`, `scu_calloc()` or `scu_realloc()` (or the underlying
 * allocator used by these functions). The caller is expected to free `*buffer`
 * with `scu_free()` when it is no longer needed.
 *
 * @param[in, out] buffer       A pointer to a buffer that may need to be
 *                              reallocated.
 * @param[in, out] size         A pointer to the size of the buffer (in bytes).
 * @param[in]      requiredSize The required size of the buffer (in bytes).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 */
static inline ScuError scu_ensure_size(
    char* restrict* restrict buffer,
    isize* restrict size,
    isize requiredSize
) {
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size != nullptr);
    SCU_ASSERT((*buffer == nullptr) == (*size == 0));
    SCU_ASSERT(*size >= 0);
    SCU_ASSERT(requiredSize >= 0);
    if (requiredSize > *size) {
        isize newSize = (*size == 0) ? 1 : *size;
        while (newSize < requiredSize) {
            newSize *= SCU_GROWTH_FACTOR;
        }
        char* newBuffer = scu_realloc(*buffer, newSize * SCU_SIZEOF(char));
        if (newBuffer == nullptr) {
            return SCU_ERROR_OUT_OF_MEMORY;
        }
        *buffer = newBuffer;
        *size = newSize;
    }
    return SCU_ERROR_NONE;
}

ScuError scu_freadln(
    ScuFile* restrict file,
    char* restrict* restrict buffer,
    isize* restrict size
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size != nullptr);
    SCU_ASSERT((*buffer == nullptr) == (*size == 0));
    SCU_ASSERT(*size >= 0);
    isize used = 0;
    while (true) {
        ScuError error = scu_ensure_size(
            buffer,
            size,
            (used + SCU_CHUNK_SIZE) * SCU_SIZEOF(char)
        );
        if (error != SCU_ERROR_NONE) {
            return error;
        }
        isize available = *size - used;
        if (fgets(*buffer + used, (int) available, file->handle) == nullptr) {
            (*buffer)[used] = '\0';
            if (ferror(file->handle)) {
                return SCU_ERROR_READING_FILE;
            }
            return (used == 0) ? SCU_ERROR_END_OF_FILE : SCU_ERROR_NONE;
        }
        isize read = scu_strlen(*buffer + used);
        used += read;
        if (((*buffer)[used - 1] == '\n') || (read < (available - 1))) {
            return SCU_ERROR_NONE;
        }
    }
}

ScuError scu_readln(char* restrict* restrict buffer, isize* restrict size) {
    return scu_freadln(SCU_STDIN, buffer, size);
}

ScuError scu_fwriteln(ScuFile* restrict file, const char* restrict buffer) {
    ScuError error = scu_fwrites(file, buffer);
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    return scu_fwritec(file, '\n');
}

ScuError scu_writeln(const char* buffer) {
    return scu_fwriteln(SCU_STDOUT, buffer);
}

ScuError scu_freadall(
    ScuFile* restrict file,
    char* restrict* restrict buffer,
    isize* restrict size
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size != nullptr);
    SCU_ASSERT((*buffer == nullptr) == (*size == 0));
    SCU_ASSERT(*size >= 0);
    isize used = 0;
    while (true) {
        ScuError error = scu_ensure_size(
            buffer,
            size,
            (used + SCU_CHUNK_SIZE) * SCU_SIZEOF(char)
        );
        if (error != SCU_ERROR_NONE) {
            return error;
        }
        isize available = *size - used - 1;
        isize read = (isize) fread(
            *buffer + used,
            sizeof(char),
            (usize) available,
            file->handle
        );
        used += read;
        if (read < available) {
            (*buffer)[used] = '\0';
            if (ferror(file->handle)) {
                return SCU_ERROR_READING_FILE;
            }
            return (used == 0) ? SCU_ERROR_END_OF_FILE : SCU_ERROR_NONE;
        }
    }
}

ScuError scu_readall(char* restrict* restrict buffer, isize* restrict size) {
    return scu_freadall(SCU_STDIN, buffer, size);
}

isize scu_vfscanf(
    ScuFile* restrict file,
    const char* restrict format,
    va_list args
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(format != nullptr);
    isize n = vfscanf(file->handle, format, args);
    return (n == EOF) ? -1 : n;
}

isize scu_fscanf(ScuFile* restrict file, const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    isize n = scu_vfscanf(file, format, args);
    va_end(args);
    return n;
}

isize scu_vscanf(const char* restrict format, va_list args) {
    return scu_vfscanf(SCU_STDIN, format, args);
}

isize scu_scanf(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    isize n = scu_vfscanf(SCU_STDIN, format, args);
    va_end(args);
    return n;
}

isize scu_vsscanf(
    const char* restrict buffer,
    const char* restrict format,
    va_list args
) {
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(format != nullptr);
    isize n = vsscanf(buffer, format, args);
    return (n == EOF) ? -1 : n;
}

isize scu_sscanf(
    const char* restrict buffer,
    const char* restrict format,
    ...
) {
    va_list args;
    va_start(args, format);
    isize n = scu_vsscanf(buffer, format, args);
    va_end(args);
    return n;
}

isize scu_vfprintf(
    ScuFile* restrict file,
    const char* restrict format,
    va_list args
) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(format != nullptr);
    isize n = vfprintf(file->handle, format, args);
    return (n < 0) ? -1 : n;
}

isize scu_fprintf(ScuFile* restrict file, const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    isize n = scu_vfprintf(file, format, args);
    va_end(args);
    return n;
}

isize scu_vprintf(const char* restrict format, va_list args) {
    return scu_vfprintf(SCU_STDOUT, format, args);
}

isize scu_printf(const char* restrict format, ...) {
    va_list args;
    va_start(args, format);
    isize n = scu_vfprintf(SCU_STDOUT, format, args);
    va_end(args);
    return n;
}

isize scu_vsnprintf(
    char* restrict buffer,
    isize size,
    const char* restrict format,
    va_list args
) {
    SCU_ASSERT(size >= 0);
    SCU_ASSERT((size == 0) || (buffer != nullptr));
    SCU_ASSERT(format != nullptr);
    isize n = vsnprintf(buffer, (usize) size, format, args);
    if (n < 0) {
        if (size > 0) {
            buffer[0] = '\0';
        }
        return -1;
    }
    return n;
}

isize scu_snprintf(
    char* restrict buffer,
    isize size,
    const char* restrict format,
    ...
) {
    va_list args;
    va_start(args, format);
    isize n = scu_vsnprintf(buffer, size, format, args);
    va_end(args);
    return n;
}

ScuError scu_vrsnprintf(
    char* restrict* restrict buffer,
    isize* restrict size,
    const char* restrict format,
    va_list args
) {
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size != nullptr);
    SCU_ASSERT((*buffer == nullptr) == (*size == 0));
    SCU_ASSERT(*size >= 0);
    SCU_ASSERT(format != nullptr);
    va_list argsCopy;
    va_copy(argsCopy, args);
    isize n = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (n < 0) {
        if (*size > 0) {
            (*buffer)[0] = '\0';
        }
        return SCU_ERROR_WRITING_BUFFER;
    }
    ScuError error = scu_ensure_size(buffer, size, (n + 1) * SCU_SIZEOF(char));
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    n = vsnprintf(*buffer, (usize) *size, format, args);
    if (n < 0) {
        (*buffer)[0] = '\0';
        return SCU_ERROR_WRITING_BUFFER;
    }
    return SCU_ERROR_NONE;
}

ScuError scu_rsnprintf(
    char* restrict* restrict buffer,
    isize* restrict size,
    const char* restrict format,
    ...
) {
    va_list args;
    va_start(args, format);
    ScuError error = scu_vrsnprintf(buffer, size, format, args);
    va_end(args);
    return error;
}

ScuError scu_vrasnprintf(
    char* restrict* restrict buffer,
    isize* restrict size,
    const char* restrict format,
    va_list args
) {
    SCU_ASSERT(buffer != nullptr);
    SCU_ASSERT(size != nullptr);
    SCU_ASSERT((*buffer == nullptr) == (*size == 0));
    SCU_ASSERT(*size >= 0);
    SCU_ASSERT(format != nullptr);
    va_list argsCopy;
    va_copy(argsCopy, args);
    isize n = vsnprintf(nullptr, 0, format, argsCopy);
    va_end(argsCopy);
    if (n < 0) {
        return SCU_ERROR_WRITING_BUFFER;
    }
    isize offset = (*buffer == nullptr) ? 0 : scu_strlen(*buffer);
    ScuError error = scu_ensure_size(
        buffer,
        size,
        (offset + n + 1) * SCU_SIZEOF(char)
    );
    if (error != SCU_ERROR_NONE) {
        return error;
    }
    n = vsnprintf(*buffer + offset, (usize) (*size - offset), format, args);
    if (n < 0) {
        (*buffer)[offset] = '\0';
        return SCU_ERROR_WRITING_BUFFER;
    }
    return SCU_ERROR_NONE;
}

ScuError scu_rasnprintf(
    char* restrict* restrict buffer,
    isize* restrict size,
    const char* restrict format,
    ...
) {
    va_list args;
    va_start(args, format);
    ScuError error = scu_vrasnprintf(buffer, size, format, args);
    va_end(args);
    return error;
}

isize scu_ftell(ScuFile* file) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
#ifdef _WIN32
    return _ftelli64(file->handle);
#else
    return ftello(file->handle);
#endif
}

ScuError scu_fseek(ScuFile* file, ScuSeekOrigin seekOrigin, isize offset) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    SCU_ASSERT(
        (seekOrigin == SCU_SEEK_ORIGIN_SET)
            || (seekOrigin == SCU_SEEK_ORIGIN_CUR)
            || (seekOrigin == SCU_SEEK_ORIGIN_END)
    );
    int origin;
    switch (seekOrigin) {
        case SCU_SEEK_ORIGIN_SET:
            origin = SEEK_SET;
            break;
        case SCU_SEEK_ORIGIN_CUR:
            origin = SEEK_CUR;
            break;
        case SCU_SEEK_ORIGIN_END:
            origin = SEEK_END;
            break;
        default:
            SCU_UNREACHABLE();
    }
#ifdef _WIN32
    return (_fseeki64(file->handle, offset, origin) != 0)
        ? SCU_ERROR_SEEKING_FILE
        : SCU_ERROR_NONE;
#else
    return (fseeko(file->handle, (off_t) offset, origin) != 0)
        ? SCU_ERROR_SEEKING_FILE
        : SCU_ERROR_NONE;
#endif
}

void scu_frewind(ScuFile* file) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    rewind(file->handle);
}

void scu_fclear(ScuFile* file) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    clearerr(file->handle);
}

bool scu_feof(ScuFile* file) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    return feof(file->handle) != 0;
}

bool scu_ferror(ScuFile* file) {
    SCU_ASSERT(file != nullptr);
    SCU_ASSERT(file->handle != nullptr);
    return ferror(file->handle) != 0;
}