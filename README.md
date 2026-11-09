# SCU <!-- omit in toc -->

SCU (short for _Simple C Utilities_) is a small library of reusable utilities
for C. It includes macros and functions for a variety of common tasks, such as
assertions and error handling, memory management, string manipulation, I/O
operations, benchmarking, and more. Additionally, SCU provides a set of common
generic data structures, including a dynamic array, hash map and hash set,
stack, queue, as well as a priority queue.

```c
#define SCU_SHORT_ALIASES
#include <scu/scu.h>

int main() {
    const char* languages[] = { "C", "Rust", "C#", "C", "C++", "C", "C#" };
    ScuHashMap* frequencies = scu_hash_map_new(
        const char*,
        i32,
        scu_hash_str,
        scu_equal_str,
        scu_equal_i32
    );
    const char** language;
    SCU_ARRAY_FOREACH(language, languages) {
        i32* frequency;
        if (scu_hash_map_try_get(frequencies, language, &frequency)) {
            (*frequency)++;
        }
        else {
            scu_hash_map_add(frequencies, language, &(i32) { 1 });
        }
    }
    ScuHashMapEntry entry;
    SCU_HASH_MAP_FOREACH(entry, frequencies) {
        const char* language = *(const char**) entry.key;
        i32 frequency = *(i32*) entry.value;
        scu_printf("%s: %" I32_PRID "\n", language, frequency);
    }
    scu_hash_map_delete(frequencies);
}
```

## Table of Contents <!-- omit in toc -->

