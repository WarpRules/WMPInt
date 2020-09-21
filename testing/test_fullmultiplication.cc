#include "test.hh"
#include <random>

#ifndef WMPINT_DEBUG_MODE
#error "Define WMPINT_DEBUG_MODE to compile the project (with -DWMPINT_DEBUG_MODE)"
#endif

static void multiply(const std::uint32_t* lhs, std::size_t size1,
                     const std::uint32_t* rhs, std::size_t size2,
                     std::uint32_t* result, std::uint32_t* tempBuffer)
{
    const std::size_t resultSize = size1 + size2;
    for(std::size_t i = 0; i < resultSize; ++i) result[i] = 0;

    for(std::size_t lhsInd = size1, resultStartInd = resultSize - 1; lhsInd-- > 0; --resultStartInd)
    {
        for(std::size_t i = size2 + 1; i-- > 0;) tempBuffer[i] = 0;

        std::uint64_t lhsValue = static_cast<std::uint64_t>(lhs[lhsInd]);

        for(std::size_t rhsInd = size2; rhsInd > 0; --rhsInd)
        {
            std::uint64_t product =
                static_cast<std::uint64_t>(rhs[rhsInd-1]) * lhsValue;
            product += tempBuffer[rhsInd];
            tempBuffer[rhsInd] = static_cast<std::uint32_t>(product);
            tempBuffer[rhsInd-1] = static_cast<std::uint32_t>(product>>32);
        }

        std::uint64_t tempVal = 0;
        for(std::size_t rhsInd = size2 + 1, resultInd = resultStartInd; rhsInd-- > 0; --resultInd)
        {
            tempVal +=
                static_cast<std::uint64_t>(tempBuffer[rhsInd]) +
                static_cast<std::uint64_t>(result[resultInd]);
            result[resultInd] = static_cast<std::uint32_t>(tempVal);
            tempVal >>= 32;
        }
    }
}

static bool testTheTest(std::uint32_t lhs, std::uint32_t rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(&lhs, 1, &rhs, 1, result, tempBuffer);
    const std::uint64_t expectedResult =
        (static_cast<std::uint64_t>(lhs) * static_cast<std::uint64_t>(rhs));
    const std::uint64_t calculatedResult =
        (static_cast<std::uint64_t>(result[0]) << 32) |
        static_cast<std::uint64_t>(result[1]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(8), lhs,
                      ", rhs=", sethexw0(8), rhs,
                      ",\n  expected=", sethexw0(16), expectedResult,
                      ",\ncalculated=", sethexw0(16), calculatedResult, ")\n");
    return true;
}

static bool testTheTest(const std::uint32_t* lhs, std::uint32_t rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(lhs, 2, &rhs, 1, result, tempBuffer);
    const std::uint64_t lhsValue =
        (static_cast<std::uint64_t>(lhs[0]) << 32) | static_cast<std::uint64_t>(lhs[1]);
    const __uint128_t expectedResult =
        (static_cast<__uint128_t>(lhsValue) * static_cast<__uint128_t>(rhs));
    const __uint128_t calculatedResult =
        (static_cast<__uint128_t>(result[0]) << 64) |
        (static_cast<__uint128_t>(result[1]) << 32) |
        static_cast<__uint128_t>(result[2]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(16), lhsValue,
                      ", rhs=", sethexw0(8), rhs,
                      ",\n  expected=", sethexw0(32), expectedResult,
                      ",\ncalculated=", sethexw0(32), calculatedResult, ")\n");
    return true;
}

static bool testTheTest(std::uint32_t lhs, const std::uint32_t* rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(&lhs, 1, rhs, 2, result, tempBuffer);
    const std::uint64_t rhsValue =
        (static_cast<std::uint64_t>(rhs[0]) << 32) | static_cast<std::uint64_t>(rhs[1]);
    const __uint128_t expectedResult =
        (static_cast<__uint128_t>(lhs) * static_cast<__uint128_t>(rhsValue));
    const __uint128_t calculatedResult =
        (static_cast<__uint128_t>(result[0]) << 64) |
        (static_cast<__uint128_t>(result[1]) << 32) |
        static_cast<__uint128_t>(result[2]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(8), lhs,
                      ", rhs=", sethexw0(16), rhsValue,
                      ",\n  expected=", sethexw0(32), expectedResult,
                      ",\ncalculated=", sethexw0(32), calculatedResult, ")\n");
    return true;
}

