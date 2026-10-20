#ifndef SCU_HASH_MAP_H
#define SCU_HASH_MAP_H

#include "scu/api.h"
#include "scu/equal.h"
#include "scu/error.h"
#include "scu/hash.h"
#include "scu/memory.h"
#include "scu/types.h"
#include "scu/util.h"

/** @brief An unordered collection of key-value pairs. */
typedef struct ScuHashMap ScuHashMap;

/**
 * @brief An iterator for a hash map.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuHashMapIterator {

    /** @brief The hash map being iterated over. */
    ScuHashMap* hashMap;

    /** @brief The current index within the hash map. */
    Scuisize index;

} ScuHashMapIterator;

/** @brief An entry in a hash map. */
typedef struct ScuHashMapEntry {

    /** @brief The key of the entry. */
    void* key;

    /** @brief The value of the entry. */
    void* value;

} ScuHashMapEntry;

/**
 * @brief Constructs a hash map with the specified key and value sizes, hash and
 * equality functions, and an unspecified default capacity.
 *
 * @param[in] keySize        The size of each key (in bytes).
 * @param[in] valueSize      The size of each value (in bytes).
 * @param[in] keyHashFunc    A function used for hashing keys.
 * @param[in] keyEqualFunc   A function used for comparing keys for equality.
 * @param[in] valueEqualFunc A function used for comparing values for equality.
 * @param[in] allocator      The allocator to use for memory management.
 * @return A pointer to the hash map, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the hash map with
 * `scu_hash_map_delete()` when it is no longer needed.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `scu_hash_map_new()` macro instead.
 */
SCU_API ScuHashMap* scu_hash_map_new_impl(
    Scuisize keySize,
    Scuisize valueSize,
    ScuHashFunc* keyHashFunc,
    ScuEqualFunc* keyEqualFunc,
    ScuEqualFunc* valueEqualFunc,
    ScuAllocator* allocator
);

/**
 * @brief Constructs a hash map with the specified key and value types, hash and
 * equality functions, and an unspecified default capacity.
 *
 * @param[in] keyType        The type of each key.
 * @param[in] valueType      The type of each value.
 * @param[in] keyHashFunc    A function used for hashing keys.
 * @param[in] keyEqualFunc   A function used for comparing keys for equality.
 * @param[in] valueEqualFunc A function used for comparing values for equality.
 * @param[in] ...            The allocator to use for memory management.
 * @return A pointer to the hash map, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the hash map with
 * `scu_hash_map_delete()` when it is no longer needed.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define scu_hash_map_new(                     \
    keyType,                                  \
    valueType,                                \
    keyHashFunc,                              \
    keyEqualFunc,                             \
    valueEqualFunc,                           \
    ...                                       \
)                                             \
    scu_hash_map_new_impl(                    \
        SCU_SIZEOF(keyType),                  \
        SCU_SIZEOF(valueType),                \
        (keyHashFunc),                        \
        (keyEqualFunc),                       \
        (valueEqualFunc),                     \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Constructs a hash map with the specified key and value sizes, hash and
 * equality functions, and initial capacity.
 *
 * @param[in] keySize        The size of each key (in bytes).
 * @param[in] valueSize      The size of each value (in bytes).
 * @param[in] capacity       The initial capacity (in number of key-value
 *                           pairs).
 * @param[in] keyHashFunc    A function used for hashing keys.
 * @param[in] keyEqualFunc   A function used for comparing keys for equality.
 * @param[in] valueEqualFunc A function used for comparing values for equality.
 * @param[in] allocator      The allocator to use for memory management.
 * @return A pointer to the hash map, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the hash map with
 * `scu_hash_map_delete()` when it is no longer needed.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `scu_hash_map_new_with_capacity()` macro instead.
 */
SCU_API ScuHashMap* scu_hash_map_new_with_capacity_impl(
    Scuisize keySize,
    Scuisize valueSize,
    Scuisize capacity,
    ScuHashFunc* keyHashFunc,
    ScuEqualFunc* keyEqualFunc,
    ScuEqualFunc* valueEqualFunc,
    ScuAllocator* allocator
);

