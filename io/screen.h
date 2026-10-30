#ifndef SCREEN_H
#define SCREEN_H

#include <stddef.h>

/*
 * Minimal terminal output primitives. They return 0 on success and -1 when
 * writing to stdout fails.
 */

int screen_puts(const char *s);
int screen_write(const char *s, size_t n);
int screen_putc(char c);
int screen_flush(void);

int screen_carriage_return(void);
int screen_erase_below(void);
int screen_goto_column(size_t column);
int screen_move_left(size_t columns);
int screen_move_up(size_t rows);
int screen_move_down(size_t rows);

#endif
