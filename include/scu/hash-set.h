#ifndef SCU_HASH_SET_H
#define SCU_HASH_SET_H

#include "scu/common.h"
#include "scu/equal.h"
#include "scu/error.h"
#include "scu/hash.h"
#include "scu/types.h"

/** @brief An unordered hash set of elements. */
typedef struct ScuHashSet ScuHashSet;

/**
 * @brief An iterator for a hash set.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuHashSetIter {

    /** @brief The hash set being iterated over. */
    ScuHashSet* hashSet;

    /** @brief The current index within the hash set. */
    Scuisize index;

} ScuHashSetIter;

/**
 * @brief Constructs a hash set with a specified element size, hash function,
 * equality function, and an unspecified default capacity.
 *
 * @param[in] elemSize  The size of each element (in bytes).
 * @param[in] hashFunc  A function used for hashing elements.
 * @param[in] equalFunc A function used for comparing elements for equality.
 * @return A pointer to the hash set, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the hash set with
 * `scu_hash_set_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()` and
 * `scu_calloc()`.
 */
ScuHashSet* scu_hash_set_new(
    Scuisize elemSize,
    ScuHashFunc* hashFunc,
    ScuEqualFunc* equalFunc
);

/**
 * @brief Constructs a hash set with a specified element size, hash function,
 * equality function, and initial capacity.
 *
 * @param[in] elemSize  The size of each element (in bytes).
 * @param[in] capacity  The initial capacity (in number of elements).
 * @param[in] hashFunc  A function used for hashing elements.
 * @param[in] equalFunc A function used for comparing elements for equality.
 * @return A pointer to the hash set, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the hash set with
 * `scu_hash_set_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()` and
 * `scu_calloc()`.
 */
ScuHashSet* scu_hash_set_new_with_capacity(
    Scuisize elemSize,
    Scuisize capacity,
    ScuHashFunc* hashFunc,
    ScuEqualFunc* equalFunc
);

/**
 * @brief Creates a shallow copy of a hash set.
 *
 * @param[in] hashSet The hash set to clone.
 * @return A pointer to the cloned hash set, or `nullptr` on failure.
 *
 * @warning The caller is responsible for deallocating the cloned hash set with
 * `scu_hash_set_free()` when it is no longer needed.
 *
 * @note This function dynamically allocates memory using `scu_malloc()`.
 */
ScuHashSet* scu_hash_set_clone(const ScuHashSet* hashSet);

/**
 * @brief Returns the capacity of a hash set, i.e., the maximum number of
 * elements that can be stored before a reallocation is required.
 *
 * @param[in] hashSet The hash set to examine.
 * @return The capacity of the hash set.
 *
 * @note The capacity can be influenced to a certain extent using
 * `scu_hash_set_ensure_capacity()` and `scu_hash_set_trim_excess()`.
 */
Scuisize scu_hash_set_capacity(const ScuHashSet* hashSet);

/**
 * @brief Returns the number of elements in a hash set.
 *
 * @param[in] hashSet The hash set to examine.
 * @return The number of elements in the hash set.
 */
Scuisize scu_hash_set_count(const ScuHashSet* hashSet);

/**
 * @brief Ensures that a hash set has at least a specified capacity.
 *
 * @param[in, out] hashSet  The hash set whose capacity to ensure.
 * @param[in]      capacity The desired capacity (in number of elements).
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @note This function dynamically allocates memory using `scu_calloc()`.
 */
ScuError scu_hash_set_ensure_capacity(ScuHashSet* hashSet, Scuisize capacity);

/**
 * @brief Adds an element to a hash set.
 *
 * @param[in, out] hashSet The hash set to add the element to.
 * @param[in]      elem    The element to add.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @warning The behavior is undefined if the element is already present in the
 * hash set. Use `scu_hash_set_try_add()` to handle this case gracefully.
 *
 * @note This function dynamically allocates memory using `scu_malloc()` and
 * `scu_calloc()`.
 */
ScuError scu_hash_set_add(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
);

/**
 * @brief Tries to add an element to a hash set.
 *
 * @param[in, out] hashSet The hash set to add the element to.
 * @param[in]      elem    The element to add.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred,
 * `SCU_ERROR_ALREADY_PRESENT` if the element is already present in the hash
 * set, or `SCU_ERROR_NONE` on success.
 *
 * @note This function dynamically allocates memory using `scu_malloc()` and
 * `scu_calloc()`.
 */
ScuError scu_hash_set_try_add(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
);

/**
 * @brief Determines whether a hash set contains a specified element.
 *
 * @param[in] hashSet The hash set to examine.
 * @param[in] elem    The element to search for.
 * @return `true` if the element is present in the hash set, `false` otherwise.
 */
bool scu_hash_set_contains(const ScuHashSet* hashSet, const void* elem);

