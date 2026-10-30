#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>

#include "fileio/fileio.h"

/* Index of the first byte of the line that contains pos. */
size_t line_start_index(const char *data, size_t pos);

/* Index of the newline that ends the line containing pos, or len. */
size_t line_end_index(const char *data, size_t len, size_t pos);

/* 0-based row and byte column of pos. */
void editor_row_column(const EditorFile *ef, size_t pos, size_t *row, size_t *column);

/*
 * What a movement did, so the caller can render it without knowing the
 * buffer layout. NONE means the cursor did not move.
 */
typedef enum CursorMotion
{
    CURSOR_MOTION_NONE,
    CURSOR_MOTION_LEFT,
    CURSOR_MOTION_RIGHT,
    CURSOR_MOTION_PREVIOUS_LINE,
    CURSOR_MOTION_NEXT_LINE,
    CURSOR_MOTION_UP,
    CURSOR_MOTION_DOWN,
} CursorMotion;

CursorMotion editor_move_left(EditorFile *ef, size_t *cursor);
CursorMotion editor_move_right(EditorFile *ef, size_t *cursor);
CursorMotion editor_move_up(EditorFile *ef, size_t *cursor);
CursorMotion editor_move_down(EditorFile *ef, size_t *cursor);

#endif
