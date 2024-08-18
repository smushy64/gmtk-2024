/**
 * @file   debug.c
 * @brief  Debugging Vis
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 17, 2024
*/
#include "debug.h"
#include "mathex.h"

void debug_draw_point( Vector3 position, f32 scale, Color color ) {
    unused(position, scale, color);
#if defined(DEBUG)
    Vector3 tl, tr, bl, br;

    Vector3 up, left, down, right;
    up    = Vector3Multiply( v3_up(), v3_scalar( scale ));
    left  = Vector3Multiply( v3_left(), v3_scalar( scale ));
    down  = Vector3Multiply( v3_down(), v3_scalar( scale ));
    right = Vector3Multiply( v3_right(), v3_scalar( scale ));

    tl = Vector3Add( Vector3Add( position, left), up);
    tr = Vector3Add( Vector3Add( position, right), up);
    bl = Vector3Add( Vector3Add( position, left), down);
    br = Vector3Add( Vector3Add( position, right), down);

    DrawLine3D( tl, br, color );
    DrawLine3D( tr, bl, color );
#endif
}
