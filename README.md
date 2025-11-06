# JoshOS - A Simple Operating System

A minimal operating system written in C and Assembly for educational purposes.

## Project Structure

```
Operating_System/
├── src/                    # Source code directory
│   ├── boot.S             # Bootloader (Assembly)
│   ├── kernel.c           # Kernel main file (C)
│   ├── keyboard.c         # Keyboard input driver
│   ├── keyboard.h         # Keyboard driver header
│   ├── memory.c           # Memory management system
│   └── memory.h           # Memory management header
├── iso/                    # ISO build directory
│   └── boot/
│       └── grub/
│           └── grub.cfg   # GRUB bootloader configuration
├── linker.ld              # Linker script (defines memory layout)
├── Makefile               # Build system
└── README.md              # This file
```

## OS Components Explained

### 1. Bootloader (boot.S)
- **Purpose**: Initializes the kernel environment
- **Multiboot Header**: Allows GRUB to recognize and load the OS
- **Stack Setup**: Allocates 16KB for kernel stack
- **Entry Point**: Jumps to `kernel_main()` in kernel.c

### 2. Kernel (kernel.c)
- **Purpose**: Main OS logic
- **VGA Text Mode**: Displays text on screen (80x25 characters)
- **Terminal**: Simple text output system
- **Entry Point**: Called from boot.S, initializes terminal and displays messages

### 3. Linker Script (linker.ld)
- **Purpose**: Defines memory layout of kernel binary
- **Kernel Location**: Loaded at 1MB (0x100000) - safe above BIOS area
- **Sections**: Organizes code (.text), data (.data), and uninitialized (.bss) sections

### 4. GRUB Configuration (grub.cfg)
- **Purpose**: Tells GRUB how to boot JoshOS
- **Multiboot**: Uses Multiboot specification to load kernel
- **Menu Entry**: Provides boot menu option

### 5. Makefile
- **Purpose**: Automates build process
- **Compilation**: Compiles C and Assembly sources
- **Linking**: Links objects into kernel binary
- **ISO Creation**: Creates bootable ISO using GRUB

## Prerequisites

On macOS, install required tools:

```bash
# Install Homebrew (if not already installed)
/bin/bash -c "$$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install required packages
brew install qemu
brew install xorriso
brew install i686-elf-grub

# Install cross-compiler toolchain (REQUIRED for macOS)
# IMPORTANT: Use i686-elf (32-bit), NOT x86_64-elf (64-bit)
brew install i686-elf-gcc i686-elf-binutils
```

**Important**: 
- macOS native `gcc`/`as`/`ld` don't support 32-bit ELF binaries
- You **must** use `i686-elf` toolchain (32-bit), **NOT** `x86_64-elf` (64-bit)
- `x86_64-elf-as` cannot assemble 32-bit code and will fail

## Build Instructions

### Step 1: Install Dependencies
Follow the prerequisites section above.

### Step 2: Build the OS

**On macOS** (use cross-compilers):
```bash
cd /Users/joshmetran/Desktop/Operating_System
CC=i686-elf-gcc AS=i686-elf-as LD=i686-elf-ld make
```

Or if installed and in PATH, just:
```bash
make
```
(The Makefile uses `?=` so you can override with environment variables)

**On Linux** (native tools):
```bash
cd /Users/joshmetran/Desktop/Operating_System
make
```

This will:
1. Compile `boot.S` (bootloader) into `build/boot.o`
2. Compile `kernel.c` into `build/kernel.o`
3. Link both into `build/kernel.bin`
4. Copy kernel to `iso/boot/kernel.bin`
5. Create bootable ISO: `JoshOS.iso`

### Step 3: Run in QEMU
```bash
make run
```

Or manually:
```bash
qemu-system-i386 -cdrom JoshOS.iso
```

### Step 4: Clean Build Files
```bash
make clean
```

## Expected Output

When you run the OS, you should see:
- Green "Welcome to JoshOS!" message
- Cyan "Kernel loaded successfully!" message
- Yellow "System ready for input." message

## Troubleshooting

### Build Errors
- **"command not found: grub-mkrescue"**: Install GRUB or use alternative method
- **"multiboot header not found"**: Check that boot.S has correct multiboot header
- **32-bit errors**: Ensure you're using 32-bit compilers (gcc -m32)

### QEMU Issues
- **ISO not booting**: Check that grub.cfg is in correct location
- **Black screen**: Kernel may have crashed - check multiboot header alignment

## Features

### Keyboard Input
- **PS/2 Keyboard Support**: Reads scan codes from keyboard controller
- **Character Mapping**: Converts scan codes to ASCII characters
- **Input Buffer**: Buffers keyboard input for command processing
- **Backspace Support**: Handles backspace key for editing input
- **Line Input**: Reads complete lines with `keyboard_readline()`

### Memory Management
- **Heap Allocator**: Simple linked-list based heap allocator
- **Allocation Functions**: `kmalloc()`, `kcalloc()`, `krealloc()`
- **Free Function**: `kfree()` with automatic block merging
- **Memory Statistics**: Track total, used, and free memory
- **1MB Heap**: Pre-allocated heap at 16MB address

### Commands
- `help` - Show available commands
- `mem` - Display memory statistics
- `test` - Test memory allocation/deallocation
- `echo [text]` - Echo text back to screen

## How It Works

### Keyboard Input
1. **Scan Code Reading**: Reads from port 0x60 (keyboard data port)
2. **Status Checking**: Checks port 0x64 for data availability
3. **Key Mapping**: Converts scan codes to ASCII using lookup table
4. **Input Processing**: Handles special keys (Enter, Backspace)
5. **Echo**: Displays typed characters on screen

### Memory Management
1. **Heap Initialization**: Sets up initial free block at startup
2. **Block Structure**: Each block has header (size, free flag, next pointer)
3. **Allocation**: First-fit algorithm finds suitable free block
4. **Splitting**: Large blocks are split to minimize waste
5. **Merging**: Adjacent free blocks are merged on free

## Next Steps

You can extend the OS by adding:
- File system support
- Process management
- Interrupt handling improvements
- Timer and scheduling
- More sophisticated memory management (paging)
- Network stack
- Graphics support
