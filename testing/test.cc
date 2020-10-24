#include "test.hh"
#include <random>
#include <utility>
#include <chrono>
#include <cstring>

std::mt19937_64 gRngEngine(0);

//============================================================================
// Benchmark
//============================================================================
struct Timer
{
    std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime =
        std::chrono::high_resolution_clock::now();

    double getTimeIntervalNow() const
    {
        const std::chrono::time_point endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = endTime - mStartTime;
        return diff.count();
    }
};

template<std::size_t kSize>
[[maybe_unused]]
static void printBenchmarkResult
(const char* title, const WMPUInt<kSize>& resultValue, double seconds)
{
    std::cout << title << ": [" << std::hex << resultValue.data()[0]
              << ", ..., " << resultValue.data()[kSize-1]
              << "]: " << seconds << " seconds" << std::endl;
}

[[maybe_unused]]
static void runAdditionBenchmarks()
{
    const std::size_t kSize = 128;

    WMPUInt<kSize> value1, value2(-1000000000001);
    for(std::size_t i = 0; i < kSize; ++i)
        value1.data()[i] = -10;

    Timer timer;
    for(unsigned i = 0; i < 10000000; ++i)
        value1 += value2;
    printBenchmarkResult("operator+=(WMPUInt)", value1, timer.getTimeIntervalNow());

    Timer timer2;
    for(unsigned i = 0; i < 500000000; ++i)
        value1 += std::uint64_t(-1000000000001);
    printBenchmarkResult("operator+=(uint64_t)", value1, timer2.getTimeIntervalNow());
}

[[maybe_unused]]
static void runMultiplicationBenchmarks()
{
    const std::size_t kSize = 128;

    WMPUInt<kSize> value1(3), value2(5);
    Timer timer;

    for(unsigned i = 0; i < 40000; ++i)
        value2 *= value1;

    printBenchmarkResult("operator*=(WMPUInt)", value2, timer.getTimeIntervalNow());
}

[[maybe_unused]]
static void runCombinedBenchmark()
{
    const std::size_t kSize = 64;

    WMPUInt<kSize> value1(1), value2(3), value3(3);
    Timer timer;

    for(unsigned i = 0; i < 120000; ++i)
    {
        value1 += value2;
        value2 *= value1;
        value2 += value3;
    }

    printBenchmarkResult("combined +*", value2, timer.getTimeIntervalNow());
}

[[maybe_unused]]
static void runBenchmarks()
{
    std::cout << "Running benchmarks..." << std::endl;
    runAdditionBenchmarks();
    runMultiplicationBenchmarks();
    runCombinedBenchmark();
}

bool testSize1Operators();
bool testAssignmentFromStr();
bool testComparison();
bool testAddition();
bool testMultiplication();
bool testFullMultiplication();
bool testModulo();
bool testNegation();
bool testShifting();

//============================================================================
// main()
//============================================================================
static void printHelp()
{
    std::cout << "Usage: test [<options>]\n\n"
        "<options>:\n"
        "  -h\n"
        "  --help : This text\n"
        "  -t1    : Run test 1 (size 1 operators)\n"
        "  -t2    : Run test 2 (assignment from string)\n"
        "  -t3    : Run test 3 (comparison operators)\n"
        "  -t4    : Run test 4 (addition and subtraction)\n"
        "  -t5    : Run test 5 (multiplication)\n"
        "  -t6    : Run test 6 (full multiplication)\n"
        "  -t7    : Run test 7 (modulo)\n"
        "  -t8    : Run test 8 (negation)\n"
        "  -t9    : Run test 9 (bitshifting)\n";
}

int main(int argc, char** argv)
{
    bool runAllTests = true;
    bool runTest[9] = { false };

    for(int argInd = 1; argInd < argc; ++argInd)
    {
        if(std::strcmp(argv[argInd], "-h") == 0 || std::strcmp(argv[argInd], "--help") == 0)
        {
            printHelp();
            return 0;
        }
        else if(argv[argInd][0] == '-' && argv[argInd][1] == 't' &&
                argv[argInd][2] >= '1' && argv[argInd][2] <= '9' && argv[argInd][3] == 0)
        {
            runTest[argv[argInd][2] - '1'] = true;
            runAllTests = false;
        }
        else
        {
            std::printf("Unknown parameter: %s\n", argv[argInd]);
            return 1;
        }
    }

    if(runAllTests) for(bool& b: runTest) b = true;

    if(runTest[0] && !testSize1Operators()) DRETM;
    if(runTest[1] && !testAssignmentFromStr()) DRETM;
    if(runTest[2] && !testComparison()) DRETM;
    if(runTest[3] && !testAddition()) DRETM;
    if(runTest[4] && !testMultiplication()) DRETM;
    if(runTest[5] && !testFullMultiplication()) DRETM;
    if(runTest[6] && !testModulo()) DRETM;
    if(runTest[7] && !testNegation()) DRETM;
    if(runTest[8] && !testShifting()) DRETM;
    std::cout << "All tests OK.\n";
    //runBenchmarks();
}
