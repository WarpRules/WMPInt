Time and again I asked the question, especially when investigating the algorithms needed for this
library: "Why don't you just use GMP?"

Rather obviously this is an interesting hobby. I'm interested in low-level algorithms and inline asm,
and this kind of project is perfect for learning such things, and I have indeed learned quite many
things during this project. A lot more than if I had "just used GMP".

But besides my personal interests, I think that there's actually merit to this kind of library.
I think there's an actual gap in the "market" of multi-precision libraries that would benefit
from being filled. Let me explain:

I have searched for many multi-precision libraries out there, and the vast majority of them can be
roughtly categorized into two types: Firstly, the absolutely massive behemoths that support absolutely
everything and do an enormous amount of things and consist of hundreds if not thousands of source files
(often being "kept hostage" by a unix-style configure script). Secondly, the small "quick&dirty" often
haphazardly made tiny libraries implemented with a principle and mentality of "as long as it works", and
"this isn't even supposed to be efficient, if you want something efficient, use GMP."

The behemoths, like GMP and similar libraries, are nice, and often quite efficient (especially for very
large numbers) and have a lot of functionality and features, but they are large and heavy, and can be
difficult to use in restricted environments, and can be a pain to compile for more exotic targets.
Small open source projects using these libraries will carry the burden of depending on them. The behemoths
can also be relatively inefficient with smallish numbers (ie. slower than an optimal implementation could
be), due to how they are designed (usually there's no inlining, and every value is dynamically allocated).

The small "quick&dirty" libraries have, rather obviously, the problem of being inefficient. Many of them
don't even implement all the operators and operations doable to a basic integer type.

Thus, I think there's a gap in the "market" of multi-precision arithmetic libraries: A library that's
relatively small and lightweight, and easy to use and integrate into a project, while still being very
competitive in terms of speed.

`WMPInt` might not be the absolutely _perfect_ library for this, but it's seriously aiming to fill that gap.

In terms of efficiency and speed, while it doesn't have anything to be ashamed of when it comes to extremely
large numbers (eg. in the millions of bits size ranges), it ought to really shine when it comes to relatively
small numbers, especially in the 128-256 bit range, but also beyond.

Indeed, in many cases the resulting binary should be pretty much as efficient as a compiler would have
generated, if it supported arbitrarily-large integers. Consider, for example, this piece of code:

```c++
#include "WMPInt/WMPInt.hh"

// 256-bit unsigned integer
using Value_t = WMPUInt<4>;

void sum(Value_t& a, const Value_t& b)
{
    a += b;
}
```

gcc produces the following asm output from that code:

```
_Z3sumR7WMPUIntILm4EERKS0_:
.LFB182:
        .cfi_startproc
        movq 24(%rsi), %rax
        addq %rax, 24(%rdi)
        movq 16(%rsi), %rax
        adcq %rax, 16(%rdi)
        movq 8(%rsi), %rax
        adcq %rax, 8(%rdi)
        movq (%rsi), %rax
        adcq %rax, (%rdi)
        ret
        .cfi_endproc
```

It's unlikely that even the compiler itself would have produced something more efficient.
And if we try with something larger, like `WMPUInt<16>`:

```
_Z3sumR7WMPUIntILm16EERKS0_:
.LFB182:
        .cfi_startproc
        movl    $13, %eax
        movq 16(%rsi,%rax,8), %rdx
        addq %rdx, 16(%rdi,%rax,8)
        movq 8(%rsi,%rax,8), %rdx
        adcq %rdx, 8(%rdi,%rax,8)
loop8:
        movq (%rsi,%rax,8), %rdx
        adcq %rdx, (%rdi,%rax,8)
        movq -8(%rsi,%rax,8), %rdx
        adcq %rdx, -8(%rdi,%rax,8)
        decq %rax
        decq %rax
        jns loop8
        ret
```
