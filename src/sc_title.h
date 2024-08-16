#if !defined(SC_TITLE_H)
#define SC_TITLE_H
/**
 * @file   sc_title.h
 * @brief  Title scene.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "common.h"
#include "raylib.h"

#define SC_TITLE_LIFETIME 1.25f
struct SceneTitle {
    f32       opacity;
    Texture2D gmtk;
};

void scene_title_load( struct SceneTitle* out_state );
void scene_title_unload( struct SceneTitle* state );
void scene_title_update( f32 dt, struct SceneTitle* state );
void scene_title_draw( f32 dt, struct SceneTitle* state );

#endif /* header guard */
