#define SCU_SHORT_ALIASES

#include <stdbit.h>
#include <stddef.h>
#include "scu/assert.h"
#include "scu/hash-set.h"
#include "scu/math.h"

/** @brief A bucket in a hash set. */
typedef struct ScuBucket {

    /**
     * @brief The hash of the element stored in the bucket.
     *
     * @note The topmost bit indicates whether a bucket is occupied or not.
     *
     * The element itself is stored at `elemOffset` bytes from the start of the
     * bucket, where `elemOffset` is stored by the owning hash set.
     */
    usize hash;

} ScuBucket;

struct ScuHashSet {

    /** @brief The allocator used for memory management. */
    ScuAllocator* allocator;

    /** @brief The size of each element (in bytes). */
    isize elemSize;

    /** @brief The offset of the element within a bucket (in bytes). */
    isize elemOffset;

    /** @brief The effective size of a bucket (in bytes). */
    isize bucketSize;

    /** @brief The alignment of a bucket (in bytes). */
    isize bucketAlign;

    /**
     * @brief The maximum number of elements that could theoretically be stored.
     *
     * @note The capacity is always a power of two and equal to the number of
     * buckets. The actual number of elements that can be stored without a
     * reallocation is lower and depends on the load factor of the hash set.
     */
    isize capacity;

    /** @brief The current number of elements. */
    isize count;

    /** @brief A function used for hashing elements. */
    ScuHashFunc* hashFunc;

    /** @brief A function used for comparing elements for equality. */
    ScuEqualFunc* equalFunc;

    /**
     * @brief The buckets storing the elements.
     *
     * @note This is a dynamically allocated array of `capacity` buckets, or
     * `nullptr` if `capacity` is zero.
     */
    ScuBucket* buckets;

    /** @brief A bucket used as scratch space for insertions. */
    ScuBucket* scratch;

};

/** @brief The default capacity of a hash set. */
static constexpr isize SCU_DEFAULT_CAPACITY = 8;

/** @brief The numerator of the maximum load factor of a hash set. */
static constexpr isize SCU_MAX_LOAD_FACTOR_NUM = 9;

/** @brief The denominator of the maximum load factor of a hash set. */
static constexpr isize SCU_MAX_LOAD_FACTOR_DEN = 10;

/** @brief The bit mask used to check whether a bucket is occupied. */
static constexpr usize SCU_OCCUPIED_BIT = ~(USIZE_MAX >> 1);

SCU_STATIC_ASSERT(
    (usize) (ISIZE_MAX / 2) < SCU_OCCUPIED_BIT,
    "Capacity mask must not overlap the occupied bit."
);

SCU_API ScuHashSet* scu_hash_set_new_impl(
    isize elemSize,
    ScuHashFunc* hashFunc,
    ScuEqualFunc* equalFunc,
    ScuAllocator* allocator
) {
    return scu_hash_set_new_with_capacity_impl(
        elemSize,
        SCU_DEFAULT_CAPACITY,
        hashFunc,
        equalFunc,
        allocator
    );
}

