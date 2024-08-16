#if !defined(SC_GAME_H)
#define SC_GAME_H
/**
 * @file   sc_game.h
 * @brief  Scene Game.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"

struct SceneGame {

};

void scene_game_load( struct SceneGame* out_state );
void scene_game_unload( struct SceneGame* state );
void scene_game_update( f32 dt, struct SceneGame* state );
void scene_game_draw( f32 dt, struct SceneGame* state );

#endif /* header guard */
