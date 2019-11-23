#include "WMPInt.hh"

#define DEBUG_MODE
#ifdef DEBUG_MODE
std::size_t gWMPInt_karatsuba_max_temp_buffer_size = 0;
static std::uint64_t* gWMPInt_tempBuffer_max_position = 0;
#endif

//----------------------------------------------------------------------------
// Long multiplication
//----------------------------------------------------------------------------
void WMPIntImplementations::doLongMultiplication
(std::size_t kSize, const std::uint64_t* lhs, const std::uint64_t* rhs,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    std::uint64_t lhsInd = kSize - 2, rhsInd = kSize - 1, rhsIndCounter, lhsValue = lhs[kSize - 1];
    for(std::size_t i = 0; i < kSize; ++i) result[i] = 0;

    asm (/* Multiplication by [L0] */
         /* Loop: rhsInd = [kSize-1, 1] */
         "\nL1%=:\n\t"
         "movq (%[rhs],%[rhsInd],8),%%rax\n\t" // rax = rhs[rhsInd]
         "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
         "addq %%rax,(%[result],%[rhsInd],8)\n\t" // result[rhsInd] += rax
         "adcq %%rdx,-8(%[result],%[rhsInd],8)\n\t" // result[rhsInd-1] = rdx
         "decq %[rhsInd]\n\t" // --rhsInd
         "jnz L1%=\n\t" // if(rhsInd > 0) goto L1
         "imulq (%[rhs]),%[lhsValue]\n\t" // lhsValue = rhs[0] * lhsValue
         "addq %[lhsValue],(%[result])\n" // result[0] += lhsValue
         /* Multiplication by the remaining of lhs. */
         /* Outer loop: lhsInd = [kSize-2, 0] */
         "L2%=:\n\t"
         /* tempBuf has to be zeroed: */
         "xorq %[lhsValue],%[lhsValue]\n\t" // lhsValue = 0
         "movq %[lhsInd],%[rhsIndCounter]\n\t" // rhsIndCounter = lhsInd
         "movq %[lhsValue],8(%[tempBuf],%[lhsInd],8)\n" // tempBuf[lhsInd+1]=0
         "L5%=:\n\t"
         "movq %[lhsValue],(%[tempBuf],%[rhsIndCounter],8)\n\t" // tempBuf[rhsIndCounter]=0
         "decq %[rhsIndCounter]\n\t" // --rhsIndCounter
         "jns L5%=\n\t" // if(rhsIndCounter >= 0) goto L5
         /* Inner loop 1: rhsIndCounter = [lhsInd, 0] */
         "movq %[lhsInd],%[rhsIndCounter]\n\t" // rhsIndCounter = lhsInd
         "movq %[kSizeM1],%[rhsInd]\n\t" // rhsInd = kSize-1
         "movq (%[lhs],%[lhsInd],8),%[lhsValue]\n" // lhsValue = lhs[lhsInd]
         "L3%=:\n\t"
         "movq (%[rhs],%[rhsInd],8),%%rax\n\t" // rax = rhs[rhsInd]
         "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
         "decq %[rhsInd]\n\t" // --rhsInd
         "addq %%rax,8(%[tempBuf],%[rhsIndCounter],8)\n\t" // tempBuf[rhsIndCounter+1] += rax
         "adcq %%rdx,(%[tempBuf],%[rhsIndCounter],8)\n\t" // tmpBuf[rhsIndCounter] += rdx
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
           [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd),
           [rhsIndCounter]"=&r"(rhsIndCounter), [lhsValue]"+&r"(lhsValue)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result),
           [tempBuf]"r"(tempBuffer), [kSizeM1]"rm"(kSize - 1),
           "m"(*(std::uint64_t(*)[kSize])lhs), "m"(*(std::uint64_t(*)[kSize])rhs)
         : "rax", "rdx", "cc");
}


