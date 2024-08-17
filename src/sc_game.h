#if !defined(SC_GAME_H)
#define SC_GAME_H
/**
 * @file   sc_game.h
 * @brief  Scene Game.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"

#define CAMERA_OFFSET v3( 0.0f, 2.0f, -5.0f )

#define PLAYER_MOVE_SPEED 2.0f
#define PLAYER_DRAG 2.0f
#define PLAYER_MAX_VELOCITY 3.0f

#define GRAVITY_SCALE 1.0f
#define GRAVITY -9.0f

struct SceneGame {
    struct Player {
        Transform transform;
        Vector3   velocity;

        struct Input {
            Vector2 move;
            Vector2 rotation;
        } input;

        Vector2 camera_rotation;
        Quaternion camera_rotation_from_look_at;
    } player;

    Model m_level_geometry;
    Model m_level_collision;

    Camera3D camera;
};

void scene_game_load( struct SceneGame* out_state );
void scene_game_unload( struct SceneGame* state );
void scene_game_update( f32 dt, struct SceneGame* state );
void scene_game_draw( f32 dt, struct SceneGame* state );

#endif /* header guard */
