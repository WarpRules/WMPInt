#pragma once
#include "../WMPInt.hh"
#include <iostream>
#include <iomanip>

//============================================================================
// Output helper functions
//============================================================================
template<std::size_t kSize>
inline std::ostream& operator<<(std::ostream& os, const WMPUInt<kSize>& value)
{
    std::ios_base::fmtflags f = os.flags();
    os << '[' << std::hex << std::uppercase << std::setw(16) << std::setfill('0')
       << value.data()[0];
    for(std::size_t i = 1; i < kSize; ++i)
        os << ',' << std::setw(16) << std::setfill('0') << value.data()[i];
    os << ']';
    os.flags(f);
    return os;
}

struct sethexw0 { unsigned w; sethexw0(unsigned val): w(val) {}; };

inline std::ostream& operator<<(std::ostream& os, sethexw0 s)
{
    return os << std::hex << std::setw(s.w) << std::setfill('0') << std::uppercase;
}

inline std::ostream& operator<<(std::ostream& os, __uint128_t value)
{
    if(os.flags() & std::ios_base::hex)
    {
        const std::uint64_t v1 = static_cast<std::uint64_t>(value >> 64);
        const std::uint64_t v2 = static_cast<std::uint64_t>(value);
        if(v1 > 0 || os.width() > 16)
        {
            if(os.width() > 16) os << std::setw(os.width() - 16);
            return os << v1 << std::setw(16) << std::setfill('0') << v2;
        }
        else return os << v2;
    }
    else
    {
        const __uint128_t modulo = 10000000000000000000llu;
        const std::uint64_t v1 = static_cast<std::uint64_t>(value / (modulo*modulo));
        const std::uint64_t v2 = static_cast<std::uint64_t>((value / modulo) % modulo);
        const std::uint64_t v3 = static_cast<std::uint64_t>(value % modulo);
        if(v1 > 0) return os << v1 << std::setw(19) << std::setfill('0') << v2
                             << std::setw(19) << std::setfill('0') << v3;
        else if(v2 > 0) return os << v2 << std::setw(19) << std::setfill('0') << v3;
        else return os << v3;
    }
}

enum class TType: unsigned char
{
    s1Op, aHex, pHex, aDec, pDec, plus, aplus, minus, aminus, mult, amult, fmult,
    mod, modop, amodop, neg, negop, flmult, fkmult, tkmult, div, adiv, comp
};

std::ostream& operator<<(std::ostream&, TType);

inline bool dprint() { return false; }

template<typename T, typename... Rest>
inline bool dprint(T&& param, Rest&&... rest)
{
    std::cout << param;
    return dprint(std::forward<Rest>(rest)...);
}

template<typename... Rest>
inline bool dprint(const char* str, TType ttype, Rest&&... rest)
{
    std::cout << ttype;
    return dprint(str, std::forward<Rest>(rest)...);
}

#define STRINGIFY_HLP(x) #x
#define STRINGIFY(x) STRINGIFY_HLP(x)
#define DPRINT(...) dprint("[" __FILE__ ":" STRINGIFY(__LINE__) "]: ", __VA_ARGS__)
#define DRET return DPRINT("Called from here.\n")
#define DRETM return !DPRINT("Called from here.\n")

constexpr const char* const kTTypes[] =
{
    "WMPUInt::<1> operators",
    "WMPUInt::assignFromHexStr()",
    "WMPUInt::printAsHexStr()",
    "WMPUInt::assignFromDecStr()",
    "WMPUInt::printAsDecStr()",
    "operator+()",
    "WMPUInt::operator+=()",
    "operator-()",
    "WMPUInt::operator-=()",
    "operator*()",
    "WMPUInt::operator*=()",
    "WMPUInt::fullMultiply()",
    "WMPUInt::modulo()",
    "WMPUInt::operator%()",
    "WMPUInt::operator%=()",
    "WMPUInt::neg()",
    "WMPUInt::operator-()",
    "WMPUInt::fullLongMultiplication()",
    "WMPUInt::fullKaratsubaMultiplication()",
    "WMPUInt::truncatedKaratsubaMultiplication()",
    "WMPUInt::operator/()",
    "WMPUInt::operator/=()",
    "comparison operators",
};

inline std::ostream& operator<<(std::ostream& os, TType ttype)
{
    return os << "[INFO] Testing " << kTTypes[unsigned(ttype)] << ":\n";
}
