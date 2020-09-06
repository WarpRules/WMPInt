//----------------------------------------------------------------------------
// WMPUInt<1> specializations
//----------------------------------------------------------------------------
template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result) const
{
    if constexpr(kSize2 == 1)
        asm (""
             ::: "cc");
    else if constexpr(kSize2 == 2)
    {
        result.mData[0] = 0;
        asm (""
             : "+m"(result.mData)
             : [lhs]"rm"(mValue), [result]"r"(result.mData),
               [rhs0]"rm"(rhs.mData[0]), [rhs1]"rm"(rhs.mData[1])
             : "cc");
    }
    else
    {
        for(std::size_t i = 0; i < 1+kSize2; ++i) result.mData[i] = 0;
        std::uint64_t rhsInd = kSize2 - 1;
        asm (""
             : "+m"(result.mData), [rhsInd]"+&r"(rhsInd)
             : "m"(rhs.mData), [lhs]"r"(mValue), [rhs]"r"(rhs.mData), [result]"r"(result.mData)
             : "cc");
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
        asm (""
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 3)
        asm (""
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 4)
        asm (""
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator+=(std::uint64_t value)
{
    const std::uint64_t zero = 0;
    if constexpr(kSize == 2)
    {
        asm ("adds %[lhs1], %[lhs1], %[rhs]\n\t"
             "adc %[lhs0], %[lhs0], %[zero]"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1])
             : [rhs]"r"(value), [zero]"r"(zero) : "cc");
    }
    else if constexpr(kSize == 3)
        asm (""
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value) : "cc");
    else if constexpr(kSize == 4)
        asm (""
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value) : "cc");
    }

    return *this;
}


//----------------------------------------------------------------------------
// Subtraction
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(const WMPUInt<kSize>& rhs)
{
    std::uint64_t tempReg;

    if constexpr(kSize == 2)
        asm (""
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 3)
        asm (""
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize == 4)
        asm (""
             : "+m"(mData), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd), [tempReg]"=&r"(tempReg)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), "m"(rhs.mData) : "cc");
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(std::uint64_t value)
{
    std::uint64_t zero = 0;

    if constexpr(kSize == 2)
        asm (""
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize == 3)
        asm (""
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize == 4)
        asm (""
             : "+m"(mData) : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t dataInd = kSize - 3;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    }
    else
    {
        std::uint64_t dataInd = kSize - 2;
        asm (""
             : "+m"(mData), [dataInd]"+&r"(dataInd)
             : [lhs]"r"(mData), [value]"r"(value), [zero]"r"(zero) : "cc");
    }

    return *this;
}


//----------------------------------------------------------------------------
// Multiplication
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline void WMPUInt<kSize>::multiply_size2
(const WMPUInt<kSize>& rhs, WMPUInt<kSize>& result) const
{
    if constexpr(kSize == 2)
    {
        /* The case of kSize==2 is simple enough to warrant its own loopless implementation */
        std::uint64_t tmp;
        asm (""
             : "+m"(result.mData), [tmp]"=&r"(tmp)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), [result]"r"(result.mData),
               "m"(mData), "m"(rhs.mData)
             : "cc");
    }
}

