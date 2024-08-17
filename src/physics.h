#if !defined(PHYSICS_H)
#define PHYSICS_H
/**
 * @file   physics.h
 * @brief  Physics.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 17, 2024
*/
#include "common.h"

Vector3 velocity_apply_drag( Vector3 velocity, f32 drag, f32 dt );
Vector3 velocity_clamp_horizontal( Vector3 velocity, f32 max );

#endif /* header guard */
