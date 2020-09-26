//----------------------------------------------------------------------------
// WMPUInt<1> specializations
//----------------------------------------------------------------------------
template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result) const
{
    if constexpr(kSize2 == 1)
    {
        asm ("umulh %[res0], %[lhs], %[rhs]\n\t"
             "mul %[res1], %[lhs], %[rhs]"
             : [res0]"=&r"(result.mData[0]), [res1]"=r"(result.mData[1])
             : [lhs]"r"(mValue), [rhs]"r"(rhs.mValue):);
    }
    else if constexpr(kSize2 == 2)
    {
        result.mData[0] = 0;
        asm (""
             : : : "cc");
    }
    else
    {
        for(std::size_t i = 0; i < 1+kSize2; ++i) result.mData[i] = 0;
        std::uint64_t rhsInd = kSize2 - 1;
        asm (""
             : : : "cc");
    }
}


//----------------------------------------------------------------------------
// Addition
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator+=(const WMPUInt<kSize>& rhs)
{
    if constexpr(kSize == 2)
        asm ("adds %[lhs1], %[lhs1], %[rhs1]\n\t"
             "adc %[lhs0], %[lhs0], %[rhs0]"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1])
             : [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]) : "cc");
    else if constexpr(kSize == 3)
        asm ("adds %[lhs2], %[lhs2], %[rhs2]\n\t"
             "adcs %[lhs1], %[lhs1], %[rhs1]\n\t"
             "adc %[lhs0], %[lhs0], %[rhs0]"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1]), [lhs2]"+&r"(mData[2])
             : [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]), [rhs2]"r"(rhs.mData[2]) : "cc");
    else
    {
        if constexpr(kSize == 4)
        {
            std::uint64_t *lhsPtr = mData + (kSize - 2), lhs0, lhs1, rhs0, rhs1;
            const std::uint64_t *rhsPtr = rhs.mData + (kSize - 2);
            asm ("ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "adds %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "adcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n\t"
                 "ldp %[rhs0], %[rhs1], [%[rhsPtr]]\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "adcs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "adc %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]]"
                 : "+m"(mData), [lhsPtr]"+&r"(lhsPtr), [rhsPtr]"+&r"(rhsPtr),
                   [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1), [rhs0]"=&r"(rhs0), [rhs1]"=&r"(rhs1)
                 : "m"(rhs.mData) : "cc");
        }
        else if constexpr(kSize % 2 == 0)
        {
            std::uint64_t *lhsPtr = mData + (kSize - 2), counter = kSize / 2 - 1, lhs0, lhs1, rhs0, rhs1;
            const std::uint64_t *rhsPtr = rhs.mData + (kSize - 2);
            asm ("ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "adds %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "adcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n"
                 "loop%=:\n\t"
                 "ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "adcs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "adcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n\t"
                 "sub %[counter], %[counter], #1\n\t"
                 "cbnz %[counter], loop%="
                 : "+m"(mData), [lhsPtr]"+&r"(lhsPtr), [rhsPtr]"+&r"(rhsPtr), [counter]"+&r"(counter),
                   [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1), [rhs0]"=&r"(rhs0), [rhs1]"=&r"(rhs1)
                 : "m"(rhs.mData) : "cc");
        }
        else
        {
            std::uint64_t *lhsPtr = mData + (kSize - 1), counter = kSize / 2, lhs0, lhs1, rhs0, rhs1;
            const std::uint64_t *rhsPtr = rhs.mData + (kSize - 1);
            asm ("ldr %[rhs0], [%[rhsPtr]], #-16\n\t"
                 "ldr %[lhs0], [%[lhsPtr]]\n\t"
                 "adds %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "str %[lhs0], [%[lhsPtr]], #-16\n"
                 "loop%=:\n\t"
                 "ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "adcs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "adcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n\t"
                 "sub %[counter], %[counter], #1\n\t"
                 "cbnz %[counter], loop%="
                 : "+m"(mData), [lhsPtr]"+&r"(lhsPtr), [rhsPtr]"+&r"(rhsPtr), [counter]"+&r"(counter),
                   [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1), [rhs0]"=&r"(rhs0), [rhs1]"=&r"(rhs1)
                 : "m"(rhs.mData) : "cc");
        }
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator+=(std::uint64_t value)
{
    if constexpr(kSize == 2)
        asm ("adds %[lhs1], %[lhs1], %[rhs]\n\t"
             "adc %[lhs0], %[lhs0], xzr"
             : [lhs0]"+r"(mData[0]), [lhs1]"+&r"(mData[1])
             : [rhs]"r"(value) : "cc");
    else if constexpr(kSize == 3)
        asm ("adds %[lhs2], %[lhs2], %[rhs]\n\t"
             "adcs %[lhs1], %[lhs1], xzr\n\t"
             "adc %[lhs0], %[lhs0], xzr"
             : [lhs0]"+r"(mData[0]), [lhs1]"+r"(mData[1]), [lhs2]"+&r"(mData[2])
             : [rhs]"r"(value) : "cc");
    else if constexpr(kSize == 4)
        asm ("adds %[lhs3], %[lhs3], %[rhs]\n\t"
             "adcs %[lhs2], %[lhs2], xzr\n\t"
             "adcs %[lhs1], %[lhs1], xzr\n\t"
             "adc %[lhs0], %[lhs0], xzr"
             : [lhs0]"+r"(mData[0]), [lhs1]"+r"(mData[1]), [lhs2]"+r"(mData[2]), [lhs3]"+&r"(mData[3])
             : [rhs]"r"(value) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t *dataPtr = mData + (kSize - 2), counter = kSize / 2 - 1, lhs0, lhs1;
        asm ("ldp %[lhs0], %[lhs1], [%[dataPtr]]\n\t"
             "adds %[lhs1], %[lhs1], %[rhs]\n\t"
             "adcs %[lhs0], %[lhs0], xzr\n\t"
             "stp %[lhs0], %[lhs1], [%[dataPtr]], #-16\n"
             "loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[dataPtr]]\n\t"
             "adcs %[lhs1], %[lhs1], xzr\n\t"
             "adcs %[lhs0], %[lhs0], xzr\n\t"
             "stp %[lhs0], %[lhs1], [%[dataPtr]], #-16\n\t"
             "b.cc end%=\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], loop%=\n"
             "end%=:"
             : "+m"(mData), [dataPtr]"+&r"(dataPtr), [counter]"+&r"(counter), [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1)
             : [rhs]"r"(value) : "cc");
    }
    else
    {
        std::uint64_t *dataPtr = mData + (kSize - 1), counter = kSize / 2, lhs0, lhs1;
        asm ("ldr %[lhs0], [%[dataPtr]]\n\t"
             "adds %[lhs0], %[lhs0], %[rhs]\n\t"
             "str %[lhs0], [%[dataPtr]], #-16\n"
             "loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[dataPtr]]\n\t"
             "adcs %[lhs1], %[lhs1], xzr\n\t"
             "adcs %[lhs0], %[lhs0], xzr\n\t"
             "stp %[lhs0], %[lhs1], [%[dataPtr]], #-16\n\t"
             "b.cc end%=\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], loop%=\n"
             "end%=:"
             : "+m"(mData), [dataPtr]"+&r"(dataPtr), [counter]"+&r"(counter), [lhs0]"=&r"(lhs0), [lhs1]"=r"(lhs1)
             : [rhs]"r"(value) : "cc");
    }

    return *this;
}


//----------------------------------------------------------------------------
// Subtraction
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(const WMPUInt<kSize>& rhs)
{
    if constexpr(kSize == 2)
        asm ("subs %[lhs1], %[lhs1], %[rhs1]\n\t"
             "sbc %[lhs0], %[lhs0], %[rhs0]"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1])
             : [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]) : "cc");
    else if constexpr(kSize == 3)
        asm ("subs %[lhs2], %[lhs2], %[rhs2]\n\t"
             "sbcs %[lhs1], %[lhs1], %[rhs1]\n\t"
             "sbc %[lhs0], %[lhs0], %[rhs0]"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1]), [lhs2]"+&r"(mData[2])
             : [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]), [rhs2]"r"(rhs.mData[2]) : "cc");
    else
    {
        if constexpr(kSize == 4)
        {
            std::uint64_t *lhsPtr = mData + (kSize - 2), lhs0, lhs1, rhs0, rhs1;
            const std::uint64_t *rhsPtr = rhs.mData + (kSize - 2);
            asm ("ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "subs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "sbcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n\t"
                 "ldp %[rhs0], %[rhs1], [%[rhsPtr]]\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "sbcs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "sbc %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]]"
                 : "+m"(mData), [lhsPtr]"+&r"(lhsPtr), [rhsPtr]"+&r"(rhsPtr),
                   [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1), [rhs0]"=&r"(rhs0), [rhs1]"=&r"(rhs1)
                 : "m"(rhs.mData) : "cc");
        }
        else if constexpr(kSize % 2 == 0)
        {
            std::uint64_t *lhsPtr = mData + (kSize - 2), counter = kSize / 2 - 1, lhs0, lhs1, rhs0, rhs1;
            const std::uint64_t *rhsPtr = rhs.mData + (kSize - 2);
            asm ("ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "subs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "sbcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n"
                 "loop%=:\n\t"
                 "ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "sbcs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "sbcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n\t"
                 "sub %[counter], %[counter], #1\n\t"
                 "cbnz %[counter], loop%="
                 : "+m"(mData), [lhsPtr]"+&r"(lhsPtr), [rhsPtr]"+&r"(rhsPtr), [counter]"+&r"(counter),
                   [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1), [rhs0]"=&r"(rhs0), [rhs1]"=&r"(rhs1)
                 : "m"(rhs.mData) : "cc");
        }
        else
        {
            std::uint64_t *lhsPtr = mData + (kSize - 1), counter = kSize / 2, lhs0, lhs1, rhs0, rhs1;
            const std::uint64_t *rhsPtr = rhs.mData + (kSize - 1);
            asm ("ldr %[rhs0], [%[rhsPtr]], #-16\n\t"
                 "ldr %[lhs0], [%[lhsPtr]]\n\t"
                 "subs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "str %[lhs0], [%[lhsPtr]], #-16\n"
                 "loop%=:\n\t"
                 "ldp %[rhs0], %[rhs1], [%[rhsPtr]], #-16\n\t"
                 "ldp %[lhs0], %[lhs1], [%[lhsPtr]]\n\t"
                 "sbcs %[lhs1], %[lhs1], %[rhs1]\n\t"
                 "sbcs %[lhs0], %[lhs0], %[rhs0]\n\t"
                 "stp %[lhs0], %[lhs1], [%[lhsPtr]], #-16\n\t"
                 "sub %[counter], %[counter], #1\n\t"
                 "cbnz %[counter], loop%="
                 : "+m"(mData), [lhsPtr]"+&r"(lhsPtr), [rhsPtr]"+&r"(rhsPtr), [counter]"+&r"(counter),
                   [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1), [rhs0]"=&r"(rhs0), [rhs1]"=&r"(rhs1)
                 : "m"(rhs.mData) : "cc");
        }
    }

    return *this;
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(std::uint64_t value)
{
    if constexpr(kSize == 2)
        asm ("subs %[lhs1], %[lhs1], %[rhs]\n\t"
             "sbc %[lhs0], %[lhs0], xzr"
             : [lhs0]"+r"(mData[0]), [lhs1]"+&r"(mData[1])
             : [rhs]"r"(value) : "cc");
    else if constexpr(kSize == 3)
        asm ("subs %[lhs2], %[lhs2], %[rhs]\n\t"
             "sbcs %[lhs1], %[lhs1], xzr\n\t"
             "sbc %[lhs0], %[lhs0], xzr"
             : [lhs0]"+r"(mData[0]), [lhs1]"+r"(mData[1]), [lhs2]"+&r"(mData[2])
             : [rhs]"r"(value) : "cc");
    else if constexpr(kSize == 4)
        asm ("subs %[lhs3], %[lhs3], %[rhs]\n\t"
             "sbcs %[lhs2], %[lhs2], xzr\n\t"
             "sbcs %[lhs1], %[lhs1], xzr\n\t"
             "sbc %[lhs0], %[lhs0], xzr"
             : [lhs0]"+&r"(mData[0]), [lhs1]"+r"(mData[1]), [lhs2]"+r"(mData[2]), [lhs3]"+r"(mData[3])
             : [rhs]"r"(value) : "cc");
    else if constexpr(kSize % 2 == 0)
    {
        std::uint64_t *dataPtr = mData + (kSize - 2), counter = kSize / 2 - 1, lhs0, lhs1;
        asm ("ldp %[lhs0], %[lhs1], [%[dataPtr]]\n\t"
             "subs %[lhs1], %[lhs1], %[rhs]\n\t"
             "sbcs %[lhs0], %[lhs0], xzr\n\t"
             "stp %[lhs0], %[lhs1], [%[dataPtr]], #-16\n"
             "loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[dataPtr]]\n\t"
             "sbcs %[lhs1], %[lhs1], xzr\n\t"
             "sbcs %[lhs0], %[lhs0], xzr\n\t"
             "stp %[lhs0], %[lhs1], [%[dataPtr]], #-16\n\t"
             "b.cs end%=\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], loop%=\n"
             "end%=:"
             : "+m"(mData), [dataPtr]"+&r"(dataPtr), [counter]"+&r"(counter), [lhs0]"=&r"(lhs0), [lhs1]"=&r"(lhs1)
             : [rhs]"r"(value) : "cc");
    }
    else
    {
        std::uint64_t *dataPtr = mData + (kSize - 1), counter = kSize / 2, lhs0, lhs1;
        asm ("ldr %[lhs0], [%[dataPtr]]\n\t"
             "subs %[lhs0], %[lhs0], %[rhs]\n\t"
             "str %[lhs0], [%[dataPtr]], #-16\n"
             "loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[dataPtr]]\n\t"
             "sbcs %[lhs1], %[lhs1], xzr\n\t"
             "sbcs %[lhs0], %[lhs0], xzr\n\t"
             "stp %[lhs0], %[lhs1], [%[dataPtr]], #-16\n\t"
             "b.cs end%=\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], loop%=\n"
             "end%=:"
             : "+m"(mData), [dataPtr]"+&r"(dataPtr), [counter]"+&r"(counter), [lhs0]"=&r"(lhs0), [lhs1]"=r"(lhs1)
             : [rhs]"r"(value) : "cc");
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
        /*           rhs0 rhs1
                   * lhs0 lhs1
             -----------------
             l1*r1(h) l1*r1(l)  (1a) (1b)
           + l1*r0(l)           (2)
           + l0*r1(l)           (3)
        */
        asm ("umulh %[res0], %[lhs1], %[rhs1]\n\t"          // res0 = lhs1*rhs1(h)        (1a)
             "madd %[res0], %[lhs1], %[rhs0], %[res0]\n\t"  // res0 = lhs1*rhs0(l) + res0 (2)
             "mul %[res1], %[lhs1], %[rhs1]\n\t"            // res1 = lhs1*rhs1(l)        (1b)
             "madd %[res0], %[lhs0], %[rhs1], %[res0]\n\t"  // res0 = lhs0*rhs1(l) + res0 (3)
             : [res0]"=&r"(result.mData[0]), [res1]"=&r"(result.mData[1])
             : [lhs0]"r"(mData[0]), [lhs1]"r"(mData[1]),
               [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]) :);
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
        asm ("umulh %[res0], %[lhs1], %[rhs]\n\t"
             "mul %[res1], %[lhs1], %[rhs]\n\t"
             "madd %[res0], %[lhs0], %[rhs], %[res0]"
             : [res0]"=&r"(result[0]), [res1]"=&r"(result[1])
             : [lhs0]"r"(mData[0]), [lhs1]"r"(mData[1]), [rhs]"r"(rhs) :);
    }
    else
    {
        const std::uint64_t* lhs = mData + (kSize - 1);
        std::uint64_t* resPtr = result + (kSize - 1);
        std::uint64_t res = 0, tmp1, tmp2;
        asm ("loop%=:\n\t"
             "ldr %[tmp1], [%[lhs]], #-8\n\t"
             "mul %[tmp2], %[tmp1], %[rhs]\n\t"
             "umulh %[tmp1], %[tmp1], %[rhs]\n\t"
             "adds %[res], %[res], %[tmp2]\n\t"
             "str %[res], [%[result]], #-8\n\t"
             "adc %[res], xzr, %[tmp1]\n\t"
             "cmp %[lhs], %[mData]\n\t"
             "bne loop%=\n\t"
             "ldr %[tmp1], [%[lhs]]\n\t"
             "madd %[res], %[tmp1], %[rhs], %[res]\n\t"
             "str %[res], [%[result]]"
             : "+m"(*(std::uint64_t(*)[kSize])result), [result]"+&r"(resPtr),
               [lhs]"+&r"(lhs), [res]"+&r"(res), [tmp1]"=&r"(tmp1), [tmp2]"=&r"(tmp2)
             : "m"(mData), [mData]"r"(mData), [rhs]"r"(rhs) : "cc");
    }
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator*=(const WMPUInt<kSize>& rhs)
{
    if constexpr(kSize == 2)
    {
        /*           rhs0 rhs1
                   * lhs0 lhs1
             -----------------
             l1*r1(h) l1*r1(l)  (1a) (1b)
           + l1*r0(l)           (2)
           + l0*r1(l)           (3)
        */
        std::uint64_t tmp;
        asm ("umulh %[tmp], %[lhs1], %[rhs1]\n\t"         // tmp  = lhs1*rhs1(h)        (1a)
             "madd %[tmp], %[lhs1], %[rhs0], %[tmp]\n\t"  // tmp  = lhs1*rhs0(l) + tmp  (2)
             "mul %[lhs1], %[lhs1], %[rhs1]\n\t"          // lhs1 = lhs1*rhs1(l)        (1b)
             "madd %[lhs0], %[lhs0], %[rhs1], %[tmp]\n\t" // lhs0 = lhs0*rhs1(l) + tmp  (3)
             : [lhs0]"+&r"(mData[0]), [lhs1]"+&r"(mData[1]), [tmp]"=&r"(tmp)
             : [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]) :);
    }
    else
    {
        /* The result of the multiplication has to be accumulated into a temporary buffer
           anyway, ie. it cannot be done "in-place", so this is essentially free: */
        *this = *this * rhs;
    }
    return *this;
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
        std::uint64_t res = 0, tmp1, tmp2;
        asm ("loop%=:\n\t"
             "ldr %[tmp1], [%[lhs]]\n\t"
             "mul %[tmp2], %[tmp1], %[rhs]\n\t"
             "umulh %[tmp1], %[tmp1], %[rhs]\n\t"
             "adds %[res], %[res], %[tmp2]\n\t"
             "str %[res], [%[lhs]], #-8\n\t"
             "adc %[res], xzr, %[tmp1]\n\t"
             "cmp %[lhs], %[mData]\n\t"
             "bne loop%=\n\t"
             "ldr %[tmp1], [%[lhs]]\n\t"
             "madd %[res], %[tmp1], %[rhs], %[res]\n\t"
             "str %[res], [%[lhs]]"
             : "+m"(mData), [lhs]"+&r"(lhs), [res]"+&r"(res), [tmp1]"=&r"(tmp1), [tmp2]"=&r"(tmp2)
             : [mData]"r"(mData), [rhs]"r"(rhs) : "cc");
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
