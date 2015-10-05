#ifndef GALOIS_H
#define GALOIS_H

#include <stdint.h>

#include "w128.h"

#define POWER 8

#define FIELD_SIZE ((1 << POWER) - 1)

#define GFERROR (-1)

/*
class Galois {
public:
    typedef uint8_t symbol;
    
    Galois() {
        alpha_to = new symbol[FIELD_SIZE + 1];
        index_of = new symbol[FIELD_SIZE + 1];
        
        generate_field();
    }
    
    INLINE symbol mul(const symbol& x, const symbol& y) {
        if (__builtin_expect(x != 0 && y != 0, true)) {
            return alpha_to[(index_of[x] + index_of[y]) % FIELD_SIZE];
        } else {
            return 0;
        }
    }
    
    INLINE uint32_t get_used_memory_count() {
        return (uint32_t) sizeof(symbol) * (FIELD_SIZE + 1) * 2;
    }
    
private:
    void generate_field();
    
    symbol* alpha_to;
    symbol* index_of;
};
*/

#if(!USE_ASM)
extern "C" uint8_t alpha_to[256];
extern "C" uint8_t index_of[256];
#endif

#if(!USE_ASM)
void generate_field();
#endif

INLINE void galois_init() {
    #if(!USE_ASM)
    generate_field();
    #endif
}

#if(USE_ASM)
extern "C" uint8_t galois_mul_asm_optimized(uint8_t x, uint8_t y);
extern "C" uint8_t galois_mul_asm_table(uint8_t x, uint8_t y);
#endif

INLINE uint8_t galois_mul(uint8_t x, uint8_t y) {
#if(!USE_ASM)
    if (__builtin_expect(x != 0 && y != 0, true)) {
        return alpha_to[(index_of[x] + index_of[y]) % FIELD_SIZE];
    } else {
        return 0;
    }
#else
#if(!USE_MUL_TABLE)
    return galois_mul_asm_optimized(x, y);
#else
    return galois_mul_asm_table(x, y);
#endif
#endif
}

INLINE uint32_t galois_get_used_memory_count() {
    return (uint32_t) sizeof(uint8_t) * (FIELD_SIZE + 1) * 2;
}

#endif