SCU_API ScuHashSet* scu_hash_set_new_with_capacity_impl(
    isize elemSize,
    isize capacity,
    ScuHashFunc* hashFunc,
    ScuEqualFunc* equalFunc,
    ScuAllocator* allocator
) {
    SCU_ASSERT(elemSize > 0);
    SCU_ASSERT(capacity >= 0 && capacity <= ISIZE_MAX / 2);
    SCU_ASSERT(hashFunc);
    SCU_ASSERT(equalFunc);
    SCU_ASSERT(allocator);
    ScuHashSet* hashSet = SCU_NEW(ScuHashSet, allocator);
    if (!hashSet) {
        return nullptr;
    }
    hashSet->allocator = allocator;
    hashSet->elemSize = elemSize;
    isize elemAlign = SCU_MIN(elemSize & -elemSize, SCU_ALIGNOF(max_align_t));
    hashSet->elemOffset = SCU_ALIGN_UP(SCU_SIZEOF(ScuBucket), elemAlign);
    hashSet->bucketAlign = SCU_MAX(elemAlign, SCU_ALIGNOF(ScuBucket));
    hashSet->bucketSize = SCU_ALIGN_UP(
        hashSet->elemOffset + hashSet->elemSize,
        hashSet->bucketAlign
    );
    hashSet->count = 0;
    hashSet->hashFunc = hashFunc;
    hashSet->equalFunc = equalFunc;
    if (capacity > 0) {
        hashSet->capacity = (isize) stdc_bit_ceil((usize) capacity);
        hashSet->buckets = scu_allocate_zeroed(
            hashSet->capacity * hashSet->bucketSize,
            hashSet->bucketAlign,
            hashSet->allocator
        );
        if (!hashSet->buckets) {
            SCU_DELETE(hashSet, hashSet->allocator);
            return nullptr;
        }
    }
    else {
        hashSet->capacity = 0;
        hashSet->buckets = nullptr;
    }
    hashSet->scratch = scu_allocate(
        hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    if (!hashSet->scratch) {
        scu_deallocate(
            hashSet->buckets,
            hashSet->capacity * hashSet->bucketSize,
            hashSet->bucketAlign,
            hashSet->allocator
        );
        SCU_DELETE(hashSet, hashSet->allocator);
        return nullptr;
    }
    return hashSet;
}

SCU_API ScuHashSet* scu_hash_set_clone(const ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    ScuHashSet* clone = SCU_NEW(ScuHashSet, hashSet->allocator);
    if (!clone) {
        return nullptr;
    }
    clone->allocator = hashSet->allocator;
    clone->elemSize = hashSet->elemSize;
    clone->elemOffset = hashSet->elemOffset;
    clone->bucketSize = hashSet->bucketSize;
    clone->bucketAlign = hashSet->bucketAlign;
    clone->capacity = hashSet->capacity;
    clone->count = hashSet->count;
    clone->hashFunc = hashSet->hashFunc;
    clone->equalFunc = hashSet->equalFunc;
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
            hashSet->buckets,
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

SCU_API isize scu_hash_set_capacity(const ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    return hashSet->capacity;
}

SCU_API isize scu_hash_set_count(const ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    return hashSet->count;
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
 * @brief Returns the probe distance of an element with a hash at an index.
 *
 * @param[in] hash     The hash of the element.
 * @param[in] index    The index of the element.
 * @param[in] capacity The capacity (in number of buckets).
 * @return The probe distance of the element.
 *
 * @note The probe distance is the number of positions an element had to travel
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
 * @brief Rehashes old buckets by adding their elements to a hash set.
 *
 * @param[in, out] hashSet     The hash set to rehash into.
 * @param[in, out] oldBuckets  The old buckets to rehash.
 * @param[in]      oldCapacity The old capacity (in number of buckets).
 *
 * @warning This function assumes that the hash set has enough capacity to hold
 * all elements of the old buckets. Additionally, the contents of `oldBuckets`
 * may be modified during the rehashing process for optimization purposes. After
 * this function returns, the state of `oldBuckets` is undefined.
 */
static inline void scu_hash_set_rehash_buckets(
    ScuHashSet* hashSet,
    ScuBucket* oldBuckets,
    isize oldCapacity
) {
    SCU_ASSERT(hashSet);
    SCU_ASSERT(oldBuckets);
    SCU_ASSERT(oldCapacity > 0);
    for (isize i = 0; i < oldCapacity; i++) {
        ScuBucket* oldBucket = scu_bucket_at(
            oldBuckets,
            i,
            hashSet->bucketSize
        );
        if (scu_bucket_is_occupied(oldBucket)) {
            isize index = scu_ideal_index(oldBucket->hash, hashSet->capacity);
            isize distance = 0;
            while (true) {
                ScuBucket* newBucket = scu_bucket_at(
                    hashSet->buckets,
                    index,
                    hashSet->bucketSize
                );
                if (!scu_bucket_is_occupied(newBucket)) {
                    scu_memcpy(newBucket, oldBucket, hashSet->bucketSize);
                    hashSet->count++;
                    break;
                }
                isize otherDistance = scu_probe_distance(
                    newBucket->hash,
                    index,
                    hashSet->capacity
                );
                if (distance > otherDistance) {
                    scu_memswap(oldBucket, newBucket, hashSet->bucketSize);
                    distance = otherDistance;
                }
                index = scu_wrap_index(index + 1, hashSet->capacity);
                distance++;
            }
        }
    }
}

SCU_API ScuError scu_hash_set_ensure_capacity(
    ScuHashSet* hashSet,
    isize capacity
) {
    SCU_ASSERT(hashSet);
    SCU_ASSERT(capacity >= 0);
    isize minCapacity = (capacity * SCU_MAX_LOAD_FACTOR_DEN
        + SCU_MAX_LOAD_FACTOR_NUM - 1) / SCU_MAX_LOAD_FACTOR_NUM;
    if (hashSet->capacity >= minCapacity) {
        return SCU_ERROR_NONE;
    }
    isize newCapacity = (isize) stdc_bit_ceil((usize) minCapacity);
    ScuBucket* newBuckets = scu_allocate_zeroed(
        newCapacity * hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    if (!newBuckets) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    isize oldCapacity = hashSet->capacity;
    isize oldCount = hashSet->count;
    ScuBucket* oldBuckets = hashSet->buckets;
    hashSet->capacity = newCapacity;
    hashSet->count = 0;
    hashSet->buckets = newBuckets;
    if (oldCount > 0) {
        scu_hash_set_rehash_buckets(hashSet, oldBuckets, oldCapacity);
    }
    scu_deallocate(
        oldBuckets,
        oldCapacity * hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    return SCU_ERROR_NONE;
}

SCU_API ScuError scu_hash_set_add(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
) {
    ScuError error = scu_hash_set_try_add(hashSet, elem);
    if (error == SCU_ERROR_ALREADY_PRESENT) {
        SCU_FATAL("The specified element is already present.\n");
    }
    return error;
}

/**
 * @brief Returns the hash of an element with the occupied bit set.
 *
 * @param[in] hashSet The hash set.
 * @param[in] elem    The element to hash.
 * @return The hash of the element with the occupied bit set.
 */
static inline usize scu_hash_elem(const ScuHashSet* hashSet, const void* elem) {
    return hashSet->hashFunc(elem) | SCU_OCCUPIED_BIT;
}

/**
 * @brief Returns a pointer to the element within a bucket.
 *
 * @param[in] bucket     The bucket containing the element.
 * @param[in] elemOffset The offset of the element within the bucket (in bytes).
 * @return A pointer to the element within the bucket.
 */
static inline void* scu_bucket_elem(const ScuBucket* bucket, isize elemOffset) {
    return (byte*) SCU_CONST_CAST(ScuBucket*, bucket) + elemOffset;
}

SCU_API ScuError scu_hash_set_try_add(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
) {
    SCU_ASSERT(hashSet);
    SCU_ASSERT(elem);
    if (
        ((hashSet->count + 1) * SCU_MAX_LOAD_FACTOR_DEN)
            > (hashSet->capacity * SCU_MAX_LOAD_FACTOR_NUM)
    ) {
        isize needed = (hashSet->capacity == 0)
            ? SCU_DEFAULT_CAPACITY
            : hashSet->count + 1;
        ScuError error = scu_hash_set_ensure_capacity(hashSet, needed);
        if (error != SCU_ERROR_NONE) {
            return error;
        }
    }
    ScuBucket* scratch = hashSet->scratch;
    scratch->hash = scu_hash_elem(hashSet, elem);
    scu_memcpy(
        scu_bucket_elem(scratch, hashSet->elemOffset),
        elem,
        hashSet->elemSize
    );
    isize index = scu_ideal_index(scratch->hash, hashSet->capacity);
    isize distance = 0;
    while (true) {
        ScuBucket* bucket = scu_bucket_at(
            hashSet->buckets,
            index,
            hashSet->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            scu_memcpy(bucket, scratch, hashSet->bucketSize);
            hashSet->count++;
            return SCU_ERROR_NONE;
        }
        if (
            bucket->hash == scratch->hash
                && hashSet->equalFunc(
                    scu_bucket_elem(bucket, hashSet->elemOffset),
                    scu_bucket_elem(scratch, hashSet->elemOffset)
                )
        ) {
            return SCU_ERROR_ALREADY_PRESENT;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashSet->capacity
        );
        if (distance > otherDistance) {
            scu_memswap(bucket, scratch, hashSet->bucketSize);
            distance = otherDistance;
        }
        index = scu_wrap_index(index + 1, hashSet->capacity);
        distance++;
    }
}

SCU_API bool scu_hash_set_contains(
    const ScuHashSet* hashSet,
    const void* elem
) {
    SCU_ASSERT(hashSet);
    SCU_ASSERT(elem);
    if (hashSet->count == 0) {
        return false;
    }
    usize hash = scu_hash_elem(hashSet, elem);
    isize index = scu_ideal_index(hash, hashSet->capacity);
    isize distance = 0;
    while (true) {
        const ScuBucket* bucket = scu_bucket_at(
            hashSet->buckets,
            index,
            hashSet->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            return false;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashSet->capacity
        );
        if (distance > otherDistance) {
            return false;
        }
        if (
            bucket->hash == hash
                && hashSet->equalFunc(
                    scu_bucket_elem(bucket, hashSet->elemOffset),
                    elem
                )
        ) {
            return true;
        }
        index = scu_wrap_index(index + 1, hashSet->capacity);
        distance++;
    }
}

SCU_API bool scu_hash_set_remove(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
) {
    SCU_ASSERT(hashSet);
    SCU_ASSERT(elem);
    if (hashSet->count == 0) {
        return false;
    }
    usize hash = scu_hash_elem(hashSet, elem);
    isize index = scu_ideal_index(hash, hashSet->capacity);
    isize distance = 0;
    while (true) {
        const ScuBucket* bucket = scu_bucket_at(
            hashSet->buckets,
            index,
            hashSet->bucketSize
        );
        if (!scu_bucket_is_occupied(bucket)) {
            return false;
        }
        isize otherDistance = scu_probe_distance(
            bucket->hash,
            index,
            hashSet->capacity
        );
        if (distance > otherDistance) {
            return false;
        }
        if (
            bucket->hash == hash
                && hashSet->equalFunc(
                    scu_bucket_elem(bucket, hashSet->elemOffset),
                    elem
                )
        ) {
            break;
        }
        index = scu_wrap_index(index + 1, hashSet->capacity);
        distance++;
    }
    ScuBucket* bucket = scu_bucket_at(
        hashSet->buckets,
        index,
        hashSet->bucketSize
    );
    while (true) {
        isize nextIndex = scu_wrap_index(index + 1, hashSet->capacity);
        ScuBucket* nextBucket = scu_bucket_at(
            hashSet->buckets,
            nextIndex,
            hashSet->bucketSize
        );
        if (!scu_bucket_is_occupied(nextBucket)) {
            break;
        }
        isize nextDistance = scu_probe_distance(
            nextBucket->hash,
            nextIndex,
            hashSet->capacity
        );
        if (nextDistance == 0) {
            break;
        }
        scu_memcpy(bucket, nextBucket, hashSet->bucketSize);
        index = nextIndex;
        bucket = nextBucket;
    }
    bucket->hash = 0;
    hashSet->count--;
    return true;
}

SCU_API void scu_hash_set_clear(ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    if (hashSet->count > 0) {
        for (isize i = 0; i < hashSet->capacity; i++) {
            ScuBucket* bucket = scu_bucket_at(
                hashSet->buckets,
                i,
                hashSet->bucketSize
            );
            bucket->hash = 0;
        }
        hashSet->count = 0;
    }
}

SCU_API ScuError scu_hash_set_trim_excess(ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    if (hashSet->count == 0) {
        scu_deallocate(
            hashSet->buckets,
            hashSet->capacity * hashSet->bucketSize,
            hashSet->bucketAlign,
            hashSet->allocator
        );
        hashSet->buckets = nullptr;
        hashSet->capacity = 0;
        return SCU_ERROR_NONE;
    }
    isize minCapacity = (hashSet->count * SCU_MAX_LOAD_FACTOR_DEN
        + SCU_MAX_LOAD_FACTOR_NUM - 1) / SCU_MAX_LOAD_FACTOR_NUM;
    isize newCapacity = (isize) stdc_bit_ceil((usize) minCapacity);
    if (hashSet->capacity <= newCapacity) {
        return SCU_ERROR_NONE;
    }
    ScuBucket* newBuckets = scu_allocate_zeroed(
        newCapacity * hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    if (!newBuckets) {
        return SCU_ERROR_OUT_OF_MEMORY;
    }
    isize oldCapacity = hashSet->capacity;
    ScuBucket* oldBuckets = hashSet->buckets;
    hashSet->capacity = newCapacity;
    hashSet->count = 0;
    hashSet->buckets = newBuckets;
    scu_hash_set_rehash_buckets(hashSet, oldBuckets, oldCapacity);
    scu_deallocate(
        oldBuckets,
        oldCapacity * hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    return SCU_ERROR_NONE;
}

SCU_API void scu_hash_set_delete(ScuHashSet* hashSet) {
    if (!hashSet) {
        return;
    }
    scu_deallocate(
        hashSet->buckets,
        hashSet->capacity * hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    hashSet->buckets = nullptr;
    scu_deallocate(
        hashSet->scratch,
        hashSet->bucketSize,
        hashSet->bucketAlign,
        hashSet->allocator
    );
    hashSet->scratch = nullptr;
    hashSet->capacity = 0;
    hashSet->count = 0;
    SCU_DELETE(hashSet, hashSet->allocator);
}

/**
 * @brief Returns the index of the first occupied bucket at or after an index.
 *
 * @param[in] hashSet  The hash set to examine.
 * @param[in] startIdx The index to start the search at.
 * @return The index of the first occupied bucket, or the capacity if there is
 * none.
 */
static inline isize scu_next_occupied_index(
    const ScuHashSet* hashSet,
    isize startIdx
) {
    SCU_ASSERT(hashSet);
    SCU_ASSERT(startIdx >= 0);
    for (isize i = startIdx; i < hashSet->capacity; i++) {
        const ScuBucket* bucket = scu_bucket_at(
            hashSet->buckets,
            i,
            hashSet->bucketSize
        );
        if (scu_bucket_is_occupied(bucket)) {
            return i;
        }
    }
    return hashSet->capacity;
}

SCU_API ScuHashSetIterator scu_hash_set_begin(const ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    return (ScuHashSetIterator) {
        .hashSet = SCU_CONST_CAST(ScuHashSet*, hashSet),
        .index = scu_next_occupied_index(hashSet, 0)
    };
}

SCU_API ScuHashSetIterator scu_hash_set_end(const ScuHashSet* hashSet) {
    SCU_ASSERT(hashSet);
    return (ScuHashSetIterator) {
        .hashSet = SCU_CONST_CAST(ScuHashSet*, hashSet),
        .index = hashSet->capacity
    };
}

SCU_API bool scu_hash_set_iterator_equal(
    const ScuHashSetIterator* lhs,
    const ScuHashSetIterator* rhs
) {
    SCU_ASSERT(lhs);
    SCU_ASSERT(rhs);
    SCU_ASSERT(lhs->hashSet == rhs->hashSet);
    return lhs->index == rhs->index;
}

SCU_API void scu_hash_set_iterator_next(ScuHashSetIterator* iterator) {
    SCU_ASSERT(iterator);
    SCU_ASSERT(iterator->index < iterator->hashSet->capacity);
    iterator->index = scu_next_occupied_index(
        iterator->hashSet,
        iterator->index + 1
    );
}

SCU_API void scu_hash_set_iterator_prev(ScuHashSetIterator* iterator) {
    SCU_ASSERT(iterator);
    ScuHashSet* hashSet = iterator->hashSet;
    isize index = iterator->index - 1;
    while (index >= 0) {
        const ScuBucket* bucket = scu_bucket_at(
            hashSet->buckets,
            index,
            hashSet->bucketSize
        );
        if (scu_bucket_is_occupied(bucket)) {
            break;
        }
        index--;
    }
    SCU_ASSERT(index >= 0);
    iterator->index = index;
}

SCU_API void* scu_hash_set_iterator_deref(const ScuHashSetIterator* iterator) {
    SCU_ASSERT(iterator);
    ScuHashSet* hashSet = iterator->hashSet;
    isize index = iterator->index;
    SCU_ASSERT(index >= 0 && index < hashSet->capacity);
    const ScuBucket* bucket = scu_bucket_at(
        hashSet->buckets,
        index,
        hashSet->bucketSize
    );
    SCU_ASSERT(scu_bucket_is_occupied(bucket));
    return scu_bucket_elem(bucket, hashSet->elemOffset);
}