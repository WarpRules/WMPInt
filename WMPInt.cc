#include "WMPInt.hh"

#ifdef WMPINT_DEBUG_MODE
std::size_t gWMPInt_karatsuba_max_temp_buffer_size = 0;
static std::uint64_t* gWMPInt_tempBuffer_max_position = 0;
#endif

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
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
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
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
}


//----------------------------------------------------------------------------
// Karatsuba algorithm helper functions
//----------------------------------------------------------------------------
/* This assumes 1 < lhsSize <= rhsSize. */
static void doFullAddition(const std::uint64_t* lhs, std::size_t lhsSize,
                           const std::uint64_t* rhs, std::size_t rhsSize,
                           std::uint64_t* result)
{
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
}

static void doAddition(std::uint64_t* lhs, std::size_t lhsSize,
                       const std::uint64_t* rhs, std::size_t rhsSize)
{
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
}

static inline void doAddition(std::uint64_t* lhs, const std::uint64_t* rhs, std::size_t size)
{
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
}

/* Assumes that lhsSize > rhsSize */
static inline void doSubtraction(std::uint64_t* lhs, std::size_t lhsSize,
                                 const std::uint64_t* rhs, std::size_t rhsSize)
{
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
}

static void doFullLongMultiplication_1xN
(std::uint64_t lhs, const std::uint64_t* rhs, std::size_t kSize2, std::uint64_t* result)
{
#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
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
#endif
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
        return WMPIntImplementations::doFullLongMultiplication(lhs, size, rhs, size, result);

    const std::size_t resultSize = size * 2;
    const std::size_t lowSize = (size+1) / 2;
    const std::size_t highSize = size - lowSize;
    const std::uint64_t* lhsLow = lhs + highSize;
    const std::uint64_t* rhsLow = rhs + highSize;

    const std::size_t highPlusLowSize = lowSize + 1;
    std::uint64_t* lhsHighPlusLow = result;
    doFullAddition(lhs, highSize, lhsLow, lowSize, lhsHighPlusLow);

    std::uint64_t* rhsHighPlusLow = lhsHighPlusLow + highPlusLowSize;
    doFullAddition(rhs, highSize, rhsLow, lowSize, rhsHighPlusLow);

    const std::size_t z1Size = highPlusLowSize * 2;
    std::uint64_t* z1 = tempBuffer;
    std::uint64_t* tempBuffer2 = tempBuffer + z1Size;

    doFullKaratsubaMultiplicationForSameSizes
        (lhsHighPlusLow, rhsHighPlusLow, highPlusLowSize, z1, tempBuffer2);

    const std::size_t z0Size = lowSize * 2;
    std::uint64_t* z0 = result + (resultSize - z0Size);
    doFullKaratsubaMultiplicationForSameSizes(lhsLow, rhsLow, lowSize, z0, tempBuffer2);

    const std::size_t z2Size = highSize * 2;
    std::uint64_t* z2 = result;
    doFullKaratsubaMultiplicationForSameSizes(lhs, rhs, highSize, z2, tempBuffer2);

    doSubtraction(z1, z1Size, z2, z2Size);
    doSubtraction(z1, z1Size, z0, z0Size);
    doAddition(result, resultSize - lowSize, z1, z1Size);

#ifdef WMPINT_DEBUG_MODE
    if(tempBuffer2 > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer2;
#endif
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
        return WMPIntImplementations::doFullLongMultiplication(rhs, rhsSize, lhs, lhsSize, result);

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
    std::uint64_t* tempBuffer2 = tempBuffer + z1Size;

    if(lhsSize <= rhsHighSize)
        ::doFullKaratsubaMultiplication(lhs, lhsSize, rhs, rhsHighSize, z1, tempBuffer2);
    else
        ::doFullKaratsubaMultiplication(rhs, rhsHighSize, lhs, lhsSize, z1, tempBuffer2);

    doAddition(result, z1, z1Size);

#ifdef WMPINT_DEBUG_MODE
    if(tempBuffer2 > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer2;
#endif
}

/* This must only be called when lhsSize > (rhsSize+1)/2 and lhsSize < rhsSize */
static void doFullKaratsubaMultiplicationForLargeLHS
(const std::uint64_t* lhs, std::size_t lhsSize,
 const std::uint64_t* rhs, std::size_t rhsSize,
 std::uint64_t* result, std::uint64_t* tempBuffer)
{
    if(rhsSize <= 32)
        return WMPIntImplementations::doFullLongMultiplication(rhs, rhsSize, lhs, lhsSize, result);

    const std::size_t resultSize = lhsSize + rhsSize;
    const std::size_t lowSize = (rhsSize+1) / 2;
    const std::size_t rhsHighSize = rhsSize - lowSize;
    const std::size_t lhsHighSize = lhsSize - lowSize;
    const std::uint64_t* rhsLow = rhs + rhsHighSize;
    const std::uint64_t* lhsLow = lhs + lhsHighSize;

    const std::size_t highPlusLowSize = lowSize + 1;
    std::uint64_t* lhsHighPlusLow = result;
    doFullAddition(lhs, lhsHighSize, lhsLow, lowSize, lhsHighPlusLow);

    std::uint64_t* rhsHighPlusLow = lhsHighPlusLow + highPlusLowSize;
    doFullAddition(rhs, rhsHighSize, rhsLow, lowSize, rhsHighPlusLow);

    const std::size_t z1Size = highPlusLowSize * 2;
    std::uint64_t* z1 = tempBuffer;
    std::uint64_t* tempBuffer2 = tempBuffer + z1Size;

    doFullKaratsubaMultiplicationForSameSizes
        (lhsHighPlusLow, rhsHighPlusLow, highPlusLowSize, z1, tempBuffer2);

    const std::size_t z0Size = lowSize * 2;
    std::uint64_t* z0 = result + (resultSize - z0Size);
    doFullKaratsubaMultiplicationForSameSizes(lhsLow, rhsLow, lowSize, z0, tempBuffer2);

    const std::size_t z2Size = lhsHighSize + rhsHighSize;
    std::uint64_t* z2 = result;
    ::doFullKaratsubaMultiplication(lhs, lhsHighSize, rhs, rhsHighSize, z2, tempBuffer2);

    doSubtraction(z1, z1Size, z2, z2Size);
    doSubtraction(z1, z1Size, z0, z0Size);
    doAddition(result, resultSize - lowSize, z1, z1Size);

#ifdef WMPINT_DEBUG_MODE
    if(tempBuffer2 > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer2;
#endif
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
        return WMPIntImplementations::doLongMultiplication(size, lhs, rhs, result);

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

#ifdef WMPINT_DEBUG_MODE
    if(tempBuffer + lhsHighSize > gWMPInt_tempBuffer_max_position)
        gWMPInt_tempBuffer_max_position = tempBuffer + lhsHighSize;
#endif
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

#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_ARM64
static char* print_uint32_full_length(std::uint32_t value, char* destination)
{
    for(unsigned i = 0; i < 9; ++i)
    {
        const std::uint32_t remainder = value % 10;
        value /= 10;
        *destination-- = static_cast<char>(remainder) + '0';
    }
    return destination;
}

static char* print_uint32(std::uint32_t value, char* destination)
{
    while(value)
    {
        const std::uint32_t remainder = value % 10;
        value /= 10;
        *destination-- = static_cast<char>(remainder) + '0';
    }
    return destination;
}

std::uint32_t WMPIntImplementations::divide32(std::uint64_t* value, std::size_t valueSize, std::uint32_t rhs)
{
    std::uint64_t dividend = 0;
    for(std::size_t index = 0; index < valueSize; ++index)
    {
        const std::uint64_t digit64 = value[index];
        dividend = (dividend << 32) | (digit64 >> 32);
        std::uint64_t result = (dividend / rhs) << 32;
        dividend %= rhs;
        dividend = (dividend << 32) | (digit64 & 0xFFFFFFFF);
        result |= dividend / rhs;
        dividend %= rhs;
        value[index] = result;
    }
    return static_cast<std::uint32_t>(dividend);
}
#endif

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

#if WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_X86_64
    std::size_t remainingValueSize = valueSize - leftIndex;
    std::uint64_t rhs = UINT64_C(10000000000000000000);

    while(true)
    {
        std::size_t counter = remainingValueSize;
        std::uint64_t* lhs = value + leftIndex;

        std::uint64_t remainder;
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

#elif WMPINT_CPU_TYPE == WMPINT_CPU_TYPE_ARM64

    std::size_t remainingValueSize = valueSize - leftIndex;
    const std::uint32_t rhs = UINT32_C(1000000000);

    while(true)
    {
        std::size_t counter = remainingValueSize;
        std::uint64_t* lhs = value + leftIndex;
        std::uint32_t remainder = WMPIntImplementations::divide32(lhs, counter, rhs);

        if(!value[leftIndex])
        {
            ++leftIndex;
            if(--remainingValueSize == 0)
                return print_uint32(remainder, destination) + 1;
        }

        destination = print_uint32_full_length(remainder, destination);
    }
#endif
}
