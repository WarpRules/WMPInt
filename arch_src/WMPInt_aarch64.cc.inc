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

    std::uint64_t lhsInd = kSize - 1, rhsInd, lhsValue, rhsValue, temp, res0, res1, flags;
    asm volatile
        ("Loop1%=:\n\t"
         "mov %[rhsInd], %[lhsInd]\n\t" // rhsInd = lhsInd
         "ldr %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t" // lhsValue = lhs[lhsInd]
         "mov %[temp], xzr\n\t" // temp = 0
         "mov %[flags], xzr\n" // flags = 0
         "Loop2%=:\n\t"
         "ldr %[rhsValue], [%[rhs], %[rhsInd], lsl #3]\n\t" // rhsValue = rhs[rhsInd]
         "mul %[res1], %[rhsValue], %[lhsValue]\n\t" // res1 = rhsValue * lhsValue (low)
         "umulh %[res0], %[rhsValue], %[lhsValue]\n\t" // res0 = rhsValue * lhsValue (high)
         "ldr %[rhsValue], [%[result], %[rhsInd], lsl #3]\n\t" // rhsValue = result[rhsInd]
         "adds %[res1], %[res1], %[temp]\n\t" // res1 += temp
         "adc %[temp], %[res0], xzr\n\t" // temp = res0 + (c-flag)
         "msr nzcv, %[flags]\n\t" // nzcv = flags
         "adcs %[rhsValue], %[rhsValue], %[res1]\n\t" // rhsValue += res1
         "str %[rhsValue], [%[result], %[rhsInd], lsl #3]\n\t" // result[rhsInd] = rhsValue
         "mrs %[flags], nzcv\n\t" // flags = nzcv
         "sub %[rhsInd], %[rhsInd], #1\n\t" // --rhsInd
         "cbnz %[rhsInd], Loop2%=\n\t" // if(rhsInd != 0) goto Loop2
         "ldr %[rhsValue], [%[rhs]]\n\t" // rhsValue = *rhs
         "madd %[temp], %[rhsValue], %[lhsValue], %[temp]\n\t" // temp = rhsValue * lhsValue + temp
         "ldr %[rhsValue], [%[result]]\n\t" // rhsValue = *result
         "msr nzcv, %[flags]\n\t" // nzcv = flags
         "adc %[temp], %[temp], %[rhsValue]\n\t" // temp += rhsValue
         "add %[rhs], %[rhs], #8\n\t" // ++rhs
         "str %[temp], [%[result]]\n\t" // *result = temp
         "sub %[lhsInd], %[lhsInd], #1\n\t" // --lhsInd
         "cbnz %[lhsInd], Loop1%=\n\t" // if(lhsInd != 0) goto Loop1
         "ldr %[lhsValue], [%[lhs]]\n\t"
         "ldr %[rhsValue], [%[rhs]]\n\t"
         "madd %[temp], %[rhsValue], %[lhsValue], %[temp]\n\t"
         "str %[temp], [%[result]]"
         : //"+m"(*(std::uint64_t(*)[kSize])result), "=m"(*(std::uint64_t(*)[kSize])tempBuffer),
           [rhs]"+&r"(rhs), [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd),
           [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue), [temp]"=&r"(temp),
           [res0]"=&r"(res0), [res1]"=&r"(res1), [flags]"=&r"(flags)
         : [lhs]"r"(lhs), [result]"r"(result)
           //, "m"(*(std::uint64_t(*)[kSize])lhs), "m"(*(std::uint64_t(*)[kSize])rhs)
         : "cc", "memory");
}


