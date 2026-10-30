#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "editor.h"
#include "io/scan.h"
#include "io/screen.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    Editor *ed = editor_open(argv[1]);

    if (ed == NULL)
    {
        return 1;
    }

    if (rawmode_on() != 0)
    {
        editor_close(ed);
        return 1;
    }

    bool done = false;
    bool interactive = isatty(STDIN_FILENO) != 0;
    int status = 0;

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
            if (save_file(ed->file) != 0)
            {
                status = 1;
                done = true;
            }
        }
        else if (c == LEFT_ARROW || c == RIGHT_ARROW || c == UP_ARROW || c == DOWN_ARROW)
        {
            CursorMotion motion;

            if (c == LEFT_ARROW)
            {
                motion = editor_move_left(ed);
            }
            else if (c == RIGHT_ARROW)
            {
                motion = editor_move_right(ed);
            }
            else if (c == UP_ARROW)
            {
                motion = editor_move_up(ed);
            }
            else
            {
                motion = editor_move_down(ed);
            }

            if (interactive && screen_apply_motion(ed->file, ed->cursor, motion) != 0)
            {
                perror("error: cannot move the cursor");
                status = 1;
                done = true;
            }
        }
        else if (c == KEY_BACKSPACE)
        {
            if (ed->cursor == 0)
            {
                continue;
            }

            size_t pos = ed->cursor - 1;
            bool removed_newline = ed->file->data[pos] == '\n';
            size_t start = line_start_index(ed->file->data, pos);

            if (remove_byte_at(ed->file, pos) != 0)
            {
                status = 1;
                done = true;
            }
            else
            {
                ed->cursor = pos;
                ed->dirty = true;

                if (interactive)
                {
                    if (removed_newline && screen_move_up(1) != 0)
                    {
                        perror("error: cannot move the cursor");
                        status = 1;
                        done = true;
                    }
                    else if (screen_redraw_from(ed->file, start, ed->cursor) != 0)
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
            bool at_end = ed->cursor == ed->file->len;
            int rc = insert_bytes(ed->file, ed->cursor, &byte, 1);

            if (rc != 0)
            {
                fprintf(stderr, "error: cannot insert the character (code %d)\n", rc);
                status = 1;
                done = true;
            }
            else
            {
                ed->cursor++;
                ed->dirty = true;

                if (interactive)
                {
                    int redraw_rc;

                    if (at_end)
                    {
                        redraw_rc = screen_putc((char)c);
                    }
                    else
                    {
                        redraw_rc = screen_redraw_from(ed->file, line_start_index(ed->file->data, ed->cursor - 1), ed->cursor);
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

    if (status == 0 && save_file(ed->file) != 0)
    {
        status = 1;
    }

    editor_close(ed);

    return status;
}
