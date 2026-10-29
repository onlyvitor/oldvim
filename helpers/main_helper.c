#include "helper.h"

int redraw_last_line(const EditorFile *ef, bool removed_newline)
{
    /* Se apagou um '\n', o cursor está na linha de baixo. */
    if (removed_newline && fputs("\033[A", stdout) == EOF)
    {
        return -1;
    }

    /* Volta ao início da linha, limpa e reimprime o que restou. */
    if (fputs("\r\033[K", stdout) == EOF)
    {
        return -1;
    }

    size_t start = 0;

    for (size_t i = ef->len; i > 0; i--)
    {
        if (ef->data[i - 1] == '\n')
        {
            start = i;
            break;
        }
    }

    size_t n = ef->len - start;

    if (fwrite(ef->data + start, 1, n, stdout) != n)
    {
        return -1;
    }

    return fflush(stdout) == EOF ? -1 : 0;
}