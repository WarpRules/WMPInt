//----------------------------------------------------------------------------
// Long multiplication
//----------------------------------------------------------------------------
void WMPIntImplementations::doLongMultiplication
(std::size_t kSize, const std::uint64_t* lhs, const std::uint64_t* rhs, std::uint64_t* result)
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
    for(std::size_t i = 0; i < kSize; ++i) result[i] = 0;

    std::uint64_t lhsInd = kSize - 1, rhsInd, lhsValue, temp;
    std::uint8_t carry;

    /* In principle this shouldn't be 'volatile' nor use the '"memory"' constraint, but gcc -O0
       gives an "impossible constraints" error for because of all the "m" constraints (is it trying
       to map them onto registers for some reason?) I haven't figured out a better way. This needs
       to be instantiated in this case anyway, so no harm done. */
    asm volatile
        ("L1%=:\n\t"
         "movq %[lhsInd], %[rhsInd]\n\t" // rhsInd = lhsInd
         "movq (%[lhs], %[lhsInd], 8), %[lhsValue]\n\t" // lhsValue = lhs[lhsInd]
         "xorl %k[temp], %k[temp]\n\t" // temp = 0
         "xorb %[carry], %[carry]\n" // carry = 0
         "L2%=:\n\t"
         "movq (%[rhs], %[rhsInd], 8), %%rax\n\t" // rax = rhs[rhsInd]
         "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
         "addq %[temp], %%rax\n\t" // rax += temp
         "adcq $0, %%rdx\n\t" // rdx += (c-flag)
         "addb $255, %[carry]\n\t" // (c-flag) = carry
         "adcq %%rax, (%[result], %[rhsInd], 8)\n\t" // result[rhsInd] += rax
         "movq %%rdx, %[temp]\n\t" // temp = rdx
         "setc %[carry]\n\t" // carry = (c-flag)
         "decq %[rhsInd]\n\t" // --rhsInd
         "jnz L2%=\n\t" // if(rhsInd > 0) goto L2
         "imulq (%[rhs]), %[lhsValue]\n\t" // lhsValue *= *rhs
         "addq %[lhsValue], %[temp]\n\t" // temp += lhsValue
         "addb $255, %[carry]\n\t" // (c-flag) = carry
         "adcq %[temp], (%[result])\n\t" // *result += temp
         "leaq 8(%[rhs]), %[rhs]\n\t" // ++rhs
         "decq %[lhsInd]\n\t" // --lhdInd
         "jnz L1%=\n\t" // if(lhsInd > 0) goto L1
         "movq (%[lhs]), %[lhsValue]\n\t" // lhsValue = *lhs
         "imulq (%[rhs]), %[lhsValue]\n\t" // lhsValue *= *rhs
         "addq %[lhsValue], (%[result])" // result += lhsValue
         : //"+m"(*(std::uint64_t(*)[kSize])result), "=m"(*(std::uint64_t(*)[kSize])tempBuffer),
           [rhs]"+&r"(rhs), [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd), [lhsValue]"=&r"(lhsValue),
           [temp]"=&r"(temp), [carry]"=&r"(carry)
         : [lhs]"r"(lhs), [result]"r"(result)
           //, "m"(*(std::uint64_t(*)[kSize])lhs), "m"(*(std::uint64_t(*)[kSize])rhs)
         : "rax", "rdx", "cc", "memory");
}


