/* kernel.c - Main kernel file for JoshOS
 * 
 * This is where the kernel starts after boot.S sets up the environment.
 * Currently displays text on screen using VGA text mode.
 * Includes keyboard input and memory management features.
 */

/* Include keyboard and memory management headers */
#include "keyboard.h"
#include "memory.h"

/* Standard integer types - must be defined before use */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t           size_t;

/* VGA text mode constants */
#define VGA_WIDTH  80          /* VGA text mode width: 80 characters */
#define VGA_HEIGHT 25          /* VGA text mode height: 25 lines */
#define VGA_MEMORY 0xB8000     /* VGA text buffer memory address */

/* Color constants for VGA text mode */
/* Format: 4 bits background, 4 bits foreground */
#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN          0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GREY    0x7
#define COLOR_DARK_GREY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW        0xE
#define COLOR_WHITE         0xF

/* Helper macro to create color byte */
#define COLOR(fg, bg) ((bg << 4) | fg)

/* Terminal state structure */
typedef struct {
    /* Pointer to VGA text buffer */
    uint16_t* buffer;
    /* Current column position */
    size_t column;
    /* Current row position */
    size_t row;
    /* Current color scheme */
    uint8_t color;
} Terminal;

/* Global terminal instance */
static Terminal terminal;

/* Initialize terminal with default settings */
void terminal_initialize(void) {
    /* Point to VGA text buffer */
    terminal.buffer = (uint16_t*) VGA_MEMORY;
    /* Start at top-left corner */
    terminal.column = 0;
    terminal.row = 0;
    /* Set color: light grey text on black background */
    terminal.color = COLOR(COLOR_LIGHT_GREY, COLOR_BLACK);
    
    /* Clear entire screen */
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            /* Calculate position in buffer */
            const size_t index = y * VGA_WIDTH + x;
            /* Write space character with color */
            terminal.buffer[index] = ((uint16_t) terminal.color << 8) | ' ';
        }
    }
}

/* Set terminal color scheme */
void terminal_setcolor(uint8_t color) {
    terminal.color = color;    /* Update color */
}

/* Write a single character at current position */
void terminal_putchar(char c) {
    /* Handle newline - move to next line */
    if (c == '\n') {
        terminal.column = 0;   /* Reset to start of line */
        if (++terminal.row == VGA_HEIGHT) { /* Check if past bottom */
            terminal.row = 0;  /* Wrap to top (simple scrolling) */
        }
        return;
    }
    
    /* Calculate position in buffer */
    const size_t index = terminal.row * VGA_WIDTH + terminal.column;
    /* Write character with color (high byte = color, low byte = character) */
    terminal.buffer[index] = ((uint16_t) terminal.color << 8) | c;
    
    /* Move to next column */
    if (++terminal.column == VGA_WIDTH) { /* Check if past right edge */
        terminal.column = 0;   /* Reset to start of line */
        if (++terminal.row == VGA_HEIGHT) { /* Check if past bottom */
            terminal.row = 0;  /* Wrap to top */
        }
    }
}

/* Write a string */
void terminal_write(const char* data, size_t size) {
    /* Loop through each character */
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]); /* Write character */
    }
}

/* Write a null-terminated string */
void terminal_writestring(const char* data) {
    /* Calculate string length */
    size_t size = 0;
    while (data[size] != '\0') { /* Find null terminator */
        size++;
    }
    terminal_write(data, size); /* Write the string */
}

