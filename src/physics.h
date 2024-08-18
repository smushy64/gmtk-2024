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

struct Capsule {
    Vector3 start;
    Vector3 end;
    f32     radius;
};

struct CollisionResult {
    b32     hit;
    Vector3 normal;
    Vector3 point;
    f32     distance;
};

struct CollisionResult collision_sphere_triangle(
    Vector3 position, f32 radius,
    Vector3 p0, Vector3 p1, Vector3 p2 );

struct CollisionResult collision_capsule_capsule(
    Vector3 cap_start0, Vector3 cap_end0, f32 radius0,
    Vector3 cap_start1, Vector3 cap_end1, f32 radius1 );

struct CollisionResult collision_capsule_triangle(
    Vector3 cap_start, Vector3 cap_end, f32 radius,
    Vector3 p0, Vector3 p1, Vector3 p2 );

struct CollisionResult collision_capsule_mesh(
    Vector3 cap_start, Vector3 cap_end, f32 radius,
    Matrix mesh_transform, Mesh mesh );

#endif /* header guard */
