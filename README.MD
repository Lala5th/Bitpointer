# Bit.hh

A header only library for manipulating individual bits, through a pointer like class `bitptr_t`. Requires `C++11`, however has some internal macros for checking and enabling some `C++20` features (`[[nodiscard]]` and concepts) if detected supported.

## Usage

`bitptr_t` can be treated as `bit*` a pointer to a single bit. `bit_t` is functionally equivalent to `bit&` a reference to a bit. Generally `bit_t` is less useful than `bitptr_t` and is mostly provided for internal use of the library. Storing it is inefficient as to access other bits it has to be dereferenced to a `bitptr_t`.

To use it `bitptr_t` can be assinged to the first bit of a memory address:
```cpp
uint8_t a;
bitptr_t bit_ptr(&a);
```
or to a specific bit at an address
```cpp
uint8_t a;
bitptr_t bit_ptr(&a,3);
```
or using the helper struct `bit_alloc` one can create dynamic arrays of bits:
```cpp
bitptr_t bit_ptr = new bit_alloc[size];
```
While it is not enforced choosing a `size` which is not a multiple of 8 will allocate memory for the smallest amount of bytes still containing `size` bits. Similarly `new bit_alloc` is evaluated but will be equivalent to `new bit_alloc[8]`.

A class is also provided to help in allocating bit arrays named `bit_array<SIZE_IN_BITS>`.

## Utility

Due to inherent overhead in `bitptr_t`, there are more memory eficient ways to do bitwise manipulation on small scale.  This library is meant for manipulating a large array of boolean like values.