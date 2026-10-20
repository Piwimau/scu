#define SCU_SHORT_ALIASES

#include <string.h>
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/memory.h"
#include "scu/string.h"

SCU_API isize scu_strlen(const char* s) {
    SCU_ASSERT(s);
    return (isize) strlen(s);
}

SCU_API isize scu_strnlen(const char* s, isize count) {
    const char* p = scu_memchr(s, '\0', count);
    return p ? p - s : count;
}

SCU_API int scu_strcmp(const char* left, const char* right) {
    SCU_ASSERT(left);
    SCU_ASSERT(right);
    int cmp = strcmp(left, right);
    return (cmp < 0) ? -1 : (cmp > 0) ? 1 : 0;
}

SCU_API int scu_strncmp(const char* left, const char* right, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return 0;
    }
    SCU_ASSERT(left);
    SCU_ASSERT(right);
    int cmp = strncmp(left, right, (usize) count);
    return (cmp < 0) ? -1 : (cmp > 0) ? 1 : 0;
}

SCU_API isize scu_str_index_of_byte(const char* s, char c) {
    SCU_ASSERT(s);
    if (c == '\0') {
        return -1;
    }
    const char* p = strchr(s, c);
    return p ? p - s : -1;
}

SCU_API isize scu_str_index_of_str(const char* s, const char* other) {
    SCU_ASSERT(s);
    SCU_ASSERT(other);
    const char* p = strstr(s, other);
    return p ? p - s : -1;
}

SCU_API isize scu_str_last_index_of_byte(const char* s, char c) {
    SCU_ASSERT(s);
    if (c == '\0') {
        return -1;
    }
    const char* p = strrchr(s, c);
    return p ? p - s : -1;
}

SCU_API isize scu_str_last_index_of_str(const char* s, const char* other) {
    SCU_ASSERT(s);
    SCU_ASSERT(other);
    isize length = scu_strlen(s);
    isize otherLength = scu_strlen(other);
    if (otherLength == 0) {
        return length;
    }
    if (otherLength > length) {
        return -1;
    }
    for (isize i = length - otherLength; i >= 0; i--) {
        if (scu_strncmp(s + i, other, otherLength) == 0) {
            return i;
        }
    }
    return -1;
}

SCU_API isize scu_str_index_of_any(const char* s, const char* anyOf) {
    SCU_ASSERT(s);
    SCU_ASSERT(anyOf);
    if (anyOf[0] == '\0') {
        return -1;
    }
    const char* p = strpbrk(s, anyOf);
    return p ? p - s : -1;
}

SCU_API isize scu_str_last_index_of_any(const char* s, const char* anyOf) {
    SCU_ASSERT(s);
    SCU_ASSERT(anyOf);
    if (anyOf[0] == '\0') {
        return -1;
    }
    for (isize i = scu_strlen(s) - 1; i >= 0; i--) {
        if (strchr(anyOf, s[i])) {
            return i;
        }
    }
    return -1;
}

SCU_API isize scu_str_index_in_range(
    const char* s,
    char lowInclusive,
    char highInclusive
) {
    SCU_ASSERT(s);
    const byte* p = (const byte*) s;
    byte low = (byte) lowInclusive;
    byte high = (byte) highInclusive;
    SCU_ASSERT(high >= low);
    for (isize i = 0; p[i] != '\0'; i++) {
        if ((p[i] >= low) && (p[i] <= high)) {
            return i;
        }
    }
    return -1;
}

SCU_API isize scu_str_last_index_in_range(
    const char* s,
    char lowInclusive,
    char highInclusive
) {
    SCU_ASSERT(s);
    const byte* p = (const byte*) s;
    byte low = (byte) lowInclusive;
    byte high = (byte) highInclusive;
    SCU_ASSERT(high >= low);
    for (isize i = scu_strlen(s) - 1; i >= 0; i--) {
        if ((p[i] >= low) && (p[i] <= high)) {
            return i;
        }
    }
    return -1;
}

SCU_API bool scu_str_starts_with_byte(const char* s, char c) {
    SCU_ASSERT(s);
    return (s[0] != '\0') && (s[0] == c);
}

SCU_API bool scu_str_starts_with_str(const char* s, const char* prefix) {
    SCU_ASSERT(s);
    SCU_ASSERT(prefix);
    isize i = 0;
    while ((prefix[i] != '\0') && (s[i] == prefix[i])) {
        i++;
    }
    return prefix[i] == '\0';
}

SCU_API bool scu_str_ends_with_byte(const char* s, char c) {
    SCU_ASSERT(s);
    isize length = scu_strlen(s);
    return (length > 0) && (s[length - 1] == c);
}

SCU_API bool scu_str_ends_with_str(const char* s, const char* suffix) {
    SCU_ASSERT(s);
    SCU_ASSERT(suffix);
    isize suffixLength = scu_strlen(suffix);
    if (suffixLength == 0) {
        return true;
    }
    isize length = scu_strlen(s);
    if (suffixLength > length) {
        return false;
    }
    return scu_strncmp(s + length - suffixLength, suffix, suffixLength) == 0;
}

SCU_API char* scu_strdup(const char* src) {
    isize length = scu_strlen(src);
    char* dest = SCU_ARRAY_NEW(char, length + 1);
    if (dest) {
        scu_memcpy(dest, src, length);
        dest[length] = '\0';
    }
    return dest;
}

SCU_API char* scu_strndup(const char* src, isize count) {
    const char* p = scu_memchr(src, '\0', count);
    isize length = p ? p - src : count;
    char* dest = SCU_ARRAY_NEW(char, length + 1);
    if (dest) {
        scu_memcpy(dest, src, length);
        dest[length] = '\0';
    }
    return dest;
}

SCU_API isize scu_strncpy(
    char* restrict dest,
    isize size,
    const char* restrict src,
    isize count
) {
    SCU_ASSERT(size >= 0);
    SCU_ASSERT(count >= 0);
    if (size == 0) {
        return 0;
    }
    SCU_ASSERT(dest);
    if (count == 0) {
        dest[0] = '\0';
        return 0;
    }
    SCU_ASSERT(src);
    isize n = SCU_MIN(size - 1, count);
    char* p = scu_memccpy(dest, src, '\0', n);
    if (!p) {
        dest[n] = '\0';
        return n;
    }
    return p - dest - 1;
}

SCU_API isize scu_strncat(
    char* restrict dest,
    isize size,
    const char* restrict src,
    isize count
) {
    SCU_ASSERT(size >= 0);
    SCU_ASSERT(count >= 0);
    if ((size == 0) || (count == 0)) {
        return 0;
    }
    SCU_ASSERT(dest);
    SCU_ASSERT(src);
    isize length = scu_strlen(dest);
    SCU_ASSERT(length < size);
    isize n = SCU_MIN(size - length - 1, count);
    if (n == 0) {
        return 0;
    }
    char* p = scu_memccpy(dest + length, src, '\0', n);
    if (!p) {
        dest[length + n] = '\0';
        return n;
    }
    return p - dest - length - 1;
}