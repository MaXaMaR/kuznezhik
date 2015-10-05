#include "galois.h"

const unsigned int primitive_poly[9] = {1,1,0,0,0,0,1,1,1};

#if(!USE_ASM)
uint8_t alpha_to[256];
uint8_t index_of[256];

void generate_field()
{
    int mask = 1;
    
    alpha_to[POWER] = 0;
    
    for (unsigned int i = 0; i < POWER; i++) {
        alpha_to[i]           = mask;
        index_of[alpha_to[i]] = i;
        
        if (primitive_poly[i] != 0) {
            alpha_to[POWER] ^= mask;
        }
        
        mask <<= 1;
    }
    
    index_of[alpha_to[POWER]] = POWER;
    
    mask >>= 1;
    
    for (unsigned int i = POWER + 1; i < FIELD_SIZE; i++) {
        if (alpha_to[i - 1] >= mask) {
            alpha_to[i] = alpha_to[POWER] ^ ((alpha_to[i - 1] ^ mask) << 1);
        } else {
            alpha_to[i] = alpha_to[i - 1] << 1;
        }
        
        index_of[alpha_to[i]] = i;
    }
    
    index_of[0] = GFERROR;
    alpha_to[FIELD_SIZE] = 1;
}
#endif

