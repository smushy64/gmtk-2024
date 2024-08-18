/**
 * @file   physics.c
 * @brief  Physics.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 17, 2024
*/
#include "physics.h"
#include "mathex.h"
// IWYU pragma: begin_keep
#include <string.h>
// IWYU pragma: end_keep

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

f32 saturate( f32 f ) {
    return fminf( fmaxf( f, 0.0f ), 1.0f );
}

Vector3 closest_point_on_line_segment( Vector3 a, Vector3 b, Vector3 point ) {
    Vector3 ab = Vector3Subtract( b, a );
    f32     t  = Vector3DotProduct( Vector3Subtract( point, a ), ab );
    f32  sat_t = saturate( t );

    return Vector3Add( a, Vector3Multiply( v3_scalar(sat_t), ab ));
}

struct CollisionResult collision_sphere_triangle(
    Vector3 position, f32 radius,
    Vector3 p0, Vector3 p1, Vector3 p2
) {
    struct CollisionResult result;
    memset( &result, 0, sizeof(result) );

    Vector3 plane_normal = Vector3Normalize(
        Vector3CrossProduct(
            Vector3Subtract( p1, p0 ),
            Vector3Subtract( p2, p0 ) ) );

    f32 signed_distance = Vector3DotProduct( 
        Vector3Subtract( position, p0 ), plane_normal );

    if( signed_distance > 0.0f ) {
        return result;
    }

    if( signed_distance < -radius || signed_distance > radius ) {
        return result;
    }

    Vector3 point0 =
        Vector3Subtract( position,
            Vector3Multiply( plane_normal, v3_scalar(signed_distance)) );

    Vector3 c0 = Vector3CrossProduct(
        Vector3Subtract( point0, p0 ), Vector3Subtract( p1, p0 ) );
    Vector3 c1 = Vector3CrossProduct(
        Vector3Subtract( point0, p1 ), Vector3Subtract( p2, p1 ) );
    Vector3 c2 = Vector3CrossProduct(
        Vector3Subtract( point0, p2 ), Vector3Subtract( p0, p2 ) );

    f32 c0_dot = Vector3DotProduct( c0, plane_normal );
    f32 c1_dot = Vector3DotProduct( c1, plane_normal );
    f32 c2_dot = Vector3DotProduct( c2, plane_normal );

    b32 inside = c0_dot <= 0.0f && c1_dot <= 0.0f && c2_dot <= 0.0f;

    f32 radius_sqr = radius * radius;

    Vector3 point1 = closest_point_on_line_segment( p0, p1, position );
    Vector3 v1     = Vector3Subtract( position, point1 );
    f32     distance_sqr_1 = Vector3DotProduct( v1, v1 );
    b32 intersects = distance_sqr_1 < radius_sqr;

    Vector3 point2 = closest_point_on_line_segment( p1, p2, position );
    Vector3 v2     = Vector3Subtract( position, point2 );
    f32     distance_sqr_2 = Vector3DotProduct( v2, v2 );
    intersects |= distance_sqr_2 < radius_sqr;

    Vector3 point3 = closest_point_on_line_segment( p2, p0, position );
    Vector3 v3     = Vector3Subtract( position, point3 );
    f32     distance_sqr_3 = Vector3DotProduct( v3, v3 );
    intersects |= distance_sqr_3 < radius_sqr;

    if( inside || intersects ) {
        Vector3 best_point = point0;
        Vector3 intersection;

        if( inside ) {
            intersection = Vector3Subtract( position, point0 );
        } else {
            Vector3 d             = Vector3Subtract( position, point1 );
            f32 best_distance_sqr = Vector3DotProduct( d, d );
            best_point            = point1;
            intersection          = d;

            d = Vector3Subtract( position, point2 );
            f32 distance_sqr = Vector3DotProduct( d, d );

            if( distance_sqr < best_distance_sqr ) {
                distance_sqr = best_distance_sqr;
                best_point   = point2;
                intersection = d;
            }

            d = Vector3Subtract( position, point3 );
            distance_sqr = Vector3DotProduct( d, d );
            if( distance_sqr < best_distance_sqr ) {
                distance_sqr = best_distance_sqr;
                best_point   = point3;
                intersection = d;
            }
        }

        f32 length = Vector3Length( intersection );
        Vector3 penetration_normal = Vector3Divide( intersection, v3_scalar(length) );
        if( length == 0.0f ) {
            penetration_normal = v3_zero();
        }
        f32 penetration_depth = radius - length;

        result.hit      = true;
        result.distance = penetration_depth;
        result.point    = best_point;
        /* result.normal   = penetration_normal; */
        result.normal   = Vector3Negate( penetration_normal );
    }

    return result;
}

