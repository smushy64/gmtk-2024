/**
 * @file   mathex.c
 * @brief  Math
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "mathex.h"

f32 inverse_lerp( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}

Vector3 v3_lerp( Vector3 a, Vector3 b, f32 t ) {
    return (Vector3) {
        Lerp( a.x, b.x, t ),
        Lerp( a.y, b.y, t ),
        Lerp( a.z, b.z, t ),
    };
}

