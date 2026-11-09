#define SCU_SHORT_ALIASES

#include <stdbit.h>
#include <stddef.h>
#include "scu/assert.h"
#include "scu/hash-map.h"
#include "scu/math.h"

/** @brief A bucket in a hash map. */
typedef struct ScuBucket {

    /**
     * @brief The hash of the key stored in the bucket.
     *
     * @note The topmost bit indicates whether a bucket is occupied or not.
     *
     * The key itself is stored at `keyOffset` bytes from the start of the
     * bucket and the value at `valueOffset`, both stored by the owning hash
     * map.
     */
    usize hash;

} ScuBucket;

struct ScuHashMap {

    /** @brief The allocator used for memory management. */
    ScuAllocator* allocator;

    /** @brief The size of each key (in bytes). */
    isize keySize;

    /** @brief The size of each value (in bytes). */
    isize valueSize;

    /** @brief The offset of the key within a bucket (in bytes). */
    isize keyOffset;

    /** @brief The offset of the value within a bucket (in bytes). */
    isize valueOffset;

    /** @brief The effective size of a bucket (in bytes). */
    isize bucketSize;

    /** @brief The alignment of a bucket (in bytes). */
    isize bucketAlign;

    /**
     * @brief The maximum number of key-value pairs that could theoretically be
     * stored.
     *
     * @note The capacity is always a power of two and equal to the number of
     * buckets. The actual number of key-value pairs that can be stored without
     * a reallocation is lower and depends on the load factor of the hash map.
     */
    isize capacity;

    /** @brief The current number of key-value pairs. */
    isize count;

    /** @brief A function used for hashing keys. */
    ScuHashFunc* keyHashFunc;

    /** @brief A function used for comparing keys for equality. */
    ScuEqualFunc* keyEqualFunc;

    /** @brief A function used for comparing values for equality. */
    ScuEqualFunc* valueEqualFunc;

    /**
     * @brief The buckets storing the key-value pairs.
     *
     * @note This is a dynamically allocated array of `capacity` buckets, or
     * `nullptr` if `capacity` is zero.
     */
    ScuBucket* buckets;

    /** @brief A bucket used as scratch space for insertions. */
    ScuBucket* scratch;

};

/** @brief The default capacity of a hash map. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The numerator of the maximum load factor of a hash map. */
static constexpr isize SCU_MAX_LOAD_FACTOR_NUM = 9;

/** @brief The denominator of the maximum load factor of a hash map. */
static constexpr isize SCU_MAX_LOAD_FACTOR_DEN = 10;

/** @brief The bit mask used to check whether a bucket is occupied. */
static constexpr usize SCU_OCCUPIED_BIT = ~(USIZE_MAX >> 1);

SCU_STATIC_ASSERT(
    (usize) (ISIZE_MAX / 2) < SCU_OCCUPIED_BIT,
    "Capacity mask must not overlap the occupied bit."
);

SCU_API ScuHashMap* scu_hash_map_new_impl(
    isize keySize,
    isize valueSize,
    ScuHashFunc* keyHashFunc,
    ScuEqualFunc* keyEqualFunc,
    ScuEqualFunc* valueEqualFunc,
    ScuAllocator* allocator
) {
    return scu_hash_map_new_with_capacity_impl(
        keySize,
        valueSize,
        SCU_DEFAULT_CAPACITY,
        keyHashFunc,
        keyEqualFunc,
        valueEqualFunc,
        allocator
    );
}