/**
 * @brief Constructs a hash map with the specified key and value types, hash and
 * equality functions, and initial capacity.
 *
 * @param[in] keyType        The type of each key.
 * @param[in] valueType      The type of each value.
 * @param[in] capacity       The initial capacity (in number of key-value
 *                           pairs).
 * @param[in] keyHashFunc    A function used for hashing keys.
 * @param[in] keyEqualFunc   A function used for comparing keys for equality.
 * @param[in] valueEqualFunc A function used for comparing values for equality.
 * @param[in] ...            The allocator to use for memory management.
 * @return A pointer to the hash map, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the hash map with
 * `scu_hash_map_delete()` when it is no longer needed.
 *
 * @note The last argument is optional and specifies the allocator to use. If
 * omitted, the default allocator will be used.
 */
#define scu_hash_map_new_with_capacity(       \
    keyType,                                  \
    valueType,                                \
    capacity,                                 \
    keyHashFunc,                              \
    keyEqualFunc,                             \
    valueEqualFunc,                           \
    ...                                       \
)                                             \
    scu_hash_map_new_with_capacity_impl(      \
        SCU_SIZEOF(keyType),                  \
        SCU_SIZEOF(valueType),                \
        (capacity),                           \
        (keyHashFunc),                        \
        (keyEqualFunc),                       \
        (valueEqualFunc),                     \
        SCU_ALLOCATOR_OR_DEFAULT(__VA_ARGS__) \
    )

/**
 * @brief Creates a shallow copy of a hash map.
 *
 * @param[in] hashMap The hash map to clone.
 * @return A pointer to the cloned hash map, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the cloned hash map with
 * `scu_hash_map_delete()` when it is no longer needed.
 */
SCU_API ScuHashMap* scu_hash_map_clone(const ScuHashMap* hashMap);

/**
 * @brief Returns the capacity of a hash map, i.e., the maximum number of
 * key-value pairs that can be stored before a reallocation is required.
 *
 * @param[in] hashMap The hash map to examine.
 * @return The capacity of the hash map.
 *
 * @note The capacity can be influenced to a certain extent using
 * `scu_hash_map_ensure_capacity()` and `scu_hash_map_trim_excess()`.
 */
SCU_API Scuisize scu_hash_map_capacity(const ScuHashMap* hashMap);

/**
 * @brief Returns the number of key-value pairs in a hash map.
 *
 * @param[in] hashMap The hash map to examine.
 * @return The number of key-value pairs in the hash map.
 */
SCU_API Scuisize scu_hash_map_count(const ScuHashMap* hashMap);

/**
 * @brief Ensures that a hash map has at least a specified capacity.
 *
 * @param[in, out] hashMap  The hash map whose capacity to ensure.
 * @param[in]      capacity The desired capacity (in number of key-value pairs).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 */
SCU_API ScuError scu_hash_map_ensure_capacity(
    ScuHashMap* hashMap,
    Scuisize capacity
);

/**
 * @brief Adds a key-value pair to a hash map.
 *
 * @param[in, out] hashMap The hash map to add the key-value pair to.
 * @param[in]      key     The key to add.
 * @param[in]      value   The value to associate with the key.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 *
 * The behavior is undefined if the key is already present in the hash map. Use
 * `scu_hash_map_try_add()` to handle this case gracefully.
 */
SCU_API ScuError scu_hash_map_add(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
);

/**
 * @brief Tries to add a key-value pair to a hash map.
 *
 * @param[in, out] hashMap The hash map to add the key-value pair to.
 * @param[in]      key     The key to add.
 * @param[in]      value   The value to associate with the key.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_ALREADY_PRESENT` if the key is already present in the hash map, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 */
SCU_API ScuError scu_hash_map_try_add(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
);

/**
 * @brief Gets the value associated with a key in a hash map.
 *
 * @param[in] hashMap The hash map to examine.
 * @param[in] key     The key to look up.
 * @return A pointer to the value associated with the key.
 *
 * @warning The behavior is undefined if the key is not present in the hash map.
 * Use `scu_hash_map_try_get()` to handle this case gracefully.
 */
SCU_API void* scu_hash_map_get(const ScuHashMap* hashMap, const void* key);

/**
 * @brief Tries to get the value associated with a key in a hash map.
 *
 * @param[in]  hashMap The hash map to examine.
 * @param[in]  key     The key to look up.
 * @param[out] value   A pointer to the value associated with the specified key
 *                     on success, or `nullptr` on failure.
 * @return `true` if the key was present in the hash map, `false` otherwise.
 *
 * @note This function is an implementation detail and not intended to be called
 * directly. Use the `scu_hash_map_try_get()` macro instead.
 */
