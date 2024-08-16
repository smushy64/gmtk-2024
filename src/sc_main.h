#if !defined(SC_MAIN_H)
#define SC_MAIN_H
/**
 * @file   sc_main.h
 * @brief  Scene Main Menu.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "common.h"
#include "raylib.h"

struct SceneMain {
};

void scene_main_load( struct SceneMain* out_state );
void scene_main_unload( struct SceneMain* state );
void scene_main_update( f32 dt, struct SceneMain* state );
void scene_main_draw( f32 dt, struct SceneMain* state );

#endif /* header guard */
