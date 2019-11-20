#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>

namespace
{
    std::mt19937_64 rngEngine(0);

    volatile std::uint64_t gValueSink1, gValueSink2;
}

template<std::size_t kSize>
void runMultiplicationBenchmark(std::size_t totalIterations)
{
    WMPUInt<kSize> value1, value2;
    WMPUInt<kSize*2> result;
    std::uint64_t tempBuffer[value1.template fullMultiplyBufferSize<value2.size()>()];
    std::uint64_t resultSumMSW = 0, resultSumLSW = 0;

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
        resultSumLSW += result.data()[kSize*2-1];
    }

    gValueSink1 = resultSumMSW; gValueSink2 = resultSumLSW;
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
    runMultiplicationBenchmark<24>(1000000);
    runMultiplicationBenchmark<32>(550000);
    runMultiplicationBenchmark<50>(220000);
    runMultiplicationBenchmark<150>(24500);
    runMultiplicationBenchmark<256>(8000);
    runMultiplicationBenchmark<1024>(500);
    runMultiplicationBenchmark<1024*16>(2);
}
