// kuznechik.c
// 04-Jan-15  Markku-Juhani O. Saarinen <mjos@iki.fi>

// This is the basic non-optimized 8-bit "readble" version of the cipher.
// Conforms to included doc/GOSTR-bsh.pdf file, which has an internal
// date of September 2, 2014.

#include "kuznechik.h"

// The S-Box from section 5.1.1

extern const uint8_t kuz_pi[0x100] = {
	0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16, 	// 00..07
	0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D, 	// 08..0F
	0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA, 	// 10..17
	0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1, 	// 18..1F
	0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21, 	// 20..27
	0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F, 	// 28..2F
	0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0, 	// 30..37
	0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F, 	// 38..3F
	0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB, 	// 40..47
	0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC, 	// 48..4F
	0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12, 	// 50..57
	0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87, 	// 58..5F
	0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7, 	// 60..67
	0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1, 	// 68..6F
	0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E, 	// 70..77
	0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57, 	// 78..7F
	0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9, 	// 80..87
	0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03, 	// 88..8F
	0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC, 	// 90..97
	0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A, 	// 98..9F
	0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44, 	// A0..A7
	0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41, 	// A8..AF
	0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F, 	// B0..B7
	0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B, 	// B8..BF
	0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7, 	// C0..C7
	0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89, 	// C8..CF
	0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE, 	// D0..D7
	0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61, 	// D8..DF
	0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B, 	// E0..E7
	0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52, 	// E8..EF
	0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0, 	// F0..F7
	0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6, 	// F8..FF
};

// Inverse S-Box

#if(!USE_ASM)
extern const uint8_t kuz_pi_inv[0x100] = {
	0xA5, 0x2D, 0x32, 0x8F, 0x0E, 0x30, 0x38, 0xC0, 	// 00..07
	0x54, 0xE6, 0x9E, 0x39, 0x55, 0x7E, 0x52, 0x91, 	// 08..0F
	0x64, 0x03, 0x57, 0x5A, 0x1C, 0x60, 0x07, 0x18, 	// 10..17
	0x21, 0x72, 0xA8, 0xD1, 0x29, 0xC6, 0xA4, 0x3F, 	// 18..1F
	0xE0, 0x27, 0x8D, 0x0C, 0x82, 0xEA, 0xAE, 0xB4, 	// 20..27
	0x9A, 0x63, 0x49, 0xE5, 0x42, 0xE4, 0x15, 0xB7, 	// 28..2F
	0xC8, 0x06, 0x70, 0x9D, 0x41, 0x75, 0x19, 0xC9, 	// 30..37
	0xAA, 0xFC, 0x4D, 0xBF, 0x2A, 0x73, 0x84, 0xD5, 	// 38..3F
	0xC3, 0xAF, 0x2B, 0x86, 0xA7, 0xB1, 0xB2, 0x5B, 	// 40..47
	0x46, 0xD3, 0x9F, 0xFD, 0xD4, 0x0F, 0x9C, 0x2F, 	// 48..4F
	0x9B, 0x43, 0xEF, 0xD9, 0x79, 0xB6, 0x53, 0x7F, 	// 50..57
	0xC1, 0xF0, 0x23, 0xE7, 0x25, 0x5E, 0xB5, 0x1E, 	// 58..5F
	0xA2, 0xDF, 0xA6, 0xFE, 0xAC, 0x22, 0xF9, 0xE2, 	// 60..67
	0x4A, 0xBC, 0x35, 0xCA, 0xEE, 0x78, 0x05, 0x6B, 	// 68..6F
	0x51, 0xE1, 0x59, 0xA3, 0xF2, 0x71, 0x56, 0x11, 	// 70..77
	0x6A, 0x89, 0x94, 0x65, 0x8C, 0xBB, 0x77, 0x3C, 	// 78..7F
	0x7B, 0x28, 0xAB, 0xD2, 0x31, 0xDE, 0xC4, 0x5F, 	// 80..87
	0xCC, 0xCF, 0x76, 0x2C, 0xB8, 0xD8, 0x2E, 0x36, 	// 88..8F
	0xDB, 0x69, 0xB3, 0x14, 0x95, 0xBE, 0x62, 0xA1, 	// 90..97
	0x3B, 0x16, 0x66, 0xE9, 0x5C, 0x6C, 0x6D, 0xAD, 	// 98..9F
	0x37, 0x61, 0x4B, 0xB9, 0xE3, 0xBA, 0xF1, 0xA0, 	// A0..A7
	0x85, 0x83, 0xDA, 0x47, 0xC5, 0xB0, 0x33, 0xFA, 	// A8..AF
	0x96, 0x6F, 0x6E, 0xC2, 0xF6, 0x50, 0xFF, 0x5D, 	// B0..B7
	0xA9, 0x8E, 0x17, 0x1B, 0x97, 0x7D, 0xEC, 0x58, 	// B8..BF
	0xF7, 0x1F, 0xFB, 0x7C, 0x09, 0x0D, 0x7A, 0x67, 	// C0..C7
	0x45, 0x87, 0xDC, 0xE8, 0x4F, 0x1D, 0x4E, 0x04, 	// C8..CF
	0xEB, 0xF8, 0xF3, 0x3E, 0x3D, 0xBD, 0x8A, 0x88, 	// D0..D7
	0xDD, 0xCD, 0x0B, 0x13, 0x98, 0x02, 0x93, 0x80, 	// D8..DF
	0x90, 0xD0, 0x24, 0x34, 0xCB, 0xED, 0xF4, 0xCE, 	// E0..E7
	0x99, 0x10, 0x44, 0x40, 0x92, 0x3A, 0x01, 0x26, 	// E8..EF
	0x12, 0x1A, 0x48, 0x68, 0xF5, 0x81, 0x8B, 0xC7, 	// F0..F7
	0xD6, 0x20, 0x0A, 0x08, 0x00, 0x4C, 0xD7, 0x74	 	// F8..FF
};
#endif

