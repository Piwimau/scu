#define SCU_SHORT_ALIASES

#include <stddef.h>
#include "scu/alloc.h"
#include "scu/assert.h"
#include "scu/hash-map.h"
#include "scu/memory.h"

/** @brief Represents a bucket in a hash map. */
typedef struct ScuBucket {

    /** @brief The hash of the key stored in the bucket. */
    usize hash;

    /** @brief Indicates whether the bucket is occupied. */
    bool isOccupied;

    /**
     * @brief The key stored in the bucket.
     *
     * @note This is a flexible array member, which is a aligned as strictly as
     * `max_align_t` to ensure proper alignment for any type with fundamental
     * alignment requirements. The actual size of the key is stored by the hash
     * map owning this bucket.
     *
     * Note that the value associated with the key is immediately stored after
     * it. To ensure the value is properly aligned, there might be some padding
     * bytes between the key and the value. The size of the value is stored by
     * the hash map owning this bucket as well.
     */
    alignas(max_align_t) byte key[];

} ScuBucket;

struct ScuHashMap {

    /** @brief The size of each key (in bytes). */
    isize keySize;

    /** @brief The size of each value (in bytes). */
    isize valueSize;

    /** @brief The offset of the value within a bucket (in bytes). */
    isize valueOffset;

    /** @brief The effective size of a bucket (in bytes). */
    isize bucketSize;

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

};

/** @brief The default capacity of a hash map. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The numerator of the maximum load factor of a hash map. */
static constexpr isize SCU_MAX_LOAD_FACTOR_NUM = 9;

/** @brief The denominator of the maximum load factor of a hash map. */
static constexpr isize SCU_MAX_LOAD_FACTOR_DEN = 10;

/** @brief The growth factor for increasing the capacity of a hash map. */
static constexpr isize SCU_GROWTH_FACTOR = 2;

[[nodiscard]]
ScuHashMap* scu_hash_map_new(
    isize keySize,
    isize valueSize,
    ScuHashFunc* keyHashFunc,
    ScuEqualFunc* keyEqualFunc,
    ScuEqualFunc* valueEqualFunc
) {
    return scu_hash_map_new_with_capacity(
        keySize,
        valueSize,
        SCU_DEFAULT_CAPACITY,
        keyHashFunc,
        keyEqualFunc,
        valueEqualFunc
    );
}

/**
 * @brief Rounds up a value to the next multiple of a specified alignment.
 *
 * @warning The behavior is undefined if `alignment` is not a power of two.
 *
 * @param[in] value     The value to round up.
 * @param[in] alignment The required alignment.
 * @return The smallest multiple of `alignment` greater than or equal to
 * `value`.
 */
static inline isize scu_align_up(isize value, isize alignment) {
    SCU_ASSERT(value >= 0);
    SCU_ASSERT(alignment > 0);
    SCU_ASSERT((alignment & (alignment - 1)) == 0);
    return (value + alignment - 1) & ~(alignment - 1);
}

/**
 * @brief Returns the smallest power of two greater than or equal to a specified
 * value.
 *
 * @param[in] n The value to examine.
 * @return The smallest power of two greater than or equal to the specified
 * value.
 */
static inline isize scu_next_power_of_two(isize n) {
    SCU_ASSERT((n >= 0) && (n <= (ISIZE_MAX / 2)));
    if (n <= 1) {
        return 1;
    }
    usize v = (usize) n;
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
#if SCU_ISIZE_WIDTH == 64
    v |= v >> 32;
#endif
    v++;
    return (isize) v;
}

