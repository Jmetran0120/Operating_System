/* kernel.c - Main kernel file for NEBULA OS
 * 
 * This is where the kernel starts after boot.S sets up the environment.
 * Displays the NEBULA OS graphical interface with glassmorphism design.
 * Includes keyboard input and memory management features.
 */

/* Include headers */
#include "keyboard.h"
#include "memory.h"
#include "graphics.h"
#include "nebula_ui.h"

/* Stub function for keyboard driver (not used in graphics mode) */
void terminal_putchar(char c) {
    /* Stub - keyboard input not yet integrated with graphics mode */
    (void)c;  /* Suppress unused parameter warning */
}

/* Kernel main function - entry point from boot.S */
void kernel_main(void) {
    /* Initialize memory manager first */
    memory_init();
    
    /* Initialize graphics subsystem (VGA Mode 13h) */
    graphics_init();
    
    /* Render the NEBULA OS interface */
    nebula_render_ui();
    
    /* Main loop - keep rendering UI */
    while (1) {
        /* For now, just keep the UI displayed */
        /* In the future, this could handle input, updates, etc. */
        __asm__ volatile ("hlt");  /* Halt CPU to save power */
    }
}

