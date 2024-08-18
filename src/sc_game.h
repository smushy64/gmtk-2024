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

#define CAMERA_OFFSET v3( 0.0f, 2.0f, -5.0f )
#define CAMERA_TARGET_OFFSET v3( 0.0f, 1.0f, 0.0f )

#define PLAYER_MOVE_SPEED 2.0f
#define PLAYER_DRAG 2.0f
#define PLAYER_MAX_VELOCITY 3.0f

#define GRAVITY_SCALE 1.0f
#define GRAVITY -9.8f

#define PLAYER_CAPSULE_HEIGHT (1.0f)
#define PLAYER_CAPSULE_RADIUS (0.25f)

#define PLAYER_GROUND_CHECK_OFFSET v3( 0.0f, 0.25f, 0.0f )
#define PLAYER_GROUND_CHECK_DIST   (0.3f)

#define PLAYER_JUMP_FORCE (6.0f)

struct json_value_s;

enum LevelObjectType {
    LOT_NULL,
    LOT_STATIC,
    LOT_GROW,
    LOT_SHRINK
};
struct LevelObject {
    enum LevelObjectType type;
    union {
        struct {
            Model   geo;
            Model   col;
            Vector3 offset;

            // not explicitly from json

            b32 has_geo;
            b32 has_col;
        } t_static;
    };
};
struct Level {
    struct LevelObject* objects;
    usize object_count;
};

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
#if defined(DEBUG)
        b32 ground[4];
        struct CollisionResult last_level_collision;
#endif
    } player;

    u32 current_level;
    struct Level level;

    /* Model m_level_geometry; */
    /* Model m_level_collision; */
    /**/
    Camera3D camera;
};

void scene_game_load( struct SceneGame* out_state );
void scene_game_unload( struct SceneGame* state );
void scene_game_update( f32 dt, struct SceneGame* state );
void scene_game_draw( f32 dt, struct SceneGame* state );

#endif /* header guard */
