# Makefile for JoshOS
#
# Builds the operating system from source files into a bootable ISO.
# Usage: make (builds ISO), make run (builds and runs in QEMU), make clean

# Compiler and tools
CC = gcc                           # C compiler
AS = as                            # Assembler
LD = ld                            # Linker
GRUB_MKRESCUE = grub-mkrescue      # GRUB ISO creation tool

# Compiler flags
CFLAGS = -m32                      # 32-bit mode
CFLAGS += -std=c11                 # C11 standard
CFLAGS += -ffreestanding           # No standard library (OS kernel)
CFLAGS += -O2                      # Optimization level 2
CFLAGS += -Wall                    # All warnings
CFLAGS += -Wextra                  # Extra warnings
CFLAGS += -nostdlib                # No standard library
CFLAGS += -fno-builtin             # No built-in functions
CFLAGS += -fno-stack-protector     # No stack protection (not needed in kernel)
CFLAGS += -fno-pic                 # No position-independent code
CFLAGS += -fno-pie                 # No position-independent executable

# Assembler flags
ASFLAGS = --32                     # 32-bit mode

# Linker flags
LDFLAGS = -m elf_i386              # 32-bit ELF format
LDFLAGS += -T linker.ld            # Use our linker script
LDFLAGS += -nostdlib               # No standard library

# Directories
SRC_DIR = src                      # Source directory
ISO_DIR = iso                      # ISO directory
BUILD_DIR = build                  # Build directory

# Source files
BOOT_SRC = $(SRC_DIR)/boot.S         # Bootloader source
KERNEL_SRC = $(SRC_DIR)/kernel.c     # Kernel source
KEYBOARD_SRC = $(SRC_DIR)/keyboard.c # Keyboard driver source
MEMORY_SRC = $(SRC_DIR)/memory.c     # Memory manager source

# Object files
BOOT_OBJ = $(BUILD_DIR)/boot.o       # Bootloader object
KERNEL_OBJ = $(BUILD_DIR)/kernel.o   # Kernel object
KEYBOARD_OBJ = $(BUILD_DIR)/keyboard.o # Keyboard driver object
MEMORY_OBJ = $(BUILD_DIR)/memory.o   # Memory manager object

# Output files
KERNEL_BIN = $(ISO_DIR)/boot/kernel.bin  # Kernel binary
ISO_FILE = JoshOS.iso                     # Final ISO file

# Default target - build ISO
all: $(ISO_FILE)

# Build ISO file
$(ISO_FILE): $(KERNEL_BIN)
	@echo "Creating ISO..."
	$(GRUB_MKRESCUE) -o $(ISO_FILE) $(ISO_DIR)

# Build kernel binary
$(KERNEL_BIN): $(BUILD_DIR)/kernel.bin
	@echo "Copying kernel to ISO directory..."
	@mkdir -p $(ISO_DIR)/boot
	cp $(BUILD_DIR)/kernel.bin $(KERNEL_BIN)

# Link kernel binary from object files
$(BUILD_DIR)/kernel.bin: $(BOOT_OBJ) $(KERNEL_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ)
	@echo "Linking kernel..."
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.bin $(BOOT_OBJ) $(KERNEL_OBJ) $(KEYBOARD_OBJ) $(MEMORY_OBJ)

# Compile bootloader
$(BOOT_OBJ): $(BOOT_SRC)
	@echo "Assembling bootloader..."
	@mkdir -p $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $(BOOT_OBJ) $(BOOT_SRC)

# Compile kernel
$(KERNEL_OBJ): $(KERNEL_SRC)
	@echo "Compiling kernel..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(KERNEL_OBJ) $(KERNEL_SRC)

# Compile keyboard driver
$(KEYBOARD_OBJ): $(KEYBOARD_SRC)
	@echo "Compiling keyboard driver..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(KEYBOARD_OBJ) $(KEYBOARD_SRC)

# Compile memory manager
$(MEMORY_OBJ): $(MEMORY_SRC)
	@echo "Compiling memory manager..."
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $(MEMORY_OBJ) $(MEMORY_SRC)

# Run in QEMU
run: $(ISO_FILE)
	@echo "Starting QEMU..."
	qemu-system-i386 -cdrom $(ISO_FILE)

# Clean build artifacts
clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR)
	rm -rf $(ISO_DIR)/boot
	rm -f $(ISO_FILE)

# Phony targets (not files)
.PHONY: all run clean

