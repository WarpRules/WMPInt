# Unsigned Karatsuba multiplication of bignum values

The traditional long multiplication algorithm performs `n*m` individual (one-word) multiplications
and additions, where `n` and `m` are the sizes (in words) of the multiplicand and the multiplier
(n<sup>2</sup> multiplications if they are of the same size).

However, this is not the fastest possible algorithm for larger input values. The so-called
_Karatsuba algorithm_ (first discovered by Anatoly Karatsuba in 1960) is the first, and perhaps
simplest, multiplication algorithm for large numbers that's faster than O(n<sup>2</sup>).
The algorithm is approximately O(n<sup>1.58</sup>)-time.

Due to implementation overhead it only becomes faster than straightforward long multiplication
for input values larger (in words) than a certain threshold, which is, depending on hardware,
somewhere around 10-15 words (ie. 640-960 bits, if each word is 64 bits).

Because of how the algorithm works, it's advantageous to also implement long multiplication
to speed up the Karatsuba algorithm. As the algorithm partitions the numbers into smaller and
smaller parts, when the abovementioned size threshold is reached, it's faster to use long
multiplication for these smaller parts. (This is quite similar to how the quicksort algorithm
can be sped up by using straightforward insertion sort for small-enough partitions.)

## Introduction to the algorithm

Given two large input numbers (essentially as arrays of eg. 64-bit unsigned values), they can be
split into two parts, and these parts multiplied together and added to the result (similar to how
long multiplication works). As an example, let's assume the input values are 6 words long.
Let's denote their words as `AAABBB` and `CCCDDD`. Their product could thus be calculated
like this:

```
        AAABBB
      * CCCDDD
  ------------
        EEEEEE = DDD*BBB
+    FFFFFF    = DDD*AAA
+    GGGGGG    = CCC*BBB
+ HHHHHH       = CCC*AAA
  ------------
  RRRRRRRRRRRR = result
```

Note that each of those smaller multiplications can be done by calling this routine recursively
(up until the input values are small enough to be multiplied directly, or by using regular long
multiplication.)

However, this does not save anything. This still performs exactly n<sup>2</sup> multiplications
and additions in total (and is in fact slower than straigforward long multiplication due to the
extra implementation overhead, and the recursion.)

However, what Karatsuba discovered is that with a bit of math, one of those sub-multiplications
(and therefore one of the recursive calls) can be omitted, by using a couple of additions and
subtractions instead (which can be calculated in-situ, not requiring any recursive calls). Since
one of the recursive calls is omitted, this reduces the total amount of operations required.
The larger the input values, the more significant this saving becomes.

## The algorithm

The Karatsuba algorithm replaces the two middle terms in the above description (ie. `DDD*AAA`
and `CCC*BBB`) with one single multiplication, two additions and two subtractions, like this:

```
        AAABBB
      * CCCDDD
  ------------
        EEEEEE = z0 = DDD*BBB
+    FFFFFF    = z1 = (AAA+BBB)*(CCC+DDD) - z2 - z0
+ HHHHHH       = z2 = CCC*AAA
  ------------
  RRRRRRRRRRRR = result
```

Note that `z0` and `z2` can be calculated first, and then used to calculate `z1`.

Also note that the two additions, `AAA+BBB` and `CCC+DDD` result in a value that's one
word larger than the sizes of the summands (in this example the result would be 4 words
large), and thus likewise the multiplication needs to be done at that size.

The original four recursive calls to this routine are thus reduced to three, which is where
the savings in the total amount of operations comes from.

Note that this algorithm requires a separate temporary buffer to hold the values required
to calculate `z1`. The values of `z0` and `z2` can be calculated directly into the result
(thus reducing the required size of the temporary buffer) by noting that they do not overlap
in the result, and thus the multiplication above can be calculated like this:

```
        AAABBB
      * CCCDDD
  ------------
  HHHHHHEEEEEE = (z2 = CCC*AAA | z0 = DDD*BBB)
+ 000FFFFFF    = z1 = (AAA+BBB)*(CCC+DDD) - z2 - z0
  ------------
  RRRRRRRRRRRR = result
```

This significantly reduces the size requirement for the temporary buffer, but requires
an extra round of add-with-carry of zeros when adding `z1` to the result (which is
indicated here by the zeros in the result of `z1`). Note, however, that the add-with-carry
of zeros can be stopped immediately when there is no carry left. In practice this does not
slow down the algorithm almost at all.

Also note that the split of the input numbers doesn't have to be exactly in the middle of them.
The algorithm works the same regardless of where the numbers are split. However, splitting in
the middle (or as close to the middle as possible) results in the least amount of operations
and thus optimal performance. But this is the reason why the algorithm works the same even if
the size (in words) of the input numbers is odd, rather than even. In this case either the
higher part will be one word smaller than the lower, or the other way around, depending on
where you decide to split them. (Note that if the original input values are of the same size,
at no point is multiplying values of different sizes required, even when they have to be split
off-middle due to the size being odd. All three multiplications will be of values of the same
size regardless.)

## The math

Why does this work?