[[nodiscard]]
ScuHashMap* scu_hash_map_new_with_capacity(
    isize keySize,
    isize valueSize,
    isize capacity,
    ScuHashFunc* keyHashFunc,
    ScuEqualFunc* keyEqualFunc,
    ScuEqualFunc* valueEqualFunc
) {
    SCU_ASSERT(keySize > 0);
    SCU_ASSERT(valueSize > 0);
    SCU_ASSERT(capacity >= 0);
    SCU_ASSERT(keyHashFunc != nullptr);
    SCU_ASSERT(keyEqualFunc != nullptr);
    SCU_ASSERT(valueEqualFunc != nullptr);
    ScuHashMap* hashMap = scu_malloc(SCU_SIZEOF(ScuHashMap));
    if (hashMap == nullptr) {
        return nullptr;
    }
    hashMap->keySize = keySize;
    hashMap->valueSize = valueSize;
    hashMap->valueOffset = scu_align_up(
        SCU_SIZEOF(ScuBucket) + keySize,
        SCU_ALIGNOF(max_align_t)
    );
    hashMap->bucketSize = scu_align_up(
        hashMap->valueOffset + valueSize,
        SCU_ALIGNOF(max_align_t)
    );
    hashMap->count = 0;
    hashMap->keyHashFunc = keyHashFunc;
    hashMap->keyEqualFunc = keyEqualFunc;
    hashMap->valueEqualFunc = valueEqualFunc;
    if (capacity > 0) {
        hashMap->capacity = scu_next_power_of_two(capacity);
        hashMap->buckets = scu_calloc(hashMap->capacity, hashMap->bucketSize);
        if (hashMap->buckets == nullptr) {
            scu_free(hashMap);
            return nullptr;
        }
    }
    else {
        hashMap->capacity = 0;
        hashMap->buckets = nullptr;
    }
    return hashMap;
}

[[nodiscard]]
ScuHashMap* scu_hash_map_clone(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap != nullptr);
    ScuHashMap* clone = scu_malloc(SCU_SIZEOF(ScuHashMap));
    if (clone == nullptr) {
        return nullptr;
    }
    clone->keySize = hashMap->keySize;
    clone->valueSize = hashMap->valueSize;
    clone->valueOffset = hashMap->valueOffset;
    clone->bucketSize = hashMap->bucketSize;
    clone->capacity = hashMap->capacity;
    clone->count = hashMap->count;
    clone->keyHashFunc = hashMap->keyHashFunc;
    clone->keyEqualFunc = hashMap->keyEqualFunc;
    clone->valueEqualFunc = hashMap->valueEqualFunc;
    if (clone->capacity > 0) {
        clone->buckets = scu_malloc(clone->bucketSize * clone->capacity);
        if (clone->buckets == nullptr) {
            scu_free(clone);
            return nullptr;
        }
        scu_memcpy(
            clone->buckets,
            hashMap->buckets,
            clone->bucketSize * clone->capacity
        );
    }
    else {
        clone->buckets = nullptr;
    }
    return clone;
}

isize scu_hash_map_capacity(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap != nullptr);
    return hashMap->capacity;
}

isize scu_hash_map_count(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap != nullptr);
    return hashMap->count;
}

/**
 * @brief Returns a pointer to a bucket at a specified index.
 *
 * @param[in] buckets    A pointer to an array of buckets.
 * @param[in] index      The index of the bucket to retrieve.
 * @param[in] bucketSize The effective size of each bucket (in bytes).
 * @return A pointer to the bucket at the specified index.
 */
static inline ScuBucket* scu_bucket_at(
    ScuBucket* buckets,
    isize index,
    isize bucketSize
) {
    SCU_ASSERT(buckets != nullptr);
    SCU_ASSERT(index >= 0);
    SCU_ASSERT(bucketSize > SCU_SIZEOF(ScuBucket));
    return (ScuBucket*) ((byte*) buckets + index * bucketSize);
}

/**
 * @brief Returns the ideal bucket index for a specified hash.
 *
 * @param[in] hash     The hash to examine.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The ideal bucket index for the specified hash.
 */
static inline isize scu_ideal_index(usize hash, isize capacity) {
    SCU_ASSERT(capacity > 0);
    return (isize) (hash & ((usize) (capacity - 1)));
}

/**
 * @brief Wraps an index to fit within the bounds of a specified capacity.
 *
 * @param[in] index    The index to wrap.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The wrapped index within the bounds of the specified capacity.
 */
static inline isize scu_wrap_index(isize index, isize capacity) {
    SCU_ASSERT(capacity > 0);
    return (isize) ((usize) index & (usize) (capacity - 1));
}

/**
 * @brief Returns the probe distance of a key with a specified hash at a given
 * index.
 *
 * @note The probe distance is the number of positions a key had to travel
 * forward (with wrap-around) from its ideal position due to collisions.
 *
 * @param[in] hash     The hash of the key.
 * @param[in] index    The index of the key.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The probe distance of the key.
 */
static inline isize scu_probe_distance(
    usize hash,
    isize index,
    isize capacity
) {
    SCU_ASSERT(capacity > 0);
    SCU_ASSERT((index >= 0) && (index < capacity));
    isize idealIndex = scu_ideal_index(hash, capacity);
    return scu_wrap_index(index - idealIndex, capacity);
}

