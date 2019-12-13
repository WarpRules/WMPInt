# Unsigned Karatsuba multiplication of bignum values

The traditional long multiplication algorithm performs `n*m` individual (one-word) multiplications
and additions, where `n` and `m` are the sizes (in words) of the multiplicand and the multiplier
(n<sup>2</sup> if they are of the same size).

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

## Algorithm pseudocode

(To be written.)

## Multiplying numbers of different sizes

(To be written.)

## Truncated Karatsuba multiplication

(To be written.)
