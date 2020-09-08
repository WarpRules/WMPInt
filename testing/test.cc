#include "test.hh"
#include <random>
#include <utility>
#include <chrono>
#include <cstring>
#include <cctype>

namespace
{
    std::mt19937_64 gRngEngine(0);
}

//============================================================================
// Test WMPUInt<1>
//============================================================================
static bool testSize1Operators()
{
    WMPUInt<1> a(20), b({ 10 });
    if(a + b != 30) return DPRINT("Error\n");
    if(a + 30 != 50) return DPRINT("Error\n");
    if(a - b != 10) return DPRINT("Error\n");
    if(a - 5 != 15) return DPRINT("Error\n");
    if(a * b != 200) return DPRINT("Error\n");
    if(a * 5 != 100) return DPRINT("Error\n");
    if((a & b) != 0) return DPRINT("Error\n");
    if((a | b) != 30) return DPRINT("Error\n");
    if((a ^ b) != 30) return DPRINT("Error\n");
    return true;
}


//============================================================================
// Test assignment from ASCII string
//============================================================================
template<typename... Values_t>
static bool testAssignmentFromHexStr(const char* str, Values_t&&... values)
{
    using UInt_t = WMPUInt<sizeof...(Values_t)>;
    UInt_t v1, v2(values...);
    const std::size_t strLength = std::strlen(str);

    const char* endPtr = v1.assignFromHexStr(str);
    if(v1 != v2) return DPRINT("Error: Initializing with \"", str, "\" failed:\n",
                               v1, "\n", v2, "\n");
    if(endPtr != str + strLength)
        return DPRINT("Error: initializing with \"", str,
                      "\" did not return the correct pointer.\n");

    const std::size_t outputLength = v2.maxHexadecimalDigits();
    char uppercaseStr[outputLength + 1] = {};
    char outputStr[outputLength + 1] = {};

    const std::size_t originalStrLength =
        (strLength > outputLength ? outputLength : strLength);
    const char* originalStr = str + (strLength - originalStrLength);

    for(std::size_t i = 0; i < originalStrLength; ++i)
        uppercaseStr[i] = std::toupper(originalStr[i]);

    v2.printAsHexStr(outputStr);

    const char* ptr = outputStr;
    while(*ptr == '0') ++ptr;
    if(!*ptr && ptr > outputStr) --ptr;

    if(std::strcmp(uppercaseStr, ptr) != 0)
        return DPRINT("Error: printAsHexStr resulted in\n\"", ptr,
                      "\"\ninstead of\n\"", uppercaseStr, "\"\n");

    return true;
}

template<typename... Values_t>
static bool testAssignmentFromDecStr(const char* str, Values_t&&... values)
{
    WMPUInt<sizeof...(Values_t)> v1, v2(values...);
    const char* endPtr = v1.assignFromDecStr(str);
    if(v1 != v2) return DPRINT("Error: Initializing with \"", str, "\" failed:\n     Got: ",
                               v1, "\nExpected: ", v2, "\n");
    if(endPtr != str + std::strlen(str))
        return DPRINT("Error: initializing with \"", str, "\" did not return the correct pointer.\n");

    WMPUInt<sizeof...(Values_t) + 1> v3, v4(values...);
    v3.data()[0] = 12345;
    endPtr = v3.assignFromDecStr(str);
    if(v3 != v4) return DPRINT("Error: Initializing with \"", str, "\" failed:\n     Got: ",
                               v3, "\nExpected: ", v4, "\n");
    if(endPtr != str + std::strlen(str))
        return DPRINT("Error: initializing with \"", str, "\" did not return the correct pointer.\n");

    return true;
}

template<std::size_t kSize>
static bool testAssignmentAndPrintingAsDecStr()
{
    const std::size_t kMaxDigits = WMPUInt<kSize>::maxDecimalDigits();
    WMPUInt<kSize> value;
    char inputStr[kMaxDigits+1], outputStr[kMaxDigits+1];
    outputStr[kMaxDigits] = 0;

    for(std::size_t strLength = 1; strLength < kMaxDigits; ++strLength)
    {
        for(unsigned iteration = 0; iteration < 1000; ++iteration)
        {
            inputStr[0] = '1' + gRngEngine() % 9;
            for(unsigned i = 1; i < strLength; ++i)
                inputStr[i] = '0' + gRngEngine() % 10;
            inputStr[strLength] = 0;

            value.assignFromDecStr(inputStr);
            const char* resultStr = value.printAsDecStr(outputStr);

            if(resultStr < outputStr)
                return DPRINT("Error: WMPUInt<", kSize,
                              ">::printAsDecStr() outputted too many characters for value\n",
                              value, "\n");

            if(std::strcmp(inputStr, resultStr) != 0)
                return DPRINT("Error: WMPUInt<", kSize,
                              ">::printAsDecStr() outputted\n\"", resultStr,
                              "\"\ninstead of\n\"", inputStr, "\"\n");
        }
    }

    return true;
}

static bool testAssignmentFromStr()
{
    std::cout << "Testing assignment from string" << std::endl;
    if(!testAssignmentFromHexStr("0", UINT64_C(0x0), UINT64_C(0x0))) DRET;
    if(!testAssignmentFromHexStr("A", UINT64_C(0x0), UINT64_C(0xA))) DRET;
    if(!testAssignmentFromHexStr("a", UINT64_C(0x0), UINT64_C(0xA))) DRET;
    if(!testAssignmentFromHexStr("aAbBcCdDeEfF", UINT64_C(0x0), UINT64_C(0xaAbBcCdDeEfF))) DRET;
    if(!testAssignmentFromHexStr("AbBcCdDeEfF", UINT64_C(0x0), UINT64_C(0xAbBcCdDeEfF))) DRET;
    if(!testAssignmentFromHexStr("1234567890123456abcdef1234567890",
                                 UINT64_C(0x1234567890123456),
                                 UINT64_C(0xabcdef1234567890))) DRET;
    if(!testAssignmentFromHexStr("AABBCC1234567890123456abcdef1234567890",
                                 UINT64_C(0x1234567890123456),
                                 UINT64_C(0xabcdef1234567890))) DRET;
    if(!testAssignmentFromHexStr("100000000000000000000000000000000000000000000000",
                                 UINT64_C(0x1000000000000000), UINT64_C(0), UINT64_C(0))) DRET;
    if(!testAssignmentFromHexStr("1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF",
                                 UINT64_C(0x1234567890ABCDEF), UINT64_C(0x1234567890ABCDEF),
                                 UINT64_C(0x1234567890ABCDEF), UINT64_C(0x1234567890ABCDEF))) DRET;
    if(!testAssignmentFromHexStr("1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF"
                                 "1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF",
                                 UINT64_C(0x1234567890ABCDEF), UINT64_C(0x1234567890ABCDEF),
                                 UINT64_C(0x1234567890ABCDEF), UINT64_C(0x1234567890ABCDEF),
                                 UINT64_C(0x1234567890ABCDEF), UINT64_C(0x1234567890ABCDEF),
                                 UINT64_C(0x1234567890ABCDEF), UINT64_C(0x1234567890ABCDEF))) DRET;

    if(!testAssignmentFromDecStr("0", UINT64_C(0), UINT64_C(0))) DRET;
    if(!testAssignmentFromDecStr("9", UINT64_C(0), UINT64_C(9))) DRET;
    if(!testAssignmentFromDecStr("1234567890", UINT64_C(0), UINT64_C(1234567890))) DRET;
    if(!testAssignmentFromDecStr("18446744073709551616", UINT64_C(1), UINT64_C(0))) DRET;
    if(!testAssignmentFromDecStr("12345678901234567890",
                                 UINT64_C(0), UINT64_C(12345678901234567890))) DRET;
    if(!testAssignmentFromDecStr("123456789012345678901",
                                 UINT64_C(0x6), UINT64_C(0xB14E9F812F366C35))) DRET;
    if(!testAssignmentFromDecStr("123456789012345678901234567890",
                                 UINT64_C(0x18EE90FF6), UINT64_C(0xC373E0EE4E3F0AD2))) DRET;
    if(!testAssignmentFromDecStr("340282366920938463463374607431768211456",
                                 UINT64_C(1), UINT64_C(0), UINT64_C(0))) DRET;
    if(!testAssignmentFromDecStr("6277101735386680763835789423207666416102355444464034512896",
                                 UINT64_C(1), UINT64_C(0), UINT64_C(0), UINT64_C(0))) DRET;
    if(!testAssignmentFromDecStr("115792089237316195423570985008687907853269984665640564039457584007913129639936",
                                 UINT64_C(1), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0))) DRET;
    if(!testAssignmentFromDecStr("2135987035920910082395021706169552114602704522356652769947041607822219725780640550022962086936576",
                                 UINT64_C(1), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0))) DRET;
    if(!testAssignmentFromDecStr
       ("1234567890123456789012345678901234567890123456789012345678901234567890",
        UINT64_C(0x0000002DCAEC4C2D), UINT64_C(0xF4268937664439BA),
        UINT64_C(0x2F162FC2D76998CB), UINT64_C(0xACCFF196CE3F0AD2))) DRET;

    if(!testAssignmentAndPrintingAsDecStr<2>()) DRET;
    if(!testAssignmentAndPrintingAsDecStr<3>()) DRET;
    if(!testAssignmentAndPrintingAsDecStr<4>()) DRET;
    if(!testAssignmentAndPrintingAsDecStr<5>()) DRET;
    if(!testAssignmentAndPrintingAsDecStr<6>()) DRET;
    if(!testAssignmentAndPrintingAsDecStr<7>()) DRET;
    return true;
}


//============================================================================
// Addition, subtraction and multiplication testing helper functions
//============================================================================
template<typename Value_t1, typename Value_t2, std::size_t kSize>
static bool checkAddition(const Value_t1& value1, const Value_t2& value2,
                          const WMPUInt<kSize>& expectedResult)
{
    const WMPUInt<kSize> result = value1 + value2;
    if(result != expectedResult)
        return DPRINT("Error: ", value1, "+", value2,
                      "\nresulted in ", result,
                      "\n instead of ", expectedResult, "\n");
    return true;
}

template<typename Value_t1, typename Value_t2, std::size_t kSize>
static bool checkSubtraction(const Value_t1& value1, const Value_t2& value2,
                             const WMPUInt<kSize>& expectedResult)
{
    const WMPUInt<kSize> result = value1 - value2;
    if(result != expectedResult)
        return DPRINT("Error: ", value1, "-", value2,
                      "\nresulted in ", result,
                      "\n instead of ", expectedResult, "\n");
    return true;
}

template<typename Value_t1, typename Value_t2, std::size_t kSize>
static bool checkMultiplication(const Value_t1& value1, const Value_t2& value2,
                                const WMPUInt<kSize>& expectedResult)
{
    const WMPUInt<kSize> result = value1 * value2;
    if(result != expectedResult)
        return DPRINT("Error: ", value1, "*", value2,
                      "\nresulted in ", result,
                      "\n instead of ", expectedResult, "\n");

    return true;
}


