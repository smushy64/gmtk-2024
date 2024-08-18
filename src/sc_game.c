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
#include <stdlib.h>
#include "gui.h"
#include "entry.h"
#include "mathex.h"
#include "physics.h"
#include "debug.h"
// IWYU pragma: end_keep
#include "json.h"

b32 string_cmp( struct json_string_s* a, struct json_string_s* b ) {
    if( a->string_size != b->string_size ) {
        return false;
    }
    return memcmp( a->string, b->string, a->string_size ) == 0;
}

void player_init( struct Player* out_player );
void player_physics( struct Player* player, struct SceneGame* scene, f32 dt );
void input_read( struct Input* out_input );

struct json_object_element_s*
find_item( struct json_object_s* object, const char* name ) {
    struct json_string_s name_s;
    name_s.string      = name;
    name_s.string_size = strlen( name );

    struct json_object_element_s* elem = object->start;
    for( ;; ) {
        if( !elem ) {
            break;
        }

        if( string_cmp( elem->name, &name_s ) ) {
            return elem;
        }

        elem = elem->next;
    }

    return NULL;
}

void level_unload( struct Level* level ) {
    for( usize i = 0; i < level->object_count; ++i ) {
        struct LevelObject* obj = level->objects + i;
        switch( obj->type ) {
            case LOT_NULL: break;
            case LOT_STATIC: {
                if( obj->t_static.has_geo ) {
                    UnloadModel( obj->t_static.geo );
                }
                if( obj->t_static.has_col ) {
                    UnloadModel( obj->t_static.col );
                }
            } break;
        }
    }

    MemFree( level->objects );
    memset( level, 0, sizeof(*level) );
}
b32 level_load( struct SceneGame* game, u32 level ) {
    const char* path = TextFormat( "resources/level_%02u.json", level );
    TraceLog( LOG_DEBUG, "Loading level %s . . .", path );
    if( !FileExists( path ) ) {
        TraceLog( LOG_WARNING, "Level %s not found!", path );
        return false;
    }

    int data_size = 0;
    unsigned char* file_data = LoadFileData( path, &data_size );

    struct json_value_s* json = json_parse( file_data, data_size );

    UnloadFileData( file_data );

    if( !json ) {
        TraceLog( LOG_FATAL, "Failed to parse level!" );
        __builtin_trap();
        return false;
    }

    struct json_object_s* root = json->payload;
    
    game->level.object_count = root->length;
    game->level.objects      =
        MemAlloc( sizeof(struct LevelObject) * game->level.object_count );
    u32 lot_i = 0;

    struct json_object_element_s* current = root->start;
    for( ;; ) {
        if( !current ) {
            break;
        }

        if( current->value->type != json_type_object ) {
            // invalid object
            continue;
        }

        struct json_object_s* obj = current->value->payload;

        struct json_object_element_s* type_elem = find_item( obj, "type" );

        if( type_elem->value->type != json_type_number ) {
            continue;
        }
        struct json_number_s* type_as_num = type_elem->value->payload;

        enum LevelObjectType type = atoi( type_as_num->number );

        switch( type ) {
            case LOT_NULL: continue;
            case LOT_STATIC: {
                struct LevelObject* lot = game->level.objects + lot_i++;
                lot->type = type;

                struct json_object_element_s* elem =
                    find_item( obj, "geo_path" );
                if( elem && elem->value->type == json_type_string ) {
                    struct json_string_s* path = elem->value->payload;
                    lot->t_static.geo          = LoadModel( path->string );
                    lot->t_static.has_geo      = true;
                }

                elem = find_item( obj, "col_path" );
                if( elem && elem->value->type == json_type_string ) {
                    struct json_string_s* path = elem->value->payload;
                    lot->t_static.col          = LoadModel( path->string );
                    lot->t_static.has_col      = true;
                }

                f32 offset[3];
                memset( offset, 0, sizeof(offset) );
                elem = find_item( obj, "offset" );
                if( elem && elem->value->type == json_type_array ) {
                    struct json_array_s* array = elem->value->payload;
                    u32 max = 3;
                    if( array->length < max ) {
                        max = array->length;
                    }
                    for( u32 array_i = 0; array_i < max; ++array_i ) {
                        struct json_array_element_s* array_elem =
                            array->start + array_i;
                        if( array_elem->value->type != json_type_number ) {
                            break;
                        }

                        struct json_number_s* array_num = array_elem->value->payload;

                        offset[array_i] = atof( array_num->number );
                    }
                }

                memcpy( &lot->t_static.offset, offset, sizeof(offset) );

            } break;
        }
        current = current->next;
    }

    game->level.object_count = lot_i;

    free(json);
    return true;
}
void level_load_next( struct SceneGame* game ) {
    if( !level_load( game, game->current_level + 1 ) ) {
        level_load( game, 0 );
    }
}

