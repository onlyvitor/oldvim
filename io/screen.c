#include "screen.h"

#include <stdio.h>

int screen_puts(const char *s)
{
    return fputs(s, stdout) == EOF ? -1 : 0;
}

int screen_write(const char *s, size_t n)
{
    if (n == 0)
    {
        return 0;
    }

    return fwrite(s, 1, n, stdout) == n ? 0 : -1;
}

int screen_putc(char c)
{
    return putchar(c) == EOF ? -1 : 0;
}

int screen_flush(void)
{
    return fflush(stdout) == EOF ? -1 : 0;
}

int screen_carriage_return(void)
{
    return fputc('\r', stdout) == EOF ? -1 : 0;
}

int screen_erase_below(void)
{
    return screen_puts("\033[J");
}

int screen_goto_column(size_t column)
{
    if (column == 0)
    {
        return 0;
    }

    return fprintf(stdout, "\033[%zuC", column) < 0 ? -1 : 0;
}

int screen_move_left(size_t columns)
{
    if (columns == 0)
    {
        return 0;
    }

    return fprintf(stdout, "\033[%zuD", columns) < 0 ? -1 : 0;
}

int screen_move_up(size_t rows)
{
    if (rows == 0)
    {
        return 0;
    }

    return fprintf(stdout, "\033[%zuA", rows) < 0 ? -1 : 0;
}

int screen_move_down(size_t rows)
{
    if (rows == 0)
    {
        return 0;
    }

    return fprintf(stdout, "\033[%zuB", rows) < 0 ? -1 : 0;
}
