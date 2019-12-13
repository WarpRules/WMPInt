#ifndef WMPINT_INCLUDE_GUARD
#define WMPINT_INCLUDE_GUARD

#define WMPINT_VERSION 0x000601
#define WMPINT_VERSION_STRING "0.6.1"
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

    char* printAsDecStr(char* destination) const;
    char* printAsDecStr(char* destination, WMPUInt<kSize>& temp) const;
    char* printAsDecStrAndReset(char* destination);

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

    WMPUInt<kSize>& operator<<=(std::size_t);
    WMPUInt<kSize> operator<<(std::size_t) const;
    WMPUInt<kSize>& operator>>=(std::size_t);
    WMPUInt<kSize> operator>>(std::size_t) const;

    template<std::size_t> void shiftLeft();
    template<std::size_t> void shiftRight();

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
    WMPUInt<kSize>& operator/=(std::uint64_t);
    WMPUInt<kSize> operator/(std::uint64_t) const;
    WMPUInt<kSize>& operator%=(std::uint64_t);
    WMPUInt<kSize> operator%(std::uint64_t) const;

    WMPUInt<kSize> operator-() const;
    void neg();

    constexpr static std::size_t multiplyBufferSize();
    void multiply(const WMPUInt<kSize>&, WMPUInt<kSize>& result, std::uint64_t* tempBuffer) const;
    void multiply(std::uint64_t, WMPUInt<kSize>& result) const;

    template<std::size_t kSize2>
    constexpr static std::size_t fullMultiplyBufferSize();

    template<std::size_t kSize2>
    void fullMultiply(const WMPUInt<kSize2>&, WMPUInt<kSize+kSize2>& result,
                      std::uint64_t* tempBuffer) const;

    template<std::size_t kSize2>
    void fullMultiply(const WMPUInt<kSize2>&, WMPUInt<kSize+kSize2>& result) const;

    template<std::size_t kSize2>
    constexpr static std::size_t fullMultiply_longMultiplicationBufferSize();

    template<std::size_t kSize2>
    void fullMultiply_longMultiplication(const WMPUInt<kSize2>&, WMPUInt<kSize+kSize2>& result,
                                         std::uint64_t* tempBuffer) const;

    template<std::size_t kSize2>
    constexpr static std::size_t fullMultiply_karatsubaBufferSize();

    template<std::size_t kSize2>
    void fullMultiply_karatsuba(const WMPUInt<kSize2>&, WMPUInt<kSize+kSize2>& result,
                                std::uint64_t* tempBuffer) const;

    std::uint64_t divide(std::uint64_t, WMPUInt<kSize>& result) const;
    std::uint64_t modulo(std::uint64_t rhs) const;

    void addTo(WMPUInt<kSize>& target1, WMPUInt<kSize>& target2) const;


//----------------------------------------------------------------------------
 private:
//----------------------------------------------------------------------------
    std::uint64_t mData[kSize];

    void multiply(std::uint64_t, std::uint64_t* result) const;
    template<std::size_t> friend class WMPUInt;
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
    WMPUInt<1>& operator<<=(std::size_t bits) { mValue <<= bits; return *this; }
    WMPUInt<1> operator<<(std::size_t bits) const { return mValue << bits; }
    WMPUInt<1>& operator>>=(std::size_t bits) { mValue >>= bits; return *this; }
    WMPUInt<1> operator>>(std::size_t bits) const { return mValue >> bits; }
    template<std::size_t kBits> void shiftLeft() { mValue <<= kBits; }
    template<std::size_t kBits> void shiftRight() { mValue >>= kBits; }
    WMPUInt<1>& operator+=(const WMPUInt<1>& rhs) { mValue += rhs.mValue; return *this; }
    WMPUInt<1> operator+(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue + rhs.mValue); }
    WMPUInt<1>& operator-=(const WMPUInt<1>& rhs) { mValue -= rhs.mValue; return *this; }
    WMPUInt<1> operator-(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue - rhs.mValue); }
    WMPUInt<1>& operator++() { ++mValue; return *this; }
    WMPUInt<1>& operator--() { --mValue; return *this; }
    WMPUInt<1>& operator*=(const WMPUInt<1>& rhs) { mValue *= rhs.mValue; return *this; }
    WMPUInt<1> operator*(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue * rhs.mValue); }
    WMPUInt<1>& operator/=(const WMPUInt<1>& rhs) { mValue /= rhs.mValue; return *this; }
    WMPUInt<1> operator/(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue / rhs.mValue); }
    WMPUInt<1>& operator%=(const WMPUInt<1>& rhs) { mValue %= rhs.mValue; return *this; }
    WMPUInt<1> operator%(const WMPUInt<1>& rhs) const { return WMPUInt<1>(mValue % rhs.mValue); }
    WMPUInt<1> operator-() const { return WMPUInt<1>(-mValue); }
    void neg() { mValue = -mValue; }

    constexpr static std::size_t multiplyBufferSize() { return 1; }
    void multiply(const WMPUInt<1>& rhs, std::uint64_t* result, std::uint64_t*) const
    { *result = mValue * rhs.mValue; }
    void multiply(std::uint64_t rhs, std::uint64_t* result) const { *result = mValue * rhs; }

    template<std::size_t kSize2>
    constexpr static std::size_t fullMultiplyBufferSize() { return 1; }
    template<std::size_t kSize2>
    void fullMultiply(const WMPUInt<kSize2>&, WMPUInt<1+kSize2>& result,
                      std::uint64_t* tempBuffer) const;
    template<std::size_t kSize2>
    void fullMultiply(const WMPUInt<kSize2>&, WMPUInt<1+kSize2>& result) const;

    template<std::size_t kSize2>
    constexpr static std::size_t fullMultiply_longMultiplicationBufferSize() { return 1; }
    template<std::size_t kSize2> void fullMultiply_longMultiplication
    (const WMPUInt<kSize2>&, WMPUInt<1+kSize2>&, std::uint64_t*) const;

    template<std::size_t kSize2>
    constexpr static std::size_t fullMultiply_karatsubaBufferSize() { return 1; }
    template<std::size_t kSize2> void fullMultiply_karatsuba
    (const WMPUInt<kSize2>&, WMPUInt<1+kSize2>&, std::uint64_t*) const;

    std::uint64_t divide(std::uint64_t, WMPUInt<1>& result) const;
    std::uint64_t modulo(std::uint64_t rhs) const { return mValue % rhs; }

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

