#if !defined(COMMON_H)
#define COMMON_H
/**
 * @file   common.h
 * @brief  Common.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include <stdint.h>

static inline void _unused( int a, ... ) { (void)a; return; }
#define unused(...) _unused( 0, __VA_ARGS__ )

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef intptr_t  isize;
typedef uintptr_t usize;

typedef float  f32;
typedef double f64;

typedef u8    b8;
typedef u16   b16;
typedef u32   b32;
typedef u64   b64;
typedef usize bsize;

typedef struct String {
    union {
        const char* cc;
        char* c;
    };
    u32 len;
} String;

#define string_new( str, length ) (String){ .cc=str, .len=length } 
#define string_text( text ) string_new( text, sizeof(text) - 1 )

#if !defined(NULL)
    #define NULL ((void*)0)
#endif

#endif /* header guard */