//============================================================================
// Test addition
//============================================================================
template<std::size_t kSize>
static bool testAdditionWithSize()
{
    WMPUInt<kSize> value1(-1), value2(1), value3(0);

    for(unsigned i = 1; i < kSize-1; ++i)
        value1.data()[i] = -1;

    if(!checkAddition(value1, value3, value1)) DRET;
    if(!checkAddition(value1, 0, value1)) DRET;
    if(!checkAddition(0, value1, value1)) DRET;
    if(!checkAddition(value2, value3, value2)) DRET;
    if(!checkAddition(value2, 0, value2)) DRET;
    if(!checkAddition(0, value2, value2)) DRET;
    if(!checkAddition(value3, value1, value1)) DRET;
    if(!checkAddition(value3, value2, value2)) DRET;
    if(!checkAddition(value3, std::uint64_t(1), value2)) DRET;
    if(!checkAddition(std::uint64_t(1), value3, value2)) DRET;

    value3.data()[0] = 1;

    if(!checkAddition(value1, value2, value3)) DRET;
    if(!checkAddition(value1, 1, value3)) DRET;
    if(!checkAddition(1, value1, value3)) DRET;
    if(!checkAddition(value2, value1, value3)) DRET;

    for(std::uint64_t value = 0; value < 100; ++value)
    {
        for(std::size_t i = 0; i < kSize; ++i)
        {
            value1.data()[i] = value;
            value2.data()[i] = value+1;
            value3.data()[i] = value+value+1;
        }

        if(!checkAddition(value1, value2, value3)) DRET;
    }

    WMPUInt<kSize> value4(-1), value5(0);

    for(std::uint64_t i = 1; i < kSize-1; ++i)
        value4.data()[i] = -1;

    value5.data()[0] = 1;

    for(std::uint64_t i = 0; i < 100; ++i)
    {
        value5.data()[kSize-1] = i;
        if(!checkAddition(value4, i+1, value5)) DRET;
        if(!checkAddition(i+1, value4, value5)) DRET;
    }

    for(std::uint64_t value = 0; value < 1000; ++value)
    {
        for(std::size_t i = 0; i < kSize; ++i)
        {
            value1.data()[i] = value*10;
            value2.data()[i] = value*7;
            value3.data()[i] = value*10 - value*7;
        }
        if(!checkSubtraction(value1, value2, value3)) DRET;
    }

    for(std::size_t i = 0; i < kSize; ++i)
        value3.data()[i] = -1;
    value2 = WMPUInt<kSize>(2);
    value1 = WMPUInt<kSize>(1);

    if(!checkSubtraction(value1, value2, value3)) DRET;
    if(!checkSubtraction(value1, 2, value3)) DRET;
    if(!checkSubtraction(1, value2, value3)) DRET;

    WMPUInt<kSize> value[3], result1[2], result2[2];

    for(std::uint64_t startValue = 1; startValue <= 100; ++startValue)
    {
        for(std::size_t cInd = 0; cInd < kSize; ++cInd)
        {
            value[0].data()[cInd] = startValue + cInd;
            value[1].data()[cInd] = startValue + cInd + 1;
            value[2].data()[cInd] = startValue + cInd + 2;
            result1[0].data()[cInd] = value[0].data()[cInd] + value[1].data()[cInd];
            result1[1].data()[cInd] = value[0].data()[cInd] + value[2].data()[cInd];
        }

        if(!checkAddition(value[0], value[1], result1[0])) DRET;
        if(!checkAddition(value[0], value[2], result1[1])) DRET;

        result2[0] = value[1];
        result2[1] = value[2];
        value[0].addTo(result2[0], result2[1]);

        if(!checkAddition(value[0], value[1], result2[0])) DRET;
        if(!checkAddition(value[0], value[2], result2[1])) DRET;

        for(std::size_t cInd = kSize-1; cInd > 0; --cInd)
        {
            for(std::size_t cInd = 0; cInd < kSize; ++cInd)
            {
                value[0].data()[cInd] = startValue + cInd;
                value[1].data()[cInd] = startValue + cInd + 1;
                value[2].data()[cInd] = startValue + cInd + 2;
                result1[0].data()[cInd] = value[0].data()[cInd] + value[1].data()[cInd];
                result1[1].data()[cInd] = value[0].data()[cInd] + value[2].data()[cInd];
            }

            value[0].data()[cInd] = UINT64_C(0xFFFFFFFFFFFFFFFF);
            result1[0].data()[cInd] = value[0].data()[cInd] + value[1].data()[cInd];
            ++result1[0].data()[cInd-1];
            result1[1].data()[cInd] = value[0].data()[cInd] + value[2].data()[cInd];
            ++result1[1].data()[cInd-1];

            if(!checkAddition(value[0], value[1], result1[0])) DRET;
            if(!checkAddition(value[0], value[2], result1[1])) DRET;

            result2[0] = value[1];
            result2[1] = value[2];
            value[0].addTo(result2[0], result2[1]);

            if(!checkAddition(value[0], value[1], result2[0])) DRET;
            if(!checkAddition(value[0], value[2], result2[1])) DRET;

            result2[0] = value[1];
            result2[1] = value[2];
            addWMPPair(result2[0], value[0], result2[1], value[0]);

            if(!checkAddition(value[0], value[1], result2[0])) DRET;
            if(!checkAddition(value[0], value[2], result2[1])) DRET;
        }
    }

    return true;
}

static bool testAddition()
{
    std::cout << "Testing addition" << std::endl;
    if(!testAdditionWithSize<2>()) return false;
    if(!testAdditionWithSize<3>()) return false;
    if(!testAdditionWithSize<4>()) return false;
    if(!testAdditionWithSize<5>()) return false;
    if(!testAdditionWithSize<6>()) return false;
    if(!testAdditionWithSize<7>()) return false;
    if(!testAdditionWithSize<8>()) return false;
    if(!testAdditionWithSize<100>()) return false;

    return true;
}


//============================================================================
// Test multiplication
//============================================================================
static __uint128_t get_uint128(std::uint64_t lsw, std::uint64_t msw)
{
    __uint128_t value = msw;
    value <<= 64;
    value |= lsw;
    return value;
}

static bool testMultiplicationWithSize2(std::uint64_t v1_lsw, std::uint64_t v1_msw,
                                        std::uint64_t v2_lsw, std::uint64_t v2_msw)
{
    WMPUInt<2> input1(v1_msw, v1_lsw), input2(v2_msw, v2_lsw);
    WMPUInt<2> output = input1 * input2;
    const __uint128_t testValue1 = get_uint128(v1_lsw, v1_msw);
    const __uint128_t testValue2 = get_uint128(v2_lsw, v2_msw);
    const __uint128_t resultValue = testValue1 * testValue2;
    const std::uint64_t resultValue_lsw = resultValue;
    const std::uint64_t resultValue_msw = resultValue >> 64;

    if(resultValue_msw != output.data()[0] ||
       resultValue_lsw != output.data()[1])
        return DPRINT("Error: ", input1, " * ", input2,
                      "\ngave result ", output,
                      "\ninstead of  [", resultValue_msw, ",", resultValue_lsw, "]\n");

    output = input1 * v2_lsw;
    const __uint128_t testValue3 = get_uint128(v2_lsw, 0);
    const __uint128_t resultValue2 = testValue1 * testValue3;
    const std::uint64_t resultValue2_lsw = resultValue2;
    const std::uint64_t resultValue2_msw = resultValue2 >> 64;

    if(resultValue2_msw != output.data()[0] ||
       resultValue2_lsw != output.data()[1])
        return DPRINT("Error: ", input1, " * ", v2_lsw,
                      "\ngave result ", output,
                      "\ninstead of  [", resultValue_msw, ",", resultValue_lsw, "]\n");

    return true;
}

static bool testMultiplicationWithSize2()
{
    WMPUInt<2> result;

    for(unsigned i = 0; i < 1000000; ++i)
    {
        if(!testMultiplicationWithSize2(gRngEngine(), gRngEngine(), gRngEngine(), gRngEngine()))
            DRET;

        const WMPUInt<1> input1(gRngEngine()), input2(gRngEngine());
        const __uint128_t testValue1 = *input1.data(), testValue2 = *input2.data();
        input1.fullMultiply(input2, result, nullptr);
        const __uint128_t resultValue = testValue1 * testValue2;
        const std::uint64_t resultValue_lsw = resultValue;
        const std::uint64_t resultValue_msw = resultValue >> 64;

        if(resultValue_msw != result.data()[0] || resultValue_lsw != result.data()[1])
            return DPRINT("Error: ", input1, " * ", input2,
                          "\ngave result ", result,
                          "\ninstead of  [", resultValue_msw, ",", resultValue_lsw, "]\n");
    }

    return true;
}

template<std::size_t kSize>
static bool testMultiplicationWithPatterns(unsigned bitPattern1, unsigned bitPattern2)
{
    WMPUInt<kSize> value1, value2;
    std::uint64_t p1 = 0, p2 = 0;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        const bool b1 = (bitPattern1 >> i) & 1;
        const bool b2 = (bitPattern2 >> i) & 1;
        value1.data()[(kSize-1)-i] = (b1 ? -1 : 0);
        value2.data()[(kSize-1)-i] = (b2 ? -1 : 0);
        if(b1) p1 |= 0xFllu << (i*4);
        if(b2) p2 |= 0xFllu << (i*4);
    }

    WMPUInt<kSize> result = value1 * value2, expectedResult;
    const std::uint64_t pResult = p1 * p2;

    for(std::size_t i = 0; i < kSize; ++i)
    {
        const std::uint64_t nibble = (pResult >> (i*4)) & 0xFllu;
        const std::uint64_t component =
            (nibble <= 4 ? nibble : 0xFFFFFFFFFFFFFFF0llu + nibble);
        expectedResult.data()[(kSize-1)-i] = component;
    }

    if(result != expectedResult)
        return DPRINT("Error: ", value1, "*", value2,
                      "\ngave result ", result,
                      "\ninstead of  ", expectedResult,
                      "\nBit pattern: ", pResult, "\n");

    if(bitPattern2 == 1)
    {
        std::uint64_t rhs = UINT64_C(0xFFFFFFFFFFFFFFFF);
        result = value1 * rhs;
        if(result != expectedResult)
            return DPRINT("Error: ", value1, "*", rhs, " (", value2, ")",
                          "\ngave result ", result,
                          "\ninstead of  ", expectedResult,
                          "\nBit pattern: ", pResult, "\n");

        result = rhs * value1;
        if(result != expectedResult)
            return DPRINT("Error: ", rhs, "*", value1,
                          "\ngave result ", result,
                          "\ninstead of  ", expectedResult,
                          "\nBit pattern: ", pResult, "\n");
    }

    return true;
}

template<std::size_t kSize>
static bool testMultiplicationWithAddition()
{
    WMPUInt<kSize> value, factor(0), result1, result2;

    for(unsigned counter = 0; counter < 5000; ++counter)
    {
        for(std::size_t i = 0; i < kSize; ++i)
            value.data()[i] = gRngEngine();

        result1 = value;

        for(std::uint64_t f = 2; f < 100; ++f)
        {
            result1 += value;
            factor.data()[kSize-1] = f;
            result2 = value * factor;

            if(result1 != result2)
                return DPRINT("Error: ", value, " added\n",
                              f, " times gave result ", result1,
                              "\nand multiplied by ", factor,
                              "\ngave result ", result2, "\n");

            result2 = value * f;

            if(result1 != result2)
                return DPRINT("Error: ", value, " added\n",
                              f, " times gave result ", result1,
                              "\nand multiplied by ", f,
                              "\ngave result ", result2, "\n");
        }
    }

    return true;
}

template<std::size_t kSize, std::size_t kArraySize>
static bool testMultiplication(const WMPUInt<kSize>(&array)[kArraySize][3])
{
    for(std::size_t i = 0; i < kArraySize; ++i)
    {
        if(!checkMultiplication(array[i][0], array[i][1], array[i][2])) DRET;
        if(!checkMultiplication(array[i][1], array[i][0], array[i][2])) DRET;

        for(unsigned j = 0; j < 3; ++j)
        {
            if(!checkMultiplication(array[i][j], 1, array[i][j])) DRET;
            const WMPUInt<kSize> doubleValue = array[i][j] + array[i][j];
            if(!checkMultiplication(array[i][j], 2, doubleValue)) DRET;
        }
    }
    return true;
}

template<std::size_t kSize>
static bool checkFullMultiplication
(const WMPUInt<kSize>& value1, const WMPUInt<kSize>& value2, const char* resultStr)
{
    WMPUInt<kSize> result1;
    result1.assignFromHexStr(resultStr);

    if(!checkMultiplication(value1, value2, result1)) DRET;
    if(!checkMultiplication(value2, value1, result1)) DRET;

    WMPUInt<kSize*2> value3, value4, result2;
    value3.assign(value1);
    value4.assign(value2);
    result2.assignFromHexStr(resultStr);

    if(!checkMultiplication(value3, value4, result2)) DRET;
    if(!checkMultiplication(value4, value3, result2)) DRET;

    if constexpr(kSize == 2)
    {
        WMPUInt<kSize*2> result3;
        std::uint64_t tempBuffer[kSize];
        value1.fullMultiply(value2, result3, tempBuffer);
        if(result3 != result2)
            return DPRINT("Error: fullMultiply of values\n", value1, "\nand\n", value2,
                          "\nresulted in ", result3,
                          "\ninstead of  ", result2, "\n");
    }

    return true;
}

