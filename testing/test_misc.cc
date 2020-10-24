#include "test.hh"
#include <random>

extern std::mt19937_64 gRngEngine;


//============================================================================
// Test WMPUInt<1>
//============================================================================
bool testSize1Operators()
{
    WMPUInt<1> a(20), b({ 10 });
    if(a + b != 30) return DPRINT(TType::s1Op, "Error\n");
    if(a + 30 != 50) return DPRINT(TType::s1Op, "Error\n");
    if(a - b != 10) return DPRINT(TType::s1Op, "Error\n");
    if(a - 5 != 15) return DPRINT(TType::s1Op, "Error\n");
    if(a * b != 200) return DPRINT(TType::s1Op, "Error\n");
    if(a * 5 != 100) return DPRINT(TType::s1Op, "Error\n");
    if((a & b) != 0) return DPRINT(TType::s1Op, "Error\n");
    if((a | b) != 30) return DPRINT(TType::s1Op, "Error\n");
    if((a ^ b) != 30) return DPRINT(TType::s1Op, "Error\n");
    return true;
}


//============================================================================
// Test comparison
//============================================================================
template<std::size_t kSize1, std::size_t kSize2>
static bool checkEqualValues(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2)
{
    if(!(v1 == v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 != v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 <= v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 < v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 >= v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 > v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    return true;
}

template<std::size_t kSize1, std::size_t kSize2>
static bool checkLessThanValue(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2)
{
    if(v1 == v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 != v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 <= v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 < v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 >= v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 > v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    return true;
}

template<std::size_t kSize1, std::size_t kSize2>
static bool checkGreaterThanValue(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2)
{
    if(v1 == v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 != v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 <= v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(v1 < v2) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 >= v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    if(!(v1 > v2)) return DPRINT(TType::comp, "Error comparing ", v1, " and ", v2, "\n");
    return true;
}

template<std::size_t kSize1, std::size_t kSize2>
static bool checkEquality(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2)
{
    if(!checkEqualValues(v1, v1)) DRET;
    if(!checkEqualValues(v2, v2)) DRET;
    if(!checkEqualValues(v1, v2)) DRET;
    if(!checkEqualValues(v2, v1)) DRET;
    return true;
}

template<std::size_t kSize1, std::size_t kSize2, std::size_t... kSizes>
static bool checkEquality(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2, const WMPUInt<kSizes>&... v3)
{
    if(!checkEquality(v1, v2)) DRET;
    if(!checkEquality(v1, v3...)) DRET;
    return checkEquality(v2, v3...);
}

template<std::size_t kSize1, std::size_t kSize2>
static bool checkLessThan(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2)
{
    if(!checkEqualValues(v1, v1)) DRET;
    if(!checkEqualValues(v2, v2)) DRET;
    if(!checkLessThanValue(v1, v2)) DRET;
    if(!checkGreaterThanValue(v2, v1)) DRET;
    return true;
}

template<std::size_t kSize1, std::size_t kSize2, std::size_t... kSizes>
static bool checkLessThan(const WMPUInt<kSize1>& v1, const WMPUInt<kSize2>& v2, const WMPUInt<kSizes>&... v3)
{
    if(!checkLessThan(v1, v2)) DRET;
    if(!checkLessThan(v1, v3...)) DRET;
    return checkLessThan(v2, v3...);
}

bool testComparison()
{
    std::cout << "Testing comparison operators" << std::endl;

    WMPUInt<1> a1(0), a2(1), a3(2);
    WMPUInt<2> b1(0, 0), b2(0, 1), b3(0, 2), b4(0, 3), b5(1, 0), b6(1, 1), b7(2, 0);
    WMPUInt<3> c1(0, 0, 0), c2(0, 0, 1), c3(0, 0, 2), c4(0, 0, 4), c5(0, 1, 0), c6(0, 1, 1),
        c7(1, 0, 0), c8(1, 0, 1), c9(1, 1, 0), c10(1, 1, 1), c11(2, 0, 0);
    WMPUInt<4> d1(0, 0, 0, 0), d2(0, 0, 0, 1), d3(0, 0, 1, 0), d4(0, 1, 0, 0),
        d5(0, 1, 0, 1), d6(1, 0, 0, 0), d7(0, 0, 1, 1), d8(1, 0, 0, 1);
    WMPUInt<5> e1(0, 0, 0, 0, 0), e2(0, 0, 0, 0, 1), e3(1, 0, 0, 0, 0);

    if(!checkEquality(a1, b1, c1, d1, e1)) DRET;
    if(!checkEquality(a2, b2, c2, d2, e2)) DRET;
    if(!checkEquality(a3, b3, c3)) DRET;
    if(!checkEquality(b6, c6, d7)) DRET;
    if(!checkEquality(c8, d5)) DRET;
    if(!checkLessThan(a1, a2, a3, b4, c4, b5, b6, b7, c7, c8, c9, c10, c11, d6, d8, e3)) DRET;
    if(!checkLessThan(b1, c2, d3, c7, c8, c9, c10, c11, d6, d8, e3)) DRET;
    if(!checkLessThan(d1, c2, a3, b4, d4, c8, c9, c10, c11, d6, d8, e3)) DRET;
    if(!checkLessThan(b1, b2, b3, b4, b5, b6, b7, e3)) DRET;
    if(!checkLessThan(c1, c2, c3, c4, c5, c6, c7, c8, c9, c10, c11, e3)) DRET;
    if(!checkLessThan(d1, d2, d3, d4, d5, d6, d8, e3)) DRET;
    if(!checkLessThan(e1, e2, c3, b4, c4, b5, c7, d5, d6, d8, e3)) DRET;
    return true;
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
                return DPRINT(TType::mod, "Error: WMPUInt<", kSize+1, ">::modulo(", divisor, ") of\n",
                              product, "\nreturned ", result1, " instead of ",
                              expectedResult, "\n");

            result2 = product % divisor;
            if(result2 != expectedResult)
                return DPRINT(TType::modop, "Error: WMPUInt<", kSize+1, ">::operator%(", divisor, ") of\n",
                              product, "\nreturned ", result1, " instead of ",
                              expectedResult, "\n");

            result2 = product;
            result2 %= divisor;
            if(result2 != expectedResult)
                return DPRINT(TType::amodop, "Error: WMPUInt<", kSize+1, ">::operator%=(", divisor, ") of\n",
                              product, "\nreturned ", result1, " instead of ",
                              expectedResult, "\n");

            ++product;
        }
    }
    return true;
}

bool testModulo()
{
    std::cout << "Testing modulo" << std::endl;
    if(!testModuloForSize<1>(gRngEngine)) DRET;
    if(!testModuloForSize<2>(gRngEngine)) DRET;
    if(!testModuloForSize<3>(gRngEngine)) DRET;
    if(!testModuloForSize<4>(gRngEngine)) DRET;
    if(!testModuloForSize<5>(gRngEngine)) DRET;
    if(!testModuloForSize<6>(gRngEngine)) DRET;
    if(!testModuloForSize<7>(gRngEngine)) DRET;
    if(!testModuloForSize<8>(gRngEngine)) DRET;
    if(!testModuloForSize<9>(gRngEngine)) DRET;
    if(!testModuloForSize<10>(gRngEngine)) DRET;
    return true;
}


//============================================================================
// Test negation
//============================================================================
template<std::size_t kSize>
static bool negationError(TType ttype, const WMPUInt<kSize>& original,
                          const WMPUInt<kSize>& result, const WMPUInt<kSize>& expected)
{
    return DPRINT(ttype, "Error: Negation of\n", original, "\nresulted in\n", result,
                  "\ninstead of\n", expected, "\n");
}

template<std::size_t kSize>
static bool testNegationWithSize()
{
    WMPUInt<kSize> positive(0), negative, result;

    for(std::size_t i = 0; i < kSize-1; ++i)
        negative.data()[i] = -1;

    result = positive;
    result.neg();
    if(result != positive) { negationError(TType::neg, positive, result, positive); DRET; }

    for(std::uint64_t value = 1; value < 100; ++value)
    {
        positive.data()[kSize-1] = value;
        negative.data()[kSize-1] = -value;

        result = positive;
        result.neg();
        if(result != negative) { negationError(TType::neg, positive, result, negative); DRET; }
        result.neg();
        if(result != positive) { negationError(TType::neg, negative, result, positive); DRET; }
        result = -positive;
        if(result != negative) { negationError(TType::negop, positive, result, negative); DRET; }
        result = -negative;
        if(result != positive) { negationError(TType::negop, negative, result, positive); DRET; }
    }

    const std::uint64_t allBits = -1;
    for(unsigned i = 0; i < 1000000; ++i)
    {
        for(std::size_t i = 0; i < kSize; ++i) positive.data()[i] = gRngEngine();
        for(std::size_t i = 0; i < kSize; ++i) negative.data()[i] = positive.data()[i] ^ allBits;
        ++negative;

        result = positive;
        result.neg();
        if(result != negative) { negationError(TType::neg, positive, result, negative); DRET; }
        result.neg();
        if(result != positive) { negationError(TType::neg, negative, result, positive); DRET; }
        result = -positive;
        if(result != negative) { negationError(TType::negop, positive, result, negative); DRET; }
        result = -negative;
        if(result != positive) { negationError(TType::negop, negative, result, positive); DRET; }
    }

    return true;
}

bool testNegation()
{
    std::cout << "Testing negation" << std::endl;
    if(!testNegationWithSize<2>()) DRET;
    if(!testNegationWithSize<3>()) DRET;
    if(!testNegationWithSize<4>()) DRET;
    if(!testNegationWithSize<5>()) DRET;
    if(!testNegationWithSize<6>()) DRET;
    if(!testNegationWithSize<7>()) DRET;
    if(!testNegationWithSize<8>()) DRET;
    if(!testNegationWithSize<9>()) DRET;
    if(!testNegationWithSize<10>()) DRET;
    if(!testNegationWithSize<11>()) DRET;
    if(!testNegationWithSize<12>()) DRET;
    if(!testNegationWithSize<50>()) DRET;
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

bool testShifting()
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
