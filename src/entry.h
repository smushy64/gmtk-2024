#if !defined(ENTRY_H)
#define ENTRY_H
/**
 * @file   entry.h
 * @brief  Entry points.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "common.h"
#include "raylib.h"

enum Scene {
    SC_NONE,
    SC_TITLE,
    SC_MAIN,
    SC_GAME,
};

static inline const char* scene_to_string( enum Scene scene ) {
    switch( scene ) {
        case SC_TITLE: return "Title";
        case SC_MAIN:  return "Main Menu";
        case SC_GAME:  return "Game";
        default: return "None";
    }
}

void scene_load( enum Scene scene );

void game_update( f32 dt );
void game_draw( f32 dt );

f32  time_elapsed(void);
Font font_title(void);
Font font_text(void);

#endif /* header guard */
