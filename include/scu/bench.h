#ifndef SCU_BENCH_H
#define SCU_BENCH_H

#include "scu/error.h"
#include "scu/types.h"

/** @brief A timing mode for measuring CPU time. */
typedef enum ScuTimingMode {

    /** @brief Measure CPU time for the entire process. */
    SCU_TIMING_MODE_PROCESS,

    /** @brief Measure CPU time for the current thread only. */
    SCU_TIMING_MODE_THREAD

} ScuTimingMode;

/**
 * @brief A stopwatch for measuring wall and CPU time.
 *
 * @warning The internal representation of the stopwatch is an implementation
 * detail and should not be relied upon. Most importantly, the behavior is
 * undefined if its fields are accessed directly.
 */
typedef struct ScuStopwatch {

    /** @brief The timing mode for measuring CPU time. */
    ScuTimingMode timingMode;

    /** @brief The last starting wall time (in nanoseconds). */
    Scui64 startWallNs;

    /** @brief The last starting CPU time (in nanoseconds). */
    Scui64 startCpuNs;

    /** @brief The accumulated wall time (in nanoseconds). */
    Scui64 accWallNs;

    /** @brief The accumulated CPU time (in nanoseconds). */
    Scui64 accCpuNs;

    /** @brief Whether the stopwatch is currently running. */
    bool isRunning;

} ScuStopwatch;

/** @brief A timing result. */
typedef struct ScuTimingResult {

    /** @brief The measured wall time (in nanoseconds). */
    Scui64 wallNs;

    /** @brief The measured CPU time (in nanoseconds). */
    Scui64 cpuNs;

    /** @brief Whether the timing was successful. */
    ScuError error;

} ScuTimingResult;

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
    Scuisize warmups;

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
 * @brief Initializes a stopwatch.
 *
 * @param[in, out] stopwatch  The stopwatch to initialize.
 * @param[in]      timingMode The timing mode for measuring CPU time.
 *
 * @note This function may be called more than once to reinitialize the same
 * stopwatch with a different timing mode. Any previously accumulated wall and
 * CPU times are discarded, and the stopwatch is stopped if it was previously
 * running.
 */
void scu_stopwatch_init(ScuStopwatch* stopwatch, ScuTimingMode timingMode);

/**
 * @brief Starts a stopwatch.
 *
 * @param[in, out] stopwatch The stopwatch to start.
 * @return `true` if the stopwatch was started successfully, `false` otherwise.
 *
 * @note If the stopwatch is already running, this function does nothing.
 * Otherwise, it starts measuring time from zero (if the stopwatch was never
 * started before or was previously reset) or resumes measuring time from the
 * accumulated wall and CPU time (if the stopwatch was previously stopped).
 *
 * This function does not reset the accumulated wall and CPU time.
 */
bool scu_stopwatch_start(ScuStopwatch* stopwatch);

/**
 * @brief Restarts a stopwatch.
 *
 * @param[in, out] stopwatch The stopwatch to restart.
 * @return `true` if the stopwatch was restarted successfully, `false`
 * otherwise.
 *
 * @note If the stopwatch is already running or was previously stopped, this
 * function resets the accumulated wall and CPU time to zero and starts
 * measuring time. Otherwise, if the stopwatch was never started before or was
 * previously reset, this function simply behaves like `scu_stopwatch_start()`.
 *
 * This function resets the accumulated wall and CPU time. It is equivalent to
 * calling `scu_stopwatch_reset()`, followed by `scu_stopwatch_start()`.
 */
bool scu_stopwatch_restart(ScuStopwatch* stopwatch);

/**
 * @brief Stops a stopwatch.
 *
 * @param[in, out] stopwatch The stopwatch to stop.
 * @return `true` if the stopwatch was stopped successfully, `false` otherwise.
 *
 * @note If the stopwatch was never started before or was previously stopped or
 * reset, this function does nothing. Otherwise, it stops measuring time and
 * retains the accumulated wall and CPU time.
 *
 * This function does not reset the accumulated wall and CPU time.
 */
bool scu_stopwatch_stop(ScuStopwatch* stopwatch);

/**
 * @brief Resets a stopwatch.
 *
 * @param[in, out] stopwatch The stopwatch to reset.
 *
 * @note If the stopwatch was never started before or was previously reset, this
 * function does nothing. Otherwise, it stops measuring time (if the stopwatch
 * is running) and resets the accumulated wall and CPU time to zero.
 *
 * This function resets the accumulated wall and CPU time. It does not start
 * the stopwatch again.
 */
void scu_stopwatch_reset(ScuStopwatch* stopwatch);

/**
 * @brief Determines whether a stopwatch is currently running.
 *
 * @param[in] stopwatch The stopwatch to examine.
 * @return `true` if the stopwatch is currently running, `false` otherwise.
 */
bool scu_stopwatch_is_running(const ScuStopwatch* stopwatch);

