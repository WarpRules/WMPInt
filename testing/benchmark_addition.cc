#include "../WMPInt.hh"
#include "benchmark_timer.hh"
#include <random>

namespace
{
    std::mt19937_64 rngEngine(0);

    volatile std::uint64_t gValueSink1, gValueSink2;
}

template<std::size_t kSize>
void runAdditionBenchmark(std::size_t totalIterations)
{
    WMPUInt<kSize> sum(0), value;

    for(std::size_t i = 0; i < kSize; ++i)
        value.data()[i] = rngEngine();

    Timer timer;

    for(std::size_t i = 0; i < totalIterations; ++i)
        sum += value;

    gValueSink1 = sum.data()[0]; gValueSink2 = sum.data()[kSize-1];
    timer.printResult(kSize, totalIterations);
}

int main()
{
    std::printf("Benchmarks for operator+=(WMPUInt):\n");

    runAdditionBenchmark<2>(360000000);
    runAdditionBenchmark<3>(350000000);
    runAdditionBenchmark<4>(320000000);
    runAdditionBenchmark<5>(230000000);
    runAdditionBenchmark<6>(220000000);
    runAdditionBenchmark<7>(180000000);
    runAdditionBenchmark<8>(160000000);
    runAdditionBenchmark<10>(125000000);
    runAdditionBenchmark<16>(80000000);
    runAdditionBenchmark<24>(50000000);
    runAdditionBenchmark<32>(38000000);
    runAdditionBenchmark<50>(25000000);
    runAdditionBenchmark<150>(7500000);
    runAdditionBenchmark<256>(4500000);
    runAdditionBenchmark<1024>(1200000);
    runAdditionBenchmark<1024*16>(100000);
}
