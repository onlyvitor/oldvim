/* Feature-test macro: must be defined before any system header. */
#define _POSIX_C_SOURCE 200809L

#ifndef SCAN_H
#define SCAN_H

#define CTRL_D 0x04
#define KEY_BACKSPACE 0x7f
/* Values above 0xff so they cannot collide with bytes typed by the user. */
#define UP_ARROW 0x100
#define DOWN_ARROW 0x101
#define LEFT_ARROW 0x102
#define RIGHT_ARROW 0x103

#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

int scan(void);
int rawmode_on(void);
int rawmode_off(void);

#endif
