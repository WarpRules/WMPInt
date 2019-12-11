# Unsigned long multiplication of bignum values

"Long multiplication" is the traditional multiplication method used in grade school to multiply
two numbers with pen and paper. While pen-and-paper long multiplication is done in base-10,
one digit at a time, in bignum arithmetic each "digit" is usually the size of the natural word
size of the CPU, for example 64-bit values.

While long multiplication is not the fastest algorithm to multiply very large numbers, it is
the fastest for smaller bignum numbers. Thus it's useful to implement it not only for these
smaller values, but also to speed up faster algorithms like the Karatsuba multiplication
(which may call the long multiplication implementation for small-enough segments).

Two modes of multiplication are useful in bignum arithmetic: Full multiplication and
truncated multiplication:

* Full multiplication calculates the entire result of multiplying two input values. The size
of the result (in words) will be the sum of the sizes (in words) of the input values.
* Truncated multiplication is used for `operator*()` (ie. when multiplying two bignum values
with an expression like `a*b`): It takes two input values of the same size, and the result
is truncated to that size (in other words, the upper half of the result is discarded).

Compared to the Karatsuba algorithm, full multiplication using the long multiplication
algorithm is faster for values of sizes up to about 12-15 words, and truncated multiplication
is faster for values of sizes up to about 25-30 words.

The algorithm descriptions below assume that the bignum values are in arrays of unsigned
integer values such that the first value of the array is the most-significant element and
the last value in the array is the least-significant element. (The algorithms work the
same if the values are the other way around, just traversing the arrays in the other
direction.)

## Short multiplication

Short multiplication is a simpler form of multiplication where the multiplier is the size of
one single digit (or, in bignum arithmetic, the size of one word). It's simpler and more
efficient to implement, and has the advantage of not requiring any additional buffer array
for temporary calculations (even if the multiplicand is overwritten with the result). Also,
long multiplication is essentially just a series of short multiplications.

### Algorithm

Given a large multiplicand named `lhs`, and a multiplier of one word, named `rhs`, and a target
array, `result`, the basic short multiplication algorithm goes as follows:

1. Zero the `result` array (if it isn't already).
2. `index` = the size of `lhs` minus 1.
3. Multiply `lhs[index]` with `rhs` into a double-word sized result `AB`.
4. Add `B` to `result[index+1]`.
5. Add-with-carry `A` to `result[index]`.
6. Decrement `index` and if it's still inside `lhs`, jump to step 3.

Note: If the first step above is omitted, the algorithm becomes a "fused multiply-and-add"
function, where the value to add should be in the `result` array, essentially for free.

The truncated version of the multiplication simply puts the result at the same index positions
as the multiplicand values, and omits writing the last most-significant word. (This can be done
by ending the loop when `index` becomes zero, and doing the last multiplication separately and
adding only `B` to `result[0]`.)

### gcc extended asm implementation

Using gcc extended asm, the algorithm can be implemented for example like this:

```c++
inline void shortMultiplication
(const uint64_t* lhs, size_t lhsSize, uint64_t rhs, uint64_t* result)
{
    for(size_t i = 0; i <= lhsSize; ++i) result[i] = 0;
    std::uint64_t lhsInd = lhsSize - 1;
    asm("loop%=:\n\t"
        "movq (%[lhs], %[lhsInd],8), %%rax\n\t"
        "mulq %[rhs]\n\t"
        "addq %%rax, 8(%[result], %[lhsInd],8)\n\t"
        "adcq %%rdx, (%[result], %[lhsInd],8)\n\t"
        "decq %[lhsInd]\n\t"
        "jns loop%="
        : "+m"(*(uint64_t(*)[lhsSize+1])result), [lhsInd]"+&r"(lhsInd)
        : "m"(*(uint64_t(*)[lhsSize])lhs), [lhs]"r"(lhs),
          [rhs]"r"(rhs), [result]"r"(result)
        : "rax", "rdx", "cc");
}
```

### In-place short multiplication

If the multiplicand (`lhs`) ought to be replaced with the result of the short multiplication
(eg. for the implementation of `operator*=(uint64_t)`), it's still possible without the use
of a temporary buffer, but the algorithm needs to be slightly modified:

1. `index` = the size of `lhs` minus 1
2. `tempValue` = `lhs[index]`
3. `lhs[index]` = 0
4. `A` = `tempValue`
5. `tempValue` = `lhs[index-1]`
6. Multiply `A` with `rhs` into a double-word sized result `AB`.
7. Add `B` to `lhs[index]`.
8. Add the carry to `A` (eg. with add-with-carry a zero to it).
9. `lhs[index-1]` = `A`
10. Decrement `index` and if it's larger than zero, jump to step 4.
11. Multiply `tempValue` with `rhs` and add the lowest word of the result to `lhs[0]`.

## Long multiplication (full)

(To be written).

## Long multiplication (truncated)

(To be written).