static bool testMultiplicationAndStrInput()
{
    const struct { WMPUInt<2> v1, v2; const char* resultStr; } values2[] =
    {
     {WMPUInt<2>(0xC96D191CF6F6AEA6, 0x401F7AC78BC80F1C),
      WMPUInt<2>(0xB5EE8CB6ABE457F8, 0xF258D22D4DB91392),
     "8F25D4E638153EA10F73DF4CDD71573EE186F458DDF0582BFF7FA537E873B1F8"},
     {WMPUInt<2>(0x04EEF2B4B5D860CC, 0x67A7AABE10D172D6),
      WMPUInt<2>(0x40565D50E72B4021, 0x05D07B7D1E8DE386),
     "13D66BF13771FCC41652F415CB0BECF98DABCE9DA23682793239BB5DA53DE04"},
     {WMPUInt<2>(0x8548DEA130821ACC, 0x583C502C832E0A3A),
      WMPUInt<2>(0x4631AEDE2E67FFD1, 0x8F9FCCBA4388A61F),
     "248BCADDE4C9B87EBAE09FEE15E476C6CC293CCC47867284C8E4F5BA5A04D906"},
     {WMPUInt<2>(0x23D9A035F5E09570, 0x8B3A26B7AA4BCECB),
      WMPUInt<2>(0x859C449A06E0302C, 0xDB696AB700FEB090),
     "12B5F2750A0EB730A2DBD6E29789B79424F8A616C4C69A6A4FA2E4B81639E230"},
     {WMPUInt<2>(0x7FF1366399D92B12, 0x6B5BD57A3C9113EF),
      WMPUInt<2>(0xBE892B0C53E40D3D, 0x3FC97B87BED94159),
     "5F3993F7B4DAF8C700A83AEB65749839D6A860C1480C43456E6E9CAE2C169D17"},
     {WMPUInt<2>(0x3D413B8D11B4CCE2, 0x51EFC5D2498D7506),
      WMPUInt<2>(0xE916957641C27421, 0x2A327E8F39FC19A6),
     "37C5C691C6B3B9AF6D8024B2CE363EB069D021F1EA856118A5F608320A0F77E4"},
     {WMPUInt<2>(0x3EDB3BFE2F0B6337, 0x32C51436B7C00275),
      WMPUInt<2>(0xB744BED2696ED37E, 0xF7C35C861856282A),
     "2CFF98F90366A21CC2A221D2DEC683802EBCA1CF55DE8BFAFC9BEDA9F130AF32"},
     {WMPUInt<2>(0xC4F978FB19FFB724, 0x14A93CA1D9BCEA61),
      WMPUInt<2>(0x75BDA2D6BFFCFCA4, 0x41DBE94941A43D12),
     "5A97EDC42F16E213A5E523565A0A9101BE47635FAC6425DCE17D67751A4597D2"},
     {WMPUInt<2>(0xC6EC7495AC0E00FD, 0x957955653083196E),
      WMPUInt<2>(0xF346DE027CA95D44, 0x702751D1BB724213),
     "BD0983D4EF0F2E2F01A5AA6E174456F7916A8BD156CCD70B675A5BC713453F2A"},
     {WMPUInt<2>(0x528184B1277F75FE, 0x884BB2027E9AC7B0),
      WMPUInt<2>(0x41A0BC6DD5C28762, 0x0BA88011CD101288),
     "1526B05E6D3C1A3EA609A35C3AF565D0F2D1BDD0D219C61441E3DC7E8F447580"},
     {WMPUInt<2>(0x814621BD927E0DAC, 0xB23CB1552B043B6E),
      WMPUInt<2>(0x175A1FED9BBDA880, 0xE838FF59B1C9D964),
     "BCACFCF5F7741150435C6DC4FE1C1570B4E07F95F57450C0C5E65631B6574F8"},
     {WMPUInt<2>(0x07EA06B48FCA72AC, 0x26EBDCF08553011A),
      WMPUInt<2>(0xFB44EA3C3A45CF1C, 0x9ED34D63DF99A685),
     "7C495FA97B97BF5D554282F01ED13DD57A50BCDE77FF927348CFB0865606E82"},
     {WMPUInt<2>(0x4C7BF671EAEA7207, 0x5C7FC5FC683A1085),
      WMPUInt<2>(0x7B20C584708499B9, 0x4C3FB0CEB4ADB6B9),
     "24C959EA804A9845B12A1F1063A4C0049EF5221F55E825B54AC2836147957E1D"},
     {WMPUInt<2>(0x4902095A15D7F3D2, 0xEC97F42C55BC9F40),
      WMPUInt<2>(0xA0FFC0F9681BB9AC, 0xC149BD468AC1AC86),
     "2DEA35E846121F783B82F9525C8566B12B52606BB01304D147A1B55C2AFA5B80"},
     {WMPUInt<2>(0xB6C1A68207BA2FC9, 0xB906A73E05A92C74),
      WMPUInt<2>(0x11E0D6EBD61D941D, 0x7CA12FB5B05B5C4D),
     "CC35EEE9219AAC68204418261B41CD25EAC9BD19E3F727856C4A3670D180EE4"},
     {WMPUInt<2>(0x16BF95DEFA2CD170, 0xC27697252E02CB81),
      WMPUInt<2>(0x6C7F49BF802C66F5, 0x98D3DAAA3B2E8562),
     "9A422D3113A284E50332E006CD1F6ACDDAAAF031366CDCD0AAF14345CF9EC62"},
     {WMPUInt<2>(0x161F5FC4BA37F6D7, 0x45E0C63E93FC6383),
      WMPUInt<2>(0x9FB1DBFBC95C83A0, 0x38DDD8A535D2CBBD),
     "DCCDB2EFFC455099BB7854EF2EB70652C472E26B264917E4DD84C9F24B458B7"},
     {WMPUInt<2>(0x39B6F08DAF36CA87, 0x6F23D32E2A0FD7FA),
      WMPUInt<2>(0xFCC027348974B455, 0x360369EDA9C0E07D),
     "38FB66D699890136D0FCF56921D5C0A558FEFDAA5B822B662DD900406C373512"},
     {WMPUInt<2>(0xDA6C4763C2C466D7, 0x48BBB7A741E6DDD9),
      WMPUInt<2>(0xD61C0C76DEB4818C, 0x5DE152345F136375),
     "B6AE7227B2FEF9233A0786EA109C0D897DC033CC1C06A7DCAF5CC07764694F2D"},
     {WMPUInt<2>(0xEF65D2FCBB279CFD, 0xDC22B9F9F9D7538D),
      WMPUInt<2>(0x7DAC563216D61E70, 0x05A6F16B79BBD6E9),
     "7585E10A354F2E38136FAFB15B9EC3BF4CA97577CC34A9F874D82B1511D1E955"},
     {WMPUInt<2>(0x5CB3B670AE90BE6C, 0xBC87A781B47462CE),
      WMPUInt<2>(0x84F579568A8972C8, 0x6C469AD3CBA9B91A),
     "30258DFFFB9EE529592947E339B36B21A43C9D6282EE6792483D858D0336E6EC"},
     {WMPUInt<2>(0x076EB3891FD21CAB, 0xE8C41087C07C91FC),
      WMPUInt<2>(0x1CB7CD1DFBDAB648, 0xFAEC2F3C1E29110D),
     "D571B9DB1FC2F04F22CBCED1E6797967E8D1C9C1E83CF50DA9C2CBF56125CC"},
     {WMPUInt<2>(0xB0158AACD4DCA9F9, 0x7CC1B5019EA1196D),
      WMPUInt<2>(0xBC647D48E5E2AEB0, 0x96B30966F70500D8),
     "8194F06DC0B9C7CA5BD77FD0EF0280C991345CFB2C50CDA1DFD23879820E73F8"},
     {WMPUInt<2>(0x87489EE810F7DAA5, 0x74A51EBA09DD373D),
      WMPUInt<2>(0xD40BB2B0A7CA242D, 0xDED20384BA4B0368),
     "700E5224DB1BC924A42DB70345D87825489CED56E7ED256A61DC7A171A6327C8"},
     {WMPUInt<2>(0x7DD248AB68B9DF14, 0xF83326963D78833D),
      WMPUInt<2>(0xE38821FAF65BB505, 0x23654FF720304706),
     "6FD45EDA0CBAA7FF15F1C2E510A4E225147765FF26F2FD59A2989C6D18A8FE6E"},
     {WMPUInt<2>(0x6FC1C8B51EEC90B2, 0x580A8A7E936A997F),
      WMPUInt<2>(0x1E7207FE6315D685, 0x8C59C6AFCBFAB7BF),
     "D4A7D4DF44B0F0448F3C076420E1877C164148403F8CFA3E11D7573CB484EC1"},
     {WMPUInt<2>(0xC24F82B980D1FA2E, 0x084B779CCC9FBE44),
      WMPUInt<2>(0x1A02F04511F6064E, 0x9640EC87EA1BEE8A),
     "13BE4E44D4267EB0A77D17DB511A69F2FD767F7F47DA76482719DF620A2BC8A8"},
     {WMPUInt<2>(0xB1EE0052DD55D069, 0xCAB4F30BB95C5561),
      WMPUInt<2>(0xD998BABCAF69019F, 0xE0126BEA2556CCD2),
     "973CE5607FC0484023EFAE72EEC2555989F7AE9754C6CEDDB8BEEA59555D5592"},
     {WMPUInt<2>(0x9B016F17C8800310, 0xF41CC5D147950F43),
      WMPUInt<2>(0xFDA9511773320334, 0xDDF85A4C56345E4D),
     "9996EFD67EE14E3DB49F59C8C5E442A44B4C37A4E23E9C83C15F0B86DF0C3127"},
     {WMPUInt<2>(0xA4E47A8EFAE8DEAB, 0x9ACAA313E6DED943),
      WMPUInt<2>(0xE9A600BE8F5C822B, 0x778D332A7E54AB53),
     "967EE02E58FED232BB26AA74E068F53778318E1FB5955508ED8998B6F95C31B9"},
     {WMPUInt<2>(0x1442A265CEFE20CA, 0xE78262E6B329807C),
      WMPUInt<2>(0xD3CCFA96FED4AD17, 0x25B6315BB4E3D4F1),
     "10C324C85889ED572FD55027ED985B3C82520F648A85E367B9B09BC5266CA4BC"},
     {WMPUInt<2>(0xCEA2B7E820395A1F, 0xAB3B169E3F7BA6BA),
      WMPUInt<2>(0x237E6923D4000B08, 0xAC1E02DF1E10EF6F),
     "1CA64818E973D6B4D6676EF4FCB0521E632DB218C7ECED9A23E0B12C2EE4F0A6"},
     {WMPUInt<2>(0xD519DC015EBF61B2, 0xF4F51187FE96B080),
      WMPUInt<2>(0xA137326E14771E17, 0x5B10D4A4C1FC81EA),
     "863335E5CFFFB1E979C4CF118D070A1A11F304FB053C8E149958D6FEE2ADD500"},
     {WMPUInt<2>(0x52BED44BC6EC0A60, 0x10359CFFB84288CE),
      WMPUInt<2>(0x47D17B92CD7647A9, 0x41C9BAFDB9158765),
     "1736A29D7947648023DDB45D2D830731AAC6C6F382A0CADE80F8DCF6DF4A9B46"},
     {WMPUInt<2>(0x16676AA636F40C88, 0x12D8AEFDFF93AD5C),
      WMPUInt<2>(0x19C55CBAB761FC6E, 0x2174EE4468BDD89F),
     "2415F1CCEC9D80E3AFBF4E0C828BB41908426B9B5840E5981AA424AB3EA4C24"},
     {WMPUInt<2>(0xA0BD26F5EDDAAC55, 0x4FDDA840F2BEA00D),
      WMPUInt<2>(0xF387CBA277EE3737, 0xF90BBA5C10DAC7B4),
     "98E8CF96FD5C2AC5196A8D4BC1955BEC411BB49D689607B50E79615DF784A424"},
     {WMPUInt<2>(0x33A43AFBDA5AEEBE, 0xB9E3019D9AF169BB),
      WMPUInt<2>(0xAD210AC8D15BBD2B, 0x9132A5599C996D32),
     "22ECA634B9DE1A0B6CC947354B18DD71757C2F744E433A29003C6B7A31EE4586"},
     {WMPUInt<2>(0xB7E64EB925C34B07, 0x35CB859F0469F3C8),
      WMPUInt<2>(0xBF1F44D40CBDFDAE, 0xBFBABEAA1611B567),
     "894B4B112C61EDF08D31E87E483B346038512E9B7078B59BFCE6E9E610457D78"},
     {WMPUInt<2>(0xE4EA67D4C915E61A, 0x1DEBFA223CA7EFE1),
      WMPUInt<2>(0xA77DFC79C3A3071A, 0x06CC239429A34614),
     "95C591F422045701DE82218F973465861D348EA1F44109EB1086DCAF6DF94394"},
     {WMPUInt<2>(0x4927012902F7E84C, 0x9CA15A0AFF31237F),
      WMPUInt<2>(0x5D9E9BC902C99CA8, 0x47FA9818255561FF),
     "1AC07E02AB838CCAB8938256D0353ABB979ACAE2D6BB3DF9A1146CFFF4907A81"},
    };

    const struct { WMPUInt<8> v1, v2; const char* resultStr; } values8[] =
    {
     {WMPUInt<8>(0xC96D191CF6F6AEA6, 0x401F7AC78BC80F1C, 0xB5EE8CB6ABE457F8, 0xF258D22D4DB91392,
                 0x04EEF2B4B5D860CC, 0x67A7AABE10D172D6, 0x40565D50E72B4021, 0x05D07B7D1E8DE386),
      WMPUInt<8>(0x8548DEA130821ACC, 0x583C502C832E0A3A, 0x4631AEDE2E67FFD1, 0x8F9FCCBA4388A61F,
                 0x23D9A035F5E09570, 0x8B3A26B7AA4BCECB, 0x859C449A06E0302C, 0xDB696AB700FEB090),
     "68DF03E68C025AB0C697F94480452256B01B47197FA3431B94FCAB4BE857BAA0EB08193CAFE777BAACCA5662441A4619E0D27E2A5C6FBAE9E9CF7A9CBA119F0148945D3EBC7263DD40308EF3FD47A06EEF603415E51A4C8B19F924079E6FCAC2BCAFAD70911AC3328D687CCA895709FB459FEED7A419C562ABE8ECF47B301B60"},
     {WMPUInt<8>(0x7FF1366399D92B12, 0x6B5BD57A3C9113EF, 0xBE892B0C53E40D3D, 0x3FC97B87BED94159,
                 0x3D413B8D11B4CCE2, 0x51EFC5D2498D7506, 0xE916957641C27421, 0x2A327E8F39FC19A6),
      WMPUInt<8>(0x3EDB3BFE2F0B6337, 0x32C51436B7C00275, 0xB744BED2696ED37E, 0xF7C35C861856282A,
                 0xC4F978FB19FFB724, 0x14A93CA1D9BCEA61, 0x75BDA2D6BFFCFCA4, 0x41DBE94941A43D12),
     "1F69FC8146D71A6D7EE95AC2AE073B5EB5B0C079BE7F67A813B7AFF036F98CA5F812312F35840BA1C50D323A6890F7B2D03A8720A031A8C027BAB0CA24C1A810770DE92340643BE2B28F0EE92D93FF7D68F932504330B640BAE4A01A5B4E26AE2843B179CE41209061804CAD3034F9EE08982121CE4BD201846C953CBA2E5BAC"},
     {WMPUInt<8>(0xC6EC7495AC0E00FD, 0x957955653083196E, 0xF346DE027CA95D44, 0x702751D1BB724213,
                 0x528184B1277F75FE, 0x884BB2027E9AC7B0, 0x41A0BC6DD5C28762, 0x0BA88011CD101288),
      WMPUInt<8>(0x814621BD927E0DAC, 0xB23CB1552B043B6E, 0x175A1FED9BBDA880, 0xE838FF59B1C9D964,
                 0x07EA06B48FCA72AC, 0x26EBDCF08553011A, 0xFB44EA3C3A45CF1C, 0x9ED34D63DF99A685),
     "6473A59F160E4580D3349AB9D078FD1845E06E201B5F4D501ED64F30E4C1CD983078AE2CA56760BF6C2DEC8A05B9EFA0AC9188FF4D693521B7F4BC01A2783A65A279A2A092FECA986494100D156014FA46E7BBA82A0E5602182B75BBA300828108E81D98B75534A608C70D64FFA7C26FFB91BE607AEB78E2B6BCB58F80A5D0A8"},
     {WMPUInt<8>(0x4C7BF671EAEA7207, 0x5C7FC5FC683A1085, 0x7B20C584708499B9, 0x4C3FB0CEB4ADB6B9,
                 0x4902095A15D7F3D2, 0xEC97F42C55BC9F40, 0xA0FFC0F9681BB9AC, 0xC149BD468AC1AC86),
      WMPUInt<8>(0xB6C1A68207BA2FC9, 0xB906A73E05A92C74, 0x11E0D6EBD61D941D, 0x7CA12FB5B05B5C4D,
                 0x16BF95DEFA2CD170, 0xC27697252E02CB81, 0x6C7F49BF802C66F5, 0x98D3DAAA3B2E8562),
     "3699FC6903F4D013B75A53CC91758AF47E1F69F03D6BA7EE8EE841C0206DACD870AA4FDBECCF7A2CB7A029AE17E150E7C54C44A95D9A4B328FE41E59011A086D3965019ADEE588BC25BE5EED898133CE282374BA16E5AA483C4C93DC7BBF877DCF7DBDFA5DFE0B999D2F802CF606E76E0D4EEA2316772DBC663059A39ED9A94C"},
     {WMPUInt<8>(0x161F5FC4BA37F6D7, 0x45E0C63E93FC6383, 0x9FB1DBFBC95C83A0, 0x38DDD8A535D2CBBD,
                 0x39B6F08DAF36CA87, 0x6F23D32E2A0FD7FA, 0xFCC027348974B455, 0x360369EDA9C0E07D),
      WMPUInt<8>(0xDA6C4763C2C466D7, 0x48BBB7A741E6DDD9, 0xD61C0C76DEB4818C, 0x5DE152345F136375,
                 0xEF65D2FCBB279CFD, 0xDC22B9F9F9D7538D, 0x7DAC563216D61E70, 0x05A6F16B79BBD6E9),
     "12E012F5401890047FF4E552452614DAFC35291705A3189AF1ABA1D1FB05DBC5343C7194836BD4EA283CB81BDEAFA3B22AADEE4C8A88DEDC84559705E1A1C12D9AD0FCC415EA71C17795B1B96C62769D2405048B1B90A00CEBACEF6A28FA4BB5FE2C19EB87AD101E8D2281DADA9F593BE0195CC30E7F258D4280C074CC83CFC5"},
     {WMPUInt<8>(0x5CB3B670AE90BE6C, 0xBC87A781B47462CE, 0x84F579568A8972C8, 0x6C469AD3CBA9B91A,
                 0x076EB3891FD21CAB, 0xE8C41087C07C91FC, 0x1CB7CD1DFBDAB648, 0xFAEC2F3C1E29110D),
      WMPUInt<8>(0xB0158AACD4DCA9F9, 0x7CC1B5019EA1196D, 0xBC647D48E5E2AEB0, 0x96B30966F70500D8,
                 0x87489EE810F7DAA5, 0x74A51EBA09DD373D, 0xD40BB2B0A7CA242D, 0xDED20384BA4B0368),
     "3FC35A62E56DE1ACC53F334D84D9FD782EE6075FA6E7739F17EA1D966827D80C2D98CCAA57E4036A4A11F6BA8E0DE0AA52616B09036CC80D1F0C2163F6856DFC8274FD36526A119D919F1D96BB0E9E85127F992C5D65C7AFDF85F9E6C851AED6407F7CF3C3DD56F840E00F560706671C8A22FA33595C4336841182E62CB11448"},
     {WMPUInt<8>(0x7DD248AB68B9DF14, 0xF83326963D78833D, 0xE38821FAF65BB505, 0x23654FF720304706,
                 0x6FC1C8B51EEC90B2, 0x580A8A7E936A997F, 0x1E7207FE6315D685, 0x8C59C6AFCBFAB7BF),
      WMPUInt<8>(0xC24F82B980D1FA2E, 0x084B779CCC9FBE44, 0x1A02F04511F6064E, 0x9640EC87EA1BEE8A,
                 0xB1EE0052DD55D069, 0xCAB4F30BB95C5561, 0xD998BABCAF69019F, 0xE0126BEA2556CCD2),
     "5F806F364770136FD04BC33E80F5C739B0E139FD2C4916B5F86D192A2FCD9A069C8ED93C4A7D57D82819F25C425A80262DCF178B42BD4A824FE21F185CBCC323839815A71C1ECCD8C1D95B147BD605D6311DE357BDF7EE50A31B8241F252FDEE7E122E0D633B84E39828A06C3D5DA39D149EE59041BC33401CE99F1F7340EEAE"},
     {WMPUInt<8>(0x9B016F17C8800310, 0xF41CC5D147950F43, 0xFDA9511773320334, 0xDDF85A4C56345E4D,
                 0xA4E47A8EFAE8DEAB, 0x9ACAA313E6DED943, 0xE9A600BE8F5C822B, 0x778D332A7E54AB53),
      WMPUInt<8>(0x1442A265CEFE20CA, 0xE78262E6B329807C, 0xD3CCFA96FED4AD17, 0x25B6315BB4E3D4F1,
                 0xCEA2B7E820395A1F, 0xAB3B169E3F7BA6BA, 0x237E6923D4000B08, 0xAC1E02DF1E10EF6F),
     "C4475610CFE992DA004F033DF05D9B56D0BAE3CFD7C947A4F6883817464FF3268D0BA374F0890A59A64CD579DBD50582DDF3E14C88D2D0E4D169663F0F64BF8ADA8E80315CB4FEA79DFCA3453EA70B7D3488ECEB402E94A8860786B1434BA095E29150CADC09982314543BCC5CE19596453A534DDA08DE86345900941D8C5FD"},
     {WMPUInt<8>(0xD519DC015EBF61B2, 0xF4F51187FE96B080, 0xA137326E14771E17, 0x5B10D4A4C1FC81EA,
                 0x52BED44BC6EC0A60, 0x10359CFFB84288CE, 0x47D17B92CD7647A9, 0x41C9BAFDB9158765),
      WMPUInt<8>(0x16676AA636F40C88, 0x12D8AEFDFF93AD5C, 0x19C55CBAB761FC6E, 0x2174EE4468BDD89F,
                 0xA0BD26F5EDDAAC55, 0x4FDDA840F2BEA00D, 0xF387CBA277EE3737, 0xF90BBA5C10DAC7B4),
     "12A64F16B44D6700D5A2E0B8F6E9F7A223166569B9C9D29824C81B4774BABB6163A0E34F092687261C76ADE43032E846EE210FDC963CA7F5046B409F26DAB3CEBC9B3E4787DF804AB0B046C73F952137A7B573AF59FA9C64AD8A047227B24FEC1CF67DC74B43807827C6DEA47F55FF17F3B8A9734202D0986046B45D7B64B604"},
     {WMPUInt<8>(0x33A43AFBDA5AEEBE, 0xB9E3019D9AF169BB, 0xAD210AC8D15BBD2B, 0x9132A5599C996D32,
                 0xB7E64EB925C34B07, 0x35CB859F0469F3C8, 0xBF1F44D40CBDFDAE, 0xBFBABEAA1611B567),
      WMPUInt<8>(0xE4EA67D4C915E61A, 0x1DEBFA223CA7EFE1, 0xA77DFC79C3A3071A, 0x06CC239429A34614,
                 0x4927012902F7E84C, 0x9CA15A0AFF31237F, 0x5D9E9BC902C99CA8, 0x47FA9818255561FF),
     "2E2D8D9838FB1E5EE810513D499A5E83DD535C6E52E3162EA237955A89846FC10B8589AD361601705F6C2565CEC40A182AEFEE36C62DFEE6C935F656A52FE6BCFAC6A5CC9E3BFAB2E93DFAE82FA19A87DE6B67CB4C7680DF0C23E19F47B6C5637545F5FA091FE235E35787CC73EE248301803186E612B185EDE06A89CF92B899"},
     {WMPUInt<8>(0xB613301CA773D9F1, 0xDE64D791FB9AC4FA, 0x1F5AC2193E8E6749, 0xE312B85C388ACFFB,
                 0x986B17A971A64FF9, 0xCB8B41A1609C47BB, 0x9132359C66F27446, 0xFD13D5B1693465E5),
      WMPUInt<8>(0xF676C5B9C8C31DEC, 0x819C9D4648BDE72E, 0xCB1B9807F2E17075, 0xB833DA21219453AE,
                 0x66F5C5F44FB6895F, 0x1DB2622EBC8A5156, 0xD4D55C5A8D8E65C8, 0x57518131D59044B5),
     "AF4AE9B49CCC17E27599C140F0A0EE33775B2313227828C4BFAB92181DDB47082C554783BE2A7D005D6F2D2510B0E46B92783C51AD2E7FBB58966B48F000DF1458913A0C78E64E08651A225EF0AEC23BBE7BB85AEC261D0AE7A7D67A541C3476D354727297CED1DF481F9857F4CFAE2B1A9234DE835546749EF1E27726ECDEE9"},
     {WMPUInt<8>(0xCFDA297096D43D12, 0x3C92C59D9F4F4FC7, 0xEF253867322ED69D, 0x75466261F580F644,
                 0xDA5501F76531DFAF, 0xBFF23DAFF1ECF103, 0x5EA264D24CAFA620, 0xA4F6E95085E2C1D3),
      WMPUInt<8>(0x96FD21923D8280B4, 0xD7E000660C4E449D, 0x0175F4EA08C6D68F, 0x2FC41E957FB4D4C4,
                 0x4C103D0C50171BC7, 0x56B4530E5704AE62, 0xB9D88E9704345821, 0xFE9BBA04DFF384A1),
     "7A975A24C859825933EEBBFA52DD3C4523FF6E79511D787646AFA8F28F0CD5E0778239C06C1D0A95BD4C5BBB53FFE4416F1FE64A82746F447BBDA4799B884DC67B3119C07E02D0B21295BEDE965D2A725AC5AAB7285EFE2A60091AB41DE3E99261BDB4C63C29FFCED821BAA324AD69B67F32AD8BEE7FB28A38C5330DE7D5B1B3"},
     {WMPUInt<8>(0xE6E0124E32EDA8E3, 0xC45BFBF985540DB8, 0x20F9DBCC42DED8C7, 0x47814256F39A4658,
                 0x20DCFE42BCB14929, 0xE38ADFBDC8AABA12, 0xCE488F3A3480BA0D, 0x669AA0A29E8FBA7C),
      WMPUInt<8>(0x87014F5F7986E0F5, 0x4C13AB920ADF86F3, 0xEAEC363831EF859D, 0xD012AD6AD0766D3E,
                 0x849098D9F6E9E379, 0x99A456E8A46CF927, 0xD5756ECF52FA0945, 0x7A595501987485DA),
     "79C1581C8F90008BD6D36DC7872EE4590578F0BA8984F782E5C854D8D13336B756CEE6241A97D0BFAAAB59A1FBEEB271BAECFA1A8EEB230A18C22CB6BAE2ADA69C38D13C42DB5CB3EE1C567BB19D9A1BFFE202483F32541D35CFA8D8CA54FD19D584CFA9512792224008DD51573D72394185EED58EB390B4518D5D31D2773998"},
     {WMPUInt<8>(0x54440BC1354AE014, 0x979DAD1D15E065DD, 0xD37E09F9234FD36F, 0x778F38E1B1FF715C,
                 0x443D82E64256A243, 0xCEB84E9FD0A49A60, 0x20BF8789B57F6A91, 0x5E2332EFBDFA86EB),
      WMPUInt<8>(0x05017BB4EB9C21B1, 0x1FBFA8B6C8CD6444, 0x2969D7638335EB59, 0x6F51C81FE6160790,
                 0xB111FE1560733B30, 0x16010E086DB16FEB, 0xFCB527B00AAA9DE5, 0x9E7078912213F6EF),
     "1A5D13710D79F785205B0D48BC1A09D5984CB0C783F0FD13BC51FC676DB636F248DA63F7E8CE14809437D0239208EDFA5872B9E2533CC905C3C63708F212703E0F0F5D2545E708A14EA77761416678FEC65377F7932D2F9AE8BAAF197EBA2B8FA967243E029565B787EF9B133C25195527EC40C87819E7C6FC9E0FF53FAC765"},
     {WMPUInt<8>(0x5F0564BEA972C16E, 0x3C96A8EA4778734A, 0x28B01E6AE9968FB3, 0x0970867931D700AE,
                 0x1974EDE07597749A, 0xAF16F2F8D8527448, 0xF3BE7DB0FE807F1D, 0xC97FAE4BA2516408),
      WMPUInt<8>(0x3C5C9FE803F69AF3, 0x5D2FBE764A80FA7F, 0x5CED7949A12AB4A1, 0xEF23EA8441CF5C53,
                 0xFFB5A3079C5F3418, 0x3373D7F543F1AB0D, 0x8D84012AFC9AA746, 0xB287A6F25E5ACDF8),
     "1667A4E75C2A1B7BCE770700CECB96A2761BC8BF71B9AFC440FEA164E2F8D28092A6B209996C19F8CA708BE7E535B93961BF7DED4963785E7285D44F6E52E1E8B5B0EE7ED2FF560F1F4BF63971CA1909926244CA555657808CC0DBF398E7A00B0441CE8DAD1406A16119BCECB4DAC6F41F55DFC360854983067A1D2886C34FC0"},
     {WMPUInt<8>(0xB12E62AD92C5C858, 0x160BA915DAD28CFD, 0x5BBC7CABA21D33D8, 0x494D103734A77824,
                 0x18255BB6DF0816AE, 0xF19033851EBE5FE6, 0x78E17B204F7AEBEB, 0x850F284430FE678C),
      WMPUInt<8>(0x26EE6D3CB34A81B6, 0x632EE4D4BC19B0DF, 0x1953EC9AB2137FDB, 0x2A50696FB641F177,
                 0x957CEA110B597FA5, 0x793421CCE815A391, 0xA63FC9D153A0E937, 0x8B60C6BEAC47689D),
     "1AF1E7604421D7F870F86B757D66FB2F7D7132790A589C99B3138713ADB90D912656A7E3A83857D90EE37C97ED25DF165D3D098C4272E7C95BDBF9C3245AC3160A6863EDCBF4D4EB235627806847E33C1AF7860E07FCE1C904E837675E887E622DB59A351A2EA4D494802698FE39BBC7ABCAEE1422EA40F5B7E30BC22DEA60DC"},
     {WMPUInt<8>(0x50E950BF550008C9, 0xEE8F8CC3947159F3, 0xFD6A98F28BBB7644, 0xC6F2749A4BB51969,
                 0x39A5526D5EFCFF0C, 0xCA7BDF2115382298, 0x0B413882D3211CB8, 0x6263FA8C0BEA8F5C),
      WMPUInt<8>(0xAB7FDBC9E7855211, 0x4D14340420C0BBEC, 0x524F3C7421A01CBE, 0x1D1D01FA782049B9,
                 0xB0E18869CB9F19A3, 0x54874F9FBCDB4904, 0xCC7BF30B09474360, 0xC863F341B0AF5D27),
     "363442264522EFD7CFE548B49236C8A9E9F85FBCA1FF884B33CE6567D1B75A3A35950A656991DD382AB1B6485EA6A035F92D5A1FA80D116B6D09B67F210E36AD716E2841BA98B912910EFB61FBF7DC89C4152271DD06E7678C4FA734715139B3A88A58614758C7288D7399BF4D36BAB10093584EA0D69981DA642DED46B44304"},
     {WMPUInt<8>(0x01D665F66CDA5235, 0xC07BF1C4ECC72D97, 0x6651FE6D9C070264, 0x6BA466B0F037BBB9,
                 0x34A910DB522EE087, 0xB3449A3FC21CAED6, 0xC6BB2898CDBCFC4F, 0x242C99385D529E6E),
      WMPUInt<8>(0x2B8ADBC84B883420, 0xBFA2B8A86F0E5948, 0xC58205B05106C9EE, 0xE9E0656B7C8AF3D6,
                 0x8F450E57FA5B2936, 0x201A9DEF8B5FF73C, 0xC1AC030578C85F28, 0x4FC1FD491B58C2BA),
     "50024734639B484CE38222E07511F2B6D32D2EC737749F94FA34BAC07B63A7EC7C4ED1037BF572FFAB61B5000C704932C368D4B42CAE21B69DAC39D95C3E600B426965B94D56B18678BC37417325475C1CE7B1AB2110BA961FB95112CA6F13F2BFD258F497E87BA5E724372F2FB9E2DD070520A69B411257CBB82679E677EC"},
     {WMPUInt<8>(0xB9A5218A37989013, 0x5EC9DAD095E0C9D7, 0x0122CFDEBB949CA1, 0x185A08FD16A48C38,
                 0xBED384725004BFA0, 0x9A529A58D8041F8F, 0xD0CF38D879664459, 0x334E4F0B4BFC5324),
      WMPUInt<8>(0xD51073BDB01BC6A0, 0x0E38BF096BCA4CD2, 0x2126777FD6FCF663, 0x5F5DBE31E464A788,
                 0xF5EFD6C6581C1181, 0x92E631EC270A4E85, 0x973E5BE7D4B4B948, 0x861B668CDD92ABF3),
     "9A825328D595EEF10CB1BD941253FA11FA7BB9A3933C2D2CDB49C20B0C851849E58098AFDEFEFFE89E43575AB19A4748A5024EE8A8020BA307C1E9FB79FB84BB96B87EF2CF5F76BF8D623447D4E268C060D87B9070BC8CD8B241B3678DC2890691451ADD74578D7A722795D90522054D8A52FE9906898A8C8B158CD82A93F72C"},
     {WMPUInt<8>(0x181B6D7286D0D2D6, 0x47040B205B7E501E, 0xEA515325015DD8DE, 0x91B0EBE4D2358307,
                 0x1AA8CA776527DC04, 0x6A131C44CD7D4030, 0x9D132341A097E11B, 0x13038D24C91C1BB8),
      WMPUInt<8>(0x5E0B18C0F57393B1, 0x2FE29C88085C779F, 0x82771CEBB026F9AB, 0x6D96130D1550FFC3,
                 0xC6800A793B624D9A, 0x03E72888D1B8CD61, 0x859ED270DADB4245, 0x0E5F5C4663804FA5),
     "8DB1DB27F5E6CE5A274F9DB13A421AFAB1587B758039C7A8A4ACFB7FB3972C447CDCB92A191FA7A5F82A69F910659357B5B33FF9AD7FA165F6673C0D4DA8E672927361B36432FC42DD77FFEB2F01D32D89EF176AB9CC64FB719FC49772B56C4F2813815DED20874C6FC5037DD91E14D84511C091DB8368C96F073DB4FABA598"},
     {WMPUInt<8>(0x0D0BF43F6F02BDA2, 0xB274450291AFC6BD, 0x1D2F853377C8C0A9, 0x103DDB4E6A9E870B,
                 0x5B2A6A294780A151, 0x87F020F9A18EA0C6, 0x32D692E98C57986C, 0xA30350FCED35CAE5),
      WMPUInt<8>(0x4B0E5C935AECA544, 0x871BFD3BC6C2997A, 0x3C02A104EB975197, 0x7D6A5BFD0231F46E,
                 0x94B480287420D9AF, 0x3037592D6CE73FDE, 0x506870A913AD745E, 0xB88364B9C3E3F904),
     "3D33BEDBF442F566DE4FAB22090EFC3952BCF952FD1D1FCEF88E41CDE8B26341A46DE3A540B422B8A9F660192744D2F4748EFE0F0CFF174DEB3745D4F2BFA0A1E2AF4B44EC1187E0852584A7BA0005E5196725EBAA1CB022D005AF3DF030A336071F89243D5E8A88190151B505F304B5D97C7F73785C2C41919AA695F3EE894"},
     {WMPUInt<8>(0x6535C326D4CDEF6D, 0x9712F15BD6EAD11F, 0x94566BC2B2C8140A, 0xF00B5343F391BFAD,
                 0xBD8B753122583531, 0x3E028E4B3CFDCC25, 0x84FF7DCA69F0D2A0, 0x1D279B16A091F0E5),
      WMPUInt<8>(0x353A1AC9C8C923FC, 0xA23404A66D87F21B, 0x23DD280FCB69E6C8, 0x1E5071B815BB859B,
                 0xF6808D50D5B86C59, 0x974889CBFEE2E71F, 0xF8DDA12087E7A74D, 0xCD660F459F7BA9D7),
     "150B1A2C7D448A3AB9963CF753FA8E6841AEDC065CE81C10574AA8BE55562BD5B436D5BC70013E71C988FFB1592D77E536666D4E4AE2D8A3B5DC002DFAB9161DEB51C410203177DDD0A25E30BAE7A8708AB4B5D9DEF804DDE9E31776DA5E8E3CA398A26F3410F74D007745D801BC0B2C75AB9E325260038263F053772B9F7D53"},
     {WMPUInt<8>(0xE14A2F6321D6AC1C, 0x17A624472B39BD28, 0x281FBF55DF49791A, 0xFAE07C68717DDD12,
                 0x20CB3B22B517CF32, 0x51F07DE4484AEEBC, 0xE8ABFDA166100827, 0x7989E8A12BF4DE0E),
      WMPUInt<8>(0x33717F017E6C40AB, 0xCF98B70D80938218, 0x858FCB6914F387F4, 0x8EE9E1160CAC113B,
                 0x4EA6B1F336159104, 0x5EC46BE287B4D9EB, 0x917A61386EF63E81, 0xBA87A68DCDA7DB4A),
     "2D45A8F4C88DD8C0B9E6E5D09E999E261544372DBB6F329B0A0CB13A3A3648AB8B5E045C8302EE6ADC7B6AB9606028E5FD3DE16F60A3554119A92C2E98C607015C0A7BBF73AED89D9BFE7F4DE055FE3D6DFCCD77DEEDE5CAEBA3DAA63B16032306570D4E9DD9A0BEDB0E35C23CFBF87144EAC6E4240ADC6D307343753FE02A0C"},
     {WMPUInt<8>(0xA79E987F841B2829, 0x4FAB483A43EBCC19, 0xC119CA58044B3BA4, 0xE9456C6834A38AA2,
                 0xFFA623543553F257, 0xE4B1141B650F19E9, 0xC7959561C89BBC34, 0xB187FE37DB073727),
      WMPUInt<8>(0x5BE0775BB4327D71, 0xA6E272975A842C2D, 0x00157B1BD7C12EF3, 0xE8B7A3D8A6865EA8,
                 0x8F1C4E54BE9E2F37, 0x09FE0DEBD90DBFC5, 0x5963EFB5DE3C5CFB, 0x890783F9A29212DF),
     "3C28592187C94D1CB551A8865A3A2AE7D03FFB9AEDB2989A336604D87470D24690FCE394885311DD1F65C25589C3AFFD6580CDA8BF2948418B9E6CED04DD1ACC013EFD05E2D2DDFBE88B2C23ACD0447D4DEEA76BAE0981F4E3607C023AAFED0323F8E90E852BF2E3BE78FE7E5D1E4D010C26F0A2946DBF2CA61A75016F67C8F9"},
     {WMPUInt<8>(0xDCB368DE3E82CFD6, 0x68E2968C02CC0007, 0xF49E47EE2CFE9A03, 0x7E2F6AE7D8630B1B,
                 0xD75DF6BAC4DA2C92, 0x042CA66F599C60D0, 0xBEFC973EE4FC3511, 0x2B422DA57A52BDB0),
      WMPUInt<8>(0x41432A6249C816FD, 0xC7F4588361AC0346, 0xF8BBFFBD366AD6E6, 0x67ACAF3B028B8A51,
                 0x32DB128DE736DD7E, 0xF49CD8671601B161, 0xD51A39F4F7A79053, 0x41E8A8892C1B34D0),
     "3843751F0B87D73ABC13B71D75D41CDFE95895E3A0C93479DA747FDC027F273A7C9FA2059F37359C2B89466E0A183FC71ED1DB7AFF6154DB64C5C666E7870601AA33AAEB24F65666EA72F7ED95D29AE3B749BBB3425385D9AB285DBFE9A488E3402CFB764E5EF03CD2604C7FAAB480FD275B53AD8BACED9A32C7D5D07351DF00"},
     {WMPUInt<8>(0x9BB87FD41E8CE4D8, 0x41C85144A987608B, 0xE69A68B5CEFD6EDC, 0x8865221E34205416,
                 0x7CB3D6EF53F4DDCA, 0xCA5289C2BA74D109, 0x98C7FB6E04B6391D, 0xE0C6744632565D91),
      WMPUInt<8>(0x94D7C0C0D2F900CC, 0x3FDA13498170A803, 0x1402CB9824C26E8D, 0x1A51AB27D56E3E22,
                 0x5C160492828DE65E, 0x66B5E497A59C32B8, 0xDF16BE6D231FACD5, 0x98C9BE4AD5AFC089),
     "5A89E719A762E26E613317D9506E235F7628C793A110F9C970505EBE31F79893F5E4A32C4093E77B427D888DE6E1CD587C0FD4F28CAE2957FBD2120BF77FA4F3308E15E5CA20D46A6DE74D93DD8AFACAFFB50974E74F40E52B9A28DCCF51656CD271622469366FDFE6E6E7849C4ACF1D45022C0864BF88A33A88CE1F5183D299"},
     {WMPUInt<8>(0xBDAACCA9EE8C6665, 0x0CE82EE89B7B3054, 0x6D9FEECBF0C0D19B, 0x6CAB2D2435EDCBFB,
                 0x4C2131EF5084A9D5, 0xECA332F50C502C6B, 0x65088D126A832048, 0x93AE6749980520B6),
      WMPUInt<8>(0xAF66F2F5CE692D74, 0xAAFE1DFC692BBA24, 0x8C479FB9A9668B65, 0xE16E2EA324E64BFA,
                 0x76929B325E8ADF66, 0x9C7781471D60477F, 0x967DB05C3CFA4EC5, 0xCD089AC0E5A4933E),
     "81F407F748A2E57E0AF580FDE33750E3F9CEB86D12BE633B4D73F6038C444244FF6C2376187B96568FF893CAC470057889290DDBB76161F31C2ACC333D8DE0B97820B890C8BBE1410ED507FBECDCD0724A157CC1989AF849316EF189A4A99BDEB072C2DB5D0EEAA26F3052625309B7D2C9D2DCF7AD83548DC6E44429859E6E14"},
     {WMPUInt<8>(0x07C65D4E30D46B13, 0x511BD0F705168E53, 0xAA4894BE1CF68F83, 0x76CB49A58D2B5FC5,
                 0xA9497C7B1388550A, 0x8CA2A131BAAF6CB9, 0x8101AD801CC946D6, 0x9690F64CE9A11DD7),
      WMPUInt<8>(0xFA18FD81EE9C5B1F, 0x8CEF36E0EEBABE85, 0x920D83A320E207D8, 0xE73E254E30B1580C,
                 0x0D67CA853859F15F, 0x7B56B0536BCC238A, 0xDA14EC273EFF4B63, 0x174AB84D251C61A7),
     "798796A176E79877563E4445AD3063E6DDC2394DF400D5568F23BA4668684AEC5A87739EADB79EAB81A65AEFBE296104F67624D157A126E5EB27B936E1F97E6EF96BA8177C37706732BCD44472B3F90DD784DB64AA9A03D01AB6A8BA1BBCD59C0C5CB8B4629FFB9317567D32E8FF20E9148522FBC6FD6F13820294CCAECEE41"},
     {WMPUInt<8>(0x9961027C1572F8C4, 0xC9EBAE91C311A9B7, 0x1C9E12E38C83B887, 0x7F202E78966AFFDF,
                 0x05ACEBFBB4A1BC53, 0xDED96A4850163589, 0x318817C20B692FDD, 0x817C8FCB73492E68),
      WMPUInt<8>(0x10532828C6CCAC47, 0x9BBFBE7001B015EE, 0x8117AB03C4547B8D, 0x94173650B749FFA6,
                 0x38DFD655DFE73E5A, 0x776466BB0D3F3EBD, 0x46B532B1C6C656C5, 0xF96A118875EE3577),
     "9C7E2AB263993024D9D8173CF6E7A1C6C41BB66167A13466C4D287AC5F2EE595561B52AEBB2EDC4C573C7A9A2DD17C7B6FC51363A51443315839F01242C1B17E1EA5274A4CC4AE8B493C69E865D17E3957CEDB011884F062B463ECA835C27D34A7198A4EF6E51A58D39E3C360D8F1459964F9A1EF5AE2811F8725EF6A501A58"},
     {WMPUInt<8>(0xECA65F1D7B9E4F16, 0x15F4AC1E43CDA6B0, 0xA6B46CFF48422C66, 0xE048B98D0C121CCA,
                 0x23462F6C6ECD2AC2, 0x3F376C52461FC7B5, 0x750C82BD85DDFC03, 0x9E7273476C0D5F83),
      WMPUInt<8>(0x4371FD826B1ACD66, 0x36F4F99412E71DCA, 0x0B8B24AF5AE42010, 0xD4CB01245CC14B9D,
                 0xE0C109257DA78525, 0xA0F42E6B1A5D7B92, 0x4E0AF223B66FAEB6, 0x9FD7914AB8F52578),
     "3E58EAADAEDCC03E9DF77F2EB7A0FC274BA6462E42C8673339148BC6193CAA6DBB473A12793C68780877DF0F030FA107B54E41F9DEE0B772F49BC433B35C78031015740ACF75F83791C2EF4F22AD7F33512413EB183791D6EEC7A33B59B8839E8D4EFBEAA9B1BB495777E517834BF8929A75261BE0865384DC4F30692571B468"},
     {WMPUInt<8>(0x395D7D272568665E, 0x517B7E8D5F3139D5, 0x1771897F7A0B1992, 0xAF5DC1991D77CB73,
                 0xCDEE0F35E1E856FF, 0x073DB4178AE86E6F, 0x27F95B8177FEF2C0, 0x742C3BF33CC73E2D),
      WMPUInt<8>(0xCE86D1271CAB8EBA, 0x12634C415EEEE071, 0xD180659F77172EAD, 0x3D5B6144BBEAAC57,
                 0xC7EA5266175795B2, 0x33A0C4884EBD82AC, 0x05ADDACCA8112592, 0x855066F6B21CF89D),
     "2E477083196F48A4D81F7F889336D364E0D6A38B45061580434EF90666B3FFD1E0590BD15DFF1A422E14C94EB64FD89B68ABDF96D8567E37A87529F8804572F969A84AF5AB24BBE7B2C696EC48CD3A2C86F76DDF67224992ADBA9DB87AA054115944F38807768BB597253F6D2F632150186F2BE96BEEC546615D43516158B999"},
     {WMPUInt<8>(0x8001A5FD0921F9C9, 0x02A4A5F4562BECF2, 0xA98B1688B0F04E23, 0x21EFD51D977BA460,
                 0x404B33B52F7E205B, 0xB6F9DA12DFFE386A, 0x5050659CC5617577, 0x8EDEC78C360D94F7),
      WMPUInt<8>(0x3A29AE9D9E4B7995, 0x3B90082EA226B547, 0xE342C303669F0353, 0x77AE94F08A6C335B,
                 0xD47577613DC5897A, 0x826A0ADABC198A45, 0x5DA252E4A7DFC9B2, 0xBFF823E7FEE3F142),
     "1D15372ED893B325705ECB0622C97C66D1AE966B6E99FF4B76462B1BA0F489DD79FAC1EFFC10B276D5E32F5472B930C474768709400202BA47C29FDA56BABF2D4B7DC33B29A8AAE7B22F08B78EE21CBB52D62EC6523FA8DA55B5D4DE6DA59195C3404106601B825D955CB4F22B4D10A480BF083CDB81CCCDD82708C1E1C1EEAE"},
     {WMPUInt<8>(0x616A10F385CC0F88, 0x8ADC227391BEB554, 0x398613B8F4F4A4EA, 0x3C691DBF38E5BB68,
                 0x7C89AE196D35BAEF, 0x06177798DEDE4079, 0x1E53A5DBB25BC98D, 0x757F6D1EAABA9572),
      WMPUInt<8>(0xA812E99ABFE84BA6, 0xA40CCA43E0D339C0, 0x96F75E181C656380, 0xA3368E19812A4AB7,
                 0x3A140637B7BE3926, 0xFE62574BC03B430E, 0x0F5D9664C81F1999, 0x011A082EEF59AC2E),
     "3FF4CD796D2FB15E734E8008F8953E0E7152B821DDDB94194C2ACCE65B8E1D0BD2583B6AF57DDC86214780FDEC2E2245E0513AE61A68F221208CEEBBA96BB9ED980147C9FCFEF1006C0C1A3AE713EF490EE279C0F8916F03706CC7F145495225CD064F894C9E2FF9D240697438CF59B62FC001BA91C9280C6D0A2D176C91727C"},
     {WMPUInt<8>(0x7F2AD107F8DDBA2A, 0x2E197DF35C7E33D0, 0x3342BDC7261F4934, 0x545E592429A921A0,
                 0x9E38EED1159F80E7, 0x32BD802173987166, 0x18152C657FA3FDC9, 0x381AC4340644F60A),
      WMPUInt<8>(0xB06A021391C6263F, 0x83D7D646BE018650, 0xD44E6328654B039C, 0x881702698D3039A6,
                 0x93ABF4A44131949D, 0xF2581AE849F879C4, 0x3D44C76ABA60A811, 0xCE1F8EEF00905867),
     "57A21878169EBDD8614094D7AF0A4EAA751012AA525EDF981882E35807BEAD79DC99038BCDF44C2AB82D04FB8FD5C06B79D842CE4EE0DBE68F367DF42DDEE3467949AED7318C3259FB2B39DD0FECB9F8C6BB519309874361B398EE51EFCDF4296F7454C3C9736FD6CBBF69E9967CB3B05353B464885D0559158B99369FF26E06"},
     {WMPUInt<8>(0xD408DE8798EDF4C9, 0x5948D49B54687E6B, 0x2E19C96C7EEEDBB5, 0x9E13DF7704F263C4,
                 0xA00178BB1A246B2A, 0x8F577B49109FA213, 0xE3FDA5C99BA516EF, 0xF8C83BAAEDCE1B21),
      WMPUInt<8>(0x09C9D2514F4C6672, 0x707BC41893479754, 0xD62FF90A17D2F0A7, 0x338D964BACBD1567,
                 0x4B2F9AABB1BF56D3, 0x6CE64E4F27593A28, 0x6F5ED950C2FB46CD, 0x1F013F7795878D75),
     "81B78FC1BDF18F3168CA1247D728026955CF64F01FC514B3E409AE892020F8CCA707B757C53B75A0D2270225210F84FBF7042CE6382D847FB07812921235E34630D2B440E5B00571F27E1C6B23A423E40B0127589518C73598AFD83D0779BFFE7DF5CA15360854718EE018EF5E3C0AE18933BE91379D3834D8982EAB78A9315"},
     {WMPUInt<8>(0xDE9D15D1C2E1C933, 0x8B7BB05EF9B36624, 0xCC6FC44CAD352A48, 0x8158FBD46DF064EF,
                 0xD011E08D9B6793A0, 0x3A0109F5EABFA0D7, 0x7C2C55F6DC851EC6, 0xCC36D694CB80EA65),
      WMPUInt<8>(0x2452E607628E3FC9, 0x42DD5C29BD374369, 0x31FFC4530976E732, 0xCBB7982155BA1E91,
                 0x661099B534F5C31C, 0x064A093E98D676E6, 0xDB016462C1B23BD0, 0xA6898CBFE3D63FCD),
     "1F962D6A0A320F64F3DF28715E051895847D06CECFFD1277E6A9ABA64099EE2D3EBD98CDBB41D151270914F1ED77633B2FE5E1EEFA9879A5678F78793A07E04166524C1932311014981DF5E168EC732D67E8A8EB44EE14374CDE283A27D9AA97C18A86680E3238CAD804A05AA7AF101654B701D2344798E99AF8FE322F588DE1"},
     {WMPUInt<8>(0xD00943C0D75A8642, 0xC116FEAE7B04B894, 0xC5C9D8290E0D7659, 0x1544DAD8340F92E3,
                 0x8EB55E64E301853E, 0xF06F431AF28642FE, 0xDBAE5E167D68F32C, 0x69C5A706E980AF6C),
      WMPUInt<8>(0x943ED2620BE0F7B4, 0x19A342DAD554652E, 0x2A8D8D1B50DE029C, 0xCB153694CE6622F7,
                 0xD4AAD10BB2BE0625, 0xCAFE81A1C85AD94A, 0xC6EFBC31EC1E4A58, 0xAB1A8F1CDEC89506),
     "787868612BFBD555AC4BD0F3A2878FFC8EF2138B26716962D3A95CB0CF6B4FB8F8F9F29F8816345B4A3E6DBA693B9EBDA0A2ED6EB6E1C81FABC20223916168096CD748F6D6E9BC270497D539584CF8681B7E09D731D6AF80F96FFE61FB0FD108551E697B352CED46162437685FED4C134A3AC150A89BE86C61C5788A137DF888"},
     {WMPUInt<8>(0xC832465983EEE93F, 0x44EE962A18AB4006, 0x9D54596C7295C71F, 0x80F7C94CB2B48C46,
                 0xFC8E664247F1387F, 0x2CEA8E49FFAE300D, 0x32C9CCCCD6DFD23A, 0x47F7BDB6D73CBED4),
      WMPUInt<8>(0x19455B1D84B65E8D, 0x46D2EEFB41E91DF6, 0xE07DF83693C0D21C, 0xD9DFC1443A4360A7,
                 0x6029071A9F316C5E, 0x6B338BD78678328A, 0xD350AFDD2158241F, 0x907E249E1D59BA3C),
     "13C325ACA87F382B075617DF74A9E4643A2687E33EC4F640EF164D4C045F0BDDAAD8374EB9F978AE49A316C1EF0CA293B6C9A9FF38A401859955A6C1F85EFE1D0C82FC0033BDBF827CD3939C980B8A377EF1356A8A358E5BBAB549564942A22F96AC37F576B6FAF9ED0521497B94EDBA50EFC566AE53879817859BD0F096C1B0"},
     {WMPUInt<8>(0x2612FD2EBEE5339E, 0x6DE203C61C0D3F5E, 0x7F9AA20E88E99B8D, 0x7164159E7AB19146,
                 0x2746EFF1B1C1E1E6, 0x660E49FF9DC47ED8, 0xF4B9E04E02949223, 0x762BC665D2D40655),
      WMPUInt<8>(0x50C104B3A7A2D41A, 0x7678F589D0F5486D, 0x288699D9339EF00F, 0x3A5F59D818F3C968,
                 0xD6B1BE6CA79D4608, 0x74F5A63149D1B1BF, 0x504DEC1ECDD47B16, 0xD7C295AFC52A1C17),
     "C02A422800FE851DB2C86BC399FB4EBAC3FEE427703FD7FFF0F8966B244CCD7E9E06804A216DC12C709CDB018AB75F8BAEA0EB1FDE03A407CE4C8B47F1D0F6788436EEBD369125DE42CEAB6F211C5F84E2BDC1B8E7CA0BC25847FC811A0DF9C9E4606CAFAD807EAD9137E3C6DD393EC923DBE9A2B5D3094059D82F894AFDDA3"},
     {WMPUInt<8>(0xAB1BF7646B530A67, 0x4D2046F6F75BE8BF, 0xF6BB5B7C9EA653CE, 0xF30ED07C3B17D1B3,
                 0xF6DC1F405B6BD912, 0xEE01B35A96A1F6C5, 0x0156C6B916E4EF03, 0x4AD363F901DB8C4F),
      WMPUInt<8>(0xB8BD7262C9FFC05A, 0x02F38B61B74C8B8C, 0xEC38BDB33DDF8C86, 0x9D534BA6B1AE1D13,
                 0x7EDD03C687FC6E50, 0xD95C8CE3EBB19679, 0x6D16571929349130, 0xC0E3B4B543373C81),
     "7B7AB9EA4E2A3EAF66C3850C64201085637E5F8B59C0587395E763E5808017267948B2D75CC3C888F1A9F7E8548CF4B4D2F44F42B54EB596895EAA09D969D1B1697DBBEF8E57FFBACB35B2FA2AC0C77CA608A8BAAD11F79572B35FA53857EA93FC4EDC0C7048CA64F27986EB47DE884E26116C42334CBDDC3EDE58A8367D37CF"},
    };

    for(const auto& data: values2)
        if(!checkFullMultiplication(data.v1, data.v2, data.resultStr)) DRET;

    for(const auto& data: values8)
        if(!checkFullMultiplication(data.v1, data.v2, data.resultStr)) DRET;

    return true;
}

