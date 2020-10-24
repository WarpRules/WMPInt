#include "test.hh"

template<typename Value_t1, typename Value_t2, std::size_t kSize>
static bool checkAddition(const Value_t1& value1, const Value_t2& value2,
                          const WMPUInt<kSize>& expectedResult)
{
    const WMPUInt<kSize> result = value1 + value2;
    if(result != expectedResult)
        return DPRINT(TType::plus, "Error: ", value1, "+", value2,
                      "\nresulted in ", result,
                      "\n instead of ", expectedResult, "\n");

    WMPUInt<kSize> result2(value1);
    result2 += value2;
    if(result2 != expectedResult)
        return DPRINT(TType::aplus, "Error: ", value1, "+=", value2,
                      "\nresulted in ", result2,
                      "\n instead of ", expectedResult, "\n");
    return true;
}

template<typename Value_t1, typename Value_t2, std::size_t kSize>
static bool checkSubtraction(const Value_t1& value1, const Value_t2& value2,
                             const WMPUInt<kSize>& expectedResult)
{
    const WMPUInt<kSize> result = value1 - value2;
    if(result != expectedResult)
        return DPRINT(TType::minus, "Error: ", value1, "-", value2,
                      "\nresulted in ", result,
                      "\n instead of ", expectedResult, "\n");

    WMPUInt<kSize> result2(value1);
    result2 -= value2;
    if(result2 != expectedResult)
        return DPRINT(TType::aminus, "Error: ", value1, "-=", value2,
                      "\nresulted in ", result2,
                      "\n instead of ", expectedResult, "\n");
    return true;
}


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

bool testAddition()
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
