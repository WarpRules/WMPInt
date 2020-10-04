# WMPInt
WMPInt v0.7.0 is a small library for multi-precision integer ("bignum") calculations for x86-64
and ARM64 platforms, in C++17 and inline asm, consisting of one header and one source file.

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

Note that these classes use gcc-style x86-64 and aarch64 inline asm, which means support is restricted
to such targets, using compatible compilers (modern gcc and clang tested, icc may work).

Consult the `WMPInt.html` file for full reference documentation and usage tips.

## Benchmarks

### x86-64

These benchmarks were run on an i7-9700K, single-threaded, using gcc 10.2.1 with compiler
flags `-Ofast -march=native`.

Addition:

| WMPUInt size  | Bits    | Time   | Additions/s          |
|:--------------|:--------|-------:|---------------------:|
| 8             | 512     | 3.1 ns | 3.2 ⋅ 10<sup>8</sup> |
| 32            | 2048    |  11 ns | 9.1 ⋅ 10<sup>7</sup> |
| 150           | 9600    |  55 ns | 1.8 ⋅ 10<sup>7</sup> |
| 256           | 16384   |  88 ns | 1.1 ⋅ 10<sup>7</sup> |
| 1024          | 65536   | 443 ns | 2.3 ⋅ 10<sup>6</sup> |
| 16386         | 1048576 | 6.1 μs | 1.6 ⋅ 10<sup>5</sup> |
| 65536         | 4194304 |  27 μs | 3.7 ⋅ 10<sup>4</sup> |

Multiplication (truncated, ie. result size is truncated to input size):

| WMPUInt size  | Bits    | Time    | Multiplications/s    |
|:--------------|:--------|--------:|---------------------:|
| 8             | 512     |   31 ns | 3.2 ⋅ 10<sup>7</sup> |
| 32            | 2048    |  471 ns | 2.1 ⋅ 10<sup>6</sup> |
| 150           | 9600    |  8.8 μs | 1.1 ⋅ 10<sup>5</sup> |
| 256           | 16384   |   23 μs |                43285 |
| 1024          | 65536   |  235 μs |                 4259 |
| 16386         | 1048576 | 20.7 ms |                 48.2 |
| 65536         | 4194304 |  189 ms |                  5.3 |

Full multiplication (full double-sized result):

| WMPUInt size  | Bits    | Time    | Multiplications/s    |
|:--------------|:--------|--------:|---------------------:|
| 8             | 512     |   62 ns | 1.6 ⋅ 10<sup>7</sup> |
| 32            | 2048    |  914 ns | 1.1 ⋅ 10<sup>6</sup> |
| 150           | 9600    | 11.1 μs |                90271 |
| 256           | 16384   |   27 μs |                36399 |
| 1024          | 65536   |  251 μs |                 3984 |
| 16386         | 1048576 | 20.7 ms |                 48.4 |
| 65536         | 4194304 |  187 ms |                  5.3 |

### ARM64

This benchmark was run on a Raspberry Pi 4 model B (Cortex-A72 (ARM v8) 64-bit SoC @ 1.5GHz),
using gcc 10.2.1 with compiler flags `-Ofast -march=native`.

Full multiplication (full double-sized result):

| WMPUInt size  | Bits    | Time    | Multiplications/s    |
|:--------------|:--------|--------:|---------------------:|
| 8             | 512     |  331 ns | 3.0 ⋅ 10<sup>6</sup> |
| 32            | 2048    |  4.9 μs | 2.1 ⋅ 10<sup>5</sup> |
| 150           | 9600    |   56 μs |                17939 |
| 256           | 16384   |  138 μs |                 7268 |
| 1024          | 65536   |  1.3 ms |                  796 |
| 16386         | 1048576 |  103 ms |                 9.66 |
| 65536         | 4194304 |  937 ms |                 1.07 |
