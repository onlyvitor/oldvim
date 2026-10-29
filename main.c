#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "fileio/fileio.h"
#include "io/scan.h"
#include "helpers/helper.h"

static int cursor_step_left(const EditorFile *ef, size_t *cursor, bool interactive)
{
    if (*cursor == 0)
    {
        return 0;
    }

    if (ef->data[*cursor - 1] == '\n')
    {
        /* Jump to the end of the previous line. */
        size_t prev_end = *cursor - 1;
        size_t prev_len = prev_end - line_start_index(ef->data, prev_end);

        if (interactive)
        {
            if (fputs("\033[A\r", stdout) == EOF)
            {
                return -1;
            }

            if (prev_len > 0 && fprintf(stdout, "\033[%zuC", prev_len) < 0)
            {
                return -1;
            }

            if (fflush(stdout) == EOF)
            {
                return -1;
            }
        }

        *cursor = prev_end;
        return 0;
    }

    if (interactive)
    {
        if (fputs("\033[D", stdout) == EOF || fflush(stdout) == EOF)
        {
            return -1;
        }
    }

    (*cursor)--;
    return 0;
}

static int cursor_step_right(const EditorFile *ef, size_t *cursor, bool interactive)
{
    if (*cursor == ef->len)
    {
        return 0;
    }

    if (ef->data[*cursor] == '\n')
    {
        /* Jump to the beginning of the next line. */
        if (interactive)
        {
            if (fputs("\r\n", stdout) == EOF || fflush(stdout) == EOF)
            {
                return -1;
            }
        }

        (*cursor)++;
        return 0;
    }

    if (interactive)
    {
        if (fputs("\033[C", stdout) == EOF || fflush(stdout) == EOF)
        {
            return -1;
        }
    }

    (*cursor)++;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    EditorFile *ef = open_file(argv[1]);

    if (ef == NULL)
    {
        return 1;
    }

    if (rawmode_on() != 0)
    {
        close_file(ef);
        return 1;
    }

    bool done = false;
    bool interactive = isatty(STDIN_FILENO) != 0;
    int status = 0;
    size_t cursor = ef->len;

    while (!done)
    {
        int c = scan();

        if (c == EOF)
        {
            if (ferror(stdin))
            {
                status = 1;
            }
            else
            {
                printf("\n[EOF]\n");
            }

            done = true;
        }
        else if (c == CTRL_D)
        {
            done = true;
        }
        else if (c == LEFT_ARROW || c == RIGHT_ARROW)
        {
            int rc = c == LEFT_ARROW
                         ? cursor_step_left(ef, &cursor, interactive)
                         : cursor_step_right(ef, &cursor, interactive);

            if (rc != 0)
            {
                perror("error: cannot move the cursor");
                status = 1;
                done = true;
            }
        }
        else if (c == UP_ARROW || c == DOWN_ARROW)
        {
            /* Vertical movement is not implemented yet. */
            continue;
        }
        else if (c == KEY_BACKSPACE)
        {
            if (cursor == 0)
            {
                continue;
            }

            size_t pos = cursor - 1;
            bool removed_newline = ef->data[pos] == '\n';
            size_t start = line_start_index(ef->data, pos);

            if (remove_byte_at(ef, pos) != 0)
            {
                status = 1;
                done = true;
            }
            else
            {
                cursor = pos;

                if (interactive)
                {
                    if (removed_newline && fputs("\033[A", stdout) == EOF)
                    {
                        perror("error: cannot move the cursor");
                        status = 1;
                        done = true;
                    }
                    else if (redraw_from(ef, start, cursor) != 0)
                    {
                        perror("error: cannot redraw the line");
                        status = 1;
                        done = true;
                    }
                }
            }
        }
        else
        {
            char byte = (char)c;
            bool at_end = cursor == ef->len;
            int rc = insert_bytes(ef, cursor, &byte, 1);

            if (rc != 0)
            {
                fprintf(stderr, "error: cannot insert the character (code %d)\n", rc);
                status = 1;
                done = true;
            }
            else
            {
                cursor++;

                if (interactive)
                {
                    int redraw_rc;

                    if (at_end)
                    {
                        redraw_rc = putchar(c) == EOF ? -1 : 0;
                    }
                    else
                    {
                        redraw_rc = redraw_from(ef, line_start_index(ef->data, cursor - 1), cursor);
                    }

                    if (redraw_rc != 0 || fflush(stdout) == EOF)
                    {
                        perror("error: cannot show the character");
                        status = 1;
                        done = true;
                    }
                }
            }
        }
    }

    if (rawmode_off() != 0)
    {
        status = 1;
    }

    if (status == 0 && save_file(ef) != 0)
    {
        status = 1;
    }

    close_file(ef);

    return status;
}
