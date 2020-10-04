//----------------------------------------------------------------------------
// WMPUInt<1> specializations
//----------------------------------------------------------------------------
template<std::size_t kSize2>
inline void WMPUInt<1>::fullMultiply(const WMPUInt<kSize2>& rhs, WMPUInt<1+kSize2>& result) const
{
    if constexpr(kSize2 == 1)
    {
        asm ("mulq %[rhs]"
             : "=a"(result.mData[1]), "=d"(result.mData[0])
             : "a"(mValue), [rhs]"rm"(rhs.mValue) : "cc");
    }
    else if constexpr(kSize2 == 2)
    {
        std::uint64_t temp;
        asm ("mulq %[lhs]\n\t"
             "movq %%rax, %[res2]\n\t"
             "movq %%rdx, %[temp]\n\t"
             "movq %[rhs0], %%rax\n\t"
             "mulq %[lhs]\n\t"
             "addq %[temp], %%rax\n\t"
             "adcq $0, %%rdx"
             : [temp]"=&r"(temp), "=&d"(result.mData[0]), "=a"(result.mData[1]), [res2]"=&rm"(result.mData[2])
             : [lhs]"rm"(mValue), [rhs0]"rm"(rhs.mData[0]), "a"(rhs.mData[1]) : "cc");
    }
    else
    {
        std::uint64_t rhsInd = kSize2 - 1, temp = 0;
        asm ("Loop%=:\n\t"
             "movq (%[rhs], %[rhsInd], 8), %%rax\n\t"
             "mulq %[lhs]\n\t"
             "addq %[temp], %%rax\n\t"
             "adcq $0, %%rdx\n\t"
             "movq %%rax, 8(%[result], %[rhsInd], 8)\n\t"
             "movq %%rdx, %[temp]\n\t"
             "decq %[rhsInd]\n\t"
             "jns Loop%=\n\t"
             "movq %[temp], (%[result])"
             : "=m"(result.mData), [rhsInd]"+&r"(rhsInd), [temp]"+&r"(temp)
             : "m"(rhs.mData), [lhs]"r"(mValue), [rhs]"r"(rhs.mData), [result]"r"(result.mData)
             : "rax", "rdx", "cc");
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
inline WMPUInt<kSize>& WMPUInt<kSize>::operator+=(std::uint64_t value)
{
    std::uint64_t zero = 0;

    if constexpr(kSize == 2)
        asm ("addq %[value], %[lhs1]\n\t"
             "adcq %[zero], %[lhs0]"
             : [lhs0]"+&rm"(mData[0]), [lhs1]"+&rm"(mData[1]) : [value]"r"(value), [zero]"r"(zero) : "cc");
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
inline WMPUInt<kSize>& WMPUInt<kSize>::operator-=(std::uint64_t value)
{
    std::uint64_t zero = 0;

    if constexpr(kSize == 2)
        asm ("subq %[value], %[lhs1]\n\t"
             "sbbq %[zero], %[lhs0]"
             : [lhs0]"+&rm"(mData[0]), [lhs1]"+&rm"(mData[1]) : [value]"r"(value), [zero]"r"(zero) : "cc");
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
        asm ("movq 8(%[rhs]),%%rax\n\t" // rax = rhs[1]
             "movq (%[lhs]),%[tmp]\n\t" // tmp = lhs[0]
             "imulq %%rax,%[tmp]\n\t" // tmp *= rax
             "mulq 8(%[lhs])\n\t" // (rax,rdx) = rax * lhs[1]
             "addq %[tmp],%%rdx\n\t" // rdx += tmp
             "movq (%[rhs]),%[tmp]\n\t" // tmp = rhs[0]
             "imulq 8(%[lhs]),%[tmp]\n\t" // tmp *= lhs[1]
             "addq %[tmp],%%rdx\n\t" // rdx += tmp
             "movq %%rax,8(%[result])\n\t" // result[1] = rax
             "movq %%rdx,(%[result])" // result[0] = rdx;
             : "+m"(result.mData), [tmp]"=&r"(tmp)
             : [lhs]"r"(mData), [rhs]"r"(rhs.mData), [result]"r"(result.mData),
               "m"(mData), "m"(rhs.mData)
             : "rax", "rdx", "cc");
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
             "adcq $0, (%[result])\n\t" // result[0] += 0
             "movq %[rhs1], %%rax\n\t" // rax = rhs[1]
             "mulq %[lhs0]\n\t" // (rdx,rax) = lhs[0] * rax
             "addq %%rax, 16(%[result])\n\t" // result[2] += rax
             "adcq %%rdx, 8(%[result])\n\t" // result[1] += rdx
             "adcq $0, (%[result])" // result[0] += 0
             : "=m"(result.mData)
             : [lhs0]"rm"(mData[0]), [lhs1]"rm"(mData[1]),
               [rhs0]"rm"(rhs.mData[0]), [rhs1]"rm"(rhs.mData[1]),
               [result]"r"(result.mData)
             : "rax", "rdx", "cc");
    }
    else if constexpr(kSize == 2)
    {
        /* (There was previously a specialized 2xN multiplication implementation here,
           but improvements in doFullLongMultiplication() actually made it faster than
           the specialized implementation, so it became obsolete.)

           Note that the first parameter being the smaller one is deliberate:
           doFullLongMultiplication() is faster when the first parameter is smaller than
           the second one.
        */
        WMPIntImplementations::doFullLongMultiplication
            (mData, kSize, rhs.mData, kSize2, result.mData);
    }
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
inline WMPUInt<kSize>& WMPUInt<kSize>::operator*=(const WMPUInt<kSize>& rhs)
{
    /* The result of the multiplication has to be accumulated into a temporary buffer
       anyway, ie. it cannot be done "in-place", so this is essentially free: */
    *this = *this * rhs;
    return *this;
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
        std::uint64_t lhsInd = kSize-1, tempValue = mData[kSize-1];
        mData[kSize-1] = 0;
        asm ("MLoop%=:\n\t"
             "movq %[tempValue], %%rax\n\t" // rax = tempValue
             "movq -8(%[lhs],%[lhsInd],8), %[tempValue]\n\t" // tempValue = lhs[lhsInd-1]
             "movq $0, -8(%[lhs],%[lhsInd],8)\n\t" // lhs[lhsInd-1] = 0
             "mulq %[rhs]\n\t" // (rdx,rax) = rax * rhs
             "addq %%rax, (%[lhs],%[lhsInd],8)\n\t" // lhs[lhsInd] += rax
             "adcq %%rdx, -8(%[lhs],%[lhsInd],8)\n\t" // lhs[lhsInd-1] += rdx
             "decq %[lhsInd]\n\t" // --lhsInd
             "jnz MLoop%=\n\t" // if(lhsInd > 0) goto loop
             "imulq %[tempValue], %[rhs]\n\t" // rhs = tempValue * rhs
             "addq %[rhs], (%[lhs])" // lhs[0] += rhs
             : "+m"(mData), [rhs]"+&r"(rhs), [lhsInd]"+&r"(lhsInd), [tempValue]"+&r"(tempValue)
             : [lhs]"r"(mData)
             : "rax", "rdx", "cc");
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


//----------------------------------------------------------------------------
// Negation
//----------------------------------------------------------------------------
template<std::size_t kSize>
inline void WMPUInt<kSize>::neg_size2()
{
    if constexpr(kSize == 2)
    {
        /* For kSize == 2, this can be done with 3 operations instead of 4. */
        asm ("negq 8(%[lhs])\n\t"
             "adcq $0, (%[lhs])\n\t"
             "negq (%[lhs])"
             : "+m"(mData) : [lhs]"r"(mData) : "cc");
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
