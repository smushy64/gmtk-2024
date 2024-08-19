#if !defined(SC_MAIN_H)
#define SC_MAIN_H
/**
 * @file   sc_main.h
 * @brief  Scene Main Menu.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "common.h"

enum MainOption {
    MO_NONE,
    MO_PLAY,
    MO_QUIT
};
#if defined(PLATFORM_WEB)
    #define MO_COUNT (MO_QUIT)
#else
    #define MO_COUNT (MO_QUIT + 1)
#endif

#define OPTIONS_START_Y (TEXT_FONT_SIZE)
#define OPTIONS_GUTTER  (TEXT_FONT_SIZE * 1.5f)

#define OPTIONS_SELECTED_COLOR (GRAY)
#define OPTIONS_COLOR (RAYWHITE)

struct SceneMain {
    enum MainOption selected;
    Music music;
};

void scene_main_load( struct SceneMain* out_state );
void scene_main_unload( struct SceneMain* state );
void scene_main_update( f32 dt, struct SceneMain* state );
void scene_main_draw( f32 dt, struct SceneMain* state );

static inline const char* main_option_to_string( enum MainOption opt ) {
    switch( opt ) {
        case MO_NONE: return "";
        case MO_PLAY: return "Play Game";
        case MO_QUIT: return "Quit Game";
    }
}

#endif /* header guard */
