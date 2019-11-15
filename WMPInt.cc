#include "WMPInt.hh"
#include <cstdio>

//----------------------------------------------------------------------------
// Multiplication
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
         "adcq %%rdx,(%[tempBuf],%[rhsIndCounter],8)\n\t" // tmpBuf[rhsIndCounter] = rdx
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