/**
 * @brief Rehashes old buckets by adding their key-value pairs to a specified
 * hash map.
 *
 * @warning This function assumes that the specified hash map has enough
 * capacity to hold all key-value pairs of the old buckets. Additionally, the
 * contents of `oldBuckets` may be modified during the rehashing process for
 * optimization purposes. After this function returns, the state of `oldBuckets`
 * is undefined.
 *
 * @param[in, out] hashMap     The hash map to rehash into.
 * @param[in, out] oldBuckets  The old buckets to rehash.
 * @param[in]      oldCapacity The old capacity (in number of buckets).
 */
static inline void scu_hash_map_rehash_buckets(
    ScuHashMap* hashMap,
    ScuBucket* oldBuckets,
    isize oldCapacity
) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(oldBuckets != nullptr);
    SCU_ASSERT(oldCapacity > 0);
    for (isize i = 0; i < oldCapacity; i++) {
        ScuBucket* oldBucket = scu_bucket_at(
            oldBuckets,
            i,
            hashMap->bucketSize
        );
        if (oldBucket->isOccupied) {
            isize index = scu_ideal_index(oldBucket->hash, hashMap->capacity);
            isize distance = 0;
            while (true) {
                ScuBucket* newBucket = scu_bucket_at(
                    hashMap->buckets,
                    index,
                    hashMap->bucketSize
                );
                if (!newBucket->isOccupied) {
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

ScuError scu_hash_map_ensure_capacity(ScuHashMap* hashMap, isize capacity) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(capacity >= 0);
    isize minCapacity = (capacity * SCU_MAX_LOAD_FACTOR_DEN
        + SCU_MAX_LOAD_FACTOR_NUM - 1) / SCU_MAX_LOAD_FACTOR_NUM;
    if (hashMap->capacity >= minCapacity) {
        return SCU_ERROR_NONE;
    }
    isize newCapacity = scu_next_power_of_two(minCapacity);
    ScuBucket* newBuckets = scu_calloc(newCapacity, hashMap->bucketSize);
    if (newBuckets == nullptr) {
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
    scu_free(oldBuckets);
    return SCU_ERROR_NONE;
}

ScuError scu_hash_map_add(
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
 * @brief Returns a pointer to the value stored in a specified bucket.
 *
 * @param[in] bucket      The bucket to examine.
 * @param[in] valueOffset The offset of the value within the bucket (in bytes).
 * @return A pointer to the value stored in the bucket.
 */
static inline void* scu_bucket_value(ScuBucket* bucket, isize valueOffset) {
    SCU_ASSERT(bucket != nullptr);
    SCU_ASSERT(valueOffset > SCU_SIZEOF(ScuBucket));
    return (byte*) bucket + valueOffset;
}

ScuError scu_hash_map_try_add(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(key != nullptr);
    SCU_ASSERT(value != nullptr);
    if (
        (hashMap->count * SCU_MAX_LOAD_FACTOR_DEN)
            >= (hashMap->capacity * SCU_MAX_LOAD_FACTOR_NUM)
    ) {
        isize newCapacity = (hashMap->capacity == 0)
            ? SCU_DEFAULT_CAPACITY
            : hashMap->capacity * SCU_GROWTH_FACTOR;
        ScuError error = scu_hash_map_ensure_capacity(hashMap, newCapacity);
        if (error != SCU_ERROR_NONE) {
            return error;
        }
    }
    ScuBucket* tempBucket = scu_malloc(hashMap->bucketSize);
    if (tempBucket == nullptr) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    tempBucket->hash = hashMap->keyHashFunc(key);
    tempBucket->isOccupied = true;
    scu_memcpy(tempBucket->key, key, hashMap->keySize);
    scu_memcpy(
        scu_bucket_value(tempBucket, hashMap->valueOffset),
        value,
        hashMap->valueSize
    );
    isize index = scu_ideal_index(tempBucket->hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!bucket->isOccupied) {
            scu_memcpy(bucket, tempBucket, hashMap->bucketSize);
            hashMap->count++;
            scu_free(tempBucket);
            return SCU_ERROR_NONE;
        }
        if (
            (bucket->hash == tempBucket->hash)
                && hashMap->keyEqualFunc(bucket->key, tempBucket->key)
        ) {
            scu_free(tempBucket);
            return SCU_ERROR_ALREADY_PRESENT;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            scu_memswap(bucket, tempBucket, hashMap->bucketSize);
            distance = otherDistance;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

void* scu_hash_map_get(const ScuHashMap* hashMap, const void* key) {
    void* value;
    if (!scu_hash_map_try_get_impl(hashMap, key, &value)) {
        SCU_FATAL("The specified key is not present.\n");
    }
    return value;
}

bool scu_hash_map_try_get_impl(
    const ScuHashMap* restrict hashMap,
    const void* restrict key,
    void* restrict* restrict value
) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(key != nullptr);
    SCU_ASSERT(value != nullptr);
    if (hashMap->count == 0) {
        *value = nullptr;
        return false;
    }
    usize hash = hashMap->keyHashFunc(key);
    isize index = scu_ideal_index(hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!bucket->isOccupied) {
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
        if ((bucket->hash == hash) && hashMap->keyEqualFunc(bucket->key, key)) {
            *value = scu_bucket_value(bucket, hashMap->valueOffset);
            return true;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

ScuError scu_hash_map_set(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(key != nullptr);
    SCU_ASSERT(value != nullptr);
    // If the key is already present in the hash map (which is the common case
    // for this operation), we update its value in-place without allocating any
    // additional memory.
    if (hashMap->count > 0) {
        usize hash = hashMap->keyHashFunc(key);
        isize index = scu_ideal_index(hash, hashMap->capacity);
        isize distance = 0;
        while (true) {
            ScuBucket* bucket = scu_bucket_at(
                hashMap->buckets,
                index,
                hashMap->bucketSize
            );
            if (!bucket->isOccupied) {
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
                (bucket->hash == hash)
                    && hashMap->keyEqualFunc(bucket->key, key)
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
        (hashMap->count * SCU_MAX_LOAD_FACTOR_DEN)
            >= (hashMap->capacity * SCU_MAX_LOAD_FACTOR_NUM)
    ) {
        isize newCapacity = (hashMap->capacity == 0)
            ? SCU_DEFAULT_CAPACITY
            : hashMap->capacity * SCU_GROWTH_FACTOR;
        ScuError error = scu_hash_map_ensure_capacity(hashMap, newCapacity);
        if (error != SCU_ERROR_NONE) {
            return error;
        }
    }
    ScuBucket* tempBucket = scu_malloc(hashMap->bucketSize);
    if (tempBucket == nullptr) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    tempBucket->hash = hashMap->keyHashFunc(key);
    tempBucket->isOccupied = true;
    scu_memcpy(tempBucket->key, key, hashMap->keySize);
    scu_memcpy(
        scu_bucket_value(tempBucket, hashMap->valueOffset),
        value,
        hashMap->valueSize
    );
    isize index = scu_ideal_index(tempBucket->hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!bucket->isOccupied) {
            scu_memcpy(bucket, tempBucket, hashMap->bucketSize);
            hashMap->count++;
            scu_free(tempBucket);
            return SCU_ERROR_NONE;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashMap->capacity
        );
        if (distance > otherDistance) {
            scu_memswap(bucket, tempBucket, hashMap->bucketSize);
            distance = otherDistance;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

bool scu_hash_map_contains_key(const ScuHashMap* hashMap, const void* key) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(key != nullptr);
    if (hashMap->count == 0) {
        return false;
    }
    usize hash = hashMap->keyHashFunc(key);
    isize index = scu_ideal_index(hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!bucket->isOccupied) {
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
        if ((bucket->hash == hash) && hashMap->keyEqualFunc(bucket->key, key)) {
            return true;
        }
        index = scu_wrap_index(index + 1, hashMap->capacity);
        distance++;
    }
}

bool scu_hash_map_contains_value(const ScuHashMap* hashMap, const void* value) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(value != nullptr);
    if (hashMap->count == 0) {
        return false;
    }
    for (isize i = 0; i < hashMap->capacity; i++) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            i,
            hashMap->bucketSize
        );
        if (bucket->isOccupied) {
            void* bucketValue = scu_bucket_value(bucket, hashMap->valueOffset);
            if (hashMap->valueEqualFunc(bucketValue, value)) {
                return true;
            }
        }
    }
    return false;
}

bool scu_hash_map_remove(
    ScuHashMap* restrict hashMap,
    const void* restrict key
) {
    SCU_ASSERT(hashMap != nullptr);
    SCU_ASSERT(key != nullptr);
    if (hashMap->count == 0) {
        return false;
    }
    usize hash = hashMap->keyHashFunc(key);
    isize index = scu_ideal_index(hash, hashMap->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (!bucket->isOccupied) {
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
        if ((bucket->hash == hash) && hashMap->keyEqualFunc(bucket->key, key)) {
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
        if (!nextBucket->isOccupied) {
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
    bucket->isOccupied = false;
    hashMap->count--;
    return true;
}

void scu_hash_map_clear(ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap != nullptr);
    if (hashMap->count > 0) {
        for (isize i = 0; i < hashMap->capacity; i++) {
            ScuBucket* bucket = scu_bucket_at(
                hashMap->buckets,
                i,
                hashMap->bucketSize
            );
            bucket->isOccupied = false;
        }
        hashMap->count = 0;
    }
}

ScuError scu_hash_map_trim_excess(ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap != nullptr);
    if (hashMap->count == 0) {
        scu_free(hashMap->buckets);
        hashMap->buckets = nullptr;
        hashMap->capacity = 0;
        return SCU_ERROR_NONE;
    }
    isize minCapacity = (hashMap->count * SCU_MAX_LOAD_FACTOR_DEN
        + SCU_MAX_LOAD_FACTOR_NUM - 1) / SCU_MAX_LOAD_FACTOR_NUM;
    isize newCapacity = scu_next_power_of_two(minCapacity);
    if (hashMap->capacity <= newCapacity) {
        return SCU_ERROR_NONE;
    }
    ScuBucket* newBuckets = scu_calloc(newCapacity, hashMap->bucketSize);
    if (newBuckets == nullptr) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    isize oldCapacity = hashMap->capacity;
    ScuBucket* oldBuckets = hashMap->buckets;
    hashMap->capacity = newCapacity;
    hashMap->count = 0;
    hashMap->buckets = newBuckets;
    scu_hash_map_rehash_buckets(hashMap, oldBuckets, oldCapacity);
    scu_free(oldBuckets);
    return SCU_ERROR_NONE;
}

ScuHashMapIter scu_hash_map_iter(const ScuHashMap* hashMap) {
    SCU_ASSERT(hashMap != nullptr);
    return (ScuHashMapIter) {
        .hashMap = SCU_CONST_CAST(ScuHashMap*, hashMap),
        .index = -1
    };
}

bool scu_hash_map_iter_move_next(ScuHashMapIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->hashMap != nullptr);
    ScuHashMap* hashMap = iter->hashMap;
    isize index = iter->index;
    SCU_ASSERT((index >= -1) && (index <= hashMap->capacity));
    if (hashMap->count == 0) {
        iter->index = hashMap->capacity;
        return false;
    }
    index++;
    while (index < hashMap->capacity) {
        ScuBucket* bucket = scu_bucket_at(
            hashMap->buckets,
            index,
            hashMap->bucketSize
        );
        if (bucket->isOccupied) {
            iter->index = index;
            return true;
        }
        index++;
    }
    iter->index = hashMap->capacity;
    return false;
}

ScuHashMapEntry scu_hash_map_iter_current(const ScuHashMapIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->hashMap != nullptr);
    ScuHashMap* hashMap = iter->hashMap;
    isize index = iter->index;
    SCU_ASSERT((index >= 0) && (index < hashMap->capacity));
    ScuBucket* bucket = scu_bucket_at(
        hashMap->buckets,
        index,
        hashMap->bucketSize
    );
    SCU_ASSERT(bucket->isOccupied);
    return (ScuHashMapEntry) {
        .key = bucket->key,
        .value = scu_bucket_value(bucket, hashMap->valueOffset)
    };
}

void scu_hash_map_iter_reset(ScuHashMapIter* iter) {
    SCU_ASSERT(iter != nullptr);
    SCU_ASSERT(iter->hashMap != nullptr);
    SCU_ASSERT((iter->index >= -1) && (iter->index <= iter->hashMap->capacity));
    iter->index = -1;
}

void scu_hash_map_free(ScuHashMap* hashMap) {
    if (hashMap != nullptr) {
        scu_free(hashMap->buckets);
        hashMap->buckets = nullptr;
        hashMap->capacity = 0;
        hashMap->count = 0;
        scu_free(hashMap);
    }
}