static bool testMultiplication()
{
    std::cout << "Testing multiplication" << std::endl;
    if(!testMultiplicationWithSize2()) DRET;

    for(unsigned ind1 = 0; ind1 < 8; ++ind1)
        for(unsigned ind2 = 0; ind2 < 8; ++ind2)
            if(!testMultiplicationWithPatterns<3>(ind1, ind2)) DRET;

    for(unsigned ind1 = 0; ind1 < 16; ++ind1)
        for(unsigned ind2 = 0; ind2 < 16; ++ind2)
            if(!testMultiplicationWithPatterns<4>(ind1, ind2)) DRET;

    for(unsigned ind1 = 0; ind1 < 32; ++ind1)
        for(unsigned ind2 = 0; ind2 < 32; ++ind2)
            if(!testMultiplicationWithPatterns<5>(ind1, ind2)) DRET;

    for(unsigned ind1 = 0; ind1 < 64; ++ind1)
        for(unsigned ind2 = 0; ind2 < 64; ++ind2)
            if(!testMultiplicationWithPatterns<6>(ind1, ind2)) DRET;

    if(!testMultiplicationWithAddition<2>()) DRET;
    if(!testMultiplicationWithAddition<3>()) DRET;
    if(!testMultiplicationWithAddition<4>()) DRET;
    if(!testMultiplicationWithAddition<5>()) DRET;
    if(!testMultiplicationWithAddition<6>()) DRET;
    if(!testMultiplicationWithAddition<7>()) DRET;
    if(!testMultiplicationWithAddition<8>()) DRET;
    if(!testMultiplicationWithAddition<9>()) DRET;

    const WMPUInt<3> values3[][3] =
    {{ WMPUInt<3>(0xABCDEF0123456789, 0x1234567890ABCDEF, 0xAABBCCDDEEFF0011),
       WMPUInt<3>(0x8ABFC0F2BF8AF59E, 0x2E0191B18E8BA3AB, 0x9856577ABDDE0121),
       WMPUInt<3>(0xA61B6ABF0B698C4D, 0x0E9068C41A05CEB6, 0xEC2B99D3699D1331) },
     { WMPUInt<3>(0xFF12FF23FF34FF45, 0xF0F1F2F3F4F5F6F7, 0xAFBFCFDFEFFFAFBF),
       WMPUInt<3>(0x0DA9199103B18B96, 0x11B8D85103D1D264, 0xDFAC7A483928B081),
       WMPUInt<3>(0xD7975D393A1FEF33, 0xD78B72B8101364BD, 0x94FD96A4B582DF3F) },
     { WMPUInt<3>(0xABCDEF0123456789, 0x1234567890ABCDEF, 0xAABBCCDDEEFF0011),
       WMPUInt<3>(0xFF12FF23FF34FF45, 0xF0F1F2F3F4F5F6F7, 0xAFBFCFDFEFFFAFBF),
       WMPUInt<3>(0x153C82E80E5ECD21, 0x2E6437444EA8D222, 0x13387ED5913BABAF) },
     { WMPUInt<3>(0x5AAB3001022EFFED, 0x10598294EF43D4F2, 0x113253907ED64ED7),
       WMPUInt<3>(0x92384923EDE34831, 0xFFDC33442341223D, 0x344FDCAA231DEA42),
       WMPUInt<3>(0x855815C047FD3B6F, 0x4DC68681CF9EAC96, 0x0E7B6904EAABD96E) },
     { WMPUInt<3>(0x9999999999999999, 0x9999999977777777, 0x7777777777777777),
       WMPUInt<3>(0x5555555555555555, 0x5555555555558333, 0x3333333333333333),
       WMPUInt<3>(0xCCCCC6AF37C048D1, 0x59E26AF382D8181B, 0x4E81B4E81B4E81B5) },
     { WMPUInt<3>(0x1002003004005006, 0x00700800900A00B0, 0x0C00D00E00F00111),
       WMPUInt<3>(0xFAF411223344FF99, 0x887766CCDDEEEE12, 0x3234345456567678),
       WMPUInt<3>(0xDC96140FEFC578AC, 0x8E32ACB47667D529, 0x31B8B98F22B455F8) },
    };

    if(!checkMultiplication(values3[0][0], values3[0][0], values3[0][1])) DRET;
    if(!checkMultiplication(values3[1][0], values3[1][0], values3[1][1])) DRET;
    if(!testMultiplication(values3)) DRET;

    const WMPUInt<4> values4[][3] =
    {{WMPUInt<4>(0xABCDEF0123456789, 0x1234567890ABCDEF, 0xAABBCCDDEEFF0011),
      WMPUInt<4>(0x1B3D605E6CBA3E5E, 0x8ABFC0F2BF8AF59E, 0x2E0191B18E8BA3AB, 0x9856577ABDDE0121),
      WMPUInt<4>(0x9AA580298FD8E299, 0xA61B6ABF0B698C4D, 0x0E9068C41A05CEB6, 0xEC2B99D3699D1331)},
     {WMPUInt<4>(0x1B3D605E6CBA3E5E, 0x8ABFC0F2BF8AF59E, 0x2E0191B18E8BA3AB, 0x9856577ABDDE0121),
      WMPUInt<4>(0x9AA580298FD8E299, 0xA61B6ABF0B698C4D, 0x0E9068C41A05CEB6, 0xEC2B99D3699D1331),
      WMPUInt<4>(0xDA324AB303CDAF44, 0x2437C777DC1AD668, 0xAD17E2B86A13456A, 0x585E01660BD0AA51)},
     {WMPUInt<4>(0x1002003004005006, 0x00700800900A00B0, 0x0C00D00E00F00111),
      WMPUInt<4>(0xFAF411223344FF99, 0x887766CCDDEEEE12, 0x3234345456567678),
      WMPUInt<4>(0xC95DCA352E40EBB3, 0xDC96140FEFC578AC, 0x8E32ACB47667D529, 0x31B8B98F22B455F8)}
    };

    if(!checkMultiplication(values4[0][0], values4[0][0], values4[0][1])) DRET;
    if(!testMultiplication(values4)) DRET;

    const WMPUInt<8> values8[][3] =
    {{WMPUInt<8>(0x1B3D605E6CBA3E5E, 0x8ABFC0F2BF8AF59E, 0x2E0191B18E8BA3AB, 0x9856577ABDDE0121),
      WMPUInt<8>(0x9AA580298FD8E299, 0xA61B6ABF0B698C4D, 0x0E9068C41A05CEB6, 0xEC2B99D3699D1331),
      WMPUInt<8>(0x1074882B06961FC6, 0x32407BE674E25854, 0xB14779D0A9EFD93F, 0xEE369F6C299D7D60,
                 0xDA324AB303CDAF44, 0x2437C777DC1AD668, 0xAD17E2B86A13456A, 0x585E01660BD0AA51)},
     {WMPUInt<8>(0xC95DCA352E40EBB3, 0xDC96140FEFC578AC, 0x8E32ACB47667D529, 0x9AA580298FD8E299,
                 0xA61B6ABF0B698C4D, 0x0E9068C41A05CEB6, 0xD7975D393A1FEF33, 0xD78B72B8101364BD),
      WMPUInt<8>(0xDA324AB303CDAF44, 0x2437C777DC1AD668, 0xAD17E2B86A13456A, 0x585E01660BD0AA51,
                 0xCCCCC6AF37C048D1, 0x59E26AF382D8181B, 0x4E81B4E81B4E81B5, 0x8E32ACB47667D529),
      WMPUInt<8>(0x011DE507E6984287, 0x59AC020BAF1ECF50, 0x8487454A68B7222F, 0xEE97DA0F32576C88,
                 0x9AB82E568CDC9EFD, 0x9FD927ABFCA12D1B, 0xBD4D1BD388672224, 0x637BBDFB5BF76345)}
    };

    if(!testMultiplication(values8)) DRET;

    WMPUInt<8> vD, vF, vE;
    vD.assign({ 0x9AA580298FD8E299, 0xA61B6ABF0B698C4D, 0x0E9068C41A05CEB6, 0xEC2B99D3699D1331 });
    vF.assign({ 0x1B3D605E6CBA3E5E, 0x8ABFC0F2BF8AF59E, 0x2E0191B18E8BA3AB, 0x9856577ABDDE0121 });
    vE.assign({ 0x1074882B06961FC6, 0x32407BE674E25854, 0xB14779D0A9EFD93F, 0xEE369F6C299D7D60,
                0xDA324AB303CDAF44, 0x2437C777DC1AD668, 0xAD17E2B86A13456A, 0x585E01660BD0AA51 });
    if(!checkMultiplication(vD, vF, vE)) DRET;
    if(!checkMultiplication(vF, vD, vE)) DRET;

    WMPUInt<10> vG(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
                   0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);
    WMPUInt<10> vH(0, 0, 0, 0,
                   0xFFFFFFFFFFFFFFFE, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,
                   0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0000000000000001);
    std::uint64_t factor = UINT64_C(0xFFFFFFFFFFFFFFFF);
    if(!checkMultiplication(vG, factor, vH)) DRET;
    if(!checkMultiplication(factor, vG, vH)) DRET;

    WMPUInt<3> vI(0x8ABFC0F2BF8AF59E, 0x2E0191B18E8BA3AB, 0x9856577ABDDE0121);
    factor = UINT64_C(0xAABBCCDDEEFF0011);
    WMPUInt<3> vJ(0xA26487FEBC5FB714, 0x16F821BCBFD053E7, 0xEC2B99D3699D1331);
    if(!checkMultiplication(vI, factor, vJ)) DRET;

    return testMultiplicationAndStrInput();
}


