/**
 * @file   sc_main.c
 * @brief  Scene Main Menu.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "sc_main.h"
#include "entry.h"
#include "gui.h"

void scene_main_load( struct SceneMain* out_state ) {
    out_state->selected = MO_NONE;
    out_state->music    = LoadMusicStream( "resources/audio/MissionPlausible.ogg" );
    PlayMusicStream( out_state->music );
}
void scene_main_unload( struct SceneMain* state ) {
    UnloadMusicStream( state->music );
}
void scene_main_update( f32 dt, struct SceneMain* state ) {
    unused(dt);

    Vector2 screen_center = gui_screen_center();
    Font text_font        = font_text();

    Vector2 mouse_pos = GetMousePosition();

    state->selected = MO_NONE;
    for( u32 i = 1; i < MO_COUNT; ++i ) {
        Vector2 pos;
        pos.x = screen_center.x;
        pos.y = (screen_center.y + OPTIONS_START_Y) + ((i - 1) * OPTIONS_GUTTER);

        Rectangle rect = gui_text_rect(
            text_font, main_option_to_string(i),
            pos, TEXT_FONT_SIZE, ANCHOR_CENTER, ANCHOR_END );

        if( CheckCollisionPointRec( mouse_pos, rect ) ) {
            state->selected = i;
        }
    }

    f32 len    = GetMusicTimeLength( state->music );
    f32 played = GetMusicTimePlayed( state->music );
    if( played >= len - 1.0f ) {
        StopMusicStream( state->music );
        PlayMusicStream( state->music );
    }
    UpdateMusicStream( state->music );

    if( state->selected ) {
        SetMouseCursor( MOUSE_CURSOR_POINTING_HAND );

        if( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) ) {
            SetMouseCursor( MOUSE_CURSOR_ARROW );

            switch( state->selected ) {
                case MO_NONE: break;
                case MO_PLAY: {
                    scene_load( SC_GAME );
                } break;
                case MO_QUIT: {
                    quit_game();
                } break;
            }
        }
    } else {
        SetMouseCursor( MOUSE_CURSOR_ARROW );
    }
}
void scene_main_draw( f32 dt, struct SceneMain* state ) {
    unused(dt);
    ClearBackground( (Color){ 102, 191, 255, 255 } );

    Vector2 screen;
    screen.x = GetScreenWidth();
    screen.y = GetScreenHeight();

    Vector2 screen_center = gui_screen_center();

    Font title_font = font_title();
    Font text_font  = font_text();

    gui_text_draw(
        title_font, GAME_NAME,
        screen_center,
        TITLE_FONT_SIZE, ANCHOR_CENTER, ANCHOR_END, RAYWHITE );

    for( u32 i = 1; i < MO_COUNT; ++i ) {
        Vector2 pos;
        pos.x = screen_center.x;
        pos.y = (screen_center.y + OPTIONS_START_Y) + ((i - 1) * OPTIONS_GUTTER);

        Color col = state->selected == i ? OPTIONS_SELECTED_COLOR : OPTIONS_COLOR;
        gui_text_draw(
            text_font, main_option_to_string(i),
            pos, TEXT_FONT_SIZE, ANCHOR_CENTER, ANCHOR_END, col );
    }

    gui_text_draw(
        text_font, "made with raylib", (Vector2){2.0f, screen.y - 1.0f - TEXT_FONT_SIZE_SMALLEST},
        TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_END, RAYWHITE );
    gui_text_draw(
        text_font, "alicia amarilla (c) 2024", (Vector2){2.0f, screen.y - 1.0f},
        TEXT_FONT_SIZE_SMALLEST, ANCHOR_START, ANCHOR_END, RAYWHITE );
}

