#ifndef W128_H
#define W128_H

#define MIN_BITS 8
#define MAX_BITS 128

#ifndef BITS
#define BITS 8
#endif

#ifndef USE_ASM
#define USE_ASM 1
#endif

#include <stdint.h>
#include <string.h>

#if(BITS == 128)
#include <mmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#define INLINE inline __attribute__((always_inline))

typedef union {
    uint64_t q[2];
    uint32_t d[4];
    uint16_t w[8];
    uint8_t  b[16];
#if(BITS == 128)
    __m128i value;
#endif
} w128_t;

#define BIT_PARTS_8 (MAX_BITS / 8)
#define BIT_PARTS_16 (MAX_BITS / 16)
#define BIT_PARTS_32 (MAX_BITS / 32)
#define BIT_PARTS_64 (MAX_BITS / 64)
#define BIT_PARTS_128 (MAX_BITS / 128)

#define BIT_PARTS (MAX_BITS / BITS)
#define MAX_BIT_PARTS (MAX_BITS / MIN_BITS)

#define ACCESS_128_VALUE_8(key, part) (key.b[part])
#define ACCESS_128_VALUE_16(key, part) (key.w[part])
#define ACCESS_128_VALUE_32(key, part) (key.d[part])
#define ACCESS_128_VALUE_64(key, part) (key.q[part])
#define ACCESS_128_VALUE_128(key, part) (key.value)

typedef uint8_t uint_t_8;
typedef uint16_t uint_t_16;
typedef uint32_t uint_t_32;
typedef uint64_t uint_t_64;
#if(BITS == 128)
typedef __m128i uint_t_128;
#endif

typedef uint8_t min_uint_t_8;
typedef uint16_t min_uint_t_16;
typedef uint32_t min_uint_t_32;
typedef uint64_t min_uint_t_64;
typedef uint64_t min_uint_t_128;

#define GENERAL_VALUE_8(x) (x)
#define GENERAL_VALUE_16(x) (x)
#define GENERAL_VALUE_32(x) (x)
#define GENERAL_VALUE_64(x) (x)
#define GENERAL_VALUE_128(x) (x.q[0])

#if(BITS == 8)
#define ACCESS_128_VALUE ACCESS_128_VALUE_8
#define GENERAL_VALUE GENERAL_VALUE_8
typedef uint_t_8 uint_t;
typedef min_uint_t_8 min_uint_t;
#endif
#if(BITS == 16)
#define ACCESS_128_VALUE ACCESS_128_VALUE_16
#define GENERAL_VALUE GENERAL_VALUE_126
typedef uint_t_16 uint_t;
typedef min_uint_t_16 min_uint_t;
#endif
#if(BITS == 32)
#define ACCESS_128_VALUE ACCESS_128_VALUE_32
#define GENERAL_VALUE GENERAL_VALUE_32
typedef uint_t_32 uint_t;
typedef min_uint_t_32 min_uint_t;
#endif
#if(BITS == 64)
#define ACCESS_128_VALUE ACCESS_128_VALUE_64
#define GENERAL_VALUE GENERAL_VALUE_64
typedef uint_t_64 uint_t;
typedef min_uint_t_64 min_uint_t;
#endif
#if(BITS == 128)
#define ACCESS_128_VALUE ACCESS_128_VALUE_128
#define GENERAL_VALUE GENERAL_VALUE_128
typedef uint_t_128 uint_t;
typedef min_uint_t_128 min_uint_t;
#endif

extern "C" void zero128asm8(w128_t* x);
extern "C" void zero128asm16(w128_t* x);
extern "C" void zero128asm32(w128_t* x);
extern "C" void zero128asm64(w128_t* x);
extern "C" void zero128asm(w128_t* x);

INLINE void zero128(w128_t& x) {
#if(USE_ASM)
#if(BITS == 8)
    zero128asm8(&x);
#endif
#if(BITS == 16)
    zero128asm16(&x);
#endif
#if(BITS == 32)
    zero128asm32(&x);
#endif
#if(BITS == 64)
    zero128asm64(&x);
#endif
#if(BITS == 128)
    for (int i = 0; i < 2; i++){
        ACCESS_128_VALUE_64(x, i) = 0;
    }
#endif
#else
#if(BITS == 8 || BITS == 16)
    memset(&x, 0, sizeof(x));
#elif(BITS == 128)
    for (int i = 0; i < 2; i++){
        ACCESS_128_VALUE_64(x, i) = 0;
    }
#else
    for (int i = 0; i < BIT_PARTS; i++){
        ACCESS_128_VALUE(x, i) = 0;
    }
#endif
#endif
}

extern "C" void copy128asm8(w128_t* to, const w128_t* from);
extern "C" void copy128asm16(w128_t* to, const w128_t* from);
extern "C" void copy128asm32(w128_t* to, const w128_t* from);
extern "C" void copy128asm64(w128_t* to, const w128_t* from);
extern "C" void copy128asm(w128_t* to, const w128_t* from);

INLINE void copy128(w128_t& to, const w128_t& from) {
#if(USE_ASM)
#if(BITS == 8)
    copy128asm8(&to, &from);
#endif
#if(BITS == 16)
    copy128asm16(&to, &from);
#endif
#if(BITS == 32)
    copy128asm32(&to, &from);
#endif
#if(BITS == 64)
    copy128asm64(&to, &from);
#endif
#if(BITS == 128)
    to.value = from.value;
#endif
#else
#if(BITS == 8 || BITS == 16)
    __builtin_memcpy(&to, &from, sizeof(w128_t));
#else
    for (int i = 0; i < BIT_PARTS; i++){
        ACCESS_128_VALUE(to, i) = ACCESS_128_VALUE(from, i);
    }
#endif
#endif
}