//============================================================================
// Test modulo
//============================================================================
template<std::size_t kSize>
static bool testModuloForSize(std::mt19937_64& rng)
{
    WMPUInt<kSize> value;
    WMPUInt<kSize+1> product, result2;

    for(std::uint64_t divisor = 2; divisor < 500; ++divisor)
    {
        for(std::size_t i = 0; i < kSize; ++i) value.data()[i] = rng();
        value.fullMultiply(WMPUInt<1>(divisor), product);

        for(std::uint64_t counter = 0; counter < divisor*2; ++counter)
        {
            const std::uint64_t expectedResult = counter % divisor;
            const std::uint64_t result1 = product.modulo(divisor);

            if(result1 != expectedResult)
                return DPRINT("Error: WMPUInt<", kSize+1, ">::modulo(", divisor, ") of\n",
                              product, "\nreturned ", result1, " instead of ",
                              expectedResult, "\n");

            result2 = product % divisor;
            if(result2 != expectedResult)
                return DPRINT("Error: WMPUInt<", kSize+1, ">::operator%(", divisor, ") of\n",
                              product, "\nreturned ", result1, " instead of ",
                              expectedResult, "\n");

            result2 = product;
            result2 %= divisor;
            if(result2 != expectedResult)
                return DPRINT("Error: WMPUInt<", kSize+1, ">::operator%=(", divisor, ") of\n",
                              product, "\nreturned ", result1, " instead of ",
                              expectedResult, "\n");

            ++product;
        }
    }
    return true;
}

