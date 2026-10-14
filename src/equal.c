#define SCU_SHORT_ALIASES

#include <math.h>
#include <string.h>
#include "scu/assert.h"
#include "scu/equal.h"
#include "scu/types.h"

bool scu_equal_bool(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const bool*) lhs == *(const bool*) rhs;
}

bool scu_equal_byte(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const byte*) lhs == *(const byte*) rhs;
}

bool scu_equal_i8(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const i8*) lhs == *(const i8*) rhs;
}

bool scu_equal_u8(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const u8*) lhs == *(const u8*) rhs;
}

bool scu_equal_i16(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const i16*) lhs == *(const i16*) rhs;
}

bool scu_equal_u16(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const u16*) lhs == *(const u16*) rhs;
}

bool scu_equal_i32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const i32*) lhs == *(const i32*) rhs;
}

bool scu_equal_u32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const u32*) lhs == *(const u32*) rhs;
}

bool scu_equal_i64(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const i64*) lhs == *(const i64*) rhs;
}

bool scu_equal_u64(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const u64*) lhs == *(const u64*) rhs;
}

bool scu_equal_iptr(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const iptr*) lhs == *(const iptr*) rhs;
}

bool scu_equal_uptr(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const uptr*) lhs == *(const uptr*) rhs;
}

bool scu_equal_isize(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const isize*) lhs == *(const isize*) rhs;
}

bool scu_equal_usize(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const usize*) lhs == *(const usize*) rhs;
}

bool scu_equal_f32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    f32 l = *(const f32*) lhs;
    f32 r = *(const f32*) rhs;
    SCU_ASSERT(!isnan(l));
    SCU_ASSERT(!isnan(r));
    return l == r;
}

bool scu_equal_f64(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    f64 l = *(const f64*) lhs;
    f64 r = *(const f64*) rhs;
    SCU_ASSERT(!isnan(l));
    SCU_ASSERT(!isnan(r));
    return l == r;
}

bool scu_equal_char(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const char*) lhs == *(const char*) rhs;
}

bool scu_equal_char8(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const char8*) lhs == *(const char8*) rhs;
}

bool scu_equal_char16(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const char16*) lhs == *(const char16*) rhs;
}

bool scu_equal_char32(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    return *(const char32*) lhs == *(const char32*) rhs;
}

bool scu_equal_str(const void* lhs, const void* rhs) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    const char* l = *(const char* const*) lhs;
    const char* r = *(const char* const*) rhs;
    return strcmp(l, r) == 0;
}