#include "scan.h"

#include <signal.h>

static struct termios saved_termios;
static volatile sig_atomic_t rawmode_enabled = 0;

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

// enter on raw mode for edit the input
int rawmode_on(void)
{
    if (rawmode_enabled)
    {
        return 0;
    }

    if (!isatty(STDIN_FILENO))
    {
        return 0;
    }

    if (tcgetattr(STDIN_FILENO, &saved_termios) == -1)
    {
        perror("error: cannot get the terminal attributes");
        return -1;
    }

    install_signal_handlers();

    struct termios raw = saved_termios;

    // desativa o buffer de linha e a exibicao automatica na tela
    raw.c_lflag &= ~(ICANON | ECHO);

    // le um byte por vez, sem timeout
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    // aplica as novas configuracoes
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1)
    {
        perror("error: cannot enable the raw mode");
        return -1;
    }

    rawmode_enabled = 1;

    return 0;
}

// return to the default config of terminal
int rawmode_off(void)
{
    if (!rawmode_enabled)
    {
        return 0;
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios) == -1)
    {
        perror("error: cannot restore the terminal attributes");
        return -1;
    }

    rawmode_enabled = 0;

    return 0;
}
