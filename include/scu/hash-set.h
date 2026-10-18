#ifndef SCU_HASH_SET_H
#define SCU_HASH_SET_H

#include "scu/equal.h"
#include "scu/error.h"
#include "scu/hash.h"
#include "scu/types.h"
#include "scu/util.h"

/** @brief An unordered hash set of elements. */
typedef struct ScuHashSet ScuHashSet;

/**
 * @brief An iterator for a hash set.
 *
 * @warning The internal representation of the iterator is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuHashSetIterator {

    /** @brief The hash set being iterated over. */
    ScuHashSet* hashSet;

    /** @brief The current index within the hash set. */
    Scuisize index;

} ScuHashSetIterator;

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
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_set_begin()` and `scu_hash_set_end()`.
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
 * @warning This function invalidates any iterators previously created with
 * `scu_hash_set_begin()` and `scu_hash_set_end()`.
 *
 * The behavior is undefined if the element is already present in the hash set.
 * Use `scu_hash_set_try_add()` to handle this case gracefully.
 *
 * @note This function dynamically allocates memory using `scu_calloc()`.
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
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_set_begin()` and `scu_hash_set_end()`.
 *
 * @note This function dynamically allocates memory using `scu_calloc()`.
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
 * @warning This function invalidates any iterators previously created with
 * `scu_hash_set_begin()` and `scu_hash_set_end()`.
 *
 * This function does not deallocate the removed element, it only removes it
 * from the hash set. The caller is responsible for deallocating the removed
 * element if it is a pointer to a dynamically allocated object and no other
 * references to it exist.
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
 * @warning This function invalidates any iterators previously created with
 * `scu_hash_set_begin()` and `scu_hash_set_end()`.
 *
 * This function does not deallocate the hash set itself nor the elements
 * contained within, it only resets the number of elements to zero. The caller
 * is responsible for deallocating the individual elements if they are pointers
 * to dynamically allocated objects and no other references to them exist.
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
 * @warning This function may invalidate any iterators previously created with
 * `scu_hash_set_begin()` and `scu_hash_set_end()`.
 *
 * @note This function dynamically allocates memory using `scu_calloc()`.
 */
ScuError scu_hash_set_trim_excess(ScuHashSet* hashSet);

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
 * @brief Returns an iterator to an element of a hash set.
 *
 * @param[in] hashSet The hash set to iterate over.
 * @return An iterator to an element of the hash set.
 *
 * @note If `hashSet` is empty, this function behaves identically to
 * `scu_hash_set_end(hashSet)`, i.e., it returns the end iterator.
 */
ScuHashSetIterator scu_hash_set_begin(const ScuHashSet* hashSet);

/**
 * @brief Returns the end iterator for a hash set.
 *
 * @param[in] hashSet The hash set to iterate over.
 * @return The end iterator for the hash set.
 */
ScuHashSetIterator scu_hash_set_end(const ScuHashSet* hashSet);

/**
 * @brief Determines whether two iterators are equal.
 *
 * @param[in] lhs The first iterator.
 * @param[in] rhs The second iterator.
 * @return `true` if the two iterators are equal, `false` otherwise.
 *
 * @warning The behavior is undefined if `lhs` and `rhs` do not belong to the
 * same hash set.
 */
bool scu_hash_set_iterator_equal(
    const ScuHashSetIterator* lhs,
    const ScuHashSetIterator* rhs
);

/**
 * @brief Moves an iterator to the next element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point after the end iterator returned by `scu_hash_set_end()`.
 */
void scu_hash_set_iterator_next(ScuHashSetIterator* iterator);

/**
 * @brief Moves an iterator to the previous element.
 *
 * @param[in, out] iterator The iterator to move.
 *
 * @warning The behavior is undefined if moving `iterator` would cause it to
 * point before the begin iterator returned by `scu_hash_set_begin()`.
 */
void scu_hash_set_iterator_prev(ScuHashSetIterator* iterator);

/**
 * @brief Returns a pointer to the current element an iterator points to.
 *
 * @param[in] iterator The iterator to dereference.
 * @return A pointer to the current element the iterator points to.
 *
 * The behavior is undefined if `iterator` points before the begin iterator
 * returned by `scu_hash_set_begin()`, if it is the end iterator returned by
 * `scu_hash_set_end()`, or if it points after the end iterator.
 */
void* scu_hash_set_iterator_deref(const ScuHashSetIterator* iterator);

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
#define SCU_HASH_SET_FOREACH(elem, hashSet)                                         \
    for (                                                                           \
        ScuHashSetIterator SCU_XCONCAT(it, __LINE__) = scu_hash_set_begin(hashSet), \
        SCU_XCONCAT(end, __LINE__) = scu_hash_set_end(hashSet);                     \
        !scu_hash_set_iterator_equal(                                               \
            &SCU_XCONCAT(it, __LINE__),                                             \
            &SCU_XCONCAT(end, __LINE__)                                             \
        )                                                                           \
            && (                                                                    \
                (elem) = scu_hash_set_iterator_deref(&SCU_XCONCAT(it, __LINE__)),   \
                true                                                                \
            );                                                                      \
        scu_hash_set_iterator_next(&SCU_XCONCAT(it, __LINE__))                      \
    )

#endif