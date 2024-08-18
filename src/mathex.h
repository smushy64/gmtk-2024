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

#define absf( x ) ( ((x) < 0.0f) ? ((x) * -1.0f) : (x) )

f32 inverse_lerp( f32 a, f32 b, f32 v );

Vector3 v3_lerp( Vector3 a, Vector3 b, f32 t );

#define v2_scalar( v ) (Vector2){ .x=v, .y=v }
#define v2( _x, _y ) (Vector2){ .x=_x, .y=_y }

#define v3_scalar( v ) v3( v, v, v )
#define v3( _x, _y, _z ) (Vector3){ _x, _y, _z }

#define v3_zero()    v3_scalar( 0.0f )
#define v3_one()     v3_scalar( 1.0f )
#define v3_down()    v3( 0.0f, -1.0f, 0.0f )
#define v3_up()      v3( 0.0f, 1.0f, 0.0f )
#define v3_left()    v3( -1.0f, 0.0f, 0.0f )
#define v3_right()   v3( 1.0f, 0.0f, 0.0f )
#define v3_back()    v3( 0.0f, 0.0f, -1.0f )
#define v3_forward() v3( 0.0f, 0.0f, 1.0f )

#endif /* header guard */