/**
 * @brief Retrieves the elapsed wall and CPU time from a stopwatch.
 *
 * @param[in] stopwatch The stopwatch to examine.
 * @return A timing result containing the elapsed wall and CPU time in
 * nanoseconds on success, or an appropriate error code on failure.
 *
 * @note If the stopwatch was never started before or was previously reset, the
 * returned timing result has both `wallNs` and `cpuNs` fields set to `0`, and
 * the `error` field set to `SCU_ERROR_NONE`. Otherwise, the timing result
 * contains the accumulated wall and CPU time measured by the stopwatch in
 * nanoseconds up to the point when it was last stopped (if the stopwatch is
 * stopped) or up to the current point in time (if the stopwatch is running).
 * The `error` field is also set to `SCU_ERROR_NONE` in this case.
 *
 * In the (unlikely) case that retrieving the elapsed times fails, the `error`
 * field of the returned timing result is set to `SCU_ERROR_TIMING_FAILED`, and
 * both `wallNs` and `cpuNs` contain an unspecified value.
 */
ScuTimingResult scu_stopwatch_elapsed(const ScuStopwatch* stopwatch);

/**
 * @brief Constructs a benchmark context.
 *
 * @param[in] timingMode  The timing mode for measuring CPU time.
 * @param[in] warmups     The number of warmup iterations.
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
 * The behavior is undefined if `warmups` or `iterations` is negative, or if
 * `benchResult` is `nullptr`.
 *
 * @note This function is an implementation detail and not intended to be used
 * directly. Use the `SCU_BENCH()` macro instead.
 */
ScuBenchContext scu_bench_context_new(
    ScuTimingMode timingMode,
    Scuisize warmups,
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
 * @brief Measures the wall and CPU time required to execute a block of code.
 *
 * This macro simplifies the process of measuring the wall and CPU time required
 * to execute a block of code. It creates a new stopwatch, starts it, executes
 * the block of code, stops the stopwatch, and retrieves the elapsed times,
 * which are assigned to `result->wallNs` and `result->cpuNs` respectively.
 *
 * The following example demonstrates the basic usage of this macro:
 *
 * ```c
 * ScuTimingResult result;
 * SCU_TIME(SCU_TIMING_MODE_PROCESS, &result) {
 *     // Some code to be timed...
 * }
 * if (result.error != SCU_ERROR_NONE) {
 *     // Handle the error...
 * }
 * else {
 *     // Use result.wallNs and result.cpuNs...
 * }
 * ```
 *
 * @param[in]  mode   The timing mode for measuring CPU time.
 * @param[out] result A timing result containing the elapsed wall and CPU
 * time in nanoseconds on success, or an appropriate error code on failure.
 *
 * @note If the stopwatch cannot be started, the block of code is not executed.
 * In this case, both `result->wallNs` and `result->cpuNs` are set to an
 * unspecified value, and `result->error` is set to `SCU_ERROR_TIMING_FAILED`.
 * This is also the case if retrieving the elapsed times fails after executing
 * the block of code and stopping the stopwatch.
 */
#define SCU_TIME(mode, result)                                     \
    for (bool scuOnce = true; scuOnce; scuOnce = false)            \
        for (                                                      \
            ScuStopwatch scuStopwatch = { .timingMode = (mode) };  \
            scuOnce;                                               \
            scuOnce = false                                        \
        )                                                          \
            for (                                                  \
                ;                                                  \
                scuOnce && (scu_stopwatch_start(&scuStopwatch)     \
                    ? true                                         \
                    : (                                            \
                        (result)->wallNs = -1,                     \
                        (result)->cpuNs = -1,                      \
                        (result)->error = SCU_ERROR_TIMING_FAILED, \
                        false                                      \
                    ));                                            \
                scuOnce = false,                                   \
                scu_stopwatch_stop(&scuStopwatch),                 \
                *(result) = scu_stopwatch_elapsed(&scuStopwatch)   \
            )

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
 * @param[in] warmups    The number of warmup iterations.
 * @param[in] iterations The number of benchmark iterations (after warmup).
 * @param[in] result     A pointer for storing the result of the benchmark.
 *
 * @warning Exiting the block of code early by means of jump statements (such as
 * `return` or `goto`) results in memory leaks, as the benchmark context is not
 * properly advanced to the next iteration and deallocated when the benchmark is
 * complete.
 *
 * The behavior is undefined if `warmups` or `iterations` is negative, or if
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
 * Zero may be passed both to `warmups` and `iterations` to indicate that no
 * warmup or benchmark iterations are required. In this case, the benchmark is
 * still considered to be successfully executed, and `result->error` is set to
 * `SCU_ERROR_NONE`. Additionally, `result->wall`, `result->cpu`, and
 * `result->iterations` are zero-initialized.
 */
#define SCU_BENCH(mode, warmups, iterations, result)              \
    for (                                                         \
        ScuBenchContext scuContext = scu_bench_context_new(       \
            (mode),                                               \
            (warmups),                                            \
            (iterations),                                         \
            (result)                                              \
        );                                                        \
        scu_bench_context_is_running(&scuContext);                \
        scu_bench_context_advance(&scuContext)                    \
    )                                                             \
        SCU_TIME(scuContext.timingMode, &scuContext.timingResult)

#endif