SCU_API ScuHashMap* scu_hash_map_new_with_capacity_impl(
    isize keySize,
    isize valueSize,
    isize capacity,
    ScuHashFunc* keyHashFunc,
    ScuEqualFunc* keyEqualFunc,
    ScuEqualFunc* valueEqualFunc,
    ScuAllocator* allocator
) {
    SCU_ASSERT(keySize > 0);
    SCU_ASSERT(valueSize > 0);
    SCU_ASSERT(capacity >= 0 && capacity <= ISIZE_MAX / 2);
    SCU_ASSERT(keyHashFunc);
    SCU_ASSERT(keyEqualFunc);
    SCU_ASSERT(valueEqualFunc);
    SCU_ASSERT(allocator);
    ScuHashMap* hashMap = SCU_NEW(ScuHashMap, allocator);
    if (!hashMap) {
        return nullptr;
    }
    hashMap->allocator = allocator;
    hashMap->keySize = keySize;
    hashMap->valueSize = valueSize;
    isize keyAlign = SCU_MIN(keySize & -keySize, SCU_ALIGNOF(max_align_t));
    hashMap->keyOffset = SCU_ALIGN_UP(SCU_SIZEOF(ScuBucket), keyAlign);
    isize valueAlign = SCU_MIN(
        valueSize & -valueSize,
        SCU_ALIGNOF(max_align_t)
    );
    hashMap->valueOffset = SCU_ALIGN_UP(
        hashMap->keyOffset + hashMap->keySize,
        valueAlign
    );
    hashMap->bucketAlign = SCU_MAX(
        SCU_MAX(keyAlign, valueAlign),
        SCU_ALIGNOF(ScuBucket)
    );
    hashMap->bucketSize = SCU_ALIGN_UP(
        hashMap->valueOffset + hashMap->valueSize,
        hashMap->bucketAlign
    );
    hashMap->count = 0;
    hashMap->keyHashFunc = keyHashFunc;
    hashMap->keyEqualFunc = keyEqualFunc;
    hashMap->valueEqualFunc = valueEqualFunc;
    if (capacity > 0) {
        hashMap->capacity = (isize) stdc_bit_ceil((usize) capacity);
        hashMap->buckets = scu_allocate_zeroed(
            hashMap->capacity * hashMap->bucketSize,
            hashMap->bucketAlign,
            hashMap->allocator
        );
        if (!hashMap->buckets) {
            SCU_DELETE(hashMap, hashMap->allocator);
            return nullptr;
        }
    }
    else {
        hashMap->capacity = 0;
        hashMap->buckets = nullptr;
    }
    hashMap->scratch = scu_allocate(
        hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    if (!hashMap->scratch) {
        scu_deallocate(
            hashMap->buckets,
            hashMap->capacity * hashMap->bucketSize,
            hashMap->bucketAlign,
            hashMap->allocator
        );
        SCU_DELETE(hashMap, hashMap->allocator);
        return nullptr;
    }
    return hashMap;
}

SCU_API ScuHashMap* scu_hash_map_clone(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    ScuHashMap* clone = SCU_NEW(ScuHashMap, hashMap->allocator);
    if (!clone) {
        return nullptr;
    }
    clone->allocator = hashMap->allocator;
    clone->keySize = hashMap->keySize;
    clone->keyOffset = hashMap->keyOffset;
    clone->valueSize = hashMap->valueSize;
    clone->valueOffset = hashMap->valueOffset;
    clone->bucketSize = hashMap->bucketSize;
    clone->bucketAlign = hashMap->bucketAlign;
    clone->capacity = hashMap->capacity;
    clone->count = hashMap->count;
    clone->keyHashFunc = hashMap->keyHashFunc;
    clone->keyEqualFunc = hashMap->keyEqualFunc;
    clone->valueEqualFunc = hashMap->valueEqualFunc;
    if (clone->capacity > 0) {
        clone->buckets = scu_allocate(
            clone->capacity * clone->bucketSize,
            clone->bucketAlign,
            clone->allocator
        );
        if (!clone->buckets) {
            SCU_DELETE(clone, clone->allocator);
            return nullptr;
        }
        scu_memcpy(
            clone->buckets,
            hashMap->buckets,
            clone->capacity * clone->bucketSize
        );
    }
    else {
        clone->buckets = nullptr;
    }
    clone->scratch = scu_allocate(
        clone->bucketSize,
        clone->bucketAlign,
        clone->allocator
    );
    if (!clone->scratch) {
        scu_deallocate(
            clone->buckets,
            clone->capacity * clone->bucketSize,
            clone->bucketAlign,
            clone->allocator
        );
        SCU_DELETE(clone, clone->allocator);
        return nullptr;
    }
    return clone;
}

SCU_API isize scu_hash_map_capacity(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    return hashMap->capacity;
}

SCU_API isize scu_hash_map_count(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    return hashMap->count;
}

/**
 * @brief Returns a pointer to a bucket at an index.
 *
 * @param[in] buckets    A pointer to an array of buckets.
 * @param[in] index      The index of the bucket to retrieve.
 * @param[in] bucketSize The effective size of each bucket (in bytes).
 * @return A pointer to the bucket at the index.
 */
static inline ScuBucket* scu_bucket_at(
    ScuBucket* buckets,
    isize index,
    isize bucketSize
) {
    SCU_ASSERT(buckets);
    SCU_ASSERT(index >= 0);
    SCU_ASSERT(bucketSize > SCU_SIZEOF(ScuBucket));
    return (ScuBucket*) ((byte*) buckets + index * bucketSize);
}

/**
 * @brief Returns the ideal bucket index for a hash.
 *
 * @param[in] hash     The hash to examine.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The ideal bucket index for the hash.
 */
static inline isize scu_ideal_index(usize hash, isize capacity) {
    SCU_ASSERT(capacity > 0);
    return (isize) (hash & ((usize) (capacity - 1)));
}

/**
 * @brief Wraps an index to fit within the bounds of a capacity.
 *
 * @param[in] index    The index to wrap.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The wrapped index within the bounds of the capacity.
 */
static inline isize scu_wrap_index(isize index, isize capacity) {
    SCU_ASSERT(capacity > 0);
    return (isize) ((usize) index & (usize) (capacity - 1));
}

/**
 * @brief Returns the probe distance of a key with a hash at an index.
 *
 * @param[in] hash     The hash of the key.
 * @param[in] index    The index of the key.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The probe distance of the key.
 *
 * @note The probe distance is the number of positions a key had to travel
 * forward (with wrap-around) from its ideal position due to collisions.
 */
static inline isize scu_probe_distance(
    usize hash,
    isize index,
    isize capacity
) {
    SCU_ASSERT(capacity > 0);
    SCU_ASSERT(index >= 0 && index < capacity);
    isize idealIndex = scu_ideal_index(hash, capacity);
    return scu_wrap_index(index - idealIndex, capacity);
}

/**
 * @brief Determines if a bucket is occupied.
 *
 * @param[in] bucket The bucket to examine.
 * @return `true` if the bucket is occupied, `false` otherwise.
 */
static inline bool scu_bucket_is_occupied(const ScuBucket* bucket) {
    SCU_ASSERT(bucket);
    return bucket->hash != 0;
}

/**
 * @brief Rehashes old buckets by adding their key-value pairs to a hash map.
 *
 * @param[in, out] hashMap     The hash map to rehash into.
 * @param[in, out] oldBuckets  The old buckets to rehash.
 * @param[in]      oldCapacity The old capacity (in number of buckets).
 *
 * @warning This function assumes that the hash map has enough capacity to hold
 * all key-value pairs of the old buckets. Additionally, the contents of
 * `oldBuckets` may be modified during the rehashing process for optimization
 * purposes. After this function returns, the state of `oldBuckets` is
 * unspecified.
 */
static inline void scu_hash_map_rehash_buckets(
    ScuHashMap* hashMap,
    ScuBucket* oldBuckets,
    isize oldCapacity
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(oldBuckets);
    SCU_ASSERT(oldCapacity > 0);
    for (isize i = 0; i < oldCapacity; i++) {
        ScuBucket* oldBucket = scu_bucket_at(
            oldBuckets,
            i,
            hashMap->bucketSize
        );
        if (scu_bucket_is_occupied(oldBucket)) {
            isize index = scu_ideal_index(oldBucket->hash, hashMap->capacity);
            isize distance = 0;
            while (true) {
                ScuBucket* newBucket = scu_bucket_at(
                    hashMap->buckets,
                    index,
                    hashMap->bucketSize
                );
                if (!scu_bucket_is_occupied(newBucket)) {
                    scu_memcpy(newBucket, oldBucket, hashMap->bucketSize);
                    hashMap->count++;
                    break;
                }
                isize otherDistance = scu_probe_distance(
                    newBucket->hash,
                    index,
                    hashMap->capacity
                );
                if (distance > otherDistance) {
                    scu_memswap(oldBucket, newBucket, hashMap->bucketSize);
                    distance = otherDistance;
                }
                index = scu_wrap_index(index + 1, hashMap->capacity);
                distance++;
            }
        }
    }
}

SCU_API ScuError scu_hash_map_ensure_capacity(
    ScuHashMap* hashMap,
    isize capacity
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(capacity >= 0);
    isize minCapacity = (capacity * SCU_MAX_LOAD_FACTOR_DEN
        + SCU_MAX_LOAD_FACTOR_NUM - 1) / SCU_MAX_LOAD_FACTOR_NUM;
    if (hashMap->capacity >= minCapacity) {
        return SCU_ERROR_NONE;
    }
    isize newCapacity = (isize) stdc_bit_ceil((usize) minCapacity);
    ScuBucket* newBuckets = scu_allocate_zeroed(
        newCapacity * hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    if (!newBuckets) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    isize oldCapacity = hashMap->capacity;
    isize oldCount = hashMap->count;
    ScuBucket* oldBuckets = hashMap->buckets;
    hashMap->capacity = newCapacity;
    hashMap->count = 0;
    hashMap->buckets = newBuckets;
    if (oldCount > 0) {
        scu_hash_map_rehash_buckets(hashMap, oldBuckets, oldCapacity);
    }
    scu_deallocate(
        oldBuckets,
        oldCapacity * hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    return SCU_ERROR_NONE;
}

SCU_API ScuError scu_hash_map_add(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
) {
    ScuError error = scu_hash_map_try_add(hashMap, key, value);
    if (error == SCU_ERROR_ALREADY_PRESENT) {
        SCU_FATAL("The specified key is already present.\n");
    }
    return error;
}

/**
 * @brief Returns the hash of a key with the occupied bit set.
 *
 * @param[in] hashMap The hash map.
 * @param[in] key     The key to hash.
 * @return The hash of the key with the occupied bit set.
 */
static inline usize scu_hash_key(const ScuHashMap* hashMap, const void* key) {
    return hashMap->keyHashFunc(key) | SCU_OCCUPIED_BIT;
}

/**
 * @brief Returns a pointer to the key within a bucket.
 *
 * @param[in] bucket    The bucket containing the key.
 * @param[in] keyOffset The offset of the key within the bucket (in bytes).
 * @return A pointer to the key within the bucket.
 */
static inline void* scu_bucket_key(const ScuBucket* bucket, isize keyOffset) {
    SCU_ASSERT(bucket);
    return (byte*) SCU_CONST_CAST(ScuBucket*, bucket) + keyOffset;
}

/**
 * @brief Returns a pointer to the value within a bucket.
 *
 * @param[in] bucket      The bucket containing the value.
 * @param[in] valueOffset The offset of the value within the bucket (in bytes).
 * @return A pointer to the value within the bucket.
 */
static inline void* scu_bucket_value(
    const ScuBucket* bucket,
    isize valueOffset
) {
    SCU_ASSERT(bucket);
    return (byte*) SCU_CONST_CAST(ScuBucket*, bucket) + valueOffset;
}

SCU_API ScuError scu_hash_map_try_add(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(key);
    SCU_ASSERT(value);
    if (
        ((hashMap->count + 1) * SCU_MAX_LOAD_FACTOR_DEN)
            > (hashMap->capacity * SCU_MAX_LOAD_FACTOR_NUM)
    ) {
        isize needed = (hashMap->capacity == 0)
            ? SCU_DEFAULT_CAPACITY
            : hashMap->count + 1;
        ScuError error = scu_hash_map_ensure_capacity(hashMap, needed);
        if (error != SCU_ERROR_NONE) {
            return error;
        }
    }
    ScuBucket* scratch = hashMap->scratch;
    scratch->hash = scu_hash_key(hashMap, key);
    scu_memcpy(
        scu_bucket_key(scratch, hashMap->keyOffset),
        key,
        hashMap->keySize
    );
    scu_memcpy(
        scu_bucket_value(scratch, hashMap->valueOffset),
        value,
        hashMap->valueSize
    );
    isize index = scu_ideal_index(scratch->hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            scu_memcpy(bucket, scratch, hashMap->bucketSize);
            hashMap->count++;
            return SCU_ERROR_NONE;
        }
        if (
            bucket->hash == scratch->hash
                && hashMap->keyEqualFunc(
                    scu_bucket_key(bucket, hashMap->keyOffset),
                    scu_bucket_key(scratch, hashMap->keyOffset)
                )
        ) {
            return SCU_ERROR_ALREADY_PRESENT;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            scu_memswap(bucket, scratch, hashMap->bucketSize);
            distance = otherDistance;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

SCU_API void* scu_hash_map_get(const ScuHashMap* hashMap, const void* key) {
    void* value;
    if (!scu_hash_map_try_get_impl(hashMap, key, &value)) {
        SCU_FATAL("The specified key is not present.\n");
    }
    return value;
}

SCU_API bool scu_hash_map_try_get_impl(
    const ScuHashMap* restrict hashMap,
    const void* restrict key,
    void* restrict* restrict value
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(key);
    SCU_ASSERT(value);
    if (hashMap->count == 0) {
        *value = nullptr;
        return false;
    }
    usize hash = scu_hash_key(hashMap, key);
    isize index = scu_ideal_index(hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        const ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            *value = nullptr;
            return false;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            *value = nullptr;
            return false;
        }
        if (
            bucket->hash == hash
                && hashMap->keyEqualFunc(
                    scu_bucket_key(bucket, hashMap->keyOffset),
                    key
                )
        ) {
            *value = scu_bucket_value(bucket, hashMap->valueOffset);
            return true;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

SCU_API ScuError scu_hash_map_set(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(key);
    SCU_ASSERT(value);
    // If the key is already present in the hash map (which is the common case
    // for this operation), we update its value in-place without allocating any
    // additional memory.
    if (hashMap->count > 0) {
        usize hash = scu_hash_key(hashMap, key);
        isize index = scu_ideal_index(hash, hashMap->capacity);
        isize distance = 0;
        while (true) {
            const ScuBucket* bucket = scu_bucket_at(
                hashMap->buckets,
                index,
                hashMap->bucketSize
            );
            if (!scu_bucket_is_occupied(bucket)) {
                break;
            }
            isize otherDistance = scu_probe_distance(
                bucket->hash,
                index,
                hashMap->capacity
            );
            if (distance > otherDistance) {
                break;
            }
            if (
                bucket->hash == hash
                    && hashMap->keyEqualFunc(
                        scu_bucket_key(bucket, hashMap->keyOffset),
                        key
                    )
            ) {
                scu_memcpy(
                    scu_bucket_value(bucket, hashMap->valueOffset),
                    value,
                    hashMap->valueSize
                );
                return SCU_ERROR_NONE;
            }
            index = scu_wrap_index(index + 1, hashMap->capacity);
            distance++;
        }
    }
    // The key is not present in the hash map, so we need to add a new key-value
    // pair. We might have to increase the capacity of the hash map first if
    // adding a new key-value pair would exceed the maximum load factor. Note
    // that we have to search for the appropriate insertion point again after
    // ensuring the capacity, since the buckets might have been rehashed to new
    // positions during the capacity increase.
    if (
        ((hashMap->count + 1) * SCU_MAX_LOAD_FACTOR_DEN)
            > (hashMap->capacity * SCU_MAX_LOAD_FACTOR_NUM)
    ) {
        isize needed = (hashMap->capacity == 0)
            ? SCU_DEFAULT_CAPACITY
            : hashMap->count + 1;
        ScuError error = scu_hash_map_ensure_capacity(hashMap, needed);
        if (error != SCU_ERROR_NONE) {
            return error;
        }
    }
    ScuBucket* scratch = hashMap->scratch;
    scratch->hash = scu_hash_key(hashMap, key);
    scu_memcpy(
        scu_bucket_key(scratch, hashMap->keyOffset),
        key,
        hashMap->keySize
    );
    scu_memcpy(
        scu_bucket_value(scratch, hashMap->valueOffset),
        value,
        hashMap->valueSize
    );
    isize index = scu_ideal_index(scratch->hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            scu_memcpy(bucket, scratch, hashMap->bucketSize);
            hashMap->count++;
            return SCU_ERROR_NONE;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            scu_memswap(bucket, scratch, hashMap->bucketSize);
            distance = otherDistance;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

SCU_API bool scu_hash_map_contains_key(
    const ScuHashMap* hashMap,
    const void* key
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(key);
    if (hashMap->count == 0) {
        return false;
    }
    usize hash = scu_hash_key(hashMap, key);
    isize index = scu_ideal_index(hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        const ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            return false;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            return false;
        }
        if (
            bucket->hash == hash
                && hashMap->keyEqualFunc(
                    scu_bucket_key(bucket, hashMap->keyOffset),
                    key
                )
        ) {
            return true;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

SCU_API bool scu_hash_map_contains_value(
    const ScuHashMap* hashMap,
    const void* value
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(value);
    if (hashMap->count == 0) {
        return false;
    }
    for (isize i = 0; i < hashMap->capacity; i++) {
        const ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            i,
            hashMap->bucketSize
        );
        if (scu_bucket_is_occupied(bucket)) {
            const void* bucketValue = scu_bucket_value(
                bucket,
                hashMap->valueOffset
            );
            if (hashMap->valueEqualFunc(bucketValue, value)) {
                return true;
            }
        }
    }
    return false;
}

SCU_API bool scu_hash_map_remove(
    ScuHashMap* restrict hashMap,
    const void* restrict key
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(key);
    if (hashMap->count == 0) {
        return false;
    }
    usize hash = scu_hash_key(hashMap, key);
    isize index = scu_ideal_index(hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        const ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            return false;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            return false;
        }
        if (
            bucket->hash == hash
                && hashMap->keyEqualFunc(
                    scu_bucket_key(bucket, hashMap->keyOffset),
                    key
                )
        ) {
            break;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
    ScuBucket* bucket = scu_bucket_at(
        hashMap->buckets,
        index,
        hashMap->bucketSize
    );
    while (true) {
        isize nextIndex = scu_wrap_index(index + 1, hashMap->capacity);
        ScuBucket* nextBucket = scu_bucket_at(
            hashMap->buckets,
            nextIndex,
            hashMap->bucketSize
        );
        if (!scu_bucket_is_occupied(nextBucket)) {
            break;
        }
        isize nextDistance = scu_probe_distance(
            nextBucket->hash,
            nextIndex,
            hashMap->capacity
        );
        if (nextDistance == 0) {
            break;
        }
        scu_memcpy(bucket, nextBucket, hashMap->bucketSize);
        index = nextIndex;
        bucket = nextBucket;
    }
    bucket->hash = 0;
    hashMap->count--;
    return true;
}

SCU_API void scu_hash_map_clear(ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    if (hashMap->count > 0) {
        for (isize i = 0; i < hashMap->capacity; i++) {
            ScuBucket* bucket = scu_bucket_at(
                hashMap->buckets,
                i,
                hashMap->bucketSize
            );
            bucket->hash = 0;
        }
        hashMap->count = 0;
    }
}

SCU_API ScuError scu_hash_map_trim_excess(ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    if (hashMap->count == 0) {
        scu_deallocate(
            hashMap->buckets,
            hashMap->capacity * hashMap->bucketSize,
            hashMap->bucketAlign,
            hashMap->allocator
        );
        hashMap->buckets = nullptr;
        hashMap->capacity = 0;
        return SCU_ERROR_NONE;
    }
    isize minCapacity = (hashMap->count * SCU_MAX_LOAD_FACTOR_DEN
        + SCU_MAX_LOAD_FACTOR_NUM - 1) / SCU_MAX_LOAD_FACTOR_NUM;
    isize newCapacity = (isize) stdc_bit_ceil((usize) minCapacity);
    if (hashMap->capacity <= newCapacity) {
        return SCU_ERROR_NONE;
    }
    ScuBucket* newBuckets = scu_allocate_zeroed(
        newCapacity * hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    if (!newBuckets) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    isize oldCapacity = hashMap->capacity;
    ScuBucket* oldBuckets = hashMap->buckets;
    hashMap->capacity = newCapacity;
    hashMap->count = 0;
    hashMap->buckets = newBuckets;
    scu_hash_map_rehash_buckets(hashMap, oldBuckets, oldCapacity);
    scu_deallocate(
        oldBuckets,
        oldCapacity * hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    return SCU_ERROR_NONE;
}

SCU_API void scu_hash_map_delete(ScuHashMap* hashMap) {
    if (!hashMap) {
        return;
    }
    scu_deallocate(
        hashMap->buckets,
        hashMap->capacity * hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    hashMap->buckets = nullptr;
    scu_deallocate(
        hashMap->scratch,
        hashMap->bucketSize,
        hashMap->bucketAlign,
        hashMap->allocator
    );
    hashMap->scratch = nullptr;
    hashMap->capacity = 0;
    hashMap->count = 0;
    SCU_DELETE(hashMap, hashMap->allocator);
}

/**
 * @brief Returns the index of the first occupied bucket at or after an index.
 *
 * @param[in] hashMap  The hash map to examine.
 * @param[in] startIdx The index to start the search at.
 * @return The index of the first occupied bucket, or the capacity if there is
 * none.
 */
static inline isize scu_next_occupied_index(
    const ScuHashMap* hashMap,
    isize startIdx
) {
    SCU_ASSERT(hashMap);
    SCU_ASSERT(startIdx >= 0);
    for (isize i = startIdx; i < hashMap->capacity; i++) {
        const ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            i,
            hashMap->bucketSize
        );
        if (scu_bucket_is_occupied(bucket)) {
            return i;
        }
    }
    return hashMap->capacity;
}

SCU_API ScuHashMapIterator scu_hash_map_begin(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    return (ScuHashMapIterator) {
        .hashMap = SCU_CONST_CAST(ScuHashMap*, hashMap),
        .index = scu_next_occupied_index(hashMap, 0)
    };
}

SCU_API ScuHashMapIterator scu_hash_map_end(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap);
    return (ScuHashMapIterator) {
        .hashMap = SCU_CONST_CAST(ScuHashMap*, hashMap),
        .index = hashMap->capacity
    };
}

SCU_API bool scu_hash_map_iterator_equal(
    const ScuHashMapIterator* lhs,
    const ScuHashMapIterator* rhs
) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    SCU_ASSERT(lhs->hashMap == rhs->hashMap);
    return lhs->index == rhs->index;
}

SCU_API void scu_hash_map_iterator_next(ScuHashMapIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index < iterator->hashMap->capacity);
    iterator->index = scu_next_occupied_index(
        iterator->hashMap,
        iterator->index + 1
    );
}

SCU_API void scu_hash_map_iterator_prev(ScuHashMapIterator* iterator) {
    SCU_ASSERT(iterator);
    ScuHashMap* hashMap = iterator->hashMap;
    isize index = iterator->index - 1;
    while (index >= 0) {
        const ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (scu_bucket_is_occupied(bucket)) {
            break;
        }
        index--;
    }
    SCU_ASSERT(index >= 0);
    iterator->index = index;
}

SCU_API ScuHashMapEntry scu_hash_map_iterator_deref(
    const ScuHashMapIterator* iterator
) {
    SCU_ASSERT(iterator);
    ScuHashMap* hashMap = iterator->hashMap;
    isize index = iterator->index;
    SCU_ASSERT(index >= 0 && index < hashMap->capacity);
    const ScuBucket* bucket = scu_bucket_at(
        hashMap->buckets,
        index,
        hashMap->bucketSize
    );
    SCU_ASSERT(scu_bucket_is_occupied(bucket));
    return (ScuHashMapEntry) {
        .key = scu_bucket_key(bucket, hashMap->keyOffset),
        .value = scu_bucket_value(bucket, hashMap->valueOffset)
    };
}
