#include "helper.h"

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
    if (fputs("\r\033[J", stdout) == EOF)
    {
        return -1;
    }

    size_t n = ef->len - start;

    if (n > 0 && fwrite(ef->data + start, 1, n, stdout) != n)
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

    if (rows > 0 && fprintf(stdout, "\033[%zuA", rows) < 0)
    {
        return -1;
    }

    if (fputc('\r', stdout) == EOF)
    {
        return -1;
    }

    /* Move right to the column where the cursor lives. */
    size_t column = cursor - line_start_index(ef->data, cursor);

    if (column > 0 && fprintf(stdout, "\033[%zuC", column) < 0)
    {
        return -1;
    }

    return fflush(stdout) == EOF ? -1 : 0;
}