//----------------------------------------------------------------------------
// Full long multiplication
//----------------------------------------------------------------------------
void WMPIntImplementations::doFullLongMultiplication
(const std::uint64_t* lhs, std::size_t kSize1, const std::uint64_t* rhs, std::size_t kSize2,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
#ifdef DEBUG_MODE
    if(tempBuffer + (kSize2+1) > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer + (kSize2+1);
#endif

    // tempBuffer size is kSize2+1, result size is kSize1+kSize2
    const std::size_t resultSize = kSize1 + kSize2;
    std::uint64_t resultStartInd = resultSize - 1, resultInd;
    std::uint64_t lhsInd = kSize1 - 1, rhsInd, lhsValue;
    for(std::size_t i = 0; i < resultSize; ++i) result[i] = 0;

    asm (/* Outer loop: lhsInd = [kSize-1, 0] */
         "L1%=:\n\t"
         /* Zero tempBuf: */
         "xorq %[lhsValue], %[lhsValue]\n\t" // lhsValue = 0
         "movq %[kSize2], %[rhsInd]\n" // rhsInd = kSize2
         "L2%=:\n\t"
         "movq %[lhsValue], (%[tempBuf],%[rhsInd],8)\n\t" // tempBuf[rhsInd]=0
         "decq %[rhsInd]\n\t" // --rhsInd
         "jns L2%=\n\t" // if(rhsInd >= 0) goto L2
         /* Inner loop 1: rhsInd = [kSize2, 1] */
         "movq %[kSize2], %[rhsInd]\n\t" // rhsInd = kSize2
         "movq (%[lhs],%[lhsInd],8), %[lhsValue]\n" // lhsValue = lhs[lhsInd]
         "L3%=:\n\t"
         "movq -8(%[rhs],%[rhsInd],8), %%rax\n\t" // rax = rhs[rhsInd-1]
         "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
         "addq %%rax,(%[tempBuf], %[rhsInd],8)\n\t" // tempBuf[rhsInd] += rax
         "adcq %%rdx,-8(%[tempBuf], %[rhsInd],8)\n\t" // tmpBuf[rhsInd-1] += rdx
         "decq %[rhsInd]\n\t" // --rhsInd
         "jnz L3%=\n\t" // if(rhsInd > 0) goto L3
         /* Inner loop 2: rhsInd = [kSize2, 0] */
         "movq %[kSize2], %[rhsInd]\n\t" // rhsInd = kSize2
         "movq %[resultStartInd], %[resultInd]\n\t" // resultInd = resultStartInd
         "clc\n"
         "L4%=:\n\t"
         "movq (%[tempBuf],%[rhsInd],8),%%rax\n\t" // rax = tempBuf[rhsInd]
         "adcq %%rax,(%[result],%[resultInd],8)\n\t" // result[resultInd] += rax
         "decq %[resultInd]\n\t" // --resultInd
         "decq %[rhsInd]\n\t" // --rhsInd
         "jns L4%=\n\t" // if(rhsIndCounter >= 0) goto L4
         "decq %[resultStartInd]\n\t" // --resultStartInd
         "decq %[lhsInd]\n\t" // --lhsInd
         "jns L1%=" // if(lhsInd >= 0) goto L1
         : "+m"(*(std::uint64_t(*)[resultSize])result),
           "=m"(*(std::uint64_t(*)[kSize2+1])tempBuffer),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd), [lhsValue]"=&r"(lhsValue),
           [resultStartInd]"+&r"(resultStartInd), [resultInd]"=&r"(resultInd)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result),
           [tempBuf]"r"(tempBuffer), [kSize2]"irm"(kSize2),
           "m"(*(std::uint64_t(*)[kSize1])lhs), "m"(*(std::uint64_t(*)[kSize2])rhs)
         : "rax", "rdx", "cc");
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
        std::uint64_t value, zero = 0;
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[lhs],%[srcInd],8), %[value]\n\t"
             "adcq (%[rhs],%[srcInd],8), %[value]\n\t"
             "movq %[value], 8(%[result],%[srcInd],8)\n\t"
             "decq %[srcInd]\n\t"
             "jns L1%=\n\t"
             "adcq %[zero], (%[result])"
             : "=m"(*(std::uint64_t(*)[lhsSize+1])result),
               [srcInd]"+&r"(srcInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result), [zero]"r"(zero),
               "m"(*(std::uint64_t(*)[lhsSize])lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
    else
    {
        for(std::size_t i = 0; i < rhsSize - lhsSize + 1; ++i) result[i] = 0;
        std::size_t lhsInd = lhsSize-1, rhsInd = rhsSize-1;
        std::uint64_t value, zero = 0;
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
             "adcq %[zero], (%[result])"
             : "=m"(*(std::uint64_t(*)[rhsSize+1])result),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), [result]"r"(result), [zero]"r"(zero),
               "m"(*(std::uint64_t(*)[lhsSize])lhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
}

static void doAddition(std::uint64_t* lhs, std::size_t lhsSize,
                       const std::uint64_t* rhs, std::size_t rhsSize)
{
    std::size_t lhsInd = lhsSize - 1, rhsInd = rhsSize - 1;
    std::uint64_t value;
    if(lhsSize <= rhsSize)
    {
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[rhs],%[rhsInd],8), %[value]\n\t"
             "adcq %[value], (%[lhs],%[lhsInd],8)\n\t"
             "decq %[rhsInd]\n\t"
             "decq %[lhsInd]\n\t"
             "jns L1%="
             : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
    else
    {
        std::uint64_t zero = 0;
        asm ("clc\n"
             "L1%=:\n\t"
             "movq (%[rhs],%[rhsInd],8), %[value]\n\t"
             "adcq %[value], (%[lhs],%[lhsInd],8)\n\t"
             "decq %[lhsInd]\n\t"
             "decq %[rhsInd]\n\t"
             "jns L1%=\n"
             "L2%=:\n\t"
             "adcq %[zero], (%[lhs],%[lhsInd],8)\n\t"
             "decq %[lhsInd]\n\t"
             "jns L2%="
             : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
               [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
             : [lhs]"r"(lhs), [rhs]"r"(rhs), [zero]"r"(zero),
               "m"(*(std::uint64_t(*)[rhsSize])rhs)
             : "cc");
    }
}

/* Assumes that lhsSize > rhsSize */
static void doSubtraction(std::uint64_t* lhs, std::size_t lhsSize,
                          const std::uint64_t* rhs, std::size_t rhsSize)
{
    std::size_t lhsInd = lhsSize - 1, rhsInd = rhsSize - 1;
    std::uint64_t value, zero = 0;
    asm ("clc\n"
         "L1%=:\n\t"
         "movq (%[rhs],%[rhsInd],8), %[value]\n\t"
         "sbbq %[value], (%[lhs],%[lhsInd],8)\n\t"
         "decq %[lhsInd]\n\t"
         "decq %[rhsInd]\n\t"
         "jns L1%=\n"
         "L2%=:\n\t"
         "sbbq %[zero], (%[lhs],%[lhsInd],8)\n\t"
         "decq %[lhsInd]\n\t"
         "jns L2%="
         : "+m"(*(std::uint64_t(*)[lhsSize])lhs),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"+&r"(rhsInd), [value]"=&r"(value)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), [zero]"r"(zero),
           "m"(*(std::uint64_t(*)[rhsSize])rhs)
         : "cc");
}


//----------------------------------------------------------------------------
// Karatsuba long multiplication
//----------------------------------------------------------------------------
static void doFullKaratsubaMultiplicationForSameSizes
(const std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(size <= 4)
        return WMPIntImplementations::doFullLongMultiplication
            (lhs, size, rhs, size, result, tempBuffer);

    const std::size_t lowSize = (size+1) / 2;
    const std::size_t resultSize = size * 2;
    const std::size_t highSize = size - lowSize;
    const std::uint64_t* lhsLow = lhs + highSize;
    const std::uint64_t* rhsLow = rhs + highSize;

    const std::size_t z0Size = lowSize * 2;
    std::uint64_t* z0 = result + (resultSize - z0Size);
    doFullKaratsubaMultiplicationForSameSizes(lhsLow, rhsLow, lowSize, z0, tempBuffer);

    const std::size_t z2Size = highSize * 2;
    std::uint64_t* z2 = result;
    doFullKaratsubaMultiplicationForSameSizes(lhs, rhs, highSize, z2, tempBuffer);

    const std::size_t highPlusLowSize = lowSize + 1;
    std::uint64_t* lhsHighPlusLow = tempBuffer;
    doFullAddition(lhs, highSize, lhsLow, lowSize, lhsHighPlusLow);

    std::uint64_t* rhsHighPlusLow = lhsHighPlusLow + highPlusLowSize;
    doFullAddition(rhs, highSize, rhsLow, lowSize, rhsHighPlusLow);

    const std::size_t z1Size = highPlusLowSize * 2;
    std::uint64_t* z1 = rhsHighPlusLow + highPlusLowSize;
    doFullKaratsubaMultiplicationForSameSizes
        (lhsHighPlusLow, rhsHighPlusLow, highPlusLowSize, z1, z1 + z1Size);

    doSubtraction(z1, z1Size, z2, z2Size);
    doSubtraction(z1, z1Size, z0, z0Size);
    doAddition(result, resultSize - lowSize, z1, z1Size);
}

static void doFullKaratsubaMultiplicationForDifferentSizes
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(lhsSize <= 2)
        return WMPIntImplementations::doFullLongMultiplication
            (rhs, rhsSize, lhs, lhsSize, result, tempBuffer);

    const std::size_t lowSize = lhsSize;
    const std::size_t resultSize = lhsSize + rhsSize;
    const std::size_t rhsHighSize = rhsSize - lowSize;
    const std::uint64_t* rhsLow = rhs + rhsHighSize;

    const std::size_t z0Size = lowSize * 2;
    std::uint64_t* z0 = result + (resultSize - z0Size);
    doFullKaratsubaMultiplicationForSameSizes(lhs, rhsLow, lowSize, z0, tempBuffer);

    const std::size_t z2Size = rhsHighSize;
    std::uint64_t* z2 = result;
    for(std::size_t i = 0; i < z2Size; ++i) z2[i] = 0;

    const std::size_t highPlusLowSize = (rhsHighSize > lowSize ? rhsHighSize : lowSize) + 1;
    std::uint64_t* rhsHighPlusLow = tempBuffer;
    if(rhsHighSize > lowSize)
        doFullAddition(rhsLow, lowSize, rhs, rhsHighSize, rhsHighPlusLow);
    else
        doFullAddition(rhs, rhsHighSize, rhsLow, lowSize, rhsHighPlusLow);

    std::uint64_t* lhsHighPlusLow = rhsHighPlusLow + highPlusLowSize;
    const std::size_t zerosAmount = highPlusLowSize - lowSize;
    for(std::size_t i = 0; i < zerosAmount; ++i)
        lhsHighPlusLow[i] = 0;
    for(std::size_t srcInd = 0, destInd=zerosAmount; srcInd < lhsSize; ++srcInd, ++destInd)
        lhsHighPlusLow[destInd] = lhs[srcInd];

    const std::size_t z1Size = highPlusLowSize * 2;
    std::uint64_t* z1 = lhsHighPlusLow + highPlusLowSize;
    doFullKaratsubaMultiplicationForSameSizes
        (lhsHighPlusLow, rhsHighPlusLow, highPlusLowSize, z1, z1 + z1Size);

    doSubtraction(z1, z1Size, z0, z0Size);
    doAddition(result, resultSize - lowSize, z1, z1Size);
}

/* This assumes that lhsSize <= rhsSize. (If that's not so, it will go out of bounds.) */
void WMPIntImplementations::doFullKaratsubaMultiplication
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
#ifdef DEBUG_MODE
    gWMPInt_tempBuffer_max_position = tempBuffer;
#endif

    if(lhsSize < rhsSize)
        doFullKaratsubaMultiplicationForDifferentSizes
            (lhs, lhsSize, rhs, rhsSize, result, tempBuffer);
    else
        doFullKaratsubaMultiplicationForSameSizes
            (lhs, rhs, lhsSize, result, tempBuffer);

#ifdef DEBUG_MODE
    gWMPInt_karatsuba_max_temp_buffer_size = gWMPInt_tempBuffer_max_position - tempBuffer;
#endif
}