static bool testModulo()
{
    std::cout << "Testing modulo" << std::endl;
    if(!testModuloForSize<1>(gRngEngine)) DRET;
    if(!testModuloForSize<2>(gRngEngine)) DRET;
    if(!testModuloForSize<3>(gRngEngine)) DRET;
    if(!testModuloForSize<4>(gRngEngine)) DRET;
    if(!testModuloForSize<5>(gRngEngine)) DRET;
    if(!testModuloForSize<10>(gRngEngine)) DRET;
    return true;
}


//============================================================================
// Test negation
//============================================================================
template<std::size_t kSize>
static bool testNegationWithSize()
{
    WMPUInt<kSize> positive, negative, result;

    for(std::size_t i = 0; i < kSize-1; ++i)
    {
        positive.data()[i] = 0;
        negative.data()[i] = -1;
    }

    for(std::uint64_t value = 1; value < 100; ++value)
    {
        positive.data()[kSize-1] = value;
        negative.data()[kSize-1] = -value;

        result = positive;
        result.neg();
        if(result != negative) return DPRINT("Error: ", result, " instead of ", negative, "\n");
        result.neg();
        if(result != positive) return DPRINT("Error: ", result, " instead of ", positive, "\n");
        result = -positive;
        if(result != negative) return DPRINT("Error: ", result, " instead of ", negative, "\n");
        result = -negative;
        if(result != positive) return DPRINT("Error: ", result, " instead of ", positive, "\n");
    }

    return true;
}

