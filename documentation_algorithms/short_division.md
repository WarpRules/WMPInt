# Unsigned short division of bignum values

So-called "short division" is a simplified version of the so-called "long division" algorithm
for the case where the divisor is only one digit. Or, in the case of bignum arithmetic, when
the divisor is the size of one (64-bit) word. Short division is much simpler than long division,
and can be implemented surprisingly trivially in x86 assembly language.

Note that this algorithm only works for unsigned (ie. non-negative) values.

## Algorithm

Given a large dividend consisting of an array of (in this case 64-bit) words, named `lhs`,
and a divisor of one word, named `rhs`, and a target array, `result`, the basic short division
algorithm goes as follows:

We assume the first element in the `lhs` array (index 0) contains the most-significant word of
the bignum value.

1. `index` = 0
2. `R` = 0
3. `A` = `lhs[index]`
4. Divide the 128-bit value `RA` by `rhs` and put the quotient in `A` and remainder in `R`.
5. `result[index]` = `A`
6. Increment `index` and if it's less than the size of `lhs`, jump to step 3.

After the loop ends, the remainder of the division will be in `R`.

The dividend can be divided in-place (ie. replacing the original value with the result)
by simply having `result` be `lhs`.

## Asm implementation

This algorithm is surprisingly simple to implement in x86 asm, as the `div` instruction
seems to have been specifically designed for this.

The `div` instruction divides the (unsigned) 128-bit value stored in the `rdx:rax` register
pair by its parameter, and stores the quotient in `rax` and the remainder in `rdx`. Which is
perfect because it's exactly where we need the remainder to be for the next iteration.

Let's assume:

* `lhs` (a pointer to the dividend array) is in the register `r8`
* `rhs` (the 64-bit divisor) is in the register `r9`
* `result` (a pointer to the result array) is in the register `r10`
* `index` is in `r11` (initialized to 0)`
* A counter is in `r12`, initialized ot the size of `lhs`.

The implementation of short division can be written as, using Intel syntax:

```
      xor edx, edx            // rdx = 0
loop: mov rax, [r8 + r11]     // rax = lhs[index]
      div r9                  // divide by rhs
      mov [r10 + r11], rax    // result[index] = rax
      lea r11, [r11 + 8]      // index += 1 (in steps of 8)
      dec r12                 // counter -= 1
      jnz loop                // if(counter > 0) goto loop;
```

Using AT&T syntax:

```
      xorl %edx, %edx          // rdx = 0
loop: movq (%r8, %r11), %rax   // rax = lhs[index]
      divq %r9                 // divide by rhs
      movq %rax, (%r10, %r11)  // result[index] = rax
      leaq 8(%r11), %r11       // index += 1 (in steps of 8)
      decq %r12                // counter -= 1
      jnz loop                 // if(counter > 0) goto loop;
```

After the loop ends, the remainder of the division will be in `rdx`.

## gcc extended asm implementation

The above can be implemented in gcc extended asm for example like this:

```c++
/* Divide lhs by rhs, putting quotient in result. Returns remainder. */
inline uint64_t shortDivision
(const uint64_t* lhs, size_t lhsSize, uint64_t rhs, uint64_t* result)
{
    size_t lhsIndex = 0, counter = lhsSize;
    uint64_t remainder;

    asm("xorl %%edx, %%edx\n"
        "loop%=:\n\t"
        "movq (%[lhs], %[lhsIndex]), %%rax\n\t"
        "divq %[rhs]\n\t"
        "movq %%rax, (%[result], %[lhsIndex])\n\t"
        "leaq 8(%[lhsIndex]), %[lhsIndex]\n\t"
        "decq %[counter]\n\t"
        "jnz loop%="
        : "=m"(*(uint64_t(*)[lhsSize])result), "=&d"(remainder),
          [lhsIndex]"+&r"(lhsIndex), [counter]"+&r"(counter)
        : "m"(*(uint64_t(*)[lhsSize])lhs), [lhs]"r"(lhs),
	  [rhs]"r"(rhs), [result]"r"(result)
        : "rax", "cc");

    return remainder;
}
```
