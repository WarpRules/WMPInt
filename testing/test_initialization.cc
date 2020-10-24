#include "test.hh"
#include <random>
#include <cstring>
#include <cctype>

extern std::mt19937_64 gRngEngine;

template<typename... Values_t>
static bool testAssignmentFromHexStr(const char* str, Values_t&&... values)
{
    using UInt_t = WMPUInt<sizeof...(Values_t)>;
    UInt_t v1, v2(values...);
    const std::size_t strLength = std::strlen(str);

    const char* endPtr = v1.assignFromHexStr(str);
    if(v1 != v2) return DPRINT(TType::aHex, "Error: Initializing with \"", str, "\" failed:\n",
                               v1, "\n", v2, "\n");
    if(endPtr != str + strLength)
        return DPRINT(TType::aHex, "Error: initializing with \"", str,
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
        return DPRINT(TType::pHex, "Error: printAsHexStr resulted in\n\"", ptr,
                      "\"\ninstead of\n\"", uppercaseStr, "\"\n");

    return true;
}

template<typename... Values_t>
static bool testAssignmentFromDecStr(const char* str, Values_t&&... values)
{
    WMPUInt<sizeof...(Values_t)> v1, v2(values...);
    const char* endPtr = v1.assignFromDecStr(str);
    if(v1 != v2) return DPRINT(TType::aDec, "Error: Initializing with \"", str, "\" failed:\n     Got: ",
                               v1, "\nExpected: ", v2, "\n");
    if(endPtr != str + std::strlen(str))
        return DPRINT(TType::aDec, "Error: initializing with \"", str, "\" did not return the correct pointer.\n");

    WMPUInt<sizeof...(Values_t) + 1> v3, v4(values...);
    v3.data()[0] = 12345;
    endPtr = v3.assignFromDecStr(str);
    if(v3 != v4) return DPRINT(TType::aDec, "Error: Initializing with \"", str, "\" failed:\n     Got: ",
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
                return DPRINT(TType::pDec, "Error: WMPUInt<", kSize,
                              ">::printAsDecStr() outputted too many characters for value\n",
                              value, "\n");

            if(std::strcmp(inputStr, resultStr) != 0)
                return DPRINT(TType::pDec, "Error: WMPUInt<", kSize,
                              ">::printAsDecStr() outputted\n\"", resultStr,
                              "\"\ninstead of\n\"", inputStr, "\"\n");
        }
    }

    return true;
}

bool testAssignmentFromStr()
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