/* Helper function to convert integer to string */
static void int_to_string(uint32_t num, char* buffer) {
    if (num == 0) {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    char temp[32];              /* Temporary buffer */
    int i = 0;                  /* Index counter */
    
    /* Convert digits */
    while (num > 0) {
        temp[i++] = '0' + (num % 10); /* Get last digit */
        num /= 10;              /* Remove last digit */
    }
    
    /* Reverse string */
    int j = 0;                  /* Index counter */
    for (int k = i - 1; k >= 0; k--) {
        buffer[j++] = temp[k];  /* Copy reversed */
    }
    buffer[j] = '\0';           /* Null terminate */
}

/* Kernel main function - entry point from boot.S */
void kernel_main(void) {
    /* Initialize terminal */
    terminal_initialize();
    
    /* Initialize memory manager */
    memory_init();
    
    /* Set color to green on black */
    terminal_setcolor(COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
    
    /* Display welcome message */
    terminal_writestring("Welcome to JoshOS!\n");
    terminal_writestring("==================\n\n");
    
    /* Change color to cyan */
    terminal_setcolor(COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
    terminal_writestring("Kernel loaded successfully!\n");
    terminal_writestring("Operating System initialized.\n");
    terminal_writestring("Memory manager initialized.\n\n");
    
    /* Change color to yellow */
    terminal_setcolor(COLOR(COLOR_YELLOW, COLOR_BLACK));
    terminal_writestring("System ready for input.\n");
    terminal_writestring("Type 'help' for commands, 'mem' for memory stats.\n\n");
    
    /* Input buffer for commands */
    char input[256];
    char* cmd;
    
    /* Main command loop */
    while (1) {
        /* Set color to light grey */
        terminal_setcolor(COLOR(COLOR_LIGHT_GREY, COLOR_BLACK));
        terminal_writestring("JoshOS> ");
        
        /* Read line from keyboard */
        keyboard_readline(input, sizeof(input));
        terminal_writestring("\n");
        
        /* Skip empty input */
        if (input[0] == '\0') {
            continue;           /* Skip to next iteration */
        }
        
        /* Parse command (simple: first word is command) */
        cmd = input;
        
        /* Handle 'help' command */
        if (cmd[0] == 'h' && cmd[1] == 'e' && cmd[2] == 'l' && cmd[3] == 'p' && cmd[4] == '\0') {
            terminal_setcolor(COLOR(COLOR_LIGHT_CYAN, COLOR_BLACK));
            terminal_writestring("Available commands:\n");
            terminal_writestring("  help - Show this help message\n");
            terminal_writestring("  mem  - Show memory statistics\n");
            terminal_writestring("  test - Test memory allocation\n");
            terminal_writestring("  echo [text] - Echo text back\n");
        }
        /* Handle 'mem' command */
        else if (cmd[0] == 'm' && cmd[1] == 'e' && cmd[2] == 'm' && cmd[3] == '\0') {
            uint32_t total, used, free;
            memory_get_stats(&total, &used, &free);
            
            terminal_setcolor(COLOR(COLOR_LIGHT_BLUE, COLOR_BLACK));
            terminal_writestring("Memory Statistics:\n");
            
            char buffer[32];    /* Buffer for number conversion */
            
            terminal_writestring("  Total: ");
            int_to_string(total, buffer);
            terminal_writestring(buffer);
            terminal_writestring(" bytes\n");
            
            terminal_writestring("  Used:  ");
            int_to_string(used, buffer);
            terminal_writestring(buffer);
            terminal_writestring(" bytes\n");
            
            terminal_writestring("  Free:  ");
            int_to_string(free, buffer);
            terminal_writestring(buffer);
            terminal_writestring(" bytes\n");
        }
        /* Handle 'test' command */
        else if (cmd[0] == 't' && cmd[1] == 'e' && cmd[2] == 's' && cmd[3] == 't' && cmd[4] == '\0') {
            terminal_setcolor(COLOR(COLOR_LIGHT_GREEN, COLOR_BLACK));
            terminal_writestring("Testing memory allocation...\n");
            
            /* Allocate some memory */
            void* ptr1 = kmalloc(100);
            void* ptr2 = kmalloc(200);
            void* ptr3 = kmalloc(50);
            
            if (ptr1 && ptr2 && ptr3) {
                terminal_writestring("  Allocated 3 blocks successfully\n");
                
                /* Free middle block */
                kfree(ptr2);
                terminal_writestring("  Freed middle block\n");
                
                /* Free remaining blocks */
                kfree(ptr1);
                kfree(ptr3);
                terminal_writestring("  Freed all blocks\n");
                terminal_writestring("Memory test completed!\n");
            } else {
                terminal_setcolor(COLOR(COLOR_LIGHT_RED, COLOR_BLACK));
                terminal_writestring("  Memory allocation failed!\n");
            }
        }
        /* Handle 'echo' command */
        else if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
            terminal_setcolor(COLOR(COLOR_LIGHT_MAGENTA, COLOR_BLACK));
            terminal_writestring(&cmd[5]); /* Print everything after "echo " */
            terminal_writestring("\n");
        }
        /* Unknown command */
        else {
            terminal_setcolor(COLOR(COLOR_LIGHT_RED, COLOR_BLACK));
            terminal_writestring("Unknown command: ");
            terminal_writestring(cmd);
            terminal_writestring("\n");
            terminal_writestring("Type 'help' for available commands.\n");
        }
    }
}

