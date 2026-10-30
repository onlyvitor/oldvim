#include "helper.h"

#include "../io/screen.h"

size_t line_start_index(const char *data, size_t pos)
{
    size_t start = pos;

    while (start > 0 && data[start - 1] != '\n')
    {
        start--;
    }

    return start;
}

int redraw_from(const EditorFile *ef, size_t start, size_t cursor)
{
    /* Move to the beginning of the line and erase everything below it. */
    if (screen_carriage_return() != 0 || screen_erase_below() != 0)
    {
        return -1;
    }

    if (screen_write(ef->data + start, ef->len - start) != 0)
    {
        return -1;
    }

    /* Count how many lines were printed after the cursor. */
    size_t rows = 0;

    for (size_t i = cursor; i < ef->len; i++)
    {
        if (ef->data[i] == '\n')
        {
            rows++;
        }
    }

    if (screen_move_up(rows) != 0 || screen_carriage_return() != 0)
    {
        return -1;
    }

    /* Move right to the column where the cursor lives. */
    size_t column = cursor - line_start_index(ef->data, cursor);

    if (screen_goto_column(column) != 0)
    {
        return -1;
    }

    return screen_flush();
}