SCU_API bool scu_hash_map_try_get_impl(
    const ScuHashMap* restrict hashMap,
    const void* restrict key,
    void* restrict* restrict value
);

/**
 * @brief Tries to get the value associated with a key in a hash map.
 *
 * @param[in]  hashMap The hash map to examine.
 * @param[in]  key     The key to look up.
 * @param[out] value   A pointer to the value associated with the specified key
 *                     on success, or `nullptr` on failure.
 * @return `true` if the key was present in the hash map, `false` otherwise.
 */
#define scu_hash_map_try_get(hashMap, key, value)             \
    scu_hash_map_try_get_impl(hashMap, key, (void**) (value))

/**
 * @brief Associates a key with a new value in a hash map.
 *
 * @param[in, out] hashMap The hash map to set the value in.
 * @param[in]      key     The key to associate with the new value.
 * @param[in]      value   The new value to associate with the key.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 *
 * @note If the key is already present in the hash map, its associated value is
 * replaced with the new value. Otherwise, a new key-value pair is added to the
 * hash map with the specified key and value. Note that this function is
 * primarily intended for updating the values of existing keys, and is optimized
 * for this use case. If it is known that the key is (likely) not present in the
 * hash map, consider using `scu_hash_map_add()` or `scu_hash_map_try_add()`
 * instead, which may be more efficient in this case.
 */
SCU_API ScuError scu_hash_map_set(
    ScuHashMap* restrict hashMap,
    const void* restrict key,
    const void* restrict value
);

/**
 * @brief Determines whether a key is present in a hash map.
 *
 * @param[in] hashMap The hash map to examine.
 * @param[in] key     The key to search for.
 * @return `true` if the key is present in the hash map, `false` otherwise.
 */
SCU_API bool scu_hash_map_contains_key(
    const ScuHashMap* hashMap,
    const void* key
);

/**
 * @brief Determines whether a value is present in a hash map.
 *
 * @param[in] hashMap The hash map to examine.
 * @param[in] value   The value to search for.
 * @return `true` if the value is present in the hash map, `false` otherwise.
 */
SCU_API bool scu_hash_map_contains_value(
    const ScuHashMap* hashMap,
    const void* value
);

/**
 * @brief Removes a key-value pair with a specified key from a hash map.
 *
 * @param[in, out] hashMap The hash map to remove the key-value pair from.
 * @param[in]      key     The key of the key-value pair to remove.
 * @return `true` if the key-value pair with the specified key was present and
 * removed, `false` otherwise.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 *
 * This function does not deallocate the key or value, it only removes the
 * key-value pair from the hash map. The caller is responsible for deallocating
 * the removed key and value if they are pointers to dynamically allocated
 * objects and no other references to them exist.
 *
 * @note Consider using `scu_hash_map_trim_excess()` if you wish to reduce the
 * memory usage of the hash map after removing key-value pairs.
 */
SCU_API bool scu_hash_map_remove(
    ScuHashMap* restrict hashMap,
    const void* restrict key
);

/**
 * @brief Removes all key-value pairs from a hash map.
 *
 * @param[in, out] hashMap The hash map to clear.
 *
 * @warning This function invalidates any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 *
 * This function does not deallocate the hash map itself nor the key-value pairs
 * contained within, it only resets the number of key-value pairs to zero. The
 * caller is responsible for deallocating the individual keys and values if they
 * are pointers to dynamically allocated objects and no other references to them
 * exist.
 *
 * @note Consider using `scu_hash_map_trim_excess()` if you wish to reduce the
 * memory usage of the hash map after clearing it.
 */
SCU_API void scu_hash_map_clear(ScuHashMap* hashMap);

/**
 * @brief Trims the excess capacity of a hash map to match its current number of
 * key-value pairs.
 *
 * @param[in, out] hashMap The hash map to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_map_begin()` and `scu_hash_map_end()`.
 */
SCU_API ScuError scu_hash_map_trim_excess(ScuHashMap* hashMap);

