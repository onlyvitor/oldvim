#define _POSIX_C_SOURCE 200809L

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include "fileio/fileio.h"
#include "io/scan.h"

#define CTRL_D 0x04

static struct termios saved_termios;
static volatile sig_atomic_t rawmode_enabled = 0;

static void restore_terminal(void)
{
    if (rawmode_enabled)
    {
        rawmode_off(&saved_termios);
        rawmode_enabled = 0;
    }
}

static void handle_signal(int signum)
{
    if (rawmode_enabled)
    {
        /* tcsetattr e _exit sao async-signal-safe */
        tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
    }

    _exit(128 + signum);
}

static void install_signal_handlers(void)
{
    static const int signals[] = {SIGINT, SIGTERM, SIGHUP, SIGQUIT};
    struct sigaction action = {
        .sa_handler = handle_signal,
        .sa_flags = 0,
    };

    sigemptyset(&action.sa_mask);

    for (size_t i = 0; i < sizeof(signals) / sizeof(signals[0]); i++)
    {
        if (sigaction(signals[i], &action, NULL) == -1)
        {
            perror("error: cannot install the signal handler");
        }
    }
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

    install_signal_handlers();

    if (isatty(STDIN_FILENO) && rawmode_on(&saved_termios) == 0)
    {
        rawmode_enabled = 1;
    }

    bool done = false;
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
        else
        {
            char byte = (char)c;

            if (append_bytes(ef, &byte, 1) != 0)
            {
                perror("error: cannot write the character to the file");
                status = 1;
                done = true;
            }
        }
    }

    restore_terminal();

    return status;
}