static bool testTheTest(const std::uint32_t* lhs, const std::uint32_t* rhs,
                        std::uint32_t* result, std::uint32_t* tempBuffer)
{
    multiply(lhs, 2, rhs, 2, result, tempBuffer);
    const std::uint64_t lhsValue =
        (static_cast<std::uint64_t>(lhs[0]) << 32) | static_cast<std::uint64_t>(lhs[1]);
    const std::uint64_t rhsValue =
        (static_cast<std::uint64_t>(rhs[0]) << 32) | static_cast<std::uint64_t>(rhs[1]);
    const __uint128_t expectedResult =
        (static_cast<__uint128_t>(lhsValue) * static_cast<__uint128_t>(rhsValue));
    const __uint128_t calculatedResult =
        (static_cast<__uint128_t>(result[0]) << 96) |
        (static_cast<__uint128_t>(result[1]) << 64) |
        (static_cast<__uint128_t>(result[2]) << 32) |
        static_cast<__uint128_t>(result[3]);

    if(expectedResult != calculatedResult)
        return DPRINT("Internal error: Test failed check.\n(lhs=", sethexw0(16), lhsValue,
                      ", rhs=", sethexw0(16), rhsValue,
                      ",\n  expected=", sethexw0(32), expectedResult,
                      ",\ncalculated=", sethexw0(32), calculatedResult, ")\n");
    return true;
}

template<std::size_t kArraySize>
static bool checkResult(const std::uint32_t(&result)[kArraySize],
                        const std::uint32_t(&expectedResult)[kArraySize])
{
    for(std::size_t i = 0; i < kArraySize; ++i)
        if(result[i] != expectedResult[i])
            return DPRINT("Internal error: Test failed check.\n");
    return true;
}

template<std::size_t kSize1, std::size_t kSize2>
static bool testTheTestWithArrays(const std::uint32_t (&lhs)[kSize1],
                                  const std::uint32_t (&rhs)[kSize2],
                                  const std::uint32_t (&expectedResult)[kSize1+kSize2])
{
    std::uint32_t result[kSize1+kSize2], tempBuffer[(kSize1>kSize2?kSize1:kSize2)+1];

    multiply(lhs, kSize1, rhs, kSize2, result, tempBuffer);
    if(!checkResult(result, expectedResult)) DRET;
    multiply(rhs, kSize2, lhs, kSize1, result, tempBuffer);
    if(!checkResult(result, expectedResult)) DRET;
    return true;
}