Given two numbers `x` and `y`, they can be divided into parts `x1`, `x0`, `y1` and `y0` such that:

```
x = x1*2^m + x0
y = y1*2^m + y0
```

for some arbitrarily chosen value `m` (which here in practice indicates the split point in number
of bits). From this it follows that:

```
x*y = (x1*2^m + x0) * (y1*2^m + y0)
    = z2*2^(2*m) + z1*2^m + z0
```

where:

```
z2 = x1*y1
z1 = x1*y0 + x0*y1
z0 = x0*y0
```

Karatsuba's observation was that `z1` can also be expressed in terms of `z0` and `z2` like this:

```
z1 = (x1+x0)*(y1+y0) - z2 - z0
```

because `x1*y0 + x0*y1 = (x1+x0)*(y1+y0) - z2 - z0`:

```
(x1+x0)*(y1+y0) - z2 - z0 =
(x1+x0)*(y1+y0) - x1*y1 - x0*y0 =
x1*y1 + x1*y0 + x0*y1 + x0*y0 - x1*y1 - x0*y0 =
x1*y0 + x0*y1
```

## Algorithm pseudocode for inputs of the same size

This algorithm description assumes that the values are stored in arrays of unsigned integers
(eg. 64-bit) such that the most-significant term is at the beginning of the array and the
least-significant term is at the end of the array.

Given two large numbers of the same size (in words), `lhs` and `rhs`, a target `result` array
(of size twice that of the input values), and a temporary buffer:

1. If `size` is less than a given threshold (like eg. 12), call the long multiplication implementation and return.
2. `lowSize` = size divided by 2, rounded up. `highSize` = `size - lowSize`.
3. Split the input values into two parts, `lhsLow` and `rhsLow` of size `lowSize`, and `lhsHigh` and `rhsHigh` of size `highSize`. (In C and C++ this can be done by simply having pointers point to the corresponding parts in the input arrays. In non-pointer-based languages this can be done with index values. There is no need to copy these values anywhere. The "split" is done only at the conceptual level.)
4. Call this routine recursively with `lhsLow` and `rhsLow`, putting the result into the `result` array at a position starting from `size*2-lowSize*2`. Let's call this result `z0`. Its size will be `lowSize*2`.
5. Call this routine recursively with `lhsHigh` and `rhsHigh`, putting the result into the `result` array, at its beginning. Let's call this result `z2`. Its size will be `highSize*2`.
6. Calculate `lhsLow+lhsHigh` into the temporary buffer. Let's call it `lhsSum`. The size of this value is `sumSize` = `lowSize+1`.
7. Calculate `rhsLow+rhsHigh` into the temporary buffer, after `lhsSum` (ie. at position `sumSize`). Let's call it `rhsSum`.
8. Call this routine recursively with `lhsSum` and `rhsSum`, putting the result into the temporary buffer after both values. Let's call this result `z1`. Its size will be `sumSize*2`. The temporary buffer given to the recursive call can point to position `sumSize*4` in the buffer.
9. Subtract `z2` from `z1` (in-place, ie. putting the result in `z1`).
10. Subtract `z0` from `z1` (in-place, ie. putting the result in `z1`).
11. Add `z1` to the `result`, at position `size*2-lowSize-sumSize*2`.

## Multiplying numbers of different sizes

If the two input values are of different sizes (in words), this poses an additional challenge
in implementing the algorithm. The above algorithm can only be used, as-is, for numbers of the
same size. Numbers of different sizes need to be handled specially.

While one approach would be to expand the smaller number to the same size as the larger one,
by appending zeros at the front, this would not be optimal, as more operations (and more recursive
calls) would be performed than necessary. The larger the difference in sizes, the more sub-optimal
it would be (essentially, there would be a lot of useless multiplications by zero).

The most optimal implementation will always split the larger number from the middle (or as close
to the middle as possible, when the size is odd). Thus we need three different implementations
of the algorithm, depending on the size of the smaller number:

1. An implementation for the case that both sizes are the same (given above.)
2. An implementation for when the smaller number is half the size of the larger number or smaller.
3. An implementation for when the smaller number is larger then half the size of the larger number.

The two latter implementations will call recursively any of the three as needed. (The first one
only needs to call itself recursively, as seen earlier.) This is easiest to implement by having
one general multiplication function that calls one of the three specialized functions depending
on the sizes of the inputs. The two latter specialized functions above would then call this
generic function.

### Algorithm for when the smaller number is less than or equal to half the size of the larger

This case is actually very simple to implement, as it doesn't actually require using the
Karatsuba method at all. (It will _call_ any of the three versions of the function above,
but this itself does not need to use the Karatsuba algorithm.) This can be implemented
essentially as a "short multiplication", using the smaller number as the multiplier.

Suppose we have two input values, for example `AAAABBBB` and `CCC`. this version of the
algorithm can be calculated like this:

```
     AAAABBBB
        * CCC
  -----------
      DDDDDDD = CCC*BBBB
+ EEEEEEE     = CCC*AAAA
  -----------
  RRRRRRRRRRR = result
```

