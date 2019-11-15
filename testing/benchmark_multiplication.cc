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
    WMPUInt<kSize> value1, value2, result;
    std::uint64_t tempBuffer[kSize];
    std::uint64_t resultSumMSW = 0, resultSumLSW = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        value1.data()[i] = rngEngine();
        value2.data()[i] = rngEngine();
    }

    Timer timer;

    for(std::size_t i = 0; i < totalIterations; ++i)
    {
        value1.multiply(value2, result, tempBuffer);
        resultSumMSW += result.data()[0];
        resultSumLSW += result.data()[kSize-1];
    }

    gValueSink1 = resultSumMSW; gValueSink2 = resultSumLSW;
    timer.printResult(kSize, totalIterations);
}

int main()
{
    std::printf("Benchmarks for WMPUInt::multiply():\n");

    runMultiplicationBenchmark<2>(1050000000);
    runMultiplicationBenchmark<3>(134000000);
    runMultiplicationBenchmark<4>(76000000);
    runMultiplicationBenchmark<5>(48000000);
    runMultiplicationBenchmark<6>(35000000);
    runMultiplicationBenchmark<7>(25000000);
    runMultiplicationBenchmark<8>(19000000);
    runMultiplicationBenchmark<10>(12000000);
    runMultiplicationBenchmark<16>(4800000);
    runMultiplicationBenchmark<24>(2000000);
    runMultiplicationBenchmark<32>(1100000);
    runMultiplicationBenchmark<50>(420000);
    runMultiplicationBenchmark<150>(45000);
    runMultiplicationBenchmark<256>(16000);
    runMultiplicationBenchmark<1024>(1000);
    runMultiplicationBenchmark<1024*16>(4);
}
