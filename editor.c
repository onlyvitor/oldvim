#include "editor.h"

#include <stdio.h>
#include <stdlib.h>

Editor *editor_open(const char *path)
{
    EditorFile *file = open_file(path);

    if (file == NULL)
    {
        return NULL;
    }

    Editor *ed = malloc(sizeof *ed);

    if (ed == NULL)
    {
        fprintf(stderr, "error: cannot allocate memory for the editor state\n");
        close_file(file);
        return NULL;
    }

    ed->file = file;
    ed->cursor = file->len;
    ed->dirty = false;

    return ed;
}

void editor_close(Editor *ed)
{
    if (ed == NULL)
    {
        return;
    }

    close_file(ed->file);
    free(ed);
}

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

CursorMotion editor_move_left(Editor *ed)
{
    if (ed->cursor == 0)
    {
        return CURSOR_MOTION_NONE;
    }

    if (ed->file->data[ed->cursor - 1] == '\n')
    {
        /* Jump to the end of the previous line. */
        ed->cursor--;
        return CURSOR_MOTION_PREVIOUS_LINE;
    }

    ed->cursor--;
    return CURSOR_MOTION_LEFT;
}

CursorMotion editor_move_right(Editor *ed)
{
    if (ed->cursor == ed->file->len)
    {
        return CURSOR_MOTION_NONE;
    }

    if (ed->file->data[ed->cursor] == '\n')
    {
        /* Jump to the beginning of the next line. */
        ed->cursor++;
        return CURSOR_MOTION_NEXT_LINE;
    }

    ed->cursor++;
    return CURSOR_MOTION_RIGHT;
}

CursorMotion editor_move_up(Editor *ed)
{
    size_t start = line_start_index(ed->file->data, ed->cursor);

    if (start == 0)
    {
        return CURSOR_MOTION_NONE;
    }

    /* The newline that ends the previous line. */
    size_t prev_end = start - 1;
    size_t prev_start = line_start_index(ed->file->data, prev_end);
    size_t prev_len = prev_end - prev_start;
    size_t column = ed->cursor - start;
    size_t new_column = column < prev_len ? column : prev_len;

    ed->cursor = prev_start + new_column;
    return CURSOR_MOTION_UP;
}

CursorMotion editor_move_down(Editor *ed)
{
    size_t start = line_start_index(ed->file->data, ed->cursor);
    size_t end = line_end_index(ed->file->data, ed->file->len, start);

    if (end == ed->file->len)
    {
        /* There is no line below the last one. */
        return CURSOR_MOTION_NONE;
    }

    size_t next_start = end + 1;
    size_t next_end = line_end_index(ed->file->data, ed->file->len, next_start);
    size_t next_len = next_end - next_start;
    size_t column = ed->cursor - start;
    size_t new_column = column < next_len ? column : next_len;

    ed->cursor = next_start + new_column;
    return CURSOR_MOTION_DOWN;
}