struct CollisionResult collision_capsule_capsule(
    Vector3 cap_start0, Vector3 cap_end0, f32 radius0,
    Vector3 cap_start1, Vector3 cap_end1, f32 radius1
) {
    Vector3 normal_0 = Vector3Normalize( Vector3Subtract( cap_end0, cap_start0 ) );
    Vector3 line_end_offset_0 = Vector3Multiply( normal_0, v3_scalar( radius0 ) );
    Vector3 a_0 = Vector3Add( cap_start0, line_end_offset_0 );
    Vector3 b_0 = Vector3Subtract( cap_end0, line_end_offset_0 );

    Vector3 normal_1 = Vector3Normalize( Vector3Subtract( cap_end1, cap_start1 ) );
    Vector3 line_end_offset_1 = Vector3Multiply( normal_1, v3_scalar( radius1 ) );
    Vector3 a_1 = Vector3Add( cap_start1, line_end_offset_1 );
    Vector3 b_1 = Vector3Subtract( cap_end1, line_end_offset_1 );

    Vector3 v0 = Vector3Subtract( a_1, a_0 );
    Vector3 v1 = Vector3Subtract( b_1, a_0 );
    Vector3 v2 = Vector3Subtract( a_1, b_0 );
    Vector3 v3 = Vector3Subtract( b_1, b_0 );

    f32 d0 = Vector3DotProduct( v0, v0 );
    f32 d1 = Vector3DotProduct( v1, v1 );
    f32 d2 = Vector3DotProduct( v2, v2 );
    f32 d3 = Vector3DotProduct( v3, v3 );

    Vector3 best_a;
    if( d2 < d0 || d2 < d1 || d3 < d0 || d3 < d1 ) {
        best_a = b_0;
    } else {
        best_a = a_0;
    }

    Vector3 best_b = closest_point_on_line_segment( a_1, b_1, best_a );
    best_a = closest_point_on_line_segment( a_0, b_0, best_b );

    Vector3 penetration_normal = Vector3Subtract( best_a, best_b );
    f32 len = Vector3Length( penetration_normal );
    if( len ) {
        penetration_normal = Vector3Divide( penetration_normal, v3_scalar(len) );
    } else {
        penetration_normal = v3_zero();
    }
    f32 depth = radius0 + radius1 - len;
    b32 intersects = depth > 0.0f;

    struct CollisionResult result;
    memset( &result, 0, sizeof(result) );
    if( !intersects ) {
        return result;
    }

    result.hit      = intersects;
    result.normal   = penetration_normal;
    result.distance = depth;
    result.point    = v3_lerp( best_a, best_b, 0.5f );

    return result;
}

