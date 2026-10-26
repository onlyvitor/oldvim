#include "scan.h"

// enter on raw mode for edit the input
void rawmode_on(struct termios *old) // need the old config of terminal
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, old);
    raw = *old;

    // desativa o buffer de linha e a exibiçao automatica na tela
    raw.c_lflag &= ~(ICANON | ECHO);

    // aplica as novas configiurações
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1)
    {
        perror("error: fail to set for raw mode");
    }
}

// return to the default config of terminal
void rawmode_off(struct termios *old)
{
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}