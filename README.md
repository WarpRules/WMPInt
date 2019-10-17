# WMPInt
WMPInt v0.1.0 is a small header-only library for multi-precision integer ("bignum")
calculations for x86-64 platforms, in C++17 and inline asm.

The `WMPInt` (not yet implemented) and `WMPUInt` classes are template classes with a compile-time
size template parameter, ie. similar to `std::array<std::uint64_t, size>`. (The data is stored
as an in-built member array; no dynamic memory allocations are performed.) These classes are
intended for situations where the size of the numbers can be determined at compile time.

Sizes of the integers are defined in multiples of 64 bits. Example of using 1024-bit
unsigned integers:

```c++
WMPUInt<16> a(10), b(20), c(30); // 1024-bit integers
WMPUInt<16> result = a + b * c;
```

Note that these classes use gcc-style x86-64 inline asm, which means support is restricted
to such targets, using compatible compilers (modern gcc and clang tested, icc may work).

Consult the `WMPInt.html` file for full reference documentation and usage tips.