struct CollisionResult collision_capsule_triangle(
    Vector3 cap_start, Vector3 cap_end, f32 radius,
    Vector3 p0, Vector3 p1, Vector3 p2
) {
    struct CollisionResult result;
    memset( &result, 0, sizeof(result) );

    Vector3 capsule_normal =
        Vector3Normalize( Vector3Subtract( cap_end, cap_start ) );
    Vector3 line_end_offset = Vector3Multiply( capsule_normal, v3_scalar( radius ) );

    Vector3 a = Vector3Add( cap_start, line_end_offset );
    Vector3 b = Vector3Subtract( cap_end, line_end_offset );

    Vector3 plane_normal = Vector3Normalize(
        Vector3CrossProduct( Vector3Subtract( p1, p0 ), Vector3Subtract( p2, p0 ) ) );

    f32 t = Vector3DotProduct(
        plane_normal,
        Vector3Divide(
            Vector3Subtract( p0, cap_start ),
            v3_scalar( absf( Vector3DotProduct( plane_normal, capsule_normal )))) );
    Vector3 line_plane_intersection =
        Vector3Add( cap_start, Vector3Multiply( capsule_normal, v3_scalar(t)) );

    Vector3 ref_point; {
        Vector3 c0 = Vector3CrossProduct(
            Vector3Subtract( line_plane_intersection, p0 ), Vector3Subtract( p1, p0 ));
        Vector3 c1 = Vector3CrossProduct(
            Vector3Subtract( line_plane_intersection, p1 ), Vector3Subtract( p2, p1 ));
        Vector3 c2 = Vector3CrossProduct(
            Vector3Subtract( line_plane_intersection, p2 ), Vector3Subtract( p0, p2 ));

        b32 inside =
            Vector3DotProduct( c0, plane_normal ) <= 0.0f &&
            Vector3DotProduct( c1, plane_normal ) <= 0.0f &&
            Vector3DotProduct( c2, plane_normal ) <= 0.0f;

        if( inside ) {
            ref_point = line_plane_intersection;
        } else {
            Vector3 point1   = closest_point_on_line_segment(
                p0, p1, line_plane_intersection );
            Vector3 v1       = Vector3Subtract( line_plane_intersection, point1 );
            f32 distance_sqr = Vector3DotProduct( v1, v1 );
            f32 best_dist    = distance_sqr;
            ref_point        = point1;

            Vector3 point2   = closest_point_on_line_segment(
                p1, p2, line_plane_intersection );
            Vector3 v2       = Vector3Subtract( line_plane_intersection, point2 );
            distance_sqr = Vector3DotProduct( v2, v2 );

            if( distance_sqr < best_dist ) {
                ref_point = point2;
                best_dist = distance_sqr;
            }

            Vector3 point3   = closest_point_on_line_segment(
                p2, p0, line_plane_intersection );
            Vector3 v3       = Vector3Subtract( line_plane_intersection, point3 );
            distance_sqr = Vector3DotProduct( v3, v3 );

            if( distance_sqr < best_dist ) {
                ref_point = point3;
                best_dist = distance_sqr;
            }
        }
    }

    Vector3 center = closest_point_on_line_segment( a, b, ref_point );

    return collision_sphere_triangle( center, radius, p0, p1, p2 );
}
struct CollisionResult collision_capsule_mesh(
    Vector3 cap_start, Vector3 cap_end, f32 radius,
    Matrix mesh_transform, Mesh mesh
) {
    BoundingBox cap_bound;
    f32 half_radius = radius / 2.0f;
    cap_bound.min = Vector3Subtract( cap_start, v3_scalar( half_radius ) );
    cap_bound.max = Vector3Add( cap_end, v3_scalar( half_radius ) );

    BoundingBox mesh_bound = GetMeshBoundingBox( mesh );
    mesh_bound.min = Vector3Transform( mesh_bound.min, mesh_transform );
    mesh_bound.max = Vector3Transform( mesh_bound.max, mesh_transform );

    struct CollisionResult result;
    memset( &result, 0, sizeof(result) );

    if( !CheckCollisionBoxes( cap_bound, mesh_bound ) ) {
        return result;
    }

    const Vector3* vertices = (const Vector3*)mesh.vertices;
    for( int i = 0; i < mesh.triangleCount; ++i ) {
        Vector3 p0, p1, p2;

        if( mesh.indices ) {
            p0 = vertices[mesh.indices[i * 3 + 0]];
            p1 = vertices[mesh.indices[i * 3 + 1]];
            p2 = vertices[mesh.indices[i * 3 + 2]];
        } else {
            p0 = vertices[i * 3 + 0];
            p1 = vertices[i * 3 + 1];
            p2 = vertices[i * 3 + 2];           
        }

        p0 = Vector3Transform( p0, mesh_transform );
        p1 = Vector3Transform( p1, mesh_transform );
        p2 = Vector3Transform( p2, mesh_transform );

        result = collision_capsule_triangle( cap_start, cap_end, radius, p0, p1, p2 );

        if( result.hit ) {
            return result;
        }
    }

    return result;
}