void scene_game_load( struct SceneGame* out_state ) {
    unused( out_state );
    level_load( out_state, 0 );
    player_init( &out_state->player );

    out_state->camera.fovy       = 60.0f;
    out_state->camera.projection = CAMERA_PERSPECTIVE;
    out_state->camera.up         = v3_up();

    /* out_state->m_level_geometry = LoadModel("resources/mesh/level/level01_geo.glb"); */
    /* out_state->m_level_collision = LoadModel("resources/mesh/level/level01_col.glb"); */
    /**/
}
void scene_game_unload( struct SceneGame* state ) {
    level_unload( &state->level );
    /* UnloadModel( state->m_level_geometry ); */
    /* UnloadModel( state->m_level_collision ); */
}
void scene_game_update( f32 dt, struct SceneGame* state ) {
    unused(dt, state);

    struct Player* player = &state->player;

    if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
        DisableCursor();
    }
    input_read( &player->input );

#if defined(DEBUG)
    if( IsKeyPressed( KEY_R ) ) {
        memset( &player->transform, 0, sizeof(player->transform ) );
        player->velocity = v3_zero();
    }
#endif

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

    Vector3 camera_target =
        Vector3Add( player->transform.translation, CAMERA_TARGET_OFFSET );
    state->camera.target = 
        v3_lerp( state->camera.target, camera_target, dt * 8.0f );

    Matrix camera_look_at =
        MatrixLookAt( state->camera.position, state->camera.target, state->camera.up );
    camera_look_at = MatrixInvert( camera_look_at );
    Quaternion camera_look_at_rotation =
        QuaternionFromMatrix( camera_look_at );

    Vector3 player_move = v3_zero();
    Vector3 camera_right =
        Vector3RotateByQuaternion(
            v3_right(), camera_look_at_rotation );
    Vector3 camera_forward =
        Vector3CrossProduct( v3_up(), camera_right );

    Vector3 move_right = Vector3Multiply(
        camera_right, v3_scalar(-player->input.move.x) );
    Vector3 move_forward = Vector3Multiply(
        camera_forward, v3_scalar(player->input.move.y));
    player_move = Vector3Add( move_right, move_forward );

    if( player->input.is_moving ) {
        Quaternion target_player_rotation = QuaternionFromVector3ToVector3(
            v3_forward(),
            player_move );
        player->transform.rotation = QuaternionSlerp(
            player->transform.rotation, target_player_rotation, dt * 10.0f );
    }

    if( player->is_grounded ) {
        if( player->input.jump ) {
            player_move.y += PLAYER_JUMP_FORCE;
        }
    }

    if( player->is_grounded && player->input.jump ) {
    }

    player->velocity = Vector3Add( player->velocity, player_move );

    player->capsule.start = player->transform.translation;
    player->capsule.end   =
        Vector3Add( player->capsule.start, v3( 0.0f, PLAYER_CAPSULE_HEIGHT, 0.0f ));

    player->capsule.radius = PLAYER_CAPSULE_RADIUS;

    player_physics( player, state, dt );
}
void scene_game_draw( f32 dt, struct SceneGame* state ) {
    unused(dt, state);
    ClearBackground( (Color){ 102, 191, 255, 255 } );

    struct Player* player = &state->player;
    unused(player);

    BeginMode3D( state->camera );

    for( usize i = 0; i < state->level.object_count; ++i ) {
        struct LevelObject* obj = state->level.objects + i;
        switch( obj->type ) {
            case LOT_NULL: continue;
            case LOT_STATIC: {
                if( obj->t_static.has_geo ) {
                    DrawModel( obj->t_static.geo, obj->t_static.offset, 1.0f, WHITE );
                }
            } break;
        }
    }

#if defined(DEBUG)
    /*debug*/ {

        Vector3 player_forward =
            Vector3RotateByQuaternion( v3_forward(), player->transform.rotation );
        Vector3 forward_line_start = Vector3Add(
            player->transform.translation, CAMERA_TARGET_OFFSET );
        Vector3 forward_line_end   = Vector3Add( forward_line_start, player_forward );
        DrawLine3D( forward_line_start, forward_line_end, BLUE );


        Vector3 ground_check_start, ground_check_end;

        ground_check_start = Vector3Add( 
            player->transform.translation, Vector3Add( Vector3Multiply( v3_forward(), v3_scalar(PLAYER_CAPSULE_RADIUS) ), PLAYER_GROUND_CHECK_OFFSET ) );
        ground_check_end = Vector3Add(
            ground_check_start,
            Vector3Multiply( v3_down(), v3_scalar( PLAYER_GROUND_CHECK_DIST ) ) );

        DrawLine3D( ground_check_start, ground_check_end, player->ground[0] ? RED : GREEN );

        ground_check_start = Vector3Add( 
            player->transform.translation, Vector3Add( Vector3Multiply( v3_back(), v3_scalar(PLAYER_CAPSULE_RADIUS) ), PLAYER_GROUND_CHECK_OFFSET ) );
        ground_check_end = Vector3Add(
            ground_check_start,
            Vector3Multiply( v3_down(), v3_scalar( PLAYER_GROUND_CHECK_DIST ) ) );

        DrawLine3D( ground_check_start, ground_check_end, player->ground[1] ? RED : GREEN );

        ground_check_start = Vector3Add( 
            player->transform.translation, Vector3Add( Vector3Multiply( v3_left(), v3_scalar(PLAYER_CAPSULE_RADIUS) ), PLAYER_GROUND_CHECK_OFFSET ) );
        ground_check_end = Vector3Add(
            ground_check_start,
            Vector3Multiply( v3_down(), v3_scalar( PLAYER_GROUND_CHECK_DIST ) ) );

        DrawLine3D( ground_check_start, ground_check_end, player->ground[2] ? RED : GREEN );

        ground_check_start = Vector3Add( 
            player->transform.translation, Vector3Add( Vector3Multiply( v3_right(), v3_scalar(PLAYER_CAPSULE_RADIUS) ), PLAYER_GROUND_CHECK_OFFSET ) );
        ground_check_end = Vector3Add(
            ground_check_start,
            Vector3Multiply( v3_down(), v3_scalar( PLAYER_GROUND_CHECK_DIST ) ) );

        DrawLine3D( ground_check_start, ground_check_end, player->ground[3] ? RED : GREEN );

        Vector3 start =
            Vector3Add( player->capsule.start, v3( 0.0f, PLAYER_CAPSULE_RADIUS, 0.0f ) );
        Vector3 end =
            Vector3Subtract( player->capsule.end, v3( 0.0f, PLAYER_CAPSULE_RADIUS, 0.0f ) );

        DrawCapsuleWires(
            start, end,
            player->capsule.radius, 8, 4, player->level_collision.hit ? RED : GREEN );

        for( usize i = 0; i < state->level.object_count; ++i ) {
            struct LevelObject* obj = state->level.objects + i;
            switch( obj->type ) {
                case LOT_NULL: continue;
                case LOT_STATIC: {
                    if( obj->t_static.has_col ) {
                        DrawModelWires(
                            obj->t_static.col, obj->t_static.offset, 1.0f, GREEN );
                        BoundingBox level_collision_bound =
                            GetMeshBoundingBox( obj->t_static.col.meshes[0] );
                        DrawBoundingBox( level_collision_bound, GREEN );
                    }
                } break;
            }
        }

        if( player->last_level_collision.hit ) {
            debug_draw_point( player->last_level_collision.point, 0.2f, RED );
            DrawLine3D( player->last_level_collision.point,
                Vector3Add( player->last_level_collision.point,
                    player->last_level_collision.normal ), YELLOW );
            DrawLine3D( player->last_level_collision.point,
                Vector3Add( player->last_level_collision.point,
                    Vector3Multiply(player->last_level_collision.normal,
                        v3_scalar(player->last_level_collision.distance) ) ), MAGENTA);
        }

    }
#endif

    EndMode3D();

#if defined(DEBUG)
    /*debug*/ {
        Color col = RAYWHITE;
        col.a = 127;
        Font font = font_text();
        gui_text_draw(
            font, TextFormat( "%i FPS", GetFPS() ),
            v2( 0.0f, 0.0f ),
            TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_START,
            col );
        gui_text_draw(
            font, TextFormat("Position: %.2f, %.2f, %.2f", player->transform.translation.x, player->transform.translation.y, player->transform.translation.z ),
            v2( 0.0f, TEXT_FONT_SIZE_SMALLEST ),
            TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_START,
            col);
        gui_text_draw(
            font, TextFormat("Velocity: %.2f, %.2f, %.2f", player->velocity.x, player->velocity.y, player->velocity.z ),
            v2( 0.0f, TEXT_FONT_SIZE_SMALLEST * 2 ),
            TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_START,
            col);
    }
#endif

}

