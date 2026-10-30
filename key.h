#ifndef KEY_H
#define KEY_H

#define CTRL_S 0x13
#define CTRL_D 0x04
#define KEY_BACKSPACE 0x7f

/* Values above 0xff so they cannot collide with bytes typed by the user. */
#define UP_ARROW 0x100
#define DOWN_ARROW 0x101
#define LEFT_ARROW 0x102
#define RIGHT_ARROW 0x103

#endif