template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result) const
{
    if constexpr(kSize2 == 1)
        asm ("mulq %[rhs]"
             : "=a"(result.mData[1]), "=d"(result.mData[0])
             : "a"(mValue), [rhs]"rm"(rhs.mValue) : "cc");
    else if constexpr(kSize2 == 2)
    {
        result.mData[0] = 0;
        asm ("movq %[rhs1], %%rax\n\t"
             "mulq %[lhs]\n\t"
             "movq %%rax, 16(%[result])\n\t"
             "movq %%rdx, 8(%[result])\n\t"
             "movq %[rhs0], %%rax\n\t"
             "mulq %[lhs]\n\t"
             "addq %%rax, 8(%[result])\n\t"
             "adcq %%rdx, (%[result])\n\t"
             : "+m"(result.mData)
             : [lhs]"rm"(mValue), [result]"r"(result.mData),
               [rhs0]"rm"(rhs.mData[0]), [rhs1]"rm"(rhs.mData[1])
             : "rax", "rdx", "cc");
    }
    else
    {
        for(std::size_t i = 0; i < 1+kSize2; ++i) result.mData[i] = 0;
        std::uint64_t rhsInd = kSize2 - 1;
        asm ("L1%=:\n\t"
             "movq (%[rhs],%[rhsInd],8), %%rax\n\t"
             "mulq %[lhs]\n\t"
             "addq %%rax, 8(%[result],%[rhsInd],8)\n\t"
             "adcq %%rdx, (%[result],%[rhsInd],8)\n\t"
             "decq %[rhsInd]\n\t"
             "jns L1%="
             : "+m"(result.mData), [rhsInd]"+&r"(rhsInd)
             : "m"(rhs.mData), [lhs]"r"(mValue), [rhs]"r"(rhs.mData), [result]"r"(result.mData)
             : "rax", "rdx", "cc");
    }
}

template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply
(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result, std::uint64_t*) const
{
    fullMultiply(rhs, result);
}

template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply_longMultiplication
(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result, std::uint64_t*) const
{
    fullMultiply(rhs, result);
}

template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply_karatsuba
(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result, std::uint64_t*) const
{
    fullMultiply(rhs, result);
}

inline std::uint64_t WMPUInt<1>::divide(std::uint64_t rhs, WMPUInt<1>& result) const
{
    // Compilers are smart enough to optimize this. No need for inline asm.
    const std::uint64_t quotient = mValue / rhs, remainder = mValue % rhs;
    result.mValue = quotient;
    return remainder;
}

inline void WMPUInt<1>::addTo(WMPUInt<1>& target1, WMPUInt<1>& target2) const
{
    target1.mValue += mValue;
    target2.mValue += mValue;
}


//----------------------------------------------------------------------------
// Functions with large implementations.
//----------------------------------------------------------------------------
namespace WMPIntImplementations
{
    char* printAsDecStrAndReset(std::uint64_t*, std::size_t, char*);

    void doLongMultiplication
    (std::size_t, const std::uint64_t*, const std::uint64_t*, std::uint64_t*, std::uint64_t*);

    void doFullLongMultiplication
    (const std::uint64_t*, std::size_t, const std::uint64_t*, std::size_t,
     std::uint64_t*, std::uint64_t*);

    void doFullKaratsubaMultiplication
    (const std::uint64_t*, std::size_t, const std::uint64_t*, std::size_t,
     std::uint64_t*, std::uint64_t*);

    void doTruncatedKaratsubaMultiplication
    (const std::uint64_t*, const std::uint64_t*, std::size_t,
     std::uint64_t*, std::uint64_t*);

    constexpr std::size_t longMultiplicationBufferSize(std::size_t lhsSize)
    { return lhsSize; }

    constexpr std::size_t fullLongMultiplicationBufferSize(std::size_t, std::size_t rhsSize)
    { return rhsSize + 1; }

