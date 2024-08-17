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
#include "physics.h"
// IWYU pragma: end_keep

void player_init( struct Player* out_player );
void player_physics( struct Player* player, f32 dt );
void input_read( struct Input* out_input );

void scene_game_load( struct SceneGame* out_state ) {
    unused( out_state );
    player_init( &out_state->player );

    out_state->camera.fovy       = 60.0f;
    out_state->camera.projection = CAMERA_PERSPECTIVE;
    out_state->camera.up         = v3_up();

    out_state->m_level_geometry = LoadModel("resources/mesh/level/level01_geo.glb");
    out_state->m_level_collision = LoadModel("resources/mesh/level/level01_col.glb");
}
void scene_game_unload( struct SceneGame* state ) {
    UnloadModel( state->m_level_geometry );
    UnloadModel( state->m_level_collision );
}
void scene_game_update( f32 dt, struct SceneGame* state ) {
    unused(dt, state);

    struct Player* player = &state->player;

    if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        DisableCursor();
    }
    input_read( &player->input );

    player->camera_rotation.x += 0.2f * ( player->input.rotation.x * dt );
    player->camera_rotation.y += 0.2f * ( -player->input.rotation.y * dt );

    static const f32 max_rot = 80.0f * DEG2RAD;
    player->camera_rotation.y =
        Clamp( player->camera_rotation.y, -max_rot, max_rot );

    Quaternion camera_rotation =
        QuaternionFromEuler(
            player->camera_rotation.y,
            player->camera_rotation.x, 0 );

    Vector3 camera_offset =
        Vector3RotateByQuaternion( CAMERA_OFFSET, camera_rotation );

    state->camera.position =
        Vector3Add( player->transform.translation, camera_offset );
    state->camera.target   = player->transform.translation;

    Matrix camera_look_at =
        MatrixLookAt( state->camera.position, state->camera.target, state->camera.up );
    camera_look_at = MatrixInvert( camera_look_at );
    state->player.camera_rotation_from_look_at =
        QuaternionFromMatrix( camera_look_at );

    Vector3 player_move = v3_zero();
    Vector3 camera_right =
        Vector3RotateByQuaternion(
            v3_right(), state->player.camera_rotation_from_look_at );
    Vector3 camera_forward =
        Vector3CrossProduct( v3_up(), camera_right );

    Vector3 move_right = Vector3Multiply(
        camera_right, v3_scalar(-player->input.move.x) );
    Vector3 move_forward = Vector3Multiply(
        camera_forward, v3_scalar(player->input.move.y));

    player_move = Vector3Add( move_right, move_forward );

    player->velocity = Vector3Add( player->velocity, player_move );

    player_physics( player, dt );

}
void scene_game_draw( f32 dt, struct SceneGame* state ) {
    unused(dt, state);
    ClearBackground( (Color){ 102, 191, 255, 255 } );

    struct Player* player = &state->player;

    BeginMode3D( state->camera );

    DrawModel( state->m_level_geometry, v3_zero(), 1.0f, WHITE );
    DrawModelWires( state->m_level_collision, v3_zero(), 1.0f, GREEN );

    f32 height = 1.0f;
    f32 radius = 0.5f;
    Vector3 cap_start, cap_end;
    cap_start = Vector3Add(
        player->transform.translation, v3( 0.0f, radius / 2.0f, 0.0f ) );
    cap_end = Vector3Subtract(
        Vector3Add( player->transform.translation, v3( 0.0f, height, 0.0f ) ),
        v3( 0.0f, radius / 2.0f, 0.0f ) );
    DrawCapsule(
        cap_start, cap_end,
        radius, 8, 8, RED );

    DrawSphere( player->transform.translation, 0.5f, WHITE );

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
    memset( &player->transform, 0, sizeof(player->transform) );
}
void player_physics( struct Player* player, f32 dt ) {
    player->velocity =
        velocity_clamp_horizontal( player->velocity, PLAYER_MAX_VELOCITY );

    player->transform.translation = Vector3Add(
        player->transform.translation,
        Vector3Multiply( player->velocity, v3_scalar(dt) ) );

    player->velocity = velocity_apply_drag( player->velocity, PLAYER_DRAG, dt );
}
void input_read( struct Input* input ) {
    memset( input, 0, sizeof(*input) );

    input->move.x = IsKeyDown( KEY_A ) - IsKeyDown( KEY_D );
    input->move.y = IsKeyDown( KEY_W ) - IsKeyDown( KEY_S );

    input->move = Vector2ClampValue( input->move, 0.0f, 1.0f );

    input->rotation = GetMouseDelta();
}
