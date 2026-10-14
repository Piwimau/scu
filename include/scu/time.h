#ifndef SCU_TIME_H
#define SCU_TIME_H

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
 * the returned timing result has both `wallNs` and `cpuNs` fields set to `0`,
 * and the `error` field set to `SCU_ERROR_NONE`. Otherwise, the timing result
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

#endif