    constexpr std::size_t fullKaratsubaMultiplicationBufferSize(std::size_t, std::size_t);
    constexpr std::size_t fullKaratsubaMultiplicationBufferSizeForSameSizes(std::size_t);
    constexpr std::size_t fullKaratsubaMultiplicationBufferSizeForSmallLHS(std::size_t,std::size_t);
    constexpr std::size_t fullKaratsubaMultiplicationBufferSizeForLargeLHS(std::size_t,std::size_t);
    constexpr std::size_t truncatedKaratsubaMultiplicationBufferSize(std::size_t);
}

constexpr std::size_t WMPIntImplementations::fullKaratsubaMultiplicationBufferSizeForSameSizes
(std::size_t size)
{
    if(size <= 12) return fullLongMultiplicationBufferSize(size, size);
    const std::size_t lowSize = (size+1) / 2;
    const std::size_t highPlusLowSize = lowSize + 1;
    return highPlusLowSize*4 + fullKaratsubaMultiplicationBufferSizeForSameSizes(highPlusLowSize);
}

constexpr std::size_t WMPIntImplementations::fullKaratsubaMultiplicationBufferSizeForSmallLHS
(std::size_t lhsSize, std::size_t rhsSize)
{
    if(lhsSize <= 2 || rhsSize <= 12) return fullLongMultiplicationBufferSize(rhsSize, lhsSize);
    const std::size_t rhsLowSize = (rhsSize+1) / 2;
    const std::size_t rhsHighSize = rhsSize - rhsLowSize;
    const std::size_t bufferSize1 =
        fullKaratsubaMultiplicationBufferSize(lhsSize, rhsLowSize);
    const std::size_t z1Size = rhsHighSize + lhsSize;
    const std::size_t bufferSize2 = z1Size +
        (lhsSize <= rhsHighSize ?
         fullKaratsubaMultiplicationBufferSize(lhsSize, rhsHighSize) :
         fullKaratsubaMultiplicationBufferSize(rhsHighSize, lhsSize));
    return bufferSize1 > bufferSize2 ? bufferSize1 : bufferSize2;
}

constexpr std::size_t WMPIntImplementations::fullKaratsubaMultiplicationBufferSizeForLargeLHS
(std::size_t lhsSize, std::size_t rhsSize)
{
    if(rhsSize <= 12) return fullLongMultiplicationBufferSize(rhsSize, lhsSize);
    const std::size_t lowSize = (rhsSize+1) / 2;
    const std::size_t rhsHighSize = rhsSize - lowSize;
    const std::size_t lhsHighSize = lhsSize - lowSize;
    const std::size_t bufferSize1_1 =
        fullKaratsubaMultiplicationBufferSizeForSameSizes(lowSize);
    const std::size_t bufferSize1_2 =
        fullKaratsubaMultiplicationBufferSize(lhsHighSize, rhsHighSize);
    const std::size_t highPlusLowSize = lowSize + 1;
    const std::size_t bufferSize2 = highPlusLowSize*4 +
        fullKaratsubaMultiplicationBufferSizeForSameSizes(highPlusLowSize);
    const std::size_t bufferSize1 = bufferSize1_1 > bufferSize1_2 ? bufferSize1_1 : bufferSize1_2;
    return bufferSize2 > bufferSize1 ? bufferSize2 : bufferSize1;
}

constexpr std::size_t WMPIntImplementations::fullKaratsubaMultiplicationBufferSize
(std::size_t lhsSize, std::size_t rhsSize)
{
    if(lhsSize == rhsSize)
        return fullKaratsubaMultiplicationBufferSizeForSameSizes(lhsSize);
    else if(lhsSize <= (rhsSize+1)/2)
        return fullKaratsubaMultiplicationBufferSizeForSmallLHS(lhsSize, rhsSize);
    else
        return fullKaratsubaMultiplicationBufferSizeForLargeLHS(lhsSize, rhsSize);
}

constexpr std::size_t WMPIntImplementations::truncatedKaratsubaMultiplicationBufferSize
(std::size_t size)
{
    if(size <= 31) return longMultiplicationBufferSize(size);
    const std::size_t lhsLowSize = size / 2;
    const std::size_t lhsHighSize = size - lhsLowSize;
    const std::size_t rhsLowSize = (size + 1) / 2;
    const std::size_t size1 = fullKaratsubaMultiplicationBufferSize(lhsLowSize, rhsLowSize);
    const std::size_t size2 = lhsHighSize + truncatedKaratsubaMultiplicationBufferSize(lhsHighSize);
    const std::size_t size3 = lhsLowSize + truncatedKaratsubaMultiplicationBufferSize(lhsLowSize);
    const std::size_t size4 = size1 > size2 ? size1 : size2;
    return (size3 > size4 ? size3 : size4);
}

template<std::size_t kSize>
constexpr inline std::size_t WMPUInt<kSize>::multiplyBufferSize()
{
    if constexpr(kSize <= 31)
        return WMPIntImplementations::longMultiplicationBufferSize(kSize);
    else
        return WMPIntImplementations::truncatedKaratsubaMultiplicationBufferSize(kSize);
}

