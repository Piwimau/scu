#define SCU_SHORT_ALIASES

#include <math.h>
#include <string.h>
#include "scu/assert.h"
#include "scu/compare.h"
#include "scu/types.h"

SCU_API int scu_compare_bool(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    bool l = *(const bool*) lhs;
    bool r = *(const bool*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_byte(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    byte l = *(const byte*) lhs;
    byte r = *(const byte*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_i8(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    i8 l = *(const i8*) lhs;
    i8 r = *(const i8*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_u8(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    u8 l = *(const u8*) lhs;
    u8 r = *(const u8*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_i16(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    i16 l = *(const i16*) lhs;
    i16 r = *(const i16*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_u16(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    u16 l = *(const u16*) lhs;
    u16 r = *(const u16*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_i32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    i32 l = *(const i32*) lhs;
    i32 r = *(const i32*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_u32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    u32 l = *(const u32*) lhs;
    u32 r = *(const u32*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_i64(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    i64 l = *(const i64*) lhs;
    i64 r = *(const i64*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_u64(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    u64 l = *(const u64*) lhs;
    u64 r = *(const u64*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_iptr(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    iptr l = *(const iptr*) lhs;
    iptr r = *(const iptr*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_uptr(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    uptr l = *(const uptr*) lhs;
    uptr r = *(const uptr*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_isize(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    isize l = *(const isize*) lhs;
    isize r = *(const isize*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_usize(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    usize l = *(const usize*) lhs;
    usize r = *(const usize*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_f32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    f32 l = *(const f32*) lhs;
    f32 r = *(const f32*) rhs;
    SCU_ASSERT(!isnan(l));
    SCU_ASSERT(!isnan(r));
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_f64(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    f64 l = *(const f64*) lhs;
    f64 r = *(const f64*) rhs;
    SCU_ASSERT(!isnan(l));
    SCU_ASSERT(!isnan(r));
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_char(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    char l = *(const char*) lhs;
    char r = *(const char*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_char8(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    char8 l = *(const char8*) lhs;
    char8 r = *(const char8*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_char16(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    char16 l = *(const char16*) lhs;
    char16 r = *(const char16*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_char32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    char32 l = *(const char32*) lhs;
    char32 r = *(const char32*) rhs;
    return (l < r) ? -1 : (l > r) ? 1 : 0;
}

SCU_API int scu_compare_str(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    const char* l = *(const char* const*) lhs;
    const char* r = *(const char* const*) rhs;
    int cmp = strcmp(l, r);
    return (cmp < 0) ? -1 : (cmp > 0) ? 1 : 0;
}