/**
 * @file   physics.c
 * @brief  Physics.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 17, 2024
*/
#include "physics.h"
#include "mathex.h"

Vector3 velocity_apply_drag( Vector3 velocity, f32 drag, f32 dt ) {
    f32 multiplier = 1.0f - drag * dt;
    if( multiplier < 0.0f ) {
        multiplier = 0.0f;
    }
    return Vector3Multiply( velocity, v3_scalar(multiplier) );
}
Vector3 velocity_clamp_horizontal( Vector3 velocity, f32 max ) {
    Vector2 hor = v2( velocity.x, velocity.z );
    hor = Vector2ClampValue( hor, 0.0f, max );

    return v3( hor.x, velocity.y, hor.y );
}

