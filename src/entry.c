/**
 * @file   entry.c
 * @brief  Entry points.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "common.h"
#include "entry.h"
#include "raylib.h"
#include "string.h"
#include <stdio.h>

#include "sc_title.h"
#include "sc_main.h"
#include "sc_game.h"

#define DEBUG_START SC_MAIN
#define DEBUG_MUTE

struct GameState {
    f32 elapsed;
    u64 frames_elapsed;
    enum Scene current_scene;
    enum Scene next_scene;
    union SceneState {
        struct SceneTitle title;
        struct SceneMain  main;
        struct SceneGame  game;
    } scene_state;

    Font font_title;
    Font font_text;
};
static struct GameState* global_game_state = NULL;

void game_init(void);
void internal_scene_load( enum Scene scene );

void game_update( f32 dt ) {
    if( !global_game_state ) {
        game_init();
    }
    if( global_game_state->next_scene ) {
        internal_scene_load( global_game_state->next_scene );
    }

    switch( global_game_state->current_scene ) {
        case SC_TITLE: {
            scene_title_update( dt, &global_game_state->scene_state.title );
        } break;
        case SC_MAIN: {
            scene_main_update( dt, &global_game_state->scene_state.main );
        } break;
        case SC_GAME: {
            scene_game_update( dt, &global_game_state->scene_state.game );
        } break;
        case SC_NONE: break;
    }

}
void game_draw( f32 dt ) {
    switch( global_game_state->current_scene ) {
        case SC_TITLE: {
            scene_title_draw( dt, &global_game_state->scene_state.title );
        } break;
        case SC_MAIN: {
            scene_main_draw( dt, &global_game_state->scene_state.main );
        } break;
        case SC_GAME: {
            scene_game_draw( dt, &global_game_state->scene_state.game );
        } break;
        case SC_NONE: break;
    }

    global_game_state->elapsed += dt;
    global_game_state->frames_elapsed++;
}

void internal_scene_load( enum Scene scene ) {
    EnableCursor();
    global_game_state->elapsed        = 0.0f;
    global_game_state->frames_elapsed = 0;
    global_game_state->next_scene     = SC_NONE;
    
    switch( global_game_state->current_scene ) {
        case SC_TITLE: {
            scene_title_unload( &global_game_state->scene_state.title );
        } break;
        case SC_MAIN: {
            scene_main_unload( &global_game_state->scene_state.main );
        } break;
        case SC_GAME: {
            scene_game_unload( &global_game_state->scene_state.game );
        } break;
        case SC_NONE: break;
    }

    global_game_state->current_scene = scene;
    switch( scene ) {
        case SC_TITLE: { 
            scene_title_load( &global_game_state->scene_state.title );
        } break;
        case SC_MAIN: {
            scene_main_load( &global_game_state->scene_state.main );
        } break;
        case SC_GAME: {
            scene_game_load( &global_game_state->scene_state.game );
        } break;
        case SC_NONE: break;
    }

    TraceLog( LOG_INFO, "Loaded scene '%s'", scene_to_string( scene ) );
}
void scene_load( enum Scene scene ) {
    global_game_state->next_scene = scene;
}

void game_init(void) {
    struct GameState* state = MemAlloc( sizeof(*state) );

#if defined(DEBUG) && defined(DEBUG_MUTE)
    SetMasterVolume( 0.0f );
#endif

    state->font_text = LoadFontEx(
        "resources/typeface/RobotoCondensed-Regular.ttf",
        TEXT_FONT_SIZE, 0, 0 );
    state->font_title = LoadFontEx(
        "resources/typeface/ChakraPetch-Bold.ttf",
        TITLE_FONT_SIZE, 0, 0 );

    volatile b32 ready = false;
    while(!ready) {
        ready =
            IsFontReady( state->font_text ) &&
            IsFontReady( state->font_title );
    }

    global_game_state = state;

#if defined(DEBUG)
    internal_scene_load( DEBUG_START );
#else
    internal_scene_load( SC_TITLE );
#endif
}

f32 time_elapsed(void) {
    return global_game_state->elapsed;
}
u64  frames_elapsed(void) {
    return global_game_state->frames_elapsed;
}
Font font_title(void) {
    return global_game_state->font_title;
}
Font font_text(void) {
    return global_game_state->font_text;
}