template<std::size_t kSize>
template<std::size_t kSize2>
constexpr inline std::size_t WMPUInt<kSize>::fullMultiplyBufferSize()
{
    if((kSize == kSize2 && kSize < 16) ||
       (kSize < 50 && kSize2 < kSize/2) ||
       (kSize2 < 50 && kSize < kSize2/2) ||
       (kSize >= 50 && kSize < 200 && kSize2 < kSize/3) ||
       (kSize2 >= 50 && kSize2 < 200 && kSize < kSize2/3) ||
       (kSize >= 200 && kSize2 < kSize/4) ||
       (kSize2 >= 200 && kSize < kSize2/4))
        return WMPIntImplementations::fullLongMultiplicationBufferSize(kSize, kSize2);
    else if(kSize <= kSize2)
        return WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kSize, kSize2);
    else
        return WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kSize2, kSize);
}

template<std::size_t kSize>
template<std::size_t kSize2>
constexpr inline std::size_t WMPUInt<kSize>::fullMultiply_longMultiplicationBufferSize()
{
    return WMPIntImplementations::fullLongMultiplicationBufferSize(kSize, kSize2);
}

template<std::size_t kSize>
template<std::size_t kSize2>
constexpr inline std::size_t WMPUInt<kSize>::fullMultiply_karatsubaBufferSize()
{
    if(kSize <= kSize2)
        return WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kSize, kSize2);
    else
        return WMPIntImplementations::fullKaratsubaMultiplicationBufferSize(kSize2, kSize);
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
inline const char* WMPUInt<kSize>::assignFromDecStr(const char* ptr)
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
                *this *= UINT64_C(10000000000000000000);
            }
            else
            {
                std::uint64_t factor = 10;
                for(unsigned i = 1; i < charCounter; ++i)
                    factor *= 10;
                *this *= factor;
            }

            *this += value;
        }
    }

    return ptr;
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

template<std::size_t kSize>
inline char* WMPUInt<kSize>::printAsDecStr(char* destination, WMPUInt<kSize>& temp) const
{
    temp = *this;
    return WMPIntImplementations::printAsDecStrAndReset
        (temp.mData, kSize, destination + (maxDecimalDigits() - 1));
}

template<std::size_t kSize>
inline char* WMPUInt<kSize>::printAsDecStr(char* destination) const
{
    WMPUInt<kSize> temp;
    return printAsDecStr(destination, temp);
}