// Linear vector from sect 5.1.2

extern const uint8_t kuz_lvec[16] = {
	0x94, 0x20, 0x85, 0x10, 0xC2, 0xC0, 0x01, 0xFB, 
	0x01, 0xC0, 0xC2, 0x10, 0x85, 0x20, 0x94, 0x01
};

//#define USE_GALOIS
//#define USE_MUL_TABLE
//#define USE_TABLES
//#define BITS 8
//#define BITS 16
//#define BITS 32
//#define BITS 64
//#define BITS 128

#if(USE_TABLES)
#if(!USE_ASM)
w128_t kuz_pil_enc128[MAX_BIT_PARTS][256];
w128_t kuz_l_dec128[MAX_BIT_PARTS][256];
w128_t kuz_pil_dec128[MAX_BIT_PARTS][256];
#endif
#endif

#if(USE_MUL_TABLE)
#if(!USE_ASM)
uint8_t mul_table[256][256];
#endif
#endif

#if(USE_GALOIS)
//Galois gal;
#endif

uint64_t used_memory_count = 0;
uint64_t encrypt_used_memory_count = 0;
uint64_t decrypt_used_memory_count = 0;

uint64_t get_used_memory_count() {
    return used_memory_count;
}

uint64_t get_encrypt_used_memory_count() {
    return encrypt_used_memory_count;
}

uint64_t get_decrypt_used_memory_count() {
    return decrypt_used_memory_count;
}

uint8_t get_bits() {
    return BITS;
}

bool use_galois() {
    return USE_GALOIS;
}

bool use_mul_table() {
    return USE_MUL_TABLE;
}

bool use_tables() {
    return USE_TABLES;
}

#include <stdio.h>

uint8_t kuz_mul_gf256_real(uint8_t x, uint8_t y)
{
    uint8_t z;
    
     z = 0;
     while (y > 0) {
         if (y & 1) {
             z ^= x;
         }
         x = (x << 1) ^ (x & 0x80 ? 0xC3 : 0x00);
         y >>= 1;
     }
    
    return z;
}

#if(USE_MUL_TABLE)
#if(!USE_ASM)
void generate_mul_table() {
    for (int l = 0; l < 256; l++){
        for (int m = 0; m < 256; m++){
#if(USE_GALOIS)
            mul_table[l][m] = galois_mul(l,m);
#else
            mul_table[l][m] = kuz_mul_gf256_real(l,m);
#endif
        }
    }
}
#endif
#endif

// poly multiplication mod p(x) = x^8 + x^7 + x^6 + x + 1

extern "C" INLINE uint8_t kuz_mul_gf256(uint8_t x, uint8_t y)
{
#if(!USE_MUL_TABLE)
#if(USE_GALOIS)
    return galois_mul(x, y);
#else
    return kuz_mul_gf256_real(x,y);
#endif
#else
#if(!USE_ASM)
    return mul_table[x][y];
#else
    return galois_mul(x, y);
#endif
#endif
}

extern "C" uint8_t kuz_mul_gf256_func(uint8_t x, uint8_t y)
{
#if(!USE_MUL_TABLE)
#if(USE_GALOIS)
    return galois_mul(x, y);
#else
    return kuz_mul_gf256_real(x,y);
#endif
#else
#if(!USE_ASM)
    return mul_table[x][y];
#else
    return galois_mul(x, y);
#endif
#endif
}

