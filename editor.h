#ifndef EDITOR_H
#define EDITOR_H

#include <stdbool.h>
#include <stddef.h>

#include "fileio/fileio.h"

/* An open buffer plus the editing state that lives above the file. */
typedef struct Editor
{
    EditorFile *file;
    size_t cursor;
    bool dirty;
} Editor;

Editor *editor_open(const char *path);
void editor_close(Editor *ed);

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

CursorMotion editor_move_left(Editor *ed);
CursorMotion editor_move_right(Editor *ed);
CursorMotion editor_move_up(Editor *ed);
CursorMotion editor_move_down(Editor *ed);

#endif
