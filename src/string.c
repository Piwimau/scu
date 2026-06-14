#define SCU_SHORT_ALIASES

#include <string.h>
#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/math.h"
#include "scu/memory.h"
#include "scu/string.h"

isize scu_strlen(const char* s) {
    SCU_ASSERT(s != nullptr);
    return (isize) strlen(s);
}

isize scu_strnlen(const char* s, isize count) {
    const char* p = scu_memchr(s, '\0', count);
    return (p == nullptr) ? count : p - s;
}

int scu_strcmp(const char* left, const char* right) {
    SCU_ASSERT(left != nullptr);
    SCU_ASSERT(right != nullptr);
    int cmp = strcmp(left, right);
    return (cmp < 0) ? -1 : (cmp > 0) ? 1 : 0;
}

int scu_strncmp(const char* left, const char* right, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return 0;
    }
    SCU_ASSERT(left != nullptr);
    SCU_ASSERT(right != nullptr);
    int cmp = strncmp(left, right, (usize) count);
    return (cmp < 0) ? -1 : (cmp > 0) ? 1 : 0;
}

isize scu_str_index_of_byte(const char* s, char c) {
    SCU_ASSERT(s != nullptr);
    if (c == '\0') {
        return -1;
    }
    const char* p = strchr(s, c);
    return (p == nullptr) ? -1 : p - s;
}

isize scu_str_index_of_str(const char* s, const char* other) {
    SCU_ASSERT(s != nullptr);
    SCU_ASSERT(other != nullptr);
    const char* p = strstr(s, other);
    return (p == nullptr) ? -1 : p - s;
}

isize scu_str_last_index_of_byte(const char* s, char c) {
    SCU_ASSERT(s != nullptr);
    if (c == '\0') {
        return -1;
    }
    const char* p = strrchr(s, c);
    return (p == nullptr) ? -1 : p - s;
}

isize scu_str_last_index_of_str(const char* s, const char* other) {
    SCU_ASSERT(s != nullptr);
    SCU_ASSERT(other != nullptr);
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

isize scu_str_index_of_any(const char* s, const char* anyOf) {
    SCU_ASSERT(s != nullptr);
    SCU_ASSERT(anyOf != nullptr);
    if (anyOf[0] == '\0') {
        return -1;
    }
    const char* p = strpbrk(s, anyOf);
    return (p == nullptr) ? -1 : p - s;
}

isize scu_str_last_index_of_any(const char* s, const char* anyOf) {
    SCU_ASSERT(s != nullptr);
    SCU_ASSERT(anyOf != nullptr);
    if (anyOf[0] == '\0') {
        return -1;
    }
    for (isize i = scu_strlen(s) - 1; i >= 0; i--) {
        if (strchr(anyOf, s[i]) != nullptr) {
            return i;
        }
    }
    return -1;
}

isize scu_str_index_in_range(
    const char* s,
    char lowInclusive,
    char highInclusive
) {
    SCU_ASSERT(s != nullptr);
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

isize scu_str_last_index_in_range(
    const char* s,
    char lowInclusive,
    char highInclusive
) {
    SCU_ASSERT(s != nullptr);
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

bool scu_str_starts_with_byte(const char* s, char c) {
    SCU_ASSERT(s != nullptr);
    return (s[0] != '\0') && (s[0] == c);
}

bool scu_str_starts_with_str(const char* s, const char* prefix) {
    SCU_ASSERT(s != nullptr);
    SCU_ASSERT(prefix != nullptr);
    isize i = 0;
    while ((prefix[i] != '\0') && (s[i] == prefix[i])) {
        i++;
    }
    return prefix[i] == '\0';
}

bool scu_str_ends_with_byte(const char* s, char c) {
    SCU_ASSERT(s != nullptr);
    isize length = scu_strlen(s);
    return (length > 0) && (s[length - 1] == c);
}

bool scu_str_ends_with_str(const char* s, const char* suffix) {
    SCU_ASSERT(s != nullptr);
    SCU_ASSERT(suffix != nullptr);
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

[[nodiscard]]
char* scu_strdup(const char* src) {
    isize length = scu_strlen(src);
    isize size = (length + 1) * SCU_SIZEOF(char);
    char* dest = scu_malloc(size);
    if (dest != nullptr) {
        scu_memcpy(dest, src, length * SCU_SIZEOF(char));
        dest[length] = '\0';
    }
    return dest;
}

[[nodiscard]]
char* scu_strndup(const char* src, isize count) {
    const char* p = scu_memchr(src, '\0', count);
    isize length = (p == nullptr) ? count : (p - src);
    isize size = (length + 1) * SCU_SIZEOF(char);
    char* dest = scu_malloc(size);
    if (dest != nullptr) {
        scu_memcpy(dest, src, length * SCU_SIZEOF(char));
        dest[length] = '\0';
    }
    return dest;
}

isize scu_strncpy(
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
    SCU_ASSERT(dest != nullptr);
    if (count == 0) {
        dest[0] = '\0';
        return 0;
    }
    SCU_ASSERT(src != nullptr);
    isize n = SCU_MIN(size - 1, count);
    char* p = scu_memccpy(dest, src, '\0', n);
    if (p == nullptr) {
        dest[n] = '\0';
        return n;
    }
    return p - dest - 1;
}

isize scu_strncat(
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
    SCU_ASSERT(dest != nullptr);
    SCU_ASSERT(src != nullptr);
    isize length = scu_strlen(dest);
    SCU_ASSERT(length < size);
    isize n = SCU_MIN(size - length - 1, count);
    if (n == 0) {
        return 0;
    }
    char* p = scu_memccpy(dest + length, src, '\0', n);
    if (p == nullptr) {
        dest[length + n] = '\0';
        return n;
    }
    return p - dest + length - 1;
}