static bool testNegation()
{
    std::cout << "Testing negation" << std::endl;
    if(!testNegationWithSize<2>()) DRET;
    if(!testNegationWithSize<3>()) DRET;
    if(!testNegationWithSize<4>()) DRET;
    if(!testNegationWithSize<5>()) DRET;
    if(!testNegationWithSize<6>()) DRET;
    if(!testNegationWithSize<7>()) DRET;
    if(!testNegationWithSize<8>()) DRET;
    if(!testNegationWithSize<100>()) DRET;
    return true;
}


//============================================================================
// Test shifting
//============================================================================
static std::uint64_t reverseValue(std::uint64_t value)
{
    value = ((value << 32) | (value >> 32));
    value = (((value & UINT64_C(0x0000FFFF0000FFFF)) << 16) |
             ((value & UINT64_C(0xFFFF0000FFFF0000)) >> 16));
    value = (((value & UINT64_C(0x00FF00FF00FF00FF)) << 8) |
             ((value & UINT64_C(0xFF00FF00FF00FF00)) >> 8));
    value = (((value & UINT64_C(0x0F0F0F0F0F0F0F0F)) << 4) |
             ((value & UINT64_C(0xF0F0F0F0F0F0F0F0)) >> 4));
    value = (((value & UINT64_C(0x3333333333333333)) << 2) |
             ((value & UINT64_C(0xCCCCCCCCCCCCCCCC)) >> 2));
    return (((value & UINT64_C(0x5555555555555555)) << 1) |
            ((value & UINT64_C(0xAAAAAAAAAAAAAAAA)) >> 1));
}

