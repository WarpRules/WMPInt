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
         "str %[rhsValue], [%[result]]\n\t"
         "sub %[result], %[result], #8\n\t"
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
}

static void doAddition(std::uint64_t* lhs, std::size_t lhsSize,
                       const std::uint64_t* rhs, std::size_t rhsSize)
{
}

static inline void doAddition(std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size)
{
}

/* Assumes that lhsSize > rhsSize */
static inline void doSubtraction(std::uint64_t* lhs, std::size_t lhsSize,
                                 const std::uint64_t* rhs, std::size_t rhsSize)
{
}

static void doFullLongMultiplication_1xN
(std::uint64_t lhs, const std::uint64_t* rhs, std::size_t kSize2, std::uint64_t* result)
{
}
