/**
 * @file   gui.c
 * @brief  GUI implementation.
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "gui.h"
#include "mathex.h"

#define rect_position( r ) *(Vector2*)(r)
#define rect_scale( r ) *((Vector2*)(r) + 1)

Vector2 gui_screen_size(void) {
    Vector2 res;
    res.x = GetScreenWidth();
    res.y = GetScreenHeight();
    return res;
}
Vector2 gui_screen_center(void) {
    return Vector2Multiply( gui_screen_size(), v2_scalar(0.5f) );
}

Rectangle gui_text_rect(
    Font font, const char* str, Vector2 position, f32 scale,
    enum Anchor anchor_hor, enum Anchor anchor_vert
) {
    Vector2 pos, size;

    size = MeasureTextEx( font, str, scale, 1.0f );
    pos = position;

    // NOTE(alicia): text start is upper-left corner
    switch( anchor_hor ) {
        case ANCHOR_START: break;
        case ANCHOR_CENTER: {
            pos.x -= size.x / 2.0f;
        } break;
        case ANCHOR_END: {
            pos.x -= size.x;
        } break;
    }
    switch( anchor_vert ) {
        case ANCHOR_START: {
        } break;
        case ANCHOR_CENTER: {
            pos.y -= size.y / 2.0f;
        } break;
        case ANCHOR_END: {
            pos.y -= size.y;
        } break;
    }

    Rectangle res;
    res.x = pos.x;
    res.y = pos.y;
    res.width = size.x;
    res.height = size.y;

    return res;
}
Rectangle gui_text_draw(
    Font font, const char* str, Vector2 position, f32 scale,
    enum Anchor anchor_hor, enum Anchor anchor_vert, Color color
) {
    Rectangle rect = gui_text_rect(
        font, str, position, scale, anchor_hor, anchor_vert );

    DrawTextEx( font, str, rect_position(&rect), scale, 1.0f, color );

    return rect;
}

