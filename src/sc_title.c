/**
 * @file   sc_title.c
 * @brief  Title scene.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "sc_title.h"
#include "entry.h"
#include <math.h>

void scene_title_load( struct SceneTitle* out_state ) {
    out_state->opacity = 0.0f;
    out_state->gmtk    = LoadTexture( "resources/img/branding/gmtk2024-logo.png");

    volatile b32 ready = false;
    while( !ready ) {
        ready =
            IsTextureReady( out_state->gmtk );
    }
}
void scene_title_unload( struct SceneTitle* state ) {
    UnloadTexture( state->gmtk );
}
f32 inverse_lerp( f32 a, f32 b, f32 v ) {
    return ( v - a ) / ( b - a );
}
void scene_title_update( f32 dt, struct SceneTitle* state ) {
    f32 t = inverse_lerp( 0.0f, SC_TITLE_LIFETIME, time_elapsed() );

    state->opacity = sin(t * PI * (t > 0.5f ? 1.2f : 1.0f));
    if( time_elapsed() >= SC_TITLE_LIFETIME ) {
        scene_load( SC_MAIN );
    }

    unused(dt);
}
void scene_title_draw( f32 dt, struct SceneTitle* state ) {
    f32 screen_width  = GetScreenWidth();
    f32 screen_height = GetScreenHeight();

    static const f32 scale = 0.3f;

    Vector2 pos;
    pos.x = (screen_width  / 2.0f) - ((state->gmtk.width * scale) / 2.0f);
    pos.y = (screen_height / 2.0f) - ((state->gmtk.height * scale) / 2.0f);

    Color col;
    col.r = 255;
    col.g = 255;
    col.b = 255;

    f32 alphaf = 255.0f * state->opacity;
    u8  alpha  = (alphaf > 255.0f ? 255 : (alphaf < 0.0f ? 0 : alphaf) );

    col.a = alpha;

    DrawTextureEx( state->gmtk, pos, 0.0f, scale, col );

    unused(dt);
}