template<std::size_t kSize>
template<std::size_t kSize2>
inline void WMPUInt<kSize>::fullMultiply_size2
(const WMPUInt<kSize2>& rhs, WMPUInt<kSize+kSize2>& result) const
{
    if constexpr(kSize == 2 && kSize2 == 2)
    {
        /* Multiplication of two 2-digit numbers (AB*CD) can be done with the algorithm:
             A*C = EF, B*D = GH, A*D = IJ, B*C = KL,
             AB*CD =
               EFGH
              + IJ
              + KL
           (This is about 4 times faster than calling doFullLongMultiplication().)
        */
        asm (""
             : "=m"(result.mData)
             : [lhs0]"rm"(mData[0]), [lhs1]"rm"(mData[1]),
               [rhs0]"rm"(rhs.mData[0]), [rhs1]"rm"(rhs.mData[1]),
               [result]"r"(result.mData)
             : "cc");
    }
    else if constexpr(kSize == 2)
    {
        /* Specialization for kSize == 2.
           In essence this does short multiplication on 128-bit values.
           This is almost three times faster than calling doFullLongMultiplication().
        */
        for(std::size_t i = 0; i < kSize+kSize2; ++i) result.mData[i] = 0;
        std::uint64_t *targetPtr = result.mData + (kSize+kSize2-4);
        std::uint64_t temp0, temp1;
        std::size_t rhsIndex = kSize2 - 1;
        asm (""
             : "=m"(result.mData), [result]"+&r"(targetPtr), [rhsIndex]"+&r"(rhsIndex),
               [temp0]"=&r"(temp0), [temp1]"=&r"(temp1)
             : "m"(rhs.mData), [lhs0]"rm"(mData[0]), [lhs1]"rm"(mData[1]),
               [rhs]"r"(rhs.mData)
             : "cc");

        if constexpr(kSize2 % 2 == 1)
        {
            /* With an odd rhs size, we need to take care of the most significant word separately.

                   ABBCCDD          JJJ ->   0A            JJJ ->   A
                      * EF                 * EF                  * EF
               -----------                 ----                   ---
               +      GGGG = EF*DD         00KK = (E*0|F*A)        KK = F*A
               +    HHHH   = EF*CC        + LL  = E*A           + LL  = E*A
               +  IIII     = EF*BB        + 00  = F*0
               + JJJ       = EF*A
            */
            asm (""
                 : "=m"(result.mData)
                 : [lhs0]"rm"(mData[0]), [lhs1]"rm"(mData[1]),
                   [rhs0]"rm"(rhs.mData[0]), [result]"r"(result.mData)
                 : "cc");
        }
    }
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::multiply(std::uint64_t rhs, std::uint64_t* result) const
{
    if constexpr(kSize == 2)
    {
        asm (""
             : "=m"(*(std::uint64_t(*)[kSize])result), [rhs]"+&r"(rhs)
             : [lhs]"r"(mData), [result]"r"(result), "m"(mData)
             : "cc");
    }
    else
    {
        for(std::size_t i = 0; i < kSize; ++i) result[i] = 0;
        std::uint64_t lhsInd = kSize-1;
        asm (""
             : "+m"(*(std::uint64_t(*)[kSize])result), [rhs]"+&r"(rhs), [lhsInd]"+&r"(lhsInd)
             : [lhs]"r"(mData), [result]"r"(result), "m"(mData)
             : "cc");
    }
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator*=(std::uint64_t rhs)
{
    if constexpr(kSize == 2)
    {
        std::uint64_t tmp;
        asm ("umulh %[tmp], %[lhs1], %[rhs]\n\t"
             "mul %[lhs1], %[lhs1], %[rhs]\n\t"
             "madd %[lhs0], %[lhs0], %[rhs], %[tmp]"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1]), [tmp]"=&r"(tmp)
             : [rhs]"r"(rhs) :);
    }
    else
    {
        std::uint64_t* lhs = mData + (kSize - 1);
        std::uint64_t res0 = 0, res1, tmp, mulResL, mulResH;
        asm ("loop%=:\n\t"
             "mov %[res1], %[res0]\n\t" // res1 = res0
             "ldr %[tmp], [%[lhs]], #-8\n\t" // tmp = *lhs; lhs -= 8
             "mov %[res0], #0\n\t" // res0 = 0
             "mul %[mulResL], %[tmp], %[rhs]\n\t" // mulResL = tmp * rhs (low)
             "umulh %[mulResH], %[tmp], %[rhs]\n\t" // mulResH = tmp * rhs (high)
             "adds %[res1], %[res1], %[mulResL]\n\t" // res1 += mulResL
             "adc %[res0], %[res0], %[mulResH]\n\t" // res0 += mulResH
             "str %[res1], [%[lhs], #8]\n\t" // *(lhs+8) = res1
             "cmp %[lhs], %[mData]\n\t" // if(lhs != mData)
             "bne loop%=\n\t" // goto loop
             "ldr %[tmp], [%[lhs]]\n\t" // tmp = *lhs
             "madd %[res0], %[tmp], %[rhs], %[res0]\n\t" // res0 = tmp * rhs + res0
             "str %[res0], [%[lhs]]" // *lhs = res0
             : "+m"(mData), [rhs]"+&r"(rhs), [lhs]"+&r"(lhs),
               [res0]"+%r"(res0), [res1]"=&r"(res1), [tmp]"=&r"(tmp),
               [mulResL]"=&r"(mulResL), [mulResH]"=&r"(mulResH)
             : [mData]"r"(mData) : "cc");
    }

    return *this;
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
        asm (""
             : "=m"(result.mData)/*, "=&d"(remainder)*/
             : "m"(mData), [lhs]"r"(mData), [rhs]"rm"(rhs), [result]"r"(result.mData)
             : "cc");
    }
    else
    {
        std::size_t counter = kSize, lhsIndex = 0;
        asm (""
             : "=m"(result.mData), /*"=&d"(remainder),*/
               [lhsIndex]"+&r"(lhsIndex), [counter]"+&r"(counter)
             : "m"(mData), [lhs]"r"(mData), [rhs]"r"(rhs), [result]"r"(result.mData)
             : "cc");
    }
    return remainder;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator/=(std::uint64_t rhs)
{
    if constexpr(kSize == 2)
    {
        asm (""
             : "+m"(mData)
             : [lhs]"r"(mData), [rhs]"rm"(rhs)
             : "cc");
    }
    else
    {
        std::uint64_t* lhs = mData;
        std::size_t counter = kSize;
        asm (""
             : "+m"(mData), [lhs]"+&r"(lhs), [counter]"+&r"(counter)
             : [rhs]"r"(rhs) : "cc");
    }

    return *this;
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
        asm (""
             : /*"=&d"(remainder)*/
             : "m"(mData), [lhs]"r"(mData), [rhs]"rm"(rhs)
             : "cc");
    }
    else
    {
        const std::uint64_t* lhs = mData;
        std::size_t counter = kSize;
        asm (""
             : /*"=&d"(remainder),*/ [lhs]"+&r"(lhs), [counter]"+&r"(counter)
             : "m"(mData), [rhs]"r"(rhs)
             : "cc");
    }
    return remainder;
}


//----------------------------------------------------------------------------
// Negation
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline void WMPUInt<kSize>::neg_size2()
{
    if constexpr(kSize == 2)
    {
        /* For kSize == 2, this can be done with 3 operations instead of 4. */
        asm (""
             : "+m"(mData) : [lhs]"r"(mData) : "cc");
    }
}


//----------------------------------------------------------------------------
// Optimized multiple addition
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline void WMPUInt<kSize>::addTo(WMPUInt<kSize>& target1, WMPUInt<kSize>& target2) const
{
    target1 += *this;
    target2 += *this;
}

template<std::size_t kSize>
inline void addWMPPair(WMPUInt<kSize>& lhs1, const WMPUInt<kSize>& rhs1,
                       WMPUInt<kSize>& lhs2, const WMPUInt<kSize>& rhs2)
{
    lhs1 += rhs1;
    lhs2 += rhs2;
}
