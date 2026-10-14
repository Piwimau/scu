#ifndef SCU_MATH_H
#define SCU_MATH_H

/**
 * @brief Returns the minimum of two values.
 *
 * @param[in] lhs The first value.
 * @param[in] rhs The second value.
 * @return The minimum of `lhs` and `rhs`.
 */
#define SCU_MIN(lhs, rhs) (((lhs) < (rhs)) ? (lhs) : (rhs))

/**
 * @brief Returns the maximum of two values.
 *
 * @param[in] lhs The first value.
 * @param[in] rhs The second value.
 * @return The maximum of `lhs` and `rhs`.
 */
#define SCU_MAX(lhs, rhs) (((lhs) > (rhs)) ? (lhs) : (rhs))

/**
 * @brief Returns the absolute of a value.
 *
 * @param[in] x The value to compute the absolute of.
 * @return The absolute of `x`.
 */
#define SCU_ABS(x) (((x) < 0) ? -(x) : (x))

#endif