#if(USE_ASM)
extern "C" void kuz_l_asm(w128_t *w);
extern "C" void kuz_l_asm_table(w128_t *w);
#endif

// linear operation l

INLINE void kuz_l(w128_t *w)
{
#if(!USE_ASM)
	uint8_t x;
	
	// 16 rounds
	for (unsigned int j = 0; j < sizeof(kuz_lvec)/sizeof(kuz_lvec[0]); j++) {

		// An LFSR with 16 elements from GF(2^8)
		x = w->b[15];	// since lvec[15] = 1

		for (int i = 14; i >= 0; i--) {
			w->b[i + 1] = w->b[i];
			x ^= kuz_mul_gf256(w->b[i], kuz_lvec[i]);
		}
		w->b[0] = x;
	}
#else
#if(USE_MUL_TABLE)
    kuz_l_asm_table(w);
#else
    kuz_l_asm(w);
#endif
#endif
}

#if(USE_ASM)
extern "C" void kuz_l_inv_asm(w128_t *w);
extern "C" void kuz_l_inv_asm_table(w128_t *w);
#endif

// inverse of linear operation l

INLINE void kuz_l_inv(w128_t *w)
{
#if(!USE_ASM)
	uint8_t x;
	
	// 16 rounds
	for (unsigned int j = 0; j < sizeof(kuz_lvec)/sizeof(kuz_lvec[0]); j++) {

		x = w->b[0];
		for (int i = 0; i < 15; i++) {
			w->b[i] = w->b[i + 1];	
			x ^= kuz_mul_gf256(w->b[i], kuz_lvec[i]);
		}
		w->b[15] = x;
	}
#else
#if(USE_MUL_TABLE)
    kuz_l_inv_asm_table(w);
#else
    kuz_l_inv_asm(w);
#endif
#endif
}

#if(USE_TABLES)
#if(!USE_ASM)

INLINE void generate_pil_enc128(w128_t& x,  int i, int j){
    zero128(x);
    x.b[i] = kuz_pi[j];
    kuz_l(&x);
}

INLINE void generate_l_dec128(w128_t& x,  int i, int j){
    zero128(x);
    x.b[i] = j; // kuz_pi[j];
    kuz_l_inv(&x);
}

INLINE void generate_pil_dec128(w128_t& x,  int i, int j){
    zero128(x);
    x.b[i] = kuz_pi_inv[j];
    kuz_l_inv(&x);
}

INLINE void generate_table_row_by_value(w128_t table_row[], void(*gen_function)(w128_t&, int, int), const w128_t& value){
    w128_t x;
    
    for (int i = 0; i < MAX_BIT_PARTS; i++) {
        gen_function(x, i, ACCESS_128_VALUE_8(value, i));
        table_row[i] = x;
    }
}

INLINE void generate_table_row(w128_t table[][256], void(*gen_function)(w128_t&, int, int), int row){
    w128_t x;
    
    for (int i = 0; i < MAX_BIT_PARTS; i++) {
        gen_function(x, i, row);
        table[i][row] = x;
    }
}

INLINE void generate_table(w128_t table[][256], void(*gen_function)(w128_t&, int, int)){
    for (int i = 0; i < 256; i++) {
        generate_table_row(table, gen_function, i);
    }
}

#endif
#endif

#if(USE_ASM)
extern "C" void get_tables_size_asm(uint64_t* value);
extern "C" void get_encrypt_tables_size_asm(uint64_t* value);
extern "C" void get_decrypt_tables_size_asm(uint64_t* value);
#endif

INLINE void calc_used_memory_count() {
    used_memory_count = 0;
    
#if(!USE_ASM)
    
#if(USE_GALOIS)
    used_memory_count += galois_get_used_memory_count();
#endif
#if(USE_MUL_TABLE)
    used_memory_count += sizeof(mul_table);
#endif
#if(USE_TABLES)
    used_memory_count += sizeof(kuz_pil_enc128) + sizeof(kuz_l_dec128) + sizeof(kuz_pil_dec128);
#endif

    encrypt_used_memory_count = 0;
#if(USE_GALOIS)
    encrypt_used_memory_count += galois_get_used_memory_count();
#endif
#if(USE_MUL_TABLE)
    encrypt_used_memory_count += sizeof(mul_table);
#endif
#if(USE_TABLES)
    encrypt_used_memory_count += sizeof(kuz_pil_enc128);
#endif

#if(USE_GALOIS)
    decrypt_used_memory_count += galois_get_used_memory_count();
#endif
#if(USE_MUL_TABLE)
    decrypt_used_memory_count += sizeof(mul_table);
#endif
#if(USE_TABLES)
    decrypt_used_memory_count += sizeof(kuz_l_dec128) + sizeof(kuz_pil_dec128);
#endif

#else

    get_tables_size_asm(&used_memory_count);
    get_encrypt_tables_size_asm(&encrypt_used_memory_count);
    get_decrypt_tables_size_asm(&decrypt_used_memory_count);

#endif
    
}

