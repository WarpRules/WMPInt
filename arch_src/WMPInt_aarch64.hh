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
        asm ("mul %[res2], %[rhs1], %[lhs]\n\t"
             "umulh %[res1], %[rhs1], %[lhs]\n\t"
             "mul %[res0], %[rhs0], %[lhs]\n\t"
             "adds %[res1], %[res1], %[res0]\n\t"
             "umulh %[res0], %[rhs0], %[lhs]\n\t"
             "adc %[res0], %[res0], xzr"
             : [res0]"=&r"(result.mData[0]), [res1]"=&r"(result.mData[1]), [res2]"=&r"(result.mData[2])
             : [lhs]"r"(mValue), [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1])
             : "cc");
    }
    else
    {
        std::uint64_t *resultPtr = result.mData + kSize2;
        std::uint64_t rhsInd = kSize2, temp = 0, rhsValue, mulRes;
        asm ("Loop%=:\n\t"
             "sub %[rhsInd], %[rhsInd], #1\n\t"
             "ldr %[rhsValue], [%[rhs], %[rhsInd], lsl #3]\n\t"
             "mul %[mulRes], %[rhsValue], %[lhs]\n\t"
             "adds %[mulRes], %[mulRes], %[temp]\n\t"
             "str %[mulRes], [%[result]], #-8\n\t"
             "umulh %[mulRes], %[rhsValue], %[lhs]\n\t"
             "adc %[temp], %[mulRes], xzr\n\t"
             "cbnz %[rhsInd], Loop%=\n\t"
             "str %[temp], [%[result]]"
             : "=m"(result.mData), [rhsInd]"+&r"(rhsInd), [temp]"+&r"(temp),
               [rhsValue]"=&r"(rhsValue), [mulRes]"=&r"(mulRes), [result]"+&r"(resultPtr)
             : "m"(rhs.mData), [lhs]"r"(mValue), [rhs]"r"(rhs.mData)
             : "cc");
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
        /*   AB = lhs0|lhs1
           * CD = rhs0|rhs1
           ----
           EFGH
           0IJ
           0KL
         */
        std::uint64_t res0, res1, res2, temp;
        asm ("mul %[res0], %[rhs1], %[lhs1]\n\t"   // res0 = H (D*B low)
             "umulh %[res2], %[rhs1], %[lhs1]\n\t" // res2 = G (D*B high)
             "str %[res0], [%[result], #24]\n\t"   // result[3] = res0
             "mul %[res1], %[rhs0], %[lhs0]\n\t"   // res1 = F (C*A low)
             "umulh %[res0], %[rhs0], %[lhs0]\n\t" // res0 = E (C*A high)
             "mul %[temp], %[rhs1], %[lhs0]\n\t"   // temp = J (D*A low)
             "adds %[res2], %[res2], %[temp]\n\t"  // res2 += J
             "umulh %[temp], %[rhs1], %[lhs0]\n\t" // temp = I (D*A high)
             "adcs %[res1], %[res1], %[temp]\n\t"  // res1 += I
             "mul %[temp], %[rhs0], %[lhs1]\n\t"   // temp = L (C*B low)
             "adc %[res0], %[res0], xzr\n\t"       // res0 += carry
             "adds %[res2], %[res2], %[temp]\n\t"  // res2 += L
             "umulh %[temp], %[rhs0], %[lhs1]\n\t" // temp = K (C*B high)
             "adcs %[res1], %[res1], %[temp]\n\t"  // res1 += K
             "adc %[res0], %[res0], xzr\n\t"       // res0 += carry
             "str %[res2], [%[result], #16]\n\t"
             "stp %[res0], %[res1], [%[result]]"
             : "=m"(result.mData), [res0]"=&r"(res0), [res1]"=&r"(res1), [res2]"=&r"(res2), [temp]"=&r"(temp)
             : [lhs0]"r"(mData[0]), [lhs1]"r"(mData[1]),
               [rhs0]"r"(rhs.mData[0]), [rhs1]"r"(rhs.mData[1]),
               [result]"r"(result.mData)
             : "cc");
    }
    else if constexpr(kSize == 2)
    {
        WMPIntImplementations::doFullLongMultiplication
            (mData, kSize, rhs.mData, kSize2, result.mData);
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
namespace WMPIntImplementations
{
    std::uint32_t divide32(std::uint64_t*, std::size_t, std::uint32_t);
    std::uint32_t divide32(const std::uint64_t*, std::size_t, std::uint32_t, std::uint64_t*);
    std::uint32_t modulo32(const std::uint64_t*, std::size_t, std::uint32_t);
}

template<std::size_t kSize>
inline std::uint32_t WMPUInt<kSize>::divide(std::uint32_t rhs, WMPUInt<kSize>& result) const
{
    return WMPIntImplementations::divide32(mData, kSize, rhs, result.mData);
}

template<std::size_t kSize>
inline WMPUInt<kSize>& WMPUInt<kSize>::operator/=(std::uint32_t rhs)
{
    WMPIntImplementations::divide32(mData, kSize, rhs);
    return *this;
}

template<std::size_t kSize>
inline std::uint32_t WMPUInt<kSize>::modulo(std::uint32_t rhs) const
{
    return WMPIntImplementations::modulo32(mData, kSize, rhs);
}


//----------------------------------------------------------------------------
// Negation
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline WMPUInt<kSize> WMPUInt<kSize>::operator-() const
{
    WMPUInt<kSize> result;

    if constexpr(kSize == 2)
    {
        asm ("negs %[res1], %[lhs1]\n\t"
             "sbc %[res0], xzr, %[lhs0]"
             : [res0]"=r"(result.mData[0]), [res1]"=&r"(result.mData[1])
             : [lhs0]"r"(mData[0]), [lhs1]"r"(mData[1]) : "cc");
    }
    else if constexpr(kSize == 3)
    {
        asm ("negs %[res2], %[lhs2]\n\t"
             "sbc %[res1], xzr, %[lhs1]\n\t"
             "sbc %[res0], xzr, %[lhs0]"
             : [res0]"=r"(result.mData[0]), [res1]"=&r"(result.mData[1]), [res2]"=&r"(result.mData[2])
             : [lhs0]"r"(mData[0]), [lhs1]"r"(mData[1]), [lhs2]"r"(mData[2]) : "cc");
    }
    else if constexpr(kSize == 4)
    {
        asm ("negs %[res3], %[lhs3]\n\t"
             "sbc %[res2], xzr, %[lhs2]\n\t"
             "sbc %[res1], xzr, %[lhs1]\n\t"
             "sbc %[res0], xzr, %[lhs0]"
             : [res0]"=r"(result.mData[0]), [res1]"=&r"(result.mData[1]),
               [res2]"=&r"(result.mData[2]), [res3]"=&r"(result.mData[3])
             : [lhs0]"r"(mData[0]), [lhs1]"r"(mData[1]), [lhs2]"r"(mData[2]), [lhs3]"r"(mData[3]) : "cc");
    }
    else if constexpr(kSize % 2 == 1)
    {
        const std::uint64_t *data = mData + (kSize-3);
        std::uint64_t *dest = result.mData + (kSize-1);
        std::uint64_t lhs0 = mData[kSize-1], lhs1, counter = kSize/2;
        asm ("negs %[lhs0], %[lhs0]\n\t"
             "str %[lhs0], [%[dest]], #-16\n"
             "Loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[dest]], #-16\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], Loop%="
             : "=m"(result.mData), [data]"+&r"(data), [dest]"+&r"(dest),
               [lhs0]"+&r"(lhs0), [lhs1]"=&r"(lhs1), [counter]"+&r"(counter)
             : "m"(mData) : "cc");
    }
    else
    {
        const std::uint64_t *data = mData + (kSize-4);
        std::uint64_t *dest = result.mData + (kSize-2);
        std::uint64_t lhs0 = mData[kSize-2], lhs1 = mData[kSize-1], counter = kSize/2 - 1;
        asm ("negs %[lhs1], %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[dest]], #-16\n\t"
             "Loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[dest]], #-16\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], Loop%="
             : "=m"(result.mData), [data]"+&r"(data), [dest]"+&r"(dest),
               [lhs0]"+&r"(lhs0), [lhs1]"+&r"(lhs1), [counter]"+&r"(counter)
             : "m"(mData) : "cc");
    }
    return result;
}

template<std::size_t kSize>
inline void WMPUInt<kSize>::neg()
{
    if constexpr(kSize == 2)
    {
        asm ("negs %[lhs1], %[lhs1]\n\t"
             "sbc %[lhs0], xzr, %[lhs0]"
             : [lhs0]"+r"(mData[0]), [lhs1]"+r"(mData[1]) : : "cc");
    }
    else if constexpr(kSize == 3)
    {
        asm ("negs %[lhs2], %[lhs2]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbc %[lhs0], xzr, %[lhs0]"
             : [lhs0]"+r"(mData[0]), [lhs1]"+r"(mData[1]), [lhs2]"+r"(mData[2]) : : "cc");
    }
    else if constexpr(kSize == 4)
    {
        asm ("negs %[lhs3], %[lhs3]\n\t"
             "sbcs %[lhs2], xzr, %[lhs2]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbc %[lhs0], xzr, %[lhs0]"
             : [lhs0]"+r"(mData[0]), [lhs1]"+r"(mData[1]), [lhs2]"+r"(mData[2]), [lhs3]"+r"(mData[3]) : : "cc");
    }
    else if constexpr(kSize == 5)
    {
        std::uint64_t *data = mData + 4, lhs0 = mData[4], lhs1;
        asm ("negs %[lhs0], %[lhs0]\n\t"
             "str %[lhs0], [%[data]], #-16\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbc %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]]"
             : "+m"(mData), [data]"+&r"(data), [lhs0]"+&r"(lhs0), [lhs1]"=&r"(lhs1) : : "cc");
    }
    else if constexpr(kSize == 6)
    {
        std::uint64_t *data = mData + 4, lhs0 = mData[4], lhs1 = mData[5];
        asm ("negs %[lhs1], %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbc %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]]"
             : "+m"(mData), [data]"+&r"(data), [lhs0]"+&r"(lhs0), [lhs1]"+&r"(lhs1) : : "cc");
    }
    else if constexpr(kSize % 2 == 1)
    {
        std::uint64_t *data = mData + (kSize-1), lhs0 = mData[kSize-1], lhs1, counter = kSize/2;
        asm ("negs %[lhs0], %[lhs0]\n\t"
             "str %[lhs0], [%[data]], #-16\n"
             "Loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], Loop%="
             : "+m"(mData), [data]"+&r"(data), [lhs0]"+&r"(lhs0), [lhs1]"=&r"(lhs1), [counter]"+&r"(counter) : : "cc");
    }
    else
    {
        std::uint64_t *data = mData + (kSize-2), lhs0 = mData[kSize-2], lhs1 = mData[kSize-1], counter = kSize/2 - 1;
        asm ("negs %[lhs1], %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "Loop%=:\n\t"
             "ldp %[lhs0], %[lhs1], [%[data]]\n\t"
             "sbcs %[lhs1], xzr, %[lhs1]\n\t"
             "sbcs %[lhs0], xzr, %[lhs0]\n\t"
             "stp %[lhs0], %[lhs1], [%[data]], #-16\n\t"
             "sub %[counter], %[counter], #1\n\t"
             "cbnz %[counter], Loop%="
             : "+m"(mData), [data]"+&r"(data), [lhs0]"+&r"(lhs0), [lhs1]"+&r"(lhs1), [counter]"+&r"(counter) : : "cc");
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
