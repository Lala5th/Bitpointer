// Header only bit allocation and manipulation library
// Written by Lajos Palanki<lala5th@gmail.com>
// Published under the MIT license

#ifndef BIT_HH
#define BIT_HH

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <new>
#include <cassert>

#if __has_cpp_attribute(nodiscard)
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

#ifdef __cpp_concepts
#include <concepts>
#define INTEGRAL(T) requires std::integral<T>
#else
#define INTEGRAL(T)
#endif

struct bit_t;
struct bit_alloc;

class bitptr_t { // class for representing a pointer to a single bit.
    private:
        char* base_ptr;
        uint8_t offset_ptr;
    public:
        bitptr_t(){
            base_ptr = nullptr;
            offset_ptr = 0;
        }
        
        bitptr_t(void* ptr_to_type){
            base_ptr = (char*) ptr_to_type;
            offset_ptr = 0;
        }

        bitptr_t(void* ptr_to_type, uint8_t sub_ptr){
            base_ptr = (char*) ptr_to_type;
            assert(sub_ptr < 8);
            offset_ptr = sub_ptr;
        }

        const char* get_base_ptr() const { return this->base_ptr; }
        const uint8_t get_offset_ptr() const { return this->offset_ptr; }

        bitptr_t(const bitptr_t&) = default;
        
        bitptr_t& operator=(const bitptr_t&) = default;
        bitptr_t& operator=(bitptr_t&&) = default;

        bit_t operator*();
        bit_t operator[](size_t);

        bitptr_t operator+(const size_t& rhs){
            bitptr_t added_value(*this);
            added_value.base_ptr += (added_value.offset_ptr + rhs)/8;
            added_value.offset_ptr = (added_value.offset_ptr + rhs)%8;
            return added_value;
        }

        bitptr_t operator-(const size_t& rhs){
            bitptr_t subbed_value(*this);
            subbed_value.base_ptr -= rhs > subbed_value.offset_ptr ? (rhs - subbed_value.offset_ptr)/8 : 0;
            size_t mod_rhs = rhs % 8;
            mod_rhs = mod_rhs == 0 ? 8 : mod_rhs;
            if(rhs > subbed_value.offset_ptr) --subbed_value.base_ptr, subbed_value.offset_ptr += 8;
            subbed_value.offset_ptr = subbed_value.offset_ptr - mod_rhs;
            return subbed_value;
        }

        template <typename T> INTEGRAL(T)
        bitptr_t operator+(const T& rhs){
            if(rhs > 0) return *this + ((size_t) rhs);
            return *this - ((size_t) -rhs);
        }

        template <typename T> INTEGRAL(T)
        bitptr_t operator-(const T& rhs){
            if(rhs > 0) return *this - ((size_t) rhs);
            return *this + ((size_t) -rhs);
        }

        template <typename T> INTEGRAL(T)
        bitptr_t& operator+=(const T& rhs){
            *this = *this + rhs;
            return *this;
        }

        template <typename T> INTEGRAL(T)
        bitptr_t& operator-=(const T& rhs){
            *this = *this - rhs;
            return *this;
        }

        bitptr_t& operator--(){
            *this -= 1;
            return *this;
        }

        bitptr_t& operator++(){
            *this = *this + 1;
            return *this;
        }

        bitptr_t operator--(int){
            bitptr_t cache(*this);
            --*this;
            return cache;
        }

        bitptr_t operator++(int){
            bitptr_t cache(*this);
            ++*this;
            return cache;
        }

        inline bool operator<(const bitptr_t& rhs) const {
            return (this->base_ptr < rhs.get_base_ptr()) || (this->base_ptr == rhs.get_base_ptr() && this->offset_ptr < rhs.get_offset_ptr());
        }

        inline bool operator> (const bitptr_t& rhs) const { return rhs < *this; }
        inline bool operator<=(const bitptr_t& rhs) const { return !(*this > rhs); }
        inline bool operator>=(const bitptr_t& rhs) const { return !(*this < rhs); }
        inline bool operator!=(const bitptr_t& rhs) const { return (*this < rhs) || (*this > rhs); }
        inline bool operator==(const bitptr_t& rhs) const { return !(*this != rhs); }

        // Casts to bool and bit_alloc*
        operator bool(){
            return base_ptr != nullptr;
        }

        operator bit_alloc*(){
            return reinterpret_cast<bit_alloc*>(this->base_ptr);
        }

        friend struct bit_t;

};

struct bit_t{
    bitptr_t pointer_to_bit;
    
    bit_t() : pointer_to_bit() {}

    bit_t(bitptr_t ptr){
        pointer_to_bit = ptr;
    }

    bit_t(const bit_t& other){
        pointer_to_bit = other.pointer_to_bit;
    }

    operator bool(){
        return *(this->pointer_to_bit.base_ptr) & (1 << this->pointer_to_bit.offset_ptr);
    }

    bitptr_t& operator&(){
        return this->pointer_to_bit;
    }

    const bitptr_t& operator&() const{
        return this->pointer_to_bit;
    }

    bit_t& operator=(const bool& val){
        uint8_t mask = 1 << this->pointer_to_bit.offset_ptr;
        *(this->pointer_to_bit.base_ptr) = ((*(this->pointer_to_bit.base_ptr))^(~mask))|(val ? mask : 0);
        return *this;
    }
};

struct bit_alloc{ // Allocation/deallocation helper. Usage: bitptr_t new_bits = new bit_alloc[bit_num]; delete[] new_bits;
    NODISCARD static void* operator new(size_t size){
        size_t to_be_allocated = ((size - 1)/8) + 1;
        void* allocated = std::malloc(to_be_allocated);
        if(!allocated) throw std::bad_alloc();
        return allocated;
    }

    NODISCARD static void* operator new(size_t size, const std::nothrow_t&) noexcept{
        size_t to_be_allocated = ((size - 1)/8) + 1;
        void* allocated = std::malloc(to_be_allocated);
        return allocated;
    }

    NODISCARD static void* operator new[](size_t size){
        size_t to_be_allocated = ((size - 1)/8) + 1;
        void* allocated = std::malloc(to_be_allocated);
        if(!allocated) throw std::bad_alloc();
        return allocated;
    }

    NODISCARD static void* operator new[](size_t size, const std::nothrow_t&) noexcept{
        size_t to_be_allocated = ((size - 1)/8) + 1;
        void* allocated = std::malloc(to_be_allocated);
        return allocated;
    }

    static void operator delete (void* ptr) noexcept {
        std::free(ptr);
    }

    static void operator delete[] (void* ptr, size_t) noexcept{
        std::free(ptr);
    }

};

bit_t bitptr_t::operator*(){
    return *this;
}

bit_t bitptr_t::operator[](size_t index){
    return *(*this + index);
}

#undef NODISCARD
#undef INTEGRAL
#endif
