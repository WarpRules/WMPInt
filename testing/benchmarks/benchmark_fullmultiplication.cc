#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>
#include <memory>

namespace
{
    std::mt19937_64 rngEngine(0);

    volatile std::uint64_t gValueSink;
}

template<std::size_t kSize>
void runMultiplicationBenchmark(std::size_t totalIterations)
{
    WMPUInt<kSize> value1, value2;
    WMPUInt<kSize*2> result;
    std::uint64_t tempBuffer[value1.template fullMultiplyBufferSize<kSize>()];
    std::uint64_t resultSumMSW = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        value1.data()[i] = rngEngine();
        value2.data()[i] = rngEngine();
    }

    Timer timer;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        value1.fullMultiply(value2, result, tempBuffer);
        resultSumMSW += result.data()[0];
    }

    gValueSink = resultSumMSW;
    timer.printResult(kSize, totalIterations);
}

template<std::size_t kSize>
void runMultiplicationBenchmarkWithAllocatedObjects(std::size_t totalIterations)
{
    std::unique_ptr<WMPUInt<kSize>> value1(new WMPUInt<kSize>);
    std::unique_ptr<WMPUInt<kSize>> value2(new WMPUInt<kSize>);
    std::unique_ptr<WMPUInt<kSize*2>> result(new WMPUInt<kSize*2>);

    const std::size_t tempBufferSize = WMPUInt<kSize>::template fullMultiplyBufferSize<kSize>();
    std::unique_ptr<std::uint64_t[]> tempBuffer(new std::uint64_t[tempBufferSize]);
    std::uint64_t resultSumMSW = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        value1->data()[i] = rngEngine();
        value2->data()[i] = rngEngine();
    }

    Timer timer;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        value1->fullMultiply(*value2, *result, tempBuffer.get());
        resultSumMSW += result->data()[0];
    }

    gValueSink = resultSumMSW;
    timer.printResult(kSize, totalIterations);
}

int main()
{
    std::printf("Benchmarks for WMPUInt::fullMultiply():\n");

    runMultiplicationBenchmark<2>(350000000);
    runMultiplicationBenchmark<3>(50000000);
    runMultiplicationBenchmark<4>(32000000);
    runMultiplicationBenchmark<5>(21000000);
    runMultiplicationBenchmark<6>(15000000);
    runMultiplicationBenchmark<7>(11500000);
    runMultiplicationBenchmark<8>(9500000);
    runMultiplicationBenchmark<10>(6000000);
    runMultiplicationBenchmark<16>(2400000);
    runMultiplicationBenchmark<24>(1200000);
    runMultiplicationBenchmarkWithAllocatedObjects<32>(700000);
    runMultiplicationBenchmarkWithAllocatedObjects<50>(300000);
    runMultiplicationBenchmarkWithAllocatedObjects<150>(53000);
    runMultiplicationBenchmarkWithAllocatedObjects<256>(22000);
    runMultiplicationBenchmarkWithAllocatedObjects<1024>(2500);
    runMultiplicationBenchmarkWithAllocatedObjects<1024*16>(30);
    runMultiplicationBenchmarkWithAllocatedObjects<1024*64>(4);
    runMultiplicationBenchmarkWithAllocatedObjects<100000>(2);
}