* [Motivation and Design Goals](#motivation-and-design-goals)
* [Overview](#overview)
* [Common Conventions](#common-conventions)
  * [Naming](#naming)
  * [Signed Integers](#signed-integers)
  * [Preconditions, Programming Errors and Error Handling](#preconditions-programming-errors-and-error-handling)
  * [Memory Management and Ownership](#memory-management-and-ownership)
  * [Thread Safety](#thread-safety)
* [Usage](#usage)
* [Contributing](#contributing)
* [License](#license)

## Motivation and Design Goals

I developed SCU alongside a few personal projects, as I found myself repeatedly
needing the same utilities and data structures over and over again. These
projects served as a testing ground for the library, allowing me to draft and
refine the API over time. Besides being a useful tool for my projects and the
occasional learning experience, my main motivation for developing SCU was to
improve upon the C standard library. Although widely used, it is often
criticized (rightfully so, if you ask me) for its inconsistencies and lack of
functionality. As such, my goal was to create a library that addresses many of
these issues, while still following idiomatic C practices. This is reflected
both in the API and the implementation of SCU, which in many cases provide
similar, but (in my opinion) more consistent and easier-to-use functionality by
wrapping and composing the C standard library functions.

The overall design goals of SCU can be summarized as follows:

* **Simplicity**: The library should be pleasant to use, not requiring users to
  jump through hoops to accomplish common tasks. Most importantly, it should
  pick good defaults and feature a consistent, predictable API, such that users
  can easily guess how other parts of the library work without needing to read
  the whole documentation. Some of these common conventions are described in
  more detail [below](#common-conventions). Finally, for reasons of simplicity,
  SCU should have ideally no to minimal external or platform-specific
  dependencies, which reduce the portability and usefulness of the library.

* **Efficiency**: The library should be reasonably efficient, although I'm
  certainly not claiming to be a performance expert. That said, I do try to be
  mindful of efficiency during the design and implementation, and I do make an
  effort to avoid unnecessary computations and memory allocations where
  sensible. Additionally, I try to keep the code simple and straightforward to
  enable better compiler optimizations.

* **Composability**: The utilities and data structures provided by the library
  should be composable and work well together. In particular, the individual
  modules should not have wildly different APIs or conventions that make it
  difficult to combine them for a higher-level, more complex task.

At this point, you might be asking yourself if SCU is really that much better
than the C standard library or other existing libraries that try to accomplish
similar goals. The honest answer is probably not, but I find SCU pleasant to use
and it serves my needs well, so I decided to share it in case it might be useful
to others as well. If you are interested in giving it a try, feel free to check
out the [Usage](#usage) section below for instructions on how to build and use
the library.

## Overview

At the most basic level, SCU is comprised of several modules containing related
types, macros and functions. For instance, the `io` module contains utilities
for input and output operations, while the `memory` module deals with memory
management. One notable exception is the [`scu.h`](include/scu/scu.h) header,
which serves as an umbrella header and can be used to include the entirety of
the library at once.

To give you a better idea of what SCU has to offer, here is a brief overview of
all modules and a one-sentence description of their contents:

| Module         | Contents                                                                                                                                 |
|----------------|------------------------------------------------------------------------------------------------------------------------------------------|
| `api.h`        | `SCU_API` macro for controlling the symbol visibility.                                                                                   |
| `array.h`      | Utilities for arrays, including the ubiquitous `SCU_COUNTOF()` and `SCU_ARRAY_FOREACH()` macros.                                         |
| `assert.h`     | Macros for compile-time and runtime assertions.                                                                                          |
| `bench.h`      | A small benchmarking module for measuring the performance of code blocks.                                                                |
| `compare.h`    | Functions for comparing values of various types, designed to be used with the data structures provided by the library.                   |
| `equal.h`      | Functions for determining the equality of values of various types, designed to be used with the data structures provided by the library. |
| `error.h`      | Utilities for error handling, including an error code type used consistently across the library.                                         |
| `hash-map.h`   | A generic hash map associating keys of one type with values of another type.                                                             |
| `hash-set.h`   | A generic hash set storing values of a single type.                                                                                      |
| `hash.h`       | Functions for hashing values of various types, designed to be used with the data structures provided by the library.                     |
| `io.h`         | Utilities for input and output operations (e.g., reading and writing files, formatted printing and scanning).                            |
| `list.h`       | A generic dynamic array storing values of a single type and supporting the usual indexing syntax (i.e., `list[i]`).                      |
| `math.h`       | Common math utilities, including the ubiquitous `SCU_MIN()` and `SCU_MAX()` macros.                                                      |
| `memory.h`     | Utilities for allocating and managing objects in memory, including support for custom allocators.                                        |
| `prio-queue.h` | A generic priority queue associating values of one type with priorities of another type.                                                 |
| `queue.h`      | A generic first-in-first-out (FIFO) queue storing values of a single type.                                                               |
| `scu.h`        | An umbrella header that includes the entirety of the library at once.                                                                    |
| `stack.h`      | A generic last-in-first-out (LIFO) stack storing values of a single type.                                                                |
| `string.h`     | Utilities for working with null-terminated byte strings.                                                                                 |
| `types.h`      | Common typedefs and constants used across the library.                                                                                   |
| `util.h`       | Other common macros and utilities.                                                                                                       |

## Common Conventions

Before diving into the details of SCU, it might be helpful to familiarize
yourself with some of the common conventions used throughout the library. These
conventions are not necessarily unique to SCU, but they are consistently applied
across all modules and should help you understand and use the API more easily.

### Naming

First and foremost, all types, constants, macros and functions provided by the
library are prefixed with `SCU` (or some variation of that) to avoid name
clashes with other libraries or user code. In some cases, the prefix is followed
by an additional underscore and a module-specific prefix to further group
related items together. To give you some examples:

```c
// Types are prefixed with `Scu` and use the `PascalCase` naming convention.
ScuError error;
ScuQueue* queue;
ScuHashSet* visited;

// Constants (including enum values) and macros are prefixed with `SCU_` and use
// the `ALL_CAPS` naming convention.
typedef enum ScuError {
    SCU_ERROR_NONE,
    SCU_ERROR_OUT_OF_MEMORY,
    ...
} ScuError;

#define SCU_SIZEOF(expr) ((Scuisize) sizeof(expr))
#define SCU_ABS(x) (((x) < 0) ? -(x) : (x))

// Functions and function-like macros are prefixed with `scu_` and use either
// the `lowercase` naming convention (if there already exists a well-known name
// that should easily be discoverable) or the `snake_case` naming convention.
scu_printf("Hello, World!\n");
scu_memcpy(dest, src, count);
scu_list_count(list);
scu_hash_map_try_add(map, &key, &value);

// Struct and union members, as well as macro and function parameters use the
// `camelCase` naming convention.
typedef struct ScuTimingResult {
    Scui64 wallNs;
    Scui64 cpuNs;
    ScuError error;
} ScuTimingResult;

ScuError scu_hash_set_ensure_capacity(ScuHashSet* hashSet, Scuisize capacity);
```

One notable exception to the conventions described above are the types defined
in [`types.h`](include/scu/types.h), which are prefixed with `Scu` but use the
`lowercase` naming convention. This is because these types are intended to feel
more like built-in types and can even be used without the `Scu` prefix in user
code. To achieve this, a macro `SCU_SHORT_ALIASES` must be defined before the
`types.h` header is (directly or transitively) included, which will define
additional short aliases.

```c
#define SCU_SHORT_ALIASES
#include <scu/types.h>

byte b = 0xFF;
isize s = SCU_SIZEOF(void*);
i32 i = 42;
f32 f = 3.14F;
```

### Signed Integers

SCU exclusively uses signed integers (e.g., `Scuisize`, which is a synonym for
`ptrdiff_t`) instead of unsigned integers for representing sizes, quantities or
indices. This is in contrast to the C standard library, which makes heavy use of
unsigned integers such as `size_t` for these purposes. Although unsigned
integers can theoretically represent a larger range of values, they also come
with a number of pitfalls regarding arithmetic operations, comparisons and
conversions. Since I have made quite a few mistakes with unsigned integers in
the past, I now consider myself among those who only use them when absolutely
necessary (e.g., for bit manipulation or when interfacing with APIs that require
them). In new code, I tend to stick to signed integers for everything else, and
perform the necessary checks and conversions in the background as required.

### Preconditions, Programming Errors and Error Handling

Most of the functions provided by SCU have preconditions that must be satisfied
by the caller. In debug builds (i.e., when the macro `NDEBUG` is not defined),
these preconditions are checked using the `SCU_ASSERT()` macro, which terminates
the program with an error message if a precondition is violated. In release
builds (i.e., when the macro `NDEBUG` is defined), these checks are omitted for
performance reasons, and it is the caller's responsibility to ensure that the
preconditions are met. Violating preconditions in release builds may lead to
undefined behavior, so it is important to be mindful of this when using the
library.

Anything that can be considered a programming error (such as passing invalid
arguments to a function) is treated as a violation of preconditions. SCU makes
an effort to detect such errors in debug builds, but does **not** attempt to
handle them gracefully. Generally speaking, pointers passed to SCU functions are
expected to be non-null, properly aligned and valid for the duration of the
call, unless explicitly stated otherwise in the documentation. The same holds
true for other types of arguments, such as sizes or indices, which are often
assumed to be non-negative (sometimes even strictly greater than zero) and
within a valid or reasonable range. Unless explicitly stated otherwise, the
behavior of SCU functions is undefined if these assumptions are violated, and
the library may not perform any checks to prevent or mitigate the consequences
of such violations.

Of course, functions provided by SCU may also fail for reasons other than
programming errors, such as encountering an out-of-memory condition or failing
to read from a file. In such cases, the functions either return a special
sentinel value (e.g., `nullptr`, `-1` or `false`) or an error code of type
`ScuError` if it is expected that the caller may want to distinguish between
different error conditions.

```c
// Returns `nullptr` if an out-of-memory condition occurs.
char* scu_strdup(const char* src);

// Returns `-1` if `c` is not found in `s`.
Scuisize scu_str_index_of_byte(const char* s, char c);

// Returns `false` if `elem` was not present in `hashSet`.
bool scu_hash_set_remove(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
);

// Returns `SCU_ERROR_END_OF_FILE` if the end-of-file condition occurred before
// a byte is read, `SCU_ERROR_READING_FILE` if an error occurred while reading
// from the specified file stream, or `SCU_ERROR_NONE` on success.
ScuError scu_freadc(ScuFile* restrict file, char* restrict c);
```

Needless to say, it is advisable to always check these return values, at least
in any non-trivial program. It should also be noted that SCU makes a great
effort to ensure that the objects managed by the library or passed to its
functions remain in a valid state even in the case of a runtime error, such that
they can be continued to be used or properly cleaned up by the caller.

### Memory Management and Ownership

Several of the functions provided by SCU dynamically allocate memory. If you
take a look around the modules of the library, you will notice that many of
these functions have names that contain or end with the suffix `_new` (e.g.,
`scu_list_new()`, `scu_stack_new()`, etc.), while the corresponding deallocation
functions usually end with `_delete` (e.g., `scu_list_delete()`,
`scu_stack_delete()`, etc.). However, this is not a strict convention, as there
are other functions whose primary purpose is not to create a new object, but
that still need to dynamically allocate memory to accomplish their task (e.g.,
`scu_freadln()`, `scu_rsnprintf()`, etc.).

SCU provides support for custom allocators, which are represented as instances
of the `ScuAllocator` type. Custom implementations must support at minimum the
operations `allocate()` (to allocate uninitialized memory) and `deallocate()`.
Further specialized operations such as `allocateZeroed()`, `resize()` and
`resizeOrMove()` may be supported, but these are optional. The convenience
functions `scu_allocate_zeroed()`, `scu_resize()` and `scu_resize_or_move()`
implement a fallback mechanism for when a custom allocator does not support one
of these operations. SCU provides a default allocator implemented in terms of
the C standard library, which supports all operations except for `resize()`, but
may not be the most efficient one for a given task.

> [!TIP]
>
> All generic data structures provided by SCU accept an allocator at
> construction, such that you can have fine-grained control over memory
> management.

If a function provided by SCU dynamically allocates memory, the ownership of
that memory is transferred to the caller, who is responsible for deallocating
with the appropriate deallocation function when it is no longer required. For
raw blocks of memory, it is usually sufficient to call `scu_deallocate()` with
the corresponding allocator, or use the convenience macros `SCU_DELETE()` or
`SCU_ARRAY_DELETE()`. More complex objects such as the generic data structures
usually provide a dedicated deallocation function, as mentioned previously.

For more information about custom allocators and memory management, see the
[`memory.h`](include/scu/memory.h) header.

### Thread Safety

Unless explicitly stated otherwise in the documentation, SCU is **not**
thread-safe. External synchronization is required when accessing shared state
from multiple threads, and it is the caller's responsibility to ensure this is
done correctly. This was a conscious design choice, as I wanted to keep the
library simple and efficient.

## Usage

Before going into more detail about how to build and use SCU, a couple of notes
on platform support and compatibility: Although SCU was written in standard C
and does not have any external dependencies, it is definitely not meant to be
used on every platform. The library is primarily intended for use on modern
64-bit desktop operating systems (i.e., Windows, GNU/Linux and macOS), and it
may not compile or work correctly on other platforms (e.g., embedded systems,
32-bit operating systems, etc.) without modifications. It should also be noted
that SCU occasionally makes use of compiler-specific extensions or
platform-specific APIs to provide certain improvements or additional
functionality. Finally, SCU requires a C23-compliant compiler, which means that
you are basically limited to using the latest versions of popular compilers such
as GCC 14+ or Clang 18+ at the moment. Support for older C standards may be
added in the future, but is not a priority.

If your platform and compiler meet the requirements described above, you can
build SCU from source. The library uses [Meson](https://mesonbuild.com/) as its
build system, so the first step is to configure the build process:

```shell
meson setup build
```

By default, this will configure the build process to build an unoptimized debug
version of the library. If you want to build an optimized release version
instead, pass the option `--buildtype=release`:

```shell
meson setup build --buildtype=release
```

> [!TIP]
>
> The `--default-library` option can be used to specify whether to build a
> static or shared library (or both). It is strongly recommended that you build
> these separately (if both are required), as some flags may interfere with each
> other and produce an incorrect build.

The next step is to compile the library:

```shell
meson compile -C build
```

Finally, you can install the library and its headers to your system:

```shell
meson install -C build
```

> [!TIP]
>
> Instead of installing the library system-wide, you can also include it as a
> subproject in your own project. For more information, see the [official
> documentation](https://mesonbuild.com/Subprojects.html).

## Contributing

As SCU is a small personal project, I don't really expect any contributions.
That said, I welcome ideas and improvements – if you have a suggestion, feel
free to open an issue and I'll see what I can do.

## License

SCU is licensed under the MIT License. See the [LICENSE](LICENSE) file for more
information.
