#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <memory>

namespace
{
    std::mt19937_64 rngEngine(0);

    volatile std::uint64_t gValueSink;
}

template<std::size_t kSize1, std::size_t kSize2>
void runMultiplicationBenchmark(std::size_t totalIterations)
{
    WMPUInt<kSize1> value1;
    std::unique_ptr<WMPUInt<kSize2>> value2(new WMPUInt<kSize2>);
    std::unique_ptr<WMPUInt<kSize1+kSize2>> result(new WMPUInt<kSize1+kSize2>);

    const std::size_t tempBufferSize = WMPUInt<kSize1>::template fullMultiplyBufferSize<kSize2>();
    std::unique_ptr<std::uint64_t[]> tempBuffer(new std::uint64_t[tempBufferSize]);
    std::uint64_t resultSumMSW = 0;

    for(std::size_t i = 0; i < kSize1; ++i)
        value1.data()[i] = rngEngine();

    for(std::size_t i = 0; i < kSize2; ++i)
        value2->data()[i] = rngEngine();

    Timer timer1;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        value1.fullMultiply(*value2, *result, tempBuffer.get());
        resultSumMSW += result->data()[0];
    }

    gValueSink = resultSumMSW;
    const double seconds1 = timer1.getElapsedSeconds();

    Timer timer2;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        WMPIntImplementations::doFullLongMultiplication
            (value1.data(), kSize1, value2->data(), kSize2, result->data());
        resultSumMSW += result->data()[0];
    }

    gValueSink = resultSumMSW;
    const double seconds2 = timer2.getElapsedSeconds();

    std::printf("%zux%zu: %.2fs (", kSize1, kSize2, seconds1);
    Timer::printTime(seconds1 / totalIterations);
    std::printf("), %.2fs (", seconds2);
    Timer::printTime(seconds2 / totalIterations);
    std::printf(")\n");
}

static constexpr std::size_t kSizes[] =
{ 2, 3, 4, 5, 6, 8, 12, 16, 32, 63, 64, 127, 128, 255, 256, 511, 512, 1024, 2048 };

template<std::size_t kSizesIndex>
static void runBenchmark1();

template<>
void runBenchmark1<sizeof(kSizes)/sizeof(*kSizes)>() {}

template<std::size_t kSizesIndex>
static void runBenchmark1()
{
    runMultiplicationBenchmark<1, kSizes[kSizesIndex]>(500000000/kSizes[kSizesIndex]);
    runBenchmark1<kSizesIndex+1>();
}

template<std::size_t kSizesIndex>
static void runBenchmark2();

template<>
void runBenchmark2<sizeof(kSizes)/sizeof(*kSizes)>() {}

template<std::size_t kSizesIndex>
static void runBenchmark2()
{
    runMultiplicationBenchmark<2, kSizes[kSizesIndex]>(250000000/kSizes[kSizesIndex]);
    runBenchmark2<kSizesIndex+1>();
}

int main()
{
    std::printf("Benchmarks for WMPUInt::fullMultiply() vs. doFullLongMultiplication():\n");
    runBenchmark1<0>();
    runBenchmark2<0>();
}