void player_init( struct Player* player ) {
    memset( player, 0, sizeof(*player) );
    player->transform.rotation = QuaternionIdentity();
}
void player_physics( struct Player* player, struct SceneGame* scene, f32 dt ) {
    unused(scene);

    player->velocity =
        velocity_clamp_horizontal( player->velocity, PLAYER_MAX_VELOCITY );

    u64 frames = frames_elapsed();

    if( frames % 2 == 0 ) {
        struct CollisionResult level_collision;
        memset( &level_collision, 0, sizeof( level_collision ) );

        for( usize i = 0; i < scene->level.object_count; ++i ) {
            struct LevelObject* obj = scene->level.objects + i;
            switch( obj->type ) {
                case LOT_NULL: continue;
                case LOT_STATIC: {
                    if( !obj->t_static.has_col ) {
                        continue;
                    }
                    Matrix level_mat =
                        MatrixTranslate(
                            obj->t_static.offset.x,
                            obj->t_static.offset.y,
                            obj->t_static.offset.z );

                    level_collision = collision_capsule_mesh(
                        player->capsule.start, player->capsule.end,
                        player->capsule.radius, level_mat,
                        obj->t_static.col.meshes[0] );

                    if( level_collision.hit ) {
                        goto exit_collision_check;
                    }
                } break;
            }
        }
    exit_collision_check:

        if( level_collision.hit ) {
#if defined(DEBUG)
            player->last_level_collision = level_collision;
#endif
        }
        player->level_collision = level_collision;

    }
    b32 ground[4];
    memset( ground, 0, sizeof(ground) );
    RayCollision ray_collision;
    memset( &ray_collision, 0, sizeof(ray_collision) );
    Ray ray;
    ray.direction = v3_down();

    for( usize i = 0; i < scene->level.object_count; ++i ) {
        struct LevelObject* obj = scene->level.objects + i;
        Mesh col;
        Matrix transform;
        switch( obj->type ) {
            case LOT_NULL: continue;
            case LOT_STATIC: {
                if( !obj->t_static.has_col ) {
                    continue;
                }
                col = obj->t_static.col.meshes[0];
                transform = MatrixTranslate(
                    obj->t_static.offset.x,
                    obj->t_static.offset.y,
                    obj->t_static.offset.z );
            } break;
        }

        ray.position  = Vector3Add(
            player->transform.translation,
            Vector3Add( Vector3Multiply( v3_forward(), v3_scalar( PLAYER_CAPSULE_RADIUS )), PLAYER_GROUND_CHECK_OFFSET ));
        ray_collision = GetRayCollisionMesh( 
            ray, col, transform );
        ground[0] =
            ray_collision.hit && ray_collision.distance <= PLAYER_GROUND_CHECK_DIST;

        ray.position  = Vector3Add(
            player->transform.translation,
            Vector3Add( Vector3Multiply( v3_back(), v3_scalar( PLAYER_CAPSULE_RADIUS )), PLAYER_GROUND_CHECK_OFFSET ));
        ray_collision = GetRayCollisionMesh( 
            ray, col, transform );
        ground[1] =
            ray_collision.hit && ray_collision.distance <= PLAYER_GROUND_CHECK_DIST;

        ray.position  = Vector3Add(
            player->transform.translation,
            Vector3Add( Vector3Multiply( v3_left(), v3_scalar( PLAYER_CAPSULE_RADIUS )), PLAYER_GROUND_CHECK_OFFSET ));
        ray_collision = GetRayCollisionMesh( 
            ray, col, transform );
        ground[2] =
            ray_collision.hit && ray_collision.distance <= PLAYER_GROUND_CHECK_DIST;

        ray.position  = Vector3Add(
            player->transform.translation,
            Vector3Add( Vector3Multiply( v3_right(), v3_scalar( PLAYER_CAPSULE_RADIUS )), PLAYER_GROUND_CHECK_OFFSET ));
        ray_collision = GetRayCollisionMesh( 
            ray, col, transform );
        ground[3] =
            ray_collision.hit && ray_collision.distance <= PLAYER_GROUND_CHECK_DIST;

        player->is_grounded = ground[0] || ground[1] || ground[2] || ground[3];
#if defined(DEBUG)
        memcpy( player->ground, ground, sizeof(ground) );
#endif
        if( player->is_grounded ) {
            break;
        }
    }

    if( !player->is_grounded ) {
        f32 gravity = GRAVITY_SCALE * GRAVITY * dt;
        player->velocity = Vector3Add( player->velocity, v3( 0, gravity, 0 ) );
    }

    if( player->level_collision.hit ) {
        f32 velocity_scale = Vector3Length( player->velocity );
        Vector3 velocity_normal;

        if( velocity_scale == 0.0f ) {
            velocity_normal = v3_zero();
        } else {
            velocity_normal =
                Vector3Divide( player->velocity, v3_scalar(velocity_scale) );
        }

        f32 dot = Vector3DotProduct( velocity_normal, player->level_collision.normal );
        Vector3 unwanted_motion =
            Vector3Multiply( player->level_collision.normal, v3_scalar(dot) );
        Vector3 wanted_motion = Vector3Subtract( velocity_normal, unwanted_motion );
        player->velocity =
            Vector3Multiply( wanted_motion, v3_scalar( velocity_scale ) );

        f32 penetration_depth = (player->level_collision.distance) + EPSILON;
        Vector3 penetration_translate =
            Vector3Multiply(
                player->level_collision.normal, v3_scalar(penetration_depth) );
        player->velocity =
            Vector3Add( player->velocity, penetration_translate );
    }

    player->transform.translation = Vector3Add(
        player->transform.translation,
        Vector3Multiply( player->velocity, v3_scalar(dt) ) );

    player->velocity = velocity_apply_drag( player->velocity, PLAYER_DRAG, dt );
}
void input_read( struct Input* input ) {
    memset( input, 0, sizeof(*input) );

    int right = IsKeyDown( KEY_D );
    int left  = IsKeyDown( KEY_A );
    int forw  = IsKeyDown( KEY_W );
    int back  = IsKeyDown( KEY_S );

    input->is_moving = right || left || forw || back;

    input->move.x = left - right;
    input->move.y = forw  - back;

    input->move = Vector2ClampValue( input->move, 0.0f, 1.0f );

    input->rotation = GetMouseDelta();

    input->jump      = IsKeyPressed( KEY_SPACE );
    input->jump_hold = IsKeyDown( KEY_SPACE );
}