//----------------------------------------------------------------------------
// Full long multiplication
//----------------------------------------------------------------------------
void WMPIntImplementations::doFullLongMultiplication
(const std::uint64_t* lhs, std::size_t kSize1, const std::uint64_t* rhs, std::size_t kSize2, std::uint64_t* result)
{
    const std::size_t resultSize = kSize1 + kSize2;
    std::uint64_t* resultPtr = result + (kSize1 - 1);
    std::uint64_t lhsInd = kSize1 - 1, rhsInd, lhsValue, temp;
    std::uint8_t carry;
    for(std::size_t i = 0; i < resultSize; ++i) result[i] = 0;

    /* In principle this shouldn't be 'volatile' nor use the '"memory"' constraint, but gcc -O0
       gives an "impossible constraints" error because of all the "m" constraints (is it trying
       to map them onto registers for some reason?) I haven't figured out a better way. This needs
       to be instantiated in this case anyway, so no harm done. */
    asm volatile
        ("L1%=:\n\t"
         "movq %[kSize2], %[rhsInd]\n\t" // rhsInd = kSize2
         "movq (%[lhs], %[lhsInd], 8), %[lhsValue]\n\t" // lhsValue = lhs[lhsInd]
         "xorl %k[temp], %k[temp]\n\t" // temp = 0
         "xorb %[carry], %[carry]\n" // carry = 0
         "L2%=:\n\t"
         "movq -8(%[rhs], %[rhsInd], 8), %%rax\n\t" // rax = rhs[rhsInd-1]
         "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
         "addq %[temp], %%rax\n\t" // rax += temp
         "adcq $0, %%rdx\n\t" // rdx += (c-flag)
         "addb $255, %[carry]\n\t" // (c-flag) = carry
         "adcq %%rax, (%[result], %[rhsInd], 8)\n\t" // result[rhsInd] += rax
         "movq %%rdx, %[temp]\n\t" // temp = rdx;
         "setc %[carry]\n\t" // carry = (c-flag)
         "decq %[rhsInd]\n\t" // --rhsInd
         "jnz L2%=\n\t" // if(rhsInd > 0) goto L2
         "addb $255, %[carry]\n\t" // c-flag = carry
         "adcq %[temp], (%[result])\n\t" // *result += temp
         "leaq -8(%[result]), %[result]\n\t" // --result
         "decq %[lhsInd]\n\t" // --lhsInd
         "jns L1%=" // if(lhsInd >= 0) goto L1
         : //"+m"(*(std::uint64_t(*)[resultSize])result), "=m"(*(std::uint64_t(*)[kSize2+1])tempBuffer),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [temp]"=&r"(temp),
           [result]"+&r"(resultPtr), [carry]"=&r"(carry)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), [kSize2]"irm"(kSize2)
           //, "m"(*(std::uint64_t(*)[kSize1])lhs), "m"(*(std::uint64_t(*)[kSize2])rhs)
         : "rax", "rdx", "cc", "memory");
}


