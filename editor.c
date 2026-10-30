#include "editor.h"

size_t line_start_index(const char *data, size_t pos)
{
    size_t start = pos;

    while (start > 0 && data[start - 1] != '\n')
    {
        start--;
    }

    return start;
}

size_t line_end_index(const char *data, size_t len, size_t pos)
{
    size_t end = pos;

    while (end < len && data[end] != '\n')
    {
        end++;
    }

    return end;
}

void editor_row_column(const EditorFile *ef, size_t pos, size_t *row, size_t *column)
{
    size_t r = 0;

    for (size_t i = 0; i < pos; i++)
    {
        if (ef->data[i] == '\n')
        {
            r++;
        }
    }

    if (row != NULL)
    {
        *row = r;
    }

    if (column != NULL)
    {
        *column = pos - line_start_index(ef->data, pos);
    }
}

CursorMotion editor_move_left(EditorFile *ef, size_t *cursor)
{
    if (*cursor == 0)
    {
        return CURSOR_MOTION_NONE;
    }

    if (ef->data[*cursor - 1] == '\n')
    {
        /* Jump to the end of the previous line. */
        (*cursor)--;
        return CURSOR_MOTION_PREVIOUS_LINE;
    }

    (*cursor)--;
    return CURSOR_MOTION_LEFT;
}

CursorMotion editor_move_right(EditorFile *ef, size_t *cursor)
{
    if (*cursor == ef->len)
    {
        return CURSOR_MOTION_NONE;
    }

    if (ef->data[*cursor] == '\n')
    {
        /* Jump to the beginning of the next line. */
        (*cursor)++;
        return CURSOR_MOTION_NEXT_LINE;
    }

    (*cursor)++;
    return CURSOR_MOTION_RIGHT;
}

CursorMotion editor_move_up(EditorFile *ef, size_t *cursor)
{
    size_t start = line_start_index(ef->data, *cursor);

    if (start == 0)
    {
        return CURSOR_MOTION_NONE;
    }

    /* The newline that ends the previous line. */
    size_t prev_end = start - 1;
    size_t prev_start = line_start_index(ef->data, prev_end);
    size_t prev_len = prev_end - prev_start;
    size_t column = *cursor - start;
    size_t new_column = column < prev_len ? column : prev_len;

    *cursor = prev_start + new_column;
    return CURSOR_MOTION_UP;
}

CursorMotion editor_move_down(EditorFile *ef, size_t *cursor)
{
    size_t start = line_start_index(ef->data, *cursor);
    size_t end = line_end_index(ef->data, ef->len, start);

    if (end == ef->len)
    {
        /* There is no line below the last one. */
        return CURSOR_MOTION_NONE;
    }

    size_t next_start = end + 1;
    size_t next_end = line_end_index(ef->data, ef->len, next_start);
    size_t next_len = next_end - next_start;
    size_t column = *cursor - start;
    size_t new_column = column < next_len ? column : next_len;

    *cursor = next_start + new_column;
    return CURSOR_MOTION_DOWN;
}
