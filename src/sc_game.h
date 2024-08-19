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

#define PLAYER_MOVE_SPEED 3.0f
#define PLAYER_DRAG 2.5f
#define PLAYER_MAX_VELOCITY 3.0f
#define PLAYER_MAX_VELOCITY_RUN 5.0f

#define GRAVITY_SCALE 2.0f
#define GRAVITY -9.8f
#define ADDED_GRAVITY -0.12f

#define PLAYER_CAPSULE_HEIGHT (1.0f)
#define PLAYER_CAPSULE_RADIUS (0.25f)

#define PLAYER_GROUND_CHECK_OFFSET v3( 0.0f, 0.25f, 0.0f )
#define PLAYER_GROUND_CHECK_DIST   (0.3f)

#define PLAYER_JUMP_FORCE (6.5f)

#define KILL_PLANE (-20.0f)

#define DEAD_TIME (3.0f)

struct json_value_s;

#define PLAYER_IDLE 1
#define PLAYER_WALK 2
#define PLAYER_RUN  3
#define PLAYER_FALL 4

#define ANIM_FT (0.0166f)

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

            b32 has_geo;
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
            b32 geo_from_state;
            b32 col_from_state;
        } t_resize;
    };
};
struct Level {
    struct LevelObject* objects;
    usize object_count;
    Vector3 level_finish;
};

#define RESIZE_TIME (0.2f)
#define RESIZE_ON_TIME (1.4f)

struct SceneGame {
    struct Player {
        Transform transform;
        Vector3   velocity;

        struct Capsule capsule;

        struct Input {
            b32 jump;
            b32 jump_hold;
            b32 is_moving;
            b32 run_hold;
            Vector2 move;
            Vector2 rotation;
            b32 resize_hold;
        } input;

        Vector2 camera_rotation;

        b32 is_dead;
        b32 won;
        b32 is_running;
        f32 max_velocity;
        b32 is_grounded;
        struct CollisionResult level_collision;
        b32 corrected_collision;

#if defined(DEBUG)
        b32 ground[4];
        struct CollisionResult last_level_collision;
#endif
    } player;

    b32 last_dead;

    Music music;
    Music music_game_over;

    Sound sfx_jump;
    Sound sfx_resize;

    Model model_player;
    Texture tx_player_main;
    Texture tx_player_hair;
    Texture tx_player_eyes;
    Texture tx_player_mouth;

    Model platform1;
    Model platform2;

    ModelAnimation* player_anim;
    int player_anim_count;

    u32 anim_frame;
    f32 anim_timer;

    f32 resize_allowed_timer;
    b32 resize_banned;
    
    b32 last_resize_enabled;
    b32 resize_enabled;
    f32 resize_timer;
    b32 resize_reverse;

    f32 dead_timer;
    f32 won_timer;

    u32 current_level;
    struct Level level;

    int current_animation;

    Camera3D camera;
};

void scene_game_load( struct SceneGame* out_state );
void scene_game_unload( struct SceneGame* state );
void scene_game_update( f32 dt, struct SceneGame* state );
void scene_game_draw( f32 dt, struct SceneGame* state );

#endif /* header guard */
