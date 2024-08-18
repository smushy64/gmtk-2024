#if !defined(SC_GAME_H)
#define SC_GAME_H
/**
 * @file   sc_game.h
 * @brief  Scene Game.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"
#include "physics.h"

#define CAMERA_OFFSET v3( 0.0f, 1.8f, -3.0f )
#define CAMERA_TARGET_OFFSET v3( 0.0f, 0.8f, 0.0f )

#define PLAYER_MOVE_SPEED 2.0f
#define PLAYER_DRAG 2.5f
#define PLAYER_MAX_VELOCITY 3.0f

#define GRAVITY_SCALE 2.0f
#define GRAVITY -9.8f
#define ADDED_GRAVITY -0.12f

#define PLAYER_CAPSULE_HEIGHT (1.0f)
#define PLAYER_CAPSULE_RADIUS (0.25f)

#define PLAYER_GROUND_CHECK_OFFSET v3( 0.0f, 0.25f, 0.0f )
#define PLAYER_GROUND_CHECK_DIST   (0.3f)

#define PLAYER_JUMP_FORCE (12.5f)

struct json_value_s;

enum LevelObjectType {
    LOT_NULL,
    LOT_STATIC,
    LOT_RESIZE,
};
struct LevelObject {
    enum LevelObjectType type;
    union {
        struct LevelObjectStatic {
            Model   geo;
            Model   col;
            Vector3 offset;

            b32 has_col;
        } t_static;
        struct LevelObjectResize {
            Model   geo;
            Model   col;
            Vector3 offset;

            Vector3 size_start;
            Vector3 size_end;

            // not in json
            Vector3 size;
        } t_resize;
        struct {
        } t_music;
    };
};
struct Level {
    struct LevelObject* objects;
    usize object_count;
};

#define RESIZE_TIME (1.0f)

struct SceneGame {
    struct Player {
        Transform transform;
        Vector3   velocity;

        struct Capsule capsule;

        struct Input {
            b32 jump;
            b32 jump_hold;
            b32 is_moving;
            Vector2 move;
            Vector2 rotation;
        } input;

        Vector2 camera_rotation;

        b32 is_grounded;
        struct CollisionResult level_collision;
        b32 corrected_collision;
#if defined(DEBUG)
        b32 ground[4];
        struct CollisionResult last_level_collision;
#endif
    } player;

    Model model_player;

    b32 last_resize_enabled;
    b32 resize_enabled;
    f32 resize_timer;
    b32 resize_complete;

    u32 current_level;
    struct Level level;

    Camera3D camera;
};

void scene_game_load( struct SceneGame* out_state );
void scene_game_unload( struct SceneGame* state );
void scene_game_update( f32 dt, struct SceneGame* state );
void scene_game_draw( f32 dt, struct SceneGame* state );

#endif /* header guard */
