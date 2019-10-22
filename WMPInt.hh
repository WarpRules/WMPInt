#ifndef WMPINT_INCLUDE_GUARD
#define WMPINT_INCLUDE_GUARD

#define WMPINT_VERSION 0x000100
#define WMPINT_VERSION_STRING "0.1.0"
#define WMPINT_COPYRIGHT_STRING "WMPInt v" WMPINT_VERSION_STRING " (C)2019 Juha Nieminen"

#include <cstdint>
#include <cstddef>
#include <initializer_list>

//============================================================================
// WMPUInt class
//============================================================================
template<std::size_t kSize>
class WMPUInt
{
    static_assert(kSize > 0, "Size must be at least 1");

 public:
    //------------------------------------------------------------------------
    // Initialization
    //------------------------------------------------------------------------
    /* This object is by default uninitialized. The value is undefined. */
    WMPUInt() {}

    explicit WMPUInt(std::uint64_t);

    template<typename... Values_t>
    explicit WMPUInt(std::uint64_t, Values_t...);

    explicit WMPUInt(std::initializer_list<std::uint64_t>);

    template<typename... Values_t>
    void assign(std::uint64_t, Values_t...);

    void assign(std::initializer_list<std::uint64_t>);

    template<std::size_t kSize2>
    void assign(const WMPUInt<kSize2>&);

    const char* assignFromHexStr(const char*);
    const char* assignFromDecStr(const char*);
    const char* assignFromDecStr(const char*, std::uint64_t* tempBuffer);

    //------------------------------------------------------------------------
    // Data retrieval
    //------------------------------------------------------------------------
    constexpr static std::size_t size() { return kSize; }
    constexpr static std::size_t bits() { return kSize * 64; }
    std::uint64_t* data() { return mData; }
    const std::uint64_t* data() const { return mData; }

    //------------------------------------------------------------------------
    // ASCII output
    //------------------------------------------------------------------------
    constexpr static std::size_t maxHexadecimalDigits();
    constexpr static std::size_t maxDecimalDigits();

    void printAsHexStr(char* destination) const;

    //------------------------------------------------------------------------
    // Comparison operators
    //------------------------------------------------------------------------
    bool operator==(const WMPUInt<kSize>&) const;
    bool operator==(std::uint64_t) const;
    bool operator!=(const WMPUInt<kSize>&) const;
    bool operator!=(std::uint64_t) const;
    bool operator<(const WMPUInt<kSize>&) const;
    bool operator<(std::uint64_t) const;
    bool operator<=(const WMPUInt<kSize>&) const;
    bool operator<=(std::uint64_t) const;
    bool operator>(const WMPUInt<kSize>&) const;
    bool operator>(std::uint64_t) const;
    bool operator>=(const WMPUInt<kSize>&) const;
    bool operator>=(std::uint64_t) const;

    //------------------------------------------------------------------------
    // Bitwise operators
    //------------------------------------------------------------------------
    WMPUInt<kSize>& operator&=(const WMPUInt<kSize>&);
    WMPUInt<kSize>& operator&=(std::uint64_t);
    WMPUInt<kSize> operator&(const WMPUInt<kSize>&) const;
    WMPUInt<kSize> operator&(std::uint64_t) const;
    WMPUInt<kSize>& operator|=(const WMPUInt<kSize>&);
    WMPUInt<kSize>& operator|=(std::uint64_t);
    WMPUInt<kSize> operator|(const WMPUInt<kSize>&) const;
    WMPUInt<kSize> operator|(std::uint64_t) const;
    WMPUInt<kSize>& operator^=(const WMPUInt<kSize>&);
    WMPUInt<kSize>& operator^=(std::uint64_t);
    WMPUInt<kSize> operator^(const WMPUInt<kSize>&) const;
    WMPUInt<kSize> operator^(std::uint64_t) const;

    //------------------------------------------------------------------------
    // Arithmetic operators
    //------------------------------------------------------------------------
    WMPUInt<kSize>& operator+=(const WMPUInt<kSize>&);
    WMPUInt<kSize> operator+(const WMPUInt<kSize>&) const;
    WMPUInt<kSize>& operator+=(std::uint64_t);
    WMPUInt<kSize> operator+(std::uint64_t) const;
    WMPUInt<kSize>& operator-=(const WMPUInt<kSize>&);
    WMPUInt<kSize> operator-(const WMPUInt<kSize>&) const;
    WMPUInt<kSize>& operator-=(std::uint64_t);
    WMPUInt<kSize> operator-(std::uint64_t) const;
    WMPUInt<kSize>& operator++();
    WMPUInt<kSize>& operator--();
    WMPUInt<kSize>& operator*=(const WMPUInt<kSize>&);
    WMPUInt<kSize> operator*(const WMPUInt<kSize>&) const;
    WMPUInt<kSize>& operator*=(std::uint64_t);
    WMPUInt<kSize> operator*(std::uint64_t) const;

    WMPUInt<kSize> operator-() const;
    void neg();

    void multiply(const WMPUInt<kSize>&, std::uint64_t* result, std::uint64_t* tempBuffer) const;
    void multiply(std::uint64_t, std::uint64_t* result) const;

    void fullMultiply(const WMPUInt<kSize>&, std::uint64_t* result,
                      std::uint64_t* tempBuffer) const;

    void addTo(WMPUInt<kSize>& target1, WMPUInt<kSize>& target2) const;


//----------------------------------------------------------------------------
 private:
//----------------------------------------------------------------------------
    std::uint64_t mData[kSize];
};


