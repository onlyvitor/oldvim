#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "fileio/fileio.h"
#include "io/scan.h"
#include "helpers/helper.h"

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
        else if (c == KEY_BACKSPACE)
        {
            if (ef->len == 0)
            {
                continue;
            }

            char removed = ef->data[ef->len - 1];
            remove_byte(ef);

            if (interactive && redraw_last_line(ef, removed == '\n') != 0)
            {
                perror("error: cannot redraw the line");
                status = 1;
                done = true;
            }
        }
        else
        {
            char byte = (char)c;
            int rc = append_bytes(ef, &byte, 1);

            if (rc != 0)
            {
                fprintf(stderr, "error: cannot append the character (code %d)\n", rc);
                status = 1;
                done = true;
            }
            else if (interactive && (putchar(c) == EOF || fflush(stdout) == EOF))
            {
                perror("error: cannot show the character");
                status = 1;
                done = true;
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
