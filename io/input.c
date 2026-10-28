#include "scan.h"

// enter on raw mode for edit the input
int rawmode_on(struct termios *old) // need the old config of terminal
{
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, old) == -1)
    {
        perror("error: cannot get the terminal attributes");
        return -1;
    }

    raw = *old;

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

    return 0;
}

// return to the default config of terminal
int rawmode_off(const struct termios *old)
{
    if (tcsetattr(STDIN_FILENO, TCSANOW, old) == -1)
    {
        perror("error: cannot restore the terminal attributes");
        return -1;
    }

    return 0;
}
