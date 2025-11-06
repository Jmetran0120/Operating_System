/* keyboard.h - Keyboard driver header for JoshOS */

#ifndef KEYBOARD_H
#define KEYBOARD_H

/* Standard integer types */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;

/* Forward declaration for terminal functions */
void terminal_putchar(char c);

/* Check if keyboard has data available */
uint8_t keyboard_has_data(void);

/* Get a single character from keyboard (blocking) */
char keyboard_getchar(void);

/* Read a line of input (up to max_len characters) */
void keyboard_readline(char* buffer, uint32_t max_len);

/* Clear the input buffer */
void keyboard_clear_buffer(void);

#endif /* KEYBOARD_H */