The two sub-multiplications can be done by calling the generic multiplication function
recursively.

### Algorithm for when the smaller number is larger than half the size of the larger

This version is, in fact, almost identical to the version with same sizes, with the only
difference being that we need to calculate and keep track of the `highSize` separately for
the two input values (as they will be different in this respect), and we need to call the
generic multiplication function (rather than this function itself) when the two values to
multiply are of different sizes. Other than that, the same algorithm can be closely followed.

## Calculating the required size of the temporary buffer

The algorithm requires a buffer to store temporary calculations. One single buffer suffices
for the entire algorithm, regardless of its recursive nature (recursive calls can simply pass
a pointer or index to the same temporary buffer, pointing to the end of whatever data is stored
by the current function.)

Calculating the total required size of this temporary buffer, however, is not trivial. There might
not be a simple formula to calculate this size with precision (especially since the algorithm
is optimized by jumping to the long multiplication algorithm for segments smaller than a given
threshold).

The exact size can be calculated though, but it has to be done by essentially replicating the
entire algorithm (all three functions above, plus the long multiplication), but simply calculating
used temp buffer size instead of anything else (which is what the `WMPInt` library does.) In C++17,
if the sizes of the input values are known at compile time, this can be done with a series of
`constexpr` functions, allowing also this size to be known at compile time. (In other languages,
or if the sizes are not known at compile time, it will probably need to be calculated at runtime,
and the temporary buffer likewise be allocated at runtime.)

## Truncated Karatsuba multiplication

Truncated multiplication is a version of multiplication where the input values are of the same
size and the result is truncated to that same size (in other words, the entire upper half of
the result is discarded). This is useful to implement `operator*` and `operator*=`.

One way to do this would be to calculate the full-sized multiplication, and simply assign the
lower half of it to the result. However, this wouldn't be optimal, as twice as many operations
would be done than necessary (and the result of those extra operations simply discarded).
Truncated multiplication requires only half the operations than full multiplication.

The truncated multiplication version of the algorithm doesn't itself use the Karatsuba method,
but it calls the Karatsuba multiplication function (which is where the efficiency comes from,
compared to truncated long multiplication).

Assume the two input values are `AAABBB` and `CCCDDD`. Truncated multiplication can be calculated
like this:

```
  AAABBB
* CCCDDD
  ------
  EEEEEE = DDD*BBB
+ FFF    = DDD*AAA (truncated)
+ GGG    = CCC*BBB (truncated)
  ------------
  RRRRRR = result
```

The first sub-multiplication (`DDD*BBB`) is a full multiplication and can be done by calling
the Karatsuba multiplication function. The other two multiplications are done by calling this
same routine recursively.

### Truncated Karatsuba multiplication of odd-sized numbers

A slight complication happens if the size (in words) of the input values is not even. In that
case they would need to be split off-middle. This would not pose a problem with the first (full)
sub-multiplication above, but the other two multiplications would need to multiply two numbers
of different sizes (differing in size by one).

However, this problem actually becomes non-existent when we realize that the two input numbers
don't actually have to be split the same way: Split one of the input numbers at half of its
size _rounded down,_ and the other at half of its size _rounded up._

Assume the two input values are `AAAABBB` and `CCCDDDD`. The multiplication can be calculated
like this:

```
  AAAABBB
* CCCDDDD
  -------
  EEEEEEE = DDDD*BBB
+ FFFF    = DDDD*AAAA (truncated)
+ GGG     = CCC*BBB (truncated)
  -------
  RRRRRRR = result
```

The first multiplication will now be of different sizes, but this is ok, because we have already
implemented a full Karatsuba multiplication for different sizes. The other two multiplications
are now of values of the same size, so we can just call this same routine recursively for them.

Note that we don't actually need two different implementations for these two cases. The one
and same implementation will work for both (because when the size of the input numbers is
even, rounding the split point up or down doesn't matter, they are still split from the same
point in the middle.)

### Truncated Karatsuba multiplication algorithm

Given two large numbers of the same size (in words), `lhs` and `rhs`, a target `result` array
(of size twice that of the input values), and a temporary buffer:

1. If `size` is less than a given threshold (like eg. 30), call the long multiplication implementation and return.
2. `lhsLowSize` = `size / 2`, rounded down. `lhsHighSize` = `size - lhsLowSize`
3. `rhsLowSize` = `size / 2`, rounded up. `rhsHighSize` = `size - rhsLowSize`
4. Split the numbers into `lhsHigh`, `lhsLow`, `rhsHigh` and `rhsLow` according to those sizes. (This can be done with just pointers or indices. There is no need to copy the values elsewhere.)
5. Call the Karatsuba full multiplication for `lhsLow` and `rhsLow`, putting the result directly into the `result` array.
6. Call this routine recursively with `lhsHigh` and `rhsLow`, putting the result into the temporary buffer.
7. Add the value in the temporary buffer to (the beginning of) `result`.
8. Call this routine recursively with `lhsLow` and `rhsHigh`, putting the result into the temporary buffer.
9. Add the value in the temporary buffer to (the beginning of) `result`.