template<std::size_t kSize>
inline char* WMPUInt<kSize>::printAsDecStrAndReset(char* destination)
{
    return WMPIntImplementations::printAsDecStrAndReset
        (mData, kSize, destination + (maxDecimalDigits() - 1));
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
// Shift operators
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator<<=(std::size_t bits)
{
    const std::size_t wordOffset = bits / 64;
    const std::size_t wordShiftBits = bits % 64;
    const std::size_t endInd = kSize - wordOffset;

    if(wordShiftBits == 0)
    {
        for(std::size_t i = 0; i < endInd; ++i) mData[i] = mData[i + wordOffset];
        for(std::size_t i = endInd; i < kSize; ++i) mData[i] = 0;
    }
    else
    {
        for(std::size_t destInd = 0, srcInd = wordOffset;
            destInd < endInd - 1; ++destInd, ++srcInd)
            mData[destInd] = ((mData[srcInd] << wordShiftBits) |
                              (mData[srcInd + 1] >> (64-wordShiftBits)));
        mData[endInd - 1] = mData[kSize - 1] << wordShiftBits;
        for(std::size_t i = endInd; i < kSize; ++i) mData[i] = 0;
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator<<(std::size_t bits) const
{
    WMPUInt<kSize> result;
    const std::size_t wordOffset = bits / 64;
    const std::size_t wordShiftBits = bits % 64;
    const std::size_t endInd = kSize - wordOffset;

    if(wordShiftBits == 0)
    {
        for(std::size_t i = 0; i < endInd; ++i) result.mData[i] = mData[i + wordOffset];
        for(std::size_t i = endInd; i < kSize; ++i) result.mData[i] = 0;
    }
    else
    {
        for(std::size_t destInd = 0, srcInd = wordOffset;
            destInd < endInd - 1; ++destInd, ++srcInd)
            result.mData[destInd] = ((mData[srcInd] << wordShiftBits) |
                                     (mData[srcInd + 1] >> (64-wordShiftBits)));
        result.mData[endInd - 1] = mData[kSize - 1] << wordShiftBits;
        for(std::size_t i = endInd; i < kSize; ++i) result.mData[i] = 0;
    }

    return result;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator>>=(std::size_t bits)
{
    const std::size_t wordOffset = bits / 64;
    const std::size_t wordShiftBits = bits % 64;
    const std::size_t endInd = wordOffset;

    if(wordShiftBits == 0)
    {
        for(std::size_t i = kSize; i-- > endInd;) mData[i] = mData[i - wordOffset];
        for(std::size_t i = 0; i < endInd; ++i) mData[i] = 0;
    }
    else
    {
        for(std::size_t destInd = kSize, srcInd = kSize-1-wordOffset;
            destInd-- > endInd + 1; --srcInd)
            mData[destInd] = ((mData[srcInd] >> wordShiftBits) |
                              (mData[srcInd - 1] << (64-wordShiftBits)));
        mData[endInd] = mData[0] >> wordShiftBits;
        for(std::size_t i = 0; i < endInd; ++i) mData[i] = 0;
    }
    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator>>(std::size_t bits) const
{
    WMPUInt<kSize> result;
    const std::size_t wordOffset = bits / 64;
    const std::size_t wordShiftBits = bits % 64;
    const std::size_t endInd = wordOffset;

    if(wordShiftBits == 0)
    {
        for(std::size_t i = kSize; i-- > endInd;) result.mData[i] = mData[i - wordOffset];
        for(std::size_t i = 0; i < endInd; ++i) result.mData[i] = 0;
    }
    else
    {
        for(std::size_t destInd = kSize, srcInd = kSize-1-wordOffset;
            destInd-- > endInd + 1; --srcInd)
            result.mData[destInd] = ((mData[srcInd] >> wordShiftBits) |
                                     (mData[srcInd - 1] << (64-wordShiftBits)));
        result.mData[endInd] = mData[0] >> wordShiftBits;
        for(std::size_t i = 0; i < endInd; ++i) result.mData[i] = 0;
    }
    return result;
}

template<std::size_t kSize>
template<std::size_t kBits>
inline void WMPUInt<kSize>::shiftLeft()
{
    if constexpr(kBits == 0) {}
    else if constexpr(kBits > kSize*64)
    {
        for(std::size_t i = 0; i < kSize; ++i) mData[i] = 0;
    }
    else
    {
        constexpr std::size_t wordOffset = kBits / 64;
        constexpr std::size_t wordShiftBits = kBits % 64;
        constexpr std::size_t endInd = kSize - wordOffset;
        if constexpr(wordShiftBits == 0)
        {
            for(std::size_t i = 0; i < endInd; ++i) mData[i] = mData[i + wordOffset];
            for(std::size_t i = endInd; i < kSize; ++i) mData[i] = 0;
        }
        else if constexpr(endInd == kSize)
        {
            for(std::size_t i = 0; i < kSize - 1; ++i)
                mData[i] = (mData[i] << wordShiftBits) | (mData[i+1] >> (64-wordShiftBits));
            mData[kSize - 1] <<= wordShiftBits;
        }
        else
        {
            for(std::size_t destInd = 0, srcInd = wordOffset;
                destInd < endInd - 1; ++destInd, ++srcInd)
                mData[destInd] = ((mData[srcInd] << wordShiftBits) |
                                  (mData[srcInd + 1] >> (64-wordShiftBits)));
            mData[endInd - 1] = mData[kSize - 1] << wordShiftBits;
            for(std::size_t i = endInd; i < kSize; ++i) mData[i] = 0;
        }
    }
}

template<std::size_t kSize>
template<std::size_t kBits>
inline void WMPUInt<kSize>::shiftRight()
{
    if constexpr(kBits == 0) {}
    else if constexpr(kBits > kSize*64)
    {
        for(std::size_t i = 0; i < kSize; ++i) mData[i] = 0;
    }
    else
    {
        constexpr std::size_t wordOffset = kBits / 64;
        constexpr std::size_t wordShiftBits = kBits % 64;
        constexpr std::size_t endInd = wordOffset;
        if constexpr(wordShiftBits == 0)
        {
            for(std::size_t i = kSize; i-- > endInd;) mData[i] = mData[i - wordOffset];
            for(std::size_t i = 0; i < endInd; ++i) mData[i] = 0;
        }
        else if constexpr(endInd == 0)
        {
            for(std::size_t i = kSize; i-- > 1;)
                mData[i] = (mData[i] >> wordShiftBits) | (mData[i-1] << (64-wordShiftBits));
            mData[0] >>= wordShiftBits;
        }
        else
        {
            for(std::size_t destInd = kSize, srcInd = kSize-1-wordOffset;
                destInd-- > endInd + 1; --srcInd)
                mData[destInd] = ((mData[srcInd] >> wordShiftBits) |
                                  (mData[srcInd - 1] << (64-wordShiftBits)));
            mData[endInd] = mData[0] >> wordShiftBits;
            for(std::size_t i = 0; i < endInd; ++i) mData[i] = 0;
        }
    }
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
             : "+m"(mData), [dataInd]"+&c"(dataInd),
               [tempReg1]"=&r"(tempReg), [tempReg2]"=&r"(tempReg2)
             : [lhsHalf]"r"(lhsHalfPoint), [rhsHalf]"r"(rhsHalfPoint),
               [lhsBegin]"r"(mData), [rhsBegin]"r"(rhs.mData), [kSizeP2]"i"(kSize/2),
               "m"(rhs.mData) : "cc");
    }
#endif
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm ("movq 16(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "addq %[tempReg], 16(%[lhs],%[dataInd],8)\n\t"
             "movq 8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "adcq %[tempReg], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "movq (%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "adcq %[tempReg], (%[lhs],%[dataInd],8)\n\t"
             "movq -8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "adcq %[tempReg], -8(%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%="
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm ("movq 8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "addq %[tempReg], 8(%[lhs],%[dataInd],8)\n\t"
             "loop%=:\n\t"
             "movq (%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "adcq %[tempReg], (%[lhs],%[dataInd],8)\n\t"
             "movq -8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "adcq %[tempReg], -8(%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%="
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
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
             : "+m"(mData), [dataInd]"+&r"(dataInd)
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
             : "+m"(mData), [dataInd]"+&r"(dataInd)
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
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm ("movq 16(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "subq %[tempReg], 16(%[lhs],%[dataInd],8)\n\t"
             "movq 8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "sbbq %[tempReg], 8(%[lhs],%[dataInd],8)\n"
             "loop%=:\n\t"
             "movq (%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "sbbq %[tempReg], (%[lhs],%[dataInd],8)\n\t"
             "movq -8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "sbbq %[tempReg], -8(%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%="
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm ("movq 8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "subq %[tempReg], 8(%[lhs],%[dataInd],8)\n\t"
             "loop%=:\n\t"
             "movq (%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "sbbq %[tempReg], (%[lhs],%[dataInd],8)\n\t"
             "movq -8(%[rhs],%[dataInd],8), %[tempReg]\n\t"
             "sbbq %[tempReg], -8(%[lhs],%[dataInd],8)\n\t"
             "decq %[dataInd]\n\t"
             "decq %[dataInd]\n\t"
             "jns loop%="
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
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
             : "+m"(mData), [dataInd]"+&r"(dataInd)
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
             : "+m"(mData), [dataInd]"+&r"(dataInd)
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
(const WMPUInt<kSize>& rhs, WMPUInt<kSize>& result, std::uint64_t* tempBuffer) const
{
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
             : "+m"(result.mData), [tmp1]"=&r"(tmp1), [tmp2]"=&r"(tmp2)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), [result]"r"(result.mData),
               "m"(mData), "m"(rhs.mData)
             : "rax", "rdx", "cc");
    }
    else if constexpr(kSize <= 31)
        WMPIntImplementations::doLongMultiplication
            (kSize, mData, rhs.mData, result.mData, tempBuffer);
    else
        WMPIntImplementations::doTruncatedKaratsubaMultiplication
            (mData, rhs.mData, kSize, result.mData, tempBuffer);
}

template<std::size_t kSize>
template<std::size_t kSize2>
inline void WMPUInt<kSize>::fullMultiply
(const WMPUInt<kSize2>& rhs, WMPUInt<kSize+kSize2>& result, std::uint64_t* tempBuffer) const
{
    if constexpr(kSize2 == 1)
        rhs.fullMultiply(*this, result);
    else if constexpr(kSize == 2 && kSize2 == 2)
    {
        /* Multiplication of two 2-digit numbers (AB*CD) can be done with the algorithm:
             A*C = EF, B*D = GH, A*D = IJ, B*C = KL,
             AB*CD =
               EFGH
               +IJ0
               +KL0
           (This is about 4 times faster than calling doFullLongMultiplication().)
        */
        std::uint64_t zero = 0;
        asm ("movq %[rhs0], %%rax\n\t" // rax = rhs[0]
             "mulq %[lhs0]\n\t" // (rdx,rax) = lhs[0] * rax
             "movq %%rdx, (%[result])\n\t" // result[0] = rdx
             "movq %%rax, 8(%[result])\n\t" // result[1] = rax
             "movq %[rhs1], %%rax\n\t" // rax = rhs[1]
             "mulq %[lhs1]\n\t" // (rdx,rax) = lhs[1] * rax
             "movq %%rdx, 16(%[result])\n\t" // result[2] = rdx
             "movq %%rax, 24(%[result])\n\t" // result[3] = rax
             "movq %[rhs0], %%rax\n\t" // rax = rhs[0]
             "mulq %[lhs1]\n\t" // (rdx,rax) = lhs[1] * rax
             "addq %%rax, 16(%[result])\n\t" // result[2] += rax
             "adcq %%rdx, 8(%[result])\n\t" // result[1] += rdx
             "adcq %[zero], (%[result])\n\t" // result[0] += 0
             "movq %[rhs1], %%rax\n\t" // rax = rhs[1]
             "mulq %[lhs0]\n\t" // (rdx,rax) = lhs[0] * rax
             "addq %%rax, 16(%[result])\n\t" // result[2] += rax
             "adcq %%rdx, 8(%[result])\n\t" // result[1] += rdx
             "adcq %[zero], (%[result])" // result[0] += 0
             : "=m"(result.mData)
             : [lhs0]"rm"(mData[0]), [lhs1]"rm"(mData[1]),
               [rhs0]"rm"(rhs.mData[0]), [rhs1]"rm"(rhs.mData[1]),
               [result]"r"(result.mData), [zero]"r"(zero)
             : "rax", "rdx", "cc");
    }
    else if constexpr((kSize == kSize2 && kSize < 16) ||
                      (kSize < 50 && kSize2 < kSize/2) ||
                      (kSize2 < 50 && kSize < kSize2/2) ||
                      (kSize >= 50 && kSize < 200 && kSize2 < kSize/3) ||
                      (kSize2 >= 50 && kSize2 < 200 && kSize < kSize2/3) ||
                      (kSize >= 200 && kSize2 < kSize/4) ||
                      (kSize2 >= 200 && kSize < kSize2/4))
    {
        WMPIntImplementations::doFullLongMultiplication
            (mData, kSize, rhs.mData, kSize2, result.mData, tempBuffer);
    }
    else if constexpr(kSize <= kSize2)
    {
        WMPIntImplementations::doFullKaratsubaMultiplication
            (mData, kSize, rhs.mData, kSize2, result.mData, tempBuffer);
    }
    else
    {
        WMPIntImplementations::doFullKaratsubaMultiplication
            (rhs.mData, kSize2, mData, kSize, result.mData, tempBuffer);
    }
}

template<std::size_t kSize>
template<std::size_t kSize2>
inline void WMPUInt<kSize>::fullMultiply
(const WMPUInt<kSize2>& rhs, WMPUInt<kSize+kSize2>& result) const
{
    if constexpr(kSize2 == 1)
        rhs.fullMultiply(*this, result);
    else
    {
        std::uint64_t tempBuf[fullMultiplyBufferSize<kSize2>()];
        fullMultiply(rhs, result, tempBuf);
    }
}

template<std::size_t kSize>
template<std::size_t kSize2>
inline void WMPUInt<kSize>::fullMultiply_longMultiplication
(const WMPUInt<kSize2>& rhs, WMPUInt<kSize+kSize2>& result, std::uint64_t* tempBuffer) const
{
    if constexpr(kSize2 == 1)
        rhs.fullMultiply(*this, result);
    else
        WMPIntImplementations::doFullLongMultiplication
            (mData, kSize, rhs.mData, kSize2, result.mData, tempBuffer);
}

template<std::size_t kSize>
template<std::size_t kSize2>
inline void WMPUInt<kSize>::fullMultiply_karatsuba
(const WMPUInt<kSize2>& rhs, WMPUInt<kSize+kSize2>& result, std::uint64_t* tempBuffer) const
{
    if constexpr(kSize2 == 1)
        rhs.fullMultiply(*this, result);
    else if constexpr(kSize <= kSize2)
    {
        WMPIntImplementations::doFullKaratsubaMultiplication
            (mData, kSize, rhs.mData, kSize2, result.mData, tempBuffer);
    }
    else
    {
        WMPIntImplementations::doFullKaratsubaMultiplication
            (rhs.mData, kSize2, mData, kSize, result.mData, tempBuffer);
    }
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator*(const WMPUInt<kSize>& rhs) const
{
    WMPUInt<kSize> result;
    if constexpr(kSize == 2)
    {
        multiply(rhs, result, nullptr);
    }
    else
    {
        std::uint64_t tempBuf[multiplyBufferSize()];
        multiply(rhs, result, tempBuf);
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
             : "=m"(*(std::uint64_t(*)[kSize])result), [rhs]"+&r"(rhs)
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
             "adcq %%rdx,-8(%[result],%[lhsInd],8)\n\t" // result[lhsInd-1] += rdx
             "decq %[lhsInd]\n\t" // --lhsInd
             "jnz loop%=\n\t" // if(lhsInd > 0) goto loop
             "imulq (%[lhs]),%[rhs]\n\t" // rhs = lhs[0] * rhs
             "addq %[rhs],(%[result])" // result[0] += rhs
             : "+m"(*(std::uint64_t(*)[kSize])result), [rhs]"+&r"(rhs), [lhsInd]"+&r"(lhsInd)
             : [lhs]"r"(mData), [result]"r"(result), "m"(mData)
             : "rax", "rdx", "cc");
    }
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::multiply(std::uint64_t rhs, WMPUInt<kSize>& result) const
{
    multiply(rhs, result.data());
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
    if constexpr(kSize == 2)
    {
        std::uint64_t lhs0 = mData[0], lhs1 = mData[1];
        asm ("movq %[lhs1], %%rax\n\t"
             "mulq %[rhs]\n\t"
             "movq %%rax, 8(%[result])\n\t"
             "movq %%rdx, (%[result])\n\t"
             "imulq %[lhs0], %[rhs]\n\t"
             "addq %[rhs], (%[result])"
             : "=m"(mData), [rhs]"+&r"(rhs)
             : [lhs0]"r"(lhs0), [lhs1]"r"(lhs1), [result]"r"(mData)
             : "rax", "rdx", "cc");
    }
    else
    {
        std::uint64_t lhsInd = kSize-1, zero = 0, tempValue = mData[kSize-1];
        mData[kSize-1] = 0;
        asm ("loop%=:\n\t"
             "movq %[tempValue], %%rax\n\t" // rax = tempValue
             "movq -8(%[lhs],%[lhsInd],8), %[tempValue]\n\t" // tempValue = lhs[lhsInd-1]
             "movq %[zero], -8(%[lhs],%[lhsInd],8)\n\t" // lhs[lhsInd-1] = zero
             "mulq %[rhs]\n\t" // (rdx,rax) = rax * rhs
             "addq %%rax, (%[lhs],%[lhsInd],8)\n\t" // lhs[lhsInd] += rax
             "adcq %%rdx, -8(%[lhs],%[lhsInd],8)\n\t" // lhs[lhsInd-1] += rdx
             "decq %[lhsInd]\n\t" // --lhsInd
             "jnz loop%=\n\t" // if(lhsInd > 0) goto loop
             "imulq %[tempValue], %[rhs]\n\t" // rhs = tempValue * rhs
             "addq %[rhs], (%[lhs])" // lhs[0] += rhs
             : "+m"(mData), [rhs]"+&r"(rhs), [lhsInd]"+&r"(lhsInd), [tempValue]"+&r"(tempValue)
             : [lhs]"r"(mData), [zero]"r"(zero)
             : "rax", "rdx", "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> operator*(std::uint64_t lhs, const WMPUInt<kSize>& rhs)
{
    return rhs * lhs;
}


//----------------------------------------------------------------------------
// Division
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline std::uint64_t WMPUInt<kSize>::divide(std::uint64_t rhs, WMPUInt<kSize>& result) const
{
    std::uint64_t remainder;
    if constexpr(kSize == 2)
    {
        asm ("xorl %%edx, %%edx\n\t"
             "movq (%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, (%[result])\n\t"
             "movq 8(%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, 8(%[result])"
             : "=m"(result.mData), "=&d"(remainder)
             : "m"(mData), [lhs]"r"(mData), [rhs]"rm"(rhs), [result]"r"(result.mData)
             : "rax", "cc");
    }
    else
    {
        std::size_t counter = kSize, lhsIndex = 0;
        asm ("xorl %%edx, %%edx\n"
             "L1%=:\n\t"
             "movq (%[lhs], %[lhsIndex]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, (%[result], %[lhsIndex])\n\t"
             "leaq 8(%[lhsIndex]), %[lhsIndex]\n\t"
             "decq %[counter]\n\t"
             "jnz L1%="
             : "=m"(result.mData), "=&d"(remainder),
               [lhsIndex]"+&r"(lhsIndex), [counter]"+&r"(counter)
             : "m"(mData), [lhs]"r"(mData), [rhs]"r"(rhs), [result]"r"(result.mData)
             : "rax", "cc");
    }
    return remainder;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator/=(std::uint64_t rhs)
{
    if constexpr(kSize == 2)
    {
        asm ("xorl %%edx, %%edx\n\t"
             "movq (%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, (%[lhs])\n\t"
             "movq 8(%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, 8(%[lhs])"
             : "+m"(mData)
             : [lhs]"r"(mData), [rhs]"rm"(rhs)
             : "rax", "rdx", "cc");
    }
    else
    {
        std::uint64_t* lhs = mData;
        std::size_t counter = kSize;
        asm ("xorl %%edx, %%edx\n"
             "L1%=:\n\t"
             "movq (%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, (%[lhs])\n\t"
             "leaq 8(%[lhs]), %[lhs]\n\t"
             "decq %[counter]\n\t"
             "jnz L1%="
             : "+m"(mData), [lhs]"+&r"(lhs), [counter]"+&r"(counter)
             : [rhs]"r"(rhs) : "rax", "rdx", "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator/(std::uint64_t rhs) const
{
    WMPUInt<kSize> result;
    divide(rhs, result);
    return result;
}


//----------------------------------------------------------------------------
// Modulo
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline std::uint64_t WMPUInt<kSize>::modulo(std::uint64_t rhs) const
{
    // Since we only need the remainder, we don't need to store the result anywhere
    std::uint64_t remainder;
    if constexpr(kSize == 2)
    {
        asm ("xorl %%edx, %%edx\n\t"
             "movq (%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq 8(%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             : "=&d"(remainder)
             : "m"(mData), [lhs]"r"(mData), [rhs]"rm"(rhs)
             : "rax", "cc");
    }
    else
    {
        const std::uint64_t* lhs = mData;
        std::size_t counter = kSize;
        asm ("xorl %%edx, %%edx\n"
             "L1%=:\n\t"
             "movq (%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "leaq 8(%[lhs]), %[lhs]\n\t"
             "decq %[counter]\n\t"
             "jnz L1%="
             : "=&d"(remainder), [lhs]"+&r"(lhs), [counter]"+&r"(counter)
             : "m"(mData), [rhs]"r"(rhs)
             : "rax", "cc");
    }
    return remainder;
}

template<std::size_t kSize>
WMPUInt<kSize>& WMPUInt<kSize>::operator%=(std::uint64_t rhs)
{
    assign(modulo(rhs));
    return *this;
}

template<std::size_t kSize>
WMPUInt<kSize> WMPUInt<kSize>::operator%(std::uint64_t rhs) const
{
    return WMPUInt<kSize>(modulo(rhs));
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
               [dataInd]"+&c"(dataInd),
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
               [dataInd]"+&c"(dataInd), [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2)
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
               [dataInd]"+&c"(dataInd), [tempReg1]"=&r"(tempReg1), [tempReg2]"=&r"(tempReg2)
             : [lhs1]"r"(lhs1.data()), [lhs2]"r"(lhs2.data()),
               [rhs1]"r"(rhs1.data()), [rhs2]"r"(rhs2.data()),
               "m"(*(std::uint64_t(*)[kSize])rhs1.data()),
               "m"(*(std::uint64_t(*)[kSize])rhs2.data())
             : "cc");
    }
#endif
}
#endif