static bool testTheTest()
{
    std::mt19937_64 rngEngine(123);
    std::uint32_t lhsArray[2], rhsArray[2], resultArray[4], tempBuffer[3];

    for(unsigned iteration = 0; iteration < 100000; ++iteration)
    {
        const std::uint64_t lhs = rngEngine(), rhs = rngEngine();
        lhsArray[0] = static_cast<std::uint32_t>(lhs >> 32);
        lhsArray[1] = static_cast<std::uint32_t>(lhs);
        rhsArray[0] = static_cast<std::uint32_t>(rhs >> 32);
        rhsArray[1] = static_cast<std::uint32_t>(rhs);

        if(!testTheTest(lhsArray[0], rhsArray[0], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[1], rhsArray[0], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[0], rhsArray[1], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[1], rhsArray[1], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[0], rhsArray, resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray[1], rhsArray, resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray, rhsArray[0], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray, rhsArray[1], resultArray, tempBuffer)) DRET;
        if(!testTheTest(lhsArray, rhsArray, resultArray, tempBuffer)) DRET;
    }

    const std::uint32_t lhs1[] =
    { 0xFEDCBA98u, 0x76543210u, 0xFEDCBA98u, 0x76543210u, 0xFEDCBA98u };
    const std::uint32_t rhs1[] =
    { 0xFFEEDDCCu, 0xBBAA9988u, 0x77665544u, 0x332211FFu };
    const std::uint32_t expectedResult1[] =
    { 0xFECBABE3u, 0xB76B4385u, 0x83503068u, 0x3BEFC907u, 0xF6C14789u,
      0x10F4704Du, 0x2C078B73u, 0x7B5EDAB7u, 0x9671F568 };

    const std::uint32_t lhs2[] =
    { 0x50506406u, 0x8282EF7Cu, 0xB6496034u, 0x180233A1u };
    const std::uint32_t rhs2[] =
    { 0xA9ECE60Du, 0x1FFAA7C0u };
    const std::uint32_t expectedResult2[] =
    { 0x354F6450u, 0xDA240DA3, 0x84D34425u, 0x88532514u, 0xEA7D1E4Fu, 0x5B8EBFC0u };

    if(!testTheTestWithArrays(lhs1, rhs1, expectedResult1)) DRET;
    if(!testTheTestWithArrays(lhs2, rhs2, expectedResult2)) DRET;

    return true;
}

template<std::size_t kSize>
static void initWMPUIntWithUint32Array(WMPUInt<kSize>& dest, const std::uint32_t* values)
{
    for(std::size_t i = 0; i < kSize; ++i)
        dest.data()[i] = ((static_cast<std::uint64_t>(values[i*2]) << 32) |
                          static_cast<std::uint64_t>(values[i*2+1]));
}

extern std::size_t gWMPInt_karatsuba_max_temp_buffer_size;
static unsigned gTestCounter = 0;
static const unsigned kTestsAmount = 75;