extern "C" void append128asm8(w128_t* x, const w128_t* y);
extern "C" void append128asm16(w128_t* x, const w128_t* y);
extern "C" void append128asm32(w128_t* x, const w128_t* y);
extern "C" void append128asm64(w128_t* x, const w128_t* y);
extern "C" void append128asm(w128_t* x, const w128_t* y);

INLINE void append128(w128_t& x, const w128_t& y){
#if(USE_ASM)
#if(BITS == 8)
    append128asm8(&x, &y);
#endif
#if(BITS == 16)
    append128asm16(&x, &y);
#endif
#if(BITS == 32)
    append128asm32(&x, &y);
#endif
#if(BITS == 64)
    append128asm64(&x, &y);
#endif
#if(BITS == 128)
    x.value ^= y.value;
#endif
#else
    for (int i = 0; i < BIT_PARTS; i++) {
        ACCESS_128_VALUE(x, i) ^= ACCESS_128_VALUE(y, i);
    }
#endif
}

extern "C" void plus128asm8(w128_t* result, const w128_t* x, const w128_t* y);
extern "C" void plus128asm16(w128_t* result, const w128_t* x, const w128_t* y);
extern "C" void plus128asm32(w128_t* result, const w128_t* x, const w128_t* y);
extern "C" void plus128asm64(w128_t* result, const w128_t* x, const w128_t* y);
extern "C" void plus128asm(w128_t* result, const w128_t* x, const w128_t* y);

INLINE void plus128(w128_t& result, const w128_t& x, const w128_t& y){
#if(USE_ASM)
#if(BITS == 8)
    plus128asm8(&result, &x, &y);
#endif
#if(BITS == 16)
    plus128asm16(&result, &x, &y);
#endif
#if(BITS == 32)
    plus128asm32(&result, &x, &y);
#endif
#if(BITS == 64)
    plus128asm64(&result, &x, &y);
#endif
#if(BITS == 128)
    result.value = x.value ^ y.value;
#endif
#else
    copy128(result, x);
    append128(result, y);
#endif
}

extern "C" void sum128asm(w128_t* result, const w128_t* array);

INLINE void sum128(w128_t& result, const w128_t* array) {
#if(USE_ASM)
    sum128asm(&result, array);
#else
    zero128(result);
    for (int i = 0; i < MAX_BIT_PARTS; i++) {
        append128(result, array[i]);
    }
#endif
}

extern "C" void plus128multi_asm8(w128_t* result, const w128_t* x, const w128_t array[][256]);
extern "C" void plus128multi_asm16(w128_t* result, const w128_t* x, const w128_t array[][256]);
extern "C" void plus128multi_asm32(w128_t* result, const w128_t* x, const w128_t array[][256]);
extern "C" void plus128multi_asm64(w128_t* result, const w128_t* x, const w128_t array[][256]);
extern "C" void plus128multi_asm(w128_t* result, const w128_t* x, const w128_t array[][256]);

// result & x must be different
INLINE void plus128multi(w128_t& result, const w128_t& x, const w128_t array[][256]) {
#if(USE_ASM)
#if(BITS == 8)
    plus128multi_asm8(&result, &x, array);
#endif
#if(BITS == 16)
    plus128multi_asm16(&result, &x, array);
#endif
#if(BITS == 32)
    plus128multi_asm32(&result, &x, array);
#endif
#if(BITS == 64)
    plus128multi_asm64(&result, &x, array);
#endif
#if(BITS == 128)
    zero128(result);
    for (int i = 0; i < MAX_BIT_PARTS; i++) {
        append128(result, array[i][ACCESS_128_VALUE_8(x, i)]);
    }
#endif
#else
    zero128(result);
    for (int i = 0; i < MAX_BIT_PARTS; i++) {
        append128(result, array[i][ACCESS_128_VALUE_8(x, i)]);
    }
#endif
}

extern "C" void append128multi_asm8(w128_t* result, w128_t* x, const w128_t array[][256]);
extern "C" void append128multi_asm16(w128_t* result, w128_t* x, const w128_t array[][256]);
extern "C" void append128multi_asm32(w128_t* result, w128_t* x, const w128_t array[][256]);
extern "C" void append128multi_asm64(w128_t* result, w128_t* x, const w128_t array[][256]);
extern "C" void append128multi_asm(w128_t* result, w128_t* x, const w128_t array[][256]);

INLINE void append128multi(w128_t& result, w128_t& x, const w128_t array[][256]) {
#if(USE_ASM)
#if(BITS == 8)
    append128multi_asm8(&result, &x, array);
#endif
#if(BITS == 16)
    append128multi_asm16(&result, &x, array);
#endif
#if(BITS == 32)
    append128multi_asm32(&result, &x, array);
#endif
#if(BITS == 64)
    append128multi_asm64(&result, &x, array);
#endif
#if(BITS == 128)
    plus128multi(result, x, array);
    copy128(x, result);
#endif
#else
    plus128multi(result, x, array);
    copy128(x, result);
#endif
}

extern "C" void convert128asm(w128_t* x, const uint8_t* array);

INLINE void convert128(w128_t& x, const uint8_t* array) {
#if(USE_ASM)
    convert128asm(&x, array);
#else
    for (int i = 0; i < MAX_BIT_PARTS; i++) {
        ACCESS_128_VALUE_8(x, i) = array[ACCESS_128_VALUE_8(x, i)];
    }
#endif
}

#endif
