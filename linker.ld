/* linker.ld - Linker script for JoshOS
 * 
 * This script tells the linker how to organize the kernel binary.
 * It defines memory layout, sections, and entry point.
 */

/* Entry point - where execution starts */
ENTRY(_start)

/* Define sections of the output binary */
SECTIONS
{
    /* Kernel starts at 1MB (0x100000) - safe location above BIOS */
    . = 1M;

    /* Multiboot header must be in first 8KB of kernel */
    /* Place it at the very beginning - GRUB will find it here */
    /* This MUST be in a LOAD segment for GRUB to find it */
    .multiboot : ALIGN(4) {
        *(.multiboot)          /* Include multiboot header from boot.S */
    }

    /* Text section - executable code - starts right after multiboot */
    /* Combined with multiboot to ensure they're in same LOAD segment */
    .text : ALIGN(4) {
        *(.text)               /* All .text sections from all files */
    }

    /* Read-only data section */
    .rodata : {
        *(.rodata)             /* Constants and read-only data */
    }

    /* Data section - initialized variables */
    .data : {
        *(.data)               /* Global and static variables with initial values */
    }

    /* BSS section - uninitialized variables */
    .bss : {
        *(COMMON)              /* Common symbols */
        *(.bss)                /* Uninitialized global and static variables */
    }

    /* End of kernel - used to calculate kernel size */
    end = .;
}

