/**
 * @file   main.c
 * @brief  GMTK 2024 main.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 15, 2024
*/
#include "raylib.h"

void Update(void);

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void _unused( int a, ... ) { (void)a; return; }
#define unused(...) _unused( 0, __VA_ARGS__ )

int main( int argc, char** argv ) {
    unused( argc, argv );

    InitWindow( 800, 600, "Hello, World!" );

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop( Update, 0, 1 );
#else
    SetTargetFPS(60);
    while( !WindowShouldClose() ) {
        Update();
    }
#endif

    CloseWindow();
    return 0;
}

void Update(void) {
    BeginDrawing();
    ClearBackground(RED);
    EndDrawing();
}