//----------------------------------------------------------------------------
// Karatsuba algorithm helper functions
//----------------------------------------------------------------------------
/* This assumes 1 < lhsSize <= rhsSize. */
static void doFullAddition(const std::uint64_t* lhs, std::size_t lhsSize,
                           const std::uint64_t* rhs, std::size_t rhsSize,
                           std::uint64_t* result)
{
    if(lhsSize == rhsSize)
    {
        *result = 0;
        std::size_t srcInd = lhsSize - 1;
        std::uint64_t value;
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[lhs],%[srcInd],8), %[value]\n\t"
             "adcq (%[rhs],%[srcInd],8), %[value]\n\t"
             "movq %[value], 8(%[result],%[srcInd],8)\n\t"
             "decq %[srcInd]\n\t"
             "jns L1%=\n\t"
             "adcq $0, (%[result])"
             : "=m"(*(std::uint64_t(*)[lhsSize+1])result),
               [srcInd]"+&r"(srcInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result),
               "m"(*(std::uint64_t(*)[lhsSize])lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
    else
    {
        for(std::size_t i = 0; i < rhsSize - lhsSize + 1; ++i) result[i] = 0;
        std::size_t lhsInd = lhsSize-1, rhsInd = rhsSize-1;
        std::uint64_t value;
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[lhs],%[lhsInd],8), %[value]\n\t"
             "adcq (%[rhs],%[rhsInd],8), %[value]\n\t"
             "movq %[value], 8(%[result],%[rhsInd],8)\n\t"
             "decq %[rhsInd]\n\t"
             "decq %[lhsInd]\n\t"
             "jns L1%=\n"
             "L2%=:\n\t"
             "movq (%[rhs],%[rhsInd],8), %[value]\n\t"
             "adcq %[value], 8(%[result],%[rhsInd],8)\n\t"
             "decq %[rhsInd]\n\t"
             "jns L2%=\n\t"
             "adcq $0, (%[result])"
             : "=m"(*(std::uint64_t(*)[rhsSize+1])result),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result),
               "m"(*(std::uint64_t(*)[lhsSize])lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
}

static void doAddition(std::uint64_t* lhs, std::size_t lhsSize,
                       const std::uint64_t* rhs, std::size_t rhsSize)
{
    std::size_t lhsInd = lhsSize - 1;
    std::uint64_t value;
    if(lhsSize <= rhsSize)
    {
        const std::uint64_t* rhsPtr = rhs + (rhsSize - 1);
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[rhs]), %[value]\n\t"
             "adcq %[value], (%[lhs],%[lhsInd],8)\n\t"
             "leaq -8(%[rhs]), %[rhs]\n\t"
             "decq %[lhsInd]\n\t"
             "jns L1%="
             : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
               [lhsInd]"+&r"(lhsInd), [rhs]"+&r"(rhsPtr), [value]"=&r"(value)
             : [lhs]"r"(lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
    else
    {
        std::size_t rhsInd = rhsSize - 1;
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[rhs],%[rhsInd],8), %[value]\n\t"
             "adcq %[value], (%[lhs],%[lhsInd],8)\n\t"
             "decq %[lhsInd]\n\t"
             "decq %[rhsInd]\n\t"
             "jns L1%=\n"
             "jnc end%=\n"
             "L2%=:\n\t"
             "incq (%[lhs],%[lhsInd],8)\n\t"
             "jnz end%=\n\t"
             "decq %[lhsInd]\n\t"
             "jns L2%=\n"
             "end%=:"
             : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
}

static inline void doAddition(std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size)
{
    std::size_t index = size - 1;
    std::uint64_t value;
    asm ("clc\n"
         "loop%=:\n\t"
         "movq (%[rhs],%[index],8), %[value]\n\t"
         "adcq %[value], (%[lhs],%[index],8)\n\t"
         "decq %[index]\n\t"
         "jns loop%="
         : "+m"(*(std::uint64_t(*)[size])lhs),
           [index]"+&r"(index), [value]"=&r"(value)
         : "m"(*(std::uint64_t(*)[size])rhs),
           [lhs]"r"(lhs), [rhs]"r"(rhs)
         : "cc");
}

/* Assumes that lhsSize > rhsSize */
static inline void doSubtraction(std::uint64_t* lhs, std::size_t lhsSize,
                                 const std::uint64_t* rhs, std::size_t rhsSize)
{
    std::size_t lhsInd = lhsSize - 1, rhsInd = rhsSize - 1;
    std::uint64_t value;
    asm ("clc\n"
         "L1%=:\n\t"
         "movq (%[rhs],%[rhsInd],8), %[value]\n\t"
         "sbbq %[value], (%[lhs],%[lhsInd],8)\n\t"
         "decq %[lhsInd]\n\t"
         "decq %[rhsInd]\n\t"
         "jns L1%=\n"
         "L2%=:\n\t"
         "sbbq $0, (%[lhs],%[lhsInd],8)\n\t"
         "decq %[lhsInd]\n\t"
         "jns L2%="
         : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
         : "cc");
}

static void doFullLongMultiplication_1xN
(std::uint64_t lhs, const std::uint64_t* rhs, std::size_t kSize2, std::uint64_t* result)
{
    for(std::size_t i = 0; i < 1+kSize2; ++i) result[i] = 0;
    std::uint64_t rhsInd = kSize2 - 1;
    asm ("L1%=:\n\t"
         "movq (%[rhs],%[rhsInd],8), %%rax\n\t"
         "mulq %[lhs]\n\t"
         "addq %%rax, 8(%[result],%[rhsInd],8)\n\t"
         "adcq %%rdx, (%[result],%[rhsInd],8)\n\t"
         "decq %[rhsInd]\n\t"
         "jns L1%="
         : "+m"(*(std::uint64_t(*)[kSize2+1])result), [rhsInd]"+&r"(rhsInd)
         : "m"(*(std::uint64_t(*)[kSize2])rhs), [lhs]"r"(lhs), [rhs]"r"(rhs),
           [result]"r"(result)
         : "rax", "rdx", "cc");
}
