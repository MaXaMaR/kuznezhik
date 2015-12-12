#ifndef KUZNECHIK_H
#define KUZNECHIK_H

#ifndef USE_GALOIS
#define USE_GALOIS 0
#endif

#ifndef USE_MUL_TABLE
#define USE_MUL_TABLE 0
#endif

#ifndef USE_TABLES
#define USE_TABLES 0
#endif

#ifndef USE_ASM
#define USE_ASM 1
#endif

#include "w128.h"
#include "galois.h"

// cipher context
typedef struct {
	w128_t k[10];		// round keys
} kuz_key_t;

// init lookup tables
void kuz_init();

// key setup
void kuz_set_encrypt_key(kuz_key_t *subkeys, const uint8_t key[32]);	
void kuz_set_decrypt_key(kuz_key_t *subkeys, const uint8_t key[32]);	

// single-block ecp ops
void kuz_encrypt_block(kuz_key_t *subkeys, w128_t& x);
void kuz_decrypt_block(kuz_key_t *subkeys, w128_t& x);

INLINE void kuz_encrypt_block(kuz_key_t *subkeys, void *x) {
    kuz_encrypt_block(subkeys, *(w128_t*) x);
}

INLINE void kuz_decrypt_block(kuz_key_t *subkeys, void *x) {
    kuz_decrypt_block(subkeys, *(w128_t*) x);
}

uint64_t get_used_memory_count();
uint64_t get_encrypt_used_memory_count();
uint64_t get_decrypt_used_memory_count();

uint8_t get_bits();

bool use_galois();

bool use_mul_table();

bool use_tables();

#if(!USE_ASM)
extern const uint8_t kuz_pi[0x100];
extern const uint8_t kuz_pi_inv[0x100];
extern const uint8_t kuz_lvec[16];
#endif

#if(USE_TABLES)
#if(!USE_ASM)
extern "C" w128_t kuz_pil_enc128[MAX_BIT_PARTS][256];
extern "C" w128_t kuz_l_dec128[MAX_BIT_PARTS][256];
extern "C" w128_t kuz_pil_dec128[MAX_BIT_PARTS][256];
#endif
#endif

extern "C" uint8_t kuz_mul_gf256_func(uint8_t x, uint8_t y);

#endif
