/* keyboard.c - Keyboard input driver for JoshOS
 * 
 * Handles PS/2 keyboard input by reading scan codes from port 0x60.
 * Implements basic key mapping and input buffer.
 */

#include "keyboard.h"

/* Keyboard status register port */
#define KEYBOARD_STATUS_PORT 0x64
/* Keyboard data register port */
#define KEYBOARD_DATA_PORT   0x60

/* Status register bits */
#define KEYBOARD_STATUS_OUTPUT_FULL 0x01  /* Data available to read */
#define KEYBOARD_STATUS_INPUT_FULL  0x02  /* Data waiting to be sent */

/* Scan code constants */
#define KEY_RELEASE_MASK 0x80              /* Bit set when key is released */
#define SCANCODE_ENTER   0x1C              /* Enter key scan code */
#define SCANCODE_BACKSPACE 0x0E            /* Backspace key scan code */

/* Keyboard state */
static char input_buffer[256];             /* Input buffer */
static uint32_t buffer_index = 0;          /* Current position in buffer */

/* Forward declaration for terminal function */
extern void terminal_putchar(char c);

/* Read a byte from a port (inline assembly) */
static inline uint8_t inb(uint16_t port) {
    uint8_t result;                       /* Result variable */
    /* Read byte from port into result */
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;                         /* Return read value */
}

/* US QWERTY keyboard layout - maps scan codes to ASCII */
static const char scan_code_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',  /* 0-9 */
    '9', '0', '-', '=', '\b',                          /* Backspace */
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', /* Tab, q-o */
    'p', '[', ']', '\n',                               /* Enter */
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',  /* Ctrl, a-l */
    ';', '\'', '`', 0,                                 /* Backslash */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',           /* z-m */
    ',', '.', '/', 0,                                  /* Slash */
    '*', 0, ' ',                                       /* Space */
};

/* Check if keyboard has data available */
uint8_t keyboard_has_data(void) {
    /* Read status register */
    uint8_t status = inb(KEYBOARD_STATUS_PORT);
    /* Check if output buffer is full (data ready) */
    return (status & KEYBOARD_STATUS_OUTPUT_FULL) != 0;
}

/* Get the next key press (blocking) */
char keyboard_getchar(void) {
    /* Wait until keyboard has data */
    while (!keyboard_has_data()) {
        /* Busy wait for key press */
    }
    
    /* Read scan code from keyboard data port */
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    
    /* Check if key was released (not pressed) */
    if (scancode & KEY_RELEASE_MASK) {
        return 0;                          /* Return 0 for key release */
    }
    
    /* Convert scan code to ASCII character */
    if (scancode < 128) {                 /* Valid scan code range */
        return scan_code_to_ascii[scancode]; /* Return ASCII character */
    }
    
    return 0;                              /* Invalid scan code */
}

/* Read a line of input (up to buffer size) */
void keyboard_readline(char* buffer, uint32_t max_len) {
    buffer_index = 0;                     /* Reset buffer index */
    
    /* Loop until Enter is pressed */
    while (buffer_index < max_len - 1) {
        /* Get character from keyboard */
        char c = keyboard_getchar();
        
        /* Handle Enter key */
        if (c == '\n') {
            buffer[buffer_index] = '\0';  /* Null terminate */
            return;                        /* Return completed line */
        }
        
        /* Handle backspace */
        if (c == '\b') {
            if (buffer_index > 0) {       /* Only if there's something to delete */
                buffer_index--;           /* Move back one position */
                buffer[buffer_index] = '\0'; /* Clear character */
                terminal_putchar('\b');   /* Move cursor back */
                terminal_putchar(' ');    /* Erase character */
                terminal_putchar('\b');   /* Move cursor back again */
            }
            continue;                     /* Continue loop */
        }
        
        /* Ignore null characters (key releases, etc.) */
        if (c == 0) {
            continue;                     /* Continue loop */
        }
        
        /* Valid character - add to buffer */
        buffer[buffer_index++] = c;       /* Store character and increment */
        terminal_putchar(c);              /* Echo character to screen */
    }
    
    /* Buffer full - null terminate */
    buffer[buffer_index] = '\0';
}

/* Clear input buffer */
void keyboard_clear_buffer(void) {
    buffer_index = 0;                     /* Reset buffer index */
    /* Clear buffer memory */
    for (uint32_t i = 0; i < 256; i++) {
        input_buffer[i] = 0;               /* Zero out each byte */
    }
}

