#define SCU_SHORT_ALIASES

#include <tgmath.h>
#include "scu/alloc.h"
#include "scu/array.h"
#include "scu/assert.h"
#include "scu/bench.h"
#include "scu/compare.h"
#include "scu/memory.h"

ScuBenchContext scu_bench_context_new(
    ScuTimingMode timingMode,
    isize warmup,
    isize iterations,
    ScuBenchResult* benchResult
) {
    SCU_ASSERT(warmup >= 0);
    SCU_ASSERT(iterations >= 0);
    SCU_ASSERT(benchResult);
    ScuBenchContext context = {
        .timingMode = timingMode,
        .warmup = warmup,
        .iterations = iterations,
        .benchResult = benchResult
    };
    if (context.iterations > 0) {
        context.wallSamples = scu_malloc(context.iterations * SCU_SIZEOF(i64));
        context.cpuSamples = scu_malloc(context.iterations * SCU_SIZEOF(i64));
        if (!context.wallSamples || !context.cpuSamples) {
            scu_free(context.wallSamples);
            context.wallSamples = nullptr;
            scu_free(context.cpuSamples);
            context.cpuSamples = nullptr;
            context.error = SCU_ERROR_OUT_OF_MEMORY;
        }
    }
    return context;
}

/**
 * @brief Returns benchmark statistics for a sequence of samples.
 *
 * @param[in, out] samples A sequence of samples (in nanoseconds).
 * @param[in]      count   The number of samples.
 * @return The benchmark statistics for the sequence of samples.
 *
 * @note The sequence of samples is sorted in-place.
 *
 * @warning The behavior is undefined if `samples` is `nullptr`, or if `count`
 * is less than or equal to zero.
 */
static ScuBenchStats scu_bench_stats_from_samples(i64* samples, isize count) {
    SCU_ASSERT(samples);
    SCU_ASSERT(count > 0);
    scu_array_sort(samples, count, SCU_SIZEOF(i64), scu_compare_i64);
    f64 totalNs = 0.0;
    for (isize i = 0; i < count; i++) {
        totalNs += (f64) samples[i];
    }
    f64 meanNs = totalNs / (f64) count;
    f64 sumOfSquaredDiffsNs = 0.0;
    for (isize i = 0; i < count; i++) {
        f64 diff = (f64) samples[i] - meanNs;
        sumOfSquaredDiffsNs += diff * diff;
    }
    f64 medianNs = (count % 2 == 0)
        ? ((f64) samples[count / 2 - 1] + (f64) samples[count / 2]) / 2.0
        : (f64) samples[count / 2];
    f64 stdDevNs = (count > 1)
        ? sqrt(sumOfSquaredDiffsNs / (f64) (count - 1))
        : 0.0;
    return (ScuBenchStats) {
        .minNs = samples[0],
        .maxNs = samples[count - 1],
        .meanNs = meanNs,
        .medianNs = medianNs,
        .stdDevNs = stdDevNs
    };
}

bool scu_bench_context_is_running(ScuBenchContext* context) {
    SCU_ASSERT(context);
    if (context->error != SCU_ERROR_NONE) {
        *context->benchResult = (ScuBenchResult) {
            .iterations = context->iteration,
            .error = context->error
        };
        return false;
    }
    if (context->iteration < context->warmup + context->iterations) {
        return true;
    }
    context->benchResult->iterations = context->iterations;
    context->benchResult->error = SCU_ERROR_NONE;
    if (context->iterations > 0) {
        context->benchResult->wall = scu_bench_stats_from_samples(
            context->wallSamples,
            context->iterations
        );
        context->benchResult->cpu = scu_bench_stats_from_samples(
            context->cpuSamples,
            context->iterations
        );
        scu_free(context->wallSamples);
        context->wallSamples = nullptr;
        scu_free(context->cpuSamples);
        context->cpuSamples = nullptr;
    }
    else {
        context->benchResult->wall = (ScuBenchStats) { };
        context->benchResult->cpu = (ScuBenchStats) { };
    }
    return false;
}

void scu_bench_context_advance(ScuBenchContext* context) {
    SCU_ASSERT(context);
    if (context->timingResult.error != SCU_ERROR_NONE) {
        scu_free(context->wallSamples);
        context->wallSamples = nullptr;
        scu_free(context->cpuSamples);
        context->cpuSamples = nullptr;
        context->error = SCU_ERROR_TIMING_FAILED;
        return;
    }
    if (context->iteration >= context->warmup) {
        isize sampleIdx = context->iteration - context->warmup;
        context->wallSamples[sampleIdx] = context->timingResult.wallNs;
        context->cpuSamples[sampleIdx] = context->timingResult.cpuNs;
    }
    context->iteration++;
}