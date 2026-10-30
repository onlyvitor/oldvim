#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "fileio/fileio.h"
#include "io/scan.h"
#include "io/screen.h"
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

        if (interactive &&
            (screen_move_up(1) != 0 || screen_carriage_return() != 0 ||
             screen_goto_column(prev_len) != 0 || screen_flush() != 0))
        {
            return -1;
        }

        *cursor = prev_end;
        return 0;
    }

    if (interactive)
    {
        if (screen_move_left(1) != 0 || screen_flush() != 0)
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
            if (screen_puts("\r\n") != 0 || screen_flush() != 0)
            {
                return -1;
            }
        }

        (*cursor)++;
        return 0;
    }

    if (interactive)
    {
        if (screen_goto_column(1) != 0 || screen_flush() != 0)
        {
            return -1;
        }
    }

    (*cursor)++;
    return 0;
}

static int cursor_step_up(const EditorFile *ef, size_t *cursor, bool interactive)
{
    size_t start = line_start_index(ef->data, *cursor);

    if (start == 0)
    {
        return 0;
    }

    /* The newline that ends the previous line. */
    size_t prev_end = start - 1;
    size_t prev_start = line_start_index(ef->data, prev_end);
    size_t prev_len = prev_end - prev_start;
    size_t column = *cursor - start;
    size_t new_column = column < prev_len ? column : prev_len;

    if (interactive &&
        (screen_move_up(1) != 0 || screen_carriage_return() != 0 ||
         screen_goto_column(new_column) != 0 || screen_flush() != 0))
    {
        return -1;
    }
    *cursor = prev_start + new_column;
    return 0;
}

static int cursor_step_down(const EditorFile *ef, size_t *cursor, bool interactive)
{
    size_t start = line_start_index(ef->data, *cursor);
    size_t end = start;

    while (end < ef->len && ef->data[end] != '\n')
    {
        end++;
    }

    if (end == ef->len)
    {
        /* There is no line below the last one. */
        return 0;
    }

    size_t next_start = end + 1;
    size_t next_end = next_start;

    while (next_end < ef->len && ef->data[next_end] != '\n')
    {
        next_end++;
    }

    size_t next_len = next_end - next_start;
    size_t column = *cursor - start;
    size_t new_column = column < next_len ? column : next_len;

    if (interactive &&
        (screen_move_down(1) != 0 || screen_carriage_return() != 0 ||
         screen_goto_column(new_column) != 0 || screen_flush() != 0))
    {
        return -1;
    }

    *cursor = next_start + new_column;
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
        else if (c == CTRL_S)
        {
            if (save_file(ef) != 0)
            {
                status = 1;
                done = true;
            }
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
            int rc = c == UP_ARROW
                         ? cursor_step_up(ef, &cursor, interactive)
                         : cursor_step_down(ef, &cursor, interactive);

            if (rc != 0)
            {
                perror("error: cannot move the cursor");
                status = 1;
                done = true;
            }
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
                    if (removed_newline && screen_move_up(1) != 0)
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
                        redraw_rc = screen_putc((char)c);
                    }
                    else
                    {
                        redraw_rc = redraw_from(ef, line_start_index(ef->data, cursor - 1), cursor);
                    }

                    if (redraw_rc != 0 || screen_flush() != 0)
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
