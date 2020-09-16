#include "WMPInt.hh"

#ifdef WMPINT_DEBUG_MODE
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
#ifdef WMPINT_DEBUG_MODE
    if(tempBuffer + kSize > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer + kSize;
#endif

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
    asm ("Loop1%=:\n\t"
         // rhs * lhs[lhsInd] into the tempBuf
         "movq %[lhsInd], %[rhsInd]\n\t"
         "movq (%[lhs], %[lhsInd], 8), %[lhsValue]\n\t"
         "xorl %k[temp], %k[temp]\n"
         "Loop3%=:\n\t"
         "movq (%[rhs], %[rhsInd], 8), %%rax\n\t"
         "mulq %[lhsValue]\n\t"
         "addq %%rax, %[temp]\n\t"
         "movq %[temp], (%[tempBuf], %[rhsInd], 8)\n\t"
         "movl $0, %k[temp]\n\t"
         "adcq %%rdx, %[temp]\n\t"
         "decq %[rhsInd]\n\t"
         "jnz Loop3%=\n\t"
         "imulq (%[rhs]), %[lhsValue]\n\t"
         "addq %[lhsValue], %[temp]\n\t"
         "mov %[temp], (%[tempBuf])\n\t"
         // Add tempBuf to the result:
         "movq %[lhsInd], %[rhsInd]\n\t"
         "clc\n"
         "Loop4%=:\n\t"
         "movq (%[tempBuf], %[rhsInd], 8), %[lhsValue]\n\t"
         "adcq %[lhsValue], (%[result], %[rhsInd], 8)\n\t"
         "decq %[rhsInd]\n\t"
         "jns Loop4%=\n\t"
         // Advance rhs and decrement lhsInd:
         "leaq 8(%[rhs]), %[rhs]\n\t"
         "decq %[lhsInd]\n\t"
         "jnz Loop1%=\n\t"
         // Do the final result[0] = lhs[0] * rhs[0]
         "movq (%[lhs]), %[lhsValue]\n\t"
         "imulq (%[rhs]), %[lhsValue]\n\t"
         "addq %[lhsValue], (%[result])"
         : "+m"(*(std::uint64_t(*)[kSize])result),
           "=m"(*(std::uint64_t(*)[kSize])tempBuffer), [rhs]"+&r"(rhs),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [temp]"=&r"(temp)
         : [lhs]"r"(lhs), [result]"r"(result), [tempBuf]"r"(tempBuffer),
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
#ifdef WMPINT_DEBUG_MODE
    if(tempBuffer + (kSize2+1) > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer + (kSize2+1);
#endif

    // tempBuffer size is kSize2+1, result size is kSize1+kSize2
    const std::size_t resultSize = kSize1 + kSize2;
    std::uint64_t* resultPtr = result + (kSize1 - 1);
    std::uint64_t lhsInd = kSize1 - 1, rhsInd, lhsValue, temp;
    for(std::size_t i = 0; i < resultSize; ++i) result[i] = 0;

    asm (/* Outer loop: lhsInd = [kSize-1, 0] */
         "L1%=:\n\t"
         /* Inner loop 1: rhsInd = [kSize2, 1] */
         "movq %[kSize2], %[rhsInd]\n\t" // rhsInd = kSize2
         "movq (%[lhs],%[lhsInd],8), %[lhsValue]\n\t" // lhsValue = lhs[lhsInd]
         "xorl %k[temp], %k[temp]\n" // temp = 0
         "L3%=:\n\t"
         "movq -8(%[rhs],%[rhsInd],8), %%rax\n\t" // rax = rhs[rhsInd-1]
         "mulq %[lhsValue]\n\t" // (rdx,rax) = rax * lhsValue
         "addq %%rax, %[temp]\n\t" // temp += rax
         "movq %[temp], (%[tempBuf], %[rhsInd], 8)\n\t" // tempBuf[rhsInd] = temp
         "movl %0, %k[temp]\n\t" // temp = 0
         "adcq %%rdx, %[temp]\n\t" // temp += rdx
         "decq %[rhsInd]\n\t" // --rhsInd
         "jnz L3%=\n\t" // if(rhsInd > 0) goto L3
         /* Inner loop 2: rhsInd = [kSize2, 0] */
         "movq %[kSize2], %[rhsInd]\n\t" // rhsInd = kSize2
         "clc\n"
         "L4%=:\n\t"
         "movq (%[tempBuf],%[rhsInd],8),%%rax\n\t" // rax = tempBuf[rhsInd]
         "adcq %%rax,(%[result],%[rhsInd],8)\n\t" // result[rhsInd] += rax
         "decq %[rhsInd]\n\t" // --rhsInd
         "jnz L4%=\n\t" // if(rhsInd > 0) goto L4
         "adcq %[temp], (%[result])\n\t" // *result += temp
         "leaq -8(%[result]), %[result]\n\t"
         "decq %[lhsInd]\n\t" // --lhsInd
         "jns L1%=" // if(lhsInd >= 0) goto L1
         : "+m"(*(std::uint64_t(*)[resultSize])result),
           "=m"(*(std::uint64_t(*)[kSize2+1])tempBuffer),
           [lhsInd]"+&r"(lhsInd), [rhsInd]"=&r"(rhsInd), [lhsValue]"=&r"(lhsValue), [temp]"=&r"(temp),
           [result]"+&r"(resultPtr)
         : [lhs]"r"(lhs), [rhs]"r"(rhs), [tempBuf]"r"(tempBuffer), [kSize2]"irm"(kSize2),
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
(std::uint64_t lhs, const std::uint64_t* rhs, std::size_t kSize2,
 std::uint64_t* result)
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


//----------------------------------------------------------------------------
// Karatsuba long multiplication
//----------------------------------------------------------------------------
/* If only multiplication between same-sized operands were supported, then
   the implementation of doFullKaratsubaMultiplicationForSameSizes() below
   would be enough (and make this much simpler). However, since we are supporting
   operands of different sizes, it becomes more complicated to do it optimally.

   We could expand the smaller operand to the size of the larger one by adding
   zeros, but this would not be optimal in terms of speed, as it would perform
   more operations that would be needed (essentially lots of multiplications by
   zero). The larger the difference in sizes, the less optimal this would be.
   If the smaller operand is less than half the size of the larger one, a slightly
   more optimal solution would be to expand the smaller to half the size of the
   larger, and create a specialized first-stage implementation that deals with
   this situation, and then calls the implementation for same-sized operands for
   the halves. However, while a bit better, it would still not be optimal. Again,
   the larger the difference in sizes, the less optimal it would be.

   The next better approach would be, if the sizes of the operands differ, to split
   the operands at the size of the smaller one, and perform the first iteration of
   the algorithm with that division, which ought to incur only a negligible penalty.
   A curious side-effect of doing this is that it can drastically increase the size
   requirement for the tempBuffer. Ironically, and unintuitively, the smaller one
   of the operands is, and the larger the other, the larger the tempBuffer size
   requirement. Much larger than if both operands were of that larger size.
   Also, ironically, the implementation of this isn't significantly simpler than the
   optimal implementation, so nothing is really saved in terms of code complexity.

   The optimal Karatsuba algorithm always splits from the middle of the larger operand.
   But with different-sized operands this means that three versions of the algorithm
   are needed, which may call each other recursively:
    - a version that handles operands of the same size (only needs to call itself),
    - a version for when lhsSize <= (rhsSize+1)/2,
    - a version for when lhsSize > (rhsSize+1)/2 and < rhsSize.
   The latter two may call any of the three recursively.
   The advantage of the second version is that it can skip one of the recursive calls
   completely (because it would just be a multiplication by zero), as well as one of the
   subtractions (likewise because it would just be subtraction with zero).
*/

/* This must only be called when rhs and lhs are of the same size */
static void doFullKaratsubaMultiplicationForSameSizes
(const std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(size <= 32)
        return WMPIntImplementations::doFullLongMultiplication
            (lhs, size, rhs, size, result, tempBuffer);

    const std::size_t resultSize = size * 2;
    const std::size_t lowSize = (size+1) / 2;
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

static void doFullKaratsubaMultiplication
(const std::uint64_t*, std::size_t, const std::uint64_t*, std::size_t,
 std::uint64_t*, std::uint64_t*);

/* This must only be called when lhsSize <= (rhsSize+1)/2 */
static void doFullKaratsubaMultiplicationForSmallLHS
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(lhsSize == 1)
        return doFullLongMultiplication_1xN(*lhs, rhs, rhsSize, result);
    if(lhsSize == 2 || rhsSize <= 32)
        return WMPIntImplementations::doFullLongMultiplication
            (rhs, rhsSize, lhs, lhsSize, result, tempBuffer);

    const std::size_t resultSize = lhsSize + rhsSize;
    const std::size_t rhsLowSize = (rhsSize+1) / 2;
    const std::size_t rhsHighSize = rhsSize - rhsLowSize;
    const std::uint64_t* rhsLow = rhs + rhsHighSize;

    const std::size_t z0Size = rhsLowSize + lhsSize;
    const std::size_t z0Position = resultSize - z0Size;
    std::uint64_t* z0 = result + z0Position;

    for(std::size_t i = 0; i < z0Position; ++i)
        result[i] = 0;

    ::doFullKaratsubaMultiplication(lhs, lhsSize, rhsLow, rhsLowSize, z0, tempBuffer);

    const std::size_t z1Size = rhsHighSize + lhsSize;
    std::uint64_t* z1 = tempBuffer;
    if(lhsSize <= rhsHighSize)
        ::doFullKaratsubaMultiplication(lhs, lhsSize, rhs, rhsHighSize, z1, tempBuffer+z1Size);
    else
        ::doFullKaratsubaMultiplication(rhs, rhsHighSize, lhs, lhsSize, z1, tempBuffer+z1Size);

    doAddition(result, z1, z1Size);
}

/* This must only be called when lhsSize > (rhsSize+1)/2 and lhsSize < rhsSize */
static void doFullKaratsubaMultiplicationForLargeLHS
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(rhsSize <= 32)
        return WMPIntImplementations::doFullLongMultiplication
            (rhs, rhsSize, lhs, lhsSize, result, tempBuffer);

    const std::size_t resultSize = lhsSize + rhsSize;
    const std::size_t lowSize = (rhsSize+1) / 2;
    const std::size_t rhsHighSize = rhsSize - lowSize;
    const std::size_t lhsHighSize = lhsSize - lowSize;
    const std::uint64_t* rhsLow = rhs + rhsHighSize;
    const std::uint64_t* lhsLow = lhs + lhsHighSize;

    const std::size_t z0Size = lowSize * 2;
    std::uint64_t* z0 = result + (resultSize - z0Size);
    doFullKaratsubaMultiplicationForSameSizes(lhsLow, rhsLow, lowSize, z0, tempBuffer);

    const std::size_t z2Size = lhsHighSize + rhsHighSize;
    std::uint64_t* z2 = result;
    ::doFullKaratsubaMultiplication(lhs, lhsHighSize, rhs, rhsHighSize, z2, tempBuffer);

    const std::size_t highPlusLowSize = lowSize + 1;
    std::uint64_t* lhsHighPlusLow = tempBuffer;
    doFullAddition(lhs, lhsHighSize, lhsLow, lowSize, lhsHighPlusLow);

    std::uint64_t* rhsHighPlusLow = lhsHighPlusLow + highPlusLowSize;
    doFullAddition(rhs, rhsHighSize, rhsLow, lowSize, rhsHighPlusLow);

    const std::size_t z1Size = highPlusLowSize * 2;
    std::uint64_t* z1 = rhsHighPlusLow + highPlusLowSize;
    doFullKaratsubaMultiplicationForSameSizes
        (lhsHighPlusLow, rhsHighPlusLow, highPlusLowSize, z1, z1 + z1Size);

    doSubtraction(z1, z1Size, z2, z2Size);
    doSubtraction(z1, z1Size, z0, z0Size);
    doAddition(result, resultSize - lowSize, z1, z1Size);
}

static void doFullKaratsubaMultiplication
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(lhsSize == rhsSize)
        doFullKaratsubaMultiplicationForSameSizes
            (lhs, rhs, lhsSize, result, tempBuffer);
    else if(lhsSize <= (rhsSize+1)/2)
        doFullKaratsubaMultiplicationForSmallLHS
            (lhs, lhsSize, rhs, rhsSize, result, tempBuffer);
    else
        doFullKaratsubaMultiplicationForLargeLHS
            (lhs, lhsSize, rhs, rhsSize, result, tempBuffer);
}

/* This assumes that lhsSize <= rhsSize. (If that's not so, it will go out of bounds.) */
void WMPIntImplementations::doFullKaratsubaMultiplication
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
#ifdef WMPINT_DEBUG_MODE
    gWMPInt_tempBuffer_max_position = tempBuffer;
#endif

    ::doFullKaratsubaMultiplication(lhs, lhsSize, rhs, rhsSize, result, tempBuffer);

#ifdef WMPINT_DEBUG_MODE
    gWMPInt_karatsuba_max_temp_buffer_size = gWMPInt_tempBuffer_max_position - tempBuffer;
#endif
}

/* This assumes size > 1 (else doLongMultiplication() will crash) */
static void doTruncatedKaratsubaMultiplication
(const std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(size <= 64)
        return WMPIntImplementations::doLongMultiplication(size, lhs, rhs, result, tempBuffer);

    const std::size_t lhsLowSize = size / 2;
    const std::size_t lhsHighSize = size - lhsLowSize;
    const std::size_t rhsLowSize = (size + 1) / 2;
    const std::size_t rhsHighSize = size - rhsLowSize;
    const std::uint64_t* lhsLow = lhs + lhsHighSize;
    const std::uint64_t* rhsLow = rhs + rhsHighSize;

    ::doFullKaratsubaMultiplication
        (lhsLow, lhsLowSize, rhsLow, rhsLowSize, result, tempBuffer);

    ::doTruncatedKaratsubaMultiplication
        (lhs, rhsLow, lhsHighSize, tempBuffer, tempBuffer + lhsHighSize);

    doAddition(result, tempBuffer, lhsHighSize);

    ::doTruncatedKaratsubaMultiplication
        (lhsLow, rhs, lhsLowSize, tempBuffer, tempBuffer + lhsLowSize);

    doAddition(result, tempBuffer, lhsLowSize);
}

void WMPIntImplementations::doTruncatedKaratsubaMultiplication
(const std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
#ifdef WMPINT_DEBUG_MODE
    gWMPInt_tempBuffer_max_position = tempBuffer;
#endif

    ::doTruncatedKaratsubaMultiplication(lhs, rhs, size, result, tempBuffer);

#ifdef WMPINT_DEBUG_MODE
    gWMPInt_karatsuba_max_temp_buffer_size = gWMPInt_tempBuffer_max_position - tempBuffer;
#endif
}


//----------------------------------------------------------------------------
// Output as decimal string
//----------------------------------------------------------------------------
#include <cstdio>

/* In all these functions "destination" points to the *last* position in the destination
   char array, to the last character where the outputted string should be put (it will be
   decremented as the value is outputted).
*/
static char* print_uint64_full_length(std::uint64_t value, char* destination)
{
    for(unsigned i = 0; i < 19; ++i)
    {
        const std::uint64_t remainder = value % 10;
        value /= 10;
        *destination-- = static_cast<char>(remainder) + '0';
    }
    return destination;
}

static char* print_uint64(std::uint64_t value, char* destination)
{
    while(value)
    {
        const std::uint64_t remainder = value % 10;
        value /= 10;
        *destination-- = static_cast<char>(remainder) + '0';
    }
    return destination;
}

char* WMPIntImplementations::printAsDecStrAndReset
(std::uint64_t* value, std::size_t valueSize, char* destination)
{
    std::size_t leftIndex = 0;
    while(leftIndex < valueSize && !value[leftIndex]) ++leftIndex;

    if(leftIndex == valueSize) // the value is all zeros
    {
        *destination = '0';
        return destination;
    }

    std::size_t remainingValueSize = valueSize - leftIndex;
    std::uint64_t rhs = UINT64_C(10000000000000000000);

    while(true)
    {
        std::size_t counter = remainingValueSize;
        std::uint64_t* lhs = value + leftIndex, remainder;
        asm ("xorl %%edx, %%edx\n"
             "L1%=:\n\t"
             "movq (%[lhs]), %%rax\n\t"
             "divq %[rhs]\n\t"
             "movq %%rax, (%[lhs])\n\t"
             "leaq 8(%[lhs]), %[lhs]\n\t"
             "decq %[counter]\n\t"
             "jnz L1%="
             : "+m"(*(std::uint64_t(*)[valueSize])value),
               [lhs]"+&r"(lhs), [counter]"+&r"(counter), "=&d"(remainder)
             : [rhs]"r"(rhs) : "rax", "cc");

        if(!value[leftIndex])
        {
            ++leftIndex;
            if(--remainingValueSize == 0)
                return print_uint64(remainder, destination) + 1;
        }

        destination = print_uint64_full_length(remainder, destination);
    }
}