//============================================================================
// WMPUInt<1> specialization
//============================================================================
template<>
class WMPUInt<1>
{
 public:
    WMPUInt() {}
    WMPUInt(std::uint64_t value): mValue(value) {} // deliberately non-explicit
    explicit WMPUInt(std::initializer_list<std::uint64_t>);
    void assign(std::uint64_t value) { mValue = value; }
    void assign(std::initializer_list<std::uint64_t>);
    template<std::size_t kSize2>
    void assign(const WMPUInt<kSize2>& rhs) { mValue = rhs.data()[kSize2-1]; }
    constexpr static std::size_t size() { return 1; }
    std::uint64_t* data() { return &mValue; }
    const std::uint64_t* data() const { return &mValue; }
    bool operator==(const WMPUInt<1>& rhs) const { return mValue == rhs.mValue; }
    bool operator!=(const WMPUInt<1>& rhs) const { return mValue != rhs.mValue; }
    bool operator<(const WMPUInt<1>& rhs) const { return mValue < rhs.mValue; }
    bool operator<=(const WMPUInt<1>& rhs) const { return mValue <= rhs.mValue; }
    bool operator>(const WMPUInt<1>& rhs) const { return mValue > rhs.mValue; }
    bool operator>=(const WMPUInt<1>& rhs) const { return mValue >= rhs.mValue; }
    WMPUInt<1>& operator&=(const WMPUInt<1>& rhs) { mValue &= rhs.mValue; return *this; }
    WMPUInt<1> operator&(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue & rhs.mValue); }
    WMPUInt<1>& operator|=(const WMPUInt<1>& rhs) { mValue |= rhs.mValue; return *this; }
    WMPUInt<1> operator|(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue | rhs.mValue); }
    WMPUInt<1>& operator^=(const WMPUInt<1>& rhs) { mValue ^= rhs.mValue; return *this; }
    WMPUInt<1> operator^(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue ^ rhs.mValue); }
    WMPUInt<1>& operator+=(const WMPUInt<1>& rhs) { mValue += rhs.mValue; return *this; }
    WMPUInt<1> operator+(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue + rhs.mValue); }
    WMPUInt<1>& operator-=(const WMPUInt<1>& rhs) { mValue -= rhs.mValue; return *this; }
    WMPUInt<1> operator-(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue - rhs.mValue); }
    WMPUInt<1>& operator++() { ++mValue; return *this; }
    WMPUInt<1>& operator--() { --mValue; return *this; }
    WMPUInt<1>& operator*=(const WMPUInt<1>& rhs) { mValue *= rhs.mValue; return *this; }
    WMPUInt<1> operator*(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue * rhs.mValue); }
    WMPUInt<1> operator-() const { return WMPUInt<1>(-mValue); }
    void neg() { mValue = -mValue; }
    void multiply(const WMPUInt<1>& rhs, std::uint64_t* result, std::uint64_t*) const
    { *result = mValue * rhs.mValue; }
    void multiply(std::uint64_t rhs, std::uint64_t* result) const { *result = mValue * rhs; }
    void fullMultiply(const WMPUInt<1>&, std::uint64_t* result, std::uint64_t* tempBuffer) const;
    void addTo(WMPUInt<1>& target1, WMPUInt<1>& target2) const;

 private:
    std::uint64_t mValue;
};


//============================================================================
// Implementations
//============================================================================
// WMPUInt<1> specializations
//----------------------------------------------------------------------------
inline WMPUInt<1>::WMPUInt(std::initializer_list<std::uint64_t> values):
    mValue(values.size() ? *values.begin() : 0)
{}

inline void WMPUInt<1>::assign(std::initializer_list<std::uint64_t> values)
{
    mValue = values.size() ? *values.begin() : 0;
}

inline void WMPUInt<1>::fullMultiply
(const WMPUInt<1>& rhs, std::uint64_t* result, std::uint64_t*) const
{
    asm ("mulq %[rhs]"
         : "=a"(result[1]), "=d"(result[0])
         : "a"(mValue), [rhs]"rm"(rhs.mValue) : "cc");
}

inline void WMPUInt<1>::addTo(WMPUInt<1>& target1, WMPUInt<1>& target2) const
{
    target1.mValue += mValue;
    target2.mValue += mValue;
}

//----------------------------------------------------------------------------
// Constructors
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>::WMPUInt(std::uint64_t value)
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        mData[i] = 0;
    mData[kSize-1] = value;
}

template<std::size_t kSize>
template<typename... Values_t>
inline WMPUInt<kSize>::WMPUInt(std::uint64_t firstValue, Values_t... rest)
{
    assign(firstValue, rest...);
}

template<std::size_t kSize>
inline WMPUInt<kSize>::WMPUInt(std::initializer_list<std::uint64_t> values)
{
    assign(values);
}


