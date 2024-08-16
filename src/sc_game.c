/**
 * @file   sc_game.c
 * @brief  Scene Game.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"
#include "sc_game.h"
#include "gui.h"
#include "entry.h"
#include "mathex.h"

void scene_game_load( struct SceneGame* out_state ) {
    unused( out_state );
}
void scene_game_unload( struct SceneGame* state ) {
    unused( state );
}
void scene_game_update( f32 dt, struct SceneGame* state ) {
    unused(dt, state);
}
void scene_game_draw( f32 dt, struct SceneGame* state ) {
    unused(dt, state);
    ClearBackground(BLACK);

#if defined(DEBUG)
    /*debug*/ {
        Color col = RAYWHITE;
        col.a = 127;
        gui_text_draw(
            font_text(), TextFormat( "%fms", dt ),
            v2( 0.0f, 0.0f ),
            TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_START,
            col );
    }
#endif
}

