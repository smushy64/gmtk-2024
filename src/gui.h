#if !defined(GUI_H)
#define GUI_H
/**
 * @file   gui.h
 * @brief  GUI
 * @author Alicia Amarilla (smushyaa@gmail.com)
 * @date   August 16, 2024
*/
#include "common.h"

enum Anchor {
    ANCHOR_START,
    ANCHOR_CENTER,
    ANCHOR_END,
};

Vector2 gui_screen_size(void);
Vector2 gui_screen_center(void);

Rectangle gui_text_rect(
    Font font, const char* str, Vector2 position, f32 scale,
    enum Anchor anchor_hor, enum Anchor anchor_vert );
Rectangle gui_text_draw(
    Font font, const char* str, Vector2 position, f32 scale,
    enum Anchor anchor_hor, enum Anchor anchor_vert, Color color );

#endif /* header guard */
