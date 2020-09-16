#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <cassert>

namespace
{
    struct SizeData { std::size_t size, iterations; };

    constexpr SizeData kSizes[] =
    {
        { 12, 10000000 },
        { 16, 6000000 },
        { 32, 1500000 },
        { 50, 800000 },
        { 60, 400000 },
        { 70, 300000 },
        { 80, 220000 },
        { 100, 150000 },
        { 200, 50000 },
        { 300, 20000 },
        { 400, 12000 },
        { 600, 6000 },
        { 800, 3000 },
        { 1024, 2000 },
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