/**
 * @brief Deallocates a hash map.
 *
 * @param[in, out] hashMap The hash map to deallocate.
 *
 * @warning This function only deallocates the memory occupied by the hash map
 * itself, but not the key-value pairs contained within. The caller is
 * responsible for deallocating the individual keys and values if they are
 * pointers to dynamically allocated objects and no other references to them
 * exist.
 *
 * The behavior is undefined if the hash map is used after it has been
 * deallocated.
 *
 * @note If `hashMap` is `nullptr`, this function does nothing.
 */
SCU_API void scu_hash_map_delete(ScuHashMap* hashMap);

/**
 * @brief Returns an iterator to an entry in a hash map.
 *
 * @param[in] hashMap The hash map to iterate over.
 * @return An iterator to an entry in the hash map.
 *
 * @note If `hashMap` is empty, this function behaves identically to
 * `scu_hash_map_end(hashMap)`, i.e., it returns the end iterator.
 */
SCU_API ScuHashMapIterator scu_hash_map_begin(const ScuHashMap* hashMap);

/**
 * @brief Returns the end iterator for a hash map.
 *
 * @param[in] hashMap The hash map to iterate over.
 * @return The end iterator for the hash map.
 */
SCU_API ScuHashMapIterator scu_hash_map_end(const ScuHashMap* hashMap);

/**
 * @brief Determines whether two iterators are equal.
 *
 * @param[in] lhs The first iterator.
 * @param[in] rhs The second iterator.
 * @return `true` if the two iterators are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` and `rhs` do not belong to the
 * same hash map.
 */
SCU_API bool scu_hash_map_iterator_equal(
    const ScuHashMapIterator* lhs,
    const ScuHashMapIterator* rhs
);

/**
 * @brief Moves an iterator to the next entry.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point after the end iterator returned by `scu_hash_map_end()`.
 */
SCU_API void scu_hash_map_iterator_next(ScuHashMapIterator* iterator);

/**
 * @brief Moves an iterator to the previous entry.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point before the begin iterator returned by `scu_hash_map_begin()`.
 */
SCU_API void scu_hash_map_iterator_prev(ScuHashMapIterator* iterator);

/**
 * @brief Returns an entry for the current element an iterator points to.
 *
 * @param[in] iterator The iterator to dereference.
 * @return An entry for the current element the iterator points to.
 *
 * @warning The behavior is undefined if `iterator` points before the begin
 * iterator returned by `scu_hash_map_begin()`, if it is the end iterator
 * returned by `scu_hash_map_end()`, or if it points after the end iterator.
 */
SCU_API ScuHashMapEntry scu_hash_map_iterator_deref(
    const ScuHashMapIterator* iterator
);

/**
 * @brief Iterates over each key-value pair in a hash map.
 *
 * This macro expands to a for loop that iterates over each key-value pair in
 * the hash map. During each iteration, the provided variable is assigned an
 * entry representing the current key-value pair.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * // K and V are the types of the keys and values stored in the hash map.
 * ScuHashMap* map = scu_hash_map_new(SCU_SIZEOF(K), SCU_SIZEOF(V), ...);
 * ...
 * ScuHashMapEntry entry;
 * SCU_HASH_MAP_FOREACH(entry, map) {
 *     // Do something with the key and value.
 *     K* key = entry.key;
 *     V* value = entry.value;
 * }
 * ```
 *
 * @param[out] entry   An entry representing the current key-value pair.
 * @param[in]  hashMap The hash map to iterate over.
 *
 * @warning The behavior is undefined if the hash map is modified (e.g.,
 * key-value pairs are added or removed) while being iterated over.
 *
 * @note The variable `entry` must be declared manually before the loop. It must
 * be of type `ScuHashMapEntry`.
 */
#define SCU_HASH_MAP_FOREACH(entry, hashMap)                                        \
    for (                                                                           \
        ScuHashMapIterator SCU_XCONCAT(it, __LINE__) = scu_hash_map_begin(hashMap), \
        SCU_XCONCAT(end, __LINE__) = scu_hash_map_end(hashMap);                     \
        !scu_hash_map_iterator_equal(                                               \
            &SCU_XCONCAT(it, __LINE__),                                             \
            &SCU_XCONCAT(end, __LINE__)                                             \
        )                                                                           \
            && (                                                                    \
                (entry) = scu_hash_map_iterator_deref(&SCU_XCONCAT(it, __LINE__)),  \
                true                                                                \
            );                                                                      \
        scu_hash_map_iterator_next(&SCU_XCONCAT(it, __LINE__))                      \
    )

#endif