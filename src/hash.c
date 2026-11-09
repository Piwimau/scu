#define SCU_SHORT_ALIASES

#include <math.h>
#include "scu/assert.h"
#include "scu/hash.h"
#include "scu/memory.h"

#if USIZE_WIDTH == 32
    /** @brief The FNV-1a offset basis for 32-bit hashes. */
    static constexpr usize SCU_FNV_OFFSET_BASIS = 0x811C9DC5;

    /** @brief The FNV-1a prime for 32-bit hashes. */
    static constexpr usize SCU_FNV_PRIME = 0x01000193;

    /** @brief The multiplier used for combining hashes. */
    static constexpr usize SCU_HASH_MULTIPLIER = 0x9E3779B9;

    /**
     * @brief Mixes the bits of a `usize` value.
     *
     * @param[in] v The value to mix.
     * @return The mixed value.
     */
    static inline usize scu_hash_mix_usize(usize v) {
        v ^= v >> 16;
        v *= 0x85EBCA6B;
        v ^= v >> 13;
        v *= 0xC2B2AE35;
        v ^= v >> 16;
        return v;
    }

    /**
     * @brief Mixes the bits of a `u64` value.
     *
     * @param[in] v The value to mix.
     * @return The mixed value.
     */
    static inline usize scu_hash_mix_u64(u64 v) {
        u32 low = (u32) v;
        u32 high = (u32) (v >> 32);
        return scu_hash_combine(
            scu_hash_mix_usize(low),
            scu_hash_mix_usize(high)
        );
    }
#else
    /** @brief The FNV-1a offset basis for 64-bit hashes. */
    static constexpr usize SCU_FNV_OFFSET_BASIS = 0xCBF29CE484222325;

    /** @brief The FNV-1a prime for 64-bit hashes. */
    static constexpr usize SCU_FNV_PRIME = 0x100000001B3;

    /** @brief The multiplier used for combining hashes. */
    static constexpr usize SCU_HASH_MULTIPLIER = 0x9E3779B97F4A7C15;

    /**
     * @brief Mixes the bits of a `usize` value.
     *
     * @param[in] v The value to mix.
     * @return The mixed value.
     */
    static inline usize scu_hash_mix_usize(usize v) {
        v ^= v >> 30;
        v *= 0xBF58476D1CE4E5B9;
        v ^= v >> 27;
        v *= 0x94D049BB133111EB;
        v ^= v >> 31;
        return v;
    }

    /**
     * @brief Mixes the bits of a `u64` value.
     *
     * @param[in] v The value to mix.
     * @return The mixed value.
     */
    static inline usize scu_hash_mix_u64(u64 v) {
        return scu_hash_mix_usize(v);
    }
#endif

SCU_API usize scu_hash_bool(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const bool*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_byte(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const byte*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_i8(const void* value) {
    SCU_ASSERT(value);
    usize v = (u8) *(const i8*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_u8(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const u8*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_i16(const void* value) {
    SCU_ASSERT(value);
    usize v = (u16) *(const i16*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_u16(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const u16*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_i32(const void* value) {
    SCU_ASSERT(value);
    usize v = (u32) *(const i32*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_u32(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const u32*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_i64(const void* value) {
    SCU_ASSERT(value);
    u64 v = (u64) *(const i64*) value;
    return scu_hash_mix_u64(v);
}

SCU_API usize scu_hash_u64(const void* value) {
    SCU_ASSERT(value);
    u64 v = *(const u64*) value;
    return scu_hash_mix_u64(v);
}

SCU_API usize scu_hash_iptr(const void* value) {
    SCU_ASSERT(value);
    usize v = (uptr) *(const iptr*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_uptr(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const uptr*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_isize(const void* value) {
    SCU_ASSERT(value);
    usize v = (usize) *(const isize*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_usize(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const usize*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_f32(const void* value) {
    SCU_ASSERT(value);
    f32 v = *(const f32*) value;
    SCU_ASSERT(!isnan(v));
    // Normalize -0.0F to +0.0F.
    if (v == 0.0F) {
        v = 0.0F;
    }
    u32 bits;
    scu_memcpy(&bits, &v, SCU_SIZEOF(f32));
    return scu_hash_mix_usize(bits);
}

SCU_API usize scu_hash_f64(const void* value) {
    SCU_ASSERT(value);
    f64 v = *(const f64*) value;
    SCU_ASSERT(!isnan(v));
    // Normalize -0.0 to +0.0.
    if (v == 0.0) {
        v = 0.0;
    }
    u64 bits;
    scu_memcpy(&bits, &v, SCU_SIZEOF(f64));
    return scu_hash_mix_u64(bits);
}

SCU_API usize scu_hash_char(const void* value) {
    SCU_ASSERT(value);
    usize v = (byte) *(const char*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_char8(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const char8*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_char16(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const char16*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_char32(const void* value) {
    SCU_ASSERT(value);
    usize v = *(const char32*) value;
    return scu_hash_mix_usize(v);
}

SCU_API usize scu_hash_bytes(const void* block, isize count) {
    SCU_ASSERT(count >= 0);
    if (count == 0) {
        return SCU_FNV_OFFSET_BASIS;
    }
    SCU_ASSERT(block);
    const byte* p = (const byte*) block;
    usize v = SCU_FNV_OFFSET_BASIS;
    for (isize i = 0; i < count; i++) {
        v ^= p[i];
        v *= SCU_FNV_PRIME;
    }
    return v;
}

SCU_API usize scu_hash_str(const void* value) {
    SCU_ASSERT(value);
    const byte* p = *(const byte* const*) value;
    usize v = SCU_FNV_OFFSET_BASIS;
    for (isize i = 0; p[i] != '\0'; i++) {
        v ^= p[i];
        v *= SCU_FNV_PRIME;
    }
    return v;
}

SCU_API usize scu_hash_combine(usize seed, usize hash) {
    return seed ^ (hash + SCU_HASH_MULTIPLIER + (seed << 6) + (seed >> 2));
}
