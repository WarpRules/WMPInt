#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <cassert>

namespace
{
    struct SizeData { std::size_t size, iterations; };

    constexpr SizeData kSizes[] =
    {
        { 12, 4000000 },
        { 16, 2400000 },
        { 25, 900000 },
        { 28, 600000 },
        { 29, 600000 },
        { 30, 600000 },
        { 31, 600000 },
        { 32, 600000 },
        { 33, 600000 },
        { 34, 600000 },
        { 35, 600000 },
        { 36, 600000 },
        { 50, 220000 },
        { 100, 50000 },
        { 200, 14000 },
        { 300, 6000 },
        { 1024, 500 },
    };

    constexpr std::size_t getMaxSize()
    {
        std::size_t maxSize = 0;
        for(const SizeData& data: kSizes)
            if(data.size > maxSize) maxSize = data.size;
        return maxSize;
    }

    constexpr std::size_t kMaxSize = getMaxSize();
    constexpr std::size_t kTempBufferMaxSize =
        WMPIntImplementations::truncatedKaratsubaMultiplicationBufferSize(kMaxSize);

    std::mt19937_64 rngEngine(0);
    volatile std::uint64_t gValueSink1;
    std::uint64_t gLHS[kMaxSize], gRHS[kMaxSize];
    std::uint64_t gResult[kMaxSize];
    std::uint64_t gTempBuffer[kTempBufferMaxSize];
}

void runMultiplicationBenchmark
(std::size_t size, std::size_t totalIterations)
{
    std::uint64_t resultSumMSW = 0;

    for(std::size_t i = 0; i < size; ++i)
        gLHS[i] = rngEngine();

    for(std::size_t i = 0; i < size; ++i)
        gRHS[i] = rngEngine();

    Timer timer1;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        WMPIntImplementations::doLongMultiplication
            (size, gLHS, gRHS, gResult, gTempBuffer);
        resultSumMSW += gResult[0];
    }

    gValueSink1 = resultSumMSW;
    const double seconds1 = timer1.getElapsedSeconds();

    Timer timer2;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        WMPIntImplementations::doTruncatedKaratsubaMultiplication
            (gLHS, gRHS, size, gResult, gTempBuffer);
        resultSumMSW += gResult[0];
    }

    gValueSink1 = resultSumMSW;
    const double seconds2 = timer2.getElapsedSeconds();

    std::printf("size:%4zu %c long: %.3f s (", size,
                seconds1<seconds2?'-':'+', seconds1);
    Timer::printTime(seconds1 / totalIterations);
    std::printf("), karatsuba: %.3f s (", seconds2);
    Timer::printTime(seconds2 / totalIterations);
    std::printf(") | %zu/%zu\n",
                WMPIntImplementations::longMultiplicationBufferSize(size),
                WMPIntImplementations::truncatedKaratsubaMultiplicationBufferSize(size));
}

int main()
{
    std::printf("Truncated long multiplication vs. Karatsuba algorithm benchmarks:\n");
    for(const SizeData& data: kSizes)
        runMultiplicationBenchmark(data.size, data.iterations);
}
