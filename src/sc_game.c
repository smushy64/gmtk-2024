/**
 * @file   sc_game.c
 * @brief  Scene Game.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"
#include "sc_game.h"
// IWYU pragma: begin_keep
#include <string.h>
#include "gui.h"
#include "entry.h"
#include "mathex.h"
// IWYU pragma: end_keep

void player_init( struct Player* out_player );

void scene_game_load( struct SceneGame* out_state ) {
    unused( out_state );
    player_init( &out_state->player );

    out_state->camera.position   = v3( 0.0f, 1.0f, -2.0f );
    out_state->camera.fovy       = 60.0f;
    out_state->camera.projection = CAMERA_PERSPECTIVE;
    out_state->camera.up         = v3_up();
}
void scene_game_unload( struct SceneGame* state ) {
    unused( state );
}
void scene_game_update( f32 dt, struct SceneGame* state ) {
    unused(dt, state);

    state->camera.target = (Vector3){0};

    Vector3 delta = v3_zero();

    delta.x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
    delta.z = IsKeyDown(KEY_W) - IsKeyDown(KEY_S);

    state->player.position = Vector3Add(
        state->player.position,
        Vector3Multiply( delta, v3_scalar(dt) ) );

}
void scene_game_draw( f32 dt, struct SceneGame* state ) {
    unused(dt, state);
    ClearBackground( BLACK );

    BeginMode3D( state->camera );

    DrawCapsule(
        state->player.position,
        Vector3Add( state->player.position, v3_up() ),
        1.0f, 16, 16, RED );

    EndMode3D();

#if defined(DEBUG)
    /*debug*/ {
        Color col = RAYWHITE;
        col.a = 127;
        gui_text_draw(
            font_text(), TextFormat( "%i FPS", GetFPS() ),
            v2( 0.0f, 0.0f ),
            TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_START,
            col );
    }
#endif

}

void player_init( struct Player* player ) {
    player->position = v3_zero();
}