//----------------------------------------------------------------------------
// Full long multiplication
//----------------------------------------------------------------------------
void WMPIntImplementations::doFullLongMultiplication
(const std::uint64_t* lhs, std::size_t kSize1, const std::uint64_t* rhs, std::size_t kSize2, std::uint64_t* result)
{
    const std::size_t resultSize = kSize1 + kSize2;
    const std::uint64_t *rhsPtr = rhs - 1;
    std::uint64_t *resultPtr = result + (kSize1 - 1);
    std::uint64_t lhsInd = kSize1 - 1, rhsInd, lhsValue, rhsValue, temp, res0, res1, flags;
    for(std::size_t i = 0; i < resultSize; ++i) result[i] = 0;

    asm volatile
        ("Loop1%=:\n\t"
         "mov %[rhsInd], %[kSize2]\n\t"
         "ldr %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
         "mov %[temp], xzr\n\t"
         "mov %[flags], xzr\n"
         "Loop2%=:\n\t"
         "ldr %[rhsValue], [%[rhs], %[rhsInd], lsl #3]\n\t"
         "mul %[res1], %[rhsValue], %[lhsValue]\n\t"
         "umulh %[res0], %[rhsValue], %[lhsValue]\n\t"
         "ldr %[rhsValue], [%[result], %[rhsInd], lsl #3]\n\t"
         "adds %[res1], %[res1], %[temp]\n\t"
         "adc %[temp], %[res0], xzr\n\t"
         "msr nzcv, %[flags]\n\t"
         "adcs %[rhsValue], %[rhsValue], %[res1]\n\t"
         "str %[rhsValue], [%[result], %[rhsInd], lsl #3]\n\t"
         "mrs %[flags], nzcv\n\t"
         "sub %[rhsInd], %[rhsInd], #1\n\t"
         "cbnz %[rhsInd], Loop2%=\n\t"
         "ldr %[rhsValue], [%[result]]\n\t"
         "msr nzcv, %[flags]\n\t"
         "adc %[rhsValue], %[rhsValue], %[temp]\n\t"
         "str %[rhsValue], [%[result]], #-8\n\t"
         "subs %[lhsInd], %[lhsInd], #1\n\t"
         "b.hs Loop1%="
         : //"+m"(*(std::uint64_t(*)[resultSize])result), "=m"(*(std::uint64_t(*)[kSize2+1])tempBuffer),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue),
           [temp]"=&r"(temp), [res0]"=&r"(res0), [res1]"=&r"(res1),
           [result]"+&r"(resultPtr), [flags]"=&r"(flags)
         : [lhs]"r"(lhs), [rhs]"r"(rhsPtr), [kSize2]"ir"(kSize2)
           //, "m"(*(std::uint64_t(*)[kSize1])lhs), "m"(*(std::uint64_t(*)[kSize2])rhs)
         : "cc", "memory");
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
        std::size_t srcInd = lhsSize;
        result += lhsSize;
        std::uint64_t lhsValue, rhsValue;
        asm ("msr nzcv, xzr\n"
             "Loop%=:\n\t"
             "sub %[srcInd], %[srcInd], #1\n\t"
             "ldr %[lhsValue], [%[lhs], %[srcInd], lsl #3]\n\t"
             "ldr %[rhsValue], [%[rhs], %[srcInd], lsl #3]\n\t"
             "adcs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
             "str %[lhsValue], [%[result]], #-8\n\t"
             "cbnz %[srcInd], Loop%=\n\t"
             "adc %[lhsValue], xzr, xzr\n\t"
             "str %[lhsValue], [%[result]]"
             : "=m"(*(std::uint64_t(*)[lhsSize+1])result), [result]"+&r"(result),
               [srcInd]"+&r"(srcInd), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue)
             : [lhs]"r"(lhs), [rhs]"r"(rhs),
               "m"(*(std::uint64_t(*)[lhsSize])lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
    else
    {
        std::size_t lhsInd = lhsSize, rhsInd = rhsSize;
        result += rhsSize;
        std::uint64_t lhsValue, rhsValue;
        asm ("msr nzcv, xzr\n"
             "Loop1%=:\n\t"
             "sub %[lhsInd], %[lhsInd], #1\n\t"
             "sub %[rhsInd], %[rhsInd], #1\n\t"
             "ldr %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "ldr %[rhsValue], [%[rhs], %[rhsInd], lsl #3]\n\t"
             "adcs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
             "str %[lhsValue], [%[result]], #-8\n\t"
             "cbnz %[lhsInd], Loop1%=\n"
             "Loop2%=:\n\t"
             "sub %[rhsInd], %[rhsInd], #1\n\t"
             "ldr %[rhsValue], [%[rhs], %[rhsInd], lsl #3]\n\t"
             "adcs %[rhsValue], %[rhsValue], xzr\n\t"
             "str %[rhsValue], [%[result]], #-8\n\t"
             "cbnz %[lhsInd], Loop2%=\n"
             "adc %[rhsValue], xzr, xzr\n\t"
             "str %[rhsValue], [%[result]]"
             : "=m"(*(std::uint64_t(*)[lhsSize+1])result), [result]"+&r"(result),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue)
             : [lhs]"r"(lhs), [rhs]"r"(rhs),
               "m"(*(std::uint64_t(*)[lhsSize])lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
}

static void doAddition(std::uint64_t* lhs, std::size_t lhsSize,
                       const std::uint64_t* rhs, std::size_t rhsSize)
{
    std::size_t lhsInd = lhsSize;
    std::uint64_t lhsValue, rhsValue;
    if(lhsSize <= rhsSize)
    {
        const std::uint64_t* rhsPtr = rhs + (rhsSize - 1);
        asm ("msr nzcv, xzr\n"
             "Loop%=:\n\t"
             "sub %[lhsInd], %[lhsInd], #1\n\t"
             "ldr %[rhsValue], [%[rhs]], #-8\n\t"
             "ldr %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "adcs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
             "str %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "cbnz %[lhsInd], Loop%="
             : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
               [lhsInd]"+&r"(lhsInd), [rhs]"+&r"(rhsPtr), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue)
             : [lhs]"r"(lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
    else
    {
        std::size_t rhsInd = rhsSize;
        asm ("msr nzcv, xzr\n"
             "Loop1%=:\n\t"
             "sub %[lhsInd], %[lhsInd], #1\n\t"
             "sub %[rhsInd], %[rhsInd], #1\n\t"
             "ldr %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "ldr %[rhsValue], [%[rhs], %[rhsInd], lsl #3]\n\t"
             "adcs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
             "str %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "cbnz %[rhsInd], Loop1%=\n"
             "Loop2%=:\n\t"
             "b.cc EndLoop%=\n\t"
             "sub %[lhsInd], %[lhsInd], #1\n\t"
             "ldr %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "adcs %[lhsValue], %[lhsValue], xzr\n\t"
             "str %[lhsValue], [%[lhs], %[lhsInd], lsl #3]\n\t"
             "cbnz %[lhsInd], Loop2%=\n"
             "EndLoop%=:"
             : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
}

static inline void doAddition(std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size)
{

    std::uint64_t lhsValue, rhsValue;
    asm ("msr nzcv, xzr\n"
         "Loop%=:\n\t"
         "sub %[index], %[index], #1\n\t"
         "ldr %[lhsValue], [%[lhs], %[index], lsl #3]\n\t"
         "ldr %[rhsValue], [%[rhs], %[index], lsl #3]\n\t"
         "adcs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
         "str %[lhsValue], [%[lhs], %[index], lsl #3]\n\t"
         "cbnz %[index], Loop%="
         : "+m"(*(std::uint64_t(*)[size])lhs),
           [index]"+&r"(size), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue)
         : "m"(*(std::uint64_t(*)[size])rhs), [lhs]"r"(lhs), [rhs]"r"(rhs)
         : "cc");
}

/* Assumes that lhsSize > rhsSize */
static inline void doSubtraction(std::uint64_t* lhs, std::size_t lhsSize,
                                 const std::uint64_t* rhs, std::size_t rhsSize)
{
    std::size_t lhsInd = lhsSize - 1, rhsInd = rhsSize - 1;
    std::uint64_t lhsValue, rhsValue;
    asm ("ldr %[lhsValue], [%[lhs], %[lhsIndex], lsl #3]\n\t"
         "ldr %[rhsValue], [%[rhs], %[rhsIndex], lsl #3]\n\t"
         "subs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
         "str %[lhsValue], [%[lhs], %[lhsIndex], lsl #3]\n"
         "Loop1%=:\n\t"
         "sub %[lhsIndex], %[lhsIndex], #1\n\t"
         "sub %[rhsIndex], %[rhsIndex], #1\n\t"
         "ldr %[lhsValue], [%[lhs], %[lhsIndex], lsl #3]\n\t"
         "ldr %[rhsValue], [%[rhs], %[rhsIndex], lsl #3]\n\t"
         "sbcs %[lhsValue], %[lhsValue], %[rhsValue]\n\t"
         "str %[lhsValue], [%[lhs], %[lhsIndex], lsl #3]\n\n"
         "cbnz %[rhsIndex], Loop1%=\n"
         "Loop2%=:\n\t"
         "b.cs EndLoop%=\n\t"
         "sub %[lhsIndex], %[lhsIndex], #1\n\t"
         "ldr %[lhsValue], [%[lhs], %[lhsIndex], lsl #3]\n\t"
         "sbcs %[lhsValue], %[lhsValue], xzr\n\t"
         "str %[lhsValue], [%[lhs], %[lhsIndex], lsl #3]\n\n"
         "cbnz %[lhsIndex], Loop2%=\n"
         "EndLoop%=:"
         : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
           [lhsIndex]"+&r"(lhsInd), [rhsIndex]"+&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [rhsValue]"=&r"(rhsValue)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
         : "cc");
}

static void doFullLongMultiplication_1xN
(std::uint64_t lhs, const std::uint64_t* rhs, std::size_t kSize2, std::uint64_t* result)
{
    std::uint64_t rhsValue, mulRes, temp = 0;
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
         : "=m"(*(std::uint64_t(*)[kSize2+1])result), [rhsInd]"+&r"(kSize2),
           [temp]"+&r"(temp), [rhsValue]"=&r"(rhsValue), [mulRes]"=&r"(mulRes)
         : "m"(*(std::uint64_t(*)[kSize2])rhs), [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result + kSize2)
         : "cc");
}
