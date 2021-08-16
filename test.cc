#include "Bit.hh"

#include <iostream>
#include <cassert>
#include <cstdint>

#define SIZE 100

void test_memory_leak(){
    for(size_t i = 0; i < 100; ++i){
        bitptr_t bit_ptr = new bit_alloc[1024*1024*8 - 1]; // Allocate 1 MB of memory (almost)
        std::cout << i << "\n";
        for(size_t j = 0; j < 1024*1024*8 - 1;++j)
            bit_ptr[j] = j%(i + 1) == 0;
        delete[] bit_ptr;
    }

    for(size_t i = 0; i < 100; ++i){
        std::cout << i << "\n";
        for(size_t j = 0; j < 1024*1024*8;++j){
            bitptr_t bit_ptr = new bit_alloc;
            *bit_ptr = j%(i + 1) == 0;
            delete bit_ptr;
        }
    }
}

int main(){
    bitptr_t bit_ptr = new bit_alloc[SIZE];
    for(size_t i = 0; i < SIZE; ++i){
        bit_ptr[i] = (i%3) == 0;
    }
    for(size_t i = 0; i < SIZE; ++i){
        std::cout << bit_ptr[i] << " ";
    }
    std::cout << "\n" << +*(bit_ptr.get_base_ptr()) << "\n";
    delete[] bit_ptr;
    uint8_t one_byte; // stack allocation for feature test
    bit_ptr = &one_byte;
    bitptr_t cache = bit_ptr++;
    std::cout << (void*) bit_ptr.get_base_ptr() << " " << +(bit_ptr.get_offset_ptr()) << "\n";
    std::cout << (void*) cache.get_base_ptr() << " " << +(cache.get_offset_ptr()) << "\n";
    std::cout << (void*) (bit_ptr - 1).get_base_ptr() << " " << +((bit_ptr - 1).get_offset_ptr()) << "\n";
    assert(bit_ptr - 1 == cache);
    assert(bit_ptr > cache);

    bit_array<48> b_array{};
    b_array[8] = true;

    test_memory_leak(); // Has to be monitored manually.

    int i;
    std::cin>>i; // So that even if test_memory_leak wasn't monitored, the memory usage can be checked afterwards
}