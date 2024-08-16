/**
 * @file   sc_main.c
 * @brief  Scene Main Menu.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "sc_main.h"
#include "entry.h"

void scene_main_load( struct SceneMain* out_state ) {
    unused(out_state);
}
void scene_main_unload( struct SceneMain* state ) {
    unused(state);
}
void scene_main_update( f32 dt, struct SceneMain* state ) {
    unused(dt, state);
}
void scene_main_draw( f32 dt, struct SceneMain* state ) {
    unused(dt, state);

    Vector2 screen;
    screen.x = GetScreenWidth();
    screen.y = GetScreenHeight();

    Font title = font_title();

    int title_font_size = 64;

    f32 text_width = MeasureText( "GMTK 2024", title_font_size );

    Vector2 title_position;
    title_position.x = (screen.x / 2.0f) - (text_width / 2.0f);
    title_position.y = (screen.y / 2.0f);

    DrawTextEx( title, "GMTK 2024", title_position, title_font_size, 1.0f, WHITE );
}

