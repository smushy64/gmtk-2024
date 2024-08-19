/**
 * @file   main.c
 * @brief  GMTK 2024 main.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "raylib.h"
#include "entry.h"
#include "common.h"
#include <stdio.h>
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

static b32 should_game_quit = false;
void Update(void);
void CustomLog( int msgType, const char* text, va_list args );

int main( int argc, char** argv ) {
    unused( argc, argv );

#if defined(DEBUG)
    SetTraceLogLevel( LOG_ALL );
    #if !defined(PLATFORM_WEB)
        SetTraceLogCallback( CustomLog );
    #endif
#else
    SetTraceLogLevel( LOG_NONE );
#endif

    InitWindow( GAME_WIDTH, GAME_HEIGHT, GAME_NAME );
    InitAudioDevice();

#if !defined(PLATFORM_WEB) && !defined(DEBUG)
    SetExitKey(KEY_NULL);
#endif

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop( Update, 0, 1 );
#else
    Image window_icon = LoadImage( "resources/img/branding/gmtk2024-icon.png" );
    while( !IsImageReady(window_icon) ) {
    }
    SetWindowIcon( window_icon );
    UnloadImage( window_icon );
    SetTargetFPS(60);
    while( !should_game_quit && !WindowShouldClose() ) {
        Update();
    }
#endif

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void Update(void) {
    BeginDrawing();

    f32 dt = GetFrameTime();

#if !defined(PLATFORM_WEB)
    if(
        IsKeyPressed(KEY_F11) ||
        (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))
    ) {
        if( IsWindowFullscreen() ) {
            int display = GetCurrentMonitor();
            int w = GetMonitorWidth( display );
            int h = GetMonitorHeight( display );

            SetWindowSize( w, h );
        } else {
            SetWindowSize( GAME_WIDTH, GAME_HEIGHT );
        }

        ToggleBorderlessWindowed();
    }
#endif

    game_update( dt );
    game_draw( dt );

    EndDrawing();
}

/// @brief Color code black.
#define ANSI_COLOR_BLACK   "\033[1;30m"
/// @brief Color code white.
#define ANSI_COLOR_WHITE   "\033[1;37m"
/// @brief Color code red.
#define ANSI_COLOR_RED     "\033[1;31m"
/// @brief Color code green.
#define ANSI_COLOR_GREEN   "\033[1;32m"
/// @brief Color code blue.
#define ANSI_COLOR_BLUE    "\033[1;34m"
/// @brief Color code magenta.
#define ANSI_COLOR_MAGENTA "\033[1;35m"
/// @brief Color code yellow.
#define ANSI_COLOR_YELLOW  "\033[1;33m"
/// @brief Color code cyan.
#define ANSI_COLOR_CYAN    "\033[1;36m"
/// @brief Color code to reset color.
#define ANSI_COLOR_RESET   "\033[1;00m"

void CustomLog( int msgType, const char* text, va_list args ) {
    switch( msgType ) {
        case LOG_INFO    : printf(ANSI_COLOR_RESET   "[I] "); break;
        case LOG_ERROR   : printf(ANSI_COLOR_RED     "[E] "); break;
        case LOG_WARNING : printf(ANSI_COLOR_YELLOW  "[W] "); break;
        case LOG_DEBUG   : printf(ANSI_COLOR_BLUE    "[D] "); break;
        case LOG_FATAL   : printf(ANSI_COLOR_MAGENTA "[F] "); break;
        case LOG_TRACE   : printf(ANSI_COLOR_WHITE   "[T] "); break;
        default          : return;
    }

    vprintf( text, args );
    printf(ANSI_COLOR_RESET "\n");
}

void quit_game(void) {
    should_game_quit = true;
}

