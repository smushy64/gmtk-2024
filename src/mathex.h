#if !defined(MATHEX_H)
#define MATHEX_H
/**
 * @file   mathex.h
 * @brief  Extended math.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"
// IWYU pragma: begin_exports
#include <math.h>
#include "raymath.h"
// IWYU pragma: end_exports

f32 inverse_lerp( f32 a, f32 b, f32 v );

#define v2_scalar( v ) (Vector2){ .x=v, .y=v }
#define v2( _x, _y ) (Vector2){ .x=_x, .y=_y }

#endif /* header guard */