//----------------------------------------------------------------------------
// Assign
//----------------------------------------------------------------------------
template<std::size_t kSize>
template<typename... Values_t>
inline void WMPUInt<kSize>::assign(std::uint64_t firstValue, Values_t... rest)
{
    static_assert(sizeof...(rest) < kSize);
    std::uint64_t values[] = { firstValue, static_cast<std::uint64_t>(rest)... };
    const std::size_t startInd = kSize - (sizeof...(rest) + 1);
    for(std::size_t i = 0; i < startInd; ++i) mData[i] = 0;
    for(std::size_t i = 0; i < sizeof...(rest)+1; ++i) mData[startInd+i] = values[i];
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::assign(std::initializer_list<std::uint64_t> values)
{
    if(values.size() > kSize)
    {
        const std::size_t startInd = values.size() - kSize;
        for(std::size_t i = 0; i < kSize; ++i)
            mData[i] = values.begin()[startInd + i];
    }
    else
    {
        const std::size_t startInd = kSize - values.size();
        for(std::size_t i = 0; i < startInd; ++i)
            mData[i] = 0;
        for(std::size_t i = 0; i < values.size(); ++i)
            mData[startInd + i] = values.begin()[i];
    }
}

template<std::size_t kSize>
template<std::size_t kSize2>
inline void WMPUInt<kSize>::assign(const WMPUInt<kSize2>& rhs)
{
    if constexpr(kSize <= kSize2)
    {
        for(std::size_t i = 0; i < kSize; ++i)
            mData[i] = rhs.data()[i + (kSize2 - kSize)];
    }
    else
    {
        for(std::size_t i = 0; i < kSize - kSize2; ++i)
            mData[i] = 0;
        for(std::size_t i = 0; i < kSize2; ++i)
            mData[i + (kSize - kSize2)] = rhs.data()[i];
    }
}

//----------------------------------------------------------------------------
// Assign from string
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline const char* WMPUInt<kSize>::assignFromHexStr(const char* ptr)
{
    const char* ptr2 = ptr - 1;
    unsigned char c;
    do
    {
        ++ptr2;
        c = static_cast<unsigned char>(*ptr2) - 0x41;
        if(c > 5) c -= 0x20;
        if(c > 5) c -= 0xCF;
    } while(c < 10);

    const char* retval = ptr2;

    std::size_t dataIndex = kSize;
    while(dataIndex > 0 && ptr2 != ptr)
    {
        --dataIndex;
        std::uint64_t value = 0;
        for(unsigned i = 0; i < 16 && ptr2 != ptr; ++i)
        {
            --ptr2;
            char c = *ptr2 - '0';
            if(c > 9) c -= ('A' - '0' - 10);
            if(c > 15) c -= ('a' - 'A');
            value |= (static_cast<std::uint64_t>(c) << (i*4));
        }
        mData[dataIndex] = value;
    }

    for(std::size_t i = 0; i < dataIndex; ++i)
        mData[i] = 0;

    return retval;
}

template<std::size_t kSize>
inline const char* WMPUInt<kSize>::assignFromDecStr(const char* ptr, std::uint64_t* tempBuffer)
{
    std::uint64_t value = 0;
    unsigned charCounter;

    for(charCounter = 0; charCounter < 19; ++charCounter, ++ptr)
    {
        const unsigned char c = static_cast<unsigned char>(*ptr) - '0';
        if(c > 9) break;
        value = value * 10 + c;
    }

    for(std::size_t i = 0; i < kSize - 1; ++i) mData[i] = 0;
    mData[kSize - 1] = value;

    while(charCounter > 0)
    {
        value = 0;
        for(charCounter = 0; charCounter < 19; ++charCounter, ++ptr)
        {
            const unsigned char c = static_cast<unsigned char>(*ptr) - '0';
            if(c > 9) break;
            value = value * 10 + c;
        }

        if(charCounter > 0)
        {
            if(charCounter == 19)
            {
                multiply(UINT64_C(10000000000000000000), tempBuffer);
            }
            else
            {
                std::uint64_t factor = 10;
                for(unsigned i = 1; i < charCounter; ++i)
                    factor *= 10;
                multiply(factor, tempBuffer);
            }

            for(std::size_t i = 0; i < kSize; ++i) mData[i] = tempBuffer[i];
            *this += value;
        }
    }

    return ptr;
}

template<std::size_t kSize>
inline const char* WMPUInt<kSize>::assignFromDecStr(const char* ptr)
{
    std::uint64_t tempBuffer[kSize];
    return assignFromDecStr(ptr, tempBuffer);
}


//----------------------------------------------------------------------------
// ASCII output
//----------------------------------------------------------------------------
template<std::size_t kSize>
constexpr std::size_t WMPUInt<kSize>::maxHexadecimalDigits()
{
    return kSize * 16;
}

template<std::size_t kSize>
constexpr std::size_t WMPUInt<kSize>::maxDecimalDigits()
{
    return std::size_t(kSize * 19.265919722494796493679289 + 1.0);
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::printAsHexStr(char* destination) const
{
    for(std::size_t componentInd = 0; componentInd < kSize; ++componentInd)
    {
        const std::uint64_t value = mData[componentInd];
        for(unsigned nibbleInd = 0; nibbleInd < 16; ++nibbleInd)
        {
            const unsigned char nibble = (value >> ((15 - nibbleInd)*4)) & 0xF;
            *destination++ = (nibble < 10 ? nibble + '0' : nibble + ('A' - 10));
        }
    }
}


//----------------------------------------------------------------------------
// Comparison operators
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator==(const WMPUInt<kSize>& rhs) const
{
    for(std::size_t i = 0; i < kSize; ++i)
        if(mData[i] != rhs.mData[i]) return false;
    return true;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator==(std::uint64_t rhs) const
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        if(mData[i]) return false;
    return mData[kSize - 1] == rhs;
}

template<std::size_t kSize>
inline bool operator==(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs == lhs;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator!=(const WMPUInt<kSize>& rhs) const
{
    return !(*this == rhs);
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator!=(std::uint64_t rhs) const
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        if(mData[i]) return true;
    return mData[kSize - 1] != rhs;
}

template<std::size_t kSize>
inline bool operator!=(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs != lhs;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator<(const WMPUInt<kSize>& rhs) const
{
    for(std::size_t i = 0; i < kSize; ++i)
    {
        if(mData[i] < rhs.mData[i]) return true;
        if(mData[i] > rhs.mData[i]) return false;
    }
    return false;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator<(std::uint64_t rhs) const
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        if(mData[i]) return false;
    return mData[kSize - 1] < rhs;
}

template<std::size_t kSize>
inline bool operator<(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs > lhs;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator<=(const WMPUInt<kSize>& rhs) const
{
    for(std::size_t i = 0; i < kSize; ++i)
    {
        if(mData[i] < rhs.mData[i]) return true;
        if(mData[i] > rhs.mData[i]) return false;
    }
    return true;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator<=(std::uint64_t rhs) const
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        if(mData[i]) return false;
    return mData[kSize - 1] <= rhs;
}

template<std::size_t kSize>
inline bool operator<=(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs >= lhs;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator>(const WMPUInt<kSize>& rhs) const
{
    for(std::size_t i = 0; i < kSize; ++i)
    {
        if(mData[i] < rhs.mData[i]) return false;
        if(mData[i] > rhs.mData[i]) return true;
    }
    return false;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator>(std::uint64_t rhs) const
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        if(mData[i]) return true;
    return mData[kSize - 1] > rhs;
}

template<std::size_t kSize>
inline bool operator>(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs < lhs;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator>=(const WMPUInt<kSize>& rhs) const
{
    for(std::size_t i = 0; i < kSize; ++i)
    {
        if(mData[i] < rhs.mData[i]) return false;
        if(mData[i] > rhs.mData[i]) return true;
    }
    return true;
}

template<std::size_t kSize>
inline bool WMPUInt<kSize>::operator>=(std::uint64_t rhs) const
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        if(mData[i]) return true;
    return mData[kSize - 1] >= rhs;
}

template<std::size_t kSize>
inline bool operator>=(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs <= lhs;
}


//----------------------------------------------------------------------------
// Bitwise operators
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator&=(const WMPUInt<kSize>& rhs)
{
    for(std::size_t i = 0; i < kSize; ++i)
        mData[i] &= rhs.mData[i];
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator&=(std::uint64_t rhs)
{
    for(std::size_t i = 0; i < kSize - 1; ++i)
        mData[i] = 0;
    mData[kSize - 1] &= rhs;
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator&(const WMPUInt<kSize>& rhs) const
{
    WMPUInt<kSize> res;
    for(std::size_t i = 0; i < kSize; ++i)
        res[i] = mData[i] & rhs.mData[i];
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator&(std::uint64_t rhs) const
{
    WMPUInt<kSize> res;
    for(std::size_t i = 0; i < kSize - 1; ++i)
        res[i] = 0;
    res[kSize - 1] = mData[kSize - 1] & rhs;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator&(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs & lhs;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator|=(const WMPUInt<kSize>& rhs)
{
    for(std::size_t i = 0; i < kSize; ++i)
        mData[i] |= rhs.mData[i];
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator|=(std::uint64_t rhs)
{
    mData[kSize - 1] |= rhs;
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator|(const WMPUInt<kSize>& rhs) const
{
    WMPUInt<kSize> res;
    for(std::size_t i = 0; i < kSize; ++i)
        res[i] = mData[i] | rhs.mData[i];
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator|(std::uint64_t rhs) const
{
    WMPUInt<kSize> res;
    for(std::size_t i = 0; i < kSize - 1; ++i)
        res[i] = mData[i];
    res[kSize - 1] = mData[kSize - 1] | rhs;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator|(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs | lhs;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator^=(const WMPUInt<kSize>& rhs)
{
    for(std::size_t i = 0; i < kSize; ++i)
        mData[i] ^= rhs.mData[i];
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator^=(std::uint64_t rhs)
{
    mData[kSize - 1] ^= rhs;
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator^(const WMPUInt<kSize>& rhs) const
{
    WMPUInt<kSize> res;
    for(std::size_t i = 0; i < kSize; ++i)
        res[i] = mData[i] ^ rhs.mData[i];
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator^(std::uint64_t rhs) const
{
    WMPUInt<kSize> res;
    for(std::size_t i = 0; i < kSize - 1; ++i)
        res[i] = mData[i];
    res[kSize - 1] = mData[kSize - 1] ^ rhs;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator^(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs ^ lhs;
}


//----------------------------------------------------------------------------
// Addition
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator+=(const WMPUInt<kSize>& rhs)
{
    std::uint64_t tempReg;

    if constexpr(kSize == 2)
        asm ("movq 8(%[rhs]), %[tempReg]\n\t"
             "addq %[tempReg], 8(%[lhs])\n\t"
             "movq (%[rhs]), %[tempReg]\n\t"
             "adcq %[tempReg], (%[lhs])"
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 3)
        asm ("movq 16(%[rhs]), %[tempReg]\n\t"
             "addq %[tempReg], 16(%[lhs])\n\t"
             "movq 8(%[rhs]), %[tempReg]\n\t"
             "adcq %[tempReg], 8(%[lhs])\n\t"
             "movq (%[rhs]), %[tempReg]\n\t"
             "adcq %[tempReg], (%[lhs])"
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 4)
        asm ("movq 24(%[rhs]), %[tempReg]\n\t"
             "addq %[tempReg], 24(%[lhs])\n\t"
             "movq 16(%[rhs]), %[tempReg]\n\t"
             "adcq %[tempReg], 16(%[lhs])\n\t"
             "movq 8(%[rhs]), %[tempReg]\n\t"
             "adcq %[tempReg], 8(%[lhs])\n\t"
             "movq (%[rhs]), %[tempReg]\n\t"
             "adcq %[tempReg], (%[lhs])"
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
#if __ADX__
    else if constexpr(kSize % 2 == 0)
    {
        const std::uint64_t* lhsHalfPoint = mData + (kSize / 2);
        const std::uint64_t* rhsHalfPoint = rhs.mData + (kSize / 2);
        std::uint64_t dataInd = 8 * (kSize/2), tempReg2;
        asm ("test %%al, %%al\n"
             "loop%=:\n\t"
             "leaq -8(%[dataInd]), %[dataInd]\n\t"
             "movq (%[lhsBegin],%[dataInd]), %[tempReg1]\n\t"
             "movq (%[lhsHalf],%[dataInd]), %[tempReg2]\n\t"
             "adoxq (%[rhsBegin],%[dataInd]), %[tempReg1]\n\t"
             "adcxq (%[rhsHalf],%[dataInd]), %[tempReg2]\n\t"
             "movq %[tempReg1], (%[lhsBegin],%[dataInd])\n\t"
             "movq %[tempReg2], (%[lhsHalf],%[dataInd])\n\t"
             "jrcxz done%=\n\t"
             "jmp loop%=\n"
             "done%=:\n\t"
             "jnc end%=\n\t"
             "movq %[kSizeP2], %[dataInd]\n"
             "loop2%=:\n\t"
             "decq %[dataInd]\n\t"
             "js end%=\n\t"
             "incq (%[lhsBegin],%[dataInd],8)\n\t"
             "jz loop2%=\n"
             "end%=:"
             : "+m"(mData), [dataInd]"+c"(dataInd),
               [tempReg1]"=&r"(tempReg), [tempReg2]"=&r"(tempReg2)
             : [lhsHalf]"r"(lhsHalfPoint), [rhsHalf]"r"(rhsHalfPoint),
               [lhsBegin]"r"(mData), [rhsBegin]"r"(rhs.mData), [kSizeP2]"i"(kSize/2),
               "m"(rhs.mData) : "cc");
    }
#endif
    else
    {
        /* There is not enough speed advantage in unrolling the loop to warrant
           the complexity it introduces to the implementation. */
        std::uint64_t dataInd = kSize - 1;
        asm ("movq (%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "addq %[tempReg], (%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "movq -8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "adcq %[tempReg], -8(%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "jnz loop%="
             : "+m"(mData), [dataInd]"+r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator+(const WMPUInt<kSize>& rhs) const
{
    /* In theory this could be implemented also in inline asm which adds the
       elements of *this with lhs and puts the result in res directly, so that
       we skip the copying happening below. However, at least gcc insists in
       zero-initializing res below even when it's not explicitly initialized here,
       and there's probably little speed benefit in doing this with its own asm. */
    WMPUInt<kSize> res = *this;
    res += rhs;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator+=(std::uint64_t value)
{
    std::uint64_t zero = 0;

    if constexpr(kSize == 2)
        asm ("addq %[value], 8(%[lhs])\n\t"
             "adcq %[zero], (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize == 3)
        asm ("addq %[value], 16(%[lhs])\n\t"
             "adcq %[zero], 8(%[lhs])\n\t"
             "adcq %[zero], (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize == 4)
        asm ("addq %[value], 24(%[lhs])\n\t"
             "adcq %[zero], 16(%[lhs])\n\t"
             "adcq %[zero], 8(%[lhs])\n\t"
             "adcq %[zero], (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");

    /* The speed advantage we get from unrolling is large enough to warrant it below. */
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm ("addq %[value], 16(%[lhs],%[dataInd],8)\n\t"
             "adcq %[zero], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "jnc exitLoop%=\n\t"
             "adcq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "adcq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%=\n"
             "exitLoop%=:"
             : "+m"(mData), [dataInd]"+r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm ("addq %[value], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "jnc exitLoop%=\n\t"
             "adcq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "adcq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%=\n"
             "exitLoop%=:"
             : "+m"(mData), [dataInd]"+r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator+(std::uint64_t value) const
{
    /* Same as with the other operator+(), there's probably little to no benefit
       in implementing this as its own asm implementation instead of copying and
       calling operator+=(). gcc would zero-initialize res anyway, and there doesn't
       seem to be any way of stopping it from doing that (plus there's probably very
       little speed benefit in implementing this operator+() separately.) */
    WMPUInt<kSize> res = *this;
    res += value;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator+(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs + lhs;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator++()
{
    /* There is probably negligible speed advantage in implementing this in the exact
       same way as operator+=(std::uint64_t) with the only difference being the first
       addq being replaced with an incq. */
    return *this += 1;
}


//----------------------------------------------------------------------------
// Subtraction
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(const WMPUInt<kSize>& rhs)
{
    std::uint64_t tempReg;

    if constexpr(kSize == 2)
        asm ("movq 8(%[rhs]), %[tempReg]\n\t"
             "subq %[tempReg], 8(%[lhs])\n\t"
             "movq (%[rhs]), %[tempReg]\n\t"
             "sbbq %[tempReg], (%[lhs])"
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 3)
        asm ("movq 16(%[rhs]), %[tempReg]\n\t"
             "subq %[tempReg], 16(%[lhs])\n\t"
             "movq 8(%[rhs]), %[tempReg]\n\t"
             "sbbq %[tempReg], 8(%[lhs])\n\t"
             "movq (%[rhs]), %[tempReg]\n\t"
             "sbbq %[tempReg], (%[lhs])"
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 4)
        asm ("movq 24(%[rhs]), %[tempReg]\n\t"
             "subq %[tempReg], 24(%[lhs])\n\t"
             "movq 16(%[rhs]), %[tempReg]\n\t"
             "sbbq %[tempReg], 16(%[lhs])\n\t"
             "movq 8(%[rhs]), %[tempReg]\n\t"
             "sbbq %[tempReg], 8(%[lhs])\n\t"
             "movq (%[rhs]), %[tempReg]\n\t"
             "sbbq %[tempReg], (%[lhs])"
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm ("movq 8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "subq %[tempReg], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "movq (%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "sbbq %[tempReg], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%="
             : "+m"(mData), [tempReg]"=&r"(tempReg), [dataInd]"+r"(dataInd)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator-(const WMPUInt<kSize>& rhs) const
{
    WMPUInt<kSize> res = *this;
    res -= rhs;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(std::uint64_t value)
{
    std::uint64_t zero = 0;

    if constexpr(kSize == 2)
        asm ("subq %[value], 8(%[lhs])\n\t"
             "sbbq %[zero], (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize == 3)
        asm ("subq %[value], 16(%[lhs])\n\t"
             "sbbq %[zero], 8(%[lhs])\n\t"
             "sbbq %[zero], (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize == 4)
        asm ("subq %[value], 24(%[lhs])\n\t"
             "sbbq %[zero], 16(%[lhs])\n\t"
             "sbbq %[zero], 8(%[lhs])\n\t"
             "sbbq %[zero], (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm ("subq %[value], 16(%[lhs],%[dataInd],8)\n\t"
             "sbbq %[zero], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "jnc exitLoop%=\n\t"
             "sbbq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "sbbq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%=\n"
             "exitLoop%=:"
             : "+m"(mData), [dataInd]"+r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm ("subq %[value], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "jnc exitLoop%=\n\t"
             "sbbq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "sbbq %[zero], (%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%=\n"
             "exitLoop%=:"
             : "+m"(mData), [dataInd]"+r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator-(std::uint64_t value) const
{
    WMPUInt<kSize> res = *this;
    res -= value;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator-(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    WMPUInt<kSize> res(lhs);
    res -= rhs;
    return res;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator--()
{
    /* There is probably negligible speed advantage in implementing this in the exact
       same way as operator-=(std::uint64_t) with the only difference being the first
       subq being replaced with a decq. */
    return *this -= 1;
}


//----------------------------------------------------------------------------
// Multiplication
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline void WMPUInt<kSize>::multiply
(const WMPUInt<kSize>& rhs, std::uint64_t* result, std::uint64_t* tempBuffer) const
{
    /* Long multiplication algorithm in base 2^64 (eg. kSize == 5)
       -----------------------------------------------------------
                        [R4][R3][R2][R1][R0] // rhs
                      * [L4][L3][L2][L1][L0] // lhs (*this)
         -----------------------------------
         [R4*L0][R3*L0][R2*L0][R1*L0][R0*L0]
       + [R3*L1][R2*L1][R1*L1][R0*L1]
       + [R2*L2][R1*L2][R0*L2]
       + [R1*L3][R0*L3]
       + [R0*L4]
    */
    if constexpr(kSize == 2)
    {
        /* The case of kSize==2 is simple enough to warrant its own loopless implementation */
        std::uint64_t tmp1, tmp2;
        asm ("movq 8(%[rhs]),%%rax /* hopsis */\n\t" // rax = rhs[1]
             "movq (%[rhs]),%[tmp2]\n\t" // tmp2 = rhs[0]
             "movq (%[lhs]),%[tmp1]\n\t" // tmp1 = lhs[0]
             "imulq %%rax,%[tmp1]\n\t" // tmp1 *= rax
             "mulq 8(%[lhs])\n\t" // (rax,rdx) = rax * lhs[1]
             "addq %[tmp1],%%rdx\n\t" // rdx += tmp1
             "imulq 8(%[lhs]),%[tmp2]\n\t" // tmp2 *= lhs[1]
             "addq %[tmp2],%%rdx\n\t" // rdx += tmp2
             "movq %%rax,8(%[result])\n\t" // result[1] = rax
             "movq %%rdx,(%[result])" // result[0] = rdx;
             : "+m"(*(std::uint64_t(*)[kSize])result),
               [tmp1]"=&r"(tmp1), [tmp2]"=&r"(tmp2)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), [result]"r"(result),
               "m"(mData), "m"(rhs.mData)
             : "rax", "rdx", "cc");
    }
    else
    {
        std::uint64_t lhsInd = kSize - 1, rhsInd = kSize - 1, rhsIndCounter,
            lhsValue = mData[kSize - 1];
        for(std::size_t i = 0; i < kSize; ++i) result[i] = 0;

        asm (/* Multiplication by [L0] */
             /* Loop: rhsInd = [kSize-1, 1] */
             "\nL1%=:\n\t"
             "movq (%[rhs],%[rhsInd],8),%%rax\n\t" // rax = rhs[rhsInd]
             "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
             "addq %%rax,(%[result],%[rhsInd],8)\n\t" // result[rhsInd] += rax
             "adcq %%rdx,-8(%[result],%[rhsInd],8)\n\t" // result[rhsInd-1] = rdx
             "decq %[rhsInd]\n\t" // --rhsInd
             "jnz L1%=\n\t" // if(lhsInd > 0) goto L1
             "imulq (%[rhs]),%[lhsValue]\n\t" // lhsValue = rhs[0] * lhsValue
             "addq %[lhsValue],(%[result])\n\t" // result[0] += lhsValue
             /* Multiplication by the remaining of lhs. */
             "decq %[lhsInd]\n" // --lhsInd
             /* Outer loop: lhsInd = [kSize-2, 0] */
             "L2%=:\n\t"
             /* tempBuf has to be zeroed: */
             "xorq %[lhsValue],%[lhsValue]\n\t" // lhsValue = 0
             "movq %[lhsInd],%[rhsIndCounter]\n" // rhsIndCounter = lhsInd
             "movq %[lhsValue],8(%[tempBuf],%[lhsInd],8)\n\t" // tempBuf[lhsInd+1]=0
             "L5%=:\n\t"
             "movq %[lhsValue],(%[tempBuf],%[rhsIndCounter],8)\n\t" // tempBuf[rhsIndCounter]=0
             "decq %[rhsIndCounter]\n\t" // --rhsIndCounter
             "jns L5%=\n\t" // if(rhsIndCounter >= 0) goto L5
             /* Inner loop 1: rhsIndCounter = [lhsInd, 0] */
             "movq %[lhsInd],%[rhsIndCounter]\n\t" // rhsIndCounter = lhsInd
             "movq %[kSizeM1],%[rhsInd]\n\t" // rhsInd = kSize-1
             "movq (%[lhs],%[lhsInd],8),%[lhsValue]\n\t" // lhsValue = lhs[lhsInd]
             "L3%=:\n\t"
             "movq (%[rhs],%[rhsInd],8),%%rax\n\t" // rax = rhs[rhsInd]
             "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
             "decq %[rhsInd]\n\t" // --rhsInd
             "addq %%rax,8(%[tempBuf],%[rhsIndCounter],8)\n\t" // tempBuf[rhsIndCounter+1] += rax
             "adcq %%rdx,(%[tempBuf],%[rhsIndCounter],8)\n\t" // tmpBuf[rhsIndCounter] = rdx
             "decq %[rhsIndCounter]\n\t" // -rhsIndCounter
             "jns L3%=\n\t" // if(rhsIndCounter >= 0) goto L3
             /* Inner loop 2: rhsIndCounter = [lhsInd, 0] */
             "movq %[lhsInd],%[rhsIndCounter]\n\t" // rhsIndCounter = lhsInd
             "clc\n"
             "L4%=:\n\t"
             "movq 8(%[tempBuf],%[rhsIndCounter],8),%%rax\n\t" // rax = tempBuf[rhsIndCounter+1]
             "adcq %%rax,(%[result],%[rhsIndCounter],8)\n\t" // result[rhsIndCounter] += rax
             "decq %[rhsIndCounter]\n\t" // --rhsIndCounter
             "jns L4%=\n\t" // if(rhsIndCounter >= 0) goto L4
             "decq %[lhsInd]\n\t" // --lhsInd
             "jns L2%=" // if(lhsInd >= 0) goto L2
             : "+m"(*(std::uint64_t(*)[kSize])result),
               "=m"(*(std::uint64_t(*)[kSize])tempBuffer),
               [lhsInd]"+r"(lhsInd), [rhsInd]"+r"(rhsInd),
               [rhsIndCounter]"=&r"(rhsIndCounter), [lhsValue]"+r"(lhsValue)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), [result]"r"(result),
               [tempBuf]"r"(tempBuffer), [kSizeM1]"i"(kSize - 1),
               "m"(mData), "m"(rhs.mData)
             : "rax", "rdx", "cc");
    }
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::fullMultiply(const WMPUInt<kSize>& rhs, std::uint64_t* result,
                                         std::uint64_t* tempBuffer) const
{
    static_assert(kSize == 2, "Not yet implemented");
    if constexpr(kSize == 2)
    {
        /* Multiplication of two 2-digit numbers (AB*CD) can be done with the algorithm:
             A*C = EF, B*D = GH, A*D = IJ, B*C = KL,
             AB*CD =
               EFGH
               +IJ0
               +KL0
        */
        std::uint64_t zero = 0;
        asm ("movq (%[rhs]), %%rax\n\t" // rax = rhs[0]
             "mulq (%[lhs])\n\t" // (rdx,rax) = lhs[0] * rax
             "movq %%rdx, (%[result])\n\t" // result[0] = rdx
             "movq %%rax, 8(%[result])\n\t" // result[1] = rax
             "movq 8(%[rhs]), %%rax\n\t" // rax = rhs[1]
             "mulq 8(%[lhs])\n\t" // (rdx,rax) = lhs[1] * rax
             "movq %%rdx, 16(%[result])\n\t" // result[2] = rdx
             "movq %%rax, 24(%[result])\n\t" // result[3] = rax
             "movq (%[rhs]), %%rax\n\t" // rax = rhs[0]
             "mulq 8(%[lhs])\n\t" // (rdx,rax) = lhs[1] * rax
             "addq %%rax, 16(%[result])\n\t" // result[2] += rax
             "adcq %%rdx, 8(%[result])\n\t" // result[1] += rdx
             "adcq %[zero], (%[result])\n\t" // result[0] += 0
             "movq 8(%[rhs]), %%rax\n\t" // rax = rhs[1]
             "mulq (%[lhs])\n\t" // (rdx,rax) = lhs[0] * rax
             "addq %%rax, 16(%[result])\n\t" // result[2] += rax
             "adcq %%rdx, 8(%[result])\n\t" // result[1] += rdx
             "adcq %[zero], (%[result])" // result[0] += 0
             : "=m"(*(std::uint64_t(*)[kSize])result)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), [result]"r"(result), [zero]"r"(zero),
               "m"(mData), "m"(rhs.mData)
             : "rax", "rdx", "cc");
    }
    else
    {
    }
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator*(const WMPUInt<kSize>& rhs) const
{
    WMPUInt<kSize> result;
    if constexpr(kSize == 2)
    {
        multiply(rhs, result.mData, nullptr);
    }
    else
    {
        std::uint64_t tempBuf[kSize];
        multiply(rhs, result.mData, tempBuf);
    }
    return result;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator*=(const WMPUInt<kSize>& rhs)
{
    /* The result of the multiplication has to be accumulated into a temporary buffer
       anyway, ie. it cannot be done "in-place", so this is essentially free: */
    *this = *this * rhs;
    return *this;
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::multiply(std::uint64_t rhs, std::uint64_t* result) const
{
    if constexpr(kSize == 2)
    {
        asm ("movq 8(%[lhs]),%%rax\n\t"
             "mulq %[rhs]\n\t"
             "movq %%rax,8(%[result])\n\t"
             "movq %%rdx,(%[result])\n\t"
             "imulq (%[lhs]),%[rhs]\n\t"
             "addq %[rhs],(%[result])"
             : "=m"(*(std::uint64_t(*)[kSize])result), [rhs]"+r"(rhs)
             : [lhs]"r"(mData), [result]"r"(result), "m"(mData)
             : "rax", "rdx", "cc");
    }
    else
    {
        for(std::size_t i = 0; i < kSize; ++i) result[i] = 0;
        std::uint64_t lhsInd = kSize-1;
        asm ("loop%=:\n\t"
             "movq (%[lhs],%[lhsInd],8),%%rax\n\t" // rax = lhs[lhsInd]
             "mulq %[rhs]\n\t" // (rdx,rax) = rax * rhs
             "addq %%rax,(%[result],%[lhsInd],8)\n\t" // result[lhsInd] += rax
             "adcq %%rdx,-8(%[result],%[lhsInd],8)\n\t" // result[lhsInd-1] = rdx
             "decq %[lhsInd]\n\t" // --lhsInd
             "jnz loop%=\n\t" // if(lhsInd > 0) goto loop
             "imulq (%[lhs]),%[rhs]\n\t" // rhs = lhs[0] * rhs
             "addq %[rhs],(%[result])" // result[0] += rhs
             : "+m"(*(std::uint64_t(*)[kSize])result), [rhs]"+r"(rhs), [lhsInd]"+r"(lhsInd)
             : [lhs]"r"(mData), [result]"r"(result), "m"(mData)
             : "rax", "rdx", "cc");
    }
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator*(std::uint64_t rhs) const
{
    WMPUInt<kSize> result;
    multiply(rhs, result.mData);
    return result;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator*=(std::uint64_t rhs)
{
    *this = *this * rhs;
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator*(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs * lhs;
}


//----------------------------------------------------------------------------
// Negation
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator-() const
{
    /* It's unlikely for there to exist a faster way of doing this.
       If there is, this may be changed to use that method. */
    WMPUInt<kSize> result;
    const std::uint64_t allBits = ~UINT64_C(0);
    for(std::size_t i = 0; i < kSize; ++i)
        result.mData[i] = mData[i] ^ allBits;
    ++result;
    return result;
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::neg()
{
    if constexpr(kSize == 2)
    {
        /* For kSize == 2, this can be done with 3 operations instead of 4. */
        asm ("negq 8(%[lhs])\n\t"
             "adcq $0, (%[lhs])\n\t"
             "negq (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData) : "cc");
    }
    else
    {
        /* It's unlikely for there to exist a faster way of doing this.
           If there is, this may be changed to use that method. */
        const std::uint64_t allBits = ~UINT64_C(0);
        for(std::size_t i = 0; i < kSize; ++i)
            mData[i] ^= allBits;
        ++*this;
    }
}


//----------------------------------------------------------------------------
// Optimized multiple addition
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline void WMPUInt<kSize>::addTo(WMPUInt<kSize>& target1, WMPUInt<kSize>& target2) const
{
#if !__ADX__
    target1 += *this;
    target2 += *this;
#else
    std::uint64_t tempReg1, tempReg2;

    if constexpr(kSize == 2)
        asm ("test %%al, %%al\n\t"
             "movq 8(%[target1]), %[tempReg1]\n\t"
             "movq 8(%[target2]), %[tempReg2]\n\t"
             "adcxq 8(%[self]), %[tempReg1]\n\t"
             "adoxq 8(%[self]), %[tempReg2]\n\t"
             "movq %[tempReg1], 8(%[target1])\n\t"
             "movq %[tempReg2], 8(%[target2])\n\t"
             "movq (%[target1]), %[tempReg1]\n\t"
             "movq (%[target2]), %[tempReg2]\n\t"
             "adcxq (%[self]), %[tempReg1]\n\t"
             "adoxq (%[self]), %[tempReg2]\n\t"
             "movq %[tempReg1], (%[target1])\n\t"
             "movq %[tempReg2], (%[target2])"
             : "+m"(target1.mData), "+m"(target2.mData),
               [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2)
             : [self]"r"(mData), [target1]"r"(target1.mData), [target2]"r"(target2.mData),
               "m"(mData) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t tempReg3, tempReg4;
        std::uint64_t dataInd = 8 * kSize;
        asm ("test %%al, %%al\n"
             "loop%=:\n\t"
             "leaq -16(%[dataInd]), %[dataInd]\n\t"
             "movq 8(%[target1],%[dataInd]), %[tempReg1]\n\t"
             "movq 8(%[target2],%[dataInd]), %[tempReg2]\n\t"
             "movq (%[target1],%[dataInd]), %[tempReg3]\n\t"
             "movq (%[target2],%[dataInd]), %[tempReg4]\n\t"
             "adcxq 8(%[self],%[dataInd]), %[tempReg1]\n\t"
             "adoxq 8(%[self],%[dataInd]), %[tempReg2]\n\t"
             "adcxq (%[self],%[dataInd]), %[tempReg3]\n\t"
             "adoxq (%[self],%[dataInd]), %[tempReg4]\n\t"
             "movq %[tempReg1], 8(%[target1],%[dataInd])\n\t"
             "movq %[tempReg2], 8(%[target2],%[dataInd])\n\t"
             "movq %[tempReg3], (%[target1],%[dataInd])\n\t"
             "movq %[tempReg4], (%[target2],%[dataInd])\n\t"
             "jrcxz done%=\n\t"
             "jmp loop%=\n"
             "done%=:"
             : "+m"(target1.mData), "+m"(target2.mData),
               [dataInd]"+c"(dataInd),
               [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2),
               [tempReg3]"=&r"(tempReg3), [tempReg4]"=&r"(tempReg4)
             : [self]"r"(mData), [target1]"r"(target1.mData), [target2]"r"(target2.mData),
               "m"(mData) : "cc");
    }
    else
    {
        std::uint64_t dataInd = 8 * kSize;
        asm ("test %%al, %%al\n"
             "loop%=:\n\t"
             "leaq -8(%[dataInd]), %[dataInd]\n\t"
             "movq (%[target1],%[dataInd]), %[tempReg1]\n\t"
             "movq (%[target2],%[dataInd]), %[tempReg2]\n\t"
             "adcxq (%[self],%[dataInd]), %[tempReg1]\n\t"
             "adoxq (%[self],%[dataInd]), %[tempReg2]\n\t"
             "movq %[tempReg1], (%[target1],%[dataInd])\n\t"
             "movq %[tempReg2], (%[target2],%[dataInd])\n\t"
             "jrcxz done%=\n\t"
             "jmp loop%=\n"
             "done%=:"
             : "+m"(target1.mData), "+m"(target2.mData),
               [dataInd]"+c"(dataInd), [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2)
             : [self]"r"(mData), [target1]"r"(target1.mData), [target2]"r"(target2.mData),
               "m"(mData) : "cc");
    }
#endif
}

template<std::size_t kSize>
inline void addWMPPair(WMPUInt<kSize>& lhs1, const WMPUInt<kSize>& rhs1,
                       WMPUInt<kSize>& lhs2, const WMPUInt<kSize>& rhs2)
{
#if !__ADX__
    lhs1 += rhs1;
    lhs2 += rhs2;
#else
    std::uint64_t tempReg1, tempReg2;

    if constexpr(kSize == 2)
        asm ("test %%al, %%al\n\t"
             "movq 8(%[lhs1]), %[tempReg1]\n\t"
             "movq 8(%[lhs2]), %[tempReg2]\n\t"
             "adcxq 8(%[rhs1]), %[tempReg1]\n\t"
             "adoxq 8(%[rhs2]), %[tempReg2]\n\t"
             "movq %[tempReg1], 8(%[lhs1])\n\t"
             "movq %[tempReg2], 8(%[lhs2])\n\t"
             "movq (%[lhs1]), %[tempReg1]\n\t"
             "movq (%[lhs2]), %[tempReg2]\n\t"
             "adcxq (%[rhs1]), %[tempReg1]\n\t"
             "adoxq (%[rhs2]), %[tempReg2]\n\t"
             "movq %[tempReg1], (%[lhs1])\n\t"
             "movq %[tempReg2], (%[lhs2])"
             : "+m"(*(std::uint64_t(*)[kSize])lhs1.data()),
               "+m"(*(std::uint64_t(*)[kSize])lhs2.data()),
               [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2)
             : [lhs1]"r"(lhs1.data()), [lhs2]"r"(lhs2.data()),
               [rhs1]"r"(rhs1.data()), [rhs2]"r"(rhs2.data()),
               "m"(*(std::uint64_t(*)[kSize])rhs1.data()),
               "m"(*(std::uint64_t(*)[kSize])rhs2.data())
             : "cc");
    else
    {
        std::uint64_t dataInd = 8 * kSize;
        asm ("test %%al, %%al\n"
             "loop%=:\n\t"
             "leaq -8(%[dataInd]), %[dataInd]\n\t"
             "movq (%[lhs1],%[dataInd]), %[tempReg1]\n\t"
             "movq (%[lhs2],%[dataInd]), %[tempReg2]\n\t"
             "adcxq (%[rhs1],%[dataInd]), %[tempReg1]\n\t"
             "adoxq (%[rhs2],%[dataInd]), %[tempReg2]\n\t"
             "movq %[tempReg1], (%[lhs1],%[dataInd])\n\t"
             "movq %[tempReg2], (%[lhs2],%[dataInd])\n\t"
             "jrcxz done%=\n\t"
             "jmp loop%=\n"
             "done%=:"
             : "+m"(*(std::uint64_t(*)[kSize])lhs1.data()),
               "+m"(*(std::uint64_t(*)[kSize])lhs2.data()),
               [dataInd]"+c"(dataInd), [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2)
             : [lhs1]"r"(lhs1.data()), [lhs2]"r"(lhs2.data()),
               [rhs1]"r"(rhs1.data()), [rhs2]"r"(rhs2.data()),
               "m"(*(std::uint64_t(*)[kSize])rhs1.data()),
               "m"(*(std::uint64_t(*)[kSize])rhs2.data())
             : "cc");
    }
#endif
}
#endif