/**
 * @brief Removes a specified element from a hash set.
 *
 * @param[in, out] hashSet The hash set to remove the element from.
 * @param[in]      elem    The element to remove.
 * @return `true` if the element was present and removed from the hash set,
 * `false` otherwise.
 *
 * @warning This function does not deallocate the removed element, it only
 * removes it from the hash set. The caller is responsible for deallocating the
 * removed element if it is a pointer to a dynamically allocated object and no
 * other references to it exist.
 *
 * @note Consider using `scu_hash_set_trim_excess()` if you wish to reduce the
 * memory usage of the hash set after removing elements.
 */
bool scu_hash_set_remove(
    ScuHashSet* restrict hashSet,
    const void* restrict elem
);

/**
 * @brief Removes all elements from a hash set.
 *
 * @param[in, out] hashSet The hash set to clear.
 *
 * @warning This function does not deallocate the hash set itself nor the
 * elements contained within, it only resets the number of elements to zero. The
 * caller is responsible for deallocating the individual elements if they are
 * pointers to dynamically allocated objects and no other references to them
 * exist.
 *
 * @note Consider using `scu_hash_set_trim_excess()` if you wish to reduce the
 * memory usage of the hash set after clearing it.
 */
void scu_hash_set_clear(ScuHashSet* hashSet);

/**
 * @brief Trims the excess capacity of a hash set to match its current number of
 * elements.
 *
 * @param[in, out] hashSet The hash set to trim.
 * @return `SCU_ERROR_OUT_OF_MEMORY` if an out-of-memory condition occurred, or
 * `SCU_ERROR_NONE` on success.
 *
 * @note This function dynamically allocates memory using `scu_calloc()`.
 */
ScuError scu_hash_set_trim_excess(ScuHashSet* hashSet);

/**
 * @brief Returns an iterator for a hash set.
 *
 * @param[in] hashSet The hash set to iterate over.
 * @return An iterator for the hash set.
 *
 * @warning The behavior is undefined if the hash set being iterated over is
 * modified (e.g., elements are added or removed) while the iterator is in use.
 *
 * @note The iterator is initially positioned before the first element of the
 * hash set (if any). This means that `scu_hash_set_iter_move_next()` must be
 * called before accessing the first and subsequent elements with
 * `scu_hash_set_iter_current()`.
 */
ScuHashSetIter scu_hash_set_iter(const ScuHashSet* hashSet);

/**
 * @brief Advances a hash set iterator to the next element.
 *
 * @param[in, out] iter The iterator to advance.
 * @return `true` if the iterator was successfully advanced to the next element,
 * `false` otherwise (i.e., the hash set does not contain any more elements).
 */
bool scu_hash_set_iter_move_next(ScuHashSetIter* iter);

/**
 * @brief Returns the current element of a hash set iterator.
 *
 * @param[in] iter The iterator to examine.
 * @return A pointer to the current element.
 */
void* scu_hash_set_iter_current(const ScuHashSetIter* iter);

/**
 * @brief Resets a hash set iterator to its initial position.
 *
 * @param[in, out] iter The iterator to reset.
 *
 * @note The iterator is initially positioned before the first element of the
 * hash set (if any). This means that `scu_hash_set_iter_move_next()` must be
 * called before accessing the first and subsequent elements with
 * `scu_hash_set_iter_current()`.
 */
void scu_hash_set_iter_reset(ScuHashSetIter* iter);

/**
 * @brief Deallocates a hash set.
 *
 * @param[in, out] hashSet The hash set to deallocate.
 *
 * @warning This function only deallocates the memory occupied by the hash set
 * itself, but not the elements contained within. The caller is responsible for
 * deallocating the individual elements if they are pointers to dynamically
 * allocated objects and no other references to them exist.
 *
 * The behavior is undefined if the hash set is used after it has been
 * deallocated.
 *
 * @note If `hashSet` is `nullptr`, this function does nothing.
 */
void scu_hash_set_free(ScuHashSet* hashSet);

/**
 * @brief Iterates over each element in a hash set.
 *
 * This macro expands to a for loop that iterates over each element in the hash
 * set. During each iteration, the provided variable is assigned a pointer to
 * the current element.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * // T is the type of the elements stored in the hash set.
 * ScuHashSet* elems = scu_hash_set_new(SCU_SIZEOF(T), ...);
 * ...
 * T* elem;
 * SCU_HASH_SET_FOREACH(elem, elems) {
 *     // Do something with *elem.
 * }
 * ```
 *
 * @param[out] elem    A pointer to the current element during each iteration.
 * @param[in]  hashSet The hash set to iterate over.
 *
 * @warning The behavior is undefined if the hash set is modified (e.g.,
 * elements are added or removed) while being iterated over.
 *
 * @note The variable `elem` must be declared manually before the loop. It must
 * be of a pointer type compatible with the hash set's element type.
 */
#define SCU_HASH_SET_FOREACH(elem, hashSet)                                     \
    for (                                                                       \
        ScuHashSetIter SCU_XCONCAT(it, __LINE__) = scu_hash_set_iter(hashSet);  \
        scu_hash_set_iter_move_next(&SCU_XCONCAT(it, __LINE__))                 \
            && (                                                                \
                (elem) = scu_hash_set_iter_current(&SCU_XCONCAT(it, __LINE__)), \
                true                                                            \
            );                                                                  \
    )

#endif