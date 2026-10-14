#ifndef SCU_BENCH_H
#define SCU_BENCH_H

#include "scu/error.h"
#include "scu/time.h"
#include "scu/types.h"

/** @brief Statistics of a benchmark. */
typedef struct ScuBenchStats {

    /** @brief The minimum time across all iterations (in nanoseconds). */
    Scui64 minNs;

    /** @brief The maximum time across all iterations (in nanoseconds). */
    Scui64 maxNs;

    /** @brief The mean time across all iterations (in nanoseconds). */
    Scuf64 meanNs;

    /** @brief The median time across all iterations (in nanoseconds). */
    Scuf64 medianNs;

    /**
     * @brief The standard deviation of the time across all iterations (in
     * nanoseconds).
     */
    Scuf64 stdDevNs;

} ScuBenchStats;

/** @brief A result of a benchmark. */
typedef struct ScuBenchResult {

    /** @brief The wall time statistics. */
    ScuBenchStats wall;

    /** @brief The CPU time statistics. */
    ScuBenchStats cpu;

    /** @brief The number of iterations performed. */
    Scuisize iterations;

    /** @brief Whether the benchmark was successful. */
    ScuError error;

} ScuBenchResult;

/**
 * @brief A context for a benchmark.
 *
 * @warning This structure is an implementation detail and not intended to be
 * used directly. Its internal representation must not be relied upon and may
 * change without notice. Most importantly, the behavior is undefined if its
 * fields are accessed directly.
 */
typedef struct ScuBenchContext {

    /** @brief The timing mode for measuring CPU time. */
    ScuTimingMode timingMode;

    /** @brief The timing result for the current iteration. */
    ScuTimingResult timingResult;

    /** @brief The wall time samples (in nanoseconds). */
    Scui64* wallSamples;

    /** @brief The CPU time samples (in nanoseconds). */
    Scui64* cpuSamples;

    /** @brief The number of warmup iterations. */
    Scuisize warmup;

    /** @brief The number of benchmark iterations (after warmup). */
    Scuisize iterations;

    /** @brief The current iteration. */
    Scuisize iteration;

    /** @brief Whether the benchmark was successful. */
    ScuError error;

    /** @brief A pointer for storing the result of the benchmark. */
    ScuBenchResult* benchResult;

} ScuBenchContext;

/**
 * @brief Constructs a benchmark context.
 *
 * @param[in] timingMode  The timing mode for measuring CPU time.
 * @param[in] warmup      The number of warmup iterations.
 * @param[in] iterations  The number of benchmark iterations (after warmup).
 * @param[in] benchResult A pointer for storing the result of the benchmark.
 * @return A benchmark context.
 *
 * @warning Although the context itself is not dynamically allocated, some of
 * its associated resources (e.g., the samples) are. It must be passed to
 * `scu_bench_context_is_running()` and `scu_bench_context_advance()`, which
 * manage the underlying resources and deallocate them when the benchmark is
 * complete.
 *
 * The behavior is undefined if `warmup` or `iterations` is negative, or if
 * `benchResult` is `nullptr`.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `SCU_BENCH()` macro instead.
 */
ScuBenchContext scu_bench_context_new(
    ScuTimingMode timingMode,
    Scuisize warmup,
    Scuisize iterations,
    ScuBenchResult* benchResult
);

/**
 * @brief Determines whether a benchmark is still running.
 *
 * @param[in, out] context The context of the benchmark to examine.
 * @return `true` if the specified benchmark is still running, `false`
 * otherwise.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `SCU_BENCH()` macro instead.
 *
 * @warning The behavior is undefined if `context` is `nullptr`.
 */
bool scu_bench_context_is_running(ScuBenchContext* context);

/**
 * @brief Advances a benchmark to the next iteration.
 *
 * @param[in, out] context The context of the benchmark to advance.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `SCU_BENCH()` macro instead.
 *
 * @warning The behavior is undefined if `context` is `nullptr`.
 */
void scu_bench_context_advance(ScuBenchContext* context);

/**
 * @brief Benchmarks a block of code.
 *
 * This macro simplifies the process of benchmarking a block of code. It
 * constructs a benchmark context, executes the block of code for the specified
 * number of iterations (with optional warmup iterations), and stores the
 * benchmark results in `*result`.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * ScuBenchResult result;
 * SCU_BENCH(SCU_TIMING_MODE_PROCESS, 10, 100, &result) {
 *     // Some code to be benchmarked...
 * }
 * if (result.error != SCU_ERROR_NONE) {
 *     // Handle the error...
 * }
 * else {
 *     // Use result.wall, result.cpu, and result.iterations...
 * }
 * ```
 *
 * @param[in] mode       The timing mode for measuring CPU time.
 * @param[in] warmup     The number of warmup iterations.
 * @param[in] iterations The number of benchmark iterations (after warmup).
 * @param[in] result     A pointer for storing the result of the benchmark.
 *
 * @warning Exiting the block of code early by means of jump statements (such as
 * `return` or `goto`) results in memory leaks, as the benchmark context is not
 * properly advanced to the next iteration and deallocated when the benchmark is
 * complete.
 *
 * The behavior is undefined if `warmup` or `iterations` is negative, or if
 * `result` is `nullptr`.
 *
 * @note If the benchmark cannot be started, the block of code is never
 * executed. In this case, `result->error` is set to an appropriate error code.
 * This is also the case if retrieving the elapsed times fails after executing
 * the block of code, or if the benchmark fails for any other reason. In all of
 * these cases, `result->wall` and `result->cpu` are zero-initialized, and
 * `result->iterations` is set to the number of iterations that were
 * successfully completed before the error occurred (which may be zero).
 *
 * Zero may be passed both to `warmup` and `iterations` to indicate that no
 * warmup or benchmark iterations are required. In this case, the benchmark is
 * still considered to be successfully executed, and `result->error` is set to
 * `SCU_ERROR_NONE`. Additionally, `result->wall`, `result->cpu`, and
 * `result->iterations` are zero-initialized.
 */
#define SCU_BENCH(mode, warmup, iterations, result)               \
    for (                                                         \
        ScuBenchContext scuContext = scu_bench_context_new(       \
            (mode),                                               \
            (warmup),                                             \
            (iterations),                                         \
            (result)                                              \
        );                                                        \
        scu_bench_context_is_running(&scuContext);                \
        scu_bench_context_advance(&scuContext)                    \
    )                                                             \
        SCU_TIME(scuContext.timingMode, &scuContext.timingResult)

#endif