template<std::size_t kSize>
static void reverseWMPUInt(WMPUInt<kSize>& number)
{
    for(std::size_t i = 0; i < (kSize+1)/2; ++i)
    {
        std::uint64_t component = number.data()[i];
        number.data()[i] = reverseValue(number.data()[kSize-1-i]);
        number.data()[kSize-1-i] = reverseValue(component);
    }
}

template<std::size_t kSize>
static bool testShiftedValues(const char* namePrefix, std::size_t bits, const char* namePostfix,
                              const WMPUInt<kSize>& originalValue,
                              const WMPUInt<kSize>& shiftedValue,
                              const WMPUInt<kSize>& expectedResult)
{
    if(shiftedValue != expectedResult)
        return DPRINT("Error: WMPUInt<", kSize, ">::", namePrefix, bits, namePostfix, " of\n",
                      originalValue, "\nresulted in\n", shiftedValue, "\ninstead of\n",
                      expectedResult, "\n");
    return true;
}

template<std::size_t kSize, std::size_t kBits>
static bool testShiftWithBits(const WMPUInt<kSize>& value)
{
    WMPUInt<kSize> shiftedValue = value, multipliedValue = value;
    shiftedValue.template shiftLeft<kBits>();
    if constexpr(kBits < 64) multipliedValue *= (UINT64_C(1) << kBits);
    else
    {
        WMPUInt<kSize> factor(0);
        factor.data()[kSize-1 - kBits/64] = (UINT64_C(1) << (kBits%64));
        multipliedValue *= factor;
    }

    if(!testShiftedValues("shiftLeft<", kBits, ">()", value, shiftedValue, multipliedValue)) DRET;

    shiftedValue = value;
    reverseWMPUInt(shiftedValue);
    reverseWMPUInt(multipliedValue);
    shiftedValue.template shiftRight<kBits>();
    if(!testShiftedValues("shiftRight<", kBits, ">()", value, shiftedValue, multipliedValue)) DRET;

    shiftedValue = value;
    shiftedValue <<= kBits;
    reverseWMPUInt(multipliedValue);
    if(!testShiftedValues("operator<<=(", kBits, ")", value, shiftedValue, multipliedValue)) DRET;

    shiftedValue = value;
    reverseWMPUInt(shiftedValue);
    shiftedValue >>= kBits;
    reverseWMPUInt(multipliedValue);
    if(!testShiftedValues("operator>>=(", kBits, ")", value, shiftedValue, multipliedValue)) DRET;

    shiftedValue = value << kBits;
    reverseWMPUInt(multipliedValue);
    if(!testShiftedValues("operator<<(", kBits, ")", value, shiftedValue, multipliedValue)) DRET;

    shiftedValue = value;
    reverseWMPUInt(shiftedValue);
    shiftedValue = shiftedValue >> kBits;
    reverseWMPUInt(multipliedValue);
    if(!testShiftedValues("operator>>(", kBits, ")", value, shiftedValue, multipliedValue)) DRET;

    return true;
}

template<std::size_t kSize>
static bool testShiftWithSize(std::mt19937_64& rngEngine)
{
    WMPUInt<kSize> value;

    for(unsigned iteration = 0; iteration < 100; ++iteration)
    {
        for(std::size_t i = 0; i < kSize; ++i) value.data()[i] = rngEngine();

        if(!testShiftWithBits<kSize, 0>(value)) DRET;
        if(!testShiftWithBits<kSize, 1>(value)) DRET;
        if(!testShiftWithBits<kSize, 2>(value)) DRET;
        if(!testShiftWithBits<kSize, 3>(value)) DRET;
        if(!testShiftWithBits<kSize, 7>(value)) DRET;
        if(!testShiftWithBits<kSize, 11>(value)) DRET;
        if(!testShiftWithBits<kSize, 33>(value)) DRET;
        if(!testShiftWithBits<kSize, 63>(value)) DRET;
        if constexpr(kSize > 1)
        {
            if(!testShiftWithBits<kSize, 64>(value)) DRET;
            if(!testShiftWithBits<kSize, 65>(value)) DRET;
            if(!testShiftWithBits<kSize, 127>(value)) DRET;
        }
        if constexpr(kSize > 2)
        {
            if(!testShiftWithBits<kSize, 128>(value)) DRET;
            if(!testShiftWithBits<kSize, 128+1>(value)) DRET;
            if(!testShiftWithBits<kSize, 158>(value)) DRET;
            if(!testShiftWithBits<kSize, 128+63>(value)) DRET;
        }
        if constexpr(kSize > 3)
        {
            if(!testShiftWithBits<kSize, 192>(value)) DRET;
            if(!testShiftWithBits<kSize, 192+1>(value)) DRET;
            if(!testShiftWithBits<kSize, 221>(value)) DRET;
            if(!testShiftWithBits<kSize, 192+63>(value)) DRET;
        }
        if constexpr(kSize > 4)
        {
            if(!testShiftWithBits<kSize, 256>(value)) DRET;
            if(!testShiftWithBits<kSize, 256+1>(value)) DRET;
            if(!testShiftWithBits<kSize, 287>(value)) DRET;
            if(!testShiftWithBits<kSize, 256+63>(value)) DRET;
        }
    }
    return true;
}

static bool testShifting()
{
    std::cout << "Testing bitshift" << std::endl;
    if(!testShiftWithSize<1>(gRngEngine)) DRET;
    if(!testShiftWithSize<2>(gRngEngine)) DRET;
    if(!testShiftWithSize<3>(gRngEngine)) DRET;
    if(!testShiftWithSize<4>(gRngEngine)) DRET;
    if(!testShiftWithSize<10>(gRngEngine)) DRET;
    if(!testShiftWithSize<15>(gRngEngine)) DRET;
    if(!testShiftWithSize<100>(gRngEngine)) DRET;
    return true;
}


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

bool testFullMultiplication();

//============================================================================
// main()
//============================================================================
int main()
{
    if(!testSize1Operators()) DRETM;
    if(!testAssignmentFromStr()) DRETM;
    if(!testAddition()) DRETM;
    if(!testMultiplication()) DRETM;
    if(!testFullMultiplication()) DRETM;
    if(!testModulo()) DRETM;
    if(!testNegation()) DRETM;
    if(!testShifting()) DRETM;
    std::cout << "All tests OK.\n";
    //runBenchmarks();
}
