#if !defined(COMMON_H)
#define COMMON_H
/**
 * @file   common.h
 * @brief  Common.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include <stdint.h>
#include "raylib.h" // IWYU pragma: export

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

#if !defined(NULL)
    #define NULL ((void*)0)
#endif

#define GAME_NAME "GMTK 2024"
#define GAME_WIDTH 1280
#define GAME_HEIGHT 720

#define TEXT_FONT_SIZE (64)
#define TEXT_FONT_SIZE_SMALL (32)
#define TEXT_FONT_SIZE_SMALLEST (24)
#define TITLE_FONT_SIZE (128)

void quit_game(void);

#endif /* header guard */