template<std::size_t kSize1, std::size_t kSize2, std::size_t kIterations>
static bool testFullMultiplication(std::mt19937& rng)
{
    std::cout << ++gTestCounter << "/" << kTestsAmount << "\r"; std::cout.flush();

    WMPUInt<kSize1> lhs;
    WMPUInt<kSize2> rhs;
    WMPUInt<kSize1 + kSize2> result, expectedResult;

    const std::size_t tempBufferSize1 = WMPUInt<kSize1>::template fullMultiplyBufferSize<kSize2>();
    const std::size_t tempBufferSize2 = WMPUInt<kSize2>::template fullMultiplyBufferSize<kSize1>();
    const std::size_t karatsubaTempBufferSize =
        WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kSize2, kSize1);

    const std::size_t tempBufferSize =
        std::max(std::max(tempBufferSize1, tempBufferSize2), karatsubaTempBufferSize);

    std::uint64_t tempBuffer[tempBufferSize];

    std::uint32_t lhsArray[kSize1*2], rhsArray[kSize2*2];
    std::uint32_t resultArray[kSize1*2+kSize2*2], tempBuffer2[kSize2*2+1];

    if constexpr(kSize2 > 1)
    {
        for(std::size_t i = 0; i < kSize1*2; ++i) lhsArray[i] = ~std::uint32_t(0);
        for(std::size_t i = 0; i < kSize2*2; ++i) rhsArray[i] = 0;
        rhsArray[1] = 1;
        rhsArray[kSize2*2 - 1] = 1;
        multiply(lhsArray, kSize1*2, rhsArray, kSize2*2, resultArray, tempBuffer2);

        initWMPUIntWithUint32Array(lhs, lhsArray);
        initWMPUIntWithUint32Array(rhs, rhsArray);
        initWMPUIntWithUint32Array(expectedResult, resultArray);

        lhs.fullMultiply(rhs, result, tempBuffer);

        if(result != expectedResult)
            return DPRINT(TType::fmult, "Error: (", kSize1, "x", kSize2, ") fullMultiply of\n", lhs, " and\n",
                          rhs, "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");
    }

    for(std::size_t iteration = 0; iteration < kIterations; ++iteration)
    {
        for(std::size_t i = 0; i < kSize1*2; ++i) lhsArray[i] = rng();
        for(std::size_t i = 0; i < kSize2*2; ++i) rhsArray[i] = rng();

        multiply(lhsArray, kSize1*2, rhsArray, kSize2*2, resultArray, tempBuffer2);

        initWMPUIntWithUint32Array(lhs, lhsArray);
        initWMPUIntWithUint32Array(rhs, rhsArray);
        initWMPUIntWithUint32Array(expectedResult, resultArray);

        lhs.fullMultiply(rhs, result, tempBuffer);

        if(result != expectedResult)
            return DPRINT(TType::fmult, "Error: fullMultiply of\n", lhs, " and\n", rhs,
                          "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

        result.assign(0);
        rhs.fullMultiply(lhs, result, tempBuffer);

        if(result != expectedResult)
            return DPRINT(TType::fmult, "Error: fullMultiply of\n", rhs, " and\n", lhs,
                          "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

        result.assign(0);
        WMPIntImplementations::doFullLongMultiplication
            (lhs.data(), kSize1, rhs.data(), kSize2, result.data());

        if(result != expectedResult)
            return DPRINT(TType::flmult, "Error: doFullLongMultiplication of\n", lhs, " and\n", rhs,
                          "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

        if constexpr(kSize1 == kSize2)
        {
            WMPUInt<kSize1> expectedShortResult, shortResult = lhs * rhs;
            initWMPUIntWithUint32Array(expectedShortResult, resultArray + kSize1*2);
            if(shortResult != expectedShortResult)
                return DPRINT(TType::mult, "Error: WMPUInt<", kSize1, ">::operator*(WMPUInt<", kSize1,
                              ">) of\n", lhs, " and\n", rhs, "\nresulted in\n", shortResult,
                              "\ninstead of\n", expectedShortResult, "\n");

            if constexpr(kSize1 > 1)
            {
                const std::size_t truncatedKaratsubaTempBufferSize =
                    WMPIntImplementations::truncatedKaratsubaMultiplicationBufferSize(kSize1);
                std::uint64_t truncatedKaratsubaTempBuffer[truncatedKaratsubaTempBufferSize];
                shortResult.assign(0);
                WMPIntImplementations::doTruncatedKaratsubaMultiplication
                    (lhs.data(), rhs.data(), kSize1, shortResult.data(),
                     truncatedKaratsubaTempBuffer);

                if(gWMPInt_karatsuba_max_temp_buffer_size != truncatedKaratsubaTempBufferSize)
                    return DPRINT
                        (TType::tkmult, "Error: truncatedKaratsubaMultiplicationBufferSize(",
                         kSize1, ")\nreturned the value ", truncatedKaratsubaTempBufferSize,
                         "\nbut the calculation used a buffer size of ",
                         gWMPInt_karatsuba_max_temp_buffer_size, ".\n");

                if(shortResult != expectedShortResult)
                    return DPRINT("Error: doTruncatedKaratsubaMultiplication of\n", lhs,
                                  " and\n", rhs, "\nresulted in\n", shortResult,
                                  "\ninstead of\n", expectedShortResult, "\n");
            }
        }
        if constexpr(kSize2 == 1)
        {
            WMPUInt<kSize1> expectedShortResult, shortResult = lhs;
            shortResult *= rhs.data()[0];
            initWMPUIntWithUint32Array(expectedShortResult, resultArray + 2);
            if(shortResult != expectedShortResult)
                return DPRINT(TType::amult, "Error: WMPUInt<", kSize1, ">::operator*=(std::uint64_t) of\n",
                              lhs, " and\n", rhs.data()[0], "\nresulted in\n", shortResult,
                              "\ninstead of\n", expectedShortResult, "\n");

            if(rhs.data()[0] > 0)
            {
                result /= rhs.data()[0];
                shortResult.assign(result);
                if(result.data()[0] > 0 || shortResult != lhs)
                    return DPRINT(TType::adiv, "Error: WMPUInt<", result.size(),
                                  ">::operator/=(std::uint64_t) of\n", expectedResult,
                                  " and\n", rhs.data()[0], "\nresulted in\n", result,
                                  "\ninstead of\n", lhs, "\n");

                result.assign(0);
                result = expectedResult / rhs.data()[0];
                shortResult.assign(result);
                if(result.data()[0] > 0 || shortResult != lhs)
                    return DPRINT(TType::div, "Error: WMPUInt<", result.size(),
                                  ">::operator/(std::uint64_t) of\n", expectedResult,
                                  " and\n", rhs.data()[0], "\nresulted in\n", result,
                                  "\ninstead of\n", lhs, "\n");
            }
        }

        if constexpr(kSize1 >= kSize2)
        {
            result.assign(0);
            WMPIntImplementations::doFullKaratsubaMultiplication
                (rhs.data(), kSize2, lhs.data(), kSize1, result.data(), tempBuffer);

            if(gWMPInt_karatsuba_max_temp_buffer_size != karatsubaTempBufferSize)
                return DPRINT
                    (TType::fkmult, "Error: WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(",
                     kSize2, ", ", kSize1, ")\nreturned the value ", karatsubaTempBufferSize,
                     "\nbut the calculation used a buffer size of ",
                     gWMPInt_karatsuba_max_temp_buffer_size, ".\n");

            if(result != expectedResult)
                return DPRINT(TType::fkmult, "Error: doFullKaratsubaMultiplication of\n", rhs, " and\n", lhs,
                              "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");
        }
    }

    if constexpr(kSize1 > 1 && kSize2 > 1)
    {
        static_assert(WMPUInt<kSize1>::template fullMultiply_karatsubaBufferSize<kSize2>()
                      ==
                      WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kSize2, kSize1));
    }

    result.assign(0);
    lhs.fullMultiply_longMultiplication(rhs, result);
    if(result != expectedResult)
        return DPRINT(TType::flmult, "Error: fullMultiply_longMultiplication of\n", rhs, " and\n", lhs,
                      "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

    result.assign(0);
    lhs.fullMultiply_karatsuba(rhs, result, tempBuffer);
    if(result != expectedResult)
        return DPRINT(TType::fkmult, "Error: fullMultiply_karatsuba of\n", rhs, " and\n", lhs,
                      "\nresulted in\n", result, "\ninstead of\n", expectedResult, "\n");

    return true;
}

bool testFullMultiplication()
{
    std::cout << "Testing full multiplication" << std::endl;
    if(!testTheTest()) DRET;

    std::mt19937 rng(1234);
    if(!testFullMultiplication<1, 1, 1000000>(rng)) DRET;
    if(!testFullMultiplication<2, 1, 1000000>(rng)) DRET;
    if(!testFullMultiplication<3, 1, 1000000>(rng)) DRET;
    if(!testFullMultiplication<4, 1, 1000000>(rng)) DRET;
    if(!testFullMultiplication<5, 1, 1000000>(rng)) DRET;
    if(!testFullMultiplication<32, 1, 100000>(rng)) DRET;
    if(!testFullMultiplication<2, 2, 1000000>(rng)) DRET;
    if(!testFullMultiplication<3, 2, 100000>(rng)) DRET;
    if(!testFullMultiplication<4, 2, 100000>(rng)) DRET;
    if(!testFullMultiplication<5, 2, 100000>(rng)) DRET;
    if(!testFullMultiplication<32, 2, 100000>(rng)) DRET;
    if(!testFullMultiplication<3, 3, 100000>(rng)) DRET;
    if(!testFullMultiplication<4, 3, 100000>(rng)) DRET;
    if(!testFullMultiplication<5, 3, 100000>(rng)) DRET;
    if(!testFullMultiplication<32, 3, 100000>(rng)) DRET;
    if(!testFullMultiplication<4, 4, 100000>(rng)) DRET;
    if(!testFullMultiplication<5, 4, 100000>(rng)) DRET;
    if(!testFullMultiplication<32, 4, 100000>(rng)) DRET;
    if(!testFullMultiplication<5, 5, 100000>(rng)) DRET;
    if(!testFullMultiplication<6, 5, 100000>(rng)) DRET;
    if(!testFullMultiplication<32, 5, 50000>(rng)) DRET;
    if(!testFullMultiplication<6, 6, 50000>(rng)) DRET;
    if(!testFullMultiplication<7, 6, 50000>(rng)) DRET;
    if(!testFullMultiplication<34, 16, 10000>(rng)) DRET;
    if(!testFullMultiplication<34, 17, 10000>(rng)) DRET;
    if(!testFullMultiplication<34, 18, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 6, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 10, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 15, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 17, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 18, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 19, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 22, 10000>(rng)) DRET;
    if(!testFullMultiplication<35, 32, 40000>(rng)) DRET;
    if(!testFullMultiplication<35, 35, 40000>(rng)) DRET;
    if(!testFullMultiplication<50, 12, 10000>(rng)) DRET;
    if(!testFullMultiplication<50, 13, 10000>(rng)) DRET;
    if(!testFullMultiplication<50, 20, 10000>(rng)) DRET;
    if(!testFullMultiplication<50, 24, 10000>(rng)) DRET;
    if(!testFullMultiplication<50, 25, 10000>(rng)) DRET;
    if(!testFullMultiplication<50, 26, 10000>(rng)) DRET;
    if(!testFullMultiplication<50, 40, 20000>(rng)) DRET;
    if(!testFullMultiplication<50, 50, 40000>(rng)) DRET;
    if(!testFullMultiplication<51, 12, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 13, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 14, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 25, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 26, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 27, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 50, 10000>(rng)) DRET;
    if(!testFullMultiplication<51, 51, 40000>(rng)) DRET;
    if(!testFullMultiplication<100, 10, 8000>(rng)) DRET;
    if(!testFullMultiplication<100, 40, 8000>(rng)) DRET;
    if(!testFullMultiplication<100, 49, 6000>(rng)) DRET;
    if(!testFullMultiplication<100, 50, 6000>(rng)) DRET;
    if(!testFullMultiplication<100, 51, 6000>(rng)) DRET;
    if(!testFullMultiplication<100, 60, 6000>(rng)) DRET;
    if(!testFullMultiplication<100, 100, 10000>(rng)) DRET;
    if(!testFullMultiplication<101, 5, 8000>(rng)) DRET;
    if(!testFullMultiplication<101, 17, 8000>(rng)) DRET;
    if(!testFullMultiplication<101, 50, 6000>(rng)) DRET;
    if(!testFullMultiplication<101, 51, 6000>(rng)) DRET;
    if(!testFullMultiplication<101, 52, 6000>(rng)) DRET;
    if(!testFullMultiplication<101, 99, 6000>(rng)) DRET;
    if(!testFullMultiplication<101, 100, 6000>(rng)) DRET;
    if(!testFullMultiplication<101, 101, 7000>(rng)) DRET;
    if(!testFullMultiplication<103, 103, 7000>(rng)) DRET;
    if(!testFullMultiplication<106, 106, 6000>(rng)) DRET;
    if(!testFullMultiplication<125, 125, 6000>(rng)) DRET;
    if(!testFullMultiplication<127, 127, 6000>(rng)) DRET;
    if(!testFullMultiplication<200, 10, 6000>(rng)) DRET;
    if(!testFullMultiplication<200, 50, 4000>(rng)) DRET;
    if(!testFullMultiplication<200, 100, 3000>(rng)) DRET;
    if(!testFullMultiplication<200, 150, 2000>(rng)) DRET;
    if(!testFullMultiplication<200, 200, 3000>(rng)) DRET;

    return true;
}