void kuz_init()
{
#if(!USE_ASM)
    
#if(USE_GALOIS)
    galois_init();
#endif
    
#if(USE_MUL_TABLE)
    generate_mul_table();
#endif
    
#if(USE_TABLES)
    generate_table(kuz_pil_enc128, generate_pil_enc128);
    generate_table(kuz_l_dec128, generate_l_dec128);
    generate_table(kuz_pil_dec128, generate_pil_dec128);
#endif
    
#endif

    calc_used_memory_count();
}

// key setup

void kuz_set_encrypt_key(kuz_key_t *kuz, const uint8_t key[32])
{
	w128_t c, x, y, z;
		
	for (int i = 0; i < 16; i++) {
		// this will be have to changed for little-endian systems
		x.b[i] = key[i];
		y.b[i] = key[i + 16];
	}

    copy128(kuz->k[0], x);
    copy128(kuz->k[1], y);

	for (int i = 1; i <= 32; i++) {

		// C Value
        zero128(c);
		c.b[15] = i;		// load round in lsb
		kuz_l(&c);
		
        plus128(z, x, c);
        convert128(z, kuz_pi);
		kuz_l(&z);
        append128(z, y);
		
        copy128(y, x);
        copy128(x, z);

		if ((i & 7) == 0) {
            int k = i >> 2;
            copy128(kuz->k[k], x);
            copy128(kuz->k[k + 1], y);
		}
	}
}

// these two funcs are identical in the simple implementation

void kuz_set_decrypt_key(kuz_key_t *kuz, const uint8_t key[32])
{
    kuz_set_encrypt_key(kuz, key);
    
    #if(USE_TABLES)
    for (int i = 1; i < 10; i++) {
        kuz_l_inv(&kuz->k[i]);
    }
    #endif
}

#if(USE_ASM)
extern "C" void kuz_encrypt_block_asm(w128_t* result, kuz_key_t *key, w128_t* x);
extern "C" void kuz_encrypt_block_asm_mul_table(w128_t* result, kuz_key_t *key, w128_t* x);
extern "C" void kuz_encrypt_block_asm_tables(w128_t* result, kuz_key_t *key, w128_t* x);
#endif

w128_t buffer;
void kuz_encrypt_block(kuz_key_t *key, w128_t& x)
{
#if(!USE_ASM)
    for (int i = 0; i < 9; i++) {
        append128(x, key->k[i]);
#if(USE_TABLES)
        append128multi(buffer, x, kuz_pil_enc128);
#else
        convert128(x, kuz_pi);
        kuz_l(&x);
#endif
    }
    
    append128(x, key->k[9]);
#else
#if(USE_TABLES)
    kuz_encrypt_block_asm_tables(&buffer, key, &x);
#elif(USE_MUL_TABLE)
    kuz_encrypt_block_asm_mul_table(&buffer, key, &x);
#else
    kuz_encrypt_block_asm(&buffer, key, &x);
#endif
#endif
}

#if(USE_ASM)
extern "C" void kuz_decrypt_block_asm(kuz_key_t *key, w128_t* x);
extern "C" void kuz_decrypt_block_asm_mul_table(kuz_key_t *key, w128_t* x);
extern "C" void kuz_decrypt_block_asm_tables(w128_t* result, kuz_key_t *key, w128_t* x);
#endif

#if(!USE_TABLES)
void kuz_decrypt_block(kuz_key_t *key, w128_t& x)
{
#if(!USE_ASM)
    append128(x, key->k[9]);
    
	for (int i = 8; i >= 0; i--) {
		kuz_l_inv(&x);
        convert128(x, kuz_pi_inv);
        append128(x, key->k[i]);
	}
#else
#if(USE_MUL_TABLE)
    kuz_decrypt_block_asm_mul_table(key, &x);
#else
    kuz_decrypt_block_asm(key, &x);
#endif
#endif
}
#endif

#if(USE_TABLES)
void kuz_decrypt_block(kuz_key_t *key, w128_t& x)
{
#if(!USE_ASM)
    append128multi(buffer, x, kuz_l_dec128);
    
    for (int i = 9; i > 1; i--) {
        append128(x, key->k[i]);
        append128multi(buffer, x, kuz_pil_dec128);
    }
    
    append128(x, key->k[1]);
    convert128(x, kuz_pi_inv);
    append128(x, key->k[0]);
#else
    kuz_decrypt_block_asm_tables(&buffer, key, &x);
#endif
}
#endif





