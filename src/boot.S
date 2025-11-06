/* boot.S - Multiboot-compliant bootloader for JoshOS
 * 
 * This bootloader sets up the kernel environment and jumps to the kernel.
 * It follows the Multiboot specification so GRUB can load it.
 */

/* Multiboot header constants - required by GRUB */
.set MAGIC,    0x1BADB002      /* Multiboot magic number - identifies Multiboot OS */
.set FLAGS,    0x0             /* Multiboot flags - we use 0 for now */
.set CHECKSUM, -(MAGIC + FLAGS) /* Checksum ensures magic + flags + checksum = 0 */

/* Multiboot header section - GRUB looks for this */
/* Must be in a LOAD segment and within first 8KB */
.section .multiboot, "a"       /* 'a' flag means allocatable (in LOAD segment) */
    .align 4                   /* Align to 4-byte boundary */
    .long MAGIC                /* Magic number */
    .long FLAGS                /* Flags */
    .long CHECKSUM             /* Checksum */

/* Stack section - provides space for kernel stack */
.section .bss
    .align 16                  /* Align stack to 16 bytes (required by x86) */
stack_bottom:                  /* Bottom of stack */
    .skip 16384                /* Reserve 16KB for stack (16384 bytes) */
stack_top:                     /* Top of stack */

/* Text section - actual code */
.section .text
    .global _start             /* Make _start visible to linker */
    .type _start, @function    /* Mark _start as a function */

_start:
    /* Setup stack pointer - stack grows downward */
    mov $stack_top, %esp       /* Move stack_top address to stack pointer */
    mov $stack_top, %ebp       /* Move stack_top address to base pointer */
    
    /* Call kernel_main - C function expects C calling convention */
    call kernel_main           /* Jump to kernel_main function in kernel.c */
    
    /* Infinite loop if kernel_main returns (shouldn't happen) */
    cli                        /* Disable interrupts */
loop:
    hlt                        /* Halt CPU */
    jmp loop                   /* Infinite loop */
    
/* Set size for debugging */
.size _start, . - _start

